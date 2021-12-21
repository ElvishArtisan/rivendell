// rdportnames.h
//
// Get audio port names
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPORTNAMES_H
#define RDPORTNAMES_H

#include <rd.h>
#include <rdairplay_conf.h>
#include <rddb.h>

class RDPortNames
{
 public:
  RDPortNames(const QString &station_name);
  QString stationName() const;
  QString portName(int card,int port) const;

 private:
  QString d_port_names[RD_MAX_CARDS][RD_MAX_PORTS];
  QString d_station_name;
};


#endif  // RDPORTNAMES_H
