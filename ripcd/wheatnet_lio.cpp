// wheatnet_lio.cpp
//
// A Rivendell switcher driver for WheatNet LIO
//
//   (C) Copyright 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "wheatnet_lio.h"

WheatnetLio::WheatnetLio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  lio_watchdog_active=false;

  lio_gpios=0;
  lio_ip_address=matrix->ipAddress(RDMatrix::Primary);
  lio_ip_port=matrix->ipPort(RDMatrix::Primary);
  lio_card=matrix->card();

  lio_socket=new QTcpSocket(this);
  connect(lio_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(lio_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(lio_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  lio_socket->connectToHost(lio_ip_address.toString(),lio_ip_port);

  lio_poll_timer=new QTimer(this);
  lio_poll_timer->setSingleShot(true);
  connect(lio_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));

  lio_reset_mapper=new QSignalMapper(this);
  connect(lio_reset_mapper,SIGNAL(mapped(int)),
	  this,SLOT(resetStateData(int)));
  for(int i=0;i<lio_gpios;i++) {
    lio_reset_timers.push_back(new QTimer(this));
    lio_reset_timers.back()->setSingleShot(true);
    connect(lio_reset_timers.back(),SIGNAL(timeout()),
	    lio_reset_mapper,SLOT(map()));
    lio_reset_mapper->setMapping(lio_reset_timers.back(),i);
    lio_reset_states.push_back(false);
  }
  lio_watchdog_timer=new QTimer(this);
  lio_watchdog_timer->setSingleShot(true);
  connect(lio_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));
}


WheatnetLio::~WheatnetLio()
{
  delete lio_watchdog_timer;
  delete lio_poll_timer;
  for(unsigned i=0;i<lio_reset_timers.size();i++) {
    delete lio_reset_timers[i];
  }
  delete lio_reset_mapper;
  delete lio_socket;
}


RDMatrix::Type WheatnetLio::type()
{
  return RDMatrix::WheatnetLio;
}


unsigned WheatnetLio::gpiQuantity()
{
  return lio_gpios;
}


unsigned WheatnetLio::gpoQuantity()
{
  return lio_gpios;
}


bool WheatnetLio::primaryTtyActive()
{
  return false;
}


bool WheatnetLio::secondaryTtyActive()
{
  return false;
}


void WheatnetLio::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
  case RDMacro::GO:
    if((cmd->argQuantity()!=5)||
       ((cmd->arg(1).toLower()!="i")&&
	(cmd->arg(1).toLower()!="o"))||
       (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>lio_gpios)||
       (cmd->arg(2).toInt()>lio_gpios)||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(1).toLower()!="i"))||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).toLower()=="i"))||
       (cmd->arg(4).toInt()<0)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(3).toInt()==0) {  // Turn OFF
      if(cmd->arg(4).toInt()==0) {
	if(cmd->arg(1).toLower()=="o") {
	  SendCommand(QString().sprintf("<LIO:%d|LVL:0>",
					cmd->arg(2).toInt()-1));
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
	if(cmd->arg(1).toLower()=="o") {
	  SendCommand(QString().sprintf("<LIO:%d|LVL:1>",
					cmd->arg(2).toInt()-1));
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,true);
	}
      }
      else {  // Pulse
	if(cmd->arg(1).toLower()=="o") {
	  SendCommand(QString().sprintf("<LIO:%d|LVL:%d>",
					cmd->arg(2).toInt()-1,
					cmd->arg(3).toInt()!=0));
	  lio_reset_states[cmd->arg(2).toInt()-1]=cmd->arg(3).toInt()==0;
	  lio_reset_timers[cmd->arg(2).toInt()-1]->
	    start(cmd->arg(4).toInt());
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


void WheatnetLio::connectedData()
{
  rda->syslog(LOG_INFO,
	      "connection to WheatNet LIO device at %s:%u established",
	      lio_ip_address.toString().toUtf8().constData(),0xffff&lio_ip_port);
  lio_watchdog_active=false;
  SendCommand("<SYS?LIO>");
}


void WheatnetLio::readyReadData()
{
  char data[1501];
  int n=0;

  while((n=lio_socket->read(data,1500))>0) {
    data[n]=0;
    for(int i=0;i<n;i++) {
      switch(0xff&data[i]) {
      case 13:
	break;

      case 10:
	ProcessCommand(lio_accum);
	lio_accum="";
	break;

      default:
	lio_accum+=data[i];
	break;
      }
    }
  }
}


void WheatnetLio::errorData(QAbstractSocket::SocketError err)
{
  watchdogData();
}


void WheatnetLio::resetStateData(int line)
{
  SendCommand(QString().sprintf("<LIO:%d|LVL:%d>",line,
				(int)lio_reset_states[line]));
  emit gpoChanged(matrixNumber(),line,lio_reset_states[line]);
}


void WheatnetLio::pollData()
{
  SendCommand("<SYS?BLID>");
}


void WheatnetLio::watchdogData()
{
  if(!lio_watchdog_active) {
    rda->syslog(LOG_WARNING,
       "connection to Wheatnet LIO device at %s:%u lost, attempting reconnect",
		lio_ip_address.toString().toUtf8().constData(),
		0xffff&lio_ip_port);
    lio_watchdog_active=true;
  }
  lio_socket->close();
  lio_socket->connectToHost(lio_ip_address.toString(),lio_ip_port);
}


void WheatnetLio::CheckLineEntry(int line)
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


void WheatnetLio::ProcessSys(const QString &cmd)
{
  //  printf("SYS: %s\n",(const char *)cmd);
  QString sql;
  RDSqlQuery *q;
  bool ok=false;

  QStringList f0=cmd.split(":");
  if((f0[0]=="LIO")&&(f0.size()==2)) {
    int lio=f0[1].toUInt(&ok);
    if(ok) {
      lio_gpios=lio;
      for(unsigned i=0;i<lio_reset_timers.size();i++) {
	delete lio_reset_timers[i];
      }
      lio_reset_timers.clear();
      lio_reset_states.clear();
      lio_gpi_states.clear();
      for(int i=0;i<lio_gpios;i++) {
	lio_reset_timers.push_back(new QTimer(this));
	connect(lio_reset_timers.back(),SIGNAL(timeout()),
		lio_reset_mapper,SLOT(map()));
	lio_reset_mapper->setMapping(lio_reset_timers.back(),i);
	lio_reset_states.push_back(false);
	lio_gpi_states.push_back(false);
	CheckLineEntry(i+1);
	SendCommand(QString().sprintf("<LIOSUB:0.%d|LVL:1>",i));
      }
      sql=QString("update MATRICES set ")+
	QString().sprintf("GPIS=%d,GPOS=%d where ",lio_gpios,lio_gpios)+
	"(STATION_NAME=\""+RDEscapeString(stationName())+"\")&&"+
	QString().sprintf("(MATRIX=%d)",matrixNumber());
      q=new RDSqlQuery(sql);
      delete q;
      lio_watchdog_timer->start(WHEATNET_LIO_WATCHDOG_INTERVAL);
      lio_poll_timer->start(WHEATNET_LIO_POLL_INTERVAL);
    }
  }
  if((f0[0]=="BLID")&&(f0.size()==2)) {
    lio_watchdog_timer->stop();
    lio_watchdog_timer->start(WHEATNET_LIO_WATCHDOG_INTERVAL);
    lio_poll_timer->start(WHEATNET_LIO_POLL_INTERVAL);
  }
}


void WheatnetLio::ProcessLioevent(int chan,QString &cmd)
{
  //  printf("ProcessLioevent(%d,%s)\n",chan,(const char *)cmd);
  QStringList f0=cmd.split(":");
  if((f0[0]=="LVL")&&(f0.size()==2)) {
    if(chan<(int)lio_gpi_states.size()) {
      bool state=f0[1]=="1";
      if(state!=lio_gpi_states[chan]) {
	lio_gpi_states[chan]=state;
	emit gpiChanged(matrixNumber(),chan,state);
      }
    }
    else {
      rda->syslog(LOG_WARNING,
	     "WheatNet device at %s:%d sent invalid LIOEVENT LVL update [%s]",
		  lio_ip_address.toString().toUtf8().constData(),
		  lio_ip_port,cmd.toUtf8().constData());
    }
    if((chan+1)==lio_gpios) {
      lio_watchdog_timer->stop();
      lio_watchdog_timer->start(1000);
    }
  }
}


void WheatnetLio::ProcessCommand(const QString &cmd)
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
      if((f1[0]=="LIOEVENT")&&(f1.size()==2)) {
	QStringList f2=f1[1].split(".");
	if((f2[0]=="0")&&(f2.size()==2)) {
	  int chan=f2[1].toUInt(&ok);
	  if(ok) {
	    ProcessLioevent(chan,f0[1]);
	  }
	}
      }
    }
  }
}


void WheatnetLio::SendCommand(const QString &cmd)
{
  lio_socket->write((cmd+"\r\n").toUtf8());
}
