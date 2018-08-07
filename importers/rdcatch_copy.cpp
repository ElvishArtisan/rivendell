// rdcatch_copy.cpp
//
// An RDCatch event copier.
//
//   (C) Copyright 2002-2005,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <qapplication.h>

#include <dbversion.h>
#include <rd.h>
#include <rdcmd_switch.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "rdcatch_copy.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  bool found=false;
  QString src_hostname;
  QString dest_hostname;
  QString src_station;
  QString dest_station;
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcatch_copy",
		    RDCATCH_COPY_USAGE);
  delete cmd;

  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();
  rd_config->setModuleName("rdcatch_copy");

  //
  // Read Switches
  //
  for(int i=1;i<qApp->argc();i+=2) {
    found=false;
    if(QString(qApp->argv()[i])=="-h") {  // Source mySQL Hostname
      if((i+1)==qApp->argc()) {
	fprintf(stderr,"rdcatch_copy: invalid argument\n");
	exit(256);
      }
      src_hostname=qApp->argv()[i+1];
      found=true;
    }
    if(QString(qApp->argv()[i])=="-s") {  // Source Rivendell Host
      if((i+1)==qApp->argc()) {
	fprintf(stderr,"rdcatch_copy: invalid argument\n");
	exit(256);
      }
      src_station=qApp->argv()[i+1];
      found=true;
    }
    if(QString(qApp->argv()[i])=="-H") {  // Source mySQL Hostname
      if((i+1)==qApp->argc()) {
	fprintf(stderr,"rdcatch_copy: invalid argument\n");
	exit(256);
      }
      dest_hostname=qApp->argv()[i+1];
      found=true;
    }
    if(QString(qApp->argv()[i])=="-S") {  // Source Rivendell Host
      if((i+1)==qApp->argc()) {
	fprintf(stderr,"rdcatch_copy: invalid argument\n");
	exit(256);
      }
      dest_station=qApp->argv()[i+1];
      found=true;
    }
    if(!found) {
      fprintf(stderr,"rdcatch_copy: invalid argument\n");
      exit(256);
    }
  }

  if(src_hostname.isEmpty()) {
    fprintf(stderr,"rdcatch_copy: invalid source mySQL hostname\n");
    exit(256);
  }
  if(src_station.isEmpty()) {
    fprintf(stderr,"rdcatch_copy: invalid source Rivendell host\n");
    exit(256);
  }
  if(dest_hostname.isEmpty()) {
    fprintf(stderr,"rdcatch_copy: invalid destination mySQL hostname\n");
    exit(256);
  }
  if(dest_station.isEmpty()) {
    fprintf(stderr,"rdcatch_copy: invalid destination Rivendell host\n");
    exit(256);
  }

  //
  // Open Databases
  //
  src_db=QSqlDatabase::addDatabase("QMYSQL3");
  src_db.setDatabaseName(rd_config->mysqlDbname());
  src_db.setUserName(rd_config->mysqlUsername());
  src_db.setPassword(rd_config->mysqlPassword());
  src_db.setHostName(src_hostname);
  if(!src_db.open()) {
    fprintf(stderr,"rdcatch_copy: unable to connect to source mySQL server\n");
    src_db.removeDatabase(rd_config->mysqlDbname());
    exit(256);
  }

  if(src_hostname==dest_hostname) {
    if(src_station==dest_station) {
      fprintf(stderr,"rdcatch_copy: cannot copy a host configuration onto itself\n");
      exit(256);
    }
    else {
      dest_db=src_db;
    }
  }
  else {
    dest_db=QSqlDatabase::addDatabase("QMYSQL3");
    dest_db.setDatabaseName(rd_config->mysqlDbname());
    dest_db.setUserName(rd_config->mysqlUsername());
    dest_db.setPassword(rd_config->mysqlPassword());
    dest_db.setHostName(dest_hostname);
    if(!dest_db.open()) {
      fprintf(stderr,
	      "rdcatch_copy: unable to connect to destination mySQL server\n");
      exit(256);
    }
  }

  //
  // Check Database Versions
  //
  sql=QString("select DB from VERSION");
  q=new QSqlQuery(sql,src_db);
  if(!q->first()) {
    fprintf(stderr,
	    "rdcatch_copy: unable to read source database version\n");
    exit(256);
  }
  if(q->value(0).toInt()!=RD_VERSION_DATABASE) {
    fprintf(stderr,"rdcatch_copy: source database version mismatch\n");
    exit(256);
  }
  delete q;

  q=new QSqlQuery(sql,dest_db);
  if(!q->first()) {
    fprintf(stderr,
	    "rdcatch_copy: unable to read destination database version\n");
    exit(256);
  }
  if(q->value(0).toInt()!=RD_VERSION_DATABASE) {
    fprintf(stderr,"rdcatch_copy: destination database version mismatch\n");
    exit(256);
  }
  delete q;

  //
  // Check Rivendell Hosts
  //
  sql=QString("select NAME from STATIONS where ")+
    "NAME=\""+RDEscapeString(src_station)+"\"";
  q=new QSqlQuery(sql,src_db);
  if(!q->first()) {
    fprintf(stderr,
	    "rdcatch_copy: source Rivendell host doesn't exist\n");
    exit(256);
  }
  delete q;

  sql=QString("select NAME from STATIONS where ")+
    "NAME=\""+RDEscapeString(dest_station)+"\"";
  q=new QSqlQuery(sql,dest_db);
  if(!q->first()) {
    fprintf(stderr,
	    "rdcatch_copy: destination Rivendell host doesn't exist\n");
    exit(256);
  }
  delete q;

  //
  // Confirmation Prompt
  //
  printf("\n");
  printf("****** WARNING ******\n");
  printf(" This operation will OVERWRITE ALL RDCATCH EVENTS on the destination Host!");
  printf(" Press RETURN to continue, or CNTL-C to abort.");
  printf("\n");
  while(getchar()!=10);
  printf("Copying events...");
  fflush(stdout);

  //
  // Delete current destination entries
  //
  sql=QString("delete from RECORDINGS where ")+
    "STATION_NAME\""+RDEscapeString(dest_station)+"\"";
  q=new QSqlQuery(sql,dest_db);
  delete q;

  //
  // Copy Entries
  //
  sql=QString("select ")+
    "IS_ACTIVE,"+         // 00
    "TYPE,"+              // 01
    "CHANNEL,"+           // 02
    "CUT_NAME,"+          // 03
    "SUN,"+               // 04
    "MON,"+               // 05
    "TUE,"+               // 06
    "WED,"+               // 07
    "THU,"+               // 08
    "FRI,"+               // 09
    "SAT,"+               // 10
    "DESCRIPTION,"+       // 11
    "START_TYPE,"+        // 12
    "START_TIME,"+        // 13
    "START_LENGTH,"+      // 14
    "START_MATRIX,"+      // 15
    "START_LINE,"+        // 16
    "START_OFFSET,"+      // 17
    "END_TYPE,"+          // 18
    "END_TIME,"+          // 19
    "END_LENGTH,"+        // 20
    "END_MATRIX,"+        // 21
    "END_LINE,"+          // 22
    "LENGTH,"+            // 23
    "TRIM_THRESHOLD,"+    // 24
    "NORMALIZE_LEVEL,"+   // 25
    "STARTDATE_OFFSET,"+  // 26
    "ENDDATE_OFFSET,"+    // 27
    "FORMAT,"+            // 28
    "CHANNELS,"+          // 29
    "SAMPRATE,"+          // 30
    "BITRATE,"+           // 31
    "QUALITY,"+           // 32
    "MACRO_CART,"+        // 33
    "SWITCH_INPUT,"+      // 34
    "SWITCH_OUTPUT,"+     // 35
    "EXIT_CODE,"+         // 36
    "ONE_SHOT,"+          // 37
    "URL,"+               // 38
    "URL_USERNAME,"+      // 39
    "URL_PASSWORD "+      // 40
    "from RECORDINGS where "+
    "STATION_NAME=\""+RDEscapeString(src_station)+"\"";
  q=new QSqlQuery(sql,src_db);
  while(q->next()) {
    sql=QString("insert into RECORDINGS set ")+
      ":IS_ACTIVE=\""+RDEscapeString(q->value(0).toString())+"\","+
      QString().sprintf("TYPE=%d,",q->value(1).toInt())+
      QString().sprintf("CHANNEL=%u,",q->value(2).toUInt())+
      "CUT_NAME=\""+RDEscapeString(q->value(3).toString())+"\","+
      "SUN=\""+RDEscapeString(q->value(4).toString())+"\","+
      "MON=\""+RDEscapeString(q->value(5).toString())+"\","+
      "TUE=\""+RDEscapeString(q->value(6).toString())+"\","+
      "WED=\""+RDEscapeString(q->value(7).toString())+"\","+
      "THU=\""+RDEscapeString(q->value(8).toString())+"\","+
      "FRI=\""+RDEscapeString(q->value(9).toString())+"\","+
      "SAT=\""+RDEscapeString(q->value(10).toString())+"\","+
      "DESCRIPTION=\""+RDEscapeString(q->value(11).toString())+"\","+
      QString().sprintf("START_TYPE=%d,",q->value(12).toInt())+
      "START_TIME=\""+RDEscapeString(q->value(13).toString())+"\","+
      QString().sprintf("START_LENGTH=%d,",q->value(14).toInt())+
      QString().sprintf("START_MATRIX=%d,",q->value(15).toInt())+
      QString().sprintf("START_LINE=%d,",q->value(16).toInt())+
      QString().sprintf("START_OFFSET=%d,",q->value(17).toInt())+
      QString().sprintf("END_TYPE=%d,",q->value(18).toInt())+
      "END_TIME=\""+RDEscapeString(q->value(19).toString())+"\","+
      QString().sprintf("END_LENGTH=%d,",q->value(20).toInt())+
      QString().sprintf("END_MATRIX=%d,",q->value(21).toInt())+
      QString().sprintf("END_LINE=%d,",q->value(22).toInt())+
      QString().sprintf("LENGTH=%u,",q->value(23).toUInt())+
      QString().sprintf("TRIM_THRESHOLD=%d,",q->value(24).toInt())+
      QString().sprintf("NORMALIZE_LEVEL=%d,",q->value(25).toInt())+
      QString().sprintf("STARTDATE_OFFSET=%u,",q->value(26).toUInt())+
      QString().sprintf("ENDDATE_OFFSET=%u,",q->value(27).toUInt())+
      QString().sprintf("FORMAT=%d,",q->value(28).toInt())+
      QString().sprintf("CHANNELS=%d,",q->value(29).toInt())+
      QString().sprintf("SAMPRATE=%d,",q->value(30).toInt())+
      QString().sprintf("BITRATE=%d,",q->value(31).toInt())+
      QString().sprintf("QUALITY=%d,",q->value(32).toInt())+
      QString().sprintf("MACRO_CART=%d,",q->value(33).toInt())+
      QString().sprintf("SWITCH_INPUT=%d,",q->value(34).toInt())+
      QString().sprintf("SWITCH_OUTPUT=%d,",q->value(35).toInt())+
      QString().sprintf("EXIT_CODE=%d,",q->value(36).toInt())+
      "ONE_SHOT=\""+RDEscapeString(q->value(37).toString())+"\","+
      "URL=\""+RDEscapeString(q->value(38).toString())+"\","+
      "URL_USERNAME=\""+RDEscapeString(q->value(39).toString())+"\","+
      "URL_PASSWORD=\""+RDEscapeString(q->value(40).toString())+"\","+
      "STATION_NAME=\""+RDEscapeString(dest_station)+"\"";
    q1=new QSqlQuery(sql,dest_db);
    delete q1;
  }
  delete q;

  printf("done.\n");
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
