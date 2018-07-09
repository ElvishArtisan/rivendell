// rdsvc.cpp
//
// Abstract a Rivendell Service.
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>

#include "rdapplication.h"
#include "rdclock.h"
#include "rdconf.h"
#include "rdcreate_log.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rd.h"
#include "rdescape_string.h"
#include "rdlog.h"
#include "rdsvc.h"
#include "rdweb.h"

//
// Global Classes
//
RDSvc::RDSvc(QString svcname,RDStation *station,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  svc_name=svcname;
  svc_station=station;
  svc_config=config;
}


bool RDSvc::exists() const
{
  return RDDoesRowExist("SERVICES","NAME",svc_name);
}


QString RDSvc::name() const
{
  return svc_name;
}


QString RDSvc::description() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"DESCRIPTION").
    toString();
}


void RDSvc::setDescription(const QString &desc) const
{
  SetRow("DESCRIPTION",desc);
}


QString RDSvc::programCode() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"PROGRAM_CODE").
    toString();
}


void RDSvc::setProgramCode(const QString &str) const
{
  SetRow("PROGRAM_CODE",str);
}


QString RDSvc::nameTemplate() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"NAME_TEMPLATE").
    toString();
}


void RDSvc::setNameTemplate(const QString &str) const
{
  SetRow("NAME_TEMPLATE",str);
}


QString RDSvc::descriptionTemplate() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"DESCRIPTION_TEMPLATE").
    toString();
}


void RDSvc::setDescriptionTemplate(const QString &str) const
{
  SetRow("DESCRIPTION_TEMPLATE",str);
}


QString RDSvc::trackGroup() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"TRACK_GROUP").
    toString();
}


void RDSvc::setTrackGroup(const QString &group) const
{
  SetRow("TRACK_GROUP",group);
}


QString RDSvc::autospotGroup() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"AUTOSPOT_GROUP").
    toString();
}


void RDSvc::setAutospotGroup(const QString &group) const
{
  SetRow("AUTOSPOT_GROUP",group);
}


bool RDSvc::autoRefresh() const
{
  return RDBool(RDGetSqlValue("SERVICES","NAME",svc_name,"AUTO_REFRESH").
    toString());
}


void RDSvc::setAutoRefresh(bool state)
{
  SetRow("AUTO_REFRESH",RDYesNo(state));
}


int RDSvc::defaultLogShelflife() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"DEFAULT_LOG_SHELFLIFE").toInt();
}


void RDSvc::setDefaultLogShelflife(int days) const
{
  SetRow("DEFAULT_LOG_SHELFLIFE",days);
}


RDSvc::ShelflifeOrigin RDSvc::logShelflifeOrigin() const
{
  return (RDSvc::ShelflifeOrigin)RDGetSqlValue("SERVICES","NAME",svc_name,
					       "LOG_SHELFLIFE_ORIGIN").toInt();
}


void RDSvc::setLogShelflifeOrigin(RDSvc::ShelflifeOrigin orig)
{
  SetRow("LOG_SHELFLIFE_ORIGIN",(int)orig);
}


int RDSvc::elrShelflife() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"ELR_SHELFLIFE").toInt();
}


void RDSvc::setElrShelflife(int days) const
{
  SetRow("ELR_SHELFLIFE",days);
}


bool RDSvc::chainto() const
{
  return 
    RDBool(RDGetSqlValue("SERVICES","NAME",svc_name,"CHAIN_LOG").toString());
}


void RDSvc::setChainto(bool state) const
{
  SetRow("CHAIN_LOG",RDYesNo(state));
}


QString RDSvc::importTemplate(ImportSource src) const
{
  QString fieldname=SourceString(src)+"IMPORT_TEMPLATE";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setImportTemplate(ImportSource src,const QString &str) const
{
  QString fieldname=SourceString(src)+"IMPORT_TEMPLATE";
  SetRow(fieldname,str);
}


QString RDSvc::breakString() const
{
  return RDGetSqlValue("SERVICES","NAME",svc_name,"MUS_BREAK_STRING").
    toString();
}


void RDSvc::setBreakString(const QString &str)
{
  SetRow("MUS_BREAK_STRING",str);
}


QString RDSvc::trackString(ImportSource src) const
{
  QString fieldname=SourceString(src)+"TRACK_STRING";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setTrackString(ImportSource src,const QString &str)
{
  QString fieldname=SourceString(src)+"TRACK_STRING";
  SetRow(fieldname,str);
}


QString RDSvc::labelCart(ImportSource src) const
{
  QString fieldname=SourceString(src)+"LABEL_CART";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).toString();
}


void RDSvc::setLabelCart(ImportSource src,const QString &str)
{
  QString fieldname=SourceString(src)+"LABEL_CART";
  SetRow(fieldname,str);
}


QString RDSvc::trackCart(ImportSource src) const
{
  QString fieldname=SourceString(src)+"TRACK_CART";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).toString();
}


void RDSvc::setTrackCart(ImportSource src,const QString &str)
{
  QString fieldname=SourceString(src)+"TRACK_CART";
  SetRow(fieldname,str);
}


QString RDSvc::importPath(ImportSource src,ImportOs os) const
{
  QString fieldname=SourceString(src)+OsString(os)+"PATH";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setImportPath(ImportSource src,ImportOs os,const QString &path) 
  const
{
  QString fieldname=SourceString(src)+OsString(os)+"PATH";
  SetRow(fieldname,path);
}


QString RDSvc::preimportCommand(ImportSource src,ImportOs os) const
{
  QString fieldname=SourceString(src)+OsString(os)+"PREIMPORT_CMD";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setPreimportCommand(ImportSource src,ImportOs os,
				const QString &path) const
{
  QString fieldname=SourceString(src)+OsString(os)+"PREIMPORT_CMD";
  SetRow(fieldname,path);
}


int RDSvc::importOffset(ImportSource src,ImportField field) const
{
  QString fieldname=SourceString(src)+FieldString(field)+"OFFSET";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).toInt();
}


void RDSvc::setImportOffset(ImportSource src,ImportField field,int offset) 
  const
{
  QString fieldname=SourceString(src)+FieldString(field)+"OFFSET";
  SetRow(fieldname,offset);
}


int RDSvc::importLength(ImportSource src,ImportField field) const
{
  QString fieldname=SourceString(src)+FieldString(field)+"LENGTH";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).toInt();
}


void RDSvc::setImportLength(ImportSource src,ImportField field,int len) const
{
  QString fieldname=SourceString(src)+FieldString(field)+"LENGTH";
  SetRow(fieldname,len);
}


QString RDSvc::importFilename(ImportSource src,const QDate &date) const
{
  QString src_str;
  switch(src) {
      case RDSvc::Traffic:
	src_str="TFC";
	break;

      case RDSvc::Music:
	src_str="MUS";
	break;
  }
  QString os_flag;
#ifdef WIN32
  os_flag="_WIN";
#endif
  QString sql=QString().sprintf("select %s%s_PATH from SERVICES \
                                 where NAME=\"%s\"",
				(const char *)src_str,
				(const char *)os_flag,
				(const char *)svc_name);
  QString ret;
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=RDDateDecode(q->value(0).toString(),date,svc_config->stationName(),
		     svc_station->shortName(),svc_name);
  }
  delete q;
  return ret;
}


bool RDSvc::import(ImportSource src,const QDate &date,const QString &break_str,
		   const QString &track_str,const QString &dest_table) const
{
  FILE *infile;
  QString src_str;
  char buf[RD_MAX_IMPORT_LINE_LENGTH];
  QString str_buf;
  int start_hour;
  int start_minutes;
  int start_seconds;
  QString hours_len_buf;
  QString minutes_len_buf;
  QString seconds_len_buf;
  unsigned cartnum;
  QString cartname;
  QString title;
  QString data_buf;
  QString eventid_buf;
  QString annctype_buf;
  QString os_flag;
  int cartlen;
  QString sql;
  bool ok=false;

  //
  // Set Import Source
  //
  switch(src) {
      case RDSvc::Traffic:
	src_str="TFC";
	break;

      case RDSvc::Music:
	src_str="MUS";
	break;
  }

  //
  // Set OS Type
  //
#ifdef WIN32
  os_flag="_WIN";
#endif

  //
  // Load Parser Parameters
  //
  sql=QString().sprintf("select %s%s_PATH,\
                         %s_LABEL_CART,\
                         %s_TRACK_CART,\
                         %s%s_PREIMPORT_CMD \
                         from SERVICES where NAME=\"%s\"",
			(const char *)src_str,
			(const char *)os_flag,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)os_flag,
			(const char *)RDEscapeString(svc_name));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  QString infilename=q->value(0).toString();
  QString label_cart=q->value(1).toString().stripWhiteSpace();
  QString track_cart=q->value(2).toString().stripWhiteSpace();
  QString preimport_cmd=q->value(3).toString();
  delete q;

  //
  // Open Source File
  //
  if((infile=
      fopen(RDDateDecode(infilename,date,svc_config->stationName(),
			 svc_station->shortName(),svc_name),"r"))==
     NULL) {
    return false;
  }

  //
  // Run Preimport Command
  //
  if(!preimport_cmd.isEmpty()) {
    system(RDDateDecode(preimport_cmd,date,svc_config->stationName(),
			svc_station->shortName(),svc_name));
  }

  QString parser_table;
  QString parser_name;
  if(importTemplate(src).isEmpty()) {
    src_str+="_";
    parser_table="SERVICES";
    parser_name=svc_name;
  }
  else {
    src_str="";
    parser_table="IMPORT_TEMPLATES";
    parser_name=importTemplate(src);
  }
  sql=QString("select ")+
    src_str+"CART_OFFSET,"+         // 00
    src_str+"CART_LENGTH,"+         // 01
    src_str+"DATA_OFFSET,"+         // 02
    src_str+"DATA_LENGTH,"+         // 03
    src_str+"EVENT_ID_OFFSET,"+     // 04
    src_str+"EVENT_ID_LENGTH,"+     // 05
    src_str+"ANNC_TYPE_OFFSET,"+    // 06
    src_str+"ANNC_TYPE_LENGTH,"+    // 07
    src_str+"TITLE_OFFSET,"+        // 08
    src_str+"TITLE_LENGTH,"+        // 09
    src_str+"HOURS_OFFSET,"+        // 10
    src_str+"HOURS_LENGTH,"+        // 11
    src_str+"MINUTES_OFFSET,"+      // 12
    src_str+"MINUTES_LENGTH,"+      // 13
    src_str+"SECONDS_OFFSET,"+      // 14
    src_str+"SECONDS_LENGTH,"+      // 15
    src_str+"LEN_HOURS_OFFSET,"+    // 16
    src_str+"LEN_HOURS_LENGTH,"+    // 17
    src_str+"LEN_MINUTES_OFFSET,"+  // 18
    src_str+"LEN_MINUTES_LENGTH,"+  // 19
    src_str+"LEN_SECONDS_OFFSET,"+  // 20
    src_str+"LEN_SECONDS_LENGTH "+  // 21
    "from "+parser_table+" where NAME=\""+RDEscapeString(parser_name)+"\"";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  int cart_offset=q->value(0).toInt();
  int cart_length=q->value(1).toInt();
  int data_offset=q->value(2).toInt();
  int data_length=q->value(3).toInt();
  int eventid_offset=q->value(4).toInt();
  int eventid_length=q->value(5).toInt();
  int annctype_offset=q->value(6).toInt();
  int annctype_length=q->value(7).toInt();
  int title_offset=q->value(8).toInt();
  int title_length=q->value(9).toInt();
  int hours_offset=q->value(10).toInt();
  int hours_length=q->value(11).toInt();
  int minutes_offset=q->value(12).toInt();
  int minutes_length=q->value(13).toInt();
  int seconds_offset=q->value(14).toInt();
  int seconds_length=q->value(15).toInt();
  int hours_len_offset=q->value(16).toInt();
  int hours_len_length=q->value(17).toInt();
  int minutes_len_offset=q->value(18).toInt();
  int minutes_len_length=q->value(19).toInt();
  int seconds_len_offset=q->value(20).toInt();
  int seconds_len_length=q->value(21).toInt();

  delete q;

  //
  // Setup Data Source and Destination
  //
  rda->dropTable(dest_table);
  sql=QString("create table ")+
    "`"+dest_table+"` ("+
    "ID int primary key,"+
    "START_HOUR int not null,"+
    "START_SECS int not null,"+
    "CART_NUMBER int unsigned,"+
    "TITLE char(255),"+
    "LENGTH int,"+
    "INSERT_BREAK enum('N','Y') default 'N',"+
    "INSERT_TRACK enum('N','Y') default 'N',"+
    "INSERT_FIRST int unsigned default 0,"+
    "TRACK_STRING char(255),"+
    "EXT_DATA char(32),"+
    "EXT_EVENT_ID char(32),"+
    "EXT_ANNC_TYPE char(8),"+
    "EXT_CART_NAME char(32),"+
    "LINK_START_TIME time default NULL,"+
    "LINK_LENGTH int default NULL,"+
    "EVENT_USED enum('N','Y') default 'N',"+
    "INDEX START_TIME_IDX (START_HOUR,START_SECS)) "+
    svc_config->createTablePostfix();
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Parse and Save
  //
  int line_id=0;
  bool insert_found=false;
  bool cart_ok=false;
  bool start_time_ok=false;
  bool line_used=false;
  bool insert_break=false;
  bool insert_track=false;
  bool break_first=false;
  bool track_first=false;
  QString track_label;
  QTime link_time;
  int link_length=-1;

  while(fgets(buf,RD_MAX_IMPORT_LINE_LENGTH,infile)!=NULL) {
    line_used=false;
    str_buf=QString(buf);

    //
    // Cart Number
    //
    cartnum=0;
    cartname=str_buf.mid(cart_offset,cart_length).stripWhiteSpace();

    //
    // Start Time
    //
    start_time_ok=true;
    start_hour=str_buf.mid(hours_offset,hours_length).toInt(&ok);
    if((!ok)||(start_hour<0)||(start_hour>23)) {
      start_time_ok=false;
    }
    start_minutes=str_buf.mid(minutes_offset,minutes_length).toInt(&ok);
    if((!ok)||(start_minutes<0)) {
      start_time_ok=false;
    }
    start_seconds=str_buf.mid(seconds_offset,seconds_length).toInt(&ok);
    if((!ok)||(start_seconds<0)||(start_seconds>59)) {
      start_time_ok=false;
    }

    //
    // Length
    //
    hours_len_buf=str_buf.mid(hours_len_offset,hours_len_length);
    minutes_len_buf=str_buf.mid(minutes_len_offset,minutes_len_length);
    seconds_len_buf=str_buf.mid(seconds_len_offset,seconds_len_length);
    cartlen=3600000*hours_len_buf.toInt()+60000*minutes_len_buf.toInt()+
      1000*seconds_len_buf.toInt();

    //
    // External Data
    //
    data_buf=str_buf.mid(data_offset,data_length);
    eventid_buf=str_buf.mid(eventid_offset,eventid_length);
    annctype_buf=str_buf.mid(annctype_offset,annctype_length);

    //
    // Title
    //
    title=str_buf.mid(title_offset,title_length).stripWhiteSpace();

    //
    // Process Line
    //
    cartnum=cartname.toUInt(&cart_ok);
    if(start_time_ok&&(cart_ok||
		       ((!label_cart.isEmpty())&&(cartname==label_cart))||
		       ((!track_cart.isEmpty())&&(cartname==track_cart)))) {
      sql=QString("insert into ")+
	"`"+dest_table+"`	set "+
	QString().sprintf("ID=%d,",line_id++)+
	QString().sprintf("START_HOUR=%d,",start_hour)+
	QString().sprintf("START_SECS=%d,",
			  60*start_minutes+start_seconds)+
	QString().sprintf("CART_NUMBER=%u,",cartnum)+
	"TITLE=\""+RDEscapeString(title)+"\","+
	QString().sprintf("LENGTH=%d,",cartlen)+
	"EXT_DATA=\""+data_buf+"\","+
	"EXT_EVENT_ID=\""+eventid_buf+"\","+
	"EXT_ANNC_TYPE=\""+annctype_buf+"\","+
	"EXT_CART_NAME=\""+cartname+"\"";
      q=new RDSqlQuery(sql);
      delete q;
      //
      // Insert Break
      //
      if(insert_break) {
	sql=QString("update ")+"`"+dest_table+"` set "+
	  "INSERT_BREAK=\"Y\"";
	if(break_first) {
	  sql+=QString().sprintf(",INSERT_FIRST=%d",
				 RDEventLine::InsertBreak);
	}
	if(link_time.isValid()&&(link_length>=0)) {
	  sql+=",LINK_START_TIME=\""+
	    link_time.toString("hh:mm:ss")+"\""+
	    QString().sprintf(",LINK_LENGTH=%d",
			      link_length);
	}
	sql+=QString().sprintf(" where ID=%d",line_id-1);
	q=new RDSqlQuery(sql);
	delete q;
      }
      //
      // Insert Track
      //
      if(insert_track) {
	if(track_first) {
	  sql=QString("update ")+
	    "`"+dest_table+"` set "+
	    "INSERT_TRACK=\"Y\","+
	    "TRACK_STRING=\""+RDEscapeString(track_label)+"\","+
	    QString().sprintf("INSERT_FIRST=%d ",RDEventLine::InsertTrack)+
	    QString().sprintf("where ID=%d",line_id-1);
	}
	else {
	  sql=QString("update ")+
	    "`"+dest_table+"` set "+
	    "INSERT_TRACK=\"Y\","+
	    "TRACK_STRING=\""+RDEscapeString(track_label)+"\" "+
	    QString().sprintf("where ID=%d",line_id-1);
	}
	q=new RDSqlQuery(sql);
	delete q;
      }
      insert_break=false;
      break_first=false;
      insert_track=false;
      track_first=false;
      insert_found=false;
      line_used=true;
    }
    if((cartname==break_str)&&start_time_ok) {
      link_time=
	QTime(start_hour,start_minutes,start_seconds);
      link_length=cartlen;
    }
    else {
      link_time=QTime();
      link_length=-1;
    }
    if(!line_used) {
      //
      // Look for Break and Track Strings
      //
      if(!break_str.isEmpty()) {
	if(str_buf.contains(break_str)) {
	  insert_break=true;
	  if(!insert_found) {
	    break_first=true;
	    insert_found=true;
	  }
	}
      }
      if(!track_str.isEmpty()) {
	if(str_buf.contains(track_str)) {
	  insert_track=true;
	  track_label=str_buf;
	  if(!insert_found) {
	    track_first=true;
	    insert_found=true;
	  }
	}
      }
    }
  }

  //
  // Cleanup
  //
  fclose(infile);
  return true;
}


bool RDSvc::generateLog(const QDate &date,const QString &logname,
			const QString &nextname,QString *report,RDUser *user,
			QString *err_msg)
{
  QString sql;
  RDSqlQuery *q;
  RDClock clock;
  //  QString err_msg;
  RDLog *log=NULL;
  RDLogLock *log_lock=NULL;

  if((!date.isValid()||logname.isEmpty())) {
    return false;
  }

  emit generationProgress(0);

  //
  // Generate Log Structure
  //
  if(RDLog::exists(logname)) {
    log_lock=new RDLogLock(logname,user,svc_station,this);
    if(!TryLock(log_lock,err_msg)) {
      delete log_lock;
      return false;
    }
    RDLog::remove(logname,svc_station,user,svc_config);
    delete log_lock;
  }
  RDLog::create(logname,svc_name,date,"RDLogManager",err_msg,svc_config);
  log_lock=new RDLogLock(logname,user,svc_station,this);
  if(!TryLock(log_lock,err_msg)) {
    delete log_lock;
    return false;
  }
  log=new RDLog(logname);
  log->setDescription(RDDateDecode(descriptionTemplate(),date,
				   svc_config->stationName(),
				   svc_station->shortName(),svc_name));

  emit generationProgress(1);

  //
  // Generate Events
  //
  for(int i=0;i<24;i++) {
    sql=QString("select CLOCK_NAME from SERVICE_CLOCKS where ")+
      "(SERVICE_NAME=\""+RDEscapeString(svc_name)+"\")&&"+
      QString().sprintf("(HOUR=%d)",24*(date.dayOfWeek()-1)+i);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      if((!q->value(0).isNull())&&(!q->value(0).toString().isEmpty())) {
	clock.setName(q->value(0).toString());
	clock.load();
	clock.generateLog(i,logname,svc_name,report);
	clock.clear();
      }
    }
    delete q;
    emit generationProgress(1+i);
  }

  //
  // Get Current Count
  //
  int count;
  sql=QString("select COUNT from `")+RDLog::tableName(logname)+"` order by COUNT desc";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    count=q->value(0).toInt()+1;
  }
  else {
    count=0;
  }
  delete q;

  //
  // Log Chain To
  //
  if(chainto()) {
    sql=QString("insert into `")+RDLog::tableName(logname)+"` set "+
      QString().sprintf("ID=%d,COUNT=%d,TYPE=%d,",count,count,RDLogLine::Chain)+
      QString().sprintf("SOURCE=%d,TRANS_TYPE=%d,",RDLogLine::Template,
			RDLogLine::Segue)+
      "LABEL=\""+RDEscapeString(nextname)+"\"";
    q=new RDSqlQuery(sql);
    delete q;
    count ++;
  }

  log->updateLinkQuantity(RDLog::SourceMusic);
  log->setLinkState(RDLog::SourceMusic,false);
  log->updateLinkQuantity(RDLog::SourceTraffic);
  log->setLinkState(RDLog::SourceTraffic,false);
  log->setNextId(count);
  log->setAutoRefresh(autoRefresh());
  delete log;
  delete log_lock;

  return true;
}


bool RDSvc::linkLog(RDSvc::ImportSource src,const QDate &date,
		    const QString &logname,QString *report,RDUser *user,
		    QString *err_msg)
{
  QString sql;
  RDSqlQuery *q;
  QString autofill_errors;

  RDLogLock *log_lock=new RDLogLock(logname,user,svc_station,this);
  if(!TryLock(log_lock,err_msg)) {
    delete log_lock;
    return false;
  }

  emit generationProgress(0);

  //
  // Load ParserStrings
  //
  QString break_str;
  QString track_str;
  QString label_cart;
  QString track_cart;
  GetParserStrings(src,&break_str,&track_str,&label_cart,&track_cart);

  //
  // Import File
  //
  QString import_name=QString("IMPORT_")+svc_name+"_"+date.toString("yyyyMMdd");

  import_name.replace(" ","_");
  if(!import(src,date,breakString(),trackString(src),import_name)) {
    *err_msg=tr("Import failed");
    delete log_lock;
    return false;
  }

  //
  // Calculate Source
  //
  RDLogLine::Type src_type=RDLogLine::UnknownType;
  RDLog::Source link_src=RDLog::SourceMusic;
  switch(src) {
      case RDSvc::Music:
	src_type=RDLogLine::MusicLink;
	link_src=RDLog::SourceMusic;
	break;

      case RDSvc::Traffic:
	src_type=RDLogLine::TrafficLink;
	link_src=RDLog::SourceTraffic;
	break;
  }
  RDLog *log=new RDLog(logname);
  int current_link=0;
  int total_links=log->linkQuantity(link_src);

  //
  // Iterate Through the Log
  //
  RDLogEvent *src_event=new RDLogEvent(RDLog::tableName(logname));
  RDLogEvent *dest_event=new RDLogEvent(RDLog::tableName(logname));
  src_event->load();
  RDLogLine *logline=NULL;
  for(int i=0;i<src_event->size();i++) {
    logline=src_event->logLine(i);
    if(logline->type()==src_type) {
      RDEventLine *e=new RDEventLine();
      e->setName(logline->linkEventName());
      e->load();
      e->linkLog(dest_event,svc_name,logline,track_str,label_cart,track_cart,
		 import_name,&autofill_errors);
      delete e;
      emit generationProgress(1+(24*current_link++)/total_links);
    }
    else {
      dest_event->insert(dest_event->size(),1,true);
      *(dest_event->logLine(dest_event->size()-1))=*logline;
      dest_event->logLine(dest_event->size()-1)->setId(dest_event->nextId());
    }
  }
  dest_event->save(svc_config);

  //
  // Update the Log Link Status
  //
  log->setLinkState(link_src,true);
  if(link_src==RDLog::SourceMusic) {
    log->updateLinkQuantity(RDLog::SourceTraffic);
  }
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  log->setLinkDatetime(current_datetime);
  log->setModifiedDatetime(current_datetime);
  delete log;

  //
  // Generate Missing Event and Skipped Events Report
  //
  QString cartname;
  QString missing_report;
  dest_event->validate(&missing_report,date);
  bool event=false;
  QString link_report=tr("The following events were not placed:\n");
  sql=QString().sprintf("select `%s`.START_HOUR,`%s`.START_SECS,\
                         `%s`.CART_NUMBER,CART.TITLE from `%s` LEFT JOIN CART\
                         ON `%s`.CART_NUMBER=CART.NUMBER \
                         where `%s`.EVENT_USED=\"N\"",
			(const char *)import_name,(const char *)import_name,
			(const char *)import_name,(const char *)import_name,
			(const char *)import_name,(const char *)import_name);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event=true;
    if(q->value(3).toString().isEmpty()) {
      cartname=tr("[unknown cart]");
    }
    else {
      cartname=q->value(3).toString();
    }
    link_report+=
      QString().sprintf("  %s - %06u - %s\n",
			(const char *)RDSvc::timeString(q->value(0).toInt(),
							q->value(1).toInt()),
			q->value(2).toUInt(),
			(const char *)cartname);
  }
  delete q;
  link_report+="\n";

  //
  // Assemble Exception Report
  //
  *report="";
  if(!autofill_errors.isEmpty()) {
    *report+=tr("Event Fill Errors\n");
    *report+=autofill_errors;
    *report+="\n";
  }
  *report+=missing_report;
  if(event) {
    *report+=link_report;
  }

  //
  // Clean Up
  //
  emit generationProgress(24);
  delete src_event;
  delete dest_event;

  //  printf("Import Table: %s\n",(const char *)import_name);
  rda->dropTable(import_name);
  delete log_lock;

  return true;
}


bool RDSvc::clearLogLinks(RDSvc::ImportSource src,const QString &logname,
			  RDUser *user,QString *err_msg)
{
  RDLogLock *log_lock=new RDLogLock(logname,user,svc_station,this);
  if(!TryLock(log_lock,err_msg)) {
    delete log_lock;
    return false;
  }
  std::vector<int> cleared_ids;
  RDLogLine::Source event_source=RDLogLine::Manual;
  switch(src) {
      case RDSvc::Music:
	event_source=RDLogLine::Music;
	break;

      case RDSvc::Traffic:
	event_source=RDLogLine::Traffic;
	break;
  }

  RDLogEvent *src_event=new RDLogEvent(RDLog::tableName(logname));
  RDLogEvent *dest_event=new RDLogEvent(RDLog::tableName(logname));
  src_event->load();
  RDLogLine *logline=NULL;
  for(int i=0;i<src_event->size();i++) {
    logline=src_event->logLine(i);
    if((logline->linkId()<0)||(logline->source()!=event_source)) {
      dest_event->insert(dest_event->size(),1,true);
      *(dest_event->logLine(dest_event->size()-1))=*logline;
      dest_event->logLine(dest_event->size()-1)->setId(dest_event->nextId());
    }
  }
  dest_event->save(svc_config);
  delete src_event;
  delete dest_event;

  RDLog *log=new RDLog(logname);
  if(src==RDSvc::Traffic) {
    log->setLinkState(RDLog::SourceTraffic,false);
  }
  if(src==RDSvc::Music) {
    log->setLinkState(RDLog::SourceMusic,false);
  }
  delete log;
  delete log_lock;
  *err_msg="OK";
  return true;
}


void RDSvc::remove() const
{
  RDSvc::remove(svc_name);
}


bool RDSvc::exists(const QString &name)
{
  bool ret=false;
  QString sql=QString("select NAME from SERVICES where ")+
    "NAME=\""+RDEscapeString(name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;
  return ret;
}


QString RDSvc::xml() const
{
  QString sql;
  RDSqlQuery *q;
  QString ret;
#ifndef WIN32
  sql="select DESCRIPTION from SERVICES where NAME=\""+
    RDEscapeString(svc_name)+"\"";

  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret+="  <service>\n";
    ret+="   "+RDXmlField("name",svc_name);
    ret+="   "+RDXmlField("description",q->value(0).toString());
    ret+="  </service>\n";
  }
  delete q;
#endif  // WIN32
  return ret;
}


bool RDSvc::create(const QString &name,QString *err_msg,
		   const QString &exemplar,RDConfig *config)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("select NAME from SERVICES where ")+
    "NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *err_msg=QObject::tr("service already exists");
    delete q;
    return false;
  }

  if(exemplar.isEmpty()) {  // Create Empty Service
    sql=QString("insert into SERVICES set NAME=\"")+
      RDEscapeString(name)+"\","+
      "NAME_TEMPLATE=\""+RDEscapeString(name)+"-%m%d\","+
      "DESCRIPTION_TEMPLATE=\""+RDEscapeString(name)+" log for %d/%m/%Y\"";
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Create Group Audio Perms
    //
    sql="select NAME from GROUPS";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into AUDIO_PERMS set\
                           GROUP_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
    
    //
    // Create Station Perms
    //
    sql="select NAME from STATIONS";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into SERVICE_PERMS set\
                           STATION_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    for(int i=0;i<168;i++) {
      sql=QString("insert into SERVICE_CLOCKS set ")+
	"SERVICE_NAME=\""+RDEscapeString(name)+"\","+
	QString().sprintf("HOUR=%d,",i)+
	"CLOCK_NAME=null";
      q=new RDSqlQuery(sql);
      delete q;
    }
  }
  else {    // Base on Existing Service
    sql=QString("select ")+
      "NAME_TEMPLATE,"+          // 00
      "DESCRIPTION_TEMPLATE,"+   // 01
      "CHAIN_LOG,"+              // 02
      "TRACK_GROUP,"+            // 03
      "AUTOSPOT_GROUP,"+         // 04
      "DEFAULT_LOG_SHELFLIFE,"+  // 05
      "LOG_SHELFLIFE_ORIGIN,"+   // 06
      "AUTO_REFRESH,"+           // 07
      "ELR_SHELFLIFE,"+          // 08
      "TFC_IMPORT_TEMPLATE,"+    // 09
      "TFC_PATH,"+               // 10
      "TFC_WIN_PATH,"+           // 11
      "TFC_CART_OFFSET,"+        // 12
      "TFC_CART_LENGTH,"+        // 13
      "TFC_TITLE_OFFSET,"        // 14
      "TFC_TITLE_LENGTH,"+       // 15
      "TFC_LEN_HOURS_OFFSET,"    // 16
      "TFC_LEN_HOURS_LENGTH,"    // 17
      "TFC_LEN_MINUTES_OFFSET,"  // 18
      "TFC_LEN_MINUTES_LENGTH,"  // 19
      "TFC_LEN_SECONDS_OFFSET,"  // 20
      "TFC_LEN_SECONDS_LENGTH,"  // 21
      "TFC_HOURS_OFFSET,"+       // 22
      "TFC_HOURS_LENGTH,"+       // 23
      "TFC_MINUTES_OFFSET,"+     // 24
      "TFC_MINUTES_LENGTH,"+     // 25
      "TFC_SECONDS_OFFSET,"+     // 26
      "TFC_SECONDS_LENGTH,"+     // 27
      "TFC_DATA_OFFSET,"+        // 28
      "TFC_DATA_LENGTH,"+        // 29
      "TFC_EVENT_ID_OFFSET,"+    // 30
      "TFC_EVENT_ID_LENGTH,"+    // 31
      "TFC_ANNC_TYPE_OFFSET,"+   // 32
      "TFC_ANNC_TYPE_LENGTH,"+   // 33
      "MUS_IMPORT_TEMPLATE,"+    // 34
      "MUS_PATH,"+               // 35
      "MUS_WIN_PATH,"+           // 36
      "MUS_CART_OFFSET,"+        // 37
      "MUS_CART_LENGTH,"+        // 38
      "MUS_TITLE_OFFSET,"        // 39
      "MUS_TITLE_LENGTH,"+       // 40
      "MUS_LEN_HOURS_OFFSET,"    // 41
      "MUS_LEN_HOURS_LENGTH,"    // 42
      "MUS_LEN_MINUTES_OFFSET,"  // 43
      "MUS_LEN_MINUTES_LENGTH,"  // 44
      "MUS_LEN_SECONDS_OFFSET,"  // 45
      "MUS_LEN_SECONDS_LENGTH,"  // 46
      "MUS_HOURS_OFFSET,"+       // 47
      "MUS_HOURS_LENGTH,"+       // 48
      "MUS_MINUTES_OFFSET,"+     // 49
      "MUS_MINUTES_LENGTH,"+     // 50
      "MUS_SECONDS_OFFSET,"+     // 51
      "MUS_SECONDS_LENGTH,"+     // 52
      "MUS_DATA_OFFSET,"+        // 53
      "MUS_DATA_LENGTH,"+        // 54
      "MUS_EVENT_ID_OFFSET,"+    // 55
      "MUS_EVENT_ID_LENGTH,"+    // 56
      "MUS_ANNC_TYPE_OFFSET,"+   // 57
      "MUS_ANNC_TYPE_LENGTH "+   // 58
      " from SERVICES where NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into SERVICES set ")+
	"NAME_TEMPLATE=\""+RDEscapeString(q->value(0).toString())+"\","+
	"DESCRIPTION_TEMPLATE=\""+RDEscapeString(q->value(1).toString())+"\","+
	"CHAIN_LOG=\""+RDEscapeString(q->value(2).toString())+"\","+
	"TRACK_GROUP=\""+RDEscapeString(q->value(3).toString())+"\","+
	"AUTOSPOT_GROUP=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("DEFAULT_LOG_SHELFLIFE=%d,",q->value(5).toInt())+
	QString().sprintf("LOG_SHELFLIFE_ORIGIN=%d,",q->value(6).toInt())+
	"AUTO_REFRESH=\""+RDEscapeString(q->value(7).toString())+"\","+
	QString().sprintf("ELR_SHELFLIFE=%d,",q->value(8).toInt())+
	"TFC_IMPORT_TEMPLATE=\""+RDEscapeString(q->value(9).toString())+"\","+
	"TFC_PATH=\""+RDEscapeString(q->value(10).toString())+"\","+
	"TFC_WIN_PATH=\""+RDEscapeString(q->value(11).toString())+"\","+
	QString().sprintf("TFC_CART_OFFSET=%d,",q->value(12).toInt())+
	QString().sprintf("TFC_CART_LENGTH=%d,",q->value(13).toInt())+
	QString().sprintf("TFC_TITLE_OFFSET=%d,",q->value(14).toInt())+
	QString().sprintf("TFC_TITLE_LENGTH=%d,",q->value(15).toInt())+
	QString().sprintf("TFC_LEN_HOURS_OFFSET=%d,",q->value(16).toInt())+
	QString().sprintf("TFC_LEN_HOURS_LENGTH=%d,",q->value(17).toInt())+
	QString().sprintf("TFC_LEN_MINUTES_OFFSET=%d,",q->value(18).toInt())+
	QString().sprintf("TFC_LEN_MINUTES_LENGTH=%d,",q->value(19).toInt())+
	QString().sprintf("TFC_LEN_SECONDS_OFFSET=%d,",q->value(20).toInt())+
	QString().sprintf("TFC_LEN_SECONDS_LENGTH=%d,",q->value(21).toInt())+
	QString().sprintf("TFC_HOURS_OFFSET=%d,",q->value(22).toInt())+
	QString().sprintf("TFC_HOURS_LENGTH=%d,",q->value(23).toInt())+
	QString().sprintf("TFC_MINUTES_OFFSET=%d,",q->value(24).toInt())+
	QString().sprintf("TFC_MINUTES_LENGTH=%d,",q->value(25).toInt())+
	QString().sprintf("TFC_SECONDS_OFFSET=%d,",q->value(26).toInt())+
	QString().sprintf("TFC_SECONDS_LENGTH=%d,",q->value(27).toInt())+
	QString().sprintf("TFC_DATA_OFFSET=%d,",q->value(28).toInt())+
	QString().sprintf("TFC_DATA_LENGTH=%d,",q->value(29).toInt())+
	QString().sprintf("TFC_EVENT_ID_OFFSET=%d,",q->value(30).toInt())+
	QString().sprintf("TFC_EVENT_ID_LENGTH=%d,",q->value(31).toInt())+
	QString().sprintf("TFC_ANNC_TYPE_OFFSET=%d,",q->value(32).toInt())+
	QString().sprintf("TFC_ANNC_TYPE_LENGTH=%d,",q->value(33).toInt())+
	"MUS_IMPORT_TEMPLATE=\""+RDEscapeString(q->value(34).toString())+"\","+
	"MUS_PATH=\""+RDEscapeString(q->value(35).toString())+"\","+
	"MUS_WIN_PATH=\""+RDEscapeString(q->value(36).toString())+"\","+
	QString().sprintf("MUS_CART_OFFSET=%d,",q->value(37).toInt())+
	QString().sprintf("MUS_CART_LENGTH=%d,",q->value(38).toInt())+
	QString().sprintf("MUS_TITLE_OFFSET=%d,",q->value(39).toInt())+
	QString().sprintf("MUS_TITLE_LENGTH=%d,",q->value(40).toInt())+
	QString().sprintf("MUS_LEN_HOURS_OFFSET=%d,",q->value(41).toInt())+
	QString().sprintf("MUS_LEN_HOURS_LENGTH=%d,",q->value(42).toInt())+
	QString().sprintf("MUS_LEN_MINUTES_OFFSET=%d,",q->value(43).toInt())+
	QString().sprintf("MUS_LEN_MINUTES_LENGTH=%d,",q->value(44).toInt())+
	QString().sprintf("MUS_LEN_SECONDS_OFFSET=%d,",q->value(45).toInt())+
	QString().sprintf("MUS_LEN_SECONDS_LENGTH=%d,",q->value(46).toInt())+
	QString().sprintf("MUS_HOURS_OFFSET=%d,",q->value(47).toInt())+
	QString().sprintf("MUS_HOURS_LENGTH=%d,",q->value(48).toInt())+
	QString().sprintf("MUS_MINUTES_OFFSET=%d,",q->value(49).toInt())+
	QString().sprintf("MUS_MINUTES_LENGTH=%d,",q->value(50).toInt())+
	QString().sprintf("MUS_SECONDS_OFFSET=%d,",q->value(51).toInt())+
	QString().sprintf("MUS_SECONDS_LENGTH=%d,",q->value(52).toInt())+
	QString().sprintf("MUS_DATA_OFFSET=%d,",q->value(53).toInt())+
	QString().sprintf("MUS_DATA_LENGTH=%d,",q->value(54).toInt())+
	QString().sprintf("MUS_EVENT_ID_OFFSET=%d,",q->value(55).toInt())+
	QString().sprintf("MUS_EVENT_ID_LENGTH=%d,",q->value(56).toInt())+
	QString().sprintf("MUS_ANNC_TYPE_OFFSET=%d,",q->value(57).toInt())+
	QString().sprintf("MUS_ANNC_TYPE_LENGTH=%d,",q->value(58).toInt())+
	"NAME=\""+RDEscapeString(name)+"\"";
      delete q;
      q=new RDSqlQuery(sql);
      delete q;
      sql=QString("select HOUR,CLOCK_NAME from SERVICE_CLOCKS where ")+
	"SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	sql=QString("insert into SERVICE_CLOCKS set ")+
	  "SERVICE_NAME=\""+RDEscapeString(name)+"\","+
	  QString().sprintf("HOUR=%d,",q->value(0).toInt());

	if(q->value(1).isNull()) {
  	  sql+="CLOCK_NAME=null";
	}
	else {
	  sql+="CLOCK_NAME=\""+RDEscapeString(q->value(1).toString())+"\"";
	}
	q1=new RDSqlQuery(sql);
	delete q1;
      }
      delete q;
    }
    else {
      *err_msg=QObject::tr("template service")+" \""+exemplar+"\" "+
	QObject::tr("does not exist");
      delete q;
      return false;
    }

    //
    // Clone Audio Perms
    //
    sql=QString().sprintf("select GROUP_NAME from AUDIO_PERMS\
                           where SERVICE_NAME=\"%s\"",
			  (const char *)RDEscapeString(exemplar));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into AUDIO_PERMS set\
                             GROUP_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Service Perms
    //
    sql=QString().sprintf("select STATION_NAME from SERVICE_PERMS\
                           where SERVICE_NAME=\"%s\"",
			  (const char *)RDEscapeString(exemplar));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into SERVICE_PERMS set\
                             STATION_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Autofill List
    //
    sql=QString().sprintf("select CART_NUMBER from AUTOFILLS\
                           where SERVICE=\"%s\"",
			  (const char *)RDEscapeString(exemplar));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into AUTOFILLS set\
                             CART_NUMBER=%u,SERVICE=\"%s\"",
			    q->value(0).toUInt(),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Clock Perms
    //
    sql=QString().sprintf("select CLOCK_NAME from CLOCK_PERMS\
                           where SERVICE_NAME=\"%s\"",
			  (const char *)RDEscapeString(exemplar));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into CLOCK_PERMS set\
                             CLOCK_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Event Perms
    //
    sql=QString().sprintf("select EVENT_NAME from EVENT_PERMS\
                           where SERVICE_NAME=\"%s\"",
			  (const char *)RDEscapeString(exemplar));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into EVENT_PERMS set\
                             EVENT_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)RDEscapeString(name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  //
  // Create Service Reconciliation Table
  //
  sql=RDCreateReconciliationTableSql(RDSvc::svcTableName(name),config);
  q=new RDSqlQuery(sql);
  delete q;

  return true;
}


void RDSvc::remove(const QString &name)
{
  QString sql;
  RDSqlQuery *q;
  QString logname;

  sql=QString("delete from AUDIO_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SERVICE_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("update RDAIRPLAY set ")+
    "DEFAULT_SERVICE=\"\" where "+
    "DEFAULT_SERVICE=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from EVENT_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from CLOCK_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from AUTOFILLS where ")+
    "SERVICE=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from REPORT_SERVICES where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SERVICES where ")+
    "NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SERVICE_CLOCKS where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("select NAME from LOGS where ")+
    "SERVICE=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    logname=q->value(0).toString();
    logname.replace(" ","_");
    rda->dropTable(RDLog::tableName(logname));
    rda->dropTable(logname+"_REC");
  }
  delete q;

  QString tablename=name;
  tablename.replace(" ","_");

  rda->dropTable(tablename+"_SRT");
  rda->dropTable(tablename+"_STACK");

  sql=QString("delete from LOGS where ")+
    "SERVICE=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


QString RDSvc::svcTableName(const QString &svc_name)
{
  QString ret=svc_name;
  ret.replace(" ","_");

  return ret+"_SRT";
}


QString RDSvc::timeString(int hour,int secs)
{
  return QString().sprintf("%02d:%02d:%02d",hour,secs/60,secs%60);
}


bool RDSvc::TryLock(RDLogLock *lock,QString *err_msg)
{
  QString username;
  QString stationname;
  QHostAddress addr;
  
  if(!lock->tryLock(&username,&stationname,&addr)) {
    *err_msg=tr("Log in use by")+" "+username+"@"+stationname;
    if(stationname!=addr.toString()) {
      *err_msg+=" ["+addr.toString()+"]";
    }
    return false;
  }
  return true;
}


QString RDSvc::SourceString(ImportSource src) const
{
  QString fieldname;
  switch(src) {
      case RDSvc::Traffic:
	fieldname="TFC_";
	break;

      case RDSvc::Music:
	fieldname="MUS_";
	break;
  }
  return fieldname;
}


QString RDSvc::OsString(ImportOs os) const
{
  QString fieldname;
  switch(os) {
      case RDSvc::Linux:
	fieldname="";
	break;

      case RDSvc::Windows:
	fieldname="WIN_";
	break;
  }
  return fieldname;
}


QString RDSvc::FieldString(ImportField field) const
{
  QString fieldname;
  switch(field) {
      case RDSvc::CartNumber:
	fieldname="CART_";
	break;

      case RDSvc::Title:
	fieldname="TITLE_";
	break;

      case RDSvc::StartHours:
	fieldname="HOURS_";
	break;

      case RDSvc::StartMinutes:
	fieldname="MINUTES_";
	break;

      case RDSvc::StartSeconds:
	fieldname="SECONDS_";
	break;

      case RDSvc::LengthHours:
	fieldname="LEN_HOURS_";
	break;

      case RDSvc::LengthMinutes:
	fieldname="LEN_MINUTES_";
	break;

      case RDSvc::LengthSeconds:
	fieldname="LEN_SECONDS_";
	break;

      case RDSvc::ExtData:
	fieldname="DATA_";
	break;

      case RDSvc::ExtEventId:
	fieldname="EVENT_ID_";
	break;

      case RDSvc::ExtAnncType:
	fieldname="ANNC_TYPE_";
	break;
  }
  return fieldname;
}


void RDSvc::SetRow(const QString &param,QString value) const
{
  RDSqlQuery *q;
  QString sql;

  value.replace("\\","\\\\");  // Needed to preserve Windows pathnames
  sql=QString().sprintf("UPDATE SERVICES SET %s=\"%s\" WHERE NAME=\"%s\"",
			(const char *)param,
			(const char *)value,
			(const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSvc::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE SERVICES SET %s=%d WHERE NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSvc::GetParserStrings(ImportSource src,QString *break_str,
			     QString *track_str,QString *label_cart,
			     QString *track_cart)
{
  QString src_str=SourceString(src);
  QString sql=QString().sprintf("select %sBREAK_STRING,%sTRACK_STRING,\
                                 %sLABEL_CART,%sTRACK_CART \
                                 from SERVICES where NAME=\"%s\"",
				(const char *)src_str,
				(const char *)src_str,
				(const char *)src_str,
				(const char *)src_str,
				(const char *)RDEscapeString(svc_name));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    *break_str=q->value(0).toString();
    *track_str=q->value(1).toString();
    *label_cart=q->value(2).toString();
    *track_cart=q->value(3).toString();
  }
  else {
    *break_str="";
    *track_str="";
    *label_cart="";
    *track_cart="";
  }
  delete q;
}


bool RDSvc::CheckId(std::vector<int> *v,int value)
{
  for(unsigned i=0;i<v->size();i++) {
    if(v->at(i)==value) {
      return false;
    }
  }
  v->push_back(value);
  return true;
}
