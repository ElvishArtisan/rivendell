// validate_cut.cpp
//
// Validate a Rivendell Audio Cut
//
//   (C) Copyright 2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <validate_cut.h>
#include <rddb.h> 

QString ValidateCutFields()
{
  QString sql;

  sql=QString("select ")+
    "PLAY_ORDER,"+          // 00
    "WEIGHT,"+              // 01   
    "DESCRIPTION,"+         // 02
    "LENGTH,"+              // 03
    "LAST_PLAY_DATETIME,"+  // 04
    "PLAY_COUNTER,"+        // 05
    "ORIGIN_DATETIME,"+     // 06
    "ORIGIN_NAME,"+         // 07
    "OUTCUE,"+              // 08
    "CUT_NAME,"+            // 09
    "LENGTH,"+              // 10
    "EVERGREEN,"+           // 11
    "START_DATETIME,"+      // 12
    "END_DATETIME,"+        // 13
    "START_DAYPART,"+       // 14
    "END_DAYPART,"+         // 15
    "MON,"+                 // 16
    "TUE,"+                 // 17
    "WED,"+                 // 18
    "THU,"+                 // 19
    "FRI,"+                 // 20
    "SAT,"+                 // 21
    "SUN,"+                 // 22
    "SHA1_HASH "+           // 23
    "from CUTS";

  return sql;
}


RDCart::Validity ValidateCut(RDSqlQuery *q,unsigned offset,
			     RDCart::Validity prev_validity,
			     const QDateTime &datetime)
{
  if(prev_validity==RDCart::AlwaysValid) {
    return RDCart::AlwaysValid;
  }
  if(q->value(offset).toInt()==0) {                   // Length
    return prev_validity;
  }
  if(q->value(offset+1).toString()=="Y") {            // Evergreen
    return RDCart::EvergreenValid;
  }
  if(q->value(offset+5+datetime.date().dayOfWeek()).toString()!="Y") {
    return prev_validity;
  }
  if(!q->value(offset+2).isNull()) {                  // Start DateTime
    if(q->value(offset+2).toDateTime()>datetime) {
      return RDCart::FutureValid;
    }
  }
  if(!q->value(offset+3).isNull()) {                  // End DateTime
    if(q->value(offset+3).toDateTime()<datetime) {
      return prev_validity;
    }
  }
  if(!q->value(offset+4).isNull()) {                  // Start Daypart
    if(q->value(offset+4).toTime()>datetime.time()) {
      return prev_validity;
    }
  }
  if(!q->value(offset+5).isNull()) {                  // End Daypart
    if(q->value(offset+5).toTime()<datetime.time()) {
      return prev_validity;
    }
  }

  return RDCart::AlwaysValid;
}
