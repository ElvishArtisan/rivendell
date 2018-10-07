// createdb.cpp
//
// Create Rivendell MySQL database RDDbConfig
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>
#include <qprocess.h>

#include <rdconfig.h>
#include <rdpaths.h>

#include "createdb.h"

CreateDb::CreateDb(QString host,QString database,QString username,QString password)
{
  db_host=host;
  db_name=database;
  db_user=username;
  db_pass=password;
}

bool CreateDb::create(QObject *parent,QString *err_str,RDConfig *config)
{
  QString sql;

  //
  // Open Database
  //
  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(config->mysqlDriver(),"createDb");
    if(!db.isValid()) {
      *err_str+= QString(QObject::tr("Couldn't initialize MySql driver!"));
      return true;
    }
    db.setHostName(db_host);
    db.setDatabaseName("mysql");
    db.setUserName(db_user);
    db.setPassword(db_pass);
    if(!db.open()) {
      *err_str+=QString(QObject::tr("Couldn't open MySQL connection on"))+
	" \""+db_host+"\".";
      db.removeDatabase(db.connectionName());
      db.close();
      return true;
    }

    QSqlQuery *q;

    sql=QString().sprintf("drop database if exists `%s`",(const char *)db_name);
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString(QObject::tr("Could not remove database"));
      return true;
    }
    delete q;

    sql=QString().sprintf("create database if not exists `%s`",(const char *)db_name);
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString(QObject::tr("Could not create database"));
      return true;
    }
    delete q;

    sql=QString().sprintf("grant all on * to %s identified by \"%s\"",(const char *)db_user,(const char *)db_pass);
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString().sprintf("Could not set permissions: %s",(const char *)sql);
      return true;
    }
    delete q;

    QProcess rddbmgrProcess(parent);
    QStringList args;
    args << QString("--create");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    rddbmgrProcess.start(QString(RD_PREFIX)+"/sbin/rddbmgr",args);
    rddbmgrProcess.waitForFinished();
    QApplication::restoreOverrideCursor();
    if (rddbmgrProcess.exitCode()) {
      fprintf(stderr,"Exit Code=%d\n",rddbmgrProcess.exitCode());
      return true;
    }
  }

  return false;
}

CreateDb::~CreateDb()
{
  if(db.isOpen()) {
    db.removeDatabase(db_name);
    db.close();
  }
}

bool CreateDb::isOpen()
{
  return db.isOpen();
}

