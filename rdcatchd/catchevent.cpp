// catch_event.cpp
//
// A container class for a Rivendell netcatch event.
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rddatedecode.h>

#include "catchevent.h"

CatchEvent::CatchEvent()
{
  clear();
}


unsigned CatchEvent::id() const
{
  return d_id;
}


void CatchEvent::setId(int id)
{
  d_id=id;
}


bool CatchEvent::isActive() const
{
  return d_is_active;
}


void CatchEvent::setIsActive(bool state)
{
  d_is_active=state;
}


RDRecording::Type CatchEvent::type() const
{
  return d_type;
}


void CatchEvent::setType(RDRecording::Type type)
{
  d_type=type;
}


unsigned CatchEvent::channel() const
{
  return d_channel;
}


void CatchEvent::setChannel(unsigned chan)
{
  d_channel=chan;
}


QString CatchEvent::cutName() const
{
  return d_cut_name;
}


void CatchEvent::setCutName(const QString &str)
{
  d_cut_name=str;
}


QString CatchEvent::tempName() const
{
  return d_temp_name;
}


void CatchEvent::setTempName(const QString &str)
{
  d_temp_name=str;
}


bool CatchEvent::deleteTempFile() const
{
  return d_delete_temp_file;
}


void CatchEvent::setDeleteTempFile(bool state)
{
  d_delete_temp_file=state;
}


bool CatchEvent::dayOfWeek(int day) const
{
  return d_day_of_week[day-1];
}


void CatchEvent::setDayOfWeek(int day,bool state)
{
  d_day_of_week[day-1]=state;
}


RDRecording::StartType CatchEvent::startType() const
{
  return d_start_type;
}


void CatchEvent::setStartType(RDRecording::StartType type)
{
  d_start_type=type;
}


QTime CatchEvent::startTime() const
{
  return d_start_time;
}


void CatchEvent::setStartTime(QTime time)
{
  d_start_time=time;
}


int CatchEvent::startLength() const
{
  return d_start_length;
}


void CatchEvent::setStartLength(int len)
{
  d_start_length=len;
}


int CatchEvent::startMatrix() const
{
  return d_start_matrix;
}


void CatchEvent::setStartMatrix(int matrix)
{
  d_start_matrix=matrix;
}


int CatchEvent::startLine() const
{
  return d_start_line;
}


void CatchEvent::setStartLine(int line)
{
  d_start_line=line;
}


int CatchEvent::startOffset() const
{
  return d_start_offset;
}


void CatchEvent::setStartOffset(int offset)
{
  d_start_offset=offset;
}


RDRecording::EndType CatchEvent::endType() const
{
  return d_end_type;
}


void CatchEvent::setEndType(RDRecording::EndType type)
{
  d_end_type=type;
}


QTime CatchEvent::endTime() const
{
  return d_end_time;
}


void CatchEvent::setEndTime(QTime time)
{
  d_end_time=time;
}


int CatchEvent::endLength() const
{
  return d_end_length;
}


void CatchEvent::setEndLength(int len)
{
  d_end_length=len;
}


int CatchEvent::endMatrix() const
{
  return d_end_matrix;
}


void CatchEvent::setEndMatrix(int matrix)
{
  d_end_matrix=matrix;
}


int CatchEvent::endLine() const
{
  return d_end_line;
}


void CatchEvent::setEndLine(int line)
{
  d_end_line=line;
}


unsigned CatchEvent::length() const
{
  return d_length;
}


void CatchEvent::setLength(unsigned len)
{
  d_length=len;
}


int CatchEvent::startGpi() const
{
  return d_start_gpi;
}


void CatchEvent::setStartGpi(int gpi)
{
  d_start_gpi=gpi;
}


int CatchEvent::endGpi() const
{
  return d_end_gpi;
}


void CatchEvent::setEndGpi(int gpi)
{
  d_end_gpi=gpi;
}


bool CatchEvent::allowMultipleRecordings() const
{
  return d_allow_multiple_recordings;
}


void CatchEvent::setAllowMultipleRecordings(bool state)
{
  d_allow_multiple_recordings=state;
}


int CatchEvent::maxGpiRecordLength() const
{
  return d_max_gpi_record_length;
}


void CatchEvent::setMaxGpiRecordLength(int len)
{
  d_max_gpi_record_length=len;
}


unsigned CatchEvent::trimThreshold() const
{
  return d_trim_threshold;
}


void CatchEvent::setTrimThreshold(unsigned level)
{
  d_trim_threshold=level;
}


unsigned CatchEvent::startdateOffset() const
{
  return d_startdate_offset;
}


void CatchEvent::setStartdateOffset(unsigned offset)
{
  d_startdate_offset=offset;
}


unsigned CatchEvent::enddateOffset() const
{
  return d_enddate_offset;
}


void CatchEvent::setEnddateOffset(unsigned offset)
{
  d_enddate_offset=offset;
}


RDCae::AudioCoding CatchEvent::format() const
{
  return d_format;
}


void CatchEvent::setFormat(RDCae::AudioCoding fmt)
{
  d_format=fmt;
}


int CatchEvent::channels() const
{
  return d_channels;
}


void CatchEvent::setChannels(int chans)
{
  d_channels=chans;
}


int CatchEvent::sampleRate() const
{
  return d_samplerate;
}


void CatchEvent::setSampleRate(int rate)
{
  d_samplerate=rate;
}


int CatchEvent::bitrate() const
{
  return d_bitrate;
}


void CatchEvent::setBitrate(int rate)
{
  d_bitrate=rate;
}


int CatchEvent::normalizeLevel() const
{
  return d_normalize_level;
}


void CatchEvent::setNormalizeLevel(int level)
{
  d_normalize_level=level;
}


int CatchEvent::quality() const
{
  return d_quality;
}


void CatchEvent::setQuality(int qual)
{
  d_quality=qual;
}


int CatchEvent::macroCart() const
{
  return d_macro_cart;
}


void CatchEvent::setMacroCart(int cart)
{
  d_macro_cart=cart;
}


int CatchEvent::switchInput() const
{
  return d_switch_input;
}


void CatchEvent::setSwitchInput(int input)
{
  d_switch_input=input;
}


int CatchEvent::switchOutput() const
{
  return d_switch_output;
}


void CatchEvent::setSwitchOutput(int output)
{
  d_switch_output=output;
}


RDDeck::Status CatchEvent::status() const
{
  return d_status;
}


void CatchEvent::setStatus(RDDeck::Status status)
{
  d_status=status;
}


bool CatchEvent::oneShot() const
{
  return d_oneshot;
}


void CatchEvent::setOneShot(bool state)
{
  d_oneshot=state;
}


QString CatchEvent::url()const
{
  return d_url;
}


void CatchEvent::setUrl(const QString &url)
{
  d_url=url;
}


QString CatchEvent::resolvedUrl() const
{
  return d_resolved_url;
}


void CatchEvent::setResolvedUrl(const QString &url)
{
  d_resolved_url=url;
}


QString CatchEvent::urlUsername() const
{
  return d_url_username;
}


void CatchEvent::setUrlUsername(const QString &name)
{
  d_url_username=name;
}


QString CatchEvent::urlPassword() const
{
  return d_url_password;
}


bool CatchEvent::enableMetadata() const
{
  return d_enable_metadata;
}


void CatchEvent::setEnableMetadata(bool state)
{
  d_enable_metadata=state;
}


void CatchEvent::setUrlPassword(const QString &passwd)
{
  d_url_password=passwd;
}


unsigned CatchEvent::tempLength() const
{
  return d_temp_length;
}


void CatchEvent::setTempLength(unsigned len)
{
  d_temp_length=len;
}


unsigned CatchEvent::finalLength() const
{
  return d_final_length;
}


void CatchEvent::setFinalLength(unsigned len)
{
  d_final_length=len;
}


QTimer *CatchEvent::gpiStartTimer() const
{
  return d_gpi_start_timer;
}


void CatchEvent::setGpiStartTimer(QTimer *timer)
{
  d_gpi_start_timer=timer;
}


QTimer *CatchEvent::gpiOffsetTimer() const
{
  return d_gpi_offset_timer;
}


void CatchEvent::setGpiOffsetTimer(QTimer *timer)
{
  d_gpi_offset_timer=timer;
}


QString CatchEvent::description() const
{
  return d_description;
}


void CatchEvent::setDescription(const QString &desc)
{
  d_description=desc;
}


int CatchEvent::feedId() const
{
  return d_feed_id;
}


void CatchEvent::setFeedId(int id)
{
  d_feed_id=id;
}


int CatchEvent::podcastLength() const
{
  return d_podcast_length;
}


void CatchEvent::setPodcastLength(int bytes)
{
  d_podcast_length=bytes;
}


int CatchEvent::podcastTime() const
{
  return d_podcast_time;
}


void CatchEvent::setPodcastTime(int msecs)
{
  d_podcast_time=msecs;
}


int CatchEvent::eventdateOffset() const
{
  return d_eventdate_offset;
}


void CatchEvent::setEventdateOffset(int days)
{
  d_eventdate_offset=days;
}


int CatchEvent::exitCode() const
{
  return d_exit_code;
}


void CatchEvent::setExitCode(int code)
{
  d_exit_code=code;
}


QString CatchEvent::exitText() const
{
  return d_exit_text;
}


void CatchEvent::setExitText(const QString &str)
{
  d_exit_text=str;
}


void CatchEvent::resolveUrl(int time_offset)
{
  QDate date=QDate::currentDate();
  QTime current_time=QTime::currentTime();
  if((current_time.msecsTo(QTime(23,59,59))+1000)<time_offset) {
    date=date.addDays(1);
  }
  setResolvedUrl(RDDateTimeDecode(url(),
				  QDateTime(date.addDays(eventdateOffset()),
					    current_time),rda->station(),
				  rda->config()));
}


void CatchEvent::clear()
{
  d_id=0;
  d_is_active=false;;
  d_type=RDRecording::Recording;
  d_channel=0;
  d_cut_name="";
  d_temp_name="";
  d_delete_temp_file=false;
  for(int i=0;i<7;i++) {
    d_day_of_week[i]=false;
  }
  d_start_type=RDRecording::HardStart;
  d_start_time=QTime();
  d_start_matrix=-1;
  d_start_line=-1;
  d_start_offset=0;
  d_end_type=RDRecording::HardEnd;
  d_end_time=QTime();
  d_end_matrix=-1;
  d_end_line=-1;
  d_length=0;
  d_start_gpi=-1;
  d_end_gpi=-1;
  d_trim_threshold=0;
  d_startdate_offset=0;
  d_enddate_offset=0;
  d_format=RDCae::Pcm16;
  d_channels=0;
  d_samplerate=0;
  d_bitrate=0;
  d_quality=0;
  d_normalize_level=-1;
  d_macro_cart=-1;
  d_switch_input=-1;
  d_switch_output=-1;
  d_status=RDDeck::Idle;
  d_oneshot=false;
  d_url="";
  d_resolved_url="";
  d_url_username="";
  d_url_password="";
  d_enable_metadata=false;
  d_temp_length=0;
  d_final_length=0;
  d_gpi_start_timer=NULL;
  d_gpi_offset_timer=NULL;
  d_allow_multiple_recordings=false;
  d_max_gpi_record_length=0;
  d_description="";
  d_feed_id=-1;
  d_podcast_length=0;
  d_podcast_time=0;
  d_eventdate_offset=0;
}


bool CatchEvent::load(unsigned id)
{
  bool ret=false;

  QString sql=sqlFields()+
    QString::asprintf("where `ID`=%u ",id)+
    "order by `START_TIME`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    loadFromQuery(q);
    ret=true;
  }
  delete q;

  return ret;
}


void CatchEvent::save() const
{
}


void CatchEvent::loadFromQuery(RDSqlQuery *q)
{
  setId(q->value(0).toUInt());
  setIsActive(RDBool(q->value(1).toString()));
  setType((RDRecording::Type)q->value(2).toInt());
  setChannel(q->value(3).toUInt());
  setCutName(q->value(4).toString());
  setDayOfWeek(7,RDBool(q->value(5).toString()));
  setDayOfWeek(1,RDBool(q->value(6).toString()));
  setDayOfWeek(2,RDBool(q->value(7).toString()));
  setDayOfWeek(3,RDBool(q->value(8).toString()));
  setDayOfWeek(4,RDBool(q->value(9).toString()));
  setDayOfWeek(5,RDBool(q->value(10).toString()));
  setDayOfWeek(6,RDBool(q->value(11).toString()));
  setStartTime(q->value(12).toTime());
  setLength(q->value(13).toUInt());
  setStartGpi(q->value(14).toInt());
  setEndGpi(q->value(15).toInt());
  setTrimThreshold(q->value(16).toUInt());
  setStartdateOffset(q->value(17).toUInt());
  setEnddateOffset(q->value(18).toUInt());
  switch((RDSettings::Format)q->value(19).toInt()) {
  case RDSettings::Pcm16:
    setFormat(RDCae::Pcm16);
    break;

  case RDSettings::Pcm24:
    setFormat(RDCae::Pcm24);
    break;

  case RDSettings::MpegL2:
  case RDSettings::MpegL2Wav:
    setFormat(RDCae::MpegL2);
    break;

  case RDSettings::MpegL3:
    setFormat(RDCae::MpegL3);
    break;

  case RDSettings::MpegL1:
  case RDSettings::Flac:
  case RDSettings::OggVorbis:
    break;
  }
  setChannels(q->value(20).toInt());
  setSampleRate(q->value(21).toUInt());
  setBitrate(q->value(22).toInt());
  setUrl(q->value(37).toString());
  setUrlUsername(q->value(38).toString());
  setUrlPassword(q->value(39).toString());
  setQuality(q->value(40).toInt());
  setNormalizeLevel(q->value(41).toInt());
  setFeedId(q->value(45).toUInt());
  setMacroCart(q->value(23).toInt());
  setSwitchInput(q->value(24).toInt());
  setSwitchOutput(q->value(25).toInt());
  setStatus(RDDeck::Idle);
  setOneShot(RDBool(q->value(26).toString()));
  setStartType((RDRecording::StartType)q->value(26).toInt());
  setStartLength(q->value(28).toInt());
  setStartMatrix(q->value(29).toInt());
  setStartLine(q->value(30).toInt());
  setStartOffset(q->value(31).toInt());
  setEndType((RDRecording::EndType)q->value(32).toInt());
  setEndTime(q->value(33).toTime());
  setEndLength(q->value(34).toInt());
  setEndMatrix(q->value(35).toInt());
  setEndLine(q->value(36).toInt());
  setAllowMultipleRecordings(RDBool(q->value(42).toString()));
  setMaxGpiRecordLength(q->value(43).toUInt());
  setDescription(q->value(44).toString());
  setEventdateOffset(q->value(46).toInt());
  setEnableMetadata(RDBool(q->value(47).toString()));
  setExitCode(q->value(48).toInt());
  setExitText(q->value(49).toString());
}


QString CatchEvent::sqlFields()
{
  return QString("select ")+
    "`RECORDINGS`.`ID`,"+                    // 00
    "`RECORDINGS`.`IS_ACTIVE`,"+             // 01
    "`RECORDINGS`.`TYPE`,"+                  // 02
    "`RECORDINGS`.`CHANNEL`,"+               // 03
    "`RECORDINGS`.`CUT_NAME`,"+              // 04
    "`RECORDINGS`.`SUN`,"+                   // 05
    "`RECORDINGS`.`MON`,"+                   // 06
    "`RECORDINGS`.`TUE`,"+                   // 07
    "`RECORDINGS`.`WED`,"+                   // 08
    "`RECORDINGS`.`THU`,"+                   // 09
    "`RECORDINGS`.`FRI`,"+                   // 10
    "`RECORDINGS`.`SAT`,"+                   // 11
    "`RECORDINGS`.`START_TIME`,"+            // 12
    "`RECORDINGS`.`LENGTH`,"+                // 13
    "`RECORDINGS`.`START_GPI`,"+             // 14
    "`RECORDINGS`.`END_GPI`,"+               // 15
    "`RECORDINGS`.`TRIM_THRESHOLD`,"+        // 16
    "`RECORDINGS`.`STARTDATE_OFFSET`,"+      // 17
    "`RECORDINGS`.`ENDDATE_OFFSET`,"+        // 18
    "`RECORDINGS`.`FORMAT`,"                 // 19
    "`RECORDINGS`.`CHANNELS`,"+              // 20
    "`RECORDINGS`.`SAMPRATE`,"+              // 21
    "`RECORDINGS`.`BITRATE`,"+               // 22
    "`RECORDINGS`.`MACRO_CART`,"+            // 23
    "`RECORDINGS`.`SWITCH_INPUT`,"+          // 24
    "`RECORDINGS`.`SWITCH_OUTPUT`,"+         // 25
    "`RECORDINGS`.`ONE_SHOT`,"+              // 26
    "`RECORDINGS`.`START_TYPE`,"+            // 27
    "`RECORDINGS`.`START_LENGTH`,"+          // 28
    "`RECORDINGS`.`START_MATRIX`,"+          // 29
    "`RECORDINGS`.`START_LINE`,"+            // 30
    "`RECORDINGS`.`START_OFFSET`,"+          // 31
    "`RECORDINGS`.`END_TYPE`,"+              // 32
    "`RECORDINGS`.`END_TIME`,"+              // 33
    "`RECORDINGS`.`END_LENGTH`,"+            // 34
    "`RECORDINGS`.`END_MATRIX`,"+            // 35
    "`RECORDINGS`.`END_LINE`,"+              // 36
    "`RECORDINGS`.`URL`,"+                   // 37
    "`RECORDINGS`.`URL_USERNAME`,"+          // 38
    "`RECORDINGS`.`URL_PASSWORD`,"+          // 39
    "`RECORDINGS`.`QUALITY`,"+               // 40
    "`RECORDINGS`.`NORMALIZE_LEVEL`,"+       // 41
    "`RECORDINGS`.`ALLOW_MULT_RECS`,"+       // 42
    "`RECORDINGS`.`MAX_GPI_REC_LENGTH`,"+    // 43
    "`RECORDINGS`.`DESCRIPTION`,"+           // 44
    "`RECORDINGS`.`FEED_ID`,"+               // 45
    "`RECORDINGS`.`EVENTDATE_OFFSET`,"+      // 46
    "`RECORDINGS`.`ENABLE_METADATA`,"+       // 47
    "`RECORDINGS`.`EXIT_CODE`,"+             // 48
    "`RECORDINGS`.`EXIT_TEXT` "+             // 49
    "from `RECORDINGS` ";
}
