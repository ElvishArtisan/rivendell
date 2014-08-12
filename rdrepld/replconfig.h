// replconfig.h
//
// A container class for a Rivendell replication configuration
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: replconfig.h,v 1.2 2010/07/29 19:32:37 cvs Exp $
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

#ifndef REPLCONFIG_H
#define REPLCONFIG_H

#include <qtimer.h>

#include <rdsettings.h>
#include <rdreplicator.h>

class ReplConfig
{
 public:
  ReplConfig();
  RDReplicator::Type type() const;
  void setType(RDReplicator::Type type);
  QString name() const;
  void setName(const QString &str);
  QString stationName() const;
  void setStationName(const QString &str);
  QString description() const;
  void setDescription(const QString &str);
  RDSettings::Format format() const;
  void setFormat(RDSettings::Format fmt);
  unsigned channels() const;
  void setChannels(unsigned chans);
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate);
  unsigned bitRate() const;
  void setBitRate(unsigned rate);
  unsigned quality() const;
  void setQuality(unsigned qual);
  QString url() const;
  void setUrl(const QString &str);
  QString urlUsername() const;
  void setUrlUsername(const QString &str);
  QString urlPassword() const;
  void setUrlPassword(const QString &str);
  bool enableMetadata() const;
  void setEnableMetadata(bool state);
  int normalizeLevel() const;
  void setNormalizeLevel(int lvl);
  void clear();

 private:
  QString repl_name;
  RDReplicator::Type repl_type;
  QString repl_station_name;
  QString repl_description;
  RDSettings::Format repl_format;
  unsigned repl_channels;
  unsigned repl_sample_rate;
  unsigned repl_bit_rate;
  unsigned repl_quality;
  QString repl_url;
  QString repl_url_username;
  QString repl_url_password;
  bool repl_enable_metadata;
  int repl_normalize_level;
};


#endif  // REPLCONFIG_H
