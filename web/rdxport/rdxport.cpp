// rdxport.cpp
//
// Rivendell web service portal
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdatetime.h>
#include <qstringlist.h>

#include <rdcgiapplication.h>
#include <rdweb.h>
#include <rdformpost.h>
#include <rdxport_interface.h>
#include <dbversion.h>

#include "rdxport.h"

Xport::Xport(QObject *parent)
  :QObject(parent)
{
  //
  // Drop Root Perms
  //
  if(setgid(rdcgi->config()->gid())<0) {
    XmlExit("Unable to set Rivendell group",500);
  }
  if(setuid(rdcgi->config()->uid())<0) {
    XmlExit("Unable to set Rivendell user",500);
  }
  if(getuid()==0) {
    XmlExit("Rivendell user should never be \"root\"!",500);
  }

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdxport: missing REQUEST_METHOD\n");
    Exit(0);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    printf("Content-type: text/html\n\n");
    printf("rdxport: invalid web method\n");
    Exit(0);
  }

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

  case RDXPORT_COMMAND_AUDIOSTORE:
    AudioStore();
    break;

  case RDXPORT_COMMAND_LISTLOGS:
    ListLogs();
    break;

  case RDXPORT_COMMAND_LISTLOG:
    ListLog();
    break;

  case RDXPORT_COMMAND_LISTSCHEDCODES:
    ListSchedCodes();
    break;

  case RDXPORT_COMMAND_ASSIGNSCHEDCODE:
    AssignSchedCode();
    break;

  case RDXPORT_COMMAND_UNASSIGNSCHEDCODE:
    UnassignSchedCode();
    break;

  case RDXPORT_COMMAND_LISTCARTSCHEDCODES:
    ListCartSchedCodes();
    break;

  case RDXPORT_COMMAND_LISTSERVICES:
    ListServices();
    break;

  default:
    printf("Content-type: text/html\n\n");
    printf("rdxport: missing/invalid command\n");
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
  rdcgi->setName(name);

  return rdcgi->user()->checkPassword(passwd,false);
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
  RDCgiApplication a(argc,argv);
  new Xport();
  return a.exec();
}
