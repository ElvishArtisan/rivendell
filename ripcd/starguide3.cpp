// starguide3.cpp
//
// A Rivendell switcher driver for the StarGuide III Satellite Receiver
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: starguide3.cpp,v 1.14 2010/08/03 23:39:26 cvs Exp $
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

#include <qsqldatabase.h>
#include <rddb.h>
#include <globals.h>
#include <starguide3.h>


StarGuide3::StarGuide3(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Get Matrix Parameters
  //
  sg_inputs=matrix->inputs();
  sg_outputs=matrix->outputs();

  //
  // Load Feed Data
  //
  sql=QString().sprintf("select NUMBER,ENGINE_NUM,DEVICE_NUM,CHANNEL_MODE\
                         from INPUTS  where STATION_NAME=\"%s\" && MATRIX=%d \
                         order by NUMBER",
			(const char *)rdstation->name(),
			matrix->matrix());
  q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<sg_inputs;i++) {
    sg_feed.push_back(StarGuideFeed());
    if(q->isValid()&&(q->value(0).toInt()==(i+1))) {
      sg_feed.back().setProviderId(q->value(1).toInt());
      sg_feed.back().setServiceId(q->value(2).toInt());
      sg_feed.back().setMode((RDMatrix::Mode)q->value(3).toInt());
      q->next();
    }
  }
  delete q;

  //
  // Initialize the TTY Port
  //
  RDTty *tty=new RDTty(rdstation->name(),matrix->port(RDMatrix::Primary));
  sg_device=new RDTTYDevice();
  if(tty->active()) {
    sg_device->setName(tty->port());
    sg_device->setSpeed(tty->baudRate());
    sg_device->setWordLength(tty->dataBits());
    sg_device->setParity(tty->parity());
    sg_device->open(IO_Raw|IO_ReadWrite);
  }
  delete tty;
}


StarGuide3::~StarGuide3()
{
  delete sg_device;
}


RDMatrix::Type StarGuide3::type()
{
  return RDMatrix::StarGuideIII;
}


unsigned StarGuide3::gpiQuantity()
{
  return 0;
}


unsigned StarGuide3::gpoQuantity()
{
  return false;
}


bool StarGuide3::primaryTtyActive()
{
  return true;
}


bool StarGuide3::secondaryTtyActive()
{
  return false;
}


void StarGuide3::processCommand(RDMacro *cmd)
{
  char str[256];
  char route;
  int input=cmd->arg(1).toInt()-1;
  int output=cmd->arg(2).toInt()-1;

  if((input<0)||(input>=sg_inputs)||
     (output<0)||(output>=sg_outputs)) {
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
    return;
  }
  switch(sg_feed[input].mode()) {
      case RDMatrix::Left:
	route='1';
	break;
	
      case RDMatrix::Right:
	route='2';
	break;
	
      default:
	route='0';
	break;
  }
  switch(cmd->command()) {
      case RDMacro::ST:
	sprintf(str,"SP A,P,%d\rSP A,S,%d\rAM %c,0%c\r",
		sg_feed[input].providerId(),
		sg_feed[input].serviceId(),
		output+'A',
		route);
	sg_device->writeBlock(str,strlen(str));
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;
	
      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}
