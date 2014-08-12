// sas64000.cpp
//
// A Rivendell switcher driver for the SAS64000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas64000.cpp,v 1.12.8.1 2013/03/03 23:30:16 cvs Exp $
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
#include <sas64000.h>


Sas64000::Sas64000(RDMatrix *matrix,QObject *parent,const char *name)
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
}


Sas64000::~Sas64000()
{
  delete sas_device;
}


RDMatrix::Type Sas64000::type()
{
  return RDMatrix::Sas64000;
}


unsigned Sas64000::gpiQuantity()
{
  return 0;
}


unsigned Sas64000::gpoQuantity()
{
  return 0;
}


bool Sas64000::primaryTtyActive()
{
  return true;
}


bool Sas64000::secondaryTtyActive()
{
  return false;
}


void Sas64000::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
      case RDMacro::ST:
	SendCommand(cmd,"\x54%03d%03d");
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void Sas64000::SendCommand(RDMacro *cmd,const char *format)
{
  char str[8];

  if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>sas_inputs)||
     (cmd->arg(2).toInt()<0)||(cmd->arg(2).toInt()>sas_outputs)) {
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
  }
  sprintf(str,format,cmd->arg(1).toInt(),cmd->arg(2).toInt());
  sas_device->writeBlock(str,7);
  cmd->acknowledge(true);
  emit rmlEcho(cmd);
}
