// rdcae.cpp
//
// Connection to the Rivendell Core Audio Engine
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcae.cpp,v 1.59.4.5 2013/11/14 02:04:57 cvs Exp $
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
#include <sys/types.h>
#include <syslog.h>

#include <qobject.h>
#include <ctype.h>

#include <qsocketdevice.h>
#include <qtimer.h>
#include <qstringlist.h>

#include <rddb.h>
#include <rdcae.h>
#include <rddebug.h>
#include <rdescape_string.h>

RDCae::RDCae(RDStation *station,RDConfig *config,
	     QObject *parent,const char *name)
  : QObject(parent,name)
{
  cae_station=station;
  cae_config=config;
  cae_connected=false;
  argnum=0;
  argptr=0;

  //
  // TCP Connection
  //
  cae_socket=new QSocketDevice(QSocketDevice::Stream);
  cae_socket->setBlocking(false);

  //
  // Meter Connection
  //
  cae_meter_socket=new QSocketDevice(QSocketDevice::Datagram);
  cae_meter_socket->setBlocking(false);
  for(Q_INT16 i=30000;i<30100;i++) {
    if(cae_meter_socket->bind(QHostAddress(),i)) {
      i=31000;
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
	cae_stream_output_levels[i][j][k]=-10000;
      }
      for(int k=0;k<RD_MAX_STREAMS;k++) {
	cae_output_status_flags[i][j][k]=false;
      }
    }
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      cae_handle[i][j]=-1;
      cae_output_positions[i][j]=0;
    }
  }

  //
  // The Clock Timer
  //
  QTimer *timer=new QTimer(this,"clock_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(clockData()));
  timer->start(RD_METER_UPDATE_INTERVAL);
}


RDCae::~RDCae() {
  delete cae_socket;
}


void RDCae::connectHost()
{
  int count=10;
  //  QHostAddress addr;
  QTimer *timer=new QTimer(this,"read_timer");

  connect(timer,SIGNAL(timeout()),this,SLOT(readyData()));
  timer->start(CAE_POLL_INTERVAL);
  while((!cae_socket->connect(cae_station->caeAddress(cae_config),
			      CAED_TCP_PORT))&&(--count>0)) {
    usleep(100000);
  } 
  usleep(100000);
  if(count>0) {
    SendCommand(QString().sprintf("PW %s!",
				  (const char *)cae_config->password()));
    for(int i=0;i<RD_MAX_CARDS;i++) {
      SendCommand(QString().sprintf("TS %d!",i));
      for(int j=0;j<RD_MAX_PORTS;j++) {
	SendCommand(QString().sprintf("IS %d %d!",i,j));
      }
    }
    SendCommand(QString().sprintf("ME %u!",cae_meter_socket->port()));
  }
}

bool RDCae::loadPlay(int card,QString name,int *stream,int *handle)
{
  int count=0;

  SendCommand(QString().sprintf("LP %d %s!",
				card,(const char *)name));

  //
  // This is really warty, but needed to make the method 'synchronous'
  // with respect to CAE.
  //
  *stream=-2;
  *handle=-1;
  while(*stream==-2) {
    readyData(stream,handle,name);
    usleep(1000);
    count++;
  }
  if(count>1000) {
    syslog(LOG_ERR,"*** LoadPlay: CAE took %d mS to return stream for %s ***",
	   count,(const char *)name);
  }
  cae_handle[card][*stream]=*handle;
  cae_pos[card][*stream]=0xFFFFFFFF;
  return true;
}


void RDCae::unloadPlay(int handle)
{
  SendCommand(QString().sprintf("UP %d!",handle));
}


void RDCae::positionPlay(int handle,int pos)
{
  if(pos<0) {
    return;
  }
  SendCommand(QString().sprintf("PP %d %u!",handle,pos));
}


void RDCae::play(int handle,unsigned length,int speed,bool pitch)
{
  int pitch_state=0;

  if(pitch) {
    pitch_state=1;
  }
  SendCommand(QString().sprintf("PY %d %u %d %d!",
				handle,length,speed,pitch_state));
}


void RDCae::stopPlay(int handle)
{
  SendCommand(QString().sprintf("SP %d!",handle));
}


void RDCae::loadRecord(int card,int stream,QString name,
		       AudioCoding coding,int chan,int samp_rate,
		       int bit_rate)
{
  // printf("RDCae::loadRecord(%d,%d,%s,%d,%d,%d,%d)\n",
  //	 card,stream,(const char *)name,coding,chan,samp_rate,bit_rate);
  SendCommand(QString().sprintf("LR %d %d %d %d %d %d %s!",
				card,stream,(int)coding,chan,samp_rate,
				bit_rate,(const char *)name));
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


void RDCae::setOutputVolume(int card,int stream,int port,int level)
{
  SendCommand(QString().sprintf("OV %d %d %d %d!",card,stream,port,level));
}


void RDCae::fadeOutputVolume(int card,int stream,int port,int level,int length)
{
  SendCommand(QString().sprintf("FV %d %d %d %d %d!",
				card,stream,port,level,length));
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


void RDCae::outputStreamMeterUpdate(int card,int stream,short levels[2])
{
  UpdateMeters();
  levels[0]=cae_stream_output_levels[card][stream][0];
  levels[1]=cae_stream_output_levels[card][stream][1];
}


unsigned RDCae::playPosition(int handle)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(cae_handle[i][j]==handle) {
	return cae_pos[i][j];
      }
    }
  }
  return 0;
}


void RDCae::requestTimescale(int card)
{
  SendCommand(QString().sprintf("TS %d!",card));
}


bool RDCae::playPortActive(int card,int port,int except_stream)
{
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(cae_output_status_flags[card][port][i]&&(i!=except_stream)) {
      return true;
    }
  }
  return false;
}


void RDCae::setPlayPortActive(int card,int port,int stream)
{
  cae_output_status_flags[card][port][stream]=true;
}


void RDCae::connectJackPorts(const QString &out,const QString &in)
{
  SendCommand(QString("JC ")+out+" | "+in+"!");
}


void RDCae::disconnectJackPorts(const QString &out,const QString &in)
{
  SendCommand(QString("JD ")+out+" | "+in+"!");
}


void RDCae::readyData()
{
  readyData(0,0,"");
}


void RDCae::readyData(int *stream,int *handle,QString name)
{
  char buf[256];
  int c;
  RDCmdCache cmd;

  if(stream==NULL) {
    for(unsigned i=0;i<delayed_cmds.size();i++) {
      DispatchCommand(&delayed_cmds[i]);
    }
    delayed_cmds.clear();
  }

  while((c=cae_socket->readBlock(buf,256))>0) {
    buf[c]=0;
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(argnum<CAE_MAX_ARGS) {
	  args[argnum][argptr]=0;
	  argnum++;
	  argptr=0;
	}
	else {
	  if(debug) {
	    printf("Argument list truncated!\n");
	  }
	}
      }
      if(buf[i]=='!') {
	args[argnum++][argptr]=0;
	if(stream==NULL) {
	  cmd.load(args,argnum,argptr);
	  /*
	  // ************************************
	  printf("DISPATCHING: ");
	  for(int z=0;z<cmd.argNum();z++) {
	    printf(" %s",cmd.arg(z));
	  }
	  printf("\n");
	  // ************************************
	  */
	  DispatchCommand(&cmd);
	}
	else {
	  if(!strcmp(args[0],"LP")) {
	    if(QString(args[2])==name) {
	      sscanf(args[3],"%d",stream);
	      sscanf(args[4],"%d",handle);
	    }
	  }
	  else {
	    cmd.load(args,argnum,argptr);
	    delayed_cmds.push_back(cmd);
	  }
	}
	argnum=0;
	argptr=0;
	if(cae_socket==NULL) {
	  return;
	}
      }
      if((isgraph(buf[i]))&&(buf[i]!='!')) {
	if(argptr<CAE_MAX_LENGTH) {
	  args[argnum][argptr]=buf[i];
	  argptr++;
	}
	else {
	  if(debug) {
	    printf("WARNING: argument truncated!\n");
	  }
	}
      }
    }
  }
}


void RDCae::clockData()
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(cae_handle[i][j]>=0) {
	if(cae_output_positions[i][j]!=cae_pos[i][j]) {
	  emit playPositionChanged(cae_handle[i][j],
				   cae_output_positions[i][j]);
	  cae_pos[i][j]=cae_output_positions[i][j];
	}
      }
    }
  }
}


void RDCae::SendCommand(QString cmd)
{
  //printf("RDCae: SendCommand(%s)\n",(const char *)cmd);
  cae_socket->writeBlock((const char *)cmd,cmd.length());
}


void RDCae::DispatchCommand(RDCmdCache *cmd)
{
  int pos;
  int card;

  if(!strcmp(cmd->arg(0),"PW")) {   // Password Response
    if(cmd->arg(1)[0]=='+') {
      emit isConnected(true);
    }
    else {
      emit isConnected(false);
    }
  }

  if(!strcmp(cmd->arg(0),"LP")) {   // Load Play
    int handle=GetHandle(cmd->arg(4));
    int card=CardNumber(cmd->arg(1));
    int stream=StreamNumber(cmd->arg(3));
    syslog(LOG_ERR,"*** RDCae::DispatchCommand: received unhandled play stream from CAE, handle=%d, card=%d, stream=%d, name=\"%s\" ***",
	   handle,card,stream,cmd->arg(2));
    
    unloadPlay(handle);
  }

  if(!strcmp(cmd->arg(0),"UP")) {   // Unload Play
    if(cmd->arg(2)[0]=='+') {
      int handle=GetHandle(cmd->arg(1));
      for(int i=0;i<RD_MAX_CARDS;i++) {
	for(int j=0;j<RD_MAX_STREAMS;j++) {
	  if(cae_handle[i][j]==handle) {
	    cae_handle[i][j]=-1;
	    for(unsigned k=0;k<RD_MAX_PORTS;k++) {
	      cae_output_status_flags[i][k][j]=false;
	    }
	  }
	}
      }
      emit playUnloaded(handle);
    }
  }

  if(!strcmp(cmd->arg(0),"PP")) {   // Position Play
    if(cmd->arg(3)[0]=='+') {
      int handle=GetHandle(cmd->arg(1));
      sscanf(cmd->arg(2),"%u",&pos);
      for(int i=0;i<RD_MAX_CARDS;i++) {
	for(int j=0;j<RD_MAX_STREAMS;j++) {
	  if(cae_handle[i][j]==handle) {
	    //emit playPositionChanged(handle,cae_output_positions[i][j]);
	    emit playPositionChanged(handle,pos);
	  }
	}
      }
      emit playPositioned(handle,pos);
    }
  }

  if(!strcmp(cmd->arg(0),"PY")) {   // Play
    if(cmd->arg(4)[0]=='+') {
      emit playing(GetHandle(cmd->arg(1)));
    }
  }

  if(!strcmp(cmd->arg(0),"SP")) {   // Stop Play
    if(cmd->arg(2)[0]=='+') {
      emit playStopped(GetHandle(cmd->arg(1)));
    }
  }

  if(!strcmp(cmd->arg(0),"TS")) {   // Timescale Supported
    if(sscanf(cmd->arg(1),"%d",&card)==1) {
      if(cmd->arg(2)[0]=='+') {
	emit timescalingSupported(card,true);
      }
      else {
	emit timescalingSupported(card,false);
      }
    }
  }

  if(!strcmp(cmd->arg(0),"LR")) {   // Load Record
    if(cmd->arg(8)[0]=='+') {
      emit recordLoaded(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)));
    }
  }

  if(!strcmp(cmd->arg(0),"UR")) {   // Unload Record
    if(cmd->arg(4)[0]=='+') {
      emit recordUnloaded(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)),
			  QString(cmd->arg(3)).toUInt());
    }
  }

  if(!strcmp(cmd->arg(0),"RD")) {   // Record
  }

  if(!strcmp(cmd->arg(0),"RS")) {   // Record Start
    if(cmd->arg(3)[0]=='+') {
      emit recording(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)));
    }
  }

  if(!strcmp(cmd->arg(0),"SR")) {   // Stop Record
    if(cmd->arg(3)[0]=='+') {
      emit recordStopped(CardNumber(cmd->arg(1)),StreamNumber(cmd->arg(2)));
    }
  }

  if(!strcmp(cmd->arg(0),"IS")) {   // Input Status
    switch(cmd->arg(3)[0]) {
	case '0':
	  emit inputStatusChanged(CardNumber(cmd->arg(1)),
				  StreamNumber(cmd->arg(2)),true);
	  input_status[CardNumber(cmd->arg(1))][StreamNumber(cmd->arg(2))]=
	    true;
	  break;

	case '1':
	  emit inputStatusChanged(CardNumber(cmd->arg(1)),
				  StreamNumber(cmd->arg(2)),false);
	  input_status[CardNumber(cmd->arg(1))][StreamNumber(cmd->arg(2))]=
	    false;
	  break;
    }
  }
}


int RDCae::CardNumber(const char *arg)
{
  int n=-1;

  sscanf(arg,"%d",&n);
  return n;
}


int RDCae::StreamNumber(const char *arg)
{
  int n=-1;

  sscanf(arg,"%d",&n);
  return n;
}


int RDCae::GetHandle(const char *arg)
{
  int n=-1;

  sscanf(arg,"%d",&n);
  return n;
}


void RDCae::UpdateMeters()
{
  char msg[1501];
  int n;
  QStringList args;

  while((n=cae_meter_socket->readBlock(msg,1500))>0) {
    msg[n]=0;
    args=args.split(" ",msg);
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
      if(args.size()==5) {
	cae_stream_output_levels[args[1].toInt()][args[2].toInt()][0]=
	    args[3].toInt();
	cae_stream_output_levels[args[1].toInt()][args[2].toInt()][1]=
	    args[4].toInt();
      }
    }
    if(args[0]=="MP") {
      if(args.size()==4) {
	cae_output_positions[args[1].toInt()][args[2].toInt()]=args[3].toUInt();
      }
    }
  }
}
