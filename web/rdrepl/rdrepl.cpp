// rdrepl.cpp
//
// Rivendell replicator portal
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <map>

#include <qapplication.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <rddb.h>
#include <rdweb.h>
#include <rdformpost.h>
#include <rdrepl_interface.h>
#include <rdescape_string.h>
#include <dbversion.h>

#include <rdreplicator.h>
#include <rdrepl.h>

Repl::Repl(QObject *parent,const char *name)
  :QObject(parent,name)
{
  //
  // Read Configuration
  //
  repl_config=new RDConfig();
  repl_config->load();

  //
  // Drop Root Perms
  //
  if(setgid(repl_config->gid())<0) {
    XmlExit("Unable to set Rivendell group",500);
  }
  if(setuid(repl_config->uid())<0) {
    XmlExit("Unable to set Rivendell user",500);
  }
  if(getuid()==0) {
    XmlExit("Rivendell user should never be \"root\"!",500);
  }

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(repl_config->mysqlDriver());
  if(!db) {
    printf("Content-type: text/html\n\n");
    printf("rdrepl: unable to initialize connection to database\n");
    Exit(0);
  }
  db->setDatabaseName(repl_config->mysqlDbname());
  db->setUserName(repl_config->mysqlUsername());
  db->setPassword(repl_config->mysqlPassword());
  db->setHostName(repl_config->mysqlHostname());
  if(!db->open()) {
    printf("Content-type: text/html\n\n");
    printf("rdrepl: unable to connect to database\n");
    db->removeDatabase(repl_config->mysqlDbname());
    Exit(0);
  }
  RDSqlQuery *q=new RDSqlQuery("select DB from VERSION");
  if(!q->first()) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdrepl: missing/invalid database version!\n");
    db->removeDatabase(repl_config->mysqlDbname());
    Exit(0);
  }
  if(q->value(0).toUInt()!=RD_VERSION_DATABASE) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdrepl: skewed database version!\n");
    db->removeDatabase(repl_config->mysqlDbname());
    Exit(0);
  }
  delete q;

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdrepl: missing REQUEST_METHOD\n");
    db->removeDatabase(repl_config->mysqlDbname());
    Exit(0);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    printf("Content-type: text/html\n\n");
    printf("rdrepl: invalid web method\n");
    db->removeDatabase(repl_config->mysqlDbname());
    Exit(0);
  }

  //
  // Load System Settings
  //
  repl_system=new RDSystem();

  //
  // Generate Post
  //
  repl_post=new RDFormPost(RDFormPost::AutoEncoded,false);
  if(repl_post->error()!=RDFormPost::ErrorOk) {
    XmlExit(repl_post->errorString(repl_post->error()),400);
    Exit(0);
  }

  //
  // Authenticate Connection
  //
  if(!Authenticate()) {
    XmlExit("Invalid login",403);
  }

  //
  // Read Command Variable and Dispatch 
  //
  int command=repl_post->value("COMMAND").toInt();
  switch(command) {
  case RDREPL_COMMAND_EXPORT:
    Export();
    break;

  case RDREPL_COMMAND_IMPORT:
    Import();
    break;

  default:
    printf("Content-type: text/html\n\n");
    printf("rdrepl: missing/invalid command\n");
    db->removeDatabase(repl_config->mysqlDbname());
    Exit(0);
    break;
  }

  Exit(0);
}


bool Repl::Authenticate()
{
  QString name;
  QString passwd;
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  if(!repl_post->getValue("REPLICATOR_NAME",&name)) {
    return false;
  }
  if(!repl_post->getValue("PASSWORD",&passwd)) {
    return false;
  }
  sql=QString("select URL_PASSWORD,TYPE_ID from REPLICATORS where ")+
    "NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=(q->value(1).toInt()==RDReplicator::TypeRivendellSink)&&
      (q->value(0).toString()==passwd);
  }
  delete q;

  return ret;
}


void Repl::Exit(int code)
{
  if(repl_post!=NULL) {
    delete repl_post;
  }
  exit(code);
}


void Repl::XmlExit(const QString &str,int code,RDAudioConvert::ErrorCode err)
{
  if(repl_post!=NULL) {
    delete repl_post;
  }
  RDXMLResult(str,code,err);
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new Repl(NULL,"main");
  return a.exec();
}
