// bt16x2.cpp
//
// A Rivendell switcher driver for the BroadcastTools 16x2
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qtimer.h>

#include <stdlib.h>

#include <globals.h>
#include <bt16x2.h>

Bt16x2::Bt16x2(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  //
  // Initialize Data Structures
  //
  bt_istate=0;
  for(int i=0;i<BT16X2_GPIO_PINS;i++) {
    bt_gpi_state[i]=false;
    bt_gpi_mask[i]=false;
  }

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
  RDTty *tty=new RDTty(rda->station()->name(),matrix->port(RDMatrix::Primary));
  bt_device=new RDTTYDevice();
  if(tty->active()) {
    bt_device->setName(tty->port());
    bt_device->setSpeed(tty->baudRate());
    bt_device->setWordLength(tty->dataBits());
    bt_device->setParity(tty->parity());
    bt_device->open(QIODevice::Unbuffered|QIODevice::ReadWrite);
  }
  delete tty;

  //
  // Interval OneShots
  //
  bt_gpi_oneshot=new RDOneShot(this);
  connect(bt_gpi_oneshot,SIGNAL(timeout(int)),this,SLOT(gpiOneshotData(int)));
  bt_gpo_oneshot=new RDOneShot(this);
  connect(bt_gpo_oneshot,SIGNAL(timeout(int)),this,SLOT(gpoOneshotData(int)));

  //
  // The Poll Timer
  //
  QTimer *timer=new QTimer(this,"poll_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(processStatus()));
  timer->start(BT16X2_POLL_INTERVAL);
}


Bt16x2::~Bt16x2()
{
  delete bt_device;
  delete bt_gpi_oneshot;
  delete bt_gpo_oneshot;
}


RDMatrix::Type Bt16x2::type()
{
  return RDMatrix::Bt16x2;
}


unsigned Bt16x2::gpiQuantity()
{
  return bt_gpis;
}


unsigned Bt16x2::gpoQuantity()
{
  return bt_gpos;
}


bool Bt16x2::primaryTtyActive()
{
  return true;
}


bool Bt16x2::secondaryTtyActive()
{
  return false;
}


void Bt16x2::processCommand(RDMacro *cmd)
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
	      sprintf(str,"*%dOR%02dF",BT16X2_UNIT_ID,cmd->arg(2).toInt());
	      bt_device->writeBlock(str,6);
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
		sprintf(str,"*%dOR%02dL",BT16X2_UNIT_ID,cmd->arg(2).toInt());
		bt_device->writeBlock(str,6);
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
		sprintf(str,"*%dOR%02dP",BT16X2_UNIT_ID,cmd->arg(2).toInt());
		bt_device->writeBlock(str,6);
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

      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>2)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).toInt()==0) {
	  sprintf(str,"*%dM%d",BT16X2_UNIT_ID,cmd->arg(2).toInt());
	  bt_device->writeBlock(str,4);
	}
	else {
	  sprintf(str,"*%d%02d%d",BT16X2_UNIT_ID,
		  cmd->arg(1).toInt(),cmd->arg(2).toInt());
	  bt_device->writeBlock(str,5);
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


void Bt16x2::processStatus()
{
  char buffer[256];
  int n;
  int gpi;

  while((n=bt_device->readBlock(buffer,255))>0) {
    for(int i=0;i<n;i++) {
      switch(bt_istate) {
	  case 0:
	    if(buffer[i]=='S') {
	      bt_istate=1;
	    }
	    break;

	  case 1:
	    if(buffer[i]==(BT16X2_UNIT_ID+'0')) {
	      bt_istate=2;
	    }
	    else {
	      bt_istate=0;
	    }
	    break;

	  case 2:
	    if(buffer[i]=='P') {
	      bt_istate=3;
	    }
	    else {
	      bt_istate=0;
	    }
	    break;

	  case 3:
	    if(buffer[i]==',') {
	      bt_istate=4;
	    }
	    else {
	      bt_istate=0;
	    }
	    break;

	  case 4:
	    if(buffer[i]=='A') {
	      bt_istate=5;
	    }
	    else {
	      bt_istate=0;
	    }
	    break;

	  case 5:
	  case 7:
	  case 9:
	  case 11:
	  case 13:
	  case 15:
	  case 17:
	  case 19:
	  case 21:
	  case 23:
	  case 25:
	  case 27:
	  case 29:
	  case 31:
	  case 33:
	  case 35:
	    if(buffer[i]==',') {
	      bt_istate++;
	    }
	    else {
	      bt_istate=0;
	    }
	    break;

	  case 6:
	  case 8:
	  case 10:
	  case 12:
	  case 14:
	  case 16:
	  case 18:
	  case 20:
	  case 22:
	  case 24:
	  case 26:
	  case 28:
	  case 30:
	  case 32:
	  case 34:
	  case 36:
	    if(buffer[i]=='0') {
	      gpi=(bt_istate-6)/2;
	      if(bt_gpi_state[gpi]&&(!bt_gpi_mask[gpi])) {
		emit gpiChanged(bt_matrix,gpi,false);
		bt_gpi_state[gpi]=false;
	      }
	      bt_istate++;
	    }
	    if(buffer[i]=='1') {
	      gpi=(bt_istate-6)/2;
	      if((!bt_gpi_state[gpi])&&(!bt_gpi_mask[gpi])) {
		emit gpiChanged(bt_matrix,gpi,true);
		bt_gpi_state[gpi]=true;
	      }
	      bt_istate++;
	    }
	    break;

	  default:
	    bt_istate=0;
      }
    }
  }
}


void Bt16x2::gpiOneshotData(int value)
{
  bt_gpi_mask[value]=false;
  bt_device->writeBlock("*0SPA",5);
}


void Bt16x2::gpoOneshotData(int value)
{
  emit gpoChanged(bt_matrix,value,false);
}
