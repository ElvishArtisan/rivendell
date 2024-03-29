// rdrssd.cpp
//
// Rivendell RSS Processor Service
//
//   (C) Copyright 2020-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <stdio.h>

#include <QCoreApplication>

#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdfeed.h>
#include <rdpodcast.h>

#include "rdrssd.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("rdrssd","rdrssd",
						   RDRSSD_USAGE,false,this));
  if(!rda->open(&err_msg,&err_type,false,true)) {
    fprintf(stderr,"rdrssd: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Shed root permissions
  //
  if(getuid()==0) {
    if(setgid(rda->config()->pypadGid())!=0) {
      rda->syslog(LOG_ERR,"unable to set GID to %d [%s], exiting",
		  rda->config()->pypadGid(),strerror(errno));
      exit(1);
    }
    if(setuid(rda->config()->pypadUid())!=0) {
      rda->syslog(LOG_ERR,"unable to set UID to %d [%s], exiting",
		  rda->config()->pypadUid(),strerror(errno));
      exit(1);
    }
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      rda->syslog(LOG_ERR,"unknown command option \"%s\"",
		  (const char *)rda->cmdSwitch()->key(i).toUtf8());
      exit(2);
    }
  }

  //
  // Connect to ripcd(8)
  //
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Scan Timer
  //
  d_timer=new QTimer(this);
  d_timer->setSingleShot(true);
  connect(d_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));

  StartTimer();

  rda->syslog(LOG_DEBUG,"started");
}


void MainObject::timeoutData()
{
  QString sql;
  RDSqlQuery *q=NULL;

  sql=QString("select ")+
    "`KEY_NAME` "+  // 00
    "from `FEEDS` where "+
    "`IS_SUPERFEED`='N'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ProcessFeed(q->value(0).toString());
  }
  delete q;

  StartTimer();
}


void MainObject::StartTimer()
{
  QDateTime now=QDateTime::currentDateTime();
  QDateTime then=now.addSecs(60);

  then.setTime(QTime(then.time().hour(),then.time().minute(),2));
  d_timer->start(now.msecsTo(then));
}


void MainObject::ProcessFeed(const QString &key_name)
{
  QString sql;
  RDSqlQuery *q=NULL;
  QDateTime now=QDateTime::currentDateTime();
  QString now_str="'"+now.toString("yyyy-MM-dd hh:mm:ss")+"'";
  QString err_msg;
  RDFeed *feed=new RDFeed(key_name,rda->config(),this);

  //
  // Update Posted XML
  //
  rda->syslog(LOG_DEBUG,"examining feed \"%s\"",key_name.toUtf8().constData());
  bool modified=false;
  sql=QString("select ")+
    "`PODCASTS`.`ID`,"+                   // 00
    "`PODCASTS`.`EXPIRATION_DATETIME`,"+  // 01
    "`ITEM_TITLE` "+                      // 02
    "from `PODCASTS` left join `FEEDS` "+
    "on `PODCASTS`.`FEED_ID`=`FEEDS`.`ID` where "+
    "(`FEEDS`.`KEY_NAME`='"+RDEscapeString(key_name)+"') && "+
    "((`FEEDS`.`LAST_BUILD_DATETIME`<`PODCASTS`.`EFFECTIVE_DATETIME`) && "+
    "(`PODCASTS`.`EFFECTIVE_DATETIME`<"+now_str+") || "+
    "(`FEEDS`.`LAST_BUILD_DATETIME`<`PODCASTS`.`EXPIRATION_DATETIME`) && "+
    "(`PODCASTS`.`EXPIRATION_DATETIME`<"+now_str+"))";

  //  rda->syslog(LOG_NOTICE,"scanning feed %s: SQL: %s\n",
  //	 key_name.toUtf8().constData(),sql.toUtf8().constData());

  q=new RDSqlQuery(sql);
  while(q->next()) {
    modified=true;
    if((!q->value(1).isNull())&&(q->value(1).toDateTime()<now)) {
      //
      // Delete expired item
      //
      RDPodcast *cast=new RDPodcast(rda->config(),q->value(0).toUInt());
      if(!cast->dropAudio(feed,&err_msg,false)) {
	rda->syslog(LOG_WARNING,
		    "audio purge failed for cast %u [%s] on feed \"%s\" [%s]",
		    q->value(0).toUInt(),
		    cast->itemTitle().toUtf8().constData(),
		    feed->keyName().toUtf8().constData(),
		    err_msg.toUtf8().constData());
      }
      delete cast;
      sql=QString("delete from `PODCASTS` where ")+
	QString::asprintf("`ID`=%u",q->value(0).toUInt());
      RDSqlQuery::apply(sql);
      rda->syslog(LOG_INFO,"purged cast %u [%s] from feed \"%s\"",
		  q->value(0).toUInt(),q->value(2).toString().toUtf8().
		  constData(),
		  feed->keyName().toUtf8().constData());
      rda->ripc()->sendNotification(RDNotification::FeedType,
				 RDNotification::ModifyAction,feed->keyName());
      rda->ripc()->sendNotification(RDNotification::FeedItemType,
				    RDNotification::DeleteAction,
				    q->value(0).toUInt());
    }
    else {
      //
      // Enable embargoed item
      //
      rda->syslog(LOG_INFO,
	   "auto-posting item for feed \"%s\": cast_id: %u, item_title: \"%s\"",
		  key_name.toUtf8().constData(),q->value(0).toUInt(),
		  q->value(2).toString().toUtf8().constData());
      rda->ripc()->sendNotification(RDNotification::FeedType,
				    RDNotification::ModifyAction,
				    feed->keyName());
      rda->ripc()->sendNotification(RDNotification::FeedItemType,
				    RDNotification::ModifyAction,
				    q->value(0).toUInt());
    }
  }
  delete q;

  //
  // Update feed XML
  //
  if(modified) {
    if(!feed->postXml(&err_msg)) {
      rda->syslog(LOG_INFO,
		  "xml update FAILED for item in feed \"%s\" [%s]",
		  key_name.toUtf8().constData(),err_msg.toUtf8().constData());
    }
  }

  delete feed;
}


int main(int argv,char *argc[])
{
  QCoreApplication a(argv,argc);

  new MainObject();

  return a.exec();
}
