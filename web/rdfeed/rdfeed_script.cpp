// rdfeed_script.cpp
//
// An RSS Feed Generator for Rivendell.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdfeed.h>
#include <rdfeedlog.h>
#include <rdformpost.h>
#include <rdpodcast.h>
#include <dbversion.h>
#include <rdweb.h>

#include "rdfeed_script.h"

char server_name[PATH_MAX];

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  char keyname[10];
  int cast_id=-1;
  bool count;

  //
  // Validate Feed Key Name
  //
  if(getenv("QUERY_STRING")==NULL) {
    printf("Content-type: text/html\n");
    printf("Status: 400\n");
    printf("\n");
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
    printf("Content-type: text/html\n");
    printf("Status: 400\n");
    printf("\n");
    printf("rdfeed: invalid feed key name\n");
    exit(0);
  }
  keyname[arg]=0;
  RDGetPostInt(getenv("QUERY_STRING")+arg+1,"cast_id",&cast_id);

  //
  // Get the Server Name
  //
  if(getenv("SERVER_NAME")==NULL) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n");
    printf("\n");
    printf("rdfeed: missing SERVER_NAME\n");
    exit(0);
  }
  strncpy(server_name,getenv("SERVER_NAME"),PATH_MAX);

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
  // Open the Database
  //
  rda=new RDApplication("rdfeed.xml","rdfeed.xml",RDFEED_XML_USAGE,this);
  if(!rda->open(&err_msg)) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n");
    printf("\n");
    printf("rdfeed.xml: %s\n",(const char *)err_msg.utf8());
    exit(0);
  }

  /*
  printf("Content-type: text/html\n\n");
  QString sql;
  RDSqlQuery *q;
  sql=QString("show variables like '%character_set%'");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("%s: %s<br>\n",(const char *)q->value(0).toString(),
	   (const char *)q->value(1).toString());
  }
  delete q;
  sql=QString("show variables like '%collation%'");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("%s: %s<br>\n",(const char *)q->value(0).toString(),
	   (const char *)q->value(1).toString());
  }
  delete q;
  exit(0);
  */

  if(cast_id<0) {
    ServeRss(keyname,count);
  }
  ServeLink(keyname,cast_id,count);
}


void MainObject::ServeRss(const char *keyname,bool count)
{
  QString err_msg;
  bool ok=false;

  RDFeed *feed=new RDFeed(keyname,rda->config(),this);
  if(!feed->exists()) {
    printf("Content-type: text/html\n\n");
    printf("no feed matches the supplied key name\n");
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
  if(!feed->redirectPath().isEmpty()) {
    Redirect(feed->redirectPath());
    exit(0);
  }

  //
  // Generate CGI Header
  //
  printf("Content-type: application/rss+xml; charset=UTF-8\n\n");

  //
  // Generate the XML
  //
  QString xml=feed->rssXml(&err_msg,&ok);
  if(!ok) {
    printf("Content-type: text/html\n\n");
    printf("%s\n",(const char *)err_msg.toUtf8());
    exit(0);
  }
  printf("%s\r\n",(const char *)xml.toUtf8());

  exit(0);
}


void MainObject::ServeLink(const char *keyname,int cast_id,bool count)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "FEEDS.BASE_URL,"+           // 00
    "PODCASTS.AUDIO_FILENAME "+  // 01
    "from FEEDS left join PODCASTS "+
    "on FEEDS.ID=PODCASTS.FEED_ID where "+
    "(FEEDS.KEY_NAME=\""+RDEscapeString(keyname)+"\")&&"+
    QString().sprintf("(PODCASTS.ID=%d)",cast_id);
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
	 (const char *)q->value(1).toString().utf8());
  delete q;

  exit(0);
}


bool MainObject::ShouldCount(const QString &hdr)
{
  bool ret=false;
  QStringList lines=hdr.split("\n");
  int n;
  QString str;

  for(int i=0;i<lines.size();i++) {
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
  printf("Location: %s\n",(const char *)url.utf8());
  printf("Content-type: text/html\n");
  printf("\n");
  printf("The feed has been relocated to %s.\n",(const char *)url.utf8());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
