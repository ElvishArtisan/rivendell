// rdcae.cpp
//
// Connection to the Rivendell Core Audio Engine
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>

#include <ctype.h>

#include <QStringList>
#include <QTimer>

#include <rdapplication.h>
#include <rddb.h>
#include <rdcae.h>
#include <rddebug.h>
#include <rdescape_string.h>

__RDCae_PlayChannel::__RDCae_PlayChannel(unsigned card,unsigned port)
{
  d_card=card;
  d_port=port;
  d_position=0;
  for(int i=0;i<2;i++) {
    d_stream_levels[i]=RD_MUTE_DEPTH;
  }
}


unsigned __RDCae_PlayChannel::card() const
{
  return d_card;
}


unsigned __RDCae_PlayChannel::port() const
{
  return d_port;
}


unsigned __RDCae_PlayChannel::position() const
{
  return d_position;
}


void __RDCae_PlayChannel::setPosition(unsigned pos)
{
  d_position=pos;
}


void __RDCae_PlayChannel::getStreamLevels(short lvls[2])
{
  for(int i=0;i<2;i++) {
    lvls[i]=d_stream_levels[i];
  }
}


void __RDCae_PlayChannel::setStreamLevels(short left_lvl,short right_lvl)
{
  d_stream_levels[0]=left_lvl;
  d_stream_levels[1]=right_lvl;
}


bool __RDCae_PlayChannel::operator==(const __RDCae_PlayChannel &other) const
{
  return (d_card==other.d_card)&&(d_port==other.d_port);
}




RDCae::RDCae(RDStation *station,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  int flags=0;

  cae_station=station;
  cae_config=config;
  cae_connected=false;
  next_serial_number=1;

  //
  // Control Connection
  //
  if((cae_socket=socket(AF_INET,SOCK_STREAM,0))<0) {
    rda->syslog(LOG_ERR,"failed to create socket [%s]",strerror(errno));
    exit(RDCoreApplication::ExitInternalError);
  }
  if((flags=fcntl(cae_socket,F_GETFL,NULL))<0) {
    rda->syslog(LOG_ERR,"failed to get control socket options [%s]",
		strerror(errno));
    exit(RDCoreApplication::ExitInternalError);
  }
  flags=flags|O_NONBLOCK;
  if(fcntl(cae_socket,F_SETFL,flags)<0) {
    rda->syslog(LOG_ERR,"failed to set control socket options [%s]",
		strerror(errno));
    exit(RDCoreApplication::ExitInternalError);
  }

  //
  // Meter Connection
  //
  if((cae_meter_socket=socket(AF_INET,SOCK_DGRAM,0))<0) {
    rda->syslog(LOG_ERR,"failed to meter create socket [%s]",strerror(errno));
    exit(RDCoreApplication::ExitInternalError);
  }
  if((flags=fcntl(cae_meter_socket,F_GETFL,NULL))<0) {
    rda->syslog(LOG_ERR,"failed to get meter socket options [%s]",
		strerror(errno));
    exit(RDCoreApplication::ExitInternalError);
  }
  flags=flags|O_NONBLOCK;
  if(fcntl(cae_meter_socket,F_SETFL,flags)<0) {
    rda->syslog(LOG_ERR,"failed to set meter socket options [%s]",
		strerror(errno));
    exit(RDCoreApplication::ExitInternalError);
  }
  cae_meter_base_port=cae_config->meterBasePort();
  cae_meter_port_range=cae_config->meterPortRange();
  if(cae_meter_port_range>999) {
    cae_meter_port_range=999;
  }
  for(int16_t i=cae_meter_base_port;i<(cae_meter_base_port+cae_meter_port_range);i++) {
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(sa));
    sa.sin_family=AF_INET;
    sa.sin_port=htons(i);
    sa.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(cae_meter_socket,(struct sockaddr *)(&sa),sizeof(sa))==0) {
      cae_meter_port=i;
      i=(cae_meter_base_port+cae_meter_port_range)+1;
    }
  }

  //
  // Initialize Data Structures
  //
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_PORTS;j++) {
      input_status[i][j]=false;
      for(unsigned k=0;k<2;k++) {
	cae_input_levels[i][j][k]=-10000;
	cae_output_levels[i][j][k]=-10000;
      }
    }
  }
}


RDCae::~RDCae() {
  if(cae_socket>=0) {
    close(cae_socket);
  }
  if(cae_meter_socket>=0) {
    close(cae_meter_socket);
  }
}


bool RDCae::connectHost(QString *err_msg)
{
  int count=10;
  struct sockaddr_in sa;
  QTimer *timer=new QTimer(this);

  connect(timer,SIGNAL(timeout()),this,SLOT(readyData()));
  timer->start(CAE_POLL_INTERVAL);
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  sa.sin_port=htons(CAED_TCP_PORT);
  sa.sin_addr.s_addr=
    htonl(rda->station()->caeAddress(rda->config()).toIPv4Address());
  while((::connect(cae_socket,(struct sockaddr *)(&sa),sizeof(sa))<0)&&
	(--count>0)) {
    usleep(100000);
  }
  usleep(100000);
  if(count>0) {
    SendCommand(QString().sprintf("PW %s!",
				  cae_config->password().toUtf8().constData()));
    for(int i=0;i<RD_MAX_CARDS;i++) {
      SendCommand(QString().sprintf("TS %d!",i));
      for(int j=0;j<RD_MAX_PORTS;j++) {
	SendCommand(QString().sprintf("IS %d %d!",i,j));
      }
    }
  }
  else {
    *err_msg=QString::asprintf("failed to connect to CAE service [%s]",
			       strerror(errno));
    rda->syslog(LOG_ERR,"%s",err_msg->toUtf8().constData());
    return false;
  }
  *err_msg="ok";
  return true;
}


void RDCae::enableMetering(QList<int> *cards)
{
  QString cmd=QString().sprintf("ME %u",0xFFFF&cae_meter_port);
  for(int i=0;i<cards->size();i++) {
    if(cards->at(i)>=0) {
      bool found=false;
      for(int j=0;j<i;j++) {
	if(cards->at(i)==cards->at(j)) {
	  found=true;
	}
      }
      if(!found) {
	cmd+=QString().sprintf(" %d",cards->at(i));
      }
    }
  }
  SendCommand(cmd+"!");
}


unsigned RDCae::loadPlay(unsigned card,unsigned port,const QString &name)
{
  /*
  cae_serials[card][port]=next_serial_number++;
  SendCommand(QString().sprintf("LP %u %u %u %s!",
				cae_serials[card][port],card,port,
				name.toUtf8().constData()));
  __RDCae_PlayChannel chan(card,port);
  for(QMap<unsigned,__RDCae_PlayChannel>::const_iterator it=cae_play_channels.begin();it!=cae_play_channels.end();it++) {
    if(it.value()==chan) {
      emit playPortStatusChanged(card,port,true);
      break;
    }
  }
  cae_play_channelscae_serials

  return cae_serials[card][port];
  */
  unsigned serial=next_serial_number++;
  SendCommand(QString().sprintf("LP %u %u %u %s!",
				serial,card,port,name.toUtf8().constData()));
  bool found=false;
  for(QMap<unsigned,__RDCae_PlayChannel *>::const_iterator it=cae_play_channels.begin();it!=cae_play_channels.end();it++) {
    if((it.value()->card()==card)&&(it.value()->port()==port)) {
      found=true;
      break;
    }
  }
  cae_play_channels[serial]=new __RDCae_PlayChannel(card,port);
  if(!found) {
    emit playPortStatusChanged(card,port,true);
  }

  return serial;
}


void RDCae::unloadPlay(unsigned serial)
{
  SendCommand(QString().sprintf("UP %u!",serial));
}


void RDCae::positionPlay(unsigned serial,int pos)
{
  if(pos<0) {
    return;
  }
  SendCommand(QString().sprintf("PP %u %u!",serial,pos));
}


void RDCae::play(unsigned serial,unsigned length,int speed,bool pitch)
{
  int pitch_state=0;

  if(pitch) {
    pitch_state=1;
  }
  SendCommand(QString().sprintf("PY %u %u %d %d!",
				serial,length,speed,pitch_state));
}


void RDCae::stopPlay(unsigned serial)
{
  SendCommand(QString().sprintf("SP %u!",serial));
}


void RDCae::loadRecord(int card,int stream,QString name,
		       AudioCoding coding,int chan,int samp_rate,
		       int bit_rate)
{
  SendCommand(QString().sprintf("LR %d %d %d %d %d %d %s!",
				card,stream,(int)coding,chan,samp_rate,
				bit_rate,name.toUtf8().constData()));
}


void RDCae::unloadRecord(int card,int stream)
{
  SendCommand(QString().sprintf("UR %d %d!",card,stream));
}


void RDCae::record(int card,int stream,unsigned length,int threshold)
{
  SendCommand(QString().sprintf("RD %d %d %u %d!",
				card,stream,length,threshold));
}


void RDCae::stopRecord(int card,int stream)
{
  SendCommand(QString().sprintf("SR %d %d!",card,stream));
}


void RDCae::setClockSource(int card,RDCae::ClockSource src)
{
  SendCommand(QString().sprintf("CS %d %d!",card,src));
}


void RDCae::setInputVolume(int card,int stream,int level)
{
  SendCommand(QString().sprintf("IV %d %d %d!",card,stream,level));
}


void RDCae::setOutputVolume(unsigned serial,int level)
{
  SendCommand(QString().sprintf("OV %u %d!",serial,level));
}


void RDCae::fadeOutputVolume(unsigned serial,int level,int length)
{
  SendCommand(QString().sprintf("FV %u %d %d!",serial,level,length));
}


void RDCae::setInputLevel(int card,int port,int level)
{
  SendCommand(QString().sprintf("IL %d %d %d!",card,port,level));
}


void RDCae::setOutputLevel(int card,int port,int level)
{
  SendCommand(QString().sprintf("OL %d %d %d!",card,port,level));
}


void RDCae::setInputMode(int card,int stream,RDCae::ChannelMode mode)
{
  SendCommand(QString().sprintf("IM %d %d %d!",card,stream,mode));
}


void RDCae::setOutputMode(int card,int stream,RDCae::ChannelMode mode)
{
  SendCommand(QString().sprintf("OM %d %d %d!",card,stream,mode));
}


void RDCae::setInputVOXLevel(int card,int stream,int level)
{
  SendCommand(QString().sprintf("IX %d %d %d!",card,stream,level));
}


void RDCae::setInputType(int card,int port,RDCae::SourceType type)
{
  SendCommand(QString().sprintf("IT %d %d %d!",card,port,type));
}


void RDCae::setPassthroughVolume(int card,int in_port,int out_port,int level)
{
  SendCommand(QString().
	      sprintf("AL %d %d %d %d!",card,in_port,out_port,level));
}


bool RDCae::inputStatus(int card,int port) const
{
  return input_status[card][port];
}


void RDCae::inputMeterUpdate(int card,int port,short levels[2])
{
  UpdateMeters();
  levels[0]=cae_input_levels[card][port][0];
  levels[1]=cae_input_levels[card][port][1];
}


void RDCae::outputMeterUpdate(int card,int port,short levels[2])
{
  UpdateMeters();
  levels[0]=cae_output_levels[card][port][0];
  levels[1]=cae_output_levels[card][port][1];
}


void RDCae::outputStreamMeterUpdate(unsigned serial,short levels[2])
{
  __RDCae_PlayChannel *chan=NULL;

  if((chan=cae_play_channels.value(serial))!=NULL) {
    UpdateMeters();
    chan->getStreamLevels(levels);
  }
}


unsigned RDCae::playPosition(unsigned serial)
{
  __RDCae_PlayChannel *chan=NULL;

  if((chan=cae_play_channels.value(serial))!=NULL) {
    return chan->position();
  }

  return 0;
}


void RDCae::requestTimescale(int card)
{
  SendCommand(QString().sprintf("TS %d!",card));
}


bool RDCae::playPortStatus(int card,int port,unsigned except_serial) const
{
  for(QMap<unsigned,__RDCae_PlayChannel *>::const_iterator it=
	cae_play_channels.begin();it!=cae_play_channels.end();it++) {
    if((((int)it.value()->card())==card)&&
       (((int)it.value()->port())==port)&&
       (it.key()!=except_serial)) {
      return true;
    }
  }
  return false;
}


void RDCae::readyData()
{
  char data[1501];
  int n;

  if((n=read(cae_socket,data,1500))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case 10:  // Ignore CR/LF
      case 13:
	break;

      case '!':
	DispatchCommand(QString::fromUtf8(cae_accum));
	cae_accum.clear();
	break;

      default:
	cae_accum+=0xFF&data[i];
	break;
      }
    }
  }
}


void RDCae::SendCommand(QString cmd)
{
  write(cae_socket,cmd.toUtf8().constData(),cmd.toUtf8().length());
}


void RDCae::DispatchCommand(const QString &cmd)
{
  __RDCae_PlayChannel *chan=NULL;
  QStringList cmds=cmd.split(" ",QString::SkipEmptyParts);
  bool was_processed=false;
  bool ok=false;

  if((cmds.at(0)=="PW")&&(cmds.size()==2)) {   // Password Response
    emit isConnected(cmds.at(1)=="+");
    was_processed=true;
  }

  if((cmds.at(0),"LP")&&(cmds.size()==5)) {   // Load Play
    // FIXME: What should go here?
    was_processed=true;
  }

  if((cmds.at(0)=="UP")&&(cmds.size()==3)) {   // Unload Play
    unsigned serial=cmds.at(1).toUInt(&ok);
    if(ok) {
      if(cmds.at(2)=='+') {
	if((chan=cae_play_channels.value(serial))!=NULL) {
	  delete chan;
	  cae_play_channels.remove(serial);
	  if(SerialCheck(serial,LINE_NUMBER)) {
	    emit playUnloaded(serial);
	  }
	}
      }
      was_processed=true;
    }
  }

  if((cmds.at(0)=="PP")&&(cmds.size()==3)) {   // Position Play
    unsigned serial=cmds.at(1).toUInt(&ok);
    if(ok) {
      unsigned pos=cmds.at(2).toUInt(&ok);
      if(ok) {
	if((chan=cae_play_channels.value(serial))!=NULL) {
	  if(SerialCheck(serial,LINE_NUMBER)) {
	    emit playPositioned(serial,pos);
	  }
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="PY")&&(cmds.size()==6)) {   // Play
    if(cmds.at(5)=='+') {
      unsigned serial=cmds.at(1).toUInt(&ok);
      if(ok) {
	if(SerialCheck(serial,LINE_NUMBER)) {
	  emit playing(serial);
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="SP")&&(cmds.size()==3)) {   // Stop Play
    if(cmds.at(2)=='+') {
      unsigned serial=cmds.at(1).toUInt(&ok);
      if(ok) {
	if(SerialCheck(serial,LINE_NUMBER)) {
	  emit playStopped(serial);
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="TS")&&(cmds.size()==3)) {   // Timescaling Support
    int card=cmds.at(1).toInt(&ok);
    if(ok&&(card>=0)&&(card<RD_MAX_CARDS)) {
      emit timescalingSupported(card,cmds.at(2)=="+");
    }
    was_processed=true;
  }

  if((cmds.at(0)=="LR")&&(cmds.size()==9)) {   // Load Record
    if(cmds.at(8)=='+') {
      int card=cmds.at(1).toInt(&ok);
      if(ok&&(card>=0)&&(card<RD_MAX_CARDS)) {
	int port=cmds.at(2).toInt(&ok);
	if(ok&&(port>=0)&&(port<RD_MAX_PORTS)) {
	  emit recordLoaded(card,port);
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="UR")&&(cmds.size()==5)) {   // Unload Record
    if(cmds.at(3)=='+') {
      int card=cmds.at(1).toInt(&ok);
      if(ok&&(card>=0)&&(card<RD_MAX_CARDS)) {
	int port=cmds.at(2).toInt(&ok);
	if(ok&&(port>=0)&&(port<RD_MAX_PORTS)) {
	  unsigned len=cmds.at(3).toUInt(&ok);
	  if(ok) {
	    emit recordUnloaded(card,port,len);
	  }
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="RD")&&(cmds.size()==6)) {   // Record
    was_processed=true;
  }

  if((cmds.at(0)=="RS")&&(cmds.size()==4)) {   // Record Start
    if(cmds.at(3)=='+') {
      int card=cmds.at(1).toInt(&ok);
      if(ok&&(card>=0)&&(card<RD_MAX_CARDS)) {
	int port=cmds.at(2).toInt(&ok);
	if(ok&&(port>=0)&&(port<RD_MAX_PORTS)) {
	  emit recording(card,port);
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="SR")&&(cmds.size()==4)) {   // Stop Record
    if(cmds.at(3)=='+') {
      int card=cmds.at(1).toInt(&ok);
      if(ok&&(card>=0)&&(card<RD_MAX_CARDS)) {
	int port=cmds.at(2).toInt(&ok);
	if(ok&&(port>=0)&&(port<RD_MAX_PORTS)) {
	  emit recordStopped(card,port);
	}
      }
    }
    was_processed=true;
  }

  if((cmds.at(0)=="IS")&&(cmds.size()==5)) {   // Input Status
    if(cmds.at(4)=='+') {
      int card=cmds.at(1).toInt(&ok);
      if(ok&&(card>=0)&&(card<RD_MAX_CARDS)) {
	int port=cmds.at(2).toInt(&ok);
	if(ok&&(port>=0)&&(port<RD_MAX_PORTS)) {
	  emit inputStatusChanged(card,port,cmds.at(3)=="0");
	  input_status[card][port]=cmds.at(3)=="0";
	}
      }
    }
    was_processed=true;
  }

  if(!was_processed) {
    rda->syslog(LOG_WARNING,"CAE response command \"%s\" was not processed",
		cmd.toUtf8().constData());
  }
}


void RDCae::UpdateMeters()
{
  char msg[1501];
  int n;
  QStringList args;
  __RDCae_PlayChannel *chan=NULL;

  bool ok=false;

  while((n=read(cae_meter_socket,msg,1500))>0) {
    msg[n]=0;
    args=QString(msg).split(" ");
    if(args[0]=="ML") {
      if(args.size()==6) {
	if(args[1]=="I") {
	  cae_input_levels[args[2].toInt()][args[3].toInt()][0]=args[4].toInt();
	  cae_input_levels[args[2].toInt()][args[3].toInt()][1]=args[5].toInt();
	}
	if(args[1]=="O") {
	  cae_output_levels[args[2].toInt()][args[3].toInt()][0]=
	    args[4].toInt();
	  cae_output_levels[args[2].toInt()][args[3].toInt()][1]=
	    args[5].toInt();
	}
      }
    }
    if(args[0]=="MO") {
      if(args.size()==4) {
	unsigned serial=args.at(1).toUInt(&ok);
	if(ok) {
	  if((chan=cae_play_channels.value(serial))!=NULL) {
	    chan->setStreamLevels(args.at(2).toShort(),args.at(3).toShort());
	  }
	}
      }
    }
    if(args[0]=="MP") {
      if(args.size()==3) {
	unsigned serial=args.at(1).toUInt(&ok);
	if(ok) {
	  emit playPositionChanged(serial,args.at(2).toUInt());
	}
      }
    }
  }
}


bool RDCae::SerialCheck(unsigned serial,int linenum) const
{
  if(serial==0) {
        rda->syslog(LOG_WARNING,
		"attempting to use null serial value at rdcae.cpp:%d",linenum);
    return false;
  }
  return true;
}
