// ripcd.cpp
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2007,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ripcd.cpp,v 1.77.4.3 2013/11/17 04:27:06 cvs Exp $
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


#include <qapplication.h>
#include <qobject.h>
#include <qtimer.h>
#include <qdir.h>
#include <qsessionmanager.h>
#include <qsignalmapper.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <rd.h>
#include <rdconf.h>
#include <rdstation.h>
#include <rdcheck_daemons.h>
#include <rdcmd_switch.h>
#include <rddb.h>
#include <dbversion.h>

#include <globals.h>
#include <ripcd_socket.h>
#include <ripcd.h>

//
// Global Objects
//
RDConfig *ripcd_config;
RDCae *rdcae;
RDStation *rdstation;


void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
  case SIGCHLD:
    pLocalPid=waitpid(-1,NULL,WNOHANG);
    while(pLocalPid>0) {
            pLocalPid=waitpid(-1,NULL,WNOHANG);
    }
    ::signal(SIGCHLD,SigHandler);
    ::signal(SIGTERM,SigHandler);
    ::signal(SIGINT,SigHandler);
    return;

  case SIGTERM:
    LogLine(RDConfig::LogInfo,"ripcd exiting normally");
    RDDeletePid(RD_PID_DIR,"ripcd.pid");
    exit(0);
    break;

  case SIGINT:
    LogLine(RDConfig::LogInfo,"ripcd exiting on SIGINT");
    RDDeletePid(RD_PID_DIR,"ripcd.pid");
    exit(0);
    break;
  }
}


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  bool skip_db_check=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"ripcd",RIPCD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }
  delete cmd;

  //
  // Load Local Configs
  //
  ripcd_config=new RDConfig(RD_CONF_FILE);
  ripcd_config->load();

  //
  // Make sure we're the only instance running
  //
  if(RDCheckDaemon(RD_RIPCD_PID)) {
    LogLine(RDConfig::LogErr,
	    "ERROR ripcd aborting - multiple instances not allowed");
    exit(1);
  }

  //
  // Initialize Data Structures
  //
  debug=false;
  for(int i=0;i<MAX_MATRICES;i++) {
    ripcd_switcher[i]=NULL;
    for(int j=0;j<MAX_GPIO_PINS;j++) {
      ripcd_gpi_state[i][j]=false;
      ripcd_gpo_state[i][j]=false;
    }
  }
  ripc_onair_flag=false;

  server=new RipcdSocket(RIPCD_TCP_PORT,0,this,"socket");
  if(!server->ok()) {
    exit(1);
  }
  connect(server,SIGNAL(connection(int)),this,SLOT(newConnection(int)));

  //
  // Macro Timers
  //
  QSignalMapper *mapper=new QSignalMapper(this,"macro_timer_mapper");
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(macroTimerData(int)));
  for(int i=0;i<RD_MAX_MACRO_TIMERS;i++) {
    ripc_macro_cart[i]=0;
    ripc_macro_timer[i]=new QTimer(this);
    mapper->setMapping(ripc_macro_timer[i],i);
    connect(ripc_macro_timer[i],SIGNAL(timeout()),mapper,SLOT(map()));
  }

  //
  // Open Database
  //
  unsigned schema=0;
  QString err (tr("ripcd: "));
  ripcd_db = RDInitDb (&schema,&err);
  if(!ripcd_db) {
    printf ("%s\n",err.ascii());
    exit (1);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,"database version mismatch, should be %u, is %u",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }
  connect (RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
	   this,SLOT(log(RDConfig::LogPriority,const QString &)));

  //
  // Station 
  //
  rdstation=new RDStation(ripcd_config->stationName());
  rdstation->setUserName(rdstation->defaultName());
  ripcd_host_addr=rdstation->address();

  //
  // CAE Connection
  //
  rdcae=new RDCae(rdstation,ripcd_config,parent,name);
  rdcae->connectHost();

  if(qApp->argc()==1) {
    RDDetach(ripcd_config->logCoreDumpDirectory());
  }
  else {
    debug=true;
  }
  ::signal(SIGCHLD,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGINT,SigHandler);
  if(!RDWritePid(RD_PID_DIR,"ripcd.pid",ripcd_config->uid())) {
    printf("ripcd: can't write pid file\n");
    exit(1);
  }

  //
  // The RML Sockets
  //
  ripcd_rml_send=new QSocketDevice(QSocketDevice::Datagram);

  ripcd_rml_echo=new QSocketDevice(QSocketDevice::Datagram);
  ripcd_rml_echo->bind(QHostAddress(),RD_RML_ECHO_PORT);
  ripcd_rml_echo->setBlocking(false);

  ripcd_rml_noecho=new QSocketDevice(QSocketDevice::Datagram);
  ripcd_rml_noecho->bind(QHostAddress(),RD_RML_NOECHO_PORT);
  ripcd_rml_noecho->setBlocking(false);

  ripcd_rml_reply=new QSocketDevice(QSocketDevice::Datagram);
  ripcd_rml_reply->bind(QHostAddress(),RD_RML_REPLY_PORT);
  ripcd_rml_reply->setBlocking(false);

  LoadGpiTable();

  //
  // Initialize local RMLs
  //
  LoadLocalMacros();

  //
  // Start RML Polling
  //
  QTimer *timer=new QTimer(this,"timer");
  timer->changeInterval(RIPCD_RML_READ_INTERVAL);
  connect(timer,SIGNAL(timeout()),this,SLOT(readRml()));
  timer->start(true);

  //
  // Database Backup Timer
  //
  databaseBackup();
  ripcd_backup_timer=new QTimer(this,"ripcd_backup_timer");
  connect(ripcd_backup_timer,SIGNAL(timeout()),this,SLOT(databaseBackup()));
  ripcd_backup_timer->start(86400000);

  //
  // Maintenance Routine Timer
  //
  srandom(QTime::currentTime().msec());
  ripcd_maint_timer=new QTimer(this,"ripcd_maint_timer");
  connect(ripcd_maint_timer,SIGNAL(timeout()),this,SLOT(checkMaintData()));
  int interval=GetMaintInterval();
  if(!ripcd_config->disableMaintChecks()) {
    ripcd_maint_timer->start(interval);
  }
  else {
    log(RDConfig::LogInfo,"maintenance checks disabled on this host!");
  }

  LogLine(RDConfig::LogInfo,"started");
}


MainObject::~MainObject()
{
  delete server;
  delete ripcd_db;
}

void MainObject::log(RDConfig::LogPriority prio,const QString &msg)
{
  LogLine(prio,msg);
}


void MainObject::newConnection(int fd)
{
  unsigned i=0;

  while((i<ripcd_conns.size())&&(ripcd_conns[i]!=NULL)) {
    i++;
  }
  if(i==ripcd_conns.size()) {      // Table full, create a new slot
    ripcd_conns.push_back(new RipcdConnection(i,fd));
  }
  else {
    ripcd_conns[i]=new RipcdConnection(i,fd);
  }
  connect(ripcd_conns[i]->socket,SIGNAL(readyReadID(int)),
	  this,SLOT(socketData(int)));
  connect(ripcd_conns[i]->socket,SIGNAL(connectionClosedID(int)),
	  this,SLOT(socketKill(int)));
}


void MainObject::sendRml(RDMacro *rml)
{
  char buf[RD_RML_MAX_LENGTH];
  int n;

  if((n=rml->generateString(buf,RD_RML_MAX_LENGTH))<0) {
    return;
  }
  buf[n]=0;
  switch(rml->role()) {
      case RDMacro::Cmd:
	ripcd_rml_send->writeBlock(buf,n,rml->address(),rml->port());
	break;

      case RDMacro::Reply:
	if(!(ripcd_host_addr==rml->address())) {
	  ripcd_rml_send->writeBlock(buf,n,rml->address(),RD_RML_REPLY_PORT);
	}
	break;

      default:
	break;
  }
}


void MainObject::readRml()
{
  ReadRmlSocket(ripcd_rml_echo,RDMacro::Cmd,true);
  ReadRmlSocket(ripcd_rml_noecho,RDMacro::Cmd,false);
  ReadRmlSocket(ripcd_rml_reply,RDMacro::Reply,false);
}


void MainObject::databaseBackup()
{
  QString cmd;
  QDateTime datetime=QDateTime::currentDateTime();
  int life;

  if((life=rdstation->backupLife())<=0) {
    return;
  }
  if(fork()==0) {
    cmd=QString().sprintf("find %s -name *.sql -ctime +%d -exec rm \\{\\} \\;",
			  (const char *)rdstation->backupPath(),
			  rdstation->backupLife());
    system((const char *)cmd);
    cmd=QString().
	sprintf("mysqldump -c Rivendell -h %s -u %s -p%s > %s/%s.sql",
		(const char *)ripcd_config->mysqlHostname(),
		(const char *)ripcd_config->mysqlUsername(),
		(const char *)ripcd_config->mysqlPassword(),
		(const char *)rdstation->backupPath(),
		(const char *)datetime.date().toString("yyyyMMdd"));
    system((const char *)cmd);
    exit(0);
  }
}


void MainObject::socketData(int ch)
{
  ParseCommand(ch);
}


void MainObject::socketKill(int ch)
{
  KillSocket(ch);
}


void MainObject::checkMaintData()
{
  QString sql;
  RDSqlQuery *q;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  bool run=false;

  RunLocalMaintRoutine();

  //
  // Should we try to run system maintenance?
  //
  if(!rdstation->systemMaint()) {
    return;
  }

  //
  // Get the system-wide maintenance timestamp
  //
  sql="lock tables VERSION write";
  q=new RDSqlQuery(sql);
  delete q;
  sql="select LAST_MAINT_DATETIME from VERSION";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    run=1000*q->value(0).toDateTime().secsTo(current_datetime)>
      RD_MAINT_MAX_INTERVAL;
  }
  delete q;
  sql="unlock tables";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Run the routines
  //
  if(run) {
    RunSystemMaintRoutine();
  }
}


void MainObject::macroTimerData(int num)
{
  ExecCart(ripc_macro_cart[num]);
  ripc_macro_cart[num]=0;
}


void MainObject::SetUser(QString username)
{
    rdstation->setUserName(username);
    BroadcastCommand(QString().sprintf("RU %s!",(const char *)username));
}


void MainObject::ParseCommand(int ch)
{
  char buf[256];
  int c;
  RipcdConnection *conn=ripcd_conns[ch];

  while((c=conn->socket->readBlock(buf,256))>0) {
    buf[c]=0;
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(conn->argnum<RD_RML_MAX_ARGS) {
	  conn->args[conn->argnum][conn->argptr]=0;
	  conn->argnum++;
	  conn->argptr=0;
	}
	else {
	  LogLine(RDConfig::LogWarning,QString().
		  sprintf("*** ParseCommand1: argument list truncated.  LocalBuffer: %s ***",buf));
	}
      }
      if(buf[i]=='!') {
	conn->args[conn->argnum++][conn->argptr]=0;
	DispatchCommand(ch);
	conn->argnum=0;
	conn->argptr=0;
	if(conn->socket==NULL) {
	  return;
	}
      }
      if((isgraph(buf[i]))&&(buf[i]!='!')) {
	if(conn->argptr<RIPCD_MAX_LENGTH) {
	  conn->args[conn->argnum][conn->argptr]=buf[i];
	  conn->argptr++;
	}
	else {
	  if(debug) {
	    LogLine(RDConfig::LogWarning,QString().
		    sprintf("*** ParseCommand2: argument list truncated.  LocalBuffer: %s ***",buf));
	  }
	}
      }
    }
  }
}


void MainObject::DispatchCommand(int ch)
{
  QString default_name;
  char str[RD_RML_MAX_LENGTH];
  RDMacro macro;
  char buffer[RD_RML_MAX_LENGTH];
  char cmd[RD_RML_MAX_LENGTH+4];
  int echo=0;
  QHostAddress addr;
  RipcdConnection *conn=ripcd_conns[ch];
  unsigned port;

  //
  // Common Commands
  // Authentication not required to execute these!
  //
  if(!strcmp(conn->args[0],"DC")) {  // Drop Connection
    conn->socket->close();
    KillSocket(ch);
    return;
  }
  if(!strcmp(conn->args[0],"PW")) {  // Password Authenticate
    if(!strcmp(conn->args[1],ripcd_config->password())) {
      conn->auth=true;
      EchoCommand(ch,"PW +!");
      return;
    }
    else {
      conn->auth=false;
      EchoCommand(ch,"PW -!");
      return;
    }
  }

  //
  // Priviledged Commands
  // Authentication required to execute these!
  //
  if(!conn->auth) {
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(conn->args[0],"RU")) {  // Request User
    EchoCommand(ch,(const char *)QString().
		sprintf("RU %s!",(const char *)rdstation->userName()));
    return;
  }

  if(!strcmp(conn->args[0],"SU")) {  // Set User
    SetUser(conn->args[1]);
  }

  if(!strcmp(conn->args[0],"MS")) {  // Send RML Command
    if(conn->argnum<4) {
      return;
    }
    strcpy(str,conn->args[3]);
    for(int i=4;i<conn->argnum;i++) {
      strcat(str," ");
      strcat(str,conn->args[i]);
    }
    strcat(str,"!");
  }
  if(macro.parseString(str,strlen(str))) {
    addr.setAddress(conn->args[1]);
    macro.setAddress(addr);
    sscanf(conn->args[2],"%u",&port);
    macro.setPort(port);
    macro.setRole(RDMacro::Cmd);

/*
  char temp[RD_RML_MAX_LENGTH];
  macro.generateString(temp,RD_RML_MAX_LENGTH);
  LogLine(QString().sprintf("RECEIVED: %s  ADDR: %s\n",temp,(const char *)macro.address().toString()));
*/

    if(!macro.address().isNull()) {
      if(macro.address()==rdstation->address()&&
	 ((macro.port()==RD_RML_ECHO_PORT)||
	  (macro.port()==RD_RML_NOECHO_PORT))) {  // Local Loopback
	macro.generateString(buffer,RD_RML_MAX_LENGTH);
	if(macro.echoRequested()) {
	  echo=1;
	}
	sprintf(cmd,"MS %s %d %s",(const char *)macro.address().toString(),
		echo,buffer);
	RunLocalMacros(&macro);
	BroadcastCommand(cmd);
      }
      else {
	sendRml(&macro);
      }
    }
  }

  if(!strcmp(conn->args[0],"ME")) {  // Send RML Reply
    if(conn->argnum<4) {
      return;
    }
    strcpy(str,conn->args[3]);
    for(int i=4;i<conn->argnum;i++) {
      strcat(str," ");
      strcat(str,conn->args[i]);
    }
    strcat(str,"!");
  }
  if(macro.parseString(str,strlen(str))) {
    QHostAddress addr;
    addr.setAddress(conn->args[1]);
    macro.setAddress(addr);
    macro.setRole(RDMacro::Reply); 
    if(macro.address()==rdstation->address()) {  // Local Loopback
      macro.generateString(buffer,RD_RML_MAX_LENGTH);
      sprintf(cmd,"ME %s 0 %s",(const char *)macro.address().toString(),
	      buffer);
      BroadcastCommand(cmd);
    }
    else {
      sendRml(&macro);
    }
  }

  if(!strcmp(conn->args[0],"RG")) {  // Reload the GPI Table
    LoadGpiTable();
  }

  if(!strcmp(conn->args[0],"GI")) {  // Send Complete GPI Status
    int matrix;
    sscanf(conn->args[1],"%d",&matrix);
    SendGpi(ch,matrix);
  }

  if(!strcmp(conn->args[0],"GO")) {  // Send Complete GPO Status
    int matrix;
    sscanf(conn->args[1],"%d",&matrix);
    SendGpo(ch,matrix);
  }

  if(!strcmp(conn->args[0],"GM")) {  // Send Complete GPI Mask States
    int matrix;
    sscanf(conn->args[1],"%d",&matrix);
    SendGpiMask(ch,matrix);
  }

  if(!strcmp(conn->args[0],"GN")) {  // Send Complete GPI Mask States
    int matrix;
    sscanf(conn->args[1],"%d",&matrix);
    SendGpoMask(ch,matrix);
  }

  if(!strcmp(conn->args[0],"GC")) {  // Send Complete GPI Cart Assignments
    int matrix;
    sscanf(conn->args[1],"%d",&matrix);
    SendGpiCart(ch,matrix);
  }

  if(!strcmp(conn->args[0],"GD")) {  // Send Complete GPO Cart Assignments
    int matrix;
    sscanf(conn->args[1],"%d",&matrix);
    SendGpoCart(ch,matrix);
  }

  if(!strcmp(conn->args[0],"TA")) {  // Send Onair Flag State
    EchoCommand(ch,QString().sprintf("TA %d!",ripc_onair_flag));
  }
}


void MainObject::KillSocket(int ch)
{
  delete ripcd_conns[ch];
  ripcd_conns[ch]=NULL;
}


void MainObject::EchoCommand(int ch,const char *command)
{
  if(ripcd_conns[ch]->socket->state()==QSocket::Connection) {
    ripcd_conns[ch]->socket->writeBlock(command,strlen(command));
  }
}


void MainObject::BroadcastCommand(const char *command)
{
  for(unsigned i=0;i<ripcd_conns.size();i++) {
    if(ripcd_conns[i]!=NULL) {
      EchoCommand(i,command);
    }
  }
}


void MainObject::EchoArgs(int ch,const char append)
{
  char command[RIPCD_MAX_LENGTH+2];
  int l;

  command[0]=0;
  for(int i=0;i<ripcd_conns[ch]->argnum;i++) {
    strcat(command,ripcd_conns[ch]->args[i]);
    strcat(command," ");
  }
  l=strlen(command);
  command[l]=append;
  command[l+1]='!';
  command[l+2]=0;
  EchoCommand(ch,command);
}


void MainObject::ReadRmlSocket(QSocketDevice *dev,RDMacro::Role role,
			       bool echo)
{
  char buffer[RD_RML_MAX_LENGTH];
  char cmd[RD_RML_MAX_LENGTH+4];
  QString output;
  int n;
  QHostAddress peer_addr;
  RDMacro macro;

  while((n=dev->readBlock(buffer,RD_RML_MAX_LENGTH))>0) {
    buffer[n]=0;
    if(macro.parseString(buffer,n)) {
      macro.setRole(role);
      macro.setAddress(dev->peerAddress());
      macro.setEchoRequested(echo);
      macro.generateString(buffer,RD_RML_MAX_LENGTH);
      switch(role) {
	  case RDMacro::Cmd:
	    sprintf(cmd,"MS %s %d %s",(const char *)macro.address().toString(),
		    echo,buffer);
	    RunLocalMacros(&macro);
	    BroadcastCommand(cmd);
	    break;

	  case RDMacro::Reply:
	    sprintf(cmd,"ME %s %d %s",(const char *)macro.address().toString(),
		    echo,buffer);
	    break;

	  default:
	    break;
      }
    }
    else {
      LogLine(RDConfig::LogWarning,
	      QString().sprintf("received malformed rml: \"%s\" from %s:%u",
				buffer,
				(const char *)dev->peerAddress().toString(),
				dev->peerPort()));
      if(echo) {
	macro.setRole(RDMacro::Reply);
	macro.setCommand(RDMacro::NN);
	macro.setArg(0,"-");
	macro.setArgQuantity(1);
	macro.setAddress(dev->peerAddress());
	sendRml(&macro);
      }
    }
  }
}


void MainObject::LoadGpiTable()
{
  for(int i=0;i<MAX_MATRICES;i++) {
    for(int j=0;j<MAX_GPIO_PINS;j++) {
      ripcd_gpi_mask[i][j]=true;
      ripcd_gpo_mask[i][j]=true;
      for(int k=0;k<2;k++) {
	ripcd_gpi_macro[i][j][k]=0;
	ripcd_gpo_macro[i][j][k]=0;
      }
    }
  }
  QString sql=QString().sprintf("select MATRIX,NUMBER,OFF_MACRO_CART,\
                                 MACRO_CART from GPIS \
                                 where STATION_NAME=\"%s\"",
				(const char *)ripcd_config->stationName());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    ripcd_gpi_macro[q->value(0).toInt()][q->value(1).toInt()-1][0]=
      q->value(2).toInt();
    ripcd_gpi_macro[q->value(0).toInt()][q->value(1).toInt()-1][1]=
      q->value(3).toInt();
  }
  delete q;

  sql=QString().sprintf("select MATRIX,NUMBER,OFF_MACRO_CART,MACRO_CART \
                         from GPOS where STATION_NAME=\"%s\"",
			(const char *)ripcd_config->stationName());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ripcd_gpo_macro[q->value(0).toInt()][q->value(1).toInt()-1][0]=
      q->value(2).toInt();
    ripcd_gpo_macro[q->value(0).toInt()][q->value(1).toInt()-1][1]=
      q->value(3).toInt();
  }
  delete q;
}


void MainObject::SendGpi(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpiQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GI %d %d %d %d!",
				     matrix,i,ripcd_gpi_state[matrix][i],
				     ripcd_gpi_mask[matrix][i]));
  }
}


void MainObject::SendGpo(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpoQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GO %d %d %d %d!",
				     matrix,i,ripcd_gpo_state[matrix][i],
				     ripcd_gpo_mask[matrix][i]));
  }
}


void MainObject::SendGpiMask(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpiQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GM %d %d %d!",
				     matrix,i,ripcd_gpi_mask[matrix][i]));
  }
}


void MainObject::SendGpoMask(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpoQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GN %d %d %d!",
				     matrix,i,ripcd_gpo_mask[matrix][i]));
  }
}


void MainObject::SendGpiCart(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpiQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GC %d %d %d %d!",
				     matrix,i,ripcd_gpi_macro[matrix][i][0],
				     ripcd_gpi_macro[matrix][i][1]));
  }
}


void MainObject::SendGpoCart(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpoQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GD %d %d %d %d!",
				     matrix,i,ripcd_gpo_macro[matrix][i][0],
				     ripcd_gpo_macro[matrix][i][1]));
  }
}


void LogLine(RDConfig::LogPriority prio,const QString &line)
{
  FILE *logfile;

  ripcd_config->log("ripcd",prio,line);

  if((!ripcd_config) || ripcd_config->ripcdLogname().isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();
  logfile=fopen(ripcd_config->ripcdLogname(),"a");
  if(logfile==NULL) {
    return;
  }
  chmod(ripcd_config->ripcdLogname(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(logfile,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line);
  fclose(logfile);
}

/* This is an overloaded virtual function to tell a session manager not to restart this daemon. */
void QApplication::saveState(QSessionManager &sm) {
  sm.setRestartHint(QSessionManager::RestartNever);
  LogLine(RDConfig::LogDebug,"ripcd saveState(), set restart hint to Never");
  return;
};


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
