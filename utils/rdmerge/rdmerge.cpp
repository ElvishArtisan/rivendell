// rddbmgr.cpp
//
// Rivendell database merging utility
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <qapplication.h>
#include <qfileinfo.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qstringlist.h>

#include <dbversion.h>
#include <rdcmd_switch.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "rdmerge.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("rdmerge","rdmerge",RDMERGE_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdmerge: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  DumpEvents();
  DumpEventLines();
  DumpClocks();
  DumpClockLines();
  DumpServices();
  DumpServiceClocks();

  exit(0);
}


void MainObject::DumpEvents() const
{
  QString sql;
  RDSqlQuery *q=NULL;

  printf("--\n");
  printf("-- Dumping data for table 'EVENTS'\n");
  printf("--\n");

  sql=QString("select ")+
    "NAME,"+                // 00
    "DISPLAY_TEXT,"+        // 01
    "NOTE_TEXT,"+           // 02
    "PREPOSITION,"+         // 03
    "TIME_TYPE,"+           // 04
    "GRACE_TIME,"+          // 05
    "USE_AUTOFILL,"+        // 06
    "AUTOFILL_SLOP,"+       // 07
    "USE_TIMESCALE,"+       // 08
    "IMPORT_SOURCE,"+       // 09
    "START_SLOP,"+          // 10
    "END_SLOP,"+            // 11
    "FIRST_TRANS_TYPE,"+    // 12
    "DEFAULT_TRANS_TYPE,"+  // 13
    "COLOR,"+               // 14
    "SCHED_GROUP,"+         // 15
    "ARTIST_SEP,"+          // 16
    "TITLE_SEP,"+           // 17
    "HAVE_CODE,"+           // 18
    "HAVE_CODE2,"+          // 19
    "HOR_SEP,"+             // 20
    "HOR_DIST,"+            // 21
    "NESTED_EVENT,"+        // 22
    "REMARKS "+             // 23
    "from EVENTS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into EVENTS set ")+
      "NAME='"+RDEscapeString(q->value(0).toString())+"',"+
      "DISPLAY_TEXT='"+RDEscapeString(q->value(1).toString())+"',"+
      "NOTE_TEXT='"+RDEscapeString(q->value(2).toString())+"',"+
      QString().sprintf("PREPOSITION=%d,",q->value(3).toInt())+
      QString().sprintf("TIME_TYPE=%d,",q->value(4).toInt())+
      QString().sprintf("GRACE_TIME=%d,",q->value(5).toInt())+
      "USE_AUTOFILL='"+RDEscapeString(q->value(6).toString())+"',"+
      QString().sprintf("AUTOFILL_SLOP=%d,",q->value(7).toInt())+
      "USE_TIMESCALE='"+RDEscapeString(q->value(8).toString())+"',"+
      QString().sprintf("IMPORT_SOURCE=%d,",q->value(9).toInt())+
      QString().sprintf("START_SLOP=%d,",q->value(10).toInt())+
      QString().sprintf("END_SLOP=%d,",q->value(11).toInt())+
      QString().sprintf("FIRST_TRANS_TYPE=%d,",q->value(12).toInt())+
      QString().sprintf("DEFAULT_TRANS_TYPE=%d,",q->value(13).toInt())+
      "COLOR='"+RDEscapeString(q->value(14).toString())+"',"+
      "SCHED_GROUP='"+RDEscapeString(q->value(15).toString())+"',"+
      QString().sprintf("ARTIST_SEP=%u,",q->value(16).toUInt())+
      QString().sprintf("TITLE_SEP=%u,",q->value(17).toUInt())+
      "HAVE_CODE='"+RDEscapeString(q->value(18).toString())+"',"+
      "HAVE_CODE2='"+RDEscapeString(q->value(19).toString())+"',"+
      QString().sprintf("HOR_SEP=%u,",q->value(20).toUInt())+
      QString().sprintf("HOR_DIST=%u,",q->value(21).toUInt())+
      "NESTED_EVENT='"+RDEscapeString(q->value(22).toString())+"',"+
      "REMARKS='"+RDEscapeString(q->value(23).toString())+"'";
    printf("%s;\n",sql.toUtf8().constData());
  }
  delete q;

  printf("\n");
}


void MainObject::DumpEventLines() const
{
  QString sql;
  RDSqlQuery *q=NULL;

  printf("--\n");
  printf("-- Dumping data for table 'EVENT_LINES'\n");
  printf("--\n");

  sql=QString("select ")+
    "EVENT_NAME,"+     // 00
    "TYPE,"+           // 01
    "COUNT,"+          // 02
    "EVENT_TYPE,"+     // 03
    "CART_NUMBER,"+    // 04
    "TRANS_TYPE,"+     // 05
    "MARKER_COMMENT "+ // 06
    "from EVENT_LINES";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into EVENT_LINES set ")+
      "EVENT_NAME='"+RDEscapeString(q->value(0).toString())+"',"+
      QString().sprintf("TYPE=%d,",q->value(1).toInt())+
      QString().sprintf("COUNT=%d,",q->value(2).toInt())+
      QString().sprintf("EVENT_TYPE=%d,",q->value(3).toInt())+
      QString().sprintf("CART_NUMBER=%u,",q->value(4).toUInt())+
      QString().sprintf("TRANS_TYPE=%d,",q->value(5).toInt())+
      "MARKER_COMMENT='"+RDEscapeString(q->value(6).toString())+"'";
    printf("%s;\n",sql.toUtf8().constData());
  }
  delete q;

  printf("\n");
}


void MainObject::DumpClocks() const
{
  QString sql;
  RDSqlQuery *q=NULL;

  printf("--\n");
  printf("-- Dumping data for table 'CLOCKS'\n");
  printf("--\n");

  sql=QString("select ")+
    "NAME,"+        // 00
    "SHORT_NAME,"+  // 01
    "ARTISTSEP,"+   // 02
    "COLOR,"+       // 03
    "REMARKS "+     // 04
    "from CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into CLOCKS set ")+
      "NAME='"+RDEscapeString(q->value(0).toString())+"',"+
      "SHORT_NAME='"+RDEscapeString(q->value(1).toString())+"',"+
      QString().sprintf("ARTISTSEP=%u,",q->value(2).toUInt())+
      "COLOR='"+RDEscapeString(q->value(3).toString())+"',"+
      "REMARKS='"+RDEscapeString(q->value(4).toString())+"'";
    printf("%s;\n",sql.toUtf8().constData());
  }
  delete q;

  printf("\n");
}


void MainObject::DumpClockLines() const
{
  QString sql;
  RDSqlQuery *q=NULL;

  printf("--\n");
  printf("-- Dumping data for table 'CLOCK_LINES'\n");
  printf("--\n");

  sql=QString("select ")+
    "CLOCK_NAME,"+  // 00
    "EVENT_NAME,"+  // 01
    "START_TIME,"+  // 02
    "LENGTH "+      // 03
    "from CLOCK_LINES";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into CLOCK_LINES set ")+
      "CLOCK_NAME='"+RDEscapeString(q->value(0).toString())+"',"+
      "EVENT_NAME='"+RDEscapeString(q->value(1).toString())+"',"+
      QString().sprintf("START_TIME=%d,",q->value(2).toInt())+
      QString().sprintf("LENGTH=%d",q->value(3).toInt());
    printf("%s;\n",sql.toUtf8().constData());
  }
  delete q;

  printf("\n");
}


void MainObject::DumpServices() const
{
  QString sql;
  RDSqlQuery *q=NULL;

  printf("--\n");
  printf("-- Dumping data for table 'SERVICES'\n");
  printf("--\n");

  sql=QString("select ")+
    "NAME,"+                    // 00
    "DESCRIPTION,"+             // 01
    "NAME_TEMPLATE,"+           // 02
    "DESCRIPTION_TEMPLATE,"+    // 03
    "PROGRAM_CODE,"+            // 04
    "CHAIN_LOG,"+               // 05
    "SUB_EVENT_INHERITANCE,"+   // 06
    "TRACK_GROUP,"+             // 07
    "AUTOSPOT_GROUP,"+          // 08
    "AUTO_REFRESH,"+            // 09
    "DEFAULT_LOG_SHELFLIFE,"+   // 10
    "LOG_SHELFLIFE_ORIGIN,"+    // 11
    "ELR_SHELFLIFE,"+           // 12
    "INCLUDE_IMPORT_MARKERS,"+  // 13
    "TFC_PATH,"+                // 14
    "TFC_PREIMPORT_CMD,"+       // 15
    "TFC_IMPORT_TEMPLATE,"+     // 16
    "TFC_LABEL_CART,"+          // 17
    "TFC_TRACK_CART,"+          // 18
    "TFC_BREAK_STRING,"+        // 19
    "TFC_TRACK_STRING,"+        // 20
    "TFC_CART_OFFSET,"+         // 21
    "TFC_CART_LENGTH,"          // 22
    "TFC_TITLE_OFFSET,"+        // 23
    "TFC_TITLE_LENGTH,"+        // 24
    "TFC_HOURS_OFFSET,"+        // 25
    "TFC_HOURS_LENGTH,"+        // 26
    "TFC_MINUTES_OFFSET,"+      // 27
    "TFC_MINUTES_LENGTH,"+      // 28
    "TFC_SECONDS_OFFSET,"+      // 29
    "TFC_SECONDS_LENGTH,"+      // 30
    "TFC_LEN_HOURS_OFFSET,"+    // 31
    "TFC_LEN_HOURS_LENGTH,"+    // 32
    "TFC_LEN_MINUTES_OFFSET,"+  // 33
    "TFC_LEN_MINUTES_LENGTH,"+  // 34
    "TFC_LEN_SECONDS_OFFSET,"+  // 35
    "TFC_LEN_SECONDS_LENGTH,"+  // 36
    "TFC_DATA_OFFSET,"+         // 37
    "TFC_DATA_LENGTH,"+         // 38
    "TFC_EVENT_ID_OFFSET,"+     // 39
    "TFC_EVENT_ID_LENGTH,"+     // 40
    "TFC_ANNC_TYPE_OFFSET,"+    // 41
    "TFC_ANNC_TYPE_LENGTH,"+    // 42
    "MUS_PATH,"+                // 43
    "MUS_PREIMPORT_CMD,"+       // 44
    "MUS_IMPORT_TEMPLATE,"+     // 45
    "MUS_LABEL_CART,"+          // 46
    "MUS_TRACK_CART,"+          // 47
    "MUS_BREAK_STRING,"+        // 48
    "MUS_TRACK_STRING,"+        // 49
    "MUS_CART_OFFSET,"+         // 50
    "MUS_CART_LENGTH,"          // 51
    "MUS_TITLE_OFFSET,"+        // 52
    "MUS_TITLE_LENGTH,"+        // 53
    "MUS_HOURS_OFFSET,"+        // 54
    "MUS_HOURS_LENGTH,"+        // 55
    "MUS_MINUTES_OFFSET,"+      // 56
    "MUS_MINUTES_LENGTH,"+      // 57
    "MUS_SECONDS_OFFSET,"+      // 58
    "MUS_SECONDS_LENGTH,"+      // 59
    "MUS_LEN_HOURS_OFFSET,"+    // 60
    "MUS_LEN_HOURS_LENGTH,"+    // 61
    "MUS_LEN_MINUTES_OFFSET,"+  // 62
    "MUS_LEN_MINUTES_LENGTH,"+  // 63
    "MUS_LEN_SECONDS_OFFSET,"+  // 64
    "MUS_LEN_SECONDS_LENGTH,"+  // 65
    "MUS_DATA_OFFSET,"+         // 66
    "MUS_DATA_LENGTH,"+         // 67
    "MUS_EVENT_ID_OFFSET,"+     // 68
    "MUS_EVENT_ID_LENGTH,"+     // 69
    "MUS_ANNC_TYPE_OFFSET,"+    // 70
    "MUS_ANNC_TYPE_LENGTH "+    // 71
    "from SERVICES";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into SERVICES set ")+
      "NAME='"+RDEscapeString(q->value(0).toString())+"',"+
      "DESCRIPTION='"+RDEscapeString(q->value(1).toString())+"',"+
      "NAME_TEMPLATE='"+RDEscapeString(q->value(2).toString())+"',"+
      "DESCRIPTION_TEMPLATE='"+RDEscapeString(q->value(3).toString())+"',"+
      "PROGRAM_CODE='"+RDEscapeString(q->value(4).toString())+"',"+
      "CHAIN_LOG='"+RDEscapeString(q->value(5).toString())+"',"+
      QString().sprintf("SUB_EVENT_INHERITANCE=%d,",q->value(6).toInt())+
      "TRACK_GROUP='"+RDEscapeString(q->value(7).toString())+"',"+
      "AUTOSPOT_GROUP='"+RDEscapeString(q->value(8).toString())+"',"+
      "AUTO_REFRESH='"+RDEscapeString(q->value(9).toString())+"',"+
      QString().sprintf("DEFAULT_LOG_SHELFLIFE=%d,",q->value(10).toInt())+
      QString().sprintf("LOG_SHELFLIFE_ORIGIN=%d,",q->value(11).toInt())+
      QString().sprintf("ELR_SHELFLIFE=%d,",q->value(12).toInt())+
      "INCLUDE_IMPORT_MARKERS='"+RDEscapeString(q->value(13).toString())+"',"+
      "TFC_PATH='"+RDEscapeString(q->value(14).toString())+"',"+
      "TFC_PREIMPORT_CMD='"+RDEscapeString(q->value(15).toString())+"',"+
      "TFC_IMPORT_TEMPLATE='"+RDEscapeString(q->value(16).toString())+"',"+
      "TFC_LABEL_CART='"+RDEscapeString(q->value(17).toString())+"',"+
      "TFC_TRACK_CART='"+RDEscapeString(q->value(18).toString())+"',"+
      "TFC_BREAK_STRING='"+RDEscapeString(q->value(19).toString())+"',"+
      "TFC_TRACK_STRING='"+RDEscapeString(q->value(20).toString())+"',"+
      QString().sprintf("TFC_CART_OFFSET=%d,",q->value(21).toInt())+
      QString().sprintf("TFC_CART_LENGTH=%d,",q->value(22).toInt())+
      QString().sprintf("TFC_TITLE_OFFSET=%d,",q->value(23).toInt())+
      QString().sprintf("TFC_TITLE_LENGTH=%d,",q->value(24).toInt())+
      QString().sprintf("TFC_HOURS_OFFSET=%d,",q->value(25).toInt())+
      QString().sprintf("TFC_HOURS_LENGTH=%d,",q->value(26).toInt())+
      QString().sprintf("TFC_MINUTES_OFFSET=%d,",q->value(27).toInt())+
      QString().sprintf("TFC_MINUTES_LENGTH=%d,",q->value(28).toInt())+
      QString().sprintf("TFC_SECONDS_OFFSET=%d,",q->value(29).toInt())+
      QString().sprintf("TFC_SECONDS_LENGTH=%d,",q->value(30).toInt())+
      QString().sprintf("TFC_LEN_HOURS_OFFSET=%d,",q->value(31).toInt())+
      QString().sprintf("TFC_LEN_HOURS_LENGTH=%d,",q->value(32).toInt())+
      QString().sprintf("TFC_LEN_MINUTES_OFFSET=%d,",q->value(33).toInt())+
      QString().sprintf("TFC_LEN_MINUTES_LENGTH=%d,",q->value(34).toInt())+
      QString().sprintf("TFC_LEN_SECONDS_OFFSET=%d,",q->value(35).toInt())+
      QString().sprintf("TFC_LEN_SECONDS_LENGTH=%d,",q->value(36).toInt())+
      QString().sprintf("TFC_DATA_OFFSET=%d,",q->value(37).toInt())+
      QString().sprintf("TFC_DATA_LENGTH=%d,",q->value(38).toInt())+
      QString().sprintf("TFC_EVENT_ID_OFFSET=%d,",q->value(39).toInt())+
      QString().sprintf("TFC_EVENT_ID_LENGTH=%d,",q->value(40).toInt())+
      QString().sprintf("TFC_ANNC_TYPE_OFFSET=%d,",q->value(41).toInt())+
      QString().sprintf("TFC_ANNC_TYPE_LENGTH=%d,",q->value(42).toInt())+

      "MUS_PATH='"+RDEscapeString(q->value(43).toString())+"',"+
      "MUS_PREIMPORT_CMD='"+RDEscapeString(q->value(44).toString())+"',"+
      "MUS_IMPORT_TEMPLATE='"+RDEscapeString(q->value(45).toString())+"',"+
      "MUS_LABEL_CART='"+RDEscapeString(q->value(46).toString())+"',"+
      "MUS_TRACK_CART='"+RDEscapeString(q->value(47).toString())+"',"+
      "MUS_BREAK_STRING='"+RDEscapeString(q->value(48).toString())+"',"+
      "MUS_TRACK_STRING='"+RDEscapeString(q->value(49).toString())+"',"+
      QString().sprintf("MUS_CART_OFFSET=%d,",q->value(50).toInt())+
      QString().sprintf("MUS_CART_LENGTH=%d,",q->value(51).toInt())+
      QString().sprintf("MUS_TITLE_OFFSET=%d,",q->value(52).toInt())+
      QString().sprintf("MUS_TITLE_LENGTH=%d,",q->value(53).toInt())+
      QString().sprintf("MUS_HOURS_OFFSET=%d,",q->value(54).toInt())+
      QString().sprintf("MUS_HOURS_LENGTH=%d,",q->value(55).toInt())+
      QString().sprintf("MUS_MINUTES_OFFSET=%d,",q->value(56).toInt())+
      QString().sprintf("MUS_MINUTES_LENGTH=%d,",q->value(57).toInt())+
      QString().sprintf("MUS_SECONDS_OFFSET=%d,",q->value(58).toInt())+
      QString().sprintf("MUS_SECONDS_LENGTH=%d,",q->value(59).toInt())+
      QString().sprintf("MUS_LEN_HOURS_OFFSET=%d,",q->value(60).toInt())+
      QString().sprintf("MUS_LEN_HOURS_LENGTH=%d,",q->value(61).toInt())+
      QString().sprintf("MUS_LEN_MINUTES_OFFSET=%d,",q->value(62).toInt())+
      QString().sprintf("MUS_LEN_MINUTES_LENGTH=%d,",q->value(63).toInt())+
      QString().sprintf("MUS_LEN_SECONDS_OFFSET=%d,",q->value(64).toInt())+
      QString().sprintf("MUS_LEN_SECONDS_LENGTH=%d,",q->value(65).toInt())+
      QString().sprintf("MUS_DATA_OFFSET=%d,",q->value(66).toInt())+
      QString().sprintf("MUS_DATA_LENGTH=%d,",q->value(67).toInt())+
      QString().sprintf("MUS_EVENT_ID_OFFSET=%d,",q->value(68).toInt())+
      QString().sprintf("MUS_EVENT_ID_LENGTH=%d,",q->value(69).toInt())+
      QString().sprintf("MUS_ANNC_TYPE_OFFSET=%d,",q->value(70).toInt())+
      QString().sprintf("MUS_ANNC_TYPE_LENGTH=%d",q->value(71).toInt());
    printf("%s;\n",sql.toUtf8().constData());
  }
  delete q;

  printf("\n");
}


void MainObject::DumpServiceClocks() const
{
  QString sql;
  RDSqlQuery *q=NULL;

  printf("--\n");
  printf("-- Dumping data for table 'SERVICE_CLOCKS'\n");
  printf("--\n");

  sql=QString("select ")+
    "SERVICE_NAME,"+  // 00
    "HOUR,"+          // 01
    "CLOCK_NAME "+    // 02
    "from SERVICE_CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into SERVICE_CLOCKS set ")+
      "SERVICE_NAME='"+RDEscapeString(q->value(0).toString())+"',"+
      QString().sprintf("HOUR=%d,",q->value(1).toInt())+
      "CLOCK_NAME='"+RDEscapeString(q->value(2).toString())+"'";
    printf("%s;\n",sql.toUtf8().constData());
  }
  delete q;

  printf("\n");
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);

  new MainObject();
  return a.exec();
}
