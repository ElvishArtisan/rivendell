// rdfeed_script.cpp
//
// An RSS Feed Generator for Rivendell.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdfeed_script.cpp,v 1.5.4.1 2013/10/16 21:14:38 cvs Exp $
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

#include <map>

#include <qapplication.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <rdconf.h>
#include <rdconfig.h>
#include <rdpodcast.h>
#include <rddb.h>
#include <rdweb.h>
#include <rdfeedlog.h>
#include <rdformpost.h>
#include <rdfeed.h>
#include <dbversion.h>

#include <rdfeed_script.h>

char server_name[PATH_MAX];

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  char keyname[10];
  int cast_id=-1;
  bool count;

  //
  // Validate Feed Key Name
  //
  if(getenv("QUERY_STRING")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: missing feed key name\n");
    exit(0);
  }
  int arg=0;
  while((getenv("QUERY_STRING")[arg]!=0)&&
	(getenv("QUERY_STRING")[arg]!='&')&&(arg<9)) {
    keyname[arg]=getenv("QUERY_STRING")[arg];
    arg++;
  }
  if(arg==9) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: invalid feed key name\n");
    exit(0);
  }
  keyname[arg]=0;
  RDGetPostInt(getenv("QUERY_STRING")+arg+1,"cast_id",&cast_id);

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
  // Read Configuration
  //
  RDConfig *config=new RDConfig();
  config->load();

  //
  // Determine Range
  //
  if(getenv("HTTP_RANGE")!=NULL) {
    count=ShouldCount(getenv("HTTP_RANGE"));
  }
  else {
    count=true;
  }

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(config->mysqlDriver());
  if(!db) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: unable to initialize connection to database\n");
    exit(0);
  }
  db->setDatabaseName(config->mysqlDbname());
  db->setUserName(config->mysqlUsername());
  db->setPassword(config->mysqlPassword());
  db->setHostName(config->mysqlHostname());
  if(!db->open()) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: unable to connect to database\n");
    db->removeDatabase(config->mysqlDbname());
    exit(0);
  }
  RDSqlQuery *q=new RDSqlQuery("select DB from VERSION");
  if(!q->first()) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdfeed: missing/invalid database version!\n");
    db->removeDatabase(config->mysqlDbname());
    exit(0);
  }
  if(q->value(0).toUInt()!=RD_VERSION_DATABASE) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdfeed: skewed database version!\n");
    db->removeDatabase(config->mysqlDbname());
    exit(0);
  }
  delete q;

  if(cast_id<0) {
    ServeRss(keyname,count);
  }
  ServeLink(keyname,cast_id,count);
}


void MainObject::ServeRss(const char *keyname,bool count)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select CHANNEL_TITLE,CHANNEL_DESCRIPTION,\
                         CHANNEL_CATEGORY,CHANNEL_LINK,CHANNEL_COPYRIGHT,\
                         CHANNEL_WEBMASTER,CHANNEL_LANGUAGE,\
                         LAST_BUILD_DATETIME,ORIGIN_DATETIME,\
                         HEADER_XML,CHANNEL_XML,ITEM_XML,BASE_URL,ID, \
                         UPLOAD_EXTENSION,CAST_ORDER,REDIRECT_PATH,\
                         BASE_PREAMBLE from FEEDS \
                         where KEY_NAME=\"%s\"",keyname);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: no feed matches the supplied key name\n");
    exit(0);
  }

  //
  // Log the Access
  //
  if(count) {
    RDIncrementFeedCount(keyname);
  }

  //
  // Redirect if necessary
  //
  if(!q->value(16).toString().isEmpty()) {
    Redirect(q->value(16).toString());
    delete q;
    exit(0);
  }

  //
  // Generate CGI Header
  //
  printf("Content-type: application/rss+xml\n\n");

  //
  // Render Header XML
  //
  printf("%s\n",(const char *)q->value(9).toString());

  //
  // Render Channel XML
  //
  printf("<channel>\n");
  printf("%s\n",(const char *)ResolveChannelWildcards(q));

  //
  // Render Item XML
  //
  sql=QString().sprintf("select ITEM_TITLE,ITEM_DESCRIPTION,ITEM_CATEGORY,\
                         ITEM_LINK,ITEM_AUTHOR,ITEM_SOURCE_TEXT,\
                         ITEM_SOURCE_URL,ITEM_COMMENTS,\
                         AUDIO_FILENAME,AUDIO_LENGTH,AUDIO_TIME,\
                         EFFECTIVE_DATETIME,ID from PODCASTS \
                         where (FEED_ID=%d)&&(STATUS=%d) \
                         order by EFFECTIVE_DATETIME",
			q->value(13).toUInt(),RDPodcast::StatusActive);
  if(q->value(15).toString()=="N") {
    sql+=" desc";
  }
  q1=new RDSqlQuery(sql);
  while(q1->next()) {
    printf("<item>\n");
    printf("%s\n",(const char *)
	   ResolveAuxWildcards(ResolveItemWildcards(keyname,q1,q),
			       getenv("QUERY_STRING"),
			       q->value(13).toUInt(),
			       q1->value(7).toUInt()));
//    printf("%s\n",(const char *)ResolveItemWildcards(q1,q));
    printf("</item>\n");
  }
  delete q1;

  printf("</channel>\n");
  printf("</rss>\n");
  delete q;

  exit(0);
}


void MainObject::ServeLink(const char *keyname,int cast_id,bool count)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select FEEDS.BASE_URL,PODCASTS.AUDIO_FILENAME from \
                         FEEDS left join PODCASTS \
                         on FEEDS.ID=PODCASTS.FEED_ID \
                         where (FEEDS.KEY_NAME=\"%s\")&&(PODCASTS.ID=%d)",
			(const char *)keyname,cast_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    RDCgiError("Unable to retrieve cast record!");
  }
  if(count) {
    RDIncrementCastCount(keyname,cast_id);
  }
  printf("Content-type: audio/x-mpeg\n");
  printf("Location: %s/%s\n\n",(const char *)q->value(0).toString(),
	 (const char *)q->value(1).toString());
  delete q;

  exit(0);
}


QString MainObject::ResolveChannelWildcards(RDSqlQuery *chan_q)
{
  QString ret=chan_q->value(10).toString();
  ret.replace("%TITLE%",RDXmlEscape(chan_q->value(0).toString()));
  ret.replace("%DESCRIPTION%",RDXmlEscape(chan_q->value(1).toString()));
  ret.replace("%CATEGORY%",RDXmlEscape(chan_q->value(2).toString()));
  ret.replace("%LINK%",RDXmlEscape(chan_q->value(3).toString()));
  ret.replace("%COPYRIGHT%",RDXmlEscape(chan_q->value(4).toString()));
  ret.replace("%WEBMASTER%",RDXmlEscape(chan_q->value(5).toString()));
  ret.replace("%LANGUAGE%",RDXmlEscape(chan_q->value(6).toString()));
  ret.replace("%BUILD_DATE%",chan_q->value(7).toDateTime().
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%PUBLISH_DATE%",chan_q->value(8).toDateTime().
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%GENERATOR%",QString().sprintf("Rivendell %s",VERSION));

  return ret;
}


QString MainObject::ResolveItemWildcards(const QString &keyname,
					 RDSqlQuery *item_q,RDSqlQuery *chan_q)
{
  RDFeed *feed=new RDFeed(keyname);
  QString ret=chan_q->value(11).toString();
  ret.replace("%ITEM_TITLE%",RDXmlEscape(item_q->value(0).toString()));
  ret.replace("%ITEM_DESCRIPTION%",
	      RDXmlEscape(item_q->value(1).toString()));
  ret.replace("%ITEM_CATEGORY%",
	      RDXmlEscape(item_q->value(2).toString()));
  ret.replace("%ITEM_LINK%",RDXmlEscape(item_q->value(3).toString()));
  ret.replace("%ITEM_AUTHOR%",RDXmlEscape(item_q->value(4).toString()));
  ret.replace("%ITEM_SOURCE_TEXT%",
	      RDXmlEscape(item_q->value(5).toString()));
  ret.replace("%ITEM_SOURCE_URL%",
	      RDXmlEscape(item_q->value(6).toString()));
  ret.replace("%ITEM_COMMENTS%",
	      RDXmlEscape(item_q->value(7).toString()));
  ret.replace("%ITEM_AUDIO_URL%",
	      (const char *)RDXmlEscape(feed->
	        audioUrl(RDFeed::LinkCounted,server_name,
			 item_q->value(12).toUInt())));
  ret.replace("%ITEM_AUDIO_LENGTH%",item_q->value(9).toString());
  ret.replace("%ITEM_AUDIO_TIME%",
	      RDGetTimeLength(item_q->value(10).toInt(),false,false));
  ret.replace("%ITEM_PUBLISH_DATE%",item_q->value(11).toDateTime().
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%ITEM_GUID%",RDPodcast::guid(chan_q->value(12).toString(),
					    item_q->value(8).toString(),
					    chan_q->value(11).toUInt(),
					    item_q->value(12).toUInt()));
  delete feed;
  return ret;
}


QString MainObject::ResolveAuxWildcards(QString xml,QString keyname,
					unsigned feed_id,unsigned cast_id)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  keyname.replace(" ","_");
  sql=QString().sprintf("select VAR_NAME from AUX_METADATA where FEED_ID=%u",
			feed_id);
  q=new RDSqlQuery(sql);
  if(q->size()==0) {
    delete q;
    return xml;
  }
  sql="select ";
  while(q->next()) {
    sql+=q->value(0).toString().mid(1,q->value(0).toString().length()-2);
    sql+=",";
  }
  sql=sql.left(sql.length()-1);
  sql+=QString().sprintf(" from %s_FIELDS where CAST_ID=%u",
			 (const char *)keyname,cast_id);
  q->seek(-1);
  q1=new RDSqlQuery(sql);
  while(q1->next()) {
    q->next();
    xml.replace(q->value(0).toString(),
		RDXmlEscape(q1->value(0).toString()));
  }
  delete q1;
  delete q;

  return xml;
}


bool MainObject::ShouldCount(const QString &hdr)
{
  bool ret=false;
  QStringList lines=QStringList::split("\n",hdr);
  int n;
  QString str;

  for(unsigned i=0;i<lines.size();i++) {
    if((n=lines[i].find("="))>0) {
      if(lines[i].left(n).lower()=="bytes") {
	str=lines[i].right(lines[i].length()-n-1).stripWhiteSpace();
	n=str.find("-");
	if(n==0) {
	  ret=true;
	}
	if(n>0) {
	  if(str.left(n)=="0") {
	    ret=true;
	  }
	}
      }
    }
  }

  return ret;
}


void MainObject::Redirect(const QString &url)
{
  printf("Status: 301 Moved Permanently\n");
  printf("Location: %s\n",(const char *)url);
  printf("Content-type: text/html\n");
  printf("\n");
  printf("The feed has been relocated to %s.\n",(const char *)url);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
