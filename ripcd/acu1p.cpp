// acu1p.cpp
//
// Rivendell switcher driver for the Sine Systems ACU-1 (Prophet)
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: acu1p.cpp,v 1.1.2.2 2012/12/13 03:14:00 cvs Exp $
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

#include <globals.h>
#include <acu1p.h>


Acu1p::Acu1p(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  char str[9];

  //
  // Get Matrix Parameters
  //
  bt_matrix=matrix->matrix();
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();
  bt_gpis=matrix->gpis();
  bt_gpos=matrix->gpos();

  //
  // Initialize the TTY Port
  //
  RDTty *tty=new RDTty(rdstation->name(),matrix->port(RDMatrix::Primary));
  bt_device=new RDTTYDevice();
  if(tty->active()) {
    bt_device->setName(tty->port());
    bt_device->setSpeed(tty->baudRate());
    bt_device->setWordLength(tty->dataBits());
    bt_device->setParity(tty->parity());
    bt_device->open(IO_Raw|IO_ReadWrite);
  }
  bt_notify=new QSocketNotifier(bt_device->socket(),QSocketNotifier::Read,this);
  connect(bt_notify,SIGNAL(activated(int)),this,SLOT(readyReadData(int)));
  delete tty;

  //
  // Initialize Relays
  //
  bt_gpo_mask=0;
  str[0]=0xAA;
  str[1]=ACU1P_UNIT_ID;
  str[2]=0x38;
  str[3]=bt_gpo_mask;
  bt_device->writeBlock(str,4);

  //
  // Interval OneShots
  //
  bt_gpi_oneshot=new RDOneShot(this);
  connect(bt_gpi_oneshot,SIGNAL(timeout(int)),this,SLOT(gpiOneshotData(int)));
  bt_gpo_oneshot=new RDOneShot(this);
  connect(bt_gpo_oneshot,SIGNAL(timeout(int)),this,SLOT(gpoOneshotData(int)));

  //
  // Poll Timer
  //
  bt_poll_timer=new QTimer(this);
  connect(bt_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
  bt_poll_timer->start(ACU1P_POLL_INTERVAL);
}


Acu1p::~Acu1p()
{
  delete bt_notify;
  delete bt_device;
  delete bt_gpi_oneshot;
  delete bt_gpo_oneshot;
}


RDMatrix::Type Acu1p::type()
{
  return RDMatrix::Acu1p;
}


unsigned Acu1p::gpiQuantity()
{
  return bt_gpis;
}


unsigned Acu1p::gpoQuantity()
{
  return bt_gpos;
}


bool Acu1p::primaryTtyActive()
{
  return true;
}


bool Acu1p::secondaryTtyActive()
{
  return false;
}


void Acu1p::processCommand(RDMacro *cmd)
{
  char str[9];

  switch(cmd->command()) {
  case RDMacro::GO:
    if((cmd->argQuantity()!=5)||
       ((cmd->arg(1).toString().lower()!="i")&&
	(cmd->arg(1).toString().lower()!="o"))||
       (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>bt_gpos)||
       (cmd->arg(2).toInt()>bt_gpos)||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(1).toString().lower()!="i"))||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).toString().lower()=="i"))||
       (cmd->arg(4).toInt()<0)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(3).toInt()==0) {  // Turn OFF
      if(cmd->arg(4).toInt()==0) {
	if(cmd->arg(1).toString().lower()=="i") {
	  if(bt_gpi_state[cmd->arg(2).toInt()-1]) {
	    emit gpiChanged(bt_matrix,cmd->arg(2).toInt()-1,false);
	    bt_gpi_state[cmd->arg(2).toInt()-1]=false;
	  }
	  bt_gpi_mask[cmd->arg(2).toInt()-1]=true;
	}
	if(cmd->arg(1).toString().lower()=="o") {
	  SetRelay(cmd->arg(2).toInt()-1,false);
	  emit gpoChanged(bt_matrix,cmd->arg(2).toInt()-1,false);
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
      if(cmd->arg(3).toInt()==-1) {  // Clear input
	bt_gpi_mask[cmd->arg(2).toInt()-1]=false;
	bt_device->writeBlock("*0SPA",5);
      }
      else { 
	if(cmd->arg(4).toInt()==0) {  // Turn ON
	  if(cmd->arg(1).toString().lower()=="i") {
	    if(!bt_gpi_state[cmd->arg(2).toInt()-1]) {
	      emit gpiChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
	      bt_gpi_state[cmd->arg(2).toInt()-1]=true;
	    }
	    bt_gpi_mask[cmd->arg(2).toInt()-1]=true;
	  }
	  if(cmd->arg(1).toString().lower()=="o") {
	    SetRelay(cmd->arg(2).toInt()-1,true);
	    emit gpoChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
	  }
	}
	else {  // Pulse
	  if(cmd->arg(1).toString().lower()=="i") {
	    if(!bt_gpi_state[cmd->arg(2).toInt()-1]) {
	      emit gpiChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
	      bt_gpi_state[cmd->arg(2).toInt()-1]=true;
	    }
	    bt_gpi_mask[cmd->arg(2).toInt()-1]=true;
	    bt_gpi_oneshot->start(cmd->arg(2).toInt()-1,500);
	  }
	  if(cmd->arg(1).toString().lower()=="o") {
	    PulseRelay(cmd->arg(2).toInt()-1);
	    emit gpoChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
	    bt_gpo_oneshot->start(cmd->arg(2).toInt()-1,500);
	  }
	}
      }
    }
    if(cmd->echoRequested()) {
      cmd->acknowledge(true);
      emit rmlEcho(cmd);
    }
    break;

  case RDMacro::SA:
    if((cmd->arg(1).toInt()<=0)||(cmd->arg(1).toInt()>bt_inputs)||
       (cmd->arg(2).toInt()!=1)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    else {
      str[0]=0xAA;
      str[1]=ACU1P_UNIT_ID;
      str[2]=0x31;
      str[3]=0x01<<(cmd->arg(1).toInt()-1);
      bt_device->writeBlock(str,4);
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  case RDMacro::SR:
    if((cmd->arg(1).toInt()<=0)||(cmd->arg(1).toInt()>bt_inputs)||
       (cmd->arg(2).toInt()!=1)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    else {
      str[0]=0xAA;
      str[1]=ACU1P_UNIT_ID;
      str[2]=0x32;
      str[3]=0x01<<(cmd->arg(1).toInt()-1);
      bt_device->writeBlock(str,4);
    }
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  case RDMacro::ST:
    if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
       (cmd->arg(2).toInt()!=1)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    str[0]=0xAA;
    str[1]=ACU1P_UNIT_ID;
    str[2]=0x51;
    if(cmd->arg(1).toInt()==0) {
      str[3]=0x00;
      str[4]=0xFF;
    }
    else {
      str[3]=0x01<<(cmd->arg(1).toInt()-1);
      str[4]=~str[3];
    }
    bt_device->writeBlock(str,5);
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;
    
  default:
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
    break;
  }
}


void Acu1p::pollData()
{
  uint8_t data[]={0xAA,ACU1P_UNIT_ID,0x39};
  bt_device->writeBlock((char *)data,3);
}


void Acu1p::readyReadData(int sock)
{
  char data[255];
  int n=0;

  while((n=bt_device->readBlock(data,255))>0) {
    if(n==2) {
      ProcessGpi(256*(0xFF&data[1])+(0xFF&data[0]));
    }
  }
}


void Acu1p::gpiOneshotData(int value)
{
}


void Acu1p::gpoOneshotData(int value)
{
  emit gpoChanged(bt_matrix,value,false);
}


void Acu1p::ProcessGpi(uint16_t gpi_data)
{
  for(int i=0;i<ACU1P_GPIO_PINS;i++) {
    uint16_t mask=1<<i;
    if(((gpi_data&mask)!=0)&&(!bt_gpi_mask[i])) {
      emit gpiChanged(bt_matrix,i,true);
      bt_gpi_mask[i]=true;
    }
   if(((gpi_data&mask)==0)&&bt_gpi_mask[i]) {
      emit gpiChanged(bt_matrix,i,false);
      bt_gpi_mask[i]=false;
    }
  }
}


void Acu1p::SetRelay(int gpo,bool state)
{
  char str[9];
  uint8_t mask=1<<gpo;

  if(state) {
    bt_gpo_mask=bt_gpo_mask|mask;
  }
  else {
    bt_gpo_mask=bt_gpo_mask&(~mask);
  }
  str[0]=0xAA;
  str[1]=ACU1P_UNIT_ID;
  str[2]=0x38;
  str[3]=bt_gpo_mask;
  bt_device->writeBlock(str,4);
}


void Acu1p::PulseRelay(int gpo)
{
  char str[9];
  uint8_t mask=1<<gpo;

  str[0]=0xAA;
  str[1]=ACU1P_UNIT_ID;
  str[2]=0x52;
  str[3]=mask;
  bt_device->writeBlock(str,4);
}
