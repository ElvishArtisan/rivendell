// mojibake_tests.cpp
//
// Mojibake Tests
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

bool MainWidget::MojibakeTests(RDUnitTestData *data)
{
  QString sql;
  QSqlQuery *q;
  bool ret=true;

  data->clear();
  data->setGroupName("Mojibake Tests");

  //
  // Character Set Support
  //
  sql=QString("show character set where ")+
    "Charset=\""+rda->config()->mysqlCharset()+"\"";
  q=new QSqlQuery(sql);
  data->addTest(QString("Character Set Support [")+rda->config()->
		mysqlCharset()+"]",
	    q->first());
  ret=ret&&q->first();
  delete q;

  //
  // Collation Support
  //
  sql=QString("show collation where ")+
    "Charset=\""+rda->config()->mysqlCharset()+"\" &&"+
    "Collation=\""+rda->config()->mysqlCollation()+"\"";
  q=new QSqlQuery(sql);
  data->addTest(QString("Collation Support [")+rda->config()->
		mysqlCollation()+"]",
	    q->first());
  ret=ret&&q->first();
  delete q;

  //
  // Unicode Test
  //
  sql=QString("create table if not exists TESTTAB(")+
    "FIELD1 int primary key auto_increment,"+
    "FIELD2 char(255),"+
    "index FIELD2_IDX(FIELD2)"+
    ")"+
    rda->config()->createTablePostfix();
  q=new QSqlQuery(sql);
  data->addTest("Unicode Table Creation Test",q->isActive());
  ret=ret&&q->isActive();
  delete q;
  if(!ret) {  // Pointless to continue!
    return ret;
  }


  //
  // Clean Up
  //
  sql=QString("drop table TESTTAB");
  q=new QSqlQuery(sql);
  delete q;

  return ret;
}
