// rdreplicator.h
//
// Abstract a Rivendell replicator configuration
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdreplicator.h,v 1.3 2010/08/03 17:52:18 cvs Exp $
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

#ifndef RDREPLICATOR_H
#define RDREPLICATOR_H

#include <qvariant.h>

#include <rdsettings.h>

class RDReplicator
{
 public:
  enum Type {TypeCitadelXds=0,TypeLast=1};
  RDReplicator(const QString &name);
  QString name() const;
  RDReplicator::Type type() const;
  void setType(RDReplicator::Type type) const;
  QString stationName() const;
  void setStationName(const QString &str);
  QString description() const;
  void setDescription(const QString &str) const;
  RDSettings::Format format() const;
  void setFormat(RDSettings::Format fmt) const;
  unsigned channels() const;
  void setChannels(unsigned chans) const;
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate) const;
  unsigned bitRate() const;
  void setBitRate(unsigned rate) const;
  unsigned quality() const;
  void setQuality(unsigned qual) const;
  QString url() const;
  void setUrl(const QString &str);
  QString urlUsername() const;
  void setUrlUsername(const QString &str) const;
  QString urlPassword() const;
  void setUrlPassword(const QString &str) const;
  bool enableMetadate() const;
  void setEnableMetadata(bool state) const;
  int normalizeLevel() const;
  void setNormalizeLevel(int lvl) const;
  QString typeString() const;
  static QString typeString(RDReplicator::Type type);

 private:
  QVariant GetValue(const QString &field) const;
  void SetRow(const QString &param,QString value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  QString replicator_name;
};


#endif  // RDREPLICATOR_H
