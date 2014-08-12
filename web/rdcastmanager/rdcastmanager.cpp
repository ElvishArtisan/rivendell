// rdcastmanager.cpp
//
// An RSS Feed Generator for Rivendell.
//
//   (C) Copyright 2002-2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastmanager.cpp,v 1.14.4.1 2013/11/13 23:36:40 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

#include <qapplication.h>
#include <qdatetime.h>

#include <rd.h>
#include <rdconf.h>
#include <rdconfig.h>
#include <rdpodcast.h>
#include <rddb.h>
#include <rdweb.h>
#include <rdescape_string.h>
#include <rdfeed.h>
#include <rdcastsearch.h>
#include <rdsystem.h>
#include <rdstation.h>
#include <dbversion.h>

#include <rdcastmanager.h>

char server_name[PATH_MAX];

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  //
  // Initialize Variables
  //
  cast_session_id=-1;
  cast_feed_id=-1;
  cast_cast_id=-1;
  cast_add_priv=false;
  cast_edit_priv=false;
  cast_delete_priv=false;
  cast_post=NULL;

  //
  // Read Configuration
  //
  cast_config=new RDConfig();
  cast_config->load();

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(cast_config->mysqlDriver());
  if(!db) {
    printf("Content-type: text/html\n\n");
    printf("rdcastmanager: unable to initialize connection to database\n");
    Exit(0);
  }
  db->setDatabaseName(cast_config->mysqlDbname());
  db->setUserName(cast_config->mysqlUsername());
  db->setPassword(cast_config->mysqlPassword());
  db->setHostName(cast_config->mysqlHostname());
  if(!db->open()) {
    printf("Content-type: text/html\n\n");
    printf("rdcastmanager: unable to connect to database\n");
    db->removeDatabase(cast_config->mysqlDbname());
    Exit(0);
  }
  RDSqlQuery *q=new RDSqlQuery("select DB from VERSION");
  if(!q->first()) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdcastmanager: missing/invalid database version!\n");
    db->removeDatabase(cast_config->mysqlDbname());
    Exit(0);
  }
  if(q->value(0).toUInt()!=RD_VERSION_DATABASE) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdcastmanager: skewed database version!\n");
    db->removeDatabase(cast_config->mysqlDbname());
    Exit(0);
  }
  delete q;

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdcastmanager: missing REQUEST_METHOD\n");
    db->removeDatabase(cast_config->mysqlDbname());
    Exit(0);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    ServeLogin();
    Exit(0);
  }

  //
  // Get the Server Name
  //
  if(getenv("SERVER_NAME")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: missing SERVER_NAME\n");
    exit(0);
  }
  strncpy(server_name,getenv("SERVER_NAME"),PATH_MAX);

  //
  // Read Post Variables and Dispatch 
  //
  RDSystem *system=new RDSystem();
  cast_post=
    new RDFormPost(RDFormPost::MultipartEncoded,system->maxPostLength());
  delete system;
  if(cast_post->error()!=RDFormPost::ErrorOk) {
    RDCgiError(cast_post->errorString(cast_post->error()));
    Exit(0);
  }
  switch(AuthenticatePost()) {
    case RDCASTMANAGER_COMMAND_LOGIN:
      ServeLogin();
      break;

    case RDCASTMANAGER_COMMAND_LOGOUT:
      ServeLogout();
      break;

    case RDCASTMANAGER_COMMAND_LIST_FEEDS:
      ServeListFeeds();
      break;

    case RDCASTMANAGER_COMMAND_LIST_CASTS:
      ServeListCasts();
      break;

    case RDCASTMANAGER_COMMAND_EDIT_CAST:
      ServeEditCast();
      break;

    case RDCASTMANAGER_COMMAND_COMMIT_CAST:
      CommitCast();
      break;

    case RDCASTMANAGER_COMMAND_CONFIRM_DELETE_CAST:
      ConfirmDeleteCast();
      break;

    case RDCASTMANAGER_COMMAND_DELETE_CAST:
      DeleteCast();
      break;

    case RDCASTMANAGER_COMMAND_SUBSCRIPTION_PICK_DATES:
      ServeDatePicker(RDCASTMANAGER_COMMAND_SUBSCRIPTION_REPORT);
      break;

    case RDCASTMANAGER_COMMAND_SUBSCRIPTION_REPORT:
      ServeSubscriptionReport();
      break;

    case RDCASTMANAGER_COMMAND_EPISODE_PICK_DATES:
      ServeDatePicker(RDCASTMANAGER_COMMAND_EPISODE_REPORT);
      break;

    case RDCASTMANAGER_COMMAND_EPISODE_REPORT:
      ServeEpisodeReport();
      break;

    case RDCASTMANAGER_COMMAND_PLAY_CAST:
      ServePlay();
      break;

    case RDCASTMANAGER_COMMAND_POST_EPISODE:
      PostEpisode();
      break;

    default:
      RDCgiError("Invalid post data!");
      break;
  }

  Exit(0);
}


int MainObject::AuthenticatePost()
{
  int cmd=0;
  QString password;

  //
  // Check for Command Value
  //
  if(!cast_post->getValue("COMMAND",&cmd)) {
    return RDCASTMANAGER_COMMAND_LOGIN;
  }

  //
  // Validate Remote Address
  //
  if(getenv("REMOTE_ADDR")==NULL) {
    RDCgiError("Invalid CGI request!");
    Exit(0);
  }
  cast_client_addr.setAddress(getenv("REMOTE_ADDR"));
  if(cast_client_addr.isNull()) {
    RDCgiError("Invalid client IP address!");
    Exit(0);
  }

  //
  // Authenticate New Login
  //
  if(!cast_post->getValue("SESSION_ID",&cast_session_id)) {
    if(!cast_post->getValue("LOGIN_NAME",&cast_login_name)) {
      return RDCASTMANAGER_COMMAND_LOGIN;
    }
    if(!cast_post->getValue("PASSWORD",&password)) {
      return RDCASTMANAGER_COMMAND_LOGIN;
    }
    if((cast_session_id=
	RDAuthenticateLogin(cast_login_name,password,cast_client_addr))<0) {
      return RDCASTMANAGER_COMMAND_LOGIN;
    }
    GetContext();
    return cmd;
  }

  //
  // Authenticate Existing Session
  //
  cast_login_name=RDAuthenticateSession(cast_session_id,cast_client_addr);
  if(cast_login_name.isEmpty()) {
    return RDCASTMANAGER_COMMAND_LOGIN;
  }
  GetContext();

  return cmd;
}


void MainObject::ServeLogin()
{
  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Login</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<input type=\"hidden\" name=\"COMMAND\" value=\"%d\">",
	 RDCASTMANAGER_COMMAND_LIST_FEEDS);

  printf("<tr>\n");
  printf("<td align=\"center\" colspan=\"2\"><big>RDCastManager Login</big></td>\n");
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Login Name:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\"  align=\"left\"><input type=\"text\" name=\"LOGIN_NAME\" length=\"8\" maxlength=\"8\"></td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<td bgcolor=\"%s\"  align=\"right\">Password:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\"  align=\"left\"><input type=\"password\" name=\"PASSWORD\" length=\"32\" maxlength=\"32\"></td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<td colspan=\"2\">&nbsp;</td>\n");
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<td>&nbsp;</td>\n");
  printf("<td align=\"right\"><input type=\"submit\" value=\"OK\"></td>\n");
  printf("</tr>\n");

  printf("</form>\n");
  printf("</table>\n");
  printf("</body>\n");
  printf("</html>\n");
  Exit(0);
}


void MainObject::ServeLogout()
{
  RDLogoutSession(cast_session_id,cast_client_addr);
  ServeLogin();
}


void MainObject::ServeListFeeds()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  int total=0;
  int active=0;

  cast_key_name="";
  cast_feed_id=-1;
  cast_cast_id=-1;

  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Feed List</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  //
  // Title Section
  //
  TitleSection("Available Feeds",RDCASTMANAGER_COMMAND_LIST_FEEDS,3);

  //
  // Column Headers
  //
  printf("<tr>\n");
  printf("<th>&nbsp;</th>\n");
  printf("<th>KEY NAME</th>\n");
  printf("<th>FEED NAME</th>\n");
  printf("<th>CASTS</th>\n");
  printf("<th>&nbsp;</th>\n");
  printf("</tr>\n");

  //
  // Data Rows
  //
  QString line_colors[2];
  line_colors[0]=RD_WEB_LINE_COLOR1;
  line_colors[1]=RD_WEB_LINE_COLOR2;
  int current_color=0;

  sql=QString().sprintf("select FEED_PERMS.KEY_NAME from \
                         FEED_PERMS left join WEB_CONNECTIONS \
                         on(FEED_PERMS.USER_NAME=WEB_CONNECTIONS.LOGIN_NAME) \
                         where WEB_CONNECTIONS.SESSION_ID=%ld",
			cast_session_id);
  q=new RDSqlQuery(sql);
  sql=QString().sprintf("select ID,KEY_NAME,CHANNEL_TITLE from FEEDS \
                         where ");
  while(q->next()) {
    sql+=QString().sprintf("(KEY_NAME=\"%s\")||",
			   (const char *)q->value(0).toString());
  }
  delete q;
  if(sql.right(2)=="||") {
    sql=sql.left(sql.length()-2);
    sql+=" order by KEY_NAME";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      total=0;
      active=0;
      printf("<tr>\n");
      sql=QString().sprintf("select STATUS from PODCASTS where FEED_ID=%u",
			    q->value(0).toUInt());
      q1=new RDSqlQuery(sql);
      while(q1->next()) {
	total++;
	switch((RDPodcast::Status)q1->value(0).toUInt()) {
	  case RDPodcast::StatusActive:
	  case RDPodcast::StatusExpired:
	    active++;
	    break;

	  case RDPodcast::StatusPending:
	    break;
	}
      }
      delete q1;
      if(active==total) {
	printf("<td bgcolor=\"%s\"><img src=\"greencheckmark.png\" border=\"0\"></td>\n",(const char *)line_colors[current_color]);
      }
      else {
	printf("<td bgcolor=\"%s\"><img src=\"redx.png\" border=\"0\"></td>\n",
	       (const char *)line_colors[current_color]);
      }
      printf("<td align=\"center\" bgcolor=\"%s\">%s</td>\n"
	     ,(const char *)line_colors[current_color],
	     (const char *)q->value(1).toString());
      printf("<td bgcolor=\"%s\">%s</td>\n",
	     (const char *)line_colors[current_color],
	     (const char *)q->value(2).toString());
      printf("<td align=\"center\" bgcolor=\"%s\">%d / %d</td>\n",
	     (const char *)line_colors[current_color],active,total);
      printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
      printf("<td bgcolor=\"%s\">\n",(const char *)line_colors[current_color]);
      SetContext(RDCASTMANAGER_COMMAND_LIST_CASTS);
      printf("<input type=\"hidden\" name=\"FEED_ID\" value=\"%d\">\n",
	     q->value(0).toInt());
      printf("<input type=\"hidden\" name=\"KEY_NAME\" value=\"%s\">\n",
	     (const char *)q->value(1).toString());
      printf("<input type=\"submit\" value=\"View Feed\">\n");
      printf("</td>\n");
      printf("</form>\n");
      printf("</tr>\n");
      if(current_color==0) {
	current_color=1;
      }
      else {
	current_color=0;
      }
    }
    delete q;
  }

  //
  // Logout Button
  //
  printf("<tr><td colspan=\"5\">&nbsp;</td></tr>\n");
  printf("<tr>\n");
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td align=\"center\" colspan=\"4\">&nbsp;</td>\n");
  printf("<td align=\"center\">\n");
  printf("<input type=\"hidden\" name=\"COMMAND\" value=\"%d\">\n",
	 RDCASTMANAGER_COMMAND_LOGOUT);
  printf("<input type=\"hidden\" name=\"SESSION_ID\" value=\"%ld\">\n",
	 cast_session_id);
  printf("<input type=\"submit\" value=\"Logout\">\n");
  printf("</td>\n");
  printf("</form>\n");
  printf("</tr>\n");

  printf("</table>\n");
  printf("</body>\n");
  printf("</html>\n");
  Exit(0);
}


void MainObject::ServeListCasts()
{
  QString sql;
  RDSqlQuery *q;
  QString filter;
  bool unexp_only;
  bool active_only;

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  cast_cast_id=-1;

  GetUserPerms();

  filter[0]=0;
  cast_post->getValue("FILTER",&filter);
  unexp_only=(cast_post->value("ONLY_NOT_EXPIRED").toString().lower()=="yes");
  active_only=(cast_post->value("ONLY_ACTIVE").toString().lower()=="yes");

  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<script type=\"text/javascript\" src=\"rdcastmanager.js\"></script>\n");
  printf("<title>Rivendell RDCastManager -- Podcasts</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);

  //
  // Big Frame Start
  //
  printf("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
  printf("<tr><td id=\"bigframe\">\n");

  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  //
  // Title Section
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<tr><td align=\"center\" colspan=\"9\"><big><big>%s Podcasts</big></big></td></tr>\n",(const char *)cast_key_name);
  SetContext(RDCASTMANAGER_COMMAND_LIST_CASTS);

  //
  // Filter
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\">&nbsp;</td>\n",RD_WEB_LINE_COLOR2);
  printf("<td colspan=\"7\" align=\"left\" bgcolor=\"%s\"><strong>Filter:</strong>\n",RD_WEB_LINE_COLOR2);
  printf("<input type=\"text\" name=\"FILTER\" value=\"%s\" size=\"70\" maxlength=\"255\">\n",(const char *)filter);
  printf("</td>\n");
  printf("<td bgcolor=\"%s\"><input type=\"submit\" value=\"Refresh\"></td>\n",RD_WEB_LINE_COLOR2);
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<td bgcolor=\"%s\">&nbsp;</td>\n",RD_WEB_LINE_COLOR2);
  printf("<td colspan=\"2\" align=\"left\" bgcolor=\"%s\"><strong>Show Only Nonexpired Casts:</strong>\n",RD_WEB_LINE_COLOR2);
  printf("<select name=\"ONLY_NOT_EXPIRED\">\n");
  printf("<option value=\"no\">No</option>\n");
  printf("<option value=\"yes\"");
  if(unexp_only) {
    printf(" selected");
  }
  printf(">Yes</option>\n");
  printf("</select>\n");
  printf("</td>\n");

  printf("<td colspan=\"6\" align=\"left\" bgcolor=\"%s\"><strong>Show Only Active Casts:</strong>\n",RD_WEB_LINE_COLOR2);
  printf("<select name=\"ONLY_ACTIVE\">\n");
  printf("<option value=\"no\">No</option>\n");
  printf("<option value=\"yes\"");
  if(active_only) {
    printf(" selected");
  }
  printf(">Yes</option>\n");
  printf("</select>\n");
  printf("</td>\n");
  printf("</tr>\n");

  printf("<tr><td colspan=\"9\">&nbsp;</td></tr>\n");
  printf("</form>\n");


  //
  // Column Headers
  //
  printf("<tr>\n");
  printf("<th>&nbsp;</th>\n");
  printf("<th>TITLE</th>\n");
  printf("<th>POSTED</th>\n");
  printf("<th>EXPIRES</th>\n");
  printf("<th>LENGTH</th>\n");
  printf("<th>CATEGORY</th>\n");
  printf("<th>&nbsp;</th>\n");
  printf("<th>&nbsp;</th>\n");
  printf("<th>&nbsp;</th>\n");
  printf("</tr>\n");

  //
  // Data Rows
  //
  QString line_colors[2];
  line_colors[0]=RD_WEB_LINE_COLOR1;
  line_colors[1]=RD_WEB_LINE_COLOR2;
  int current_color=0;
  sql="select ID,STATUS,ITEM_TITLE,ORIGIN_DATETIME,SHELF_LIFE,ITEM_CATEGORY,\
       AUDIO_TIME from PODCASTS "+
    RDCastSearch(cast_feed_id,filter,unexp_only,active_only)+
    " order by ORIGIN_DATETIME desc";

  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("<tr>\n");
    switch((RDPodcast::Status)q->value(1).toUInt()) {
      case RDPodcast::StatusActive:
	printf("<td bgcolor=\"%s\"><img src=\"greenball.png\" border=\"0\"></td>\n",(const char *)line_colors[current_color]);
	break;

      case RDPodcast::StatusPending:
	printf("<td bgcolor=\"%s\"><img src=\"redball.png\" border=\"0\"></td>\n",(const char *)line_colors[current_color]);
	break;

      case RDPodcast::StatusExpired:
	printf("<td bgcolor=\"%s\"><img src=\"whiteball.png\" border=\"0\"></td>\n",(const char *)line_colors[current_color]);
	break;
    }
    printf("<td bgcolor=\"%s\">%s</td>\n",
	   (const char *)line_colors[current_color],
	   (const char *)q->value(2).toString());
    printf("<td align=\"center\" bgcolor=\"%s\">%s</td>\n",
	   (const char *)line_colors[current_color],
	   (const char *)RDUtcToLocal(q->value(3).toDateTime()).
	   toString("hh:mm:ss MM/dd/yyyy"));
    if(q->value(4).toInt()>0) {
      printf("<td align=\"center\" bgcolor=\"%s\">%s</td>\n",
	     (const char *)line_colors[current_color],
	     (const char *)RDUtcToLocal(q->value(3).toDateTime()).
	     addDays(q->value(4).toInt()).toString("hh:mm:ss MM/dd/yyyy"));
    }
    else {
      printf("<td align=\"center\" bgcolor=\"%s\">Never</td>\n",
	     (const char *)line_colors[current_color]);
    }
    printf("<td align=\"right\" bgcolor=\"%s\">%s</td>\n",
	   (const char *)line_colors[current_color],
	   (const char *)RDGetTimeLength(q->value(6).toInt(),false,false));
    printf("<td align=\"center\" bgcolor=\"%s\">%s</td>\n",
	   (const char *)line_colors[current_color],
	   (const char *)q->value(5).toString());

    //
    // Play Button
    //
    if(1==1) {
      printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
      printf("<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">\n",
	   (const char *)line_colors[current_color]);
      SetContext(RDCASTMANAGER_COMMAND_PLAY_CAST);
      printf("<input type=\"hidden\" name=\"CAST_ID\" value=\"%d\">\n",
	     q->value(0).toInt());
      printf("<input type=\"submit\" value=\"Play\">\n");
      printf("</td>\n");
      printf("</form>\n");
    }
    else {
      printf("<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">\n",
	   (const char *)line_colors[current_color]);
      printf("&nbsp;\n");
      printf("</td>\n");
    }

    //
    // Edit Button
    //
    if(cast_edit_priv) {
      printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
      printf("<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">\n",
	   (const char *)line_colors[current_color]);
      SetContext(RDCASTMANAGER_COMMAND_EDIT_CAST);
      printf("<input type=\"hidden\" name=\"CAST_ID\" value=\"%d\">\n",
	     q->value(0).toInt());
      printf("<input type=\"submit\" value=\"Edit\">\n");
      printf("</td>\n");
      printf("</form>\n");
    }
    else {
      printf("<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">\n",
	   (const char *)line_colors[current_color]);
      printf("&nbsp;\n");
      printf("</td>\n");
    }

    //
    // Delete Button
    //
    if(cast_delete_priv) {
      printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
      printf("<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">\n",
	     (const char *)line_colors[current_color]);
      SetContext(RDCASTMANAGER_COMMAND_CONFIRM_DELETE_CAST);
      printf("<input type=\"hidden\" name=\"CAST_ID\" value=\"%d\">\n",
	     q->value(0).toInt());
      printf("<input type=\"submit\" value=\"Delete\">\n");
      printf("</td>\n");
      printf("</form>\n");
    }
    else {
      printf("<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">\n",
	   (const char *)line_colors[current_color]);
      printf("&nbsp;\n");
      printf("</td>\n");
    }
    printf("</tr>\n");
    if(current_color==0) {
      current_color=1;
    }
    else {
      current_color=0;
    }
  }
  delete q;

  //
  // Spacer
  //
  printf("<tr><td colspan=\"9\">&nbsp;</td></tr>\n");
  printf("<tr>\n");
  printf("<td>&nbsp;</td>\n");

  //
  // Post Episode Button
  //
  if(cast_add_priv) {
    printf("<form id=\"addpostform\" action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\" onsubmit=\"PostCast()\">\n");
    printf("<td id=\"postform\" aling=\"right\" valign=\"middle\" colspan=\"2\">\n");
    printf("<table cellspacing=\"0\" cellpadding=\"2\" border=\"0\">\n");
    printf("<tr>\n");
    printf("<td align=\"left\" valign=\"middle\" bgcolor=\"%s\">\n",
	   RD_WEB_LINE_COLOR1);
    SetContext(RDCASTMANAGER_COMMAND_POST_EPISODE);
    printf("<strong>File:</strong>");
    printf("</td>\n");
    printf("<td align=\"right\" valign=\"middle\" bgcolor=\"%s\">\n",
	   RD_WEB_LINE_COLOR1);
    printf("<input id=\"mediafile\" type=\"file\" name=\"MEDIA_FILE\">\n");
    printf("</td>\n");
    printf("<td aling=\"center\" valign=\"middle\" colspan=\"2\" bgcolor=\"%s\">\n",RD_WEB_LINE_COLOR1);
    printf("<input type=\"submit\" value=\"Post New Episode\">\n");
    printf("</td>\n");
    printf("</tr>\n");
    printf("</table>\n");
    printf("</td>\n");
    printf("</form>\n");
  }
  else {
    printf("<td colspan=\"2\">&nbsp;</td>\n");
  }

  //
  // Subscription Report Button
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td align=\"center\" valign=\"middle\" colspan=\"3\">\n");
  SetContext(RDCASTMANAGER_COMMAND_SUBSCRIPTION_PICK_DATES);
  printf("<input type=\"submit\" value=\"Subscription Report\">\n");
  printf("</td>\n");
  printf("</form>\n");

  //
  // Close Button
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td align=\"center\" valign=\"middle\">\n");
  printf("<input type=\"hidden\" name=\"SESSION_ID\" value=\"%ld\">\n",
	 cast_session_id);
  printf("<input type=\"hidden\" name=\"COMMAND\" value=\"%d\">\n",
	 RDCASTMANAGER_COMMAND_LIST_FEEDS);
  printf("<input type=\"submit\" value=\"Close\">\n");
  printf("</td>\n");
  printf("</form>\n");
  printf("</tr>\n");

  printf("</table>\n");

  //
  // Big Frame End
  //
  printf("</td></tr>\n");
  printf("</table>\n");

  printf("</body>\n");
  printf("</html>\n");
  Exit(0);
}


void MainObject::ServeEditCast(int cast_id)
{
  QString sql;
  RDSqlQuery *q;

  GetUserPerms();
  if(!cast_edit_priv) {
    RDCgiError("Insufficient privileges for this operation!");
    Exit(0);
  }

  if(cast_id<0) {
    GetContext();
    if(cast_cast_id<0) {
      RDCgiError("Missing CAST_ID");
      Exit(0);
    }
  }
  else {
    cast_cast_id=cast_id;
  }
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }

  sql=QString().sprintf("select ITEM_TITLE,ITEM_AUTHOR,\
                         ITEM_CATEGORY,ITEM_LINK,ITEM_DESCRIPTION,\
                         ITEM_COMMENTS,ITEM_SOURCE_TEXT,ITEM_SOURCE_URL,\
                         ITEM_COMMENTS,SHELF_LIFE,ORIGIN_DATETIME,STATUS,\
                         EFFECTIVE_DATETIME \
                         from PODCASTS where ID=%d",cast_cast_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    RDCgiError("Unable to fetch cast record!");
    Exit(0);
  }
  QDateTime origin_datetime=RDUtcToLocal(q->value(10).toDateTime());
  QDateTime effective_datetime=RDUtcToLocal(q->value(12).toDateTime());

  RDFeed *feed=new RDFeed(cast_feed_id);

  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Editing \"%s\"</title>\n",
	 (const char *)q->value(0).toString());
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  //
  // Title Section
  //
  TitleSection("Editing Podcast",RDCASTMANAGER_COMMAND_EDIT_CAST,1);

  //
  // Media Link
  //
  if(feed->mediaLinkMode()!=RDFeed::LinkNone) {
    printf("<tr>\n");
    printf("<td align=\"right\" bgcolor=\"%s\">Media Link:</td>\n",
	   RD_WEB_LINE_COLOR1);
    printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	   RD_WEB_LINE_COLOR1);
    printf("%s\n",(const char *)feed->audioUrl(feed->mediaLinkMode(),
					       server_name,cast_cast_id));
    printf("</td>\n");
    printf("</tr>\n");
    
    printf("<tr>\n");
    printf("<td colspan=\"3\">&nbsp;</td>\n");
    printf("</tr>\n");
  }

  //
  // Cast Data
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  SetContext(RDCASTMANAGER_COMMAND_COMMIT_CAST);

  //
  // Title
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Title:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_TITLE\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",(const char *)q->value(0).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Author E-Mail
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Author E-Mail:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_AUTHOR\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",(const char *)q->value(1).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Category
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Category:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_CATEGORY\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",(const char *)q->value(2).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Link URL
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Link URL:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_LINK\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",(const char *)q->value(3).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Description
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\" valign=\"top\">Description:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<textarea name=\"ITEM_DESCRIPTION\" rows=\"4\" cols=\"50\">%s</textarea>\n",(const char *)q->value(4).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Source Text
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Source Text:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_SOURCE_TEXT\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",
	 (const char *)q->value(6).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Source Link
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Source Link:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_SOURCE_URL\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",
	 (const char *)q->value(7).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Comments
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Comments:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ITEM_COMMENTS\" value=\"%s\" size=\"50\" maxlength=\"255\">\n",(const char *)q->value(5).toString());
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Origin Datetime
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Posted At:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"ORIGIN_DATETIME\" value=\"%s\" size=\"25\" maxlength=\"255\" readonly>\n",
	 (const char *)origin_datetime.toString("MM/dd/yyyy - hh:mm:ss"));
  printf("</td>\n");
  printf("</tr>\n");

  //
  // Effective Date/Time
  //
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" align=\"right\">Air Date/Time:</td>\n",
	 RD_WEB_LINE_COLOR1);
  printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	 RD_WEB_LINE_COLOR1);
  printf("<input type=\"text\" name=\"EFFECTIVE_MONTH\" value=\"%02d\" size=\"2\" maxlength=\"2\">/\n",effective_datetime.date().month());
  printf("<input type=\"text\" name=\"EFFECTIVE_DAY\" value=\"%02d\" size=\"2\" maxlength=\"2\">/\n",effective_datetime.date().day());
  printf("<input type=\"text\" name=\"EFFECTIVE_YEAR\" value=\"%04d\" size=\"4\" maxlength=\"4\"> -- \n",effective_datetime.date().year());
  printf("<input type=\"text\" name=\"EFFECTIVE_HOUR\" value=\"%02d\" size=\"2\" maxlength=\"2\">:\n",effective_datetime.time().hour());
  printf("<input type=\"text\" name=\"EFFECTIVE_MINUTE\" value=\"%02d\" size=\"2\" maxlength=\"2\">:\n",effective_datetime.time().minute());
  printf("<input type=\"text\" name=\"EFFECTIVE_SECOND\" value=\"%02d\" size=\"2\" maxlength=\"2\">\n",effective_datetime.time().second());
  printf("</td>\n");
  printf("</tr>\n");

  QDateTime exp=origin_datetime.addDays(q->value(9).toInt());
  if(q->value(11).toInt()==RDPodcast::StatusExpired) {
    printf("<input type=\"hidden\" name=\"STATUS\" value=\"%u\">\n",
	   RDPodcast::StatusExpired);
    if(q->value(9).toInt()==0) {
      printf("<input type=\"hidden\" name=\"EXPIRES\" value=\"No\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_MONTH\" value=\"0\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_DAY\" value=\"0\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_YEAR\" value=\"0\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_HOUR\" value=\"0\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_MINUTE\" value=\"0\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_SECOND\" value=\"0\">\n");
    }
    else {
      printf("<input type=\"hidden\" name=\"EXPIRES\" value=\"Yes\">\n");
      printf("<input type=\"hidden\" name=\"EXPIRATION_MONTH\" value=\"%s\">\n",(const char *)exp.toString("MM"));
      printf("<input type=\"hidden\" name=\"EXPIRATION_DAY\" value=\"%s\">\n",
	     (const char *)exp.toString("dd"));
      printf("<input type=\"hidden\" name=\"EXPIRATION_YEAR\" value=\"%s\">\n",
	     (const char *)exp.toString("yyyy"));
      printf("<input type=\"hidden\" name=\"EXPIRATION_HOUR\" value=\"%s\">\n",
	     (const char *)exp.toString("hh"));
      printf("<input type=\"hidden\" name=\"EXPIRATION_MINUTE\" value=\"%s\">\n",(const char *)exp.toString("mm"));
      printf("<input type=\"hidden\" name=\"EXPIRATION_SECOND\" value=\"%s\">\n",(const char *)exp.toString("ss"));
    }
  }
  else {
    //
    // Expiration
    //
    printf("<tr>\n");
    printf("<td bgcolor=\"%s\" align=\"right\">Cast Expires:</td>\n",
	   RD_WEB_LINE_COLOR1);
    printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	   RD_WEB_LINE_COLOR1);
    printf("<select name=\"EXPIRES\">\n");
    if(q->value(9).toInt()==0) {
      printf("<option value=\"yes\">Yes</option>\n");
      printf("<option value=\"no\" selected>No</option>\n");
    }
    else {
      printf("<option value=\"yes\" selected>Yes</option>\n");
      printf("<option value=\"no\">No</option>\n");
    }
    printf("</select>\n");
    printf("</td>\n");
    printf("</tr>\n");
    
    if(q->value(9).toInt()==0) {
      printf("<tr>\n");
      printf("<td bgcolor=\"%s\" align=\"right\">Expires At:</td>\n",
	     RD_WEB_LINE_COLOR1);
      printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	     RD_WEB_LINE_COLOR1);
      printf("<input type=\"text\" name=\"EXPIRATION_MONTH\" value=\"0\" size=\"2\" maxlength=\"2\">/\n");
      printf("<input type=\"text\" name=\"EXPIRATION_DAY\" value=\"0\" size=\"2\" maxlength=\"2\">/\n");
      printf("<input type=\"text\" name=\"EXPIRATION_YEAR\" value=\"0\" size=\"4\" maxlength=\"4\"> -- \n");
      printf("<input type=\"text\" name=\"EXPIRATION_HOUR\" value=\"0\" size=\"2\" maxlength=\"2\">:\n");
      printf("<input type=\"text\" name=\"EXPIRATION_MINUTE\" value=\"0\" size=\"2\" maxlength=\"2\">:\n");
      printf("<input type=\"text\" name=\"EXPIRATION_SECOND\" value=\"0\" size=\"2\" maxlength=\"2\">\n");
      printf("</td>\n");
      printf("</tr>\n");
    }
    else {
      printf("<tr>\n");
      printf("<td bgcolor=\"%s\" align=\"right\">Expires At:</td>\n",
	     RD_WEB_LINE_COLOR1);
      printf("<td bgcolor=\"%s\" align=\"left\" colspan=\"2\">\n",
	     RD_WEB_LINE_COLOR1);
      printf("<input type=\"text\" name=\"EXPIRATION_MONTH\" value=\"%s\" size=\"2\" maxlength=\"2\">/\n",(const char *)exp.toString("MM"));
      printf("<input type=\"text\" name=\"EXPIRATION_DAY\" value=\"%s\" size=\"2\" maxlength=\"2\">/\n",(const char *)exp.toString("dd"));
      printf("<input type=\"text\" name=\"EXPIRATION_YEAR\" value=\"%s\" size=\"4\" maxlength=\"4\"> -- \n",(const char *)exp.toString("yyyy"));
      printf("<input type=\"text\" name=\"EXPIRATION_HOUR\" value=\"%s\" size=\"2\" maxlength=\"2\">:\n",(const char *)exp.toString("hh"));
      printf("<input type=\"text\" name=\"EXPIRATION_MINUTE\" value=\"%s\" size=\"2\" maxlength=\"2\">:\n",(const char *)exp.toString("mm"));
      printf("<input type=\"text\" name=\"EXPIRATION_SECOND\" value=\"%s\" size=\"2\" maxlength=\"2\">\n",(const char *)exp.toString("ss"));
      printf("</td>\n");
      printf("</tr>\n");
    }
    
    //
    // Posting Status
    //
    printf("<tr>\n");
    printf("<td bgcolor=\"%s\" align=\"right\">Posting Status:</td>\n",
	   RD_WEB_LINE_COLOR1);
    printf("<td bgcolor=\"%s\" align=\"left\">\n",RD_WEB_LINE_COLOR1);
    if((q->value(11).toInt()==RDPodcast::StatusPending)) {
      printf("<input type=\"radio\" name=\"STATUS\" value=%d checked=\"checked\">Hold\n",
	     RDPodcast::StatusPending);
      printf("</td>\n");
      printf("<td bgcolor=\"%s\" align=\"left\">\n",RD_WEB_LINE_COLOR1);
      printf("<input type=\"radio\" name=\"STATUS\" value=%d>Active</td>\n",
	     RDPodcast::StatusActive);
    }
    else {
      printf("<input type=\"radio\" name=\"STATUS\" value=%d>Hold\n",
	     RDPodcast::StatusPending);
      printf("</td>\n");
      printf("<td bgcolor=\"%s\" align=\"left\">\n",RD_WEB_LINE_COLOR1);
      printf("<input type=\"radio\" name=\"STATUS\" value=%d checked=\"checked\">Active</td>\n",
	     RDPodcast::StatusActive);
      
    }
    printf("</td>\n");
    printf("</tr>\n");
  }

  printf("<tr>\n");

  //
  // OK Button
  //
  printf("<td align=\"right\" valign=\"middle\">\n");
  printf("<input type=\"submit\" value=\"OK\">\n");
  printf("</td>\n");
  printf("</form>\n");

  //
  // Cancel Button
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td align=\"left\" valign=\"left\">\n");
  int id=cast_cast_id;   // UGLY, UGLY UGLY!!!
  cast_cast_id=-1;
  SetContext(RDCASTMANAGER_COMMAND_LIST_CASTS);
  cast_cast_id=id;
  printf("<input type=\"submit\" value=\"Cancel\">\n");
  printf("</td>\n");
  printf("</form>\n");

  //
  // Episode Report Button
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td align=\"right\" valign=\"right\">\n");
  SetContext(RDCASTMANAGER_COMMAND_EPISODE_PICK_DATES);
  printf("<input type=\"submit\" value=\"Episode Report\">\n");
  printf("</td>\n");
  printf("</form>\n");

  printf("</tr>\n");

  printf("</table>\n");
  printf("</body>\n");
  printf("</html>\n");

  delete feed;

  Exit(0);
}


void MainObject::ServePlay()
{
  QString sql;
  RDSqlQuery *q;

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  if(cast_cast_id<0) {
    RDCgiError("Missing CAST_ID");
    Exit(0);
  }

  sql=QString().sprintf("select FEEDS.BASE_URL,PODCASTS.AUDIO_FILENAME \
                         from FEEDS left join PODCASTS \
                         on FEEDS.ID=PODCASTS.FEED_ID \
                         where PODCASTS.ID=%d",cast_cast_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    printf("Content-type: audio/x-mpeg\n");
    printf("Location: %s/%s\n\n",(const char *)q->value(0).toString(),
	   (const char *)q->value(1).toString());
  }
  else {
    printf("Status: 500 Internal Server Error\n");
    printf("Content-type: text/html\n");
    printf("\n");
    printf("The cast ID was not found in the database.\n");
  }
  delete q;
}


void MainObject::CommitCast()
{
  QString sql;
  RDSqlQuery *q;
  QString item_title;
  QString item_author;
  QString item_category;
  QString item_link;
  QString item_description;
  QString item_source_text;
  QString item_source_url;
  QString item_comments;
  QString expires;
  int expiration_day=0;
  int expiration_month=0;
  int expiration_year=0;
  int expiration_hour=0;
  int expiration_minute=0;
  int expiration_second=0;
  int effective_day=0;
  int effective_month=0;
  int effective_year=0;
  int effective_hour=0;
  int effective_minute=0;
  int effective_second=0;
  unsigned shelf_life=0;
  int status;

  GetUserPerms();
  if(!cast_edit_priv) {
    RDCgiError("Insufficient privileges for this operation!");
    Exit(0);
  }

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  if(cast_cast_id<0) {
    RDCgiError("Missing CAST_ID");
    Exit(0);
  }
  if(!cast_post->getValue("ITEM_TITLE",&item_title)) {
    RDCgiError("Missing ITEM_TITLE");
    Exit(0);
  }
  cast_post->getValue("ITEM_AUTHOR",&item_author);
  cast_post->getValue("ITEM_CATEGORY",&item_category);
  cast_post->getValue("ITEM_LINK",&item_link);
  cast_post->getValue("ITEM_DESCRIPTION",&item_description);
  cast_post->getValue("ITEM_SOURCE_TEXT",&item_source_text);
  cast_post->getValue("ITEM_SOURCE_URL",&item_source_url);
  cast_post->getValue("ITEM_COMMENTS",&item_comments);
  if(!cast_post->getValue("EFFECTIVE_DAY",&effective_day)) {
    RDCgiError("Missing EFFECTIVE_DAY");
    Exit(0);
  }
  if(!cast_post->getValue("EFFECTIVE_MONTH",&effective_month)) {
    RDCgiError("Missing EFFECTIVE_MONTH");
    Exit(0);
  }
  if(!cast_post->getValue("EFFECTIVE_YEAR",&effective_year)) {
    RDCgiError("Missing EFFECTIVE_YEAR");
    Exit(0);
  }
  if(!cast_post->getValue("EFFECTIVE_HOUR",&effective_hour)) {
    RDCgiError("Missing EFFECTIVE_HOUR");
    Exit(0);
  }
  if(!cast_post->getValue("EFFECTIVE_MINUTE",&effective_minute)) {
    RDCgiError("Missing EFFECTIVE_MINUTE");
    Exit(0);
  }
  if(!cast_post->getValue("EFFECTIVE_SECOND",&effective_second)) {
    RDCgiError("Missing EFFECTIVE_SECOND");
    Exit(0);
  }
  if(!cast_post->getValue("EXPIRES",&expires)) {
    RDCgiError("Missing EXPIRES");
    Exit(0);
  }
  if(expires.lower()=="yes") {
    if(!cast_post->getValue("EXPIRATION_DAY",&expiration_day)) {
      RDCgiError("Missing EXPIRATION_DAY");
      Exit(0);
    }
    if(!cast_post->getValue("EXPIRATION_MONTH",&expiration_month)) {
      RDCgiError("Missing EXPIRATION_MONTH");
      Exit(0);
    }
    if(!cast_post->getValue("EXPIRATION_YEAR",&expiration_year)) {
      RDCgiError("Missing EXPIRATION_YEAR");
      Exit(0);
    }
    if(!cast_post->getValue("EXPIRATION_HOUR",&expiration_hour)) {
      RDCgiError("Missing EXPIRATION_HOUR");
      Exit(0);
    }
    if(!cast_post->getValue("EXPIRATION_MINUTE",&expiration_minute)) {
      RDCgiError("Missing EXPIRATION_MINUTE");
      Exit(0);
    }
    if(!cast_post->getValue("EXPIRATION_SECOND",&expiration_second)) {
      RDCgiError("Missing EXPIRATION_SECOND");
      Exit(0);
    }
    sql=QString().sprintf("select ORIGIN_DATETIME from PODCASTS \
                           where ID=%d",cast_cast_id);
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      delete q;
      RDCgiError("Unable to access cast record!");
    }
    QDateTime dt(QDate(expiration_year,expiration_month,expiration_day),
		 QTime(expiration_hour,expiration_minute,expiration_second));
    if(dt.isNull()) {
      RDCgiError("Invalid expiration date!");
      delete q;
      Exit(0);
    }
    shelf_life=q->value(0).toDateTime().daysTo(dt);
    delete q;
  }
  if(!cast_post->getValue("STATUS",&status)) {
    RDCgiError("Missing STATUS");
    Exit(0);
  }
  QDateTime 
    effective_datetime(QDate(effective_year,effective_month,effective_day),
		      QTime(effective_hour,effective_minute,effective_second));
  sql=QString().sprintf("update PODCASTS set \
                         STATUS=%d,\
                         ITEM_TITLE=\"%s\",\
                         ITEM_DESCRIPTION=\"%s\",\
                         ITEM_CATEGORY=\"%s\",\
                         ITEM_LINK=\"%s\",\
                         ITEM_COMMENTS=\"%s\",\
                         ITEM_AUTHOR=\"%s\",\
                         ITEM_SOURCE_TEXT=\"%s\",\
                         ITEM_SOURCE_URL=\"%s\",\
                         SHELF_LIFE=%d,\
                         EFFECTIVE_DATETIME=\"%s\" \
                         where ID=%d",
			status,
			(const char *)RDEscapeString(item_title),
			(const char *)RDEscapeString(item_description),
			(const char *)RDEscapeString(item_category),
			(const char *)RDEscapeString(item_link),
			(const char *)RDEscapeString(item_comments),
			(const char *)RDEscapeString(item_author),
			(const char *)RDEscapeString(item_source_text),
			(const char *)RDEscapeString(item_source_url),
			shelf_life,
			(const char *)RDLocalToUtc(effective_datetime).
			toString("yyyy-MM-dd hh:mm:ss"),
			cast_cast_id);
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString().sprintf("update FEEDS set LAST_BUILD_DATETIME=UTC_TIMESTAMP()\
                         where ID=%d",cast_feed_id);
  q=new RDSqlQuery(sql);
  delete q;

  ServeListCasts();
}


void MainObject::ConfirmDeleteCast()
{
  QString sql;
  RDSqlQuery *q;

  GetUserPerms();
  if(!cast_delete_priv) {
    RDCgiError("Insufficient privileges for this operation!");
    Exit(0);
  }

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  if(cast_cast_id<0) {
    RDCgiError("Missing CAST_ID");
    Exit(0);
  }

  sql=QString().sprintf("select ITEM_TITLE,ORIGIN_DATETIME from PODCASTS \
                         where ID=%d",cast_cast_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    RDCgiError("unable to access cast record!");
    Exit(0);
  }
  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Confirm Delete</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");
  printf("<tr>\n");
  printf("<td bgcolor=\"%s\" >Are you sure you want to delete cast \"%s\", posted on %s at %s?</td>\n",
	 RD_WEB_LINE_COLOR2,
	 (const char *)q->value(0).toString(),
	 (const char *)q->value(1).toDateTime().toString("MM/dd/yyyy"),
	 (const char *)q->value(1).toDateTime().toString("hh:mm:ss"));
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<tr><td>&nbsp;</td></tr>\n");

  printf("<tr><td align=\"right\">\n");
  printf("<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\">\n");
  printf("<tr>\n");
  
  //
  // Yes Button
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td>\n");
  SetContext(RDCASTMANAGER_COMMAND_DELETE_CAST);
  printf("<input type=\"submit\" value=\"Yes\">\n");
  printf("</td>\n");
  printf("</form>\n");

  printf("<td>&nbsp;</td>\n");

  //
  // No Button
  //
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td>\n");
  cast_cast_id=-1;
  SetContext(RDCASTMANAGER_COMMAND_LIST_CASTS);
  printf("<input type=\"submit\" value=\"No\">\n");
  printf("</td>\n");
  printf("</form>\n");

  printf("</tr>\n");
  printf("</table>\n");

  printf("</td>\n");
  printf("</tr>\n");
  printf("</table>\n");
}


void MainObject::DeleteCast()
{
  QString sql;
  RDSqlQuery *q;
  QString errs;

  GetUserPerms();
  if(!cast_delete_priv) {
    RDCgiError("Insufficient privileges for this operation!");
    Exit(0);
  }

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  if(cast_cast_id<0) {
    RDCgiError("Missing CAST_ID");
    Exit(0);
  }

  RDFeed *feed=new RDFeed(cast_feed_id);
  RDPodcast *cast=new RDPodcast(cast_cast_id);
  cast->removeAudio(feed,&errs,cast_config->logXloadDebugData());
  delete cast;
  delete feed;

  sql=QString().sprintf("delete from PODCASTS where ID=%d",cast_cast_id);
  q=new RDSqlQuery(sql);
  delete q;

  ServeListCasts();
}


void MainObject::ServeSubscriptionReport()
{
  QString sql;
  RDSqlQuery *q;
  unsigned rss_total=0;
  unsigned audio_total=0;
  unsigned total=0;

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  cast_cast_id=-1;
  RDFeed *feed=new RDFeed(cast_key_name,this);

  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Subscription Report</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  //
  // Title Section
  //
  printf("<tr>\n");
  printf("<td align=\"center\" colspan=\"2\">\n");
  printf("<big><big><strong>Subscription Report for \"%s\"</strong></big></big>\n",
	 (const char *)feed->channelTitle());
  printf("</td></tr>\n");

  //
  // Column Headers
  //
  printf("<tr>\n");
  printf("<th>DATE</th>\n");
  printf("<th align=\"right\">RSS</th>\n");
  printf("<th align=\"right\">AUDIO</th>\n");
  printf("</tr>\n");


  //
  // Data Rows
  //
  QString line_colors[2];
  line_colors[0]=RD_WEB_LINE_COLOR1;
  line_colors[1]=RD_WEB_LINE_COLOR2;
  int current_color=0;
  QString keyname_esc=cast_key_name;
  keyname_esc.replace(" ","_");
  sql=QString().sprintf("select ACCESS_DATE,ACCESS_COUNT,CAST_ID from %s_FLG \
                         where (ACCESS_DATE>=\"%s\")&&\
                         (ACCESS_DATE<=\"%s\") \
                         order by ACCESS_DATE,CAST_ID desc",
			(const char *)keyname_esc,
			(const char *)cast_start_date.toString("yyyy-MM-dd"),
			(const char *)cast_end_date.toString("yyyy-MM-dd"));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(2).toUInt()==0) {
      printf("<tr><td align=\"center\" bgcolor=\"%s\">%s</td>\n",
	     (const char *)line_colors[current_color],
	     (const char *)q->value(0).toDate().
	     toString("MM/dd/yyyy"));
      printf("<td align=\"right\" bgcolor=\"%s\">%9u</td>\n",
	     (const char *)line_colors[current_color],
	     q->value(1).toUInt());
      printf("<td align=\"right\" bgcolor=\"%s\">%9u</td></tr>\n",
	     (const char *)line_colors[current_color],
	     total);
      total=0;
      rss_total+=q->value(1).toUInt();
      if(current_color==0) {
	current_color=1;
      }
      else {
	current_color=0;
      }
    }
    else {
      total+=q->value(1).toUInt();
      audio_total+=q->value(1).toUInt();
    }
  }
  printf("<tr><td align=\"center\"><strong>TOTAL</strong></td>\n");
  printf("<td align=\"right\"><strong>%9u</strong></td>\n",rss_total);
  printf("<td align=\"right\"><strong>%9u</strong></td></tr>\n",audio_total);
  delete q;

  delete feed;

  //
  // Close Button
  //
  printf("<tr><td colspan=\"3\">&nbsp;</td></tr>\n");
  printf("<tr>\n");
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td>&nbsp;</td>\n");
  printf("<td>&nbsp;</td>\n");
  printf("<td align=\"right\">\n");
  SetContext(RDCASTMANAGER_COMMAND_LIST_CASTS);
  printf("<input type=\"submit\" value=\"Close\">\n");
  printf("</td>\n");
  printf("</form>\n");
  printf("</tr>\n");

  printf("</table>\n");
  printf("</body>\n");
  printf("</html>\n");
  Exit(0);
}


void MainObject::PostEpisode()
{
  QString media_file;

  GetContext();
  GetUserPerms();
  if(!cast_add_priv) {
    RDCgiError("Insufficient privileges for this operation!");
    Exit(0);
  }
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  if(!cast_post->getValue("MEDIA_FILE",&media_file)) {
    RDCgiError("Missing MEDIA_FILE");
    Exit(0);
  }
  if(!cast_post->isFile("MEDIA_FILE")) {
    RDCgiError("No MEDIA_FILE submitted!");
    Exit(0);
  }
  RDStation *station=new RDStation(cast_config->stationName(),0);
  if(!station->exists()) {
    RDCgiError("Server station entry not found!");
    Exit(0);
  }
  RDFeed::Error err;
  RDFeed *feed=new RDFeed(cast_feed_id,this);
  int cast_id=feed->postFile(station,media_file,&err,
			     cast_config->logXloadDebugData(),cast_config);
  delete feed;
  delete station;
  if(err!=RDFeed::ErrorOk) {
    RDCgiError(RDFeed::errorString(err));
    Exit(0);
  }
  ServeEditCast(cast_id);
    
  Exit(0);
}


void MainObject::ServeEpisodeReport()
{
  QString sql;
  RDSqlQuery *q;
  unsigned total=0;

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }
  if(cast_cast_id<0) {
    RDCgiError("Missing CAST_ID");
    Exit(0);
  }
  RDPodcast *cast=new RDPodcast(cast_cast_id);

  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Episode Report</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  //
  // Title Section
  //
  printf("<tr>\n");
  printf("<td align=\"center\" colspan=\"2\">\n");
  printf("<big><big><strong>Episode Report for \"%s\"</strong></big></big>\n",
	 (const char *)cast->itemTitle());
  printf("</td></tr>\n");

  //
  // Column Headers
  //
  printf("<tr>\n");
  printf("<th>DATE</th>\n");
  printf("<th align=\"right\">DOWNLOADS</th>\n");
  printf("</tr>\n");


  //
  // Data Rows
  //
  QString line_colors[2];
  line_colors[0]=RD_WEB_LINE_COLOR1;
  line_colors[1]=RD_WEB_LINE_COLOR2;
  int current_color=0;
  QString keyname_esc=cast_key_name;
  keyname_esc.replace(" ","_");
  sql=QString().sprintf("select ACCESS_DATE,ACCESS_COUNT from %s_FLG \
                         where (ACCESS_DATE>=\"%s\")&&\
                         (ACCESS_DATE<=\"%s\")&& \
                         (CAST_ID=%d) order by ACCESS_DATE",
			(const char *)keyname_esc,
			(const char *)cast_start_date.toString("yyyy-MM-dd"),
			(const char *)cast_end_date.toString("yyyy-MM-dd"),
			cast_cast_id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("<tr><td align=\"center\" bgcolor=\"%s\">%s</td>\n",
	   (const char *)line_colors[current_color],
	   (const char *)q->value(0).toDate().
	   toString("MM/dd/yyyy"));
    printf("<td align=\"right\" bgcolor=\"%s\">%9u</td></tr>\n",
	   (const char *)line_colors[current_color],
	   q->value(1).toUInt());
    if(current_color==0) {
      current_color=1;
    }
    else {
      current_color=0;
    }
    total+=q->value(1).toUInt();
  }
  printf("<tr><td align=\"center\"><strong>TOTAL</strong></td>\n");
  printf("<td align=\"right\"><strong>%9u</strong></td></tr>\n",total);
  delete q;

  delete cast;

  //
  // Close Button
  //
  printf("<tr><td colspan=\"2\">&nbsp;</td></tr>\n");
  printf("<tr>\n");
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td>&nbsp;</td>\n");
  printf("<td align=\"right\">\n");
  SetContext(RDCASTMANAGER_COMMAND_EDIT_CAST);
  printf("<input type=\"submit\" value=\"Close\">\n");
  printf("</td>\n");
  printf("</form>\n");
  printf("</tr>\n");

  printf("</table>\n");
  printf("</body>\n");
  printf("</html>\n");
  Exit(0);
}


void MainObject::ServeDatePicker(int cmd)
{
  QDate yesterday_date=QDate::currentDate().addDays(-1);

  GetContext();
  if(cast_key_name.isEmpty()) {
    RDCgiError("Missing KEY_NAME");
    Exit(0);
  }
  if(cast_feed_id<0) {
    RDCgiError("Missing FEED_ID");
    Exit(0);
  }

  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Rivendell RDCastManager -- Choose Dates</title>\n");
  printf("</head>\n");

  printf("<body bgcolor=\"%s\">\n",RD_WEB_BACKGROUND_COLOR);
  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  printf("<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");

  printf("<tr>\n");
  printf("<td align=\"center\" colspan=\"2\">SELECT DATES</td>\n");
  printf("</tr>\n");

  //
  // Start Date
  //
  printf("<tr>\n");
  printf("<td align=\"right\">Start Date:</td>\n");
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  SetContext(cmd);
  printf("<td><input type=\"text\" name=\"START_MONTH\" value=\"%s\" size=\"2\" maxlength=\"2\">/\n",(const char *)yesterday_date.addMonths(-1).toString("MM"));
  printf("<input type=\"text\" name=\"START_DAY\" value=\"%s\" size=\"2\" maxlength=\"2\">/\n",(const char *)yesterday_date.addMonths(-1).toString("dd"));
  printf("<input type=\"text\" name=\"START_YEAR\" value=\"%s\" size=\"4\" maxlength=\"4\"></td>\n",(const char *)yesterday_date.addMonths(-1).toString("yyyy"));
  printf("</tr>\n");

  //
  // End Date
  //
  printf("<tr>\n");
  printf("<td align=\"right\">End Date:</td>\n");
  printf("<td><input type=\"text\" name=\"END_MONTH\" value=\"%s\" size=\"2\" maxlength=\"2\">/\n",(const char *)yesterday_date.toString("MM"));
  printf("<input type=\"text\" name=\"END_DAY\" value=\"%s\" size=\"2\" maxlength=\"2\">/\n",(const char *)yesterday_date.toString("dd"));
  printf("<input type=\"text\" name=\"END_YEAR\" value=\"%s\" size=\"4\" maxlength=\"4\"></td>\n",(const char *)yesterday_date.toString("yyyy"));
  printf("</tr>\n");

  //
  // Next Button
  //
  printf("<tr>\n");
  printf("<td align=\"right\" colspan=\"2\">\n");
  printf("<input type=\"submit\" value=\"Next\"></td>\n");
  printf("</form>\n");
  printf("</tr>\n");

  printf("</table>\n");
  printf("</body>\n");
  printf("</html>\n");
  Exit(0);
}


void MainObject::TitleSection(const QString &title,int cmd,int colspan) const
{
  printf("<tr>\n");
  printf("<td>&nbsp;</td>\n");
  printf("<td align=\"center\" colspan=\"%d\"><big><big>%s</big></big></td>\n",
	 colspan,(const char *)title);
  printf("<td><table cellpadding=\"3\" cellspacing=\"3\" border=\"0\">\n");
  //
  // Refresh Button
  //
  printf("<tr>\n");
  printf("<form action=\"rdcastmanager.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("<td align=\"right\">\n");
  SetContext(cmd);
  printf("<input type=\"submit\" value=\"Refresh\">\n");
  printf("</td>\n");
  printf("</form>\n");
  printf("</tr>\n");
  printf("</table></td>\n");
  printf("</tr>\n");
}


void MainObject::GetUserPerms()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select USERS.ADD_PODCAST_PRIV,\
                         USERS.EDIT_PODCAST_PRIV,USERS.DELETE_PODCAST_PRIV \
                         from USERS left join WEB_CONNECTIONS \
                         on USERS.LOGIN_NAME=WEB_CONNECTIONS.LOGIN_NAME \
                         where WEB_CONNECTIONS.SESSION_ID=%ld",
			cast_session_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    RDCgiError("Unable to load user security settings!");
    Exit(0);
  }
  cast_add_priv=RDBool(q->value(0).toString());
  cast_edit_priv=RDBool(q->value(1).toString());
  cast_delete_priv=RDBool(q->value(2).toString());
  delete q;
}


void MainObject::GetContext()
{
  int day=0;
  int month=0;
  int year=0;

  //
  // Basic Context
  //
  cast_post->getValue("KEY_NAME",&cast_key_name);
  cast_post->getValue("FEED_ID",&cast_feed_id);
  cast_post->getValue("CAST_ID",&cast_cast_id);

  //
  // Start Date
  //
  cast_post->getValue("START_DAY",&day);
  cast_post->getValue("START_MONTH",&month);
  cast_post->getValue("START_YEAR",&year);
  cast_start_date=QDate(year,month,day);

  //
  // End Date
  //
  day=0;
  month=0;
  year=0;
  cast_post->getValue("END_DAY",&day);
  cast_post->getValue("END_MONTH",&month);
  cast_post->getValue("END_YEAR",&year);
  cast_end_date=QDate(year,month,day);
}


void MainObject::SetContext(int cmd) const
{
  printf("<input type=\"hidden\" name=\"COMMAND\" value=\"%d\">\n",
	 cmd);
  printf("<input type=\"hidden\" name=\"SESSION_ID\" value=\"%ld\">\n",
	 cast_session_id);
  if(!cast_key_name.isEmpty()) {
    printf("<input type=\"hidden\" name=\"KEY_NAME\" value=\"%s\">\n",
	   (const char *)cast_key_name);
  }
  if(cast_feed_id>=0) {
    printf("<input type=\"hidden\" name=\"FEED_ID\" value=\"%d\">\n",
	   cast_feed_id);
  }
  if(cast_cast_id>=0) {
    printf("<input type=\"hidden\" name=\"CAST_ID\" value=\"%d\">\n",
	   cast_cast_id);
  }
}


void MainObject::Exit(int code)
{
  if(cast_post!=NULL) {
    delete cast_post;
  }
  exit(code);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
