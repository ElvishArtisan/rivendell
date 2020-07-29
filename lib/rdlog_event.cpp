// rdlog_event.cpp
//
// Abstract Rivendell Log Events.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <map>

#include "rd.h"
#include "rdapplication.h"
#include "rdconf.h"
#include "rddb.h"
#include "rddebug.h"
#include "rdescape_string.h"
#include "rdlog.h"
#include "rdlog_event.h"
#include "rdreport.h"

//
// Global Classes
//
RDLogEvent::RDLogEvent(QString name)
{
  log_name=name;
  log_max_id=0;
}


RDLogEvent::~RDLogEvent()
{
  for(unsigned i=0;i<log_line.size();i++) {
    delete log_line[i];
  }
}


bool RDLogEvent::exists()
{
  return RDLog::exists(log_name);
}


bool RDLogEvent::exists(int line)
{
  if((int)log_line.size()>line) {
    return true;
  }
  return false;
}


bool RDLogEvent::exists(const QTime &hard_time,int except_line)
{
  for(int i=0;i<size();i++) {
    if((logLine(i)->timeType()==RDLogLine::Hard)&&
       (logLine(i)->startTime(RDLogLine::Logged)==hard_time)&&
       (i!=except_line)) {
      return true;
    }
  }
  return false;
}


QString RDLogEvent::logName() const
{
  return log_name;
}


void RDLogEvent::setLogName(QString logname)
{
  RDLog *log=new RDLog(logname);
  log_name=log->name();  // So we normalize the case
  delete log;
}


QString RDLogEvent::serviceName() const
{
  return log_service_name;
}


int RDLogEvent::load(bool track_ptrs)
{
  RDLogLine line;
  QString sql;
  RDSqlQuery *q;

  //
  // Get the service name
  //
  sql=QString("select SERVICE from LOGS where ")+
    "NAME=\""+RDEscapeString(log_name)+"\"";
  q=new RDSqlQuery(sql);
  if(q->next()) {
    log_service_name=q->value(0).toString();
  }
  delete q;

  RDLog *log=new RDLog(log_name);
  log_max_id=log->nextId();
  delete log;

  LoadLines(log_name,0,track_ptrs);

  return log_line.size();
}

void RDLogEvent::saveModified(RDConfig *config,bool update_tracks)
{
  for(unsigned i=0;i<log_line.size();i++) {
    if(log_line[i]->hasBeenModified()) {
      save(config,update_tracks, i);
    }
  }
}

void RDLogEvent::save(RDConfig *config,bool update_tracks,int line)
{
  QString sql;
  RDSqlQuery *q;

  if(log_name.isEmpty()) {
    return;
  }
  if(line<0) {
    if(exists()) {
      sql=QString("delete from LOG_LINES where ")+
	"LOG_NAME=\""+RDEscapeString(log_name)+"\"";
      RDSqlQuery::apply(sql);
    }
    if (log_line.size() > 0) {
       QString values = "";
       for(unsigned i=0;i<log_line.size();i++) {
         InsertLineValues(&values, i);
        if (i<log_line.size()-1) {
           values += ",";
         }
       }
       InsertLines(values);
    }
  }
  else {
    sql=QString("delete from LOG_LINES where ")+
      "LOG_NAME=\""+RDEscapeString(log_name)+"\" && "+
      QString().sprintf("COUNT=%d",line);
    q=new RDSqlQuery(sql);
    delete q;
    SaveLine(line);
    // BPM - Clear the modified flag
    log_line[line]->clearModified();
  }
  RDLog *log=new RDLog(log_name);
  if(log->nextId()<nextId()) {
    log->setNextId(nextId());
  }
  if(update_tracks) {
    log->updateTracks();
  }
  delete log;
}


int RDLogEvent::append(const QString &logname,bool track_ptrs)
{
  return LoadLines(logname,log_max_id,track_ptrs);
}


void RDLogEvent::clear()
{
  log_name="";
  log_line.resize(0);
  log_max_id=0;
}


int RDLogEvent::validate(QString *report,const QDate &date)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  int errs=0;

  //
  // Report Header
  //
  *report="Rivendell Log Exception Report\n";
  *report+=QString("Generated at: ")+
    QDate::currentDate().toString("MM/dd/yyyy")+" - "+
    QTime::currentTime().toString("hh:mm:ss")+"\n";
  *report+=QString("Log: ")+log_name+"\n";
  *report+=QString("Effective Airdate: ")+date.toString("MM/dd/yyyy")+"\n";
  *report+="\n";

  //
  // Line Scan
  //
  for(int i=0;i<size();i++) {
    if(logLine(i)->cartNumber()>0) {
      sql=QString("select ")+
	"TYPE,"+   // 00
	"TITLE "+  // 01
	"from CART where "+
	QString().sprintf("NUMBER=%d",logLine(i)->cartNumber());
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	*report+=QString(" ")+
	  logLine(i)->startTime(RDLogLine::Logged).toString("hh:mm:ss")+
	  QString().sprintf(" - missing cart %06d",logLine(i)->cartNumber())+
	  "\n";
	errs++;
      }
      else {
	if((RDCart::Type)q->value(0).toInt()==RDCart::Audio) {
	  if(logLine(i)->startTime(RDLogLine::Logged).isNull()) {
	    //
	    // Handle events with no logged start time (e.g. manual inserts)
	    //
		//TODO do we need to verify date here?
	    sql=QString("select CUT_NAME from CUTS where ")+
	      QString().sprintf("(CART_NUMBER=%u)&&",logLine(i)->cartNumber())+
	      "((START_DATETIME is null)||"+
	      "(START_DATETIME<=\""+date.toString("yyyy-MM-dd")+" 23:59:59\"))&&"+
	      "((END_DATETIME is null)||"+
	      "(END_DATETIME>=\""+date.toString("yyyy-MM-dd")+" 00:00:00\"))&&"+
	      "("+RDDowCode(date.dayOfWeek())+"=\"Y\")&&(LENGTH>0)";
	  }
	  else {
		//TODO Do we need to verify date and logLine(i)->startTime?
	    sql=QString("select CUT_NAME from CUTS where ")+
	      QString().sprintf("(CART_NUMBER=%u)&&",logLine(i)->cartNumber())+
	      "((START_DATETIME is null)||"+
	      "(START_DATETIME<=\""+date.toString("yyyy-MM-dd")+" "+
	      logLine(i)->startTime(RDLogLine::Logged).toString("hh:mm:ss")+
	      "\"))&&"+
	      "((END_DATETIME is null)||"+
	      "(END_DATETIME>=\""+date.toString("yyyy-MM-dd")+" "+
	      logLine(i)->startTime(RDLogLine::Logged).toString("hh:mm:ss")+
	      "\"))&&"+
	      "((START_DAYPART is null)||"+
	      "(START_DAYPART<=\""+
	      logLine(i)->startTime(RDLogLine::Logged).
	      toString("hh:mm:ss")+"\"))&&"+
	      "((END_DAYPART is null)||"+
	      "(END_DAYPART>=\""+logLine(i)->startTime(RDLogLine::Logged).
	      toString("hh:mm:ss")+"\"))&&"+
	      "("+RDDowCode(date.dayOfWeek())+"=\"Y\")&&(LENGTH>0)";
	  }
	  q1=new RDSqlQuery(sql);
	  if(!q1->first()) {
	    *report+=QString(" ")+
	      logLine(i)->startTime(RDLogLine::Logged).toString("hh:mm:ss")+
	      QString().sprintf(" - cart %06d [",logLine(i)->cartNumber())+
	      q->value(1).toString()+"] "+QObject::tr("is not playable")+"\n";
	    errs++;
	  }
	  delete q1;
	}
      }
      delete q;
    }
  }
  *report+="\n";
  if(errs==1) {
    *report+=QString().sprintf("%d validation exception found.\n\n",errs);
  }
  else {
    *report+=QString().sprintf("%d validation exceptions found.\n\n",errs);
  }
  return errs;
}


void RDLogEvent::refresh(int line)
{
  if(log_name.isEmpty()||log_line[line]->cartNumber()==0) {
    return;
  }
  QString sql=QString("select ")+
    "CART.TYPE,"+                  // 00
    "CART.GROUP_NAME,"+            // 01
    "CART.TITLE,"+                 // 02
    "CART.ARTIST,"+                // 03
    "CART.ALBUM,"+                 // 04
    "CART.YEAR,"+                  // 05
    "CART.LABEL,"+                 // 06
    "CART.CLIENT,"+                // 07
    "CART.AGENCY,"+                // 08
    "CART.USER_DEFINED,"+          // 09
    "CART.FORCED_LENGTH,"+         // 10
    "CART.CUT_QUANTITY,"+          // 11
    "CART.LAST_CUT_PLAYED,"+       // 12
    "CART.PLAY_ORDER,"+            // 13
    "CART.ENFORCE_LENGTH,"+        // 14
    "CART.PRESERVE_PITCH,"+        // 15
    "CART.PUBLISHER,"+             // 16
    "CART.COMPOSER,"+              // 17
    "CART.USAGE_CODE,"+            // 18
    "CART.AVERAGE_SEGUE_LENGTH,"+  // 19
    "CART.VALIDITY,"+              // 20
    "CART.NOTES,"+                 // 21
    "GROUPS.COLOR "+               // 22
    "from CART left join GROUPS "+
    "on CART.GROUP_NAME=GROUPS.NAME where "+
    QString().sprintf("CART.NUMBER=%u",log_line[line]->cartNumber());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    switch((RDCart::Type)q->value(0).toInt()) {
    case RDCart::Audio:
      log_line[line]->setType(RDLogLine::Cart);
      break;
	  
    case RDCart::Macro:
      log_line[line]->setType(RDLogLine::Macro);
      break;
	  
    default:
      break;
    }	
    log_line[line]->
      setCartType((RDCart::Type)q->value(0).toInt());        // Cart Type
    log_line[line]->setGroupName(q->value(1).toString());       // Group Name
    log_line[line]->setTitle(q->value(2).toString());           // Title
    log_line[line]->setArtist(q->value(3).toString());          // Artist
    log_line[line]->setPublisher(q->value(16).toString());      // Publisher
    log_line[line]->setComposer(q->value(17).toString());       // Composer
    log_line[line]->setAlbum(q->value(4).toString());           // Album
    log_line[line]->setYear(q->value(5).toDate());              // Year
    log_line[line]->setLabel(q->value(6).toString());           // Label
    log_line[line]->setClient(q->value(7).toString());          // Client
    log_line[line]->setAgency(q->value(8).toString());          // Agency
    log_line[line]->setUserDefined(q->value(9).toString());     // User Defined
    log_line[line]->setUsageCode((RDCart::UsageCode)q->value(16).toInt());
    log_line[line]->setForcedLength(q->value(10).toUInt());   // Forced Length
    log_line[line]->setAverageSegueLength(q->value(19).toUInt());
    log_line[line]->setCutQuantity(q->value(11).toUInt());       // Cut Quantity
    log_line[line]->setLastCutPlayed(q->value(12).toUInt());  // Last Cut Played
    log_line[line]->
      setPlayOrder((RDCart::PlayOrder)q->value(13).toUInt()); // Play Order
    log_line[line]->
      setEnforceLength(RDBool(q->value(14).toString()));     // Enforce Length
    log_line[line]->
      setPreservePitch(RDBool(q->value(15).toString()));     // Preserve Pitch
    log_line[line]->setValidity((RDCart::Validity)q->value(20).toInt());
    log_line[line]->setCartNotes(q->value(21).toString());   // Cart Notes
    log_line[line]->setGroupColor(q->value(22).toString());  // Group Color
  }
  else {
    log_line[line]->setValidity(RDCart::NeverValid);
  }
  delete q;
}


int RDLogEvent::size() const
{
  return log_line.size();
}


void RDLogEvent::insert(int line,int num_lines,bool preserve_trans)
{
  if(!preserve_trans) {
    if((line>0)&&(log_line[line-1]!=NULL)) {
      log_line[line-1]->setEndPoint(-1,RDLogLine::LogPointer);
      log_line[line-1]->setSegueStartPoint(-1,RDLogLine::LogPointer);
      log_line[line-1]->setSegueEndPoint(-1,RDLogLine::LogPointer);
    }
    if(line<(size()-1)) {
      log_line[line]->setStartPoint(-1,RDLogLine::LogPointer);
      log_line[line]->setHasCustomTransition(false);
    }
  }
  if(line<size()) {
    for(int i=0;i<num_lines;i++) {
      log_line.insert(log_line.begin()+line+i,1,new RDLogLine());
      log_line[line+i]->setId(++log_max_id);
    }
    return;
  }
  if(line>=size()) {
    for(int i=0;i<num_lines;i++) {
      log_line.push_back(new RDLogLine());
      log_line.back()->setId(++log_max_id);
    }
    return;
  }
}


void RDLogEvent::remove(int line,int num_lines,bool preserve_trans)
{
  if(!preserve_trans) {
  if(line>0) {
    log_line[line-1]->setEndPoint(-1,RDLogLine::LogPointer);
    log_line[line-1]->setSegueStartPoint(-1,RDLogLine::LogPointer);
    log_line[line-1]->setSegueEndPoint(-1,RDLogLine::LogPointer);
  }
  if(line<((int)log_line.size()-num_lines)) {
    log_line[line+num_lines]->setStartPoint(-1,RDLogLine::LogPointer);
    log_line[line+num_lines]->setHasCustomTransition(false);
  }
  }  
  for(int i=line;i<(line+num_lines);i++) {
    delete log_line[i];
  }
  std::vector<RDLogLine *>::iterator it=log_line.begin()+line;
  log_line.erase(it,it+num_lines);
}


void RDLogEvent::move(int from_line,int to_line)
{
  int src_offset=0;
  int dest_offset=1;
  RDLogLine *srcline;
  RDLogLine *destline;

  if(to_line<from_line) {
    src_offset=1;
    dest_offset=0;
  }
  insert(to_line+dest_offset,1);
  if((to_line+1)>=size()) {
    to_line=size()-1;
    dest_offset=0;
  }

  if(((destline=logLine(to_line+dest_offset))==NULL)||
     (srcline=logLine(from_line+src_offset))==NULL) {
    remove(to_line+dest_offset,1);
    return;
  }
  *destline=*srcline;
  destline->clearTrackData(RDLogLine::AllTrans);
  remove(from_line+src_offset,1);
}


void RDLogEvent::copy(int from_line,int to_line)
{
  RDLogLine *srcline;
  RDLogLine *destline;

  insert(to_line,1);
  if(((destline=logLine(to_line))==NULL)||
     (srcline=logLine(from_line))==NULL) {
    remove(to_line,1);
    return;
  }
  *destline=*srcline;
  destline->clearExternalData();
  destline->clearTrackData(RDLogLine::AllTrans);
  destline->setSource(RDLogLine::Manual);
}


int RDLogEvent::length(int from_line,int to_line,QTime *sched_time)
{
  if(sched_time!=NULL) {
    *sched_time=QTime();
  }
  if(to_line<0) {
    to_line=size();
    for(int i=from_line;i<size();i++) {
      if(logLine(i)->timeType()==RDLogLine::Hard) {
	to_line=i;
	i=size();
	if(sched_time!=NULL) {
	  *sched_time=logLine(i)->startTime(RDLogLine::Logged);
	}
      }
    }
  }
  int len=0;
  for(int i=from_line;i<to_line;i++) {
    if(((i+1)>=size())||(logLine(i+1)->transType()!=RDLogLine::Segue)||
       (logLine(i)->segueStartPoint()<0)) {
      len+=logLine(i)->forcedLength();
    }
    else {
      len+=logLine(i)->segueStartPoint()-logLine(i)->startPoint();
    }
  }

  return len;
}


int RDLogEvent::lengthToStop(int from_line,QTime *sched_time)
{
  int to_line=-1;

  for(int i=from_line;i<size();i++) {
    if(logLine(i)->transType()==RDLogLine::Stop) {
      to_line=i;
    }
  }
  if(to_line<0) {
    return -1;
  }
  return length(from_line,to_line,sched_time);
}


bool RDLogEvent::blockLength(int *nominal_length,int *actual_length,int line)
{
  *nominal_length=0;
  *actual_length=0;
  QTime start_time;
  int start_line=-1;
  QTime end_time;
  int end_line=-1;

  if((line<0)||(line>(size()-1))) {
    *nominal_length=0;
    *actual_length=0;
    return false;
  }

  //
  // Find Block Start
  //
  for(int i=line;i>=0;i--) {
    if(logLine(i)->timeType()==RDLogLine::Hard) {
      start_time=logLine(i)->startTime(RDLogLine::Logged);
      start_line=i;
      i=-1;
    }
  }
  if(start_line<0) {
    return false;
  }

  //
  // Find Block End
  //
  for(int i=line+1;i<size();i++) {
    if(logLine(i)->timeType()==RDLogLine::Hard) {
      end_time=logLine(i)->startTime(RDLogLine::Logged);
      end_line=i;
      i=size();
    }
  }
  if(end_line<0) {
    return false;
  }

  //
  // Calculate Lengths
  //
  *nominal_length=start_time.msecsTo(end_time);
  for(int i=start_line;i<end_line;i++) {
	  if((i<(size()+1))&&((logLine(i+1)->transType()==RDLogLine::Segue))) {
      *actual_length+=logLine(i)->averageSegueLength();
    }
    else {
      *actual_length+=logLine(i)->forcedLength();
    }
  }

  return true;
}

QTime RDLogEvent::blockStartTime(int line)
{
  int actual_length=0;
  QTime start_time(0,0,0);
  QTime return_time(0,0,0);
  int start_line=0;

  if((line<0)||(line>(size()-1))) {
    actual_length=0;
    return return_time;
  }

  //
  // Find Block Start
  //
  for(int i=line;i>=0;i--) {
    if(logLine(i)->timeType()==RDLogLine::Hard) {
      start_time=logLine(i)->startTime(RDLogLine::Logged);
      start_line=i;
      i=-1;
    }
  }
  if(start_line == line) {
    return start_time;
  }

  //
  // Calculate Lengths
  //
  for(int i=start_line;i<line;i++) {
    if((i<(size()+1))&&((logLine(i+1)->transType()==RDLogLine::Segue))) {
      if(logLine(i)->segueStartPoint(RDLogLine::LogPointer)<0) {
	actual_length+=100*(logLine(i)->averageSegueLength()/100);
      }
      else {
	if(logLine(i)->startPoint(RDLogLine::LogPointer)<0) {
	  actual_length+=(logLine(i)->segueStartPoint(RDLogLine::LogPointer)-
			  logLine(i)->startPoint(RDLogLine::CartPointer));
	}
	else {
	  actual_length+=(logLine(i)->segueStartPoint(RDLogLine::LogPointer)-
			  logLine(i)->startPoint(RDLogLine::LogPointer));
	}
      }
    }
    else {
      actual_length+=100*(logLine(i)->forcedLength()/100);
    }
  }
  return_time=start_time.addMSecs(actual_length);
  return return_time;
}


RDLogLine *RDLogEvent::logLine(int line) const
{
  if((line<0)||((unsigned)line>=log_line.size())) {
    return NULL;
  }
  return log_line[line];
}


void RDLogEvent::setLogLine(int line,RDLogLine *ll)
{
  int id=log_line[line]->id();
  *log_line[line]=*ll;
  log_line[line]->setId(id);
}


RDLogLine *RDLogEvent::loglineById(int id, bool ignore_holdovers) const
{
  int line = lineById(id, ignore_holdovers);
  if(line == -1)
    return NULL;
  else
    return log_line[line];
}


int RDLogEvent::lineById(int id, bool ignore_holdovers) const
{
  for(int i=0;i<size();i++) {
    if(ignore_holdovers && log_line[i]->isHoldover()) {
      continue;
    }    
    if(log_line[i]->id()==id) {
      return i;
    }
  }
  return -1;
}


int RDLogEvent::lineByStartHour(int hour,RDLogLine::StartTimeType type) const
{
  for(int i=0;i<size();i++) {
    if(!log_line[i]->startTime(type).isNull()&&
       (log_line[i]->startTime(type).hour()==hour)) {
      return i;
    }
  }
  return -1;
}


int RDLogEvent::lineByStartHour(int hour) const
{
  int line=-1;

  if((line=lineByStartHour(hour,RDLogLine::Initial))<0) {
    if((line=lineByStartHour(hour,RDLogLine::Predicted))<0) {
      line=lineByStartHour(hour,RDLogLine::Imported);
    }
  }
  return line;
}


int RDLogEvent::nextTimeStart(QTime after)
{
  for(unsigned i=0;i<log_line.size();i++) {
    if((log_line[i]->timeType()==RDLogLine::Hard)&&
       (log_line[i]->startTime(RDLogLine::Logged)>after)) {
      return i;
    }
  }
  return -1;
}


RDLogLine::TransType RDLogEvent::nextTransType(int line)
{
  if(line<(size()-1)) {
    return logLine(line+1)->transType();
  }
  return RDLogLine::Stop;
}


void RDLogEvent::removeCustomTransition(int line)
{
  if((line<0)||(line>(size()-1))) {
    return;
  }
  logLine(line)->setStartPoint(-1,RDLogLine::LogPointer);
  logLine(line)->setFadeupPoint(-1,RDLogLine::LogPointer);
  logLine(line)->setFadeupGain(0);
  logLine(line)->setDuckUpGain(0);
  logLine(line)->setHasCustomTransition(false);
  if(line<1) {
    return;
  }
  if(logLine(line-1)->type()!=RDLogLine::Track) {
    logLine(line-1)->setEndPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setSegueStartPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setSegueEndPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setSegueGain(RD_FADE_DEPTH);
    logLine(line-1)->setFadedownPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setFadedownGain(0);
    logLine(line-1)->setDuckDownGain(0);
    return;
  }
  if(line<2) {
    return;
  }
  logLine(line-2)->setEndPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setSegueStartPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setSegueEndPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setSegueGain(RD_FADE_DEPTH);
  logLine(line-2)->setFadedownPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setFadedownGain(0);
  logLine(line-2)->setDuckDownGain(0);
}


int RDLogEvent::nextId() const
{
  int id=-1;
  for(int i=0;i<size();i++) {
    if(log_line[i]->id()>id) {
      id=log_line[i]->id();
    }
  }
  return id+1;
}


int RDLogEvent::nextLinkId() const
{
  int id=-1;
  for(int i=0;i<size();i++) {
    if(log_line[i]->linkId()>id) {
      id=log_line[i]->linkId();
    }
  }
  return id+1;
}


QString RDLogEvent::xml() const
{
  QString ret;

  ret+="<logList>\n";
  for(int i=0;i<size();i++) {
    ret+=logLine(i)->xml(i);
  }
  ret+="</logList>\n";

  return ret;
}


int RDLogEvent::LoadLines(const QString &logname,int id_offset,bool track_ptrs)
{
  RDLogLine line;
  RDSqlQuery *q1;
  QString sql;
  RDSqlQuery *q;
  bool prev_custom=false;
  unsigned lines=0;
  unsigned start_line=log_line.size();

  //
  // Load the group color table
  //
  std::map<QString,QColor> group_colors;
  sql="select NAME,COLOR from GROUPS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    group_colors[q->value(0).toString()]=QColor(q->value(1).toString());
  }
  delete q;

  //
  // Load log lines
  //
  sql=QString("select ")+
    "LOG_LINES.LINE_ID,"+            // 00
    "LOG_LINES.CART_NUMBER,"+        // 01
    "LOG_LINES.START_TIME,"+         // 02
    "LOG_LINES.TIME_TYPE,"+          // 03
    "LOG_LINES.TRANS_TYPE,"+         // 04
    "LOG_LINES.START_POINT,"+        // 05
    "LOG_LINES.END_POINT,"+          // 06
    "LOG_LINES.SEGUE_START_POINT,"+  // 07
    "LOG_LINES.SEGUE_END_POINT,"+    // 08
    "CART.TYPE,"+                    // 09
    "CART.GROUP_NAME,"+              // 10
    "CART.TITLE,"+                   // 11
    "CART.ARTIST,"+                  // 12
    "CART.ALBUM,"+                   // 13
    "CART.YEAR,"+                    // 14
    "CART.LABEL,"+                   // 15
    "CART.CLIENT,"+                  // 16
    "CART.AGENCY,"+                  // 17
    "CART.USER_DEFINED,"+            // 18
    "CART.CONDUCTOR,"+               // 19
    "CART.SONG_ID,"+                 // 20
    "CART.FORCED_LENGTH,"+           // 21
    "CART.CUT_QUANTITY,"+            // 22
    "CART.LAST_CUT_PLAYED,"+         // 23
    "CART.PLAY_ORDER,"+              // 24
    "CART.ENFORCE_LENGTH,"+          // 25
    "CART.PRESERVE_PITCH ,"+         // 26
    "LOG_LINES.TYPE,"+               // 27
    "LOG_LINES.COMMENT,"+            // 28
    "LOG_LINES.LABEL,"+              // 29
    "LOG_LINES.GRACE_TIME,"+         // 30
    "LOG_LINES.SOURCE,"+             // 31
    "LOG_LINES.EXT_START_TIME,"+     // 32
    "LOG_LINES.EXT_LENGTH,"+         // 33
    "LOG_LINES.EXT_DATA,"+           // 34
    "LOG_LINES.EXT_EVENT_ID,"+       // 35
    "LOG_LINES.EXT_ANNC_TYPE,"+      // 36
    "LOG_LINES.EXT_CART_NAME,"+      // 37
    "CART.ASYNCRONOUS,"+             // 38
    "LOG_LINES.FADEUP_POINT,"+       // 39
    "LOG_LINES.FADEUP_GAIN,"+        // 40
    "LOG_LINES.FADEDOWN_POINT,"+     // 41
    "LOG_LINES.FADEDOWN_GAIN,"+      // 42
    "LOG_LINES.SEGUE_GAIN,"+         // 43
    "CART.PUBLISHER,"+               // 44
    "CART.COMPOSER,"+                // 45
    "CART.USAGE_CODE,"+              // 46
    "CART.AVERAGE_SEGUE_LENGTH,"+    // 47
    "LOG_LINES.LINK_EVENT_NAME,"+    // 48
    "LOG_LINES.LINK_START_TIME,"+    // 49
    "LOG_LINES.LINK_LENGTH,"+        // 50
    "LOG_LINES.LINK_ID,"+            // 51
    "LOG_LINES.LINK_EMBEDDED,"+      // 52
    "LOG_LINES.ORIGIN_USER,"+        // 53
    "LOG_LINES.ORIGIN_DATETIME,"+    // 54
    "CART.VALIDITY,"+                // 55
    "LOG_LINES.LINK_START_SLOP,"+    // 56
    "LOG_LINES.LINK_END_SLOP,"+      // 57
    "LOG_LINES.DUCK_UP_GAIN,"+       // 58
    "LOG_LINES.DUCK_DOWN_GAIN,"+     // 59
    "CART.START_DATETIME,"+          // 60
    "CART.END_DATETIME,"+            // 61
    "LOG_LINES.EVENT_LENGTH,"+       // 62
    "CART.USE_EVENT_LENGTH,"+        // 63
    "CART.NOTES	"+                   // 64
    "from LOG_LINES left join CART "+
    "on LOG_LINES.CART_NUMBER=CART.NUMBER where "+
    "LOG_LINES.LOG_NAME=\""+RDEscapeString(logname)+"\" "+
    "order by COUNT";
   q=new RDSqlQuery(sql);
  if(q->size()<=0) {
    delete q;
    return 0;
  }
  for(int i=0;i<q->size();i++) {
    lines++;
    line.clear();
    q->next();
    line.setType((RDLogLine::Type)q->value(27).toInt());       // Type
    line.setId(q->value(0).toInt()+id_offset);                 // Log Line ID
    if((q->value(0).toInt()+id_offset)>log_max_id) {
      log_max_id=q->value(0).toInt()+id_offset;
    }
    line.setStartTime(RDLogLine::Imported,
		      QTime().addMSecs(q->value(2).toInt())); // Start Time
    line.setStartTime(RDLogLine::Logged,
		      QTime().addMSecs(q->value(2).toInt()));
    line.
      setTimeType((RDLogLine::TimeType)q->value(3).toInt());   // Time Type
    line.
      setTransType((RDLogLine::TransType)q->value(4).toInt()); // Trans Type
    line.setMarkerComment(q->value(28).toString());         // Comment
    line.setMarkerLabel(q->value(29).toString());           // Label
    line.setGraceTime(q->value(30).toInt());                // Grace Time
    line.setUseEventLength(RDBool(q->value(63).toString())); // Use Event Length
    line.setEventLength(q->value(62).toInt());              // Event Length
    line.setSource((RDLogLine::Source)q->value(31).toUInt());
    line.setLinkEventName(q->value(48).toString());         // Link Event Name
    line.setLinkStartTime(QTime().addMSecs(q->value(49).toInt()));   // Link Start Time
    line.setLinkLength(q->value(50).toInt());               // Link Length
    line.setLinkStartSlop(q->value(56).toInt());            // Link Start Slop
    line.setLinkEndSlop(q->value(57).toInt());              // Link End Slop
    line.setLinkId(q->value(51).toInt());                   // Link ID
    line.setLinkEmbedded(RDBool(q->value(52).toString()));   // Link Embedded
    line.setOriginUser(q->value(53).toString());            // Origin User
    line.setOriginDateTime(q->value(54).toDateTime());      // Origin DateTime
    switch(line.type()) {
    case RDLogLine::Cart:
      line.setCartNumber(q->value(1).toUInt());          // Cart Number
      line.setStartPoint(q->value(5).toInt(),RDLogLine::LogPointer);
      line.setEndPoint(q->value(6).toInt(),RDLogLine::LogPointer);
      line.setSegueStartPoint(q->value(7).toInt(),RDLogLine::LogPointer);
      line.setSegueEndPoint(q->value(8).toInt(),RDLogLine::LogPointer);
      line.setCartType((RDCart::Type)q->value(9).toInt());  // Cart Type
      line.setGroupName(q->value(10).toString());       // Group Name
      line.setGroupColor(group_colors[q->value(10).toString()]);
      line.setTitle(q->value(11).toString());           // Title
      line.setArtist(q->value(12).toString());          // Artist
      line.setPublisher(q->value(44).toString());       // Publisher
      line.setComposer(q->value(45).toString());        // Composer
      line.setAlbum(q->value(13).toString());           // Album
      line.setYear(q->value(14).toDate());              // Year
      line.setLabel(q->value(15).toString());           // Label
      line.setClient(q->value(16).toString());          // Client
      line.setAgency(q->value(17).toString());          // Agency
      line.setUserDefined(q->value(18).toString());     // User Defined
      line.setCartNotes(q->value(64).toString());       // Cart Notes
      line.setConductor(q->value(19).toString());       // Conductor
      line.setSongId(q->value(20).toString());          // Song ID
      line.setUsageCode((RDCart::UsageCode)q->value(46).toInt());
      line.setForcedLength(q->value(21).toUInt());      // Forced Length
      if(q->value(7).toInt()<0) {
	line.setAverageSegueLength(q->value(47).toInt());
      }
      else {
	line.
	  setAverageSegueLength(q->value(7).toInt()-q->value(5).toInt());
      }
      line.setCutQuantity(q->value(22).toUInt());       // Cut Quantity
      line.setLastCutPlayed(q->value(23).toUInt());     // Last Cut Played
      line.
	setPlayOrder((RDCart::PlayOrder)q->value(24).toUInt()); // Play Ord
      line.
	setEnforceLength(RDBool(q->value(25).toString())); // Enforce Length
      line.
	setPreservePitch(RDBool(q->value(26).toString())); // Preserve Pitch
      if(!q->value(32).isNull()) {                      // Ext Start Time
	line.setExtStartTime(q->value(32).toTime());
      }
      if(!q->value(33).isNull()) {                      // Ext Length
	line.setExtLength(q->value(33).toInt());
      }
      if(!q->value(34).isNull()) {                      // Ext Data
	line.setExtData(q->value(34).toString());
      }
      if(!q->value(35).isNull()) {                      // Ext Event ID
	line.setExtEventId(q->value(35).toString());
      }
      if(!q->value(36).isNull()) {                      // Ext Annc. Type
	line.setExtAnncType(q->value(36).toString());
      }
      if(!q->value(37).isNull()) {                      // Ext Cart Name
	line.setExtCartName(q->value(37).toString());
      }
      if(!q->value(39).isNull()) {                      // FadeUp Point
	line.setFadeupPoint(q->value(39).toInt(),RDLogLine::LogPointer);
      }
      if(!q->value(40).isNull()) {                      // FadeUp Gain
	line.setFadeupGain(q->value(40).toInt());
      }
      if(!q->value(41).isNull()) {                      // FadeDown Point
	line.setFadedownPoint(q->value(41).toInt(),RDLogLine::LogPointer);
      }
      if(!q->value(42).isNull()) {                      // FadeDown Gain
	line.setFadedownGain(q->value(42).toInt());
      }
      if(!q->value(43).isNull()) {                      // Segue Gain
	line.setSegueGain(q->value(43).toInt());
      }
      if(!q->value(58).isNull()) {                      // Duck Up Gain
	line.setDuckUpGain(q->value(58).toInt());
      }
      if(!q->value(59).isNull()) {                      // Duck Down Gain
	line.setDuckDownGain(q->value(59).toInt());
      }
      if(!q->value(60).isNull()) {                      // Start Datetime
	line.setStartDatetime(q->value(60).toDateTime());
      }
      if(!q->value(61).isNull()) {                      // End Datetime
	line.setEndDatetime(q->value(61).toDateTime());
      }
      line.setValidity((RDCart::Validity)q->value(55).toInt()); // Validity
      break;

    case RDLogLine::Macro:
      line.setCartNumber(q->value(1).toUInt());          // Cart Number
      line.setCartType((RDCart::Type)q->value(9).toInt());  // Cart Type
      line.setGroupName(q->value(10).toString());       // Group Name
      line.setGroupColor(group_colors[q->value(10).toString()]);
      line.setTitle(q->value(11).toString());           // Title
      line.setArtist(q->value(12).toString());          // Artist
      line.setPublisher(q->value(44).toString());       // Publisher
      line.setComposer(q->value(45).toString());        // Composer
      line.setAlbum(q->value(13).toString());           // Album
      line.setYear(q->value(14).toDate());              // Year
      line.setLabel(q->value(15).toString());           // Label
      line.setClient(q->value(16).toString());          // Client
      line.setAgency(q->value(17).toString());          // Agency
      line.setUserDefined(q->value(18).toString());     // User Defined
      line.setCartNotes(q->value(64).toString());       // Cart Notes
      line.setForcedLength(q->value(21).toUInt());      // Forced Length
      line.setAverageSegueLength(q->value(21).toInt());
      if(!q->value(32).isNull()) {                      // Ext Start Time
	line.setExtStartTime(q->value(32).toTime());
      }
      if(!q->value(33).isNull()) {                      // Ext Length
	line.setExtLength(q->value(33).toInt());
      }
      if(!q->value(34).isNull()) {                      // Ext Data
	line.setExtData(q->value(34).toString());
      }
      if(!q->value(35).isNull()) {                      // Ext Event ID
	line.setExtEventId(q->value(35).toString());
      }
      if(!q->value(36).isNull()) {                      // Ext Annc. Type
	line.setExtAnncType(q->value(36).toString());
      }
      if(!q->value(37).isNull()) {                      // Ext Cart Name
	line.setExtCartName(q->value(37).toString());
      }
      if(!q->value(38).isNull()) {                      // Asyncronous
	line.setAsyncronous(RDBool(q->value(38).toString()));
      }
      break;

    case RDLogLine::Marker:
      break;

    case RDLogLine::Track:
      break;

    case RDLogLine::Chain:
      sql=QString("select DESCRIPTION from LOGS where ")+
	"NAME=\""+RDEscapeString(line.markerLabel())+"\"";
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	line.setMarkerComment(q1->value(0).toString());
      }
      delete q1;
      break;

    default:
      break;
    }

    line.setHasCustomTransition(prev_custom||(q->value(5).toInt()>=0)||\
				(q->value(39).toInt()>=0));
    if(line.type()==RDLogLine::Cart) {
      prev_custom=(q->value(6).toInt()>=0)||(q->value(7).toInt()>=0)||
	(q->value(8).toInt()>=0)||(q->value(41).toInt()>=0);
    }
    else {
      prev_custom=false;
    }
    line.clearModified();
    log_line.push_back(new RDLogLine(line));
  }
  delete q;

  LoadNowNext(start_line);

  if(track_ptrs) {
    //
    // Load default cart pointers for "representative" cuts.  This is
    // really only useful when setting up a voice tracker.
    //
    for(int i=start_line;i<size();i++) {
      RDLogLine *ll=logLine(i);
      if(ll->cartType()==RDCart::Audio) {
	sql=QString("select START_POINT,END_POINT,")+
	  "SEGUE_START_POINT,SEGUE_END_POINT,"+
	  "TALK_START_POINT,TALK_END_POINT,"
	  "HOOK_START_POINT,HOOK_END_POINT,"+
	  "FADEUP_POINT,FADEDOWN_POINT,CUT_NAME,"+
	  "ORIGIN_NAME,ORIGIN_DATETIME from CUTS "+
	  QString().sprintf("where CART_NUMBER=%u ",ll->cartNumber())+
	  "order by CUT_NAME";
	q=new RDSqlQuery(sql);
	if(q->first()) {
	  ll->setStartPoint(q->value(0).toInt(),RDLogLine::CartPointer);
	  ll->setEndPoint(q->value(1).toInt(),RDLogLine::CartPointer);
	  ll->setSegueStartPoint(q->value(2).toInt(),RDLogLine::CartPointer);
	  ll->setSegueEndPoint(q->value(3).toInt(),RDLogLine::CartPointer);
	  ll->setTalkStartPoint(q->value(4).toInt());
	  ll->setTalkEndPoint(q->value(5).toInt());
	  ll->setHookStartPoint(q->value(6).toInt());
	  ll->setHookEndPoint(q->value(7).toInt());
	  ll->setFadeupPoint(q->value(8).toInt(),RDLogLine::CartPointer);
	  ll->setFadedownPoint(q->value(9).toInt(),RDLogLine::CartPointer);
	  ll->setCutNumber(RDCut::cutNumber(q->value(10).toString()));
	  ll->setOriginUser(q->value(11).toString());
	  ll->setOriginDateTime(q->value(12).toDateTime());
	}
	delete q;
      }
    }
  }

  return lines;
}


void RDLogEvent::InsertLines(QString values) {
  QString sql;
  RDSqlQuery *q;

  sql = QString("insert into LOG_LINES (")+
    "LOG_NAME,"+           // 00
    "LINE_ID,"+            // 01
    "COUNT,"+              // 02
    "CART_NUMBER,"+        // 03
    "START_TIME,"+         // 04
    "TIME_TYPE,"+          // 05
    "TRANS_TYPE,"+         // 06
    "START_POINT,"+        // 07
    "END_POINT,"+          // 08
    "SEGUE_START_POINT,"+  // 09
    "SEGUE_END_POINT,"+    // 10
    "TYPE,"+               // 11
    "COMMENT,"+            // 12
    "LABEL,"+              // 13
    "GRACE_TIME,"+         // 14
    "SOURCE,"+             // 15
    "EXT_START_TIME,"+     // 16
    "EXT_LENGTH,"+         // 17
    "EXT_DATA,"+           // 18
    "EXT_EVENT_ID,"+       // 19
    "EXT_ANNC_TYPE,"+      // 20
    "EXT_CART_NAME,"+      // 21
    "FADEUP_POINT,"+       // 22
    "FADEUP_GAIN,"+        // 23
    "FADEDOWN_POINT,"+     // 24
    "FADEDOWN_GAIN,"+      // 25
    "SEGUE_GAIN,"+         // 26
    "LINK_EVENT_NAME,"+    // 27
    "LINK_START_TIME,"+    // 28
    "LINK_LENGTH,"+        // 29
    "LINK_ID,"+            // 30
    "LINK_EMBEDDED,"+      // 31
    "ORIGIN_USER,"+        // 32
    "ORIGIN_DATETIME,"+    // 33
    "LINK_START_SLOP,"+    // 34
    "LINK_END_SLOP,"+      // 35
    "DUCK_UP_GAIN,"+       // 36
    "DUCK_DOWN_GAIN,"+     // 37
    "EVENT_LENGTH) "+      // 38
    "values "+values;
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLogEvent::InsertLineValues(QString *query, int line)
{
  // one line to save query space
  RDLogLine *ll=log_line[line];
  QString sql=QString("(")+
    "\""+RDEscapeString(log_name)+"\","+
    QString().sprintf("%d,",ll->id())+
    QString().sprintf("%d,",line)+
    QString().sprintf("%u,",ll->cartNumber())+
    QString().sprintf("%d,",QTime().msecsTo(ll->startTime(RDLogLine::Logged)))+
    QString().sprintf("%d,",ll->timeType())+
    QString().sprintf("%d,",ll->transType())+
    QString().sprintf("%d,",ll->startPoint(RDLogLine::LogPointer))+
    QString().sprintf("%d,",ll->endPoint(RDLogLine::LogPointer))+
    QString().sprintf("%d,",ll->segueStartPoint(RDLogLine::LogPointer))+
    QString().sprintf("%d,",ll->segueEndPoint(RDLogLine::LogPointer))+
    QString().sprintf("%d,",ll->type())+
    "\""+RDEscapeString(ll->markerComment())+"\","+
    "\""+RDEscapeString(ll->markerLabel())+"\","+
    QString().sprintf("%d,",ll->graceTime())+
    QString().sprintf("%d,",ll->source())+
    RDCheckDateTime(ll->extStartTime(),"hh:mm:ss")+","+
    QString().sprintf("%d,",ll->extLength())+
    "\""+RDEscapeString(ll->extData())+"\","+
    "\""+RDEscapeString(ll->extEventId())+"\","+
    "\""+RDEscapeString(ll->extAnncType())+"\","+
    "\""+RDEscapeString(ll->extCartName())+"\","+
    QString().sprintf("%d,",ll->fadeupPoint(RDLogLine::LogPointer))+
    QString().sprintf("%d,",ll->fadeupGain())+
    QString().sprintf("%d,",ll->fadedownPoint(RDLogLine::LogPointer))+
    QString().sprintf("%d,",ll->fadedownGain())+
    QString().sprintf("%d,",ll->segueGain())+
    "\""+RDEscapeString(ll->linkEventName())+"\","+
    QString().sprintf("%d,",QTime().msecsTo(ll->linkStartTime()))+
    QString().sprintf("%d,",ll->linkLength())+
    QString().sprintf("%d,",ll->linkId())+
    "\""+RDYesNo(ll->linkEmbedded())+"\","+
    "\""+RDEscapeString(ll->originUser())+"\","+
    RDCheckDateTime(ll->originDateTime(),"yyyy-MM-dd hh:mm:ss")+","+
    QString().sprintf("%d,",ll->linkStartSlop())+
    QString().sprintf("%d,",ll->linkEndSlop())+
    QString().sprintf("%d,",ll->duckUpGain())+
    QString().sprintf("%d,",ll->duckDownGain())+
    QString().sprintf("%d)",ll->eventLength());
  *query += sql;
}

void RDLogEvent::SaveLine(int line)
{
  QString values = "";
  InsertLineValues(&values, line);
  InsertLines(values);
}


void RDLogEvent::LoadNowNext(unsigned from_line)
{
  std::vector<QString> groups;
  std::vector<bool> now_nexts;

  //
  // Load the Lookup Table
  //
  RDSqlQuery *q=new RDSqlQuery("select NAME,ENABLE_NOW_NEXT from GROUPS");
  while(q->next()) {
    groups.push_back(QString(q->value(0).toString()));
    now_nexts.push_back(RDBool(q->value(1).toString()));
  }
  delete q;

  for(unsigned i=from_line;i<log_line.size();i++) {
    for(unsigned j=0;j<groups.size();j++) {
      if(log_line[i]->groupName()==groups[j]) {
	log_line[i]->setNowNextEnabled(now_nexts[j]);
      }
    }
  }
}

