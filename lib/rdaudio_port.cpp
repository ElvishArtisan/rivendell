// rdaudio_port.cpp
//
// Abstract a Rivendell Audio Port.
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdaudio_port.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDAudioPort::RDAudioPort(QString station,int card,bool create)
{
  RDSqlQuery *q;
  QString sql;

  port_station=station;
  port_card=card;

  if(create) {
    sql=QString("select ")+
      "ID "+
      "from AUDIO_PORTS where "+
      "(STATION_NAME=\""+RDEscapeString(port_station)+"\")&&"+
      QString().sprintf("(CARD_NUMBER=%d)",port_card);
    q=new RDSqlQuery(sql);
    if(q->size()!=1) {
      delete q;
      sql=QString("insert into AUDIO_PORTS set ")+
	"STATION_NAME=\""+RDEscapeString(port_station)+"\","+
	QString().sprintf("CARD_NUMBER=%d",port_card);
      q=new RDSqlQuery(sql);
      delete q;
    }
    else {
      delete q;
    }
  }
}


QString RDAudioPort::station() const
{
  return port_station;
}


int RDAudioPort::card() const
{
  return port_card;
}


RDCae::ClockSource RDAudioPort::clockSource()
{
  return (RDCae::ClockSource)GetIntValue("CLOCK_SOURCE");
}


void RDAudioPort::setClockSource(RDCae::ClockSource src)
{
  SetRow("CLOCK_SOURCE",(int)src);
}


RDAudioPort::PortType RDAudioPort::inputPortType(int port)
{
  if(port<0) {
    return RDAudioPort::Analog;
  }
  return 
   (RDAudioPort::PortType)GetIntValue(QString().sprintf("INPUT_%d_TYPE",port));
}


void RDAudioPort::setInputPortType(int port,RDAudioPort::PortType type)
{
  if(port<0) {
    return;
  }
  SetRow(QString().sprintf("INPUT_%d_TYPE",port),(int)type);
}


RDCae::ChannelMode RDAudioPort::inputPortMode(int port)
{
  if(port<0) {
    return RDCae::Normal;
  }
  return 
   (RDCae::ChannelMode)GetIntValue(QString().sprintf("INPUT_%d_MODE",port));
}


void RDAudioPort::setInputPortMode(int port,RDCae::ChannelMode mode)
{
  if(port<0) {
    return;
  }
  SetRow(QString().sprintf("INPUT_%d_MODE",port),(int)mode);
}


int RDAudioPort::inputPortLevel(int port)
{
  if(port<0) {
    return 400;
  }
  return GetIntValue(QString().sprintf("INPUT_%d_LEVEL",port));
}


void RDAudioPort::setInputPortLevel(int port,int level)
{
  if(port<0) {
    return;
  }
  SetRow(QString().sprintf("INPUT_%d_LEVEL",port),level);
}


int RDAudioPort::outputPortLevel(int port)
{
  if(port<0) {
    return 400;
  }
  return GetIntValue(QString().sprintf("OUTPUT_%d_LEVEL",port));
}


void RDAudioPort::setOutputPortLevel(int port,int level)
{
  if(port<0) {
    return;
  }
  SetRow(QString().sprintf("OUTPUT_%d_LEVEL",port),level);
}


int RDAudioPort::GetIntValue(QString field)
{
  QString sql;
  RDSqlQuery *q;
  int accum;

  sql=QString("select ")+field+" from AUDIO_PORTS where "+
    "(STATION_NAME=\""+RDEscapeString(port_station)+"\")&&"+
    QString().sprintf("(CARD_NUMBER=%d)",port_card);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


void RDAudioPort::SetRow(QString param,int value)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update AUDIO_PORTS set ")+
    param+QString().sprintf("=%d where ",value)+
    "(STATION_NAME=\""+RDEscapeString(port_station)+"\")&&"+
    QString().sprintf("(CARD_NUMBER=%d)",port_card);
  q=new RDSqlQuery(sql);
  delete q;
}


