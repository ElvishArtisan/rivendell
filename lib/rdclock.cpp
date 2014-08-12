// rdclock.cpp
//
// Abstract a Rivendell Log Manager Clock.
//
//   (C) Copyright 2002-2004,2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdclock.cpp,v 1.23.10.1 2014/06/24 18:27:03 cvs Exp $
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
#include <rddb.h>
#include <rdclock.h>
#include <rdevent_line.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDClock::RDClock()
{
  clear();
}


QString RDClock::name() const
{
  return clock_name;
}


void RDClock::setName(const QString &name)
{
  clock_name=name;
  clock_name_esc=name;
  clock_name_esc.replace(" ","_");
}


QString RDClock::shortName() const
{
  return clock_short_name;
}


void RDClock::setShortName(const QString &name)
{
  clock_short_name=name;
}


QColor RDClock::color() const
{
  return clock_color;
}


void RDClock::setColor(const QColor &color)
{
  clock_color=color;
}


QString RDClock::remarks() const
{
  return clock_remarks;
}


void RDClock::setRemarks(const QString &str)
{
  clock_remarks=str;
}


unsigned RDClock::getArtistSep()
{
  return artistsep;
}

void RDClock::setArtistSep(unsigned artist_sep)
{
  artistsep=artist_sep;
}

bool RDClock::getRulesModified()
{
  return rules_modified;
}


void RDClock::setRulesModified(bool modified)
{
  rules_modified=modified;
}


void RDClock::clear()
{
   clock_name="";
   clock_short_name="";
   clock_color=QColor();
   clock_remarks="";
   clock_events.clear();
   artistsep=15;
   rules_modified=false;
}


RDEventLine *RDClock::eventLine(int line)
{
  if((line<0)||((unsigned)line>=clock_events.size())) {
    return NULL;
  }
  return &clock_events[line];
}


int RDClock::size() const
{
  return clock_events.size();
}


bool RDClock::load()
{
  QString sql=QString().sprintf("select SHORT_NAME,COLOR,ARTISTSEP,REMARKS from\
                                 CLOCKS where NAME=\"%s\"",
				(const char *)clock_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  clock_short_name=q->value(0).toString();
  if(q->value(1).isNull()) {
    clock_color=QColor();
  }
  else {
    clock_color=QColor(q->value(1).toString());
  }
  artistsep=q->value(2).toUInt();
  clock_remarks=q->value(3).toString();
  delete q;

  sql=QString().sprintf("select EVENT_NAME,START_TIME,LENGTH from %s_CLK\
                         order by ID",
			(const char *)clock_name_esc);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    clock_events.push_back(RDEventLine());
    clock_events.back().setName(q->value(0).toString());
    clock_events.back().setStartTime(QTime().addMSecs(q->value(1).toInt()));
    clock_events.back().setLength(q->value(2).toInt());
    clock_events.back().load();
  }
  delete q;
  return true;
}


bool RDClock::save()
{
  if(clock_short_name.isEmpty()) {
    clock_short_name=clock_name.left(3);
  }
  QString sql=QString().sprintf("select NAME from CLOCKS where NAME=\"%s\"",
				(const char *)clock_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    delete q;
    sql=QString().sprintf("update CLOCKS set SHORT_NAME=\"%s\",COLOR=\"%s\",\
                           ARTISTSEP=%d,REMARKS=\"%s\" where NAME=\"%s\"",
			  (const char *)clock_short_name,
			  (const char *)clock_color.name(),
                          artistsep,
			  (const char *)RDEscapeString(clock_remarks),
			  (const char *)clock_name);
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString().sprintf("delete from %s_CLK",(const char *)clock_name_esc);
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    delete q;
    sql=QString().sprintf("insert into CLOCKS set NAME=\"%s\",\
                           SHORT_NAME=\"%s\",COLOR=\"%s\",ARTISTSEP=%d,\
                           REMARKS=\"%s\"",
			  (const char *)clock_name,
			  (const char *)clock_short_name,
			  (const char *)clock_color.name(),
                          artistsep,
			  (const char *)RDEscapeString(clock_remarks));
    
    q=new RDSqlQuery(sql);
    delete q;
  }
  sql=QString().sprintf("delete from %s_CLK",
			(const char *)clock_name_esc);
  q=new RDSqlQuery(sql);
  delete q;
  for(unsigned i=0;i<clock_events.size();i++) {
    sql=QString().sprintf("insert into %s_CLK set EVENT_NAME=\"%s\",\
                           START_TIME=%d,LENGTH=%d",
			  (const char *)clock_name_esc,
			  (const char *)clock_events[i].name(),
			  QTime().msecsTo(clock_events[i].startTime()),
			  clock_events[i].length());
    q=new RDSqlQuery(sql);
    delete q;
  }
  return true;
}


bool RDClock::insert(const QString &event_name,int line)
{
  QString sql=QString().sprintf("select NAME from EVENTS where NAME=\"%s\"",
				(const char *)event_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  delete q;
  if(line>=size()) {
    clock_events.push_back(RDEventLine());
  }
  else {
    vector<RDEventLine>::iterator it=clock_events.begin()+line;
    clock_events.insert(it,1,RDEventLine());
  }
  clock_events[line].setName(event_name);
  clock_events[line].load();
  return true;
}


void RDClock::remove(int line)
{
  vector<RDEventLine>::iterator it=clock_events.begin()+line;
  clock_events.erase(it,it+1);
}


void RDClock::move(int from_line,int to_line)
{
  int src_offset=0;
  int dest_offset=1;
  RDEventLine *srcline;
  RDEventLine *destline;

  if(to_line<from_line) {
    src_offset=1;
    dest_offset=0;
  }
  insert(clock_events[from_line].name(),to_line+dest_offset);
  if((to_line+1)>=size()) {
    to_line=clock_events.size()-1;
    dest_offset=0;
  }

  if(((destline=eventLine(to_line+dest_offset))==NULL)||
     (srcline=eventLine(from_line+src_offset))==NULL) {
    remove(to_line+dest_offset);
    return;
  }
  *destline=*srcline;
  remove(from_line+src_offset);
}


bool RDClock::validate(const QTime &start_time,int length,int except_line)
{
  QTime end_time=start_time.addMSecs(length);
  QTime end;
  for(unsigned i=0;i<clock_events.size();i++) {
    if(i!=(unsigned)except_line) {
      end=clock_events[i].startTime().addMSecs(clock_events[i].length());
      if((start_time>=clock_events[i].startTime())&&(start_time<end)) {
	return false;
      }
      if(((end_time>clock_events[i].startTime())&&
	  (end_time<end))||
	 ((start_time<end)&&(end_time>end))) {
	return false;
      }
    }
  }   
  return true;
}


bool RDClock::generateLog(int hour,const QString &logname,
			  const QString &svc_name,QString *errors)
{
  QString sql;
  RDSqlQuery *q;
  RDEventLine eventline;

  sql=QString().sprintf("select EVENT_NAME,START_TIME,LENGTH from %s_CLK\
                         order by START_TIME",
			(const char *)clock_name_esc);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    eventline.setName(q->value(0).toString());
    eventline.load();
    //    eventline.setStartTime(q->value(1).toTime().addSecs(3600*hour));
    eventline.setStartTime(QTime().addMSecs(q->value(1).toInt()).
			   addSecs(3600*hour));
    eventline.setLength(q->value(2).toInt());
    eventline.generateLog(logname,svc_name,errors,artistsep,clock_name_esc);
    eventline.clear();
  }
  delete q;
  return true;
}


QString RDClock::tableName(const QString &name)
{
  QString ret=name;
  ret.replace(" ","_");

  return ret+"_CLK";
}
