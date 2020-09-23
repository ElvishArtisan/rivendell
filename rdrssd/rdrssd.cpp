// rdrssd.cpp
//
// Rivendell RSS Processor Service
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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
  bool ok=false;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;

  d_process_interval=RDRSSD_DEFAULT_PROCESS_INTERVAL;

  //
  // Open the Database
  //
  rda=new RDApplication("rdrssd","rdrssd",RDRSSD_USAGE,this);
  if(!rda->open(&err_msg,&err_type,false)) {
    fprintf(stderr,"rdrssd: %s\n",(const char *)err_msg);
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
    if(rda->cmdSwitch()->key(i)=="--process-interval") {
      d_process_interval=1000*rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(d_process_interval<=0)) {
	fprintf(stderr,
		"rdrssd: invalid value specified for --process-interval\n");
	exit(1);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
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
  d_timer->start(0);

  rda->syslog(LOG_DEBUG,"started");
}


void MainObject::timeoutData()
{
  QString sql;
  RDSqlQuery *q=NULL;

  sql=QString("select ")+
    "KEY_NAME "+  // 00
    "from FEEDS where "+
    "IS_SUPERFEED='N'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ProcessFeed(q->value(0).toString());
  }
  delete q;

  d_timer->start(d_process_interval);
}


void MainObject::ProcessFeed(const QString &key_name)
{
  QString sql;
  RDSqlQuery *q=NULL;
  QDateTime now=QDateTime::currentDateTime();
  QString now_str="\""+now.toString("yyyy-MM-dd hh:mm:ss")+"\"";
  QString err_msg;
  RDFeed *feed=new RDFeed(key_name,rda->config(),this);

  //
  // Update Posted XML
  //
  sql=QString("select ")+
    "PODCASTS.ID,"+                   // 00
    "PODCASTS.EXPIRATION_DATETIME "+  // 01
    "from PODCASTS left join FEEDS "+
    "on PODCASTS.FEED_ID=FEEDS.ID where "+
    "(FEEDS.KEY_NAME=\""+RDEscapeString(key_name)+"\") && "+
    "((FEEDS.LAST_BUILD_DATETIME<PODCASTS.EFFECTIVE_DATETIME) && "+
    "(PODCASTS.EFFECTIVE_DATETIME<"+now_str+") || "+
    "(FEEDS.LAST_BUILD_DATETIME<PODCASTS.EXPIRATION_DATETIME) && "+
    "(PODCASTS.EXPIRATION_DATETIME<"+now_str+"))";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    bool deleted=false;
    if(q->value(1).toDateTime()<now) {  // Delete expired cast
      RDPodcast *cast=new RDPodcast(rda->config(),q->value(0).toUInt());
      if(!cast->dropAudio(feed,&err_msg,false)) {
	rda->syslog(LOG_WARNING,
		    "audio purge failed for cast %u [%s] on feed \"%s\" [%s]",
		    q->value(0).toUInt(),
		    cast->itemTitle().toUtf8().constData(),
		    feed->keyName().toUtf8().constData(),
		    err_msg.toUtf8().constData());
      }
      sql=QString("delete from PODCASTS where ")+
	QString().sprintf("ID=%u",q->value(0).toUInt());
      RDSqlQuery::apply(sql);
      rda->syslog(LOG_INFO,"purged cast %u [%s] from feed \"%s\"",
		  q->value(0).toUInt(),cast->itemTitle().toUtf8().constData(),
		  feed->keyName().toUtf8().constData());
      delete cast;

      rda->ripc()->sendNotification(RDNotification::FeedType,
				 RDNotification::ModifyAction,feed->keyName());
      rda->ripc()->sendNotification(RDNotification::FeedItemType,
				    RDNotification::DeleteAction,
				    q->value(0).toUInt());
      deleted=true;
    }
    if(feed->postXml()) {
      rda->syslog(LOG_DEBUG,
		  "repost of XML for feed \"%s\" triggered by cast id %u",
		  key_name.toUtf8().constData(),q->value(0).toUInt());
      if(!deleted) {
	rda->ripc()->sendNotification(RDNotification::FeedType,
				      RDNotification::ModifyAction,
				      feed->keyName());
	rda->ripc()->sendNotification(RDNotification::FeedType,
				      RDNotification::ModifyAction,
				      feed->keyName());
      }
    }
    else {
      rda->syslog(LOG_WARNING,"repost of XML for feed \"%s\" failed");
    }
  }
  delete q;

  delete feed;
}


int main(int argv,char *argc[])
{
  QCoreApplication a(argv,argc);

  new MainObject();

  return a.exec();
}
