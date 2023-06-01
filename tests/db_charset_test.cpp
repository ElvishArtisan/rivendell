// db_charset_test.cpp
//
// Display charset/collation parameters for a DB connection
//
//   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>

#include <rdapplication.h>

#include "db_charset_test.h"

MainObject::MainObject(QObject *parent)
  : QObject()
{
  QString err_msg;
  QString sql;
  RDSqlQuery *q;
  RDCmdSwitch *cmd=new RDCmdSwitch("db_charset_test",DB_CHARSET_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"db_charset_test: unknown option \"%s\"\n",
	      cmd->value(i).toUtf8().constData());
      exit(256);
    }
  }

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDApplication("db_charset_test",
			     "rdvairplayd",DB_CHARSET_TEST_USAGE,false,this));
  if(!rda->open(&err_msg,NULL,true)) {
    fprintf(stderr,"db_charset_test: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  sql=QString("show variables like '%character_set%'");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("%s: %s\n",q->value(0).toString().toUtf8().constData(),
	   q->value(1).toString().toUtf8().constData());
  }
  delete q;

  sql=QString("show variables like '%collation%'");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("%s: %s\n",q->value(0).toString().toUtf8().constData(),
	   q->value(1).toString().toUtf8().constData());
  }
  delete q;

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);

  MainObject();
  return a.exec();
}
