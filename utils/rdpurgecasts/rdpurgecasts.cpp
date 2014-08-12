// rdpurgecasts.cpp
//
// A Utility to Purge Expired Podcasts.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpurgecasts.cpp,v 1.7 2011/06/21 22:20:44 cvs Exp $
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
#include <rdconf.h>
#include <rdpurgecasts.h>
#include <rdlibrary_conf.h>
#include <rdescape_string.h>
#include <rddb.h>
#include <rdurl.h>
#include <rdfeed.h>
#include <rdpodcast.h>


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  QString sql;
  RDSqlQuery *q;
  unsigned schema=0;

  //
  // Initialize Data Structures
  //
  purge_verbose=false;

  //
  // Read Command Options
  //
  purge_cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),
			     "rdpurgecasts",RDPURGECASTS_USAGE);
  if(purge_cmd->keys()>2) {
    fprintf(stderr,"\n");
    fprintf(stderr,"%s",RDPURGECASTS_USAGE);
    fprintf(stderr,"\n");
    delete purge_cmd;
    exit(256);
  }
  for(unsigned i=0;i<purge_cmd->keys();i++) {
    if(purge_cmd->key(i)=="--verbose") {
      purge_verbose=true;
    }
  }

  //
  // Read Configuration
  //
  purge_config=new RDConfig();
  purge_config->load();

  //
  // Open Database
  //
  QString err (tr("rdpurgecasts: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete purge_cmd;
    exit(256);
  }
  delete purge_cmd;

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

  sql=QString().sprintf("select FEEDS.ID,FEEDS.KEEP_METADATA,FEEDS.KEY_NAME \
                         from PODCASTS left join FEEDS \
                         on(PODCASTS.FEED_ID=FEEDS.ID) \
                         where PODCASTS.ID=%u",id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    feed=new RDFeed(q->value(0).toUInt());
    cast=new RDPodcast(id);
    cast->removeAudio(feed,&errs,purge_config->logXloadDebugData());
    if(purge_verbose) {
      printf("purging cast: ID=%d,cmd=\"%s\"\n",id,(const char *)cmd);
    }
    delete cast;
    delete feed;
    if(RDBool(q->value(1).toString())) {
      sql=QString().sprintf("update PODCASTS set STATUS=%u \
                             where ID=%u",
			    RDPodcast::StatusExpired,id);
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    else {
      QString keyname=q->value(2).toString();
      keyname.replace(" ","_");
      sql=QString().sprintf("delete from %s_FLG where CAST_ID=%d",
			    (const char *)keyname,id);
      q1=new RDSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("delete from PODCASTS where ID=%d",id);
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    sql=QString().sprintf("update FEEDS set LAST_BUILD_DATETIME=\"%s\" \
                           where ID=%u",
			  (const char *)current_datetime.
			  toString("yyyy-MM-dd hh:mm:ss"),
			  q->value(0).toUInt());
    q1=new RDSqlQuery(sql);
    delete q1;

  }
  delete q;
}

  
int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
