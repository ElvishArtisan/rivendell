// low_level_db_tests.cpp
//
// Low-Level DB Tests
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

#include <rdapplication.h>
#include <rdescape_string.h>

#include "rdunittest.h"

bool MainWidget::LowLevelDbTests(RDUnitTestData *data)
{
  QString sql;
  QSqlQuery *q;
  bool pass;
  bool ret=true;

  data->clear();
  data->setGroupName("Low-Level Database Tests");

  //
  // Create Table Test
  //
  sql=QString("create table if not exists TESTTAB(")+
    "FIELD1 int primary key auto_increment,"+
    "FIELD2 char(255),"+
    "index FIELD2_IDX(FIELD2)"+
    ")";
  q=new QSqlQuery(sql);
  data->addTest("CREATE Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;
  if(!ret) {  // Pointless to continue!
    return ret;
  }

  //
  // Insert Test
  //
  sql=QString("insert into TESTTAB set ")+
    "FIELD2=\""+RDEscapeString("Some kind of mayhem.")+"\"";
  q=new QSqlQuery(sql);
  data->addTest("INSERT Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;

  //
  // Select Test
  //
  sql=QString("select ")+
    "FIELD2 "+
    "from TESTTAB where "+
    "FIELD2=\""+RDEscapeString("Some kind of mayhem.")+"\"";
  q=new QSqlQuery(sql);
  data->addTest("SELECT Test",q->first());
  ret=ret&&q->isActive();
  delete q;

  //
  // Alter Test
  //
  sql=QString("alter table TESTTAB add column FIELD3 int");
  q=new QSqlQuery(sql);
  data->addTest("ALTER Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;

  //
  // Index Test
  //
  sql=QString("create index FIELD1_IDX on TESTTAB (FIELD1)");
  q=new QSqlQuery(sql);
  data->addTest("INDEX Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;

  //
  // Lock Tables Test
  //
  sql=QString("lock tables TESTTAB read");
  q=new QSqlQuery(sql);
  data->addTest("LOCK TABLES Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;
  sql=QString("unlock tables");
  q=new QSqlQuery(sql);
  delete q;

  //
  // Delete Test
  //
  sql=QString("delete ")+
    "from TESTTAB where "+
    "FIELD2=\""+RDEscapeString("Some kind of mayhem.")+"\"";
  q=new QSqlQuery(sql);
  data->addTest("DELETE Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;

  //
  // Drop Table Test
  //
  sql=QString("drop table TESTTAB");
  q=new QSqlQuery(sql);
  data->addTest("DROP Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;

  //
  // Engine Test
  //
  pass=false;
  sql=QString("create table if not exists TESTTAB(")+
    "FIELD1 int primary key auto_increment,"+
    "FIELD2 char(255),"+
    "index FIELD2_IDX(FIELD2)"+
    ") "+
    "engine=\""+rda->config()->mysqlEngine()+"\"";
  q=new QSqlQuery(sql);
  ret=ret&&q->isActive();
  if(q->isActive()) {
    delete q;
    sql=QString("show table status where ")+
      "Name=\"TESTTAB\"";
    q=new QSqlQuery(sql);
    ret=ret&&q->first();
    if(q->first()) {
      pass=q->value(1).toString().toLower()==
	rda->config()->mysqlEngine().toLower();
      ret=ret&&pass;
    }
  }
  delete q;
  sql=QString("drop table TESTTAB");
  q=new QSqlQuery(sql);
  delete q;

  return ret;
}
