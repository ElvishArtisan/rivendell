// rdstatus.cpp
//
// Functions for getting system status.
//
//   (C) Copyright 2016-2020 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <errno.h>

#include <QDir>
#include <qsqlquery.h>
#include <qstringlist.h>
#include <qvariant.h>

#include "rdapplication.h"
#include "rdstatus.h"

bool RDAudioStoreValid(RDConfig *config)
{
  FILE *f=NULL;
  char line[1024];
  bool ret=false;

  //
  // Make sure the filesystem is actually mounted
  //
  QDir dir(config->audioRoot());
  dir.count();

  if((f=fopen("/etc/mtab","r"))==NULL) {
    rda->syslog(LOG_ERR,"unable to read mtab(5) [%s]",strerror(errno));
    return false;
  }
  if(config->audioStoreMountSource().isEmpty()) {  // Audio store is local
    ret=true;
    while(fgets(line,1024,f)!=NULL) {
      QStringList fields=QString(line).split(" ");
      if(fields.size()>=2) {
	ret=ret&&(fields[1]!=RD_AUDIO_ROOT);
      }
    }
  }
  else {    // Audio store is remote
    ret=false;
    while(fgets(line,1024,f)!=NULL) {
      QStringList fields=QString(line).split(" ");
      if(fields.size()>=2) {
	ret=ret||fields[0]==config->audioStoreMountSource();
      }
    }
  }
  fclose(f);
  return ret;
}


bool RDDbValid(RDConfig *config,int *schema)
{
  QString sql;
  QSqlQuery *q;
  bool ret=false;

  QSqlDatabase *db=
    new QSqlDatabase(QSqlDatabase::addDatabase(config->mysqlDriver()));
  QString conn_name=db->connectionName();
  db->setDatabaseName(config->mysqlDbname());
  db->setUserName(config->mysqlUsername());
  db->setPassword(config->mysqlPassword());
  db->setHostName(config->mysqlHostname());
  if(db->open()) {
    ret=true;
    sql="select DB from VERSION";
    q=new QSqlQuery(sql);
    if(q->first()) {
      *schema=q->value(0).toInt();
    }
    delete q;
    db->close();
  }
  delete db;
  QSqlDatabase::removeDatabase(conn_name);

  return ret;
}
