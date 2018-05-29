// rdvairplayd.cpp
//
// Headless log player
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qapplication.h>

#include <rdapplication.h>
#include <rdcheck_daemons.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddbheartbeat.h>
#include <rdescape_string.h>
#include <rdmixer.h>
#include <rdweb.h>

#include "rdvairplayd.h"

bool global_exiting=false;

void SigHandler(int signo)
{
  switch(signo) {
  case SIGINT:
  case SIGTERM:
    global_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  //
  // Ensure that system daemons are running
  //
  RDInitializeDaemons();

  //
  // Startup DateTime
  //
  air_startup_datetime=QDateTime::currentDateTime();

  //
  // Open the Database
  //
  rda=new RDApplication("rdvairplayd","rdvairplayd",RDVAIRPLAYD_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdvairplayd: %s\n",(const char *)err_msg);
    exit(1);
  }
  air_previous_exit_code=rda->airplayConf()->virtualExitCode();
  rda->airplayConf()->setVirtualExitCode(RDAirPlayConf::ExitDirty);

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdvairplayd: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // CAE Connection
  //
  rda->cae()->connectHost();

  //
  // Set Audio Assignments
  //
  //  air_segue_length=rda->airplayConf()->segueLength()+1;
  RDSetMixerPorts(rda->config()->stationName(),rda->cae());

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));
  //  connect(rda->ripc(),SIGNAL(gpiStateChanged(int,int,bool)),
  //	  this,SLOT(gpiStateChangedData(int,int,bool)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Macro Player
  //
  air_event_player=new RDEventPlayer(rda->ripc(),this);

  //
  // UDP Transmission Socket
  //
  air_nownext_socket=new QSocketDevice(QSocketDevice::Datagram);

  //
  // Log Machines
  //
  QSignalMapper *reload_mapper=new QSignalMapper(this);
  connect(reload_mapper,SIGNAL(mapped(int)),this,SLOT(logReloadedData(int)));
  QSignalMapper *rename_mapper=new QSignalMapper(this);
  QString default_svcname=rda->airplayConf()->defaultSvc();
  for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    air_logs[i]=new RDLogPlay(i+RD_RDVAIRPLAY_LOG_BASE,air_event_player,
			      air_nownext_socket,"",&air_plugin_hosts);
    air_logs[i]->setDefaultServiceName(default_svcname);
    //
    // FIXME: Add the ability to specify default carts for vLogs!
    //
    air_logs[i]->setNowCart(rda->airplayConf()->logNowCart(i));
    air_logs[i]->setNextCart(rda->airplayConf()->logNextCart(i));
    reload_mapper->setMapping(air_logs[i],i);
    connect(air_logs[i],SIGNAL(reloaded()),reload_mapper,SLOT(map()));
    rename_mapper->setMapping(air_logs[i],i);
    connect(air_logs[i],SIGNAL(renamed()),rename_mapper,SLOT(map()));
    //    connect(air_logs[i],SIGNAL(refreshStatusChanged(bool)),
    //	    this,SLOT(refreshStatusChangedData(bool)));
    //    connect(air_logs[i],SIGNAL(channelStarted(int,int,int,int)),
    //	    this,SLOT(logChannelStartedData(int,int,int,int)));
    //    connect(air_logs[i],SIGNAL(channelStopped(int,int,int,int)),
    //	    this,SLOT(logChannelStoppedData(int,int,int,int)));
    int cards[2]={0,0};
    cards[0]=rda->airplayConf()->virtualCard(i+RD_RDVAIRPLAY_LOG_BASE);
    cards[1]=rda->airplayConf()->virtualCard(i+RD_RDVAIRPLAY_LOG_BASE);
    if(rda->station()->cardDriver(cards[0])==RDStation::None) {
      cards[0]=-1;
      cards[1]=-1;
    }
    int ports[2]={0,0};
    ports[0]=rda->airplayConf()->virtualPort(i+RD_RDVAIRPLAY_LOG_BASE);
    ports[1]=rda->airplayConf()->virtualPort(i+RD_RDVAIRPLAY_LOG_BASE);
    if((cards[0]<0)||(ports[0]>=rda->station()->cardOutputs(cards[0]))) {
      ports[0]=-1;
      ports[1]=-1;
    }
    QString start_rml[2]={"",""};
    start_rml[0]=rda->airplayConf()->virtualStartRml(i+RD_RDVAIRPLAY_LOG_BASE);
    start_rml[1]=rda->airplayConf()->virtualStartRml(i+RD_RDVAIRPLAY_LOG_BASE);
    QString stop_rml[2]={"",""};
    stop_rml[0]=rda->airplayConf()->virtualStopRml(i+RD_RDVAIRPLAY_LOG_BASE);
    stop_rml[1]=rda->airplayConf()->virtualStopRml(i+RD_RDVAIRPLAY_LOG_BASE);
    air_logs[i]->setChannels(cards,ports,start_rml,stop_rml);
    air_logs[i]->
      setOpMode(rda->airplayConf()->opMode(i+RD_RDVAIRPLAY_LOG_BASE));
  }
  //  connect(air_logs[0],SIGNAL(transportChanged()),
  //	  this,SLOT(transportChangedData()));
  
  //
  // Load Plugins
  //
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "PLUGIN_PATH,"+
    "PLUGIN_ARG "+
    "from NOWNEXT_PLUGINS where "+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
    "(LOG_MACHINE=0)";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    air_plugin_hosts.
      push_back(new RDRLMHost(q->value(0).toString(),q->value(1).toString(),
			      air_nownext_socket,this));
    rda->log(RDConfig::LogInfo,QString().
	     sprintf("Loading RLM \"%s\"",
		     (const char *)q->value(0).toString()));
    if(!air_plugin_hosts.back()->load()) {
      rda->log(RDConfig::LogWarning,QString().
	       sprintf("Failed to load RLM \"%s\"",
		       (const char *)q->value(0).toString()));
    }
  }
  delete q;

  //
  // Exit Timer
  //
  air_exit_timer=new QTimer(this);
  connect(air_exit_timer, SIGNAL(timeout()),this,SLOT(exitData()));
  air_exit_timer->start(1000);
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
}


void MainObject::ripcConnectedData(bool state)
{
  QHostAddress addr;
  QString sql;
  RDSqlQuery *q;
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  addr.setAddress("127.0.0.1");
  rml.setAddress(addr);
  rml.setEchoRequested(false);

  //
  // Get Onair Flag State
  //
  rda->ripc()->sendOnairFlag();

  //
  // Load Initial Logs
  //
  for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    int mach=i+RD_RDVAIRPLAY_LOG_BASE;
    switch(rda->airplayConf()->startMode(mach)) {
    case RDAirPlayConf::StartEmpty:
      break;
	    
    case RDAirPlayConf::StartPrevious:
      air_start_lognames[i]=RDDateTimeDecode(rda->airplayConf()->currentLog(mach),
			    air_startup_datetime,rda->station(),rda->config());
      if(!air_start_lognames[i].isEmpty()) {
	if(air_previous_exit_code==RDAirPlayConf::ExitDirty) {
	  if((air_start_lines[i]=rda->airplayConf()->logCurrentLine(mach))>=0) {
	    air_start_starts[i]=rda->airplayConf()->autoRestart(mach)&&
	      rda->airplayConf()->logRunning(mach);
	  }
	}
	else {
	  air_start_lines[i]=0;
	  air_start_starts[i]=false;
	}
      }
      break;

    case RDAirPlayConf::StartSpecified:
      air_start_lognames[i]=RDDateTimeDecode(rda->airplayConf()->logName(mach),
			       air_startup_datetime,rda->station(),
			       rda->config());
      if(!air_start_lognames[i].isEmpty()) {
	if(air_previous_exit_code==RDAirPlayConf::ExitDirty) {
	  if(air_start_lognames[i]==rda->airplayConf()->currentLog(mach)) {
	    if((air_start_lines[i]=rda->airplayConf()->logCurrentLine(mach))>=0) {
	      air_start_starts[i]=rda->airplayConf()->autoRestart(mach)&&
		rda->airplayConf()->logRunning(mach);
	    }
	    else {
	      air_start_lines[i]=0;
	      air_start_starts[i]=false;
	    }
	  }
	}
      }
      break;
    }
    if(!air_start_lognames[i].isEmpty()) {
      sql=QString("select NAME from LOGS where ")+
	"NAME=\""+RDEscapeString(air_start_lognames[i])+"\"";
      q=new RDSqlQuery(sql);
      if(q->first()) {
	rml.setCommand(RDMacro::LL);  // Load Log
	rml.setArgQuantity(2);
	rml.setArg(0,mach+1);
	rml.setArg(1,air_start_lognames[i]);
	rda->ripc()->sendRml(&rml);
      }
      else {
	rda->log(RDConfig::LogWarning,QString().sprintf("vlog %d: ",mach+1)+
		 "log \""+air_start_lognames[i]+"\" doesn't exist");
      }
      delete q;
    }
  }
}


void MainObject::userData()
{
  //  printf("User connected!\n");
}


void MainObject::logReloadedData(int log)
{
  QHostAddress addr;
  int mach=log+RD_RDVAIRPLAY_LOG_BASE;

  //
  // Load Initial Log
  //
  if(air_start_lognames[log].isEmpty()) {
    return;
  }
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  addr.setAddress("127.0.0.1");
  rml.setAddress(addr);
  rml.setEchoRequested(false);

  if(air_start_lines[log]<air_logs[log]->size()) {
    rml.setCommand(RDMacro::MN);  // Make Next
    rml.setArgQuantity(2);
    rml.setArg(0,mach+1);
    rml.setArg(1,air_start_lines[log]);
    rda->ripc()->sendRml(&rml);
    
    if(air_start_starts[log]) {
      rml.setCommand(RDMacro::PN);  // Start Next
      rml.setArgQuantity(1);
      rml.setArg(0,mach+1);
      rda->ripc()->sendRml(&rml);
    }
  }
  else {
    rda->log(RDConfig::LogWarning,QString().sprintf("vlog %d: ",mach+1)+
	     QString().sprintf("line %d doesn't exist ",air_start_lines[log])+
	     "in log \""+air_start_lognames[log]+"\"");
  }
  air_start_lognames[log]="";
}


void MainObject::exitData()
{
  if(global_exiting) {
    for(unsigned i=0;i<air_plugin_hosts.size();i++) {
      air_plugin_hosts[i]->unload();
    }
    for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
      delete air_logs[i];
    }
    rda->airplayConf()->setVirtualExitCode(RDAirPlayConf::ExitClean);
    rda->log(RDConfig::LogInfo,"exiting");
    exit(0);
  }
}


void MainObject::SetAutoMode(int index)
{
  air_logs[index]->setOpMode(RDAirPlayConf::Auto);
  rda->log(RDConfig::LogInfo,
	   QString().sprintf("log machine %d mode set to AUTOMATIC",
			     index+RD_RDVAIRPLAY_LOG_BASE+1));
}


void MainObject::SetLiveAssistMode(int index)
{
  air_logs[index]->setOpMode(RDAirPlayConf::LiveAssist);
  rda->log(RDConfig::LogInfo,
	   QString().sprintf("log machine %d mode set to LIVE ASSIST",
			     index+RD_RDVAIRPLAY_LOG_BASE+1));
}


void MainObject::SetManualMode(int index)
{
  air_logs[index]->setOpMode(RDAirPlayConf::Manual);
  rda->log(RDConfig::LogInfo,
	   QString().sprintf("log machine %d mode set to MANUAL",
			     index+RD_RDVAIRPLAY_LOG_BASE+1));
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
