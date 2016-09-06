// rdvguestresource.h
//
// Abstract a Rivendell Switcher Endpoint
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDVGUESTRESOURCE_H
#define RDVGUESTRESOURCE_H

#include <stdint.h>

#include <QVariant>

#include <rdmatrix.h>

class RDVguestResource
{
 public:
  RDVguestResource(RDMatrix *matrix,RDMatrix::VguestType type,int num);
  int id() const;
  QString stationName() const;
  int matrixNumber() const;
  RDMatrix::VguestType type() const;
  int number() const;
  int engineNumber() const;
  void setEngineNumber(int num) const;
  int deviceNumber() const;
  void setDeviceNumber(int num) const;
  int surfaceNumber() const;
  void setSurfaceNumber(int num) const;
  int relayNumber() const;
  void setRelayNumber(int num) const;
  int bussNumber() const;
  void setBussNumber(int num) const;

 private:
  QVariant GetRow(const QString &param) const;
  void SetRow(const QString &param,int value) const;
  RDMatrix *guest_matrix;
  RDMatrix::VguestType guest_type;
  int guest_number;
  int guest_id;
};


#endif  // RDVGUESTRESOURCE_H
