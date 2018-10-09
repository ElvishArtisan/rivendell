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
#include <qmessagebox.h>

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

bool CreateDb::create(QWidget *parent,QString *err_str,RDConfig *rd_config)
{
  QString sql;

  //
  // Open Database
  //
  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(rd_config->mysqlDriver(),"createDb");
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
      *err_str+=QString(QObject::tr("Could not remove old database"));
      return true;
    }
    delete q;

    sql=QString().sprintf("create database if not exists `%s`",(const char *)db_name);
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString(QObject::tr("Could not create new database"));
      return true;
    }
    delete q;

    //
    // Drop any existing 'rduser'@'%' and 'rduser'@'localhost' users
    //
    sql=QString().sprintf("drop user '%s'@'%%'",(const char *)rd_config->mysqlUsername());
    q=new QSqlQuery(sql,db);
    delete q;
    sql=QString().sprintf("drop user '%s'@'localhost'",(const char *)rd_config->mysqlUsername());
    q=new QSqlQuery(sql,db);
    delete q;

    sql=QString("flush privileges");
    q=new QSqlQuery(sql,db);
    delete q;

    sql=QString().sprintf("create user '%s'@'%%' identified by \"%s\"",
      (const char *)rd_config->mysqlUsername(),(const char *)rd_config->mysqlPassword());
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString().sprintf("Could not create user: '%s'@'%%'",(const char *)sql);
      return true;
    }
    delete q;

    sql=QString().sprintf("create user '%s'@'localhost' identified by \"%s\"",
      (const char *)rd_config->mysqlUsername(),(const char *)rd_config->mysqlPassword());
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString().sprintf("Could not create user: '%s'@'localhost'",(const char *)sql);
      return true;
    }
    delete q;

    sql=QString().sprintf("grant SELECT, INSERT, UPDATE, DELETE, CREATE, DROP,\
	          INDEX, ALTER, LOCK TABLES on %s.* to %s", 
	          (const char *)db_name, (const char *)rd_config->mysqlUsername());
    q=new QSqlQuery(sql,db);
    if (!q->isActive()) {
      *err_str+=QString().sprintf("Could not set permissions: %s",(const char *)sql);
      return true;
    }
    delete q;

    sql=QString("flush privileges");
    q=new QSqlQuery(sql,db);
    delete q;

    QProcess rddbmgrProcess(parent);
    QStringList args;
    args << QString("--create");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    rddbmgrProcess.start(QString(RD_PREFIX)+"/sbin/rddbmgr",args);
    rddbmgrProcess.waitForFinished();
    QApplication::restoreOverrideCursor();
    if (rddbmgrProcess.exitCode()) {
        *err_str+=QString().sprintf("Failed to create %s database. Rddbmgr exit code=%d", 
        (const char *)db_name,rddbmgrProcess.exitCode());
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

