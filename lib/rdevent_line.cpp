// rdevent_line.cpp
//
// Abstract a Rivendell Log Manager Event
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

#include <qobject.h>
#include <q3textstream.h>

#include "rdconf.h"
#include "rdcart.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdevent.h"
#include "rdevent_line.h"
#include "rdeventimportlist.h"
#include "rdschedcartlist.h"

RDEventLine::RDEventLine(RDStation *station)
{
  event_station=station;
  event_preimport_list=new RDEventImportList();
  event_postimport_list=new RDEventImportList();
  clear();
}


QString RDEventLine::name() const
{
  return event_name;
}


void RDEventLine::setName(const QString &name)
{
  event_name=name;
  event_preimport_list->setEventName(event_name);
  event_preimport_list->setType(RDEventImportList::PreImport);
  event_postimport_list->setEventName(event_name);
  event_postimport_list->setType(RDEventImportList::PostImport);
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


QString RDEventLine::schedGroup() const
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

QString RDEventLine::HaveCode2() const
{
  return event_have_code2;
}


void RDEventLine::setHaveCode2(QString str)
{
  event_have_code2=str;
}

unsigned RDEventLine::titleSep() const
{
  return event_title_sep;
}


void RDEventLine::setTitleSep(unsigned titlesep)
{
  event_title_sep=titlesep;
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
   event_preposition=0;
   event_time_type=RDLogLine::Relative;
   event_grace_time=0;
   event_use_autofill=false;
   event_use_timescale=false;
   event_import_source=RDEventLine::None;
   event_start_slop=0;
   event_end_slop=0;
   event_first_transtype=RDLogLine::Segue;
   event_default_transtype=RDLogLine::Segue;
   event_color=QColor();
   event_preimport_list->clear();
   event_postimport_list->clear();
   event_start_time=QTime(0,0,0,0);
   event_length=0;
   event_autofill_slop=-1;
   event_sched_group="";
   event_have_code="";
   event_have_code2="";
   event_artist_sep=15;
   event_title_sep=100;
   event_nested_event="";
}


bool RDEventLine::load()
{
  QString sql=QString("select ")+
    "PREPOSITION,"+         // 00
    "TIME_TYPE,"+           // 01
    "GRACE_TIME,"+          // 02
    "USE_AUTOFILL,"+        // 03
    "USE_TIMESCALE,"+       // 04
    "IMPORT_SOURCE,"+       // 05
    "START_SLOP,"+          // 06
    "END_SLOP,"+            // 07
    "FIRST_TRANS_TYPE,"+    // 08
    "DEFAULT_TRANS_TYPE,"+  // 09
    "COLOR,"+               // 10
    "AUTOFILL_SLOP,"+       // 11
    "NESTED_EVENT,"+        // 12
    "SCHED_GROUP,"+         // 13
    "ARTIST_SEP,"+          // 14
    "TITLE_SEP,"+           // 15
    "HAVE_CODE,"+           // 16
    "HAVE_CODE2	"+          // 17
    "from EVENTS where "+
    "NAME=\""+RDEscapeString(event_name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    fprintf(stderr,"RDEventLine::load() EVENT NOT FOUND: %s\n",
	    (const char *)event_name);
    delete q;
    return false;
  }
  event_preposition=q->value(0).toInt();
  event_time_type=(RDLogLine::TimeType)q->value(1).toInt();
  event_grace_time=q->value(2).toInt();
  event_use_autofill=RDBool(q->value(3).toString());
  event_use_timescale=RDBool(q->value(4).toString());
  event_import_source=(RDEventLine::ImportSource)q->value(5).toInt();
  event_start_slop=q->value(6).toInt();
  event_end_slop=q->value(7).toInt();
  event_first_transtype=(RDLogLine::TransType)q->value(8).toInt();
  event_default_transtype=(RDLogLine::TransType)q->value(9).toInt();
  if(q->value(10).isNull()) {
    event_color=QColor();
  }
  else {
    event_color=QColor(q->value(10).toString());
  }
  event_autofill_slop=q->value(11).toInt();
  event_nested_event=q->value(12).toString();
  event_sched_group=q->value(13).toString();
  event_artist_sep=q->value(14).toInt();
  event_title_sep=q->value(15).toInt();
  event_have_code=q->value(16).toString();
  event_have_code2=q->value(17).toString();
  delete q;

  event_preimport_list->load();
  event_postimport_list->load();
  return true;
}


bool RDEventLine::save(RDConfig *config)
{
  QString sql=QString("select NAME from EVENTS where ")+
    "NAME=\""+RDEscapeString(event_name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("update EVENTS set ")+
      QString().sprintf("PREPOSITION=%d,",event_preposition)+
      QString().sprintf("TIME_TYPE=%d,",event_time_type)+
      QString().sprintf("GRACE_TIME=%d,",event_grace_time)+
      "USE_AUTOFILL=\""+RDYesNo(event_use_autofill)+"\","+
      "USE_TIMESCALE=\""+RDYesNo(event_use_timescale)+"\","+
      QString().sprintf("IMPORT_SOURCE=%d,",event_import_source)+
      QString().sprintf("START_SLOP=%d,",event_start_slop)+
      QString().sprintf("END_SLOP=%d,",event_end_slop)+
      QString().sprintf("FIRST_TRANS_TYPE=%d,",event_first_transtype)+
      QString().sprintf("DEFAULT_TRANS_TYPE=%d,",event_default_transtype)+
      "COLOR=\""+RDEscapeString(event_color.name())+"\""+
      QString().sprintf("AUTOFILL_SLOP=%d,",event_autofill_slop)+
      "NESTED_EVENT=\""+RDEscapeString(event_nested_event)+"\","+
      "SCHED_GROUP=\""+RDEscapeString(event_sched_group)+"\","+
      QString().sprintf("ARTIST_SEP=%d,",event_artist_sep)+
      QString().sprintf("TITLE_SEP=%d,",event_title_sep)+
      "HAVE_CODE=\""+RDEscapeString(event_have_code)+"\","+
      "HAVE_CODE2=\""+RDEscapeString(event_have_code2)+"\" "+
      "where NAME=\""+RDEscapeString(event_name)+"\"";
  }
  else {
    sql=QString("insert into EVENTS set ")+
      "NAME=\""+RDEscapeString(event_name)+"\","+
      QString().sprintf("PREPOSITION=%d,",event_preposition)+
      QString().sprintf("TIME_TYPE=%d,",event_time_type)+
      QString().sprintf("GRACE_TIME=%d,",event_grace_time)+
      "USE_AUTOFILL=\""+RDYesNo(event_use_autofill)+"\","+
      "USE_TIMESCALE=\""+RDYesNo(event_use_timescale)+"\","+
      QString().sprintf("IMPORT_SOURCE=%d,",event_import_source)+
      QString().sprintf("START_SLOP=%d,",event_start_slop)+
      QString().sprintf("END_SLOP=%d,",event_end_slop)+
      QString().sprintf("FIRST_TRANS_TYPE=%d,",event_first_transtype)+
      QString().sprintf("DEFAULT_TRANS_TYPE=%d,",event_default_transtype)+
      "COLOR=\""+RDEscapeString(event_color.name())+"\","+
      QString().sprintf("AUTOFILL_SLOP=%d,",event_autofill_slop)+
      "SCHED_GROUP=\""+RDEscapeString(event_sched_group)+"\" where "+
      "NAME=\""+RDEscapeString(event_name)+"\"";
  }
  delete q;
  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    delete q;
    return false;
  }
  delete q;
  event_preimport_list->save();
  event_postimport_list->save();
  return true;
}


bool RDEventLine::generateLog(QString logname,const QString &svcname,
			      QString *report, QString clockname)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QTime time=event_start_time;
  QTime fill_start_time;
  int count=0;
  QString import_table;
  int postimport_length=0;
  RDLogLine::TransType trans_type=event_first_transtype;
  RDLogLine::TimeType time_type=event_time_type;
  RDLogLine::Type link_type=RDLogLine::MusicLink;
  int grace_time=event_grace_time;
  int link_id=0;

  //
  // Get Current Count and Link ID
  //
  sql=QString("select COUNT from LOG_LINES where ")+
    "LOG_NAME=\""+RDEscapeString(logname)+"\" "+
    "order by COUNT desc";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    count=q->value(0).toInt()+1;
  }
  delete q;

  sql=QString("select LINK_ID from LOG_LINES where ")+
    "LOG_NAME=\""+RDEscapeString(logname)+"\" && "+
    "LINK_ID>=0 "+
    "order by LINK_ID desc";
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
  postimport_length=0;
  for(int i=0;i<event_preimport_list->size()-1;i++) {
    RDEventImportItem *i_item=event_preimport_list->item(i);
    sql=QString("insert into LOG_LINES set ")+
      "LOG_NAME=\""+RDEscapeString(logname)+"\","+
      QString().sprintf("LINE_ID=%d,",count)+
      QString().sprintf("COUNT=%d,",count)+
      QString().sprintf("TYPE=%d,",i_item->eventType())+
      QString().sprintf("SOURCE=%d,",RDLogLine::Template)+
      QString().sprintf("START_TIME=%d,",QTime().msecsTo(time))+
      QString().sprintf("GRACE_TIME=%d,",grace_time)+
      QString().sprintf("CART_NUMBER=%u,",i_item->cartNumber())+
      QString().sprintf("TIME_TYPE=%d,",time_type)+
      QString().sprintf("TRANS_TYPE=%d,",i_item->transType())+
      "COMMENT=\""+RDEscapeString(i_item->markerComment())+"\","+
      QString().sprintf("EVENT_LENGTH=%d",event_length);
    RDSqlQuery::apply(sql);
    count++;
    trans_type=event_default_transtype;
    time_type=RDLogLine::Relative;
    grace_time=-1;

    postimport_length+=GetLength(i_item->cartNumber());
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

    sql=QString("insert into LOG_LINES set ")+
      "LOG_NAME=\""+RDEscapeString(logname)+"\","+
      QString().sprintf("LINE_ID=%d,",count)+
      QString().sprintf("COUNT=%d,",count)+
      QString().sprintf("TYPE=%d,",link_type)+
      QString().sprintf("SOURCE=%d,",RDLogLine::Template)+
      QString().sprintf("START_TIME=%d,",QTime().msecsTo(time))+
      QString().sprintf("GRACE_TIME=%d,",grace_time)+
      QString().sprintf("TIME_TYPE=%d,",time_type)+
      QString().sprintf("TRANS_TYPE=%d,",trans_type)+
      "LINK_EVENT_NAME=\""+RDEscapeString(event_name)+"\","+
      QString().sprintf("LINK_START_TIME=%d,",
			QTime().msecsTo(event_start_time))+
      QString().sprintf("LINK_LENGTH=%d,",
			event_start_time.msecsTo(end_start_time))+
      QString().sprintf("LINK_ID=%d,",link_id)+
      QString().sprintf("LINK_START_SLOP=%d,",event_start_slop)+
      QString().sprintf("LINK_END_SLOP=%d,",event_end_slop)+
      QString().sprintf("EVENT_LENGTH=%d",event_length);
    q=new RDSqlQuery(sql);
    delete q;
    count++;
    time=time.addMSecs(event_length);
    trans_type=event_default_transtype;
    time_type=RDLogLine::Relative;
    grace_time=-1;
  }

  // Scheduler 

  if(event_import_source == RDEventLine::Scheduler) {
    int artistsep;
    int titlesep;
    int stackid;
    int counter;   		
    RDLogLine::Source source=RDLogLine::Music;
    
    QString svcname_rp = svcname;
    svcname_rp.replace(" ","_");
    
    time.addMSecs(postimport_length);

    if(event_artist_sep>=-1 && event_artist_sep<=50000) {
      artistsep = event_artist_sep;
    }
    else {
      artistsep = 15;
    }

    if(event_title_sep>=-1 && event_title_sep<=50000) {
      titlesep = event_title_sep;
    }
    else {
      titlesep = 100;
    }

    //
    // Get next stack id from the stack
    //
    sql=QString("select ")+
      "MAX(SCHED_STACK_ID) "+
      "from STACK_LINES where "+
      "SERVICE_NAME=\""+RDEscapeString(svcname)+"\"";
    q=new RDSqlQuery(sql);
    if (q->next()) { 
      stackid=q->value(0).toUInt();
    }
    else { 
      stackid=0;
    }
    stackid++;    
    delete q;
      
    //
    // Load all carts in requested group into schedCL
    //
    sql=QString("select NUMBER,ARTIST,")+
      "CONCAT(GROUP_CONCAT(RPAD(SC.SCHED_CODE,11,' ') separator ''),'.') as SCHED_CODES"+
      " from CART LEFT JOIN CART_SCHED_CODES AS SC on (NUMBER=SC.CART_NUMBER)"+
      " where GROUP_NAME='"+RDEscapeString(schedGroup())+"'"+
      " group by NUMBER";
    RDSchedCartList *schedCL=new RDSchedCartList();
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QStringList codes=q->value(2).toString().split(" ",QString::SkipEmptyParts);
      if((codes.size()>0)&&(codes.last()==".")) {
	codes.removeLast();
      }
      schedCL->
	insertItem(q->value(0).toUInt(),0,0,q->value(1).toString(),codes);
    }
    delete q;
      
    //////////////////////////////////
    //                              //
    // Add deconflicting rules here //
    //                              //
    //////////////////////////////////

    // Reduce schedCL to match requested scheduler code
    if(event_have_code!=""||event_have_code2!="") {
      QStringList codes;
      if(event_have_code!="") {
        codes << event_have_code;
      }
      if(event_have_code2!="") {
        codes << event_have_code2;
      }
      for(counter=0;counter<schedCL->getNumberOfItems();counter++) { 
        if(!schedCL->itemHasCodes(counter,codes)) {
          schedCL->removeItem(counter);
          counter--;
        }
      }
    }

    if(schedCL->getNumberOfItems()) {
      //
      // Title separation
      //
      // Iterate through schedCL and remove carts from schedCL that
      // match cart number on the stack essentially removing matched titles.
      //
      if(titlesep>=0) {
        schedCL->save();		  
        sql=QString("select CART from STACK_LINES where ")+
          "SERVICE_NAME=\""+RDEscapeString(svcname)+"\" && "+
          QString().sprintf("SCHED_STACK_ID >= %d",stackid-titlesep);
        q=new RDSqlQuery(sql);
        while (q->next()) {
          for(counter=0;counter<schedCL->getNumberOfItems();counter++) { 
            if(q->value(0).toUInt()==schedCL->getItemCartNumber(counter)) {
              schedCL->removeItem(counter);
              counter--;
            }
          }
        }
        delete q;
        if(schedCL->getNumberOfItems()==0) {
          *report+=time.toString("hh:mm:ss")+" "+
            QObject::tr("Rule broken: Title separation");
          if(!HaveCode().isEmpty()) {
            *report+=QObject::tr(" with sched code(s): ")+HaveCode()+" "+HaveCode2();
          }
          *report+="\n";
          schedCL->restore();
        }
      }
      
      //
      // Artist separation
      //
      // Iterate through schedCL and remove carts from schedCL that
      // match artist on the stack.
      //
      if(artistsep>=0) {
        schedCL->save();		  
        sql=QString("select ARTIST from STACK_LINES where ")+
          "SERVICE_NAME=\""+RDEscapeString(svcname)+"\" && "+
          QString().sprintf("SCHED_STACK_ID >= %d",stackid-artistsep);
        q=new RDSqlQuery(sql);
        while (q->next()) {
          for(counter=0;counter<schedCL->getNumberOfItems();counter++) { 
            if(q->value(0).toString()==schedCL->getItemArtist(counter)) {
              schedCL->removeItem(counter);
              counter--;
            }
          }
        }          
        delete q;
        if(schedCL->getNumberOfItems()==0) {
          *report+=time.toString("hh:mm:ss")+" "+
            QObject::tr("Rule broken: Artist separation");
          if(!HaveCode().isEmpty()) {
            *report+=QObject::tr(" with sched code(s): ")+HaveCode()+" "+HaveCode2();
          }
          *report+="\n";
          schedCL->restore();
        }
      }
      
      // Clock Scheduler Rules
      sql=QString("select ")+
	"CODE,"+         // 00
	"MAX_ROW,"+      // 01
	"MIN_WAIT,"+     // 02
	"NOT_AFTER,"+    // 03
	"OR_AFTER,"+     // 04
	"OR_AFTER_II "+  // 05
	"from RULE_LINES where "+
	"CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
      q=new RDSqlQuery(sql);
      while (q->next()) {
	// max in a row, min wait
	schedCL->save();	
	int range=q->value(1).toInt()+q->value(2).toInt(); 
	int allowed=q->value(1).toInt();
	QString wstr=q->value(0).toString();
	wstr+="          ";
	wstr=wstr.left(11);
	sql=QString("select STACK_LINES.CART ")+
	  "from STACK_LINES left join STACK_SCHED_CODES "+
	  "on STACK_LINES.ID=STACK_SCHED_CODES.STACK_LINES_ID where "+
	  "STACK_LINES.SERVICE_NAME=\""+RDEscapeString(svcname)+"\" && "+
	  QString().sprintf("STACK_LINES.SCHED_STACK_ID > %d && ",
			    stackid-range)+
	  "STACK_SCHED_CODES.SCHED_CODE=\""+RDEscapeString(wstr)+"\"";
	q1=new RDSqlQuery(sql);
	if(q1->size()>=allowed || allowed==0) {
	  for(counter=0;counter<schedCL->getNumberOfItems();counter++) {
	    if(schedCL->removeIfCode(counter,q->value(0).toString())) {
	      counter--;
	    }
	  }
	}
	delete q1;
	if(schedCL->getNumberOfItems()==0) {
	  *report+=time.toString("hh:mm:ss")+" "+
	    QObject::tr("Rule broken: Max. in a Row/Min. Wait for ")+
	    q->value(0).toString()+"\n";
	  schedCL->restore();
	}

	// do not play after
	if(q->value(3).toString()!="") {
	  schedCL->save();	
	  QString wstr=q->value(3).toString();
	  wstr+="          ";
	  wstr=wstr.left(11);
	  sql=QString("select STACK_LINES.CART ")+
	    "from STACK_LINES left join STACK_SCHED_CODES "+
	    "on STACK_LINES.ID=STACK_SCHED_CODES.STACK_LINES_ID where "+
	    "STACK_LINES.SERVICE_NAME=\""+RDEscapeString(svcname)+"\" && "+
	    QString().sprintf("STACK_LINES.SCHED_STACK_ID=%d && ",stackid-1)+
	    "STACK_SCHED_CODES.SCHED_CODE=\""+RDEscapeString(wstr)+"\"";
	  q1=new RDSqlQuery(sql);
	  if(q1->size()>0) {
	    for(counter=0;counter<schedCL->getNumberOfItems();counter++) {
	      if(schedCL->removeIfCode(counter,q->value(0).toString())) {
		counter--;
	      }
	    }
	  }
	  delete q1;
	  if(schedCL->getNumberOfItems()==0) {
	    *report+=time.toString("hh:mm:ss")+" "+
	      QObject::tr("Rule broken: Do not schedule ")+
	      q->value(0).toString()+" "+QObject::tr("after")+" "+
	      q->value(3).toString()+"\n";
	    schedCL->restore();
	  }
	}
	// or after
	if (q->value(4).toString()!="") {
	  schedCL->save();
	  QString wstr=q->value(4).toString();
	  wstr+="          ";
	  wstr=wstr.left(11);
	  sql=QString("select STACK_LINES.CART ")+
	    "from STACK_LINES left join STACK_SCHED_CODES "+
	    "on STACK_LINES.ID=STACK_SCHED_CODES.STACK_LINES_ID where "+
	    QString().sprintf("STACK_LINES.SCHED_STACK_ID=%d && ",stackid-1)+
	    "STACK_SCHED_CODES.SCHED_CODE=\""+RDEscapeString(wstr)+"\"";
	  q1=new RDSqlQuery(sql);
	  if(q1->size()>0) {	
	    for(counter=0;counter<schedCL->getNumberOfItems();counter++) {
	      if(schedCL->removeIfCode(counter,q->value(0).toString())) {
		counter--;
	      }
	    }
	  }
	  delete q1;
	  if(schedCL->getNumberOfItems()==0) {
	    *report+=time.toString("hh:mm:ss")+" "+
	      QObject::tr("Rule broken: Do not schedule")+" "+
	      q->value(0).toString()+" "+QObject::tr("after")+" "+
	      q->value(4).toString()+"\n";
	    schedCL->restore();
	  }
	}
	// or after II
	if (q->value(5).toString()!="") {
	  schedCL->save();
	  QString wstr=q->value(5).toString();
	  wstr+="          ";
	  wstr=wstr.left(11);
	  sql=QString("select STACK_LINES.CART ")+
	    "from STACK_LINES left join STACK_SCHED_CODES "+
	    "on STACK_LINES.ID=STACK_SCHED_CODES.STACK_LINES_ID where "+
	    QString().sprintf("STACK_LINES.SCHED_STACK_ID=%d && ",stackid-1)+
	    "STACK_SCHED_CODES.SCHED_CODE=\""+RDEscapeString(wstr)+"\"";
	  q1=new RDSqlQuery(sql);
	  if(q1->size()>0) {
	    for(counter=0;counter<schedCL->getNumberOfItems();counter++) {
	      if(schedCL->removeIfCode(counter,q->value(0).toString())) {
		counter--;
	      }
	    }
	  }
	  delete q1;
	  if(schedCL->getNumberOfItems()==0) {
	    *report+=time.toString("hh:mm:ss")+" "+
	      QObject::tr("Rule broken: Do not schedule")+" "+
	      q->value(0).toString()+" "+QObject::tr("after")+" "+
	      q->value(5).toString()+"\n";
	    schedCL->restore();
	  }
	}
      }
      delete q;

      ////////////////////////////////
      //                            //
      // End of deconflicting rules //
      //                            //
      ////////////////////////////////
      
      //
      // Pick a random cart from those that are remaining.
      //
      int schedpos=rand()%schedCL->getNumberOfItems();
      sql=QString("insert into LOG_LINES set ")+
	"LOG_NAME=\""+RDEscapeString(logname)+"\","+
	QString().sprintf("LINE_ID=%d,",count)+
	QString().sprintf("COUNT=%d,",count)+
	QString().sprintf("TYPE=%d,",RDLogLine::Cart)+
	QString().sprintf("SOURCE=%d,",source)+
	QString().sprintf("START_TIME=%d,",QTime().msecsTo(time))+
	QString().sprintf("GRACE_TIME=%d,",grace_time)+
	QString().sprintf("CART_NUMBER=%u,",schedCL->getItemCartNumber(schedpos))+
	QString().sprintf("TIME_TYPE=%d,",time_type)+
	QString().sprintf("TRANS_TYPE=%d,",trans_type)+
	"EXT_START_TIME="+RDCheckDateTime(time,"hh:mm:ss")+","+
	QString().sprintf("EVENT_LENGTH=%d",event_length);
      q=new RDSqlQuery(sql);
      delete q;

      count++;

      sql=QString("insert into STACK_LINES set ")+
	"SERVICE_NAME=\""+RDEscapeString(svcname)+"\","+
	"SCHEDULED_AT=now(),"+
	QString().sprintf("SCHED_STACK_ID=%u,",stackid)+
	QString().sprintf("CART=%u,",schedCL->getItemCartNumber(schedpos))+
	"ARTIST=\""+RDEscapeString(schedCL->getItemArtist(schedpos))+"\"";
      unsigned line_id=RDSqlQuery::run(sql).toUInt();
      QStringList codes=schedCL->getItemSchedCodes(schedpos);
      for(int i=0;i<codes.size();i++) {
	sql=QString("insert into STACK_SCHED_CODES set ")+
	  QString().sprintf("STACK_LINES_ID=%u,",line_id)+
	  "SCHED_CODE=\""+RDEscapeString(codes.at(i))+"\"";
	RDSqlQuery::apply(sql);
      }
      delete schedCL;
    }
    else {
      // We don't have any carts to work with
      *report+=time.toString("hh:mm:ss")+
        " "+QObject::tr("No carts found in group")+" "+schedGroup();
      if(!HaveCode().isEmpty()) {
        *report+=QObject::tr(" with sched code(s): ")+HaveCode()+" "+HaveCode2();
      }
      *report+="\n";

      delete schedCL;
    }
  }

  //
  // Post-Import Carts
  //
  for(int i=0;i<event_postimport_list->size()-1;i++) {
    RDEventImportItem *i_item=event_postimport_list->item(i);
    sql=QString("insert into LOG_LINES set ")+
      "LOG_NAME=\""+RDEscapeString(logname)+"\","+
      QString().sprintf("LINE_ID=%d,",count)+
      QString().sprintf("COUNT=%d,",count)+
      QString().sprintf("TYPE=%d,",i_item->eventType())+
      QString().sprintf("SOURCE=%d,",RDLogLine::Template)+
      QString().sprintf("START_TIME=%d,",QTime().msecsTo(time))+
      QString().sprintf("GRACE_TIME=%d,",grace_time)+
      QString().sprintf("CART_NUMBER=%u,",i_item->cartNumber())+
      QString().sprintf("TIME_TYPE=%d,",time_type)+
      QString().sprintf("TRANS_TYPE=%d,",i_item->transType())+
      "COMMENT=\""+RDEscapeString(i_item->markerComment())+"\","+
      QString().sprintf("EVENT_LENGTH=%d",event_length);
    RDSqlQuery::apply(sql);
    count++;
    time=time.addMSecs(GetLength(i_item->cartNumber()));
    time_type=RDLogLine::Relative;
    trans_type=event_default_transtype;
    grace_time=-1;
  }

  return true;
}


bool RDEventLine::linkLog(RDLogEvent *e,RDLog *log,const QString &svcname,
			  RDLogLine *link_logline,const QString &track_str,
			  const QString &label_cart,const QString &track_cart,
			  QString *errors)
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
  // Insert Parent Link
  //
  if(log->includeImportMarkers()) {
    e->insert(e->size(),1);
    logline=new RDLogLine();
    *logline=*link_logline;
    logline->setId(e->nextId());
    *(e->logLine(e->size()-1))=*logline;
    delete logline;
    logline=NULL;

    //
    // Clear Leading Event Values
    //
    time_type=RDLogLine::Relative;
    trans_type=event_default_transtype;
    grace_time=-1;
  }
  else {
    //
    // Propagate Leading Event Values to Next Event
    //
    time_type=link_logline->timeType();
    trans_type=link_logline->transType();
    grace_time=link_logline->graceTime();
  }

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
  sql=QString("select ")+
    "CART_NUMBER,"+     // 00
    "START_SECS,"+      // 01
    "LENGTH,"+          // 02
    "EXT_DATA,"+        // 03
    "EXT_EVENT_ID,"+    // 04
    "EXT_ANNC_TYPE,"+   // 05
    "EXT_CART_NAME,"+   // 06
    "TITLE,"+           // 07
    "TYPE,"+            // 08
    "LINK_START_TIME,"+ // 09
    "LINK_LENGTH "+     // 10
    "from IMPORTER_LINES where "+
    "STATION_NAME=\""+RDEscapeString(event_station->name())+"\" && "+
    QString().sprintf("PROCESS_ID=%u && ",getpid())+
    QString().sprintf("(START_HOUR=%d)&&",start_start_hour)+
    QString().sprintf("(START_SECS>=%d)&&",start_start_secs/1000)+
    QString().sprintf("(START_SECS<=%d)&&",end_start_secs/1000)+
    "(EVENT_USED=\"N\") order by LINE_ID";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    int length=GetLength(q->value(0).toUInt(),q->value(2).toInt());

    //
    // Inline Traffic Break
    //
    if(q->value(8).toUInt()==RDLogLine::TrafficLink) {
      if((!event_nested_event.isEmpty()&&(event_nested_event!=event_name))) {
	e->insert(e->size(),1);
	logline=e->logLine(e->size()-1);
	logline->setId(e->nextId());
	logline->setStartTime(RDLogLine::Logged,time);
	logline->setType(RDLogLine::TrafficLink);
	logline->setSource(event_src);
	logline->setTransType(trans_type);
	logline->setEventLength(event_length);
	logline->setLinkEventName(event_nested_event);
	logline->setLinkStartTime(q->value(9).toTime());
	logline->setLinkLength(q->value(10).toInt());
	logline->setLinkStartSlop(link_logline->linkStartSlop());
	logline->setLinkEndSlop(link_logline->linkEndSlop());
	logline->setLinkId(link_logline->linkId());
	logline->setLinkEmbedded(true);
      }
    }

    //
    // Voicetrack Marker
    //
    if(q->value(8).toUInt()==RDLogLine::Track) {
      e->insert(e->size(),1);
      logline=e->logLine(e->size()-1);
      logline->setId(e->nextId());
      logline->setStartTime(RDLogLine::Logged,time);
      logline->setType(RDLogLine::Track);
      logline->setSource(event_src);
      logline->setTransType(RDLogLine::Segue);
      logline->setMarkerComment(q->value(7).toString());
      logline->setEventLength(event_length);
      logline->setLinkEventName(event_name);
      logline->setLinkStartTime(link_logline->linkStartTime());
      logline->setLinkLength(link_logline->linkLength());
      logline->setLinkStartSlop(link_logline->linkStartSlop());
      logline->setLinkEndSlop(link_logline->linkEndSlop());
      logline->setLinkId(link_logline->linkId());
      logline->setLinkEmbedded(true);
    }

    //
    // Cart
    //
    if(q->value(8).toUInt()==RDLogLine::Cart) {
      e->insert(e->size(),1);
      logline=e->logLine(e->size()-1);
      logline->setId(e->nextId());
      logline->setSource(event_src);
      logline->
	setStartTime(RDLogLine::Logged,
		     QTime(start_start_hour,0,0).addSecs(q->value(1).toInt()));
      logline->setGraceTime(grace_time);
      logline->setTimeType(time_type);
      logline->setTransType(trans_type);
      logline->setExtStartTime(QTime().addSecs(3600*start_start_hour+
					       q->value(1).toInt()));
      logline->setExtLength(q->value(2).toInt());
      logline->setExtData(q->value(3).toString().trimmed());
      logline->setExtEventId(q->value(4).toString().trimmed());
      logline->setExtAnncType(q->value(5).toString().trimmed());
      logline->setExtCartName(q->value(6).toString().trimmed());
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
	logline->setMarkerComment(q->value(7).toString());
	logline->setCartNumber(0);
      }
      else {
	if((q->value(6).toString()==track_cart)&&(!track_cart.isEmpty())) {
	  logline->setType(RDLogLine::Track);
	  logline->setMarkerComment(q->value(7).toString());
	  logline->setCartNumber(0);
	}
	else {
	  logline->setType(RDLogLine::Cart);
	  logline->setCartNumber(q->value(0).toUInt());
	}
      }
      time=time.addMSecs(length);
    }

    //
    // Clear Leading Event Values
    //
    time_type=RDLogLine::Relative;
    trans_type=event_default_transtype;
    grace_time=-1;
  }
  delete q;

  //
  // Mark Events as Used
  //
  sql=QString("update IMPORTER_LINES set ")+
    "EVENT_USED=\"Y\" where "+
    "STATION_NAME=\""+RDEscapeString(event_station->name())+"\" && "+
    QString().sprintf("PROCESS_ID=%u && ",getpid())+
    QString().sprintf("(START_HOUR=%d)&&",start_start_hour)+
    QString().sprintf("(START_SECS>=%d)&&",start_start_secs/1000)+
    QString().sprintf("(START_SECS<=%d)&&",end_start_secs/1000)+
    "(EVENT_USED=\"N\")";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Autofill
  //
  QTime end_time=link_logline->startTime(RDLogLine::Logged).
    addMSecs(link_logline->linkLength());
  if(event_use_autofill&&(event_start_time<=time)) {
    QTime fill_start_time=time;
    sql=QString("select ")+
      "AUTOFILLS.CART_NUMBER,"+  // 00
      "CART.FORCED_LENGTH "+     // 01
      "from AUTOFILLS left join CART "+
      "on AUTOFILLS.CART_NUMBER=CART.NUMBER where "+
      "(AUTOFILLS.SERVICE=\""+RDEscapeString(svcname)+"\")&&"+
      QString().sprintf("(CART.FORCED_LENGTH<=%d)&&",time.msecsTo(end_time))+
      "(CART.FORCED_LENGTH>0) "+
      "order by CART.FORCED_LENGTH desc";
    q=new RDSqlQuery(sql);
    bool fit=true;
    while(fit) {
      fit=false;
      while(q->next()&&(fill_start_time<=time)) {
	if((time.addMSecs(q->value(1).toInt())<=end_time)&&
	   (time.addMSecs(q->value(1).toInt())>time)) {
	  e->insert(e->size(),1);
	  logline=e->logLine(e->size()-1);
	  logline->setId(e->nextId());
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
	*errors+=QString("  ")+time.toString("hh:mm:ss")+
	  " -- \""+event_name+"\" "+QObject::tr("is underscheduled by")+" "+
	  QTime().addMSecs(slop).toString("hh:mm:ss")+".\n";
      }
      else {
	*errors+=QString("  ")+time.toString("hh:mm:ss")+
	  " -- \""+event_name+"\" "+QObject::tr("is overscheduled by")+" "+
	  QTime().addMSecs(-slop).toString("hh:mm:ss")+".\n";
      }
    }
  }
  
  return false;
}


QString RDEventLine::propertiesText() const
{
  QString ret;
  QString sql=QString("select ")+
    "NAME,"+              // 00
    "COLOR,"+             // 01
    "PREPOSITION,"+       // 02
    "FIRST_TRANS_TYPE,"+  // 03
    "TIME_TYPE,"+         // 04
    "GRACE_TIME,"+        // 05
    "USE_AUTOFILL,"+      // 06
    "IMPORT_SOURCE,"+     // 07
    "NESTED_EVENT "+      // 08
    "from EVENTS where "+
    "NAME=\""+RDEscapeString(event_name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=RDEventLine::
      propertiesText(q->value(2).toInt(),
		     (RDLogLine::TransType)q->value(3).toUInt(),
		     (RDLogLine::TimeType)q->value(4).toUInt(),
		     q->value(5).toInt(),
		     RDBool(q->value(6).toString()),
		     (RDEventLine::ImportSource)q->value(7).toUInt(),
		     !q->value(8).toString().isEmpty());  
  }
  delete q;

  return ret;
}


QString RDEventLine::propertiesText(int prepos_msec,
				    RDLogLine::TransType first_trans,
				    RDLogLine::TimeType time_type,
				    int grace_msec,
				    bool autofill,
				    RDEventLine::ImportSource import_src,
				    bool inline_tfc)
{
  QString ret="";
  QString str;

  if(prepos_msec>=0) {
    ret+=QObject::tr("Cue")+
      "(-"+QTime(0,0,0).addMSecs(prepos_msec).toString("mm:ss")+"), ";
  }

  if(time_type==RDLogLine::Hard) {
    switch(grace_msec) {
    case 0:
      ret+=QObject::tr("Timed(Start), ");
      break;

    case -1:
      ret+=QObject::tr("Timed(MakeNext), ");
      break;

    default:
      ret+=", "+QObject::tr("Timed(Wait)")+" "+
	QTime(0,0,0).addMSecs(grace_msec).toString("mm:ss")+", ";
      break;
    }
  }

  if(autofill) {
    ret+=QObject::tr("Fill")+", ";
  }

  switch(import_src) {
  case RDEventLine::Traffic:
    ret+=QObject::tr("Traffic, ");
    break;

  case RDEventLine::Music:
    ret+=QObject::tr("Music, ");
    break;

  case RDEventLine::Scheduler:
    ret+=QObject::tr("Scheduler, ");
    break;

  default:
    break;
  }

  if(inline_tfc) {
    ret+=QObject::tr("Inline Traffic, ");
  }

  return ret.left(ret.length()-2);
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
