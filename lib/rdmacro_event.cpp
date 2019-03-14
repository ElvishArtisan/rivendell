// rdmacro_event.cpp
//
// A container class for a list of RML macros.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qstringlist.h>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdmacro_event.h>
#include <rdstation.h>

RDMacroEvent::RDMacroEvent(RDRipc *ripc,QObject *parent,const char *name)
  : QObject(parent,name)
{
  QHostAddress addr;
  addr.setAddress("127.0.0.1");
  event_ripc=ripc;
  event_address=addr;
  event_whole_list=false;
  event_line=-1;

  event_sleep_timer=new QTimer(this,"event_sleep_timer");
  connect(event_sleep_timer,SIGNAL(timeout()),this,SLOT(sleepTimerData()));
}


RDMacroEvent::RDMacroEvent(QHostAddress addr,RDRipc *ripc,
			   QObject *parent,const char *name)
  : QObject(parent,name)
{
  event_ripc=ripc;
  event_address=addr;
  event_whole_list=false;
  event_line=-1;

  event_sleep_timer=new QTimer(this,"event_sleep_timer");
  connect(event_sleep_timer,SIGNAL(timeout()),this,SLOT(sleepTimerData()));
}


RDMacroEvent::~RDMacroEvent()
{
  for(unsigned i=0;i<event_cmds.size();i++) {
    delete event_cmds[i];
  }
}


int RDMacroEvent::line() const
{
  return event_line;
}


void RDMacroEvent::setLine(int line)
{
  event_line=line;
}


QTime RDMacroEvent::startTime() const
{
  return event_start_time;
}


void RDMacroEvent::setStartTime(QTime time)
{
  event_start_time=time;
}


RDMacro *RDMacroEvent::command(int line)
{
  return event_cmds.at(line);
}


int RDMacroEvent::size() const
{
 return event_cmds.size();
}


unsigned RDMacroEvent::length() const
{
  unsigned length=0;
  for(unsigned i=0;i<event_cmds.size();i++) {
    length+=event_cmds[i]->length();
  }
  return length;
}


bool RDMacroEvent::load(const QString &str)
{
  RDMacro cmd;
  QString rmlstr="";

  for(int i=0;i<str.length();i++) {
    QChar c=str.at(i);
    rmlstr+=c;
    if(c=='!') {
      cmd=RDMacro::fromString(rmlstr,RDMacro::Cmd);
      if(cmd.isNull()) {
	clear();
	return false;
      }
      cmd.setAddress(event_address);
      cmd.setEchoRequested(false);
      event_cmds.push_back(new RDMacro(cmd));
      rmlstr="";
    }
  }
  return true;
}


bool RDMacroEvent::load(unsigned cartnum)
{
  QString sql=QString().
    sprintf("select MACROS from CART where (NUMBER=%d)&&(TYPE=2)",cartnum);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    clear();
    return false;
  }
  bool ret=load(q->value(0).toString());
  delete q;
  return ret;
}


QString RDMacroEvent::save()
{
  QString str="";

  for(unsigned i=0;i<event_cmds.size();i++) {
    str+=event_cmds[i]->toString();
  }
  return str;
}


void RDMacroEvent::insert(int line,const RDMacro *cmd)
{
  std::vector<RDMacro *>::iterator it=event_cmds.begin()+line;

  event_cmds.insert(it,1,new RDMacro(*cmd));
}


void RDMacroEvent::remove(int line)
{
  std::vector<RDMacro *>::iterator it=event_cmds.begin()+line;

  delete event_cmds[line];
  event_cmds.erase(it,it+1);
}


void RDMacroEvent::move(int from_line,int to_line)
{
  int src_offset=0;

  if(to_line<from_line) {
    src_offset=1;
  }
  insert(to_line,command(from_line));
  remove(from_line+src_offset);
}


void RDMacroEvent::copy(int from_line,int to_line)
{
  insert(to_line,command(from_line));
}


void RDMacroEvent::clear()
{
  event_cmds.clear();
  event_line=-1;
  event_start_time=QTime();
}


void RDMacroEvent::exec()
{
  if(event_ripc==NULL) {
    return;
  }
  ExecList(0);
}


void RDMacroEvent::exec(int line)
{
  QString sql;
  RDSqlQuery *q;
  QString stationname;
  QStringList args;
  Q_UINT16 port=0;

  if(event_ripc==NULL) {
    return;
  }
  RDMacro rml;
  RDStation *station;
  QHostAddress addr;
  RDMacro::Command cmd;
  emit started(line);
  switch(event_cmds[line]->command()) {
  case RDMacro::SP:   // Sleep
    event_sleeping_line=line;
    event_sleep_timer->start(event_cmds[line]->arg(0).toInt(),true);
    break;

  case RDMacro::CC:   // Send Command
    args=args.split(":",event_cmds[line]->arg(0));
    stationname=args[0];
    if(args.size()==2) {
      port=args[1].toUInt();
    }
    if(stationname.lower()=="localhost") {
      addr.setAddress("127.0.0.2");
      rml.setAddress(addr);
    }
    else {
      sql=QString("select VARVALUE from HOSTVARS where ")+
	"(STATION_NAME=\""+RDEscapeString(event_ripc->station())+"\")&&"+
	"(NAME=\""+RDEscapeString(stationname)+"\")";
      q=new RDSqlQuery(sql);
      if(q->first()) {
	stationname=q->value(0).toString();
      }
      delete q;
      station=new RDStation(stationname);
      if(station->exists()) {
	rml.setAddress(station->address());
      }
      else {
	addr.setAddress(stationname);
	if(addr.isNull()) {
	  emit finished(line);
	  delete station;
	  return;
	}
	rml.setAddress(addr);
      }
      delete station;
    }
    rml.setCommand(event_cmds[line]->arg(1));
    for(int i=2;i<event_cmds[line]->argQuantity();i++) {
      rml.addArg(event_cmds[line]->arg(i));
    }
    rml.setRole(RDMacro::Cmd);
    rml.setPort(port);
    rml.setEchoRequested(event_cmds[line]->echoRequested());
    event_ripc->sendRml(&rml);
    emit finished(line);
    break;

  default:
    event_ripc->sendRml(event_cmds[line]);
    emit finished(line);
    break;
  }
}


void RDMacroEvent::stop()
{
  //
  // This will work only for 'Sleep' [SP] macros -- all others are
  // assumed to execute 'instaneously', and hence trying to 'stop'
  // them would make no sense.
  //
  if(event_sleep_timer->isActive()) {
    event_sleep_timer->stop();
    emit stopped();
  }
}


void RDMacroEvent::sleepTimerData()
{
  emit finished(event_sleeping_line);
  if(event_whole_list) {
    ExecList(event_sleeping_line+1);
  }
}


void RDMacroEvent::ExecList(int line)
{
  if(line==0) {
    event_whole_list=true;
    emit started();
  }
  for(unsigned i=line;i<event_cmds.size();i++) {
    switch(event_cmds[i]->command()) {
    case RDMacro::SP:  // Sleep
      exec(i);
      return;

    default:
      exec(i);
      break;
    }
  }
  event_whole_list=false;
  emit finished();
}
