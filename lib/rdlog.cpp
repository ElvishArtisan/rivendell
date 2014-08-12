// rdlog.cpp
//
// Abstract a Rivendell Log.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlog.cpp,v 1.23.4.7.2.3 2014/06/24 18:27:04 cvs Exp $
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
#include <rddb.h>
#include <rdconf.h>
#include <rdlog.h>
#include <rdlog_line.h>
#include <rdescape_string.h>
#include <rdweb.h>

//
// Global Classes
//
RDLog::RDLog(const QString &name,bool create)
{
  RDSqlQuery *q;
  QString sql;

  log_name=name;

  if(create) {
    sql=QString().sprintf("select NAME from LOGS where \
(NAME=\"%s\")",(const char *)RDEscapeString(log_name));
    q=new RDSqlQuery(sql);
    if(q->size()!=1) {
      delete q;
      sql=QString().
        sprintf("INSERT INTO LOGS SET NAME=\"%s\",ORIGIN_DATETIME=NOW(),\
                 LINK_DATETIME=NOW(),MODIFIED_DATETIME=now()",
		(const char *)RDEscapeString(log_name));
      q=new RDSqlQuery(sql);
      delete q;
    }
    else {
      delete q;
    }
  }
}


QString RDLog::name() const
{
  return log_name;
}


bool RDLog::exists() const
{
  QString sql=QString().sprintf("select NAME from LOGS where NAME=\"%s\"",
				(const char *)RDEscapeString(log_name));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    delete q;
    return true;
  }
  delete q;
  return false;
}


bool RDLog::logExists() const
{
  return RDBool(GetStringValue("LOG_EXISTS"));
}


void RDLog::setLogExists(bool state) const
{
  SetRow("LOG_EXISTS",RDYesNo(state));
}


RDLog::Type RDLog::type() const
{
  return(RDLog::Type)GetIntValue("TYPE");
}


void RDLog::setType(RDLog::Type type) const
{
  SetRow("TYPE",(int)type);
}


QString RDLog::description() const
{
  return GetStringValue("DESCRIPTION");
}


void RDLog::setDescription(const QString &desc) const
{
  SetRow("DESCRIPTION",desc);
}


QString RDLog::service() const
{
  return GetStringValue("SERVICE");
}


void RDLog::setService(const QString &svc) const
{
  SetRow("SERVICE",svc);
}


QDate RDLog::startDate() const
{
  return GetDateValue("START_DATE");
}


void RDLog::setStartDate(const QDate &date) const
{
  SetRow("START_DATE",date);
}


QDate RDLog::endDate() const
{
  return GetDateValue("END_DATE");
}


void RDLog::setEndDate(const QDate &date) const
{
  SetRow("END_DATE",date);
}


QDate RDLog::purgeDate() const
{
  return GetDateValue("PURGE_DATE");
}


void RDLog::setPurgeDate(const QDate &date) const
{
  SetRow("PURGE_DATE",date);
}


QString RDLog::originUser() const
{
  return GetStringValue("ORIGIN_USER");
}
 

void RDLog::setOriginUser(const QString &user) const
{
  SetRow("ORIGIN_USER",user);
}


QDateTime RDLog::originDatetime() const
{
  return GetDatetimeValue("ORIGIN_DATETIME");
}


void RDLog::setOriginDatetime(const QDateTime &datetime) const
{
  SetRow("ORIGIN_DATETIME",datetime);
}


QDateTime RDLog::linkDatetime() const
{
  return GetDatetimeValue("LINK_DATETIME");
}


void RDLog::setLinkDatetime(const QDateTime &datetime) const
{
  SetRow("LINK_DATETIME",datetime);
}


QDateTime RDLog::modifiedDatetime() const
{
  return GetDatetimeValue("MODIFIED_DATETIME");
}


void RDLog::setModifiedDatetime(const QDateTime &datetime) const
{
  SetRow("MODIFIED_DATETIME",datetime);
}


bool RDLog::autoRefresh() const
{
  return RDBool(GetStringValue("AUTO_REFRESH"));
}


void RDLog::setAutoRefresh(bool state) const
{
  SetRow("AUTO_REFRESH",RDYesNo(state));
}


unsigned RDLog::scheduledTracks() const
{
  return GetUnsignedValue("SCHEDULED_TRACKS");
}


void RDLog::setScheduledTracks(unsigned tracks) const
{
  SetRow("SCHEDULED_TRACKS",tracks);
}


unsigned RDLog::completedTracks() const
{
  return GetUnsignedValue("COMPLETED_TRACKS");
}


void RDLog::setCompletedTracks(unsigned tracks) const
{
  SetRow("COMPLETED_TRACKS",tracks);
}


int RDLog::linkQuantity(RDLog::Source src) const
{
  switch(src) {
      case RDLog::SourceMusic:
	return GetIntValue("MUSIC_LINKS");

      case RDLog::SourceTraffic:
	return GetIntValue("TRAFFIC_LINKS");
  }
  return 0;
}


void RDLog::setLinkQuantity(RDLog::Source src,int quan) const
{
  switch(src) {
      case RDLog::SourceMusic:
	SetRow("MUSIC_LINKS",quan);
	break;

      case RDLog::SourceTraffic:
	SetRow("TRAFFIC_LINKS",quan);
	break;
  }
}


void RDLog::updateLinkQuantity(RDLog::Source src) const
{
  QString sql;
  RDSqlQuery *q;
  switch(src) {
      case RDLog::SourceMusic:
	sql=QString("select ID from `")+RDLog::tableName(log_name)+
	  "` where "+QString().sprintf("TYPE=%d",RDLogLine::MusicLink);
	q=new RDSqlQuery(sql);
	sql=QString("update LOGS set ")+
	  QString().sprintf("MUSIC_LINKS=%d ",q->size())+
	  "where NAME=\""+RDEscapeString(log_name)+"\"";
	break;

      case RDLog::SourceTraffic:
	sql=QString("select ID from `")+RDLog::tableName(log_name)+
	  "` where "+QString().sprintf("TYPE=%d",RDLogLine::TrafficLink);
	q=new RDSqlQuery(sql);

	sql=QString("update LOGS set ")+
	  QString().sprintf("TRAFFIC_LINKS=%d ",q->size())+
	  "where NAME=\""+RDEscapeString(log_name)+"\"";
	break;

      default:
	return;
  }
  delete q;
  q=new RDSqlQuery(sql);
  delete q;
}


RDLog::LinkState RDLog::linkState(RDLog::Source src) const
{
  if(linkQuantity(src)==0) {
    return RDLog::LinkNotPresent;
  }
  switch(src) {
      case RDLog::SourceMusic:
	return (RDLog::LinkState)RDBool(GetStringValue("MUSIC_LINKED"));

      case RDLog::SourceTraffic:
	return (RDLog::LinkState)RDBool(GetStringValue("TRAFFIC_LINKED"));
  }
  return RDLog::LinkNotPresent;
}


void RDLog::setLinkState(RDLog::Source src,bool state) const
{
  switch(src) {
      case RDLog::SourceMusic:
	SetRow("MUSIC_LINKED",RDYesNo(state));
	break;

      case RDLog::SourceTraffic:
	SetRow("TRAFFIC_LINKED",RDYesNo(state));
	break;
  }
}


int RDLog::nextId() const
{
  return GetIntValue("NEXT_ID");
}


void RDLog::setNextId(int id) const
{
  SetRow("NEXT_ID",id);
}


bool RDLog::isReady() const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString().sprintf("select MUSIC_LINKS,MUSIC_LINKED,TRAFFIC_LINKS,\
                         TRAFFIC_LINKED,SCHEDULED_TRACKS,COMPLETED_TRACKS \
                         from LOGS where NAME=\"%s\"",
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=((q->value(0).toInt()==0)||(q->value(1).toString()=="Y"))&&
      ((q->value(2).toInt()==0)||(q->value(3).toString()=="Y"))&&
      ((q->value(4).toInt()==0)||(q->value(4).toInt()==q->value(5).toInt()));
  }
  delete q;
  return ret;
}


bool RDLog::remove(RDStation *station,RDUser *user,RDConfig *config) const
{
  QString sql;
  RDSqlQuery *q;
  QString name=log_name;

  name.replace(" ","_");
  if(removeTracks(station,user,config)<0) {
    return false;
  }
  sql=QString().sprintf("drop table `%s_LOG`",(const char *)name);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from LOGS where (NAME=\"%s\" && TYPE=0)",
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
  return true;
}


void RDLog::updateTracks()
{
  QString sql;
  RDSqlQuery *q;
  unsigned scheduled=0;
  unsigned completed=0;

  sql=QString().sprintf("select NUMBER from CART where OWNER=\"%s\"",
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  completed=q->size();
  delete q;

  sql=QString("select ID from `")+RDLog::tableName(log_name)+
    "` where "+QString().sprintf("TYPE=%d",RDLogLine::Track);
  q=new RDSqlQuery(sql);
  scheduled=q->size()+completed;
  delete q;

  sql=QString().sprintf("update LOGS set SCHEDULED_TRACKS=%d,\
                         COMPLETED_TRACKS=%u where NAME=\"%s\"",
			scheduled,completed,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
}


int RDLog::removeTracks(RDStation *station,RDUser *user,RDConfig *config) const
{
  QString sql;
  RDSqlQuery *q;
  int count=0;
  RDCart *cart;

  QString owner=log_name;
  owner.replace(" ","_");
  sql=QString().sprintf("select NUMBER from CART where OWNER=\"%s\"",
			(const char *)owner);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cart=new RDCart(q->value(0).toUInt());
    if(!cart->remove(station,user,config)) {
      delete cart;
      return -1;
    }
    delete cart;
    count++;
  }
  delete q;

  return count;
}


RDLogEvent *RDLog::createLogEvent() const
{
  QString logname=name()+"_LOG";
  logname.replace(" ","_");
  return new RDLogEvent(logname);
}



QString RDLog::xml() const
{
  QString sql;
  RDSqlQuery *q;
  QString ret;
#ifndef WIN32
  sql=QString("select NAME,SERVICE,DESCRIPTION,ORIGIN_USER,")+
    "ORIGIN_DATETIME,LINK_DATETIME,MODIFIED_DATETIME,"+
    "AUTO_REFRESH,START_DATE,END_DATE,SCHEDULED_TRACKS,COMPLETED_TRACKS,"+
    "MUSIC_LINKS,MUSIC_LINKED,TRAFFIC_LINKS,TRAFFIC_LINKED,NEXT_ID "+
    "from LOGS where NAME=\""+RDEscapeString(log_name)+"\"";

  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret+="  <log>\n";
    ret+="   "+RDXmlField("name",log_name);
    ret+="   "+RDXmlField("serviceName",q->value(1).toString());
    ret+="   "+RDXmlField("description",q->value(2).toString());
    ret+="   "+RDXmlField("originUserName",q->value(3).toString());
    ret+="   "+RDXmlField("originDatetime",q->value(4).toDateTime());
    ret+="   "+RDXmlField("linkDatetime",q->value(5).toDateTime());
    ret+="   "+RDXmlField("modifiedDatetime",q->value(6).toDateTime());
    ret+="   "+RDXmlField("autoRefresh",RDBool(q->value(7).toString()));
    ret+="   "+RDXmlField("startDate",q->value(8).toDate());
    ret+="   "+RDXmlField("endDate",q->value(9).toDate());
    ret+="   "+RDXmlField("scheduledTracks",q->value(10).toInt());
    ret+="   "+RDXmlField("completedTracks",q->value(11).toInt());
    ret+="   "+RDXmlField("musicLinks",q->value(12).toInt());
    ret+="   "+RDXmlField("musicLinked",RDBool(q->value(13).toString()));
    ret+="   "+RDXmlField("trafficLinks",q->value(14).toInt());
    ret+="   "+RDXmlField("trafficLinked",RDBool(q->value(15).toString()));
    ret+="  </log>\n";
  }
  delete q;
#endif  // WIN32
  return ret;
}


bool RDLog::exists(const QString &name)
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select NAME from LOGS where NAME=\"")+
    RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;

  return ret;
}


QString RDLog::tableName(const QString &log_name)
{
  QString ret=log_name;
  ret.replace(" ","_");
  return ret+"_LOG";
}


int RDLog::GetIntValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  int accum;
  
  sql=QString().sprintf("select %s from LOGS where NAME=\"%s\"",
			(const char *)field,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


unsigned RDLog::GetUnsignedValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  unsigned accum;
  
  sql=QString().sprintf("select %s from LOGS where NAME=\"%s\"",
			(const char *)field,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toUInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


QString RDLog::GetStringValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  QString accum;
  
  sql=QString().sprintf("select %s from LOGS where NAME=\"%s\"",
			(const char *)field,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toString();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


QDate RDLog::GetDateValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  QDate accum;
  
  sql=QString().sprintf("select %s from LOGS where NAME=\"%s\"",
			(const char *)field,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toDate();
    delete q;
    return accum;
  }
  delete q;
  return QDate();    
}


QDateTime RDLog::GetDatetimeValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  QDateTime accum;
  
  sql=QString().sprintf("select %s from LOGS where NAME=\"%s\"",
			(const char *)field,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toDateTime();
    delete q;
    return accum;
  }
  delete q;
  return QDateTime();    
}


void RDLog::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE LOGS SET %s=%d WHERE NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLog::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE LOGS SET %s=%u WHERE NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLog::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE LOGS SET %s=\"%s\" WHERE NAME=\"%s\"",
			(const char *)param,
			(const char *)RDEscapeString(value),
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLog::SetRow(const QString &param,const QDate &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE LOGS SET %s=\"%s\" WHERE NAME=\"%s\"",
			(const char *)param,
			(const char *)value.toString("yyyy/MM/dd"),
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLog::SetRow(const QString &param,const QDateTime &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE LOGS SET %s=\"%s\" WHERE NAME=\"%s\"",
			(const char *)param,
			(const char *)value.toString("yyyy-MM-dd hh:mm:ss"),
			(const char *)RDEscapeString(log_name));
  q=new RDSqlQuery(sql);
  delete q;
}
