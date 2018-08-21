// btss21.cpp
//
// A Rivendell switcher driver for the BroadcastTools SS 2.1
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

#include <qstringlist.h>
#include <qtimer.h>

#include <stdlib.h>

#include <globals.h>
#include <btss21.h>


BtSs21::BtSs21(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
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


BtSs21::~BtSs21()
{
  delete bt_device;
}


RDMatrix::Type BtSs21::type()
{
  return RDMatrix::BtSs21;
}


unsigned BtSs21::gpiQuantity()
{
  return 0;
}


unsigned BtSs21::gpoQuantity()
{
  return 0;
}


bool BtSs21::primaryTtyActive()
{
  return true;
}


bool BtSs21::secondaryTtyActive()
{
  return false;
}


void BtSs21::processCommand(RDMacro *cmd)
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
      str=QString().sprintf("*%dM",BTSS21_UNIT_ID);
      bt_device->writeBlock(str,str.length());
    }
    else {
      str=QString().sprintf("*%d%d",BTSS21_UNIT_ID,
			    cmd->arg(1).toInt());
      bt_device->writeBlock(str,str.length());
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
