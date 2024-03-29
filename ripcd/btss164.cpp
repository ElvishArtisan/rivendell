// btss164.cpp
//
// A Rivendell switcher driver for the BroadcastTools SS 16.4
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

#include <rdapplication.h>

#include "btss164.h"
#include "globals.h"

BtSs164::BtSs164(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  //
  // Initialize Data Structures
  //
  bt_istate=0;
  for(int i=0;i<BTSS164_GPIO_PINS;i++) {
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
  connect(bt_device,SIGNAL(readyRead()),this,SLOT(processStatus()));
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
}


RDMatrix::Type BtSs164::type()
{
  return RDMatrix::BtSs164;
}


unsigned BtSs164::gpiQuantity()
{
  return bt_gpis;
}


unsigned BtSs164::gpoQuantity()
{
  return bt_gpos;
}


bool BtSs164::primaryTtyActive()
{
  return true;
}


bool BtSs164::secondaryTtyActive()
{
  return false;
}


BtSs164::~BtSs164()
{
  delete bt_device;
  delete bt_gpi_oneshot;
  delete bt_gpo_oneshot;
}


void BtSs164::processCommand(RDMacro *cmd)
{
  char str[256];

  switch(cmd->command()) {
      case RDMacro::GO:
	if((cmd->argQuantity()!=5)||
	   ((cmd->arg(1).toLower()!="i")&&
	    (cmd->arg(1).toLower()!="o"))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>bt_gpos)||
	   (cmd->arg(2).toInt()>bt_gpos)||
	   ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	    (cmd->arg(1).toLower()!="i"))||
	   ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	    (cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).toLower()=="i"))||
	   (cmd->arg(4).toInt()<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(3).toInt()==0) {  // Turn OFF
	  if(cmd->arg(4).toInt()==0) {
	    if(cmd->arg(1).toLower()=="i") {
	      if(bt_gpi_state[cmd->arg(2).toInt()-1]) {
		emit gpiChanged(bt_matrix,cmd->arg(2).toInt()-1,false);
		bt_gpi_state[cmd->arg(2).toInt()-1]=false;
	      }
	      bt_gpi_mask[cmd->arg(2).toInt()-1]=true;
	    }
	    if(cmd->arg(1).toLower()=="o") {
	      if(cmd->arg(2).toInt()<8) {
		sprintf(str,"*%dOR%dF",BTSS164_UNIT_ID,cmd->arg(2).toInt());
		bt_device->write(str,6);
	      }
	      else {
		sprintf(str,"*%dOOR%02dF",BTSS164_UNIT_ID,
			cmd->arg(2).toInt()-8);
		bt_device->write(str,8);
	      }
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
	    bt_device->write("*0SPA",5);
	  }
	  else { 
	    if(cmd->arg(4).toInt()==0) {  // Turn ON
	      if(cmd->arg(1).toLower()=="i") {
		if(!bt_gpi_state[cmd->arg(2).toInt()-1]) {
		  emit gpiChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
		  bt_gpi_state[cmd->arg(2).toInt()-1]=true;
		}
		bt_gpi_mask[cmd->arg(2).toInt()-1]=true;
	      }
	      if(cmd->arg(1).toLower()=="o") {
		if(cmd->arg(2).toInt()<8) {
		  sprintf(str,"*%dOR%dL",BTSS164_UNIT_ID,cmd->arg(2).toInt());
		  bt_device->write(str,6);
		}
		else {
		  sprintf(str,"*%dOOR%02dL",BTSS164_UNIT_ID,
			  cmd->arg(2).toInt()-8);
		  bt_device->write(str,8);
		}
		emit gpoChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
	      }
	    }
	    else {  // Pulse
	      if(cmd->arg(1).toLower()=="i") {
		if(!bt_gpi_state[cmd->arg(2).toInt()-1]) {
		  emit gpiChanged(bt_matrix,cmd->arg(2).toInt()-1,true);
		  bt_gpi_state[cmd->arg(2).toInt()-1]=true;
		}
		bt_gpi_mask[cmd->arg(2).toInt()-1]=true;
		bt_gpi_oneshot->start(cmd->arg(2).toInt()-1,500);
	      }
	      if(cmd->arg(1).toLower()=="o") {
		if(cmd->arg(2).toInt()<8) {
		  sprintf(str,"*%dOR%dP",BTSS164_UNIT_ID,cmd->arg(2).toInt());
		  bt_device->write(str,6);
		}
		else {
		  sprintf(str,"*%dOOR%02dP",BTSS164_UNIT_ID,
			  cmd->arg(2).toInt()-8);
		  bt_device->write(str,8);
		}
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
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>4)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(str,"*%d%02d%d",BTSS164_UNIT_ID,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	bt_device->write(str,5);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SR:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>4)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(str,"*%d%02dM%d",BTSS164_UNIT_ID,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	bt_device->write(str,6);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>4)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).toInt()==0) {
	  sprintf(str,"*%dM%d",BTSS164_UNIT_ID,cmd->arg(2).toInt());
	  bt_device->write(str,4);
	}
	else {
	  sprintf(str,"*%dM%d",BTSS164_UNIT_ID,cmd->arg(2).toInt());
	  bt_device->write(str,4);
	  sprintf(str,"*%d%02d%d",BTSS164_UNIT_ID,
		  cmd->arg(1).toInt(),cmd->arg(2).toInt());
	  bt_device->write(str,5);
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


void BtSs164::processStatus()
{
  char buffer[256];
  int n;
  int gpi;

  while((n=bt_device->read(buffer,255))>0) {
    for(int i=0;i<n;i++) {
      switch(bt_istate) {
	  case 0:
	    if(buffer[i]=='S') {
	      bt_istate=1;
	    }
	    break;

	  case 1:
	    if(buffer[i]==(BTSS164_UNIT_ID+'0')) {
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
	  case 37:
	  case 39:
	  case 41:
	  case 43:
	  case 45:
	  case 47:
	  case 49:
	  case 51:
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
	  case 38:
	  case 40:
	  case 42:
	  case 44:
	  case 46:
	  case 48:
	  case 50:
	  case 52:
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


void BtSs164::gpiOneshotData(int value)
{
  bt_gpi_mask[value]=false;
  bt_device->write("*0SPA",5);
}


void BtSs164::gpoOneshotData(int value)
{
  emit gpoChanged(bt_matrix,value,false);
}
