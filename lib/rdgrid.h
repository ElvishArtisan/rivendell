// rdgrid.h
//
// Abstract a Rivendell Log Manager Grid
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgrid.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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


#ifndef RDGRID_H
#define RDGRID_H

#include <qsqldatabase.h>

#include <rdclock.h>

class RDGrid
{
  public:
   RDGrid(QString svc_name);
   QString serviceName() const;
   void setServiceName(QString name);
   RDClock *clock(int dayofweek,int hour);
   void setClock(int dayofweek,int hour,RDClock *clock);
   bool load();
   void save();
   void clear();

  private:
   QString grid_name;
   RDClock *grid_clocks[7][24];
};


#endif 
