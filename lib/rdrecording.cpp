// rdrecording.cpp
//
// Abstract a Rivendell Netcatch Recording.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdrecording.cpp,v 1.30 2012/01/12 16:24:50 cvs Exp $
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

#include <unistd.h>

#include <qobject.h>

#include <rd.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdrecording.h>
#include <rddeck.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDRecording::RDRecording(int id,bool create)
{
  RDSqlQuery *q;
  QString sql;

  if(id<0) {
    rec_id=AddRecord();
    create=false;
  }
  else {
    rec_id=id;
  }

  if(create) {
    sql=QString().sprintf("select ID from RECORDINGS where ID=%d",rec_id);
    q=new RDSqlQuery(sql);
    if(q->size()!=1) {
      delete q;
      sql=QString().sprintf("insert into RECORDINGS set ID=%d",rec_id);
      q=new RDSqlQuery(sql);
    }
    delete q;
  }
}


int RDRecording::id() const
{
  return rec_id;
}


bool RDRecording::isActive() const
{
  return RDBool(GetStringValue("IS_ACTIVE"));
}


void RDRecording::setIsActive(bool state) const
{
  SetRow("IS_ACTIVE",RDYesNo(state));
}


QString RDRecording::station() const
{
  return GetStringValue("STATION_NAME");
}


void RDRecording::setStation(QString name) const
{
  SetRow("STATION_NAME",name);
}


RDRecording::Type RDRecording::type() const
{
  return (RDRecording::Type)GetIntValue("TYPE");
}


void RDRecording::setType(RDRecording::Type type) const
{
  SetRow("TYPE",(int)type);
}


unsigned RDRecording::channel() const
{
  return GetUIntValue("CHANNEL");
}


void RDRecording::setChannel(unsigned chan) const
{
  SetRow("CHANNEL",chan);
}


QTime RDRecording::startTime() const
{
  return GetTimeValue("START_TIME");
}


void RDRecording::setStartTime(QTime time) const
{
  SetRow("START_TIME",time);
}


QTime RDRecording::endTime() const
{
  return GetTimeValue("END_TIME");
}


void RDRecording::setEndTime(QTime time) const
{
  SetRow("END_TIME",time);
}


QString RDRecording::cutName() const
{
  return GetStringValue("CUT_NAME");
}


void RDRecording::setCutName(QString name) const
{
  SetRow("CUT_NAME",name);
}


bool RDRecording::sun() const
{
  return GetBoolValue("SUN");
}


void RDRecording::setSun(bool state) const
{
  SetRow("SUN",state);
}


bool RDRecording::mon() const
{
  return GetBoolValue("MON");
}


void RDRecording::setMon(bool state) const
{
  SetRow("MON",state);
}


bool RDRecording::tue() const
{
  return GetBoolValue("TUE");
}


void RDRecording::setTue(bool state) const
{
  SetRow("TUE",state);
}


bool RDRecording::wed() const
{
  return GetBoolValue("WED");
}


void RDRecording::setWed(bool state) const
{
  SetRow("WED",state);
}


bool RDRecording::thu() const
{
  return GetBoolValue("THU");
}


void RDRecording::setThu(bool state) const
{
  SetRow("THU",state);
}


bool RDRecording::fri() const
{
  return GetBoolValue("FRI");
}


void RDRecording::setFri(bool state) const
{
  SetRow("FRI",state);
}


bool RDRecording::sat() const
{
  return GetBoolValue("SAT");
}


void RDRecording::setSat(bool state) const
{
  SetRow("SAT",state);
}


int RDRecording::switchSource() const
{
  return GetIntValue("SWITCH_INPUT");
}


void RDRecording::setSwitchSource(int input) const
{
  SetRow("SWITCH_INPUT",input);
}


int RDRecording::switchDestination() const
{
  return GetIntValue("SWITCH_OUTPUT");
}


void RDRecording::setSwitchDestination(int output) const
{
  SetRow("SWITCH_OUTPUT",output);
}


QString RDRecording::description() const
{
  return GetStringValue("DESCRIPTION");
}


void RDRecording::setDescription(QString string) const
{
  SetRow("DESCRIPTION",string);
}


unsigned RDRecording::length() const
{
  return GetUIntValue("LENGTH");
}


void RDRecording::setLength(unsigned length) const
{
  SetRow("LENGTH",length);
}


int RDRecording::startGpi() const
{
  return GetIntValue("START_GPI");
}


void RDRecording::setStartGpi(int line) const
{
  SetRow("START_GPI",line);
}


int RDRecording::endGpi() const
{
  return GetIntValue("END_GPI");
}


void RDRecording::setEndGpi(int line) const
{
  SetRow("END_GPI",line);
}


bool RDRecording::allowMultipleRecordings() const
{
  return RDBool(GetStringValue("ALLOW_MULT_RECS"));
}


void RDRecording::setAllowMultipleRecordings(bool state) const
{
  SetRow("ALLOW_MULT_RECS",state);
}


unsigned RDRecording::maxGpiRecordingLength() const
{
  return GetUIntValue("MAX_GPI_REC_LENGTH");
}


void RDRecording::setMaxGpiRecordingLength(unsigned len) const
{
  SetRow("MAX_GPI_REC_LENGTH",len);
}


unsigned RDRecording::trimThreshold() const
{
  return GetUIntValue("TRIM_THRESHOLD");
}


void RDRecording::setTrimThreshold(unsigned level) const
{
  SetRow("TRIM_THRESHOLD",level);
}


unsigned RDRecording::startdateOffset() const
{
  return GetUIntValue("STARTDATE_OFFSET");
}


void RDRecording::setStartdateOffset(unsigned offset) const
{
  SetRow("STARTDATE_OFFSET",offset);
}


unsigned RDRecording::enddateOffset() const
{
  return GetUIntValue("ENDDATE_OFFSET");
}


void RDRecording::setEnddateOffset(unsigned offset) const
{
  SetRow("ENDDATE_OFFSET",offset);
}


int RDRecording::eventdateOffset() const
{
  return GetIntValue("EVENTDATE_OFFSET");
}


void RDRecording::setEventdateOffset(int offset) const
{
  SetRow("EVENTDATE_OFFSET",offset);
}


RDSettings::Format RDRecording::format() const
{
  return (RDSettings::Format)GetIntValue("FORMAT");
}


void RDRecording::setFormat(RDSettings::Format fmt) const
{
  SetRow("FORMAT",(int)fmt);
}


int RDRecording::channels() const
{
  return GetIntValue("CHANNELS");
}


void RDRecording::setChannels(int chan) const
{
  SetRow("CHANNELS",chan);
}


int RDRecording::sampleRate() const
{
  return GetIntValue("SAMPRATE");
}


void RDRecording::setSampleRate(int rate)
{
  SetRow("SAMPRATE",rate);
}


int RDRecording::bitrate() const
{
  return GetIntValue("BITRATE");
}


int RDRecording::quality() const
{
  return GetIntValue("QUALITY");
}


void RDRecording::setQuality(int qual) const
{
  SetRow("QUALITY",qual);
}


void RDRecording::setBitrate(int rate) const
{
  SetRow("BITRATE",rate);
}


int RDRecording::normalizationLevel() const
{
  return GetIntValue("NORMALIZE_LEVEL");
}


void RDRecording::setNormalizationLevel(int level) const
{
  SetRow("NORMALIZE_LEVEL",level);
}


int RDRecording::macroCart() const
{
  return GetIntValue("MACRO_CART");
}


void RDRecording::setMacroCart(int cart) const
{
  SetRow("MACRO_CART",cart);
}


bool RDRecording::oneShot() const
{
  return GetBoolValue("ONE_SHOT");
}


void RDRecording::setOneShot(bool state) const
{
  SetRow("ONE_SHOT",state);
}


RDRecording::StartType RDRecording::startType() const
{
  return (RDRecording::StartType)GetUIntValue("START_TYPE");
}


void RDRecording::setStartType(StartType type) const
{
  SetRow("START_TYPE",(unsigned)type);
}


RDRecording::EndType RDRecording::endType() const
{
  return (RDRecording::EndType)GetUIntValue("END_TYPE");
}


void RDRecording::setEndType(EndType type) const
{
  SetRow("END_TYPE",(unsigned)type);
}


int RDRecording::startMatrix() const
{
  return GetIntValue("START_MATRIX");
}


void RDRecording::setStartMatrix(int matrix) const
{
  SetRow("START_MATRIX",matrix);
}


int RDRecording::startLine() const
{
  return GetIntValue("START_LINE");
}


void RDRecording::setStartLine(int line) const
{
  SetRow("START_LINE",line);
}


int RDRecording::endMatrix() const
{
  return GetIntValue("END_MATRIX");
}


void RDRecording::setEndMatrix(int matrix) const
{
  SetRow("END_MATRIX",matrix);
}


int RDRecording::endLine() const
{
  return GetIntValue("END_LINE");
}


void RDRecording::setEndLine(int line) const
{
  SetRow("END_LINE",line);
}


int RDRecording::startLength() const
{
  return GetIntValue("START_LENGTH");
}


void RDRecording::setStartLength(int len) const
{
  SetRow("START_LENGTH",len);
}


int RDRecording::endLength() const
{
  return GetIntValue("END_LENGTH");
}


void RDRecording::setEndLength(int len) const
{
  SetRow("END_LENGTH",len);
}


int RDRecording::startOffset() const
{
  return GetIntValue("START_OFFSET");
}


void RDRecording::setStartOffset(int offset) const
{
  SetRow("START_OFFSET",offset);
}


QString RDRecording::url() const
{
  return GetStringValue("URL");
}


void RDRecording::setUrl(QString url) const
{
  SetRow("URL",url);
}


QString RDRecording::urlUsername() const
{
  return GetStringValue("URL_USERNAME");
}


void RDRecording::setUrlUsername(QString name) const
{
  SetRow("URL_USERNAME",name);
}


QString RDRecording::urlPassword() const
{
  return GetStringValue("URL_PASSWORD");
}


void RDRecording::setUrlPassword(QString passwd) const
{
  SetRow("URL_PASSWORD",passwd);
}


bool RDRecording::enableMetadata() const
{
  return GetBoolValue("ENABLE_METADATA");
}


void RDRecording::setEnableMetadata(bool state) const
{
  SetRow("ENABLE_METADATA",state);
}


int RDRecording::feedId() const
{
  return GetIntValue("FEED_ID");
}


void RDRecording::setFeedId(int id) const
{
  SetRow("FEED_ID",id);
}


void RDRecording::setFeedId(const QString &keyname) const
{
  QString sql=QString().sprintf("select ID from FEEDS where KEY_NAME=\"%s\"",
				(const char *)keyname);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    setFeedId(q->value(0).toInt());
  }
  else {
    setFeedId(-1);
  }
  delete q;
}


QString RDRecording::feedKeyName() const
{
  QString ret;
  QString sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%d",
				feedId());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    QString ret=q->value(0).toString();
  }
  delete q;
  return ret;
}


QString RDRecording::typeString(RDRecording::Type type)
{
  QString str;

  switch(type) {
      case RDRecording::Recording:
	str=QObject::tr("Recording");
	break;

      case RDRecording::MacroEvent:
	str=QObject::tr("Macro Event");
	break;

      case RDRecording::SwitchEvent:
	str=QObject::tr("Switch Event");
	break;

      case RDRecording::Playout:
	str=QObject::tr("Playout");
	break;

      case RDRecording::Download:
	str=QObject::tr("Download");
	break;

      case RDRecording::Upload:
	str=QObject::tr("Upload");
	break;
  }

  return str;
}


QString RDRecording::exitString(RDRecording::ExitCode code)
{
  QString ret;

  switch(code) {
      case RDRecording::Ok:
	ret=QObject::tr("Ok");
	break;

      case RDRecording::Short:
	ret=QObject::tr("Short Length");
	break;

      case RDRecording::LowLevel:
	ret=QObject::tr("Low Level");
	break;

      case RDRecording::HighLevel:
	ret=QObject::tr("High Level");
	break;

      case RDRecording::Downloading:
	ret=QObject::tr("Downloading");
	break;

      case RDRecording::Uploading:
	ret=QObject::tr("Uploading");
	break;

      case RDRecording::ServerError:
	ret=QObject::tr("Server Error");
	break;

      case RDRecording::InternalError:
	ret=QObject::tr("Internal Error");
	break;

      case RDRecording::Interrupted:
	ret=QObject::tr("Interrupted");
	break;

      case RDRecording::RecordActive:
	ret=QObject::tr("Recording");
	break;

      case RDRecording::PlayActive:
	ret=QObject::tr("Playing");
	break;

      case RDRecording::Waiting:
	ret=QObject::tr("Waiting");
	break;

      case RDRecording::DeviceBusy:
	ret=QObject::tr("Device Busy");
	break;

      case RDRecording::NoCut:
	ret=QObject::tr("No Such Cart/Cut");
	break;

      case RDRecording::UnknownFormat:
	ret=QObject::tr("Unknown Audio Format");
	break;
  }
  return ret;
}


int RDRecording::GetIntValue(QString field) const
{
  QString sql;
  RDSqlQuery *q;
  int accum;
  
  sql=QString().sprintf("select %s from RECORDINGS where ID=%d",
			(const char *)field,rec_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


unsigned RDRecording::GetUIntValue(QString field) const
{
  QString sql;
  RDSqlQuery *q;
  int accum;
  
  sql=QString().sprintf("select %s from RECORDINGS where ID=%d",
			(const char *)field,rec_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toUInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


bool RDRecording::GetBoolValue(QString field) const
{
  QString sql;
  RDSqlQuery *q;
  bool state;

  sql=QString().sprintf("select %s from RECORDINGS where ID=%d",
			(const char *)field,rec_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    state=RDBool(q->value(0).toString());
    delete q;
    return state;
  }
  delete q;
  return false;    
}


QString RDRecording::GetStringValue(QString field) const
{
  QString sql;
  RDSqlQuery *q;
  QString accum;

  sql=QString().sprintf("select %s from RECORDINGS where ID=%d",
			(const char *)field,rec_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toString();
    delete q;
    return accum; 
  }
  delete q;
  return QString();    
}


QTime RDRecording::GetTimeValue(QString field) const
{
  QString sql;
  RDSqlQuery *q;
  QTime accum;

  sql=QString().sprintf("select %s from RECORDINGS where ID=%d",
			(const char *)field,rec_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toTime();
    delete q;
    return accum; 
  }
  delete q;
  return QTime();    
}


int RDRecording::AddRecord() const
{
  QString sql;
  RDSqlQuery *q;
  int n;

  sql=QString("select ID from RECORDINGS order by ID desc limit 1");
  q=new RDSqlQuery(sql);
  if(q->first()) {
    n=q->value(0).toInt()+1;
  }
  else {
    n=1;
  }
  delete q;
  sql=QString().sprintf("insert into RECORDINGS set ID=%d",n);
  q=new RDSqlQuery(sql);
  delete q;
  return n;
}


void RDRecording::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update RECORDINGS set %s=%d where ID=%d",
			(const char *)param,value,rec_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDRecording::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update RECORDINGS set %s=%u where ID=%d",
			(const char *)param,value,rec_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDRecording::SetRow(const QString &param,bool value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update RECORDINGS set %s=\'%s\' where ID=%d",
			(const char *)param,(const char *)RDYesNo(value),
			rec_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDRecording::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update RECORDINGS set %s=\"%s\" where ID=%d",
			(const char *)param,
			(const char *)RDEscapeString(value),
			rec_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDRecording::SetRow(const QString &param,const QTime &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update RECORDINGS set %s=\"%s\" where ID=%d",
			(const char *)param,
			(const char *)value.toString("hh:mm:ss"),rec_id);
  q=new RDSqlQuery(sql);
  delete q;
}
