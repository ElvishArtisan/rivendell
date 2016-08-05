// panel_copy.cpp
//
// An RDCatch event copier.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddb.h>
#include <rdescape_string.h>
#include <rd.h>
#include <dbversion.h>
#include <panel_copy.h>
#include <rdcmd_switch.h>

MainObject::MainObject(QObject *parent)
  : QObject(parent)
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
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"panel_copy",
		    PANEL_COPY_USAGE);
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
	fprintf(stderr,"panel_copy: invalid argument\n");
	exit(256);
      }
      src_hostname=qApp->argv()[i+1];
      found=true;
    }
    if(QString(qApp->argv()[i])=="-H") {  // Source mySQL Hostname
      if((i+1)==qApp->argc()) {
	fprintf(stderr,"panel_copy: invalid argument\n");
	exit(256);
      }
      dest_hostname=qApp->argv()[i+1];
      found=true;
    }
    if(!found) {
      fprintf(stderr,"panel_copy: invalid argument\n");
      exit(256);
    }
  }

  if(src_hostname.isEmpty()) {
    fprintf(stderr,"panel_copy: invalid source mySQL hostname\n");
    exit(256);
  }
  if(dest_hostname.isEmpty()) {
    fprintf(stderr,"panel_copy: invalid destination mySQL hostname\n");
    exit(256);
  }
  if(src_hostname==dest_hostname) {
    fprintf(stderr,"panel_copy: cannot copy a database onto itself\n");
    exit(256);
  }

  //
  // Open Databases
  //
  src_db=QSqlDatabase::addDatabase("QMYSQL3","SRCDB");
  if(!src_db) {
    fprintf(stderr,"panel_copy: can't open source mySQL database\n");
    exit(1);
  }
  src_db->setDatabaseName(rd_config->mysqlDbname());
  src_db->setUserName(rd_config->mysqlUsername());
  src_db->setPassword(rd_config->mysqlPassword());
  src_db->setHostName(src_hostname);
  if(!src_db->open()) {
    fprintf(stderr,"panel_copy: unable to connect to source mySQL server\n");
    src_db->removeDatabase(rd_config->mysqlDbname());
    exit(256);
  }

  dest_db=QSqlDatabase::addDatabase("QMYSQL3","DESTDB");
  if(!dest_db) {
    fprintf(stderr,"panel_copy: can't open destination mySQL database\n");
    exit(1);
  }
  dest_db->setDatabaseName(rd_config->mysqlDbname());
  dest_db->setUserName(rd_config->mysqlUsername());
  dest_db->setPassword(rd_config->mysqlPassword());
  dest_db->setHostName(dest_hostname);
  if(!dest_db->open()) {
    fprintf(stderr,
	    "panel_copy: unable to connect to destination mySQL server\n");
    dest_db->removeDatabase(rd_config->mysqlDbname());
    exit(256);
  }

  //
  // Check Database Versions
  //
  sql="select DB from VERSION";
  q=new RDSqlQuery(sql,src_db);
  if(!q->first()) {
    fprintf(stderr,
	    "panel_copy: unable to read source database version\n");
    exit(256);
  }
  if(q->value(0).toInt()!=RD_VERSION_DATABASE) {
    fprintf(stderr,"panel_copy: source database version mismatch\n");
    exit(256);
  }
  delete q;

  q=new RDSqlQuery(sql,dest_db);
  if(!q->first()) {
    fprintf(stderr,
	    "panel_copy: unable to read destination database version\n");
    exit(256);
  }
  if(q->value(0).toInt()!=RD_VERSION_DATABASE) {
    fprintf(stderr,"panel_copy: destination database version mismatch\n");
    exit(256);
  }
  delete q;

  //
  // Confirmation Prompt
  //
  printf("\n");
  printf("****** WARNING ******\n");
  printf(" This operation will OVERWRITE ALL SOUNDPANEL BUTTON ASSIGNMENTS on the destination database!");
  printf(" Press RETURN to continue, or CNTL-C to abort.");
  printf("\n");
  while(getchar()!=10);
  printf("Copying button assignments...");
  fflush(stdout);

  //
  // Delete current destination entries
  //
  sql="delete from PANELS";
  q=new RDSqlQuery(sql,dest_db);
  delete q;

  //
  // Copy Entries
  //
  sql=QString("select ")+
    "TYPE,"+           // 00
    "OWNER,"+          // 01
    "PANEL_NO,"+       // 02
    "ROW_NO,"+         // 03
    "COLUMN_NO,"+      // 04
    "LABEL,"+          // 05
    "CART,"+           // 06
    "DEFAULT_COLOR "+  // 07
    "from PANELS";
  q=new RDSqlQuery(sql,src_db);
  while(q->next()) {
    sql=QString("insert into PANELS set ")+
      QString().sprintf("TYPE=%d,",q->value(0).toInt())+
      "OWNER=\""+RDEscapeString(q->value(1).toString())+"\","+
      QString().sprintf("PANEL_NO=%d,",q->value(2).toInt())+
      QString().sprintf("ROW_NO=%d,",q->value(3).toInt())+
      QString().sprintf("COLUMN_NO=%d,",q->value(4).toInt())+
      "LABEL=\""+RDEscapeString(q->value(5).toString())+"\","+
      QString().sprintf("CART=%d,",q->value(6).toInt())+
      "DEFAULT_COLOR=\""+RDEscapeString(q->value(7).toString())+"\"";
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
  new MainObject(NULL);
  return a.exec();
}
