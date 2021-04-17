// rdcatch_conf.cpp
//
// Abstract an RDCatch Configuration.
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

#include "rddb.h"
#include "rdconf.h"
#include "rdcatch_conf.h"
#include "rdescape_string.h"

RDCatchConf::RDCatchConf(const QString &station)
{
  RDSqlQuery *q;
  QString sql;

  air_station=station;

  sql=QString("select `ID` from `RDCATCH` where ")+
    "`STATION`='"+RDEscapeString(air_station)+"'";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString("insert into `RDCATCH` set ")+
      "`STATION`='"+RDEscapeString(air_station)+"'";
    RDSqlQuery::apply(sql);
  } 
  else {
    delete q;
  }
}


QString RDCatchConf::station() const
{
  return air_station;
}


QString RDCatchConf::errorRml() const
{
  return RDGetSqlValue("RDCATCH","STATION",air_station,"ERROR_RML").toString();
}


void RDCatchConf::setErrorRml(const QString &str) const
{
  SetRow("ERROR_RML",str);
}


void RDCatchConf::SetRow(const QString &param,const QString &value) const
{
  QString sql;

  sql=QString("update `RDCATCH` set `")+
    param+"`='"+RDEscapeString(value)+"\" where "+
    "`STATION`='"+RDEscapeString(air_station)+"'";
  RDSqlQuery::apply(sql);
}
