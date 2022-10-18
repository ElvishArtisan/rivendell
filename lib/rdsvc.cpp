// rdsvc.cpp
//
// Abstract a Rivendell Service.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QMessageBox>

#include "rdapplication.h"
#include "rdclock.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rd.h"
#include "rdescape_string.h"
#include "rdevent_line.h"
#include "rdlogmodel.h"
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


bool RDSvc::bypassMode() const
{
  return RDBool(RDGetSqlValue("SERVICES","NAME",svc_name,"BYPASS_MODE").
    toString());
}


void RDSvc::setBypassMode(bool state) const
{
  SetRow("BYPASS_MODE",RDYesNo(state));
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


bool RDSvc::includeImportMarkers(RDSvc::ImportSource src) const
{
  if(src==RDSvc::Music) {
    return 
      RDBool(RDGetSqlValue("SERVICES","NAME",svc_name,
			   "INCLUDE_MUS_IMPORT_MARKERS").toString());
  }
  return 
    RDBool(RDGetSqlValue("SERVICES","NAME",svc_name,
			 "INCLUDE_TFC_IMPORT_MARKERS").toString());
}


void RDSvc::setIncludeImportMarkers(RDSvc::ImportSource src,bool state)
{
  if(src==RDSvc::Music) {
    SetRow("INCLUDE_MUS_IMPORT_MARKERS",RDYesNo(state));
  }
  else {
    SetRow("INCLUDE_TFC_IMPORT_MARKERS",RDYesNo(state));
  }
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
  QString sql=QString("select `")+
    src_str+os_flag+"_PATH` from `SERVICES` where "+
    "`NAME`='"+RDEscapeString(svc_name)+"'";
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
		   const QString &track_str,bool resolve_implied_times) const
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
  int grace_time=0;
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
    "`"+src_str+os_flag+"_PATH`,"+           // 00
    "`"+src_str+"_LABEL_CART`,"+             // 01
    "`"+src_str+"_TRACK_CART`,"+             // 02
    "`"+src_str+os_flag+"_PREIMPORT_CMD`,"+  // 03
    "`SUB_EVENT_INHERITANCE` "+            // 04
    "from `SERVICES` where "+
    "`NAME`='"+RDEscapeString(svc_name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  QString infilename=q->value(0).toString();
  QString label_cart=q->value(1).toString().trimmed();
  QString track_cart=q->value(2).toString().trimmed();
  QString preimport_cmd=q->value(3).toString();
  RDSvc::SubEventInheritance inherit=
    (RDSvc::SubEventInheritance)q->value(4).toUInt();
  delete q;

  //
  // Open Source File
  //
  if((infile=
      fopen(RDDateDecode(infilename.toUtf8(),date,svc_station,svc_config,svc_name).toUtf8(),"r"))==
     NULL) {
    return false;
  }

  //
  // Run Preimport Command
  //
  if(!preimport_cmd.isEmpty()) {
    RDCheckExitCode("RDSvc::import system",
		    system(RDDateDecode(preimport_cmd,date,svc_station,
					svc_config,svc_name).toUtf8()));
  }

  QString parser_table;
  QString parser_name;
  if(importTemplate(src).isEmpty()) {
    src_str+="_";
    parser_table="`SERVICES`";
    parser_name=svc_name;
  }
  else {
    src_str="";
    parser_table="`IMPORT_TEMPLATES`";
    parser_name=importTemplate(src);
  }
  sql=QString("select ")+
    "`"+src_str+"CART_OFFSET`,"+         // 00
    "`"+src_str+"CART_LENGTH`,"+         // 01
    "`"+src_str+"DATA_OFFSET`,"+         // 02
    "`"+src_str+"DATA_LENGTH`,"+         // 03
    "`"+src_str+"EVENT_ID_OFFSET`,"+     // 04
    "`"+src_str+"EVENT_ID_LENGTH`,"+     // 05
    "`"+src_str+"ANNC_TYPE_OFFSET`,"+    // 06
    "`"+src_str+"ANNC_TYPE_LENGTH`,"+    // 07
    "`"+src_str+"TITLE_OFFSET`,"+        // 08
    "`"+src_str+"TITLE_LENGTH`,"+        // 09
    "`"+src_str+"HOURS_OFFSET`,"+        // 10
    "`"+src_str+"HOURS_LENGTH`,"+        // 11
    "`"+src_str+"MINUTES_OFFSET`,"+      // 12
    "`"+src_str+"MINUTES_LENGTH`,"+      // 13
    "`"+src_str+"SECONDS_OFFSET`,"+      // 14
    "`"+src_str+"SECONDS_LENGTH`,"+      // 15
    "`"+src_str+"LEN_HOURS_OFFSET`,"+    // 16
    "`"+src_str+"LEN_HOURS_LENGTH`,"+    // 17
    "`"+src_str+"LEN_MINUTES_OFFSET`,"+  // 18
    "`"+src_str+"LEN_MINUTES_LENGTH`,"+  // 19
    "`"+src_str+"LEN_SECONDS_OFFSET`,"+  // 20
    "`"+src_str+"LEN_SECONDS_LENGTH`,"+  // 21
    "`"+src_str+"TRANS_TYPE_OFFSET`,"+   // 22
    "`"+src_str+"TRANS_TYPE_LENGTH`,"+   // 23
    "`"+src_str+"TIME_TYPE_OFFSET`,"+    // 24
    "`"+src_str+"TIME_TYPE_LENGTH` "+    // 25
    "from "+parser_table+" where `NAME`='"+RDEscapeString(parser_name)+"'";
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
  int trans_type_offset=q->value(22).toInt();
  int trans_type_length=q->value(23).toInt();
  int time_type_offset=q->value(24).toInt();
  int time_type_length=q->value(25).toInt();

  delete q;

  //
  // Setup Data Source and Destination
  //
  sql=QString("delete from `IMPORTER_LINES` where ")+
    "`STATION_NAME`='"+RDEscapeString(svc_station->name())+"' && "+
    QString::asprintf("`PROCESS_ID`=%u",getpid());
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
    QString trans_type_buf;
    RDLogLine::TransType trans_type=RDLogLine::NoTrans;
    QString time_type_buf;
    RDLogLine::TimeType time_type=RDLogLine::Relative;

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
    // Transition Type
    //
    trans_type_buf=str_buf.mid(trans_type_offset,trans_type_length).trimmed();
    if(trans_type_buf.toLower()=="play") {
      trans_type=RDLogLine::Play;
    }
    if(trans_type_buf.toLower()=="segue") {
      trans_type=RDLogLine::Segue;
    }
    if(trans_type_buf.toLower()=="stop") {
      trans_type=RDLogLine::Stop;
    }

    //
    // Time Type
    //
    time_type_buf=str_buf.mid(time_type_offset,time_type_length).trimmed();
    if(time_type_buf.toLower()=="soft") {
      time_type=RDLogLine::Hard;
      grace_time=-1;
    }
    if(time_type_buf.toLower()=="hard") {
      time_type=RDLogLine::Hard;
      grace_time=0;
    }

    //
    // Process Line
    //
    cartnum=cartname.toUInt(&cart_ok);

    //
    // Common SQL Elements
    //
    sql=QString("insert into `IMPORTER_LINES` set ")+
      "`STATION_NAME`='"+RDEscapeString(svc_station->name())+"',"+
      QString::asprintf("`TRANS_TYPE`=%d,",trans_type)+
      QString::asprintf("`TIME_TYPE`=%d,",time_type)+
      QString::asprintf("`GRACE_TIME`=%d,",grace_time)+
      QString::asprintf("`PROCESS_ID`=%d,",getpid())+
      QString::asprintf("`FILE_LINE`=%u,",file_line)+
      QString::asprintf("`LINE_ID`=%d,",line_id);
    if(start_time_ok) {
      sql+=QString::asprintf("`START_HOUR`=%d,",start_hour)+
	QString::asprintf("`START_SECS`=%d,",60*start_minutes+start_seconds);
    }
    if(cartlen>=0) {
      sql+=QString::asprintf("`LENGTH`=%d,",cartlen);
    }

    //
    // Cart
    //
    if(start_time_ok&&cart_ok&&(cartnum>0)&&(cartnum<=RD_MAX_CART_NUMBER)) {
      sql+=QString::asprintf("`TYPE`=%u,",RDLogLine::Cart)+
	"`EXT_DATA`='"+data_buf.trimmed()+"',"+
	"`EXT_EVENT_ID`='"+eventid_buf.trimmed()+"',"+
	"`EXT_ANNC_TYPE`='"+annctype_buf.trimmed()+"',"+
	"`EXT_CART_NAME`='"+cartname.trimmed()+"',"+
	QString::asprintf("`CART_NUMBER`=%u,",cartnum)+
	"`TITLE`='"+RDEscapeString(title)+"'";
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
	sql+=QString::asprintf("`TYPE`=%u ",RDLogLine::TrafficLink);
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
      sql+=QString::asprintf("`TYPE`=%u,",RDLogLine::Track);
      switch(inherit) {
      case RDSvc::ParentEvent:
	sql+="`TITLE`='"+RDEscapeString(str_buf.simplified().trimmed())+"'";
	break;

      case RDSvc::SchedFile:
	sql+="`TITLE`='"+RDEscapeString(title)+"'";
	break;
      }
      RDSqlQuery::apply(sql);
      line_id++;
      file_line++;
      continue;
    }

    //
    // Label/Note Cart
    //
    if((!label_cart.isEmpty())&&(str_buf.contains(label_cart))) {
      sql+=QString::asprintf("`TYPE`=%u,",RDLogLine::Marker);
      switch(inherit) {
      case RDSvc::ParentEvent:
	sql+="`TITLE`='"+RDEscapeString(str_buf.simplified().trimmed())+"'";
	break;

      case RDSvc::SchedFile:
	sql+="`TITLE`='"+RDEscapeString(title)+"'";
	break;
      }
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

  //
  // Resolve Implied Start Time/Duration for Inline Events
  //
  if(resolve_implied_times&&subEventInheritance()==RDSvc::ParentEvent) {
    int prev_hour=0;
    int prev_secs=0;
    int prev_length=0;
    QList<unsigned> prev_ids;

    sql=QString("select ")+
      "`ID`,"+          // 00
      "`START_HOUR`,"+  // 01
      "`START_SECS`,"+  // 02
      "`LENGTH` "+      // 03
      "from `IMPORTER_LINES` where "+
      "`STATION_NAME`='"+RDEscapeString(svc_station->name())+"' && "+
      QString::asprintf("`PROCESS_ID`=%d ",getpid())+
      "order by `FILE_LINE`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if((!q->value(1).isNull())&&(!q->value(2).isNull())&&
	 (!q->value(3).isNull())) {
	if(prev_ids.size()>0) {
	  int len=1000*(q->value(2).toInt()-prev_secs)-prev_length;
	  if(len<0) {
	    len=0;
	  }
	  sql=QString("update `IMPORTER_LINES` set ")+
	    QString::asprintf("`START_HOUR`=%d,",prev_hour)+
	    QString::asprintf("`START_SECS`=%d,",prev_secs+prev_length/1000)+
	    QString::asprintf("`LENGTH`=%d ",len)+
	    "where ";
	  for(int i=0;i<prev_ids.size();i++) {
	    sql+=QString::asprintf("(`ID`=%u)||",prev_ids.at(i));
	  }
	  sql=sql.left(sql.length()-2);
	  RDSqlQuery::apply(sql);
	  prev_ids.clear();
	}
	prev_hour=q->value(1).toInt();
	prev_secs=q->value(2).toInt();
	prev_length=q->value(3).toInt();
      }
      else {
	prev_ids.push_back(q->value(0).toUInt());
      }
    }
    delete q;

    //
    // Handle trailing implied start time events
    //
    if(prev_ids.size()>0) {
      sql=QString("update `IMPORTER_LINES` set ")+
	QString::asprintf("`START_HOUR`=%d,",prev_hour)+
	QString::asprintf("`START_SECS`=%d,",prev_secs+prev_length/1000)+
	"`LENGTH`=0 "+
	"where ";
      for(int i=0;i<prev_ids.size();i++) {
	sql+=QString::asprintf("(`ID`=%u)||",prev_ids.at(i));
      }
      sql=sql.left(sql.length()-2);
      RDSqlQuery::apply(sql);
      prev_ids.clear();
    }
  }

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
  int count=0;

  if((!date.isValid()||logname.isEmpty())) {
    return false;
  }

  emit generationProgress(0);
  qApp->processEvents();

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
  log->setIncludeImportMarkers(RDLog::SourceMusic,
			       includeImportMarkers(RDSvc::Music));
  log->setIncludeImportMarkers(RDLog::SourceTraffic,
			       includeImportMarkers(RDSvc::Traffic));
  emit generationProgress(1);
  qApp->processEvents();

  if(bypassMode()) {
    //
    // Generate Single Music Import Event
    //
    sql=QString("insert into `LOG_LINES` set ")+
      "`LOG_NAME`='"+RDEscapeString(logname)+"',"+
      QString::asprintf("`LINE_ID`=%d,",count)+
      QString::asprintf("`COUNT`=%d,",count)+
      QString::asprintf("`TYPE`=%d,",RDLogLine::MusicLink)+
      QString::asprintf("`SOURCE`=%d,",RDLogLine::Template)+
      "`START_TIME`=0,"+
      "`GRACE_TIME`=0,"+
      QString::asprintf("`TIME_TYPE`=%d,",RDLogLine::Relative)+
      QString::asprintf("`TRANS_TYPE`=%d,",RDLogLine::Play)+
      "`LINK_EVENT_NAME`='"+RDEscapeString("bypass")+"',"+
      "`LINK_START_TIME`=0,"+
      "`LINK_LENGTH`=86400000,"+
      "`LINK_ID`=1,"+
      "`LINK_START_SLOP`=0,"+
      "`LINK_END_SLOP`=0,"+
      "`EVENT_LENGTH`=86400000";
    RDSqlQuery::apply(sql);
    count++;
  }
  else {
    //
    // Generate Grid Events
    //
    for(int i=0;i<24;i++) {
      sql=QString("select `CLOCK_NAME` from `SERVICE_CLOCKS` where ")+
	"(`SERVICE_NAME`='"+RDEscapeString(svc_name)+"')&&"+
	QString::asprintf("(`HOUR`=%d)",24*(date.dayOfWeek()-1)+i);
      q=new RDSqlQuery(sql);
      if(q->first()) {
	if((!q->value(0).isNull())&&(!q->value(0).toString().isEmpty())) {
	  clock.setClockName(q->value(0).toString());
	  clock.load();
	  clock.generateLog(i,logname,svc_name,report);
	  clock.clear();
	}
      }
      delete q;
      emit generationProgress(1+i);
      qApp->processEvents();
    }

    //
    // Get Current Count
    //
    sql=QString("select `COUNT` from `LOG_LINES` where ")+
      "`LOG_NAME`='"+RDEscapeString(logname)+"' "+
      "order by `COUNT` desc";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      count=q->value(0).toInt()+1;
    }
    else {
      count=0;
    }
    delete q;
  }

  //
  // Log Chain To
  //
  if(chainto()) {
    sql=QString("insert into `LOG_LINES` set ")+
      "`LOG_NAME`='"+RDEscapeString(logname)+"',"+
      QString::asprintf("`LINE_ID`=%d,",count)+
      QString::asprintf("`COUNT`=%d,",count)+
      QString::asprintf("`TYPE`=%d,",RDLogLine::Chain)+
      QString::asprintf("`SOURCE`=%d,",RDLogLine::Template)+
      QString::asprintf("`TRANS_TYPE`=%d,",RDLogLine::Segue)+
      QString::asprintf("`TIME_TYPE`=%d,",RDLogLine::Relative)+
      "`LABEL`='"+RDEscapeString(nextname)+"'";
    RDSqlQuery::apply(sql);
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
  RDLogModel *src_model=NULL;
  RDLogModel *dst_model=NULL;

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
  //  int current_link=0;
  //  int total_links=log->linkQuantity(link_src);

  emit generationProgress(0);
  qApp->processEvents();

  //
  // Load Parser Strings
  //
  QString break_str;
  QString track_str;
  QString label_cart;
  QString track_cart;
  GetParserStrings(src,&break_str,&track_str,&label_cart,&track_cart);

  //
  // Import File
  //
  if(!import(src,date,breakString(),trackString(src),true)) {
    *err_msg=tr("Import failed");
    delete log_lock;
    return false;
  }

  if(bypassMode()) {
    if(src==RDSvc::Music) {
      if(!ValidateInlineEvents(err_msg)) {
	delete log_lock;
	*err_msg=tr("Import file")+": \""+importFilename(src,date)+"\"\n\n"+
	  *err_msg;
	return false;
      }
      src_model=new RDLogModel(logname,true,this);
      dst_model=new RDLogModel(logname,true,this);
      src_model->load();
      ProcessBypassMusicEvents(log,dst_model,src_model,track_str,label_cart,
			       track_cart,link_src,src_type,&autofill_errors);
    }
    else {
      src_model=new RDLogModel(logname,true,this);
      dst_model=new RDLogModel(logname,true,this);
      src_model->load();
      ProcessBypassTrafficEvents(log,dst_model,src_model,track_str,label_cart,
				 track_cart,link_src,&autofill_errors);
    }
  }
  else {
    //
    // Resolve embedded link parameters
    //
    if(src==RDSvc::Music) {
      if(!ResolveInlineEvents(logname,err_msg)) {
	delete log_lock;
	*err_msg=tr("Import file")+": \""+importFilename(src,date)+"\"\n\n"+
	  *err_msg;
	return false;
      }
    }

    //
    // Iterate Through the Log
    //
    src_model=new RDLogModel(logname,true,this);
    dst_model=new RDLogModel(logname,true,this);
    src_model->load();
    ProcessGridEvents(log,dst_model,src_model,track_str,label_cart,track_cart,
		      link_src,src_type,&autofill_errors);
  }

  dst_model->save(svc_config);

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
  dst_model->validate(&missing_report,date);
  bool event=false;
  QString link_report=tr("The following events were not placed:\n");
  sql=QString("select ")+
    "`IMPORTER_LINES`.`START_HOUR`,"+   // 00
    "`IMPORTER_LINES`.`START_SECS`,"+   // 01
    "`IMPORTER_LINES`.`TYPE`,"+         // 02
    "`IMPORTER_LINES`.`CART_NUMBER`,"+  // 03
    "`IMPORTER_LINES`.`TITLE`,"+        // 04
    "`CART`.`TITLE` "+                  // 05
    "from `IMPORTER_LINES` left join `CART` "+
    "on `IMPORTER_LINES`.`CART_NUMBER`=`CART`.`NUMBER` where "+
    "`IMPORTER_LINES`.`STATION_NAME`='"+
    RDEscapeString(svc_station->name())+"' && "+
    QString::asprintf("`IMPORTER_LINES`.`PROCESS_ID`=%u && ",getpid())+
    "`IMPORTER_LINES`.`EVENT_USED`='N' "+
    "order by `LINE_ID`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event=true;

    link_report+=QString("  ")+
      RDSvc::timeString(q->value(0).toInt(),q->value(1).toInt());
    switch((RDLogLine::Type)q->value(2).toUInt()) {
    case RDLogLine::Cart:
    case RDLogLine::Macro:
      if(q->value(5).toString().isEmpty()) {
	cartname=q->value(4).toString();
      }
      else {
	cartname=q->value(5).toString();
      }
      link_report+=
	QString::asprintf(" - %06u - ",q->value(3).toUInt())+cartname+"\n";
      break;

    case RDLogLine::Marker:
      link_report+=" - "+tr("Note Cart")+": \""+q->value(4).toString()+"\"\n";
      break;

    case RDLogLine::Track:
      link_report+=" - "+tr("Track")+": \""+q->value(4).toString()+"\"\n";
      break;

    case RDLogLine::TrafficLink:
      link_report+=" - "+tr("Traffic Link")+"\n";
      break;

    case RDLogLine::OpenBracket:
    case RDLogLine::CloseBracket:
    case RDLogLine::MusicLink:
    case RDLogLine::Chain:
    case RDLogLine::UnknownType:
      link_report+=" - "+tr("Unexpected event")+" \""+
	RDLogLine::typeText((RDLogLine::Type)q->value(2).toUInt())+"\"\n";
      break;
    }
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
  qApp->processEvents();
  delete src_model;
  delete dst_model;

  sql=QString("delete from `IMPORTER_LINES` where ")+
    "`STATION_NAME`='"+RDEscapeString(svc_station->name())+"' && "+
    QString::asprintf("`PROCESS_ID`=%u",getpid());
  //  printf("Importer Table Cleanup SQL: %s\n",sql.toUtf8().constData());
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

  RDLogModel *src_model=new RDLogModel(logname,false,this);
  RDLogModel *dst_model=new RDLogModel(logname,false,this);
  src_model->load();
  RDLogLine *logline=NULL;
  for(int i=0;i<src_model->lineCount();i++) {
    logline=src_model->logLine(i);
    if((logline->linkId()<0)||(logline->source()!=event_source)) {
      dst_model->insert(dst_model->lineCount(),1,true);
      *(dst_model->logLine(dst_model->lineCount()-1))=*logline;
      dst_model->logLine(dst_model->lineCount()-1)->setId(dst_model->nextId());
    }
  }
  dst_model->save(svc_config);
  delete src_model;
  delete dst_model;

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
  QString sql=QString("select `NAME` from `SERVICES` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
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
  sql="select `DESCRIPTION` from `SERVICES` where `NAME`='"+
    RDEscapeString(svc_name)+"'";

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

  if(name.simplified()!=name) {
    *err_msg=QObject::tr("whitespace is not permitted in service names");
    return false;
  }

  sql=QString("select `NAME` from `SERVICES` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *err_msg=QObject::tr("service already exists");
    delete q;
    return false;
  }

  if(exemplar.isEmpty()) {  // Create Empty Service
    sql=QString("insert into `SERVICES` set `NAME`='")+
      RDEscapeString(name)+"',"+
      "`NAME_TEMPLATE`='"+RDEscapeString(name)+"-%m%d',"+
      "`DESCRIPTION_TEMPLATE`='"+RDEscapeString(name)+" log for %d/%m/%Y'";
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Create Group Audio Perms
    //
    sql="select `NAME` from `GROUPS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `AUDIO_PERMS` set ")+
	"`GROUP_NAME`='"+RDEscapeString(q->value(0).toString())+"',"
	"`SERVICE_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;
    
    //
    // Create Station Perms
    //
    sql="select `NAME` from `STATIONS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `SERVICE_PERMS` set ")+
	"`STATION_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;

    for(int i=0;i<168;i++) {
      sql=QString("insert into `SERVICE_CLOCKS` set ")+
	"`SERVICE_NAME`='"+RDEscapeString(name)+"',"+
	QString::asprintf("`HOUR`=%d,",i)+
	"`CLOCK_NAME`=null";
      RDSqlQuery::apply(sql);
    }
  }
  else {    // Base on Existing Service
    sql=QString("select ")+
      "`DESCRIPTION`,"+            // 00
      "`BYPASS_MODE`,"+            // 01
      "`NAME_TEMPLATE`,"+          // 02
      "`DESCRIPTION_TEMPLATE`,"+   // 03
      "`PROGRAM_CODE`,"+           // 04
      "`CHAIN_LOG`,"+              // 05
      "`SUB_EVENT_INHERITANCE`,"+  // 06
      "`TRACK_GROUP`,"+            // 07
      "`AUTOSPOT_GROUP`,"+         // 08
      "`DEFAULT_LOG_SHELFLIFE`,"+  // 09
      "`LOG_SHELFLIFE_ORIGIN`,"+   // 10
      "`AUTO_REFRESH`,"+           // 11
      "`ELR_SHELFLIFE`,"+          // 12
      "`TFC_PREIMPORT_CMD`,"+      // 13
      "`TFC_IMPORT_TEMPLATE`,"+    // 14
      "`TFC_PATH`,"+               // 15
      "`TFC_LABEL_CART`,"+         // 16
      "`TFC_TRACK_CART`,"+         // 17
      "`TFC_BREAK_STRING`,"+       // 18
      "`TFC_TRACK_STRING`,"+       // 19
      "`TFC_CART_OFFSET`,"+        // 20
      "`TFC_CART_LENGTH`,"+        // 21
      "`TFC_TITLE_OFFSET`,"        // 22
      "`TFC_TITLE_LENGTH`,"+       // 23
      "`TFC_LEN_HOURS_OFFSET`,"    // 24
      "`TFC_LEN_HOURS_LENGTH`,"    // 25
      "`TFC_LEN_MINUTES_OFFSET`,"  // 26
      "`TFC_LEN_MINUTES_LENGTH`,"  // 27
      "`TFC_LEN_SECONDS_OFFSET`,"  // 28
      "`TFC_LEN_SECONDS_LENGTH`,"  // 29
      "`TFC_HOURS_OFFSET`,"+       // 30
      "`TFC_HOURS_LENGTH`,"+       // 31
      "`TFC_MINUTES_OFFSET`,"+     // 32
      "`TFC_MINUTES_LENGTH`,"+     // 33
      "`TFC_SECONDS_OFFSET`,"+     // 34
      "`TFC_SECONDS_LENGTH`,"+     // 35
      "`TFC_DATA_OFFSET`,"+        // 36
      "`TFC_DATA_LENGTH`,"+        // 37
      "`TFC_EVENT_ID_OFFSET`,"+    // 38
      "`TFC_EVENT_ID_LENGTH`,"+    // 39
      "`TFC_ANNC_TYPE_OFFSET`,"+   // 40
      "`TFC_ANNC_TYPE_LENGTH`,"+   // 41
      "`MUS_PREIMPORT_CMD`,"+      // 42
      "`MUS_IMPORT_TEMPLATE`,"+    // 43
      "`MUS_PATH`,"+               // 44
      "`MUS_LABEL_CART`,"+         // 45
      "`MUS_TRACK_CART`,"+         // 46
      "`MUS_BREAK_STRING`,"+       // 47
      "`MUS_TRACK_STRING`,"+       // 48
      "`MUS_CART_OFFSET`,"+        // 49
      "`MUS_CART_LENGTH`,"+        // 50
      "`MUS_TITLE_OFFSET`,"        // 51
      "`MUS_TITLE_LENGTH`,"+       // 52
      "`MUS_LEN_HOURS_OFFSET`,"    // 53
      "`MUS_LEN_HOURS_LENGTH`,"    // 54
      "`MUS_LEN_MINUTES_OFFSET`,"  // 55
      "`MUS_LEN_MINUTES_LENGTH`,"  // 56
      "`MUS_LEN_SECONDS_OFFSET`,"  // 57
      "`MUS_LEN_SECONDS_LENGTH`,"  // 58
      "`MUS_HOURS_OFFSET`,"+       // 59
      "`MUS_HOURS_LENGTH`,"+       // 60
      "`MUS_MINUTES_OFFSET`,"+     // 61
      "`MUS_MINUTES_LENGTH`,"+     // 62
      "`MUS_SECONDS_OFFSET`,"+     // 63
      "`MUS_SECONDS_LENGTH`,"+     // 64
      "`MUS_DATA_OFFSET`,"+        // 65
      "`MUS_DATA_LENGTH`,"+        // 66
      "`MUS_EVENT_ID_OFFSET`,"+    // 67
      "`MUS_EVENT_ID_LENGTH`,"+    // 68
      "`MUS_ANNC_TYPE_OFFSET`,"+   // 69
      "`MUS_ANNC_TYPE_LENGTH`,"+   // 70
      "`MUS_TRANS_TYPE_OFFSET`,"+  // 71
      "`MUS_TRANS_TYPE_LENGTH`,"+  // 72
      "`MUS_TIME_TYPE_OFFSET`,"+   // 73
      "`MUS_TIME_TYPE_LENGTH` "+   // 74
      " from `SERVICES` where `NAME`='"+RDEscapeString(exemplar)+"'";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into `SERVICES` set ")+
	"`DESCRIPTION`='"+
	RDEscapeString(tr("Copy of")+" "+q->value(0).toString())+"',"+
	"`BYPASS_MODE`='"+RDEscapeString(q->value(1).toString())+"',"+
	"`NAME_TEMPLATE`='"+RDEscapeString(q->value(2).toString())+"',"+
	"`DESCRIPTION_TEMPLATE`='"+RDEscapeString(q->value(3).toString())+"',"+
	"`PROGRAM_CODE`='"+RDEscapeString(q->value(4).toString())+"',"+
	"`CHAIN_LOG`='"+RDEscapeString(q->value(5).toString())+"',"+
	QString::asprintf("`SUB_EVENT_INHERITANCE`=%d,",q->value(6).toInt())+
	"`TRACK_GROUP`='"+RDEscapeString(q->value(7).toString())+"',"+
	"`AUTOSPOT_GROUP`='"+RDEscapeString(q->value(8).toString())+"',"+
	QString::asprintf("`DEFAULT_LOG_SHELFLIFE`=%d,",q->value(9).toInt())+
	QString::asprintf("`LOG_SHELFLIFE_ORIGIN`=%d,",q->value(10).toInt())+
	"`AUTO_REFRESH`='"+RDEscapeString(q->value(11).toString())+"',"+
	QString::asprintf("`ELR_SHELFLIFE`=%d,",q->value(12).toInt())+
	"`TFC_PREIMPORT_CMD`='"+RDEscapeString(q->value(13).toString())+"',"+
	"`TFC_IMPORT_TEMPLATE`='"+RDEscapeString(q->value(14).toString())+"',"+
	"`TFC_PATH`='"+RDEscapeString(q->value(15).toString())+"',"+
	"`TFC_LABEL_CART`='"+RDEscapeString(q->value(16).toString())+"',"+
	"`TFC_TRACK_CART`='"+RDEscapeString(q->value(17).toString())+"',"+
	"`TFC_BREAK_STRING`='"+RDEscapeString(q->value(18).toString())+"',"+
	"`TFC_TRACK_STRING`='"+RDEscapeString(q->value(19).toString())+"',"+
	QString::asprintf("`TFC_CART_OFFSET`=%d,",q->value(20).toInt())+
	QString::asprintf("`TFC_CART_LENGTH`=%d,",q->value(21).toInt())+
	QString::asprintf("`TFC_TITLE_OFFSET`=%d,",q->value(22).toInt())+
	QString::asprintf("`TFC_TITLE_LENGTH`=%d,",q->value(23).toInt())+
	QString::asprintf("`TFC_LEN_HOURS_OFFSET`=%d,",q->value(24).toInt())+
	QString::asprintf("`TFC_LEN_HOURS_LENGTH`=%d,",q->value(25).toInt())+
	QString::asprintf("`TFC_LEN_MINUTES_OFFSET`=%d,",q->value(26).toInt())+
	QString::asprintf("`TFC_LEN_MINUTES_LENGTH`=%d,",q->value(27).toInt())+
	QString::asprintf("`TFC_LEN_SECONDS_OFFSET`=%d,",q->value(28).toInt())+
	QString::asprintf("`TFC_LEN_SECONDS_LENGTH`=%d,",q->value(29).toInt())+
	QString::asprintf("`TFC_HOURS_OFFSET`=%d,",q->value(30).toInt())+
	QString::asprintf("`TFC_HOURS_LENGTH`=%d,",q->value(31).toInt())+
	QString::asprintf("`TFC_MINUTES_OFFSET`=%d,",q->value(32).toInt())+
	QString::asprintf("`TFC_MINUTES_LENGTH`=%d,",q->value(33).toInt())+
	QString::asprintf("`TFC_SECONDS_OFFSET`=%d,",q->value(34).toInt())+
	QString::asprintf("`TFC_SECONDS_LENGTH`=%d,",q->value(35).toInt())+
	QString::asprintf("`TFC_DATA_OFFSET`=%d,",q->value(36).toInt())+
	QString::asprintf("`TFC_DATA_LENGTH`=%d,",q->value(37).toInt())+
	QString::asprintf("`TFC_EVENT_ID_OFFSET`=%d,",q->value(38).toInt())+
	QString::asprintf("`TFC_EVENT_ID_LENGTH`=%d,",q->value(39).toInt())+
	QString::asprintf("`TFC_ANNC_TYPE_OFFSET`=%d,",q->value(40).toInt())+
	QString::asprintf("`TFC_ANNC_TYPE_LENGTH`=%d,",q->value(41).toInt())+
	"`MUS_PREIMPORT_CMD`='"+RDEscapeString(q->value(42).toString())+"',"+
	"`MUS_IMPORT_TEMPLATE`='"+RDEscapeString(q->value(43).toString())+"',"+
	"`MUS_PATH`='"+RDEscapeString(q->value(44).toString())+"',"+
	"`MUS_LABEL_CART`='"+RDEscapeString(q->value(45).toString())+"',"+
	"`MUS_TRACK_CART`='"+RDEscapeString(q->value(46).toString())+"',"+
	"`MUS_BREAK_STRING`='"+RDEscapeString(q->value(47).toString())+"',"+
	"`MUS_TRACK_STRING`='"+RDEscapeString(q->value(48).toString())+"',"+
	QString::asprintf("`MUS_CART_OFFSET`=%d,",q->value(49).toInt())+
	QString::asprintf("`MUS_CART_LENGTH`=%d,",q->value(50).toInt())+
	QString::asprintf("`MUS_TITLE_OFFSET`=%d,",q->value(51).toInt())+
	QString::asprintf("`MUS_TITLE_LENGTH`=%d,",q->value(52).toInt())+
	QString::asprintf("`MUS_LEN_HOURS_OFFSET`=%d,",q->value(53).toInt())+
	QString::asprintf("`MUS_LEN_HOURS_LENGTH`=%d,",q->value(54).toInt())+
	QString::asprintf("`MUS_LEN_MINUTES_OFFSET`=%d,",q->value(55).toInt())+
	QString::asprintf("`MUS_LEN_MINUTES_LENGTH`=%d,",q->value(56).toInt())+
	QString::asprintf("`MUS_LEN_SECONDS_OFFSET`=%d,",q->value(57).toInt())+
	QString::asprintf("`MUS_LEN_SECONDS_LENGTH`=%d,",q->value(58).toInt())+
	QString::asprintf("`MUS_HOURS_OFFSET`=%d,",q->value(59).toInt())+
	QString::asprintf("`MUS_HOURS_LENGTH`=%d,",q->value(60).toInt())+
	QString::asprintf("`MUS_MINUTES_OFFSET`=%d,",q->value(61).toInt())+
	QString::asprintf("`MUS_MINUTES_LENGTH`=%d,",q->value(62).toInt())+
	QString::asprintf("`MUS_SECONDS_OFFSET`=%d,",q->value(63).toInt())+
	QString::asprintf("`MUS_SECONDS_LENGTH`=%d,",q->value(64).toInt())+
	QString::asprintf("`MUS_DATA_OFFSET`=%d,",q->value(65).toInt())+
	QString::asprintf("`MUS_DATA_LENGTH`=%d,",q->value(66).toInt())+
	QString::asprintf("`MUS_EVENT_ID_OFFSET`=%d,",q->value(67).toInt())+
	QString::asprintf("`MUS_EVENT_ID_LENGTH`=%d,",q->value(68).toInt())+
	QString::asprintf("`MUS_ANNC_TYPE_OFFSET`=%d,",q->value(69).toInt())+
	QString::asprintf("`MUS_ANNC_TYPE_LENGTH`=%d,",q->value(70).toInt())+
	QString::asprintf("`MUS_TRANS_TYPE_OFFSET`=%d,",q->value(71).toInt())+
	QString::asprintf("`MUS_TRANS_TYPE_LENGTH`=%d,",q->value(72).toInt())+
	QString::asprintf("`MUS_TIME_TYPE_OFFSET`=%d,",q->value(73).toInt())+
	QString::asprintf("`MUS_TIME_TYPE_LENGTH`=%d,",q->value(74).toInt())+
	"`NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
      delete q;

      sql=QString("select ")+
	"`HOUR`,"+        // 00
	"`CLOCK_NAME` "+  // 01
	"from `SERVICE_CLOCKS` where "+
	"`SERVICE_NAME`='"+RDEscapeString(exemplar)+"'";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	sql=QString("insert into `SERVICE_CLOCKS` set ")+
	  "`SERVICE_NAME`='"+RDEscapeString(name)+"',"+
	  QString::asprintf("`HOUR`=%d,",q->value(0).toInt());

	if(q->value(1).isNull()) {
  	  sql+="`CLOCK_NAME`=null";
	}
	else {
	  sql+="`CLOCK_NAME`='"+RDEscapeString(q->value(1).toString())+"'";
	}
	RDSqlQuery::apply(sql);
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
    sql=QString("select `GROUP_NAME` from `AUDIO_PERMS` where ")+
      "`SERVICE_NAME`='"+RDEscapeString(exemplar)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `AUDIO_PERMS` set ")+
	"`GROUP_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;

    //
    // Clone Service Perms
    //
    sql=QString("select `STATION_NAME` from `SERVICE_PERMS` where ")+
      "`SERVICE_NAME`='"+RDEscapeString(exemplar)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `SERVICE_PERMS` set ")+
	"`STATION_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;

    //
    // Clone Autofill List
    //
    sql=QString("select `CART_NUMBER` from `AUTOFILLS` where ")+
      "`SERVICE`='"+RDEscapeString(exemplar)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `AUTOFILLS` set ")+
	QString::asprintf("`CART_NUMBER`=%u,",q->value(0).toUInt())+
	"`SERVICE`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;

    //
    // Clone Clock Perms
    //
    sql=QString("select `CLOCK_NAME` from `CLOCK_PERMS` where ")+
      "`SERVICE_NAME`='"+RDEscapeString(exemplar)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `CLOCK_PERMS` set ")+
	"`CLOCK_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;

    //
    // Clone Event Perms
    //
    sql=QString("select `EVENT_NAME` from `EVENT_PERMS` where ")+
      "`SERVICE_NAME`='"+RDEscapeString(exemplar)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `EVENT_PERMS` set ")+
	"`EVENT_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
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

  sql=QString("delete from `AUDIO_PERMS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `SERVICE_PERMS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("update `RDAIRPLAY` set ")+
    "`DEFAULT_SERVICE`='' where "+
    "`DEFAULT_SERVICE`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `EVENT_PERMS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `CLOCK_PERMS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `AUTOFILLS` where ")+
    "`SERVICE`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `REPORT_SERVICES` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `SERVICES` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `SERVICE_CLOCKS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("select `NAME` from `LOGS` where ")+
    "`SERVICE`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("delete from `LOG_LINES` where ")+
      "`LOG_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;

  sql=QString("select ID from `STACK_LINES` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("delete from `STACK_SCHED_CODES` where ")+
      QString::asprintf("`STACK_LINES_ID`=%u",q->value(0).toUInt());
    RDSqlQuery::apply(sql);
  }
  delete q;
  sql=QString("delete from `STACK_LINES` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  sql=QString("delete from `ELR_LINES` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  sql=QString("delete from `LOGS` where ")+
    "`SERVICE`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
}


QString RDSvc::timeString(int hour,int secs)
{
  return QString::asprintf("%02d:%02d:%02d",hour,secs/60,secs%60);
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

      case RDSvc::TransType:
	fieldname="TRANS_TYPE_";
	break;

      case RDSvc::TimeType:
	fieldname="TIME_TYPE_";
	break;
  }
  return fieldname;
}


void RDSvc::SetRow(const QString &param,QString value) const
{
  QString sql;

  sql=QString("update `SERVICES` set `")+
    param+"`='"+RDEscapeString(value)+"' where "+
    "`NAME`='"+RDEscapeString(svc_name)+"'";
  RDSqlQuery::apply(sql);
}


void RDSvc::SetRow(const QString &param,int value) const
{
  QString sql;

  sql=QString("update `SERVICES` set `")+
    param+QString::asprintf("`=%d where ",value)+
    "`NAME`='"+RDEscapeString(svc_name)+"'";
  RDSqlQuery::apply(sql);
}


void RDSvc::GetParserStrings(ImportSource src,QString *break_str,
			     QString *track_str,QString *label_cart,
			     QString *track_cart)
{
  QString src_str=SourceString(src);
  QString sql=QString("select ")+
    "`"+src_str+"BREAK_STRING`,"+  // 00
    "`"+src_str+"TRACK_STRING`,"+  // 01
    "`"+src_str+"LABEL_CART`,"+    // 02
    "`"+src_str+"TRACK_CART` "+    // 03
    "from `SERVICES` where "+
    "`NAME`='"+RDEscapeString(svc_name)+"'";
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
  return margin+tr("Line")+QString::asprintf(" %u: ",1+lineno)+msg+"\n";
}


bool RDSvc::ResolveInlineEvents(const QString &logname,QString *err_msg)
{
  RDLogModel *model=NULL;
  RDLogLine *logline=NULL;
  QTime start;
  QTime end;
  QString sql;
  RDSqlQuery *q=NULL;
  bool ok=false;

  switch(subEventInheritance()) {
  case RDSvc::ParentEvent:
    model=new RDLogModel(logname,false,this);
    model->load();
    ok=true;
    for(int i=0;i<model->lineCount();i++) {
      logline=model->logLine(i);
      if(logline->type()==RDLogLine::MusicLink) {
	start=logline->linkStartTime();
	end=logline->linkStartTime().addSecs(logline->linkLength());
	sql=QString("select ")+
	  "`ID`,"+          // 00
	  "`FILE_LINE` "+   // 01
	  "from `IMPORTER_LINES` where "+
	  "`IMPORTER_LINES`.`STATION_NAME`=\""+
	  RDEscapeString(svc_station->name())+"\" && "+
	  QString::asprintf("`IMPORTER_LINES`.`PROCESS_ID`=%u && ",getpid())+
	  QString::asprintf("`TYPE`=%u && ",RDLogLine::TrafficLink)+
	  QString::asprintf("`START_HOUR`=%d && ",start.hour())+
	  QString::asprintf("`START_SECS`>=%d && ",
			    60*start.minute()+start.second())+
	  QString::asprintf("`START_SECS`<%d",60*start.minute()+start.second()+logline->linkLength()/1000);
	q=new RDSqlQuery(sql);
	if(q->size()>1) {
	  *err_msg+=tr("In event")+" \""+logline->linkEventName()+"\"@"+
	    rda->timeString(logline->startTime(RDLogLine::Logged))+":\n";
	  while(q->next()) {
	    *err_msg+=MakeErrorLine(4,q->value(1).toUInt(),
				    tr("multiple inline traffic breaks not permitted within the same music event"));
	  }
	  *err_msg+="\n";
	  ok=false;
	}
	if(q->first()) {
	  sql=QString("update `IMPORTER_LINES` set ")+
	    "`LINK_START_TIME`='"+
	    logline->linkStartTime().toString("hh:mm:ss")+"',"+
	    QString::asprintf("`LINK_LENGTH`=%d where ",logline->linkLength())+
	    QString::asprintf("`ID`=%u",q->value(0).toUInt());
	  RDSqlQuery::apply(sql);
	}
	delete q;
      }
    }
    delete model;
    model=NULL;

    if(!ok) {
      return false;
    }
    break;

  case RDSvc::SchedFile:
    if(!ValidateInlineEvents(err_msg)) {
      return false;
    }

    //
    // Resolve link parameters
    //
    sql=QString("select ")+
      "`ID`,"+          // 00
      "`START_HOUR`,"+  // 01
      "`START_SECS`,"+  // 02
      "`LENGTH` "+      // 03
      "from `IMPORTER_LINES` where "+
      QString::asprintf("`TYPE`=%u",RDLogLine::TrafficLink);
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update `IMPORTER_LINES` set ")+
	"`LINK_START_TIME`='"+QTime(q->value(1).toInt(),0,0).
	addSecs(q->value(2).toInt()).toString("hh:mm:ss")+"',"+
	QString::asprintf("`LINK_LENGTH`=%d where ",q->value(3).toInt())+
	QString::asprintf("`ID`=%u",q->value(0).toUInt());
      RDSqlQuery::apply(sql);
    }
    delete q;
    break;
  }

  return true;
}


bool RDSvc::ValidateInlineEvents(QString *err_msg) const
{
  //
  // Verify that all inline traffic and voicetrack events have explicit
  // start times and length
  //
  bool ret=true;

  QString sql=QString("select ")+
    "`FILE_LINE`,"+   // 00
    "`TYPE` "+        // 01
    "from `IMPORTER_LINES` where "+
    "`IMPORTER_LINES`.`STATION_NAME`=\""+
    RDEscapeString(svc_station->name())+"\" && "+
    QString::asprintf("`IMPORTER_LINES`.`PROCESS_ID`=%u && ",getpid())+
    QString::asprintf("((`TYPE`=%u) || ",RDLogLine::TrafficLink)+
    QString::asprintf("(`TYPE`=%u) ||",RDLogLine::Marker)+
    QString::asprintf("(`TYPE`=%u)) && ",RDLogLine::Track)+
    "(`START_HOUR` is null || `START_SECS` is null || `LENGTH` is null)";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    switch((RDLogLine::Type)q->value(1).toUInt()) {
    case RDLogLine::Marker:
      *err_msg+=MakeErrorLine(0,q->value(0).toUInt(),
			      tr("invalid start time and/or length on note cart."));
      break;

    case RDLogLine::TrafficLink:
      *err_msg+=MakeErrorLine(0,q->value(0).toUInt(),
			      tr("invalid start time and/or length on inline traffic break."));
      break;

    case RDLogLine::Track:
      *err_msg+=MakeErrorLine(0,q->value(0).toUInt(),
			      tr("invalid start time and/or length on track marker."));
      break;

    case RDLogLine::Cart:
    case RDLogLine::Macro:
    case RDLogLine::OpenBracket:
    case RDLogLine::CloseBracket:
    case RDLogLine::Chain:
    case RDLogLine::MusicLink:
    case RDLogLine::UnknownType:
      *err_msg+=MakeErrorLine(0,q->value(0).toUInt(),
			      tr("unexpected event type")+
			      " \""+RDLogLine::typeText((RDLogLine::Type)q->value(1).toUInt())+"\"");
      break;
    }
    ret=false;
  }
  delete q;

  return ret;
}


void RDSvc::ProcessGridEvents(RDLog *log,RDLogModel *dst_model,
			      RDLogModel *src_model,const QString &track_str,
			      const QString &label_cart,
			      const QString &track_cart,RDLog::Source link_src,
			      RDLogLine::Type src_type,QString *err_msgs)
{
  int current_link=0;
  int total_links=log->linkQuantity(link_src);
  RDLogLine *logline=NULL;

  for(int i=0;i<src_model->lineCount();i++) {
    logline=src_model->logLine(i);
    if(logline->type()==src_type) {
      RDEventLine *e=new RDEventLine(svc_station);
      e->setName(logline->linkEventName());
      e->load();
      e->linkLog(dst_model,log,svc_name,logline,track_str,label_cart,
		 track_cart,err_msgs);
      delete e;
      emit generationProgress(1+(24*current_link++)/total_links);
      qApp->processEvents();
    }
    else {
      dst_model->insert(dst_model->lineCount(),1,true);
      *(dst_model->logLine(dst_model->lineCount()-1))=*logline;
      dst_model->logLine(dst_model->lineCount()-1)->
	setId(dst_model->nextId());
    }
  }
}


void RDSvc::ProcessBypassMusicEvents(RDLog *log,RDLogModel *dst_model,
				     RDLogModel *src_model,
				     const QString &track_str,
				     const QString &label_cart,
				     const QString &track_cart,
				     RDLog::Source link_src,
				     RDLogLine::Type src_type,QString *err_msgs)
{
  QString sql;
  RDSqlQuery *q=NULL;
  RDLogLine *logline=NULL;

  for(int i=0;i<src_model->lineCount();i++) {
    RDLogLine *link_logline=src_model->logLine(i);
    if(link_logline->type()==RDLogLine::MusicLink) {
      //
      // Load Imported Events and Insert into Log
      //
      sql=QString("select ")+
	"`CART_NUMBER`,"+     // 00
	"`START_HOUR`,"+      // 01
	"`START_SECS`,"+      // 02
	"`LENGTH`,"+          // 03
	"`EXT_DATA`,"+        // 04
	"`EXT_EVENT_ID`,"+    // 05
	"`EXT_ANNC_TYPE`,"+   // 06
	"`EXT_CART_NAME`,"+   // 07
	"`TITLE`,"+           // 08
	"`TYPE`,"+            // 09
	"`LINK_START_TIME`,"+ // 10
	"`LINK_LENGTH`,"+     // 11
	"`TRANS_TYPE`,"+      // 12
	"`TIME_TYPE`,"+       // 13
	"`GRACE_TIME` "+      // 14
	"from `IMPORTER_LINES` where "+
	"`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"' && "+
	QString::asprintf("`PROCESS_ID`=%u && ",getpid())+
	"(`EVENT_USED`='N') order by `LINE_ID`";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	QTime start_time=
	  QTime(q->value(1).toInt(),0,0).addSecs(q->value(2).toInt());
	int length=GetCartLength(q->value(0).toUInt(),q->value(3).toInt());
	RDLogLine::TransType trans_type=
	  (RDLogLine::TransType)q->value(12).toInt();
	if(trans_type==RDLogLine::NoTrans) {
	  trans_type=RDLogLine::Play;
	}

	//
	// Inline Traffic Break
	//
	if(q->value(9).toUInt()==RDLogLine::TrafficLink) {
	  dst_model->insert(dst_model->lineCount(),1);
	  logline=dst_model->logLine(dst_model->lineCount()-1);
	  logline->setId(dst_model->nextId());
	  logline->setStartTime(RDLogLine::Logged,start_time);
	  logline->setType(RDLogLine::TrafficLink);
	  logline->setSource(RDLogLine::Music);
	  logline->setEventLength(length);
	  logline->setLinkEventName("Traffic BYPASS");
	  logline->setLinkStartTime(QTime(0,0,0).
		      addSecs(3600*q->value(1).toInt()+q->value(2).toInt()));
	  logline->setLinkLength(q->value(3).toInt());
	  logline->setLinkStartSlop(0);
	  logline->setLinkEndSlop(0);
	  logline->setLinkId(link_logline->linkId());
	  logline->setLinkEmbedded(true);
	}

	//
	// Voicetrack Marker
	//
	if(q->value(9).toUInt()==RDLogLine::Track) {
	  dst_model->insert(dst_model->lineCount(),1);
	  logline=dst_model->logLine(dst_model->lineCount()-1);
	  logline->setId(dst_model->nextId());
	  logline->setStartTime(RDLogLine::Logged,start_time);
	  logline->setType(RDLogLine::Track);
	  logline->setSource(RDLogLine::Music);
	  logline->setMarkerComment(q->value(8).toString());
	  logline->setEventLength(length);
	  logline->setLinkEventName("bypass");
	  logline->setLinkStartTime(QTime(0,0,0).
		      addSecs(3600*q->value(1).toInt()+q->value(2).toInt()));
	  logline->setLinkLength(q->value(3).toInt());
	  logline->setLinkStartSlop(0);
	  logline->setLinkEndSlop(0);
	  logline->setLinkId(link_logline->linkId());
	  logline->setLinkEmbedded(true);
	}

	//
	// Label/Note Cart
	//
	if(q->value(9).toUInt()==RDLogLine::Marker) {
	  dst_model->insert(dst_model->lineCount(),1);
	  logline=dst_model->logLine(dst_model->lineCount()-1);
	  logline->setId(dst_model->nextId());
	  logline->setStartTime(RDLogLine::Logged,start_time);
	  logline->setType(RDLogLine::Marker);
	  logline->setSource(RDLogLine::Music);
	  logline->setMarkerComment(q->value(8).toString());
	  logline->setEventLength(length);
	  logline->setLinkEventName("bypass");
	  logline->setLinkStartTime(QTime(0,0,0).
		      addSecs(3600*q->value(1).toInt()+q->value(2).toInt()));
	  logline->setLinkLength(q->value(3).toInt());
	  logline->setLinkStartSlop(0);
	  logline->setLinkEndSlop(0);
	  logline->setLinkId(link_logline->linkId());
	  logline->setLinkEmbedded(true);
	}

	//
	// Cart
	//
	if(q->value(9).toUInt()==RDLogLine::Cart) {
	  dst_model->insert(dst_model->lineCount(),1);
	  logline=dst_model->logLine(dst_model->lineCount()-1);
	  logline->setId(dst_model->nextId());
	  logline->setSource(RDLogLine::Music);
	  logline->setStartTime(RDLogLine::Logged,start_time);
	  logline->setType(RDLogLine::Cart);
	  logline->setCartNumber(q->value(0).toUInt());
	  logline->setExtStartTime(start_time);
	  logline->setExtLength(q->value(3).toInt());
	  logline->setExtData(q->value(4).toString().trimmed());
	  logline->setExtEventId(q->value(5).toString().trimmed());
	  logline->setExtAnncType(q->value(6).toString().trimmed());
	  logline->setExtCartName(q->value(7).toString().trimmed());
	  logline->setEventLength(length);
	  logline->setLinkEventName("bypass");
	  logline->setLinkStartTime(QTime(0,0,0).
		      addSecs(3600*q->value(1).toInt()+q->value(2).toInt()));
	  logline->setLinkLength(q->value(3).toInt());
	  logline->setLinkStartSlop(0);
	  logline->setLinkEndSlop(0);
	  logline->setLinkId(logline->linkId());
	  logline->setLinkEmbedded(link_logline->linkEmbedded());
	}

	//
	// Apply Common Values
	//
	if(logline!=NULL) {
	  logline->setGraceTime(q->value(14).toInt());
	  logline->setTimeType((RDLogLine::TimeType)q->value(13).toInt());
	  logline->setTransType(trans_type);
	}
      }

      delete q;

      //
      // Mark Events as Used
      //
      sql=QString("update `IMPORTER_LINES` set ")+
	"`EVENT_USED`='Y' where "+
	"`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"' && "+
	QString::asprintf("`PROCESS_ID`=%u",getpid());
      q=new RDSqlQuery(sql);
      delete q;
    }

    if((logline!=NULL)&&(logline->type()==RDLogLine::Chain)) {
      dst_model->insert(dst_model->lineCount(),1,true);
      *(dst_model->logLine(dst_model->lineCount()-1))=*logline;
      dst_model->logLine(dst_model->lineCount()-1)->
	setId(dst_model->nextId());
    }
  }
}


void RDSvc::ProcessBypassTrafficEvents(RDLog *log,RDLogModel *dst_model,
				       RDLogModel *src_model,
				       const QString &track_str,
				       const QString &label_cart,
				       const QString &track_cart,
				       RDLog::Source link_src,
				       QString *err_msgs)
{
  RDEventLine *evt=new RDEventLine(rda->station());

  evt->loadBypass();
  for(int i=0;i<src_model->lineCount();i++) {
    RDLogLine *logline=src_model->logLine(i);
    if(logline->type()==RDLogLine::TrafficLink) {
      evt->linkLog(dst_model,log,rda->station()->name(),logline,
		   track_str,label_cart,track_cart,err_msgs);
    }
    else {
      dst_model->insert(dst_model->lineCount(),1);
      *(dst_model->logLine(dst_model->lineCount()-1))=*logline;
      dst_model->logLine(dst_model->lineCount()-1)->
	setId(dst_model->nextId());
    }
  }

  delete evt;
}


int RDSvc::GetCartLength(unsigned cartnum,int def_length) const
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
