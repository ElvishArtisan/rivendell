// rdevent_line.cpp
//
// Abstract a Rivendell Log Manager Event
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdevent_line.cpp,v 1.60.2.4.2.2 2014/06/24 18:27:04 cvs Exp $
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

#include <qtextstream.h>

#include <rdconf.h>
#include <rdcart.h>
#include <rdevent.h>
#include <rdevent_line.h>
#include <schedcartlist.h>
#include <rddb.h>
#include <rdescape_string.h>


RDEventLine::RDEventLine()
{
  event_preimport_log=new RDLogEvent();
  event_postimport_log=new RDLogEvent();
  clear();
}


QString RDEventLine::name() const
{
  return event_name;
}


void RDEventLine::setName(const QString &name)
{
  event_name=name;
  event_preimport_log->setLogName(RDEvent::preimportTableName(event_name));
  event_postimport_log->setLogName(RDEvent::postimportTableName(event_name));
}


QString RDEventLine::properties() const
{
  return event_properties;
}


void RDEventLine::setProperties(const QString &str)
{
  event_properties=str;
}


int RDEventLine::preposition() const
{
  return event_preposition;
}


void RDEventLine::setPreposition(int offset)
{
  event_preposition=offset;
}


RDLogLine::TimeType RDEventLine::timeType() const
{
  return event_time_type;
}


void RDEventLine::setTimeType(RDLogLine::TimeType type)
{
  event_time_type=type;
}


int RDEventLine::graceTime() const
{
  return event_grace_time;
}


void RDEventLine::setGraceTime(int offset)
{
  event_grace_time=offset;
}


bool RDEventLine::postPoint() const
{
  return event_post_point;
}


void RDEventLine::setPostPoint(bool state)
{
  event_post_point=state;
}


bool RDEventLine::useAutofill() const
{
  return event_use_autofill;
}


void RDEventLine::setUseAutofill(bool state)
{
  event_use_autofill=state;
}


int RDEventLine::autofillSlop() const
{
  return event_autofill_slop;
}


void RDEventLine::setAutofillSlop(int slop)
{
  event_autofill_slop=slop;
}


bool RDEventLine::useTimescale() const
{
  return event_use_timescale;
}


void RDEventLine::setUseTimescale(bool state)
{
  event_use_timescale=state;
}


RDEventLine::ImportSource RDEventLine::importSource() const
{
  return event_import_source;
}


void RDEventLine::setImportSource(RDEventLine::ImportSource src)
{
  event_import_source=src;
}


int RDEventLine::startSlop() const
{
  return event_start_slop;
}


void RDEventLine::setStartSlop(int slop)
{
  event_start_slop=slop;
}


int RDEventLine::endSlop() const
{
  return event_end_slop;
}


void RDEventLine::setEndSlop(int slop)
{
  event_end_slop=slop;
}


RDLogLine::TransType RDEventLine::firstTransType() const
{
  return event_first_transtype;
}


void RDEventLine::setFirstTransType(RDLogLine::TransType trans)
{
  event_first_transtype=trans;
}


RDLogLine::TransType RDEventLine::defaultTransType() const
{
  return event_default_transtype;
}


void RDEventLine::setDefaultTransType(RDLogLine::TransType trans)
{
  event_default_transtype=trans;
}


QColor RDEventLine::color() const
{
  return event_color;
}


void RDEventLine::setColor(const QColor &color)
{
  event_color=color;
}


QString RDEventLine::SchedGroup() const
{
  return event_sched_group;
}


void RDEventLine::setSchedGroup(QString str)
{
  event_sched_group=str;
}


QString RDEventLine::HaveCode() const
{
  return event_have_code;
}


void RDEventLine::setHaveCode(QString str)
{
  event_have_code=str;
}


unsigned RDEventLine::titleSep() const
{
  return event_title_sep;
}


void RDEventLine::setTitleSep(unsigned titlesep)
{
  event_title_sep=titlesep;
}



RDLogEvent *RDEventLine::preimportCarts()
{
  return event_preimport_log;
}


RDLogEvent *RDEventLine::postimportCarts()
{
  return event_postimport_log;
}


QTime RDEventLine::startTime() const
{
  return event_start_time;
}


void RDEventLine::setStartTime(const QTime &time)
{
  event_start_time=time;
}


int RDEventLine::length() const
{
  return event_length;
}


void RDEventLine::setLength(int msecs)
{
  event_length=msecs;
}


void RDEventLine::clear()
{
   event_name="";
   event_properties="";
   event_preposition=0;
   event_time_type=RDLogLine::Relative;
   event_grace_time=0;
   event_post_point=false;
   event_use_autofill=false;
   event_use_timescale=false;
   event_import_source=RDEventLine::None;
   event_start_slop=0;
   event_end_slop=0;
   event_first_transtype=RDLogLine::Segue;
   event_default_transtype=RDLogLine::Segue;
   event_color=QColor();
   event_preimport_log->clear();
   event_postimport_log->clear();
   event_start_time=QTime();
   event_length=0;
   event_autofill_slop=-1;
   event_sched_group="";
   event_have_code="";
   event_title_sep=100;
   event_nested_event="";
}


bool RDEventLine::load()
{
  QString sql=QString().sprintf("select PROPERTIES,PREPOSITION,TIME_TYPE,\
                                 GRACE_TIME,POST_POINT,USE_AUTOFILL,\
                                 USE_TIMESCALE,IMPORT_SOURCE,START_SLOP,\
                                 END_SLOP,FIRST_TRANS_TYPE,DEFAULT_TRANS_TYPE,\
                                 COLOR,AUTOFILL_SLOP,NESTED_EVENT,SCHED_GROUP,TITLE_SEP,HAVE_CODE \
                                 from EVENTS where NAME=\"%s\"",
				(const char *)event_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    fprintf(stderr,"RDEventLine::load() EVENT NOT FOUND: %s\n",
	    (const char *)event_name);
    delete q;
    return false;
  }
  event_properties=q->value(0).toString();
  event_preposition=q->value(1).toInt();
  event_time_type=(RDLogLine::TimeType)q->value(2).toInt();
  event_grace_time=q->value(3).toInt();
  event_post_point=RDBool(q->value(4).toString());
  event_use_autofill=RDBool(q->value(5).toString());
  event_use_timescale=RDBool(q->value(6).toString());
  event_import_source=(RDEventLine::ImportSource)q->value(7).toInt();
  event_start_slop=q->value(8).toInt();
  event_end_slop=q->value(9).toInt();
  event_first_transtype=(RDLogLine::TransType)q->value(10).toInt();
  event_default_transtype=(RDLogLine::TransType)q->value(11).toInt();
  if(q->value(12).isNull()) {
    event_color=QColor();
  }
  else {
    event_color=QColor(q->value(12).toString());
  }
  event_autofill_slop=q->value(13).toInt();
  event_nested_event=q->value(14).toString();
  event_sched_group=q->value(15).toString();
  event_title_sep=q->value(16).toUInt();
  event_have_code=q->value(17).toString();
  
  delete q;
  event_preimport_log->load();
  event_postimport_log->load();
  return true;
}


bool RDEventLine::save()
{
  QString sql=QString().sprintf("select NAME from EVENTS where NAME=\"%s\"",
				(const char *)event_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString().sprintf("update EVENTS set PROPERTIES=\"%s\",\
                           PREPOSITION=%d,TIME_TYPE=%d,\
                           GRACE_TIME=%d,POST_POINT=\"%s\",\
                           USE_AUTOFILL=\"%s\",USE_TIMESCALE=\"%s\",\
                           IMPORT_SOURCE=%d,START_SLOP=%d,\
                           END_SLOP=%d,FIRST_TRANS_TYPE=%d,\
                           DEFAULT_TRANS_TYPE=%d,COLOR=\"%s\"\
                           AUTOFILL_SLOP=%d,NESTED_EVENT=\"%s\",\
                           SCHED_GROUP=\"%s\",TITLE_SEP=%d,HAVE_CODE=\"%s\" \
                           where NAME=\"%s\"",
			  (const char *)RDEscapeString(event_properties),
			  event_preposition,event_time_type,
			  event_grace_time,
			  (const char *)RDYesNo(event_post_point),
			  (const char *)RDYesNo(event_use_autofill),
			  (const char *)RDYesNo(event_use_timescale),
			  event_import_source,event_start_slop,
			  event_end_slop,event_first_transtype,
			  event_default_transtype,
			  (const char *)event_color.name(),
			  event_autofill_slop,
			  (const char *)RDEscapeString(event_nested_event),
			  (const char *)RDEscapeString(event_sched_group),
			  event_title_sep,
			  (const char*)event_have_code,
			  (const char *)RDEscapeString(event_name));
  }
  else {
    sql=QString().sprintf("insert into EVENTS set NAME=\"%s\",\
                           PROPERTIES=\"%s\",\
                           PREPOSITION=%d,TIME_TYPE=%d,\
                           GRACE_TIME=%d,POST_POINT=\"%s\",\
                           USE_AUTOFILL=\"%s\",USE_TIMESCALE=\"%s\",\
                           IMPORT_SOURCE=%d,START_SLOP=%d,\
                           END_SLOP=%d,FIRST_TRANS_TYPE=%d,\
                           DEFAULT_TRANS_TYPE=%d,COLOR=\"%s\"\
                           AUTOFILL_SLOP=%d,SCHED_GROUP=\"%s\" where NAME=\"%s\"",
			  (const char *)RDEscapeString(event_name),
			  (const char *)RDEscapeString(event_properties),
			  event_preposition,event_time_type,
			  event_grace_time,
			  (const char *)RDYesNo(event_post_point),
			  (const char *)RDYesNo(event_use_autofill),
			  (const char *)RDYesNo(event_use_timescale),
			  event_import_source,event_start_slop,
			  event_end_slop,event_first_transtype,
			  event_default_transtype,
			  (const char *)event_color.name(),
			  event_autofill_slop,
			  (const char *)RDEscapeString(event_sched_group),
			  (const char *)RDEscapeString(event_name));
  }
  delete q;
  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    delete q;
    return false;
  }
  delete q;
  event_preimport_log->save();
  event_postimport_log->save();
  return true;
}


bool RDEventLine::generateLog(QString logname,const QString &svcname,
			      QString *errors, unsigned artistsep,
			      QString clockname)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QTime time=event_start_time;
  QTime fill_start_time;
  int count=0;
  logname.replace(" ","_");
  RDLogLine *logline;
  QString import_table;
  int postimport_length=0;
  RDLogLine::TransType trans_type=event_first_transtype;
  RDLogLine::TimeType time_type=event_time_type;
  RDLogLine::Type link_type=RDLogLine::MusicLink;
  bool post_point=event_post_point;
  int grace_time=event_grace_time;
  int link_id=0;

  //
  // Get Current Count and Link ID
  //
  sql=QString().sprintf("select COUNT from `%s_LOG` order by COUNT desc",
			(const char *)logname);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    count=q->value(0).toInt()+1;
  }
  delete q;

  sql=QString().sprintf("select LINK_ID from `%s_LOG` where LINK_ID>=0 \
                         order by LINK_ID desc",
			(const char *)logname);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    link_id=q->value(0).toInt()+1;
  }
  delete q;

  //
  // Override Default Parameters if Preposition Set
  //
  if(event_preposition>=0) {
    time_type=RDLogLine::Hard;
    grace_time=-1;
    if(QTime().msecsTo(time)>event_preposition) {
      time=time.addMSecs(-event_preposition);
    }
    else {
      time=QTime();
    }
  }

  //
  // Pre-Import Carts
  //
  for(int i=0;i<event_preimport_log->size();i++) {
    if((logline=event_preimport_log->logLine(i))!=NULL) {
      sql=QString().sprintf("insert into `%s_LOG` set ID=%d,COUNT=%d,TYPE=%d,\
                             SOURCE=%d,START_TIME=%d,GRACE_TIME=%d,\
                             CART_NUMBER=%u,TIME_TYPE=%d,POST_POINT=\"%s\",\
                             TRANS_TYPE=%d,COMMENT=\"%s\",EVENT_LENGTH=%d",
			    (const char *)logname,count,count,
			    logline->type(),
			    RDLogLine::Template,
			    QTime().msecsTo(time),
			    grace_time,logline->cartNumber(),
			    time_type,
			    (const char *)RDYesNo(post_point),
			    logline->transType(),
			    (const char *)
			    RDEscapeString(logline->markerComment()),
			    event_length);
      q=new RDSqlQuery(sql);
      delete q;
      count++;
      time=time.addMSecs(GetLength(logline->cartNumber()));
      trans_type=event_default_transtype;
      time_type=RDLogLine::Relative;
      post_point=false;
      grace_time=-1;
    }
  }

  //
  // Calculate Post Import Length
  //
  for(int i=0;i<event_postimport_log->size();i++) {
    postimport_length+=
      GetLength(event_postimport_log->logLine(i)->cartNumber());
  }

  //
  // Import Links
  //
  if(event_import_source==RDEventLine::Traffic || event_import_source==RDEventLine::Music) {
    switch(event_import_source) {
	case RDEventLine::Traffic:
	  link_type=RDLogLine::TrafficLink;
	  break;
	  
	case RDEventLine::Music:
	  link_type=RDLogLine::MusicLink;
	  break;
	  
	default:
	  break;
    }
    QTime end_start_time=event_start_time.addMSecs(event_length);

    sql=QString().sprintf("insert into `%s_LOG` set ID=%d,COUNT=%d,TYPE=%d,\
                           SOURCE=%d,START_TIME=%d,GRACE_TIME=%d,\
                           TIME_TYPE=%d,POST_POINT=\"%s\",TRANS_TYPE=%d,\
                           LINK_EVENT_NAME=\"%s\",LINK_START_TIME=%d,\
                           LINK_LENGTH=%d,LINK_ID=%d,LINK_START_SLOP=%d,\
                           LINK_END_SLOP=%d,EVENT_LENGTH=%d",
			  (const char *)logname,count,count,
			  link_type,RDLogLine::Template,
			  QTime().msecsTo(time),
			  grace_time,
			  time_type,
			  (const char *)RDYesNo(post_point),
			  trans_type,
			  (const char *)RDEscapeString(event_name),
			  QTime().msecsTo(event_start_time),
			  event_start_time.msecsTo(end_start_time),
			  link_id,
			  event_start_slop,
			  event_end_slop,
			  event_length);
    q=new RDSqlQuery(sql);
    delete q;
    count++;
    time=time.addMSecs(event_length);
    trans_type=event_default_transtype;
    time_type=RDLogLine::Relative;
    post_point=false;
    grace_time=-1;
  }

// Scheduler 

  if(event_import_source == RDEventLine::Scheduler ) {
    int titlesep;
    int stackid;
    int counter;   		
    RDLogLine::Source source=RDLogLine::Music;
    
    QString svcname_rp = svcname;
    svcname_rp.replace(" ","_");
    
    time.addMSecs(postimport_length);
    
    sql=QString().sprintf("select NUMBER,ARTIST,SCHED_CODES from CART where GROUP_NAME='%s'",(const char *)SchedGroup()); 
    
    q=new RDSqlQuery(sql);
    if(q->size()>0)
    {
      if(event_title_sep>=0 && event_title_sep<=50000)
      {
	titlesep = (int)event_title_sep;
      }
      else
      {
	titlesep = 100;
      }
      
      int querysize=(int)q->size();
      SchedCartList *schedCL;
      schedCL=new SchedCartList(querysize);
      
      for(counter=0;counter<querysize;counter++)
      {
	q->seek(counter);
	schedCL->insertItem(q->value(0).toUInt(),0,0,q->value(1).toString(),q->value(2).toString());
      }
      delete q;
      
      sql=QString().sprintf("SELECT SCHED_STACK_ID from %s_STACK order by SCHED_STACK_ID",(const char*)svcname_rp);
      q=new RDSqlQuery(sql);
      if (q->last())
      { 
	stackid=q->value(0).toUInt();
      }
      else
      { 
	stackid=0;
      }
      stackid++;    
      delete q;
      
      
      // Add deconflicting rules here		  
      // Title separation 
      schedCL->save();		  
      sql=QString().sprintf("select CART from %s_STACK \
			  where SCHED_STACK_ID >= %d",(const char*)svcname_rp,(stackid-titlesep));
      q=new RDSqlQuery(sql);
      
      while (q->next())	
      {
	for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	{ 
	  if(q->value(0).toUInt()==schedCL->getItemCartnumber(counter))
	  {
	    schedCL->removeItem(counter);
	    counter--;
	  }
	}
      }          
      delete q;
      if(schedCL->getNumberOfItems()==0)
	*errors+=QString().sprintf("%s Rule broken: Title Separation\n",(const char *)time.toString("hh:mm:ss"));
      schedCL->restore();
      
      // Artist separation
      schedCL->save();		  
      sql=QString().sprintf("select ARTIST from %s_STACK \
			  where SCHED_STACK_ID >= %d",(const char*)svcname_rp,(stackid-artistsep));
      q=new RDSqlQuery(sql);
      
      while (q->next())	
      {
	for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	{ 
	  if(q->value(0).toString()==schedCL->getItemArtist(counter))
	  {
	    schedCL->removeItem(counter);
	    counter--;
	  }
	}
      }          
      
      delete q;
      if(schedCL->getNumberOfItems()==0)
	*errors+=QString().sprintf("%s Rule broken: Artist Separation\n",(const char *)time.toString("hh:mm:ss"));
      schedCL->restore();
      
      // Must have scheduler code
      if(event_have_code!="")
      {
	schedCL->save();		  
	for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	{ 
	  if(!schedCL->itemHasCode(counter,event_have_code))
	  {
	    schedCL->removeItem(counter);
	    counter--;
	  }
	}
	if(schedCL->getNumberOfItems()==0)
	  *errors+=QString().sprintf("%s Rule broken: Must have code %s\n",(const char *)time.toString("hh:mm:ss"),(const char*)event_have_code);
	schedCL->restore();
      }
      
      // Scheduler Codes 
      sql=QString().sprintf("select CODE,MAX_ROW,MIN_WAIT,NOT_AFTER, OR_AFTER,OR_AFTER_II from %s_RULES",(const char *)clockname);
      q=new RDSqlQuery(sql);
      while (q->next())
      {
	// max in a row, min wait
	schedCL->save();	
	int range=q->value(1).toInt()+q->value(2).toInt(); 
	int allowed=q->value(1).toInt();
	QString wstr=q->value(0).toString();
	wstr+="          ";
	wstr=wstr.left(11);
	sql=QString().sprintf("select CART from %s_STACK \
			  where SCHED_STACK_ID > %d and SCHED_CODES like \"%%%s%%\"",(const char*)svcname_rp,(stackid-range),(const char *)wstr);
	q1=new RDSqlQuery(sql);
	if (q1->size()>=allowed || allowed==0)	
	  for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	    if (                           schedCL->removeIfCode(counter,q->value(0).toString()))
	      counter--;
	delete q1;
	if(schedCL->getNumberOfItems()==0)
	  *errors+=QString().sprintf("%s Rule broken: Max. in a Row/Min. Wait for %s\n",(const char *)time.toString("hh:mm:ss"),(const char *)q->value(0).toString());
	schedCL->restore();
	// do not play after
	if (q->value(3).toString()!="")
	{ 
	  schedCL->save();	
	  QString wstr=q->value(3).toString();
	  wstr+="          ";
	  wstr=wstr.left(11);
	  sql=QString().sprintf("select CART from %s_STACK \
			  where SCHED_STACK_ID = %d and SCHED_CODES like \"%%%s%%\"",(const char*)svcname_rp,(stackid-1),(const char *)wstr);
	  q1=new RDSqlQuery(sql);
	  if (q1->size()>0)	
	    for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	      if (                           schedCL->removeIfCode(counter,q->value(0).toString()))
		counter--;
	  delete q1;
	  if(schedCL->getNumberOfItems()==0)
	    *errors+=QString().sprintf("%s Rule broken: Do not schedule %s after %s\n",(const char *)time.toString("hh:mm:ss"),(const char *)q->value(0).toString(),(const char *)q->value(3).toString());
	  schedCL->restore();
	}
	// or after
	if (q->value(4).toString()!="")
	{ 
	  schedCL->save();
	  QString wstr=q->value(4).toString();
	  wstr+="          ";
	  wstr=wstr.left(11);
	  sql=QString().sprintf("select CART from %s_STACK \
			  where SCHED_STACK_ID = %d and SCHED_CODES like \"%%%s%%\"",(const char*)svcname_rp,(stackid-1),(const char *)wstr);
	  q1=new RDSqlQuery(sql);
	  if (q1->size()>0)	
	    for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	      if (                           schedCL->removeIfCode(counter,q->value(0).toString()))
		counter--;
	  delete q1;
	  if(schedCL->getNumberOfItems()==0)
	    *errors+=QString().sprintf("%s Rule broken: Do not schedule %s after %s\n",(const char *)time.toString("hh:mm:ss"),(const char *)q->value(0).toString(),(const char *)q->value(4).toString());
	  schedCL->restore();
	}
	// or after II
	if (q->value(5).toString()!="")
	{ 
	  schedCL->save();
	  QString wstr=q->value(5).toString();
	  wstr+="          ";
	  wstr=wstr.left(11);
	  sql=QString().sprintf("select CART from %s_STACK \
			  where SCHED_STACK_ID = %d and SCHED_CODES like \"%%%s%%\"",(const char*)svcname_rp,(stackid-1),(const char *)wstr);
	  q1=new RDSqlQuery(sql);
	  if (q1->size()>0)	
	    for(counter=0;counter<schedCL->getNumberOfItems();counter++)
	      if (                           schedCL->removeIfCode(counter,q->value(0).toString()))
		counter--;
	  delete q1;
	  if(schedCL->getNumberOfItems()==0)
	    *errors+=QString().sprintf("%s Rule broken: Do not schedule %s after %s\n",(const char *)time.toString("hh:mm:ss"),(const char *)q->value(0).toString(),(const char *)q->value(5).toString());
	  schedCL->restore();
	}
      }
      delete q;
      
      
// end of deconflicting rules
      
      int schedpos = rand()%schedCL->getNumberOfItems();
      sql=QString().sprintf("insert into `%s_LOG` set ID=%d,COUNT=%d,TYPE=%d,\
			     SOURCE=%d,START_TIME=%d,GRACE_TIME=%d, \
			     CART_NUMBER=%u,TIME_TYPE=%d,POST_POINT=\"%s\", \
			     TRANS_TYPE=%d,EXT_START_TIME=\"%s\",\
                             EVENT_LENGTH=%d",
			    (const char *)logname,count,count,
			    RDLogLine::Cart,source,
			    QTime().msecsTo(time),
			    grace_time,
			    schedCL->getItemCartnumber(schedpos),
			    time_type,
			    (const char *)RDYesNo(post_point),
			    trans_type,
			    (const char *)time.toString("hh:mm:ss"),
			    event_length);
      q=new RDSqlQuery(sql);
      delete q;



      count++;



      sql=QString().sprintf("insert into `%s_STACK` set SCHED_STACK_ID=%u,CART=%u,ARTIST=\"%s\",SCHED_CODES=\"%s\"",(const char*)svcname_rp,
			    stackid,schedCL->getItemCartnumber(schedpos),
			    (const char *)RDEscapeString(schedCL->getItemArtist(schedpos)),(const char *)schedCL->getItemSchedCodes(schedpos));
      q=new RDSqlQuery(sql);
      delete q;
      delete schedCL;
    }
    else
    {
      delete q;
    }
  }
  
  
  //
  // Post-Import Carts
  //
  for(int i=0;i<event_postimport_log->size();i++) {
    if((logline=event_postimport_log->logLine(i))!=NULL) {
      sql=QString().sprintf("insert into `%s_LOG` set ID=%d,COUNT=%d,TYPE=%d,\
                             SOURCE=%d,START_TIME=%d,GRACE_TIME=%d,\
                             CART_NUMBER=%u,TIME_TYPE=%d,POST_POINT=\"%s\",\
                             TRANS_TYPE=%d,COMMENT=\"%s\",EVENT_LENGTH=%d",
			    (const char *)logname,count,count,
			    logline->type(),
			    RDLogLine::Template,
			    QTime().msecsTo(time),
			    grace_time,logline->cartNumber(),
			    time_type,
			    (const char *)RDYesNo(post_point),
			    logline->transType(),
			    (const char *)
			    RDEscapeString(logline->markerComment()),
			    event_length);
      q=new RDSqlQuery(sql);
      delete q;
      count++;
      time=time.addMSecs(GetLength(logline->cartNumber()));
      time_type=RDLogLine::Relative;
      trans_type=event_default_transtype;
      post_point=false;
      grace_time=-1;
    }
  }

  return true;
}


bool RDEventLine::linkLog(RDLogEvent *e,int next_id,const QString &svcname,
			  RDLogLine *link_logline,const QString &track_str,
			  const QString &label_cart,const QString &track_cart,
			  const QString &import_table,QString *errors)
{
  QString sql;
  RDSqlQuery *q;
  RDLogLine *logline=NULL;

  //
  // Initial Import Parameters
  //
  RDLogLine::Source event_src=RDLogLine::Manual;
  switch(event_import_source) {
  case RDEventLine::Music:
    event_src=RDLogLine::Music;
    break;

  case RDEventLine::Traffic:
    event_src=RDLogLine::Traffic;
    break;

  case RDEventLine::Scheduler:
  case RDEventLine::None:
    break;
  }
  RDLogLine::TimeType time_type=link_logline->timeType();
  RDLogLine::TransType trans_type=link_logline->transType();
  int grace_time=link_logline->graceTime();
  QTime time=link_logline->startTime(RDLogLine::Logged);

  //
  // Calculate Event Time Boundaries
  //
  int start_start_hour=link_logline->linkStartTime().hour();
  int start_start_secs=60000*link_logline->linkStartTime().minute()+
    1000*link_logline->linkStartTime().second();
  int end_start_secs=start_start_secs+link_logline->linkLength();

  //
  // Apply Slop Factors
  //
  if(start_start_hour==link_logline->linkStartTime().
     addMSecs(-link_logline->linkStartSlop()).hour()) {
    start_start_secs-=link_logline->linkStartSlop();
  }
  else {
    start_start_secs=0;  // So we don't slop over into the previous hour
  }
  end_start_secs+=link_logline->linkEndSlop();

  //
  // Load Matching Events and Insert into Log
  //
  int id=-1;
  sql=QString().sprintf("select CART_NUMBER,START_SECS,LENGTH,EXT_DATA,\
                         EXT_EVENT_ID,EXT_ANNC_TYPE,EXT_CART_NAME,\
                         INSERT_BREAK,INSERT_TRACK,INSERT_FIRST,TITLE,\
                         TRACK_STRING from `%s` where (START_HOUR=%d)&&\
                         (START_SECS>=%d)&&(START_SECS<=%d)&&\
                         (EVENT_USED=\"N\") order by ID",
			(const char *)import_table,
			start_start_hour,start_start_secs/1000,
			end_start_secs/1000);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if((id=e->nextId())>next_id) {
      next_id=id;
    }
    int length=GetLength(q->value(0).toUInt(),q->value(2).toInt());
    if(q->value(9).toUInt()==RDEventLine::InsertBreak) {
      if(q->value(7).toString()=="Y") {  // Insert Break
	if((!event_nested_event.isEmpty()&&(event_nested_event!=event_name))) {
	  e->insert(e->size(),1);
	  logline=e->logLine(e->size()-1);
	  logline->setId(next_id++);
	  logline->setStartTime(RDLogLine::Logged,time);
	  logline->setType(RDLogLine::TrafficLink);
	  logline->setSource(event_src);
	  logline->setTransType(trans_type);
	  logline->setEventLength(event_length);
	  logline->setLinkEventName(event_nested_event);
	  logline->setLinkStartTime(link_logline->linkStartTime());
	  logline->setLinkLength(link_logline->linkLength());
	  logline->setLinkStartSlop(link_logline->linkStartSlop());
	  logline->setLinkEndSlop(link_logline->linkEndSlop());
	  logline->setLinkId(link_logline->linkId());
	  logline->setLinkEmbedded(true);
	}
      }
      if(q->value(8).toString()=="Y") {  // Insert Track
	e->insert(e->size(),1);
	logline=e->logLine(e->size()-1);
	logline->setId(next_id++);
	logline->setStartTime(RDLogLine::Logged,time);
	logline->setType(RDLogLine::Track);
	logline->setSource(event_src);
	logline->setTransType(RDLogLine::Segue);
	logline->setMarkerComment(q->value(11).toString());
	logline->setEventLength(event_length);
	logline->setLinkEventName(event_name);
	logline->setLinkStartTime(link_logline->linkStartTime());
	logline->setLinkLength(link_logline->linkLength());
	logline->setLinkStartSlop(link_logline->linkStartSlop());
	logline->setLinkEndSlop(link_logline->linkEndSlop());
	logline->setLinkId(link_logline->linkId());
	logline->setLinkEmbedded(true);
      }
    }
    else {
      if(q->value(8).toString()=="Y") {  // Insert Track
	e->insert(e->size(),1);
	logline=e->logLine(e->size()-1);
	logline->setId(next_id++);
	logline->setStartTime(RDLogLine::Logged,time);
	logline->setType(RDLogLine::Track);
	logline->setSource(event_src);
	logline->setTransType(RDLogLine::Segue);
	logline->setMarkerComment(q->value(11).toString());
        logline->setEventLength(event_length);
	logline->setLinkEventName(event_name);
	logline->setLinkStartTime(link_logline->linkStartTime());
	logline->setLinkLength(link_logline->linkLength());
	logline->setLinkStartSlop(link_logline->linkStartSlop());
	logline->setLinkEndSlop(link_logline->linkEndSlop());
	logline->setLinkId(link_logline->linkId());
	logline->setLinkEmbedded(true);
      }
      if(q->value(7).toString()=="Y") {  // Insert Break
	if((!event_nested_event.isEmpty()&&(event_nested_event!=event_name))) {
	  e->insert(e->size(),1);
	  logline=e->logLine(e->size()-1);
	  logline->setId(next_id++);
	  logline->setStartTime(RDLogLine::Logged,time);
	  logline->setType(RDLogLine::TrafficLink);
	  logline->setSource(event_src);
	  logline->setTransType(trans_type);
	  logline->setEventLength(event_length);
	  logline->setLinkEventName(event_nested_event);
	  logline->setLinkStartTime(link_logline->linkStartTime());
	  logline->setLinkLength(link_logline->linkLength());
	  logline->setLinkStartSlop(link_logline->linkStartSlop());
	  logline->setLinkEndSlop(link_logline->linkEndSlop());
	  logline->setLinkId(link_logline->linkId());
	  logline->setLinkEmbedded(true);
	}
      }
    }

    e->insert(e->size(),1);
    logline=e->logLine(e->size()-1);
    logline->setId(next_id++);
    logline->setSource(event_src);
    logline->setStartTime(RDLogLine::Logged,time);
    logline->setGraceTime(grace_time);
    logline->setTimeType(time_type);
    logline->setTransType(trans_type);
    logline->setExtStartTime(QTime().addSecs(3600*start_start_hour+
					     q->value(1).toInt()));
    logline->setExtLength(q->value(2).toInt());
    logline->setExtData(q->value(3).toString());
    logline->setExtEventId(q->value(4).toString());
    logline->setExtAnncType(q->value(5).toString());
    logline->setExtCartName(q->value(6).toString());
    logline->setEventLength(event_length);
    logline->setLinkEventName(event_name);
    logline->setLinkStartTime(link_logline->linkStartTime());
    logline->setLinkLength(link_logline->linkLength());
    logline->setLinkStartSlop(link_logline->linkStartSlop());
    logline->setLinkEndSlop(link_logline->linkEndSlop());
    logline->setLinkId(link_logline->linkId());
    logline->setLinkEmbedded(link_logline->linkEmbedded());
    if((q->value(6).toString()==label_cart)&&(!label_cart.isEmpty())) {
      logline->setType(RDLogLine::Marker);
      logline->setMarkerComment(q->value(10).toString());
      logline->setCartNumber(0);
    }
    else {
      if((q->value(6).toString()==track_cart)&&(!track_cart.isEmpty())) {
	logline->setType(RDLogLine::Track);
	logline->setMarkerComment(q->value(10).toString());
	logline->setCartNumber(0);
      }
      else {
	logline->setType(RDLogLine::Cart);
	logline->setCartNumber(q->value(0).toUInt());
      }
    }

    //
    // Clear Leading Event Values
    //
    time_type=RDLogLine::Relative;
    trans_type=event_default_transtype;
    time=time.addMSecs(length);
    grace_time=-1;

  }
  delete q;

  //
  // Mark Events as Used
  //
  sql=QString().sprintf("update `%s` set EVENT_USED=\"Y\"\
                         where (START_HOUR=%d)&&(START_SECS>=%d)&&\
                         (START_SECS<=%d)&&(EVENT_USED=\"N\")",
			(const char *)import_table,
			start_start_hour,start_start_secs/1000,
			end_start_secs/1000);
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Autofill
  //
  QTime end_time=link_logline->startTime(RDLogLine::Logged).
    addMSecs(link_logline->linkLength());
  if(event_use_autofill&&(event_start_time<=time)) {
    QTime fill_start_time=time;
    sql=QString().
      sprintf("select AUTOFILLS.CART_NUMBER,CART.FORCED_LENGTH from\
               AUTOFILLS left join CART on AUTOFILLS.CART_NUMBER=CART.NUMBER\
               where (AUTOFILLS.SERVICE=\"%s\")&&(CART.FORCED_LENGTH<=%d)&&\
               (CART.FORCED_LENGTH>0) order by CART.FORCED_LENGTH desc",
	      (const char *)svcname,time.msecsTo(end_time));
    q=new RDSqlQuery(sql);
    bool fit=true;
    while(fit) {
      fit=false;
      while(q->next()&&(fill_start_time<=time)) {
	if((time.addMSecs(q->value(1).toInt())<=end_time)&&
	   (time.addMSecs(q->value(1).toInt())>time)) {
	  e->insert(e->size(),1);
	  logline=e->logLine(e->size()-1);
	  logline->setId(next_id++);
	  logline->setStartTime(RDLogLine::Logged,time);
	  logline->setType(RDLogLine::Cart);
	  logline->setSource(event_src);
	  logline->setTransType(trans_type);
	  logline->setGraceTime(grace_time);
	  logline->setCartNumber(q->value(0).toUInt());
	  logline->setTimeType(time_type);
	  logline->setEventLength(event_length);
	  logline->setLinkEventName(event_name);
	  logline->setLinkStartTime(link_logline->linkStartTime());
	  logline->setLinkLength(link_logline->linkLength());
	  logline->setLinkStartSlop(link_logline->linkStartSlop());
	  logline->setLinkEndSlop(link_logline->linkEndSlop());
	  logline->setLinkId(link_logline->linkId());
	  logline->setLinkEmbedded(false);
	  time=time.addMSecs(q->value(1).toInt());
	  time_type=RDLogLine::Relative;
	  trans_type=event_default_transtype;
	  grace_time=-1;
	  q->seek(-1);
	  fit=true;
	}
      }
    }
    delete q;
  }

  //
  // Fill Check
  //
  if(event_autofill_slop>=0) {
    int slop=QTime().msecsTo(end_time)-QTime().msecsTo(time);
    if(abs(slop)>=event_autofill_slop) {
      if(slop>0) {
	*errors+=QString().
	  sprintf("  %s -- \"%s\" is underscheduled by %s.\n",
		  (const char *)time.toString("hh:mm:ss"),
		  (const char *)event_name,
		  (const char *)QTime().addMSecs(slop).
		  toString("hh:mm:ss"));
      }
      else {
	*errors+=QString().
	  sprintf("  %s -- \"%s\" is overscheduled by %s.\n",
		  (const char *)time.toString("hh:mm:ss"),
		  (const char *)event_name,
		  (const char *)QTime().addMSecs(-slop).
		  toString("hh:mm:ss"));
      }
    }
  }
  
  return false;
}


int RDEventLine::GetLength(unsigned cartnum,int def_length)
{
  RDCart *cart=new RDCart(cartnum);
  if(!cart->exists()) {
    delete cart;
    return def_length;
  }
  int length=cart->forcedLength();
  delete cart;
  return length;
}
