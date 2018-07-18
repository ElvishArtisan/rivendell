// unity4000.cpp
//
// A Rivendell switcher driver for the UNITY4000
//
//   (C) Copyright 2002-2003,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddb.h>
#include <rdescape_string.h>

#include "globals.h"
#include "unity4000.h"

Unity4000::Unity4000(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Get Matrix Parameters
  //
  unity_inputs=matrix->inputs();
  unity_outputs=matrix->outputs();

  //
  // Load Feed Data
  //
  sql=QString("select ")+
    "NUMBER,"+        // 00
    "FEED_NAME,"+     // 01
    "CHANNEL_MODE "+  // 02
    "from INPUTS where "+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\" && "+
    QString().sprintf("MATRIX=%d ",matrix->matrix())+
    "order by NUMBER";
  q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<unity_inputs;i++) {
    unity_feed.push_back(UnityFeed());
    if(q->isValid()&&(q->value(0).toInt()==(i+1))) {
      unity_feed.back().setFeed(q->value(1).toString());
      unity_feed.back().setMode((RDMatrix::Mode)q->value(2).toInt());
      q->next();
    }
  }
  delete q;

  //
  // Initialize the TTY Port
  //
  RDTty *tty=new RDTty(rda->station()->name(),matrix->port(RDMatrix::Primary));
  unity_device=new RDTTYDevice();
  if(tty->active()) {
    unity_device->setName(tty->port());
    unity_device->setSpeed(tty->baudRate());
    unity_device->setWordLength(tty->dataBits());
    unity_device->setParity(tty->parity());
    unity_device->open(IO_Raw|IO_ReadWrite);
  }
  delete tty;
}


Unity4000::~Unity4000()
{
  delete unity_device;
}


RDMatrix::Type Unity4000::type()
{
  return RDMatrix::Unity4000;
}


unsigned Unity4000::gpiQuantity()
{
  return 0;
}


unsigned Unity4000::gpoQuantity()
{
  return 0;
}


bool Unity4000::primaryTtyActive()
{
  return true;
}


bool Unity4000::secondaryTtyActive()
{
  return false;
}


void Unity4000::processCommand(RDMacro *cmd)
{
  char str[256];
  char route;
  int input=cmd->arg(1).toInt()-1;
  int output=cmd->arg(2).toInt()-1;

  if((input<-1)||(input>=unity_inputs)||
     (output<0)||(output>=unity_outputs)) {
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
    return;
  }
  switch(unity_feed[input].mode()) {
      case RDMatrix::Left:
	route='1';
	break;
	
      case RDMatrix::Right:
	route='2';
	break;
	
      default:
	route='S';
	break;
  }
  switch(cmd->command()) {
      case RDMacro::ST:
	if(input==-1) {
	  sprintf(str,"\x0DMUTE A/%d\x0D",output+1);
	  unity_device->writeBlock(str,strlen(str));
	}
	else {
	  sprintf(str,"\x0DUNMUTE A/%d\x0D",output+1);
	  unity_device->writeBlock(str,strlen(str));
	  sprintf(str,"\x0DSETAUDIO %d %c %s\x0D",
		  output+1,
		  route,
		  (const char *)unity_feed[input].feed());
	  unity_device->writeBlock(str,strlen(str));
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
