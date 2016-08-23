// high_level_db_tests.cpp
//
// High-Level DB Tests
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

#include <errno.h>
#include <unistd.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "rdunittest.h"

bool MainWidget::HighLevelDbTests(RDUnitTestData *data)
{
  QString sql;
  //  QSqlQuery *q;
  RDSqlQuery *r;
  QString err_str;
  //  bool pass;
  bool ret=true;

  data->clear();
  data->setGroupName("High-Level Database Tests");

  //
  // Create Database Test
  //
  ret=ret&&RDMakeDb(&err_str,rda->config());
  if(ret) {
    data->addTest("Create Database Test",ret);
  }
  else {
    data->addTest("Create Database Test",ret,err_str);
    return ret;
  }

  //
  // Initialize Database Test
  //
  ret=ret&&RDInitDb(&err_str,rda->config());
  if(ret) {
    data->addTest("Initialize Database Test",ret);
  }
  else {
    data->addTest("Initialize Database Test",ret,err_str);
    return ret;
  }

  //
  // Host Creation Test
  //
  sql=QString("select NAME from STATIONS");
  r=new RDSqlQuery(sql);
  if(r->first()) {
    if(r->value(0).toString()==rda->config()->stationName()) {
      data->addTest("Host Creation Test",true);
    }
    else {
      data->addTest("Host Creation Test",false,"Created hostname: "+
		    r->value(0).toString());
    }
  }
  else {
    data->addTest("Host Creation Test",false,"No station records found");
  }
  delete r;

  //
  // Clean Up
  //
  RemoveAllTables();

  return ret;
}
