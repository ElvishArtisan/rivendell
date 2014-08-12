// rdmonitor_config.cpp
//
// A container class for a Rivendell RDMonitor Configuration
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmonitor_config.cpp,v 1.1.2.2 2013/11/08 03:57:14 cvs Exp $
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
#include <stdio.h>

#include <qobject.h>

#include <rdprofile.h>

#include <rdmonitor_config.h>

RDMonitorConfig::RDMonitorConfig()
{
  clear();
  mon_filename="/.rdmonitorrc";
  if(getenv("HOME")!=NULL) {
    mon_filename=QString(getenv("HOME"))+mon_filename;
  }
}


int RDMonitorConfig::screenNumber() const
{
  return mon_screen_number;
}


void RDMonitorConfig::setScreenNumber(int screen)
{
  mon_screen_number=screen;
}


RDMonitorConfig::Position RDMonitorConfig::position() const
{
  return mon_position;
}


void RDMonitorConfig::setPosition(RDMonitorConfig::Position pos)
{
  mon_position=pos;
}


int RDMonitorConfig::xOffset() const
{
  return mon_x_offset;
}


void RDMonitorConfig::setXOffset(int offset)
{
  mon_x_offset=offset;
}


int RDMonitorConfig::yOffset() const
{
  return mon_y_offset;
}


void RDMonitorConfig::setYOffset(int offset)
{
  mon_y_offset=offset;
}


bool RDMonitorConfig::load()
{
  RDProfile *p=new RDProfile();
  if(!p->setSource(mon_filename)) {
    return false;
  }
  mon_screen_number=p->intValue("Monitor","ScreenNumber",0);
  mon_position=(RDMonitorConfig::Position)
    p->intValue("Monitor","Position",(int)RDMonitorConfig::UpperLeft);
  mon_x_offset=p->intValue("Monitor","XOffset",0);
  mon_y_offset=p->intValue("Monitor","YOffset",0);
  delete p;
  return true;
}


bool RDMonitorConfig::save()
{
  FILE *f=NULL;

  if((f=fopen(mon_filename,"w"))==NULL) {
    return false;
  }
  fprintf(f,"[Monitor]\n");
  fprintf(f,"ScreenNumber=%d\n",mon_screen_number);
  fprintf(f,"Position=%u\n",mon_position);
  fprintf(f,"XOffset=%d\n",mon_x_offset);
  fprintf(f,"YOffset=%d\n",mon_y_offset);
  fprintf(f,"\n");

  fclose(f);
  return true;
}


void RDMonitorConfig::clear()
{
  mon_screen_number=0;
  mon_position=RDMonitorConfig::UpperLeft;
  mon_x_offset=0;
  mon_y_offset=0;
}


QString RDMonitorConfig::positionText(RDMonitorConfig::Position pos)
{
  QString ret=QObject::tr("Unknown Position");
  switch(pos) {
  case RDMonitorConfig::UpperLeft:
    ret=QObject::tr("Top Left");
    break;

  case RDMonitorConfig::UpperCenter:
    ret=QObject::tr("Top Center");
    break;

  case RDMonitorConfig::UpperRight:
    ret=QObject::tr("Top Right");
    break;

  case RDMonitorConfig::LowerLeft:
    ret=QObject::tr("Bottom Left");
    break;

  case RDMonitorConfig::LowerCenter:
    ret=QObject::tr("Bottom Center");
    break;

  case RDMonitorConfig::LowerRight:
    ret=QObject::tr("Bottom Right");
    break;

  case RDMonitorConfig::LastPosition:
    break;
  }
  return ret;
}
