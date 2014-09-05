// rdcut.cpp
//
// Abstract a Rivendell Cut.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcut.cpp,v 1.76.6.10.2.2 2014/07/15 20:02:22 cvs Exp $
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
#include <unistd.h>
#endif  // WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <rd.h>
#include <rdconf.h>
#ifndef WIN32
#include <rdwavefile.h>
#endif

#include <rdcut.h>
#include <rdtextvalidator.h>
#include <rdconfig.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdweb.h>
#include <rdcopyaudio.h>

//
// Global Classes
//
RDCut::RDCut(const QString &name,bool create,QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  cut_db=db;
  cut_name=name;

  cut_signal=new QSignal();

  if(name.isEmpty()) {
    cut_number=0;
    cart_number=0;
    return;
  }

  sscanf((const char *)name+7,"%u",&cut_number);
  sscanf((const char *)name.left(6),"%u",&cart_number);
  if(create) {
    sql=QString().sprintf("insert into CUTS set CUT_NAME=\"%s\",CART_NUMBER=%u",
			  (const char *)cut_name,cart_number);
    q=new RDSqlQuery(sql,cut_db);
    delete q;
  }
}


RDCut::RDCut(unsigned cartnum,int cutnum,bool create,QSqlDatabase *db)
{
  RDSqlQuery *q;
  QString sql;

  cut_db=db;
  cut_name=RDCut::cutName(cartnum,cutnum);

  cut_signal=new QSignal();

  if(create) {
    sql=QString().sprintf("insert into CUTS set CUT_NAME=\"%s\",CART_NUMBER=%u",
			  (const char *)cut_name,cartnum);
    q=new RDSqlQuery(sql,cut_db);
    delete q;
  }
  cut_number=cutnum;
  cart_number=cartnum;
}


RDCut::~RDCut()
{
  delete cut_signal;
}


bool RDCut::exists() const
{
  return RDDoesRowExist("CUTS","CUT_NAME",cut_name,cut_db);
}


bool RDCut::isValid() const
{
  return isValid(QDateTime(QDate::currentDate(),QTime::currentTime()));
}


bool RDCut::isValid(const QTime &time) const
{
  return isValid(QDateTime(QDate::currentDate(),time));
}


bool RDCut::isValid(const QDateTime &datetime) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select MON,TUE,WED,THU,FRI,SAT,SUN,EVERGREEN,\
                         START_DATETIME,END_DATETIME,START_DAYPART,END_DAYPART\
                         from CUTS where CUT_NAME=\"%s\"",
			(const char *)cut_name);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  if(q->value(7).toString()=="Y") {   // Evergreen
    delete q;
    return true;
  }
  if(q->value(datetime.date().dayOfWeek()-1).toString()!="Y") {  // Day of Week
    delete q;
    return false;
  }
  if((!q->value(8).isNull())&&(q->value(8).toDateTime()>datetime)) {
    delete q;
    return false;
  }
  if((!q->value(9).isNull())&&(q->value(9).toDateTime()<datetime)) {
    delete q;
    return false;
  }
  if((!q->value(10).isNull())&&(q->value(10).toTime()>datetime.time())) {
    delete q;
    return false;
  }
  if((!q->value(11).isNull())&&(q->value(11).toTime()<datetime.time())) {
    delete q;
    return false;
  }
  delete q;
  return true;
}


QString RDCut::cutName() const
{
  return cut_name;
}


unsigned RDCut::cutNumber() const
{
  return cut_number;
}


unsigned RDCut::cartNumber() const
{
  return cart_number;
}


void RDCut::setCartNumber(unsigned num) const
{
  SetRow("CART_NUMBER",num);
}


bool RDCut::evergreen() const
{
  return RDBool(RDGetSqlValue("CUTS","CUT_NAME",cut_name,"EVERGREEN").
	       toString());
}


void RDCut::setEvergreen(bool state) const
{
  SetRow("EVERGREEN",RDYesNo(state));
}


QString RDCut::description() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"DESCRIPTION",cut_db).
    toString();
}


void RDCut::setDescription(const QString &string) const
{
  SetRow("DESCRIPTION",string);
}


QString RDCut::outcue() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"OUTCUE",cut_db).toString();
}


void RDCut::setOutcue(const QString &string) const
{
  SetRow("OUTCUE",string);
}


QString RDCut::isrc(IsrcFormat fmt) const
{
  QString str= RDGetSqlValue("CUTS","CUT_NAME",cut_name,"ISRC",cut_db).
    toString();
  if((fmt==RDCut::RawIsrc)||(str.length()!=12)) {
    return str;
  }
  str.insert(2,"-");
  str.insert(6,"-");
  str.insert(9,"-");
  return str;
}


void RDCut::setIsrc(const QString &isrc) const
{
  SetRow("ISRC",isrc);
}


QString RDCut::isci() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"ISCI",cut_db).toString();
}


void RDCut::setIsci(const QString &isci) const
{
  SetRow("ISCI",isci);
}


unsigned RDCut::length() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"LENGTH",cut_db).
    toUInt();
}


void RDCut::setLength(int length) const
{
  SetRow("LENGTH",length);
}


QDateTime RDCut::originDatetime(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"ORIGIN_DATETIME",cut_db,valid).
    toDateTime();
}


void RDCut::setOriginDatetime(const QDateTime &datetime) const
{
  SetRow("ORIGIN_DATETIME",datetime);
}


QDateTime RDCut::startDatetime(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"START_DATETIME",cut_db,valid).
    toDateTime();
}


void RDCut::setStartDatetime(const QDateTime &datetime,bool valid) const
{
  if(valid) {
    SetRow("START_DATETIME",datetime);
  }
  else {
    SetRow("START_DATETIME");
  }
}


QDateTime RDCut::endDatetime(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"END_DATETIME",cut_db,valid).
    toDateTime();
}


void RDCut::setEndDatetime(const QDateTime &datetime,bool valid) const
{
  if(valid) {
    SetRow("END_DATETIME",datetime);
  }
  else {
    SetRow("END_DATETIME");
  }
}


QTime RDCut::startDaypart(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"START_DAYPART",cut_db,valid).
    toTime();
}


void RDCut::setStartDaypart(const QTime &time,bool valid) const
{
  if(valid) {
    SetRow("START_DAYPART",time);
  }
  else {
    SetRow("START_DAYPART");
  }
}


bool RDCut::weekPart(int dayofweek) const
{
  return RDBool(RDGetSqlValue("CUTS","CUT_NAME",cut_name,
			    RDGetShortDayNameEN(dayofweek).upper(),cut_db).
	       toString());
}


void RDCut::setWeekPart(int dayofweek,bool state) const
{
  SetRow(RDGetShortDayNameEN(dayofweek).upper(),RDYesNo(state));
}


QTime RDCut::endDaypart(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"END_DAYPART",cut_db,valid).
    toTime();
}


void RDCut::setEndDaypart(const QTime &time,bool valid) const
{
  if(valid) {
    SetRow("END_DAYPART",time);
  }
  else {
    SetRow("END_DAYPART");
  }
}


QString RDCut::originName() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"ORIGIN_NAME",cut_db).
    toString();
}


void RDCut::setOriginName(const QString &name) const
{
  SetRow("ORIGIN_NAME",name);
}


unsigned RDCut::weight() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"WEIGHT",cut_db).
    toUInt();
}


void RDCut::setWeight(int value) const
{
  SetRow("WEIGHT",value);
}


QDateTime RDCut::lastPlayDatetime(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"LAST_PLAY_DATETIME",cut_db,valid).
    toDateTime();
}


void RDCut::setLastPlayDatetime(const QDateTime &datetime,bool valid) const
{
  if(valid) {
    SetRow("LAST_PLAY_DATETIME",datetime);
  }
  else {
    SetRow("LAST_PLAY_DATETIME");
  }
}


QDateTime RDCut::uploadDatetime(bool *valid) const
{
  return 
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"UPLOAD_DATETIME",cut_db,valid).
    toDateTime();
}


void RDCut::setUploadDatetime(const QDateTime &datetime,bool valid) const
{
  if(valid) {
    SetRow("UPLOAD_DATETIME",datetime);
  }
  else {
    SetRow("UPLOAD_DATETIME");
  }
}


unsigned RDCut::playCounter() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"PLAY_COUNTER",cut_db).
    toUInt();
}


void RDCut::setPlayCounter(unsigned count) const
{
  SetRow("PLAY_COUNTER",count);
}


RDCut::Validity RDCut::validity() const
{
  return (RDCut::Validity)
    RDGetSqlValue("CUTS","CUT_NAME",cut_name,"VALIDITY").toUInt();
}


void RDCut::setValidity(RDCut::Validity state)
{
  SetRow("VALIDITY",(int)state);
}


unsigned RDCut::localCounter() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"LOCAL_COUNTER",cut_db).
    toUInt();
}


void RDCut::setLocalCounter(unsigned count) const
{
  SetRow("LOCAL_COUNTER",count);
}


unsigned RDCut::codingFormat() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"CODING_FORMAT",cut_db).
    toUInt();
}


void RDCut::setCodingFormat(unsigned format) const
{
  SetRow("CODING_FORMAT",format);
}


unsigned RDCut::sampleRate() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"SAMPLE_RATE",cut_db).
    toUInt();
}


void RDCut::setSampleRate(unsigned rate) const
{
  SetRow("SAMPLE_RATE",rate);
}


unsigned RDCut::bitRate() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"BIT_RATE",cut_db).
    toUInt();
}


void RDCut::setBitRate(unsigned rate) const
{
  SetRow("BIT_RATE",rate);
}


unsigned RDCut::channels() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"CHANNELS",cut_db).
    toUInt();
}


void RDCut::setChannels(unsigned chan) const
{
  SetRow("CHANNELS",chan);
}


int RDCut::playGain() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"PLAY_GAIN",cut_db).
    toInt();
}


void RDCut::setPlayGain(int gain) const
{
  SetRow("PLAY_GAIN",gain);
}


int RDCut::startPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"START_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"START_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return 0;
}


void RDCut::setStartPoint(int point) const
{
  SetRow("START_POINT",point);
}


int RDCut::endPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"END_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"END_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return (int)length();
}


void RDCut::setEndPoint(int point) const
{
  SetRow("END_POINT",point);
}


int RDCut::fadeupPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"FADEUP_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"FADEUP_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return 0;
}


void RDCut::setFadeupPoint(int point) const
{
  SetRow("FADEUP_POINT",point);
}


int RDCut::fadedownPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"FADEDOWN_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"FADEDOWN_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return effectiveEnd();
}


void RDCut::setFadedownPoint(int point) const
{
  SetRow("FADEDOWN_POINT",point);
}


int RDCut::segueStartPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"SEGUE_START_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"SEGUE_START_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return 0;
}


void RDCut::setSegueStartPoint(int point) const
{
  SetRow("SEGUE_START_POINT",point);
}


int RDCut::segueEndPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"SEGUE_END_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"SEGUE_END_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return effectiveEnd();
}


void RDCut::setSegueEndPoint(int point) const
{
  SetRow("SEGUE_END_POINT",point);
}


int RDCut::segueGain() const
{
  return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"SEGUE_GAIN",cut_db).toInt();
}


void RDCut::setSegueGain(int gain) const
{
  SetRow("SEGUE_GAIN",gain);
}


int RDCut::hookStartPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"HOOK_START_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"HOOK_START_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return 0;
}


void RDCut::setHookStartPoint(int point) const
{
  SetRow("HOOK_START_POINT",point);
}


int RDCut::hookEndPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"HOOK_END_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"HOOK_END_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return effectiveEnd();
}


void RDCut::setHookEndPoint(int point) const
{
  SetRow("HOOK_END_POINT",point);
}


int RDCut::talkStartPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"TALK_START_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"TALK_START_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return 0;
}


void RDCut::setTalkStartPoint(int point) const
{
  SetRow("TALK_START_POINT",point);
}


int RDCut::talkEndPoint(bool calc) const
{
  int n;

  if(!calc) {
    return RDGetSqlValue("CUTS","CUT_NAME",cut_name,"TALK_END_POINT",cut_db).
      toInt();
  }
  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"TALK_END_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return effectiveEnd();
}


void RDCut::setTalkEndPoint(int point) const
{
  SetRow("TALK_END_POINT",point);
}


int RDCut::effectiveStart() const
{
  int n;

  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"START_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return 0;
}


int RDCut::effectiveEnd() const
{
  int n;

  if((n=RDGetSqlValue("CUTS","CUT_NAME",cut_name,"END_POINT",cut_db).
      toInt())!=-1) {
    return n;
  }
  return (int)length();
}


void RDCut::logPlayout() const
{
  QString sql=
    QString().sprintf("update CUTS set LAST_PLAY_DATETIME=\"%s\",\
                       PLAY_COUNTER=%d,LOCAL_COUNTER=%d where CUT_NAME=\"%s\"",
		      (const char *)QDateTime(QDate::currentDate(),
		        QTime::currentTime()).toString("yyyy-MM-dd hh:mm:ss"),
		      playCounter()+1,localCounter()+1,(const char *)cut_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


bool RDCut::copyTo(RDStation *station,RDUser *user,
		   const QString &cutname,RDConfig *config) const
{
#ifdef WIN32
  return false;
#else
  QString sql;
  RDSqlQuery *q;
  bool ret=true;

  //
  // Copy the Database Record
  //
  sql=
    QString().sprintf("select DESCRIPTION,OUTCUE,LENGTH,\
                       CODING_FORMAT,SAMPLE_RATE,\
                       BIT_RATE,CHANNELS,PLAY_GAIN,START_POINT,END_POINT,\
                       FADEUP_POINT,FADEDOWN_POINT,SEGUE_START_POINT,\
                       SEGUE_END_POINT,HOOK_START_POINT,HOOK_END_POINT,\
                       TALK_START_POINT,TALK_END_POINT from CUTS\
                       where CUT_NAME=\"%s\"",(const char *)cut_name);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString().sprintf("update CUTS set\
                           PLAY_COUNTER=0,\
                           DESCRIPTION=\"%s\",\
                           OUTCUE=\"%s\",\
                           LENGTH=%u,\
                           ORIGIN_DATETIME=now(),\
                           ORIGIN_NAME=\"%s\",\
                           CODING_FORMAT=%u,\
                           SAMPLE_RATE=%u,\
                           BIT_RATE=%u,\
                           CHANNELS=%u,\
                           PLAY_GAIN=%d,\
                           START_POINT=%d,\
                           END_POINT=%d,\
                           FADEUP_POINT=%d,\
                           FADEDOWN_POINT=%d,\
                           SEGUE_START_POINT=%d,\
                           SEGUE_END_POINT=%d,\
                           HOOK_START_POINT=%d,\
                           HOOK_END_POINT=%d,\
                           TALK_START_POINT=%d,\
                           TALK_END_POINT=%d where CUT_NAME=\"%s\"",
			  (const char *)q->value(0).toString().utf8(),
			  (const char *)q->value(1).toString().utf8(),
			  q->value(2).toUInt(),
			  (const char *)RDEscapeString(station->name()),
			  q->value(3).toUInt(),
			  q->value(4).toUInt(),
			  q->value(5).toUInt(),
			  q->value(6).toUInt(),
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
			  (const char *)cutname);
  }
  delete q;
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Copy the Audio
  //
  RDCopyAudio *conv=new RDCopyAudio(station,config);
  conv->setSourceCartNumber(cart_number);
  conv->setSourceCutNumber(cut_number);
  conv->setDestinationCartNumber(RDCut::cartNumber(cutname));
  conv->setDestinationCutNumber(RDCut::cutNumber(cutname));
  ret=conv->runCopy(user->name(),user->password())==RDCopyAudio::ErrorOk;
  delete conv;

  return ret;
#endif
}


void RDCut::getMetadata(RDWaveData *data) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select DESCRIPTION,OUTCUE,ISRC,ISCI,ORIGIN_DATETIME,\
                         START_DATETIME,END_DATETIME,START_DAYPART,END_DAYPART,\
                         SEGUE_START_POINT,\
                         SEGUE_END_POINT,TALK_START_POINT,TALK_END_POINT,\
                         START_POINT,END_POINT,HOOK_START_POINT,\
                         HOOK_END_POINT,FADEUP_POINT,FADEDOWN_POINT \
                         from CUTS where CUT_NAME=\"%s\"",
			(const char *)cut_name);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    data->setTitle(q->value(0).toString());
    data->setOutCue(q->value(1).toString());
    data->setIsrc(q->value(2).toString());
    data->setIsci(q->value(3).toString());
    data->setOriginationDate(q->value(4).toDate());
    data->setOriginationTime(q->value(4).toTime());
    data->setStartDate(q->value(5).toDate());
    data->setStartTime(q->value(5).toTime());
    data->setEndDate(q->value(6).toDate());
    data->setEndTime(q->value(6).toTime());
    data->setDaypartStartTime(q->value(7).toTime());
    data->setDaypartEndTime(q->value(8).toTime());
    data->setSegueStartPos(q->value(9).toInt());
    data->setSegueEndPos(q->value(10).toInt());
    data->setIntroStartPos(q->value(11).toInt());
    data->setIntroEndPos(q->value(12).toInt());
    data->setStartPos(q->value(13).toInt());
    data->setEndPos(q->value(14).toInt());
    data->setHookStartPos(q->value(15).toInt());
    data->setHookEndPos(q->value(16).toInt());
    data->setFadeUpPos(q->value(17).toInt());
    data->setFadeDownPos(q->value(18).toInt());
    data->setMetadataFound(true);
  }
  delete q;
}


void RDCut::setMetadata(RDWaveData *data) const
{
  QString sql="update CUTS set ";
  if(!data->description().isEmpty()) {
    sql+=QString().sprintf("DESCRIPTION=\"%s\",",
	      (const char *)RDTextValidator::stripString(data->description()).
			   utf8());
  }
  if(!data->outCue().isEmpty()) {
    sql+=QString().sprintf("OUTCUE=\"%s\",",
    (const char *)RDTextValidator::stripString(data->outCue()).utf8());
  }
  else {
    switch(data->endType()) {
	case RDWaveData::FadeEnd:
	  sql+="OUTCUE=\"[music fades]\",";
	  break;
	  
	case RDWaveData::ColdEnd:
	  sql+="OUTCUE=\"[music ends cold]\",";
	  break;
	  
	case RDWaveData::UnknownEnd:
	  break;
    }
  }
  if(!data->isrc().isEmpty()) {
    sql+=QString().sprintf("ISRC=\"%s\",",
    (const char *)RDTextValidator::stripString(data->isrc()).utf8());
  }
  if(!data->isci().isEmpty()) {
    sql+=QString().sprintf("ISCI=\"%s\",",
    (const char *)RDTextValidator::stripString(data->isci()).utf8());
  }
  if(data->startPos()>=0) {
    sql+=QString().sprintf("START_POINT=%d,",data->startPos());
  }
  if(data->endPos()>=0) {
    sql+=QString().sprintf("END_POINT=%d,",data->endPos());
  }
  if((data->introStartPos()==data->startPos())&&
     (data->introEndPos()==data->endPos())) {
    sql+="TALK_START_POINT=-1,TALK_END_POINT=-1,";
  }
  else {
    if(data->introStartPos()>=0) {
      if(data->introStartPos()<data->startPos()) {
	sql+=QString().sprintf("TALK_START_POINT=%d,",data->startPos());
      }
      else {
	sql+=QString().sprintf("TALK_START_POINT=%d,",data->introStartPos());
      }
    }
    if(data->introEndPos()>=0) {
      if((data->introEndPos()>data->endPos())&&(data->endPos()!=-1)) {
	sql+=QString().sprintf("TALK_END_POINT=%d,",data->endPos());
      }
      else {
	sql+=QString().sprintf("TALK_END_POINT=%d,",data->introEndPos());
      }
    }
  }
  if(((data->segueStartPos()==data->startPos())&&
      (data->segueEndPos()==data->endPos()))||(data->segueStartPos()==0)) {
    sql+="SEGUE_START_POINT=-1,SEGUE_END_POINT=-1,";
  }
  else {
    if(data->segueStartPos()>=0) {
      if(data->segueStartPos()<data->startPos()) {
	sql+=QString().sprintf("SEGUE_START_POINT=%d,",data->startPos());
      }
      else {
	sql+=QString().sprintf("SEGUE_START_POINT=%d,",data->segueStartPos());
      }
    }
    if(data->segueEndPos()>=0) {
      if(data->segueEndPos()>data->endPos()) {
	if(data->endPos()<0) {
	  sql+=QString().sprintf("SEGUE_END_POINT=%d,",data->segueStartPos()+1);
	}
	else {
	  sql+=QString().sprintf("SEGUE_END_POINT=%d,",data->endPos());
	}
      }
      else {
	sql+=QString().sprintf("SEGUE_END_POINT=%d,",data->segueEndPos());
      }
    }
  }
  if(data->startDate().isValid()&&data->endDate().isValid()&&
     (data->startTime().isNull())&&(data->endTime().isNull())) {
    data->setEndTime(QTime(23,59,59));
  }
  if(data->daypartStartTime().isValid()&&data->daypartEndTime().isValid()&&
     (data->daypartStartTime()<data->daypartEndTime())) {
    sql+="START_DAYPART=\""+data->daypartStartTime().toString("hh:mm:ss")+"\","+
      "END_DAYPART=\""+data->daypartEndTime().toString("hh:mm:ss")+"\",";
  }
  if((data->hookStartPos()>=data->startPos())&&
     (data->hookStartPos()<=data->endPos())&&
     (data->hookEndPos()>=data->startPos())&&
     (data->hookEndPos()<=data->endPos())&&
     (data->hookEndPos()>data->hookStartPos())) {
    sql+=QString().sprintf("HOOK_START_POINT=%d,HOOK_END_POINT=%d,",
			   data->hookStartPos(),data->hookEndPos());
  }
  if((data->fadeUpPos()>data->startPos())&&
     (data->fadeUpPos()<=data->endPos())) {
    sql+=QString().sprintf("FADEUP_POINT=%d,",data->fadeUpPos());
  }
  if((data->fadeDownPos()>data->startPos())&&
     (data->fadeDownPos()<=data->endPos())) {
    sql+=QString().sprintf("FADEDOWN_POINT=%d,",data->fadeDownPos());
  }
  if((data->startDate()>QDate(1900,1,1))&&(data->endDate().year()<8000)) {
    if(data->startTime().isValid()) {
    sql+=QString().sprintf("START_DATETIME=\"%s %s\",",
			   (const char *)data->startDate().
			   toString("yyyy-MM-dd"),
			   (const char *)data->startTime().
			   toString("hh:mm:ss"));
    }
    else {
      sql+=QString().sprintf("START_DATETIME=\"%s 00:00:00\",",
			     (const char *)data->startDate().
			     toString("yyyy-MM-dd"));
    }
    if(data->endDate().isValid()&&(data->endDate().year()<8000)) {
      if(data->endTime().isValid()) {
	sql+=QString().sprintf("END_DATETIME=\"%s %s\",",
			       (const char *)data->endDate().
			       toString("yyyy-MM-dd"),
			       (const char *)data->endTime().
			       toString("hh:mm:ss"));
      }
      else {
	sql+=QString().sprintf("END_DATETIME=\"%s 23:59:59\",",
			       (const char *)data->endDate().
			       toString("yyyy-MM-dd"));
      }
    }
  }
  if(sql.right(1)==",") {
    sql=sql.left(sql.length()-1);
  }
  sql+=QString().
    sprintf(" where CUT_NAME=\"%s\"",(const char *)cut_name.utf8());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


QString RDCut::xml() const
{
#ifdef WIN32
  return QString();
#else
  QString sql;
  RDSqlQuery *q;
  QString ret="";

  sql=QString().sprintf("select EVERGREEN,DESCRIPTION,OUTCUE,ISRC,ISCI,\
                         LENGTH,ORIGIN_DATETIME,START_DATETIME,END_DATETIME,\
                         SUN,MON,TUE,WED,THU,FRI,SAT,START_DAYPART,END_DAYPART,\
                         ORIGIN_NAME,WEIGHT,LAST_PLAY_DATETIME,\
                         PLAY_COUNTER,LOCAL_COUNTER,VALIDITY,CODING_FORMAT,\
                         SAMPLE_RATE,BIT_RATE,CHANNELS,PLAY_GAIN,START_POINT,\
                         END_POINT,FADEUP_POINT,FADEDOWN_POINT,\
                         SEGUE_START_POINT,SEGUE_END_POINT,SEGUE_GAIN\
                         HOOK_START_POINT,HOOK_END_POINT,\
                         TALK_START_POINT,TALK_END_POINT \
                         from CUTS where CUT_NAME=\"%s\"",
			(const char *)cut_name);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret+="<cut>\n";
    ret+="  "+RDXmlField("cutName",cut_name);
    ret+="  "+RDXmlField("cartNumber",cart_number);
    ret+="  "+RDXmlField("cutNumber",cut_number);
    ret+="  "+RDXmlField("evergreen",RDBool(q->value(0).toString()));
    ret+="  "+RDXmlField("description",q->value(1).toString());
    ret+="  "+RDXmlField("outcue",q->value(2).toString());
    ret+="  "+RDXmlField("isrc",q->value(3).toString());
    ret+="  "+RDXmlField("isci",q->value(4).toString());
    ret+="  "+RDXmlField("length",q->value(5).toUInt());
    if(q->value(6).isNull()) {
      ret+="  "+RDXmlField("originDatetime","");
    }
    else {
      ret+="  "+RDXmlField("originDatetime",q->value(6).toDateTime());
    }
    if(q->value(7).isNull()) {
      ret+="  "+RDXmlField("startDatetime","");
    }
    else {
      ret+="  "+RDXmlField("startDatetime",q->value(7).toDateTime());
    }
    if(q->value(8).isNull()) {
      ret+="  "+RDXmlField("endDatetime","");
    }
    else {
      ret+="  "+RDXmlField("endDatetime",q->value(8).toDateTime());
    }
    ret+="  "+RDXmlField("sun",RDBool(q->value(9).toString()));
    ret+="  "+RDXmlField("mon",RDBool(q->value(10).toString()));
    ret+="  "+RDXmlField("tue",RDBool(q->value(11).toString()));
    ret+="  "+RDXmlField("wed",RDBool(q->value(12).toString()));
    ret+="  "+RDXmlField("thu",RDBool(q->value(13).toString()));
    ret+="  "+RDXmlField("fri",RDBool(q->value(14).toString()));
    ret+="  "+RDXmlField("sat",RDBool(q->value(15).toString()));
    if(q->value(16).isNull()) {
      ret+="  "+RDXmlField("startDaypart","");
    }
    else {
      ret+="  "+RDXmlField("startDaypart",q->value(16).toTime());
    }
    if(q->value(17).isNull()) {
      ret+="  "+RDXmlField("endDaypart","");
    }
    else {
      ret+="  "+RDXmlField("endDaypart",q->value(17).toTime());
    }
    ret+="  "+RDXmlField("originName",q->value(18).toString());
    ret+="  "+RDXmlField("weight",q->value(19).toUInt());
    ret+="  "+RDXmlField("lastPlayDatetime",q->value(20).toDateTime());
    ret+="  "+RDXmlField("playCounter",q->value(21).toUInt());
    ret+="  "+RDXmlField("localCounter",q->value(22).toUInt());
    ret+="  "+RDXmlField("validity",q->value(23).toUInt());
    ret+="  "+RDXmlField("codingFormat",q->value(24).toUInt());
    ret+="  "+RDXmlField("sampleRate",q->value(25).toUInt());
    ret+="  "+RDXmlField("bitRate",q->value(26).toUInt());
    ret+="  "+RDXmlField("channels",q->value(27).toUInt());
    ret+="  "+RDXmlField("playGain",q->value(28).toUInt());
    ret+="  "+RDXmlField("startPoint",q->value(29).toInt());
    ret+="  "+RDXmlField("endPoint",q->value(30).toInt());
    ret+="  "+RDXmlField("fadeupPoint",q->value(31).toInt());
    ret+="  "+RDXmlField("fadedownPoint",q->value(32).toInt());
    ret+="  "+RDXmlField("segueStartPoint",q->value(33).toInt());
    ret+="  "+RDXmlField("segueEndPoint",q->value(34).toInt());
    ret+="  "+RDXmlField("segueGain",q->value(35).toInt());
    ret+="  "+RDXmlField("hookStartPoint",q->value(36).toInt());
    ret+="  "+RDXmlField("hookEndPoint",q->value(37).toInt());
    ret+="  "+RDXmlField("talkStartPoint",q->value(38).toInt());
    ret+="  "+RDXmlField("talkEndPoint",q->value(39).toInt());
    ret+="</cut>\n";
  }
  delete q;
  return ret;
#endif  // WIN32
}


bool RDCut::checkInRecording(const QString &stationname,RDSettings *settings,
			     unsigned msecs) const
{
#ifdef WIN32
  return false;
#else
  QString sql;
  RDSqlQuery *q;
  int format;

  switch(settings->format()) {
  case RDSettings::MpegL2:
    format=1;
    break;

  default:
    format=0;
    break;
  }
  sql=QString().sprintf("update CUTS set START_POINT=0,END_POINT=%d,\
                         FADEUP_POINT=-1,FADEDOWN_POINT=-1,\
                         SEGUE_START_POINT=-1,SEGUE_END_POINT=-1,\
                         TALK_START_POINT=-1,TALK_END_POINT=-1,\
                         HOOK_START_POINT=-1,HOOK_END_POINT=-1,\
                         PLAY_GAIN=0,PLAY_COUNTER=0,LOCAL_COUNTER=0,\
                         CODING_FORMAT=%d,SAMPLE_RATE=%d,\
                         BIT_RATE=%d,CHANNELS=%d,LENGTH=%d,\
                         ORIGIN_DATETIME=\"%s %s\",ORIGIN_NAME=\"%s\",\
                         UPLOAD_DATETIME=null \
                         where CUT_NAME=\"%s\"",
			msecs,
			format,
			settings->sampleRate(),
			settings->bitRate(),
			settings->channels(),
			msecs,
			(const char *)QDate::currentDate().
			toString("yyyy-MM-dd"),
			(const char *)QTime::currentTime().
			toString("hh:mm:ss"),
			(const char *)stationname,
			(const char *)cut_name);
  q=new RDSqlQuery(sql);
  delete q;
  return true;
#endif  // WIN32
}


void RDCut::autoTrim(RDCut::AudioEnd end,int level)
{
#ifndef WIN32
  int point;
  int start_point=0;
  int end_point=-1;

  if(!exists()) {
    return;
  }
  QString wavename=RDCut::pathName(cut_name); 
  RDWaveFile *wave=new RDWaveFile(wavename);
  if(!wave->openWave()) {
    delete wave;
    return;
  }
  if(level>=0) {
    if((end==RDCut::AudioHead)||(end==RDCut::AudioBoth)) {
      setStartPoint(0);
    }
    if((end==RDCut::AudioTail)||(end==RDCut::AudioBoth)) {
      setEndPoint(wave->getExtTimeLength());
    }
    setLength(endPoint()-startPoint());
    delete wave;
    return;
  }
  if((end==RDCut::AudioHead)||(end==RDCut::AudioBoth)) {
    if((point=wave->startTrim(REFERENCE_LEVEL-level))>-1) {
      start_point=(int)(1000.0*(double)point/(double)wave->getSamplesPerSec());
    }
  }
  if((end==RDCut::AudioTail)||(end==RDCut::AudioBoth)) {
    if((point=wave->endTrim(+REFERENCE_LEVEL-level))>-1) {
      end_point=(int)(1000.0*(double)point/(double)wave->getSamplesPerSec());
    }
    else {
      end_point=wave->getExtTimeLength();
    }
  }
  else {
    end_point=wave->getExtTimeLength();
  }
  setStartPoint(start_point);
  setEndPoint(end_point);
  if(segueEndPoint()>end_point) {
    setSegueEndPoint(end_point);
  }
  if(segueStartPoint()>end_point) {
    setSegueStartPoint(-1);
    setSegueEndPoint(-1);
  }
  setLength(end_point-start_point);
  delete wave;
#endif  // WIN32
}


void RDCut::autoSegue(int level,int length)
{
#ifndef WIN32
  int point;
  int start_point;

  if(!exists()) {
    return;
  }
  QString wavename=RDCut::pathName(cut_name); 
  RDWaveFile *wave=new RDWaveFile(wavename);
  if(!wave->openWave()) {
    delete wave;
    return;
  }
  if(level<0) {
    if((point=wave->endTrim(+REFERENCE_LEVEL-level))>-1) {
      start_point=(int)(1000.0*(double)point/(double)wave->getSamplesPerSec());
      setSegueStartPoint(start_point);
      if(length>0 && (start_point+length)<endPoint()){
        setSegueEndPoint(start_point+length);
        }
      else {
        setSegueEndPoint(endPoint());
        }
      }
    }
  else {
    if(length>0) {
       if((endPoint()-length)>startPoint()){
          setSegueStartPoint(endPoint()-length);
          setSegueEndPoint(endPoint());
          }
       else {
          setSegueStartPoint(startPoint());
          setSegueEndPoint(endPoint());
          }
       }
    }
  delete wave;
#endif  // WIN32
}


void RDCut::reset() const
{
#ifndef WIN32
  QString sql;
  RDSqlQuery *q;
  int format;

  if(!exists()) {
    return;
  }
  RDWaveFile *wave=new RDWaveFile(RDCut::pathName(cut_name)); 
  if(wave->openWave()) {
    switch(wave->getFormatTag()) {
	case WAVE_FORMAT_MPEG:
	  format=wave->getHeadLayer()-1;
	  break;
	default:
	  format=0;
	  break;
    }
    sql=QString().sprintf("update CUTS set LENGTH=%u,\
                           ORIGIN_DATETIME=NOW(),\
                           ORIGIN_NAME=\"\",\
                           LAST_PLAY_DATETIME=NULL,PLAY_COUNTER=0,\
                           CODING_FORMAT=%d,SAMPLE_RATE=%u,BIT_RATE=%u,\
                           CHANNELS=%u,PLAY_GAIN=0,\
                           START_POINT=0,END_POINT=%u,FADEUP_POINT=-1,\
                           FADEDOWN_POINT=-1,\
                           SEGUE_START_POINT=-1,SEGUE_END_POINT=-1,\
		           SEGUE_GAIN=%d,\
                           HOOK_START_POINT=-1,HOOK_END_POINT=-1,\
                           TALK_START_POINT=-1,TALK_END_POINT=-1 \
                           where CUT_NAME=\"%s\"",
			  wave->getExtTimeLength(),
			  format,
			  wave->getSamplesPerSec(),
			  wave->getHeadBitRate(),
			  wave->getChannels(),
			  wave->getExtTimeLength(),
			  RD_FADE_DEPTH,
			  (const char *)cut_name);
  }
  else {
    sql=QString().sprintf("update CUTS set LENGTH=0,\
                           ORIGIN_DATETIME=NULL,\
                           ORIGIN_NAME=\"\",\
                           LAST_PLAY_DATETIME=NULL,PLAY_COUNTER=0,\
                           CODING_FORMAT=0,SAMPLE_RATE=0,BIT_RATE=0,\
                           CHANNELS=0,PLAY_GAIN=0,\
                           START_POINT=-1,END_POINT=-1,FADEUP_POINT=-1,\
                           FADEDOWN_POINT=-1,\
                           SEGUE_START_POINT=-1,SEGUE_END_POINT=-1,\
		           SEGUE_GAIN= %d,\
                           HOOK_START_POINT=-1,HOOK_END_POINT=-1,\
                           TALK_START_POINT=-1,TALK_END_POINT=-1 \
                           where CUT_NAME=\"%s\"",
			   RD_FADE_DEPTH,
			  (const char *)cut_name);
  }
  q=new RDSqlQuery(sql,cut_db);
  delete q;
  wave->closeWave();
  delete wave;
#endif  // WIN32
}


void RDCut::connect(QObject *receiver,const char *member) const
{
  cut_signal->connect(receiver,member);
}


void RDCut::disconnect(QObject *receiver,const char *member) const
{
  cut_signal->disconnect(receiver,member);
}


QString RDCut::cutName(unsigned cartnum,unsigned cutnum)
{
  if((cartnum<1)||(cartnum>RD_MAX_CART_NUMBER)||
     (cutnum<1)||(cutnum>RD_MAX_CUT_NUMBER)) {
    return QString();
  }
  return QString().sprintf("%06u_%03u",cartnum,cutnum);
}


unsigned RDCut::cartNumber(const QString &cutname)
{
  return cutname.left(6).toUInt();
}


unsigned RDCut::cutNumber(const QString &cutname)
{
  return cutname.right(3).toUInt();
}


bool RDCut::exists(unsigned cartnum,unsigned cutnum)
{
  return RDCut::exists(RDCut::cutName(cartnum,cutnum));
}


bool RDCut::exists(const QString &cutname)
{
  RDSqlQuery *q=new RDSqlQuery(QString().sprintf("select CUT_NAME from CUTS\
                                                where CUT_NAME=\"%s\"",
					       (const char *)cutname));
  bool ret=q->first();
  delete q;
  return ret;
}


QString RDCut::pathName(unsigned cartnum,unsigned cutnum)
{
  return RDCut::pathName(RDCut::cutName(cartnum,cutnum));
}


QString RDCut::pathName(const QString &cutname)
{
  return RDConfiguration()->audioFileName(cutname); 
}


bool RDCut::FileCopy(const QString &srcfile,const QString &destfile) const
{
#ifndef WIN32
  int src_fd;
  int dest_fd;
  struct stat src_stat;
  struct stat dest_stat;
  char *buf=NULL;
  int n;
  unsigned bytes=0;
  int previous_step=0;
  int step=0;

  if((src_fd=open((const char *)srcfile.utf8(),O_RDONLY))<0) {
    return false;
  }
  if(fstat(src_fd,&src_stat)<0) {
    close(src_fd);
    return false;
  }
  if((dest_fd=open((const char *)destfile.utf8(),O_RDWR|O_CREAT,src_stat.st_mode))
     <0) {
    close(src_fd);
    return false;
  }
  if(fstat(dest_fd,&dest_stat)<0) {
    close(src_fd);
    close(dest_fd);
    return false;
  }
  buf=(char *)malloc(dest_stat.st_blksize);
  while((n=read(src_fd,buf,dest_stat.st_blksize))==dest_stat.st_blksize) {
    write(dest_fd,buf,dest_stat.st_blksize);
    bytes+=dest_stat.st_blksize;
    if((step=10*bytes/src_stat.st_size)!=previous_step) {
      cut_signal->setValue(step);
      cut_signal->activate();
      previous_step=step;
    }
  }
  write(dest_fd,buf,n);
  cut_signal->setValue(10);
  cut_signal->activate();
  free(buf);
  close(src_fd);
  close(dest_fd);
#endif
  return true;
}


void RDCut::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CUTS SET %s=\"%s\" WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			(const char *)RDEscapeString(value.utf8()),
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}


void RDCut::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CUTS SET %s=%u WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}


void RDCut::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CUTS SET %s=%d WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}


void RDCut::SetRow(const QString &param,const QDateTime &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CUTS SET %s=\"%s\" WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			(const char *)value.toString("yyyy-MM-dd hh:mm:ss"),
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}


void RDCut::SetRow(const QString &param,const QDate &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE CUTS SET %s=\"%s\" WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			(const char *)value.toString("yyyy-MM-dd"),
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}


void RDCut::SetRow(const QString &param,const QTime &value) const
{
  RDSqlQuery *q;
  QString sql;
  sql=QString().sprintf("UPDATE CUTS SET %s=\"%s\" WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			(const char *)value.toString("hh:mm:ss"),
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}


void RDCut::SetRow(const QString &param) const
{
  RDSqlQuery *q;
  QString sql;
  sql=QString().sprintf("UPDATE CUTS SET %s=NULL WHERE CUT_NAME=\"%s\"",
			(const char *)param,
			(const char *)cut_name);
  q=new RDSqlQuery(sql,cut_db);
  delete q;
}
