// rdsystem.h
//
// System-wide Rivendell settings
//
//   (C) Copyright 2009-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSYSTEM_H
#define RDSYSTEM_H

#include <qhostaddress.h>
#include <qvariant.h>

class RDSystem
{
 public:
  RDSystem();
  QString realmName() const;
  void setRealmName(const QString &str) const;
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate) const;
  bool allowDuplicateCartTitles() const;
  void setAllowDuplicateCartTitles(bool state) const;
  bool fixDuplicateCartTitles() const;
  void setFixDuplicateCartTitles(bool state) const;
  QString isciXreferencePath() const;
  void setIsciXreferencePath(const QString &str) const;
  QString originEmailAddress() const;
  void setOriginEmailAddress(const QString &str) const;
  QString tempCartGroup() const;
  void setTempCartGroup(const QString &str) const;
  bool showUserList() const;
  void setShowUserList(bool state) const;
  QHostAddress notificationAddress() const;
  void setNotificationAddress(const QHostAddress &addr);
  QString rssProcessorStation() const;
  void setRssProcessorStation(const QString &str=QString()) const;
  QString longDateFormat() const;
  void setLongDateFormat(const QString &str);
  QString shortDateFormat() const;
  void setShortDateFormat(const QString &str);
  bool showTwelveHourTime() const;
  void setShowTwelveHourTime(bool state) const;
  QString xml() const;

 private:
  QVariant GetValue(const QString &field) const;
  void SetRow(const QString &param,QString value) const;
  void SetRow(const QString &param,int value) const;
};


#endif  // RDSYSTEM_H
