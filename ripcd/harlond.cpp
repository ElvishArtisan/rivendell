// harlond.cpp
//
// A Rivendell switcher driver for the Harlond Virtual Mixer
//
//   (C) Copyright 2002-2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: harlond.cpp,v 1.2.2.2 2012/08/06 00:12:07 cvs Exp $
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

#include <qstringlist.h>

#include "globals.h"
#include "harlond.h"

Harlond::Harlond(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  bt_recv_buffer="";

  //
  // Get Matrix Parameters
  //
  bt_ip_address=matrix->ipAddress(RDMatrix::Primary);
  bt_tcp_port=matrix->ipPort(RDMatrix::Primary);
  bt_password=matrix->password(RDMatrix::Primary);
  bt_matrix=matrix->matrix();
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();
  bt_start_cart=matrix->startCart(RDMatrix::Primary);
  bt_stop_cart=matrix->stopCart(RDMatrix::Primary);

  //
  // Reset Timers
  //
  bt_reset_mapper=new QSignalMapper(this);
  connect(bt_reset_mapper,SIGNAL(mapped(int)),this,SLOT(resetTimeoutData(int)));
  for(int i=0;i<bt_inputs;i++) {
    bt_reset_states.push_back(false);
    bt_reset_timers.push_back(new QTimer(this));
    bt_reset_mapper->setMapping(bt_reset_timers.back(),i);
    connect(bt_reset_timers.back(),SIGNAL(timeout()),
	    bt_reset_mapper,SLOT(map()));
  }

  //
  // Initialize TCP/IP Connection
  //
  bt_socket=new QSocket(this);
  connect(bt_socket,SIGNAL(connected()),this,SLOT(socketConnectedData()));
  connect(bt_socket,SIGNAL(disconnected()),this,SLOT(socketDisconnectedData()));
  connect(bt_socket,SIGNAL(readyRead()),this,SLOT(socketReadyReadData()));
  connect(bt_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(socketErrorData(QAbstractSocket::SocketError)));
  bt_watchdog_timer=new QTimer(this);
  connect(bt_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogTimeoutData()));
  bt_watchdog_timer->start(5000,true);
}


Harlond::~Harlond()
{
  for(unsigned i=0;i<bt_reset_timers.size();i++) {
    delete bt_reset_timers[i];
  }
  delete bt_reset_mapper;
  delete bt_socket;
}


RDMatrix::Type Harlond::type()
{
  return RDMatrix::Harlond;
}


unsigned Harlond::gpiQuantity()
{
  return 0;
}


unsigned Harlond::gpoQuantity()
{
  return 0;
}


bool Harlond::primaryTtyActive()
{
  return false;
}


bool Harlond::secondaryTtyActive()
{
  return false;
}


void Harlond::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
  case RDMacro::FS:      // Fire Salvo
    if(cmd->argQuantity()<2) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(!ProcessSalvo(cmd->rollupArgs(1))) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;


  case RDMacro::GO:      // GPO Set
    if(cmd->argQuantity()!=5) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(!ProcessGpo(cmd->arg(2).toInt(),cmd->arg(3).toBool(),
		   cmd->arg(4).toInt())) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  case RDMacro::SL:     // Switch Level
    if(cmd->argQuantity()!=3) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(!SetInputLevel(cmd->arg(1).toInt(),cmd->arg(2).toInt())) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  case RDMacro::SA:     // Switch Add
  case RDMacro::SR:     // Switch Remove
  case RDMacro::ST:     // Switch Take
    if(cmd->argQuantity()!=3) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(!ProcessCrosspoint(cmd->command(),cmd->arg(1).toInt(),
			  cmd->arg(2).toInt())) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  default:
    break;
  }
}


void Harlond::resetTimeoutData(int line)
{
  ProcessGpo(line,!bt_reset_states[line],0);
}


void Harlond::socketConnectedData()
{
  QString str=QString("PW ")+bt_password+"!";
  bt_socket->writeBlock(str,str.length());
  if(bt_start_cart>0) {
    executeMacroCart(bt_start_cart);
  }
}


void Harlond::socketDisconnectedData()
{
  bt_watchdog_timer->start(HARLOND_RECONNECT_INTERVAL,true);
  LogLine(RDConfig::LogWarning,tr("connection to harlond device at ")+
	  bt_ip_address.toString()+
	  QString().sprintf(":%d ",bt_tcp_port)+
	  tr("closed unexpectedly, attempting reconnect"));
  if(bt_stop_cart>0) {
    executeMacroCart(bt_stop_cart);
  }
}


void Harlond::socketReadyReadData()
{
  char data[1500];
  int n;
  while((n=bt_socket->readBlock(data,1500))>0) {
    for(int i=0;i<n;i++) {
      if(data[i]=='!') {
	ProcessResponse(bt_recv_buffer);
	bt_recv_buffer="";
      }
      else {
	bt_recv_buffer+=data[i];
      }
    }
  }
}


void Harlond::socketErrorData(int err)
{
  bt_watchdog_timer->start(HARLOND_RECONNECT_INTERVAL,true);
  switch(err) {
  case QSocket::ErrConnectionRefused:
  LogLine(RDConfig::LogWarning,tr("connection to harlond device at ")+
	  bt_ip_address.toString()+QString().sprintf(":%d ",bt_tcp_port)+
	  tr("refused, attempting reconnect"));
    break;

  default:
    LogLine(RDConfig::LogWarning,tr("received network error")+
	    QString().sprintf(" %d ",err)+tr("from harlond device at ")+
	    bt_ip_address.toString()+QString().sprintf(":%d ",bt_tcp_port)+", "+
	    tr("attempting reconnect"));
    break;
  }
}


void Harlond::watchdogTimeoutData()
{
  if(bt_socket->state()!=QSocket::Connected) {
    bt_socket->connectToHost(bt_ip_address.toString(),bt_tcp_port);
  }
}


void Harlond::ProcessResponse(const QString &str)
{
  //  LogLine(RDConfig::LogNotice,str);
  QStringList cmds=cmds.split(" ",str);

  if(cmds[0]=="PW") {
    if(cmds.size()==2) {
      if(cmds[1]=="+") {
	LogLine(RDConfig::LogInfo,tr("connection to harlond device at ")+
		bt_ip_address.toString()+QString().sprintf(":%d ",bt_tcp_port)+
		tr("established"));
	bt_socket->writeBlock("SS!",3);
	return;
      }
    }
    LogLine(RDConfig::LogInfo,tr("connection to harlond device at ")+
	    bt_ip_address.toString()+QString().sprintf(":%d ",bt_tcp_port)+
	    tr("refused, invalid password"));
  }

  if(cmds[0]=="ON") {
    if(cmds.size()==2) {
      emit gpiChanged(bt_matrix,cmds[1].toInt()-1,true);
    }
  }

  if(cmds[0]=="OF") {
    if(cmds.size()==2) {
      emit gpiChanged(bt_matrix,cmds[1].toInt()-1,false);
    }
  }
}


bool Harlond::ProcessSalvo(const QString &str)
{
  QString arg=QString("LL ")+str+"!";
  bt_socket->writeBlock(arg,arg.length());
  return true;
}


bool Harlond::ProcessGpo(int line,bool state,int msecs)
{
  QString code="OF";
  QString str;

  if((line<1)||(line>bt_inputs)) {
    return false;
  }
  if(state) {
    code="ON";
  }
  bt_reset_states[line]=state;
  str=code+QString().sprintf(" %02d!",line);
  bt_socket->writeBlock(str,str.length());
  bt_reset_timers[line]->stop();
  if(msecs>0) {
    bt_reset_timers[line]->start(msecs,true);
  }

  return true;
}


bool Harlond::SetInputLevel(int input,int db)
{
  QString str;

  if((input<1)||(input>bt_inputs)) {
    return false;
  }
  str=QString().sprintf("VL %02d %04d!",input,db);
  bt_socket->writeBlock(str,str.length());

  return true;
}


bool Harlond::ProcessCrosspoint(RDMacro::Command cmd,int input,int output)
{
  bool ret=false;

  if((input<1)||(input>bt_inputs)) {
    return false;
  }
  switch(cmd) {
  case RDMacro::ST:
    ret=TakeCrosspoint(input,output);
    break;

  case RDMacro::SA:
    ret=AddCrosspoint(input,output);
    break;

  case RDMacro::SR:
    ret=RemoveCrosspoint(input,output);
    break;

  default:
    break;
  }
  return ret;
}


bool Harlond::TakeCrosspoint(int input,int output)
{
  QString str;
  QString code=GetBussCode(output);

  if(code.isEmpty()) {
    return false;
  }
  AddCrosspoint(input,output);
  for(int i=1;i<input;i++) {
    RemoveCrosspoint(i,output);
  }
  LogLine(RDConfig::LogNotice,QString().sprintf("inputs: %d",bt_inputs));

  for(int i=input+1;i<=bt_inputs;i++) {
    RemoveCrosspoint(i,output);
  }
  return true;
}


bool Harlond::AddCrosspoint(int input,int output)
{
  QString str;
  QString code=GetBussCode(output);

  if(code.isEmpty()) {
    return false;
  }
  str=code+QString().sprintf(" %02d +!",input);
  bt_socket->writeBlock(str,str.length());
  return true;
}


bool Harlond::RemoveCrosspoint(int input,int output)
{
  QString str;
  QString code=GetBussCode(output);

  if(code.isEmpty()) {
    return false;
  }
  str=code+QString().sprintf(" %02d -!",input);
  bt_socket->writeBlock(str,str.length());
  return true;
}


QString Harlond::GetBussCode(int output)
{
  QString code="";
  QString str;

  if((bt_outputs<1)||(output>=bt_outputs)) {
    return QString();
  }
  switch(output) {
  case 1:        // Program
    code="PG";
    break;

  case 2:        // Audition
    code="AU";
    break;

  case 3:        // Utility
    code="UT";
    break;

  case 4:        // Cue
    code="QU";
    break;

  default:
    return QString();
  }
  return code;
}
