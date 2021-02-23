// kernelgpio.cpp
//
// A Rivendell switcher driver for the Kernel GPIO interface. 
//
//   (C) Copyright 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "kernelgpio.h"

KernelGpio::KernelGpio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  printf("HERE\n");
  //
  // Get Matrix Parameters
  //
  gpio_matrix=matrix->matrix();
  gpio_gpis=matrix->gpis();
  gpio_gpos=matrix->gpos();

  //
  // Initialize the interface
  //
  gpio_reset_mapper=new QSignalMapper(this);
  connect(gpio_reset_mapper,SIGNAL(mapped(int)),this,SLOT(gpoResetData(int)));
  gpio_gpio=new RDKernelGpio(this);
  connect(gpio_gpio,SIGNAL(valueChanged(int,bool)),
	  this,SLOT(gpiChangedData(int,bool)));
  for(int i=0;i<gpio_gpis;i++) {
    gpio_gpio->addGpio(i);
    gpio_gpio->setDirection(i,RDKernelGpio::In);
    gpio_gpi_mask.push_back(false);
  }
  for(int i=0;i<gpio_gpos;i++) {
    gpio_gpio->addGpio(gpio_gpis+i);
    gpio_gpio->setDirection(gpio_gpis+i,RDKernelGpio::Out);
    gpio_gpi_mask.push_back(false);
    gpio_reset_states.push_back(false);
    gpio_reset_timers.push_back(new QTimer(this));
    gpio_reset_timers.back()->setSingleShot(true);
    connect(gpio_reset_timers.back(),SIGNAL(timeout()),
	    gpio_reset_mapper,SLOT(map()));
    gpio_reset_mapper->setMapping(gpio_reset_timers.back(),i);
  }

  //
  // Interval OneShot
  //
  gpio_gpi_oneshot=new RDOneShot(this);
  connect(gpio_gpi_oneshot,SIGNAL(timeout(int)),this,SLOT(gpiOneshotData(int)));
}


KernelGpio::~KernelGpio()
{
  for(unsigned i=0;i<gpio_reset_timers.size();i++) {
    delete gpio_reset_timers[i];
  }
  delete gpio_reset_mapper;
  delete gpio_gpio;
  delete gpio_gpi_oneshot;
}


RDMatrix::Type KernelGpio::type()
{
  return RDMatrix::KernelGpio;
}


unsigned KernelGpio::gpiQuantity()
{
  return gpio_gpis;
}


unsigned KernelGpio::gpoQuantity()
{
  return gpio_gpos;
}


bool KernelGpio::primaryTtyActive()
{
  return false;
}


bool KernelGpio::secondaryTtyActive()
{
  return false;
}


void KernelGpio::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
      case RDMacro::GO:
	if((cmd->argQuantity()!=5)||
	   ((cmd->arg(1).toLower()!="i")&&
	    (cmd->arg(1).toLower()!="o"))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>gpio_gpos)||
	   ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	    (cmd->arg(3).toInt()!=-1))||(cmd->arg(4).toInt()<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).toLower()=="i") {
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
	if(cmd->arg(1).toLower()=="o") {
	  if(cmd->arg(3).toInt()==0) {
	    gpio_gpio->setValue(gpio_gpis+cmd->arg(2).toInt()-1,false);
	    if(cmd->arg(4).toInt()>0) {
	      gpio_reset_states[cmd->arg(2).toInt()-1]=true;
	      gpio_reset_timers[cmd->arg(2).toInt()-1]->
		start(cmd->arg(4).toInt());
	    }
	  }
	  else {
	    if(cmd->arg(3).toInt()==1) {
	      gpio_gpio->setValue(gpio_gpis+cmd->arg(2).toInt()-1,true);
	      if(cmd->arg(4).toInt()>0) {
		gpio_reset_states[cmd->arg(2).toInt()-1]=false;
		gpio_reset_timers[cmd->arg(2).toInt()-1]->
		  start(cmd->arg(4).toInt());
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


void KernelGpio::gpiChangedData(int line,bool state)
{
  if(line<gpio_gpis) {
    emit gpiChanged(gpio_matrix,line,state);
  }
  else {
    emit gpoChanged(gpio_matrix,line-gpio_gpis,state);
  }
}


void KernelGpio::gpiOneshotData(int value)
{
  gpio_gpi_mask[value]=false;
  gpiChangedData(value,gpio_gpio->value(value));
}


void KernelGpio::gpoResetData(int line)
{
  gpio_gpio->setValue(gpio_gpis+line,gpio_reset_states[line]);
  emit gpoChanged(gpio_matrix,line,gpio_reset_states[line]);
}
