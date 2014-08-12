// rdcatch_copy.cpp
//
// An RDCatch event copier.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcatch_copy.cpp,v 1.8 2010/07/29 19:32:32 cvs Exp $
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

#include <qapplication.h>

#include <rd.h>
#include <dbversion.h>
#include <rdcatch_copy.h>
#include <rdcmd_switch.h>
#include <rddb.h>

MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  bool found=false;
  QString src_hostname;
  QString dest_hostname;
  QString src_station;
  QString dest_station;
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcatch_copy",
		    RDCATCH_COPY_USAGE);
  delete cmd;

  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();

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
  if(!src_db) {
    fprintf(stderr,"rdcatch_copy: can't open source mySQL database\n");
    exit(1);
  }
  src_db->setDatabaseName(rd_config->mysqlDbname());
  src_db->setUserName(rd_config->mysqlUsername());
  src_db->setPassword(rd_config->mysqlPassword());
  src_db->setHostName(src_hostname);
  if(!src_db->open()) {
    fprintf(stderr,"rdcatch_copy: unable to connect to source mySQL server\n");
    src_db->removeDatabase(rd_config->mysqlDbname());
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
    if(!dest_db) {
      fprintf(stderr,"rdcatch_copy: can't open destination mySQL database\n");
      exit(1);
    }
    dest_db->setDatabaseName(rd_config->mysqlDbname());
    dest_db->setUserName(rd_config->mysqlUsername());
    dest_db->setPassword(rd_config->mysqlPassword());
    dest_db->setHostName(dest_hostname);
    if(!dest_db->open()) {
      fprintf(stderr,
	      "rdcatch_copy: unable to connect to destination mySQL server\n");
      dest_db->removeDatabase(rd_config->mysqlDbname());
      exit(256);
    }
  }

  //
  // Check Database Versions
  //
  sql="select DB from VERSION";
  q=new RDSqlQuery(sql,src_db);
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

  q=new RDSqlQuery(sql,dest_db);
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
  sql=QString().sprintf("select NAME from STATIONS where NAME=\"%s\"",
			(const char *)src_station);
  q=new RDSqlQuery(sql,src_db);
  if(!q->first()) {
    fprintf(stderr,
	    "rdcatch_copy: source Rivendell host doesn't exist\n");
    exit(256);
  }
  delete q;

  sql=QString().sprintf("select NAME from STATIONS where NAME=\"%s\"",
			(const char *)dest_station);
  q=new RDSqlQuery(sql,dest_db);
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
  sql=QString().sprintf("delete from RECORDINGS where STATION_NAME\"%s\"",
			(const char *)dest_station);
  q=new RDSqlQuery(sql,dest_db);
  delete q;

  //
  // Copy Entries
  //
  sql=QString().sprintf("select IS_ACTIVE,TYPE,CHANNEL,CUT_NAME,SUN,MON,TUE,\
                         WED,THU,FRI,SAT,DESCRIPTION,START_TYPE,START_TIME,\
                         START_LENGTH,START_MATRIX,START_LINE,START_OFFSET,\
                         END_TYPE,END_TIME,END_LENGTH,END_MATRIX,END_LINE,\
                         LENGTH,TRIM_THRESHOLD,NORMALIZE_LEVEL,\
                         STARTDATE_OFFSET,ENDDATE_OFFSET,FORMAT,CHANNELS,\
                         SAMPRATE,BITRATE,QUALITY,MACRO_CART,SWITCH_INPUT,\
                         SWITCH_OUTPUT,EXIT_CODE,ONE_SHOT,URL,URL_USERNAME,\
                         URL_PASSWORD from RECORDINGS\
                         where STATION_NAME=\"%s\"",
			(const char *)src_station);
  q=new RDSqlQuery(sql,src_db);
  while(q->next()) {
    sql=QString().sprintf("insert into RECORDINGS set IS_ACTIVE=\"%s\",\
                           TYPE=%d,CHANNEL=%u,CUT_NAME=\"%s\",SUN=\"%s\",\
                           MON=\"%s\",TUE=\"%s\",WED=\"%s\",THU=\"%s\",\
                           FRI=\"%s\",SAT=\"%s\",DESCRIPTION=\"%s\",\
                           START_TYPE=%d,START_TIME=\"%s\",START_LENGTH=%d,\
                           START_MATRIX=%d,START_LINE=%d,START_OFFSET=%d,\
                           END_TYPE=%d,END_TIME=\"%s\",END_LENGTH=%d,\
                           END_MATRIX=%d,END_LINE=%d,LENGTH=%u,\
                           TRIM_THRESHOLD=%d,NORMALIZE_LEVEL=%d,\
                           STARTDATE_OFFSET=%u,ENDDATE_OFFSET=%u,FORMAT=%d,\
                           CHANNELS=%d,SAMPRATE=%d,BITRATE=%d,QUALITY=%d,\
                           MACRO_CART=%d,SWITCH_INPUT=%d,SWITCH_OUTPUT=%d,\
                           EXIT_CODE=%d,ONE_SHOT=\"%s\",URL=\"%s\",\
                           URL_USERNAME=\"%s\",URL_PASSWORD=\"%s\",\
                           STATION_NAME=\"%s\"",
			  (const char *)q->value(0).toString(),

			  q->value(1).toInt(),q->value(2).toUInt(),
			  (const char *)q->value(3).toString(),
			  (const char *)q->value(4).toString(),

			  (const char *)q->value(5).toString(),
			  (const char *)q->value(6).toString(),
			  (const char *)q->value(7).toString(),
			  (const char *)q->value(8).toString(),
			  
			  (const char *)q->value(9).toString(),
			  (const char *)q->value(10).toString(),
			  (const char *)q->value(11).toString(),

			  q->value(12).toInt(),
			  (const char *)q->value(13).toString(),
			  q->value(14).toInt(),

			  q->value(15).toInt(),
			  q->value(16).toInt(),
			  q->value(17).toInt(),

			  q->value(18).toInt(),
			  (const char *)q->value(19).toString(),
			  q->value(20).toInt(),

			  q->value(21).toInt(),
			  q->value(22).toInt(),
			  q->value(23).toUInt(),

			  q->value(24).toInt(),
			  q->value(25).toInt(),

			  q->value(26).toUInt(),
			  q->value(27).toUInt(),
			  q->value(28).toInt(),

			  q->value(29).toInt(),
			  q->value(30).toInt(),
			  q->value(31).toInt(),
			  q->value(32).toInt(),

			  q->value(33).toInt(),
			  q->value(34).toInt(),
			  q->value(35).toInt(),

			  q->value(36).toInt(),
			  (const char *)q->value(37).toString(),
			  (const char *)q->value(38).toString(),

			  (const char *)q->value(39).toString(),
			  (const char *)q->value(40).toString(),
			  (const char *)dest_station);
    q1=new RDSqlQuery(sql,dest_db);
    delete q1;
  }
  delete q;

  printf("done.\n");
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
