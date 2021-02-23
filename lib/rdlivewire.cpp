// rdlivewire.cpp
//
// A LiveWire Node Driver for Rivendell
//
//   (C) Copyright 2007-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <syslog.h>
#include <unistd.h>

#include <QApplication>
#include <QSignalMapper>

#include "rd.h"
#include "rdapplication.h"
#include "rdlivewire.h"
#include "rdsocketstrings.h"

AString::AString()
  : QString()
{
}


AString::AString(const AString &lhs)
  : QString(lhs)
{
}


AString::AString(const QString &lhs)
  : QString(lhs)
{
}


QStringList AString::split(const QString &sep,const QString &esc) const
{
  if(esc.isEmpty()) {
    return QString::split(sep);
  }
  QStringList list;
  bool escape=false;
  QChar e=esc.at(0);
  list.push_back(QString());
  for(int i=0;i<length();i++) {
    if(at(i)==e) {
      escape=!escape;
    }
    else {
      if((!escape)&&(mid(i,1)==sep)) {
	list.push_back(QString());
      }
      else {
	list.back()+=at(i);
      }
    }
  }
  return list;
  /*
  if(esc.isEmpty()) {
    return QStringList::split(sep,*this);
  }
  QStringList list;
  bool escape=false;
  QChar e=esc.at(0);
  list.push_back(QString());
  for(int i=0;i<length();i++) {
    if(at(i)==e) {
      escape=!escape;
    }
    else {
      if((!escape)&&(mid(i,1)==sep)) {
	list.push_back(QString());
      }
      else {
	list.back()+=at(i);
      }
    }
  }
  return list;
  */
}


RDLiveWire::RDLiveWire(unsigned id,QObject *parent)
  : QObject(parent)
{
  live_id=id;
  live_sources=0;
  live_destinations=0;
  live_channels=RD_LIVEWIRE_DEFAULT_CHANNELS;
  live_gpis=0;
  live_gpos=0;
  live_tcp_port=0;
  live_base_output=0;
  live_ptr=0;
  live_connected=false;
  live_load_ver_count=0;
  live_gpi_initialized=false;
  live_gpo_initialized=false;

  //
  // Connection Socket
  //
  live_socket=new QTcpSocket(this);
  connect(live_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(live_socket,SIGNAL(connectionClosed()),
	  this,SLOT(connectionClosedData()));
  connect(live_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(live_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));

  //
  // Watchdog Timers
  //
  live_watchdog_timer=new QTimer(this);
  live_watchdog_timer->setSingleShot(true);
  connect(live_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));

  live_watchdog_timeout_timer=new QTimer(this);
  connect(live_watchdog_timeout_timer,SIGNAL(timeout()),
  	  this,SLOT(watchdogTimeoutData()));

  live_holdoff_timer=new QTimer(this);
  live_holdoff_timer->setSingleShot(true);
  connect(live_holdoff_timer,SIGNAL(timeout()),this,SLOT(holdoffData()));
}


unsigned RDLiveWire::id() const
{
  return live_id;
}


QString RDLiveWire::hostname() const
{
  return live_hostname;
}


uint16_t RDLiveWire::tcpPort() const
{
  return live_tcp_port;
}


unsigned RDLiveWire::baseOutput()
{
  return live_base_output;
}


void RDLiveWire::connectToHost(const QString &hostname,uint16_t port,
			       const QString &passwd,unsigned base_output)
{
  live_hostname=hostname;
  live_tcp_port=port;
  live_password=passwd;
  live_base_output=base_output;
  live_socket->connectToHost(hostname,port);
}


bool RDLiveWire::loadSettings(const QString &hostname,uint16_t port,
			      const QString &passwd,unsigned base_output)
{
  int passes=50;

  live_load_ver_count=1;
  connectToHost(hostname,port,passwd,base_output);
  while(--passes>0) {
    usleep(100000);
    qApp->processEvents();
    if(live_load_ver_count==0) {
      return true;
    }
  }
  return false;
}


QString RDLiveWire::deviceName() const
{
  return live_device_name;
}


QString RDLiveWire::protocolVersion() const
{
  return live_protocol_version;
}


QString RDLiveWire::systemVersion() const
{
  return live_system_version;
}


int RDLiveWire::sources() const
{
  return live_sources;
}


int RDLiveWire::destinations() const
{
  return live_destinations;
}


int RDLiveWire::channels() const
{
  return live_channels;
}


int RDLiveWire::gpis() const
{
  return live_gpis;
}


int RDLiveWire::gpos() const
{
  return live_gpos;
}


unsigned RDLiveWire::gpiChannel(int slot,int line) const
{
  return live_gpi_channels[slot][line];
}


unsigned RDLiveWire::gpoChannel(int slot,int line) const
{
  return live_gpo_channels[slot][line];
}


bool RDLiveWire::gpiState(int slot,int line) const
{
  return live_gpi_states[slot][line];
}


bool RDLiveWire::gpoState(int slot,int line) const
{
  return live_gpo_states[slot][line];
}


void RDLiveWire::gpiSet(int slot,int line,unsigned interval)
{
  QString cmd=QString().sprintf("GPI %d ",slot+1);
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==line) {
      cmd+="l";
    }
    else {
      if(live_gpi_states[slot][i]) {
	cmd+="l";
      }
      else {
	cmd+="h";
      }
    }
  }
  cmd+="\"";
  SendCommand(cmd);
  live_gpi_states[slot][line]=true;
  if(interval>0) {
    live_gpi_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval);
  }
  emit gpiChanged(live_id,slot,line,true);
}


void RDLiveWire::gpiReset(int slot,int line,unsigned interval)
{
  QString cmd=QString().sprintf("GPI %d ",slot+1);
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==line) {
      cmd+="h";
    }
    else {
      if(live_gpi_states[slot][i]) {
	cmd+="l";
      }
      else {
	cmd+="h";
      }
    }
  }
  cmd+="\"";
  SendCommand(cmd);
  live_gpi_states[slot][line]=false;
  if(interval>0) {
    live_gpi_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval);
  }
  emit gpiChanged(live_id,slot,line,false);
}


void RDLiveWire::gpoSet(int slot,int line,unsigned interval)
{
  QString cmd=QString().sprintf("GPO %d ",slot+1);
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==line) {
      cmd+="l";
    }
    else {
      if(live_gpo_states[slot][i]) {
	cmd+="l";
      }
      else {
	cmd+="h";
      }
    }
  }
  SendCommand(cmd);
  live_gpo_states[slot][line]=true;
  if(interval>0) {
    live_gpo_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->start(interval);
  }
  emit gpoChanged(live_id,slot,line,true);
}


void RDLiveWire::gpoReset(int slot,int line,unsigned interval)
{
  QString cmd=QString().sprintf("GPO %d ",slot+1);
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==line) {
      cmd+="h";
    }
    else {
      if(live_gpo_states[slot][i]) {
	cmd+="l";
      }
      else {
	cmd+="h";
      }
    }
  }
  SendCommand(cmd);
  live_gpo_states[slot][line]=false;
  if(interval>0) {
    live_gpo_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval);
  }
  emit gpoChanged(live_id,slot,line,false);
}


void RDLiveWire::setRoute(int src_num,int dest_slot) const
{
  QString str;
  str=QString().sprintf("DST %d ADDR:\"239.192.%d.%d\"\r\n",
			dest_slot+1,src_num/256,src_num%256);
  SendCommand(str);
}


void RDLiveWire::connectedData()
{
  QString str="LOGIN";
  if(!live_password.isEmpty()) {
    str+=(" "+live_password);
  }
  SendCommand(str);
  SendCommand("VER");
}


void RDLiveWire::connectionClosedData()
{
  if(!live_watchdog_state) {
    live_watchdog_state=true;
    int holdoff=GetHoldoff();
    emit watchdogStateChanged(live_id,QString().sprintf(
       "connection to LiveWire node at %s:%d closed, attempting reconnect, holdoff = %d mS",
       live_hostname.toUtf8().constData(),live_tcp_port,holdoff));
    live_holdoff_timer->start(holdoff);
  }
}


void RDLiveWire::readyReadData()
{
  char buf[RD_LIVEWIRE_MAX_CMD_LENGTH];

  int n;

  while((n=live_socket->read(buf,RD_LIVEWIRE_MAX_CMD_LENGTH))>0) {
    buf[n]=0;
    for(int i=0;i<n;i++) {
      if(buf[i]=='\n') {
	live_buf[live_ptr]=0;
	DespatchCommand(live_buf);
	live_ptr=0;
      }
      else {
	if(buf[i]!='\r') {
	  live_buf[live_ptr++]=buf[i];
	}
      }
      if(live_ptr>=RD_LIVEWIRE_MAX_CMD_LENGTH) {
	fprintf(stderr,"LiveWire: status string truncated");
	live_ptr=0;
      }
    }
  }
}


void RDLiveWire::errorData(QAbstractSocket::SocketError err)
{
  int interval=RDLIVEWIRE_RECONNECT_MIN_INTERVAL;

  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    live_watchdog_state=true;
    interval=GetHoldoff();
    emit watchdogStateChanged(live_id,QString().sprintf(
      "connection to LiveWire node at %s:%d refused, attempting reconnect, holdoff = %d mS",
      live_hostname.toUtf8().constData(),
      live_tcp_port,interval));
    live_holdoff_timer->start(interval);
    break;

  default:
    rda->syslog(LOG_WARNING,
		"socket error on connection to LiveWire node at %s:%d: %s",
		live_hostname.toUtf8().constData(),
		live_tcp_port,
		RDSocketStrings(err).toUtf8().constData());
    break;
  }
}


void RDLiveWire::gpiTimeoutData(int id)
{
  int chan=id/RD_LIVEWIRE_GPIO_BUNDLE_SIZE;
  int line=id%RD_LIVEWIRE_GPIO_BUNDLE_SIZE;

  QString cmd=QString().sprintf("GPI %d ",chan+1);
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==line) {
      if(live_gpi_states[chan][i]) {
	cmd+="h";
      }
      else {
	cmd+="l";
      }
    }
    else {
      if(live_gpi_states[chan][i]) {
	cmd+="l";
      }
      else {
	cmd+="h";
      }
    }
  }
  cmd+="\"";
  SendCommand(cmd);
  live_gpi_states[chan][line]=!live_gpi_states[chan][line];
  emit gpiChanged(live_id,chan,line,live_gpi_states[chan][line]);
}


void RDLiveWire::gpoTimeoutData(int id)
{
  int chan=id/RD_LIVEWIRE_GPIO_BUNDLE_SIZE;
  int line=id%RD_LIVEWIRE_GPIO_BUNDLE_SIZE;

  QString cmd=QString().sprintf("GPO %d ",chan+1);
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==line) {
      if(live_gpo_states[chan][i]) {
	cmd+="h";
      }
      else {
	cmd+="l";
      }
    }
    else {
      if(live_gpo_states[chan][i]) {
	cmd+="l";
      }
      else {
	cmd+="h";
      }
    }
  }
  SendCommand(cmd);
  live_gpo_states[chan][line]=!live_gpo_states[chan][line];
  emit gpoChanged(live_id,chan,line,live_gpo_states[chan][line]);
}


void RDLiveWire::watchdogData()
{
  SendCommand("VER");
}


void RDLiveWire::watchdogTimeoutData()
{
  live_watchdog_state=true;
  live_connected=false;
  live_gpi_initialized=false;
  live_gpo_initialized=false;
  int holdoff=GetHoldoff();
  emit watchdogStateChanged(live_id,QString().sprintf(
	 "connection to LiveWire node at %s:%d lost, attempting reconnect, holdoff = %d mS",
	 live_hostname.toUtf8().constData(),live_tcp_port,holdoff));
  live_holdoff_timer->start(holdoff);
}


void RDLiveWire::holdoffData()
{
  ResetConnection();
}


void RDLiveWire::resetConnectionData()
{
  live_socket->close();
  connectToHost(live_hostname,live_tcp_port,live_password,live_base_output);
}


void RDLiveWire::DespatchCommand(const QString &cmd)
{
  int offset=cmd.indexOf(" ");
  QString opcode=cmd.left(offset);
  QString str;
  if(opcode=="VER") {
    ReadVersion(cmd.right(cmd.length()-offset-1));
  }
    
  if(opcode=="SET") {
  }

  if(opcode=="SRC") {
    ReadSources(cmd.right(cmd.length()-offset-1));
  }

  if(opcode=="DST") {
    ReadDestinations(cmd.right(cmd.length()-offset-1));
  }

  if(opcode=="GPO") {
    ReadGpos(cmd.right(cmd.length()-offset-1));
  }

  if(opcode=="GPI") {
    ReadGpis(cmd.right(cmd.length()-offset-1));
  }

  if(opcode=="CFG") {
    str=cmd.right(cmd.length()-offset-1);
    offset=str.indexOf(" ");
    if(str.left(offset)=="GPO") {
      ReadGpioConfig(str.right(str.length()-offset-1));
    }
  }
}


void RDLiveWire::SendCommand(const QString &cmd) const
{
  live_socket->write((cmd+"\r\n").toAscii(),cmd.length()+2);
}


void RDLiveWire::ReadVersion(const QString &cmd)
{
  QStringList f0;
  QStringList f1;

  if(!live_connected) {
    f0=AString(cmd).split(" ","\"");
    for(int i=0;i<f0.size();i++) {
      f1=f0.at(i).split(":",QString::KeepEmptyParts);
      if(f1.size()==2) {
	if(f1[0]=="LWRP") {
	  live_protocol_version=f1[1];
	}
	if(f1[0]=="DEVN") {
	  live_device_name=f1[1];
	}
	if(f1[0]=="SYSV") {
	  live_system_version=f1[1];
	}
	if(f1[0]=="NSRC") {
	  int delimiter=f1[1].indexOf("/");
	  if(delimiter<0) {
	    live_sources=f1[1].toInt();
	  }
	  else {
	    live_sources=f1[1].left(delimiter).toInt();
	    live_channels=f1[1].right(f1[1].length()-delimiter-1).toInt();
	  }
	  if(live_sources>0) {
	    SendCommand("SRC");
	  }
	}
	if(f1[0]=="NDST") {
	  int delimiter=f1[1].indexOf("/");
	  if(delimiter<0) {
	    live_destinations=f1[1].toInt();
	  }
	  else {
	    live_destinations=f1[1].left(delimiter).toInt();
	    live_channels=f1[1].right(f1[1].length()-delimiter-1).toInt();
	  }
	  if(live_destinations>0) {
	    SendCommand("DST");
	  }
	}
	if(f1[0]=="NGPI") {
	  live_gpis=f1[1].toInt();
	  QSignalMapper *mapper=new QSignalMapper(this);
	  connect(mapper,SIGNAL(mapped(int)),this,SLOT(gpiTimeoutData(int)));
	  for(int i=0;i<live_gpis;i++) {
	    live_gpi_states.push_back(new bool[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	    live_gpi_channels.
	      push_back(new unsigned[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	    for(int j=0;j<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;j++) {
	      live_gpi_states.back()[j]=false;
	      live_gpi_channels.back()[j]=i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j;
	      live_gpi_timers.push_back(new QTimer(this));
	      live_gpi_timers.back()->setSingleShot(true);
	      mapper->setMapping(live_gpi_timers.back(),
				 i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j);
	      connect(live_gpi_timers.back(),SIGNAL(timeout()),mapper,SLOT(map()));
	    }	
	  }
	  if(!live_gpi_initialized) {
	    if(live_gpis>0) {
	      SendCommand("ADD GPI");
	    }
	    live_gpi_initialized=true;
	  }
	}
	if(f1[0]=="NGPO") {
	  live_gpos=f1[1].toInt();
	  QSignalMapper *mapper=new QSignalMapper(this);
	  connect(mapper,SIGNAL(mapped(int)),this,SLOT(gpoTimeoutData(int)));
	  for(int i=0;i<live_gpos;i++) {
	    live_gpo_states.push_back(new bool[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	    live_gpo_channels.
	      push_back(new unsigned[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	    for(int j=0;j<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;j++) {
	      live_gpo_states.back()[j]=false;
	      live_gpo_channels.back()[j]=i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j;
	      live_gpo_timers.push_back(new QTimer(this));
	      live_gpo_timers.back()->setSingleShot(true);
	      mapper->setMapping(live_gpo_timers.back(),
				 i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j);
	      connect(live_gpo_timers.back(),SIGNAL(timeout()),mapper,SLOT(map()));
	    }
	  }
	  if(!live_gpo_initialized) {
	    if(live_gpos>0) {
	      SendCommand("CFG GPO");
	      SendCommand("ADD GPO");
	    }
	    live_gpo_initialized=true;
	  }
	}
      }
    }
    live_connected=true;
    emit connected(live_id);
  }
  if(live_load_ver_count>0) {
    live_load_ver_count--;
  }
  if(live_watchdog_state) {
    live_watchdog_state=false;
    emit watchdogStateChanged(live_id,QString().sprintf(
	    "connection to LiveWire node at %s:%d restored",
	    live_hostname.toUtf8().constData(),live_tcp_port));
  }
  live_watchdog_timer->start(RDLIVEWIRE_WATCHDOG_INTERVAL);
  live_watchdog_timeout_timer->stop();
  live_watchdog_timeout_timer->start(RDLIVEWIRE_WATCHDOG_TIMEOUT);
}


void RDLiveWire::ReadSources(const QString &cmd)
{
  QHostAddress addr;
  QStringList f1;
  RDLiveWireSource *src=new RDLiveWireSource();
  QStringList f0=AString(cmd).split(" ","\"");
  src->setSlotNumber(f0[0].toInt());
  for(int i=1;i<f0.size();i++) {
    f1=f0.at(i).split(":",QString::KeepEmptyParts);
    if(f1.size()==2) {
      if(f1[0]=="PSNM") {
	src->setPrimaryName(f1[1]);
      }
      if(f1[0]=="LABL") {
	src->setLabelName(f1[1]);
      }
      if(f1[0]=="FASM") {
	// ????
      }
      if(f1[0]=="RTPE") {
	src->setRtpEnabled(f1[1].toInt());
      }
      if(f1[0]=="RTPA") {
	addr.setAddress(f1[1]);
	src->setStreamAddress(addr);
      }
      if(f1[0]=="INGN") {
	src->setInputGain(f1[1].toInt());
      }
      if(f1[0]=="SHAB") {
	src->setShareable(f1[1].toInt());
      }
      if(f1[0]=="NCHN") {
	src->setChannels(f1[1].toInt());
      }
      if(f1[0]=="RTPP") {
	// ????
      }
    }
  }
  emit sourceChanged(live_id,src);
  delete src;
}


void RDLiveWire::ReadDestinations(const QString &cmd)
{
  QHostAddress addr;
  QStringList f1;
  RDLiveWireDestination *dst=new RDLiveWireDestination();
  QStringList f0=AString(cmd).split(" ","\"");
  dst->setSlotNumber(f0[0].toInt());
  for(int i=1;i<f0.size();i++) {
    f1=f0.at(i).split(":",QString::KeepEmptyParts);
    if(f1.size()==2) {
      if(f1[0]=="NAME") {
	dst->setPrimaryName(f1[1]);
      }
      if(f1[0]=="ADDR") {
	addr.setAddress(f1[1]);
	dst->setStreamAddress(addr);
      }
      if(f1[0]=="NCHN") {
	dst->setChannels(f1[1].toInt());
      }
      if(f1[0]=="LOAD") {
	dst->setOutputGain((RDLiveWireDestination::Load)f1[1].toInt());
      }
      if(f1[0]=="OUGN") {
	dst->setOutputGain(f1[1].toInt());
      }
    }
  }
  emit destinationChanged(live_id,dst);
  delete dst;
}


void RDLiveWire::ReadGpis(const QString &cmd)
{
  //
  // FIXME: This is currently emitting the relative slot number, which is 
  //        wrong.  How do we get the associated source number?
  //

//  printf("GPI: %s\n",(const char *)cmd);

  int offset=cmd.indexOf(" ");
  int slot=cmd.left(offset).toInt()-1;
  QString str=cmd.right(cmd.length()-offset-1).toLower();
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if((str[i]=='h')&&live_gpi_states[slot][i]) {
      live_gpi_states[slot][i]=false;
      emit gpiChanged(live_id,slot,i,false);
    }
    if((str[i]=='l')&&(!live_gpi_states[slot][i])) {
      live_gpi_states[slot][i]=true;
      emit gpiChanged(live_id,slot,i,true);
    }
  }
}


void RDLiveWire::ReadGpos(const QString &cmd)
{
  //
  // FIXME: This is currently emitting the relative slot number, which is 
  //        wrong.  How do we get the associated source number?
  //

//  printf("GPO: %s\n",(const char *)cmd);

  int offset=cmd.indexOf(" ");
  int slot=cmd.left(offset).toInt()-1;
  QString str=cmd.right(cmd.length()-offset-1).toLower();
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if((str.mid(i,1)=="h")&&live_gpo_states[slot][i]) {
      live_gpo_states[slot][i]=false;
      emit gpoChanged(live_id,slot,i,false);
    }
      if((str.mid(i,1)=="l")&&(!live_gpo_states[slot][i])) {
      live_gpo_states[slot][i]=true;
      emit gpoChanged(live_id,slot,i,true);
    }
  }
}


void RDLiveWire::ReadGpioConfig(const QString &cmd)
{
  QStringList f0;
  QStringList f1;

  f0=AString(cmd).split(" ","\"");
  int slot=f0[0].toInt()-1;
  for(int i=1;i<f0.size();i++) {
    f1=f0.at(i).split(":",QString::KeepEmptyParts);
    if(f1.size()==2) {
      if(f1[0]=="SRCA") {
	int chan=PruneUrl(f1[1]).toInt();
	for(unsigned i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
	  live_gpi_channels[slot][i]=chan*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+i;
	  live_gpo_channels[slot][i]=chan*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+i;
	  emit gpoConfigChanged(live_id,slot,live_gpo_channels[slot][i]);
	}
      }
    }
  }
}


QString RDLiveWire::PruneUrl(const QString &str)
{
  QString ret=str;
  int offset=str.indexOf("<");
  if(offset>=0) {
    ret=str.left(offset);
  }
  return ret;
}


void RDLiveWire::ResetConnection()
{
  live_socket->close();
  connectToHost(live_hostname,live_tcp_port,live_password,live_base_output);
}


int RDLiveWire::GetHoldoff()
{
  return (int)(RDLIVEWIRE_RECONNECT_MIN_INTERVAL+
	       (RDLIVEWIRE_RECONNECT_MAX_INTERVAL-
		RDLIVEWIRE_RECONNECT_MIN_INTERVAL)*
	       (double)random()/(double)RAND_MAX);
}
