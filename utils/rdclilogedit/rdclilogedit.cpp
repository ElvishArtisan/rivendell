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

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qapplication.h>
#include <qcstring.h>
#include <qfile.h>
#include <qstringlist.h>

#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rdcreate_log.h>
#include <rdescape_string.h>
#include <rdweb.h>

#include "rdclilogedit.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  edit_log=NULL;
  edit_log_event=NULL;
  edit_modified=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdimport",RDCLILOGEDIT_USAGE);
  for(int i=0;i<(int)cmd->keys()-1;i++) {
  }

  //
  // Read Configuration
  //
  edit_config=new RDConfig();
  edit_config->load();

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(edit_config->mysqlDriver());
  if(!db) {
    fprintf(stderr,"rdclilogedit: unable to initialize connection to database\n");
    exit(256);
  }
  db->setDatabaseName(edit_config->mysqlDbname());
  db->setUserName(edit_config->mysqlUsername());
  db->setPassword(edit_config->mysqlPassword());
  db->setHostName(edit_config->mysqlHostname());
  if(!db->open()) {
    fprintf(stderr,"rdclilogedit: unable to connect to database\n");
    db->removeDatabase(edit_config->mysqlDbname());
    exit(256);
  }

  //
  // RDAirPlay Configuration
  //
  edit_airplay_conf=new RDAirPlayConf(edit_config->stationName(),"RDAIRPLAY");

  //
  // RIPC Connection
  //
  edit_user=NULL;
  edit_input_notifier=NULL;
  edit_ripc=new RDRipc(edit_config->stationName());
  connect(edit_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  edit_ripc->
    connectHost("localhost",RIPCD_TCP_PORT,edit_config->password());
}


void MainObject::userData()
{
  //
  // Get User Context
  //
  disconnect(edit_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  if(edit_user!=NULL) {
    delete edit_user;
  }
  edit_user=new RDUser(edit_ripc->user());

  //
  // Start up command processor
  //
  if(edit_input_notifier==NULL) {
    int flags=fcntl(0,F_GETFL,NULL);
    flags|=O_NONBLOCK;
    fcntl(0,F_SETFL,flags);
    edit_input_notifier=new QSocketNotifier(0,QSocketNotifier::Read,this);
    connect(edit_input_notifier,SIGNAL(activated(int)),
	    this,SLOT(inputActivatedData(int)));
    PrintPrompt();
  }
}


void MainObject::inputActivatedData(int sock)
{
  char data[1024];
  int n;

  while((n=read(sock,data,1024))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case 10:
	DispatchCommand(edit_accum);
	edit_accum="";
	break;

      case 13:
	break;

      default:
	edit_accum+=data[i];
      }
    }
  }
}


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


void MainObject::OverwriteError(const QString &cmd) const
{
  fprintf(stderr,"%s: buffer not saved (append \"!\" to override)\n",
	  (const char *)cmd);
}


void MainObject::Print(const QString &str) const
{
  printf("%s",(const char *)str);
  usleep(100);
}


void MainObject::DispatchCommand(QString cmd)
{ 
  bool processed=false;
  int line;
  QTime time;
  bool ok=false;
  bool overwrite=!edit_modified;
  QStringList cmds;
  QString verb;

  cmd=cmd.stripWhiteSpace();
  if(cmd.right(1)=="!") {
    overwrite=true;
    cmd=cmd.left(cmd.length()-1).stripWhiteSpace();
  }
  cmds=cmds.split(" ",cmd);
  verb=cmds[0].lower();

  //
  // No loaded log needed for these
  //
  if((verb=="exit")||(verb=="quit")||(verb=="bye")) {
    exit(0);
  }

  if((verb=="help")||(verb=="?")) {
    Help(cmds);
    processed=true;
  }

  if(verb=="listlogs") {
    ListLogs();
    processed=true;
  }

  if(verb=="load") {
    if(overwrite) {
      if(cmds.size()==2) {
	Load(cmds[1]);
      }
      else {
	fprintf(stderr,"load: invalid command arguments\n");
      }
    }
    else {
      OverwriteError("load");
    }
    processed=true;
  }

  //
  // These need a log loaded
  //
  if((processed)||(edit_log_event!=NULL)) {
    if(verb=="addcart") {
      if(cmds.size()==3) {
	line=cmds[1].toInt(&ok);
	if(ok&&(line>=0)) {
	  unsigned cartnum=cmds[2].toUInt(&ok);
	  if(ok&&(cartnum<=RD_MAX_CART_NUMBER)) {
	    Addcart(line,cartnum);
	  }
	  else {
	    fprintf(stderr,"addcart: invalid cart number\n");
	  }
	}
	else {
	  fprintf(stderr,"addcart: invalid line number\n");
	}
      }
      else {
	fprintf(stderr,"addcart: invalid command arguments\n");
      }
      processed=true;
    }
    
    if(verb=="list") {
      List();
      processed=true;
    }

    if(verb=="remove") {
      if(cmds.size()==2) {
	line=cmds[1].toInt(&ok);
	if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	  Remove(line);
	}
	else {
	  fprintf(stderr,"remove: invalid line number\n");
	}
      }
      else {
	fprintf(stderr,"remove: invalid command arguments\n");
      }
      processed=true;
    }

    if(verb=="save") {
      Save();
      processed=true;
    }

    if(verb=="saveas") {
      if(cmds.size()==2) {
	if(cmds[1].length()>64) {
	  fprintf(stderr,"saveas: log name too long\n");
	}
	Saveas(cmds[1]);
      }
      else {
	fprintf(stderr,"saveas: invalid command arguments\n");
      }
      processed=true;
    }

    if(verb=="setcart") {
      if(cmds.size()==3) {
	line=cmds[1].toInt(&ok);
	if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	  unsigned cartnum=cmds[2].toUInt(&ok);
	  if(ok&&(cartnum<=RD_MAX_CART_NUMBER)) {
	    Setcart(line,cartnum);
	  }
	  else {
	    fprintf(stderr,"setcart: invalid cart number\n");
	  }
	}
	else {
	  fprintf(stderr,"setcart: invalid line number\n");
	}
      }
      else {
	fprintf(stderr,"setcart: invalid command arguments\n");
      }
      processed=true;
    }

    if(verb=="settime") {
      if(cmds.size()>=3) {
	line=cmds[1].toInt(&ok);
	if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	  RDLogLine::TimeType ttype=RDLogLine::NoTime;
	  if(cmds[2].lower()=="hard") {
	    ttype=RDLogLine::Hard;
	  }
	  if(cmds[2].lower()=="none") {
	    ttype=RDLogLine::Relative;
	  }
	  switch(ttype) {
	  case RDLogLine::Hard:
	    if(cmds.size()>=4) {
	      time=RDGetWebTime(cmds[3],&ok);
	      if(ok) {
		Settime(line,ttype,time);
	      }
	      else {
		fprintf(stderr,"settime: invalid time value\n");
	      }
	    }
	    else {
	      fprintf(stderr,"settime: missing time value\n");
	    }
	    break;

	  case RDLogLine::Relative:
	    Settime(line,ttype);
	    break;

	  case RDLogLine::NoTime:
	    fprintf(stderr,"settime: invalid time type\n");
	    break;
	  }
	}
	else {
	  fprintf(stderr,"settime: invalid line number\n");
	}
      }
      else {
	fprintf(stderr,"settime: invalid command arguments\n");
      }
      processed=true;
    }

    if(verb=="settrans") {
      if(cmds.size()==3) {
	line=cmds[1].toInt(&ok);
	if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	  RDLogLine::TransType trans=RDLogLine::NoTrans;
	  if(cmds[2].lower()=="play") {
	    trans=RDLogLine::Play;
	  }
	  if(cmds[2].lower()=="segue") {
	    trans=RDLogLine::Segue;
	  }
	  if(cmds[2].lower()=="stop") {
	    trans=RDLogLine::Stop;
	  }
	  if(trans!=RDLogLine::NoTrans) {
	    Settrans(line,trans);
	  }
	  else {
	    fprintf(stderr,"settrans: invalid transition type\n");
	  }
	}
	else {
	  fprintf(stderr,"settrans: invalid line number\n");
	}
      }
      else {
	fprintf(stderr,"settrans: invalid command arguments\n");
      }
      processed=true;
    }

    if(verb=="unload") {
      if(overwrite) {
	Unload();
      }
      else {
	OverwriteError("unload");
      }
      processed=true;
    }
  }
  else {
    fprintf(stderr,"%s: no log loaded\n",(const char *)verb);
    processed=true;
  }

  if(!processed) {
    fprintf(stderr,"invalid command\n");
  }
  PrintPrompt();
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


void MainObject::PrintPrompt() const
{
  if(edit_log==NULL) {
    Print("logedit> ");
  }
  else {
    if(edit_modified) {
      Print(QString().sprintf("logedit[%s*]> ",
			      (const char *)edit_log->name()));
    }
    else {
      Print(QString().sprintf("logedit[%s]> ",(const char *)edit_log->name()));
    }
  }
  fflush(stdout);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
