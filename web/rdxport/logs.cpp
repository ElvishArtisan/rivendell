// logs.cpp
//
// Rivendell web service portal -- Log services
//
//   (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <rdcreate_log.h>
#include <rddb.h>
#include <rdformpost.h>
#include <rdweb.h>
#include <rdsvc.h>
#include <rduser.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdlog_line.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include <rdxport.h>

void Xport::AddLog()
{
  QString log_name;
  QString service_name;

  //
  // Get Arguments
  //
  if(!xport_post->getValue("LOG_NAME",&log_name)) {
    XmlExit("Missing LOG_NAME",400);
  }
  if(!xport_post->getValue("SERVICE_NAME",&service_name)) {
    XmlExit("Missing SERVICE_NAME",400);
  }
  RDSvc *svc=new RDSvc(service_name);
  if(!svc->exists()) {
    XmlExit("No such service",404);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->createLog()) {
    XmlExit("Unauthorized",404);
  }

  RDLog *log=new RDLog(log_name);
  if(!log->exists()) {
    delete log;
    log=new RDLog(log_name,true);
    if(!log->exists()) {
      delete log;
      XmlExit("Unable to create log",500);
    }
    log->setOriginUser(xport_user->name());
    log->setDescription("[new log]");
    log->setService(service_name);
  }
  delete log;
  RDCreateLogTable(RDLog::tableName(log_name));

  XmlExit("OK",200);
}


void Xport::DeleteLog()
{
  QString log_name;

  //
  // Get Arguments
  //
  if(!xport_post->getValue("LOG_NAME",&log_name)) {
    XmlExit("Missing LOG_NAME",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->deleteLog()) {
    XmlExit("Unauthorized",404);
  }

  RDLog *log=new RDLog(log_name);
  if(log->exists()) {
    if(!log->remove(xport_station,xport_user,xport_config)) {
      delete log;
      XmlExit("Unable to delete log",500);
    }
  }
  delete log;

  XmlExit("OK",200);
}


void Xport::ListLogs()
{
  QString sql;
  RDSqlQuery *q;
  RDLog *log;
  QString service_name="";
  QString log_name="";
  QString trackable;

  //
  // Get Options
  //
  xport_post->getValue("SERVICE_NAME",&service_name);
  xport_post->getValue("LOG_NAME",&log_name);
  xport_post->getValue("TRACKABLE",&trackable);

  //
  // Generate Log List
  //
  sql="select NAME from LOGS";
  if((!service_name.isEmpty())||(!log_name.isEmpty())||(trackable=="1")) {
    sql+=" where";
    if(!log_name.isEmpty()) {
      sql+=" (NAME=\""+RDEscapeString(log_name)+"\")&&";
    }
    if(!service_name.isEmpty()) {
      sql+=" (SERVICE=\""+RDEscapeString(service_name)+"\")&&";
    }
    if(trackable=="1") {
      sql+=" (SCHEDULED_TRACKS>0)&&";
    }
    sql=sql.left(sql.length()-2);
  }
  sql+=" order by NAME";
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<logList>\n");
  while(q->next()) {
    log=new RDLog(q->value(0).toString());
    printf("%s",(const char *)log->xml());
    delete log;
  }
  printf("</logList>\n");

  delete q;
  Exit(0);
}


void Xport::ListLog()
{
  RDLog *log;
  QString name="";

  //
  // Get Options
  //
  xport_post->getValue("NAME",&name);

  //
  // Verify that log exists
  //
  log=new RDLog(name);
  if(!log->exists()) {
    delete log;
    XmlExit("No such log",404);
  }

  //
  // Generate Log Listing
  //
  RDLogEvent *log_event=log->createLogEvent();
  log_event->load(true);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("%s\n",(const char *)log_event->xml());

  Exit(0);
}


void Xport::SaveLog()
{
  //
  // Verify User Perms
  //
  if((!xport_user->addtoLog())||(!xport_user->removefromLog())||(!xport_user->arrangeLog())) {
    XmlExit("No user privilege",404);
  }

  QString log_name;
  QString service_name;
  QString description;
  QDate purge_date;
  bool auto_refresh;
  QDate start_date;
  QDate end_date;
  int line_quantity;

  //
  // Header Data
  //
  if(!xport_post->getValue("LOG_NAME",&log_name)) {
    XmlExit("Missing LOG_NAME",400);
  }
  if(!xport_post->getValue("SERVICE_NAME",&service_name)) {
    XmlExit("Missing SERVICE_NAME",400);
  }
  if(!xport_post->getValue("DESCRIPTION",&description)) {
    XmlExit("Missing DESCRIPTION",400);
  }
  if(!xport_post->getValue("PURGE_DATE",&purge_date)) {
    XmlExit("Missing PURGE_DATE",400);
  }
  if(!xport_post->getValue("AUTO_REFRESH",&auto_refresh)) {
    XmlExit("Missing AUTO_REFRESH",400);
  }
  if(!xport_post->getValue("START_DATE",&start_date)) {
    XmlExit("Missing START_DATE",400);
  }
  if(!xport_post->getValue("END_DATE",&end_date)) {
    XmlExit("Missing END_DATE",400);
  }
  if(!xport_post->getValue("LINE_QUANTITY",&line_quantity)) {
    XmlExit("Missing LINE_QUANTITY",400);
  }

  //
  // Logline Data
  //
  RDLogEvent *logevt=new RDLogEvent(RDLog::tableName(log_name));
  for(int i=0;i<line_quantity;i++) {
    logevt->insert(i,1);
    RDLogLine *ll=logevt->logLine(i);
    QString line=QString().sprintf("LINE%d_",i);
    QString str;
    int integer1;
    int integer2;
    QDateTime datetime;
    QTime time;
    bool state;
    if(!xport_post->getValue(line+"_ID",&integer1)) {
      XmlExit("Missing "+line+"_ID",400);
    }
    ll->setId(integer1);

    if(!xport_post->getValue(line+"_TYPE",&integer1)) {
      XmlExit("Missing "+line+"_TYPE",400);
    }
    ll->setType((RDLogLine::Type)integer1);

    if(!xport_post->getValue(line+"_CART_NUMBER",&integer1)) {
      XmlExit("Missing "+line+"_CART_NUMBER",400);
    }
    ll->setCartNumber(integer1);

    if(!xport_post->getValue(line+"_START_TIME",&integer1)) {
      XmlExit("Missing "+line+"_START_TIME",400);
    }
    if(!xport_post->getValue(line+"_TIME_TYPE",&integer2)) {
      XmlExit("Missing "+line+"_TIME_TYPE",400);
    }
    ll->setStartTime((RDLogLine::StartTimeType)integer2,
		     QTime().addMSecs(integer1));

    if(!xport_post->getValue(line+"_GRACE_TIME",&integer1)) {
      XmlExit("Missing "+line+"_GRACE_TIME",400);
    }
    ll->setGraceTime(integer1);

    if(!xport_post->getValue(line+"_TRANS_TYPE",&str)) {
      XmlExit("Missing "+line+"_TRANS_TYPE",400);
    }
    integer1=-1;
    if(str.lower()=="play") {
      integer1=RDLogLine::Play;
    }
    if(str.lower()=="segue") {
      integer1=RDLogLine::Segue;
    }
    if(str.lower()=="stop") {
      integer1=RDLogLine::Stop;
    }
    if(integer1<0) {
      XmlExit("Invalid transition type in "+line+"_TRANS_TYPE",400);
    }
    ll->setTransType((RDLogLine::TransType)integer1);

    if(!xport_post->getValue(line+"_START_POINT",&integer1)) {
      XmlExit("Missing "+line+"_START_POINT",400);
    }
    ll->setStartPoint(integer1,RDLogLine::LogPointer);

    if(!xport_post->getValue(line+"_END_POINT",&integer1)) {
      XmlExit("Missing "+line+"_END_POINT",400);
    }
    ll->setEndPoint(integer1,RDLogLine::LogPointer);

    if(!xport_post->getValue(line+"_SEGUE_START_POINT",&integer1)) {
      XmlExit("Missing "+line+"_SEGUE_START_POINT",400);
    }
    ll->setSegueStartPoint(integer1,RDLogLine::LogPointer);

    if(!xport_post->getValue(line+"_SEGUE_END_POINT",&integer1)) {
      XmlExit("Missing "+line+"_SEGUE_END_POINT",400);
    }
    ll->setSegueEndPoint(integer1,RDLogLine::LogPointer);

    if(!xport_post->getValue(line+"_FADEUP_POINT",&integer1)) {
      XmlExit("Missing "+line+"_FADEUP_POINT",400);
    }
    ll->setFadeupPoint(integer1,RDLogLine::LogPointer);

    if(!xport_post->getValue(line+"_FADEDOWN_POINT",&integer1)) {
      XmlExit("Missing "+line+"_FADEDOWN_POINT",400);
    }
    ll->setFadedownPoint(integer1,RDLogLine::LogPointer);

    if(!xport_post->getValue(line+"_DUCK_UP_GAIN",&integer1)) {
      XmlExit("Missing "+line+"_DUCK_UP_GAIN",400);
    }
    ll->setDuckUpGain(integer1);

    if(!xport_post->getValue(line+"_DUCK_DOWN_GAIN",&integer1)) {
      XmlExit("Missing "+line+"_DUCK_DOWN_GAIN",400);
    }
    ll->setDuckDownGain(integer1);

    if(!xport_post->getValue(line+"_COMMENT",&str)) {
      XmlExit("Missing "+line+"_COMMENT",400);
    }
    ll->setMarkerComment(str);

    if(!xport_post->getValue(line+"_LABEL",&str)) {
      XmlExit("Missing "+line+"_LABEL",400);
    }
    ll->setMarkerLabel(str);

    if(!xport_post->getValue(line+"_ORIGIN_USER",&str)) {
      XmlExit("Missing "+line+"_ORIGIN_USER",400);
    }
    ll->setOriginUser(str);

    if(!xport_post->getValue(line+"_ORIGIN_DATETIME",&datetime)) {
      XmlExit("Missing "+line+"_ORIGIN_DATETIME",400);
    }
    ll->setOriginDateTime(datetime);

    if(!xport_post->getValue(line+"_EVENT_LENGTH",&integer1)) {
      XmlExit("Missing "+line+"_EVENT_LENGTH",400);
    }
    ll->setEventLength(integer1);

    if(!xport_post->getValue(line+"_LINK_EVENT_NAME",&str)) {
      XmlExit("Missing "+line+"_LINK_EVENT_NAME",400);
    }
    ll->setLinkEventName(str);

    if(!xport_post->getValue(line+"_LINK_START_TIME",&integer1)) {
      XmlExit("Missing "+line+"_LINK_START_TIME",400);
    }
    ll->setLinkStartTime(QTime().addMSecs(integer1));

    if(!xport_post->getValue(line+"_LINK_LENGTH",&integer1)) {
      XmlExit("Missing "+line+"_LINK_LENGTH",400);
    }
    ll->setLinkLength(integer1);

    if(!xport_post->getValue(line+"_LINK_START_SLOP",&integer1)) {
      XmlExit("Missing "+line+"_LINK_START_SLOP",400);
    }
    ll->setLinkStartSlop(integer1);

    if(!xport_post->getValue(line+"_LINK_END_SLOP",&integer1)) {
      XmlExit("Missing "+line+"_LINK_END_SLOP",400);
    }
    ll->setLinkEndSlop(integer1);

    if(!xport_post->getValue(line+"_LINK_ID",&integer1)) {
      XmlExit("Missing "+line+"_LINK_ID",400);
    }
    ll->setLinkId(integer1);

    if(!xport_post->getValue(line+"_LINK_EMBEDDED",&state)) {
      XmlExit("Missing "+line+"_LINK_EMBEDDED",400);
    }
    ll->setLinkEmbedded(state);

    if(!xport_post->getValue(line+"_EXT_START_TIME",&time)) {
      XmlExit("Missing "+line+"_EXT_START_TIME",400);
    }
    ll->setExtStartTime(time);

    if(!xport_post->getValue(line+"_EXT_CART_NAME",&str)) {
      XmlExit("Missing "+line+"_EXT_CART_NAME",400);
    }
    ll->setExtCartName(str);

    if(!xport_post->getValue(line+"_EXT_DATA",&str)) {
      XmlExit("Missing "+line+"_EXT_DATA",400);
    }
    ll->setExtData(str);

    if(!xport_post->getValue(line+"_EXT_EVENT_ID",&str)) {
      XmlExit("Missing "+line+"_EXT_EVENT_ID",400);
    }
    ll->setExtEventId(str);

    if(!xport_post->getValue(line+"_EXT_ANNC_TYPE",&str)) {
      XmlExit("Missing "+line+"_EXT_ANNC_TYPE",400);
    }
    ll->setExtAnncType(str);
  }

  RDLog *log=new RDLog(log_name);
  if(!log->exists()) {
    XmlExit("No such log",404);
  }
  log->setService(service_name);
  log->setDescription(description);
  log->setPurgeDate(purge_date);
  log->setAutoRefresh(auto_refresh);
  log->setStartDate(start_date);
  log->setEndDate(end_date);

  logevt->save();

  XmlExit("OK",200);
}
