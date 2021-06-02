// am16.cpp
//
// A Rivendell switcher driver for the 360 Systems AM16
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include <rdapplication.h>
#include <rdconf.h>

#include "am16.h"
#include "globals.h"

Am16::Am16(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  unsigned sockopt;

  //
  // Get Matrix Parameters
  //
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();
  bt_midi_socket=-1;
  bt_notifier=NULL;
  bt_sysex_active=false;
  bt_data_ptr=0;

  if((bt_midi_socket=open(matrix->gpioDevice().toUtf8(),O_RDWR))<0) {
    rda->syslog(LOG_WARNING,"unable to open MIDI device at \"%s\"",
		matrix->gpioDevice().toUtf8().constData());
    return;
  }
  sockopt=O_NONBLOCK;
  fcntl(bt_midi_socket,F_SETFL,sockopt);

  bt_notifier=new QSocketNotifier(bt_midi_socket,QSocketNotifier::Read,this);
  connect(bt_notifier,SIGNAL(activated(int)),this,SLOT(readyReadData(int)));

  //
  // Timeout Timer
  //
  bt_timeout_timer=new QTimer(this);
  bt_timeout_timer->setSingleShot(true);
  connect(bt_timeout_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
}


Am16::~Am16()
{
  if(bt_notifier!=NULL) {
    delete bt_notifier;
  }
  if(bt_midi_socket>=0) {
    close(bt_midi_socket);
  }
}


RDMatrix::Type Am16::type()
{
  return RDMatrix::Am16;
}


unsigned Am16::gpiQuantity()
{
  return 0;
}


unsigned Am16::gpoQuantity()
{
  return 0;
}


bool Am16::primaryTtyActive()
{
  return false;
}


bool Am16::secondaryTtyActive()
{
  return false;
}


void Am16::processCommand(RDMacro *cmd)
{
  char data[1024];

  switch(cmd->command()) {
  case RDMacro::ST:
    if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
       (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>bt_outputs)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }

    //
    // Save the desired values
    //
    bt_pending_inputs.push_back(cmd->arg(1).toInt());
    bt_pending_outputs.push_back(cmd->arg(2).toInt());

    //
    // Request the current crosspoint map
    //
    if(bt_pending_inputs.size()==1) {
      data[0]=AM16_SYSEX_START;
      data[1]=0x00;
      data[2]=0x00;
      data[3]=AM16_SYSTEMS_ID;
      data[4]=AM16_DEVICE_NUMBER;
      data[5]=AM16_DEVICE_ADDRESS;
      data[6]=0x07;   // Request Program NN
      data[7]=AM16_PATCH_NUMBER;
      data[8]=AM16_SYSEX_END;
      RDCheckReturnCode("process command ST",write(bt_midi_socket,data,9),9);
      bt_timeout_timer->start(AM16_TIMEOUT_INTERVAL);
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  default:
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
    break;
  }
}


void Am16::readyReadData(int sock)
{
  char data[1024];
  int n;

  while((n=read(sock,data,1024))>0) {
    for(int i=0;i<n;i++) {
      if(bt_sysex_active) {
	bt_data_buffer[bt_data_ptr++]=data[i];
	if((0xFF&data[i])==AM16_SYSEX_END) {
	  ProcessMessage(bt_data_buffer,bt_data_ptr);
	  bt_data_ptr=0;
	  bt_sysex_active=false;
	}
      }
      else {
	if((0xFF&data[i])==AM16_SYSEX_START) {
	  bt_data_buffer[bt_data_ptr++]=data[i];
	  bt_sysex_active=true;
	}
      }
    }
  }
}


void Am16::timeoutData()
{
  rda->syslog(LOG_WARNING,
	 "AM16 driver: timed out waiting for crosspoint map, %lu event(s) lost",
	 bt_pending_inputs.size());
  bt_pending_inputs.clear();
  bt_pending_outputs.clear();
}


void Am16::ProcessMessage(char *msg,int len)
{
  QString str;

  if((len<7)||((0xFF&msg[3])!=AM16_SYSTEMS_ID)||
     ((0xFF&msg[4])!=AM16_DEVICE_NUMBER)||
     ((0xFF&msg[5])!=AM16_DEVICE_ADDRESS)) {
    return;
  }
  switch(0xFF&msg[6]) {
  case 0x08:   // Receive program map
    if(bt_pending_inputs.size()>0) {
      bt_timeout_timer->stop();

      //
      // Apply Changes
      //
      for(int i=(int)bt_pending_inputs.size()-1;i>=0;i--) {
	msg[7+bt_pending_outputs[i]]=0xFF&bt_pending_inputs[i];
      }
      bt_pending_inputs.clear();
      bt_pending_outputs.clear();

      //
      // Send to Programs
      //
      RDCheckReturnCode("ProcessMessage() write",
			write(bt_midi_socket,msg,len),len);
      msg[7]++;
      RDCheckReturnCode("ProcessMessage() write",
			write(bt_midi_socket,msg,len),len);

      //
      // Toggle Active Programs
      //
      msg[0]=0xC1;   // Channel 1
      msg[1]=AM16_PATCH_NUMBER+1;
      RDCheckReturnCode("ProcessMessage() write",
			write(bt_midi_socket,msg,2),2);
      msg[1]=AM16_PATCH_NUMBER;
      RDCheckReturnCode("ProcessMessage() write",
			write(bt_midi_socket,msg,2),2);
    }
    break;

  case 0x0B:   // ACK / NCK
    switch(0xFF&msg[7]) {
    case 0:
      // ACK -- command was successful!
      break;

    case 0x7E:
      rda->syslog(LOG_WARNING,"AM16 driver: data error");
      break;

    case 0x7F:
      rda->syslog(LOG_WARNING,
	  "AM16 driver: memory protect mode is on, cannot change crosspoints");
      break;

    default:
      rda->syslog(LOG_WARNING,"AM16 driver: received unknown ACK code [%d]",
		  0xFF&msg[7]);
      break;
    }
    break;

  default:
    for(int i=0;i<len;i++) {
      str+=QString().sprintf("%02X ",0xFF&msg[i]);
    }
    rda->syslog(LOG_DEBUG,
		"AM16 driver: received unrecognized MIDI message [%s]",
		str.toUtf8().constData());
    break;
  }
}
