// rdcart.cpp
//
// Abstract a Rivendell Cart.
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

#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <curl/curl.h>

#include <vector>

#include <qstringlist.h>
#include <qobject.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rdconfig.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdescape_string.h>
#include <rdformpost.h>
#include <rdgroup.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rdtextvalidator.h>
#include <rdxport_interface.h>
#include <rdweb.h>

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
    rda->syslog(LOG_DEBUG,
	   "RDCart::selectCut(): cart doesn't exist, CUT=%s",(const char *)cut);
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

  switch(type()) {
  case RDCart::Audio:
    sql=QString("select ")+
      "CUT_NAME,"+
      "PLAY_ORDER,"+
      "WEIGHT,"+
      "LOCAL_COUNTER,"+
      "LAST_PLAY_DATETIME "+
      "from CUTS  where ("+
      "((START_DATETIME<=\""+datetime_str+"\")&&"+
      "(END_DATETIME>=\""+datetime_str+"\"))||"+
      "(START_DATETIME is null))&&"+
      "(((START_DAYPART<=\""+time_str+"\")&&"+
      "(END_DAYPART>=\""+time_str+"\")||"+
      "START_DAYPART is null))&&"+
      "("+RDGetShortDayNameEN(current_date.dayOfWeek()).upper()+"=\"Y\")&&"+
      QString().sprintf("(CART_NUMBER=%u)&&(EVERGREEN=\"N\")&&",cart_number)+
      "(LENGTH>0)";
    if(useWeighting()) {
      sql+=" order by LOCAL_COUNTER ASC, ISNULL(END_DATETIME), END_DATETIME ASC, \
             LAST_PLAY_DATETIME ASC";
    }
    else {
      sql+=" order by LAST_PLAY_DATETIME desc, PLAY_ORDER desc";
    }
    q=new RDSqlQuery(sql);
    cutname=GetNextCut(q);
    delete q;
    break;

  case RDCart::Macro:
  case RDCart::All:
    break;
  }
  if(cutname.isEmpty()) {   // No valid cuts, try the evergreen
    sql=QString("select ")+
      "CUT_NAME,"+
      "PLAY_ORDER,"+
      "WEIGHT,"+
      "LOCAL_COUNTER "+
      "LAST_PLAY_DATETIME "+
      "from CUTS where "+
      QString().sprintf("(CART_NUMBER=%u)&&",cart_number)+
      "(EVERGREEN=\"Y\")&&"+
      "(LENGTH>0)";
    if(useWeighting()) {
      sql+=" order by LOCAL_COUNTER";
    }
    else {
      sql+=" order by LAST_PLAY_DATETIME desc";
    }
    q=new RDSqlQuery(sql);
    cutname=GetNextCut(q);
    delete q;
  }
  if(cutname.isEmpty()) {
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
  SetRow("TITLE",title);
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
  QStringList f0=
    RDGetSqlValue("CART","NUMBER",cart_number,"YEAR").toString().split("-");
  return f0[0].toInt();
}


void RDCart::setYear(int year)
{
  if((year>0)&&(year<10000)) {
    SetRow("YEAR",QString().sprintf("%04d-01-01",year));
  }
  else {
    SetRow("YEAR");
  }
  metadata_changed=true;
}


QString RDCart::schedCodes() const
{
  QString sched_codes="";

  QStringList list = schedCodesList();

  for(int i=0;i<list.size();i++) {
    sched_codes+=QString().sprintf("%-11s",(const char *)list.at(i));
  }
  sched_codes+=".";
  return sched_codes;
}


void RDCart::setSchedCodes(const QString &sched_codes) const
{
  QStringList list;

  for(int i=0;i<255;i+=11) {
    QString code=sched_codes.mid(i,11);
    if((!code.isEmpty())&&(code.stripWhiteSpace()!=".")) {
      list.push_back(code.stripWhiteSpace());
    }
  }
  setSchedCodesList(list);
}


QStringList RDCart::schedCodesList() const
{
  QStringList list;
  RDSqlQuery *q;

  QString sql=QString().sprintf("select SCHED_CODE from CART_SCHED_CODES where CART_NUMBER=%u", cart_number);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    list.push_back(q->value(0).toString());
  }

  return list;
}


void RDCart::setSchedCodesList(const QStringList &codes) const
{
  RDSqlQuery *q;
  QString sql;
  QString sched_codes="";
  
  sql=QString().sprintf("delete from CART_SCHED_CODES where CART_NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;

  for(int i=0;i<codes.size();i++) {
    sql=QString().sprintf("insert into CART_SCHED_CODES set CART_NUMBER=%u,SCHED_CODE='%s'",cart_number,(const char *)codes.at(i));
    q=new RDSqlQuery(sql);
    delete q;
  }
}


void RDCart::addSchedCode(const QString &code) const
{
  QStringList codes=schedCodesList();
  codes.push_back(code);
  setSchedCodesList(codes);
}


void RDCart::removeSchedCode(const QString &code) const
{
  QStringList codes=schedCodesList();
  QStringList new_codes;

  for(int i=0;i<codes.size();i++) {
    if(codes[i].lower()!=code.lower()) {
      new_codes.push_back(codes[i]);
    }
  }
  setSchedCodesList(new_codes);
}


void RDCart::updateSchedCodes(const QString &add_codes,const QString &remove_codes) const
{
  QString sched_codes;
  QStringList save_codes;
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
          save_codes.push_back(wstr.stripWhiteSpace());
  	}
  }
  delete q;
  setSchedCodesList(save_codes);
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


unsigned RDCart::minimumTalkLength() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "MINIMUM_TALK_LENGTH").toUInt();
}


void RDCart::setMinimumTalkLength(unsigned length) const
{
  SetRow("MINIMUM_TALK_LENGTH",length);
}


unsigned RDCart::maximumTalkLength() const
{
  return RDGetSqlValue("CART","NUMBER",cart_number,
		      "MAXIMUM_TALK_LENGTH").toUInt();
}


void RDCart::setMaximumTalkLength(unsigned length) const
{
  SetRow("MAXIMUM_TALK_LENGTH",length);
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


bool RDCart::useWeighting() const
{
  return RDBool(RDGetSqlValue("CART","NUMBER",cart_number,
			    "USE_WEIGHTING").toString());
}


void RDCart::setUseWeighting(bool state)
{
  SetRow("USE_WEIGHTING",RDYesNo(state));
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
  QString sql;
  RDSqlQuery *q;

  sql=QString("update CART set PENDING_STATION=\"")+
    RDEscapeString(station_name)+"\","+
    "PENDING_DATETIME=now(),"+
    "PENDING_PID="+QString().sprintf("%d ",getpid())+
    QString().sprintf("where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
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

  sql=QString("select ")+
    "TITLE,"+         // 00
    "ARTIST,"+        // 01
    "ALBUM,"+         // 02
    "YEAR,"+          // 03
    "LABEL,"+         // 04
    "CLIENT,"+        // 05
    "AGENCY,"+        // 06
    "PUBLISHER,"+     // 07
    "COMPOSER,"+      // 08
    "USER_DEFINED,"+  // 09
    "CONDUCTOR,"+     // 10
    "SONG_ID,"+       // 11
    "BPM,"+           // 12
    "USAGE_CODE "+    // 13
    QString().sprintf(" from CART where NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    data->setCartNumber(cart_number);
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
    data->setUsageCode((RDWaveData::UsageCode)q->value(13).toUInt());
    data->setSchedCodes(schedCodesList());
    data->setMetadataFound(true);
  }
  delete q;
}


void RDCart::setMetadata(const RDWaveData *data)
{
  QString sql="update CART set ";
  if(!data->title().isEmpty()) {
    sql+=QString("TITLE=\"")+RDEscapeString(data->title())+"\",";
  }
  if(!data->artist().isEmpty()) {
    sql+=QString("ARTIST=\"")+RDEscapeString(data->artist())+"\",";
  }
  if(!data->album().isEmpty()) {
    sql+=QString("ALBUM=\"")+RDEscapeString(data->album())+"\",";
  }
  if(data->releaseYear()>0) {
    sql+=QString().sprintf("YEAR=\"%04d-01-01\",",data->releaseYear());
  }
  if(!data->label().isEmpty()) {
    sql+=QString("LABEL=\"")+RDEscapeString(data->label())+"\",";
  }
  if(!data->conductor().isEmpty()) {
    sql+=QString("CONDUCTOR=\"")+RDEscapeString(data->conductor())+"\",";
  }
  if(!data->client().isEmpty()) {
    sql+=QString("CLIENT=\"")+RDEscapeString(data->client())+"\",";
  }
  if(!data->agency().isEmpty()) {
    sql+=QString("AGENCY=\"")+RDEscapeString(data->agency())+"\",";
  }
  if(!data->publisher().isEmpty()) {
    sql+=QString("PUBLISHER=\"")+RDEscapeString(data->publisher())+"\",";
  }
  if(!data->composer().isEmpty()) {
    sql+=QString("COMPOSER=\"")+RDEscapeString(data->composer())+"\",";
  }
  if(!data->userDefined().isEmpty()) {
    sql+=QString("USER_DEFINED=\"")+RDEscapeString(data->userDefined())+"\",";
  }
  if(!data->tmciSongId().isEmpty()) {
    sql+=QString("SONG_ID=\"")+RDEscapeString(data->tmciSongId())+"\",";
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


QString RDCart::xml(bool include_cuts,bool absolute,
		    RDSettings *settings,int cutnum) const
{
  QString sql=RDCart::xmlSql(include_cuts)+
    QString().sprintf(" where (CART.NUMBER=%u)",cart_number);
  if(cutnum>=0) {
    sql+=QString("&&(CUT_NAME=\"")+RDCut::cutName(cart_number,cutnum)+"\")";
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  QString xml=RDCart::xml(q,include_cuts,absolute,settings);
  delete q;

  return xml;
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
  long long min_talk_len=LLONG_MAX;
  long long max_talk_len=0;
  unsigned weight_total=0;
  unsigned weight = 0;
  QDateTime end_date;

  bool dow_active[7]={false,false,false,false,false,false,false};
  bool time_ok=true;
  QString sql=QString().
    sprintf("select LENGTH,SEGUE_START_POINT,SEGUE_END_POINT,START_POINT,\
             SUN,MON,TUE,WED,THU,FRI,SAT,START_DAYPART,END_DAYPART,\
             HOOK_START_POINT,HOOK_END_POINT,WEIGHT,END_DATETIME,\
             TALK_START_POINT,TALK_END_POINT \
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
    if(min_talk_len>q->value(18).toUInt()-q->value(17).toUInt()) {
      min_talk_len=q->value(18).toUInt()-q->value(17).toUInt();
    }
    if(max_talk_len<q->value(18).toUInt()-q->value(17).toUInt()) {
      max_talk_len=q->value(18).toUInt()-q->value(17).toUInt();
    }
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
  setMinimumTalkLength(min_talk_len);
  setMaximumTalkLength(max_talk_len);
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

  sql=QString("select ")+
    "CUT_NAME,"+        // 00
    "START_DAYPART,"+   // 01
    "END_DAYPART,"+     // 02
    "LENGTH,"+          // 03
    "SUN,"+             // 04
    "MON,"+             // 05
    "TUE,"+             // 06
    "WED,"+             // 07
    "THU,"+             // 08
    "FRI,"+             // 09
    "SAT,"+             // 10
    "EVERGREEN,"+       // 11
    "START_DATETIME,"+  // 12
    "END_DATETIME "+    // 13
    "from CUTS where "+
    QString().sprintf("CART_NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cut_validity=ValidateCut(q,enforce_length,length,&time_ok);
    sql=QString().sprintf("update CUTS set VALIDITY=%u where ",cut_validity)+
      "CUT_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
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
    sql+=QString("START_DATETIME=")+
      RDCheckDateTime(start_datetime,"yyyy-MM-dd hh:mm:ss")+",";
  }
  if(end_datetime.isNull()||(!dates_valid)) {
    sql+="END_DATETIME=NULL,";
  }
  else {
    sql+=QString("END_DATETIME=")+
      RDCheckDateTime(end_datetime,"yyyy-MM-dd hh:mm:ss")+",";
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
  if(!RDCut::create(next_name)) {
    return -1;
  }
  sql=QString("update CUTS set ")+
    "ISCI=\""+RDEscapeString(isci)+"\","+
    "DESCRIPTION=\""+RDEscapeString(desc)+"\","+
    "LENGTH=0,"+
    QString().sprintf("CODING_FORMAT=%d,",format)+
    QString().sprintf("BIT_RATE=%d,",bitrate)+
    QString().sprintf("CHANNELS=%d,",chans)+
    QString().sprintf("PLAY_ORDER=%d where ",next)+
    "CUT_NAME=\""+next_name+"\"";
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
  sql=QString("delete from REPL_CUT_STATE where ")+
    "CUT_NAME=\""+RDEscapeString(cutname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from CUTS where ")+
    "CUT_NAME=\""+RDEscapeString(cutname)+"\"";
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


bool RDCart::remove(RDStation *station,RDUser *user,RDConfig *config) const
{
  return RDCart::removeCart(cart_number,station,user,config);
}


unsigned RDCart::create(const QString &groupname,RDCart::Type type,
			QString *err_msg,unsigned cartnum)
{
  bool ok=false;

  RDGroup *group=new RDGroup(groupname);
  if(!group->exists()) {
    *err_msg=QObject::tr("No such group");
    delete group;
    return 0;
  }

  if(cartnum==0) {
    while(!ok) {
      if((cartnum=group->nextFreeCart())==0) {
	*err_msg=QObject::tr("No free cart available in group");
	delete group;
	return 0;
      }
      QString sql=QString("insert into CART set ")+
	QString().sprintf("NUMBER=%d,",cartnum)+
	QString().sprintf("TYPE=%d,",type)+
	"GROUP_NAME=\""+RDEscapeString(groupname)+"\","+
	"TITLE=\""+RDEscapeString(RDCart::uniqueCartTitle(cartnum))+"\"";
      RDSqlQuery *q=new RDSqlQuery(sql);
      ok=q->isActive();
      delete q;
    }
    return cartnum;
  }
  else {
    QString sql=QString("insert into CART set ")+
      QString().sprintf("NUMBER=%d,",cartnum)+
      QString().sprintf("TYPE=%d,",type)+
      "GROUP_NAME=\""+RDEscapeString(groupname)+"\","+
      "TITLE=\""+RDEscapeString(RDCart::uniqueCartTitle(cartnum))+"\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    ok=q->isActive();
    delete q;
  }
  delete group;
  if(!ok) {
    return 0;
  }
  return cartnum;
}


QString RDCart::xmlSql(bool include_cuts)
{
  QString sql=QString("select ")+
    "CART.NUMBER,"+                // 00
    "CART.TYPE,"+                  // 01
    "CART.GROUP_NAME,"+            // 02
    "CART.TITLE,"+                 // 03
    "CART.ARTIST,"+                // 04
    "CART.ALBUM,"+                 // 05
    "CART.YEAR,"+                  // 06
    "CART.LABEL,"+                 // 07
    "CART.CLIENT,"+                // 08
    "CART.AGENCY,"+                // 09
    "CART.PUBLISHER,"+             // 10
    "CART.COMPOSER,"+              // 11
    "CART.USER_DEFINED,"+          // 12
    "CART.USAGE_CODE,"+            // 13
    "CART.FORCED_LENGTH,"+         // 14
    "CART.AVERAGE_LENGTH,"+        // 15
    "CART.LENGTH_DEVIATION,"+      // 16
    "CART.AVERAGE_SEGUE_LENGTH,"+  // 17
    "CART.AVERAGE_HOOK_LENGTH,"+   // 18
    "CART.MINIMUM_TALK_LENGTH,"+   // 19
    "CART.MAXIMUM_TALK_LENGTH,"+   // 20
    "CART.CUT_QUANTITY,"+          // 21
    "CART.LAST_CUT_PLAYED,"+       // 22
    "CART.VALIDITY,"+              // 23
    "CART.ENFORCE_LENGTH,"+        // 24
    "CART.ASYNCRONOUS,"+           // 25
    "CART.OWNER,"+                 // 26
    "CART.METADATA_DATETIME,"+     // 27
    "CART.CONDUCTOR,"+             // 28
    "CART.MACROS,"+                // 29
    "CART.SONG_ID ";               // 30
  if(include_cuts) {
    sql+=QString(",")+
      "CUTS.CUT_NAME,"+            // 31
      "CUTS.EVERGREEN,"+           // 32
      "CUTS.DESCRIPTION,"+         // 33
      "CUTS.OUTCUE,"+              // 34
      "CUTS.ISRC,"+                // 35
      "CUTS.ISCI,"+                // 36
      "CUTS.LENGTH,"+              // 37
      "CUTS.ORIGIN_DATETIME,"+     // 38
      "CUTS.START_DATETIME,"+      // 39
      "CUTS.END_DATETIME,"+        // 40
      "CUTS.SUN,"+                 // 41
      "CUTS.MON,"+                 // 42
      "CUTS.TUE,"+                 // 43
      "CUTS.WED,"+                 // 44
      "CUTS.THU,"+                 // 45
      "CUTS.FRI,"+                 // 46
      "CUTS.SAT,"+                 // 47
      "CUTS.START_DAYPART,"+       // 48
      "CUTS.END_DAYPART,"+         // 49
      "CUTS.ORIGIN_NAME,"+         // 50
      "CUTS.ORIGIN_LOGIN_NAME,"+   // 51
      "CUTS.SOURCE_HOSTNAME,"+     // 52
      "CUTS.WEIGHT,"+              // 53
      "CUTS.LAST_PLAY_DATETIME,"+  // 54
      "CUTS.PLAY_COUNTER,"+        // 55
      "CUTS.LOCAL_COUNTER,"+       // 56
      "CUTS.VALIDITY,"+            // 57
      "CUTS.CODING_FORMAT,"+       // 58
      "CUTS.SAMPLE_RATE,"+         // 59
      "CUTS.BIT_RATE,"+            // 60
      "CUTS.CHANNELS,"+            // 61
      "CUTS.PLAY_GAIN,"+           // 62
      "CUTS.START_POINT,"+         // 63
      "CUTS.END_POINT,"+           // 64
      "CUTS.FADEUP_POINT,"+        // 65
      "CUTS.FADEDOWN_POINT,"+      // 66
      "CUTS.SEGUE_START_POINT,"+   // 67
      "CUTS.SEGUE_END_POINT,"+     // 68
      "CUTS.SEGUE_GAIN,"+          // 69
      "CUTS.HOOK_START_POINT,"+    // 70
      "CUTS.HOOK_END_POINT,"+      // 71
      "CUTS.TALK_START_POINT,"+    // 72
      "CUTS.TALK_END_POINT,"+      // 73
      "CUTS.RECORDING_MBID,"+      // 74
      "CUTS.RELEASE_MBID "+        // 75

      "from CART left join CUTS "+
      "on CART.NUMBER=CUTS.CART_NUMBER ";
  }
  else {
    sql+=" from CART ";
  }
  return sql;
}


QString RDCart::xml(RDSqlQuery *q,bool include_cuts,
		    bool absolute,RDSettings *settings,int cutnum)
{
  QStringList mlist;
  unsigned cartnum;
  QString xml="";

  while(q->next()) {
    xml+="<cart>\n";
    xml+="  "+RDXmlField("number",q->value(0).toUInt());
    switch((RDCart::Type)q->value(1).toUInt()) {
    case RDCart::Audio:
      xml+="  "+RDXmlField("type","audio");
      break;

    case RDCart::Macro:
      xml+="  "+RDXmlField("type","macro");
      break;

    case RDCart::All:
      break;
    }
    xml+="  "+RDXmlField("groupName",q->value(2).toString());
    xml+="  "+RDXmlField("title",q->value(3).toString());
    xml+="  "+RDXmlField("artist",q->value(4).toString());
    xml+="  "+RDXmlField("album",q->value(5).toString());
    xml+="  "+RDXmlField("year",q->value(6).toDate().toString("yyyy"));
    xml+="  "+RDXmlField("label",q->value(7).toString());
    xml+="  "+RDXmlField("client",q->value(8).toString());
    xml+="  "+RDXmlField("agency",q->value(9).toString());
    xml+="  "+RDXmlField("publisher",q->value(10).toString());
    xml+="  "+RDXmlField("composer",q->value(11).toString());
    xml+="  "+RDXmlField("conductor",q->value(26).toString());
    xml+="  "+RDXmlField("userDefined",q->value(12).toString());
    xml+="  "+RDXmlField("usageCode",q->value(13).toInt());
    xml+="  "+RDXmlField("forcedLength",
			 "0"+RDGetTimeLength(q->value(14).toUInt(),true));
    xml+="  "+RDXmlField("averageLength",
			 "0"+RDGetTimeLength(q->value(15).toUInt(),true));
    xml+="  "+RDXmlField("lengthDeviation",
			 "0"+RDGetTimeLength(q->value(16).toUInt(),true));
    xml+="  "+RDXmlField("averageSegueLength",
			 "0"+RDGetTimeLength(q->value(17).toUInt(),true));
    xml+="  "+RDXmlField("averageHookLength",
			 "0"+RDGetTimeLength(q->value(18).toUInt(),true));
    xml+="  "+RDXmlField("minimumTalkLength",
			 "0"+RDGetTimeLength(q->value(19).toUInt(),true));
    xml+="  "+RDXmlField("maximumTalkLength",
			 "0"+RDGetTimeLength(q->value(20).toUInt(),true));
    xml+="  "+RDXmlField("cutQuantity",q->value(21).toUInt());
    xml+="  "+RDXmlField("lastCutPlayed",q->value(22).toUInt());
    xml+="  "+RDXmlField("enforceLength",RDBool(q->value(24).toString()));
    xml+="  "+RDXmlField("asyncronous",RDBool(q->value(25).toString()));
    xml+="  "+RDXmlField("owner",q->value(26).toString());
    xml+="  "+RDXmlField("metadataDatetime",q->value(27).toDateTime());
    xml+="  "+RDXmlField("songId",q->value(30).toString());
    switch((RDCart::Type)q->value(1).toInt()) {
    case RDCart::Audio:
      if(include_cuts) {
	cartnum=q->value(0).toUInt();
	if(q->value(31).toString().isEmpty()) {
	  xml+="  <cutList/>\n";
	}
	else {
	  xml+="  <cutList>\n";
	  xml+="  "+RDCut::xml(q,absolute,settings);
	  while(q->next()) {
	    if(q->value(0).toUInt()==cartnum) {
	      xml+="  "+RDCut::xml(q,absolute,settings);
	    }
	    else {
	      q->prev();
	      break;
	    }
	  }
	  xml+="  </cutList>\n";
	}
      }
      break;

    case RDCart::Macro:
      mlist=q->value(29).toString().split("!");
      if(mlist.size()==0) {
	xml+="  <macroList/>\n";
      }
      else {
	xml+="  <macroList>\n";
	for(int i=0;i<mlist.size();i++) {
	  xml+="    "+RDXmlField(QString().sprintf("macro%d",i),mlist[i]+"!");
	}
	xml+="  </macroList>\n";
      }
      break;
	
    case RDCart::All:
      break;
    }
    xml+="</cart>\n";
  }

  return xml;
}


QString RDCart::cutXml(unsigned cartnum,int cutnum,bool absolute,
		       RDSettings *settings)
{
  QString xml="";
  QString sql=RDCart::xmlSql(true)+" where "+
    "CUTS.CUT_NAME=\""+RDCut::cutName(cartnum,cutnum)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    xml=RDCut::xml(q,absolute,settings);
  }
  delete q;

  return xml;
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
  sql=QString().sprintf("delete from CART_SCHED_CODES where CART_NUMBER=%u",cart_num);
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
  CURL *curl=NULL;
  long response_code=0;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;
  char url[1024];
  QString xml="";
  QString sql;
  RDSqlQuery *q;

  if(user==NULL) { 
    unlink(RDCut::pathName(cutname));
    unlink(RDCut::pathName(cutname)+".energy");
    sql=QString("delete from CUT_EVENTS where ")+
      "CUT_NAME=\""+cutname+"\"";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    //
    // Generate POST Data
    //
    curl_formadd(&first,&last,CURLFORM_PTRNAME,"COMMAND",
		 CURLFORM_COPYCONTENTS,
		 (const char *)QString().sprintf("%u",RDXPORT_COMMAND_DELETEAUDIO),
	       CURLFORM_END);
    curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
		 CURLFORM_COPYCONTENTS,(const char *)user->name().utf8(),CURLFORM_END);
    curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
		 CURLFORM_COPYCONTENTS,(const char *)user->password().utf8(),
		 CURLFORM_END);
    curl_formadd(&first,&last,CURLFORM_PTRNAME,"CART_NUMBER",
		 CURLFORM_COPYCONTENTS,
		 (const char *)QString().sprintf("%u",cart_num),
		 CURLFORM_END);
    curl_formadd(&first,&last,CURLFORM_PTRNAME,"CUT_NUMBER",
		 CURLFORM_COPYCONTENTS,
		 (const char *)QString().sprintf("%u",RDCut::cutNumber(cutname)),
		 CURLFORM_END);
    if((curl=curl_easy_init())==NULL) {
      curl_formfree(first);
      return false;
    }
    //
    // Write out URL as a C string before passing to curl_easy_setopt(), 
    // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
    // error.
    //
    strncpy(url,station->webServiceUrl(config),1024);
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
    curl_easy_setopt(curl,CURLOPT_USERAGENT,
		     (const char *)rda->config()->userAgent());
    curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,CartWriteCallback);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&xml);
    ret&=curl_easy_perform(curl)==0;
    curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
    ret&=response_code==200;
    curl_easy_cleanup(curl);
    curl_formfree(first);
  }
  return ret;
}


void RDCart::removePending(RDStation *station,RDUser *user,RDConfig *config)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from CART where ")+
    "(PENDING_STATION=\""+RDEscapeString(station->name())+"\")&&"+
    "(PENDING_PID="+QString().sprintf("%d)",getpid());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    
  }
  delete q;
}


unsigned RDCart::readXml(std::vector<RDWaveData> *data,const QString &xml)
{
  //
  // HACK! HACK! HACK! HACK! HACK!
  //
  // This is totally ad-hoc and will likely break horribly for XML that
  // deviates the least bit from that generated by Rivendell's own xml()
  // methods.
  //
  // Slated to be put out of its misery as soon as we get a Real XML Parser.
  //
  int istate=0;
  RDWaveData cartdata;
  RDSettings *settings=NULL;
  QStringList f0=xml.split("\n");

  for(int i=0;i<f0.size();i++) {
    f0[i]=f0[i].trimmed();
  }

  for(int i=0;i<f0.size();i++) {
    switch(istate) {
    case 0:
      if(f0[i]=="<cart>") {
	istate=1;
      }
      break;

    case 1:  // Cart-level objects
      if(f0[i].contains("<number>")) {
	cartdata.setCartNumber(GetXmlValue("number",f0[i]).toUInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<groupName>")) {
	cartdata.setCategory(GetXmlValue("groupName",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<title>")) {
	cartdata.setTitle(GetXmlValue("title",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<artist>")) {
	cartdata.setArtist(GetXmlValue("artist",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<album>")) {
	cartdata.setAlbum(GetXmlValue("album",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<label>")) {
	cartdata.setLabel(GetXmlValue("label",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<client>")) {
	cartdata.setClient(GetXmlValue("client",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<agency>")) {
	cartdata.setAgency(GetXmlValue("agency",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<composer>")) {
	cartdata.setComposer(GetXmlValue("composer",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<publisher>")) {
	cartdata.setPublisher(GetXmlValue("publisher",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<songId>")) {
	cartdata.setSongId(GetXmlValue("songId",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<conductor>")) {
	cartdata.setConductor(GetXmlValue("conductor",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<userDefined>")) {
	cartdata.setUserDefined(GetXmlValue("userDefined",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<year>")) {
	cartdata.setReleaseYear(GetXmlValue("year",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<forcedLength>")) {
	cartdata.
	  setForcedLength(RDSetTimeLength(GetXmlValue("forcedLength",f0[i]).
					  toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<averageLength>")) {
	cartdata.
	  setAverageLength(RDSetTimeLength(GetXmlValue("averageLength",f0[i]).
					   toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<lengthDeviation>")) {
	cartdata.setLengthDeviation(RDSetTimeLength(GetXmlValue("lengthDeviation",f0[i]).
					   toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<averageHookLength>")) {
	cartdata.
	  setAverageHookLength(RDSetTimeLength(GetXmlValue("averageHookLength",f0[i]).
					   toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<averageSegueLength>")) {
	cartdata.
	  setAverageSegueLength(RDSetTimeLength(GetXmlValue("averageSegueLength",f0[i]).
					   toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<minimumTalkLength>")) {
	cartdata.
	  setMinimumTalkLength(RDSetTimeLength(GetXmlValue("minimumTalkLength",f0[i]).
					   toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<maximumTalkLength>")) {
	cartdata.
	  setMaximumTalkLength(RDSetTimeLength(GetXmlValue("maximumTalkLength",f0[i]).
					   toString()));
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<cutQuantity>")) {
	cartdata.setCutQuantity(GetXmlValue("cutQuantity",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<lastCutPlayed>")) {
	cartdata.setLastCutPlayed(GetXmlValue("lastCutPlayed",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<enforceLength>")) {
	cartdata.setEnforceLength(GetXmlValue("enforceLength",f0[i]).toBool());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<asyncronous>")) {
	cartdata.setAsyncronous(GetXmlValue("asyncronous",f0[i]).toBool());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<owner>")) {
	cartdata.setOwner(GetXmlValue("owner",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<metadataDatetime>")) {
	cartdata.setMetadataDatetime(GetXmlValue("metadataDatetime",f0[i]).
				     toDateTime());
	cartdata.setMetadataFound(true);
      }

      if(f0[i]=="</cart>") {
	istate=0;
      }

      if(f0[i]=="<cutList>") {
	data->push_back(RDWaveData());
	data->back()=cartdata;
	istate=2;
      }
      break;

    case 2:  // Cut List
      if(f0[i]=="<cut>") {  // Start of new cut
	data->push_back(RDWaveData());
	data->back()=cartdata;
	settings=new RDSettings();
	istate=3;
      }
      if(f0[i]=="</cutList>") { // End of cut
	istate=1;
      }
      break;

    case 3:  // Cut Object
      if(f0[i].contains("<description>")) {
	data->
	  back().setDescription(GetXmlValue("description",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<cutNumber>")) {
	data->back().setCutNumber(GetXmlValue("cutNumber",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<cutName>")) {
	data->back().setCutName(GetXmlValue("cutName",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<evergreen>")) {
	data->back().setEvergreen(GetXmlValue("evergreen",f0[i]).toBool());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<outcue>")) {
	data->back().setOutCue(GetXmlValue("outcue",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<isrc>")) {
	data->back().setIsrc(GetXmlValue("isrc",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<isci>")) {
	data->back().setIsci(GetXmlValue("isci",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<recordingMbId>")) {
	data->back().setRecordingMbId(GetXmlValue("recordingMbId",f0[i]).
				      toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<releaseMbId>")) {
	data->back().setReleaseMbId(GetXmlValue("releaseMbId",
						f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<length>")) {
	data->back().setLength(GetXmlValue("length",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<originName>")) {
	data->back().setOriginator(GetXmlValue("originName",f0[i]).toString());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<originDatetime>")) {
	data->back().setOriginationDate(GetXmlValue("originDatetime",f0[i]).
				    toDateTime().date());
	data->back().setOriginationTime(GetXmlValue("originDatetime",f0[i]).
				    toDateTime().time());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<playCounter>")) {
	data->back().setPlayCounter(GetXmlValue("playCounter",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<startDatetime>")) {
	data->back().setStartDate(GetXmlValue("startDatetime",f0[i]).
				    toDateTime().date());
	data->back().setStartTime(GetXmlValue("startDatetime",f0[i]).
				    toDateTime().time());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<endDatetime>")) {
	data->back().setEndDate(GetXmlValue("endDatetime",f0[i]).
			    toDateTime().date());
	data->back().setEndTime(GetXmlValue("endDatetime",f0[i]).
			    toDateTime().time());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<lastPlayDatetime>")) {
	data->back().setLastPlayDatetime(GetXmlValue("lastPlayDatetime",f0[i]).
			    toDateTime());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<codingFormat>")) {
	settings->setFormat((RDSettings::Format)GetXmlValue("codingFormat",f0[i]).
		     toUInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<sampleRate>")) {
	settings->setSampleRate(GetXmlValue("sampleRate",f0[i]).toUInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<bitRate>")) {
	settings->setBitRate(GetXmlValue("bitRate",f0[i]).toUInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<channels>")) {
	settings->setChannels(GetXmlValue("channels",f0[i]).toUInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<playGain>")) {
	data->back().setPlayGain(GetXmlValue("playGain",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<startPoint>")) {
	data->back().setStartPos(GetXmlValue("startPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<endPoint>")) {
	data->back().setEndPos(GetXmlValue("endPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<segueStartPoint>")) {
	data->back().setSegueStartPos(GetXmlValue("segueStartPoint",f0[i]).
				      toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<segueEndPoint>")) {
	data->back().setSegueEndPos(GetXmlValue("segueEndPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<segueGain>")) {
	data->back().setSegueGain(GetXmlValue("segueGain",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<talkStartPoint>")) {
	data->back().setTalkStartPos(GetXmlValue("talkStartPoint",f0[i]).
				      toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<talkEndPoint>")) {
	data->back().setTalkEndPos(GetXmlValue("talkEndPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<hookStartPoint>")) {
	data->back().setHookStartPos(GetXmlValue("hookStartPoint",f0[i]).
				      toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<hookEndPoint>")) {
	data->back().setHookEndPos(GetXmlValue("hookEndPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<fadeupPoint>")) {
	data->back().setFadeUpPos(GetXmlValue("fadeupPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      if(f0[i].contains("<fadedownPoint>")) {
	data->back().setFadeDownPos(GetXmlValue("fadedownPoint",f0[i]).toInt());
	cartdata.setMetadataFound(true);
      }
      
      if(f0[i]=="</cut>") { // End of cut
	data->back().setAudioSettings(*settings);
	delete settings;
	settings=NULL;
	istate=2;
      }
      break;
    }
  }
  return data->size();
}


QString RDCart::uniqueCartTitle(unsigned cartnum)
{
  QString basename=QObject::tr("new cart");
  QString title;
  QString sql;
  RDSqlQuery *q=NULL;
  int count=0;

  if(cartnum!=0) {
    basename=QObject::tr("cart")+QString().sprintf(" %06u",cartnum);
  }
  do {
    title="["+basename+QString().sprintf("-%d",count+1)+"]";
    sql=QString("select NUMBER from CART where ")+
      "TITLE=\""+RDEscapeString(title)+"\"";
    count++;
    if(q!=NULL) {
      delete q;
    }
    q=new RDSqlQuery(sql);
  } while(q->first());

  return title;
}


bool RDCart::titleIsUnique(unsigned except_cartnum,const QString &str)
{
  bool ret=false;

  QString sql=QString("select NUMBER from CART where ")+
    "(TITLE=\""+RDEscapeString(str)+"\")&&"+
    QString().sprintf("NUMBER!=%u",except_cartnum);
  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=!q->first();
  delete q;

  return ret;
}


QString RDCart::ensureTitleIsUnique(unsigned except_cartnum,
				    const QString &str)
{
  QString ret=str;
  QString sql;
  RDSqlQuery *q;
  int n=1;

  while(n<1000000) {
    sql=QString("select ")+
      "NUMBER "+  // 00
      "from CART where "+
      "(TITLE=\""+RDEscapeString(ret)+"\") && "+
      QString().sprintf("(NUMBER!=%u)",except_cartnum);
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      delete q;
      return ret;
    }
    delete q;
    ret=str+QString().sprintf(" [%d]",n++);
  }

  return QString();

}


QVariant RDCart::GetXmlValue(const QString &tag,const QString &line)
{
  bool ok=false;
  QString value=line;
  value=value.remove("<"+tag+">").remove("</"+tag+">");

  value.toUInt(&ok);
  if(ok) {
    return QVariant(value.toUInt());
  }
  value.toInt(&ok);
  if(ok) {
    return QVariant(value.toInt());
  }

  return QVariant(RDXmlUnescape(value));
}


QString RDCart::GetNextCut(RDSqlQuery *q) const
{
  QString cutname;
  double ratio;
  double play_ratio=100000000.0;
  int play=RD_MAX_CUT_NUMBER+1;
  int last_play;

  if(useWeighting()) {
    while(q->next()) {
      if((ratio=q->value(3).toDouble()/q->value(2).toDouble())<play_ratio) {
	play_ratio=ratio;
	cutname=q->value(0).toString();
      }
    }
  }
  else {
    if(q->first()) {
      last_play=q->value(1).toInt();
      while(q->next()) {
	if((q->value(1).toInt()>last_play)&&(q->value(1).toInt()<play)) {
	  play=q->value(1).toInt();
	  cutname=q->value(0).toString();
	}
      }
      if(!cutname.isEmpty()) {
	return cutname;
      }
    }
    q->seek(-1);
    while(q->next()) {
      if(q->value(1).toInt()<play) {
	play=q->value(1).toInt();
	cutname=q->value(0).toString();
      }
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
      sql=QString("select NUMBER from CART where ")+
       "(TITLE=\""+RDEscapeString(ret)+"\")&&"+
       QString().sprintf("(NUMBER!=%u)",cart_number);
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

  sql=QString("update CART set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    QString().sprintf("NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update CART set ")+
    param+QString().sprintf("=%d where NUMBER=%u",value,cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param,const QDateTime &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update CART set ")+
    param+"="+RDCheckDateTime(value,"yyyy-MM-dd hh:mm:ss")+" where "+
    QString().sprintf("NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param,const QDate &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update CART set ")+
    param+"="+RDCheckDateTime(value,"yyyy-MM-dd")+" where "+
    QString().sprintf("NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDCart::SetRow(const QString &param) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update CART set ")+
    param+"=NULL where "+
    QString().sprintf("NUMBER=%u",cart_number);
  q=new RDSqlQuery(sql);
  delete q;
}
