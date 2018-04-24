// rdcatchd.cpp
//
// The Rivendell Replicator Daemon
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <QCoreApplication>

#include <rdapplication.h>
#include <rddb.h>
#include <rdconf.h>
#include <rdcmd_switch.h>
#include <rdescape_string.h>
#include <rdcart.h>
#include <dbversion.h>

#include <citadelxds.h>
#include <rdrepld.h>

void SigHandler(int signum)
{
  pid_t local_pid;

  switch(signum) {
      case SIGINT:
      case SIGTERM:
	RDDeletePid(RD_PID_DIR,"rdrepld.pid");
	exit(0);
	break;

      case SIGCHLD:
	local_pid=waitpid(-1,NULL,WNOHANG);
	while(local_pid>0) {
	  local_pid=waitpid(-1,NULL,WNOHANG);
	}
	signal(SIGCHLD,SigHandler);
	return;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  new RDApplication(RDApplication::Console,"rdrepld",RDREPLD_USAGE);

  //
  // Make sure we're the only instance running
  //
  if(RDCheckDaemon(RD_RDREPLD_PID)) {
    printf("rdrepld: aborting - multiple instances not allowed");
    exit(1);
  }

  //
  // Initialize Data Structures
  //
  debug=false;

  //
  // Calculate Temporary Directory
  //
  repl_temp_dir=RDTempDir();

  //
  // Detach
  //
  if(qApp->argc()==1) {
    RDDetach(rda->config()->logCoreDumpDirectory());
  }
  else {
    debug=true;
  }

  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGCHLD,SigHandler);
  if(!RDWritePid(RD_PID_DIR,"rdrepld.pid")) {
    printf("rdrepld: aborting - can't write pid file\n");
    exit(1);
  }

  //
  // Start the Main Loop
  //
  repl_loop_timer=new QTimer(this);
  connect(repl_loop_timer,SIGNAL(timeout()),this,SLOT(mainLoop()));
  repl_loop_timer->start(RD_RDREPL_SCAN_INTERVAL,true);

  rda->config()->log("rdrepld",RDConfig::LogNotice,"started");
}


void MainObject::mainLoop()
{
  LoadReplicators();
  ProcessCarts();
  FreeReplicators();
  repl_loop_timer->start(RD_RDREPL_SCAN_INTERVAL,true);
}


void MainObject::log(RDConfig::LogPriority prio,const QString &msg)
{
  rda->config()->log("rdrepld",prio,msg);
}


void MainObject::ProcessCarts()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  QString repl_name;
  QString where;
  bool stale;

  for(unsigned i=0;i<repl_replicators.size();i++) {
    where="";
    repl_name=repl_replicators[i]->config()->name();
    sql=QString("select GROUP_NAME from REPLICATOR_MAP where ")+
      "REPLICATOR_NAME=\""+RDEscapeString(repl_name)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      where+=QString("(GROUP_NAME=\"")+RDEscapeString(q->value(0).toString())+
	"\")||";
    }
    delete q;
    where=where.left(where.length()-2);
    sql=QString("select ")+
      "NUMBER,"+
      "TYPE,"+
      "METADATA_DATETIME "+
      "from CART where "+where;
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("select ")+
	"ID,"+
	"ITEM_DATETIME "+
	"from REPL_CART_STATE where "+
	"(REPLICATOR_NAME=\""+RDEscapeString(repl_name)+"\")&&"+
	QString().sprintf("(CART_NUMBER=%u)",q->value(0).toUInt());
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	stale=q->value(2).toDateTime()>q1->value(1).toDateTime();
      }
      else {
	stale=true;
      }
      if(stale) {
	if(repl_replicators[i]->processCart(q->value(0).toUInt())) {
	  if(q1->isValid()) {
	    sql=QString("update REPL_CART_STATE set ")+
	      "ITEM_DATETIME=now() where "+
	      QString().sprintf("ID=%u",q1->value(0).toUInt());
	  }
	  else {
	    sql=QString("insert into REPL_CART_STATE set ")+
	      "REPLICATOR_NAME=\""+RDEscapeString(repl_name)+"\","+
	      QString().sprintf("CART_NUMBER=%u,",q->value(0).toUInt())+
	      "ITEM_DATETIME=now()";
	  }
	  q2=new RDSqlQuery(sql);
	  delete q2;
	}
      }
      delete q1;
    }
    delete q;
  }
}


void MainObject::LoadReplicators()
{
  QString sql;
  RDSqlQuery *q;
  ReplConfig *config;

  sql=QString("select ")+
    "NAME,"+                 // 00
    "TYPE_ID,"+              // 01
    "FORMAT,"+               // 02
    "CHANNELS,"+             // 03
    "SAMPRATE,"+             // 04
    "BITRATE,"+              // 05
    "QUALITY,"+              // 06
    "URL,"+                  // 07
    "URL_USERNAME,"+         // 08
    "URL_PASSWORD,"+         // 09
    "ENABLE_METADATA,"+      // 10 
    "NORMALIZATION_LEVEL "+  // 11
    "from REPLICATORS where "+
    "STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    config=new ReplConfig();
    config->setName(q->value(0).toString());
    config->setType((RDReplicator::Type)q->value(1).toUInt());
    config->setFormat((RDSettings::Format)q->value(2).toUInt());
    config->setChannels(q->value(3).toUInt());
    config->setSampleRate(q->value(4).toUInt());
    config->setBitRate(q->value(5).toUInt());
    config->setQuality(q->value(6).toUInt());
    config->setUrl(q->value(7).toString());
    config->setUrlUsername(q->value(8).toString());
    config->setUrlPassword(q->value(9).toString());
    config->setEnableMetadata(RDBool(q->value(10).toString()));
    config->setNormalizeLevel(q->value(11).toInt());
    switch(config->type()) {
    case RDReplicator::TypeCitadelXds:
      repl_replicators.push_back(new CitadelXds(config));
      break;

    case RDReplicator::TypeLast:
      break;
    }
    repl_replicators.back()->startProcess();
  }
  delete q;
}


void MainObject::FreeReplicators()
{
  for(unsigned i=0;i<repl_replicators.size();i++) {
    delete repl_replicators[i];
  }
  repl_replicators.clear();
}

int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
