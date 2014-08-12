// sas32000.cpp
//
// A Rivendell switcher driver for the SAS32000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas32000.cpp,v 1.14.8.1 2013/03/03 22:58:22 cvs Exp $
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
#include <sas32000.h>


Sas32000::Sas32000(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  //
  // Get Matrix Parameters
  //
  sas_inputs=matrix->inputs();
  sas_outputs=matrix->outputs();

  //
  // Initialize the TTY Port
  //
  RDTty *tty=new RDTty(rdstation->name(),matrix->port(RDMatrix::Primary));
  sas_device=new RDTTYDevice();
  if(tty->active()) {
    sas_device->setName(tty->port());
    sas_device->setSpeed(tty->baudRate());
    sas_device->setWordLength(tty->dataBits());
    sas_device->setParity(tty->parity());
    sas_device->open(IO_Raw|IO_ReadWrite);
  }
  delete tty;

  //
  // Queue Timer
  //
  sas_timer=new QTimer(this,"sas_timer");
  connect(sas_timer,SIGNAL(timeout()),this,SLOT(runQueue()));
}


Sas32000::~Sas32000()
{
  delete sas_device;
}


RDMatrix::Type Sas32000::type()
{
  return RDMatrix::Sas32000;
}


unsigned Sas32000::gpiQuantity()
{
  return 0;
}


unsigned Sas32000::gpoQuantity()
{
  return 0;
}


bool Sas32000::primaryTtyActive()
{
  return true;
}


bool Sas32000::secondaryTtyActive()
{
  return false;
}


void Sas32000::processCommand(RDMacro *cmd)
{
  char str[9];
  char sign='1';

  switch(cmd->command()) {
      case RDMacro::ST:
	SendCommand(cmd,"DT1%02d%02d");
	break;

      case RDMacro::SA:
	SendCommand(cmd,"DS1%02d%02d");
	break;

      case RDMacro::SR:
	SendCommand(cmd,"DS0%02d%02d");
	break;

      case RDMacro::SL:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<SAS32000_MIN_GAIN)||
	   (cmd->arg(2).toInt()>SAS32000_MAX_GAIN)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	}
	if(cmd->arg(2).toInt()<0) {
	  sign='0';
	}
	sprintf(str,"DL%02d%c%03d",cmd->arg(1).toInt(),sign,
		10*abs(cmd->arg(2).toInt()));
	sas_device->writeBlock(str,8);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void Sas32000::runQueue()
{
  if(sas_commands.size()==0) {
    return;
  }
  sas_device->writeBlock((const char *)sas_commands.front(),7);
  sas_commands.pop();
  if(sas_commands.size()==0) {
    sas_timer->stop();
  }
}


void Sas32000::SendCommand(RDMacro *cmd,const char *format)
{
  char str[8];

  if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>sas_inputs)||
     (cmd->arg(2).toInt()<0)||(cmd->arg(2).toInt()>sas_outputs)) {
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
  }
  sprintf(str,format,cmd->arg(1).toInt(),cmd->arg(2).toInt());
  sas_commands.push(QString(str));
  if(!sas_timer->isActive()) {
    sas_timer->start(SAS32000_COMMAND_DELAY);
  }
  cmd->acknowledge(true);
  emit rmlEcho(cmd);
}
