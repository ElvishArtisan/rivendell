// rdhostvariable.cpp
//
// Abstract a Rivendell Host Variable
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdhostvariable.h"

RDHostVariable::RDHostVariable(const QString &stationname,const QString &name)
{
  var_id=-1;
  var_name=name;

  QString sql=QString("select ID from HOSTVARS where ")+
    "(STATION_NAME=\""+RDEscapeString(stationname)+"\")&&"+
    "(NAME=\""+RDEscapeString(name)+"\")";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    var_id=q->value(0).toInt();
  }
  delete q;
}


RDHostVariable::RDHostVariable(int id)
{
  var_id=id;

  QString sql=QString("select NAME from HOSTVARS where ")+
    QString().sprintf("ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    var_name=q->value(0).toString();
  }
  delete q;
}


QString RDHostVariable::name() const
{
  return var_name;
}


bool RDHostVariable::exists() const
{
  return RDHostVariable::exists(var_id);
}


QString RDHostVariable::value() const
{
  return RDGetSqlValue("HOSTVARS","NAME",var_name,"VARVALUE").
    toString();
}


void RDHostVariable::setValue(const QString &str)
{
  SetRow("VARVALUE",str);
}


QString RDHostVariable::remarks() const
{
  return RDGetSqlValue("HOSTVARS","NAME",var_name,"REMARK").
    toString();
}


void RDHostVariable::setRemarks(const QString &str)
{
  SetRow("REMARK",str);
}


int RDHostVariable::create(const QString &stationname,const QString &name)
{
  int ret=-1;

  QString sql=QString("insert into HOSTVARS set ")+
    "STATION_NAME=\""+RDEscapeString(stationname)+"\","+
    "NAME=\""+RDEscapeString(name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->isActive()) {
    ret=q->lastInsertId().toInt();
  }
  delete q;

  return ret;
}


void RDHostVariable::remove(int id)
{
  QString sql=QString("delete from HOSTVARS where ")+
    QString().sprintf("ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


bool RDHostVariable::exists(const QString &stationname,const QString &name)
{
  bool ret=false;

  QString sql=QString("select ID from HOSTVARS where ")+
    "(STATION_NAME=\""+RDEscapeString(stationname)+"\")&&"+
    "(NAME=\""+RDEscapeString(name)+"\")";
  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;

  return ret;
}


bool RDHostVariable::exists(int id)
{
  bool ret=false;

  QString sql=QString("select ID from HOSTVARS where ")+
    QString().sprintf("ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;

  return ret;
}


void RDHostVariable::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update HOSTVARS set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    "NAME=\""+RDEscapeString(var_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}
