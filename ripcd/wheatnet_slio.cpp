// wheatnet_slio.cpp
//
// A Rivendell switcher driver for WheatNet SLIO
//
//   (C) Copyright 2017-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

#include <rddb.h>
#include <rdescape_string.h>

#include "wheatnet_slio.h"

WheatnetSlio::WheatnetSlio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  slio_watchdog_active=false;

  slio_gpios=0;
  slio_ip_address=matrix->ipAddress(RDMatrix::Primary);
  slio_ip_port=matrix->ipPort(RDMatrix::Primary);

  slio_socket=new Q3Socket(this);
  connect(slio_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(slio_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(slio_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
  slio_socket->connectToHost(slio_ip_address.toString(),slio_ip_port);

  slio_poll_timer=new QTimer(this);
  connect(slio_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));

  slio_reset_mapper=new QSignalMapper(this);
  connect(slio_reset_mapper,SIGNAL(mapped(int)),
	  this,SLOT(resetStateData(int)));
  for(int i=0;i<slio_gpios;i++) {
    slio_reset_timers.push_back(new QTimer(this));
    connect(slio_reset_timers.back(),SIGNAL(timeout()),
	    slio_reset_mapper,SLOT(map()));
    slio_reset_mapper->setMapping(slio_reset_timers.back(),i);
    slio_reset_states.push_back(false);
  }
  slio_watchdog_timer=new QTimer(this);
  connect(slio_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));
}


WheatnetSlio::~WheatnetSlio()
{
  delete slio_watchdog_timer;
  delete slio_poll_timer;
  for(unsigned i=0;i<slio_reset_timers.size();i++) {
    delete slio_reset_timers[i];
  }
  delete slio_reset_mapper;
  delete slio_socket;
}


RDMatrix::Type WheatnetSlio::type()
{
  return RDMatrix::WheatnetSlio;
}


unsigned WheatnetSlio::gpiQuantity()
{
  return slio_gpios;
}


unsigned WheatnetSlio::gpoQuantity()
{
  return slio_gpios;
}


bool WheatnetSlio::primaryTtyActive()
{
  return false;
}


bool WheatnetSlio::secondaryTtyActive()
{
  return false;
}


void WheatnetSlio::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
  case RDMacro::GO:
    if((cmd->argQuantity()!=5)||
       ((cmd->arg(1).lower()!="i")&&
	(cmd->arg(1).lower()!="o"))||
       (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>slio_gpios)||
       (cmd->arg(2).toInt()>slio_gpios)||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(1).lower()!="i"))||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).lower()=="i"))||
       (cmd->arg(4).toInt()<0)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(3).toInt()==0) {  // Turn OFF
      if(cmd->arg(4).toInt()==0) {
	if(cmd->arg(1).lower()=="o") {
	  SendCommand(QString().sprintf("<SLIO:%d|LVL:0>",cmd->arg(2).toInt()));
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,false);
	}
      }
      else {
	if(cmd->echoRequested()) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	}
	return;
      }
    }
    else {
      if(cmd->arg(4).toInt()==0) {  // Turn ON
	if(cmd->arg(1).lower()=="o") {
	  SendCommand(QString().sprintf("<SLIO:%d|LVL:1>",cmd->arg(2).toInt()));
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,true);
	}
      }
      else {  // Pulse
	if(cmd->arg(1).lower()=="o") {
	  SendCommand(QString().sprintf("<SLIO:%d|LVL:%d>",
					cmd->arg(2).toInt(),
					cmd->arg(3).toInt()!=0));
	  slio_reset_states[cmd->arg(2).toInt()-1]=cmd->arg(3).toInt()==0;
	  slio_reset_timers[cmd->arg(2).toInt()-1]->
	    start(cmd->arg(4).toInt(),true);
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,
			  cmd->arg(3).toInt()!=0);
	}
      }
    }
    
    if(cmd->echoRequested()) {
      cmd->acknowledge(true);
      emit rmlEcho(cmd);
    }
    break;

  default:
    break;
  }
}


void WheatnetSlio::connectedData()
{
  syslog(LOG_INFO,
	 "connection to WheatNet SLIO device at %s:%u established",
	 (const char *)slio_ip_address.toString(),0xffff&slio_ip_port);
  slio_watchdog_active=false;
  SendCommand("<SYS?SLIO>");
}


void WheatnetSlio::readyReadData()
{
  char data[1501];
  int n=0;

  while((n=slio_socket->readBlock(data,1500))>0) {
    data[n]=0;
    for(int i=0;i<n;i++) {
      switch(0xff&data[i]) {
      case 13:
	break;

      case 10:
	ProcessCommand(slio_accum);
	slio_accum="";
	break;

      default:
	slio_accum+=data[i];
	break;
      }
    }
  }
}


void WheatnetSlio::errorData(int err)
{
  watchdogData();
}


void WheatnetSlio::resetStateData(int line)
{
  SendCommand(QString().sprintf("<SLIO:%d|LVL:%d>",line+1,
				(int)slio_reset_states[line]));
  emit gpoChanged(matrixNumber(),line,slio_reset_states[line]);
}


void WheatnetSlio::pollData()
{
  SendCommand("<SYS?BLID>");
}


void WheatnetSlio::watchdogData()
{
  if(!slio_watchdog_active) {
    syslog(LOG_WARNING,
	   "connection to Wheatnet SLIO device at %s:%u lost, attempting reconnect",
	   (const char *)slio_ip_address.toString(),0xffff&slio_ip_port);
    slio_watchdog_active=true;
  }
  slio_socket->close();
  slio_socket->connectToHost(slio_ip_address.toString(),slio_ip_port);
}


void WheatnetSlio::CheckLineEntry(int line)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ID from GPIS where ")+
    "(STATION_NAME=\""+RDEscapeString(stationName())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrixNumber())+
    QString().sprintf("(NUMBER=%d)",line);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString("insert into GPIS set ")+
      "STATION_NAME=\""+RDEscapeString(stationName())+"\","+
      QString().sprintf("MATRIX=%d,",matrixNumber())+
      QString().sprintf("NUMBER=%d",line);
    q=new RDSqlQuery(sql);
  }
  delete q;

  sql=QString("select ID from GPOS where ")+
    "(STATION_NAME=\""+RDEscapeString(stationName())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrixNumber())+
    QString().sprintf("(NUMBER=%d)",line);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString("insert into GPOS set ")+
      "STATION_NAME=\""+RDEscapeString(stationName())+"\","+
      QString().sprintf("MATRIX=%d,",matrixNumber())+
      QString().sprintf("NUMBER=%d",line);
    q=new RDSqlQuery(sql);
  }
  delete q;
}


void WheatnetSlio::ProcessSys(const QString &cmd)
{
  //  printf("SYS: %s\n",(const char *)cmd);
  QString sql;
  RDSqlQuery *q;
  bool ok=false;

  QStringList f0=cmd.split(":");
  if((f0[0]=="SLIO")&&(f0.size()==2)) {
    int slio=f0[1].toUInt(&ok);
    if(ok) {
      slio_gpios=slio;
      for(unsigned i=0;i<slio_reset_timers.size();i++) {
	delete slio_reset_timers[i];
      }
      slio_reset_timers.clear();
      slio_reset_states.clear();
      slio_gpi_states.clear();
      for(int i=0;i<slio_gpios;i++) {
	slio_reset_timers.push_back(new QTimer(this));
	connect(slio_reset_timers.back(),SIGNAL(timeout()),
		slio_reset_mapper,SLOT(map()));
	slio_reset_mapper->setMapping(slio_reset_timers.back(),i);
	slio_reset_states.push_back(false);
	slio_gpi_states.push_back(false);
	CheckLineEntry(i+1);
	SendCommand(QString().sprintf("<SLIOSUB:%d|LVL:1>",i+1));
      }
      sql=QString("update MATRICES set ")+
	QString().sprintf("GPIS=%d,GPOS=%d where ",slio_gpios,slio_gpios)+
	"(STATION_NAME=\""+RDEscapeString(stationName())+"\")&&"+
	QString().sprintf("(MATRIX=%d)",matrixNumber());
      q=new RDSqlQuery(sql);
      delete q;
      slio_watchdog_timer->start(WHEATNET_SLIO_WATCHDOG_INTERVAL,true);
      slio_poll_timer->start(WHEATNET_SLIO_POLL_INTERVAL,true);
    }
  }
  if((f0[0]=="BLID")&&(f0.size()==2)) {
    slio_watchdog_timer->stop();
    slio_watchdog_timer->start(WHEATNET_SLIO_WATCHDOG_INTERVAL,true);
    slio_poll_timer->start(WHEATNET_SLIO_POLL_INTERVAL,true);
  }
}


void WheatnetSlio::ProcessSlioevent(int chan,QString &cmd)
{
  //  printf("ProcessSlip(%d,%s)\n",chan,(const char *)cmd);
  QStringList f0=cmd.split(":");
  if((f0[0]=="LVL")&&(f0.size()==2)) {
    if(chan<=(int)slio_gpi_states.size()) {
      bool state=f0[1]=="1";
      if(state!=slio_gpi_states[chan-1]) {
	slio_gpi_states[chan-1]=state;
	emit gpiChanged(matrixNumber(),chan-1,state);
      }
    }
    else {
      syslog(LOG_WARNING,
	     "WheatNet device at %s:%d sent invalid SLIOEVENT LVL update [%s]",
	     (const char *)slio_ip_address.toString(),
	     slio_ip_port,(const char *)cmd);
    }
    if(chan==slio_gpios) {
      slio_poll_timer->start(50,true);
      slio_watchdog_timer->stop();
      slio_watchdog_timer->start(1000,true);
    }
  }
}


void WheatnetSlio::ProcessCommand(const QString &cmd)
{
  //  printf("ProcessCommand(%s)\n",(const char *)cmd);
  bool ok=false;

  if((cmd.left(1)=="<")&&(cmd.right(1)==">")) {
    QStringList f0=cmd.mid(1,cmd.length()-2).split("|");
    if(f0.size()==2) {
      QStringList f1=f0[0].split(":");
      if(f1[0]=="SYS") {
	ProcessSys(f0[1]);
      }
      if((f1[0]=="SLIOEVENT")&&(f1.size()==2)) {
	int chan=f1[1].toUInt(&ok);
	if(ok) {
	  ProcessSlioevent(chan,f0[1]);
	}
      }
    }
  }
}


void WheatnetSlio::SendCommand(const QString &cmd)
{
  slio_socket->writeBlock(cmd+"\r\n",cmd.length()+2);
}
