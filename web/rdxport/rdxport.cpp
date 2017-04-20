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
#include <openssl/sha.h>

#include <map>

#include <qapplication.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdweb.h>
#include <rdformpost.h>
#include <rdxport_interface.h>
#include <dbversion.h>

#include <rdxport.h>

Xport::Xport(QObject *parent)
  :QObject(parent)
{
  xport_user=NULL;

  //
  // Read Configuration
  //
  xport_config=new RDConfig();
  xport_config->load();

  //
  // Drop root permissions
  //
  if(setgid(xport_config->gid())<0) {
    XmlExit("Unable to set Rivendell group",500,"rdxport.cpp",LINE_NUMBER);
  }
  if(setuid(xport_config->uid())<0) {
    XmlExit("Unable to set Rivendell user",500,"rdxport.cpp",LINE_NUMBER);
  }
  if(getuid()==0) {
    XmlExit("Rivendell user should never be \"root\"!",500,"rdxport.cpp",
	    LINE_NUMBER);
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
  xport_station=new RDStation(xport_config->stationName());

  //
  // Generate Post
  //
  xport_post=new RDFormPost(RDFormPost::AutoEncoded,false);
  if(xport_post->error()!=RDFormPost::ErrorOk) {
    XmlExit(xport_post->errorString(xport_post->error()),400,"rdxport.cpp",
	    LINE_NUMBER);
    Exit(0);
  }

  //
  // Authenticate Connection
  //
  if(!Authenticate()) {
    XmlExit("Invalid User",403,"rdxport.cpp",LINE_NUMBER);
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

  case RDXPORT_COMMAND_ADDLOG:
    AddLog();
    break;

  case RDXPORT_COMMAND_DELETELOG:
    DeleteLog();
    break;

  case RDXPORT_COMMAND_LISTLOGS:
    ListLogs();
    break;

  case RDXPORT_COMMAND_LISTLOG:
    ListLog();
    break;

  case RDXPORT_COMMAND_SAVELOG:
    SaveLog();
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

  case RDXPORT_COMMAND_REHASH:
    Rehash();
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
  QString ticket;
  QString sql;
  RDSqlQuery *q;
  QString name;
  QString passwd;

  //
  // First, attempt ticket authentication
  //
  if(xport_post->getValue("TICKET",&ticket)) {
    sql=QString("select LOGIN_NAME from WEBAPI_AUTHS where ")+
      "(TICKET=\""+RDEscapeString(ticket)+"\")&&"+
      "(IPV4_ADDRESS=\""+xport_post->clientAddress().toString()+"\")&&"+
      "(EXPIRATION_DATETIME>now())";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      xport_user=new RDUser(q->value(0).toString());
      delete q;
      return true;
    }
    delete q;
  }

  //
  // Next, check the whitelist
  //
  if(!xport_post->getValue("LOGIN_NAME",&name)) {
    return false;
  }
  if(!xport_post->getValue("PASSWORD",&passwd)) {
    return false;
  }
  xport_user=new RDUser(name);
  if(!xport_user->exists()) {
    return false;
  }
  if((xport_post->clientAddress().toIPv4Address()>>24)==127) {  // Localhost
    TryCreateTicket(name);
    return true;
  }
  sql=QString("select NAME from STATIONS where ")+
    "IPV4_ADDRESS=\""+xport_post->clientAddress().toString()+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    delete q;
    TryCreateTicket(name);
    return true;
  }
  delete q;

  //
  // Finally, try password
  //
  if(!xport_user->checkPassword(passwd,false)) {
    return false;
  }
  TryCreateTicket(name);

  return true;
}


void Xport::TryCreateTicket(const QString &name)
{
  QString ticket;
  QString passwd;
  int command;
  unsigned char rawstr[1024];
  unsigned char sha1[SHA_DIGEST_LENGTH];
  QString sql;
  RDSqlQuery *q;

  if(xport_post->getValue("COMMAND",&command)) {
    if(command==RDXPORT_COMMAND_CREATETICKET) {
      QDateTime now=QDateTime::currentDateTime();
      snprintf((char *)rawstr,1024,"%s %s %s",
	       (const char *)now.toString("yyyy-MM-dd hh:mm:ss.zzz"),
	       (const char *)name,
	       (const char *)xport_post->clientAddress().toString());
      SHA1(rawstr,strlen((char *)rawstr),sha1);
      ticket="";
      for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
	ticket+=QString().sprintf("%02x",0xFF&rawstr[i]);
      }
      sql=QString("insert into WEBAPI_AUTHS set ")+
	"TICKET=\""+RDEscapeString(ticket)+"\","+
	"LOGIN_NAME=\""+RDEscapeString(name)+"\","+
	"IPV4_ADDRESS=\""+xport_post->clientAddress().toString()+"\","+
	"EXPIRATION_DATETIME=\""+
	now.addSecs(xport_user->webapiAuthTimeout()).
	toString("yyyy-MM-dd hh:mm:ss")+"\"";
      q=new RDSqlQuery(sql);
      delete q;
      printf("Content-type: application/xml\n\n");
      printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
      printf("<ticketInfo>\n");
      printf("  %s\n",(const char *)RDXmlField("ticket",ticket));
      printf("  %s\n",(const char *)
	  RDXmlField("expires",now.addSecs(xport_user->webapiAuthTimeout())));
      printf("</ticketInfo>\n");
      exit(0);
    }
  }
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
#ifdef RDXPORT_DEBUG
  if(srcline>0) {
    RDXMLResult(str+"\""+srcfile+"\" "+QString().sprintf("line %d",srcline),
		code,err);
  }
  else {
    RDXMLResult(str,code,err);
  }
#else
  RDXMLResult(str,code,err);
#endif  // RDXPORT_DEBUG
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new Xport();
  return a.exec();
}
