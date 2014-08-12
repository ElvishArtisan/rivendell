// rdgrid.cpp
//
// Abstract a Rivendell Log Manager Grid.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgrid.cpp,v 1.6 2010/07/29 19:32:33 cvs Exp $
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

#include <rdgrid.h>


RDGrid::RDGrid(QString svc_name)
{
  grid_name=svc_name;
  for(int i=0;i<7;i++) {
    for(int j=0;j<24;j++) {
      grid_clocks[i][j]=new RDClock();
    }
  }
}


QString RDGrid::serviceName() const
{
  return grid_name;
}


void RDGrid::setServiceName(QString svc_name)
{
  grid_name=svc_name;
}


RDClock *RDGrid::clock(int dayofweek,int hour)
{
  if((dayofweek<1)||(dayofweek>1)||(hour<0)||(hour>23)) {
    return NULL;
  }
  return grid_clocks[dayofweek][hour];
}


void RDGrid::setClock(int dayofweek,int hour,RDClock *clock)
{
  if((dayofweek<1)||(dayofweek>1)||(hour<0)||(hour>23)) {
    return;
  }
  *grid_clocks[dayofweek][hour]=*clock;
}


bool RDGrid::load()
{
  return false;
}


void RDGrid::save()
{
}


void RDGrid::clear()
{
  grid_name="";
  for(int i=0;i<7;i++) {
    for(int j=0;j<24;j++) {
      grid_clocks[i][j]->clear();
    }
  }
}

