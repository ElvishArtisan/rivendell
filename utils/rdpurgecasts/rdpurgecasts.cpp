// rdpurgecasts.cpp
//
// A Utility to Purge Expired Podcasts.
//
//   (C) Copyright 2007,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <limits.h>
#include <glob.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <qapplication.h>
#include <qdir.h>

#include <rd.h>
#include <rddb.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdfeed.h>
#include <rdlibrary_conf.h>
#include <rdpodcast.h>
#include <rdurl.h>

#include "rdpurgecasts.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString sql;
  RDSqlQuery *q;
  QString err_msg;

  //
  // Initialize Data Structures
  //
  purge_verbose=false;

  //
  // Open the Database
  //
  rda=new RDApplication("rdpurgecasts","rdpurgecasts",RDPURGECASTS_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdpurgecasts: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  if(rda->cmdSwitch()->keys()>2) {
    fprintf(stderr,"\n");
    fprintf(stderr,"%s",RDPURGECASTS_USAGE);
    fprintf(stderr,"\n");
    exit(256);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      purge_verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }

    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdpurgecasts: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Scan Podcasts
  //
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  sql=QString().sprintf("select ID,ORIGIN_DATETIME,SHELF_LIFE from PODCASTS \
                         where (SHELF_LIFE>0)&&(STATUS=%u)",
			RDPodcast::StatusActive);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(1).toDateTime().addDays(q->value(2).toInt())<
       current_datetime) {
      PurgeCast(q->value(0).toUInt());
    }
  }
  delete q;

  exit(0);
}


void MainObject::PurgeCast(unsigned id)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString cmd;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  RDFeed *feed=NULL;
  RDPodcast *cast=NULL;
  QString errs;

  sql=QString("select ")+
    "FEEDS.ID,"+             // 00
    "FEEDS.KEEP_METADATA,"+  // 01
    "FEEDS.KEY_NAME "+       // 02
    "from PODCASTS left join FEEDS "+
    "on(PODCASTS.FEED_ID=FEEDS.ID) where "+
    QString().sprintf("PODCASTS.ID=%u",id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    feed=new RDFeed(q->value(0).toUInt(),rda->config());
    cast=new RDPodcast(rda->config(),id);
    cast->removeAudio(feed,&errs,rda->config()->logXloadDebugData());
    if(purge_verbose) {
      printf("purging cast: ID=%d,cmd=\"%s\"\n",id,(const char *)cmd);
    }
    delete cast;
    delete feed;
    if(RDBool(q->value(1).toString())) {
      sql=QString("update PODCASTS set ")+
	QString().sprintf("STATUS=%u where ",RDPodcast::StatusExpired)+
	QString().sprintf("ID=%u",id);
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    else {
      sql=QString("delete from CAST_DOWNLOADS where ")+
	"FEED_KEY_NAME=\""+RDEscapeString(q->value(2).toString())+"\" && "+
	QString().sprintf("CAST_ID=%d",id);
      q1=new RDSqlQuery(sql);
      delete q1;

      sql=QString("delete from PODCASTS where ")+
	QString().sprintf("ID=%d",id);
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    sql=QString("update FEEDS set ")+
      "LAST_BUILD_DATETIME=\""+
      RDEscapeString(current_datetime.toString("yyyy-MM-dd hh:mm:ss"))+
      "\" where "+
      QString().sprintf("ID=%u",q->value(0).toUInt());
    q1=new RDSqlQuery(sql);
    delete q1;

  }
  delete q;
}

  
int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
