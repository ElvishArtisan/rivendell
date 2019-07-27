// btss41mlr.cpp
//
// A Rivendell switcher driver for the BroadcastTools SS 4.1 MLR
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qstringlist.h>
#include <qtimer.h>

#include <rdapplication.h>

#include "btss41mlr.h"
#include "globals.h"

BtSs41Mlr::BtSs41Mlr(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  //
  // Initialize Data Structures
  //
  bt_istate=0;
  for(int i=0;i<BTSS41MLR_GPIO_PINS;i++) {
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
  connect(timer,SIGNAL(timeout()),this,SLOT(readyReadData()));
  timer->start(BTSS41MLR_POLL_INTERVAL);
}


BtSs41Mlr::~BtSs41Mlr()
{
  delete bt_device;
  delete bt_gpi_oneshot;
  delete bt_gpo_oneshot;
}


RDMatrix::Type BtSs41Mlr::type()
{
  return RDMatrix::BtSs41Mlr;
}


unsigned BtSs41Mlr::gpiQuantity()
{
  return bt_gpis;
}


unsigned BtSs41Mlr::gpoQuantity()
{
  return bt_gpos;
}


bool BtSs41Mlr::primaryTtyActive()
{
  return true;
}


bool BtSs41Mlr::secondaryTtyActive()
{
  return false;
}


void BtSs41Mlr::processCommand(RDMacro *cmd)
{
  QString str;

  switch(cmd->command()) {
      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>1)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).toInt()==0) {
	  str=QString().sprintf("*%dMA",BTSS41MLR_UNIT_ID);
	  bt_device->write(str,str.length());
	}
	else {
	  str=QString().sprintf("*%d%02d",BTSS41MLR_UNIT_ID,
				cmd->arg(1).toInt());
	  bt_device->write(str,str.length());
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


void BtSs41Mlr::readyReadData()
{
  char buffer[256];
  int n;

  while((n=bt_device->read(buffer,255))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&buffer[i]) {
      case 10:
	ProcessStatus(bt_accum);
	bt_accum="";
	break;

      case 13:
	break;

      default:
	bt_accum+=buffer[i];
	break;
      }
    }
  }
}


void BtSs41Mlr::gpiOneshotData(int value)
{
  bt_gpi_mask[value]=false;
  bt_device->write("*0SPA",5);
}


void BtSs41Mlr::gpoOneshotData(int value)
{
  emit gpoChanged(bt_matrix,value,false);
}


void BtSs41Mlr::ProcessStatus(const QString &msg)
{
  QStringList f0=msg.split(",");

  if(f0.size()==10) {
    if((f0[0]==QString().sprintf("S%dP",BTSS41MLR_UNIT_ID))&&(f0[1]=="A")) {
      for(int i=0;i<bt_gpis;i++) {
	if(f0[2+i]=="0") {
	  if(bt_gpi_state[i]&&(!bt_gpi_mask[i])) {
	    emit gpiChanged(bt_matrix,i,false);
	    bt_gpi_state[i]=false;
	  }
	}
	if(f0[2+i]=="1") {
	  if(!bt_gpi_state[i]&&(!bt_gpi_mask[i])) {
	    emit gpiChanged(bt_matrix,i,true);
	    bt_gpi_state[i]=true;
	  }
	}
      }
    }
  }
}
