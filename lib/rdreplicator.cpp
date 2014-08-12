// rdreplicator.cpp
//
// Abstract a Rivendell replicator configuration
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdreplicator.cpp,v 1.3 2010/08/03 17:52:18 cvs Exp $
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

#include <rdconf.h>
#include <rdescape_string.h>
#include <rddb.h>

#include <rdreplicator.h>

RDReplicator::RDReplicator(const QString &name)
{
  replicator_name=name;
}


QString RDReplicator::name() const
{
  return replicator_name;
}


RDReplicator::Type RDReplicator::type() const
{
  return (RDReplicator::Type)GetValue("TYPE_ID").toUInt();
}


void RDReplicator::setType(RDReplicator::Type type) const
{
  SetRow("TYPE_ID",(unsigned)type);
}


QString RDReplicator::stationName() const
{
  return GetValue("STATION_NAME").toString();
}


void RDReplicator::setStationName(const QString &str)
{
  SetRow("STATION_NAME",str);
}


QString RDReplicator::description() const
{
  return GetValue("DESCRIPTION").toString();
}


void RDReplicator::setDescription(const QString &str) const
{
  SetRow("DESCRIPTION",str);
}


RDSettings::Format RDReplicator::format() const
{
  return (RDSettings::Format)GetValue("FORMAT").toUInt();
}


void RDReplicator::setFormat(RDSettings::Format fmt) const
{
  SetRow("FORMAT",(unsigned)fmt);
}


unsigned RDReplicator::channels() const
{
  return GetValue("CHANNELS").toUInt();
}


void RDReplicator::setChannels(unsigned chans) const
{
  SetRow("CHANNELS",chans);
}


unsigned RDReplicator::sampleRate() const
{
  return GetValue("SAMPRATE").toUInt();
}


void RDReplicator::setSampleRate(unsigned rate) const
{
  SetRow("SAMPRATE",rate);
}


unsigned RDReplicator::bitRate() const
{
  return GetValue("BITRATE").toUInt();
}


void RDReplicator::setBitRate(unsigned rate) const
{
  SetRow("BITRATE",rate);
}


unsigned RDReplicator::quality() const
{
  return GetValue("QUALITY").toUInt();
}


void RDReplicator::setQuality(unsigned qual) const
{
  SetRow("QUALITY",qual);
}


QString RDReplicator::url() const
{
  return GetValue("URL").toString();
}


void RDReplicator::setUrl(const QString &str)
{
  SetRow("URL",str);
}


QString RDReplicator::urlUsername() const
{
  return GetValue("URL_USERNAME").toString();
}


void RDReplicator::setUrlUsername(const QString &str) const
{
  SetRow("URL_USERNAME",str);
}


QString RDReplicator::urlPassword() const
{
  return GetValue("URL_PASSWORD").toString();
}


void RDReplicator::setUrlPassword(const QString &str) const
{
  SetRow("URL_PASSWORD",str);
}


bool RDReplicator::enableMetadate() const
{
  return RDBool(GetValue("ENABLE_PASSWORD").toString());
}


void RDReplicator::setEnableMetadata(bool state) const
{
  SetRow("ENABLE_METADATA",RDYesNo(state));
}


QString RDReplicator::typeString() const
{
  return RDReplicator::typeString(type());
}


int RDReplicator::normalizeLevel() const
{
  return GetValue("NORMALIZATION_LEVEL").toInt();
}


void RDReplicator::setNormalizeLevel(int lvl) const
{
  SetRow("NORMALIZATION_LEVEL",lvl);
}


QString RDReplicator::typeString(RDReplicator::Type type)
{
  QString ret="Unknown type";
  switch(type) {
  case RDReplicator::TypeCitadelXds:
    ret="Citadel X-Digital Portal";
    break;

  case RDReplicator::TypeLast:
    break;
  }
  return ret;
}


QVariant RDReplicator::GetValue(const QString &field) const
{
  QVariant ret;
  QString sql=QString().sprintf("select %s from REPLICATORS where NAME=\"%s\"",
				(const char *)field,
				(const char *)RDEscapeString(replicator_name));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0);
  }
  delete q;
  return ret;
}


void RDReplicator::SetRow(const QString &param,QString value) const
{
  RDSqlQuery *q;
  QString sql;

  value.replace("\\","\\\\");  // Needed to preserve Windows pathnames
  sql=QString().sprintf("update REPLICATORS set %s=\"%s\" where NAME=\"%s\"",
			(const char *)param,
			(const char *)RDEscapeString(value),
			(const char *)RDEscapeString(replicator_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDReplicator::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update REPLICATORS set %s=%d where NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(replicator_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDReplicator::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update REPLICATORS set %s=%u where NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(replicator_name));
  q=new RDSqlQuery(sql);
  delete q;
}
