// rossnkscp.cpp
//
// A Rivendell switcher driver for the Ross NK switchers via the SCP/A
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>

#include "globals.h"
#include "rossnkscp.h"

RossNkScp::RossNkScp(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  //
  // Get Matrix Parameters
  //
  ross_inputs=matrix->inputs();
  ross_outputs=matrix->outputs();
  ross_breakaway=matrix->card()+1;

  //
  // Initialize the TTY Port
  //
  RDTty *tty=new RDTty(rda->station()->name(),matrix->port(RDMatrix::Primary));
  ross_device=new RDTTYDevice();
  if(tty->active()) {
    ross_device->setName(tty->port());
    ross_device->setSpeed(tty->baudRate());
    ross_device->setWordLength(tty->dataBits());
    ross_device->setParity(tty->parity());
    ross_device->open(QIODevice::Unbuffered|QIODevice::ReadWrite);
  }
  delete tty;
}


RossNkScp::~RossNkScp()
{
  delete ross_device;
}


RDMatrix::Type RossNkScp::type()
{
  return RDMatrix::RossNkScp;
}


unsigned RossNkScp::gpiQuantity()
{
  return 0;
}


unsigned RossNkScp::gpoQuantity()
{
  return 0;
}


bool RossNkScp::primaryTtyActive()
{
  return true;
}


bool RossNkScp::secondaryTtyActive()
{
  return false;
}


void RossNkScp::processCommand(RDMacro *cmd)
{
  char str[11];

  switch(cmd->command()) {
      case RDMacro::ST:
	if((cmd->arg(1).toInt()<=0)||(cmd->arg(1).toInt()>ross_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>ross_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(str,"X%03d,%03d,%d\x0d",cmd->arg(2).toInt()-1,
		cmd->arg(1).toInt()-1,ross_breakaway);
	syslog(LOG_WARNING,"sent: %s\n",str);
	ross_device->writeBlock(str,11);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}
