// rdcatch_conf.cpp
//
// Abstract an RDCatch Configuration.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcatch_conf.cpp,v 1.8 2010/07/29 19:32:33 cvs Exp $
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
#include <rdcatch_conf.h>
#include <rdescape_string.h>


//
// Global Classes
//
RDCatchConf::RDCatchConf(const QString &station)
{
  RDSqlQuery *q;
  QString sql;

  air_station=station;

  sql=QString().
    sprintf("select ID from RDCATCH where STATION=\"%s\"",
			(const char *)air_station);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString().
      sprintf("insert into RDCATCH set STATION=\"%s\"",
			  (const char *)air_station);
    q=new RDSqlQuery(sql);
    delete q;
  } else {
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
  RDSqlQuery *q;
  QString sql;

  sql=QString().
    sprintf("UPDATE RDCATCH SET %s=\"%s\" WHERE STATION=\"%s\"",
	    (const char *)param,
	    (const char *)RDEscapeString(value),
	    (const char *)RDEscapeString(air_station));
  q=new RDSqlQuery(sql);
  delete q;
}
