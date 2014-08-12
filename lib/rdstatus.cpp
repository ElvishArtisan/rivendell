// rdstatus.cpp
//
// Functions for getting system status.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdstatus.cpp,v 1.1.2.2 2012/10/22 23:09:39 cvs Exp $
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

#include <qstringlist.h>
#include <qsqldatabase.h>

#include "rdstatus.h"

bool RDAudioStoreValid(RDConfig *config)
{
  FILE *f=NULL;
  char line[1024];
  bool ret=false;

  if((f=fopen("/etc/mtab","r"))==NULL) {
    syslog(LOG_CRIT,"unable to read mtab(5) [%s]",strerror(errno));
    return false;
  }
  if(config->audioStoreMountSource().isEmpty()) {  // Audio store is local
    ret=true;
    while(fgets(line,1024,f)!=NULL) {
      QStringList fields=fields.split(" ",QString(line));
      if(fields.size()>=2) {
	ret=ret&&(fields[1]!=RD_AUDIO_ROOT);
      }
    }
  }
  else {    // Audio store is remote
    ret=false;
    while(fgets(line,1024,f)!=NULL) {
      QStringList fields=fields.split(" ",QString(line));
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

  QSqlDatabase *db=QSqlDatabase::addDatabase(config->mysqlDriver());
  if(db) {
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
    QSqlDatabase::removeDatabase(db);
  }
  return ret;
}
