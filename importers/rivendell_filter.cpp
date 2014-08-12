// rivendell_filter.cpp
//
// A Library import filter for an external Rivendell system
//
//   (C) Copyright 2002-2005,2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rivendell_filter.cpp,v 1.3 2010/07/29 19:32:32 cvs Exp $
//      $Date: 2010/07/29 19:32:32 $
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


MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
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
  sql=QString().sprintf("select NAME from GROUPS where NAME=\"%s\"",
			(const char *)default_group);
  q=new QSqlQuery(sql,filter_db);
  if(!q->next()) {
    fprintf(stderr,"rivendell_filter: default group does not exist\n");
    exit(256);
  }
  delete q;

  //
  // Transfer Loop
  //
  sql=QString().sprintf("select NUMBER,TYPE,GROUP_NAME,TITLE,ARTIST,ALBUM,\
                         YEAR,ISRC,LABEL,CLIENT,AGENCY,PUBLISHER,COMPOSER,\
                         USER_DEFINED,USAGE_CODE,FORCED_LENGTH,AVERAGE_LENGTH,\
                         LENGTH_DEVIATION,AVERAGE_SEGUE_LENGTH,\
                         AVERAGE_HOOK_LENGTH,CUT_QUANTITY,LAST_CUT_PLAYED,\
                         PLAY_ORDER,VALIDITY,\
                         ENFORCE_LENGTH,PRESERVE_PITCH,ASYNCRONOUS,\
                         OWNER,MACROS,SCHED_CODES from CART \
                         where (NUMBER>=%u)&&(NUMBER<=%u)",
			start_cartnum,end_cartnum);
  q=new QSqlQuery(sql,ext_db);
  while(q->next()) {
    printf("Transferring cart %06u [%s]...",q->value(0).toUInt(),
	   (const char *)q->value(3).toString());
    fflush(stdout);

    //
    // Validate Group
    //
    sql=QString().sprintf("select NAME from GROUPS where NAME=\"%s\"",
			  (const char *)RDEscapeString(q->value(2).toString()));
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
    sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%u",
			  q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    while(q1->next()) {
      unlink(RDCut::pathName(q1->value(0).toString()));
    }
    delete q1;
    sql=QString().sprintf("delete from CUTS where CART_NUMBER=%u",
			  q->value(0).toUInt());
    q1=new QSqlQuery(sql,filter_db);
    delete q1;
    sql=QString().sprintf("delete from CART where NUMBER=%u",
			  q->value(0).toUInt());
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
    sql=QString().sprintf("insert into CART set NUMBER=%u,\
                           TYPE=%u,\
                           GROUP_NAME=\"%s\",\
                           TITLE=\"%s\",\
                           ARTIST=\"%s\",\
                           ALBUM=\"%s\",\
                           YEAR=\"%s\",\
                           ISRC=\"%s\",\
                           LABEL=\"%s\",\
                           CLIENT=\"%s\",\
                           AGENCY=\"%s\",\
                           PUBLISHER=\"%s\",\
                           COMPOSER=\"%s\",\
                           USER_DEFINED=\"%s\",\
                           USAGE_CODE=\"%s\",\
                           FORCED_LENGTH=%u,\
                           AVERAGE_LENGTH=%u,\
                           LENGTH_DEVIATION=%u,\
                           AVERAGE_SEGUE_LENGTH=%u,\
                           AVERAGE_HOOK_LENGTH=%u,\
                           CUT_QUANTITY=%u,\
                           LAST_CUT_PLAYED=%u,\
                           PLAY_ORDER=%u,\
                           VALIDITY=%u,\
                           ENFORCE_LENGTH=\"%s\",\
                           PRESERVE_PITCH=\"%s\",\
                           ASYNCRONOUS=\"%s\",\
                           OWNER=%s,\
                           MACROS=\"%s\",\
                           SCHED_CODES=\"%s\"",
			  q->value(0).toUInt(),
			  q->value(1).toUInt(),
			  (const char *)RDEscapeString(group),
			  (const char *)RDEscapeString(q->value(3).toString()),
			  (const char *)RDEscapeString(q->value(4).toString()),
			  (const char *)RDEscapeString(q->value(5).toString()),
			  (const char *)q->value(6).toDate().
			  toString("yyyy-MM-dd"),
			  (const char *)RDEscapeString(q->value(7).toString()),
			  (const char *)RDEscapeString(q->value(8).toString()),
			  (const char *)RDEscapeString(q->value(9).toString()),
			  (const char *)RDEscapeString(q->value(10).toString()),
			  (const char *)RDEscapeString(q->value(11).toString()),
			  (const char *)RDEscapeString(q->value(12).toString()),
			  (const char *)RDEscapeString(q->value(13).toString()),
			  (const char *)RDEscapeString(q->value(14).toString()),
			  q->value(15).toUInt(),
			  q->value(16).toUInt(),
			  q->value(17).toUInt(),
			  q->value(18).toUInt(),
			  q->value(19).toUInt(),
			  q->value(20).toUInt(),
			  q->value(21).toUInt(),
			  q->value(22).toUInt(),
			  q->value(23).toUInt(),
			  (const char *)RDEscapeString(q->value(24).toString()),
			  (const char *)RDEscapeString(q->value(25).toString()),
			  (const char *)RDEscapeString(q->value(26).toString()),
			  (const char *)owner,
			  (const char *)RDEscapeString(q->value(28).toString()),
			  (const char *)RDEscapeString(q->value(29).
						       toString()));
    q1=new QSqlQuery(sql,filter_db);
    delete q1;
    sql=QString().sprintf("select CUT_NAME,EVERGREEN,DESCRIPTION,OUTCUE,ISRC,\
                           LENGTH,ORIGIN_DATETIME,START_DATETIME,END_DATETIME,\
                           SUN,MON,TUE,WED,THU,FRI,SAT,START_DAYPART,\
                           END_DAYPART,ORIGIN_NAME,WEIGHT,VALIDITY,\
                           CODING_FORMAT,SAMPLE_RATE,BIT_RATE,CHANNELS,\
                           PLAY_GAIN,START_POINT,END_POINT,FADEUP_POINT,\
                           FADEDOWN_POINT,SEGUE_START_POINT,SEGUE_END_POINT,\
                           SEGUE_GAIN,HOOK_START_POINT,HOOK_END_POINT,\
                           TALK_START_POINT,TALK_END_POINT from CUTS \
                           where CART_NUMBER=%u",q->value(0).toUInt());
    q1=new QSqlQuery(sql,ext_db);
    while(q1->next()) {
      if(q1->value(7).isNull()) {
	start_datetime="null";
      }
      else {
	start_datetime=QString().sprintf("\"%s\"",
			(const char *)q1->value(7).
			toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
      }
      if(q1->value(8).isNull()) {
	end_datetime="null";
      }
      else {
	end_datetime=QString().sprintf("\"%s\"",
			(const char *)q1->value(8).
			toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
      }
      if(q1->value(16).isNull()) {
	start_daypart="null";
      }
      else {
	start_daypart=QString().sprintf("\"%s\"",
					(const char *)q1->value(16).
					toTime().toString("hh:mm:ss"));
      }
      if(q1->value(17).isNull()) {
	end_daypart="null";
      }
      else {
	end_daypart=QString().sprintf("\"%s\"",
					(const char *)q1->value(17).
					toTime().toString("hh:mm:ss"));
      }
      sql=QString().sprintf("insert into CUTS set CART_NUMBER=%u,\
                             CUT_NAME=\"%s\",\
                             EVERGREEN=\"%s\",\
                             DESCRIPTION=\"%s\",\
                             OUTCUE=\"%s\",\
                             ISRC=\"%s\",\
                             LENGTH=%u,\
                             ORIGIN_DATETIME=\"%s\",\
                             START_DATETIME=%s,\
                             END_DATETIME=%s,\
                             SUN=\"%s\",\
                             MON=\"%s\",\
                             TUE=\"%s\",\
                             WED=\"%s\",\
                             THU=\"%s\",\
                             FRI=\"%s\",\
                             SAT=\"%s\",\
                             START_DAYPART=%s,\
                             END_DAYPART=%s,\
                             ORIGIN_NAME=\"%s\",\
                             WEIGHT=%u,\
                             VALIDITY=%u,\
                             CODING_FORMAT=%u,\
                             SAMPLE_RATE=%u,\
                             BIT_RATE=%u,\
                             CHANNELS=%u,\
                             PLAY_GAIN=%d,\
                             START_POINT=%d,\
                             END_POINT=%d,\
                             FADEUP_POINT=%d,\
                             FADEDOWN_POINT=%d,\
                             SEGUE_START_POINT=%d,\
                             SEGUE_END_POINT=%d,\
                             SEGUE_GAIN=%d,\
                             HOOK_START_POINT=%d,\
                             HOOK_END_POINT=%d,\
                             TALK_START_POINT=%d,\
                             TALK_END_POINT=%d",
			    q->value(0).toUInt(),
			    (const char *)RDEscapeString(q1->value(0).
							 toString()),
			    (const char *)RDEscapeString(q1->value(1).
							 toString()),
			    (const char *)RDEscapeString(q1->value(2).
							 toString()),
			    (const char *)RDEscapeString(q1->value(3).
							 toString()),
			    (const char *)RDEscapeString(q1->value(4).
							 toString()),
			    q1->value(5).toUInt(),
			    (const char *)RDEscapeString(q1->value(6).
							 toString()),
			    (const char *)start_datetime,
			    (const char *)end_datetime,
			    (const char *)RDEscapeString(q1->value(9).
							 toString()),
			    (const char *)RDEscapeString(q1->value(10).
							 toString()),
			    (const char *)RDEscapeString(q1->value(11).
							 toString()),
			    (const char *)RDEscapeString(q1->value(12).
							 toString()),
			    (const char *)RDEscapeString(q1->value(13).
							 toString()),
			    (const char *)RDEscapeString(q1->value(14).
							 toString()),
			    (const char *)RDEscapeString(q1->value(15).
							 toString()),
			    (const char *)start_daypart,
			    (const char *)end_daypart,
			    (const char *)RDEscapeString(q1->value(18).
							 toString()),
			    q1->value(19).toUInt(),
			    q1->value(20).toUInt(),
			    q1->value(21).toUInt(),
			    q1->value(22).toUInt(),
			    q1->value(23).toUInt(),
			    q1->value(24).toUInt(),
			    q1->value(25).toInt(),
			    q1->value(26).toInt(),
			    q1->value(27).toInt(),
			    q1->value(28).toInt(),
			    q1->value(29).toInt(),
			    q1->value(30).toInt(),
			    q1->value(31).toInt(),
			    q1->value(32).toInt(),
			    q1->value(33).toInt(),
			    q1->value(34).toInt(),
			    q1->value(35).toInt(),
			    q1->value(36).toInt());
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
  new MainObject(NULL,"main");
  return a.exec();
}
