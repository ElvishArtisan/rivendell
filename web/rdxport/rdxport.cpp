// rdxport.cpp
//
// Rivendell web service portal
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <map>

#include <QApplication>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdweb.h>
#include <rdformpost.h>
#include <rdxport_interface.h>
#include <dbversion.h>

#include "rdxport.h"

Xport::Xport(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("rdxport.cgi",
			      "rdxport.cgi",RDXPORT_CGI_USAGE,false,this));
  if(!rda->open(&err_msg,NULL,false,false)) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n");
    printf("\n");
    printf("rdxport.cgi: %s\n",(const char *)err_msg.toUtf8());
    Exit(0);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      printf("Content-type: text/html\n");
      printf("Status: 500\n");
      printf("\n");
      printf("rdxport.cgi: unknown command option \"%s\"\n",
	     (const char *)rda->cmdSwitch()->key(i).toUtf8());
      Exit(0);
    }
  }

  //
  // Drop root permissions
  //
  if(setgid(rda->config()->gid())<0) {
    XmlExit("Unable to set Rivendell group",500,"rdxport.cpp",LINE_NUMBER);
  }
  if(setuid(rda->config()->uid())<0) {
    XmlExit("Unable to set Rivendell user",500,"rdxport.cpp",LINE_NUMBER);
  }
  if(getuid()==0) {
    XmlExit("Rivendell user should never be \"root\"!",500,"rdxport.cpp",
	    LINE_NUMBER);
  }

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    printf("Content-type: text/html\n\n");
    printf("rdxport: missing REQUEST_METHOD\n");
    Exit(0);
  }
  if(QString(getenv("REQUEST_METHOD")).toLower()!="post") {
    printf("Content-type: text/html\n\n");
    printf("rdxport: invalid web method\n");
    Exit(0);
  }
  if(getenv("REMOTE_ADDR")!=NULL) {
    xport_remote_address.setAddress(getenv("REMOTE_ADDR"));
  }
  if(getenv("REMOTE_HOST")!=NULL) {
    xport_remote_hostname=getenv("REMOTE_HOST");
  }
  if(xport_remote_hostname.isEmpty()) {
    xport_remote_hostname=xport_remote_address.toString();
  }

  //
  // Generate Post
  //
  xport_post=new RDFormPost(RDFormPost::AutoEncoded,true);
  if(xport_post->error()!=RDFormPost::ErrorOk) {
    XmlExit(xport_post->errorString(xport_post->error()),400,"rdxport.cpp",
	    LINE_NUMBER);
    Exit(0);
  }
  /*
  printf("Content-type: text/html; charset=UTF-8\n\n");
  xport_post->dump();
  exit(0);
  */
  //
  // Authenticate Connection
  //
  if(!Authenticate()) {
    XmlExit("Invalid User",403,"rdxport.cpp",LINE_NUMBER);
  }

  //
  // Connect to ripcd(8)
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void Xport::ripcConnectedData(bool state)
{
  if(!state) {
    XmlExit("unable to connect to ripc service",500,"rdxport.cpp",LINE_NUMBER);
    Exit(0);
  }

  //
  // Read Command Variable and Dispatch 
  //
  int command=xport_post->value("COMMAND").toInt();

  switch(command) {
  case RDXPORT_COMMAND_EXPORT:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_EXPORT");
    Export();
    break;

  case RDXPORT_COMMAND_IMPORT:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_IMPORT");
    Import();
    break;

  case RDXPORT_COMMAND_DELETEAUDIO:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_DELETEAUDIO");
    DeleteAudio();
    break;

  case RDXPORT_COMMAND_LISTGROUPS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTGROUPS");
    ListGroups();
    break;

  case RDXPORT_COMMAND_LISTGROUP:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTGROUP");
    ListGroup();
    break;

  case RDXPORT_COMMAND_ADDCART:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_ADDCART");
    AddCart();
    break;

  case RDXPORT_COMMAND_LISTCARTS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTCARTS");
    ListCarts();
    break;

  case RDXPORT_COMMAND_LISTCART:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTCART");
    ListCart();
    break;

  case RDXPORT_COMMAND_EDITCART:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_EDITCART");
    EditCart();
    break;

  case RDXPORT_COMMAND_REMOVECART:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_REMOVECART");
    RemoveCart();
    break;

  case RDXPORT_COMMAND_ADDCUT:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_ADDCUT");
    AddCut();
    break;

  case RDXPORT_COMMAND_LISTCUTS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTCUTS");
    ListCuts();
    break;

  case RDXPORT_COMMAND_LISTCUT:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTCUT");
    ListCut();
    break;

  case RDXPORT_COMMAND_EDITCUT:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_EDITCUT");
    EditCut();
    break;

  case RDXPORT_COMMAND_REMOVECUT:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_REMOVECUT");
    RemoveCut();
    break;

  case RDXPORT_COMMAND_EXPORT_PEAKS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_EXPORT_PEAKS");
    ExportPeaks();
    break;

  case RDXPORT_COMMAND_TRIMAUDIO:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_TRIMAUDIO");
    TrimAudio();
    break;

  case RDXPORT_COMMAND_COPYAUDIO:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_COPYAUDIO");
    CopyAudio();
    break;

  case RDXPORT_COMMAND_AUDIOINFO:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_AUDIOINFO");
    AudioInfo();
    break;

  case RDXPORT_COMMAND_AUDIOSTORE:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_AUDIOSTORE");
    AudioStore();
    break;

  case RDXPORT_COMMAND_ADDLOG:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_ADDLOG");
    AddLog();
    break;

  case RDXPORT_COMMAND_DELETELOG:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_DELETELOG");
    DeleteLog();
    break;

  case RDXPORT_COMMAND_LISTLOGS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTLOGS");
    ListLogs();
    break;

  case RDXPORT_COMMAND_LISTLOG:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTLOG");
    ListLog();
    break;

  case RDXPORT_COMMAND_SAVELOG:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_SAVELOG");
    SaveLog();
    break;

  case RDXPORT_COMMAND_LISTSCHEDCODES:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTSCHEDCODES");
    ListSchedCodes();
    break;

  case RDXPORT_COMMAND_ASSIGNSCHEDCODE:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_ASSIGNSCHEDCODE");
    AssignSchedCode();
    break;

  case RDXPORT_COMMAND_UNASSIGNSCHEDCODE:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_UNASSIGNSCHEDCODE");
    UnassignSchedCode();
    break;

  case RDXPORT_COMMAND_LISTCARTSCHEDCODES:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTCARTSCHEDCODES");
    ListCartSchedCodes();
    break;

  case RDXPORT_COMMAND_LISTSERVICES:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTSERVICES");
    ListServices();
    break;

  case RDXPORT_COMMAND_LISTSYSTEMSETTINGS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LISTSYSTEMSETTINGS");
    ListSystemSettings();
    break;

  case RDXPORT_COMMAND_LOCKLOG:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_LOCKLOG");
    LockLog();
    break;

  case RDXPORT_COMMAND_REHASH:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_REHASH");
    Rehash();
    break;

  case RDXPORT_COMMAND_SAVESTRING:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_SAVESTRING");
    SaveString();
    break;

  case RDXPORT_COMMAND_SAVEFILE:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_SAVEFILE");
    SaveFile();
    break;

  case RDXPORT_COMMAND_SAVE_PODCAST:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_SAVE_PODCAST");
    SavePodcast();
    break;

  case RDXPORT_COMMAND_GET_PODCAST:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_GET_PODCAST");
    GetPodcast();
    break;

  case RDXPORT_COMMAND_DELETE_PODCAST:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_DELETE_PODCAST");
    DeletePodcast();
    break;

  case RDXPORT_COMMAND_POST_PODCAST:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_POST_PODCAST");
    PostPodcast();
    break;

  case RDXPORT_COMMAND_REMOVE_PODCAST:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_REMOVE_PODCAST");
    RemovePodcast();
    break;

  case RDXPORT_COMMAND_DOWNLOAD_RSS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_DOWNLOAD_RSS");
    DownloadRss();
    break;

  case RDXPORT_COMMAND_POST_RSS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_POST_RSS");
    PostRss();
    break;

  case RDXPORT_COMMAND_REMOVE_RSS:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_REMOVE_RSS");
    RemoveRss();
    break;

  case RDXPORT_COMMAND_POST_IMAGE:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_POST_IMAGE");
    PostImage();
    break;

  case RDXPORT_COMMAND_REMOVE_IMAGE:
    rda->syslog(LOG_DEBUG,"processing RDXPORT_COMMAND_REMOVE_IMAGE");
    RemoveImage();
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
  bool used_ticket=false;
  bool ok=xport_post->authenticate(&used_ticket);

  if(ok&&(!used_ticket)) {
    TryCreateTicket(rda->user()->name());
  }

  return ok;
}


void Xport::TryCreateTicket(const QString &name)
{
  int command;

  if(xport_post->getValue("COMMAND",&command)) {
    if(command==RDXPORT_COMMAND_CREATETICKET) {
      QDateTime now=QDateTime::currentDateTime();
      QString ticket;
      QDateTime expire_datetime;
      if(rda->user()->createTicket(&ticket,&expire_datetime,
				   xport_post->clientAddress(),now)) {
	printf("Content-type: application/xml; charset=utf-8\n\n");
	printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	printf("<ticketInfo>\n");
	printf("  %s\n",RDXmlField("ticket",ticket).toUtf8().constData());
	printf("  %s\n",
	       RDXmlField("expires",expire_datetime).toUtf8().constData());
	printf("</ticketInfo>\n");
	exit(0);
      }
      else {
	XmlExit("Ticket creation failed",500,"rdxport.cpp",LINE_NUMBER);
      }
    }
  }
}


void Xport::SendNotification(RDNotification::Type type,
			     RDNotification::Action action,const QVariant &id)
{
  RDNotification *notify=new RDNotification(type,action,id);
  rda->ripc()->sendNotification(*notify);
  qApp->processEvents();
  delete notify;
}


void Xport::Exit(int code)
{
  if(xport_post!=NULL) {
    delete xport_post;
  }
  exit(code);
}


void Xport::XmlExit(const QString &str,int code,const QString &srcfile,
		    int srcline,RDAudioConvert::ErrorCode err)
{
  if(xport_post!=NULL) {
    delete xport_post;
  }
  if(code>=400) {
    rda->syslog(LOG_WARNING,"%s '%s' %s",
		(const char *)str.toUtf8(),(const char *)srcfile.toUtf8(),
		(srcline>0)?(const char *)QString::asprintf("line %d",srcline).
		toUtf8():"");
  }
#ifdef RDXPORT_DEBUG
  if(srcline>0) {
    RDXMLResult((str+" \""+srcfile+"\" "+QString::asprintf("line %d",srcline)).toUtf8(),
		code,err);
  }
  else {
    RDXMLResult(str.toUtf8(),code,err);
  }
#else
  RDXMLResult(str.toUtf8(),code,err);
#endif  // RDXPORT_DEBUG
  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication::setSetuidAllowed(true);
  QCoreApplication a(argc,argv,false);
  new Xport();
  return a.exec();
}
