// rdlivewire.cpp
//
// A LiveWire Node Driver for Rivendell
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlivewire.cpp,v 1.7.8.2 2013/11/17 02:03:19 cvs Exp $
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

#include <qapplication.h>
#include <qsignalmapper.h>

#include <rd.h>
#include <rdlivewire.h>


RDLiveWire::RDLiveWire(unsigned id,QObject *parent,const char *name)
  : QObject(parent,name)
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

  //
  // Connection Socket
  //
  live_socket=new QSocket(this,"live_socket");
  connect(live_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(live_socket,SIGNAL(connectionClosed()),
	  this,SLOT(connectionClosedData()));
  connect(live_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(live_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));

  //
  // Watchdog Timers
  //
  live_watchdog_timer=new QTimer(this,",live_watchdog_timer");
  connect(live_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));

  live_watchdog_timeout_timer=new QTimer(this,",live_watchdog_timeout_timer");
  connect(live_watchdog_timeout_timer,SIGNAL(timeout()),
	  this,SLOT(watchdogTimeoutData()));

  live_holdoff_timer=new QTimer(this,",live_holdoff_timer");
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


Q_UINT16 RDLiveWire::tcpPort() const
{
  return live_tcp_port;
}


unsigned RDLiveWire::baseOutput()
{
  return live_base_output;
}


void RDLiveWire::connectToHost(const QString &hostname,Q_UINT16 port,
			       const QString &passwd,unsigned base_output)
{
  live_hostname=hostname;
  live_tcp_port=port;
  live_password=passwd;
  live_base_output=base_output;
  live_socket->connectToHost(hostname,port);
}


bool RDLiveWire::loadSettings(const QString &hostname,Q_UINT16 port,
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
  cmd+="\"\r\n";
  live_socket->writeBlock(cmd,cmd.length());
  live_gpi_states[slot][line]=true;
  if(interval>0) {
    live_gpi_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval,true);
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
  cmd+="\"\r\n";
  live_socket->writeBlock(cmd,cmd.length());
  live_gpi_states[slot][line]=false;
  if(interval>0) {
    live_gpi_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval,true);
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
  cmd+="\r\n";
  live_socket->writeBlock(cmd,cmd.length());
  live_gpo_states[slot][line]=true;
  if(interval>0) {
    live_gpo_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval,true);
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
  cmd+="\r\n";
  live_socket->writeBlock(cmd,cmd.length());
  live_gpo_states[slot][line]=false;
  if(interval>0) {
    live_gpo_timers[slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line]->
      start(interval,true);
  }
  emit gpoChanged(live_id,slot,line,false);
}


void RDLiveWire::setRoute(int src_num,int dest_slot) const
{
  QString str;
  str=QString().sprintf("DST %d ADDR:\"239.192.%d.%d\"\r\n",
			dest_slot+1,src_num/256,src_num%256);
  live_socket->writeBlock(str,str.length());
}


void RDLiveWire::connectedData()
{
  QString str="LOGIN";
  if(!live_password.isEmpty()) {
    str+=(" "+live_password);
  }
  str+="\r\n";
  live_socket->writeBlock(str,str.length());
  live_socket->writeBlock("VER\r\n",5);
}


void RDLiveWire::connectionClosedData()
{
  if(!live_watchdog_state) {
    live_watchdog_state=true;
    int holdoff=GetHoldoff();
    emit watchdogStateChanged(live_id,QString().sprintf(
       "Connection to LiveWire node at %s:%d closed, attempting reconnect, holdoff = %d mS",
       (const char *)live_hostname,live_tcp_port,holdoff));
    live_holdoff_timer->start(holdoff,true);
  }
}


void RDLiveWire::readyReadData()
{
  char buf[RD_LIVEWIRE_MAX_CMD_LENGTH];

  int n=live_socket->readBlock(buf,RD_LIVEWIRE_MAX_CMD_LENGTH);
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


void RDLiveWire::errorData(int err)
{
  int interval=RDLIVEWIRE_RECONNECT_MIN_INTERVAL;

  switch((QSocket::Error)err) {
      case QSocket::ErrConnectionRefused:
	live_watchdog_state=true;
	interval=GetHoldoff();
	emit watchdogStateChanged(live_id,QString().sprintf(
	 "Connection to LiveWire node at %s:%d refused, attempting reconnect, holdoff = %d mS",
				  (const char *)live_hostname,
				  live_tcp_port,interval));
	live_holdoff_timer->start(interval,true);
	break;

      case QSocket::ErrHostNotFound:
	emit watchdogStateChanged(live_id,QString().sprintf(
	  "Error on connection to LiveWire node at %s:%d: Host Not Found",
				  (const char *)live_hostname,
				  live_tcp_port));
	break;

      case QSocket::ErrSocketRead:
	emit watchdogStateChanged(live_id,QString().sprintf(
	  "Error on connection to LiveWire node at %s:%d: Socket Read Error",
				  (const char *)live_hostname,
				  live_tcp_port));
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
  cmd+="\"\r\n";
  live_socket->writeBlock(cmd,cmd.length());
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
  cmd+="\r\n";
  live_socket->writeBlock(cmd,cmd.length());
  live_gpo_states[chan][line]=!live_gpo_states[chan][line];
  emit gpoChanged(live_id,chan,line,live_gpo_states[chan][line]);
}


void RDLiveWire::watchdogData()
{
  live_socket->writeBlock("VER\r\n",4);
}


void RDLiveWire::watchdogTimeoutData()
{
  live_watchdog_state=true;
  int holdoff=GetHoldoff();
  emit watchdogStateChanged(live_id,QString().sprintf(
	 "Connection to LiveWire node at %s:%d lost, attempting reconnect, holdoff = %d mS",
	 (const char *)live_hostname,live_tcp_port,holdoff));
  live_holdoff_timer->start(holdoff,true);
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
  int offset=cmd.find(" ");
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
    offset=str.find(" ");
    if(str.left(offset)=="GPO") {
      ReadGpioConfig(str.right(str.length()-offset-1));
    }
  }
}


void RDLiveWire::ReadVersion(const QString &cmd)
{
  int ptr=0;
  QString tag;
  QString value;
  if(!live_connected) {
    while((ptr=ParseString(cmd,ptr,&tag,&value))>=0) {
      if(tag=="LWRP") {
	live_protocol_version=value;
      }
      if(tag=="DEVN") {
	live_device_name=value;
      }
      if(tag=="SYSV") {
	live_system_version=value;
      }
      if(tag=="NSRC") {
	int delimiter=value.find("/");
	if(delimiter<0) {
	  live_sources=value.toInt();
	}
	else {
	  live_sources=value.left(delimiter).toInt();
	  live_channels=value.right(value.length()-delimiter-1).toInt();
	}
	if(live_sources>0) {
	  live_socket->writeBlock("SRC\r\n",5);
	}
      }
      if(tag=="NDST") {
	int delimiter=value.find("/");
	if(delimiter<0) {
	  live_destinations=value.toInt();
	}
	else {
	  live_destinations=value.left(delimiter).toInt();
	  live_channels=value.right(value.length()-delimiter-1).toInt();
	}
	if(live_destinations>0) {
	  live_socket->writeBlock("DST\r\n",5);
	}
      }
      if(tag=="NGPI") {
	live_gpis=value.toInt();
	QSignalMapper *mapper=new QSignalMapper(this,"gpi_mapper");
	connect(mapper,SIGNAL(mapped(int)),this,SLOT(gpiTimeoutData(int)));
	for(int i=0;i<live_gpis;i++) {
	  live_gpi_states.push_back(new bool[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	  live_gpi_channels.
	    push_back(new unsigned[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	  for(int j=0;j<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;j++) {
	    live_gpi_states.back()[j]=false;
	    live_gpi_channels.back()[j]=i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j;
	    live_gpi_timers.push_back(new QTimer(this));
	    mapper->setMapping(live_gpi_timers.back(),
			       i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j);
	    connect(live_gpi_timers.back(),SIGNAL(timeout()),mapper,SLOT(map()));
	  }	
	}
	if(live_gpis>0) {
	  live_socket->writeBlock("ADD GPI\r\n",9);
	}
      }
      if(tag=="NGPO") {
	live_gpos=value.toInt();
	QSignalMapper *mapper=new QSignalMapper(this,"gpo_mapper");
	connect(mapper,SIGNAL(mapped(int)),this,SLOT(gpoTimeoutData(int)));
	for(int i=0;i<live_gpos;i++) {
	  live_gpo_states.push_back(new bool[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	  live_gpo_channels.
	    push_back(new unsigned[RD_LIVEWIRE_GPIO_BUNDLE_SIZE]);
	  for(int j=0;j<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;j++) {
	    live_gpo_states.back()[j]=false;
	    live_gpo_channels.back()[j]=i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j;
	    live_gpo_timers.push_back(new QTimer(this));
	    mapper->setMapping(live_gpo_timers.back(),
			       i*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+j);
	    connect(live_gpo_timers.back(),SIGNAL(timeout()),mapper,SLOT(map()));
	  }
	}
	if(live_gpos>0) {
	  live_socket->writeBlock("CFG GPO\r\n",9);
	  live_socket->writeBlock("ADD GPO\r\n",9);
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
	    "Connection to LiveWire node at %s:%d restored",
	 (const char *)live_hostname,live_tcp_port));
  }
  live_watchdog_timer->start(RDLIVEWIRE_WATCHDOG_INTERVAL,true);
  live_watchdog_timeout_timer->stop();
  live_watchdog_timeout_timer->start(RDLIVEWIRE_WATCHDOG_TIMEOUT,true);
}


void RDLiveWire::ReadSources(const QString &cmd)
{
//  printf("SOURCES: %s\n",(const char *)cmd);
  int ptr=0;
  QString tag;
  QString value;
  RDLiveWireSource *src=new RDLiveWireSource();
  QHostAddress addr;

  int offset=cmd.find(" ");
  src->setSlotNumber(cmd.left(offset).toInt());
  ptr=offset+1;
  while((ptr=ParseString(cmd,ptr,&tag,&value))>=0) {
    if(tag=="PSNM") {
      src->setPrimaryName(value);
    }
    if(tag=="LABL") {
      src->setLabelName(value);
    }
    if(tag=="FASM") {
      // ????
    }
    if(tag=="RTPE") {
      src->setRtpEnabled(value.toInt());
    }
    if(tag=="RTPA") {
      addr.setAddress(value);
      src->setStreamAddress(addr);
    }
    if(tag=="INGN") {
      src->setInputGain(value.toInt());
    }
    if(tag=="SHAB") {
      src->setShareable(value.toInt());
    }
    if(tag=="NCHN") {
      src->setChannels(value.toInt());
    }
    if(tag=="RTPP") {
      // ????
    }
  }
  emit sourceChanged(live_id,src);
  delete src;
}


void RDLiveWire::ReadDestinations(const QString &cmd)
{
  int ptr=0;
  QString tag;
  QString value;
  RDLiveWireDestination *dst=new RDLiveWireDestination();
  QHostAddress addr;

  int offset=cmd.find(" ");
  dst->setSlotNumber(cmd.left(offset).toInt());
  ptr=offset+1;
  while((ptr=ParseString(cmd,ptr,&tag,&value))>=0) {
    if(tag=="NAME") {
      dst->setPrimaryName(value);
    }
    if(tag=="ADDR") {
      addr.setAddress(value);
      dst->setStreamAddress(addr);
    }
    if(tag=="NCHN") {
      dst->setChannels(value.toInt());
    }
    if(tag=="LOAD") {
      dst->setLoad((RDLiveWireDestination::Load)value.toInt());
    }
    if(tag=="OUGN") {
      dst->setOutputGain(value.toInt());
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

  int offset=cmd.find(" ");
  int slot=cmd.left(offset).toInt()-1;
  QString str=cmd.right(cmd.length()-offset-1).lower();
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

  int offset=cmd.find(" ");
  int slot=cmd.left(offset).toInt()-1;
  QString str=cmd.right(cmd.length()-offset-1).lower();
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
  // printf("GpioConfig: %s\n",(const char *)cmd);
  int ptr=0;
  QString tag;
  QString value;
  int offset=cmd.find(" ");
  int slot=cmd.left(offset).toInt()-1;
  QString str=cmd.right(cmd.length()-offset-1).lower();
  ptr=offset+1;
  while((ptr=ParseString(cmd,ptr,&tag,&value))>=0) {
    if(tag=="SRCA") {
      int chan=PruneUrl(value).toInt();
      for(unsigned i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
	live_gpi_channels[slot][i]=chan*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+i;
	live_gpo_channels[slot][i]=chan*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+i;
	emit gpoConfigChanged(live_id,slot,live_gpo_channels[slot][i]);
      }
    }
  }
}


int RDLiveWire::ParseString(const QString &str,int ptr,
			    QString *tag,QString *value) const
{
  int len=(int)str.length();
  bool quote_mode=false;

  if(ptr>=len) {
    return -1;
  }
  *tag="";
  *value="";
  //
  // Get Tag
  //
  for(int i=ptr;i<len;i++) {
    if(str[i]==':') {
      ptr=i+1;
      i=len;
    }
    else {
      (*tag)+=str[i];
    }
  }

  //
  // Get Value
  //
  for(int i=ptr;i<len;i++) {
    if(str[i]=='\"') {
      quote_mode=!quote_mode;
    }
    else {
      if((str[i]==' ')&&(!quote_mode)) {
	ptr=i+1;
	return ptr;
      }
      else {
	(*value)+=str[i];
      }
    }
    if(i==(len-1)) {
      ptr=i+1;
      return ptr;
    }
  }

  return ptr;
}


QString RDLiveWire::PruneUrl(const QString &str)
{
  QString ret=str;
  int offset=str.find("<");
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
