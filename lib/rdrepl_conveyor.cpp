// rdrepl_conveyor.cpp
//
// Manage replicator conveyor queues.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>
#include <unistd.h>

#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdrepl_conveyor.h"


RDReplConveyor::RDReplConveyor(const QString &repl_name)
{
  conv_repl_name=repl_name;
}


bool RDReplConveyor::push(Direction dir,const QString &filename) const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  bool ret=true;

  sql=QString("insert into REPL_PACKAGES set ")+
    QString().sprintf("DIRECTION=%u,",dir)+
    "REPLICATOR_NAME=\""+RDEscapeString(conv_repl_name)+"\","+
    "DATESTAMP=now()";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("select LAST_INSERT_ID() from REPL_PACKAGES");
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(!RDMove(filename,RDReplConveyor::fileName(q->value(0).toInt()))) {
      syslog(LOG_WARNING,"unable to create replicator package \"%s\"",
	     (const char *)RDReplConveyor::fileName(q->value(0).toInt()));
      sql=QString().sprintf("delete from REPL_PACKAGES where ID=%d",
			    q->value(0).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
      ret=false;
    }
  }
  delete q;

  return ret;
}


bool RDReplConveyor::nextPackageReady(int *id,Direction dir) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select ID from REPL_PACKAGES where ")+
    QString().sprintf("(DIRECTION=%u)&&",dir)+
    "(REPLICATOR_NAME=\""+RDEscapeString(conv_repl_name)+"\") "+
    "order by ID";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *id=q->value(0).toInt();
    ret=true;
  }
  delete q;

  return ret;
}


bool RDReplConveyor::nextPackage(int *id,Direction dir,
				 const QString &outfile) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=true;

  sql=QString("select ID from REPL_PACKAGES where ")+
    QString().sprintf("(DIRECTION=%u)&&",dir)+
    "(REPLICATOR_NAME=\""+RDEscapeString(conv_repl_name)+"\") "+
    "order by ID";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *id=q->value(0).toInt();
    if(!RDCopy(RDReplConveyor::fileName(*id),outfile)) {
      ret=false;
      syslog(LOG_WARNING,"unable to retrive replicator package \"%s\"",
	     (const char *)RDReplConveyor::fileName(*id));
    }
  }
  else {
    ret=false;
  }
  delete q;

  return ret;
}


void RDReplConveyor::pop(int id) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("delete from REPL_PACKAGES where ID=%d",id);
  q=new RDSqlQuery(sql);
  delete q;

  unlink(fileName(id));
}


void RDReplConveyor::popNextPackage(Direction dir)
{
  int id=-1;

  if(nextPackageReady(&id,dir)) {
    pop(id);
  }
}


QString RDReplConveyor::fileName(int id)
{
  return QString(RD_REPL_DIR)+QString().sprintf("/package%06d.pkg",id);
}
