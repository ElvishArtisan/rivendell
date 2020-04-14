// swauthority.cpp
//
// A Rivendell switcher driver for systems using Software Authority Protocol
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qstringlist.h>

#include <rdapplication.h>
#include <rdescape_string.h>

#include "globals.h"
#include "swauthority.h"

SoftwareAuthority::SoftwareAuthority(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  swa_matrix=matrix->matrix();
  swa_ptr=0;
  swa_istate=0;

  //
  // Get Matrix Parameters
  //
  swa_username=matrix->username(RDMatrix::Primary);
  swa_password=matrix->password(RDMatrix::Primary);
  swa_porttype=matrix->portType(RDMatrix::Primary);
  swa_ipaddress=matrix->ipAddress(RDMatrix::Primary);
  swa_ipport=matrix->ipPort(RDMatrix::Primary);
  swa_card=matrix->card();
  swa_inputs=0;
  swa_outputs=0;
  swa_gpis=0;
  swa_gpos=0;
  swa_start_cart=matrix->startCart(RDMatrix::Primary);
  swa_stop_cart=matrix->stopCart(RDMatrix::Primary);
  swa_is_gpio=false;

  rda->syslog(LOG_DEBUG,"%p - card: %d\n",this,swa_card);

  //
  // Reconnection Timer
  //
  swa_reconnect_timer=new QTimer(this);
  connect(swa_reconnect_timer,SIGNAL(timeout()),this,SLOT(ipConnect()));

  //
  // Initialize the connection
  //
  swa_socket=new QTcpSocket(this);
  connect(swa_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(swa_socket,SIGNAL(connectionClosed()),
	  this,SLOT(connectionClosedData()));
  connect(swa_socket,SIGNAL(readyRead()),
	  this,SLOT(readyReadData()));
  connect(swa_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  ipConnect();
}


RDMatrix::Type SoftwareAuthority::type()
{
  return RDMatrix::SoftwareAuthority;
}


unsigned SoftwareAuthority::gpiQuantity()
{
  return swa_gpis;
}


unsigned SoftwareAuthority::gpoQuantity()
{
  return swa_gpos;
}


bool SoftwareAuthority::primaryTtyActive()
{
  return false;
}


bool SoftwareAuthority::secondaryTtyActive()
{
  return false;
}


void SoftwareAuthority::processCommand(RDMacro *cmd)
{
  QString str;

  switch(cmd->command()) {
      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>swa_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>swa_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	SendCommand(QString().sprintf("activateroute %d %d %d",swa_card,
				      cmd->arg(2).toInt(),cmd->arg(1).toInt()));
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::GO:
	if(((cmd->arg(1).lower()!="i")&&
	    (cmd->arg(1).lower()!="o"))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>swa_gpos)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).lower()=="i") {
	  str="triggergpi";
	}
	else {
	  str="triggergpo";
	}
	str+=QString().sprintf(" %d",swa_card);
	str+=QString().sprintf(" %d ",
		      1+(cmd->arg(2).toInt()-1)/RD_LIVEWIRE_GPIO_BUNDLE_SIZE);
	str+=BundleString((cmd->arg(2).toInt()-1)%RD_LIVEWIRE_GPIO_BUNDLE_SIZE,
			  cmd->arg(3).toInt()!=0);
	if(cmd->arg(4).toInt()>0) {   // Momentary
	  str+=QString().sprintf(" %d",cmd->arg(4).toInt());
	}
	SendCommand(str);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void SoftwareAuthority::ipConnect()
{
  rda->syslog(LOG_DEBUG,"%p - connecting to %s:%d",this,
	      swa_ipaddress.toString().toUtf8().constData(),swa_ipport);
  swa_socket->connectToHost(swa_ipaddress.toString(),swa_ipport);
}


void SoftwareAuthority::connectedData()
{
  SendCommand(QString("login")+" "+swa_username+" "+swa_password);
}


void SoftwareAuthority::connectionClosedData()
{
  rda->syslog(LOG_WARNING,
	 "connection to SoftwareAuthority device at %s:%d closed unexpectedly, attempting reconnect",
	      (const char *)swa_ipaddress.toString().toUtf8(),
	      swa_ipport);
  if(swa_stop_cart>0) {
    ExecuteMacroCart(swa_stop_cart);
  }
  swa_reconnect_timer->start(SWAUTHORITY_RECONNECT_INTERVAL,true);
}


void SoftwareAuthority::readyReadData()
{
  char buffer[256];
  unsigned n;

  while((n=swa_socket->readBlock(buffer,255))>0) {
    buffer[n]=0;
    ///    printf("RECV: %s\n",buffer);
    for(unsigned i=0;i<n;i++) {
      if(buffer[i]==10) {  // End of line
	swa_buffer[--swa_ptr]=0;
	DispatchCommand();
	swa_ptr=0;
      }
      else {
	if(swa_ptr==SWAUTHORITY_MAX_LENGTH) {  // Buffer overflow
	  swa_ptr=0;
	}
	swa_buffer[swa_ptr++]=buffer[i];
      }
    }
  }
}


void SoftwareAuthority::errorData(QAbstractSocket::SocketError err)
{
  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    rda->syslog(LOG_WARNING,
		"connection to SoftwareAuthority device at %s:%d refused, attempting reconnect",
		(const char *)swa_ipaddress.toString().toUtf8(),
		swa_ipport);
    swa_reconnect_timer->start(SWAUTHORITY_RECONNECT_INTERVAL,true);
    break;

  case QAbstractSocket::HostNotFoundError:
    rda->syslog(LOG_WARNING,
		"error on connection to SoftwareAuthority device at %s:%d: Host Not Found",
		(const char *)swa_ipaddress.toString().toUtf8(),
		swa_ipport);
    break;

  default:
    rda->syslog(LOG_WARNING,
		"error %d on connection to SoftwareAuthority device at %s:%d",
		err,
		(const char *)swa_ipaddress.toString().toUtf8(),
		swa_ipport);
    break;
  }
}


void SoftwareAuthority::SendCommand(const char *str)
{
  //  LogLine(RDConfig::LogDebug,QString().sprintf("sending SA cmd: %s",(const char *)PrettifyCommand(str)));
  rda->syslog(LOG_DEBUG,"%p - sending \"%s\"",this,str);
  QString cmd=QString().sprintf("%s\x0d\x0a",(const char *)str);
  swa_socket->writeBlock((const char *)cmd,strlen(cmd));
}


void SoftwareAuthority::DispatchCommand()
{
  char buffer[SWAUTHORITY_MAX_LENGTH];
  QString cmd;
  QString label;
  QString sql;
  RDSqlQuery *q;
  QStringList f0;
  QString name;

  //  LogLine(RDConfig::LogNotice,QString().sprintf("RECEIVED: %s",(const char *)swa_buffer));

  QString line_in=swa_buffer;
  QString section=line_in.lower().replace(">>","");
  rda->syslog(LOG_DEBUG,"%p - received \"%s\"",this,
	      section.toUtf8().constData());

  //
  // Startup Sequence. Get initial GPIO states and the input and output lists.
  //
  if(section=="login successful") {
    swa_inputs=0;
    swa_outputs=0;
    swa_gpis=0;
    swa_gpos=0;
    swa_is_gpio=false;
    sprintf(buffer,"gpistat %d\x0D\x0A",swa_card);      // Request GPI States
    SendCommand(buffer);
    sprintf(buffer,"gpostat %d\x0D\x0A",swa_card);      // Request GPO States
    SendCommand(buffer);
    sprintf(buffer,"sourcenames %d\x0D\x0A",swa_card);  // Request Input List
    SendCommand(buffer);
    sprintf(buffer,"destnames %d\x0D\x0A",swa_card);    // Request Output List
    SendCommand(buffer);
    return;
  }
  if(section=="login failure") {
    rda->syslog(LOG_WARNING,
	   "error on connection to SoftwareAuthority device at %s:%d: Login Failure",
		(const char *)swa_ipaddress.toString().toUtf8(),
		swa_ipport);
    swa_socket->close();
    return;
  }

  switch(swa_istate) {
  case 0:   // No section selected
    if(section==QString().sprintf("begin sourcenames - %d",swa_card)) {
      swa_istate=1;
      swa_inputs=0;
      return;
    }
    if(section==QString().sprintf("begin destnames - %d",swa_card)) {
      swa_istate=2;
      swa_outputs=0;
      return;
    }
    break;

  case 1:   // Source List
    if(section==QString().sprintf("end sourcenames - %d",swa_card)) {
      //
      // Write Sources Data
      //
      swa_istate=0;
      if(swa_is_gpio) {
	swa_gpis=swa_inputs*RD_LIVEWIRE_GPIO_BUNDLE_SIZE;
      }
      sql=QString("update MATRICES set ")+
	QString().sprintf("INPUTS=%d,",swa_inputs)+
	QString().sprintf("GPIS=%d ",swa_gpis)+
	"where (STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
	QString().sprintf("(MATRIX=%d)",swa_matrix);
      q=new RDSqlQuery(sql);
      delete q;
      return;
    }
    f0=line_in.split("\t",QString::KeepEmptyParts);
    name=f0[1];
    if(f0.size()>=7) {
      name=f0[6]+": "+f0[2];
    }
    sql=QString("select NUMBER from INPUTS where ")+
      "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
      QString().sprintf("(MATRIX=%d)&&",swa_matrix)+
      QString().sprintf("(NUMBER=%d)",f0[0].toInt());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("update INPUTS set ")+
	"NAME=\""+RDEscapeString(name)+"\" where "+
	"(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
	QString().sprintf("(MATRIX=%d)&&",swa_matrix)+
	QString().sprintf("(NUMBER=%d)",f0[0].toInt());
    }
    else {
      sql=QString("insert into INPUTS set ")+
	"NAME=\""+RDEscapeString(name)+"\","+
	"STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\","+
	QString().sprintf("MATRIX=%d,",swa_matrix)+
	QString().sprintf("NUMBER=%d",f0[0].toInt());
    }
    if(f0[0].toInt()>swa_inputs) {
      swa_inputs=f0[0].toInt();
    }
    delete q;
    q=new RDSqlQuery(sql);
    delete q;
    break;

  case 2:   // Destinations List
    if(section==QString().sprintf("end destnames - %d",swa_card)) {
      //
      // Write Destinations Data
      //
      swa_istate=0;
      if(swa_is_gpio) {
	swa_gpos=swa_outputs*RD_LIVEWIRE_GPIO_BUNDLE_SIZE;
      }
      sql=QString("update MATRICES set ")+
	QString().sprintf("OUTPUTS=%d,",swa_outputs)+
	QString().sprintf("GPOS=%d ",swa_gpos)+
	"where (STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
	QString().sprintf("(MATRIX=%d)",swa_matrix);
      q=new RDSqlQuery(sql);
      delete q;

      rda->syslog(LOG_INFO,
		  "connection to SoftwareAuthority device at %s:%d established",
		  (const char *)swa_ipaddress.toString().toUtf8(),
		  swa_ipport);
      if(swa_start_cart>0) {
	ExecuteMacroCart(swa_start_cart);
      }
      return;
    }
    f0=line_in.split("\t",QString::KeepEmptyParts);
    name=f0[1];
    if(f0.size()>=6) {
      name=f0[3]+"/"+f0[5]+": "+f0[2];
    }
    sql=QString("select NUMBER from OUTPUTS where ")+
      "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
      QString().sprintf("(MATRIX=%d)&&",swa_matrix)+
      QString().sprintf("(NUMBER=%d)",f0[0].toInt());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("update OUTPUTS set ")+
	"NAME=\""+RDEscapeString(name)+"\" where "+
	"(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
	QString().sprintf("(MATRIX=%d)&&",swa_matrix)+
	QString().sprintf("(NUMBER=%d)",f0[0].toInt());
    }
    else {
      sql=QString("insert into OUTPUTS set ")+
	"NAME=\""+RDEscapeString(name)+"\","+
	"STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\","+
	QString().sprintf("MATRIX=%d,",swa_matrix)+
	QString().sprintf("NUMBER=%d",f0[0].toInt());
    }
    if(f0[0].toInt()>swa_outputs) {
      swa_outputs=f0[0].toInt();
    }
    delete q;
    q=new RDSqlQuery(sql);
    delete q;
    break;
  }

  //
  // GPIO State Parser
  //
  f0=section.split(" ");
  if((f0.size()==4)&&(f0[0].lower()=="gpistat")&&(f0[1].toInt()==swa_card)) {
    if(swa_gpi_states[f0[2].toInt()].isEmpty()) {
      swa_gpi_states[f0[2].toInt()]=f0[3];
    }
    else {
      for(unsigned i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
	int gpi=(f0[2].toInt()-1)*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+i;
	if(f0[3].at(i)!=swa_gpi_states[f0[2].toInt()].at(i)) {
	  emit gpiChanged(swa_matrix,gpi,f0[3].at(i)=='l');
	}
      }
      swa_gpi_states[f0[2].toInt()]=f0[3];
    }
    swa_is_gpio=true;
  }
  if((f0.size()==4)&&(f0[0].lower()=="gpostat")&&(f0[1].toInt()==swa_card)) {
    if(swa_gpo_states[f0[2].toInt()].isEmpty()) {
      swa_gpo_states[f0[2].toInt()]=f0[3];
    }
    else {
      for(unsigned i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
	int gpo=(f0[2].toInt()-1)*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+i;
	if(f0[3].at(i)!=swa_gpo_states[f0[2].toInt()].at(i)) {
	  emit gpoChanged(swa_matrix,gpo,f0[3].at(i)=='l');
	}
      }
      swa_gpo_states[f0[2].toInt()]=f0[3];
    }
    swa_is_gpio=true;
  }
}


void SoftwareAuthority::ExecuteMacroCart(unsigned cartnum)
{
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  rml.setCommand(RDMacro::EX);
  rml.setAddress(rda->station()->address());
  rml.setEchoRequested(false);
  rml.addArg(cartnum);
  emit rmlEcho(&rml);
}


QString SoftwareAuthority::PrettifyCommand(const char *cmd) const
{
  QString ret;
  if(cmd[0]<26) {
    ret=QString().sprintf("^%c%s",'@'+cmd[0],cmd+1);
  }
  else {
    ret=cmd;
  }
  return ret;
}


QString SoftwareAuthority::BundleString(int offset,bool state)
{
  QString ret="";

  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE;i++) {
    if(i==offset) {
      if(state) {
	ret+="l";
      }
      else {
	ret+="h";
      }
    }
    else {
      ret+="x";
    }
  }
  
  return ret;
}
