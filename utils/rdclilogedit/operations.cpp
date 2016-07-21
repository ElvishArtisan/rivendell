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

#include <rdcreate_log.h>
#include <rdescape_string.h>

#include "rdclilogedit.h"

void MainObject::Addcart(int line,unsigned cartnum)
{
  if(edit_user->addtoLog()) {
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
  else {
    fprintf(stderr,"addcart: insufficient privileges [Add Log Items]\n");
  }
}


void MainObject::Addchain(int line,const QString &logname)
{
  if(edit_user->addtoLog()) {
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
  else {
    fprintf(stderr,"addchain: insufficient privileges [Add Log Items]\n");
  }
}


void MainObject::Addmarker(int line)
{
  if(edit_user->addtoLog()) {
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
  else {
    fprintf(stderr,"addmarker: insufficient privileges [Add Log Items]\n");
  }
}


void MainObject::Addtrack(int line)
{
  if(edit_user->addtoLog()) {
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
  else {
    fprintf(stderr,"addtrack: insufficient privileges [Add Log Items]\n");
  }
}


void MainObject::ListLogs() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select NAME from LOGS order by NAME");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    Print(QString().sprintf("%s\n",(const char *)q->value(0).toString()));
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
    Print(QString().sprintf("%4d %s\n",i,
			    (const char *)ListLine(edit_log_event,i)));
  }
  fflush(stdout);
}


void MainObject::Remove(int line)
{
  edit_log_event->remove(line,1);
  edit_modified=true;
}


void MainObject::Save()
{
  if(edit_user->arrangeLog()) {
    edit_log_event->save();
    edit_log->
      setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    edit_modified=false;
  }
  else {
    fprintf(stderr,"save: insufficient privileges [Rearrange Log Items]\n");
  }
}


void MainObject::Saveas(const QString &logname)
{
  QString sql;
  RDSqlQuery *q;

  if(edit_user->arrangeLog()) {
    RDLog *log=new RDLog(logname);
    if(!log->exists()) {
      sql=QString("insert into LOGS set ")+
	"NAME=\""+RDEscapeString(logname)+"\","+
	"TYPE=0,"+
	"DESCRIPTION=\""+"Copy of "+RDEscapeString(edit_log->name())+"\","+
	"ORIGIN_USER=\""+RDEscapeString(edit_user->name())+"\","+
	"ORIGIN_DATETIME=now(),"+
	"LINK_DATETIME=now(),"+
	"MODIFIED_DATETIME=now(),"+
	"SERVICE=\""+edit_log->service()+"\"";
      q=new RDSqlQuery(sql);
      delete q;
      RDCreateLogTable(RDLog::tableName(logname));
      edit_log_event->setLogName(RDLog::tableName(logname));
      edit_log_event->save();
      delete edit_log;
      edit_log=log;
      edit_modified=false;
    }
    else {
      fprintf(stderr,"saveas: log already exists\n");
      delete log;
    }
  }
  else {
    fprintf(stderr,"saveas: insufficient privileges [Rearrange Log Items]\n");
  }
}


void MainObject::Setcart(int line,unsigned cartnum)
{
  if(edit_user->arrangeLog()) {
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
  else {
    fprintf(stderr,"setcart: insufficient privileges [Rearrange Log Items]\n");
  }
}


void MainObject::Setcomment(int line,const QString &str)
{
  if(edit_user->arrangeLog()) {
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
  else {
    fprintf(stderr,"setcomment: insufficient privileges [Rearrange Log Items]\n");
  }  
}


void MainObject::Setlabel(int line,const QString &str)
{
  if(edit_user->arrangeLog()) {
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
  else {
    fprintf(stderr,"setlabel: insufficient privileges [Rearrange Log Items]\n");
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
