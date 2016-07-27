// rdsvc.cpp
//
// Abstract a Rivendell Service.
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

#include <qmessagebox.h>

#include <rdconf.h>
#include <rd.h>
#include <rdsvc.h>
#include <rddatedecode.h>
#include <rdcreate_log.h>
#include <rdclock.h>
#include <rdlog.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdweb.h>

//
// Global Classes
//
RDSvc::RDSvc(QString svcname,QObject *parent)
  : QObject(parent)
{
  svc_name=svcname;
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
    ret=RDDateDecode(q->value(0).toString(),date);
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
  if((infile=fopen(RDDateDecode(infilename,date),"r"))==NULL) {
    return false;
  }

  //
  // Run Preimport Command
  //
  if(!preimport_cmd.isEmpty()) {
    system(RDDateDecode(preimport_cmd,date));
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
  sql=QString().sprintf("drop table `%s`",(const char *)dest_table);
  QSqlQuery *qq;          // Use QSqlQuery so we don't generate a 
  qq=new QSqlQuery(sql);  // spurious error message.
  delete qq;
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
    "INDEX START_TIME_IDX (START_HOUR,START_SECS))";
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
    str_buf=QString(buf).stripWhiteSpace();

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
			const QString &nextname,QString *report)
{
  QString sql;
  RDSqlQuery *q;
  RDClock clock;

  if((!date.isValid()||logname.isEmpty())) {
    return false;
  }

  emit generationProgress(0);

  //
  // Generate Log Structure
  //
  QDate purge_date;
  if(defaultLogShelflife()>=0) {
    purge_date=date.addDays(defaultLogShelflife());
  }
  sql=QString().sprintf("select NAME from LOGS where NAME=\"%s\"",
			(const char *)RDEscapeString(logname));
  q=new RDSqlQuery(sql);
  if(q->first()) {   // Already Exists
    delete q;
    sql=QString().sprintf("update LOGS set SERVICE=\"%s\",\
                           DESCRIPTION=\"%s\",ORIGIN_USER=\"%s\",\
                           ORIGIN_DATETIME=now(),LINK_DATETIME=now(),\
                           MODIFIED_DATETIME=now(),START_DATE=null,\
                           END_DATE=null,NEXT_ID=0",
			  (const char *)RDEscapeString(svc_name),
			  (const char *)RDEscapeString(RDDateDecode(descriptionTemplate(),date)),
			  "RDLogManager");
    if(!purge_date.isValid()) {
      sql+=(",PURGE_DATE=\""+purge_date.toString("yyyy-MM-dd")+"\"");
    }
    sql+=(" where NAME=\""+RDEscapeString(logname)+"\"");

    q=new RDSqlQuery(sql);
    delete q;
    sql=QString("drop table `")+RDLog::tableName(logname)+"`";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {             // Doesn't exist
    delete q;
    sql=QString().sprintf("insert into LOGS set NAME=\"%s\",\
                           SERVICE=\"%s\",DESCRIPTION=\"%s\",\
                           ORIGIN_USER=\"%s\",ORIGIN_DATETIME=now(),\
                           LINK_DATETIME=now(),MODIFIED_DATETIME=now(),\
                           PURGE_DATE=%s",
			  (const char *)RDEscapeString(logname),
			  (const char *)RDEscapeString(svc_name),
			  (const char *)RDEscapeString(RDDateDecode(descriptionTemplate(),date)),
			  "RDLogManager",
			  (const char *)RDCheckDateTime(purge_date,"yyyy-MM-dd"));
    q=new RDSqlQuery(sql);
    delete q;
  }
  RDCreateLogTable(RDLog::tableName(logname));
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

  RDLog *log=new RDLog(logname);
  log->updateLinkQuantity(RDLog::SourceMusic);
  log->setLinkState(RDLog::SourceMusic,false);
  log->updateLinkQuantity(RDLog::SourceTraffic);
  log->setLinkState(RDLog::SourceTraffic,false);
  log->setNextId(count);
  log->setAutoRefresh(autoRefresh());
  delete log;

  return true;
}


bool RDSvc::linkLog(RDSvc::ImportSource src,const QDate &date,
		    const QString &logname,QString *report)
{
  QString sql;
  RDSqlQuery *q;
  QString autofill_errors;

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
      e->linkLog(dest_event,dest_event->nextId(),svc_name,logline,track_str,
		 label_cart,track_cart,import_name,&autofill_errors);
      delete e;
      emit generationProgress(1+(24*current_link++)/total_links);
    }
    else {
      dest_event->insert(dest_event->size(),1,true);
      *(dest_event->logLine(dest_event->size()-1))=*logline;
      dest_event->logLine(dest_event->size()-1)->setId(dest_event->nextId());
    }
  }
  dest_event->save();

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

  sql=QString().sprintf("drop table `%s`",(const char *)import_name);
  q=new RDSqlQuery(sql);
  delete q;

  return true;
}


void RDSvc::clearLogLinks(RDSvc::ImportSource src,const QDate &date,
			  const QString &logname)
{
  std::vector<int> cleared_ids;
  RDLogLine::Type event_type=RDLogLine::UnknownType;
  RDLogLine::Source event_source=RDLogLine::Manual;
  switch(src) {
      case RDSvc::Music:
	event_type=RDLogLine::MusicLink;
	event_source=RDLogLine::Music;
	break;

      case RDSvc::Traffic:
	event_type=RDLogLine::TrafficLink;
	event_source=RDLogLine::Traffic;
	break;
  }

  RDLogEvent *src_event=new RDLogEvent(RDLog::tableName(logname));
  RDLogEvent *dest_event=new RDLogEvent(RDLog::tableName(logname));
  src_event->load();
  RDLogLine *logline=NULL;
  for(int i=0;i<src_event->size();i++) {
    logline=src_event->logLine(i);
    if((logline->linkId()>=0)&&(logline->source()==event_source)) {
      if(CheckId(&cleared_ids,logline->linkId())) {
	dest_event->insert(dest_event->size(),1);
	RDLogLine *lline=dest_event->logLine(dest_event->size()-1);
	lline->setId(dest_event->nextId());
	lline->setStartTime(RDLogLine::Logged,logline->linkStartTime());
	lline->setType(event_type);
	if(logline->linkEmbedded()) {
	  lline->setSource(RDLogLine::Music);
	}
	else {
	  lline->setSource(RDLogLine::Template);
	}
	lline->setTransType(logline->transType());
	lline->setLinkEventName(logline->linkEventName());
	lline->setLinkStartTime(logline->linkStartTime());
	lline->setLinkLength(logline->linkLength());
	lline->setLinkStartSlop(logline->linkStartSlop());
	lline->setLinkEndSlop(logline->linkEndSlop());
	lline->setLinkId(logline->linkId());
	lline->setLinkEmbedded(logline->linkEmbedded());
      }
    }
    else {
      dest_event->insert(dest_event->size(),1);
      *(dest_event->logLine(dest_event->size()-1))=*logline;
      dest_event->logLine(dest_event->size()-1)->setId(dest_event->nextId());
    }
  }
  dest_event->save();
  delete src_event;
  delete dest_event;
}


void RDSvc::create(const QString exemplar) const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  if(exemplar.isEmpty()) {  // Create Empty Service
    sql=QString("insert into SERVICES set NAME=\"")+
      RDEscapeString(svc_name)+"\","+
      "NAME_TEMPLATE=\""+RDEscapeString(svc_name)+"-%m%d\","+
      "DESCRIPTION_TEMPLATE=\""+RDEscapeString(svc_name)+" log for %d/%m/%Y\"";
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
			    (const char *)RDEscapeString(svc_name));
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
			    (const char *)RDEscapeString(svc_name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    for(int i=0;i<168;i++) {
      sql=QString("insert into SERVICE_CLOCKS set ")+
	"SERVICE_NAME=\""+RDEscapeString(svc_name)+"\","+
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
      "AUTO_REFRESH,"+           // 03
      "ELR_SHELFLIFE,"+          // 04
      "TFC_PATH,"+               // 05
      "TFC_WIN_PATH,"+           // 06
      "TFC_CART_OFFSET,"+        // 07
      "TFC_CART_LENGTH,"+        // 08
      "TFC_START_OFFSET,"+       // 09
      "TFC_START_LENGTH,"+       // 10
      "TFC_LENGTH_OFFSET,"+      // 11
      "TFC_LENGTH_LENGTH,"+      // 12
      "TFC_HOURS_OFFSET,"+       // 13
      "TFC_HOURS_LENGTH,"+       // 14
      "TFC_MINUTES_OFFSET,"+     // 15
      "TFC_MINUTES_LENGTH,"+     // 16
      "TFC_SECONDS_OFFSET,"+     // 17
      "TFC_SECONDS_LENGTH,"+     // 18
      "TFC_DATA_OFFSET,"+        // 19
      "TFC_DATA_LENGTH,"+        // 20
      "TFC_EVENT_ID_OFFSET,"+    // 21
      "TFC_EVENT_ID_LENGTH,"+    // 22
      "TFC_ANNC_TYPE_OFFSET,"+   // 23
      "TFC_ANNC_TYPE_LENGTH,"+   // 24
      "MUS_PATH,"+               // 25
      "MUS_WIN_PATH,"+           // 26
      "MUS_CART_OFFSET,"+        // 27
      "MUS_CART_LENGTH,"+        // 28
      "MUS_START_OFFSET,"+       // 29
      "MUS_START_LENGTH,"+       // 30
      "MUS_LENGTH_OFFSET,"+      // 31
      "MUS_LENGTH_LENGTH,"+      // 32
      "MUS_HOURS_OFFSET,"+       // 33
      "MUS_HOURS_LENGTH,"+       // 34
      "MUS_MINUTES_OFFSET,"+     // 35
      "MUS_MINUTES_LENGTH,"+     // 36
      "MUS_SECONDS_OFFSET,"+     // 37
      "MUS_SECONDS_LENGTH,"+     // 38
      "MUS_DATA_OFFSET,"+        // 39
      "MUS_DATA_LENGTH,"+        // 40
      "MUS_EVENT_ID_OFFSET,"+    // 41
      "MUS_EVENT_ID_LENGTH,"+    // 42
      "MUS_ANNC_TYPE_OFFSET,"+   // 43
      "MUS_ANNC_TYPE_LENGTH "+   // 44
      " from SERVICES where NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into SERVICES set ")+
	"NAME_TEMPLATE=\""+RDEscapeString(q->value(0).toString())+"\","+
	"DESCRIPTION_TEMPLATE=\""+RDEscapeString(q->value(1).toString())+"\","+
	"CHAIN_LOG=\""+RDEscapeString(q->value(2).toString())+"\","+
	"AUTO_REFRESH=\""+RDEscapeString(q->value(3).toString())+"\","+
	QString().sprintf("ELR_SHELFLIFE=%d,",q->value(4).toInt())+
	"TFC_PATH=\""+RDEscapeString(q->value(5).toString())+"\","+
	"TFC_WIN_PATH=\""+RDEscapeString(q->value(6).toString())+"\","+
	QString().sprintf("TFC_CART_OFFSET=%d,",q->value(7).toInt())+
	QString().sprintf("TFC_CART_LENGTH=%d,",q->value(8).toInt())+
	QString().sprintf("TFC_START_OFFSET=%d,",q->value(9).toInt())+
	QString().sprintf("TFC_START_LENGTH=%d,",q->value(10).toInt())+
	QString().sprintf("TFC_LENGTH_OFFSET=%d,",q->value(11).toInt())+
	QString().sprintf("TFC_LENGTH_LENGTH=%d,",q->value(12).toInt())+
	QString().sprintf("TFC_HOURS_OFFSET=%d,",q->value(13).toInt())+
	QString().sprintf("TFC_HOURS_LENGTH=%d,",q->value(14).toInt())+
	QString().sprintf("TFC_MINUTES_OFFSET=%d,",q->value(15).toInt())+
	QString().sprintf("TFC_MINUTES_LENGTH=%d,",q->value(16).toInt())+
	QString().sprintf("TFC_SECONDS_OFFSET=%d,",q->value(17).toInt())+
	QString().sprintf("TFC_SECONDS_LENGTH=%d,",q->value(18).toInt())+
	QString().sprintf("TFC_DATA_OFFSET=%d,",q->value(19).toInt())+
	QString().sprintf("TFC_DATA_LENGTH=%d,",q->value(20).toInt())+
	QString().sprintf("TFC_EVENT_ID_OFFSET=%d,",q->value(21).toInt())+
	QString().sprintf("TFC_EVENT_ID_LENGTH=%d,",q->value(22).toInt())+
	QString().sprintf("TFC_ANNC_TYPE_OFFSET=%d,",q->value(23).toInt())+
	QString().sprintf("TFC_ANNC_TYPE_LENGTH=%d,",q->value(24).toInt())+
	"MUS_PATH=\""+RDEscapeString(q->value(25).toString())+"\","+
	"MUS_WIN_PATH=\""+RDEscapeString(q->value(26).toString())+"\","+
	QString().sprintf("MUS_CART_OFFSET=%d,",q->value(27).toInt())+
	QString().sprintf("MUS_CART_LENGTH=%d,",q->value(28).toInt())+
	QString().sprintf("MUS_START_OFFSET=%d,",q->value(29).toInt())+
	QString().sprintf("MUS_START_LENGTH=%d,",q->value(30).toInt())+
	QString().sprintf("MUS_LENGTH_OFFSET=%d,",q->value(31).toInt())+
	QString().sprintf("MUS_LENGTH_LENGTH=%d,",q->value(32).toInt())+
	QString().sprintf("MUS_HOURS_OFFSET=%d,",q->value(33).toInt())+
	QString().sprintf("MUS_HOURS_LENGTH=%d,",q->value(34).toInt())+
	QString().sprintf("MUS_MINUTES_OFFSET=%d,",q->value(35).toInt())+
	QString().sprintf("MUS_MINUTES_LENGTH=%d,",q->value(36).toInt())+
	QString().sprintf("MUS_SECONDS_OFFSET=%d,",q->value(37).toInt())+
	QString().sprintf("MUS_SECONDS_LENGTH=%d,",q->value(38).toInt())+
	QString().sprintf("MUS_DATA_OFFSET=%d,",q->value(39).toInt())+
	QString().sprintf("MUS_DATA_LENGTH=%d,",q->value(40).toInt())+
	QString().sprintf("MUS_EVENT_ID_OFFSET=%d,",q->value(41).toInt())+
	QString().sprintf("MUS_EVENT_ID_LENGTH=%d,",q->value(42).toInt())+
	QString().sprintf("MUS_ANNC_TYPE_OFFSET=%d,",q->value(43).toInt())+
	QString().sprintf("MUS_ANNC_TYPE_LENGTH=%d,",q->value(44).toInt())+
	"NAME=\""+RDEscapeString(svc_name)+"\"";
      q=new RDSqlQuery(sql);
      delete q;
      sql=QString("select HOUR,CLOCK_NAME from SERVICE_CLOCKS where ")+
	"SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	sql=QString("insert into SERVICE_CLOCKS set ")+
	  "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\","+
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
			    (const char *)RDEscapeString(svc_name));
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
			    (const char *)RDEscapeString(svc_name));
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
			    (const char *)RDEscapeString(svc_name));
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
			    (const char *)RDEscapeString(svc_name));
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
			    (const char *)RDEscapeString(svc_name));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  //
  // Create Service Reconciliation Table
  //
  sql=RDCreateReconciliationTableSql(RDSvc::svcTableName(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSvc::remove() const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString logname;

  sql=QString("delete from AUDIO_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SERVICE_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("update RDAIRPLAY set ")+
    "DEFAULT_SERVICE=\"\" where "+
    "DEFAULT_SERVICE=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from EVENT_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from CLOCK_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from AUTOFILLS where ")+
    "SERVICE=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from REPORT_SERVICES where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SERVICES where ")+
    "NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SERVICE_CLOCKS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("select NAME from LOGS where ")+
    "SERVICE=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    logname=q->value(0).toString();
    logname.replace(" ","_");
    sql=QString("drop table `")+RDLog::tableName(logname)+"`";
    q1=new RDSqlQuery(sql);
    delete q1;
    sql=QString().sprintf("drop table `%s_REC`",(const char *)logname);
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;

  QString tablename=svc_name;
  tablename.replace(" ","_");
  sql=QString().sprintf("drop table `%s_SRT`",(const char *)tablename);
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString().sprintf("drop table `%s_STACK`",(const char *)tablename);
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from LOGS where ")+
    "SERVICE=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
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
