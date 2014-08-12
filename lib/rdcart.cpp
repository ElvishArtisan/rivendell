// rdcart.cpp
//
// Abstract a Rivendell Cart.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcart.cpp,v 1.72.4.7.2.9 2014/06/02 22:52:25 cvs Exp $
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

#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <curl/curl.h>
#endif  // WIN32

#include <vector>

#include <qstringlist.h>
#include <qobject.h>

#include <rd.h>
#include <rdconf.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdtextvalidator.h>
#include <rdescape_string.h>
#include <rdsystem.h>
#include <rdxport_interface.h>
#include <rdformpost.h>
#include <rdweb.h>
#include <rdstation.h>

//
// CURL Callbacks
//
size_t CartWriteCallback(void *ptr,size_t size,size_t nmemb,void *userdata)
{
  QString *xml=(QString *)userdata;
  for(unsigned i=0;i<(size*nmemb);i++) {
    *xml+=((const char *)ptr)[i];
  }
  return size*nmemb;
}


RDCart::RDCart(unsigned number)
{
  cart_number=number;
  metadata_changed=false;
}


RDCart::~RDCart()
{
  if(metadata_changed) {
    writeTimestamp();
  }
}


bool RDCart::exists() const
{
  return RDDoesRowExist("CART","NUMBER",cart_number);
}


bool RDCart::selectCut(QString *cut) const
{
  return selectCut(cut,QTime::currentTime());
}


bool RDCart::selectCut(QString *cut,const QTime &time) const
{
  bool ret;

  if(!exists()) {
    ret=(*cut=="");
    *cut="";
#ifndef WIN32
    syslog(LOG_USER|LOG_WARNING,
	   "RDCart::selectCut(): cart doesn't exist, CUT=%s",
	   (const char *)cut);
#endif  // WIN32
    return ret;
  }

  if(!cut->isEmpty()) {
    RDCut *rdcut=new RDCut(*cut);
    delete rdcut;
  }

  QString sql;
  RDSqlQuery *q;
  QString cutname;
  QDate current_date=QDate::currentDate();
  QString datetime_str=QDateTime(current_date,time).
    toString("yyyy-MM-dd hh:mm:ss");
  QString time_str=QDateTime(current_date,time).toString("hh:mm:ss");

  //  if(type()==RDCart::Audio) {
  switch(type()) {
  case RDCart::Audio:
    sql=QString().sprintf("select CUT_NAME,WEIGHT,LOCAL_COUNTER\
                           from CUTS  where (((START_DATETIME<=\"%s\")&&\
                           (END_DATETIME>=\"%s\"))||\
                           (START_DATETIME is null))&&\
                           (((START_DAYPART<=\"%s\")&&(END_DAYPART>=\"%s\")||\
                           START_DAYPART is null))&&\
                           (%s=\"Y\")&&(CART_NUMBER=%u)&&(EVERGREEN=\"N\")&&\
                           (LENGTH>0) order by LOCAL_COUNTER",
			  (const char *)datetime_str,
			  (const char *)datetime_str,
			  (const char *)time_str,
			  (const char *)time_str,
	(const char *)RDGetShortDayNameEN(current_date.dayOfWeek()).upper(),
			  cart_number);
    q=new RDSqlQuery(sql);
    cutname=GetNextCut(q);
    delete q;
    break;

  case RDCart::Macro:
  case RDCart::All:
    break;
  }
  if(cutname.isEmpty()) {   // No valid cuts, try the evergreen
#ifndef WIN32
    // syslog(LOG_USER|LOG_WARNING,"RDCart::selectCut(): no valid cuts, trying evergreen, SQL=%s",(const char *)sql);
#endif  // WIN32
    sql=QString().sprintf("select CUT_NAME,WEIGHT,LOCAL_COUNTER\
                           from CUTS where (CART_NUMBER=%u)&&\
                           (EVERGREEN=\"Y\")&&(LENGTH>0) \
                           order by LOCAL_COUNTER",
			  cart_number);
    q=new RDSqlQuery(sql);
    cutname=GetNextCut(q);
    delete q;
  }
  if(cutname.isEmpty()) {
#ifndef WIN32
    // syslog(LOG_USER|LOG_WARNING,"RDCart::selectCut(): no valid evergreen cuts, SQL=%s",(const char *)sql);
#endif  // WIN32
  }
  *cut=cutname;
  return true;
}


unsigned RDCart::number() const
{
  return cart_number;
}


QString RDCart::groupName() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"GROUP_NAME").
    toString();
}


void RDCart::setGroupName(const QString &name)
{
  SetRow("GROUP_NAME",name);
  metadata_changed=true;
}


RDCart::Type RDCart::type() const
{
  return (RDCart::Type)RDGetSqlValue("CART","NUMBER",cart_number,
				    "TYPE").toUInt();
}


void RDCart::setType(RDCart::Type type)
{
  SetRow("TYPE",(unsigned)type);
  metadata_changed=true;
}


QString RDCart::title() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"TITLE").toString();
}


void RDCart::setTitle(const QString &title)
{
  SetRow("TITLE",VerifyTitle(title));
  metadata_changed=true;
}


QString RDCart::artist() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"ARTIST").toString();
}


void RDCart::setArtist(const QString &artist)
{
  SetRow("ARTIST",artist);
  metadata_changed=true;
}


QString RDCart::album() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"ALBUM").toString();
}


void RDCart::setAlbum(const QString &album)
{
  SetRow("ALBUM",album);
  metadata_changed=true;
}


int RDCart::year() const
{
  QString value;
  value=RDGetSqlValue("CART","NUMBER",cart_number,"YEAR").toString();
  QStringList f0=f0.split("-",value);
  return f0[0].toInt();
}


void RDCart::setYear(int year)
{
  SetRow("YEAR",QString().sprintf("%04d-01-01",year));
  metadata_changed=true;
}


QString RDCart::schedCodes() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"SCHED_CODES").toString();
}


void RDCart::setSchedCodes(const QString &sched_codes) const
{
  SetRow("SCHED_CODES",sched_codes);
}


QStringList RDCart::schedCodesList() const
{
  QStringList list;
  QString sched_codes=
    RDGetSqlValue("CART","NUMBER",cart_number,"SCHED_CODES").toString();

  for(int i=0;i<255;i+=11) {
    QString code=sched_codes.mid(i,11);
    if((!code.isEmpty())&&(code.stripWhiteSpace()!=".")) {
      list.push_back(code.stripWhiteSpace());
    }
  }

  return list;
}


void RDCart::setSchedCodesList(const QStringList &codes)
{
  QString sched_codes="";

  for(unsigned i=0;i<codes.size();i++) {
    sched_codes+=QString().sprintf("%-11s",(const char *)codes[i].left(11));
  }
  sched_codes+=".";
  SetRow("SCHED_CODES",sched_codes);  
}


void RDCart::addSchedCode(const QString &code)
{
  QStringList codes=schedCodesList();
  codes.push_back(code);
  setSchedCodesList(codes);
}


void RDCart::updateSchedCodes(const QString &add_codes,const QString &remove_codes) const
{
  QString sched_codes;
  QString save_codes="";
  QString sql;
  RDSqlQuery *q;
  QString str;

  sched_codes=schedCodes();

  sql=QString().sprintf("select CODE from SCHED_CODES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
  	QString wstr=q->value(0).toString();
  	wstr+="          ";
    wstr=wstr.left(11);
  	if((sched_codes.contains(wstr)>0||add_codes.contains(wstr)>0)&&remove_codes.contains(wstr)==0) {
  	  save_codes+=wstr;
  	}
  }
  delete q;

  save_codes+=".";
  SetRow("SCHED_CODES",save_codes);
}	


QString RDCart::label() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"LABEL").toString();
}


void RDCart::setLabel(const QString &label)
{
  SetRow("LABEL",label);
  metadata_changed=true;
}


QString RDCart::conductor() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"CONDUCTOR").toString();
}


void RDCart::setConductor(const QString &cond)
{
  SetRow("CONDUCTOR",cond);
  metadata_changed=true;
}


QString RDCart::client() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"CLIENT").toString();
}


void RDCart::setClient(const QString &client)
{
  SetRow("CLIENT",client);
  metadata_changed=true;
}


QString RDCart::agency() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"AGENCY").toString();
}


void RDCart::setAgency(const QString &agency)
{
  SetRow("AGENCY",agency);
  metadata_changed=true;
}


QString RDCart::publisher() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "PUBLISHER").toString();
}


void RDCart::setPublisher(const QString &publisher)
{
  SetRow("PUBLISHER",publisher);
  metadata_changed=true;
}


QString RDCart::composer() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "COMPOSER").toString();
}


void RDCart::setComposer(const QString &composer)
{
  SetRow("COMPOSER",composer);
  metadata_changed=true;
}


QString RDCart::userDefined() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "USER_DEFINED").toString();
}


void RDCart::setUserDefined(const QString &string)
{
  SetRow("USER_DEFINED",string);
  metadata_changed=true;
}


QString RDCart::songId() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"SONG_ID").toString();
}


void RDCart::setSongId(const QString &id)
{
  SetRow("SONG_ID",id);
  metadata_changed=true;
}


unsigned RDCart::beatsPerMinute() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"BPM").toUInt();
}


void RDCart::setBeatsPerMinute(unsigned bpm)
{
  SetRow("BPM",bpm);
  metadata_changed=true;
}


RDCart::UsageCode RDCart::usageCode() const
{
  return (RDCart::UsageCode) RDGetSqlValue("CART","NUMBER",cart_number,
		      "USAGE_CODE").toInt();
}


void RDCart::setUsageCode(RDCart::UsageCode code)
{
  SetRow("USAGE_CODE",(int)code);
  metadata_changed=true;
}


QString RDCart::notes() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"NOTES").toString();
}


void RDCart::setNotes(const QString &notes)
{
  SetRow("NOTES",notes);
  metadata_changed=true;
}


unsigned RDCart::forcedLength() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "FORCED_LENGTH").toUInt();
}


void RDCart::setForcedLength(unsigned length)
{
  SetRow("FORCED_LENGTH",length);
  metadata_changed=true;
}


unsigned RDCart::lengthDeviation() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "LENGTH_DEVIATION").toUInt();
}


void RDCart::setLengthDeviation(unsigned length) const
{
  SetRow("LENGTH_DEVIATION",length);
}


unsigned RDCart::calculateAverageLength(unsigned *max_dev) const
{
  unsigned total=0;
  unsigned count=0;
  unsigned high=0;
  unsigned low=0xFFFFFFFF;
  unsigned avg=0;
  unsigned weight;
  QDateTime end_datetime;
  QString sql;
  RDSqlQuery *q;

  switch(type()) {
  case RDCart::Audio:
    sql=QString().sprintf("select LENGTH, WEIGHT,END_DATETIME from CUTS\
                           where (CART_NUMBER=%u)&&(LENGTH>0)",
			  cart_number);
    q=new RDSqlQuery(sql);
    while(q->next()) {
      weight = q->value(1).toUInt();
      end_datetime = q->value(2).toDateTime();
      if (end_datetime.isValid() && (end_datetime <QDateTime::currentDateTime ())){
	// This cut has expired, it is no more, set its weight to zero.
	weight = 0;
      }
      total+=(q->value(0).toUInt() * weight);
      if((weight) && (q->value(0).toUInt()>high)) {
	high=q->value(0).toUInt();
      }
      if((weight) && (q->value(0).toUInt()<low)) {
	low=q->value(0).toUInt();
      }
      count += weight;    
    }
    delete q;
    if(count==0) {
      avg=0;
      low=0;
      high=0;
    }
    else {
      avg=total/count;
    }
    if(max_dev!=NULL) {
      if((high-avg)>(avg-low)) {
	*max_dev=high-avg;
      }
      else {
	*max_dev=avg-low;
      }
    }
    break;

  case RDCart::Macro:
  case RDCart::All:
    break;
  }
  return avg;
}


unsigned RDCart::averageLength() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "AVERAGE_LENGTH").toUInt();
}


void RDCart::setAverageLength(unsigned length) const
{
  SetRow("AVERAGE_LENGTH",length);
}


unsigned RDCart::averageSegueLength() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "AVERAGE_SEGUE_LENGTH").toUInt();
}


void RDCart::setAverageSegueLength(unsigned length) const
{
  SetRow("AVERAGE_SEGUE_LENGTH",length);
}


unsigned RDCart::averageHookLength() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "AVERAGE_HOOK_LENGTH").toUInt();
}


void RDCart::setAverageHookLength(unsigned length) const
{
  SetRow("AVERAGE_HOOK_LENGTH",length);
}


unsigned RDCart::cutQuantity() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "CUT_QUANTITY").toUInt();
}


void RDCart::setCutQuantity(unsigned quan) const
{
  SetRow("CUT_QUANTITY",quan);
}


unsigned RDCart::lastCutPlayed() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "LAST_CUT_PLAYED").toUInt();
}


void RDCart::setLastCutPlayed(unsigned cut) const
{
  SetRow("LAST_CUT_PLAYED",cut);
}


RDCart::PlayOrder RDCart::playOrder() const
{
  return (RDCart::PlayOrder)RDGetSqlValue("CART","NUMBER",cart_number,
					 "PLAY_ORDER").toUInt();
}


void RDCart::setPlayOrder(RDCart::PlayOrder order) const
{
  SetRow("PLAY_ORDER",(unsigned)order);
}


RDCart::Validity RDCart::validity() const
{
  return (RDCart::Validity)RDGetSqlValue("CART","NUMBER",cart_number,
					 "VALIDITY").toUInt();
}


void RDCart::setValidity(RDCart::Validity state)
{
  SetRow("VALIDITY",(unsigned)state);
}


QDateTime RDCart::startDateTime() const
{
  QDateTime value;
  value=RDGetSqlValue("CART","NUMBER",cart_number,
		     "START_DATETIME").toDateTime();
  if(value.isValid()) {
    return value;
  }
  return QDateTime(QDate(),QTime());
}


void RDCart::setStartDateTime(const QDateTime &time)
{
  SetRow("START_DATETIME",time);
  metadata_changed=true;
}


void RDCart::setStartDateTime()
{
  SetRow("START_DATETIME");
  metadata_changed=true;
}


QDateTime RDCart::endDateTime() const
{
  QDateTime value;
  value=RDGetSqlValue("CART","NUMBER",cart_number,
		     "END_DATETIME").toDateTime();
  if(value.isValid()) {
    return value;
  }
  return QDateTime(QDate(),QTime());
}


void RDCart::setEndDateTime(const QDateTime &time)
{
  SetRow("END_DATETIME",time);
  metadata_changed=true;
}


void RDCart::setEndDateTime()
{
  SetRow("END_DATETIME");
  metadata_changed=true;
}


bool RDCart::enforceLength() const
{
  return RDBool(RDGetSqlValue("CART","NUMBER",cart_number,
			    "ENFORCE_LENGTH").toString());
}


void RDCart::setEnforceLength(bool state)
{
  SetRow("ENFORCE_LENGTH",RDYesNo(state));
  metadata_changed=true;
}


bool RDCart::preservePitch() const
{
  return RDBool(RDGetSqlValue("CART","NUMBER",cart_number,
			    "PRESERVE_PITCH").toString());
}


void RDCart::setPreservePitch(bool state) const
{
  SetRow("PRESERVE_PITCH",RDYesNo(state));
}


bool RDCart::asyncronous() const
{
  return RDBool(RDGetSqlValue("CART","NUMBER",cart_number,
			      "ASYNCRONOUS").toString());
}


void RDCart::setAsyncronous(bool state) const
{
  SetRow("ASYNCRONOUS",RDYesNo(state));
}


QString RDCart::owner() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"OWNER").toString();
}


void RDCart::setOwner(const QString &owner) const
{
  SetRow("OWNER",owner);
}


bool RDCart::useEventLength() const
{
  return RDBool(RDGetSqlValue("CART","NUMBER",cart_number,
			      "USE_EVENT_LENGTH").toString());
}


void RDCart::setUseEventLength(bool state) const
{
  SetRow("USE_EVENT_LENGTH",RDYesNo(state));
}


void RDCart::setPending(const QString &station_name)
{
#ifndef WIN32
  QString sql;
  RDSqlQuery *q;

  sql=QString("update CART set PENDING_STATION=\"")+
    RDEscapeString(station_name)+"\","+
    "PENDING_DATETIME=now(),"+
    "PENDING_PID="+QString().sprintf("%d ",getpid())+
    QString().sprintf("where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
#endif  // WIN32
}


void RDCart::clearPending() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update CART set PENDING_STATION=NULL,")+
    "PENDING_DATETIME=NULL "+
    QString().sprintf("where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


QString RDCart::macros() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,"MACROS").toString();
}


void RDCart::setMacros(const QString &cmds) const
{
  SetRow("MACROS",cmds);
}


void RDCart::getMetadata(RDWaveData *data) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select TITLE,ARTIST,ALBUM,YEAR,LABEL,CLIENT,")+
    "AGENCY,PUBLISHER,COMPOSER,USER_DEFINED,CONDUCTOR,SONG_ID,BPM,USAGE_CODE"+
    QString().sprintf(" from CART where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    data->setTitle(q->value(0).toString());
    data->setArtist(q->value(1).toString());
    data->setAlbum(q->value(2).toString());
    data->setReleaseYear(q->value(3).toInt());
    data->setLabel(q->value(4).toString());
    data->setClient(q->value(5).toString());
    data->setAgency(q->value(6).toString());
    data->setPublisher(q->value(7).toString());
    data->setComposer(q->value(8).toString());
    data->setUserDefined(q->value(9).toString());
    data->setConductor(q->value(10).toString());
    data->setTmciSongId(q->value(11).toString());
    data->setBeatsPerMinute(q->value(12).toUInt());
    data->setUsageCode(q->value(13).toUInt());
    data->setSchedCodes(schedCodesList());
    data->setMetadataFound(true);
  }
  delete q;
}


void RDCart::setMetadata(const RDWaveData *data)
{
  QString sql="update CART set ";
  if(!data->title().isEmpty()) {
    sql+=QString().sprintf("TITLE=\"%s\",",(const char *)
			   RDEscapeString(VerifyTitle(data->title())).utf8());
  }
  if(!data->artist().isEmpty()) {
    sql+=QString().sprintf("ARTIST=\"%s\",",(const char *)
			   RDEscapeString(data->artist()).utf8());
  }
  if(!data->album().isEmpty()) {
    sql+=QString().sprintf("ALBUM=\"%s\",",(const char *)
			   RDEscapeString(data->album()).utf8());
  }
  if(data->releaseYear()>0) {
    sql+=QString().sprintf("YEAR=\"%04d-01-01\",",data->releaseYear());
  }
  if(!data->label().isEmpty()) {
    sql+=QString().sprintf("LABEL=\"%s\",",(const char *)
			   RDEscapeString(data->label()).utf8());
  }
  if(!data->conductor().isEmpty()) {
    sql+=QString().sprintf("CONDUCTOR=\"%s\",",(const char *)
			   RDEscapeString(data->conductor()).utf8());
  }
  if(!data->client().isEmpty()) {
    sql+=QString().sprintf("CLIENT=\"%s\",",(const char *)
			   RDEscapeString(data->client()).utf8());
  }
  if(!data->agency().isEmpty()) {
    sql+=QString().sprintf("AGENCY=\"%s\",",(const char *)
			   RDEscapeString(data->agency()).utf8());
  }
  if(!data->publisher().isEmpty()) {
    sql+=QString().sprintf("PUBLISHER=\"%s\",",(const char *)
			   RDEscapeString(data->publisher()).utf8());
  }
  if(!data->composer().isEmpty()) {
    sql+=QString().sprintf("COMPOSER=\"%s\",",(const char *)
			   RDEscapeString(data->composer()).utf8());
  }
  if(!data->userDefined().isEmpty()) {
    sql+=QString().sprintf("USER_DEFINED=\"%s\",",(const char *)
			   RDEscapeString(data->userDefined()).utf8());
  }
  if(!data->tmciSongId().isEmpty()) {
    sql+=QString().sprintf("SONG_ID=\"%s\",",(const char *)
			   RDEscapeString(data->tmciSongId()).utf8());
  }
  if(data->beatsPerMinute()>0) {
    sql+=QString().sprintf("BPM=%u,",data->beatsPerMinute());
  }
  sql+=QString().sprintf("USAGE_CODE=%u,",data->usageCode());
  if(sql.right(1)==",") {
    sql=sql.left(sql.length()-1);
    sql+=QString().sprintf(" where NUMBER=%u",cart_number);
    RDSqlQuery *q=new RDSqlQuery(sql);
    delete q;
  }
  setSchedCodesList(data->schedCodes());
  metadata_changed=true;
}


bool RDCart::validateLengths(int len) const
{
  int maxlen=(int)(RD_TIMESCALE_MAX*(double)len);
  int minlen=(int)(RD_TIMESCALE_MIN*(double)len);
  QString sql=QString().sprintf("select LENGTH from CUTS where CART_NUMBER=%u",
				cart_number);
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    if((q->value(0).toInt()>maxlen)||(q->value(0).toInt()<minlen)) {
      delete q;
      return false;
    }
  }
  delete q;

  return true;
}


QString RDCart::xml(bool include_cuts) const
{
#ifdef WIN32
  return QString();
#else
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString ret="";
  RDCut *cut;
  QStringList mlist;

  sql=QString().sprintf("select TYPE,GROUP_NAME,TITLE,ARTIST,ALBUM,YEAR,\
                         LABEL,CLIENT,AGENCY,PUBLISHER,COMPOSER,USER_DEFINED,\
                         USAGE_CODE,FORCED_LENGTH,AVERAGE_LENGTH,\
                         LENGTH_DEVIATION,AVERAGE_SEGUE_LENGTH,\
                         AVERAGE_HOOK_LENGTH,CUT_QUANTITY,LAST_CUT_PLAYED,\
                         VALIDITY,\
                         ENFORCE_LENGTH,ASYNCRONOUS,OWNER,METADATA_DATETIME \
                         from CART where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret+="<cart>\n";
    ret+="  "+RDXmlField("number",cart_number);
    switch((RDCart::Type)q->value(0).toUInt()) {
    case RDCart::Audio:
      ret+="  "+RDXmlField("type","audio");
      break;

    case RDCart::Macro:
      ret+="  "+RDXmlField("type","macro");
      break;

    case RDCart::All:
      break;
    }
    ret+="  "+RDXmlField("groupName",q->value(1).toString());
    ret+="  "+RDXmlField("title",q->value(2).toString());
    ret+="  "+RDXmlField("artist",q->value(3).toString());
    ret+="  "+RDXmlField("album",q->value(4).toString());
    ret+="  "+RDXmlField("year",q->value(5).toDate().toString("yyyy"));
    ret+="  "+RDXmlField("label",q->value(6).toString());
    ret+="  "+RDXmlField("client",q->value(7).toString());
    ret+="  "+RDXmlField("agency",q->value(8).toString());
    ret+="  "+RDXmlField("publisher",q->value(9).toString());
    ret+="  "+RDXmlField("composer",q->value(10).toString());
    ret+="  "+RDXmlField("userDefined",q->value(11).toString());
    ret+="  "+RDXmlField("usageCode",q->value(12).toInt());
    ret+="  "+RDXmlField("forcedLength",
			 RDGetTimeLength(q->value(13).toUInt(),true));
    ret+="  "+RDXmlField("averageLength",
			 RDGetTimeLength(q->value(14).toUInt(),true));
    ret+="  "+RDXmlField("lengthDeviation",
			 RDGetTimeLength(q->value(15).toUInt(),true));
    ret+="  "+RDXmlField("averageSegueLenth",
			 RDGetTimeLength(q->value(16).toUInt(),true));
    ret+="  "+RDXmlField("averageHookLength",
			 RDGetTimeLength(q->value(17).toUInt(),true));
    ret+="  "+RDXmlField("cutQuantity",q->value(18).toUInt());
    ret+="  "+RDXmlField("lastCutPlayed",q->value(19).toUInt());
    ret+="  "+RDXmlField("validity",q->value(20).toUInt());
    ret+="  "+RDXmlField("enforceLength",RDBool(q->value(21).toString()));
    ret+="  "+RDXmlField("asyncronous",RDBool(q->value(22).toString()));
    ret+="  "+RDXmlField("owner",q->value(23).toString());
    ret+="  "+RDXmlField("metadataDatetime",q->value(24).toDateTime());
      switch(type()) {
      case RDCart::Audio:
	if(include_cuts) {
	  ret+="<cutList>\n";
	  sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%u",
				cart_number);
	  q1=new RDSqlQuery(sql);
	  while(q1->next()) {
	    cut=new RDCut(q1->value(0).toString());
	    ret+=cut->xml();
	    delete cut;
	  }
	  delete q1;
	  ret+="</cutList>\n";
	}
	break;

      case RDCart::Macro:
	mlist=mlist.split("!",macros());
	ret+="  <macroList>\n";
	for(unsigned i=0;i<mlist.size();i++) {
	  ret+=QString().sprintf("    <macro%d>",i)+mlist[i]+
	    QString().sprintf("!</macro%d>\n",i);
	}
	ret+="  </macroList>\n";
	break;
	
      case RDCart::All:
	break;
      }
      ret+="</cart>\n";
  }
  delete q;
  return ret;
#endif  // WIN32
}


void RDCart::updateLength()
{
  updateLength(enforceLength(),forcedLength());
}


void RDCart::updateLength(bool enforce_length,unsigned length)
{
  //
  // Update Length
  //
  long long total=0;
  long long segue_total=0;
  long long hook_total=0;
  unsigned weight_total=0;
  unsigned weight = 0;
  QDateTime end_date;

  bool dow_active[7]={false,false,false,false,false,false,false};
  bool time_ok=true;
  QString sql=QString().
    sprintf("select LENGTH,SEGUE_START_POINT,SEGUE_END_POINT,START_POINT,\
             SUN,MON,TUE,WED,THU,FRI,SAT,START_DAYPART,END_DAYPART,\
             HOOK_START_POINT,HOOK_END_POINT,WEIGHT,END_DATETIME \
             from CUTS where (CUT_NAME like \"%06d%%\")&&(LENGTH>0)",
	    cart_number);
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    for(unsigned i=0;i<7;i++) {
      dow_active[i]|=RDBool(q->value(4+i).toString());
    }
    weight = q->value(15).toUInt();
    end_date = q->value(16).toDateTime();
    if (end_date.isValid() && (end_date <QDateTime::currentDateTime ())){
      // This cut has expired, it is no more, set its weight to zero.
      weight = 0;
    }
    total+=q->value(0).toUInt() * weight;
    if((q->value(1).toInt()<0)||(q->value(2).toInt()<0)) {
      segue_total+=q->value(0).toUInt() * weight;
    }
    else {
      segue_total+=(q->value(1).toInt()-q->value(3).toInt()) * weight;
    }
    hook_total+=(q->value(14).toUInt()-q->value(13).toUInt()) * weight;
    weight_total += weight;
  }
  if(weight_total>0) {
    setAverageLength(total/weight_total);
    setAverageSegueLength(segue_total/weight_total);
    setAverageHookLength(hook_total/weight_total);
    if(!enforce_length) {
      setForcedLength(total/weight_total);
    }
  }
  else {
    setAverageLength(0);
    setAverageSegueLength(0);
    setAverageHookLength(0);
    if(!enforce_length) {
      setForcedLength(0);
    }
  }
  setCutQuantity(q->size());
  delete q;

  //
  // Update Validity
  //
  RDCut::Validity cut_validity=RDCut::NeverValid;
  RDCart::Validity cart_validity=RDCart::NeverValid;
  bool evergreen=true;
  QDateTime start_datetime;
  QDateTime end_datetime;
  RDSqlQuery *q1;
  QDateTime valid_until;
  bool dates_valid=true;

  sql=QString().sprintf("select CUT_NAME,START_DAYPART,END_DAYPART,LENGTH,\
                         SUN,MON,TUE,WED,THU,FRI,SAT,EVERGREEN,\
                         START_DATETIME,END_DATETIME from CUTS\
                         where CART_NUMBER=%u",
			cart_number);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cut_validity=ValidateCut(q,enforce_length,length,&time_ok);
    sql=QString().sprintf("update CUTS set VALIDITY=%u where CUT_NAME=\"%s\"",
			  cut_validity,(const char *)q->value(0).toString());
    q1=new RDSqlQuery(sql);
    delete q1;
    evergreen&=RDBool(q->value(11).toString());
    if((int)cut_validity>(int)cart_validity) {
      cart_validity=(RDCart::Validity)cut_validity;
    }
    if((cut_validity!=RDCut::NeverValid)&&(q->value(13).isNull())) {
      dates_valid=false;
    }
    if(!q->value(12).isNull()) {
      if((start_datetime>q->value(12).toDateTime())||
	 start_datetime.isNull()) {
	start_datetime=q->value(12).toDateTime();
      }
    }
    if(!q->value(13).isNull()) {
      if((end_datetime<q->value(13).toDateTime())||
	 (end_datetime.isNull())) {
	end_datetime=q->value(13).toDateTime();
      }
    }
  }
  delete q;
  if(cart_validity==RDCart::ConditionallyValid) {  // Promote to Always?
    bool all_dow=true;
    for(unsigned i=0;i<7;i++) {
      all_dow&=dow_active[i];
    }
    if(all_dow&&time_ok) {
      cart_validity=RDCart::AlwaysValid;
    }
  }
  if(evergreen) {  // Promote to Evergreen?
    cart_validity=RDCart::EvergreenValid;
  }

  //
  // Set start/end datetimes
  //
  sql="update CART set ";
  if(start_datetime.isNull()||(!dates_valid)) {
    sql+="START_DATETIME=NULL,";
  }
  else {
    sql+=QString().sprintf("START_DATETIME=\"%s\",",
		(const char *)start_datetime.toString("yyyy-MM-dd hh:mm:ss"));
  }
  if(end_datetime.isNull()||(!dates_valid)) {
    sql+="END_DATETIME=NULL,";
  }
  else {
    sql+=QString().sprintf("END_DATETIME=\"%s\",",
		(const char *)end_datetime.toString("yyyy-MM-dd hh:mm:ss"));
  }
  sql+=QString().sprintf("VALIDITY=%u where NUMBER=%u",
			 cart_validity,cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::resetRotation() const
{
  QString sql=
    QString().sprintf("update CUTS set LOCAL_COUNTER=0 where CART_NUMBER=%d",
		      cart_number);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::writeTimestamp()
{
  QString sql;
  RDSqlQuery *q;
  sql=QString().sprintf("update CART set METADATA_DATETIME=now() \
                         where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
  metadata_changed=false;
}


int RDCart::addCut(unsigned format,unsigned bitrate,unsigned chans,
		   const QString &isci,QString desc)
{
  RDSqlQuery *q;
  QString sql;
  int next;

  if((next=GetNextFreeCut())<0) {
    return -1;
  }
  QString next_name=QString().sprintf("%06d_%03d",cart_number,next);
  if(desc.isEmpty()) {
    desc=QString().sprintf("Cut %03d",next);
  }
  sql=QString().sprintf("insert into CUTS set CUT_NAME=\"%s\",\
                         CART_NUMBER=%d,ISCI=\"%s\",DESCRIPTION=\"%s\",\
                         LENGTH=0,CODING_FORMAT=%d,BIT_RATE=%d,CHANNELS=%d",
			(const char *)next_name,
			cart_number,
			(const char *)RDEscapeString(isci),
			(const char *)RDEscapeString(desc),
			format,
			bitrate,
			chans);
  q=new RDSqlQuery(sql);
  delete q;

  setCutQuantity(cutQuantity()+1);
  updateLength();
  resetRotation();
  metadata_changed=true;
  return next;
}


bool RDCart::removeAllCuts(RDStation *station,RDUser *user,RDConfig *config)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%u",
			cart_number);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(!removeCut(station,user,q->value(0).toString(),config)) {
      delete q;
      return false;
    }
  }
  delete q;
  metadata_changed=true;
  return true;
}


bool RDCart::removeCut(RDStation *station,RDUser *user,const QString &cutname,
		       RDConfig *config)
{
  if(!exists()) {
    return true;
  }

  QString sql;
  RDSqlQuery *q;
  QString filename;

  filename = RDCut::pathName(cutname); 
  if(!RDCart::removeCutAudio(station,user,cart_number,cutname,config)) {
    return false;
  }
  sql=QString().sprintf("delete from REPL_CUT_STATE where CUT_NAME=\"%s\"",
			(const char *)cutname);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from CUTS where CUT_NAME=\"%s\"",
			(const char *)cutname);
  q=new RDSqlQuery(sql);
  delete q;
  setCutQuantity(cutQuantity()-1);
  metadata_changed=true;

  return true;
}


bool RDCart::removeCutAudio(RDStation *station,RDUser *user,
			    const QString &cutname,RDConfig *config)
{
  return RDCart::removeCutAudio(station,user,cart_number,cutname,config);
}


bool RDCart::create(const QString &groupname,RDCart::Type type)
{
  QString sql=QString().sprintf("insert into CART set NUMBER=%d,TYPE=%d,\
                                 GROUP_NAME=\"%s\",TITLE=\"%s\"",
				cart_number,type,
				(const char *)RDEscapeString(groupname),
				(const char *)
				RDEscapeString(RD_DEFAULT_CART_TITLE));
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool ret=q->isActive();
  delete q;
  metadata_changed=true;

  return ret;
}


bool RDCart::remove(RDStation *station,RDUser *user,RDConfig *config) const
{
  return RDCart::removeCart(cart_number,station,user,config);
}


bool RDCart::exists(unsigned cartnum)
{
  RDSqlQuery *q=new RDSqlQuery(QString().sprintf("select NUMBER from CART\
                                                where NUMBER=%u",cartnum));
  bool ret=q->first();
  delete q;
  return ret;
}


QString RDCart::playOrderText(RDCart::PlayOrder order)
{
  switch(order) {
      case RDCart::Sequence:
	return QObject::tr("Sequentially");

      case RDCart::Random:
	return QObject::tr("Randomly");
  }
  return QObject::tr("Unknown");
}


QString RDCart::usageText(RDCart::UsageCode usage)
{
  switch(usage) {
      case RDCart::UsageFeature:
	return QObject::tr("Feature");

      case RDCart::UsageOpen:
	return QObject::tr("Theme Open");

      case RDCart::UsageClose:
	return QObject::tr("Theme Close");

      case RDCart::UsageTheme:
	return QObject::tr("Theme Open/Close");

      case RDCart::UsageBackground:
	return QObject::tr("Background");

      case RDCart::UsagePromo:
	return QObject::tr("Commercial/Jingle/Promo");

      case RDCart::UsageLast:
	return QObject::tr("Unknown");  
	break;
  }
  return QObject::tr("Unknown");  
}


QString RDCart::typeText(RDCart::Type type)
{
  QString ret=QObject::tr("Unknown");

  switch(type) {
  case RDCart::All:
    ret=QObject::tr("All");
    break;

  case RDCart::Audio:
    ret=QObject::tr("Audio");
    break;

  case RDCart::Macro:
    ret=QObject::tr("Macro");
    break;

  }

  return ret;
}


bool RDCart::removeCart(unsigned cart_num,RDStation *station,RDUser *user,
			RDConfig *config)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select CUT_NAME from CUTS  where CART_NUMBER=%u",
			cart_num);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(!RDCart::removeCutAudio(station,user,cart_num,q->value(0).toString(),
			       config)) {
      delete q;
      return false;
    }
  }
  delete q;
  sql=QString().sprintf("delete from CUTS where CART_NUMBER=%u",cart_num);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from REPL_CART_STATE where CART_NUMBER=%u",
			cart_num);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from CART where NUMBER=%u",cart_num);
  q=new RDSqlQuery(sql);
  delete q;

  return true;
}


bool RDCart::removeCutAudio(RDStation *station,RDUser *user,unsigned cart_num,
			    const QString &cutname,RDConfig *config)
{
  bool ret=true;
#ifndef WIN32
  CURL *curl=NULL;
  long response_code=0;
  char url[1024];
  QString xml="";

  if(user==NULL) { 
    unlink(RDCut::pathName(cutname));
    unlink(RDCut::pathName(cutname)+".energy");
  }
  else {
    //
    // Generate POST Data
    //
    QString post=QString().
      sprintf("COMMAND=%d&LOGIN_NAME=%s&PASSWORD=%s&CART_NUMBER=%u&CUT_NUMBER=%u",
	      RDXPORT_COMMAND_DELETEAUDIO,
	      (const char *)RDFormPost::urlEncode(user->name()),
	      (const char *)RDFormPost::urlEncode(user->password()),
	      cart_num,
	      cutname.right(3).toUInt());
    if((curl=curl_easy_init())==NULL) {
      return false;
    }
    //
    // Write out URL as a C string before passing to curl_easy_setopt(), 
    // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
    // error.
    //
    strncpy(url,station->webServiceUrl(config),1024);
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_POST,1);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,(const char *)post);
    curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,CartWriteCallback);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&xml);
    ret&=curl_easy_perform(curl)==0;
    curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
    ret&=response_code==200;
    curl_easy_cleanup(curl);
  }
#endif  // WIN32
  return ret;
}


void RDCart::removePending(RDStation *station,RDUser *user,RDConfig *config)
{
#ifndef WIN32
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from CART where ")+
    "(PENDING_STATION=\""+RDEscapeString(station->name())+"\")&&"+
    "(PENDING_PID="+QString().sprintf("%d)",getpid());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    
  }
  delete q;
#endif  // WIN32
}


QString RDCart::GetNextCut(RDSqlQuery *q) const
{
  QString cutname;
  double ratio;
  double play_ratio=100000000.0;
  std::vector<int> eligibles;


  while(q->next()) {
    if((ratio=q->value(2).toDouble()/q->value(1).toDouble())<play_ratio) {
      play_ratio=ratio;
      cutname=q->value(0).toString();
    }
  }
  return cutname;
}


int RDCart::GetNextFreeCut() const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%d\
                         order by CUT_NAME",
			cart_number);
  q=new RDSqlQuery(sql);
  for(int i=1;i<=RD_MAX_CUT_NUMBER;i++) {
    if(q->next()) {
      if(q->value(0).toString()!=RDCut::cutName(cart_number,i)) {
	delete q;
	return i;
      }
    }
    else {
      delete q;
      return i;
    }
  }
  return -1;
}


RDCut::Validity RDCart::ValidateCut(RDSqlQuery *q,bool enforce_length,
				    unsigned length,bool *time_ok) const
{
  RDCut::Validity ret=RDCut::AlwaysValid;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());

  if(q->value(3).toUInt()==0) {
    return RDCut::NeverValid;
  }
  if(q->value(11).toString()=="N") {  // Not an Evergreen Cut!
    //
    // Dayparts
    //
    if((!q->value(1).isNull())||(!q->value(2).isNull())) { 
      *time_ok=false;
      ret=RDCut::ConditionallyValid;
    }
    
    //
    // Days of the Week
    //
    bool dow_found=false;
    bool all_dow_found=true;
    for(int i=4;i<11;i++) {
      if(q->value(i).toString()=="Y") {
	dow_found=true;
      }
      else {
	all_dow_found=false;
      }
    }
    if(!dow_found) {
      return RDCut::NeverValid;
    }
    if(!all_dow_found) {
      ret=RDCut::ConditionallyValid;
    }

    //
    // Start/End DayTimes
    //
    if(!q->value(13).isNull()) {
      *time_ok=false;
      if(q->value(13).toDateTime()<current_datetime) {
	return RDCut::NeverValid;
      }
      if(q->value(12).toDateTime()>current_datetime) {
	ret=RDCut::FutureValid;
      }
      else {
	ret=RDCut::ConditionallyValid;
      }
    }
  }

  //
  // Timescaling
  //
  if(enforce_length) {
    double len=(double)length;
    if(((q->value(3).toDouble()*RD_TIMESCALE_MAX)<len)||
       ((q->value(3).toDouble()*RD_TIMESCALE_MIN)>len)) {
      *time_ok=false;
      return RDCut::NeverValid;
    }
  }

  return ret;
}


QString RDCart::VerifyTitle(const QString &title) const
{
  QString ret=title;
  QString sql;
  RDSqlQuery *q;
  RDSystem *system=new RDSystem();

  if(!system->allowDuplicateCartTitles()) {
    int n=1;
    while(1==1) {
      sql=QString().sprintf("select NUMBER from CART \
                             where (TITLE=\"%s\")&&(NUMBER!=%u)",
			    (const char *)RDEscapeString(ret),cart_number);
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	delete q;
	return ret;
      }
      delete q;
      ret=title+QString().sprintf(" [%d]",n++);
    }
  }
  delete system;
  return ret;
}


void RDCart::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CART SET %s=\"%s\" WHERE NUMBER=%u",
			(const char *)param,
			(const char *)RDEscapeString(value.utf8()),
			cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CART SET %s=%d WHERE NUMBER=%u",
			(const char *)param,
			value,
			cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param,const QDateTime &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CART SET %s=\"%s\" WHERE NUMBER=%u",
			(const char *)param,
			(const char *)value.toString("yyyy-MM-dd hh:mm:ss"),
			cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param,const QDate &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CART SET %s=\"%s\" WHERE NUMBER=%u",
			(const char *)param,
			(const char *)value.toString("yyyy-MM-dd"),
			cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CART SET %s=NULL WHERE NUMBER=%u",
			(const char *)param,
			cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}
