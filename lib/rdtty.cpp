// rdtty.cpp
//
// Abstract a Rivendell TTY.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtty.cpp,v 1.10 2010/07/29 19:32:34 cvs Exp $
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
#include <rdconf.h>
#include <rdtty.h>


//
// Global Classes
//
RDTty::RDTty(const QString &station,unsigned port_id,bool create)
{
  RDSqlQuery *q;
  QString sql;

  tty_station=station;
  tty_id=port_id;

  if(create) {
    sql=QString().sprintf("select ID from TTYS where \
(STATION_NAME=\"%s\")&&(PORT_ID=%d)",(const char *)tty_station,tty_id);
    q=new RDSqlQuery(sql);
    if(q->size()!=1) {
      delete q;
      sql=QString().sprintf("INSERT INTO TTYS SET STATION_NAME=\"%s\",PORT_ID=%d",
			    (const char *)tty_station,tty_id);
      q=new RDSqlQuery(sql);
      delete q;
    }
    else {
      delete q;
    }
  }
}


QString RDTty::station() const
{
  return tty_station;
}


int RDTty::portId() const
{
  return tty_id;
}


bool RDTty::active()
{
  return GetBoolValue("ACTIVE");
}


void RDTty::setActive(bool state)
{
  SetRow("ACTIVE",state);
}


QString RDTty::port()
{
  return GetStringValue("PORT");
}


void RDTty::setPort(QString port)
{
  SetRow("PORT",port);
}


int RDTty::baudRate()
{
  return GetIntValue("BAUD_RATE");
}


void RDTty::setBaudRate(int rate)
{
  SetRow("BAUD_RATE",rate);
}


int RDTty::dataBits()
{
  return GetIntValue("DATA_BITS");
}


void RDTty::setDataBits(int bits)
{
  SetRow("DATA_BITS",bits);
}


int RDTty::stopBits()
{
  return GetIntValue("STOP_BITS");
}


void RDTty::setStopBits(int bits)
{
  SetRow("STOP_BITS",bits);
}


RDTTYDevice::Parity RDTty::parity()
{
  return (RDTTYDevice::Parity)GetIntValue("PARITY");
}


void RDTty::setParity(RDTTYDevice::Parity parity)
{
  SetRow("PARITY",(int)parity);
}


RDTty::Termination RDTty::termination()
{
  return (RDTty::Termination)GetIntValue("TERMINATION");
}


void RDTty::setTermination(RDTty::Termination term)
{
  SetRow("TERMINATION",(int)term);
}


bool RDTty::GetBoolValue(const QString &field)
{
  QString sql;
  RDSqlQuery *q;
  bool state;

  sql=QString().sprintf("select %s from TTYS where \
(STATION_NAME=\"%s\")&&(PORT_ID=%d)",(const char *)field,
			(const char *)tty_station,tty_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    state=RDBool(q->value(0).toString());
    delete q;
    return state;
  }
  delete q;
  return false;    
}


QString RDTty::GetStringValue(const QString &field)
{
  QString sql;
  RDSqlQuery *q;
  QString accum;

  sql=QString().sprintf("select %s from TTYS where \
(STATION_NAME=\"%s\")&&(PORT_ID=%d)",(const char *)field,
			(const char *)tty_station,tty_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toString();
    delete q;
    return accum; 
  }
  delete q;
  return QString();    
}


int RDTty::GetIntValue(const QString &field)
{
  QString sql;
  RDSqlQuery *q;
  int accum;

  sql=QString().sprintf("select %s from TTYS where \
(STATION_NAME=\"%s\")&&(PORT_ID=%d)",(const char *)field,
			(const char *)tty_station,tty_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


void RDTty::SetRow(const QString &param,bool value)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE TTYS SET %s=\"%s\" \
WHERE (STATION_NAME=\"%s\")&&(PORT_ID=%d)",
			(const char *)param,
			(const char *)RDYesNo(value),
			(const char *)tty_station,
			tty_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDTty::SetRow(const QString &param,const QString &value)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE TTYS SET %s=\"%s\" \
WHERE (STATION_NAME=\"%s\")&&(PORT_ID=%d)",
			(const char *)param,
			(const char *)value,
			(const char *)tty_station,
			tty_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDTty::SetRow(const QString &param,int value)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE TTYS SET %s=%d \
WHERE (STATION_NAME=\"%s\")&&(PORT_ID=%d)",
			(const char *)param,
			value,
			(const char *)tty_station,
			tty_id);
  q=new RDSqlQuery(sql);
  delete q;
}
