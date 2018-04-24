// rdclilogedit.cpp
//
// A command-line log editor for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rdcreate_log.h>
#include <rdescape_string.h>
#include <rdsvc.h>

#include "rdclilogedit.h"

void MainObject::Addcart(int line,unsigned cartnum)
{
  if(line>edit_log_event->size()) {
    line=edit_log_event->size();
  }
  edit_log_event->insert(line,1);
  edit_log_event->logLine(line)->
    setTransType(rda->airplayConf()->defaultTransType());
  edit_log_event->logLine(line)->setFadeupGain(-3000);
  edit_log_event->logLine(line)->setFadedownGain(-3000);
  edit_log_event->logLine(line)->setCartNumber(cartnum);
  edit_log_event->refresh(line);
  edit_modified=true;
}


void MainObject::Addchain(int line,const QString &logname)
{
  if(line>edit_log_event->size()) {
    line=edit_log_event->size();
  }
  edit_log_event->insert(line,1);
  edit_log_event->logLine(line)->setType(RDLogLine::Chain);
  edit_log_event->logLine(line)->
    setTransType(rda->airplayConf()->defaultTransType());
  edit_log_event->logLine(line)->setMarkerLabel(logname);
  edit_log_event->refresh(line);
  edit_modified=true;
}


void MainObject::Addmarker(int line)
{
  if(line>edit_log_event->size()) {
    line=edit_log_event->size();
  }
  edit_log_event->insert(line,1);
  edit_log_event->logLine(line)->setType(RDLogLine::Marker);
  edit_log_event->logLine(line)->
    setTransType(rda->airplayConf()->defaultTransType());
  edit_log_event->logLine(line)->setMarkerLabel(tr("Label"));
  edit_log_event->logLine(line)->setMarkerComment(tr("Marker Comment"));
  edit_log_event->refresh(line);
  edit_modified=true;
}


void MainObject::Addtrack(int line)
{
  if(line>edit_log_event->size()) {
    line=edit_log_event->size();
  }
  edit_log_event->insert(line,1);
  edit_log_event->logLine(line)->setType(RDLogLine::Track);
  edit_log_event->logLine(line)->
    setTransType(rda->airplayConf()->defaultTransType());
  edit_log_event->logLine(line)->setMarkerComment(tr("Voice Track"));
  edit_log_event->refresh(line);
  edit_modified=true;
}


void MainObject::Deletelog(QString logname)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Normalize log name case
  //
  // FIXME: This should really be handled by use of collations in the
  //        where clause.
  //
  sql=QString("select NAME from LOGS where ")+
    "NAME=\""+RDEscapeString(logname)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    logname=q->value(0).toString();
  }
  delete q;

  if((edit_log==NULL)||(edit_log->name()!=logname)) {
    RDLog *log=new RDLog(logname);
    if(log->exists()) {
      if(!log->remove(rda->station(),rda->user(),rda->config())) {
	fprintf(stderr,"deletelog: audio deletion error, log not deleted\n");
      }
    }
    else {
      fprintf(stderr,"deletelog: no such log\n");
    }
    delete log;
  }
  else {
    fprintf(stderr,"deletelog: log currently loaded (try \"unload\" first)\n");
  }
}


void MainObject::Header() const
{
  printf(" Description: %s\n",(const char *)edit_description);
  printf("     Service: %s\n",(const char *)edit_service);
  if(edit_auto_refresh) {
    printf("Auto Refresh: Yes\n");
  }
  else {
    printf("Auto Refresh: No\n");
  }
  if(edit_start_date.isNull()) {
    printf("  Start Date: None\n");
  }
  else {
    printf("  Start Date: %s\n",
	   (const char *)edit_start_date.toString("yyyy-MM-dd"));
  }
  if(edit_end_date.isNull()) {
    printf("    End Date: None\n");
  }
  else {
    printf("    End Date: %s\n",
	   (const char *)edit_end_date.toString("yyyy-MM-dd"));
  }
  if(edit_purge_date.isNull()) {
    printf("  Purge Date: None\n");
  }
  else {
    printf("  Purge Date: %s\n",
	   (const char *)edit_purge_date.toString("yyyy-MM-dd"));
  }
}


void MainObject::List()
{
  for(int i=0;i<edit_log_event->size();i++) {
    printf("%4d %s\n",i,(const char *)ListLine(edit_log_event,i));
  }
  printf("%4d  --- end of log ---\n",edit_log_event->size());
}


void MainObject::ListLogs() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select NAME from LOGS order by NAME");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("%s\n",(const char *)q->value(0).toString());
  }
  delete q;
}


void MainObject::Listservices() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select NAME from SERVICES order by NAME");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("%s\n",(const char *)q->value(0).toString());
  }
  delete q;
}


void MainObject::Load(QString logname)
{
  QString sql;
  RDSqlQuery *q;

  if(edit_log!=NULL) {
    delete edit_log;
    edit_log=NULL;
  }
  if(edit_log_event!=NULL) {
    delete edit_log_event;
    edit_log_event=NULL;
  }

  //
  // Normalize log name case
  //
  // FIXME: This should really be handled by use of collations in the
  //        where clause.
  //
  sql=QString("select NAME from LOGS where ")+
    "NAME=\""+RDEscapeString(logname)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    logname=q->value(0).toString();
  }
  delete q;

  edit_log=new RDLog(logname);
  if(edit_log->exists()) {
    edit_log_event=new RDLogEvent(RDLog::tableName(logname));
    edit_log_event->load();
    edit_description=edit_log->description();
    edit_service=edit_log->service();
    edit_start_date=edit_log->startDate();
    edit_end_date=edit_log->endDate();
    edit_purge_date=edit_log->purgeDate();
    edit_auto_refresh=edit_log->autoRefresh();
    edit_modified=false;
  }
  else {
    fprintf(stderr,"log \"%s\" does not exist\n",(const char *)logname);
    delete edit_log;
    edit_log=NULL;
  }
}


QString MainObject::ListLine(RDLogEvent *evt,int line) const
{
  QString ret="";
  RDLogLine *logline=evt->logLine(line);

  switch(logline->timeType()) {
  case RDLogLine::Hard:
    ret+=QString().
      sprintf("T%s  ",(const char *)logline->startTime(RDLogLine::Logged).
	      toString("hh:mm:ss"));
    break;

  case RDLogLine::Relative:
    ret+=QString().
      sprintf(" %s  ",(const char *)evt->blockStartTime(line).
	      toString("hh:mm:ss"));
    break;

  case RDLogLine::NoTime:
    ret+="          ";
    break;
  }
  ret+=QString().sprintf("%-7s",
		(const char *)RDLogLine::transText(logline->transType()));
  switch(logline->type()) {
  case RDLogLine::Cart:
  case RDLogLine::Macro:
    ret+=QString().sprintf("%06u   ",logline->cartNumber());
    ret+=QString().sprintf("%-12s",(const char *)logline->groupName());
    ret+=QString().sprintf("%5s",
      (const char *)RDGetTimeLength(logline->forcedLength(),false,false))+"  ";
    ret+=logline->title();
    break;

  case RDLogLine::Marker:
    ret+="MARKER   ";
    ret+="            ";
    ret+="       ";
    ret+=logline->markerComment();
    break;

  case RDLogLine::Track:
    ret+="TRACK    ";
    ret+="            ";
    ret+="       ";
    ret+=logline->markerComment();
    break;

  case RDLogLine::Chain:
    ret+="LOG CHN  ";
    ret+="            ";
    ret+="       ";
    ret+=logline->markerLabel();
    break;

  case RDLogLine::MusicLink:
    ret+="LINK     ";
    ret+="            ";
    ret+="       ";
    ret+="[music import]";
    break;

  case RDLogLine::TrafficLink:
    ret+="LINK     ";
    ret+="            ";
    ret+="       ";
    ret+="[traffic import]";
    break;

  case RDLogLine::OpenBracket:
  case RDLogLine::CloseBracket:
  case RDLogLine::UnknownType:
    break;
  }
  return ret;
}


void MainObject::New(const QString &logname)
{
  QString sql;
  RDSqlQuery *q;

  if(edit_log!=NULL) {
    delete edit_log;
  }
  if(edit_log_event!=NULL) {
    delete edit_log_event;
  }
  edit_log=new RDLog(logname);
  if(!edit_log->exists()) {
    edit_log_event=new RDLogEvent(RDLog::tableName(logname));
    edit_description=logname+" log";
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql);
    if(q->first()) {
      edit_service=q->value(0).toString();
    }
    delete q;
    edit_start_date=QDate();
    edit_end_date=QDate();
    edit_purge_date=QDate();
    edit_auto_refresh=false;
    edit_new_log=true;
    edit_modified=false;
  }
  else {
    fprintf(stderr,"new: log already exists\n");
  }
}


void MainObject::Remove(int line)
{
  edit_log_event->remove(line,1);
  edit_modified=true;
}


void MainObject::Save()
{
  if(edit_new_log) {
    Saveas(edit_log->name());
  }
  else {
    edit_log_event->save();
    edit_log->setDescription(edit_description);
    edit_log->setStartDate(edit_start_date);
    edit_log->setEndDate(edit_end_date);
    edit_log->setPurgeDate(edit_purge_date);
    edit_log->setAutoRefresh(edit_auto_refresh);
    edit_log->setService(edit_service);
    edit_log->
      setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    edit_modified=false;
  }
}


void MainObject::Saveas(const QString &logname)
{
  QString sql;
  RDSqlQuery *q;

  RDLog *log=new RDLog(logname);
  if(!log->exists()) {
    sql=QString("insert into LOGS set ")+
      "NAME=\""+RDEscapeString(logname)+"\","+
      "TYPE=0,"+
      "DESCRIPTION=\""+RDEscapeString(edit_description)+"\","+
      "ORIGIN_USER=\""+RDEscapeString(rda->user()->name())+"\","+
      "ORIGIN_DATETIME=now(),"+
      "LINK_DATETIME=now(),"+
      "MODIFIED_DATETIME=now(),"+
      "START_DATE="+RDCheckDateTime(edit_start_date,"yyyy-MM-dd")+","+
      "END_DATE="+RDCheckDateTime(edit_end_date,"yyyy-MM-dd")+","+
      "PURGE_DATE="+RDCheckDateTime(edit_purge_date,"yyyy-MM-dd")+","+
      "AUTO_REFRESH=\""+RDYesNo(edit_auto_refresh)+"\","+
      "SERVICE=\""+RDEscapeString(edit_service)+"\"";
    q=new RDSqlQuery(sql);
    delete q;
    RDCreateLogTable(RDLog::tableName(logname));
    edit_log_event->setLogName(RDLog::tableName(logname));
    edit_log_event->save();
    delete edit_log;
    edit_log=log;
    edit_modified=false;
    edit_new_log=false;
  }
  else {
    fprintf(stderr,"saveas: log already exists\n");
    delete log;
  }
}


void MainObject::Setautorefresh(bool state)
{
  edit_auto_refresh=state;
  edit_modified=true;
}


void MainObject::Setcart(int line,unsigned cartnum)
{
  RDLogLine *logline=edit_log_event->logLine(line);
  if(logline!=NULL) {
    if((logline->type()==RDLogLine::Cart)||
       (logline->type()==RDLogLine::Macro)) {
      logline->setCartNumber(cartnum);
      edit_log_event->refresh(line);
      edit_modified=true;
    }
    else {
      fprintf(stderr,"setcart: incompatible event type\n");
    }
  }
  else {
    fprintf(stderr,"setcart: no such line\n");
  }
}


void MainObject::Setcomment(int line,const QString &str)
{
  RDLogLine *logline=edit_log_event->logLine(line);
  if(logline!=NULL) {
    if((logline->type()==RDLogLine::Marker)||
       (logline->type()==RDLogLine::Track)) {
      logline->setMarkerComment(str);
      edit_log_event->refresh(line);
      edit_modified=true;
    }
    else {
      fprintf(stderr,"setcomment: incompatible event type\n");
    }
  }
  else {
    fprintf(stderr,"setcomment: no such line\n");
  }
}


void MainObject::Setdesc(const QString &str)
{
  edit_description=str;
  edit_modified=true;
}


void MainObject::Setenddate(const QDate &date)
{
  edit_end_date=date;
  edit_modified=true;
}


void MainObject::Setlabel(int line,const QString &str)
{
  RDLogLine *logline=edit_log_event->logLine(line);
  if(logline!=NULL) {
    if((logline->type()==RDLogLine::Chain)||
       (logline->type()==RDLogLine::Marker)) {
      logline->setMarkerLabel(str);
      edit_log_event->refresh(line);
      edit_modified=true;
    }
    else {
      fprintf(stderr,"setlabel: incompatible event type\n");
    }
  }
  else {
    fprintf(stderr,"setlabel: no such line\n");
  }
}


void MainObject::Setpurgedate(const QDate &date)
{
  edit_purge_date=date;
  edit_modified=true;
}


void MainObject::Setservice(const QString &str)
{
  RDSvc *svc=new RDSvc(str);
  if(svc->exists()) {
    edit_service=str;
    edit_modified=true;
  }
  else {
    fprintf(stderr,"setservice: no such service\n");
    fprintf(stderr,"(Do \"listservices\" for a list\n");
  }
  delete svc;
}


void MainObject::Settime(int line,RDLogLine::TimeType type,const QTime &time)
{
  edit_log_event->logLine(line)->setTimeType(type);
  edit_log_event->logLine(line)->setStartTime(RDLogLine::Logged,time);
  edit_modified=true;
}


void MainObject::Settrans(int line,RDLogLine::TransType type)
{
  edit_log_event->logLine(line)->setTransType(type);
  edit_log_event->refresh(line);
  edit_modified=true;
}


void MainObject::Setstartdate(const QDate &date)
{
  edit_start_date=date;
  edit_modified=true;
}


void MainObject::Unload()
{
  if(edit_log!=NULL) {
    delete edit_log;
    edit_log=NULL;
  }
  if(edit_log_event!=NULL) {
    delete edit_log_event;
    edit_log_event=NULL;
  }
  edit_modified=false;
}
