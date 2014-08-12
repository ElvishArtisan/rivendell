// rdencoderlist.cpp
//
// Abstract a Rivendell Custom Encoder
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdencoderlist.cpp,v 1.1 2008/09/18 19:02:07 fredg Exp $
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

#include <rdescape_string.h>
#include <rddb.h>
#include <rdencoderlist.h>

RDEncoderList::RDEncoderList(const QString &stationname)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select ID,NAME,DEFAULT_EXTENSION,COMMAND_LINE \
                        from ENCODERS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(stationname));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    list_encoders.push_back(new RDEncoder());
    list_encoders.back()->setId(q->value(0).toInt());
    list_encoders.back()->setName(q->value(1).toString());
    list_encoders.back()->setDefaultExtension(q->value(2).toString());
    list_encoders.back()->setCommandLine(q->value(3).toString());
    sql=QString().sprintf("select CHANNELS from ENCODER_CHANNELS \
                           where ENCODER_ID=%d order by CHANNELS",
			  q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      list_encoders.back()->addAllowedChannel(q1->value(0).toInt());
    }
    delete q1;
    sql=QString().sprintf("select SAMPLERATES from ENCODER_SAMPLERATES \
                           where ENCODER_ID=%d order by SAMPLERATES",
			  q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      list_encoders.back()->addAllowedSamplerate(q1->value(0).toInt());
    }
    delete q1;
    sql=QString().sprintf("select BITRATES from ENCODER_BITRATES \
                           where ENCODER_ID=%d order by BITRATES",
			  q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      list_encoders.back()->addAllowedBitrate(q1->value(0).toInt());
    }
    delete q1;
  }
  delete q;
}


RDEncoderList::~RDEncoderList()
{
  for(unsigned i=0;i<list_encoders.size();i++) {
    delete list_encoders[i];
  }
  list_encoders.clear();
}


unsigned RDEncoderList::encoderQuantity() const
{
  return list_encoders.size();
}


RDEncoder *RDEncoderList::encoder(unsigned n)
{
  return list_encoders[n];
}
