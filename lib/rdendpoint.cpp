// rdendpoint.cpp
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
#include "rdendpoint.h"
#include "rdescape_string.h"

RDEndPoint::RDEndPoint(RDMatrix *matrix,int ptnum,RDMatrix::Endpoint pt)
{
  end_point_number=ptnum;
  end_point_type=pt;
  end_id=-1;

  SetTable(pt);

  QString sql=QString("select ID from ")+end_table+" where "+
    "(STATION_NAME=\""+RDEscapeString(matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrix->matrix())+
    QString().sprintf("(NUMBER=%d)",ptnum);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    end_id=q->value(0).toInt();
  }
  delete q;
}


RDEndPoint::RDEndPoint(int id,RDMatrix::Endpoint pt)
{
  end_id=id;
  end_point_type=pt;

  SetTable(pt);
  QString sql=QString("select ")+
    "NUMBER from "+end_table+" where "+
    QString().sprintf("ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    end_point_number=q->value(0).toInt();
  }
  delete q;
}


int RDEndPoint::id() const
{
  return end_id;
}


int RDEndPoint::pointNumber() const
{
  return end_point_number;
}


RDMatrix::Endpoint RDEndPoint::pointType() const
{
  return end_point_type;
}


QString RDEndPoint::stationName() const
{
  return GetRow("STATION_NAME").toString();
}


int RDEndPoint::matrixNumber() const
{
  return GetRow("MATRIX").toInt();
}


QString RDEndPoint::name() const
{
  return GetRow("NAME").toString();
}


void RDEndPoint::setName(const QString &str) const
{
  SetRow("NAME",str);
}


QString RDEndPoint::feedName() const
{
  return GetRow("FEED_NAME").toString();
}


void RDEndPoint::setFeedName(const QString &str) const
{
  SetRow("FEED_NAME",str);
}


RDMatrix::Mode RDEndPoint::channelMode() const
{
  return (RDMatrix::Mode)GetRow("CHANNEL_MODE").toInt();
}


void RDEndPoint::setChannelMode(RDMatrix::Mode mode)
{
  SetRow("CHANNEL_MODE",(int)mode);
}


int RDEndPoint::engineNumber() const
{
  return GetRow("ENGINE_NUM").toInt();
}


void RDEndPoint::setEngineNumber(int num) const
{
  SetRow("ENGINE_NUM",num);
}


int RDEndPoint::deviceNumber() const
{
  return GetRow("DEVICE_NUM").toInt();
}


void RDEndPoint::setDeviceNumber(int num) const
{
  SetRow("DEVICE_NUM",num);
}


QString RDEndPoint::nodeHostname() const
{
  return GetRow("NODE_HOSTNAME").toString();
}


void RDEndPoint::setNodeHostname(const QString &str) const
{
  SetRow("NODE_HOSTNAME",str);
}


uint16_t RDEndPoint::nodeTcpPort() const
{
  return GetRow("NODE_TCP_PORT").toInt();
}


void RDEndPoint::setNodeTcpPort(uint16_t port) const
{
  SetRow("NODE_TCP_PORT",(unsigned)port);
}


unsigned RDEndPoint::nodeSlot() const
{
  return GetRow("NODE_SLOT").toUInt();
}


void RDEndPoint::setNodeSlot(unsigned slot) const
{
  SetRow("NODE_SLOT",slot);
}


void RDEndPoint::SetTable(RDMatrix::Endpoint pt)
{
  switch(pt) {
  case RDMatrix::Input:
    end_table="INPUTS";
    break;

  case RDMatrix::Output:
    end_table="OUTPUTS";
    break;
  }
}


QVariant RDEndPoint::GetRow(const QString &param) const
{
  QVariant var;
  QString sql=QString("select `")+param+
    "` from "+end_table+" where "+
    QString().sprintf("ID=%d",end_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    var=q->value(0);
  }
  delete q;
  return var;
}


void RDEndPoint::SetRow(const QString &param,const QString &value) const
{
  QString sql=QString("update ")+end_table+" set "+
    param+"=\""+RDEscapeString(value)+"\" where "+
    QString().sprintf("ID=%d",end_id);
  RDSqlQuery::run(sql);
}


void RDEndPoint::SetRow(const QString &param,int value) const
{
  QString sql=QString("update ")+end_table+" set "+
    param+QString().sprintf("=%d ",value)+"where "+
    QString().sprintf("ID=%d",end_id);
  RDSqlQuery::run(sql);
}


void RDEndPoint::SetRow(const QString &param,unsigned value) const
{
  QString sql=QString("update ")+end_table+" set "+
    param+QString().sprintf("=%d ",value)+"where "+
    QString().sprintf("ID=%d",end_id);
  RDSqlQuery::run(sql);
}
