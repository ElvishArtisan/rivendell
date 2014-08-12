// catch_connect.h
//
// A container class for a Rivendell netcatch event.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: catch_event.h,v 1.22 2010/07/29 19:32:36 cvs Exp $
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

#ifndef CATCH_CONNECT_H
#define CATCH_CONNECT_H

#include <qtimer.h>

#include <rdrecording.h>
#include <rddeck.h>
#include <rdcae.h>

class CatchEvent
{
 public:
  CatchEvent();
  unsigned id() const;
  void setId(int id);
  bool isActive() const;
  void setIsActive(bool state);
  RDRecording::Type type() const;
  void setType(RDRecording::Type type);
  unsigned channel() const;
  void setChannel(unsigned chan);
  QString cutName() const;
  void setCutName(const QString &str);
  QString tempName() const;
  void setTempName(const QString &str);
  bool deleteTempFile() const;
  void setDeleteTempFile(bool state);
  bool dayOfWeek(int day) const;
  void setDayOfWeek(int day,bool state);
  RDRecording::StartType startType() const;
  void setStartType(RDRecording::StartType type);
  QTime startTime() const;
  void setStartTime(QTime time);
  int startLength() const;
  void setStartLength(int len);
  int startMatrix() const;
  void setStartMatrix(int matrix);
  int startLine() const;
  void setStartLine(int line);
  int startOffset() const;
  void setStartOffset(int offset);
  RDRecording::EndType endType() const;
  void setEndType(RDRecording::EndType type);
  QTime endTime() const;
  void setEndTime(QTime time);
  int endLength() const;
  void setEndLength(int len);
  int endMatrix() const;
  void setEndMatrix(int matrix);
  int endLine() const;
  void setEndLine(int line);
  unsigned length() const;
  void setLength(unsigned len);
  int startGpi() const;
  void setStartGpi(int gpi);
  int endGpi() const;
  void setEndGpi(int gpi);
  bool allowMultipleRecordings() const;
  void setAllowMultipleRecordings(bool state);
  int maxGpiRecordLength() const;
  void setMaxGpiRecordLength(int len);
  unsigned trimThreshold() const;
  void setTrimThreshold(unsigned level);
  unsigned startdateOffset() const;
  void setStartdateOffset(unsigned offset);
  unsigned enddateOffset() const;
  void setEnddateOffset(unsigned offset);
  RDCae::AudioCoding format() const;
  void setFormat(RDCae::AudioCoding fmt);
  int channels() const;
  void setChannels(int chans);
  int sampleRate() const;
  void setSampleRate(int rate);
  int bitrate() const;
  void setBitrate(int rate);
  int quality() const;
  void setQuality(int qual);
  int normalizeLevel() const;
  void setNormalizeLevel(int level);
  int macroCart() const;
  void setMacroCart(int cart);
  int switchInput() const;
  void setSwitchInput(int input);
  int switchOutput() const;
  void setSwitchOutput(int output);
  RDDeck::Status status() const;
  void setStatus(RDDeck::Status status);
  bool oneShot() const;
  void setOneShot(bool state);
  QString url()const;
  void setUrl(const QString &url);
  QString resolvedUrl() const;
  void setResolvedUrl(const QString &url);
  QString urlUsername() const;
  void setUrlUsername(const QString &name);
  QString urlPassword() const;
  void setUrlPassword(const QString &passwd);
  bool enableMetadata() const;
  void setEnableMetadata(bool state);
  unsigned tempLength() const;
  void setTempLength(unsigned len);
  unsigned finalLength() const;
  void setFinalLength(unsigned len);
  QTimer *gpiStartTimer() const;
  void setGpiStartTimer(QTimer *timer);
  QTimer *gpiOffsetTimer() const;
  void setGpiOffsetTimer(QTimer *timer);
  QString description() const;
  void setDescription(const QString &desc);
  int feedId() const;
  void setFeedId(int id);
  int podcastLength() const;
  void setPodcastLength(int bytes);
  int podcastTime() const;
  void setPodcastTime(int msecs);
  int eventdateOffset() const;
  void setEventdateOffset(int days);
  void resolveUrl(int time_offset);
  void clear();

 private:
  unsigned catch_id;
  bool catch_is_active;
  RDRecording::Type catch_type;
  unsigned catch_channel;
  QString catch_cut_name;
  QString catch_temp_name;
  bool catch_delete_temp_file;
  bool catch_day_of_week[7];
  RDRecording::StartType catch_start_type;
  QTime catch_start_time;
  int catch_start_length;
  int catch_start_matrix;
  int catch_start_line;
  int catch_start_offset;
  RDRecording::EndType catch_end_type;
  QTime catch_end_time;
  int catch_end_length;
  int catch_end_matrix;
  int catch_end_line;
  unsigned catch_length;
  int catch_start_gpi;
  int catch_end_gpi;
  unsigned catch_trim_threshold;
  unsigned catch_startdate_offset;
  unsigned catch_enddate_offset;
  RDCae::AudioCoding catch_format;
  int catch_channels;
  int catch_samplerate;
  int catch_bitrate;
  int catch_quality;
  int catch_normalize_level;
  int catch_macro_cart;
  int catch_switch_input;
  int catch_switch_output;
  bool catch_oneshot;
  RDDeck::Status catch_status;
  QString catch_url;
  QString catch_resolved_url;
  QString catch_url_username;
  QString catch_url_password;
  bool catch_enable_metadata;
  unsigned catch_temp_length;
  unsigned catch_final_length;
  QTimer *catch_gpi_start_timer;
  QTimer *catch_gpi_offset_timer;
  bool catch_allow_multiple_recordings;
  int catch_max_gpi_record_length;
  QString catch_description;
  int catch_feed_id;
  int catch_podcast_length;
  int catch_podcast_time;
  int catch_eventdate_offset;
};


#endif  // CATCH_CONNECT_H
