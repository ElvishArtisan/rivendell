// rdgrid.h
//
// Abstract a Rivendell Log Manager Grid
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdclock.h>
#include <rdstation.h>

class RDGrid
{
 public:
  RDGrid(QString svc_name,RDStation *station);
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
  RDStation *grid_station;
};


#endif  // RDGRID_H
