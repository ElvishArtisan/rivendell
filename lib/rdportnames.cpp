// rdportnames.cpp
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

#include "rdescape_string.h"
#include "rdportnames.h"

RDPortNames::RDPortNames(const QString &station_name)
{
  d_station_name=station_name;

  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Load Channel Labels
  //
  sql=QString("select ")+
    "`LABEL`,"+        // 00
    "`CARD_NUMBER`,"+  // 01
    "`PORT_NUMBER` "+  // 02
    "from `AUDIO_OUTPUTS` where "+
    "`STATION_NAME`='"+RDEscapeString(d_station_name)+"' "+
    "order by `CARD_NUMBER`,`PORT_NUMBER`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_port_names[q->value(1).toInt()][q->value(2).toInt()]=
      q->value(0).toString();
  }
  delete q;
}


QString RDPortNames::stationName() const
{
  return d_station_name;
}


QString RDPortNames::portName(int card,int port) const
{
  if((card<0)||(port<0)) {
    return QString("----");
  }
  return d_port_names[card][port];
}
