// rdsvc.cpp
//
// Abstract a Rivendell Service.
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>

#include <qmessagebox.h>

#include "debugvars.h"
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
RDSvc::RDSvc(QString svcname,QObject *parent,const char *name)
  : QObject(parent,name)
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
  QString fieldname=SourceString(src)+"_IMPORT_TEMPLATE";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setImportTemplate(ImportSource src,const QString &str) const
{
  QString fieldname=SourceString(src)+"_IMPORT_TEMPLATE";
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
  QString fieldname=SourceString(src)+"_TRACK_STRING";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setTrackString(ImportSource src,const QString &str)
{
  QString fieldname=SourceString(src)+"_TRACK_STRING";
  SetRow(fieldname,str);
}


QString RDSvc::labelCart(ImportSource src) const
{
  QString fieldname=SourceString(src)+"_LABEL_CART";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).toString();
}


void RDSvc::setLabelCart(ImportSource src,const QString &str)
{
  QString fieldname=SourceString(src)+"_LABEL_CART";
  SetRow(fieldname,str);
}


QString RDSvc::trackCart(ImportSource src) const
{
  QString fieldname=SourceString(src)+"_TRACK_CART";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).toString();
}


void RDSvc::setTrackCart(ImportSource src,const QString &str)
{
  QString fieldname=SourceString(src)+"_TRACK_CART";
  SetRow(fieldname,str);
}


QString RDSvc::importPath(ImportSource src,ImportOs os) const
{
  QString fieldname=SourceString(src)+"_"+OsString(os)+"PATH";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setImportPath(ImportSource src,ImportOs os,const QString &path) 
  const
{
  QString fieldname=SourceString(src)+"_"+OsString(os)+"PATH";
  SetRow(fieldname,path);
}


QString RDSvc::preimportCommand(ImportSource src,ImportOs os) const
{
  QString fieldname=SourceString(src)+"_"+OsString(os)+"PREIMPORT_CMD";
  return RDGetSqlValue("SERVICES","NAME",svc_name,fieldname).
    toString();
}


void RDSvc::setPreimportCommand(ImportSource src,ImportOs os,
				const QString &path) const
{
  QString fieldname=SourceString(src)+"_"+OsString(os)+"PREIMPORT_CMD";
  SetRow(fieldname,path);
}


int RDSvc::importOffset(ImportSource src,ImportField field,
			const QString &templ) const
{
  int ret=0;
  QString sql="";

  if(templ.isEmpty()) {
    sql=QString("select OFFSET from LOG_PARSERS where ")+
      "(SERVICE_NAME=\""+RDEscapeString(svc_name)+"\")&&"+
      "(CLASS=\""+RDEscapeString(SourceString(src))+"\")&&"+
      "(PARAMETER=\""+RDEscapeString(FieldString(field))+"\")";
  }
  else {
    sql=QString("select OFFSET from PARSER_TEMPLATES where ")+
      "(NAME=\""+RDEscapeString(templ)+"\")&&"+
      "(PARAMETER=\""+RDEscapeString(FieldString(field))+"\")";
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toInt();
  }
  delete q;

  return ret;
}


void RDSvc::setImportOffset(ImportSource src,ImportField field,int offset) 
  const
{
  QString sql=QString("update LOG_PARSERS set ")+
    QString().sprintf("OFFSET=%d where ",offset)+
    "(SERVICE_NAME=\""+RDEscapeString(svc_name)+"\")&&"+
    "(CLASS=\""+RDEscapeString(SourceString(src))+"\")&&"+
    "(PARAMETER=\""+RDEscapeString(FieldString(field))+"\")";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


int RDSvc::importLength(ImportSource src,ImportField field,
			const QString &templ) const
{
  int ret=0;
  QString sql="";

  if(templ.isEmpty()) {
    sql=QString("select LENGTH from LOG_PARSERS where ")+
      "(SERVICE_NAME=\""+RDEscapeString(svc_name)+"\")&&"+
      "(CLASS=\""+RDEscapeString(SourceString(src))+"\")&&"+
      "(PARAMETER=\""+RDEscapeString(FieldString(field))+"\")";
  }
  else {
    sql=QString("select LENGTH from PARSER_TEMPLATES where ")+
      "(NAME=\""+RDEscapeString(templ)+"\")&&"+
      "(PARAMETER=\""+RDEscapeString(FieldString(field))+"\")";
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toInt();
  }
  delete q;

  return ret;
}


void RDSvc::setImportLength(ImportSource src,ImportField field,int len) const
{
  QString sql=QString("update LOG_PARSERS set ")+
    QString().sprintf("LENGTH=%d where ",len)+
    "(SERVICE_NAME=\""+RDEscapeString(svc_name)+"\")&&"+
    "(CLASS=\""+RDEscapeString(SourceString(src))+"\")&&"+
    "(PARAMETER=\""+RDEscapeString(FieldString(field))+"\")";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
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

      case RDSvc::NoSource:
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
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  char buf[RD_MAX_IMPORT_LINE_LENGTH];
  FILE *infile;
  int hour=0;
  int min=0;
  int sec=0;
  int len=0;

  //
  // Get OS Type
  //
#ifdef WIN32
  QString os_flag="_WIN";
#else
  QString os_flag="";
#endif  // WIN32

  //
  // Load Parser Parameters
  //
  sql=QString("select ")+RDSvc::className(src)+os_flag+"_PATH,"+
    RDSvc::className(src)+"_LABEL_CART,"+
    RDSvc::className(src)+"_TRACK_CART,"+
    RDSvc::className(src)+os_flag+"_PREIMPORT_CMD "+
    "from SERVICES where NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
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

  //
  // Create Import Table
  //
  sql=QString("drop table `")+dest_table+"`";
  QSqlQuery *qq;          // Use QSqlQuery so we don't generate a 
  qq=new QSqlQuery(sql);  // spurious error message.
  delete qq;

  sql=QString("create table `")+dest_table+"` ("+
    "ID int primary key auto_increment,"+
    "TYPE int not null,"+
    "START_HOUR int not null,"+
    "START_SECS int not null,"+
    "CART_NUMBER int unsigned,"+
    "TITLE char(255),"+
    "LENGTH int,"+
    QString().sprintf("TRANS_TYPE int default %d,",RDLogLine::NoTrans)+
    "TIME_TYPE int,"+
    "GRACE_TIME int,"+
    "EXT_DATA char(32),"+
    "EXT_EVENT_ID char(32),"+
    "EXT_ANNC_TYPE char(8),"+
    "EXT_CART_NAME char(32),"+
    "EVENT_USED enum('N','Y') default 'N',"+
    "index START_TIME_IDX (START_HOUR,START_SECS),"+
    "index TYPE_IDX(TYPE,LENGTH))";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Now we're ready to Do The Business
  //
  while(fgets(buf,RD_MAX_IMPORT_LINE_LENGTH,infile)!=NULL) {
    GetImportLine(buf,src,date,break_str,track_str,label_cart,
		  dest_table,&hour,&min,&sec);
  }

  //
  // Attempt to autodetect any missing Break Event lengths
  //
  sql=QString("select ID,START_HOUR,START_SECS from `")+dest_table+"` where "+
    QString().sprintf("(LENGTH is null)&&(TYPE=%d)",RDSvc::Break);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select START_HOUR,START_SECS from `")+dest_table+"` where "+
      QString().sprintf("ID=%d",q->value(0).toInt()+1);
    q1=new RDSqlQuery(sql);
    if(q1->first()) {
      len=1000*((3600*q1->value(0).toInt()+q1->value(1).toInt())-
               (3600*q->value(1).toInt()+q->value(2).toInt()));
    }
    else {  // Last event, use 23:59:59
      len=1000*(86399-(3600*q->value(1).toInt()+q->value(2).toInt()));
     }
    if(len<0) {  // Events not in chronological order, so
      len=0;     // we cannot autodetect length!
    }
    sql=QString("update `")+dest_table+"` set "+
      QString().sprintf("LENGTH=%d ",len)+
      QString().sprintf(" where ID=%d",q->value(0).toInt());
    q2=new RDSqlQuery(sql);
    delete q2;
    delete q1;
  }
  delete q;

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
  QString purge_date;
  if(defaultLogShelflife()>=0) {
    purge_date=date.addDays(defaultLogShelflife()).toString("yyyy-MM-dd");
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
    if(!purge_date.isEmpty()) {
      sql+=(",PURGE_DATE=\""+purge_date+"\"");
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
                           PURGE_DATE=\"%s\"",
			  (const char *)RDEscapeString(logname),
			  (const char *)RDEscapeString(svc_name),
			  (const char *)RDEscapeString(RDDateDecode(descriptionTemplate(),date)),
			  "RDLogManager",
			  (const char *)purge_date);
    q=new RDSqlQuery(sql);
    delete q;
  }
  RDCreateLogTable(RDLog::tableName(logname));
  emit generationProgress(1);

  //
  // Generate Events
  //
  for(int i=0;i<24;i++) {
    sql=QString("select CLOCK_NAME from SVC_CLOCKS ")+
      "where (SERVICE_NAME=\""+RDEscapeString(svc_name)+"\")&&"+
      QString().
      sprintf("(HOUR=%d)",RDClock::svcClockNumber(date.dayOfWeek(),i));
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

      case RDSvc::NoSource:
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

  sql=QString("select ")+
    "`"+import_name+"`.START_HOUR,"+
    "`"+import_name+"`.START_SECS,"+
    "`"+import_name+"`.CART_NUMBER,"+
    "CART.TITLE from `"+import_name+"` LEFT JOIN CART "+
    "ON `"+import_name+"`.CART_NUMBER=CART.NUMBER "+
    "where `"+import_name+"`.EVENT_USED=\"N\"";
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
  if(getenv(RD_DEBUG_KEEP_IMPORT_TABLE)==NULL) {
    sql=QString("drop table `")+import_name+"`";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    printf("RDSvc - IMPORT TABLE: %s\n",(const char *)import_name);
  }

  return true;
}


void RDSvc::clearLogLinks(RDSvc::ImportSource src,const QString &logname)
{
  RDLogLine::Source event_source=RDLogLine::Manual;
  switch(src) {
      case RDSvc::Music:
	event_source=RDLogLine::Music;
	break;

      case RDSvc::Traffic:
	event_source=RDLogLine::Traffic;
	break;

      case RDSvc::NoSource:
        break;
  }

  RDLogEvent *src_event=new RDLogEvent(RDLog::tableName(logname));
  RDLogEvent *dest_event=new RDLogEvent(RDLog::tableName(logname));
  src_event->load();
  RDLogLine *logline=NULL;
  for(int i=0;i<src_event->size();i++) {
    logline=src_event->logLine(i);
    if(logline->source()!=event_source) {
      dest_event->insert(dest_event->size(),1);
      *(dest_event->logLine(dest_event->size()-1))=*logline;
      dest_event->logLine(dest_event->size()-1)->setId(dest_event->nextId());
    }
  }

  //
  // Calculate new traffic link count
  //
  int tfc_links=0;
  if(src==RDSvc::Music) {
    for(int i=0;i<dest_event->size();i++) {
      if(dest_event->logLine(i)->type()==RDLogLine::TrafficLink) {
       tfc_links++;
      }
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
  QStringList classes=RDSvc::importerClassList();
  QStringList parameters=RDSvc::importerParameterList();

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

    //
    // Create Clock Entries
    //
    for(int i=0;i<168;i++) {
      sql=QString("insert into SVC_CLOCKS set ")+
       "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\","+
       QString().sprintf("HOUR=%d",i);
      q=new RDSqlQuery(sql);
      delete q;
    }

    //
    // Create Log Parser Entries
    //
    for(unsigned i=0;i<classes.size();i++) {
      for(unsigned j=0;j<parameters.size();j++) {
       sql=QString("insert into LOG_PARSERS set ")+
         "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\","+
         "CLASS=\""+RDEscapeString(classes[i])+"\","+
         "PARAMETER=\""+RDEscapeString(parameters[j])+"\"";
       q=new RDSqlQuery(sql);
       delete q;
      }
    }
  }
  else {    // Base on Existing Service
    sql=QString("select NAME_TEMPLATE,DESCRIPTION_TEMPLATE,")+
      "CHAIN_LOG,AUTO_REFRESH,ELR_SHELFLIFE,TFC_PATH,TFC_WIN_PATH,"+
      "MUS_PATH,MUS_WIN_PATH "+
      "from SERVICES where NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into SERVICES set ")+
       "NAME_TEMPLATE=\""+RDEscapeString(q->value(0).toString())+"\","+
       "DESCRIPTION_TEMPLATE=\""+RDEscapeString(q->value(1).toString())+"\","+
       "CHAIN_LOG=\""+q->value(2).toString()+"\","+
       "AUTO_REFRESH=\""+q->value(3).toString()+"\","+
       QString().sprintf("ELR_SHELFLIFE=%d,",q->value(4).toInt())+
       "TFC_PATH=\""+q->value(5).toString()+"\","+
       "TFC_WIN_PATH=\""+q->value(6).toString()+"\","+
       "MUS_PATH=\""+q->value(7).toString()+"\","+
       "MUS_WIN_PATH=\""+q->value(8).toString()+"\","+
       "NAME=\""+RDEscapeString(svc_name)+"\"";
     q=new RDSqlQuery(sql);
      delete q;
    }

    //
    // Copy Service Clock Entries
    //
    sql=QString("select HOUR,CLOCK_NAME from SVC_CLOCKS ")+
      "where SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into SVC_CLOCKS set ")+
       "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\","+
       QString().sprintf("HOUR=%d",q->value(0).toInt());
      if(!q->value(1).toString().isEmpty()) {
       sql+=",CLOCK_NAME=\""+RDEscapeString(q->value(1).toString())+"\"";
      }
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Copy Log Parser Parameters
    //
    sql=QString("select CLASS,PARAMETER,OFFSET,LENGTH from LOG_PARSERS where ")+
      "SERVICE_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into LOG_PARSERS set ")+
       "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\","+
       "CLASS=\""+RDEscapeString(q->value(0).toString())+"\","+
       "PARAMETER=\""+RDEscapeString(q->value(1).toString())+"\","+
       QString().sprintf("OFFSET=%d,",q->value(2).toInt())+
       QString().sprintf("LENGTH=%d",q->value(3).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

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

  sql=QString().sprintf("delete from AUDIO_PERMS where SERVICE_NAME=\"%s\"",
			(const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from SERVICE_PERMS where SERVICE_NAME=\"%s\"",
		      (const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("update RDAIRPLAY set DEFAULT_SERVICE=\"\" \
                         where DEFAULT_SERVICE=\"%s\"",
			(const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from EVENT_PERMS where SERVICE_NAME=\"%s\"",
		      (const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from CLOCK_PERMS where SERVICE_NAME=\"%s\"",
		      (const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from AUTOFILLS where SERVICE=\"%s\"",
		      (const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from REPORT_SERVICES where SERVICE_NAME=\"%s\"",
		      (const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from SERVICES where NAME=\"%s\"",
			(const char *)RDEscapeString(svc_name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from SVC_CLOCKS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString().sprintf("select NAME from LOGS where SERVICE=\"%s\"",
			(const char *)RDEscapeString(svc_name));
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

  sql=QString().sprintf("delete from LOGS where SERVICE=\"%s\"",
			(const char *)RDEscapeString(svc_name));
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


QString RDSvc::className(ImportSource src)
{
  QString ret;

  switch(src) {
  case RDSvc::Traffic:
    ret="TFC";
    break;

  case RDSvc::Music:
    ret="MUS";
    break;

  case RDSvc::NoSource:
    break;
  }

  return ret;
}


QStringList RDSvc::importerClassList()
{
  QStringList ret;

  ret.push_back("MUS");
  ret.push_back("TFC");

  return ret;
}


QStringList RDSvc::importerParameterList()
{
  QStringList ret;

  ret.push_back("CART");
  ret.push_back("TITLE");
  ret.push_back("START");
  ret.push_back("HOURS");
  ret.push_back("MINUTES");
  ret.push_back("SECONDS");
  ret.push_back("LEN_HOURS");
  ret.push_back("LEN_MINUTES");
  ret.push_back("LEN_SECONDS");
  ret.push_back("LENGTH");
  ret.push_back("DATA");
  ret.push_back("EVENT_ID");
  ret.push_back("ANNC_TYPE");
  ret.push_back("TIME_TYPE");
  ret.push_back("WAIT_SECONDS");
  ret.push_back("WAIT_MINUTES");
  ret.push_back("TRANS_TYPE");

  return ret;
}


QString RDSvc::timeString(int hour,int secs)
{
  return QString().sprintf("%02d:%02d:%02d",hour,secs/60,secs%60);
}


void RDSvc::GetImportLine(const QString &line,ImportSource src,
                         const QDate &date,const QString &break_str,
                         const QString &track_str,const QString &label_cart,
                         const QString &dest_table,
                         int *prev_hour,int *prev_min,int *prev_sec) const
{
  QString sql;
  RDSqlQuery *q;
  bool start_time_ok=true;
  bool ok=false;
  
  //
  // Required Fields
  //
  QString cartname=GetImportField(line,src,RDSvc::CartNumber);
  unsigned cartnum=cartname.toUInt(&ok);
  int hours=GetImportField(line,src,RDSvc::StartHours).toInt(&ok);
  if(!ok) {
    start_time_ok=false;
  }
  int minutes=GetImportField(line,src,RDSvc::StartMinutes).toInt(&ok);
  if(!ok) {
    start_time_ok=false;
  }
  int seconds=GetImportField(line,src,RDSvc::StartSeconds).toInt(&ok);
  if(!ok) {
    start_time_ok=false;
  }
  if(start_time_ok) {
    *prev_hour=hours;
    *prev_min=minutes;
    *prev_sec=seconds;
  }

  //
  // Length
  //
  int len=3600*GetImportField(line,src,RDSvc::LengthHours).toInt()+
    60*GetImportField(line,src,RDSvc::LengthMinutes).toInt()+
    GetImportField(line,src,RDSvc::LengthSeconds).toInt();

  //
  // Transition Type
  //
  RDLogLine::TransType trans_type=RDLogLine::NoTrans;
  if(GetImportField(line,src,RDSvc::TransitionType).lower()=="play") {
    trans_type=RDLogLine::Play;
  }
  if(GetImportField(line,src,RDSvc::TransitionType).lower()=="segue") {
    trans_type=RDLogLine::Segue;
  }
  if(GetImportField(line,src,RDSvc::TransitionType).lower()=="stop") {
    trans_type=RDLogLine::Stop;
  }

  //
  // Time Type
  //
  RDLogLine::TimeType time_type=RDLogLine::NoTime;
  int grace_time=0;
  if(GetImportField(line,src,RDSvc::TimeType).lower()=="h") {
    time_type=RDLogLine::Hard;
    grace_time=60000*GetImportField(line,src,RDSvc::TimeWaitMinutes).toInt()+
      1000*GetImportField(line,src,RDSvc::TimeWaitSeconds).toInt();
  }
  if(GetImportField(line,src,RDSvc::TimeType).lower()=="s") {
    time_type=RDLogLine::Hard;
    grace_time=-1;
  }

  //
  // Write to import table
  //
  if(start_time_ok&&(cartnum>0)&&(cartnum<=RD_MAX_CART_NUMBER)) {
    // Cart Event
    sql=QString("insert into `")+dest_table+"` set "+
      QString().sprintf("TYPE=%d,",RDSvc::Cart)+
      QString().sprintf("START_HOUR=%d,",hours)+
      QString().sprintf("START_SECS=%d,",60*minutes+seconds)+
      QString().sprintf("CART_NUMBER=%u,",cartnum)+
      "TITLE=\""+RDEscapeString(GetImportField(line,src,RDSvc::Title).
                               stripWhiteSpace())+"\","+
      QString().sprintf("LENGTH=%d,",1000*len)+
      QString().sprintf("TRANS_TYPE=%d,",trans_type)+
      QString().sprintf("TIME_TYPE=%d,",time_type)+
      QString().sprintf("GRACE_TIME=%d,",grace_time)+
      "EXT_DATA=\""+
      RDEscapeString(GetImportField(line,src,RDSvc::ExtData))+"\","+
      "EXT_EVENT_ID=\""+
      RDEscapeString(GetImportField(line,src,RDSvc::ExtEventId))+"\","+
      "EXT_ANNC_TYPE=\""+
      RDEscapeString(GetImportField(line,src,RDSvc::ExtAnncType))+"\","+
      "EXT_CART_NAME=\""+
      RDEscapeString(GetImportField(line,src,RDSvc::CartNumber))+"\"";
    q=new RDSqlQuery(sql);
    delete q;

  }
  else {
    if((!label_cart.isEmpty())&&(cartname==label_cart)) {
      // Note Cart Entry
      sql=QString("insert into `")+dest_table+"` set "+
	QString().sprintf("TYPE=%d,",RDSvc::Label)+
	QString().sprintf("START_HOUR=%d,",*prev_hour)+
	QString().sprintf("START_SECS=%d,",60*(*prev_min)+(*prev_sec))+
	"TITLE=\""+RDEscapeString(GetImportField(line,src,RDSvc::Title))+"\","+
	QString().sprintf("LENGTH=%d,",1000*len)+
	QString().sprintf("TRANS_TYPE=%d,",trans_type)+
	QString().sprintf("TIME_TYPE=%d,",time_type)+
	QString().sprintf("GRACE_TIME=%d,",grace_time)+
	"EXT_DATA=\""+
	RDEscapeString(GetImportField(line,src,RDSvc::ExtData))+"\","+
	"EXT_EVENT_ID=\""+
	RDEscapeString(GetImportField(line,src,RDSvc::ExtEventId))+"\","+
	"EXT_ANNC_TYPE=\""+
	RDEscapeString(GetImportField(line,src,RDSvc::ExtAnncType))+"\","+
	"EXT_CART_NAME=\""+
	RDEscapeString(GetImportField(line,src,RDSvc::CartNumber))+"\"";
      q=new RDSqlQuery(sql);
      delete q;
    }
    if((!break_str.isEmpty())&&line.stripWhiteSpace().contains(break_str)) {
      // Break Event
      sql=QString("insert into `")+dest_table+"` set "+
       QString().sprintf("TYPE=%d,",RDSvc::Break)+
       QString().sprintf("START_HOUR=%d,",*prev_hour)+
       QString().sprintf("START_SECS=%d,",60*(*prev_min)+(*prev_sec))+
       "TITLE=\""+tr("[spot break]")+"\"";
      if(len>0) {
       sql+=QString().sprintf(",LENGTH=%d",1000*len);
      }
      q=new RDSqlQuery(sql);
      delete q;
    }
    if((!track_str.isEmpty())&&line.stripWhiteSpace().contains(track_str)) {
      // VoiceTrack Event
      sql=QString("insert into `")+dest_table+"` set "+
       QString().sprintf("TYPE=%d,",RDSvc::Track)+
       QString().sprintf("START_HOUR=%d,",*prev_hour)+
       QString().sprintf("START_SECS=%d,",60*(*prev_min)+(*prev_sec))+
       "TITLE=\""+RDEscapeString(track_str)+"\"";
      q=new RDSqlQuery(sql);
      delete q;
    }
  }
}


QString RDSvc::GetImportField(const QString &line,ImportSource src,
                              ImportField fld) const
{
  return line.mid(importOffset(src,fld,importTemplate(src)),
                 importLength(src,fld,importTemplate(src))).stripWhiteSpace();
}


QString RDSvc::SourceString(ImportSource src) const
{
  QString fieldname;
  switch(src) {
      case RDSvc::Traffic:
	fieldname="TFC";
	break;

      case RDSvc::Music:
	fieldname="MUS";
	break;

      case RDSvc::NoSource:
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
	fieldname="CART";
	break;

      case RDSvc::Title:
	fieldname="TITLE";
	break;

      case RDSvc::StartHours:
	fieldname="HOURS";
	break;

      case RDSvc::StartMinutes:
	fieldname="MINUTES";
	break;

      case RDSvc::StartSeconds:
	fieldname="SECONDS";
	break;

      case RDSvc::LengthHours:
	fieldname="LEN_HOURS";
	break;

      case RDSvc::LengthMinutes:
	fieldname="LEN_MINUTES";
	break;

      case RDSvc::LengthSeconds:
	fieldname="LEN_SECONDS";
	break;

      case RDSvc::TimeType:
       fieldname="TIME_TYPE";
       break;

      case RDSvc::TimeWaitMinutes:
       fieldname="WAIT_MINUTES";
       break;

      case RDSvc::TimeWaitSeconds:
       fieldname="WAIT_SECONDS";
       break;

      case RDSvc::TransitionType:
       fieldname="TRANS_TYPE";
       break;

      case RDSvc::ExtData:
	fieldname="DATA";
	break;

      case RDSvc::ExtEventId:
	fieldname="EVENT_ID";
	break;

      case RDSvc::ExtAnncType:
	fieldname="ANNC_TYPE";
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
  QString sql=QString("select ")+
    src_str+"_BREAK_STRING,"+
    src_str+"_TRACK_STRING,"+
    src_str+"_LABEL_CART,"+
    src_str+"_TRACK_CART "+
    "from SERVICES where NAME=\""+RDEscapeString(svc_name)+"\"";
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


int RDSvc::GetTimeDiff(int hours1,int secs1,int hours2,int secs2) const
{
  return (3600*hours1+secs1)-(3600*hours2+secs2);
}
