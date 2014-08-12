// rddeck.cpp
//
// Abstract a Rivendell Deck.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddeck.cpp,v 1.17 2010/07/29 19:32:33 cvs Exp $
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
#include <rddeck.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDDeck::RDDeck(QString station,unsigned channel,bool create)
{
  RDSqlQuery *q;
  QString sql;

  deck_station=station;
  deck_channel=channel;

  if(create) {
    sql=QString().sprintf("select ID from DECKS where \
(STATION_NAME=\"%s\")&&(CHANNEL=%d)",(const char *)deck_station,deck_channel);
    q=new RDSqlQuery(sql);
    if(q->size()!=1) {
      delete q;
      sql=QString().
        sprintf("INSERT INTO DECKS SET STATION_NAME=\"%s\",CHANNEL=%d",
		(const char *)deck_station,deck_channel);
      q=new RDSqlQuery(sql);
      delete q;
    }
    else {
      delete q;
    }
  }
}


bool RDDeck::isActive() const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString().sprintf("select ID from DECKS where (STATION_NAME=\"%s\")&&\
                         (CHANNEL=%u)&&(CARD_NUMBER>=0)&&(PORT_NUMBER>=0)",
			(const char *)RDEscapeString(deck_station),
			deck_channel);
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;

  return ret;
}


QString RDDeck::station() const
{
  return deck_station;
}


int RDDeck::channel() const
{
  return deck_channel;
}


int RDDeck::cardNumber() const
{
  return GetIntValue("CARD_NUMBER");
}


void RDDeck::setCardNumber(int card) const
{
  SetRow("CARD_NUMBER",card);
}


int RDDeck::streamNumber() const
{
  return GetIntValue("STREAM_NUMBER");
}


void RDDeck::setStreamNumber(int stream) const
{
  SetRow("STREAM_NUMBER",stream);
}


int RDDeck::portNumber() const
{
  return GetIntValue("PORT_NUMBER");
}


void RDDeck::setPortNumber(int port) const
{
  SetRow("PORT_NUMBER",port);
}


int RDDeck::monitorPortNumber() const
{
  return GetIntValue("MON_PORT_NUMBER");
}


void RDDeck::setMonitorPortNumber(int port) const
{
  SetRow("MON_PORT_NUMBER",port);
}


bool RDDeck::defaultMonitorOn() const
{
  return RDBool(GetStringValue("DEFAULT_MONITOR_ON"));
}


void RDDeck::setDefaultMonitorOn(bool state) const
{
  SetRow("DEFAULT_MONITOR_ON",state);
}


RDSettings::Format RDDeck::defaultFormat() const
{
  return (RDSettings::Format)GetIntValue("DEFAULT_FORMAT");
}


void RDDeck::setDefaultFormat(RDSettings::Format format) const
{
  SetRow("DEFAULT_FORMAT",(int)format);
}


int RDDeck::defaultChannels() const
{
  return GetIntValue("DEFAULT_CHANNELS");
}


void RDDeck::setDefaultChannels(int chan) const
{
  SetRow("DEFAULT_CHANNELS",chan);
}


int RDDeck::defaultBitrate() const
{
  return GetIntValue("DEFAULT_BITRATE");
}


void RDDeck::setDefaultBitrate(int rate) const
{
  SetRow("DEFAULT_BITRATE",rate);
}


int RDDeck::defaultThreshold() const
{
  return GetIntValue("DEFAULT_THRESHOLD");
}


void RDDeck::setDefaultThreshold(int level) const
{
  SetRow("DEFAULT_THRESHOLD",level);
}


QString RDDeck::switchStation() const
{
  return GetStringValue("SWITCH_STATION");
}


void RDDeck::setSwitchStation(QString str) const
{
  SetRow("SWITCH_STATION",str);
}


int RDDeck::switchMatrix() const
{
  return GetIntValue("SWITCH_MATRIX");
}


QString RDDeck::switchMatrixName() const
{
  QString matrix_name;

  RDSqlQuery *q=new RDSqlQuery(QString().sprintf("select NAME from MATRICES \
                             where (STATION_NAME=\"%s\")&&(MATRIX=%d)",
					       (const char *)switchStation(),
					       switchMatrix()));
  if(q->first()) {
    matrix_name=q->value(0).toString();
  }
  delete q;
  return matrix_name;
}


void RDDeck::setSwitchMatrix(int matrix) const
{
  SetRow("SWITCH_MATRIX",matrix);
}


int RDDeck::switchOutput() const
{
  return GetIntValue("SWITCH_OUTPUT");
}


QString RDDeck::switchOutputName() const
{
  QString output_name;

  RDSqlQuery *q=new RDSqlQuery(QString().sprintf("select NAME from OUTPUTS \
                             where (STATION_NAME=\"%s\")&&(MATRIX=%d)&&\
                                   (NUMBER=%d)",
					       (const char *)switchStation(),
					       switchMatrix(),
					       switchOutput()));
  if(q->first()) {
    output_name=q->value(0).toString();
  }
  delete q;
  return output_name;
}


void RDDeck::setSwitchOutput(int output) const
{
  SetRow("SWITCH_OUTPUT",output);
}


int RDDeck::switchDelay() const
{
  return GetIntValue("SWITCH_DELAY");
}


void RDDeck::setSwitchDelay(int delay) const
{
  SetRow("SWITCH_DELAY",delay);
}


int RDDeck::GetIntValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  int accum;
  
  sql=QString().sprintf("select %s from DECKS where \
(STATION_NAME=\"%s\")&&(CHANNEL=%d)",(const char *)field,
			(const char *)deck_station,deck_channel);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


QString RDDeck::GetStringValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  QString accum;
  
  sql=QString().sprintf("select %s from DECKS where \
(STATION_NAME=\"%s\")&&(CHANNEL=%d)",(const char *)field,
			(const char *)deck_station,deck_channel);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toString();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


void RDDeck::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE DECKS SET %s=%d \
WHERE (STATION_NAME=\"%s\")&&(CHANNEL=%d)",
			(const char *)param,
			value,
			(const char *)deck_station,
			deck_channel);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDeck::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE DECKS SET %s=\"%s\" \
WHERE (STATION_NAME=\"%s\")&&(CHANNEL=%d)",
			(const char *)param,
			(const char *)RDEscapeString(value),
			(const char *)deck_station,
			deck_channel);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDeck::SetRow(const QString &param,bool value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE DECKS SET %s=\"%s\" \
WHERE (STATION_NAME=\"%s\")&&(CHANNEL=%d)",
			(const char *)param,
			(const char *)RDYesNo(value),
			(const char *)deck_station,
			deck_channel);
  q=new RDSqlQuery(sql);
  delete q;
}
