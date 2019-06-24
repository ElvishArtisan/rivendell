// modemlines.cpp
//
// A Rivendell switcher driver for using TTY modem lines for GPIO
//
//   (C) Copyright 2015-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rdapplication.h>

#include "globals.h"
#include "modemlines.h"

ModemLines::ModemLines(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  gpio_poll_state=0;
  gpio_poll_timer=NULL;
  gpio_gpi_oneshot=NULL;
  gpio_gpi_table[0]=TIOCM_DSR;
  gpio_gpi_table[1]=TIOCM_CTS;
  gpio_gpi_table[2]=TIOCM_CAR;
  gpio_gpi_table[3]=TIOCM_RNG;
  gpio_gpo_table[0]=TIOCM_DTR;
  gpio_gpo_table[1]=TIOCM_RTS;
  for(int i=0;i<4;i++) {
    gpio_gpi_mask[i]=false;
  }

  //
  // Get Matrix Parameters
  //
  gpio_matrix=matrix->matrix();
  gpio_gpis=matrix->gpis();
  gpio_gpos=matrix->gpos();

  //
  // Open TTY
  //
  gpio_tty=new RDTty(rda->station()->name(),matrix->port(RDMatrix::Primary));
  if((gpio_fd=open(gpio_tty->port(),O_RDONLY))<0) {
    rda->syslog(LOG_WARNING,"unable to open tty \"%s\"",
		(const char *)gpio_tty->port());
    return;
  }
  for(int i=0;i<gpio_gpos;i++) {  // So we don't false trigger
    ioctl(gpio_fd,TIOCMBIC,&gpio_gpo_table[i]);
  }
  gpio_poll_timer=new QTimer(this);
  connect(gpio_poll_timer,SIGNAL(timeout()),this,SLOT(pollTtyData()));
  gpio_poll_timer->start(MODEMLINES_POLL_INTERVAL);

  //
  // Interval OneShot
  //
  gpio_gpi_oneshot=new RDOneShot(this);
  connect(gpio_gpi_oneshot,SIGNAL(timeout(int)),this,SLOT(gpiOneshotData(int)));

  //
  // GPO Timers
  //
  gpio_gpo_mapper=new QSignalMapper(this);
  connect(gpio_gpo_mapper,SIGNAL(mapped(int)),this,SLOT(gpoResetData(int)));
  for(int i=0;i<gpio_gpos;i++) {
    gpio_gpo_timers[i]=new QTimer(this);
    gpio_gpo_mapper->setMapping(gpio_gpo_timers[i],i);
    connect(gpio_gpo_timers[i],SIGNAL(timeout()),gpio_gpo_mapper,SLOT(map()));
    gpio_gpo_pending_states[i]=false;
  }
}


ModemLines::~ModemLines()
{
  if(gpio_fd>=0) {
    close(gpio_fd);
    delete gpio_poll_timer;
    delete gpio_gpi_oneshot;
    delete gpio_gpo_mapper;
    delete gpio_gpo_timers[0];
    delete gpio_gpo_timers[1];
  }
  delete gpio_tty;
}


RDMatrix::Type ModemLines::type()
{
  return RDMatrix::ModemLines;
}


unsigned ModemLines::gpiQuantity()
{
  return gpio_gpis;
}


unsigned ModemLines::gpoQuantity()
{
  return gpio_gpos;
}


bool ModemLines::primaryTtyActive()
{
  return true;
}


bool ModemLines::secondaryTtyActive()
{
  return false;
}


void ModemLines::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
      case RDMacro::GO:
	if((gpio_fd<0)||(cmd->argQuantity()!=5)||
	   ((cmd->arg(1).lower()!="i")&&
	    (cmd->arg(1).lower()!="o"))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>gpio_gpos)||
	   ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	    (cmd->arg(3).toInt()!=-1))||(cmd->arg(4).toInt()<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).lower()=="i") {
	  if(cmd->arg(3).toInt()==0) {
	    emit gpiChanged(gpio_matrix,cmd->arg(2).toInt()-1,false);
	    gpio_gpi_mask[cmd->arg(2).toInt()-1]=true;
	    if(cmd->arg(4).toInt()>0) {
	      gpio_gpi_oneshot->
		start(cmd->arg(2).toInt()-1,cmd->arg(4).toInt());
	    }
	  }
	  else {
	    if(cmd->arg(3).toInt()==1) {
	      emit gpiChanged(gpio_matrix,cmd->arg(2).toInt()-1,true);
	      gpio_gpi_mask[cmd->arg(2).toInt()-1]=true;
	      if(cmd->arg(4).toInt()>0) {
		gpio_gpi_oneshot->
		  start(cmd->arg(2).toInt()-1,cmd->arg(4).toInt());
	      }
	    }
	    else {
	      gpiOneshotData(cmd->arg(2).toInt()-1);
	    }
	  }
	  cmd->acknowledge(true);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).lower()=="o") {
	  if(cmd->arg(3).toInt()==0) {
	    ioctl(gpio_fd,TIOCMBIC,&gpio_gpo_table[cmd->arg(2).toInt()-1]);
	    if(cmd->arg(4).toInt()>0) {
	      gpio_gpo_timers[cmd->arg(2).toInt()-1]->
		start(cmd->arg(4).toInt(),true);
	      gpio_gpo_pending_states[cmd->arg(2).toInt()-1]=true;
	    }
	  }
	  else {
	    if(cmd->arg(3).toInt()==1) {
	      ioctl(gpio_fd,TIOCMBIS,&gpio_gpo_table[cmd->arg(2).toInt()-1]);
	      if(cmd->arg(4).toInt()>0) {
		gpio_gpo_timers[cmd->arg(2).toInt()-1]->
		  start(cmd->arg(4).toInt(),true);
		gpio_gpo_pending_states[cmd->arg(2).toInt()-1]=false;
	      }
	    }
	    else {
	      cmd->acknowledge(false);
	      emit rmlEcho(cmd);
	      return;
	    }
	  }
	  cmd->acknowledge(true);
	  emit rmlEcho(cmd);
	  return;
	}
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void ModemLines::pollTtyData()
{
  int lines=0;

  if(ioctl(gpio_fd,TIOCMGET,&lines)==0) {
    if(lines==gpio_poll_state) {
      return;
    }
    for(int i=0;i<gpio_gpis;i++) {
      if((lines&gpio_gpi_table[i])!=(gpio_poll_state&gpio_gpi_table[i])) {
	emit gpiChanged(gpio_matrix,i,(lines&gpio_gpi_table[i])!=0);
      }
    }
    gpio_poll_state=lines;
  }
}


void ModemLines::gpiOneshotData(int value)
{
  int lines=0;

  gpio_gpi_mask[value]=false;
  if(ioctl(gpio_fd,TIOCMGET,&lines)==0) {
    emit gpiChanged(gpio_matrix,value,(lines&gpio_gpi_table[value])!=0);
  }
}


void ModemLines::gpoResetData(int gpo)
{
  if(gpio_gpo_pending_states[gpo]) {
    ioctl(gpio_fd,TIOCMBIS,&gpio_gpo_table[gpo]);
  }
  else {
    ioctl(gpio_fd,TIOCMBIC,&gpio_gpo_table[gpo]);
  }
}
