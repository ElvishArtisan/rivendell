// rdxport.cpp
//
// Rivendell web service portal
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdxport.cpp,v 1.10.2.3 2013/10/14 04:23:54 cvs Exp $
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
#include <rdxport_interface.h>
#include <dbversion.h>

#include <rdxport.h>

Xport::Xport(QObject *parent,const char *name)
  :QObject(parent,name)
{
  xport_user=NULL;

  //
  // Read Configuration
  //
  xport_config=new RDConfig();
  xport_config->load();

  //
  // Drop Root Perms
  //
  if(setgid(xport_config->gid())<0) {
    XmlExit("Unable to set Rivendell group",500);
  }
  if(setuid(xport_config->uid())<0) {
    XmlExit("Unable to set Rivendell user",500);
  }
  if(getuid()==0) {
    XmlExit("Rivendell user should never be \"root\"!",500);
  }

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(xport_config->mysqlDriver());
  if(!db) {
    printf("Content-type: text/html\n\n");
    printf("rdfeed: unable to initialize connection to database\n");
    Exit(0);
  }
  db->setDatabaseName(xport_config->mysqlDbname());
  db->setUserName(xport_config->mysqlUsername());
  db->setPassword(xport_config->mysqlPassword());
  db->setHostName(xport_config->mysqlHostname());
  if(!db->open()) {
    printf("Content-type: text/html\n\n");
    printf("rdxport: unable to connect to database\n");
    db->removeDatabase(xport_config->mysqlDbname());
    Exit(0);
  }
  RDSqlQuery *q=new RDSqlQuery("select DB from VERSION");
  if(!q->first()) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdxport: missing/invalid database version!\n");
    db->removeDatabase(xport_config->mysqlDbname());
    Exit(0);
  }
  if(q->value(0).toUInt()!=RD_VERSION_DATABASE) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n\n");
    printf("rdxport: skewed database version!\n");
    db->removeDatabase(xport_config->mysqlDbname());
    Exit(0);
  }
  delete q;

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdxport: missing REQUEST_METHOD\n");
    db->removeDatabase(xport_config->mysqlDbname());
    Exit(0);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    printf("Content-type: text/html\n\n");
    printf("rdxport: invalid web method\n");
    db->removeDatabase(xport_config->mysqlDbname());
    Exit(0);
  }

  //
  // Load System Settings
  //
  xport_system=new RDSystem();

  //
  // Generate Post
  //
  xport_post=new RDFormPost(RDFormPost::AutoEncoded,false);
  if(xport_post->error()!=RDFormPost::ErrorOk) {
    XmlExit(xport_post->errorString(xport_post->error()),400);
    Exit(0);
  }

  //
  // Authenticate Connection
  //
  if(!Authenticate()) {
    XmlExit("Invalid User",403);
  }

  //
  // Read Command Variable and Dispatch 
  //
  int command=xport_post->value("COMMAND").toInt();
  switch(command) {
  case RDXPORT_COMMAND_EXPORT:
    Export();
    break;

  case RDXPORT_COMMAND_IMPORT:
    Import();
    break;

  case RDXPORT_COMMAND_DELETEAUDIO:
    DeleteAudio();
    break;

  case RDXPORT_COMMAND_LISTGROUPS:
    ListGroups();
    break;

  case RDXPORT_COMMAND_LISTGROUP:
    ListGroup();
    break;

  case RDXPORT_COMMAND_ADDCART:
    AddCart();
    break;

  case RDXPORT_COMMAND_LISTCARTS:
    ListCarts();
    break;

  case RDXPORT_COMMAND_LISTCART:
    ListCart();
    break;

  case RDXPORT_COMMAND_EDITCART:
    EditCart();
    break;

  case RDXPORT_COMMAND_REMOVECART:
    RemoveCart();
    break;

  case RDXPORT_COMMAND_ADDCUT:
    AddCut();
    break;

  case RDXPORT_COMMAND_LISTCUTS:
    ListCuts();
    break;

  case RDXPORT_COMMAND_LISTCUT:
    ListCut();
    break;

  case RDXPORT_COMMAND_EDITCUT:
    EditCut();
    break;

  case RDXPORT_COMMAND_REMOVECUT:
    RemoveCut();
    break;

  case RDXPORT_COMMAND_EXPORT_PEAKS:
    ExportPeaks();
    break;

  case RDXPORT_COMMAND_TRIMAUDIO:
    TrimAudio();
    break;

  case RDXPORT_COMMAND_COPYAUDIO:
    CopyAudio();
    break;

  case RDXPORT_COMMAND_AUDIOINFO:
    AudioInfo();
    break;

  case RDXPORT_COMMAND_LISTLOGS:
    ListLogs();
    break;

  case RDXPORT_COMMAND_LISTLOG:
    ListLog();
    break;

  case RDXPORT_COMMAND_LISTSERVICES:
    ListServices();
    break;

  default:
    printf("Content-type: text/html\n\n");
    printf("rdxport: missing/invalid command\n");
    db->removeDatabase(xport_config->mysqlDbname());
    Exit(0);
    break;
  }

  Exit(0);
}


bool Xport::Authenticate()
{
  QString name;
  QString passwd;

  if(!xport_post->getValue("LOGIN_NAME",&name)) {
    return false;
  }
  if(!xport_post->getValue("PASSWORD",&passwd)) {
    return false;
  }
  xport_user=new RDUser(name);

  return xport_user->checkPassword(passwd,false);
}


void Xport::Exit(int code)
{
  if(xport_post!=NULL) {
    delete xport_post;
  }
  exit(code);
}


void Xport::XmlExit(const QString &str,int code,RDAudioConvert::ErrorCode err)
{
  if(xport_post!=NULL) {
    delete xport_post;
  }
  RDXMLResult(str,code,err);
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new Xport(NULL,"main");
  return a.exec();
}
