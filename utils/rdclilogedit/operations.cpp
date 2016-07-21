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

#include <rdconf.h>
#include <rdcreate_log.h>
#include <rdescape_string.h>

#include "rdclilogedit.h"

void MainObject::Addcart(int line,unsigned cartnum)
{
  if(line>edit_log_event->size()) {
    line=edit_log_event->size();
  }
  edit_log_event->insert(line,1);
  edit_log_event->logLine(line)->
    setTransType(edit_airplay_conf->defaultTransType());
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
    setTransType(edit_airplay_conf->defaultTransType());
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
    setTransType(edit_airplay_conf->defaultTransType());
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
    setTransType(edit_airplay_conf->defaultTransType());
  edit_log_event->logLine(line)->setMarkerComment(tr("Voice Track"));
  edit_log_event->refresh(line);
  edit_modified=true;
}


void MainObject::Header() const
{
  printf(" Description: %s\n",(const char *)edit_log->description());
  printf("     Service: %s\n",(const char *)edit_log->service());
  if(edit_log->autoRefresh()) {
    printf("Auto Refresh: Yes\n");
  }
  else {
    printf("Auto Refresh: No\n");
  }
  if(edit_log->startDate().isNull()) {
    printf("  Start Date: None\n");
  }
  else {
    printf("  Start Date: %s\n",
	   (const char *)edit_log->startDate().toString("yyyy-MM-dd"));
  }
  if(edit_log->endDate().isNull()) {
    printf("    End Date: None\n");
  }
  else {
    printf("    End Date: %s\n",
	   (const char *)edit_log->endDate().toString("yyyy-MM-dd"));
  }
  if(edit_log->purgeDate().isNull()) {
    printf("  Purge Date: None\n");
  }
  else {
    printf("  Purge Date: %s\n",
	   (const char *)edit_log->purgeDate().toString("yyyy-MM-dd"));
  }
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


void MainObject::Load(const QString &logname)
{
  if(edit_log!=NULL) {
    delete edit_log;
    edit_log=NULL;
  }
  if(edit_log_event!=NULL) {
    delete edit_log_event;
    edit_log_event=NULL;
  }
  edit_log=new RDLog(logname);
  if(edit_log->exists()) {
    edit_log_event=new RDLogEvent(RDLog::tableName(logname));
    edit_log_event->load();
    edit_modified=false;
  }
  else {
    fprintf(stderr,"log \"%s\" does not exist\n",(const char *)logname);
    delete edit_log;
    edit_log=NULL;
  }
}


void MainObject::List()
{
  for(int i=0;i<edit_log_event->size();i++) {
    printf("%4d %s\n",i,(const char *)ListLine(edit_log_event,i));
  }
  fflush(stdout);
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
  if(edit_log!=NULL) {
    delete edit_log;
  }
  if(edit_log_event!=NULL) {
    delete edit_log_event;
  }
  edit_log=new RDLog(logname);
  if(!edit_log->exists()) {
    edit_log_event=new RDLogEvent(RDLog::tableName(logname));
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
    if(edit_log->exists()) {
      sql=QString("insert into LOGS set ")+
	"NAME=\""+RDEscapeString(logname)+"\","+
	"TYPE=0,"+
	"DESCRIPTION=\""+"Copy of "+RDEscapeString(edit_log->name())+"\","+
	"ORIGIN_USER=\""+RDEscapeString(edit_user->name())+"\","+
	"ORIGIN_DATETIME=now(),"+
	"LINK_DATETIME=now(),"+
	"MODIFIED_DATETIME=now(),"+
	"SERVICE=\""+edit_log->service()+"\"";
    }
    else {
      sql=QString("select NAME from SERVICES");
      q=new RDSqlQuery(sql);
      if(q->first()) {
	sql=QString("insert into LOGS set ")+
	  "NAME=\""+RDEscapeString(logname)+"\","+
	  "TYPE=0,"+
	  "DESCRIPTION=\""+RDEscapeString(logname+" log")+"\","+
	  "ORIGIN_USER=\""+RDEscapeString(edit_user->name())+"\","+
	  "ORIGIN_DATETIME=now(),"+
	  "LINK_DATETIME=now(),"+
	  "MODIFIED_DATETIME=now(),"+
	  "SERVICE=\""+RDEscapeString(q->value(0).toString())+"\"";
      }
      delete q;
    }
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
