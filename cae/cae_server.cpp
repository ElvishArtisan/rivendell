// cae_server.cpp
//
// Network server for caed(8).
//
//   (C) Copyright 2019-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QByteArray>
#include <QNetworkDatagram>
#include <QStringList>

#include <rdapplication.h>

#include "cae_server.h"

//
// Uncomment this to send all protocol messages to syslog (DEBUG priority)
//
// #define __CAE_SERVER_LOG_PROTOCOL_MESSAGES

CaeServer::CaeServer(QObject *parent)
  : QObject(parent)
{
  d_server_socket=new QUdpSocket(this);
  connect(d_server_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


uint16_t CaeServer::meterPort(const SessionId &sid) const
{
  Connection *conn=NULL;

  if((conn=cae_connections.value(sid.normalized()))==NULL) {
    return 0;
  }
  return conn->meterPort();
}


bool CaeServer::bind(const QHostAddress &addr,uint16_t port)
{
  return d_server_socket->bind(port);
}


void CaeServer::sendCommand(const QString &cmd)
{
  /*
  for(QMap<int,CaeServerConnection *>::const_iterator it=
	cae_connections.begin();it!=cae_connections.end();it++) {
    if(it.value()->authenticated) {
      sendCommand(it.key(),cmd);
    }
  }
  */
}


void CaeServer::sendCommand(const SessionId &dest,const QString &cmd)
{
#ifdef __CAE_SERVER_LOG_PROTOCOL_MESSAGES
  rda->syslog(LOG_DEBUG,"sending \"%s\" to %s",
	      cmd.toUtf8().constData(),dest.dump().toUtf8().constData());
#endif  // __CAE_SERVER_LOG_PROTOCOL_MESSAGES
  d_server_socket->writeDatagram(cmd.toUtf8(),dest.address(),dest.port());
}


void CaeServer::readyReadData()
{
  QNetworkDatagram dgram=d_server_socket->receiveDatagram(1500);
  ProcessCommand(dgram.senderAddress(),dgram.senderPort(),
		 QString::fromUtf8(dgram.data()));
}


void CaeServer::connectionExpiredData(const SessionId &sid)
{
  Connection *conn=cae_connections.value(sid);

  if(conn!=NULL) {
    conn->deleteLater();
    cae_connections.remove(sid);
    emit connectionClosed(sid);
  }
}

/*
void CaeServer::connectionClosedData(int id)
{
  QString logmsg=
    QString::asprintf("removed connection %d [%s:%u]",
		      id,
		      peerAddress(id).toString().toUtf8().constData(),
		      0xFFFF&peerPort(id));
  int priority=LOG_DEBUG;
  if(!cae_connections.value(id)->authenticated) {
    logmsg=
      QString::asprintf("removed never authenticated connection %d [%s:%u]",
			id,
			peerAddress(id).toString().toUtf8().constData(),
			0xFFFF&peerPort(id));
    priority=LOG_WARNING;
  }
  emit connectionDropped(id);
  cae_connections.value(id)->socket->disconnect();
  delete cae_connections.value(id);
  cae_connections.remove(id);

  RDApplication::syslog(cae_config,priority,"%s",logmsg.toUtf8().constData());
}
*/

bool CaeServer::ProcessCommand(const QHostAddress &src_addr,uint16_t src_port,
			       const QString &cmd)
{
  bool was_processed=false;
  bool ok=false;
  QStringList f0=cmd.split(" ",QString::SkipEmptyParts);
  unsigned serial;
  QString cutname;
  unsigned cardnum;
  unsigned portnum;
  int start_pos;
  int end_pos;
  int position;
  int speed;
  int level;
  int length;
  int threshold;
  int coding;
  int channels;
  int bitrate;
  int interval;
  int volume;
  SessionId origin(src_addr,src_port);

  //
  // Connection Management
  //
  if((f0.at(0)=="TO")&&(f0.size()==2)) {  // Set Timeout
    interval=f0.at(1).toInt(&ok);
    if(ok&&(interval>=0)) {
      Connection *conn=GetConnection(origin);
      /*
      Connection *conn=cae_connections.value(origin);
      if(conn==NULL) {
	conn=new Connection(origin,this);
	connect(conn,SIGNAL(connectionExpired(const SessionId &)),
		this,SLOT(connectionExpiredData(const SessionId &)));
	cae_connections[origin]=conn;
      }
      */
      conn->setTimeout(interval);
      was_processed=true;
    }
  }

  if((f0.at(0)=="TH")&&(f0.size()==1)) {  // Touch
    Connection *conn=cae_connections.value(origin);
    if(conn==NULL) {
      rda->syslog(LOG_WARNING,"%s attempted to touch non-existent connection",
		  origin.dump().toUtf8().constData());
    }
    else {
      conn->touch();
    }
    was_processed=true;
  }

  //
  // Playback Operations
  //
  if((f0.at(0)=="PY")&&(f0.size()==9)) {  // Start Playback
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      cutname=f0.at(2);
      if(cutname.length()==10) {
	cardnum=f0.at(3).toUInt(&ok);
	if(ok&&(cardnum<RD_MAX_CARDS)) {
	  portnum=f0.at(4).toInt(&ok);
	  if(ok&&(portnum<RD_MAX_PORTS)) {
	    start_pos=f0.at(5).toInt(&ok);
	    if(ok&&(start_pos>=0)) {
	      end_pos=f0.at(6).toInt(&ok);
	      if(ok&&(end_pos>=0)&&(end_pos>=start_pos)) {
		speed=f0.at(7).toInt(&ok);
		if(ok&&(speed>0)) {
		  volume=f0.at(8).toInt(&ok);
		  if(ok) {
		    emit startPlaybackReq(origin,cutname,cardnum,portnum,
					  start_pos,end_pos,speed,volume);
		    was_processed=true;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  if((f0.at(0)=="PP")&&(f0.size()==3)) {  // Play Position
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      position=f0.at(2).toInt(&ok);
      if(ok&&(position>=0)) {
	emit playPositionReq(origin,position);
	was_processed=true;
      }
    }
  }

  if((f0.at(0)=="PE")&&(f0.size()==2)) {  // Pause Playback
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      emit playPauseReq(origin);
      was_processed=true;
    }
  }

  if((f0.at(0)=="PR")&&(f0.size()==2)) {  // Resume Playback
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      emit playResumeReq(origin);
      was_processed=true;
    }
  }

  if((f0.at(0)=="SP")&&(f0.size()==2)) {  // Stop Playback
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      emit playStopReq(origin);
      was_processed=true;
    }
  }

  if((f0.at(0)=="OV")&&(f0.size()==3)) {  // Set Output Volume
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      level=f0.at(2).toInt(&ok);
      if(ok) {
	emit playSetOutputVolumeReq(origin,level);
	was_processed=true;
      }
    }
  }

  if((f0.at(0)=="FV")&&(f0.size()==4)) {  // Fade Output Volume
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      level=f0.at(2).toInt(&ok);
      if(ok) {
	length=f0.at(3).toInt(&ok);
	if(ok&&length>=0) {
	  emit playFadeOutputVolumeReq(origin,level,length);
	  was_processed=true;
	}
      }
    }
  }

  //
  // Record Operations
  //
  if((f0.at(0)=="LR")&&(f0.size()==8)) {  // Cue Recording
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      cutname=f0.at(2);
      if(cutname.length()==10) {
	cardnum=f0.at(3).toUInt(&ok);
	if(ok&&(cardnum<RD_MAX_CARDS)) {
	  portnum=f0.at(4).toInt(&ok);
	  if(ok&&(portnum<RD_MAX_PORTS)) {
	    coding=f0.at(5).toInt(&ok);
	    if(ok&&(coding>=0)&&(coding<=4)) {
	      channels=f0.at(6).toInt(&ok);
	      if(ok&&(channels>0)) {
		bitrate=f0.at(7).toInt(&ok);
		if(ok&&(bitrate>=0)) {
		  emit recordCueReq(origin,cutname,cardnum,portnum,
				    coding,channels,bitrate);
		  was_processed=true;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  if((f0.at(0)=="RD")&&(f0.size()==4)) {  // Start Recording
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      length=f0.at(2).toInt(&ok);
      if(ok&&(length>=0)) {
	threshold=f0.at(3).toInt(&ok);
	if(ok&&(threshold<=0)) {
	  emit recordStartReq(origin,length,threshold);
	  was_processed=true;
	}
      }
    }
  }

  if((f0.at(0)=="RC")&&(f0.size()==10)) {  // Cue and Start Recording
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      cutname=f0.at(2);
      if(cutname.length()==10) {
	cardnum=f0.at(3).toUInt(&ok);
	if(ok&&(cardnum<RD_MAX_CARDS)) {
	  portnum=f0.at(4).toInt(&ok);
	  if(ok&&(portnum<RD_MAX_PORTS)) {
	    coding=f0.at(5).toInt(&ok);
	    if(ok&&(coding>=0)&&(coding<=4)) {
	      channels=f0.at(6).toInt(&ok);
	      if(ok&&(channels>0)) {
		bitrate=f0.at(7).toInt(&ok);
		if(ok&&(bitrate>=0)) {
		  length=f0.at(8).toInt(&ok);
		  if(ok&&(length>=0)) {
		    threshold=f0.at(9).toInt(&ok);
		    if(ok&&(threshold<=0)) {
		      emit recordCueAndStartReq(origin,cutname,
						cardnum,portnum,
						coding,channels,bitrate,
						length,threshold);
		      was_processed=true;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  if((f0.at(0)=="SR")&&(f0.size()==2)) {  // Stop Recording
    serial=f0.at(1).toUInt(&ok);
    if(ok) {
      origin.setSerialNumber(serial);
      emit recordStopReq(origin);
      was_processed=true;
    }
  }

  //
  // Mixer Operations
  //
  if((f0.at(0)=="IS")&&(f0.size()==3)) {  // Get Input Status
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned port=f0.at(2).toUInt(&ok);
      if(ok&&(port<RD_MAX_PORTS)) {
	emit getInputStatusReq(origin,card,port);
	was_processed=true;
      }
    }
  }

  if((f0.at(0)=="AL")&&(f0.size()==5)) {  // Set Audio Passthrough Level
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned input=f0.at(2).toUInt(&ok);
      if(ok&&(input<RD_MAX_PORTS)) {
	unsigned output=f0.at(3).toUInt(&ok);
	if(ok&&(output<RD_MAX_PORTS)) {
	  int level=f0.at(4).toInt(&ok);
	  if(ok) {
	    if(level<RD_MUTE_DEPTH) {
	      level=RD_MUTE_DEPTH;
	    }
	    emit setAudioPassthroughLevelReq(origin.address(),
					     card,input,output,level);
	    was_processed=true;
	  }
	}
      }
    }
  }

  if(f0.at(0)=="AP") {  // Update Audio Ports
    emit updateAudioPortsReq();
    was_processed=true;
  }

  //
  // External Operations
  //
  if(f0.at(0)=="CO") {  // Open RTP Capture Channel
  }

  //
  // Meter Commands
  //
  if(f0.at(0)=="ME") {  // Meter Enable
    if(f0.size()>2) {  // So we don't warn if no cards are specified
      uint16_t udp_port=0xFFFF&f0.at(1).toUInt(&ok);
      if(ok) {
	QList<unsigned> cards;
	for(int i=2;i<f0.size();i++) {
	  cards.push_back(f0.at(i).toUInt());
	}
	Connection *conn=GetConnection(origin);
	conn->setMeterPort(udp_port);
	emit meterEnableReq(src_addr,udp_port,cards);
      }
    }
    was_processed=true;
  }

  if(!was_processed) {
    rda->syslog(LOG_WARNING,
		"%s sent malformed command \"%s\"",
		origin.dump().toUtf8().constData(),
		cmd.toUtf8().constData());
  }



  /*
  //  rda->syslog(LOG_NOTICE,"processing command: \"%s\"",cmd.toUtf8().constData());
  CaeServerConnection *conn=cae_connections.value(id);
  bool ok=false;
  QString cmdstr=cmd;
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
      RDApplication::syslog(cae_config,LOG_DEBUG,
			    "PASSED authentication: connection %d [%s:%u]",
			    id,
			    peerAddress(id).toString().toUtf8().constData(),
			    0xFFFF&peerPort(id));
    }
    else {
      conn->authenticated=false;
      sendCommand(id,"PW -!");
      RDApplication::syslog(cae_config,LOG_WARNING,
			    "FAILED authentication: connection %d [%s:%u]",
			    id,
			    peerAddress(id).toString().toUtf8().constData(),
			    0xFFFF&peerPort(id));
      connectionClosedData(id);
      return true;
    }
    return false;
  }  

  //
  // Priviledged Commands
  // Authentication required to execute these!
  //
  if(!conn->authenticated) {
    RDApplication::syslog(cae_config,LOG_WARNING,
			  "unauthenticated connection %d [%s:%u] sent command \"%s\"",
			  id,
			  peerAddress(id).toString().toUtf8().constData(),
			  0xFFFF&peerPort(id),
			  cmdstr.toUtf8().constData());
    connectionClosedData(id);
    return true;
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
  if((f0.at(0)=="OP")&&(f0.size()==5)) {  // Set Output Port
    unsigned card=f0.at(1).toUInt(&ok);
    if(ok&&(card<RD_MAX_CARDS)) {
      unsigned stream=f0.at(2).toUInt(&ok);
      if(ok&&(stream<RD_MAX_STREAMS)) {
	if(ok) {
	  unsigned port=f0.at(3).toUInt(&ok);
	  if(ok&&(port<RD_MAX_PORTS)) {
	    int level=f0.at(4).toInt(&ok);
	    if(ok) {
	      emit setOutputPortReq(id,card,stream,port,level);
	      was_processed=true;
	    }
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
	  int port=f0.at(3).toInt(&ok);
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
  */
  return false;
}


Connection *CaeServer::GetConnection(const SessionId &sid)
{
  Connection *conn=cae_connections.value(sid);
  if(conn==NULL) {
    conn=new Connection(sid,this);
    printf("Added connection %s\n",conn->sessionId().dump().toUtf8().constData());
    connect(conn,SIGNAL(connectionExpired(const SessionId &)),
	    this,SLOT(connectionExpiredData(const SessionId &)));
    cae_connections[sid]=conn;
  }
  return conn;
}
