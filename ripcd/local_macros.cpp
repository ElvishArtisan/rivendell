// local_macros.cpp
//
// Local RML Macros for the Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: local_macros.cpp,v 1.60.2.1 2013/06/20 20:00:09 cvs Exp $
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <rd.h>
#include <rdconf.h>
#include <rdmatrix.h>
#include <rdtty.h>
#include <rduser.h>
#include <rddb.h>
#include <ripcd.h>
#include <rdescape_string.h>

void MainObject::gpiChangedData(int matrix,int line,bool state)
{
  if(state) {
    LogLine(RDConfig::LogInfo,QString().sprintf("GPI %d:%d ON",matrix,line+1));
  }
  else {
    LogLine(RDConfig::LogInfo,QString().sprintf("GPI %d:%d OFF",matrix,line+1));
  }
  ripcd_gpi_state[matrix][line]=state;
  BroadcastCommand(QString().sprintf("GI %d %d %d %d!",matrix,line,state,
				     ripcd_gpi_mask[matrix][line]));
  if(!ripcd_gpi_mask[matrix][line]) {
    return;
  }
  if(ripcd_gpi_macro[matrix][line][state]>0) {
    ExecCart(ripcd_gpi_macro[matrix][line][state]);
  }
}


void MainObject::gpoChangedData(int matrix,int line,bool state)
{
  if(state) {
    LogLine(RDConfig::LogInfo,QString().sprintf("GPO %d:%d ON",matrix,line+1));
  }
  else {
    LogLine(RDConfig::LogInfo,QString().sprintf("GPO %d:%d OFF",matrix,line+1));
  }
  ripcd_gpo_state[matrix][line]=state;
  BroadcastCommand(QString().sprintf("GO %d %d %d %d!",matrix,line,state,
				     ripcd_gpo_mask[matrix][line]));
  if(!ripcd_gpo_mask[matrix][line]) {
    return;
  }
  if(ripcd_gpo_macro[matrix][line][state]>0) {
    ExecCart(ripcd_gpo_macro[matrix][line][state]);
  }
}


void MainObject::gpiStateData(int matrix,unsigned line,bool state)
{
  // LogLine(RDConfig::LogWarning,QString().sprintf("gpiStateData(%d,%d,%d)",matrix,line,state));

  BroadcastCommand(QString().sprintf("GI %d %u %d %d!",matrix,line,state,
				     ripcd_gpi_mask[matrix][line]));
}


void MainObject::gpoStateData(int matrix,unsigned line,bool state)
{
  // LogLine(RDConfig::LogWarning,QString().sprintf("gpoStateData(%d,%d,%d)",matrix,line,state));

  BroadcastCommand(QString().sprintf("GO %d %u %d %d!",matrix,line,state,
				     ripcd_gpo_mask[matrix][line]));
}


void MainObject::ttyTrapData(int cartnum)
{
  ExecCart(cartnum);
}


void MainObject::ttyScanData()
{
  char buf[256];
  int n;

  for(int i=0;i<MAX_TTYS;i++) {
    if(ripcd_tty_dev[i]!=NULL) {
      while((n=ripcd_tty_dev[i]->readBlock(buf,255))>0) {
	ripcd_tty_trap[i]->scan(buf,n);
      }
    }
  }
}


void MainObject::ExecCart(int cartnum)
{
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  rml.setCommand(RDMacro::EX);
  rml.setAddress(rdstation->address());
  rml.setEchoRequested(false);
  rml.setArgQuantity(1);
  rml.setArg(0,cartnum);
  sendRml(&rml);
}


void MainObject::LoadLocalMacros()
{
  QString sql;
  RDSqlQuery *q;
  unsigned tty_port;

  for(int i=0;i<MAX_MATRICES;i++) {
    ripcd_switcher_tty[i][0]=-1;
    ripcd_switcher_tty[i][1]=-1;
  }
  for(int i=0;i<MAX_TTYS;i++) {
    ripcd_tty_inuse[i]=false;
    ripcd_tty_dev[i]=NULL;
  }

  //
  // Initialize Matrices
  //
  sql=QString().sprintf("select MATRIX,TYPE,PORT,INPUTS,OUTPUTS from MATRICES \
                         where STATION_NAME=\"%s\"",
			(const char *)rdstation->name());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(!LoadSwitchDriver(q->value(0).toInt())) {
      LogLine(RDConfig::LogErr,QString().
	      sprintf("attempted to load unknown switcher driver for matrix %d",
		      q->value(0).toInt()));
    }
  }
  delete q;

  //
  // Initialize TTYs
  //
  sql=QString().sprintf("select PORT_ID,PORT,BAUD_RATE,DATA_BITS,PARITY,\
                         TERMINATION from TTYS where (STATION_NAME=\"%s\")&&\
                         (ACTIVE=\"Y\")",
			(const char *)rdstation->name());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    tty_port=q->value(0).toUInt();
    if(!ripcd_tty_inuse[tty_port]) {
      ripcd_tty_dev[tty_port]=new RDTTYDevice();
      ripcd_tty_dev[tty_port]->setName(q->value(1).toString());
      ripcd_tty_dev[tty_port]->setSpeed(q->value(2).toInt());
      ripcd_tty_dev[tty_port]->setWordLength(q->value(3).toInt());
      ripcd_tty_dev[tty_port]->
	setParity((RDTTYDevice::Parity)q->value(4).toInt());
      if(ripcd_tty_dev[tty_port]->open(IO_ReadWrite)) {
	ripcd_tty_term[tty_port]=(RDTty::Termination)q->value(5).toInt();
	ripcd_tty_inuse[tty_port]=true;
	ripcd_tty_trap[tty_port]=new RDCodeTrap(this);
	connect(ripcd_tty_trap[tty_port],SIGNAL(trapped(int)),
		this,SLOT(ttyTrapData(int)));
      }
      else {
	delete ripcd_tty_dev[tty_port];
	ripcd_tty_dev[tty_port]=NULL;
      }
    }
  }
  delete q;
  QTimer *timer=new QTimer(this,"tty_scan_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(ttyScanData()));
  timer->start(RIPCD_TTY_READ_INTERVAL);
}


void MainObject::RunLocalMacros(RDMacro *rml)
{
  int matrix_num;
  int gpi;
  int tty_port;
  int severity=0;
  QString str;
  QString sql;
  QString cmd;
  RDSqlQuery *q;
  QHostAddress addr;
  RDUser *rduser;
  char logstr[RD_RML_MAX_LENGTH];
  char bin_buf[RD_RML_MAX_LENGTH];
  int d;
  RDMatrix::GpioType gpio_type;
  QByteArray data;

  rml->generateString(logstr,RD_RML_MAX_LENGTH-1);
  LogLine(RDConfig::LogInfo,QString().sprintf("received rml: \'%s\' from %s",
	  (const char *)logstr,(const char *)rml->address().toString()));

  ForwardConvert(rml);

  switch(rml->command()) {
  case RDMacro::BO:
    tty_port=rml->arg(0).toInt();
    if((tty_port<0)||(tty_port>MAX_TTYS)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
	return;
      }
    }
    if(ripcd_tty_dev[tty_port]==NULL) {
      rml->acknowledge(false);
      sendRml(rml);
      return;
    }
    for(int i=1;i<(rml->argQuantity());i++) {
      sscanf((const char *)rml->arg(i).toString(),"%x",&d);
      bin_buf[i-1]=0xFF&d;
    }
    ripcd_tty_dev[tty_port]->writeBlock(bin_buf,rml->argQuantity()-1);
    rml->acknowledge(true);
    sendRml(rml);
    return;
    break;
      
  case RDMacro::DB:
    if(rml->argQuantity()!=1) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(fork()==0) {
      cmd=QString().sprintf("mysqldump -c Rivendell -h %s -u %s -p%s > %s",
			    (const char *)ripcd_config->mysqlHostname(),
			    (const char *)ripcd_config->mysqlUsername(),
			    (const char *)ripcd_config->mysqlPassword(),
			    (const char *)rml->arg(0).toString());
      system((const char *)cmd);
      exit(0);
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;
      
  case RDMacro::GI:
    if(rml->argQuantity()!=5) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    matrix_num=rml->arg(0).toInt();
    if(rml->arg(1).toString().lower()=="i") {
      gpio_type=RDMatrix::GpioInput;
    }
    else {
      if(rml->arg(1).toString().lower()=="o") {
	gpio_type=RDMatrix::GpioOutput;
      }
      else {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  sendRml(rml);
	}
	return;
      }
    }
    gpi=rml->arg(2).toInt()-1;
    if((ripcd_switcher[matrix_num]==NULL)||
       (gpi>(MAX_GPIO_PINS-1))||
       (gpi<0)||
       (rml->arg(3).toInt()<0)||(rml->arg(3).toInt()>1)||
       (rml->arg(4).toInt()<-1)||(rml->arg(4).toInt()>999999)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    switch(gpio_type) {
    case RDMatrix::GpioInput:
      ripcd_gpi_macro[matrix_num][gpi][rml->arg(3).toInt()]=
	rml->arg(4).toInt();
      BroadcastCommand(QString().sprintf("GC %d %d %d %d!",matrix_num,gpi,
					 ripcd_gpi_macro[matrix_num][gpi][0],
					 ripcd_gpi_macro[matrix_num][gpi][1]));
      LogLine(RDConfig::LogWarning,QString().sprintf("cart: %u",
		       ripcd_gpi_macro[matrix_num][gpi][rml->arg(3).toInt()]));
      break;

    case RDMatrix::GpioOutput:
      ripcd_gpo_macro[matrix_num][gpi][rml->arg(3).toInt()]=
	rml->arg(4).toInt();
      BroadcastCommand(QString().sprintf("GD %d %d %d %d!",matrix_num,gpi,
					 ripcd_gpo_macro[matrix_num][gpi][0],
					 ripcd_gpo_macro[matrix_num][gpi][1]));
      break;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;
      
  case RDMacro::GE:
    if(rml->argQuantity()!=4) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    matrix_num=rml->arg(0).toInt();
    if(rml->arg(1).toString().lower()=="i") {
      gpio_type=RDMatrix::GpioInput;
    }
    else {
      if(rml->arg(1).toString().lower()=="o") {
	gpio_type=RDMatrix::GpioOutput;
      }
      else {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  sendRml(rml);
	}
	return;
      }
    }
    gpi=rml->arg(2).toInt()-1;
    if((ripcd_switcher[matrix_num]==NULL)||
       (gpi>(MAX_GPIO_PINS-1))||
       (gpi<0)||
       (rml->arg(3).toInt()<0)||(rml->arg(3).toInt()>1)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    switch(gpio_type) {
    case RDMatrix::GpioInput:
      if(rml->arg(3).toInt()==1) {
	ripcd_gpi_mask[matrix_num][gpi]=true;
	BroadcastCommand(QString().sprintf("GM %d %d 1!",matrix_num,gpi));
      }
      else {
	ripcd_gpi_mask[matrix_num][gpi]=false;
	BroadcastCommand(QString().sprintf("GM %d %d 0!",matrix_num,gpi));
      }
      break;

    case RDMatrix::GpioOutput:
      if(rml->arg(3).toInt()==1) {
	ripcd_gpo_mask[matrix_num][gpi]=true;
	BroadcastCommand(QString().sprintf("GN %d %d 1!",matrix_num,gpi));
      }
      else {
	ripcd_gpo_mask[matrix_num][gpi]=false;
	BroadcastCommand(QString().sprintf("GN %d %d 0!",matrix_num,gpi));
      }
      break;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;

  case RDMacro::JC:
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    rdcae->connectJackPorts(rml->arg(0).toString(),rml->arg(1).toString());
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;

  case RDMacro::JD:
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    rdcae->disconnectJackPorts(rml->arg(0).toString(),rml->arg(1).toString());
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;
      
  case RDMacro::LO:
    if(rml->argQuantity()>2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(rml->argQuantity()==0) {
      rduser=new RDUser(rdstation->defaultName());
    }
    else {
      rduser=new RDUser(rml->arg(0).toString());
      if(!rduser->exists()) {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  sendRml(rml);
	}
	delete rduser;
	return;
      }
      if(!rduser->checkPassword(rml->arg(1).toString(),false)) {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  sendRml(rml);
	}
	delete rduser;
	return;
      }
    }
    SetUser(rduser->name());
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    delete rduser;
    break;
      
  case RDMacro::MB:
    if(rml->argQuantity()<3) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    severity=rml->arg(1).toInt();
    if((severity<1)||(severity>3)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(fork()==0) {
      if(getuid()==0) {
	if(setegid(ripcd_config->gid())<0) {
	  LogLine(RDConfig::LogWarning,QString().
		  sprintf("unable to set group id %d for RDPopup",
			  ripcd_config->gid()));
	  if(rml->echoRequested()) {
	    rml->acknowledge(false);
	    sendRml(rml);
	  }
	}
	if(seteuid(ripcd_config->uid())<0) {
	  LogLine(RDConfig::LogWarning,QString().
		  sprintf("unable to set user id %d for RDPopup",
			  ripcd_config->uid()));
	  if(rml->echoRequested()) {
	    rml->acknowledge(false);
	    sendRml(rml);
	  }
	}
      }
      if(system(QString().
		sprintf("rdpopup -display %s %s %s",
			(const char *)rml->arg(0).toString(),
			(const char *)rml->arg(1).toString(),
			(const char *)RDEscapeString(rml->rollupArgs(2))))<0) {
	LogLine(RDConfig::LogWarning,"RDPopup returned an error");
      }
      exit(0);
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;

  case RDMacro::MT:
    if(rml->argQuantity()!=3) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if((rml->arg(0).toUInt()==0)||
       (rml->arg(0).toUInt()>RD_MAX_MACRO_TIMERS)||
       (rml->arg(2).toUInt()>999999)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if((rml->arg(1).toUInt()==0)||
       (rml->arg(2).toUInt()==0)) {
      ripc_macro_cart[rml->arg(0).toUInt()-1]=0;
      ripc_macro_timer[rml->arg(0).toUInt()-1]->stop();
      if(rml->echoRequested()) {
	rml->acknowledge(true);
	sendRml(rml);
      }
      return;
    }
    ripc_macro_cart[rml->arg(0).toUInt()-1]=rml->arg(2).toUInt();
    ripc_macro_timer[rml->arg(0).toUInt()-1]->stop();
    ripc_macro_timer[rml->arg(0).toUInt()-1]->
      start(rml->arg(1).toInt(),true);
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    return;
      
  case RDMacro::RN:
    if(rml->argQuantity()<1) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(fork()==0) {
      QString cmd=rml->arg(0).toString();
      for(int i=1;i<=rml->argQuantity();i++) {
	cmd+=" "+rml->arg(i).toString();
      }
      if(getuid()==0) {
	if(setgid(ripcd_config->gid())<0) {
	  LogLine(RDConfig::LogWarning,QString().
		  sprintf("unable to set group id %d for RN",
			  ripcd_config->gid()));
	  if(rml->echoRequested()) {
	    rml->acknowledge(false);
	    sendRml(rml);
	  }
	}
	if(setuid(ripcd_config->uid())<0) {
	  LogLine(RDConfig::LogWarning,QString().
		  sprintf("unable to set user id %d for RN",
			  ripcd_config->uid()));
	  if(rml->echoRequested()) {
	    rml->acknowledge(false);
	    sendRml(rml);
	  }
	}
      }
      system((const char *)cmd);
      exit(0);
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;
      
  case RDMacro::SI:
    tty_port=rml->arg(0).toInt();
    if((tty_port<0)||(tty_port>MAX_TTYS)||(rml->argQuantity()!=3)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(ripcd_tty_dev[tty_port]==NULL) {
      rml->acknowledge(false);
      sendRml(rml);
      return;
    }
    for(int i=2;i<(rml->argQuantity()-1);i++) {
      str+=(rml->arg(i).toString()+" ");
    }
    str+=rml->arg(rml->argQuantity()-1).toString();
    ripcd_tty_trap[tty_port]->addTrap(rml->arg(1).toInt(),
				      str,str.length());
    rml->acknowledge(true);
    sendRml(rml);
    return;
    break;
      
  case RDMacro::SC:
    tty_port=rml->arg(0).toInt();
    if((tty_port<0)||(tty_port>MAX_TTYS)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
	return;
      }
    }
    if(ripcd_tty_dev[tty_port]==NULL) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    switch(rml->argQuantity()) {
    case 1:
      ripcd_tty_trap[tty_port]->clear();
      if(rml->echoRequested()) {
	rml->acknowledge(true);
	sendRml(rml);
      }
      break;

    case 2:
      ripcd_tty_trap[tty_port]->removeTrap(rml->arg(1).toInt());
      if(rml->echoRequested()) {
	rml->acknowledge(true);
	sendRml(rml);
      }
      break;
	  
    case 3:
      ripcd_tty_trap[tty_port]->removeTrap(rml->arg(1).toInt(),
					   (const char *)rml->arg(2).toString(),
					   rml->arg(2).toString().length());
      if(rml->echoRequested()) {
	rml->acknowledge(true);
	sendRml(rml);
      }
      break;
	  
    default:
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
      break;
    }
    break;
      
  case RDMacro::SO:
    tty_port=rml->arg(0).toInt();
    if((tty_port<0)||(tty_port>MAX_TTYS)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
	return;
      }
    }
    if(ripcd_tty_dev[tty_port]==NULL) {
      rml->acknowledge(false);
      sendRml(rml);
      return;
    }
    for(int i=1;i<(rml->argQuantity()-1);i++) {
      str+=(rml->arg(i).toString()+" ");
    }
    str+=rml->arg(rml->argQuantity()-1).toString();
    switch(ripcd_tty_term[tty_port]) {
    case RDTty::CrTerm:
      str+=QString().sprintf("\x0d");
      break;
      
    case RDTty::LfTerm:
      str+=QString().sprintf("\x0a");
      break;
      
    case RDTty::CrLfTerm:
      str+=QString().sprintf("\x0d\x0a");
      break;
      
    default:
      break;
    }
    data=RDStringToData(str);
    ripcd_tty_dev[tty_port]->writeBlock((const char *)data,data.size());
    rml->acknowledge(true);
    sendRml(rml);
    return;
    break;
      
  case RDMacro::CL:
  case RDMacro::FS:
  case RDMacro::GO:
  case RDMacro::ST:
  case RDMacro::SA:
  case RDMacro::SD:
  case RDMacro::SG:
  case RDMacro::SR:
  case RDMacro::SL:
  case RDMacro::SX:
    if((rml->arg(0).toInt()<0)||(rml->arg(0).toInt()>=MAX_MATRICES)) {
      if(!rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(ripcd_switcher[rml->arg(0).toInt()]==NULL) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
    }
    else {
      ripcd_switcher[rml->arg(0).toInt()]->processCommand(rml);
    }
    break;
      
  case RDMacro::SY:
    if(rml->argQuantity()!=1) {
      return;
    }
    tty_port=rml->arg(0).toInt();
    if((tty_port<0)||(tty_port>=MAX_TTYS)) {
      return;
    }
      
    //
    // Shutdown TTY Port
    //
    if(ripcd_tty_dev[tty_port]!=NULL) {
      ripcd_tty_dev[tty_port]->close();
      delete ripcd_tty_dev[tty_port];
      ripcd_tty_dev[tty_port]=NULL;
      ripcd_tty_inuse[tty_port]=false;
      delete ripcd_tty_trap[tty_port];
      ripcd_tty_trap[tty_port]=NULL;
    }
  
    //
    // Try to Restart
    //
    sql=QString().sprintf("select PORT_ID,PORT,BAUD_RATE,DATA_BITS,PARITY,\
                         TERMINATION from TTYS where (STATION_NAME=\"%s\")&& \
                         (ACTIVE=\"Y\")&&(PORT_ID=%d)",
			  (const char *)rdstation->name(),tty_port);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      if(!ripcd_tty_inuse[tty_port]) {
	ripcd_tty_dev[tty_port]=new RDTTYDevice();
	ripcd_tty_dev[tty_port]->setName(q->value(1).toString());
	ripcd_tty_dev[tty_port]->setSpeed(q->value(2).toInt());
	ripcd_tty_dev[tty_port]->setWordLength(q->value(3).toInt());
	ripcd_tty_dev[tty_port]->
	  setParity((RDTTYDevice::Parity)q->value(4).toInt());
	if(ripcd_tty_dev[tty_port]->open(IO_ReadWrite)) {
	  ripcd_tty_term[tty_port]=(RDTty::Termination)q->value(5).toInt();
	  ripcd_tty_inuse[tty_port]=true;
	  ripcd_tty_trap[tty_port]=new RDCodeTrap(this);
	  connect(ripcd_tty_trap[tty_port],SIGNAL(trapped(int)),
		  this,SLOT(ttyTrapData(int)));
	}
	else {
	  delete ripcd_tty_dev[tty_port];
	  ripcd_tty_dev[tty_port]=NULL;
	}
      }
    }
    delete q;
    break;

  case RDMacro::SZ:
    if(rml->argQuantity()!=1) {
      return;
    }
    matrix_num=rml->arg(0).toInt();
    if((matrix_num<0)||(matrix_num>=MAX_MATRICES)) {
      return;
    }
      
    //
    // Shutdown the old switcher
    //
    for(int i=0;i<2;i++) {
      if(ripcd_switcher_tty[matrix_num][i]>-1) {
	ripcd_tty_inuse[ripcd_switcher_tty[matrix_num][i]]=false;
	ripcd_switcher_tty[matrix_num][i]=-1;
      }
    }
    delete ripcd_switcher[matrix_num];
    ripcd_switcher[matrix_num]=NULL;

    //
    // Startup the new
    //
    if(!LoadSwitchDriver(matrix_num)) {
      LogLine(RDConfig::LogErr,QString().
            sprintf("attempted to load unknown switcher driver for matrix %d",
		      matrix_num));
    }
    break;
    
  case RDMacro::TA:
    if((rml->argQuantity()!=1)||
       (rml->arg(0).toInt()<0)||(rml->arg(0).toInt()>1)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if((rml->arg(0).toInt()==0)&&ripc_onair_flag) {
      BroadcastCommand("TA 0!");
      LogLine(RDConfig::LogInfo,"onair flag OFF");
    }
    if((rml->arg(0).toInt()==1)&&(!ripc_onair_flag)) {
      BroadcastCommand("TA 1!");
      LogLine(RDConfig::LogInfo,"onair flag ON");
    }
    ripc_onair_flag=rml->arg(0).toInt();
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;

  case RDMacro::UO:
    if(rml->argQuantity()<3) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if(!addr.setAddress(rml->arg(0).toString())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    if((rml->arg(1).toInt()<0)||(rml->arg(1).toInt()>0xFFFF)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	sendRml(rml);
      }
      return;
    }
    for(int i=2;i<(rml->argQuantity()-1);i++) {
      str+=(rml->arg(i).toString()+" ");
    }
    str+=rml->arg(rml->argQuantity()-1).toString();
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("Sending \"%s\" to %s:%d",(const char *)str,
		    (const char *)addr.toString(),rml->arg(1).toInt()));
    data=RDStringToData(str);
    ripcd_rml_send->writeBlock((const char *)data,data.size(),addr,
			       (Q_UINT16)(rml->arg(1).toInt()));
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      sendRml(rml);
    }
    break;
 
  default:
//	LogLine(RDConfig::LogDebug,QString().sprintf("unhandled rml: \'%s\' from %s",
//	       (const char *)logstr,(const char *)rml->address().toString()));
    break;
  }
}


void MainObject::ForwardConvert(RDMacro *rml) const
{
  //
  // Convert old RML syntax to current forms
  //
  switch(rml->command()) {
    case RDMacro::GE:
      if(rml->argQuantity()==3) {
	rml->setArgQuantity(4);
	for(int i=2;i>=1;i--) {
	  rml->setArg(i+1,rml->arg(i));
	}
	rml->setArg(1,"I");
      }
      break;

    case RDMacro::GI:
      if(rml->argQuantity()==3) {
	rml->setArgQuantity(4);
	rml->setArg(3,0);
      }
      if(rml->argQuantity()==4) {
	rml->setArgQuantity(5);
	for(int i=3;i>=1;i--) {
	  rml->setArg(i+1,rml->arg(i));
	}
	rml->setArg(1,"I");
      }
      break;

    case RDMacro::GO:
      if(rml->argQuantity()==4) {
	rml->setArgQuantity(5);
	for(int i=3;i>=1;i--) {
	  rml->setArg(i+1,rml->arg(i));
	}
	rml->setArg(1,"O");
      }
      break;

    default:
      break;
  }
}
