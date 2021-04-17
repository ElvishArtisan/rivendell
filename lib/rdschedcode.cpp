// rdschedcode.cpp
//
// Abstract a Rivendell Scheduler Code
//
//   (C) Copyright 2015-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdschedcode.h"
#include "rdweb.h"

RDSchedCode::RDSchedCode(const QString &code)
{
  sched_code=code;

  //
  // Get proper case for code
  //
  if(exists()) {
    sched_code=RDGetSqlValue("SCHED_CODES","CODE",code,"CODE").
      toString();
  }
}


QString RDSchedCode::code() const
{
  return sched_code;
}


bool RDSchedCode::exists() const
{
  return RDDoesRowExist("SCHED_CODES","CODE",sched_code);
}


QString RDSchedCode::description() const
{
  return RDGetSqlValue("SCHED_CODES","CODE",sched_code,"DESCRIPTION").
    toString();
}


void RDSchedCode::setDescription(const QString &desc) const
{
  SetRow("DESCRIPTION",desc);
}


QString RDSchedCode::xml() const
{
  QString xml;

  xml+="<schedCode>\n";
  xml+="  "+RDXmlField("code",sched_code);
  xml+="  "+RDXmlField("description",description());
  xml+="</schedCode>\n";

  return xml;
}


void RDSchedCode::SetRow(const QString &param,const QString &value) const
{
  QString sql;

  sql=QString("update `SCHED_CODES` set `")+
    param+"`='"+RDEscapeString(value)+"' "+
    "where `CODE`='"+RDEscapeString(sched_code)+"'";
  RDSqlQuery::apply(sql);
}
