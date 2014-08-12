// catch_event.cpp
//
// A container class for a Rivendell netcatch event.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: catch_event.cpp,v 1.21 2010/07/29 19:32:36 cvs Exp $
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

#include <rddatedecode.h>

#include <catch_event.h>

CatchEvent::CatchEvent()
{
  clear();
}


unsigned CatchEvent::id() const
{
  return catch_id;
}


void CatchEvent::setId(int id)
{
  catch_id=id;
}


bool CatchEvent::isActive() const
{
  return catch_is_active;
}


void CatchEvent::setIsActive(bool state)
{
  catch_is_active=state;
}


RDRecording::Type CatchEvent::type() const
{
  return catch_type;
}


void CatchEvent::setType(RDRecording::Type type)
{
  catch_type=type;
}


unsigned CatchEvent::channel() const
{
  return catch_channel;
}


void CatchEvent::setChannel(unsigned chan)
{
  catch_channel=chan;
}


QString CatchEvent::cutName() const
{
  return catch_cut_name;
}


void CatchEvent::setCutName(const QString &str)
{
  catch_cut_name=str;
}


QString CatchEvent::tempName() const
{
  return catch_temp_name;
}


void CatchEvent::setTempName(const QString &str)
{
  catch_temp_name=str;
}


bool CatchEvent::deleteTempFile() const
{
  return catch_delete_temp_file;
}


void CatchEvent::setDeleteTempFile(bool state)
{
  catch_delete_temp_file=state;
}


bool CatchEvent::dayOfWeek(int day) const
{
  return catch_day_of_week[day-1];
}


void CatchEvent::setDayOfWeek(int day,bool state)
{
  catch_day_of_week[day-1]=state;
}


RDRecording::StartType CatchEvent::startType() const
{
  return catch_start_type;
}


void CatchEvent::setStartType(RDRecording::StartType type)
{
  catch_start_type=type;
}


QTime CatchEvent::startTime() const
{
  return catch_start_time;
}


void CatchEvent::setStartTime(QTime time)
{
  catch_start_time=time;
}


int CatchEvent::startLength() const
{
  return catch_start_length;
}


void CatchEvent::setStartLength(int len)
{
  catch_start_length=len;
}


int CatchEvent::startMatrix() const
{
  return catch_start_matrix;
}


void CatchEvent::setStartMatrix(int matrix)
{
  catch_start_matrix=matrix;
}


int CatchEvent::startLine() const
{
  return catch_start_line;
}


void CatchEvent::setStartLine(int line)
{
  catch_start_line=line;
}


int CatchEvent::startOffset() const
{
  return catch_start_offset;
}


void CatchEvent::setStartOffset(int offset)
{
  catch_start_offset=offset;
}


RDRecording::EndType CatchEvent::endType() const
{
  return catch_end_type;
}


void CatchEvent::setEndType(RDRecording::EndType type)
{
  catch_end_type=type;
}


QTime CatchEvent::endTime() const
{
  return catch_end_time;
}


void CatchEvent::setEndTime(QTime time)
{
  catch_end_time=time;
}


int CatchEvent::endLength() const
{
  return catch_end_length;
}


void CatchEvent::setEndLength(int len)
{
  catch_end_length=len;
}


int CatchEvent::endMatrix() const
{
  return catch_end_matrix;
}


void CatchEvent::setEndMatrix(int matrix)
{
  catch_end_matrix=matrix;
}


int CatchEvent::endLine() const
{
  return catch_end_line;
}


void CatchEvent::setEndLine(int line)
{
  catch_end_line=line;
}


unsigned CatchEvent::length() const
{
  return catch_length;
}


void CatchEvent::setLength(unsigned len)
{
  catch_length=len;
}


int CatchEvent::startGpi() const
{
  return catch_start_gpi;
}


void CatchEvent::setStartGpi(int gpi)
{
  catch_start_gpi=gpi;
}


int CatchEvent::endGpi() const
{
  return catch_end_gpi;
}


void CatchEvent::setEndGpi(int gpi)
{
  catch_end_gpi=gpi;
}


bool CatchEvent::allowMultipleRecordings() const
{
  return catch_allow_multiple_recordings;
}


void CatchEvent::setAllowMultipleRecordings(bool state)
{
  catch_allow_multiple_recordings=state;
}


int CatchEvent::maxGpiRecordLength() const
{
  return catch_max_gpi_record_length;
}


void CatchEvent::setMaxGpiRecordLength(int len)
{
  catch_max_gpi_record_length=len;
}


unsigned CatchEvent::trimThreshold() const
{
  return catch_trim_threshold;
}


void CatchEvent::setTrimThreshold(unsigned level)
{
  catch_trim_threshold=level;
}


unsigned CatchEvent::startdateOffset() const
{
  return catch_startdate_offset;
}


void CatchEvent::setStartdateOffset(unsigned offset)
{
  catch_startdate_offset=offset;
}


unsigned CatchEvent::enddateOffset() const
{
  return catch_enddate_offset;
}


void CatchEvent::setEnddateOffset(unsigned offset)
{
  catch_enddate_offset=offset;
}


RDCae::AudioCoding CatchEvent::format() const
{
  return catch_format;
}


void CatchEvent::setFormat(RDCae::AudioCoding fmt)
{
  catch_format=fmt;
}


int CatchEvent::channels() const
{
  return catch_channels;
}


void CatchEvent::setChannels(int chans)
{
  catch_channels=chans;
}


int CatchEvent::sampleRate() const
{
  return catch_samplerate;
}


void CatchEvent::setSampleRate(int rate)
{
  catch_samplerate=rate;
}


int CatchEvent::bitrate() const
{
  return catch_bitrate;
}


void CatchEvent::setBitrate(int rate)
{
  catch_bitrate=rate;
}


int CatchEvent::normalizeLevel() const
{
  return catch_normalize_level;
}


void CatchEvent::setNormalizeLevel(int level)
{
  catch_normalize_level=level;
}


int CatchEvent::quality() const
{
  return catch_quality;
}


void CatchEvent::setQuality(int qual)
{
  catch_quality=qual;
}


int CatchEvent::macroCart() const
{
  return catch_macro_cart;
}


void CatchEvent::setMacroCart(int cart)
{
  catch_macro_cart=cart;
}


int CatchEvent::switchInput() const
{
  return catch_switch_input;
}


void CatchEvent::setSwitchInput(int input)
{
  catch_switch_input=input;
}


int CatchEvent::switchOutput() const
{
  return catch_switch_output;
}


void CatchEvent::setSwitchOutput(int output)
{
  catch_switch_output=output;
}


RDDeck::Status CatchEvent::status() const
{
  return catch_status;
}


void CatchEvent::setStatus(RDDeck::Status status)
{
  catch_status=status;
}


bool CatchEvent::oneShot() const
{
  return catch_oneshot;
}


void CatchEvent::setOneShot(bool state)
{
  catch_oneshot=state;
}


QString CatchEvent::url()const
{
  return catch_url;
}


void CatchEvent::setUrl(const QString &url)
{
  catch_url=url;
}


QString CatchEvent::resolvedUrl() const
{
  return catch_resolved_url;
}


void CatchEvent::setResolvedUrl(const QString &url)
{
  catch_resolved_url=url;
}


QString CatchEvent::urlUsername() const
{
  return catch_url_username;
}


void CatchEvent::setUrlUsername(const QString &name)
{
  catch_url_username=name;
}


QString CatchEvent::urlPassword() const
{
  return catch_url_password;
}


bool CatchEvent::enableMetadata() const
{
  return catch_enable_metadata;
}


void CatchEvent::setEnableMetadata(bool state)
{
  catch_enable_metadata=state;
}


void CatchEvent::setUrlPassword(const QString &passwd)
{
  catch_url_password=passwd;
}


unsigned CatchEvent::tempLength() const
{
  return catch_temp_length;
}


void CatchEvent::setTempLength(unsigned len)
{
  catch_temp_length=len;
}


unsigned CatchEvent::finalLength() const
{
  return catch_final_length;
}


void CatchEvent::setFinalLength(unsigned len)
{
  catch_final_length=len;
}


QTimer *CatchEvent::gpiStartTimer() const
{
  return catch_gpi_start_timer;
}


void CatchEvent::setGpiStartTimer(QTimer *timer)
{
  catch_gpi_start_timer=timer;
}


QTimer *CatchEvent::gpiOffsetTimer() const
{
  return catch_gpi_offset_timer;
}


void CatchEvent::setGpiOffsetTimer(QTimer *timer)
{
  catch_gpi_offset_timer=timer;
}


QString CatchEvent::description() const
{
  return catch_description;
}


void CatchEvent::setDescription(const QString &desc)
{
  catch_description=desc;
}


int CatchEvent::feedId() const
{
  return catch_feed_id;
}


void CatchEvent::setFeedId(int id)
{
  catch_feed_id=id;
}


int CatchEvent::podcastLength() const
{
  return catch_podcast_length;
}


void CatchEvent::setPodcastLength(int bytes)
{
  catch_podcast_length=bytes;
}


int CatchEvent::podcastTime() const
{
  return catch_podcast_time;
}


void CatchEvent::setPodcastTime(int msecs)
{
  catch_podcast_time=msecs;
}


int CatchEvent::eventdateOffset() const
{
  return catch_eventdate_offset;
}


void CatchEvent::setEventdateOffset(int days)
{
  catch_eventdate_offset=days;
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
					    current_time)));
}


void CatchEvent::clear()
{
  catch_id=0;
  catch_is_active=false;;
  catch_type=RDRecording::Recording;
  catch_channel=0;
  catch_cut_name="";
  catch_temp_name="";
  catch_delete_temp_file=false;
  for(int i=0;i<7;i++) {
    catch_day_of_week[i]=false;
  }
  catch_start_type=RDRecording::HardStart;
  catch_start_time=QTime();
  catch_start_matrix=-1;
  catch_start_line=-1;
  catch_start_offset=0;
  catch_end_type=RDRecording::HardEnd;
  catch_end_time=QTime();
  catch_end_matrix=-1;
  catch_end_line=-1;
  catch_length=0;
  catch_start_gpi=-1;
  catch_end_gpi=-1;
  catch_trim_threshold=0;
  catch_startdate_offset=0;
  catch_enddate_offset=0;
  catch_format=RDCae::Pcm16;
  catch_channels=0;
  catch_samplerate=0;
  catch_bitrate=0;
  catch_quality=0;
  catch_normalize_level=-1;
  catch_macro_cart=-1;
  catch_switch_input=-1;
  catch_switch_output=-1;
  catch_status=RDDeck::Idle;
  catch_oneshot=false;
  catch_url="";
  catch_resolved_url="";
  catch_url_username="";
  catch_url_password="";
  catch_enable_metadata=false;
  catch_temp_length=0;
  catch_final_length=0;
  catch_gpi_start_timer=NULL;
  catch_gpi_offset_timer=NULL;
  catch_allow_multiple_recordings=false;
  catch_max_gpi_record_length=0;
  catch_description="";
  catch_feed_id=-1;
  catch_podcast_length=0;
  catch_podcast_time=0;
  catch_eventdate_offset=0;
}
