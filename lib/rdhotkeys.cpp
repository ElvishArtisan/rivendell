// rdhotkeys.cpp
//
// Abstract an RDHotKeys Configuration.
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

#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdhotkeys.h"

RDHotkeys::RDHotkeys(const QString &station,const QString &module)
{
  QString sql;
  RDSqlQuery *q;
  
  station_hotkeys=station;
  module_name=module;

  sql=QString("select `STATION_NAME` from `RDHOTKEYS` where ")+
    "`STATION_NAME`='"+RDEscapeString(station_hotkeys)+"' && "+
    "`MODULE_NAME`='"+RDEscapeString(module_name)+"'";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    InsertHotkeys();
  }
  delete q;
}


QString RDHotkeys::station() const
{
  return station_hotkeys;
}


QString RDHotkeys::GetRowLabel(const QString &station,const QString &module,const QString &value) const
{
  RDSqlQuery *q;
  QString sql; 
  QString hotkey_label;

  sql=QString::asprintf("select `KEY_LABEL` from `RDHOTKEYS` where ")+
    "`STATION_NAME`='"+RDEscapeString(station)+"' && "+
    "`MODULE_NAME`='"+RDEscapeString(module)+"' && "+
    "`KEY_VALUE`='"+RDEscapeString(value)+"'";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    hotkey_label=QString("");
  }
  else {
    hotkey_label=q->value(0).toString();
  }
  delete q;
  return hotkey_label;
}


void RDHotkeys::InsertHotkeys() const
{
  QString sql;
  QStringList labels;

  labels.push_back(QObject::tr("Start Line 1"));
  labels.push_back(QObject::tr("Stop Line 1"));
  labels.push_back(QObject::tr("Pause Line 1"));
  labels.push_back(QObject::tr("Start Line 2"));
  labels.push_back(QObject::tr("Stop Line 2"));
  labels.push_back(QObject::tr("Pause Line 2"));
  labels.push_back(QObject::tr("Start Line 3"));
  labels.push_back(QObject::tr("Stop Line 3"));
  labels.push_back(QObject::tr("Pause Line 3"));
  labels.push_back(QObject::tr("Start Line 4"));
  labels.push_back(QObject::tr("Stop Line 4"));
  labels.push_back(QObject::tr("Pause Line 4"));
  labels.push_back(QObject::tr("Start Line 5"));
  labels.push_back(QObject::tr("Stop Line 5"));
  labels.push_back(QObject::tr("Pause Line 5"));
  labels.push_back(QObject::tr("Start Line 6"));
  labels.push_back(QObject::tr("Stop Line 6"));
  labels.push_back(QObject::tr("Pause Line 6"));
  labels.push_back(QObject::tr("Start Line 7"));
  labels.push_back(QObject::tr("Stop Line 7"));
  labels.push_back(QObject::tr("Pause Line 7"));
  labels.push_back(QObject::tr("Add"));
  labels.push_back(QObject::tr("Delete"));
  labels.push_back(QObject::tr("Copy"));
  labels.push_back(QObject::tr("Move"));
  labels.push_back(QObject::tr("Sound Panel"));
  labels.push_back(QObject::tr("Main Log"));
  labels.push_back(QObject::tr("Aux Log 1"));
  labels.push_back(QObject::tr("Aux Log 2"));

  for(int i=0;i<labels.size();i++) {
    sql=QString("insert into `RDHOTKEYS` set ")+
      "`STATION_NAME`='"+RDEscapeString(station_hotkeys)+"',"+
      "`MODULE_NAME`='airplay',"+
      QString::asprintf("`KEY_ID`=%u,",i+1)+
      "`KEY_LABEL`='"+RDEscapeString(labels[i])+"'";
    RDSqlQuery::apply(sql);
  }
}
