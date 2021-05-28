// rdmaint.cpp
//
// A Utility for running periodic system maintenance.
//
//   (C) Copyright 2008-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <ctype.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

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
  if(!rda->open(&err_msg,NULL,false)) {
    fprintf(stderr,"rdmaint: %s\n",err_msg.toUtf8().constData());
    rda->syslog(LOG_ERR,"%s",err_msg.toUtf8().constData());
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
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::userData()
{
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

  PrintMessage("Starting System Maintenance");

  PurgeCuts();
  PurgeLogs();
  PurgeElr();
  PurgeGpioEvents();
  PurgeWebapiAuths();
  PurgeStacks();
  RehashCuts();

  PrintMessage("Finished System Maintenance");
}


void MainObject::RunLocalMaintenance()
{
  PurgeDropboxes();
}


void MainObject::PurgeCuts()
{
  PrintMessage("Starting PurgeCuts()");

  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select ")+
    "NAME,"+                // 00
    "CUT_SHELFLIFE,"+       // 01
    "DELETE_EMPTY_CARTS "+  // 02
    "from GROUPS where "+
    "CUT_SHELFLIFE>=0";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select ")+
      "CART.NUMBER,"+    // 00
      "CUTS.CUT_NAME "+  // 01
      "from CUTS left join CART "+
      "on CUTS.CART_NUMBER=CART.NUMBER where "+
      "(CART.GROUP_NAME=\""+RDEscapeString(q->value(0).toString())+"\")&&"+
      "(CUTS.END_DATETIME<\""+
      RDEscapeString(dt.addDays(-q->value(1).toInt()).toString("yyyy-MM-dd"))+
      " 00:00:00\")";
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      RDCart *cart=new RDCart(q1->value(0).toUInt());
      if(cart->removeCut(rda->station(),rda->user(),q1->value(1).toString(),
			 rda->config())) {
	rda->syslog(LOG_INFO,"purged cut %s",
		    (const char *)q1->value(1).toString().toUtf8());
      }
      else {
	rda->syslog(LOG_WARNING,"unable to purge cut %s: audio deletion error",
		    (const char *)q1->value(1).toString().toUtf8());
      }
      if(q->value(2).toString()=="Y") {  // Delete Empty Cart
	sql=QString("select CUT_NAME from CUTS where ")+
	  QString().sprintf("CART_NUMBER=%u",q1->value(0).toUInt());
	q2=new RDSqlQuery(sql);
	if(!q2->first()) {
	  cart->remove(rda->station(),rda->user(),rda->config());
	  rda->syslog(LOG_INFO,"deleted purged cart %06u",cart->number());
	}
	delete q2;
      }
      delete cart;
    }
    delete q1;
  }
  delete q;

  PrintMessage("Completed PurgeCuts()");
}


void MainObject::PurgeLogs()
{
  PrintMessage("Starting PurgeLogs()");

  QString sql;
  RDSqlQuery *q;
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());

  sql=QString("select NAME from LOGS where ")+
    "(PURGE_DATE!=\"0000-00-00\")&&"+
    "(PURGE_DATE is not null)&&"+
    "(PURGE_DATE<\""+dt.date().toString("yyyy-MM-dd")+"\")";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    rda->syslog(LOG_INFO,"purged log %s",
		(const char *)q->value(0).toString().toUtf8());
    RDLog *log=new RDLog(q->value(0).toString());
    log->remove(rda->station(),rda->user(),rda->config());
    delete log;
  }
  delete q;

  PrintMessage("Completed PurgeLogs()");
}


void MainObject::PurgeElr()
{
  PrintMessage("Starting PurgeElr()");

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

  PrintMessage("Completed PurgeElr()");
}


void MainObject::PurgeDropboxes()
{
  PrintMessage("Starting PurgeDropboxes()");

  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("select ")+
    "DROPBOX_PATHS.FILE_PATH,"+  // 00
    "DROPBOX_PATHS.ID "+         // 01
    "from DROPBOXES left join DROPBOX_PATHS "+
    "on (DROPBOXES.ID=DROPBOX_PATHS.DROPBOX_ID) where "+
    "DROPBOXES.STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+
    "\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(!QFile::exists(q->value(0).toString())) {
      sql=QString("delete from DROPBOX_PATHS where ")+
	QString().sprintf("ID=%d",q->value(1).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
  }
  delete q;

  PrintMessage("Completed PurgeDropboxes()");
}


void MainObject::PurgeGpioEvents()
{
  PrintMessage("Starting PurgeGpioEvents()");

  QString sql;

  sql=QString("delete from GPIO_EVENTS where ")+
    "EVENT_DATETIME<\""+
    QDate::currentDate().addDays(-RD_GPIO_EVENT_DAYS).toString("yyyy-MM-dd")+" 00:00:00\"";
  printf("SQL: %s\n",sql.toUtf8().constData());
  RDSqlQuery::apply(sql);

  PrintMessage("Starting Completed GpioEvents()");
}


void MainObject::PurgeWebapiAuths()
{
  PrintMessage("Starting PurgeWebapiAuths()");

  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from WEBAPI_AUTHS where EXPIRATION_DATETIME<now()");
  q=new RDSqlQuery(sql);
  delete q;

  PrintMessage("Completed PurgeWebapiAuths()");
}


void MainObject::PurgeStacks()
{
  PrintMessage("Starting PurgeStacks()");

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
    sql=QString("select MAX(SCHED_STACK_ID) from STACK_LINES where ")+
      "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
    q1=new RDSqlQuery(sql);
    if (q1->next()) {
      stackid=q1->value(0).toUInt();
      if (stackid-stacksize > 0) {
        sql=QString("select ID from STACK_LINES where ")+
	  "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	  QString().sprintf("SCHED_STACK_ID<=%d",stackid-stacksize);
	q2=new RDSqlQuery(sql);
	while(q2->next()) {
	  sql=QString("delete from STACK_SCHED_CODES where ")+
	    QString().sprintf("STACK_LINES_ID=%u",q2->value(0).toUInt());
	  RDSqlQuery::apply(sql);
	}
	delete q2;
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

  PrintMessage("Completed PurgeStacks()");
}


void MainObject::RehashCuts()
{
  PrintMessage("Starting RehashCuts()");

  QString sql;
  RDSqlQuery *q;
  RDRehash::ErrorCode err;

  sql="select CUT_NAME from CUTS where SHA1_HASH is null limit 100";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if((err=RDRehash::rehash(rda->station(),rda->user(),rda->config(),
			     RDCut::cartNumber(q->value(0).toString()),
			     RDCut::cutNumber(q->value(0).toString())))!=RDRehash::ErrorOk) {
      rda->syslog(LOG_WARNING,"failed to rehash cut %s [%s]",
		  (const char *)q->value(0).toString().toUtf8(),
		  (const char *)RDRehash::errorText(err).toUtf8());
      
    }
    else {
      if(maint_verbose) {
	fprintf(stderr,"rehashed cut \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    sleep(1);
  }
  delete q;

  PrintMessage("Completed RehashCuts()");
}


 void MainObject::PrintMessage(const QString &msg) const
 {
   if(maint_verbose) {
     printf("%s: %s\n",
	    QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").
	    toUtf8().constData(),
	    msg.toUtf8().constData());
   }
 }


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
