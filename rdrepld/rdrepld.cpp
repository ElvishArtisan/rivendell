// rdcatchd.cpp
//
// The Rivendell Replicator Daemon
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdrepld.cpp,v 1.4 2011/06/21 22:20:44 cvs Exp $
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

#include <qapplication.h>

#include <rddb.h>
#include <rdconf.h>
#include <rdcmd_switch.h>
#include <rdescape_string.h>
#include <rdcart.h>
#include <dbversion.h>

#include <globals.h>
#include <citadelxds.h>
#include <rdrepld.h>


RDConfig *rdconfig;
RDSystem *rdsystem;

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


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  bool skip_db_check=false;
  unsigned schema=0;

  //
  // Load the config
  //
  rdconfig=new RDConfig();
  rdconfig->load();

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdrepld",RDREPLD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }

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
  // Open Database
  //
  QString err (tr("ERROR rdrepld aborting - "));

  repl_db=RDInitDb(&schema,&err);
  if(!repl_db) {
    printf(err.ascii());
    exit(1);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,"rdrepld: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }
  connect (RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
	   this,SLOT(log(RDConfig::LogPriority,const QString &)));

  //
  // System Configuration
  //
  rdsystem=new RDSystem();

  //
  // Station Configuration
  //
  repl_station=new RDStation(rdconfig->stationName());

  //
  // Detach
  //
  if(qApp->argc()==1) {
    RDDetach(rdconfig->logCoreDumpDirectory());
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
  repl_loop_timer=new QTimer(this,"repl_loop_timer");
  connect(repl_loop_timer,SIGNAL(timeout()),this,SLOT(mainLoop()));
  repl_loop_timer->start(RD_RDREPL_SCAN_INTERVAL,true);

  rdconfig->log("rdrepld",RDConfig::LogNotice,"started");
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
  rdconfig->log("rdrepld",prio,msg);
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
    sql=QString().
      sprintf("select GROUP_NAME from REPLICATOR_MAP \
                           where REPLICATOR_NAME=\"%s\"",
	      (const char *)RDEscapeString(repl_name));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      where+=QString().
	sprintf("(GROUP_NAME=\"%s\")||",
		(const char *)RDEscapeString(q->value(0).toString()));
    }
    delete q;
    where=where.left(where.length()-2);
    sql=QString().sprintf("select NUMBER,TYPE,METADATA_DATETIME \
                           from CART where %s",
			  (const char *)where);
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("select ID,ITEM_DATETIME from REPL_CART_STATE \
                             where (REPLICATOR_NAME=\"%s\")&&\
                             (CART_NUMBER=%u)",
			    (const char *)RDEscapeString(repl_name),
			    q->value(0).toUInt());
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
	    sql=QString().sprintf("update REPL_CART_STATE set \
                                   ITEM_DATETIME=now() where ID=%u",
				  q1->value(0).toUInt());
	  }
	  else {
	    sql=QString().sprintf("insert into REPL_CART_STATE set \
                                   REPLICATOR_NAME=\"%s\",\
                                   CART_NUMBER=%u,\
                                   ITEM_DATETIME=now()",
				  (const char *)RDEscapeString(repl_name),
				  q->value(0).toUInt());
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

  sql=QString().
    sprintf("select NAME,TYPE_ID,FORMAT,CHANNELS,SAMPRATE,\
             BITRATE,QUALITY,URL,URL_USERNAME,URL_PASSWORD,\
             ENABLE_METADATA,NORMALIZATION_LEVEL from \
             REPLICATORS where STATION_NAME=\"%s\"",
	    (const char *)RDEscapeString(rdconfig->stationName()));
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
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
