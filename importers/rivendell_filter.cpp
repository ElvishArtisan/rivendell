// rivendell_filter.cpp
//
// A Library import filter for an external Rivendell system
//
//   (C) Copyright 2002-2005,2008,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <string.h>
#include <ctype.h>

#include <qapplication.h>
#include <qdir.h>

#include <rddb.h>
#include <rd.h>
#include <rdconf.h>
#include <rdconfig.h>
#include <rdcmd_switch.h>
#include <rdescape_string.h>
#include <rdcut.h>
#include <rivendell_filter.h>

//
// Global Variables
//
RDConfig *rdconfig;


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString ext_dbname;
  QString ext_hostname;
  QString ext_username;
  QString ext_password;
  QString ext_audiodir;
  QString default_group;
  unsigned start_cartnum=0;
  unsigned end_cartnum=0;
  QSqlDatabase *filter_db;
  QSqlDatabase *ext_db;
  bool ok=false;
  bool found;
  QString start_datetime;
  QString end_datetime;
  QString start_daypart;
  QString end_daypart;
  QString owner;
  QString group;
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;
  QSqlQuery *q2;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rivendell_filter",
		    RIVENDELL_FILTER_USAGE);
  delete cmd;

  rdconfig=new RDConfig(RD_CONF_FILE);
  rdconfig->load();

  //
  // Open Local Database
  //
  filter_db=QSqlDatabase::addDatabase(rdconfig->mysqlDriver(),"LOCAL_DB");
  if(!filter_db) {
    fprintf(stderr,"rivendell_filter: can't open local mySQL database\n");
    exit(1);
  }
  filter_db->setDatabaseName(rdconfig->mysqlDbname());
  filter_db->setUserName(rdconfig->mysqlUsername());
  filter_db->setPassword(rdconfig->mysqlPassword());
  filter_db->setHostName(rdconfig->mysqlHostname());
  if(!filter_db->open()) {
    fprintf(stderr,
	    "rivendell_filter: unable to connect to local mySQL Server\n");
    filter_db->removeDatabase(rdconfig->mysqlDbname());
    exit(1);
  }

  //
  // Read Arguments
  //
  for(int i=1;i<(qApp->argc()-1);i+=2) {
    found=false;
    if(!strcmp("-h",qApp->argv()[i])) {
      ext_hostname=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-u",qApp->argv()[i])) {
      ext_username=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-p",qApp->argv()[i])) {
      ext_password=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-A",qApp->argv()[i])) {
      ext_audiodir=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-g",qApp->argv()[i])) {
      default_group=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-s",qApp->argv()[i])) {
      start_cartnum=QString(qApp->argv()[i+1]).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"\nrivendell_filter: invalid group number\n");
	exit(256);
      }
      found=true;
    }
    if(!strcmp("-e",qApp->argv()[i])) {
      end_cartnum=QString(qApp->argv()[i+1]).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"\nrivendell_filter: invalid group number\n");
	exit(256);
      }
      found=true;
    }
    if(!found) {
      fprintf(stderr,"\nrivendell_filter %s\n",RIVENDELL_FILTER_USAGE);
      exit(1);
    }
  }
  if((start_cartnum==0)||(start_cartnum>999999)) {
    fprintf(stderr,"\nrivendell_filter: invalid start cart value\n");
    exit(256);
  }
  if((end_cartnum==0)||(end_cartnum>999999)) {
    fprintf(stderr,"\nrivendell_filter: invalid end cart value\n");
    exit(256);
  }
  if(start_cartnum>end_cartnum) {
    fprintf(stderr,"\nrivendell_filter: invalid cart values\n");
    exit(256);
  }
  if(ext_audiodir.isEmpty()) {
    fprintf(stderr,"rivendell_filter: invalid source audio directory\n");
    exit(256);
  }
  QDir dir=QDir(ext_audiodir);
  if(!dir.exists()) {
    fprintf(stderr,"rivendell_filter: invalid source audio directory\n");
    exit(256);
  }
  if(!dir.isReadable()) {
    fprintf(stderr,
	    "rivendell_filter: source audio directory is not readable\n");
    exit(256);
  }
  if(ext_audiodir.right(1)!="/") {
    ext_audiodir+="/";
  }

  //
  // Open Remote Database
  //
  ext_db=QSqlDatabase::addDatabase(rdconfig->mysqlDriver(),"REMOTE_DB");
  if(!ext_db) {
    fprintf(stderr,"rivendell_filter: can't open remote mySQL database\n");
    exit(1);
  }
  ext_db->setDatabaseName(rdconfig->mysqlDbname());
  ext_db->setUserName(ext_username);
  ext_db->setPassword(ext_password);
  ext_db->setHostName(ext_hostname);
  if(!ext_db->open()) {
    fprintf(stderr,
	    "rivendell_filter: unable to connect to remote mySQL Server\n");
    ext_db->removeDatabase(rdconfig->mysqlDbname());
    exit(1);
  }

  //
  // Verify that default group exists
  //
  sql=QString("select NAME from GROUPS where ")+
    "NAME=\""+RDEscapeString(default_group)+"\"";
  q=new QSqlQuery(sql,filter_db);
  if(!q->next()) {
    fprintf(stderr,"rivendell_filter: default group does not exist\n");
    exit(256);
  }
  delete q;

  //
  // Transfer Loop
  //
  sql=QString("select ")+
    "NUMBER,"+                // 00
    "TYPE,"+                  // 01
    "GROUP_NAME,"+            // 02
    "TITLE,"+                 // 03
    "ARTIST,"+                // 04
    "ALBUM,"+                 // 05
    "YEAR,"+                  // 06
    "ISRC,"+                  // 07
    "LABEL,"+                 // 08
    "CLIENT,"+                // 09
    "AGENCY,"+                // 10
    "PUBLISHER,"+             // 11
    "COMPOSER,"+              // 12
    "USER_DEFINED,"+          // 13
    "USAGE_CODE,"+            // 14
    "FORCED_LENGTH,"+         // 15
    "AVERAGE_LENGTH,"+        // 16
    "LENGTH_DEVIATION,"+      // 17
    "AVERAGE_SEGUE_LENGTH,"+  // 18
    "AVERAGE_HOOK_LENGTH,"+   // 19
    "CUT_QUANTITY,"+          // 20
    "LAST_CUT_PLAYED,"+       // 21
    "PLAY_ORDER,"+            // 22
    "VALIDITY,"+              // 23
    "ENFORCE_LENGTH,"+        // 24
    "PRESERVE_PITCH,"+        // 25
    "ASYNCRONOUS,"+           // 26
    "OWNER,"+                 // 27
    "MACROS,"+                // 28
    "SCHED_CODES "+           // 29
    "from CART where "+
    QString().sprintf("(NUMBER>=%u)&&(NUMBER<=%u)",start_cartnum,end_cartnum);
  q=new QSqlQuery(sql,ext_db);
  while(q->next()) {
    printf("Transferring cart %06u [%s]...",q->value(0).toUInt(),
	   (const char *)q->value(3).toString());
    fflush(stdout);

    //
    // Validate Group
    //
    sql=QString("select NAME from GROUPS where ")+
      "NAME=\""+RDEscapeString(q->value(2).toString())+"\"";
    q1=new QSqlQuery(sql,filter_db);
    if(q1->first()) {
      group=q->value(2).toString();
    }
    else {
      group=default_group;
    }
    delete q1;

    //
    // Purge old entries
    //
    sql=QString("select CUT_NAME from CUTS where ")+
      QString().sprintf("CART_NUMBER=%u",q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    while(q1->next()) {
      unlink(RDCut::pathName(q1->value(0).toString()));
    }
    delete q1;
    sql=QString("delete from CUTS where ")+
      QString().sprintf("CART_NUMBER=%u",q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    delete q1;
    sql=QString("delete from CART where ")+
      QString().sprintf("NUMBER=%u",q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    delete q1;

    //
    // Create new entries
    //
    if(q->value(27).isNull()) {
      owner="null";
    }
    else {
      owner=QString().sprintf("\"%s\"",
	       (const char *)RDEscapeString(q->value(27).toString()));
    }
    sql=QString().sprintf("insert into CART set ")+
      QString().sprintf("NUMBER=%u,",q->value(0).toUInt())+
      QString().sprintf("TYPE=%u,",q->value(1).toUInt())+
      "GROUP_NAME=\""+RDEscapeString(q->value(2).toString())+"\","+
      "TITLE=\""+RDEscapeString(q->value(3).toString())+"\","+
      "ARTIST=\""+RDEscapeString(q->value(4).toString())+"\","+
      "ALBUM=\""+RDEscapeString(q->value(5).toString())+"\","+
      "YEAR="+RDCheckDateTime(q->value(6).toDate(),"yyyy-MM-dd")+","+
      "ISRC=\""+RDEscapeString(q->value(7).toString())+"\","+
      "LABEL=\""+RDEscapeString(q->value(8).toString())+"\","+
      "CLIENT=\""+RDEscapeString(q->value(9).toString())+"\","+
      "AGENCY=\""+RDEscapeString(q->value(10).toString())+"\","+
      "PUBLISHER=\""+RDEscapeString(q->value(11).toString())+"\","+
      "COMPOSER=\""+RDEscapeString(q->value(12).toString())+"\","+
      "USER_DEFINED=\""+RDEscapeString(q->value(13).toString())+"\","+
      "USAGE_CODE=\""+RDEscapeString(q->value(14).toString())+"\","+
      QString().sprintf("FORCED_LENGTH=%u,",q->value(15).toUInt())+
      QString().sprintf("AVERAGE_LENGTH=%u,",q->value(16).toUInt())+
      QString().sprintf("LENGTH_DEVIATION=%u,",q->value(17).toUInt())+
      QString().sprintf("AVERAGE_SEGUE_LENGTH=%u,",q->value(18).toUInt())+
      QString().sprintf("AVERAGE_HOOK_LENGTH=%u,",q->value(19).toUInt())+
      QString().sprintf("CUT_QUANTITY=%u,",q->value(20).toUInt())+
      QString().sprintf("LAST_CUT_PLAYED=%u,",q->value(21).toUInt())+
      QString().sprintf("PLAY_ORDER=%u,",q->value(22).toUInt())+
      QString().sprintf("VALIDITY=%u,",q->value(23).toUInt())+
      "ENFORCE_LENGTH=\""+RDEscapeString(q->value(24).toString())+"\","+
      "PRESERVE_PITCH=\""+RDEscapeString(q->value(25).toString())+"\","+
      "ASYNCRONOUS=\""+RDEscapeString(q->value(26).toString())+"\","+
      "OWNER="+owner+","+
      "MACROS=\""+RDEscapeString(q->value(28).toString())+"\","+
      "SCHED_CODES=\""+RDEscapeString(q->value(29).toString())+"\"";
    q1=new QSqlQuery(sql,filter_db);
    delete q1;

    sql=QString("select ")+
      "CUT_NAME,"+            // 00
      "EVERGREEN,"+           // 01
      "DESCRIPTION,"+         // 02
      "OUTCUE,"+              // 03
      "ISRC,"+                // 04
      "LENGTH,"+              // 05
      "ORIGIN_DATETIME,"+     // 06
      "START_DATETIME,"+      // 07
      "END_DATETIME,"+        // 08
      "SUN,"+                 // 09
      "MON,"+                 // 10
      "TUE,"+                 // 11
      "WED,"+                 // 12
      "THU,"+                 // 13
      "FRI,"+                 // 14
      "SAT,"+                 // 15
      "START_DAYPART,"+       // 16
      "END_DAYPART,"+         // 17
      "ORIGIN_NAME,"+         // 18
      "WEIGHT,VALIDITY,"+     // 19
      "CODING_FORMAT,"+       // 20
      "SAMPLE_RATE,"+         // 21
      "BIT_RATE,"+            // 22
      "CHANNELS,"+            // 23
      "PLAY_GAIN,"+           // 24
      "START_POINT,"+         // 25
      "END_POINT,"+           // 26
      "FADEUP_POINT,"+        // 27
      "FADEDOWN_POINT,"+      // 28
      "SEGUE_START_POINT,"+   // 29
      "SEGUE_END_POINT,"+     // 30
      "SEGUE_GAIN,"+          // 31
      "HOOK_START_POINT,"+    // 32
      "HOOK_END_POINT,"+      // 33
      "TALK_START_POINT,"+    // 34
      "TALK_END_POINT "+      // 35
      "from CUTS where "+
      QString().sprintf("CART_NUMBER=%u",q->value(0).toUInt());
    q1=new QSqlQuery(sql,ext_db);
    while(q1->next()) {
      if(q1->value(7).isNull()) {
	start_datetime="null";
      }
      else {
	start_datetime=
	  RDCheckDateTime(q1->value(7).toDateTime(),"yyyy-MM-dd hh:mm:ss");
      }
      if(q1->value(8).isNull()) {
	end_datetime="null";
      }
      else {
	end_datetime=
	  RDCheckDateTime(q1->value(8).toDateTime(),"yyyy-MM-dd hh:mm:ss");
      }
      if(q1->value(16).isNull()) {
	start_daypart="null";
      }
      else {
	start_daypart=RDCheckDateTime(q1->value(16).toTime(),"hh:mm:ss");
      }
      if(q1->value(17).isNull()) {
	end_daypart="null";
      }
      else {
	end_daypart=RDCheckDateTime(q1->value(17).toTime(),"hh:mm:ss");
      }
      sql=QString("insert into CUTS set ")+
	QString().sprintf("CART_NUMBER=%u,",q->value(0).toUInt())+
	"CUT_NAME=\""+RDEscapeString(q1->value(0).toString())+"\","+
	"EVERGREEN=\""+RDEscapeString(q1->value(1).toString())+"\","+
	"DESCRIPTION=\""+RDEscapeString(q1->value(2).toString())+"\","+
	"OUTCUE=\""+RDEscapeString(q1->value(3).toString())+"\","+
	"ISRC=\""+RDEscapeString(q1->value(4).toString())+"\","+
	QString().sprintf("LENGTH=%u,",q1->value(5).toUInt())+
	"ORIGIN_DATETIME=\""+RDEscapeString(q1->value(6).toString())+"\","+
	"START_DATETIME="+start_datetime+","+
	"END_DATETIME="+end_datetime+","+
	"SUN=\""+q1->value(9).toString()+"\","+
	"MON=\""+q1->value(10).toString()+"\","+
	"TUE=\""+q1->value(11).toString()+"\","+
	"WED=\""+q1->value(12).toString()+"\","+
	"THU=\""+q1->value(13).toString()+"\","+
	"FRI=\""+q1->value(14).toString()+"\","+
	"SAT=\""+q1->value(15).toString()+"\","+
	"START_DAYPART="+start_daypart+","+
	"END_DAYPART="+end_daypart+","+
	"ORIGIN_NAME=\""+RDEscapeString(q1->value(18).toString())+"\","+
	QString().sprintf("WEIGHT=%u,",q1->value(19).toUInt())+
	QString().sprintf("VALIDITY=%u,",q1->value(20).toUInt())+
	QString().sprintf("CODING_FORMAT=%u,",q1->value(21).toUInt())+
	QString().sprintf("SAMPLE_RATE=%u,",q1->value(22).toUInt())+
	QString().sprintf("BIT_RATE=%u,",q1->value(23).toUInt())+
	QString().sprintf("CHANNELS=%u,",q1->value(24).toUInt())+
	QString().sprintf("PLAY_GAIN=%d,",q1->value(25).toInt())+
	QString().sprintf("START_POINT=%d,",q1->value(26).toInt())+
	QString().sprintf("END_POINT=%d,",q1->value(27).toInt())+
	QString().sprintf("FADEUP_POINT=%d,",q1->value(28).toInt())+
	QString().sprintf("FADEDOWN_POINT=%d,",q1->value(29).toInt())+
	QString().sprintf("SEGUE_START_POINT=%d,",q1->value(30).toInt())+
	QString().sprintf("SEGUE_END_POINT=%d,",q1->value(31).toInt())+
	QString().sprintf("SEGUE_GAIN=%d,",q1->value(32).toInt())+
	QString().sprintf("HOOK_START_POINT=%d,",q1->value(33).toInt())+
	QString().sprintf("HOOK_END_POINT=%d,",q1->value(34).toInt())+
	QString().sprintf("TALK_START_POINT=%d,",q1->value(35).toInt())+
	QString().sprintf("TALK_END_POINT=%d",q1->value(36).toInt());
      q2=new QSqlQuery(sql,filter_db);
      delete q2;
      ok=RDCopy(QString().sprintf("%s%s.%s",(const char *)ext_audiodir,
				  (const char *)q1->value(0).toString(),
				  RD_AUDIO_EXTENSION),
		RDCut::pathName(q1->value(0).toString()));
      if(!ok) {
	printf("[WARNING -- NO AUDIO FOUND]...");
	fflush(stdout);
      }
    }
    delete q1;
    printf("done.\n");
  }
  delete q;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
