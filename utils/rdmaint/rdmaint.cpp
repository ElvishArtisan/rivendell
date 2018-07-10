// rdmaint.cpp
//
// A Utility for running periodic system maintenance.
//
//   (C) Copyright 2008-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <qfile.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlibrary_conf.h>
#include <rdlog.h>
#include <rdmaint.h>
#include <rdpodcast.h>
#include <rdrehash.h>
#include <rdurl.h>

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString sql;
  RDSqlQuery *q;
  QString err_msg;

  //
  // Initialize Data Structures
  //
  maint_verbose=false;
  maint_system=false;

  //
  // Open the Database
  //
  rda=new RDApplication("rdmaint","rdmaint",RDMAINT_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdmaint: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  if(rda->cmdSwitch()->keys()>3) {
    fprintf(stderr,"\n");
    fprintf(stderr,"%s",RDMAINT_USAGE);
    fprintf(stderr,"\n");
    exit(2);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      maint_verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--system") {
      maint_system=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdmaint: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
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

  sql="update VERSION set LAST_MAINT_DATETIME=now()";
  q=new RDSqlQuery(sql);
  delete q;

  PurgeCuts();
  PurgeLogs();
  PurgeElr();
  PurgeGpioEvents();
  PurgeWebapiAuths();
  PurgeStacks();
  RehashCuts();
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

  sql=QString("select NAME,CUT_SHELFLIFE,DELETE_EMPTY_CARTS from GROUPS ")+
    "where CUT_SHELFLIFE>=0";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("select CART.NUMBER,CUTS.CUT_NAME \
                           from CUTS left join CART \
                           on CUTS.CART_NUMBER=CART.NUMBER \
                           where (CART.GROUP_NAME=\"%s\")&&\
                           (CUTS.END_DATETIME<\"%s 00:00:00\")",
			  (const char *)RDEscapeString(q->value(0).toString()),
			  (const char *)dt.addDays(-q->value(1).toInt()).
			  toString("yyyy-MM-dd"));
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      RDCart *cart=new RDCart(q1->value(0).toUInt());
      if(cart->removeCut(rda->station(),rda->user(),q1->value(1).toString(),
			 rda->config())) {
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
	  cart->remove(rda->station(),rda->user(),rda->config());
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
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select ")+
    "NAME,"+
    "ELR_SHELFLIFE "+
    "from SERVICES where "+
    "ELR_SHELFLIFE>=0";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("delete from ELR_LINES where ")+
      "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
      "EVENT_DATETIME<\""+
      dt.addDays(-q->value(1).toInt()).toString("yyyy-MM-dd")+" 00:00:00\"";
    RDSqlQuery::apply(sql);
  }
  delete q;
}


void MainObject::PurgeDropboxes()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select DROPBOX_PATHS.FILE_PATH,DROPBOX_PATHS.ID from \
                         DROPBOXES left join DROPBOX_PATHS \
                         on (DROPBOXES.ID=DROPBOX_PATHS.DROPBOX_ID) \
                         where DROPBOXES.STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(rda->config()->
						     stationName()));
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


void MainObject::PurgeWebapiAuths()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from WEBAPI_AUTHS where EXPIRATION_DATETIME<now()");
  q=new RDSqlQuery(sql);
  delete q;
}


void MainObject::PurgeStacks()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  int stackid;
  int stacksize;
  int artistsep=50000;
  int titlesep=50000;

  sql="select MAX(ARTISTSEP) from CLOCKS";
  q=new RDSqlQuery(sql);
  if(q->next()) {
    artistsep=q->value(0).toInt();
  }
  delete q;

  sql="select MAX(TITLE_SEP) from EVENTS";
  q=new RDSqlQuery(sql);
  if(q->next()) {
    titlesep=q->value(0).toInt();
  }
  delete q;

  stacksize=(artistsep<titlesep)?titlesep:artistsep;

  sql="select NAME from SERVICES";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("select MAX(SCHED_STACK_ID) from STACK_LINES where ")+
      "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
    q1=new RDSqlQuery(sql);
    if (q1->next()) {
      stackid=q1->value(0).toUInt();
      if (stackid-stacksize > 0) {
        sql=QString("delete from STACK_LINES where ")+
	  "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	  QString().sprintf("SCHED_STACK_ID<=%d",stackid-stacksize);
	RDSqlQuery::apply(sql);

        sql=QString("update STACK_LINES set ")+
	  QString().sprintf("SCHED_STACK_ID=SCHED_STACK_ID-%d where ",
			    stackid-stacksize)+
	  "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
        q2=new RDSqlQuery(sql);
        delete q2;
      }
    }
    delete q1;
  }
  delete q;
}


void MainObject::RehashCuts()
{
  QString sql;
  RDSqlQuery *q;
  RDRehash::ErrorCode err;

  sql="select CUT_NAME from CUTS where SHA1_HASH is null limit 100";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("CUT: %s\n",(const char *)q->value(0).toString());
    if((err=RDRehash::rehash(rda->station(),rda->user(),rda->config(),
			     RDCut::cartNumber(q->value(0).toString()),
			     RDCut::cutNumber(q->value(0).toString())))!=RDRehash::ErrorOk) {
      rda->config()->
	log("rdmaint",
	    RDConfig::LogErr,QString().sprintf("failed to rehash cut %s [%s]",
					       (const char *)q->value(0).toString(),
					       (const char *)RDRehash::errorText(err)));
      
    }
    if(maint_verbose) {
      fprintf(stderr,"rehashed cut \"%s\"\n",
	      (const char *)q->value(0).toString());
    }
    sleep(1);
  }
  delete q;
}

  
int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
