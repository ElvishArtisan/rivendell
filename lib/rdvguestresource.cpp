// rdvguestresource.cpp
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

#include "rddb.h"
#include "rdescape_string.h"
#include "rdvguestresource.h"

RDVguestResource::RDVguestResource(RDMatrix *matrix,RDMatrix::VguestType type,
				   int num)
{
  guest_matrix=matrix;
  guest_type=type;
  guest_number=num;
  guest_id=-1;

  QString sql=QString("select ID from VGUEST_RESOURCES where ")+
    "(STATION_NAME=\""+RDEscapeString(matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX_NUM=%d)&&",matrix->matrix())+
    QString().sprintf("(VGUEST_TYPE=%d)&&",type)+
    QString().sprintf("(NUMBER=%d)",num);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    guest_id=q->value(0).toInt();
  }
  delete q;
}


int RDVguestResource::id() const
{
  return guest_id;
}


QString RDVguestResource::stationName() const
{
  return guest_matrix->station();
}


int RDVguestResource::matrixNumber() const
{
  return guest_matrix->matrix();
}


RDMatrix::VguestType RDVguestResource::type() const
{
  return guest_type;
}


int RDVguestResource::number() const
{
  return guest_number;
}


int RDVguestResource::engineNumber() const
{
  return GetRow("ENGINE_NUM").toInt();
}


void RDVguestResource::setEngineNumber(int num) const
{
  SetRow("ENGINE_NUM",num);
}


int RDVguestResource::deviceNumber() const
{
  return GetRow("DEVICE_NUM").toInt();
}


void RDVguestResource::setDeviceNumber(int num) const
{
  SetRow("DEVICE_NUM",num);
}


int RDVguestResource::surfaceNumber() const
{
  return GetRow("SURFACE_NUM").toInt();
}


void RDVguestResource::setSurfaceNumber(int num) const
{
  SetRow("SURFACE_NUM",num);
}


int RDVguestResource::relayNumber() const
{
  return GetRow("RELAY_NUM").toInt();
}


void RDVguestResource::setRelayNumber(int num) const
{
  SetRow("RELAY_NUM",num);
}


int RDVguestResource::bussNumber() const
{
  return GetRow("BUSS_NUM").toInt();
}


void RDVguestResource::setBussNumber(int num) const
{
  SetRow("BUSS_NUM",num);
}


QVariant RDVguestResource::GetRow(const QString &param) const
{
  QVariant var;
  QString sql=QString("select `")+param+
    "` from VGUEST_RESOURCES where "+
    QString().sprintf("ID=%d",guest_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    var=q->value(0);
  }
  delete q;
  return var;
}


void RDVguestResource::SetRow(const QString &param,int value) const
{
  QString sql=QString("update VGUEST_RESOURCES set ")+
    param+QString().sprintf("=%d ",value)+"where "+
    QString().sprintf("ID=%d",guest_id);
  RDSqlQuery::run(sql);
}
