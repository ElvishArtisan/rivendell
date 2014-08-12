// btss124.cpp
//
// A Rivendell switcher driver for the BroadcastTools SS 12.4
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: btss124.cpp,v 1.11 2010/08/03 23:39:25 cvs Exp $
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
#include <btss124.h>


BtSs124::BtSs124(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  //
  // Get Matrix Parameters
  //
  bt_matrix=matrix->matrix();
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();

  //
  // Initialize the TTY Port
  //
  RDTty *tty=new RDTty(rdstation->name(),matrix->port(RDMatrix::Primary));
  bt_device=new RDTTYDevice();
  if(tty->active()) {
    bt_device->setName(tty->port());
    bt_device->setSpeed(tty->baudRate());
    bt_device->setWordLength(tty->dataBits());
    bt_device->setParity(tty->parity());
    bt_device->open(IO_Raw|IO_ReadWrite);
  }
  delete tty;
}


BtSs124::~BtSs124()
{
  delete bt_device;
}


RDMatrix::Type BtSs124::type()
{
  return RDMatrix::BtSs124;
}


unsigned BtSs124::gpiQuantity()
{
  return 0;
}


unsigned BtSs124::gpoQuantity()
{
  return 0;
}


bool BtSs124::primaryTtyActive()
{
  return true;
}


bool BtSs124::secondaryTtyActive()
{
  return false;
}


void BtSs124::processCommand(RDMacro *cmd)
{
  char str[9];

  switch(cmd->command()) {
      case RDMacro::SA:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>4)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(str,"*%d%02d%d",BTSS124_UNIT_ID,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	bt_device->writeBlock(str,5);
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
	sprintf(str,"*%d%02dM%d",BTSS124_UNIT_ID,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	bt_device->writeBlock(str,6);
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
	  sprintf(str,"*%dM%d",BTSS124_UNIT_ID,cmd->arg(2).toInt());
	  bt_device->writeBlock(str,4);
	}
	else {
	  sprintf(str,"*%d%02d%d",BTSS124_UNIT_ID,
		  cmd->arg(1).toInt(),cmd->arg(2).toInt());
	  bt_device->writeBlock(str,5);
	  for(int i=1;i<cmd->arg(1).toInt();i++) {
	    sprintf(str,"*%d%02dM%d",BTSS124_UNIT_ID,
		    i,cmd->arg(2).toInt());
	    bt_device->writeBlock(str,6);
	  }
	  for(int i=cmd->arg(1).toInt()+1;i<9;i++) {
	    sprintf(str,"*%d%02dM%d",BTSS124_UNIT_ID,
		    i,cmd->arg(2).toInt());
	    bt_device->writeBlock(str,6);
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
