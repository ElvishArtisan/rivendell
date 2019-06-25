// sas16000.cpp
//
// A Rivendell switcher driver for the SAS USI Protocol (2 digit)
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
#include <syslog.h>

#include <rdapplication.h>
#include <rddb.h>

#include "globals.h"
#include "sas16000.h"

Sas16000::Sas16000(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  RDTty *tty;
  sas_matrix=matrix->matrix();
  sas_ptr=0;

  //
  // Get Matrix Parameters
  //
  sas_inputs=matrix->inputs();
  sas_outputs=matrix->outputs();
  sas_gpis=matrix->gpis();
  sas_gpos=matrix->gpos();

  //
  // Initialize the connection
  //
  tty=new RDTty(rda->station()->name(),matrix->port(RDMatrix::Primary));
  sas_device=new RDTTYDevice();
  if(tty->active()) {
    sas_device->setName(tty->port());
    sas_device->setSpeed(tty->baudRate());
    sas_device->setWordLength(tty->dataBits());
    sas_device->setParity(tty->parity());
    sas_device->open(QIODevice::Unbuffered|QIODevice::ReadWrite);
  }
  delete tty;
}


RDMatrix::Type Sas16000::type()
{
  return RDMatrix::Sas16000;
}


unsigned Sas16000::gpiQuantity()
{
  return sas_gpis;
}


unsigned Sas16000::gpoQuantity()
{
  return sas_gpos;
}


bool Sas16000::primaryTtyActive()
{
  return true;
}


bool Sas16000::secondaryTtyActive()
{
  return false;
}


void Sas16000::processCommand(RDMacro *cmd)
{
  char str[256];

  switch(cmd->command()) {
  case RDMacro::ST:
    if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>sas_inputs)||
       (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    snprintf(str,256,"%c%02d%02d\x0D\x0A",20,
	     cmd->arg(1).toInt(),cmd->arg(2).toInt());
    SendCommand(str);
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  default:
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
    break;
  }
}


void Sas16000::SendCommand(char *str)
{
  rda->syslog(LOG_INFO,"sending USI cmd: %s",
	      (const char *)PrettifyCommand(str));

  sas_device->writeBlock(str,strlen(str));
}


QString Sas16000::PrettifyCommand(const char *cmd) const
{
  QString ret;
  if(cmd[0]<26) {
    ret=QString().sprintf("^%c%s",'@'+cmd[0],cmd+1);
  }
  else {
    ret=cmd;
  }
  return ret;
}
