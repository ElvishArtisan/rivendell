// rdlogedit_conf.cpp
//
// Abstract an RDLogedit Configuration.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogedit_conf.cpp,v 1.12.8.1 2014/01/08 18:14:34 cvs Exp $
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
#include <rdlogedit_conf.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDLogeditConf::RDLogeditConf(const QString &station)
{
  QString sql;
  RDSqlQuery *q;
  
  lib_station=station;

  sql=QString().sprintf("select ID from RDLOGEDIT where STATION=\"%s\"",
			(const char *)RDEscapeString(lib_station));
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString().sprintf("insert into RDLOGEDIT set STATION=\"%s\"",
			  (const char *)RDEscapeString(lib_station));
    q=new RDSqlQuery(sql);
  }
  delete q;
}


QString RDLogeditConf::station() const
{
  return lib_station;
}


int RDLogeditConf::inputCard() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"INPUT_CARD").toInt();
}


int RDLogeditConf::inputPort() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"INPUT_PORT").toInt();
}


void RDLogeditConf::setInputCard(int input) const
{
  SetRow("INPUT_CARD",input);
}


void RDLogeditConf::setInputPort(int input) const
{
  SetRow("INPUT_PORT",input);
}


int RDLogeditConf::outputCard() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"OUTPUT_CARD").toInt();
}


int RDLogeditConf::outputPort() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"OUTPUT_PORT").toInt();
}


void RDLogeditConf::setOutputCard(int output) const
{
  SetRow("OUTPUT_CARD",output);
}


void RDLogeditConf::setOutputPort(int output) const
{
  SetRow("OUTPUT_PORT",output);
}


unsigned RDLogeditConf::format() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"FORMAT").toUInt();
}


void RDLogeditConf::setFormat(unsigned format) const
{
  SetRow("FORMAT",format);
}


unsigned RDLogeditConf::layer() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"LAYER").toUInt();
}


void RDLogeditConf::setLayer(unsigned layer) const
{
  SetRow("LAYER",layer);
}


unsigned RDLogeditConf::bitrate() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"BITRATE").toUInt();
}


void RDLogeditConf::setBitrate(unsigned rate) const
{
  SetRow("BITRATE",rate);
}


bool RDLogeditConf::enableSecondStart() const
{
  return RDBool(RDGetSqlValue("RDLOGEDIT","STATION",lib_station,
			      "ENABLE_SECOND_START").toString());
}
  

void RDLogeditConf::setEnableSecondStart(bool state) const
{
  SetRow("ENABLE_SECOND_START",state);
}


unsigned RDLogeditConf::defaultChannels() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"DEFAULT_CHANNELS").toUInt();
}


void RDLogeditConf::setDefaultChannels(unsigned chans) const
{
  SetRow("DEFAULT_CHANNELS",chans);
}


unsigned RDLogeditConf::maxLength() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"MAXLENGTH").toUInt();
}


void RDLogeditConf::setMaxLength(unsigned length) const
{
  SetRow("MAXLENGTH",length);
}


unsigned RDLogeditConf::tailPreroll() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"TAIL_PREROLL").
    toUInt();
}


void RDLogeditConf::setTailPreroll(unsigned length) const
{
  SetRow("TAIL_PREROLL",length);
}


unsigned RDLogeditConf::startCart() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"START_CART").toUInt();
}


void RDLogeditConf::setStartCart(unsigned cartnum) const
{
  SetRow("START_CART",cartnum);
}


unsigned RDLogeditConf::endCart() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"END_CART").toUInt();
}


void RDLogeditConf::setEndCart(unsigned cartnum) const
{
  SetRow("END_CART",cartnum);
}


unsigned RDLogeditConf::recStartCart() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"REC_START_CART").
    toUInt();
}


void RDLogeditConf::setRecStartCart(unsigned cartnum) const
{
  SetRow("REC_START_CART",cartnum);
}


unsigned RDLogeditConf::recEndCart() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"REC_END_CART").
    toUInt();
}


void RDLogeditConf::setRecEndCart(unsigned cartnum) const
{
  SetRow("REC_END_CART",cartnum);
}


int RDLogeditConf::trimThreshold() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"TRIM_THRESHOLD").
    toInt();
}


void RDLogeditConf::setTrimThreshold(int level)
{
  SetRow("TRIM_THRESHOLD",level);
}


int RDLogeditConf::ripperLevel() const
{
  return RDGetSqlValue("RDLOGEDIT","STATION",lib_station,"RIPPER_LEVEL").
    toInt();
}


void RDLogeditConf::setRipperLevel(int level)
{
  SetRow("RIPPER_LEVEL",level);
}


RDLogLine::TransType RDLogeditConf::defaultTransType() const
{
  return (RDLogLine::TransType)RDGetSqlValue("RDLOGEDIT","STATION",lib_station,
					    "DEFAULT_TRANS_TYPE").toInt();
}


void RDLogeditConf::setDefaultTransType(RDLogLine::TransType type)
{
  SetRow("DEFAULT_TRANS_TYPE",(int)type);
}


#ifndef WIN32
void RDLogeditConf::getSettings(RDSettings *s) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select DEFAULT_CHANNELS,SAMPRATE,\
                         FORMAT,BITRATE,RIPPER_LEVEL,\
                         TRIM_THRESHOLD from RDLOGEDIT \
                         where STATION=\"%s\"",
			(const char *)RDEscapeString(lib_station));
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
#endif  // WIN32


void RDLogeditConf::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE RDLOGEDIT SET %s=%d WHERE STATION=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(lib_station));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLogeditConf::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE RDLOGEDIT SET %s=%d WHERE STATION=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(lib_station));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLogeditConf::SetRow(const QString &param,bool value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update RDLOGEDIT set %s=\"%s\" where STATION=\"%s\"",
			(const char *)param,
			(const char *)RDYesNo(value),
			(const char *)RDEscapeString(lib_station));
  q=new RDSqlQuery(sql);
  delete q;
}
