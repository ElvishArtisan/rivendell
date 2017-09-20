// rdcatchd.cpp
//
// The Rivendell Netcatcher Daemon Dropbox methods
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdescape_string.h>

#include "rdcatchd.h"

void MainObject::StartDropboxes()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  //
  // Kill Old Instances
  //
  system("killall rdimport");

  //
  // Launch Dropbox Configurations
  //
  sql=QString("select ")+
    "ID,"+                       // 00
    "GROUP_NAME,"+               // 01
    "PATH,"+                     // 02
    "NORMALIZATION_LEVEL,"+      // 03
    "AUTOTRIM_LEVEL,"+           // 04
    "TO_CART,"+                  // 05
    "USE_CARTCHUNK_ID,"+         // 06
    "TITLE_FROM_CARTCHUNK_ID,"+  // 07
    "DELETE_CUTS,"+              // 08
    "METADATA_PATTERN,"+         // 09
    "FIX_BROKEN_FORMATS,"+       // 10
    "LOG_PATH,"+                 // 11
    "DELETE_SOURCE,"+            // 12
    "STARTDATE_OFFSET,"+         // 13
    "ENDDATE_OFFSET,"+           // 14
    "ID,"+                       // 15
    "IMPORT_CREATE_DATES,"+      // 16
    "CREATE_STARTDATE_OFFSET,"+  // 17
    "CREATE_ENDDATE_OFFSET,"+    // 18
    "SET_USER_DEFINED,"+         // 19
    "FORCE_TO_MONO "+            // 20
    "from DROPBOXES where "+
    "STATION_NAME=\""+RDEscapeString(catch_config->stationName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QString cmd=QString().
      sprintf("nice rdimport --persistent-dropbox-id=%d --drop-box --log-mode",
	      q->value(15).toInt());
    sql=QString("select SCHED_CODE from DROPBOX_SCHED_CODES where ")+
      QString().sprintf("DROPBOX_ID=%d",q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      cmd+=QString(" --add-scheduler-code=\"")+q1->value(0).toString()+"\"";
    }
    delete q1;
    cmd+=
      QString().sprintf(" --normalization-level=%d",q->value(3).toInt()/100);
    cmd+=
      QString().sprintf(" --autotrim-level=%d",q->value(4).toInt()/100);
    if(q->value(5).toUInt()>0) {
      cmd+=QString().sprintf(" --to-cart=%u",q->value(5).toUInt());
    }
    if(q->value(6).toString()=="Y") {
      cmd+=" --use-cartchunk-cutid";
    }
    if(q->value(7).toString()=="Y") {
      cmd+=" --title-from-cartchunk-cutid";
    }
    if(q->value(8).toString()=="Y") {
      cmd+=" --delete-cuts";
    }
    if(q->value(20).toString()=="Y") {
      cmd+=" --to-mono";
    }
    if(!q->value(9).toString().isEmpty()) {
      cmd+=QString().sprintf(" \"--metadata-pattern=%s\"",
			     (const char *)q->value(9).toString());
    }
    if(q->value(10).toString()=="Y") {
      cmd+=" --fix-broken-formats";
    }
    if(q->value(12).toString()=="Y") {
      cmd+=" --delete-source";
    }
    if(q->value(16).toString()=="Y") {
      cmd+=QString().sprintf(" --create-startdate-offset=%d",
			     q->value(17).toInt());
      cmd+=QString().sprintf(" --create-enddate-offset=%d",
			     q->value(18).toInt());
    }
    if(!q->value(19).toString().isEmpty()) {
      cmd+=" --set-user-defined="+RDEscapeString(q->value(19).toString());
    }
    cmd+=QString().sprintf(" --startdate-offset=%d",q->value(13).toInt());
    cmd+=QString().sprintf(" --enddate-offset=%d",q->value(14).toInt());
    cmd+=QString().sprintf(" %s \"%s\"",(const char *)q->value(1).toString(),
			   (const char *)q->value(2).toString());
    if(!q->value(11).toString().isEmpty()) {
      cmd+=QString().sprintf(" >> %s 2>> %s",
			     (const char *)q->value(11).toString(),
			     (const char *)q->value(11).toString());
    }
    else {
      cmd+=" > /dev/null 2> /dev/null";
    }
    cmd+=" &";
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("launching dropbox configuration: \"%s\"",
		    (const char *)cmd));
    if(fork()==0) {
      system(cmd);
      exit(0);
    }
  }
  delete q;
}
