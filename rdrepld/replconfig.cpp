// replconfig.cpp
//
// A container class for a Rivendell replication configuration
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: replconfig.cpp,v 1.2 2010/07/29 19:32:37 cvs Exp $
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

#include <rd.h>

#include <replconfig.h>

ReplConfig::ReplConfig()
{
  clear();
}


RDReplicator::Type ReplConfig::type() const
{
  return repl_type;
}


void ReplConfig::setType(RDReplicator::Type type)
{
  repl_type=type;
}


QString ReplConfig::name() const
{
  return repl_name;
}


void ReplConfig::setName(const QString &str)
{
  repl_name=str;
}


QString ReplConfig::stationName() const
{
  return repl_station_name;
}


void ReplConfig::setStationName(const QString &str)
{
  repl_station_name=str;
}


QString ReplConfig::description() const
{
  return repl_description;
}


void ReplConfig::setDescription(const QString &str)
{
  repl_description=str;
}


RDSettings::Format ReplConfig::format() const
{
  return repl_format;
}


void ReplConfig::setFormat(RDSettings::Format fmt)
{
  repl_format=fmt;
}


unsigned ReplConfig::channels() const
{
  return repl_channels;
}


void ReplConfig::setChannels(unsigned chans)
{
  repl_channels=chans;
}


unsigned ReplConfig::sampleRate() const
{
  return repl_sample_rate;
}


void ReplConfig::setSampleRate(unsigned rate)
{
  repl_sample_rate=rate;
}


unsigned ReplConfig::bitRate() const
{
  return repl_bit_rate;
}


void ReplConfig::setBitRate(unsigned rate)
{
  repl_bit_rate=rate;
}


unsigned ReplConfig::quality() const
{
  return repl_quality;
}


void ReplConfig::setQuality(unsigned qual)
{
  repl_quality=qual;
}


QString ReplConfig::url() const
{
  return repl_url;
}


void ReplConfig::setUrl(const QString &str)
{
  repl_url=str;
}


QString ReplConfig::urlUsername() const
{
  return repl_url_username;
}


void ReplConfig::setUrlUsername(const QString &str)
{
  repl_url_username=str;
}


QString ReplConfig::urlPassword() const
{
  return repl_url_password;
}


void ReplConfig::setUrlPassword(const QString &str)
{
  repl_url_password=str;
}


bool ReplConfig::enableMetadata() const
{
  return repl_enable_metadata;
}


void ReplConfig::setEnableMetadata(bool state)
{
  repl_enable_metadata=state;
}


int ReplConfig::normalizeLevel() const
{
  return repl_normalize_level;
}


void ReplConfig::setNormalizeLevel(int lvl)
{
  repl_normalize_level=lvl;
}


void ReplConfig::clear()
{
  repl_name="";
  repl_station_name="";
  repl_description="";
  repl_format=RDSettings::Pcm16;
  repl_channels=2;
  repl_sample_rate=RD_DEFAULT_SAMPLE_RATE;
  repl_bit_rate=0;
  repl_quality=0;
  repl_url="";
  repl_url_username="";
  repl_url_password="";
  repl_enable_metadata=false;
  repl_normalize_level=0;
}
