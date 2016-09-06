// rdendpoint.h
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

#ifndef RDENDPOINT_H
#define RDENDPOINT_H

#include <stdint.h>

#include <QVariant>

#include <rdmatrix.h>

class RDEndPoint
{
 public:
  RDEndPoint(RDMatrix *matrix,int ptnum,RDMatrix::Endpoint pt);
  RDEndPoint(int id,RDMatrix::Endpoint pt);
  int id() const;
  int pointNumber() const;
  RDMatrix::Endpoint pointType() const;
  QString stationName() const;
  int matrixNumber() const;
  QString name() const;
  void setName(const QString &str) const;
  QString feedName() const;
  void setFeedName(const QString &str) const;
  RDMatrix::Mode channelMode() const;
  void setChannelMode(RDMatrix::Mode mode);
  int engineNumber() const;
  void setEngineNumber(int num) const;
  int deviceNumber() const;
  void setDeviceNumber(int num) const;
  QString nodeHostname() const;
  void setNodeHostname(const QString &str) const;
  uint16_t nodeTcpPort() const;
  void setNodeTcpPort(uint16_t port) const;
  unsigned nodeSlot() const;
  void setNodeSlot(unsigned slot) const;

 private:
  void SetTable(RDMatrix::Endpoint pt);
  QVariant GetRow(const QString &param) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  int end_point_number;
  RDMatrix::Endpoint end_point_type;
  int end_id;
  QString end_table;
};


#endif  // RDENDPOINT_H
