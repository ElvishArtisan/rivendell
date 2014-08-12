// rdrecording.h
//
// Abstract a Rivendell RDCatch Event
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdrecording.h,v 1.29 2011/06/21 18:31:33 cvs Exp $
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

#include <qsqldatabase.h>
#include <qdatetime.h>

#include <rdsettings.h>

#ifndef RDRECORDING_H
#define RDRECORDING_H

class RDRecording
{
 public:
  enum StartType {HardStart=0,GpiStart=1};
  enum EndType {HardEnd=0,GpiEnd=1,LengthEnd=2};
  enum Type {Recording=0,MacroEvent=1,SwitchEvent=2,Playout=3,
	     Download=4,Upload=5};
  enum ExitCode {Ok=0,Short=1,LowLevel=2,HighLevel=3,
		 Downloading=4,Uploading=5,ServerError=6,InternalError=7,
		 Interrupted=8,RecordActive=9,PlayActive=10,Waiting=11,
		 DeviceBusy=12,NoCut=13,UnknownFormat=14};
  RDRecording(int id,bool create=false);
  int id() const;
  bool isActive() const;
  void setIsActive(bool state) const;
  QString station() const;
  void setStation(QString name) const;
  RDRecording::Type type() const;
  void setType(RDRecording::Type type) const;
  unsigned channel() const;
  void setChannel(unsigned chan) const;
  QTime startTime() const;
  void setStartTime(QTime time) const;
  QTime endTime() const;
  void setEndTime(QTime time) const;
  QString cutName() const;
  void setCutName(QString name) const;
  bool sun() const;
  void setSun(bool state) const;
  bool mon() const;
  void setMon(bool state) const;
  bool tue() const;
  void setTue(bool state) const;
  bool wed() const;
  void setWed(bool state) const;
  bool thu() const;
  void setThu(bool state) const;
  bool fri() const;
  void setFri(bool state) const;
  bool sat() const;
  void setSat(bool state) const;
  int switchSource() const;
  void setSwitchSource(int input) const;
  int switchDestination() const;
  void setSwitchDestination(int output) const;
  QString description() const;
  void setDescription(QString string) const;
  unsigned length() const;
  void setLength(unsigned length) const;
  int startGpi() const;
  void setStartGpi(int line) const;
  int endGpi() const;
  void setEndGpi(int line) const;
  bool allowMultipleRecordings() const;
  void setAllowMultipleRecordings(bool state) const;
  unsigned maxGpiRecordingLength() const;
  void setMaxGpiRecordingLength(unsigned len) const;
  unsigned trimThreshold() const;
  void setTrimThreshold(unsigned level) const;
  unsigned startdateOffset() const;
  void setStartdateOffset(unsigned offset) const;
  unsigned enddateOffset() const;
  void setEnddateOffset(unsigned offset) const;
  int eventdateOffset() const;
  void setEventdateOffset(int offset) const;
  RDSettings::Format format() const;
  void setFormat(RDSettings::Format fmt) const;
  int channels() const;
  void setChannels(int chan) const;
  int sampleRate() const;
  void setSampleRate(int rate);
  int bitrate() const;
  void setBitrate(int rate) const;
  int quality() const;
  void setQuality(int qual) const;
  int normalizationLevel() const;
  void setNormalizationLevel(int level) const;
  int macroCart() const;
  void setMacroCart(int cart) const;
  bool oneShot() const;
  void setOneShot(bool state) const;
  StartType startType() const;
  void setStartType(StartType type) const;
  EndType endType() const;
  void setEndType(EndType type) const;
  int startMatrix() const;
  void setStartMatrix(int matrix) const;
  int startLine() const;
  void setStartLine(int line) const;
  int endMatrix() const;
  void setEndMatrix(int matrix) const;
  int endLine() const;
  void setEndLine(int line) const;
  int startLength() const;
  void setStartLength(int len) const;
  int endLength() const;
  void setEndLength(int len) const;
  int startOffset() const;
  void setStartOffset(int offset) const;
  QString url() const;
  void setUrl(QString url) const;
  QString urlUsername() const;
  void setUrlUsername(QString name) const;
  QString urlPassword() const;
  void setUrlPassword(QString passwd) const;
  bool enableMetadata() const;
  void setEnableMetadata(bool state) const;
  int feedId() const;
  void setFeedId(int id) const;
  void setFeedId(const QString &keyname) const;
  QString feedKeyName() const;
  static QString typeString(RDRecording::Type type);
  static QString exitString(RDRecording::ExitCode code);
  
 private:
  int GetIntValue(QString field) const;
  unsigned GetUIntValue(QString field) const;
  bool GetBoolValue(QString field) const;
  QString GetStringValue(QString field) const;
  QTime GetTimeValue(QString field) const;
  int AddRecord() const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,bool value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,const QTime &value) const;
  int rec_id;
};


#endif 
