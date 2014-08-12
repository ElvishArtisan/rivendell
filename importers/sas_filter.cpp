// sas_filter.cpp
//
// An RDCatch event import filter for the SAS64000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas_filter.cpp,v 1.11 2011/06/21 22:20:43 cvs Exp $
//      $Date: 2011/06/21 22:20:43 $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.//
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
#include <rd.h>
#include <rdcmd_switch.h>
#include <dbversion.h>

#include <sas_filter.h>

//
// Global Variables
//


MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  bool skip_db_check=false;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"sas_filter",SAS_FILTER_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }
  delete cmd;

  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();
  filter_switch_count=0;
  filter_macro_count=0;

  //
  // Open Database
  //
  QString err(tr("sas_filter: "));
  filter_db=RDInitDb(&schema,&err);
  if(!filter_db) {
    fprintf(stderr,"%s\n",err.ascii());
    exit(1);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "sas_filter: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }

  //
  // RIPCD Connection
  //
  filter_ripc=new RDRipc("");
  filter_ripc->connectHost("localhost",RIPCD_TCP_PORT,rd_config->password());

  //
  // Station Configuration
  //
  filter_rdstation=new RDStation(rd_config->stationName());

  //
  // RDCatchd Connection
  //
  filter_connect=new RDCatchConnect(0,this,"filter_connect");
  filter_connect->connectHost("localhost",RDCATCHD_TCP_PORT,
			     rd_config->password());

  //
  // Read Switches
  //
  if((qApp->argc()==2)&&(!strcmp(qApp->argv()[1],"-d"))) {   // Delete List
    DeleteList();
    filter_connect->reset();
    exit(0);
  }
  if((qApp->argc()==3)&&(!strcmp(qApp->argv()[1],"-i"))) {   // Insert List
    InsertList();
    filter_connect->reset();
    exit(0);
  }
  fprintf(stderr,"\nsas_filter %s\n",SAS_FILTER_USAGE);
  exit(1);
}


void MainObject::InsertList()
{
  char line[256];
  int count=0;

  FILE *fh=fopen(qApp->argv()[2],"r");
  if(fh==NULL) {
    perror("sas_filter");
    exit(1);
  }
  printf("Importing events from %s...",qApp->argv()[2]);
  fflush(0);
  while(fgets(line,256,fh)!=NULL) {
    if(strlen(line)==79) {
      InjectLine(line);
      count++;
    }
  }
  printf("done.\n");
  fclose(fh);
  printf("Imported %d switch events, %d macro events, %d total.\n",
	 filter_switch_count,filter_macro_count,
	 filter_switch_count+filter_macro_count);
}


void MainObject::DeleteList()
{
  QString sql="delete from RECORDINGS";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  filter_connect->reset();
  printf("RDCatch events list deleted!\n");
}


void MainObject::InjectLine(char *line)
{
  QString temp;
  int input=0;
  int output=0;
  int gpo=0;

  //
  // Initialize the SQL clause
  //
  QString base_sql=QString().sprintf("insert into RECORDINGS set\
                                      STATION_NAME=\"%s\",CHANNEL=%d,",
				     (const char *)rd_config->sasStation(),
				     rd_config->sasMatrix());

  //
  // Day of the week fields
  //
  if(line[0]=='X') {
    base_sql+="MON=\"Y\",";
  }
  if(line[1]=='X') {
    base_sql+="TUE=\"Y\",";
  }
  if(line[2]=='X') {
    base_sql+="WED=\"Y\",";
  }
  if(line[3]=='X') {
    base_sql+="THU=\"Y\",";
  }
  if(line[4]=='X') {
    base_sql+="FRI=\"Y\",";
  }
  if(line[5]=='X') {
    base_sql+="SAT=\"Y\",";
  }
  if(line[6]=='X') {
    base_sql+="SUN=\"Y\",";
  }

  //
  // Time
  //
  line[17]=0;
  base_sql+=QString().sprintf("START_TIME=\"%s\",",line+9);

  //
  // Title
  //
  line[60]=0;
  temp=QString(line+19).stripWhiteSpace();
  base_sql+=QString().sprintf("DESCRIPTION=\"%s\",",(const char *)temp);

  //
  // Active Flag
  //
  if(line[77]=='I') {
    base_sql+="IS_ACTIVE=\"N\",";
  }

  //
  // Output
  //
  line[65]=0;
  sscanf(line+62,"%d",&output);

  //
  // Input
  //
  line[70]=0;
  sscanf(line+67,"%d",&input);

  //
  // GPO
  //
  line[75]=0;
  sscanf(line+73,"%d",&gpo);

  if((input>0)&&(output>0)) {
    InjectSwitchEvent(base_sql,input,output);
  }
  if(gpo>0) {
    InjectCartEvent(base_sql,gpo);
  }
}


void MainObject::InjectSwitchEvent(QString sql,int input,int output)
{
  //
  // Event Type
  //
  sql+="TYPE=2,";

  //
  // Input and Output
  //
  sql+=QString().sprintf("SWITCH_INPUT=%d,SWITCH_OUTPUT=%d",input,output);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  filter_switch_count++;
}


void MainObject::InjectCartEvent(QString sql,int gpo)
{
  //
  // Event Type
  //
  sql+="TYPE=1,";

  //
  // Macro Cart
  //
  sql+=QString().sprintf("MACRO_CART=%d",gpo+rd_config->sasBaseCart());
  filter_macro_count++;
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
//  printf("SQL: %s\n",(const char *)sql);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
