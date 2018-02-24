// rdlibrary_conf.cpp
//
// Abstract an RDLibrary Configuration.
//
//   (C) Copyright 2002-2003,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdconf.h>
#include <rdlibrary_conf.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDLibraryConf::RDLibraryConf(const QString &station)
{
  RDSqlQuery *q;
  QString sql;

  lib_station=station;

  sql=QString("select ID from RDLIBRARY where ")+
    "STATION=\""+RDEscapeString(lib_station)+"\"";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString("insert into RDLIBRARY set ")+
      "STATION=\""+RDEscapeString(lib_station)+"\"";
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString("select LAST_INSERT_ID() from RDLIBRARY");
    q=new RDSqlQuery(sql);
    q->first();
  }
  lib_id=q->value(0).toUInt();
  delete q;
}


QString RDLibraryConf::station() const
{
  return lib_station;
}


int RDLibraryConf::inputCard() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"INPUT_CARD").toInt();
}


int RDLibraryConf::inputPort() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"INPUT_PORT").toInt();
}


void RDLibraryConf::setInputCard(int input) const
{
  SetRow("INPUT_CARD",input);
}


void RDLibraryConf::setInputPort(int input) const
{
  SetRow("INPUT_PORT",input);
}


int RDLibraryConf::outputCard() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"OUTPUT_CARD").toInt();
}


int RDLibraryConf::outputPort() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"OUTPUT_PORT").toInt();
}


void RDLibraryConf::setOutputCard(int output) const
{
  SetRow("OUTPUT_CARD",output);
}


void RDLibraryConf::setOutputPort(int output) const
{
  SetRow("OUTPUT_PORT",output);
}


int RDLibraryConf::voxThreshold() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"VOX_THRESHOLD").toInt();
}


void RDLibraryConf::setVoxThreshold(int level) const
{
  SetRow("VOX_THRESHOLD",level);
}


int RDLibraryConf::trimThreshold() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"TRIM_THRESHOLD").toInt();
}


void RDLibraryConf::setTrimThreshold(int level) const
{
  SetRow("TRIM_THRESHOLD",level);
}


unsigned RDLibraryConf::defaultFormat() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"DEFAULT_FORMAT").toUInt();
}


void RDLibraryConf::setDefaultFormat(unsigned format) const
{
  SetRow("DEFAULT_FORMAT",format);
}


unsigned RDLibraryConf::defaultChannels() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"DEFAULT_CHANNELS").toUInt();
}


void RDLibraryConf::setDefaultChannels(unsigned chans) const
{
  SetRow("DEFAULT_CHANNELS",chans);
}


unsigned RDLibraryConf::defaultLayer() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"DEFAULT_LAYER").toUInt();
}


void RDLibraryConf::setDefaultLayer(unsigned layer) const
{
  SetRow("DEFAULT_LAYER",layer);
}


unsigned RDLibraryConf::defaultBitrate() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"DEFAULT_BITRATE").toUInt();
}


void RDLibraryConf::setDefaultBitrate(unsigned rate) const
{
  SetRow("DEFAULT_BITRATE",rate);
}


RDLibraryConf::RecordMode RDLibraryConf::defaultRecordMode() const
{
  return (RDLibraryConf::RecordMode)
    RDGetSqlValue("RDLIBRARY","ID",lib_id,"DEFAULT_RECORD_MODE").toUInt();
}


void RDLibraryConf::setDefaultRecordMode(RecordMode mode) const
{
  SetRow("DEFAULT_RECORD_MODE",(unsigned)mode);
}


bool RDLibraryConf::defaultTrimState() const
{
  return RDBool(RDGetSqlValue("RDLIBRARY","ID",lib_id,"DEFAULT_TRIM_STATE").
	       toString());
}


void RDLibraryConf::setDefaultTrimState(bool state) const
{
  SetRow("DEFAULT_TRIM_STATE",state);
}


unsigned RDLibraryConf::maxLength() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"MAXLENGTH").toUInt();
}


void RDLibraryConf::setMaxLength(unsigned length) const
{
  SetRow("MAXLENGTH",length);
}


unsigned RDLibraryConf::tailPreroll() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"TAIL_PREROLL").toUInt();
}


void RDLibraryConf::setTailPreroll(unsigned length) const
{
  SetRow("TAIL_PREROLL",length);
}


QString RDLibraryConf::ripperDevice() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"RIPPER_DEVICE").toString();
}


void RDLibraryConf::setRipperDevice(QString dev) const
{
  SetRow("RIPPER_DEVICE",dev);
}


int RDLibraryConf::paranoiaLevel() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"PARANOIA_LEVEL").toInt();
}


void RDLibraryConf::setParanoiaLevel(int level) const
{
  SetRow("PARANOIA_LEVEL",level);
}


int RDLibraryConf::ripperLevel() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"RIPPER_LEVEL").toInt();
}
 

void RDLibraryConf::setRipperLevel(int level) const
{
  SetRow("RIPPER_LEVEL",level);
}


QString RDLibraryConf::cddbServer() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"CDDB_SERVER").toString();
}


void RDLibraryConf::setCddbServer(QString server) const
{
  SetRow("CDDB_SERVER",server);
}


bool RDLibraryConf::readIsrc() const
{
  return RDBool(RDGetSqlValue("RDLIBRARY","ID",lib_id,"READ_ISRC").
		toString());
}


void RDLibraryConf::setReadIsrc(bool state) const
{
  SetRow("READ_ISRC",RDYesNo(state));
}


bool RDLibraryConf::enableEditor() const
{
  return RDBool(RDGetSqlValue("RDLIBRARY","ID",lib_id,"ENABLE_EDITOR").
		toString());
}


void RDLibraryConf::setEnableEditor(bool state) const
{
  SetRow("ENABLE_EDITOR",RDYesNo(state));
}


int RDLibraryConf::srcConverter() const
{
  return RDGetSqlValue("RDLIBRARY","ID",lib_id,"SRC_CONVERTER").toInt();
}


void RDLibraryConf::setSrcConverter(int conv) const
{
  SetRow("SRC_CONVERTER",conv);
}


RDLibraryConf::SearchLimit RDLibraryConf::limitSearch() const
{
  return (RDLibraryConf::SearchLimit)
    RDGetSqlValue("RDLIBRARY","ID",lib_id,"LIMIT_SEARCH").toInt();
}


void RDLibraryConf::setLimitSearch(RDLibraryConf::SearchLimit lmt) const
{
  SetRow("LIMIT_SEARCH",(int)lmt);
}


bool RDLibraryConf::searchLimited() const
{
  return RDBool(RDGetSqlValue("RDLIBRARY","ID",lib_id,"SEARCH_LIMITED").
		toString());
}


void RDLibraryConf::setSearchLimited(bool state) const
{
  SetRow("SEARCH_LIMITED",RDYesNo(state));
}


void RDLibraryConf::getSettings(RDSettings *s) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "DEFAULT_CHANNELS,"+  // 00
    "DEFAULT_SAMPRATE,"+  // 01
    "DEFAULT_FORMAT,"+    // 02
    "DEFAULT_BITRATE,"+   // 03
    "RIPPER_LEVEL,"+      // 04
    "TRIM_THRESHOLD "+    // 05
    "from RDLIBRARY where "+
    "STATION=\""+RDEscapeString(lib_station)+"\"";
  q=new RDSqlQuery(sql);
  s->clear();
  if(q->first()) {
    s->setChannels(q->value(0).toUInt());
    s->setSampleRate(q->value(1).toUInt());
    switch(q->value(2).toInt()) {
	case 0:
	  s->setFormat(RDSettings::Pcm16);
	  break;

	case 1:
	  s->setFormat(RDSettings::MpegL2);
	  break;
    }
    s->setBitRate(q->value(3).toUInt());
    s->setNormalizationLevel(q->value(4).toUInt());
    s->setAutotrimLevel(q->value(5).toUInt());
  }
  delete q;
}


void RDLibraryConf::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update RDLIBRARY set ")+
    param+QString().sprintf("=%d  where ",value)+
    "STATION=\""+RDEscapeString(lib_station)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLibraryConf::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update RDLIBRARY set ")+
    param+QString().sprintf("=%u where ",value)+
    "STATION=\""+RDEscapeString(lib_station)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLibraryConf::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update RDLIBRARY set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    "STATION=\""+RDEscapeString(lib_station)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLibraryConf::SetRow(const QString &param,bool value) const
{
  SetRow(param,RDYesNo(value));
}
