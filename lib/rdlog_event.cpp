// rdlog_event.cpp
//
// Abstract Rivendell Log Events.
//
//   (C) Copyright 2002-2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlog_event.cpp,v 1.101.4.13.2.2 2014/05/22 19:37:44 cvs Exp $
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
#include <rddb.h>
#include <rdconf.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdcreate_log.h>
#include <rddebug.h>
#include <rd.h>
#include <rdescape_string.h>

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
  if(log_name.isEmpty()) {
    return false;
  }
  QString sql="show tables";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(0).toString()==log_name) {
      delete q;
      return true;
    }
  }
  delete q;

  return false;
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
  log_name=logname;
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
  sql=QString().sprintf("select SERVICE from LOGS where NAME=\"%s\"",
	     (const char *)RDEscapeString(log_name.left(log_name.length()-4)));
  q=new RDSqlQuery(sql);
  if(q->next()) {
    log_service_name=q->value(0).toString();
  }
  delete q;

  RDLog *log=new RDLog(log_name.left(log_name.length()-4));
  log_max_id=log->nextId();
  delete log;

  LoadLines(log_name,0,track_ptrs);

  return log_line.size();
}


void RDLogEvent::save(bool update_tracks,int line)
{
  QString sql;
  RDSqlQuery *q;

  if(log_name.isEmpty()) {
    return;
  }
  if(line<0) {
    if(exists()) {
      sql=QString().sprintf("drop table `%s`",(const char *)log_name);
      q=new RDSqlQuery(sql);
      delete q;
    }
    RDCreateLogTable(log_name);
    for(unsigned i=0;i<log_line.size();i++) {
      SaveLine(i);
    }
  }
  else {
    sql=QString().sprintf("delete from `%s` where COUNT=%d",
			  (const char *)log_name,line);
    q=new RDSqlQuery(sql);
    delete q;
    SaveLine(line);
  }
  RDLog *log=new RDLog(log_name.left(log_name.length()-4));
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
  return LoadLines(RDLog::tableName(logname),log_max_id,track_ptrs);
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
  *report+=
    QString().
    sprintf("Generated at: %s - %s\n",
	    (const char *)QDate::currentDate().toString("MM/dd/yyyy"),
	    (const char *)QTime::currentTime().toString("hh:mm:ss"));
  *report+=QString().sprintf("Log: %s\n",
			     (const char *)log_name.left(log_name.length()-4));
  *report+=QString().sprintf("Effective Airdate: %s\n",
			     (const char *)date.toString("MM/dd/yyyy"));
  *report+="\n";

  //
  // Line Scan
  //
  for(int i=0;i<size();i++) {
    if(logLine(i)->cartNumber()>0) {
      sql=QString().sprintf("select TYPE,TITLE from CART where NUMBER=%d",
			    logLine(i)->cartNumber());
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	*report+=QString().
	  sprintf(" %s - missing cart %06d\n",
		  (const char *)logLine(i)->startTime(RDLogLine::Logged).
		  toString("hh:mm:ss"),
		  logLine(i)->cartNumber());
	errs++;
      }
      else {
	if((RDCart::Type)q->value(0).toInt()==RDCart::Audio) {
	  if(logLine(i)->startTime(RDLogLine::Logged).isNull()) {
	    //
	    // Handle events with no logged start time (e.g. manual inserts)
	    //
	    sql=QString().
	      sprintf("select CUT_NAME from CUTS where \
                      (CART_NUMBER=%u)&&			\
                      ((START_DATETIME is null)||		\
                         (START_DATETIME<=\"%s 23:59:59\"))&&	\
                      ((END_DATETIME is null)||			\
                         (END_DATETIME>=\"%s 00:00:00\"))&&	\
                      (%s=\"Y\")&&(LENGTH>0)",
		      logLine(i)->cartNumber(),
		      (const char *)date.toString("yyyy-MM-dd"),
		      (const char *)date.toString("yyyy-MM-dd"),
		      (const char *)RDDowCode(date.dayOfWeek()));
	  }
	  else {
	    sql=QString().
	      sprintf("select CUT_NAME from CUTS where \
                     (CART_NUMBER=%u)&&					\
                     ((START_DATETIME is null)||(START_DATETIME<=\"%s %s\"))&& \
                     ((END_DATETIME is null)||(END_DATETIME>=\"%s %s\"))&& \
                     ((START_DAYPART is null)||(START_DAYPART<=\"%s\"))&& \
                     ((END_DAYPART is null)||(END_DAYPART>=\"%s\"))&&	\
                     (%s=\"Y\")&&(LENGTH>0)",
		      logLine(i)->cartNumber(),
		      (const char *)date.toString("yyyy-MM-dd"),
		      (const char *)logLine(i)->startTime(RDLogLine::Logged).
		      toString("hh:mm:ss"),
		      (const char *)date.toString("yyyy-MM-dd"),
		      (const char *)logLine(i)->startTime(RDLogLine::Logged).
		      toString("hh:mm:ss"),
		      (const char *)logLine(i)->startTime(RDLogLine::Logged).
		      toString("hh:mm:ss"),
		      (const char *)logLine(i)->startTime(RDLogLine::Logged).
		      toString("hh:mm:ss"),
		      (const char *)RDDowCode(date.dayOfWeek()));
	  }
	  q1=new RDSqlQuery(sql);
	  if(!q1->first()) {
	    *report+=QString().
	      sprintf(" %s - cart %06d [%s] is not playable\n",
		      (const char *)logLine(i)->startTime(RDLogLine::Logged).
		      toString("hh:mm:ss"),logLine(i)->cartNumber(),
		      (const char *)q->value(1).toString());
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
    *report+=QString().sprintf("%d exception found.\n\n",errs);
  }
  else {
    *report+=QString().sprintf("%d exceptions found.\n\n",errs);
  }
  return errs;
}


void RDLogEvent::refresh(int line)
{
  if(log_name.isEmpty()||log_line[line]->cartNumber()==0) {
    return;
  }
  QString sql=QString().sprintf("select CART.TYPE,CART.GROUP_NAME,CART.TITLE,\
                                 CART.ARTIST,CART.ALBUM,CART.YEAR,CART.LABEL,\
                                 CART.CLIENT,CART.AGENCY,CART.USER_DEFINED,\
                                 CART.FORCED_LENGTH,CART.CUT_QUANTITY,\
                                 CART.LAST_CUT_PLAYED,CART.PLAY_ORDER,\
                                 CART.ENFORCE_LENGTH,CART.PRESERVE_PITCH,\
                                 CART.PUBLISHER,CART.COMPOSER,CART.USAGE_CODE,\
                                 CART.AVERAGE_SEGUE_LENGTH,CART.VALIDITY,\
                                 CART.NOTES,GROUPS.COLOR from CART \
                                 left join GROUPS \
                                 on CART.GROUP_NAME=GROUPS.NAME \
                                 where CART.NUMBER=%u",
				log_line[line]->cartNumber());
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
    if(line>0) {
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
      actual_length+=logLine(i)->averageSegueLength();
    }
    else {
      actual_length+=logLine(i)->forcedLength();
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


RDLogLine *RDLogEvent::loglineById(int id) const
{
  for(int i=0;i<size();i++) {
    if(log_line[i]->id()==id) {
      return log_line[i];
    }
  }
  return NULL;
}


int RDLogEvent::lineById(int id) const
{
  for(int i=0;i<size();i++) {
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


int RDLogEvent::LoadLines(const QString &log_table,int id_offset,
			  bool track_ptrs)
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
  // Field Offsets:
  //  0 - LOG.ID                    1 - LOG.CART_NUMBER
  //  2 - LOG.START_TIME            3 - LOG.TIME_TYPE
  //  4 - LOG.TRANS_TYPE            5 - LOG.START_POINT
  //  6 - LOG.END_POINT             7 - LOG.SEGUE_START_POINT
  //  8 - LOG.SEGUE_END_POINT       9 - CART.TYPE
  // 10 - CART.GROUP_NAME          11 - CART.TITLE
  // 12 - CART.ARTIST              13 - CART.ALBUM
  // 14 - CART.YEAR
  // 15 - CART.LABEL               16 - CART.CLIENT
  // 17 - CART.AGENCY              18 - CART.USER_DEFINED
  // 19 - CART.CONDUCTOR           20 - CART.SONG_ID
  // 21 - CART.FORCED_LENGTH       22 - CART.CUT_QUANTITY
  // 23 - CART.LAST_CUT_PLAYED     24 - CART.PLAY_ORDER
  // 25 - CART.ENFORCE_LENGTH      26 - CART.PRESERVE_PITCH
  // 27 - LOG.TYPE                 28 - LOG.COMMENT
  // 29 - LOG.LABEL                30 - LOG.GRACE_TIME
  // 31 - LOG.POST_POINT           32 - LOG.SOURCE
  // 33 - LOG.EXT_START_TIME       34 - LOG.EXT_LENGTH           
  // 35 - LOG.EXT_DATA             36 - LOG.EXT_EVENT_ID
  // 37 - LOG.EXT_ANNC_TYPE        38 - LOG.EXT_CART_NAME
  // 39 - CART.ASYNCRONOUS         40 - LOG.FADEUP_POINT
  // 41 - LOG.FADEUP_GAIN          42 - LOG.FADEDOWN_POINT
  // 43 - LOG.FADEDOWN_GAIN        44 - LOG.SEGUE_GAIN
  // 45 - CART.PUBLISHER           46 - CART.COMPOSER
  // 47 - CART.USAGE_CODE          48 - CART.AVERAGE_SEGUE_LENGTH
  // 49 - LOG.LINK_EVENT_NAME      50 - LOG.LINK_START_TIME
  // 51 - LOG.LINK_LENGTH          52 - LOG.LINK_ID
  // 53 - LOG.LINK_EMBEDDED        54 - LOG.ORIGIN_USER
  // 55 - LOG.ORIGIN_DATETIME      56 - CART.VALIDITY
  // 57 - LOG.LINK_START_SLOP      58 - LOG.LINK_END_SLOP
  // 59 - LOG.DUCK_UP_GAIN         60 - LOG.DUCK_DOWN_GAIN
  // 61 - CART.START_DATETIME      62 - CART.END_DATETIME
  // 63 - LOG.EVENT_LENGTH         64 - CART.USE_EVENT_LENGTH
  // 65 - CART.NOTES
  //
  sql=QString().sprintf("select `%s`.ID,`%s`.CART_NUMBER,\
`%s`.START_TIME,`%s`.TIME_TYPE,`%s`.TRANS_TYPE,`%s`.START_POINT,\
`%s`.END_POINT,`%s`.SEGUE_START_POINT,`%s`.SEGUE_END_POINT,\
CART.TYPE,CART.GROUP_NAME,CART.TITLE,CART.ARTIST,CART.ALBUM,CART.YEAR,\
CART.LABEL,CART.CLIENT,CART.AGENCY,CART.USER_DEFINED,\
CART.CONDUCTOR,CART.SONG_ID,\
CART.FORCED_LENGTH,CART.CUT_QUANTITY,CART.LAST_CUT_PLAYED,CART.PLAY_ORDER,\
CART.ENFORCE_LENGTH,CART.PRESERVE_PITCH ,`%s`.TYPE,`%s`.COMMENT,\
`%s`.LABEL,`%s`.GRACE_TIME,`%s`.POST_POINT,`%s`.SOURCE,\
`%s`.EXT_START_TIME,`%s`.EXT_LENGTH,`%s`.EXT_DATA,`%s`.EXT_EVENT_ID,\
`%s`.EXT_ANNC_TYPE,`%s`.EXT_CART_NAME,CART.ASYNCRONOUS,`%s`.FADEUP_POINT,\
`%s`.FADEUP_GAIN,`%s`.FADEDOWN_POINT,`%s`.FADEDOWN_GAIN,`%s`.SEGUE_GAIN,\
CART.PUBLISHER,CART.COMPOSER,CART.USAGE_CODE,CART.AVERAGE_SEGUE_LENGTH,\
`%s`.LINK_EVENT_NAME,`%s`.LINK_START_TIME,`%s`.LINK_LENGTH,`%s`.LINK_ID, \
`%s`.LINK_EMBEDDED,`%s`.ORIGIN_USER,`%s`.ORIGIN_DATETIME,CART.VALIDITY, \
`%s`.LINK_START_SLOP,`%s`.LINK_END_SLOP, \
`%s`.DUCK_UP_GAIN,`%s`.DUCK_DOWN_GAIN,CART.START_DATETIME,CART.END_DATETIME,\
`%s`.EVENT_LENGTH,CART.USE_EVENT_LENGTH,CART.NOTES \
from `%s` left join CART on `%s`.CART_NUMBER=CART.NUMBER order by COUNT",
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table,
				(const char *)log_table);
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
    if((line.timeType()==RDLogLine::Hard)&&
       (q->value(31).toString()==QString("Y"))) {              // Post Point
    }
    line.
      setTransType((RDLogLine::TransType)q->value(4).toInt()); // Trans Type
    line.setMarkerComment(q->value(28).toString());         // Comment
    line.setMarkerLabel(q->value(29).toString());           // Label
    line.setGraceTime(q->value(30).toInt());                // Grace Time
    line.setUseEventLength(RDBool(q->value(64).toString())); // Use Event Length
    line.setEventLength(q->value(63).toInt());              // Event Length
    line.setSource((RDLogLine::Source)q->value(32).toUInt());
    line.setLinkEventName(q->value(49).toString());         // Link Event Name
    line.setLinkStartTime(QTime().addMSecs(q->value(50).toInt()));   // Link Start Time
    line.setLinkLength(q->value(51).toInt());               // Link Length
    line.setLinkStartSlop(q->value(57).toInt());            // Link Start Slop
    line.setLinkEndSlop(q->value(58).toInt());              // Link End Slop
    line.setLinkId(q->value(52).toInt());                   // Link ID
    line.setLinkEmbedded(RDBool(q->value(53).toString()));   // Link Embedded
    line.setOriginUser(q->value(54).toString());            // Origin User
    line.setOriginDateTime(q->value(55).toDateTime());      // Origin DateTime
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
      line.setPublisher(q->value(45).toString());       // Publisher
      line.setComposer(q->value(46).toString());        // Composer
      line.setAlbum(q->value(13).toString());           // Album
      line.setYear(q->value(14).toDate());              // Year
      line.setLabel(q->value(15).toString());           // Label
      line.setClient(q->value(16).toString());          // Client
      line.setAgency(q->value(17).toString());          // Agency
      line.setUserDefined(q->value(18).toString());     // User Defined
      line.setCartNotes(q->value(65).toString());       // Cart Notes
      line.setConductor(q->value(19).toString());       // Conductor
      line.setSongId(q->value(20).toString());          // Song ID
      line.setUsageCode((RDCart::UsageCode)q->value(47).toInt());
      line.setForcedLength(q->value(21).toUInt());      // Forced Length
      if((q->value(7).toInt()<0)||(q->value(8).toInt()<0)) {
	line.setAverageSegueLength(q->value(48).toInt());
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
      if(!q->value(33).isNull()) {                      // Ext Start Time
	line.setExtStartTime(q->value(33).toTime());
      }
      if(!q->value(34).isNull()) {                      // Ext Length
	line.setExtLength(q->value(34).toInt());
      }
      if(!q->value(35).isNull()) {                      // Ext Data
	line.setExtData(q->value(35).toString());
      }
      if(!q->value(36).isNull()) {                      // Ext Event ID
	line.setExtEventId(q->value(36).toString());
      }
      if(!q->value(37).isNull()) {                      // Ext Annc. Type
	line.setExtAnncType(q->value(37).toString());
      }
      if(!q->value(38).isNull()) {                      // Ext Cart Name
	line.setExtCartName(q->value(38).toString());
      }
      if(!q->value(40).isNull()) {                      // FadeUp Point
	line.setFadeupPoint(q->value(40).toInt(),RDLogLine::LogPointer);
      }
      if(!q->value(41).isNull()) {                      // FadeUp Gain
	line.setFadeupGain(q->value(41).toInt());
      }
      if(!q->value(42).isNull()) {                      // FadeDown Point
	line.setFadedownPoint(q->value(42).toInt(),RDLogLine::LogPointer);
      }
      if(!q->value(43).isNull()) {                      // FadeDown Gain
	line.setFadedownGain(q->value(43).toInt());
      }
      if(!q->value(44).isNull()) {                      // Segue Gain
	line.setSegueGain(q->value(44).toInt());
      }
      if(!q->value(59).isNull()) {                      // Duck Up Gain
	line.setDuckUpGain(q->value(59).toInt());
      }
      if(!q->value(60).isNull()) {                      // Duck Down Gain
	line.setDuckDownGain(q->value(60).toInt());
      }
      if(!q->value(61).isNull()) {                      // Start Datetime
	line.setStartDatetime(q->value(61).toDateTime());
      }
      if(!q->value(62).isNull()) {                      // End Datetime
	line.setEndDatetime(q->value(62).toDateTime());
      }
      line.setValidity((RDCart::Validity)q->value(56).toInt()); // Validity
      break;

    case RDLogLine::Macro:
      line.setCartNumber(q->value(1).toUInt());          // Cart Number
      line.setCartType((RDCart::Type)q->value(9).toInt());  // Cart Type
      line.setGroupName(q->value(10).toString());       // Group Name
      line.setGroupColor(group_colors[q->value(10).toString()]);
      line.setTitle(q->value(11).toString());           // Title
      line.setArtist(q->value(12).toString());          // Artist
      line.setPublisher(q->value(45).toString());       // Publisher
      line.setComposer(q->value(46).toString());        // Composer
      line.setAlbum(q->value(13).toString());           // Album
      line.setYear(q->value(14).toDate());              // Year
      line.setLabel(q->value(15).toString());           // Label
      line.setClient(q->value(16).toString());          // Client
      line.setAgency(q->value(17).toString());          // Agency
      line.setUserDefined(q->value(18).toString());     // User Defined
      line.setCartNotes(q->value(65).toString());       // Cart Notes
      line.setForcedLength(q->value(21).toUInt());      // Forced Length
      line.setAverageSegueLength(q->value(21).toInt());
      if(!q->value(33).isNull()) {                      // Ext Start Time
	line.setExtStartTime(q->value(33).toTime());
      }
      if(!q->value(34).isNull()) {                      // Ext Length
	line.setExtLength(q->value(34).toInt());
      }
      if(!q->value(35).isNull()) {                      // Ext Data
	line.setExtData(q->value(35).toString());
      }
      if(!q->value(36).isNull()) {                      // Ext Event ID
	line.setExtEventId(q->value(36).toString());
      }
      if(!q->value(37).isNull()) {                      // Ext Annc. Type
	line.setExtAnncType(q->value(37).toString());
      }
      if(!q->value(38).isNull()) {                      // Ext Cart Name
	line.setExtCartName(q->value(38).toString());
      }
      if(!q->value(39).isNull()) {                      // Asyncronous
	line.setAsyncronous(RDBool(q->value(39).toString()));
      }
      break;

    case RDLogLine::Marker:
      break;

    case RDLogLine::Track:
      break;

    case RDLogLine::Chain:
      sql=
	QString().sprintf("select DESCRIPTION from LOGS where NAME=\"%s\"",
			  (const char *)line.markerLabel());
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
				(q->value(40).toInt()>=0));
    if(line.type()==RDLogLine::Cart) {
      prev_custom=(q->value(6).toInt()>=0)||(q->value(7).toInt()>=0)||
	(q->value(8).toInt()>=0)||(q->value(42).toInt()>=0);
    }
    else {
      prev_custom=false;
    }
/*    
    printf("LINE: %u  START: %d  END: %d  S_START: %d  S_END: %d  FD_UP: %d  FD_DN: %d\n",
	   log_line.size(),
	   q->value(5).toInt(),
	   q->value(6).toInt(),
	   q->value(7).toInt(),
	   q->value(8).toInt(),
	   q->value(38).toInt(),
	   q->value(40).toInt());
*/

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


void RDLogEvent::SaveLine(int line)
{
  QString sql;
  RDSqlQuery *q;
  sql=QString().sprintf("insert into `%s` set ID=%d,COUNT=%d,\
                         CART_NUMBER=%u,START_TIME=%d,TIME_TYPE=%d,\
                         TRANS_TYPE=%d,START_POINT=%d,END_POINT=%d,\
                         SEGUE_START_POINT=%d,SEGUE_END_POINT=%d,TYPE=%d,\
                         COMMENT=\"%s\",LABEL=\"%s\",GRACE_TIME=%d,\
                         SOURCE=%d,EXT_START_TIME=\"%s\",\
                         EXT_LENGTH=%d,EXT_DATA=\"%s\",EXT_EVENT_ID=\"%s\",\
                         EXT_ANNC_TYPE=\"%s\",EXT_CART_NAME=\"%s\",\
                         FADEUP_POINT=%d,FADEUP_GAIN=%d,FADEDOWN_POINT=%d,\
                         FADEDOWN_GAIN=%d,SEGUE_GAIN=%d,\
                         LINK_EVENT_NAME=\"%s\",LINK_START_TIME=%d,\
                         LINK_LENGTH=%d,LINK_ID=%d,LINK_EMBEDDED=\"%s\",\
                         ORIGIN_USER=\"%s\",ORIGIN_DATETIME=\"%s\",\
                         LINK_START_SLOP=%d,LINK_END_SLOP=%d,\
                         DUCK_UP_GAIN=%d,DUCK_DOWN_GAIN=%d,\
                         EVENT_LENGTH=%d",
			(const char *)log_name,
			log_line[line]->id(),
			line,
			log_line[line]->cartNumber(),
			QTime().msecsTo(log_line[line]->
					startTime(RDLogLine::Logged)),
			(int)log_line[line]->timeType(),
			(int)log_line[line]->transType(),
			log_line[line]->startPoint(RDLogLine::LogPointer),
			log_line[line]->endPoint(RDLogLine::LogPointer),
			log_line[line]->segueStartPoint(RDLogLine::LogPointer),
			log_line[line]->segueEndPoint(RDLogLine::LogPointer),
			log_line[line]->type(),
			(const char *)
			RDEscapeString(log_line[line]->markerComment()),
			(const char *)
			RDEscapeString(log_line[line]->markerLabel()),
			log_line[line]->graceTime(),
			log_line[line]->source(),
			(const char *)log_line[line]->extStartTime().
			toString("hh:mm:ss"),
			log_line[line]->extLength(),
			(const char *)RDEscapeString(log_line[line]->extData()),
			(const char *)
			RDEscapeString(log_line[line]->extEventId()),
			(const char *)
			RDEscapeString(log_line[line]->extAnncType()),
			(const char *)
			RDEscapeString(log_line[line]->extCartName()),
			log_line[line]->fadeupPoint(RDLogLine::LogPointer),
			log_line[line]->fadeupGain(),
			log_line[line]->fadedownPoint(RDLogLine::LogPointer),
			log_line[line]->fadedownGain(),
			log_line[line]->segueGain(),
			(const char *)
			RDEscapeString(log_line[line]->linkEventName()),
			QTime().msecsTo(log_line[line]->linkStartTime()),
			log_line[line]->linkLength(),
			log_line[line]->linkId(),
			(const char *)RDYesNo(log_line[line]->linkEmbedded()),
			(const char *)
			RDEscapeString(log_line[line]->originUser()),
			(const char *)log_line[line]->originDateTime().
			toString("yyyy-MM-dd hh:mm:ss"),
			log_line[line]->linkStartSlop(),
			log_line[line]->linkEndSlop(),
                        log_line[line]->duckUpGain(),
                        log_line[line]->duckDownGain(),
			log_line[line]->eventLength());

  // printf("SQL: %s\n",(const char *)sql);
  q=new RDSqlQuery(sql);
  delete q;
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

