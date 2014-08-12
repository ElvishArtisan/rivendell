// unity4000.cpp
//
// A Rivendell switcher driver for the UNITY4000
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: unity4000.cpp,v 1.13 2010/08/03 23:39:26 cvs Exp $
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

#include <rddb.h>

#include <globals.h>
#include <unity4000.h>


Unity4000::Unity4000(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
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
  sql=QString().sprintf("select NUMBER,FEED_NAME,CHANNEL_MODE from INPUTS \
                         where STATION_NAME=\"%s\" && MATRIX=%d \
                         order by NUMBER",
			(const char *)rdstation->name(),
			matrix->matrix());
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
  RDTty *tty=new RDTty(rdstation->name(),matrix->port(RDMatrix::Primary));
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
