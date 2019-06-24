// cae_server.cpp
//
// Network server for caed(8).
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <ctype.h>
#include <stdio.h>

#include <qbytearray.h>
#include <qstringlist.h>

#include <rdapplication.h>

#include "cae_server.h"

//
// Uncomment this to send all protocol messages to syslog (DEBUG priority)
//
// #define __CAE_SERVER_LOG_PROTOCOL_MESSAGES

CaeServerConnection::CaeServerConnection(QTcpSocket *sock)
{
  socket=sock;
  authenticated=false;
  accum="";
  meter_port=0;
  for(int i=0;i<RD_MAX_CARDS;i++) {
    meters_enabled[i]=false;
  }
}


CaeServerConnection::~CaeServerConnection()
{
  socket->deleteLater();
}




CaeServer::CaeServer(RDConfig *config,QObject *parent)
  : QObject(parent)
{
  cae_config=config;

  cae_server=new QTcpServer(this);
  connect(cae_server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));

  cae_ready_read_mapper=new QSignalMapper(this);
  connect(cae_ready_read_mapper,SIGNAL(mapped(int)),
	  this,SLOT(readyReadData(int)));

  cae_connection_closed_mapper=new QSignalMapper(this);
  connect(cae_connection_closed_mapper,SIGNAL(mapped(int)),
	  this,SLOT(connectionClosedData(int)));
}


QList<int> CaeServer::connectionIds() const
{
  QList<int> ret;

  for(QMap<int,CaeServerConnection *>::const_iterator it=
	cae_connections.begin();it!=cae_connections.end();it++) {
    ret.push_back(it.key());
  }

  return ret;
}


QHostAddress CaeServer::peerAddress(int id) const
{
  return cae_connections[id]->socket->peerAddress();
}


uint16_t CaeServer::peerPort(int id) const
{
  return cae_connections[id]->socket->peerPort();
}


uint16_t CaeServer::meterPort(int id) const
{
  return cae_connections[id]->meter_port;
}


void CaeServer::setMeterPort(int id,uint16_t port)
{
  cae_connections[id]->meter_port=port;
}


bool CaeServer::metersEnabled(int id,unsigned card) const
{
  return cae_connections[id]->meters_enabled[card];
}


void CaeServer::setMetersEnabled(int id,unsigned card,bool state)
{
  cae_connections[id]->meters_enabled[card]=state;
}


bool CaeServer::listen(const QHostAddress &addr,uint16_t port)
{
  return cae_server->listen(addr,port);
}


void CaeServer::sendCommand(const QString &cmd)
{
  for(QMap<int,CaeServerConnection *>::const_iterator it=
	cae_connections.begin();it!=cae_connections.end();it++) {
    if(it.value()->authenticated) {
      sendCommand(it.key(),cmd);
    }
  }
}


void CaeServer::sendCommand(int id,const QString &cmd)
{
#ifdef __CAE_SERVER_LOG_PROTOCOL_MESSAGES
  RDApplication::syslog(cae_config,LOG_DEBUG,
			"send[%d]: %s",id,(const char *)cmd.toUtf8());
#endif  // __CAE_SERVER_LOG_PROTOCOL_MESSAGES
  cae_connections.value(id)->socket->write(cmd.toAscii());
}


void CaeServer::newConnectionData()
{
  QTcpSocket *sock=cae_server->nextPendingConnection();

  cae_connection_closed_mapper->setMapping(sock,sock->socketDescriptor());
  connect(sock,SIGNAL(disconnected()),cae_connection_closed_mapper,SLOT(map()));

  cae_ready_read_mapper->setMapping(sock,sock->socketDescriptor());
  connect(sock,SIGNAL(readyRead()),cae_ready_read_mapper,SLOT(map()));

  cae_connections[sock->socketDescriptor()]=new CaeServerConnection(sock);

  RDApplication::syslog(cae_config,LOG_DEBUG,
			"added connection %d",sock->socketDescriptor());
}


void CaeServer::readyReadData(int id)
{
  QByteArray data=cae_connections.value(id)->socket->readAll();
  for(int i=0;i<data.size();i++) {
    char c=0xFF&data[i];
    switch(c) {
    case '!':
      if(ProcessCommand(id,cae_connections.value(id)->accum)) {
	return;
      }
      break;

    case 10:
    case 13:
      break;

    default:
      cae_connections.value(id)->accum+=c;
      break;
    }
  }
}


void CaeServer::connectionClosedData(int id)
{
  emit connectionDropped(id);
  cae_connections.value(id)->socket->disconnect();
  delete cae_connections.value(id);
  cae_connections.remove(id);

  RDApplication::syslog(cae_config,LOG_DEBUG,"removed connection %d",id);
}


bool CaeServer::ProcessCommand(int id,const QString &cmd)
{
  CaeServerConnection *conn=cae_connections.value(id);
  bool ok=false;
  QStringList f0=cmd.split(" ",QString::SkipEmptyParts);

  if(f0.size()==0) {
    return false;
  }
#ifdef __CAE_SERVER_LOG_PROTOCOL_MESSAGES
  RDApplication::syslog(cae_config,LOG_DEBUG,
			"recv[%d]: %s",id,(const char *)cmd.toUtf8());
#endif  // __CAE_SERVER_LOG_PROTOCOL_MESSAGES

  cae_connections.value(id)->accum="";

  //
  // Unpriviledged Commands
  //
  if(f0.at(0)=="DC") {
    connectionClosedData(id);
    return true;
  }

  if(f0.at(0)=="PW") {
    if((f0.size()==2)&&(f0.at(1)==cae_config->password())) {
      conn->authenticated=true;
      sendCommand(id,"PW +!");
    }
    else {
      conn->authenticated=false;
      sendCommand(id,"PW -!");
    }
    return false;
  }  

  //
  // Priviledged Commands
  // Authentication required to execute these!
  //
  if(!conn->authenticated) {
    return false;
  }
  bool was_processed=false;

  if((f0.at(0)=="LP")&&(f0.size()==3)) {  // Load Playback
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      emit loadPlaybackReq(id,card,f0.at(2));
      was_processed=true;
    }
  }
  if((f0.at(0)=="UP")&&(f0.size()==2)) {  // Unload Playback
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok) {
      emit unloadPlaybackReq(id,card);
      was_processed=true;
    }
  }
  if((f0.at(0)=="PP")&&(f0.size()==3)) {  // Play Position
    unsigned handle=f0.at(1).toUInt(&ok);
    if(ok) {
      unsigned pos=f0.at(2).toUInt(&ok);
      if(ok) {
	emit playPositionReq(id,handle,pos);
	was_processed=true;
      }
    }
  }
  if((f0.at(0)=="PY")&&(f0.size()==5)) {  // Play
    unsigned handle=f0.at(1).toUInt(&ok);
    if(ok) {
      unsigned len=f0.at(2).toUInt(&ok);
      if(ok) {
	unsigned speed=f0.at(3).toUInt(&ok);
	if(ok) {
	  unsigned pitch=f0.at(4).toUInt(&ok);
	  if(ok) {
	    emit playReq(id,handle,len,speed,pitch);
	    was_processed=true;
	  }
	}
      }
    }
  }
  if((f0.at(0)=="SP")&&(f0.size()==2)) {  // Stop Playback
    unsigned handle=f0.at(1).toUInt(&ok);
    if(ok) {
      emit stopPlaybackReq(id,handle);
      was_processed=true;
    }
  }
  if((f0.at(0)=="TS")&&(f0.size()==2)) {  // Timescaling Support
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      emit timescalingSupportReq(id,card);
      was_processed=true;
    }
  }
  if((f0.at(0)=="LR")&&(f0.size()==8)) {  // Load Recording
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	unsigned coding=f0.at(3).toUInt(&ok);
	if(ok&&(coding<5)) {
	  unsigned chans=f0.at(4).toUInt(&ok);
	  if(ok&&(chans<=2)) {
	    unsigned samprate=f0.at(5).toUInt(&ok);
	    if(ok) {
	      unsigned bitrate=f0.at(6).toUInt(&ok);
	      if(ok) {
		emit loadRecordingReq(id,card,port,coding,chans,samprate,
				      bitrate,f0.at(7));
		was_processed=true;
	      }
	    }
	  }
	}
      }
    }
  }
  if((f0.at(0)=="UR")&&(f0.size()==3)) {  // Unload Recording
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	emit unloadRecordingReq(id,card,stream);
	was_processed=true;
      }
    }
  }
  if((f0.at(0)=="RD")&&(f0.size()==5)) {  // Record
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	if(ok) {
	  unsigned len=f0.at(3).toUInt(&ok);
	  if(ok) {
	    int thres=f0.at(4).toInt(&ok);
	    if(ok) {
	      emit recordReq(id,card,stream,len,thres);
	      was_processed=true;
	    }
	  }
	}
      }
    }
  }
  if((f0.at(0)=="SR")&&(f0.size()==3)) {  // Stop Recording
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	if(ok) {
	  emit stopRecordingReq(id,card,stream);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="IV")&&(f0.size()==4)) {  // Set Input Volume
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	if(ok) {
	  int level=f0.at(3).toInt(&ok);
	  if(ok) {
	    emit setInputVolumeReq(id,card,stream,level);
	    was_processed=true;
	  }
	}
      }
    }
  }
  if((f0.at(0)=="OV")&&(f0.size()==5)) {  // Set Output Volume
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	if(ok) {
	  unsigned port=f0.at(3).toUInt(&ok);
	  if(ok&&(port<RD_MAX_PORTS)) {
	    int level=f0.at(4).toInt(&ok);
	    if(ok) {
	      emit setOutputVolumeReq(id,card,stream,port,level);
	      was_processed=true;
	    }
	  }
	}
      }
    }
  }
  if((f0.at(0)=="FV")&&(f0.size()==6)) {  // Fade Output Volume
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	if(ok) {
	  unsigned port=f0.at(3).toUInt(&ok);
	  if(ok&&(port<RD_MAX_PORTS)) {
	    int level=f0.at(4).toInt(&ok);
	    if(ok) {
	      int len=f0.at(5).toUInt(&ok);
	      if(ok) {
		emit fadeOutputVolumeReq(id,card,stream,port,level,len);
		was_processed=true;
	      }
	    }
	  }
	}
      }
    }
  }
  if((f0.at(0)=="IL")&&(f0.size()==4)) {  // Set Input Level
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	int level=f0.at(3).toInt(&ok);
	if(ok) {
	  emit setInputLevelReq(id,card,port,level);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="OL")&&(f0.size()==4)) {  // Set Output Level
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	int level=f0.at(3).toInt(&ok);
	if(ok) {
	  emit setOutputLevelReq(id,card,port,level);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="IM")&&(f0.size()==4)) {  // Set Input Mode
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	unsigned mode=f0.at(3).toUInt(&ok);
	if(ok&&(mode<=3)) {
	  emit setInputModeReq(id,card,port,mode);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="OM")&&(f0.size()==4)) {  // Set Output Mode
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	unsigned mode=f0.at(3).toUInt(&ok);
	if(ok&&(mode<=3)) {
	  emit setOutputModeReq(id,card,port,mode);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="IX")&&(f0.size()==4)) {  // Set Input Vox Level
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	int level=f0.at(3).toInt(&ok);
	if(ok) {
	  emit setInputVoxLevelReq(id,card,stream,level);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="IT")&&(f0.size()==4)) {  // Set Input Type
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	int type=f0.at(3).toInt(&ok);
	if(ok&&(type<=1)) {
	  emit setInputTypeReq(id,card,port,type);
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="IS")&&(f0.size()==3)) {  // Get Input Status
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	emit getInputStatusReq(id,card,port);
	was_processed=true;
      }
    }
  }
  if((f0.at(0)=="AL")&&(f0.size()==5)) {  // Set Audio Passthrough Level
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned input=f0.at(2).toUInt(&ok);
      if(ok&&(input<RD_MAX_PORTS)) {
	unsigned output=f0.at(2).toUInt(&ok);
	if(ok&&(output<RD_MAX_PORTS)) {
	  int level=f0.at(3).toUInt(&ok);
	  if(ok) {
	    emit setAudioPassthroughLevelReq(id,card,input,output,level);
	    was_processed=true;
	  }
	}
      }
    }
  }
  if((f0.at(0)=="CS")&&(f0.size()==3)) {  // Set Clock Source
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned input=f0.at(2).toUInt(&ok);
      if(ok&&(input<RD_MAX_PORTS)) {
	emit setClockSourceReq(id,card,input);
	was_processed=true;
      }
    }
  }
  if((f0.at(0)=="OS")&&(f0.size()==5)) {  // Set Output Status Flag
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	unsigned stream=f0.at(3).toUInt(&ok);
	if(ok&&(stream<RD_MAX_STREAMS)) {
	  emit setOutputStatusFlagReq(id,card,port,stream,f0.at(4)=="1");
	  was_processed=true;
	}
      }
    }
  }
  if((f0.at(0)=="JC")&&(f0.size()==3)) {  // Connect Jack Ports
    emit jackConnectPortsReq(id,f0.at(1),f0.at(2));
    was_processed=true;
  }
  if((f0.at(0)=="JD")&&(f0.size()==3)) {  // Disconnect Jack Ports
    emit jackDisconnectPortsReq(id,f0.at(1),f0.at(2));
    was_processed=true;
  }
  if((f0.at(0)=="ME")&&(f0.size()>=3)) {  // Meter Enable
    uint16_t udp_port=0xFFFF&f0.at(1).toUInt(&ok);
    if(ok) {
      QList<unsigned> cards;
      for(int i=2;i<f0.size();i++) {
	cards.push_back(f0.at(i).toUInt());
      }
      emit meterEnableReq(id,udp_port,cards);
      was_processed=true;
    }
  }

  if(!was_processed) {  // Send generic error response
    sendCommand(id,f0.join(" ")+"-!");
  }

  return false;
}
