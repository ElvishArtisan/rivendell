// rivendell_filter.cpp
//
// A Library import filter for an external Rivendell system
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QDir>

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
  QSqlDatabase filter_db;
  QSqlDatabase ext_db;
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
    new RDCmdSwitch("rivendell_filter",RIVENDELL_FILTER_USAGE);
  delete cmd;

  rdconfig=new RDConfig(RD_CONF_FILE);
  rdconfig->load();
  rdconfig->setModuleName("rivendell_filter");

  //
  // Open Local Database
  //
  filter_db=QSqlDatabase::addDatabase(rdconfig->mysqlDriver(),"LOCAL_DB");
  filter_db.setDatabaseName(rdconfig->mysqlDbname());
  filter_db.setUserName(rdconfig->mysqlUsername());
  filter_db.setPassword(rdconfig->mysqlPassword());
  filter_db.setHostName(rdconfig->mysqlHostname());
  if(!filter_db.open()) {
    fprintf(stderr,
	    "rivendell_filter: unable to connect to local mySQL Server\n");
    exit(1);
  }

  //
  // Read Arguments
  //
  for(int i=1;i<(qApp->arguments().size()-1);i+=2) {
    found=false;
    if(qApp->arguments().at(i)=="-h") {
      ext_hostname=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-u") {
      ext_username=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-p") {
      ext_password=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-A") {
      ext_audiodir=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-g") {
      default_group=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-s") {
      start_cartnum=QString(qApp->arguments().at(i+1)).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"\nrivendell_filter: invalid group number\n");
	exit(256);
      }
      found=true;
    }
    if(qApp->arguments().at(i)=="-e") {
      end_cartnum=QString(qApp->arguments().at(i+1)).toUInt(&ok);
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
  ext_db.setDatabaseName(rdconfig->mysqlDbname());
  ext_db.setUserName(ext_username);
  ext_db.setPassword(ext_password);
  ext_db.setHostName(ext_hostname);
  if(!ext_db.open()) {
    fprintf(stderr,
	    "rivendell_filter: unable to connect to remote mySQL Server\n");
    exit(1);
  }

  //
  // Verify that default group exists
  //
  sql=QString("select ")+
    "`NAME` "+
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(default_group)+"'";
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
    "`NUMBER`,"+                // 00
    "`TYPE`,"+                  // 01
    "`GROUP_NAME`,"+            // 02
    "`TITLE`,"+                 // 03
    "`ARTIST`,"+                // 04
    "`ALBUM`,"+                 // 05
    "`YEAR`,"+                  // 06
    "`LABEL`,"+                 // 08
    "`CLIENT`,"+                // 09
    "`AGENCY`,"+                // 10
    "`PUBLISHER`,"+             // 11
    "`COMPOSER`,"+              // 12
    "`USER_DEFINED`,"+          // 13
    "`USAGE_CODE`,"+            // 14
    "`FORCED_LENGTH`,"+         // 15
    "`AVERAGE_LENGTH`,"+        // 16
    "`LENGTH_DEVIATION`,"+      // 17
    "`AVERAGE_SEGUE_LENGTH`,"+  // 18
    "`AVERAGE_HOOK_LENGTH`,"+   // 19
    "`CUT_QUANTITY`,"+          // 20
    "`LAST_CUT_PLAYED`,"+       // 21
    "`PLAY_ORDER`,"+            // 22
    "`VALIDITY`,"+              // 23
    "`ENFORCE_LENGTH`,"+        // 24
    "`PRESERVE_PITCH`,"+        // 25
    "`ASYNCRONOUS`,"+           // 26
    "`OWNER`,"+                 // 27
    "`MACROS`,"+                // 28
    "`SCHED_CODES` "+           // 29
    "from `CART` where "+
    QString::asprintf("(`NUMBER`>=%u)&&(`NUMBER`<=%u)",start_cartnum,end_cartnum);
  q=new QSqlQuery(sql,ext_db);
  while(q->next()) {
    printf("Transferring cart %06u [%s]...",q->value(0).toUInt(),
	   q->value(3).toString().toUtf8().constData());
    fflush(stdout);

    //
    // Validate Group
    //
    sql=QString("select `NAME` from `GROUPS` where ")+
      "`NAME`='"+RDEscapeString(q->value(2).toString())+"'";
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
    sql=QString::asprintf("select `CUT_NAME` from `CUTS` where `CART_NUMBER`=%u",
			  q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    while(q1->next()) {
      unlink(RDCut::pathName(q1->value(0).toString()).toUtf8());
    }
    delete q1;
    sql=QString::asprintf("delete from `CUTS` where `CART_NUMBER`=%u",
			  q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    delete q1;
    sql=QString::asprintf("delete from `CART` where `NUMBER`=%u",
			  q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    delete q1;

    //
    // Create new entries
    //
    if(q->value(26).isNull()) {
      owner="null";
    }
    else {
      owner=QString("'")+RDEscapeString(q->value(26).toString())+"'";
    }
    sql=QString("insert into `CART` set ")+
      QString::asprintf("`NUMBER`=%u,",q->value(0).toUInt())+
      QString::asprintf("`TYPE`=%u,",q->value(1).toUInt())+
      "`GROUP_NAME`='"+RDEscapeString(group)+"',"+
      "`TITLE`='"+RDEscapeString(q->value(3).toString())+"',"+
      "`ARTIST`='"+RDEscapeString(q->value(4).toString())+"',"+
      "`ALBUM`='"+RDEscapeString(q->value(5).toString())+"',"+
      "`YEAR`=%s,"+RDCheckDateTime(q->value(6).toDate(),"yyyy-MM-dd")+","+
      "`LABEL`='"+RDEscapeString(q->value(7).toString())+"',"+
      "`CLIENT`='"+RDEscapeString(q->value(8).toString())+"',"+
      "`AGENCY`='"+RDEscapeString(q->value(9).toString())+"',"+
      "`PUBLISHER`='"+RDEscapeString(q->value(10).toString())+"',"+
      "`COMPOSER`='"+RDEscapeString(q->value(11).toString())+"',"+
      "`USER_DEFINED`='"+RDEscapeString(q->value(12).toString())+"',"+
      "`USAGE_CODE`='"+RDEscapeString(q->value(13).toString())+"',"+
      QString::asprintf("`FORCED_LENGTH`=%u,",q->value(14).toUInt())+
      QString::asprintf("`AVERAGE_LENGTH`=%u,",q->value(15).toUInt())+
      QString::asprintf("`LENGTH_DEVIATION`=%u,",q->value(16).toUInt())+
      QString::asprintf("`AVERAGE_SEGUE_LENGTH`=%u,",q->value(17).toUInt())+
      QString::asprintf("`AVERAGE_HOOK_LENGTH`=%u,",q->value(18).toUInt())+
      QString::asprintf("`CUT_QUANTITY`=%u,",q->value(19).toUInt())+
      QString::asprintf("`LAST_CUT_PLAYED`=%u,",q->value(20).toUInt())+
      QString::asprintf("`PLAY_ORDER`=%u,",q->value(21).toUInt())+
      QString::asprintf("`VALIDITY`=%u,",q->value(22).toUInt())+
      "`ENFORCE_LENGTH`='"+RDEscapeString(q->value(23).toString())+"',"+
      "`PRESERVE_PITCH`='"+RDEscapeString(q->value(24).toString())+"',"+
      "`ASYNCRONOUS`='"+RDEscapeString(q->value(25).toString())+"',"+
      "`OWNER`="+owner+","+
      "`MACROS`='"+RDEscapeString(q->value(27).toString())+"',"+
      "`SCHED_CODES`='"+RDEscapeString(q->value(28).toString())+"'";
    q1=new QSqlQuery(sql,filter_db);
    delete q1;
    sql=QString("select ")+
      "`CUT_NAME`,"+           // 00
      "`EVERGREEN`,"+          // 01
      "`DESCRIPTION`,"+        // 02
      "`OUTCUE`,"+             // 03
      "`ISRC`,"+               // 04
      "`LENGTH`,"+             // 05
      "`ORIGIN_DATETIME`,"+    // 06
      "`START_DATETIME`,"+     // 07
      "`END_DATETIME`,"+       // 08
      "`SUN`,"+                // 09
      "`MON`,"+                // 10
      "`TUE`,"+                // 11
      "`WED`,"+                // 12
      "`THU`,"+                // 13
      "`FRI`,"+                // 14
      "`SAT`,"+                // 15
      "`START_DAYPART`,"+      // 16
      "`END_DAYPART`,"+        // 17
      "`ORIGIN_NAME`,"+        // 18
      "`WEIGHT`,"+             // 19
      "`VALIDITY`,"+           // 20
      "`CODING_FORMAT`,"+      // 21
      "`BIT_RATE`,"+           // 22
      "`CHANNELS`,"+           // 23
      "`PLAY_GAIN`,"+          // 24
      "`START_POINT`,"+        // 25
      "`END_POINT`,"+          // 26
      "`FADEUP_POINT`,"+       // 27
      "`FADEDOWN_POINT`,"+     // 28
      "`SEGUE_START_POINT`,"+  // 29
      "`SEGUE_END_POINT`,"+    // 30
      "`SEGUE_GAIN`,"+         // 31
      "`HOOK_START_POINT`,"+   // 32
      "`HOOK_END_POINT`,"+     // 33
      "`TALK_START_POINT`,"+   // 34
      "`TALK_END_POINT` "+     // 35
      "from `CUTS` where "+
      QString::asprintf("`CART_NUMBER`=%u",q->value(0).toUInt());
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
      sql=QString("insert into `CUTS` set ")+
	"`CUT_NAME`='"+RDEscapeString(q1->value(0).toString())+"',"+
	QString::asprintf("`CART_NUMBER`=%u,",q->value(0).toUInt())+
	"`EVERGREEN`='"+RDEscapeString(q1->value(1).toString())+"',"+
	"`DESCRIPTION`='"+RDEscapeString(q1->value(2).toString())+"',"+
	"`OUTCUE`='"+RDEscapeString(q1->value(3).toString())+"',"+
	"`ISRC`='"+RDEscapeString(q1->value(4).toString())+"',"+
	QString::asprintf("`LENGTH`=%u,",q1->value(5).toUInt())+
	"`ORIGIN_DATETIME`='"+RDEscapeString(q1->value(6).toString())+"',"+
	"`START_DATETIME`="+start_datetime+","+
	"`END_DATETIME`="+end_datetime+","+
	"`SUN`='"+RDEscapeString(q1->value(9).toString())+"',"+
	"`MON`='"+RDEscapeString(q1->value(10).toString())+"',"+
	"`TUE`='"+RDEscapeString(q1->value(11).toString())+"',"+
	"`WED`='"+RDEscapeString(q1->value(12).toString())+"',"+
	"`THU`='"+RDEscapeString(q1->value(13).toString())+"',"+
	"`FRI`='"+RDEscapeString(q1->value(14).toString())+"',"+
	"`SAT`='"+RDEscapeString(q1->value(15).toString())+"',"+
	"`START_DAYPART`="+start_daypart+","+
	"`END_DAYPART`="+end_daypart+","+
	"`ORIGIN_NAME`='"+RDEscapeString(q1->value(18).toString())+"',"+
	QString::asprintf("`WEIGHT`=%u,",q1->value(19).toUInt())+
	QString::asprintf("`VALIDITY`=%u,",q1->value(20).toUInt())+
	QString::asprintf("`CODING_FORMAT`=%u,",q1->value(21).toUInt())+
	QString::asprintf("`SAMPLE_RATE`=%u,",q1->value(22).toUInt())+
	QString::asprintf("`BIT_RATE`=%u,",q1->value(23).toUInt())+
	QString::asprintf("`CHANNELS`=%u,",q1->value(24).toUInt())+
	QString::asprintf("`PLAY_GAIN`=%d,",q1->value(25).toInt())+
	QString::asprintf("`START_POINT`=%d,",q1->value(26).toInt())+
	QString::asprintf("`END_POINT`=%d,",q1->value(27).toInt())+
	QString::asprintf("`FADEUP_POINT`=%d,",q1->value(28).toInt())+
	QString::asprintf("`FADEDOWN_POINT`=%d,",q1->value(29).toInt())+
	QString::asprintf("`SEGUE_START_POINT`=%d,",q1->value(30).toInt())+
	QString::asprintf("`SEGUE_END_POINT`=%d,",q1->value(31).toInt())+
	QString::asprintf("`SEGUE_GAIN`=%d,",q1->value(32).toInt())+
	QString::asprintf("`HOOK_START_POINT`=%d,",q1->value(33).toInt())+
	QString::asprintf("`HOOK_END_POINT`=%d,",q1->value(34).toInt())+
	QString::asprintf("`TALK_START_POINT`=%d,",q1->value(35).toInt())+
	QString::asprintf("`TALK_END_POINT`=%d",q1->value(36).toInt());
      q2=new QSqlQuery(sql,filter_db);
      delete q2;
      ok=RDCopy(ext_audiodir+q1->value(0).toString()+"."+RD_AUDIO_EXTENSION,
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
