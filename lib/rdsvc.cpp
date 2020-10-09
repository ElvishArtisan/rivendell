// rdsvc.cpp
//
// Abstract a Rivendell Service.
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

#include <qmessagebox.h>

#include "rdapplication.h"
#include "rdclock.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rd.h"
#include "rdescape_string.h"
#include "rdlog.h"
#include "rdsvc.h"
#include "rdweb.h"

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


bool RDSvc::includeImportMarkers() const
{
  return 
    RDBool(RDGetSqlValue("SERVICES","NAME",svc_name,"INCLUDE_IMPORT_MARKERS").
	   toString());
}


void RDSvc::setIncludeImportMarkers(bool state)
{
  SetRow("INCLUDE_IMPORT_MARKERS",RDYesNo(state));
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


RDSvc::SubEventInheritance RDSvc::subEventInheritance() const
{
  return (RDSvc::SubEventInheritance)
    RDGetSqlValue("SERVICES","NAME",svc_name,"SUB_EVENT_INHERITANCE").toInt();
}


void RDSvc::setSubEventInheritance(RDSvc::SubEventInheritance inherit) const
{
  SetRow("SUB_EVENT_INHERITANCE",inherit);
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


QString RDSvc::importPath(ImportSource src) const
{
  QString fieldname=SourceString(src)+"PATH";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setImportPath(ImportSource src,const QString &path) 
  const
{
  QString fieldname=SourceString(src)+"PATH";
  SetRow(fieldname,path);
}


QString RDSvc::preimportCommand(ImportSource src) const
{
  QString fieldname=SourceString(src)+"PREIMPORT_CMD";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setPreimportCommand(ImportSource src,const QString &path) const
{
  QString fieldname=SourceString(src)+"PREIMPORT_CMD";
  SetRow(fieldname,path);
}


int RDSvc::importOffset(ImportSource src,ImportField field) const
{
  QString parser_table;
  QString parser_name;
  QString src_str="";

  //
  // Set Import Source
  //
  if(importTemplate(src).isEmpty()) {
    src_str=SourceString(src);
    parser_table="SERVICES";
    parser_name=svc_name;
  }
  else {
    src_str="";
    parser_table="IMPORT_TEMPLATES";
    parser_name=importTemplate(src);
  }
  QString fieldname=src_str+FieldString(field)+"OFFSET";
  return RDGetSqlValue(parser_table,"NAME",parser_name,fieldname).toInt();
}


void RDSvc::setImportOffset(ImportSource src,ImportField field,int offset) 
  const
{
  QString fieldname=SourceString(src)+FieldString(field)+"OFFSET";
  SetRow(fieldname,offset);
}


int RDSvc::importLength(ImportSource src,ImportField field) const
{
  QString parser_table;
  QString parser_name;
  QString src_str="";

  //
  // Set Import Source
  //
  if(importTemplate(src).isEmpty()) {
    src_str=SourceString(src);
    parser_table="SERVICES";
    parser_name=svc_name;
  }
  else {
    src_str="";
    parser_table="IMPORT_TEMPLATES";
    parser_name=importTemplate(src);
  }
  QString fieldname=src_str+FieldString(field)+"LENGTH";
  return RDGetSqlValue(parser_table,"NAME",parser_name,fieldname).toInt();
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
  QString sql=QString("select ")+
    src_str+os_flag+"_PATH from SERVICES where "+
    "NAME=\""+RDEscapeString(svc_name)+"\"";
  QString ret;
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=RDDateDecode(q->value(0).toString(),date,svc_station,svc_config,
		     svc_name);
  }
  delete q;
  return ret;
}


bool RDSvc::import(ImportSource src,const QDate &date,const QString &break_str,
		   const QString &track_str) const
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
  // Load Parser Parameters
  //
  sql=QString("select ")+
    src_str+os_flag+"_PATH,"+           // 00
    src_str+"_LABEL_CART,"+             // 01
    src_str+"_TRACK_CART,"+             // 02
    src_str+os_flag+"_PREIMPORT_CMD "+  // 03
    "from SERVICES where "+
    "NAME=\""+RDEscapeString(svc_name)+"\"";
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
     fopen(RDDateDecode(infilename,date,svc_station,svc_config,svc_name),"r"))==
     NULL) {
    return false;
  }

  //
  // Run Preimport Command
  //
  if(!preimport_cmd.isEmpty()) {
    system(RDDateDecode(preimport_cmd,date,svc_station,svc_config,svc_name));
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
  sql=QString("delete from IMPORTER_LINES where ")+
    "STATION_NAME=\""+RDEscapeString(svc_station->name())+"\" && "+
    QString().sprintf("PROCESS_ID=%u",getpid());
  RDSqlQuery::apply(sql);

  //
  // Parse and Save
  //
  int line_id=0;
  bool cart_ok=false;
  bool start_time_ok=false;
  QString track_label;
  QTime link_time;

  int file_line=0;
  while(fgets(buf,RD_MAX_IMPORT_LINE_LENGTH,infile)!=NULL) {
    str_buf=QString::fromUtf8(buf);

    //
    // Cart Number
    //
    cartnum=0;
    cartname=str_buf.mid(cart_offset,cart_length).trimmed();

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
    cartlen=-1;
    int len=0;
    bool ok1=true;
    bool ok2=true;
    bool ok3=true;
    if(hours_len_length>0) {
      QString buf=str_buf.mid(hours_len_offset,hours_len_length);
      len+=3600000*buf.toUInt(&ok1);
    }
    if(minutes_len_length>0) {
      QString buf=str_buf.mid(minutes_len_offset,minutes_len_length);
      len+=60000*buf.toInt(&ok2);
    }
    if(seconds_len_length>0) {
      QString buf=str_buf.mid(seconds_len_offset,seconds_len_length);
      len+=1000*buf.toUInt(&ok3);
    }
    if(ok1&&ok2&&ok3) {
      cartlen=len;
    }

    //
    // External Data
    //
    data_buf=str_buf.mid(data_offset,data_length);
    eventid_buf=str_buf.mid(eventid_offset,eventid_length);
    annctype_buf=str_buf.mid(annctype_offset,annctype_length);

    //
    // Title
    //
    title=str_buf.mid(title_offset,title_length).trimmed();

    //
    // Process Line
    //
    cartnum=cartname.toUInt(&cart_ok);

    //
    // Common SQL Elements
    //
    sql=QString("insert into IMPORTER_LINES set ")+
      "STATION_NAME=\""+RDEscapeString(svc_station->name())+"\","+
      QString().sprintf("PROCESS_ID=%d,",getpid())+
      QString().sprintf("FILE_LINE=%u,",file_line)+
      QString().sprintf("LINE_ID=%d,",line_id);
    if(start_time_ok) {
      sql+=QString().sprintf("START_HOUR=%d,",start_hour)+
	QString().sprintf("START_SECS=%d,",
			  60*start_minutes+start_seconds);
    }
    if(cartlen>=0) {
      sql+=QString().sprintf("LENGTH=%d,",cartlen);
    }

    //
    // Cart
    //
    if(start_time_ok&&cart_ok&&(cartnum>0)&&(cartnum<=RD_MAX_CART_NUMBER)) {
      sql+=QString().sprintf("TYPE=%u,",RDLogLine::Cart)+
	"EXT_DATA=\""+data_buf.trimmed()+"\","+
	"EXT_EVENT_ID=\""+eventid_buf.trimmed()+"\","+
	"EXT_ANNC_TYPE=\""+annctype_buf.trimmed()+"\","+
	"EXT_CART_NAME=\""+cartname.trimmed()+"\","+
	QString().sprintf("CART_NUMBER=%u,",cartnum)+
	"TITLE=\""+RDEscapeString(title)+"\"";
      RDSqlQuery::apply(sql);
      line_id++;
      file_line++;
      continue;
    }

    //
    // Inline Break
    //
    if((src==RDSvc::Music)&&(!break_str.isEmpty())) {
      if(str_buf.contains(break_str)) {
	sql+=QString().sprintf("TYPE=%u",RDLogLine::TrafficLink);
	RDSqlQuery::apply(sql);
	line_id++;
	file_line++;
	continue;
      }
    }

    //
    // Track Marker
    //
    if((!track_str.isEmpty())&&(str_buf.contains(track_str))) {
      sql+=QString().sprintf("TYPE=%u,",RDLogLine::Track)+
	"TITLE=\""+RDEscapeString(str_buf.simplified().trimmed())+"\"";
      RDSqlQuery::apply(sql);
      line_id++;
      file_line++;
      continue;
    }
    file_line++;
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
  RDClock clock(svc_station);
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
  log->setDescription(RDDateDecode(descriptionTemplate(),date,svc_station,
				   svc_config,svc_name));
  log->setIncludeImportMarkers(includeImportMarkers());

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
  sql=QString("select COUNT from LOG_LINES where ")+
    "LOG_NAME=\""+RDEscapeString(logname)+"\" "+
    "order by COUNT desc";
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
    sql=QString("insert into LOG_LINES set ")+
      "LOG_NAME=\""+RDEscapeString(logname)+"\","+
      QString().sprintf("LINE_ID=%d,",count)+
      QString().sprintf("COUNT=%d,",count)+
      QString().sprintf("TYPE=%d,",RDLogLine::Chain)+
      QString().sprintf("SOURCE=%d,",RDLogLine::Template)+
      QString().sprintf("TRANS_TYPE=%d,",RDLogLine::Segue)+
      QString().sprintf("TIME_TYPE=%d,",RDLogLine::Relative)+
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
  QTime prev_start_time;
  RDLogEvent *src_event=NULL;
  RDLogEvent *dest_event=NULL;
  RDLogLine *logline=NULL;

  *err_msg="";

  RDLogLock *log_lock=new RDLogLock(logname,user,svc_station,this);
  if(!TryLock(log_lock,err_msg)) {
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
  if(!import(src,date,breakString(),trackString(src))) {
    *err_msg=tr("Import failed");
    delete log_lock;
    return false;
  }

  //
  // Resolve embedded link parameters
  //
  if(src==RDSvc::Music) {
    if(!ResolveInlineTrafficLinks(logname,err_msg)) {
      delete log_lock;
      return false;
    }
  }

  //
  // Iterate Through the Log
  //
  src_event=new RDLogEvent(logname);
  dest_event=new RDLogEvent(logname);
  src_event->load();
  for(int i=0;i<src_event->size();i++) {
    logline=src_event->logLine(i);
    if(logline->type()==src_type) {
      RDEventLine *e=new RDEventLine(svc_station);
      e->setName(logline->linkEventName());
      e->load();
      e->linkLog(dest_event,log,svc_name,logline,track_str,label_cart,
		 track_cart,&autofill_errors);
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
  sql=QString("select ")+
    "IMPORTER_LINES.START_HOUR,"+   // 00
    "IMPORTER_LINES.START_SECS,"+   // 01
    "IMPORTER_LINES.CART_NUMBER,"+  // 02
    "CART.TITLE "+                  // 03
    "from IMPORTER_LINES left join CART "+
    "on IMPORTER_LINES.CART_NUMBER=CART.NUMBER where "+
    "IMPORTER_LINES.STATION_NAME=\""+
    RDEscapeString(svc_station->name())+"\" && "+
    QString().sprintf("IMPORTER_LINES.PROCESS_ID=%u && ",getpid())+
    "IMPORTER_LINES.EVENT_USED=\"N\" "+
    "order by LINE_ID";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event=true;
    if(q->value(3).toString().isEmpty()) {
      cartname=tr("[unknown cart]");
    }
    else {
      cartname=q->value(3).toString();
    }
    link_report+=QString("  ")+
      RDSvc::timeString(q->value(0).toInt(),q->value(1).toInt())+
      QString().sprintf(" - %06u - ",q->value(2).toUInt())+cartname+"\n";
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

  sql=QString("delete from IMPORTER_LINES where ")+
    "STATION_NAME=\""+RDEscapeString(svc_station->name())+"\" && "+
    QString().sprintf("PROCESS_ID=%u",getpid());
  // printf("Importer Table Cleanup SQL: %s\n",(const char *)sql);
  RDSqlQuery::apply(sql);
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

  RDLogEvent *src_event=new RDLogEvent(logname);
  RDLogEvent *dest_event=new RDLogEvent(logname);
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
  return ret;
}


bool RDSvc::create(const QString &name,QString *err_msg,
		   const QString &exemplar,RDConfig *config)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  if(name.simplified()!=name) {
    *err_msg=QObject::tr("whitespace is not permitted in service names");
    return false;
  }

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
      sql=QString("insert into AUDIO_PERMS set ")+
	"GROUP_NAME=\""+RDEscapeString(q->value(0).toString())+"\","
	"SERVICE_NAME=\""+RDEscapeString(name)+"\"";
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
      sql=QString("insert into SERVICE_PERMS set ")+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(name)+"\"";
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
      "TFC_CART_OFFSET,"+        // 11
      "TFC_CART_LENGTH,"+        // 12
      "TFC_TITLE_OFFSET,"        // 13
      "TFC_TITLE_LENGTH,"+       // 14
      "TFC_LEN_HOURS_OFFSET,"    // 15
      "TFC_LEN_HOURS_LENGTH,"    // 16
      "TFC_LEN_MINUTES_OFFSET,"  // 17
      "TFC_LEN_MINUTES_LENGTH,"  // 18
      "TFC_LEN_SECONDS_OFFSET,"  // 19
      "TFC_LEN_SECONDS_LENGTH,"  // 20
      "TFC_HOURS_OFFSET,"+       // 21
      "TFC_HOURS_LENGTH,"+       // 22
      "TFC_MINUTES_OFFSET,"+     // 23
      "TFC_MINUTES_LENGTH,"+     // 24
      "TFC_SECONDS_OFFSET,"+     // 25
      "TFC_SECONDS_LENGTH,"+     // 26
      "TFC_DATA_OFFSET,"+        // 27
      "TFC_DATA_LENGTH,"+        // 28
      "TFC_EVENT_ID_OFFSET,"+    // 29
      "TFC_EVENT_ID_LENGTH,"+    // 30
      "TFC_ANNC_TYPE_OFFSET,"+   // 31
      "TFC_ANNC_TYPE_LENGTH,"+   // 32
      "MUS_IMPORT_TEMPLATE,"+    // 33
      "MUS_PATH,"+               // 34
      "MUS_CART_OFFSET,"+        // 35
      "MUS_CART_LENGTH,"+        // 36
      "MUS_TITLE_OFFSET,"        // 37
      "MUS_TITLE_LENGTH,"+       // 38
      "MUS_LEN_HOURS_OFFSET,"    // 39
      "MUS_LEN_HOURS_LENGTH,"    // 40
      "MUS_LEN_MINUTES_OFFSET,"  // 41
      "MUS_LEN_MINUTES_LENGTH,"  // 42
      "MUS_LEN_SECONDS_OFFSET,"  // 43
      "MUS_LEN_SECONDS_LENGTH,"  // 44
      "MUS_HOURS_OFFSET,"+       // 45
      "MUS_HOURS_LENGTH,"+       // 46
      "MUS_MINUTES_OFFSET,"+     // 47
      "MUS_MINUTES_LENGTH,"+     // 48
      "MUS_SECONDS_OFFSET,"+     // 49
      "MUS_SECONDS_LENGTH,"+     // 50
      "MUS_DATA_OFFSET,"+        // 51
      "MUS_DATA_LENGTH,"+        // 52
      "MUS_EVENT_ID_OFFSET,"+    // 53
      "MUS_EVENT_ID_LENGTH,"+    // 54
      "MUS_ANNC_TYPE_OFFSET,"+   // 55
      "MUS_ANNC_TYPE_LENGTH "+   // 56
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
	QString().sprintf("TFC_CART_OFFSET=%d,",q->value(11).toInt())+
	QString().sprintf("TFC_CART_LENGTH=%d,",q->value(12).toInt())+
	QString().sprintf("TFC_TITLE_OFFSET=%d,",q->value(13).toInt())+
	QString().sprintf("TFC_TITLE_LENGTH=%d,",q->value(14).toInt())+
	QString().sprintf("TFC_LEN_HOURS_OFFSET=%d,",q->value(15).toInt())+
	QString().sprintf("TFC_LEN_HOURS_LENGTH=%d,",q->value(16).toInt())+
	QString().sprintf("TFC_LEN_MINUTES_OFFSET=%d,",q->value(17).toInt())+
	QString().sprintf("TFC_LEN_MINUTES_LENGTH=%d,",q->value(18).toInt())+
	QString().sprintf("TFC_LEN_SECONDS_OFFSET=%d,",q->value(19).toInt())+
	QString().sprintf("TFC_LEN_SECONDS_LENGTH=%d,",q->value(20).toInt())+
	QString().sprintf("TFC_HOURS_OFFSET=%d,",q->value(21).toInt())+
	QString().sprintf("TFC_HOURS_LENGTH=%d,",q->value(22).toInt())+
	QString().sprintf("TFC_MINUTES_OFFSET=%d,",q->value(23).toInt())+
	QString().sprintf("TFC_MINUTES_LENGTH=%d,",q->value(24).toInt())+
	QString().sprintf("TFC_SECONDS_OFFSET=%d,",q->value(25).toInt())+
	QString().sprintf("TFC_SECONDS_LENGTH=%d,",q->value(26).toInt())+
	QString().sprintf("TFC_DATA_OFFSET=%d,",q->value(27).toInt())+
	QString().sprintf("TFC_DATA_LENGTH=%d,",q->value(28).toInt())+
	QString().sprintf("TFC_EVENT_ID_OFFSET=%d,",q->value(29).toInt())+
	QString().sprintf("TFC_EVENT_ID_LENGTH=%d,",q->value(30).toInt())+
	QString().sprintf("TFC_ANNC_TYPE_OFFSET=%d,",q->value(31).toInt())+
	QString().sprintf("TFC_ANNC_TYPE_LENGTH=%d,",q->value(32).toInt())+
	"MUS_IMPORT_TEMPLATE=\""+RDEscapeString(q->value(33).toString())+"\","+
	"MUS_PATH=\""+RDEscapeString(q->value(34).toString())+"\","+
	QString().sprintf("MUS_CART_OFFSET=%d,",q->value(35).toInt())+
	QString().sprintf("MUS_CART_LENGTH=%d,",q->value(36).toInt())+
	QString().sprintf("MUS_TITLE_OFFSET=%d,",q->value(37).toInt())+
	QString().sprintf("MUS_TITLE_LENGTH=%d,",q->value(38).toInt())+
	QString().sprintf("MUS_LEN_HOURS_OFFSET=%d,",q->value(39).toInt())+
	QString().sprintf("MUS_LEN_HOURS_LENGTH=%d,",q->value(40).toInt())+
	QString().sprintf("MUS_LEN_MINUTES_OFFSET=%d,",q->value(41).toInt())+
	QString().sprintf("MUS_LEN_MINUTES_LENGTH=%d,",q->value(42).toInt())+
	QString().sprintf("MUS_LEN_SECONDS_OFFSET=%d,",q->value(43).toInt())+
	QString().sprintf("MUS_LEN_SECONDS_LENGTH=%d,",q->value(44).toInt())+
	QString().sprintf("MUS_HOURS_OFFSET=%d,",q->value(45).toInt())+
	QString().sprintf("MUS_HOURS_LENGTH=%d,",q->value(46).toInt())+
	QString().sprintf("MUS_MINUTES_OFFSET=%d,",q->value(47).toInt())+
	QString().sprintf("MUS_MINUTES_LENGTH=%d,",q->value(48).toInt())+
	QString().sprintf("MUS_SECONDS_OFFSET=%d,",q->value(49).toInt())+
	QString().sprintf("MUS_SECONDS_LENGTH=%d,",q->value(50).toInt())+
	QString().sprintf("MUS_DATA_OFFSET=%d,",q->value(51).toInt())+
	QString().sprintf("MUS_DATA_LENGTH=%d,",q->value(52).toInt())+
	QString().sprintf("MUS_EVENT_ID_OFFSET=%d,",q->value(53).toInt())+
	QString().sprintf("MUS_EVENT_ID_LENGTH=%d,",q->value(54).toInt())+
	QString().sprintf("MUS_ANNC_TYPE_OFFSET=%d,",q->value(55).toInt())+
	QString().sprintf("MUS_ANNC_TYPE_LENGTH=%d,",q->value(56).toInt())+
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
    sql=QString("select GROUP_NAME from AUDIO_PERMS where ")+
      "SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into AUDIO_PERMS set ")+
	"GROUP_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(name)+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Service Perms
    //
    sql=QString("select STATION_NAME from SERVICE_PERMS where ")+
      "SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into SERVICE_PERMS set ")+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(name)+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Autofill List
    //
    sql=QString("select CART_NUMBER from AUTOFILLS where ")+
      "SERVICE=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into AUTOFILLS set ")+
	QString().sprintf("CART_NUMBER=%u,",q->value(0).toUInt())+
	"SERVICE=\""+RDEscapeString(name)+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Clock Perms
    //
    sql=QString("select CLOCK_NAME from CLOCK_PERMS where ")+
      "SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into CLOCK_PERMS set ")+
	"CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(name)+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Event Perms
    //
    sql=QString("select EVENT_NAME from EVENT_PERMS where ")+
      "SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into EVENT_PERMS set ")+
	"EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(name)+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

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
    sql=QString("delete from LOG_LINES where ")+
      "LOG_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
    RDSqlQuery::apply(sql);
    logname=q->value(0).toString();
    logname.replace(" ","_");
    rda->dropTable(logname+"_REC");
  }
  delete q;

  sql=QString("select ID from STACK_LINES where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("delete from STACK_SCHED_CODES where ")+
      QString().sprintf("STACK_LINES_ID=%u",q->value(0).toUInt());
    RDSqlQuery::apply(sql);
  }
  delete q;
  sql=QString("delete from STACK_LINES where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  RDSqlQuery::apply(sql);

  sql=QString("delete from ELR_LINES where ")+
    "SERVICE_NAME=\""+RDEscapeString(name)+"\"";
  RDSqlQuery::apply(sql);

  sql=QString("delete from LOGS where ")+
    "SERVICE=\""+RDEscapeString(name)+"\"";
  RDSqlQuery::apply(sql);
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

  //  value.replace("\\","\\\\");  // Needed to preserve Windows pathnames
  sql=QString("update SERVICES set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    "NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSvc::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update SERVICES set ")+
    param+QString().sprintf("=%d where ",value)+
    "NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSvc::GetParserStrings(ImportSource src,QString *break_str,
			     QString *track_str,QString *label_cart,
			     QString *track_cart)
{
  QString src_str=SourceString(src);
  QString sql=QString("select ")+
    src_str+"BREAK_STRING,"+  // 00
    src_str+"TRACK_STRING,"+  // 01
    src_str+"LABEL_CART,"+    // 02
    src_str+"TRACK_CART "+    // 03
    "from SERVICES where "+
    "NAME=\""+RDEscapeString(svc_name)+"\"";
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


QString RDSvc::MakeErrorLine(int indent,unsigned lineno,const QString &msg) 
  const
{
  QString margin;

  for(int i=0;i<indent;i++) {
    margin+=" ";
  }
  return margin+tr("Line")+QString().sprintf(" %u: ",1+lineno)+msg+"\n";
}


bool RDSvc::ResolveInlineTrafficLinks(const QString &logname,QString *err_msg)
  const
{
  RDLogEvent *evt=NULL;
  RDLogLine *logline=NULL;
  QTime start;
  QTime end;
  QString sql;
  RDSqlQuery *q=NULL;
  bool ok=false;

  switch(subEventInheritance()) {
  case RDSvc::ParentEvent:
    evt=new RDLogEvent(logname);
    evt->load();
    ok=true;
    for(int i=0;i<evt->size();i++) {
      logline=evt->logLine(i);
      if(logline->type()==RDLogLine::MusicLink) {
	start=logline->linkStartTime();
	end=logline->linkStartTime().addSecs(logline->linkLength());
	sql=QString("select ")+
	  "ID,"+          // 00
	  "FILE_LINE "+   // 01
	  "from IMPORTER_LINES where "+
	  "IMPORTER_LINES.STATION_NAME=\""+
	  RDEscapeString(svc_station->name())+"\" && "+
	  QString().sprintf("IMPORTER_LINES.PROCESS_ID=%u && ",getpid())+
	  QString().sprintf("TYPE=%u && ",RDLogLine::TrafficLink)+
	  QString().sprintf("START_HOUR=%d && ",start.hour())+
	  QString().sprintf("START_SECS>=%d && ",
			    60*start.minute()+start.second())+
	  QString().sprintf("START_SECS<%d",60*start.minute()+start.second()+logline->linkLength()/1000);
	q=new RDSqlQuery(sql);
	if(q->size()>1) {
	  *err_msg+=tr("In event")+" \""+logline->linkEventName()+"\"@"+
	    logline->startTime(RDLogLine::Logged).toString("hh:mm:ss")+":\n";
	  while(q->next()) {
	    *err_msg+=MakeErrorLine(4,q->value(1).toUInt(),
				    tr("multiple inline traffic breaks not permitted within the same music event"));
	  }
	  *err_msg+="\n";
	  ok=false;
	}
	if(q->first()) {
	  sql=QString("update IMPORTER_LINES set ")+
	    "LINK_START_TIME=\""+
	    logline->linkStartTime().toString("hh:mm:ss")+"\","+
	    QString().sprintf("LINK_LENGTH=%d where ",logline->linkLength())+
	    QString().sprintf("ID=%u",q->value(0).toUInt());
	  RDSqlQuery::apply(sql);
	}
	delete q;
      }
    }
    delete evt;
    evt=NULL;

    if(!ok) {
      return false;
    }
    break;

  case RDSvc::SchedFile:
    //
    // Verify that all inline traffic events have explicit start times
    // and length
    //
    sql=QString("select ")+
      "FILE_LINE "+   // 00
      "from IMPORTER_LINES where "+
      "IMPORTER_LINES.STATION_NAME=\""+
      RDEscapeString(svc_station->name())+"\" && "+
      QString().sprintf("IMPORTER_LINES.PROCESS_ID=%u && ",getpid())+
      QString().sprintf("TYPE=%u && ",RDLogLine::TrafficLink)+
      "(START_HOUR is null || START_SECS is null || LENGTH is null)";
    ok=true;
    q=new RDSqlQuery(sql);
    while(q->next()) {
      *err_msg=MakeErrorLine(0,q->value(0).toUInt(),
			     tr("invalid start time and/or length on inline traffic break."));
      ok=false;
    }
    delete q;
    if(!ok) {
      return false;
    }

    //
    // Resolve link parameters
    //
    sql=QString("select ")+
      "ID,"+          // 00
      "START_HOUR,"+  // 01
      "START_SECS,"+  // 02
      "LENGTH "+      // 03
      "from IMPORTER_LINES where "+
      QString().sprintf("TYPE=%u",RDLogLine::TrafficLink);
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update IMPORTER_LINES set ")+
	"LINK_START_TIME=\""+QTime(q->value(1).toInt(),0,0).
	addSecs(q->value(2).toInt()).toString("hh:mm:ss")+"\","+
	QString().sprintf("LINK_LENGTH=%d where ",q->value(3).toInt())+
	QString().sprintf("ID=%u",q->value(0).toUInt());
      RDSqlQuery::apply(sql);
    }
    delete q;
    break;
  }

  return true;
}
