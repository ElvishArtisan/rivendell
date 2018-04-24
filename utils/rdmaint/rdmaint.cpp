// rdmaint.cpp
//
// A Utility for running periodic system maintenance.
//
//   (C) Copyright 2008-2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdurl.h>
#include <rdpodcast.h>
#include <rdcart.h>
#include <rdlog.h>

#include "rdmaint.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  new RDApplication(RDApplication::Console,"rdmain",RDMAINT_USAGE);

  QString sql;
  RDSqlQuery *q;

  //
  // Initialize Data Structures
  //
  maint_verbose=false;
  maint_system=false;

  //
  // Read Command Options
  //
  if(rda->cmdSwitch()->keys()>3) {
    fprintf(stderr,"\n");
    fprintf(stderr,"%s",RDMAINT_USAGE);
    fprintf(stderr,"\n");
    exit(256);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      maint_verbose=true;
    }
    if(rda->cmdSwitch()->key(i)=="--system") {
      maint_system=true;
    }
  }

  //
  // Get User
  //
  sql="select LOGIN_NAME from USERS where ADMIN_CONFIG_PRIV=\"Y\"";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    fprintf(stderr,"unable to find valid user\n");
    exit(256);
  }
  rda->setUser(q->value(0).toString());
  delete q;

  if(maint_system) {
    RunSystemMaintenance();
  }
  else {
    RunLocalMaintenance();
  }

  exit(0);
}


void MainObject::RunSystemMaintenance()
{
  QString sql;
  RDSqlQuery *q;

  PurgeCuts();
  PurgeLogs();
  PurgeElr();
  PurgeGpioEvents();
  sql="update VERSION set LAST_MAINT_DATETIME=now()";
  q=new RDSqlQuery(sql);
  delete q;
}


void MainObject::RunLocalMaintenance()
{
  PurgeDropboxes();
}


void MainObject::PurgeCuts()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select ")+
    "NAME,"+
    "CUT_SHELFLIFE,"+
    "DELETE_EMPTY_CARTS "+
    "from GROUPS where "+
    "CUT_SHELFLIFE>=0";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select ")+
      "CART.NUMBER,"+
      "CUTS.CUT_NAME "+
      "from CUTS left join CART "+
      "on CUTS.CART_NUMBER=CART.NUMBER where "+
      "(CART.GROUP_NAME=\""+RDEscapeString(q->value(0).toString())+"\")&&"+
      "(CUTS.END_DATETIME<\""+
      dt.addDays(-q->value(1).toInt()).toString("yyyy-MM-dd")+" 00:00:00\")";
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      RDCart *cart=new RDCart(q1->value(0).toUInt());
      if(cart->removeCut(q1->value(1).toString())) {
	rda->config()->
	  log("rdmaint",RDConfig::LogInfo,QString().sprintf("purged cut %s",
			(const char *)q1->value(1).toString()));
      }
      else {
	rda->config()->
	  log("rdmaint",RDConfig::LogErr,QString().
	      sprintf("unable to purge cut %s: audio deletion error",
		      (const char *)q1->value(1).toString()));
      }
      if(q->value(2).toString()=="Y") {  // Delete Empty Cart
	sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%u",
			      q1->value(0).toUInt());
	q2=new RDSqlQuery(sql);
	if(!q2->first()) {
	  cart->remove();
	  rda->config()->
	    log("rdmaint",RDConfig::LogInfo,QString().
		sprintf("deleted purged cart %06u",cart->number()));
	}
	delete q2;
      }
      delete cart;
    }
    delete q1;
  }
  delete q;
}


void MainObject::PurgeLogs()
{
  QString sql;
  RDSqlQuery *q;
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select NAME from LOGS where ")+
    "(PURGE_DATE!=\"0000-00-00\")&&"+
    "(PURGE_DATE is not null)&&"+
    "(PURGE_DATE<\""+dt.date().toString("yyyy-MM-dd")+"\")";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    rda->config()->
      log("rdmain",RDConfig::LogInfo,QString().sprintf("purged log %s",
		       (const char *)q->value(0).toString()));
    RDLog *log=new RDLog(q->value(0).toString());
    log->remove(rda->station(),rda->user(),rda->config());
    delete log;
  }
  delete q;
}


void MainObject::PurgeElr()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());

  sql="select NAME,ELR_SHELFLIFE from SERVICES where ELR_SHELFLIFE>=0";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QString tablename=q->value(0).toString()+"_SRT";
    tablename.replace(" ","_");
    sql=QString("delete from `")+tablename+"` where "+
      "EVENT_DATETIME<\""+
      dt.addDays(-q->value(1).toInt()).toString("yyyy-MM-dd")+" 00:00:00\"";
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}


void MainObject::PurgeDropboxes()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("select ")+
    "DROPBOX_PATHS.FILE_PATH,"+
    "DROPBOX_PATHS.ID "+
    "from DROPBOXES left join DROPBOX_PATHS "+
    "on (DROPBOXES.ID=DROPBOX_PATHS.DROPBOX_ID) where "+
    "DROPBOXES.STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+
    "\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(!QFile::exists(q->value(0).toString())) {
      sql=QString().sprintf("delete from DROPBOX_PATHS where ID=%d",
			    q->value(1).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
  }
  delete q;
}


void MainObject::PurgeGpioEvents()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from GPIO_EVENTS where ")+
    "EVENT_DATETIME<\""+
    QDate::currentDate().addDays(-30).toString("yyyy-MM-dd")+" 00:00:00\"";
  q=new RDSqlQuery(sql);
  delete q;
}

  
int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
