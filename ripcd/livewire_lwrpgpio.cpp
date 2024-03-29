// livewire_lwrpgpio.cpp
//
// A Rivendell LWRP GPIO driver for LiveWire networks.
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddb.h>
#include <rdescape_string.h>

#include "livewire_lwrpgpio.h"

LiveWireLwrpGpio::LiveWireLwrpGpio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  gpio_is_virtual=matrix->layer()=='V';

  //
  // Node Interface
  //
  gpio_livewire=new RDLiveWire(0,this);
  if(gpio_is_virtual) {
    connect(gpio_livewire,SIGNAL(gpiChanged(unsigned,unsigned,unsigned,bool)),
	    this,SLOT(gpiChangedData(unsigned,unsigned,unsigned,bool)));
    connect(gpio_livewire,SIGNAL(gpoChanged(unsigned,unsigned,unsigned,bool)),
	    this,SLOT(gpoChangedData(unsigned,unsigned,unsigned,bool)));
  }
  else {
    connect(gpio_livewire,SIGNAL(gpiChanged(unsigned,unsigned,unsigned,bool)),
	    this,SLOT(gpoChangedData(unsigned,unsigned,unsigned,bool)));
    connect(gpio_livewire,SIGNAL(gpoChanged(unsigned,unsigned,unsigned,bool)),
	    this,SLOT(gpiChangedData(unsigned,unsigned,unsigned,bool)));
  }
  connect(gpio_livewire,SIGNAL(connected(unsigned)),
	  this,SLOT(connectedData(unsigned)));
  connect(gpio_livewire,SIGNAL(watchdogStateChanged(unsigned,const QString &)),
	  this,SLOT(watchdogStateChangedData(unsigned,const QString &)));
  gpio_livewire->connectToHost(matrix->ipAddress(RDMatrix::Primary).toString(),
			       RD_LIVEWIRE_DEFAULT_TCP_PORT,
			       matrix->password(RDMatrix::Primary),0);
}


LiveWireLwrpGpio::~LiveWireLwrpGpio()
{
  delete gpio_livewire;
}


RDMatrix::Type LiveWireLwrpGpio::type()
{
  return RDMatrix::LiveWireLwrpGpio;
}


unsigned LiveWireLwrpGpio::gpiQuantity()
{
  return RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpis();
}


unsigned LiveWireLwrpGpio::gpoQuantity()
{
  return RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpos();
}


bool LiveWireLwrpGpio::primaryTtyActive()
{
  return false;
}


bool LiveWireLwrpGpio::secondaryTtyActive()
{
  return false;
}


void LiveWireLwrpGpio::processCommand(RDMacro *cmd)
{
  int slot;
  int line;

  switch(cmd->command()) {
  case RDMacro::GO:
    if((cmd->argQuantity()!=5)||
       (cmd->arg(2).toInt()<1)||
       ((cmd->arg(1).toLower()!="i")&&
	(cmd->arg(1).toLower()!="o"))) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(((cmd->arg(1).toLower()=="i")&&
	(cmd->arg(2).toUInt()>gpio_gpi_limit))||
       ((cmd->arg(1).toLower()=="o")&&
	(cmd->arg(2).toUInt()>gpio_gpo_limit))) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    slot=(cmd->arg(2).toInt()-1)/5;
    line=(cmd->arg(2).toInt()-1)%5;
    if(cmd->arg(1).toLower()=="i") {
      if(cmd->arg(3).toInt()==0) {
	if(gpio_is_virtual) {
	  gpio_livewire->gpoReset(slot,line,cmd->arg(4).toInt());
	}
	else {
	  gpio_livewire->gpiReset(slot,line,cmd->arg(4).toInt());
	}
      }
      else {
	if(gpio_is_virtual) {
	  gpio_livewire->gpoSet(slot,line,cmd->arg(4).toInt());
	}
	else {
	  gpio_livewire->gpiSet(slot,line,cmd->arg(4).toInt());
	}
      }
    }
    if(cmd->arg(1).toLower()=="o") {
      if(cmd->arg(3).toInt()==0) {
	if(gpio_is_virtual) {
	  gpio_livewire->gpiReset(slot,line,cmd->arg(4).toInt());
	}
	else {
	  gpio_livewire->gpoReset(slot,line,cmd->arg(4).toInt());
	}
      }
      else {
	if(gpio_is_virtual) {
	  gpio_livewire->gpiSet(slot,line,cmd->arg(4).toInt());
	}
	else {
	  gpio_livewire->gpoSet(slot,line,cmd->arg(4).toInt());
	}
      }
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


void LiveWireLwrpGpio::gpiChangedData(unsigned id,unsigned slot,unsigned line,
				     bool state)
{
  emit gpoChanged(matrixNumber(),slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line,state);
}


void LiveWireLwrpGpio::gpoChangedData(unsigned id,unsigned slot,unsigned line,
				     bool state)
{
  emit gpiChanged(matrixNumber(),slot*RD_LIVEWIRE_GPIO_BUNDLE_SIZE+line,state);
}


void LiveWireLwrpGpio::connectedData(unsigned id)
{
  QString sql;

  sql=QString::asprintf("update `MATRICES` set `GPIS`=%u,`GPOS`=%u ",
			RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpis(),
			RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpos())+
    "where (`STATION_NAME`='"+RDEscapeString(stationName())+"')&&"+
    QString::asprintf("(`MATRIX`=%u)",matrixNumber());
  RDSqlQuery::apply(sql);

  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpis();i++) {
    insertGpioEntry(false,i+1);
  }
  for(int i=0;i<RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpos();i++) {
    insertGpioEntry(true,i+1);
  }
  if(gpio_is_virtual) {
    gpio_gpi_limit=RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpos();
    gpio_gpo_limit=RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpis();
  }
  else {
    gpio_gpi_limit=RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpis();
    gpio_gpo_limit=RD_LIVEWIRE_GPIO_BUNDLE_SIZE*gpio_livewire->gpos();
  }

  rda->syslog(LOG_DEBUG,"livewire LWRP gpio driver connected to %s at %s:%d",
	      gpio_livewire->deviceName().toUtf8().constData(),
	      gpio_livewire->hostname().toUtf8().constData(),
	      0xFFFF&gpio_livewire->tcpPort());
}


void LiveWireLwrpGpio::watchdogStateChangedData(unsigned id,const QString &msg)
{
  rda->syslog(LOG_WARNING,
	      "livewire LWRP driver watchdog update for device %s at %s:%d: %s",
	      gpio_livewire->deviceName().toUtf8().constData(),
	      gpio_livewire->hostname().toUtf8().constData(),
	      0xFFFF&gpio_livewire->tcpPort(),
	      msg.toUtf8().constData());
}
