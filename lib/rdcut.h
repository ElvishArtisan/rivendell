// rdcut.h
//
// Abstract a Rivendell Cut
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcut.h,v 1.44.6.2.2.1 2014/05/22 14:30:45 cvs Exp $
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
#include <qsignal.h>
#include <qobject.h>

#include <rdconfig.h>
#include <rdwavedata.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rduser.h>

#ifndef RDCUT_H
#define RDCUT_H

class RDCut
{
 public:
  enum AudioEnd {AudioBoth=0,AudioHead=1,AudioTail=2};
  enum IsrcFormat {RawIsrc=0,FormattedIsrc=1};
  enum Validity {NeverValid=0,ConditionallyValid=1,AlwaysValid=2,FutureValid=3};
  RDCut(const QString &name,bool create=false,QSqlDatabase *db=0);
  RDCut(unsigned cartnum,int cutnum,bool create=false,QSqlDatabase *db=0);
  ~RDCut();
  bool exists() const;
  bool isValid() const;
  bool isValid(const QTime &time) const;
  bool isValid(const QDateTime &datetime) const;
  QString cutName() const;
  unsigned cutNumber() const;
  unsigned cartNumber() const;
  void setCartNumber(unsigned num) const;
  bool evergreen() const;
  void setEvergreen(bool state) const;
  QString description() const;
  void setDescription(const QString &string) const;
  QString outcue() const;
  void setOutcue(const QString &string) const;
  QString isrc(IsrcFormat fmt=RawIsrc) const;
  void setIsrc(const QString &isrc) const;
  QString isci() const;
  void setIsci(const QString &isci) const;
  unsigned length() const;
  void setLength(int length) const;
  QDateTime originDatetime(bool *valid) const;
  void setOriginDatetime(const QDateTime &datetime) const;
  QDateTime startDatetime(bool *valid) const;
  void setStartDatetime(const QDateTime &datetime,bool valid) const;
  QDateTime endDatetime(bool *valid) const;
  void setEndDatetime(const QDateTime &datetime,bool valid) const;
  QTime startDaypart(bool *valid) const;
  void setStartDaypart(const QTime &time,bool valid) const;
  QTime endDaypart(bool *valid) const;
  void setEndDaypart(const QTime &time,bool valid) const;
  bool weekPart(int dayofweek) const;
  void setWeekPart(int dayofweek,bool state) const;
  QString originName() const;
  void setOriginName(const QString &name) const;
  unsigned weight() const;
  void setWeight(int value) const;
  QDateTime lastPlayDatetime(bool *valid) const;
  void setLastPlayDatetime(const QDateTime &datetime,bool valid) const;
  QDateTime uploadDatetime(bool *valid) const;
  void setUploadDatetime(const QDateTime &datetime,bool valid) const;
  unsigned playCounter() const;
  void setPlayCounter(unsigned count) const;
  RDCut::Validity validity() const;
  void setValidity(RDCut::Validity state);
  unsigned localCounter() const;
  void setLocalCounter(unsigned count) const;
  unsigned codingFormat() const;
  void setCodingFormat(unsigned format) const;
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate) const;
  unsigned bitRate() const;
  void setBitRate(unsigned rate) const;
  unsigned channels() const;
  void setChannels(unsigned chan) const;
  int playGain() const;
  void setPlayGain(int gain) const;
  int startPoint(bool calc=false) const;
  void setStartPoint(int point) const;
  int endPoint(bool calc=false) const;
  void setEndPoint(int point) const;
  int fadeupPoint(bool calc=false) const;
  void setFadeupPoint(int point) const;
  int fadedownPoint(bool calc=false) const;
  void setFadedownPoint(int point) const;
  int segueStartPoint(bool calc=false) const;
  void setSegueStartPoint(int point) const;
  int segueEndPoint(bool calc=false) const;
  void setSegueEndPoint(int point) const;
  int segueGain() const;
  void setSegueGain(int gain) const;
  int hookStartPoint(bool calc=false) const;
  void setHookStartPoint(int point) const;
  int hookEndPoint(bool calc=false) const;
  void setHookEndPoint(int point) const;
  int talkStartPoint(bool calc=false) const;
  void setTalkStartPoint(int point) const;
  int talkEndPoint(bool calc=false) const;
  void setTalkEndPoint(int point) const;
  int effectiveStart() const;
  int effectiveEnd() const;
  void logPlayout() const;
  bool copyTo(RDStation *station,RDUser *user,const QString &cutname,
	      RDConfig *config) const;
  void getMetadata(RDWaveData *data) const;
  void setMetadata(RDWaveData *data) const;
  QString xml() const;
  bool checkInRecording(const QString &stationname,RDSettings *settings,
			unsigned msecs) const;
  void autoTrim(RDCut::AudioEnd end,int level);
  void autoSegue(int level,int length);
  void reset() const;
  void connect(QObject *receiver,const char *member) const;
  void disconnect(QObject *receiver,const char *member) const;
  static QString cutName(unsigned cartnum,unsigned cutnum);
  static unsigned cartNumber(const QString &cutname);
  static unsigned cutNumber(const QString &cutname);
  static bool exists(unsigned cartnum,unsigned cutnum);
  static bool exists(const QString &cutname);
  static QString pathName(unsigned cartnum,unsigned cutnum);
  static QString pathName(const QString &cutname);

 private:
  bool FileCopy(const QString &srcfile,const QString &destfile) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QDateTime &value) const;
  void SetRow(const QString &param,const QDate &value) const;
  void SetRow(const QString &param,const QTime &value) const;
  void SetRow(const QString &param) const;
  QSignal *cut_signal;
  QSqlDatabase *cut_db;
  QString cut_name;
  unsigned cart_number;
  unsigned cut_number;
};


#endif 
