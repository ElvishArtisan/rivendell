// bt16x1.cpp
//
// A Rivendell switcher driver for the BroadcastTools 16x1
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

#include <stdlib.h>

#include <globals.h>
#include <bt16x1.h>

Bt16x1::Bt16x1(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  //
  // Get Matrix Parameters
  //
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();

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
}


Bt16x1::~Bt16x1()
{
  delete bt_device;
}


RDMatrix::Type Bt16x1::type()
{
  return RDMatrix::Bt16x1;
}


unsigned Bt16x1::gpiQuantity()
{
  return 0;
}


unsigned Bt16x1::gpoQuantity()
{
  return 0;
}


bool Bt16x1::primaryTtyActive()
{
  return true;
}


bool Bt16x1::secondaryTtyActive()
{
  return false;
}


void Bt16x1::processCommand(RDMacro *cmd)
{
  char str[9];

  switch(cmd->command()) {
      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()!=1)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).toInt()==0) {
	  sprintf(str,"*m\x0d");
	  bt_device->writeBlock(str,3);
	}
	else {
	  sprintf(str,"*%02d\x0d",cmd->arg(1).toInt());
	  bt_device->writeBlock(str,4);
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
