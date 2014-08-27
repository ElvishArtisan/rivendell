// rdsvc.cpp
//
// Abstract a Rivendell Service.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsvc.cpp,v 1.71.8.10.2.5 2014/06/24 18:27:05 cvs Exp $
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
  char var_buf[RD_MAX_IMPORT_LINE_LENGTH];
  int start_hour;
  int start_minutes;
  int start_seconds;
  QString hours_len_buf;
  QString minutes_len_buf;
  QString seconds_len_buf;
  unsigned cartnum;
  QString cartname;
  QString title;
  char data_buf[RD_MAX_IMPORT_LINE_LENGTH];
  char eventid_buf[RD_MAX_IMPORT_LINE_LENGTH];
  char annctype_buf[RD_MAX_IMPORT_LINE_LENGTH];
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
  sql=QString().sprintf("select %sCART_OFFSET,%sCART_LENGTH,\
                         %sDATA_OFFSET,%sDATA_LENGTH,		\
                         %sEVENT_ID_OFFSET,%sEVENT_ID_LENGTH,	\
                         %sANNC_TYPE_OFFSET,%sANNC_TYPE_LENGTH,	\
                         %sTITLE_OFFSET,%sTITLE_LENGTH,\
                         %sHOURS_OFFSET,%sHOURS_LENGTH,\
                         %sMINUTES_OFFSET,%sMINUTES_LENGTH,\
                         %sSECONDS_OFFSET,%sSECONDS_LENGTH,\
                         %sLEN_HOURS_OFFSET,%sLEN_HOURS_LENGTH,\
                         %sLEN_MINUTES_OFFSET,%sLEN_MINUTES_LENGTH,\
                         %sLEN_SECONDS_OFFSET,%sLEN_SECONDS_LENGTH \
                         from %s where NAME=\"%s\"",
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)src_str,
			(const char *)parser_table,
			(const char *)RDEscapeString(parser_name));
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  int cart_offset=q->value(0).toInt();
  int cart_length=q->value(1).toInt();
  unsigned cart_size=cart_offset+cart_length;
  int data_offset=q->value(2).toInt();
  int data_length=q->value(3).toInt();
  unsigned data_size=data_offset+data_length;
  int eventid_offset=q->value(4).toInt();
  int eventid_length=q->value(5).toInt();
  unsigned eventid_size=eventid_offset+eventid_length;
  int annctype_offset=q->value(6).toInt();
  int annctype_length=q->value(7).toInt();
  unsigned annctype_size=annctype_offset+annctype_length;
  int title_offset=q->value(8).toInt();
  int title_length=q->value(9).toInt();
  unsigned title_size=title_offset+title_length;
  int hours_offset=q->value(10).toInt();
  int hours_length=q->value(11).toInt();
  unsigned hours_size=hours_offset+hours_length;
  int minutes_offset=q->value(12).toInt();
  int minutes_length=q->value(13).toInt();
  unsigned minutes_size=minutes_offset+minutes_length;
  int seconds_offset=q->value(14).toInt();
  int seconds_length=q->value(15).toInt();
  unsigned seconds_size=seconds_offset+seconds_length;
  int hours_len_offset=q->value(16).toInt();
  int hours_len_length=q->value(17).toInt();
  unsigned hours_len_size=hours_len_offset+hours_len_length;
  int minutes_len_offset=q->value(18).toInt();
  int minutes_len_length=q->value(19).toInt();
  unsigned minutes_len_size=minutes_len_offset+minutes_len_length;
  int seconds_len_offset=q->value(20).toInt();
  int seconds_len_length=q->value(21).toInt();
  unsigned seconds_len_size=seconds_len_offset+seconds_len_length;

  delete q;

  //
  // Setup Data Source and Destination
  //
  /*
  if((infile=fopen(RDDateDecode(infilename,date),"r"))==NULL) {
    return false;
  }
  */
  sql=QString().sprintf("drop table `%s`",(const char *)dest_table);
  QSqlQuery *qq;          // Use QSqlQuery so we don't generate a 
  qq=new QSqlQuery(sql);  // spurious error message.
  delete qq;
  sql=QString().sprintf("create table `%s` (\
                         ID int primary key,\
                         START_HOUR int not null,\
                         START_SECS int not null,\
                         CART_NUMBER int unsigned,\
                         TITLE char(255),\
                         LENGTH int,\
                         INSERT_BREAK enum('N','Y') default 'N',\
                         INSERT_TRACK enum('N','Y') default 'N',\
                         INSERT_FIRST int unsigned default 0,\
                         TRACK_STRING char(255),\
                         EXT_DATA char(32),\
                         EXT_EVENT_ID char(32),\
                         EXT_ANNC_TYPE char(8),\
                         EXT_CART_NAME char(32),\
                         EVENT_USED enum('N','Y') default 'N',\
                         INDEX START_TIME_IDX (START_HOUR,START_SECS))",
			(const char *)dest_table);
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Parse and Save
  //
  int line_id=0;
  bool insert_found=false;
  bool cart_ok=false;
  bool line_used=false;
  bool insert_break=false;
  bool insert_track=false;
  bool break_first=false;
  bool track_first=false;
  QString track_label;
  while(fgets(buf,RD_MAX_IMPORT_LINE_LENGTH,infile)!=NULL) {
    line_used=false;
    str_buf=buf;
    cartnum=0;     // Get Cart Number
    if(strlen(buf)>=cart_size) {
      for(int i=cart_offset;i<(cart_offset+cart_length);i++) {
	var_buf[i-cart_offset]=buf[i];
      }
      var_buf[cart_length]=0;
      cartname=QString(var_buf).stripWhiteSpace();
      if(strlen(buf)>=hours_size) {           // Get Start Hours
	start_hour=QString(buf).mid(hours_offset,hours_length).toInt(&ok);
	if(ok&&(strlen(buf)>=minutes_size)) {           // Get Start Minutes
	  start_minutes=
	    QString(buf).mid(minutes_offset,minutes_length).toInt(&ok);
	  if(ok&&(strlen(buf)>=seconds_size)) {           // Get Start Seconds
	    start_seconds=
	      QString(buf).mid(seconds_offset,seconds_length).toInt(&ok);
	    if(ok&&(strlen(buf)>=hours_len_size)) {     // Get Length Hours
	      hours_len_buf=
		QString(buf).mid(hours_len_offset,hours_len_length);
	      if(strlen(buf)>=minutes_len_size) {       // Get Length Minutes
		minutes_len_buf=
		  QString(buf).mid(minutes_len_offset,minutes_len_length);
		if(strlen(buf)>=seconds_len_size) {     // Get Length Seconds
		  seconds_len_buf=
		    QString(buf).mid(seconds_len_offset,seconds_len_length);
		  cartlen=3600000*hours_len_buf.toInt()+
		    60000*minutes_len_buf.toInt()+
		    1000*seconds_len_buf.toInt();
		  if(strlen(buf)>=data_size) {        // Get Ext Data
		    for(int i=data_offset;i<(data_offset+data_length);i++) {
		      data_buf[i-data_offset]=buf[i];
		    }
		    data_buf[data_length]=0;
		    if(strlen(buf)>=eventid_size) {   // Get Ext Event ID
		      for(int i=eventid_offset;
			  i<(eventid_offset+eventid_length);i++) {
			eventid_buf[i-eventid_offset]=buf[i];
		      }
		      eventid_buf[eventid_length]=0;
		      if(strlen(buf)>=annctype_size) {   // Get Ext Annc Type
			for(int i=annctype_offset;
			    i<(annctype_offset+annctype_length);
			    i++) {
			  annctype_buf[i-annctype_offset]=buf[i];
			}
			annctype_buf[annctype_length]=0;
			if(strlen(buf)>=title_size) {   // Get Title
			  title=QString(buf).mid(title_offset,title_length);
			  cartnum=cartname.toUInt(&cart_ok);
			  if(cart_ok||
			     ((!label_cart.isEmpty())&&
			      (cartname==label_cart))||
			     ((!track_cart.isEmpty())&&
			      (cartname==track_cart))) {
			    sql=QString().sprintf("insert into `%s` \
                                  set ID=%d,START_HOUR=%d,START_SECS=%d,\
                                  CART_NUMBER=%u,TITLE=\"%s\",LENGTH=%d,\
                                  EXT_DATA=\"%s\",EXT_EVENT_ID=\"%s\",\
                                  EXT_ANNC_TYPE=\"%s\",EXT_CART_NAME=\"%s\"",
						  (const char *)dest_table,
						  line_id++,
						  start_hour,
						  60*start_minutes+
						  start_seconds,
						  cartnum,
						  (const char *)
						  RDEscapeString(title),
						  cartlen,
						  (const char *)data_buf,
						  (const char *)eventid_buf,
						  (const char *)annctype_buf,
						  (const char *)cartname);
			    q=new RDSqlQuery(sql);
			    delete q;
			    //
			    // Insert Break
			    //
			    if(insert_break) {
			      if(break_first) {
				sql=QString().
				  sprintf("update `%s` set INSERT_BREAK=\"Y\",\
                                       INSERT_FIRST=%d where ID=%d",
					  (const char *)dest_table,
					  RDEventLine::InsertBreak,line_id-1);
			      }
			      else {
				sql=QString().
				  sprintf("update `%s` set INSERT_BREAK=\"Y\" \
                                       where ID=%d",
					  (const char *)dest_table,line_id-1);
				
			      }
			      q=new RDSqlQuery(sql);
			      delete q;
			    }
			    //
			    // Insert Track
			    //
			    if(insert_track) {
			      if(track_first) {
				sql=QString().
				  sprintf("update `%s` set INSERT_TRACK=\"Y\",\
                                       TRACK_STRING=\"%s\",\
                                       INSERT_FIRST=%d where ID=%d",
					  (const char *)dest_table,
					  (const char *)track_label,
					  RDEventLine::InsertTrack,line_id-1);
			      }
			      else {
				sql=QString().
				  sprintf("update `%s` set INSERT_TRACK=\"Y\",\
                                       TRACK_STRING=\"%s\" where ID=%d",
					  (const char *)dest_table,
					  (const char *)track_label,
					  line_id-1);
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
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    if(!line_used) {
      //
      // Look for Break and Track Strings
      //
      str_buf=str_buf.stripWhiteSpace();
      if(!break_str.isEmpty()) {
	if(str_buf.contains(break_str)) {
	  insert_break=true;
	  if(!insert_found) {
	    break_first=true;
	    insert_found=true;
	  }
	  //	  q=new RDSqlQuery(sql);
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
  //  RDCreateLogTable(QString().sprintf("%s_LOG",(const char *)logname_esc));
  emit generationProgress(1);

  //
  // Generate Events
  //
  for(int i=0;i<24;i++) {
    sql=QString().sprintf("select CLOCK%d from SERVICES where NAME=\"%s\"",
			  24*(date.dayOfWeek()-1)+i,
			  (const char *)RDEscapeString(svc_name));
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
  }
  else {    // Base on Existing Service
    sql=QString().sprintf("select NAME_TEMPLATE,DESCRIPTION_TEMPLATE,\
                           CHAIN_LOG,AUTO_REFRESH,\
                           ELR_SHELFLIFE,TFC_PATH,TFC_WIN_PATH,\
                           TFC_CART_OFFSET,TFC_CART_LENGTH,\
                           TFC_START_OFFSET,TFC_START_LENGTH,\
                           TFC_LENGTH_OFFSET,TFC_LENGTH_LENGTH,\
                           TFC_HOURS_OFFSET,TFC_HOURS_LENGTH,\
                           TFC_MINUTES_OFFSET,TFC_MINUTES_LENGTH,\
                           TFC_SECONDS_OFFSET,TFC_SECONDS_LENGTH,\
                           TFC_DATA_OFFSET,TFC_DATA_LENGTH,\
                           TFC_EVENT_ID_OFFSET,TFC_EVENT_ID_LENGTH,\
                           TFC_ANNC_TYPE_OFFSET,TFC_ANNC_TYPE_LENGTH,\
                           MUS_PATH,MUS_WIN_PATH,\
                           MUS_CART_OFFSET,MUS_CART_LENGTH,\
                           MUS_START_OFFSET,MUS_START_LENGTH,\
                           MUS_LENGTH_OFFSET,MUS_LENGTH_LENGTH,\
                           MUS_HOURS_OFFSET,MUS_HOURS_LENGTH,\
                           MUS_MINUTES_OFFSET,MUS_MINUTES_LENGTH,\
                           MUS_SECONDS_OFFSET,MUS_SECONDS_LENGTH,\
                           MUS_DATA_OFFSET,MUS_DATA_LENGTH,\
                           MUS_EVENT_ID_OFFSET,MUS_EVENT_ID_LENGTH,\
                           MUS_ANNC_TYPE_OFFSET,MUS_ANNC_TYPE_LENGTH,");
    for(int i=0;i<168;i++) {
      sql+=QString().sprintf("CLOCK%d,",i);
    }
    sql=sql.left(sql.length()-1);
    sql+=QString().sprintf(" from SERVICES where NAME=\"%s\"",
			   (const char *)exemplar);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into SERVICES set\
                             NAME_TEMPLATE=\"%s\",\
                             DESCRIPTION_TEMPLATE=\"%s\",\
                             CHAIN_LOG=\"%s\",\
                             AUTO_REFRESH=\"%s\",ELR_SHELFLIFE=%d,\
                             TFC_PATH=\"%s\",TFC_WIN_PATH=\"%s\",\
                             TFC_CART_OFFSET=%d,TFC_CART_LENGTH=%d,\
                             TFC_START_OFFSET=%d,TFC_START_LENGTH=%d,\
                             TFC_LENGTH_OFFSET=%d,TFC_LENGTH_LENGTH=%d,\
                             TFC_HOURS_OFFSET=%d,TFC_HOURS_LENGTH=%d,\
                             TFC_MINUTES_OFFSET=%d,TFC_MINUTES_LENGTH=%d,\
                             TFC_SECONDS_OFFSET=%d,TFC_SECONDS_LENGTH=%d,\
                             TFC_DATA_OFFSET=%d,TFC_DATA_LENGTH=%d,\
                             TFC_EVENT_ID_OFFSET=%d,TFC_EVENT_ID_LENGTH=%d,\
                             TFC_ANNC_TYPE_OFFSET=%d,TFC_ANNC_TYPE_LENGTH=%d,\
                             MUS_PATH=\"%s\",MUS_WIN_PATH=\"%s\",\
                             MUS_CART_OFFSET=%d,MUS_CART_LENGTH=%d,\
                             MUS_START_OFFSET=%d,MUS_START_LENGTH=%d,\
                             MUS_LENGTH_OFFSET=%d,MUS_LENGTH_LENGTH=%d,\
                             MUS_HOURS_OFFSET=%d,MUS_HOURS_LENGTH=%d,\
                             MUS_MINUTES_OFFSET=%d,MUS_MINUTES_LENGTH=%d,\
                             MUS_SECONDS_OFFSET=%d,MUS_SECONDS_LENGTH=%d,\
                             MUS_DATA_OFFSET=%d,MUS_DATA_LENGTH=%d,\
                             MUS_EVENT_ID_OFFSET=%d,MUS_EVENT_ID_LENGTH=%d,\
                             MUS_ANNC_TYPE_OFFSET=%d,MUS_ANNC_TYPE_LENGTH=%d,\
                             NAME=\"%s\",",
			    (const char *)
			    RDEscapeString(q->value(0).toString()),
			    (const char *)
			    RDEscapeString(q->value(1).toString()),
			    (const char *)q->value(2).toString(),
			    (const char *)q->value(3).toString(),
                            q->value(4).toInt(),
			    (const char *)q->value(5).toString(),
			    (const char *)q->value(6).toString(),
			    q->value(7).toInt(),
			    q->value(8).toInt(),
			    q->value(9).toInt(),
			    q->value(10).toInt(),
			    q->value(11).toInt(),
			    q->value(12).toInt(),
			    q->value(13).toInt(),
			    q->value(14).toInt(),
			    q->value(15).toInt(),
			    q->value(16).toInt(),
			    q->value(17).toInt(),
			    q->value(18).toInt(),
			    q->value(19).toInt(),
			    q->value(20).toInt(),
			    q->value(21).toInt(),
			    q->value(22).toInt(),
			    q->value(23).toInt(),
			    q->value(24).toInt(),
			    (const char *)q->value(25).toString(),
			    (const char *)q->value(26).toString(),
			    q->value(27).toInt(),
			    q->value(28).toInt(),
			    q->value(29).toInt(),
			    q->value(30).toInt(),
			    q->value(31).toInt(),
			    q->value(32).toInt(),
			    q->value(33).toInt(),
			    q->value(34).toInt(),
			    q->value(35).toInt(),
			    q->value(36).toInt(),
			    q->value(37).toInt(),
			    q->value(38).toInt(),
			    q->value(39).toInt(),
			    q->value(40).toInt(),
			    q->value(41).toInt(),
			    q->value(42).toInt(),
			    q->value(43).toInt(),
			    q->value(44).toInt(),
			    (const char *)RDEscapeString(svc_name));
      for(int i=0;i<168;i++) {
	sql+=QString().sprintf("CLOCK%d=\"%s\",",
			      i,(const char *)q->value(45+i).toString());
      }
      sql=sql.left(sql.length()-1);
      q=new RDSqlQuery(sql);
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
