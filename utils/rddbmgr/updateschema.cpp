// updateschema.cpp
//
// Update Rivendell DB schema.
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdatetime.h>

#include <rdcart.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdfeed.h>
#include <rdpaths.h>

#include "rddbmgr.h"

bool MainObject::UpdateSchema(int cur_schema,int set_schema,QString *err_msg)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  QString tablename;
  RDCart *cart;
  bool length_update_required=false;

  if((cur_schema<3)&&(set_schema>=3)) {
    //
    // Create RDAIRPLAY Table
    //
    sql=QString("create table if not exists RDAIRPLAY (")+
      "ID int not null primary key,"+
      "STATION char(40) not null,"+
      "INSTANCE int unsigned not null,"+
      "AUTO_CARD0 int default -1,"+
      "AUTO_STREAM0 int default -1,"+
      "AUTO_PORT0 int default -1,"+
      "AUTO_CARD1 int default -1,"+
      "AUTO_STREAM1 int default -1,"+
      "AUTO_PORT1 int default -1,"+
      "PANEL_CARD0 int default -1,"+
      "PANEL_STREAM0 int default -1,"+
      "PANEL_PORT0 int default -1,"+
      "PANEL_CARD1 int default -1,"+
      "PANEL_STREAM1 int default -1,"+
      "PANEL_PORT1 int default -1,"+
      "index STATION_IDX (STATION,INSTANCE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create Default RDAirPlay Configuration
    //
    sql=QString("insert into RDAIRPLAY (STATION,INSTANCE) ")+
      "values (\"DEFAULT\",0)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema=3;
    WriteSchemaVersion(cur_schema);
  }

  if((cur_schema<4)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY modify ID int not null auto_increment");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<5)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add SOURCE int not null after COUNT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<6)&&(set_schema>cur_schema)) {
    // 
    // Update RDAIRPLAY Structure
    //
    sql=QString("alter table RDAIRPLAY add column CARD int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column PORT0 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column PORT1 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column PORT2 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM0 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM1 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM2 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM3 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM4 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM5 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM6 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column STREAM7 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    for(int i=0;i<2;i++) {
      DropColumn("RDAIRPLAY",QString().sprintf("AUTO_CARD%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("AUTO_STREAM%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("AUTO_PORT%d",i)); 
      DropColumn("RDAIRPLAY",QString().sprintf("PANEL_CARD%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("PANEL_STREAM%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("PANEL_PORT%d",i));
    }
    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<7)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column SEGUE_LENGTH int default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<8)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column PORT3 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column OP_MODE int default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column START_MODE int default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column PIE_COUNT_LENGTH int default 15000");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column PIE_COUNT_ENDPOINT int default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<9)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column PORT4 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY add column PORT5 int default -1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<10)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+
	q->value(0).toString()+"_LOG` add TYPE int default 0 after COUNT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+
	q->value(0).toString()+"_LOG` add COMMENT char(255)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+
	q->value(0).toString()+"_LOG` add LABEL char(10)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+
	q->value(0).toString()+"_LOG` add POST_TIME time default '24:00:00'";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+
	q->value(0).toString()+"_LOG` add index LABEL_IDX (LABEL)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }


  if((cur_schema<11)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column CHECK_TIMESYNC enum('N','Y') default 'N'",false);
    delete q;    

  //
  // Create PANELS Table
  //
    sql=QString("create table if not exists PANELS (")+
      "ID int not null primary key,"+
      "TYPE int not null,"+
      "OWNER char(64) not null,"+
      "PANEL_NO int not null,"+
      "ROW_NO int not null,"+
      "COLUMN_NO int not null,"+
      "LABEL char(64),"+
      "CART int,"+
      "DEFAULT_COLOR char(6),"+
      "index OWNER_IDX (OWNER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<12)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column STATION_PANELS int default 3",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column USER_PANELS int default 3",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<13)&&(set_schema>cur_schema)) {
    if(!DropTable("PANELS",err_msg)) {
      return false;
    }

    sql=QString("create table if not exists PANELS (")+
      "ID int auto_increment not null primary key,"+
      "TYPE int not null,"+
      "OWNER char(64) not null,"+
      "PANEL_NO int not null,"+
      "ROW_NO int not null,"+
      "COLUMN_NO int not null,"+
      "LABEL char(64),"+
      "CART int,"+
      "DEFAULT_COLOR char(6),"+
      "index OWNER_IDX (OWNER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<14)&&(set_schema>cur_schema)) {
    if(!DropTable("PANELS",err_msg)) {
      return false;
    }

    sql=QString("create table if not exists PANELS (")+
      "ID int auto_increment not null primary key,"+
      "TYPE int not null,"+
      "OWNER char(64) not null,"+
      "PANEL_NO int not null,"+
      "ROW_NO int not null,"+
      "COLUMN_NO int not null,"+
      "LABEL char(64),"+
      "CART int,"+
      "DEFAULT_COLOR char(6),"+
      "index LOAD_IDX (TYPE,OWNER,PANEL_NO),"+
      "index SAVE_IDX (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<15)&&(set_schema>cur_schema)) {
    DropColumn("PANELS","DEFAULT_COLOR");
    q=new RDSqlQuery("alter table PANELS add column DEFAULT_COLOR char(7)",
		     false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<16)&&(set_schema>cur_schema)) {
    //
    // Create MATRICES Table
    //
    sql=QString("create table if not exists MATRICES (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "NAME char(64),"+
      "MATRIX int not null,"+
      "TYPE int not null,"+
      "PORT int not null,"+
      "GPIO_DEVICE char(255),"+
      "INPUTS int not null,"+
      "OUTPUTS int not null,"+
      "GPIS int not null,"+
      "GPOS int not null,"+
      "index MATRIX_IDX (STATION_NAME,MATRIX))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    
    //
    // Create INPUTS Table
    //
    sql=QString("create table if not exists INPUTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "NAME char(64),"+
      "FEED_NAME char(8),"+
      "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    
    //
    // Create OUTPUTS Table
    //
    sql=QString("create table if not exists OUTPUTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "NAME char(64),"+
      "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<17)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table INPUTS add column CHANNEL_MODE int",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<18)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table STATIONS add column IPV4_ADDRESS char(15)",
		     false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<19)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists EVENTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "SUN enum('N','Y') not null,"+
      "MON enum('N','Y') not null,"+
      "TUE enum('N','Y') not null,"+
      "WED enum('N','Y') not null,"+
      "THU enum('N','Y') not null,"+
      "FRI enum('N','Y') not null,"+
      "SAT enum('N','Y') not null,"+
      "TIME time not null,"+
      "DESCRIPTION char(64),"+
      "COMMAND char(255),"+
      "index STATION_IDX (STATION_NAME),"+
      "index SUN_IDX (STATION_NAME,SUN),"+
      "index MON_IDX (STATION_NAME,MON),"+
      "index TUE_IDX (STATION_NAME,TUE),"+
      "index WED_IDX (STATION_NAME,WED),"+
      "index THU_IDX (STATION_NAME,THU),"+
      "index FRI_IDX (STATION_NAME,FRI),"+
      "index SAT_IDX (STATION_NAME,SAT))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<20)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table CART add column MACROS text",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<21)&&(set_schema>cur_schema)) {
    q=new 
      RDSqlQuery("alter table RECORDINGS add column MACRO_CART int default -1",
		 false);
    delete q;
    if(!DropTable("EVENTS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<22)&&(set_schema>cur_schema)) {
    DropColumn("DECKS","SWITCH_TYPE");
    DropColumn("DECKS","TTY_ID");
    q=new RDSqlQuery("alter table DECKS add column SWITCH_STATION char(64)",
		     false);
    delete q;
    q=new
      RDSqlQuery("alter table DECKS add column SWITCH_MATRIX int default -1",
		 false);
    delete q;
    q=new
      RDSqlQuery("alter table DECKS add column SWITCH_OUTPUT int default -1",
		 false);
    delete q;
    q=new
      RDSqlQuery("alter table DECKS add column SWITCH_DELAY int default 0",
		 false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<23)&&(set_schema>cur_schema)) {
    DropColumn("RECORDINGS","SOURCE_NAME");
    q=new 
      RDSqlQuery("alter table RECORDINGS add column SWITCH_INPUT int default -1",false);
    delete q;
    if(!DropTable("SOURCES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<24)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RECORDINGS add column TYPE int default 0 after STATION_NAME",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS add column SWITCH_OUTPUT int default -1",false);
    delete q;
    q=new RDSqlQuery("update RECORDINGS set TYPE=1 where MACRO_CART!=-1",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<25)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RECORDINGS drop index SUN_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index MON_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index TUE_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index WED_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index THU_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index FRI_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index SAT_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS add column IS_ACTIVE enum('N','Y') default 'Y' after ID",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS add index SUN_IDX (STATION_NAME,SUN,IS_ACTIVE)",false);
    q=new RDSqlQuery("alter table RECORDINGS add index MON_IDX (STATION_NAME,MON,IS_ACTIVE)",false);
    q=new RDSqlQuery("alter table RECORDINGS add index TUE_IDX (STATION_NAME,TUE,IS_ACTIVE)",false);
    q=new RDSqlQuery("alter table RECORDINGS add index WED_IDX (STATION_NAME,WED,IS_ACTIVE)",false);
    q=new RDSqlQuery("alter table RECORDINGS add index THU_IDX (STATION_NAME,THU,IS_ACTIVE)",false);
    q=new RDSqlQuery("alter table RECORDINGS add index FRI_IDX (STATION_NAME,FRI,IS_ACTIVE)",false);
    q=new RDSqlQuery("alter table RECORDINGS add index SAT_IDX (STATION_NAME,SAT,IS_ACTIVE)",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<26)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RECORDINGS drop index SUN_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index MON_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index TUE_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index WED_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index THU_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index FRI_IDX",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS drop index SAT_IDX",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<27)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists GPIS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "MACRO_CART int default -1,"+
      "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<28)&&(set_schema>cur_schema)) {
    sql="alter table CUTS alter column ORIGIN_DATETIME set default NULL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CUTS alter column START_DATETIME set default NULL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CUTS alter column END_DATETIME set default NULL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CUTS alter column START_DAYPART set default NULL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CUTS alter column END_DAYPART set default NULL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update CUTS set ORIGIN_DATETIME=NULL where ORIGIN_DATETIME=\"0000-00-00 00:00:00\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update CUTS set START_DATETIME=NULL where START_DATETIME=\"0000-00-00 00:00:00\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update CUTS set END_DATETIME=NULL where END_DATETIME=\"0000-00-00 00:00:00\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"drop index START_TIME_IDX";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"alter column START_TIME set default NULL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"modify column START_TIME int";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"alter column POST_TIME set default NULL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("update `")+q->value(0).toString()+"_LOG` "+
	"set START_TIME=NULL where START_TIME=\"00:00:00\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("update `")+q->value(0).toString()+"_LOG` "+
	"set POST_TIME=NULL where POST_TIME=\"00:00:00\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<29)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RECORDINGS add column EXIT_CODE int default 0",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<30)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RECORDINGS add column ONE_SHOT enum('N','Y') default 'N'",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<31)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table STATIONS add column TIME_OFFSET int default 0",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<32)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table GROUPS add column DEFAULT_LOW_CART int unsigned default 0",false);
    q=new RDSqlQuery("alter table GROUPS add column DEFAULT_HIGH_CART int unsigned default 0",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<33)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table CUTS add column SUN enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column MON enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column TUE enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column WED enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column THU enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column FRI enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column SAT enum('N','Y') default 'Y' after END_DAYPART",false);

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<34)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG "+
	"add GRACE_TIME int default 0 after START_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<35)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column TRANS_LENGTH int default 0 after SEGUE_LENGTH",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<36)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add POST_POINT enum('N','Y') default 'N' after TIME_TYPE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<37)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table LOGS add column TYPE int not null default 0 after NAME",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<38)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists EVENTS (")+
      "NAME char(64) not null primary key,"+
      "PROPERTIES char(64),"+
      "DISPLAY_TEXT char(64),"+
      "NOTE_TEXT char(255),"+
      "PREPOSITION int default -1,"+
      "TIME_TYPE int default 0,"+
      "GRACE_TIME int default 0,"+
      "POST_POINT enum('N','Y') default 'N',"+
      "USE_AUTOFILL enum('N','Y') default 'N',"+
      "USE_TIMESCALE enum('N','Y') default 'N',"+
      "IMPORT_SOURCE int default 0,"+
      "START_SLOP int default 0,"+
      "END_SLOP int default 0,"+
      "FIRST_TRANS_TYPE int default 0,"+
      "DEFAULT_TRANS_TYPE int default 0,"+
      "COLOR char(7))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<39)&&(set_schema>cur_schema)) {
    // Transpose RDLogLine::Stop and RDLogLine::Segue
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `")+q->value(0).toString()+"_LOG` set "+
	"TRANS_TYPE=100 where TRANS_TYPE=1";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("update `")+q->value(0).toString()+"_LOG` set "+
	"TRANS_TYPE=1 where TRANS_TYPE=2";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("update `")+q->value(0).toString()+"_LOG` set "+
	"TRANS_TYPE=2 where TRANS_TYPE=100";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<40)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists CLOCKS (")+
      "NAME char(64) not null primary key,"+
      "SHORT_NAME char(8),"+
      "COLOR char(7))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<41)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD0 int default -1 after INSTANCE",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD1 int default -1 after PORT0",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD2 int default -1 after PORT1",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD3 int default -1 after PORT2",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD4 int default -1 after PORT3",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD5 int default -1 after PORT4",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<42)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table SERVICES add column CLOCK0 char(64) after DESCRIPTION",false);
    delete q;
    for(int i=1;i<168;i++) {
      sql=QString("alter table SERVICES add column ")+
	QString().sprintf("CLOCK%d char(64) after CLOCK%d",i,i-1);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<43)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column SHOW_AUX_1 enum('N','Y') default 'Y' after USER_PANELS",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column SHOW_AUX_2 enum('N','Y') default 'Y' after SHOW_AUX_1",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<44)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table CUTS add column LOCAL_COUNTER int unsigned default 0 after PLAY_COUNTER",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<45)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table CUTS add column EVERGREEN enum('N','Y') default 'N' after CART_NUMBER",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<46)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table CART add column LENGTH_DEVIATION int unsigned default 0 after FORCED_LENGTH",false);
    delete q;
    q=new RDSqlQuery("select NUMBER from CART where TYPE=1",false);
    while(q->next()) {
      AverageCuts89(q->value(0).toUInt());
      /*
      cart=new RDCart(q->value(0).toUInt());
      cart->calculateAverageLength(&dev);
      cart->setLengthDeviation(dev);
      delete cart;
      */
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<47)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table SERVICES add column NAME_TEMPLATE char(255) after DESCRIPTION",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_PATH char(255) after NAME_TEMPLATE",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_WIN_PATH char(255) after TFC_PATH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_CART_OFFSET int after TFC_WIN_PATH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_CART_LENGTH int after TFC_CART_OFFSET",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_START_OFFSET int after TFC_CART_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_START_LENGTH int after TFC_START_OFFSET",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_PATH char(255) after TFC_START_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_WIN_PATH char(255) after MUS_PATH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_CART_OFFSET int after MUS_PATH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_CART_LENGTH int after MUS_CART_OFFSET",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_START_OFFSET int after MUS_CART_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_START_LENGTH int after MUS_START_OFFSET",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<48)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table SERVICES add column TFC_LENGTH_OFFSET int after TFC_START_LENGTH",false);
    delete q;

    q=new RDSqlQuery("alter table SERVICES add column TFC_LENGTH_LENGTH int after TFC_LENGTH_OFFSET",false);
    delete q;

    q=new RDSqlQuery("alter table SERVICES add column MUS_LENGTH_OFFSET int after MUS_START_LENGTH",false);
    delete q;

    q=new RDSqlQuery("alter table SERVICES add column MUS_LENGTH_LENGTH int after MUS_LENGTH_OFFSET",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<49)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists AUTOFILLS (")+
      "ID int not null primary key auto_increment,"+
      "SERVICE char(10),"+
      "CART_NUMBER int unsigned,"+
      "index SERVICE_IDX (SERVICE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<50)&&(set_schema>cur_schema)) {
    sql=QString("alter table SERVICES ")+
      "add column CHAIN_LOG enum('N','Y') default 'N'"+
      "after NAME_TEMPLATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<51)&&(set_schema>cur_schema)) {
    sql=QString("alter table USERS ")+
      "modify column PASSWORD char(32)"+
      "after DESCRIPTION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<52)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists HOSTVARS (")+
      "ID int not null primary key auto_increment,"+
      "STATION_NAME char(64) not null,"+
      "NAME char(32) not null,"+
      "VARVALUE char(255),"+
      "REMARK char(255),"+
      "index NAME_IDX (STATION_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<53)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table STATIONS add column BACKUP_DIR char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table STATIONS add column BACKUP_LIFE int default 0",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<54)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column CLEAR_FILTER enum(\'N\',\'Y\') default \'N\'",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column BAR_ACTION int unsigned default 0",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<55)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column FLASH_PANEL enum(\'N\',\'Y\') default \'N\'",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<56)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table STATIONS add column HEARTBEAT_CART int unsigned default 0",false);
    delete q;
    q=new RDSqlQuery("alter table STATIONS add column HEARTBEAT_INTERVAL int unsigned default 0",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<57)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists SERVICE_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64),"+
      "SERVICE_NAME char(10),"+
      "index STATION_IDX (STATION_NAME),"+
      "index SERVICE_IDX (SERVICE_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    q=new RDSqlQuery("select NAME from STATIONS",false);
    while(q->next()) {
      q1=new RDSqlQuery("select NAME from SERVICES",false);
      while(q1->next()) {
	sql=QString("insert into SERVICE_PERMS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<58)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column EXT_START_TIME time";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column EXT_LENGTH int";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table '")+q->value(0).toString()+"_LOG` "+
	"add column EXT_DATA char(32)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column EXT_EVENT_ID char(8)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<59)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table SERVICES add column TFC_DATA_OFFSET int after TFC_LENGTH_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_DATA_LENGTH int after TFC_DATA_OFFSET",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_EVENT_ID_OFFSET int after TFC_DATA_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column TFC_EVENT_ID_LENGTH int after TFC_EVENT_ID_OFFSET",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_DATA_OFFSET int after MUS_LENGTH_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_DATA_LENGTH int after MUS_DATA_OFFSET",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_EVENT_ID_OFFSET int after MUS_DATA_LENGTH",false);
    delete q;
    q=new RDSqlQuery("alter table SERVICES add column MUS_EVENT_ID_LENGTH int after MUS_EVENT_ID_OFFSET",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<60)&&(set_schema>cur_schema)) {
  //
  // Version 60 and 61 code removed, as per-log reconciliation data is no 
  // longer used.   FFG 11/08/2005
  //
    cur_schema+=2;
  }

  if((cur_schema<62)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table GROUPS add column REPORT_TFC enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add column REPORT_MUS enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add index IDX_REPORT_TFC (REPORT_TFC)",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add index IDX_REPORT_MUS (REPORT_MUS)",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<63)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists REPORTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "NAME char(64) not null unique,"+
      "DESCRIPTION char(64),"+
      "EXPORT_FILTER int,"+
      "EXPORT_PATH char(255),"+
      "WIN_EXPORT_PATH char(255),"+
      "index IDX_NAME (NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("create table if not exists REPORT_SERVICES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "REPORT_NAME char(64) not null,"+
      "SERVICE_NAME char(10),"+
      "index IDX_REPORT_NAME (REPORT_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("create table if not exists REPORT_STATIONS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "REPORT_NAME char(64) not null,"+
      "STATION_NAME char(64),"+
      "index IDX_REPORT_NAME (REPORT_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<64)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table REPORTS add column EXPORT_TFC enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table REPORTS add column EXPORT_MUS enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table REPORTS add column EXPORT_GEN enum('N','Y') default 'N'",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<65)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table REPORTS add column STATION_ID char(16)",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<66)&&(set_schema>cur_schema)) {
    q=new 
      RDSqlQuery("alter table RDAIRPLAY alter column OP_MODE set default 2",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<67)&&(set_schema>cur_schema)) {
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column PAUSE_ENABLED enum('N','Y') default 'N'",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<68)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_ADDR0 char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_PORT0 int unsigned",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_STRING0 char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_ADDR1 char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_PORT1 int unsigned",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_STRING1 char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_ADDR2 char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_PORT2 int unsigned",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column UDP_STRING2 char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add column ENABLE_NOW_NEXT enum('N','Y') default 'N'",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<69)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table MATRICES add column PORT_TYPE int default 0 after TYPE",false);
    delete q;
    q=new RDSqlQuery("alter table MATRICES add column IP_ADDRESS char(16) after PORT",false);
    delete q;
    q=new RDSqlQuery("alter table MATRICES add column IP_PORT int after IP_ADDRESS",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<70)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_REC`"+
	"add column PLAY_SOURCE int default 0 after EVENT_TYPE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_REC` "+
	"add column CUT_NUMBER int default 0 after CART_NUMBER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<71)&&(set_schema>cur_schema)) {
    sql=QString("alter table RECORDINGS add column END_LINE int default -1 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column END_MATRIX int default -1 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column END_LENGTH int default 0 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column END_TIME time after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column END_TYPE int unsigned default 2 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column START_OFFSET int default 0 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column START_LINE int default -1 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column START_MATRIX int default -1 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column START_LENGTH int default 0 after START_TIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS add column START_TYPE int default 0 after DESCRIPTION");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<72)&&(set_schema>cur_schema)) {
    sql=QString("alter table GROUPS add column DEFAULT_CART_TYPE int unsigned default 1 after DESCRIPTION");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<73)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column DEFAULT_TRANS_TYPE int unsigned default 0 after CLEAR_FILTER");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<74)&&(set_schema>cur_schema)) {
    //
    // Create CLOCK_PERMS Table
    //
    sql=QString("create table if not exists CLOCK_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "CLOCK_NAME char(64),"+
      "SERVICE_NAME char(10),"+
      "index CLOCK_IDX (CLOCK_NAME),"+
      "index SERVICE_IDX (SERVICE_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select NAME from CLOCKS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql="select NAME from SERVICES";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into CLOCK_PERMS set ")+
	  "CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    //
    // Create EVENT_PERMS Table
    //
    sql=QString("create table if not exists EVENT_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "EVENT_NAME char(64),"+
      "SERVICE_NAME char(10),"+
      "index EVENT_IDX (EVENT_NAME),"+
      "index SERVICE_IDX (SERVICE_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql="select NAME from SERVICES";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into EVENT_PERMS set ")+
	  "EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<75)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table MATRICES add column CARD int default -1 after PORT_TYPE",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<76)&&(set_schema>cur_schema)) {
    q=new RDSqlQuery("alter table DECKS add column MON_PORT_NUMBER int default -1 after PORT_NUMBER",false);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<77)&&(set_schema>cur_schema)) {
    //
    // Create USER_PERMS table
    //
    sql=QString("create table if not exists USER_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "USER_NAME char(8),"+
      "GROUP_NAME char(10),"+
      "index USER_IDX (USER_NAME),"+
      "index GROUP_IDX (GROUP_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select LOGIN_NAME from USERS");
    q=new RDSqlQuery(sql,false);
    sql=QString("select NAME from GROUPS");
    q1=new RDSqlQuery(sql,false);
    while(q->next()) {
      q1->seek(-1);
      while(q1->next()) {
	sql=QString("insert into USER_PERMS set ")+
	  "USER_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  "GROUP_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q1;
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<78)&&(set_schema>cur_schema)) {
    sql="alter table USERS add column MODIFY_TEMPLATE_PRIV enum('N','Y') not null default 'N' after ARRANGE_LOG_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update USERS set MODIFY_TEMPLATE_PRIV=\"Y\" where CREATE_LOG_PRIV=\"Y\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<79)&&(set_schema>cur_schema)) {
    sql="alter table GROUPS add column ENFORCE_CART_RANGE enum('N','Y') default 'N' after DEFAULT_HIGH_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<80)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column TFC_ANNC_TYPE_OFFSET int after TFC_EVENT_ID_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_ANNC_TYPE_LENGTH int after TFC_ANNC_TYPE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_ANNC_TYPE_OFFSET int after MUS_EVENT_ID_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_ANNC_TYPE_LENGTH int after MUS_ANNC_TYPE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPORTS add column FORCE_TFC enum('N','Y') default 'N' after EXPORT_TFC";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table REPORTS add column FORCE_MUS enum('N','Y') default 'N' after EXPORT_MUS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table GROUPS alter column REPORT_TFC set default 'Y'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table GROUPS alter column REPORT_MUS set default 'Y'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
   }

  if((cur_schema<81)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_REC` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_REC` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<82)&&(set_schema>cur_schema)) {
    sql="alter table INPUTS add column ENGINE_NUM int default -1 after CHANNEL_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table INPUTS add column DEVICE_NUM int default -1 after ENGINE_NUM";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table OUTPUTS add column ENGINE_NUM int default -1 after NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table OUTPUTS add column DEVICE_NUM int default -1 after ENGINE_NUM";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column USERNAME char(32) after IP_PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table MATRICES add column PASSWORD char(32) after USERNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists VGUEST_RESOURCES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX_NUM int not null,"+
      "VGUEST_TYPE int not null,"+
      "NUMBER int not null,"+
      "ENGINE_NUM int default -1,"+
      "DEVICE_NUM int default -1,"+
      "SURFACE_NUM int default -1,"+
      "RELAY_NUM int default -1,"+
      "BUSS_NUM int default -1,"+
      "index STATION_MATRIX_IDX (STATION_NAME,MATRIX_NUM,VGUEST_TYPE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column FADERS int default 0 after GPOS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table MATRICES add column DISPLAYS int default 0 after FADERS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<83)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS add column URL char(255) after ONE_SHOT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table RECORDINGS add column URL_USERNAME char(64) after URL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table RECORDINGS add column URL_PASSWORD char(64) after URL_USERNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<84)&&(set_schema>cur_schema)) {
    sql=
      "alter table STATIONS add column STATION_SCANNED enum('N','Y') default 'N'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column HPI_VERSION char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column JACK_VERSION char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column ALSA_VERSION char(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=
      "alter table STATIONS add column HAVE_OGGENC enum('N','Y') default 'N'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=
      "alter table STATIONS add column HAVE_OGG123 enum('N','Y') default 'N'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=
      "alter table STATIONS add column HAVE_FLAC enum('N','Y') default 'N'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=
      "alter table STATIONS add column HAVE_LAME enum('N','Y') default 'N'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=
      "alter table STATIONS add column HAVE_MPG321 enum('N','Y') default 'N'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD0_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD0_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD0_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD0_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD1_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD1_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD1_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD1_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD2_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD2_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD2_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD2_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD3_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD3_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD3_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD3_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD4_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD4_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD4_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD4_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD5_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD5_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD5_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD5_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD6_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD6_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD6_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD6_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CARD7_DRIVER int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD7_NAME char(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD7_INPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table STATIONS add column CARD7_OUTPUTS int default -1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<85)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS add column NORMALIZE_LEVEL int default -1300 after TRIM_THRESHOLD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RECORDINGS add column QUALITY int default 0 after BITRATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<86)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS alter column END_TYPE set default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<87)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_REC` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<88)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS add column ALLOW_MULT_RECS enum('N','Y') default 'N' after END_GPI";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RECORDINGS add column MAX_GPI_REC_LENGTH int unsigned default 3600000 after ALLOW_MULT_RECS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "ID,"+
      "START_TIME,"+
      "END_TIME,"+
      "END_LENGTH "+
      "from RECORDINGS where "+
      "END_TYPE=1";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString().
	sprintf("update RECORDINGS set MAX_GPI_REC_LENGTH=%u where ID=%u",
		QTime().msecsTo(q->value(2).toTime())+q->value(3).toUInt()-
		QTime().msecsTo(q->value(1).toTime()),q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<89)&&(set_schema>cur_schema)) {
    sql="alter table CART add column AVERAGE_LENGTH int unsigned after FORCED_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add column ASYNCRONOUS enum('N','Y') default 'N' after PRESERVE_PITCH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NUMBER from CART where TYPE=1");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      AverageCuts89(q->value(0).toUInt());
    }
    delete q;

    //    RDMacroEvent *macro_event;
    sql=QString("select NUMBER from CART where TYPE=2");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      TotalMacros89(q->value(0).toUInt());
      /*
      macro_event=new RDMacroEvent();
      macro_event->load(q->value(0).toUInt());
      sql=QString("update CART set ")+
	QString().sprintf("AVERAGE_LENGTH=%u,",macro_event->length())+
	QString().sprintf("FORCED_LENGTH=%u ",macro_event->length())+
	QString().sprintf("where NUMBER=%u",q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
      delete macro_event;
      */
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<90)&&(set_schema>cur_schema)) {
    sql="alter table REPORTS add column CART_DIGITS int unsigned default 6 after STATION_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPORTS add column USE_LEADING_ZEROS enum('N','Y') default 'N' after CART_DIGITS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<91)&&(set_schema>cur_schema)) {
    sql="alter table DECKS add column DEFAULT_MONITOR_ON enum('N','Y') default 'N' after MON_PORT_NUMBER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<92)&&(set_schema>cur_schema)) {
    sql="alter table EVENTS add column AUTOFILL_SLOP int default -1 after USE_AUTOFILL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<93)&&(set_schema>cur_schema)) {
    sql="alter table LOGS add column IMPORT_DATE date after END_DATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column FADEUP_POINT int default -1 after END_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_LOG` "+
	QString().sprintf("add column FADEUP_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEUP_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column FADEDOWN_POINT int default -1 after FADEUP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_LOG` "+
	QString().sprintf("add column FADEDOWN_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEDOWN_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_LOG` "+
	QString().sprintf("add column SEGUE_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after SEGUE_END_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column FADEUP_POINT int default -1 after END_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	QString().sprintf("add column FADEUP_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEUP_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column FADEDOWN_POINT int default -1 after FADEUP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table '")+tablename+"_PRE' "+
	QString().sprintf("add column FADEDOWN_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEDOWN_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	QString().sprintf("add column SEGUE_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after SEGUE_END_POINT";
	if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table '")+tablename+"_POST' "+
	"add column FADEUP_POINT int default -1 after END_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table '")+tablename+"_POST' "+
	QString().sprintf("add column FADEUP_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEUP_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column FADEDOWN_POINT int default -1 after FADEUP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	QString().sprintf("add column FADEDOWN_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEDOWN_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	QString().sprintf("add column SEGUE_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after SEGUE_END_POINT";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<94)&&(set_schema>cur_schema)) {
    sql="alter table CART add column OWNER char(64) after ASYNCRONOUS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add index OWNER_IDX (OWNER)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TRACK_GROUP char(10) after CHAIN_LOG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column SCHEDULED_TRACKS int unsigned default 0 after IMPORT_DATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column COMPLETED_TRACKS int unsigned default 0 after SCHEDULED_TRACKS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<95)&&(set_schema>cur_schema)) {
    sql="alter table USERS add column VOICETRACK_LOG_PRIV enum('N','Y') not null default 'N' after ADDTO_LOG_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<96)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists RDLOGEDIT (")+
      "ID int unsigned primary key auto_increment,"+
      "STATION char(64) not null,"+
      "INPUT_CARD int default -1,"+
      "INPUT_PORT int default 0,"+
      "OUTPUT_CARD int default -1,"+
      "OUTPUT_PORT int default 0,"+
      "FORMAT int unsigned default 0,"+
      "SAMPRATE int unsigned default 44100,"+
      "LAYER int unsigned default 0,"+
      "BITRATE int unsigned default 0,"+
      "DEFAULT_CHANNELS int unsigned default 2,"+
      "MAXLENGTH int default 0,"+
      "TAIL_PREROLL int unsigned default 2000,"+
      "START_CART int unsigned default 0,"+
      "END_CART int unsigned default 0,"+
      "index STATION_IDX (STATION))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from STATIONS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("insert into RDLOGEDIT set ")+
	"STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<97)&&(set_schema>cur_schema)) {
    sql="alter table LOGS add column LOG_EXISTS enum('N','Y') default 'Y' after NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table LOGS add index NAME_IDX (NAME,LOG_EXISTS)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table USERS add column DELETE_REC_PRIV enum('N','Y') default 'N' after DELETE_LOG_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update USERS set DELETE_REC_PRIV=\"Y\" where DELETE_LOG_PRIV=\"Y\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<98)&&(set_schema>cur_schema)) {
    QString tablename;
    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("create table `")+q->value(0).toString()+"_SRT` ("+
	"ID int unsigned auto_increment primary key,"+
	"LENGTH int,"+
	"LOG_NAME char(64),"+
	"LOG_ID int,"+
	"CART_NUMBER int unsigned,"+
	"CUT_NUMBER int,"+
	"TITLE char(255),"+
	"ARTIST char(255),"+
	"STATION_NAME char(64),"+
	"EVENT_DATETIME datetime,"+
	"SCHEDULED_TIME time,"+
	"EVENT_TYPE int,"+
	"EVENT_SOURCE int,"+
	"PLAY_SOURCE int,"+
	"START_SOURCE int default 0,"+
	"EXT_START_TIME time,"+
	"EXT_LENGTH int,"+
	"EXT_CART_NAME char(32),"+
	"EXT_DATA char(32),"+
	"EXT_EVENT_ID char(8),"+
	"EXT_ANNC_TYPE char(8),"+
	"index EVENT_DATETIME_IDX(EVENT_DATETIME))"+
	" charset latin1 collate latin1_swedish_ci"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("select NAME from LOGS where ")+
	"SERVICE=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	tablename=q1->value(0).toString();
	tablename.replace(" ","_");
	sql=QString("select ")+
	  "`"+tablename+"_REC`.LENGTH,"+
	  "`"+tablename+"_REC`.LOG_ID,"+
	  "`"+tablename+"_REC`.CART_NUMBER,"+
	  "`"+tablename+"_REC`.CUT_NUMBER,"+
	  "`"+tablename+"_REC`.STATION_NAME,"+
	  "`"+tablename+"_REC`.EVENT_DATETIME,"+
	  "`"+tablename+"_REC`.EVENT_TYPE,"+
	  "`"+tablename+"_REC`.EVENT_SOURCE,"+
	  "`"+tablename+"_REC`.PLAY_SOURCE,"+
	  "`"+tablename+"_REC`.EXT_START_TIME,"+
	  "`"+tablename+"_REC`.EXT_LENGTH,"+
	  "`"+tablename+"_REC`.EXT_CART_NAME,"+
	  "`"+tablename+"_REC`.EXT_DATA,"+
	  "`"+tablename+"_REC`.EXT_EVENT_ID,"+
	  "`"+tablename+"_REC`.EXT_ANNC_TYPE,"+
	  "CART.TITLE,"+
	  "CART.ARTIST "+
	  "from CART right join `"+tablename+"_REC` on "+
	  "CART.NUMBER=`"+tablename+"_REC`.CART_NUMBER";
	q2=new RDSqlQuery(sql,false);
	while(q2->next()) {
	  sql=QString("insert into `")+q->value(0).toString()+"_SRT` set "+
	    QString().sprintf("LENGTH=%d,",q2->value(0).toInt())+
	    "LOG_NAME=\""+RDEscapeString(q1->value(0).toString())+"\","+
	    QString().sprintf("LOG_ID=%d,",q2->value(1).toInt())+
	    QString().sprintf("CART_NUMBER=%u,",q2->value(2).toUInt())+
	    QString().sprintf("CUT_NUMBER=%d,",q2->value(3).toInt())+
	    "TITLE=\""+RDEscapeString(q2->value(15).toString())+"\","+
	    "ARTIST=\""+RDEscapeString(q2->value(16).toString())+"\","+
	    "STATION_NAME=\""+RDEscapeString(q2->value(4).toString())+"\","+
	    "EVENT_DATETIME=\""+RDEscapeString(q2->value(5).toDateTime().
					toString("yyyy-MM-dd hh:mm:ss"))+"\","+
	    "SCHEDULED_TIME=\"00:00:00\","+
	    QString().sprintf("EVENT_TYPE=%d,",q2->value(6).toInt())+
	    QString().sprintf("EVENT_SOURCE=%d,",q2->value(7).toInt())+
	    QString().sprintf("PLAY_SOURCE=%d,",q2->value(8).toInt())+
	    "EXT_START_TIME=\""+RDEscapeString(q2->value(9).toTime().
					       toString("hh:mm:ss"))+"\","+
	    QString().sprintf("EXT_LENGTH=%d,",q2->value(10).toInt())+
	    "EXT_CART_NAME=\""+RDEscapeString(q2->value(11).toString())+"\","+
	    "EXT_DATA=\""+RDEscapeString(q2->value(12).toString())+"\","+
	    "EXT_EVENT_ID=\""+RDEscapeString(q2->value(13).toString())+"\","+
	    "EXT_ANNC_TYPE=\""+RDEscapeString(q2->value(14).toString())+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q2;
      }
      delete q1;
    }
    delete q;

    sql="alter table RDAIRPLAY add column DEFAULT_SERVICE char(10) after PAUSE_ENABLED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<99)&&(set_schema>cur_schema)) {
    sql="alter table USERS add column CONFIG_PANELS_PRIV enum('N','Y') default 'N' after REMOVEFROM_LOG_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update USERS set CONFIG_PANELS_PRIV=\"Y\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<100)&&(set_schema>cur_schema)) {
    sql="alter table CUTS add column ISRC char(12) after OUTCUE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select NUMBER,ISRC from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update CUTS set ")+
	"ISRC=\""+RDEscapeString(q->value(1).toString())+"\" where "+
	QString().sprintf("CART_NUMBER=%u",q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<101)&&(set_schema>cur_schema)) {
    for(int i=0;i<RD_MAX_CARDS;i++) {
      sql=QString("alter table AUDIO_PORTS ")+
	QString().sprintf("add column INPUT_%d_MODE int default 0 ",i)+
	QString().sprintf("after INPUT_%d_TYPE",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<102)&&(set_schema>cur_schema)) {
    sql="alter table CART add column PUBLISHER char(64) after AGENCY";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CART add index PUBLISHER_IDX (PUBLISHER)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add column COMPOSER char(64) after PUBLISHER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CART add index COMPOSER_IDX (COMPOSER)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<103)&&(set_schema>cur_schema)) {
    // There is no update 103!

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<104)&&(set_schema>cur_schema)) {
    sql="alter table CART add column USAGE_CODE int default 0\
         after USER_DEFINED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPORTS add column LINES_PER_PAGE int default 66\
         after USE_LEADING_ZEROS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPORTS add column STATION_TYPE int default 0\
         after LINES_PER_PAGE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPORTS add column STATION_FORMAT char(64)\
         after STATION_TYPE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column PUBLISHER char(64) after ARTIST";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column COMPOSER char(64) after PUBLISHER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column ISRC char(12) after PUBLISHER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column USAGE_CODE int default 0 after ISRC";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<105)&&(set_schema>cur_schema)) {
    for(int i=0;i<6;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column START_RML%d char(255) after PORT%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
      sql=QString().sprintf("alter table RDAIRPLAY add column STOP_RML%d char(255) after START_RML%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<106)&&(set_schema>cur_schema)) {
    for(int i=0;i<3;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column LOG_RML%d char(255) after UDP_STRING%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<107)&&(set_schema>cur_schema)) {
    sql="alter table RDLOGEDIT add column REC_START_CART int unsigned default 0 after END_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table RDLOGEDIT add column REC_END_CART int unsigned default 0 after REC_START_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<108)&&(set_schema>cur_schema)) {
    sql="alter table CART add column AVERAGE_SEGUE_LENGTH int unsigned after LENGTH_DEVIATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select NUMBER from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<109)&&(set_schema>cur_schema)) {
    sql="alter table EVENTS add column NESTED_EVENT char(64) after COLOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<110)&&(set_schema>cur_schema)) {
    for(unsigned i=6;i<10;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column CARD%d int default -1 after STOP_RML%d",i,i-1);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY add column PORT%d int default -1 after CARD%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY add column START_RML%d char(255) after PORT%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY add column STOP_RML%d char(255) after START_RML%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
    sql=QString("select ")+
      "ID,"+          // 00
      "CARD2,"+       // 01
      "PORT2,"+       // 02
      "START_RML2,"+  // 03
      "STOP_RML2 "+   // 04
      "from RDAIRPLAY";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update RDAIRPLAY set ")+
	QString().sprintf("CARD6=%d,",q->value(1).toInt())+
	QString().sprintf("PORT6=%d,",q->value(2).toInt())+
	"START_RML6=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STOP_RML6=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("CARD7=%d,",q->value(1).toInt())+
	QString().sprintf("PORT7=%d,",q->value(2).toInt())+
	"START_RML7=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STOP_RML7=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("CARD8=%d,",q->value(1).toInt())+
	QString().sprintf("PORT8=%d,",q->value(2).toInt())+
	"START_RML8=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STOP_RML8=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("CARD9=%d,",q->value(1).toInt())+
	QString().sprintf("PORT9=%d,",q->value(2).toInt())+
	"START_RML9=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STOP_RML9=\""+RDEscapeString(q->value(4).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<111)&&(set_schema>cur_schema)) {
    sql="alter table CART add column VALIDITY int unsigned default 2 after PLAY_ORDER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CUTS add column VALIDITY int unsigned default 2 after LOCAL_COUNTER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select NUMBER from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<112)&&(set_schema>cur_schema)) {
    sql="alter table RDLOGEDIT add column TRIM_THRESHOLD int default -3000 after REC_END_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table RDLOGEDIT add column RIPPER_LEVEL int default -1300 after TRIM_THRESHOLD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select STATION,TRIM_THRESHOLD,RIPPER_LEVEL from RDLIBRARY";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update RDLOGEDIT set ")+
	QString().sprintf("TRIM_THRESHOLD=%d,",q->value(1).toInt())+
	QString().sprintf("RIPPER_LEVEL=%d where ",q->value(2).toInt())+
	"STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="alter table RDLIBRARY modify RIPPER_LEVEL int default -1300";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDLOGEDIT modify DEFAULT_CHANNELS int unsigned default 1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<113)&&(set_schema>cur_schema)) {
    sql="alter table VGUEST_RESOURCES modify SURFACE_NUM int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_BREAK_STRING char(64) after MUS_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_TRACK_STRING char(64) after MUS_BREAK_STRING";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<114)&&(set_schema>cur_schema)) {
    sql="alter table LOGS add column MUSIC_LINKS int default 0 after COMPLETED_TRACKS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column MUSIC_LINKED enum('N','Y') default 'N' after MUSIC_LINKS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column TRAFFIC_LINKS int default 0 after MUSIC_LINKED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column TRAFFIC_LINKED enum('N','Y') default 'N' after TRAFFIC_LINKS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_EVENT_NAME char(64) after POST_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_START_TIME time after LINK_EVENT_NAME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString().sprintf("alter table `")+tablename+"_LOG` "+
	"add column LINK_LENGTH int default 0 after LINK_START_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_ID int default -1 after LINK_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_EVENT_NAME char(64) after POST_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_START_TIME time after LINK_EVENT_NAME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_LENGTH int default 0 after LINK_START_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_ID int default -1 after LINK_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_EVENT_NAME char(64) after POST_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_START_TIME time after LINK_EVENT_NAME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_LENGTH int default 0 after LINK_START_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_ID int default -1 after LINK_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<115)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"modify column LABEL char(64)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"modify column LABEL char(64)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"modify column LABEL char(64)";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="alter table SERVICES add column TRACK_GROUP char(10) after CHAIN_LOG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<116)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_EMBEDDED enum('N','Y') default 'N' after LINK_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_EMBEDDED enum('N','Y') default 'N' after LINK_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_EMBEDDED enum('N','Y') default 'N' after LINK_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<117)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column ORIGIN_USER char(8) after POST_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column ORIGIN_DATETIME datetime after ORIGIN_USER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      DropColumn(tablename+"_LOG","ORIGIN_NAME");
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column ORIGIN_USER char(8) after POST_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column ORIGIN_DATETIME datetime after ORIGIN_USER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column ORIGIN_USER char(8) after POST_TIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column ORIGIN_DATETIME datetime after ORIGIN_USER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      DropColumn(tablename+"_PRE","ORIGIN_NAME");
      DropColumn(tablename+"_POST","ORIGIN_NAME");
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<118)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column TFC_LABEL_CART char(32) after TFC_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_TRACK_CART char(32) after TFC_LABEL_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_LABEL_CART char(32) after MUS_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_TRACK_CART char(32) after MUS_LABEL_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_TITLE_OFFSET int after TFC_CART_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_TITLE_LENGTH int after TFC_TITLE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_TITLE_OFFSET int after MUS_CART_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_TITLE_LENGTH int after MUS_TITLE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<119)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column TFC_HOURS_OFFSET int after TFC_START_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_HOURS_LENGTH int after TFC_HOURS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TFC_MINUTES_OFFSET int after TFC_HOURS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_MINUTES_LENGTH int after TFC_MINUTES_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TFC_SECONDS_OFFSET int after TFC_MINUTES_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_SECONDS_LENGTH int after TFC_SECONDS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME,TFC_START_OFFSET from SERVICES where TFC_START_LENGTH=8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update SERVICES set ")+
	QString().sprintf("TFC_HOURS_OFFSET=%d,",q->value(1).toInt())+
	"TFC_HOURS_LENGTH=2,"+
	QString().sprintf("TFC_MINUTES_OFFSET=%d,",q->value(1).toInt()+3)+
	"TFC_MINUTES_LENGTH=2,"+
	QString().sprintf("TFC_SECONDS_OFFSET=%d,",q->value(1).toInt()+6)+
	"TFC_SECONDS_LENGTH=2 where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="alter table SERVICES add column MUS_HOURS_OFFSET int after MUS_START_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_HOURS_LENGTH int after MUS_HOURS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_MINUTES_OFFSET int after MUS_HOURS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_MINUTES_LENGTH int after MUS_MINUTES_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_SECONDS_OFFSET int after MUS_MINUTES_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_SECONDS_LENGTH int after MUS_SECONDS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME,MUS_START_OFFSET from SERVICES where MUS_START_LENGTH=8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update SERVICES set ")+
	QString().sprintf("MUS_HOURS_OFFSET=%d,",q->value(1).toInt())+
	"MUS_HOURS_LENGTH=2,"+
	QString().sprintf("MUS_MINUTES_OFFSET=%d,",q->value(1).toInt()+3)+
	"MUS_MINUTES_LENGTH=2,"+
	QString().sprintf("MUS_SECONDS_OFFSET=%d,",q->value(1).toInt()+6)+
	"MUS_SECONDS_LENGTH=2 where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<120)&&(set_schema>cur_schema)) {
    sql="alter table GROUPS add column COLOR char(7) default \"#000000\" after ENABLE_NOW_NEXT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDLOGEDIT add column DEFAULT_TRANS_TYPE int default 0 after RIPPER_LEVEL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<121)&&(set_schema>cur_schema)) {
    sql="alter table LOGS add column LINK_DATETIME datetime not null after ORIGIN_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column NEXT_ID int default 0 after TRAFFIC_LINKED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME,ORIGIN_DATETIME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update LOGS set ")+
	"LINK_DATETIME=\""+q->value(1).toDateTime().
	toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("select ID from `")+q->value(0).toString()+"_LOG` "+
	"order by ID";
      q1=new RDSqlQuery(sql,false);
      if(q1->last()) {
	sql=QString("update LOGS set ")+
	  QString().sprintf("NEXT_ID=%d where ",q1->value(0).toInt()+1)+
	  "NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<122)&&(set_schema>cur_schema)) {
    sql="alter table LOGS add column MODIFIED_DATETIME datetime not null after LINK_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column AUTO_REFRESH enum('N','Y') default 'N' after MODIFIED_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME,LINK_DATETIME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update LOGS set ")+
	"MODIFIED_DATETIME=\""+RDEscapeString(q->value(1).toDateTime().
			      toString("yyyy-MM-dd hh:mm:ss"))+"\" where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql="alter table SERVICES add column AUTO_REFRESH enum('N','Y') default 'N' after TRACK_GROUP";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<123)&&(set_schema>cur_schema)) {
    sql="alter table REPORTS add column FILTER_ONAIR_FLAG enum('N','Y') default 'N' after STATION_FORMAT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_SRT` "+
	"add column ONAIR_FLAG enum('N','Y') default 'N' after START_SOURCE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<124)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column TFC_LEN_HOURS_OFFSET int after TFC_SECONDS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_LEN_HOURS_LENGTH int after TFC_LEN_HOURS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TFC_LEN_MINUTES_OFFSET int after TFC_LEN_HOURS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_LEN_MINUTES_LENGTH int after TFC_LEN_MINUTES_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TFC_LEN_SECONDS_OFFSET int after TFC_LEN_MINUTES_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column TFC_LEN_SECONDS_LENGTH int after TFC_LEN_SECONDS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME,TFC_LENGTH_OFFSET,TFC_LENGTH_LENGTH from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      if(q->value(2).toInt()==5) {
	sql=QString("update SERVICES set ")+
	  QString().sprintf("TFC_LEN_MINUTES_OFFSET=%d,",q->value(1).toInt())+
	  "TFC_LEN_MINUTES_LENGTH=2,"+
	  QString().sprintf("TFC_LEN_SECONDS_OFFSET=%d,",q->value(1).toInt()+3)+
	  "TFC_LEN_SECONDS_LENGTH=2 where "+
	  "NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      }
      else {
	sql=QString("update SERVICES set ")+
	  QString().sprintf("TFC_LEN_SECONDS_OFFSET=%d,",q->value(1).toInt())+
	  QString().sprintf("TFC_LEN_SECONDS_LENGTH=%d where ",
			    q->value(2).toInt())+
	  "NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      }
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="alter table SERVICES add column MUS_LEN_HOURS_OFFSET int after MUS_SECONDS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_LEN_HOURS_LENGTH int after MUS_LEN_HOURS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_LEN_MINUTES_OFFSET int after MUS_LEN_HOURS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_LEN_MINUTES_LENGTH int after MUS_LEN_MINUTES_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_LEN_SECONDS_OFFSET int after MUS_LEN_MINUTES_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table SERVICES add column MUS_LEN_SECONDS_LENGTH int after MUS_LEN_SECONDS_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME,MUS_LENGTH_OFFSET,MUS_LENGTH_LENGTH from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      if(q->value(2).toInt()==5) {
	sql=QString("update SERVICES set ")+
	  QString().sprintf("MUS_LEN_MINUTES_OFFSET=%d,",q->value(1).toInt())+
	  "MUS_LEN_MINUTES_LENGTH=2,"+
	  QString().sprintf("MUS_LEN_SECONDS_OFFSET=%d,",q->value(1).toInt()+3)+
	  "MUS_LEN_SECONDS_LENGTH=2 where "+
	  "NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      }
      else {
	sql=QString("update SERVICES set ")+
	  QString().sprintf("MUS_LEN_SECONDS_OFFSET=%d,",q->value(1).toInt())+
	  QString().sprintf("MUS_LEN_SECONDS_LENGTH=%d where ",
			    q->value(2).toInt())+
	  "NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      }
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<125)&&(set_schema>cur_schema)) {
    sql="alter table REPORTS add column SERVICE_NAME char(64) after LINES_PER_PAGE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_SRT` "+
	"add column ALBUM char(255) after COMPOSER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+q->value(0).toString()+"_SRT` "+
	"add column LABEL char(64) after ALBUM";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<126)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column LINK_START_SLOP int default 0 after LINK_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column LINK_END_SLOP int default 0 after LINK_START_SLOP";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<127)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY add column PANEL_PAUSE_ENABLED enum('N','Y') default 'N' after FLASH_PANEL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<128)&&(set_schema>cur_schema)) {
    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_START_SLOP int default 0 after LINK_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_END_SLOP int default 0 after LINK_START_SLOP";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_START_SLOP int default 0 after LINK_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_END_SLOP int default 0 after LINK_START_SLOP";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<129)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY add column EXIT_CODE int default 0 after LOG_RML2";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column EXIT_PASSWORD char(41) default \"\" after EXIT_CODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }


    sql="alter table RDAIRPLAY add column LOG0_START_MODE int default 0 after EXIT_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_AUTO_RESTART enum('N','Y') default 'N' after LOG0_START_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_LOG_NAME char(64) after LOG0_AUTO_RESTART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_CURRENT_LOG char(64) after LOG0_LOG_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_RUNNING enum('N','Y') default 'N' after LOG0_CURRENT_LOG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_LOG_ID int default -1 after LOG0_RUNNING";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_LOG_LINE int default -1 after LOG0_LOG_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_START_MODE int default 0 after LOG0_LOG_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_AUTO_RESTART enum('N','Y') default 'N' after LOG1_START_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_LOG_NAME char(64) after LOG1_AUTO_RESTART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_CURRENT_LOG char(64) after LOG1_LOG_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_RUNNING enum('N','Y') default 'N' after LOG1_CURRENT_LOG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_LOG_ID int default -1 after LOG1_RUNNING";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_LOG_LINE int default -1 after LOG1_LOG_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_START_MODE int default 0 after LOG1_LOG_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_AUTO_RESTART enum('N','Y') default 'N' after LOG2_START_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_LOG_NAME char(64) after LOG2_AUTO_RESTART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_CURRENT_LOG char(64) after LOG2_LOG_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_RUNNING enum('N','Y') default 'N' after LOG2_CURRENT_LOG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_LOG_ID int default -1 after LOG2_RUNNING";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_LOG_LINE int default -1 after LOG2_LOG_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<130)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists RDCATCH (")+
      "ID int unsigned primary key auto_increment,"+
      "STATION char(64) not null,"+
      "ERROR_RML char(255),"+
      "index STATION_IDX (STATION))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<131)&&(set_schema>cur_schema)) {
    sql="alter table `EVENTS` add column `SCHED_GROUP` varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table `EVENTS` add column `TITLE_SEP` int(10) unsigned";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table `EVENTS` add column `HAVE_CODE` varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table `EVENTS` add column `HOR_SEP` int(10) unsigned";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table `EVENTS` add column `HOR_DIST` int(10) unsigned";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table `CLOCKS` add column `ARTISTSEP` int(10) unsigned";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table `CART` add column `SCHED_CODES` varchar( 255 ) NULL default NULL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("create table if not exists SCHED_CODES (")+
      "CODE varchar(10) not null primary key,"+
      "DESCRIPTION varchar(255))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    if(!DropTable("SCHED_STACK",err_msg)) {
      return false;
    }
    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString svc=q->value(0).toString();
      if(!DropTable(svc.replace(" ","_")+"_STACK",err_msg)) {
	return false;
      }
    }
    delete q;
    sql="select NAME from LOGS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;
    sql="select NAME from EVENTS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_PRE` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_PRE` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
		  "add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<132)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists DROPBOXES (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64),"+
      "GROUP_NAME char(10),"+
      "PATH char(255),"+
      "NORMALIZATION_LEVEL int default 1,"+
      "AUTOTRIM_LEVEL int default 1,"+
      "SINGLE_CART enum('N','Y') default 'N',"+
      "TO_CART int unsigned default 0,"+
      "USE_CARTCHUNK_ID enum('N','Y') default 'N',"+
      "DELETE_CUTS enum('N','Y') default 'N',"+
      "METADATA_PATTERN char(64),"+
      "FIX_BROKEN_FORMATS enum('N','Y') default 'N',"+
      "LOG_PATH char(255),"+
      "index STATION_NAME_IDX (STATION_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<133)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;
    sql="select NAME from EVENTS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<134)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists RDPANEL (")+
      "ID int not null primary key auto_increment,"+
      "STATION char(40) not null,"+
      "CARD2 int default -1,"+
      "PORT2 int default -1,"+
      "START_RML2 char(255),"+
      "STOP_RML2 char(255),"+
      "CARD6 int default -1,"+
      "PORT6 int default -1,"+
      "START_RML6 char(255),"+
      "STOP_RML6 char(255),"+
      "CARD7 int default -1,"+
      "PORT7 int default -1,"+
      "START_RML7 char(255),"+
      "STOP_RML7 char(255),"+
      "CARD8 int default -1,"+
      "PORT8 int default -1,"+
      "START_RML8 char(255),"+
      "STOP_RML8 char(255),"+
      "CARD9 int default -1,"+
      "PORT9 int default -1,"+
      "START_RML9 char(255),"+
      "STOP_RML9 char(255),"+
      "STATION_PANELS int default 3,"+
      "USER_PANELS int default 3,"+
      "CLEAR_FILTER enum('N','Y') default 'N',"+
      "FLASH_PANEL enum('N','Y') default 'N',"+
      "PANEL_PAUSE_ENABLED enum('N','Y') default 'N',"+
      "DEFAULT_SERVICE char(10),"+
      "index STATION_IDX (STATION))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create EXTENDED_PANELS Table
    //
    sql=QString("create table if not exists EXTENDED_PANELS (")+
      "ID int auto_increment not null primary key,"+
      "TYPE int not null,"+
      "OWNER char(64) not null,"+
      "PANEL_NO int not null,"+
      "ROW_NO int not null,"+
      "COLUMN_NO int not null,"+
      "LABEL char(64),"+
      "CART int,"+
      "DEFAULT_COLOR char(7),"+
      "index LOAD_IDX (TYPE,OWNER,PANEL_NO),"+
      "index SAVE_IDX (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<135)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column STARTUP_CART int unsigned default 0 after HEARTBEAT_INTERVAL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<136)&&(set_schema>cur_schema)) {
    //
    // Create PANEL_NAMES Table
    //
    sql=QString("create table if not exists PANEL_NAMES (")+
      "ID int auto_increment not null primary key,"+
      "TYPE int not null,"+
      "OWNER char(64) not null,"+
      "PANEL_NO int not null,"+
      "NAME char(64),"+
      "index LOAD_IDX (TYPE,OWNER,PANEL_NO))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create EXTENDED_PANEL_NAMES Table
    //
    sql=QString("create table if not exists EXTENDED_PANEL_NAMES (")+
      "ID int auto_increment not null primary key,"+
      "TYPE int not null,"+
      "OWNER char(64) not null,"+
      "PANEL_NO int not null,"+
      "NAME char(64),"+
      "index LOAD_IDX (TYPE,OWNER,PANEL_NO))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  } 

  if((cur_schema<137)&&(set_schema>cur_schema)) {
    sql="alter table CART add column AVERAGE_HOOK_LENGTH int unsigned default 0 after AVERAGE_SEGUE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NUMBER from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<138)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY ")+
      "add column BUTTON_LABEL_TEMPLATE char(32) "+
      "default \"%t\" after PANEL_PAUSE_ENABLED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDPANEL add column BUTTON_LABEL_TEMPLATE char(32) default \"%t\" after PANEL_PAUSE_ENABLED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<139)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY modify EXIT_PASSWORD char(41) default \"\""; 
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<140)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column TFC_BREAK_STRING char(64) after TFC_TRACK_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TFC_TRACK_STRING char(64) after TFC_TRACK_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<141)&&(set_schema>cur_schema)) {
    sql="alter table DROPBOXES add column TITLE_FROM_CARTCHUNK_ID enum('N','Y') default 'N' after USE_CARTCHUNK_ID"; 
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<142)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists FEEDS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "KEY_NAME char(8) unique not null,"+
      "CHANNEL_TITLE char(255),"+
      "CHANNEL_DESCRIPTION text,"+
      "CHANNEL_CATEGORY char(64),"+
      "CHANNEL_LINK char(255),"+
      "CHANNEL_COPYRIGHT char(64),"+
      "CHANNEL_WEBMASTER char(64),"+
      "CHANNEL_LANGUAGE char(5) default \"en-us\","+
      "BASE_URL char(255),"+
      "PURGE_URL char(255),"+
      "PURGE_USERNAME char(64),"+
      "PURGE_PASSWORD char(64),"+
      "HEADER_XML text,"+
      "CHANNEL_XML text,"+
      "ITEM_XML text,"+
      "MAX_SHELF_LIFE int,"+
      "LAST_BUILD_DATETIME datetime,"+
      "ORIGIN_DATETIME datetime,"+
      "ENABLE_AUTOPOST enum('N','Y') default 'N',"+
      "index KEY_NAME_IDX(KEY_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists PODCASTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "FEED_ID int unsigned not null,"+
      "STATUS int unsigned default 0,"+
      "ITEM_TITLE char(255),"+
      "ITEM_DESCRIPTION text,"+
      "ITEM_CATEGORY char(64),"+
      "ITEM_LINK char(255),"+
      "AUDIO_FILENAME char(255),"+
      "AUDIO_LENGTH int unsigned,"+
      "SHELF_LIFE int,"+
      "ORIGIN_DATETIME datetime,"+
      "index FEED_ID_IDX(FEED_ID,ORIGIN_DATETIME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RECORDINGS add column FEED_ID int default -1 after URL_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table VERSION add column LAST_MAINT_DATETIME datetime default \"1970-01-01 00:00:00\" after DB";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<143)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists AUX_METADATA (")+
      "ID int unsigned auto_increment not null primary key,"+
      "FEED_ID int unsigned,"+
      "VAR_NAME char(16),"+
      "CAPTION char(64),"+
      "index FEED_ID_IDX(FEED_ID))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select KEY_NAME from FEEDS"; 
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      CreateAuxFieldsTable143(q->value(0).toString());
      //      RDCreateAuxFieldsTable(q->value(0).toString(),config);
    }
    delete q;
    
    WriteSchemaVersion(++cur_schema);
  }
  
  if((cur_schema<144)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column UPLOAD_FORMAT int default 2 after ENABLE_AUTOPOST";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table FEEDS add column UPLOAD_CHANNELS int default 2 after UPLOAD_FORMAT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table FEEDS add column UPLOAD_SAMPRATE int default 44100 after UPLOAD_CHANNELS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table FEEDS add column UPLOAD_BITRATE int default 32000 after UPLOAD_CHANNELS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table FEEDS add column UPLOAD_QUALITY int default 0 after UPLOAD_BITRATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table FEEDS add column NORMALIZE_LEVEL int default -100 after UPLOAD_QUALITY";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table PODCASTS add column ITEM_COMMENTS char(255) after ITEM_LINK";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table PODCASTS add column ITEM_AUTHOR char(255) after ITEM_COMMENTS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table PODCASTS add column ITEM_SOURCE_TEXT char(64) after ITEM_AUTHOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table PODCASTS add column ITEM_SOURCE_URL char(255) after ITEM_SOURCE_TEXT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<145)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists FEED_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "USER_NAME char(8),"+
      "KEY_NAME char(8),"+
      "index USER_IDX (USER_NAME),"+
      "index KEYNAME_IDX (KEY_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select LOGIN_NAME from USERS where ")+
      "(ADMIN_USERS_PRIV='N')&&(ADMIN_CONFIG_PRIV='N')";
    q=new RDSqlQuery(sql,false);
    sql="select KEY_NAME from FEEDS";
    q1=new RDSqlQuery(sql,false);
    while(q->next()) {
      while(q1->next()) {
	sql=QString("insert into FEED_PERMS set ")+
	  "USER_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  "KEY_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      q1->seek(-1);
    }
    delete q1;
    delete q;


    sql="alter table USERS add column ADD_PODCAST_PRIV enum('N','Y') not null default 'N' after EDIT_CATCHES_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table USERS add column EDIT_PODCAST_PRIV enum('N','Y') not null default 'N' after ADD_PODCAST_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table USERS add column DELETE_PODCAST_PRIV enum('N','Y') not null default 'N' after EDIT_PODCAST_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="update USERS set ADD_PODCAST_PRIV='Y',EDIT_PODCAST_PRIV='Y', DELETE_PODCAST_PRIV='Y' where (ADMIN_CONFIG_PRIV='N')&&(ADMIN_USERS_PRIV='N')";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }
  if((cur_schema<146)&&(set_schema>cur_schema)) {
   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<147)&&(set_schema>cur_schema)) {
    sql="alter table USERS add column ENABLE_WEB enum('N','Y') default 'N' after PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists WEB_CONNECTIONS (")+
      "SESSION_ID int unsigned not null primary key,"+
      "LOGIN_NAME char(8),"+
      "IP_ADDRESS char(16),"+
      "TIME_STAMP datetime)"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<148)&&(set_schema>cur_schema)) {
    sql=QString("alter table CUTS add column ")+
      QString().sprintf("SEGUE_GAIN int default %d after SEGUE_END_POINT",
			RD_FADE_DEPTH);
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table PODCASTS add column AUDIO_TIME int unsigned after AUDIO_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }


  if((cur_schema<149)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists SWITCHER_NODES (")+
      "ID int not null auto_increment primary key,"+
      "STATION_NAME char(64),"+
      "MATRIX int,"+
      "BASE_OUTPUT int default 0,"+
      "HOSTNAME char(64),"+
      "PASSWORD char(64),"+
      "TCP_PORT int,"+
      "DESCRIPTION char(255),"+
      "index STATION_IDX (STATION_NAME,MATRIX))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table INPUTS add column NODE_HOSTNAME char(255) after DEVICE_NUM";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table INPUTS add column NODE_TCP_PORT int after NODE_HOSTNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table INPUTS add column NODE_SLOT int after NODE_TCP_PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table add index NODE_IDX(STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table OUTPUTS add column NODE_HOSTNAME char(255) after DEVICE_NUM";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table OUTPUTS add column NODE_TCP_PORT int after NODE_HOSTNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table OUTPUTS add column NODE_SLOT int after NODE_TCP_PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table add index NODE_IDX(STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<150)&&(set_schema>cur_schema)) {
    sql="alter table MATRICES add column PORT_2 int not null after PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column PORT_TYPE_2 int default 0 after PORT_TYPE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column IP_ADDRESS_2 char(16) after IP_ADDRESS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column IP_PORT_2 int after IP_PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column USERNAME_2 char(32) after USERNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column PASSWORD_2 char(32) after PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<151)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column KEEP_METADATA enum('N','Y') default 'Y' after ENABLE_AUTOPOST";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select KEY_NAME from FEEDS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      CreateFeedLog151(q->value(0).toString());
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<152)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column EDITOR_PATH char(255) default \"\"\
         after STARTUP_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDPANEL add column CARD3 int default -1 after STOP_RML2";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDPANEL add column PORT3 int default -1 after CARD3";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDPANEL add column START_RML3 char(255) after PORT3";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDPANEL add column STOP_RML3 char(255) after START_RML3";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<153)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column FILTER_MODE int default 0 after EDITOR_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<154)&&(set_schema>cur_schema)) {
    sql=QString().sprintf("alter table CUTS add column SEGUE_GAIN int default %d after SEGUE_END_POINT",RD_FADE_DEPTH);
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<155)&&(set_schema>cur_schema)) {
    sql="alter table RDLIBRARY add column ENABLE_EDITOR enum('N','Y') default 'N' after CDDB_SERVER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<156)&&(set_schema>cur_schema)) {
    sql="alter table MATRICES add column LAYER int default 86 after TYPE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<157)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column BROADCAST_SECURITY int unsigned default 0 after BACKUP_LIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<158)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS;";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("update `")+tablename+"_LOG` set TRANS_TYPE=1,SEGUE_GAIN=0 where TRANS_TYPE=3";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<159)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY add column SKIN_PATH char(255) default \"";
    sql+=RD_DEFAULT_RDAIRPLAY_SKIN;
    sql+="\" after EXIT_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDPANEL add column SKIN_PATH char(255) default \"";
    sql+=RD_DEFAULT_RDPANEL_SKIN;
    sql+="\" after DEFAULT_SERVICE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<160)&&(set_schema>cur_schema)) {
    //
    // Create ENCODERS Table
    //
    sql=QString("create table if not exists ENCODERS (")+
      "ID int not null auto_increment primary key,"+
      "NAME char(32) not null,"+
      "STATION_NAME char(64),"+
      "COMMAND_LINE char(255),"+
      "DEFAULT_EXTENSION char(16),"+
      "index NAME_IDX(NAME,STATION_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    // Ensure that dynamic format IDs start after 100
    sql="insert into ENCODERS set ID=100,NAME=\"dummy\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="delete from ENCODERS where ID=100";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create ENCODER_BITRATES Table
    //
    sql=QString("create table if not exists ENCODER_BITRATES (")+
      "ID int not null auto_increment primary key,"+
      "ENCODER_ID int not null,"+
      "BITRATES int not null,"+
      "index ENCODER_ID_IDX(ENCODER_ID))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    
    //
    // Create ENCODER_CHANNELS Table
    //
    sql=QString("create table if not exists ENCODER_CHANNELS (")+
      "ID int not null auto_increment primary key,"+
      "ENCODER_ID int not null,"+
      "CHANNELS int not null,"+
      "index ENCODER_ID_IDX(ENCODER_ID))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create ENCODER_SAMPLERATES Table
    //
    sql=QString("create table if not exists ENCODER_SAMPLERATES (")+
      "ID int not null auto_increment primary key,"+
      "ENCODER_ID int not null,"+
      "SAMPLERATES int not null,"+
      "index ENCODER_ID_IDX(ENCODER_ID))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<161)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column UPLOAD_EXTENSION char(16) default \"mp3\" after UPLOAD_QUALITY";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<162)&&(set_schema>cur_schema)) {
    sql="alter table GPIS alter column MACRO_CART set default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists GPOS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "MACRO_CART int default 0,"+
      "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select STATION_NAME,MATRIX,GPOS from MATRICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<q->value(2).toInt();i++) {
	sql=QString("insert into GPOS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MATRIX=%d,",q->value(1).toInt())+
	  QString().sprintf("NUMBER=%d,",i+1)+
	  "MACRO_CART=0";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<163)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS add column EVENTDATE_OFFSET int default 0 after ENDDATE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<164)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists NOWNEXT_PLUGINS (")+
      "ID int auto_increment not null primary key,    "+
      "STATION_NAME char(64) not null,		     "+
      "LOG_MACHINE int unsigned not null default 0,   "+
      "PLUGIN_PATH char(255),"+
      "PLUGIN_ARG char(255),"+
      "index STATION_IDX (STATION_NAME,LOG_MACHINE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<165)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists DROPBOX_PATHS (")+
      "ID int auto_increment not null primary key,"+
      "DROPBOX_ID int not null,"+
      "FILE_PATH char(255) not null,"+
      "FILE_DATETIME datetime,"+
      "index FILE_PATH_IDX (DROPBOX_ID,FILE_PATH))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table DROPBOXES add DELETE_SOURCE enum('N','Y') default 'Y' after DELETE_CUTS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table DROPBOXES add column STARTDATE_OFFSET int default 0 after METADATA_PATTERN";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table DROPBOXES add column ENDDATE_OFFSET int default 0 after STARTDATE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<166)&&(set_schema>cur_schema)) {
    sql="alter table GROUPS add column CUT_SHELFLIFE int default -1 after DEFAULT_HIGH_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table GROUPS add column DEFAULT_TITLE char(255) default \"Imported from %f.%e\" after CUT_SHELFLIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column SYSTEM_MAINT enum('N','Y') default 'Y' after FILTER_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column ELR_SHELFLIFE int default -1 after AUTO_REFRESH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table EVENTS add column REMARKS char(255) after NESTED_EVENT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CLOCKS add column REMARKS char(255) after COLOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<167)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY add column LOG0_NOW_CART int unsigned default 0 after LOG0_LOG_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG0_NEXT_CART int unsigned default 0 after LOG0_NOW_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_NOW_CART int unsigned default 0 after LOG1_LOG_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG1_NEXT_CART int unsigned default 0 after LOG1_NOW_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_NOW_CART int unsigned default 0 after LOG2_LOG_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column LOG2_NEXT_CART int unsigned default 0 after LOG2_NOW_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<168)&&(set_schema>cur_schema)) {
    sql="alter table GPIS add column OFF_MACRO_CART int default 0 after MACRO_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table GPOS add column OFF_MACRO_CART int default 0 after MACRO_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<169)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column DEFAULT_LOG_SHELFLIFE int default -1 after AUTO_REFRESH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS add column PURGE_DATE date after END_DATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<170)&&(set_schema>cur_schema)) {
    sql="alter table USERS modify column LOGIN_NAME char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table USERS modify column FULL_NAME char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table FEED_PERMS modify column USER_NAME char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table LOGS modify column ORIGIN_USER char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().
	sprintf("alter table %s_LOG modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().
	sprintf("alter table %s_PRE modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
      sql=QString().
	sprintf("alter table %s_POST modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="alter table STATIONS modify column USER_NAME char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS modify column DEFAULT_NAME char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table USER_PERMS modify column USER_NAME char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table WEB_CONNECTIONS modify column LOGIN_NAME char(255)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<171)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column CAST_ORDER enum('N','Y') default 'N' after ITEM_XML";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table PODCASTS add column EFFECTIVE_DATETIME datetime after ORIGIN_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select ID,ORIGIN_DATETIME from PODCASTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update PODCASTS set ")+
	"EFFECTIVE_DATETIME=\""+q->value(1).toDateTime().
	toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
        QString().sprintf("ID=%u",q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<172)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column REDIRECT_PATH char(255) after NORMALIZE_LEVEL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<173)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column BASE_PREAMBLE char(255) after BASE_URL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<174)&&(set_schema>cur_schema)) {
    sql="alter table MATRICES add column START_CART int unsigned after PASSWORD_2";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column STOP_CART int unsigned after START_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column START_CART_2 int unsigned after STOP_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table MATRICES add column STOP_CART_2 int unsigned after START_CART_2";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<175)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists SYSTEM (")+
      "ID int auto_increment not null primary key,"+
      "DUP_CART_TITLES enum('N','Y') not null default 'Y')"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="insert into SYSTEM set DUP_CART_TITLES=\"Y\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<176)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY add column SHOW_COUNTERS enum('N','Y') default 'N' after SKIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table RDAIRPLAY add column AUDITION_PREROLL int default 10000 after SHOW_COUNTERS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<177)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDPANEL ")+
      "add column SKIN_PATH char(255) default \""+RD_DEFAULT_RDPANEL_SKIN+"\" "+
      "after DEFAULT_SERVICE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    DropColumn("PANELS","SKIN_PATH");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<178)&&(set_schema>cur_schema)) {
    sql="alter table PODCASTS modify column STATUS int unsigned default 1";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SYSTEM ")+
      "add column MAX_POST_LENGTH "+
      QString().sprintf("int unsigned default %u after DUP_CART_TITLES",
			RD_DEFAULT_MAX_POST_LENGTH);
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<179)&&(set_schema>cur_schema)) {
    sql="alter table FEEDS add column MEDIA_LINK_MODE int default 0 after REDIRECT_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<180)&&(set_schema>cur_schema)) {
    sql="alter table RDLIBRARY add column SRC_CONVERTER int default 1 after ENABLE_EDITOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<181)&&(set_schema>cur_schema)) {
    length_update_required=true;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<182)&&(set_schema>cur_schema)) {
    sql="alter table CART add column NOTES text after MACROS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<183)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists IMPORT_TEMPLATES (")+
      "NAME char(64) not null primary key,"+
      "CART_OFFSET int,"+
      "CART_LENGTH int,"+
      "TITLE_OFFSET int,"+
      "TITLE_LENGTH int,"+
      "HOURS_OFFSET int,"+
      "HOURS_LENGTH int,"+
      "MINUTES_OFFSET int,"+
      "MINUTES_LENGTH int,"+
      "SECONDS_OFFSET int,"+
      "SECONDS_LENGTH int,"+
      "LEN_HOURS_OFFSET int,"+
      "LEN_HOURS_LENGTH int,"+
      "LEN_MINUTES_OFFSET int,"+
      "LEN_MINUTES_LENGTH int,"+
      "LEN_SECONDS_OFFSET int,"+
      "LEN_SECONDS_LENGTH int,"+
      "LENGTH_OFFSET int,"+
      "LENGTH_LENGTH int,"+
      "DATA_OFFSET int,"+
      "DATA_LENGTH int,"+
      "EVENT_ID_OFFSET int,"+
      "EVENT_ID_LENGTH int,"+
      "ANNC_TYPE_OFFSET int,"+
      "ANNC_TYPE_LENGTH int)"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    InsertImportFormats(err_msg);

    sql="alter table SERVICES add column TFC_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\" after TFC_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\" after MUS_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="update SERVICES set TFC_IMPORT_TEMPLATE=\"\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="update SERVICES set MUS_IMPORT_TEMPLATE=\"\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<184)&&(set_schema>cur_schema)) {
    //
    // Removed as redundant with update 183 [call to InsertImportFormats()].
    //

    WriteSchemaVersion(++cur_schema);
  }  

  if((cur_schema<185)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column TFC_PREIMPORT_CMD text after TFC_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column TFC_WIN_PREIMPORT_CMD text after TFC_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_PREIMPORT_CMD text after MUS_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SERVICES add column MUS_WIN_PREIMPORT_CMD text after MUS_WIN_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<186)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      if(!UpdateLogTable186(tablename+"_LOG",err_msg)) {
	return false;
      }
    }
    delete q;
    
    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      if(!UpdateLogTable186(tablename+"_PRE",err_msg)) {
	return false;
      }
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      if(!UpdateLogTable186(tablename+"_POST",err_msg)) {
	return false;
      }
    }

    sql="select NAME from CLOCKS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      if(!ConvertTimeField186(tablename+"_CLK","START_TIME",err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<187)&&(set_schema>cur_schema)) {
    sql="alter table CUTS add column ISCI char(32) after ISRC";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column ISCI char(32) after ISRC";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<188)&&(set_schema>cur_schema)) {
    // Nothing to be done here

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<189)&&(set_schema>cur_schema)) {
    sql="alter table CUTS add column UPLOAD_DATETIME datetime after LAST_PLAY_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<190)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column HAVE_TWOLAME enum('N','Y') default 'N' after HAVE_FLAC";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<191)&&(set_schema>cur_schema)) {
    sql="alter table SERVICES add column PROGRAM_CODE char(255) after NAME_TEMPLATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<192)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS add column EXIT_TEXT text after EXIT_CODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<193)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS alter column IPV4_ADDRESS set default \"127.0.0.2\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<194)&&(set_schema>cur_schema)) {
    sql=QString().sprintf("alter table SYSTEM ")+
      "add column SAMPLE_RATE "+
      QString().sprintf("int unsigned default %d after ID",
			RD_DEFAULT_SAMPLE_RATE);
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="select DEFAULT_SAMPRATE from RDLIBRARY";
    q=new RDSqlQuery(sql,false);
    if(q->first()) {
      sql=QString().sprintf("update SYSTEM set SAMPLE_RATE=%u",
			    q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<195)&&(set_schema>cur_schema)) {
    sql="alter table RECORDINGS add column ENABLE_METADATA enum('N','Y') default 'N' after URL_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<196)&&(set_schema>cur_schema)) {
    sql="alter table DROPBOXES add column IMPORT_CREATE_DATES enum('N','Y') default 'N' after LOG_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table DROPBOXES add column CREATE_STARTDATE_OFFSET int default 0 after IMPORT_CREATE_DATES";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table DROPBOXES add column CREATE_ENDDATE_OFFSET int default 0 after CREATE_STARTDATE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<197)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists REPLICATORS (")+
      "NAME char(32) not null primary key,"+
      "DESCRIPTION char(64),"+
      "TYPE_ID int unsigned not null,"+
      "STATION_NAME char(64),"+
      "FORMAT int unsigned default 0,"+
      "CHANNELS int unsigned default 2,"+
      QString().sprintf("SAMPRATE int unsigned default %u,",
			RD_DEFAULT_SAMPLE_RATE)+
      "BITRATE int unsigned default 0,"+
      "QUALITY int unsigned default 0,"+
      "URL char(255),"+
      "URL_USERNAME char(64),"+
      "URL_PASSWORD char(64),"+
      "ENABLE_METADATA enum('N','Y') default 'N',"+
      "NORMALIZATION_LEVEL int unsigned default 0,"+
      "index TYPE_ID_IDX (TYPE_ID))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists REPLICATOR_MAP (")+
      "ID int unsigned not null auto_increment primary key,"+
      "REPLICATOR_NAME char(32) not null,"+
      "GROUP_NAME char(10) not null,"+
      "index REPLICATOR_NAME_IDX(REPLICATOR_NAME),"+
      "index GROUP_NAME_IDX(GROUP_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add column METADATA_DATETIME datetime after NOTES";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="update CART set METADATA_DATETIME=now()";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create REPL_CART_STATE Table
    //
    sql=QString("create table if not exists REPL_CART_STATE (")+
      "ID int unsigned not null auto_increment primary key,"+
      "REPLICATOR_NAME char(32) not null,"+
      "CART_NUMBER int unsigned not null,"+
      "ITEM_DATETIME datetime not null,"+
      "unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CART_NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    //
    // Create REPL_CUT_STATE Table
    //
    sql=QString("create table if not exists REPL_CUT_STATE (")+
      "ID int unsigned not null auto_increment primary key,"+
      "REPLICATOR_NAME char(32) not null,"+
      "CUT_NAME char(12) not null,"+
      "ITEM_DATETIME datetime not null,"+
      "index REPLICATOR_NAME_IDX(REPLICATOR_NAME),"+
      "index CUT_NAME_IDX(CUT_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<198)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists ISCI_XREFERENCE (")+
      "ID int unsigned not null auto_increment primary key,"+
      "CART_NUMBER int unsigned not null,"+
      "ISCI char(32) not null,"+
      "FILENAME char(64) not null,"+
      "LATEST_DATE date not null,"+
      "TYPE char(1) not null,"+
      "ADVERTISER_NAME char(30),"+
      "PRODUCT_NAME char(35),"+
      "CREATIVE_TITLE char(30),"+
      "REGION_NAME char(80),"+
      "index CART_NUMBER_IDX(CART_NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table VERSION add column LAST_ISCI_XREFERENCE datetime default \"1970-01-01 00:00:00\" after LAST_MAINT_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table SYSTEM add column ISCI_XREFERENCE_PATH char(255) after MAX_POST_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
  }

  if((cur_schema<199)&&(set_schema>cur_schema)) {
    sql="create index TYPE_IDX on ISCI_XREFERENCE (TYPE,LATEST_DATE)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="create index LATEST_DATE_IDX on ISCI_XREFERENCE (LATEST_DATE)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<200)&&(set_schema>cur_schema)) {
    // Nothing to be done here

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<201)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column HTTP_STATION char(64) default \"localhost\" after IPV4_ADDRESS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column CAE_STATION char(64) \
         default \"localhost\" after HTTP_STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<202)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists RDHOTKEYS (")+
      "ID int unsigned not null auto_increment primary key,"+
      "STATION_NAME char(64),"+
      "MODULE_NAME char(64),"+
      "KEY_ID int,"+
      "KEY_VALUE char(64),"+
      "KEY_LABEL char(64))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="select NAME from STATIONS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      if (!InsertRDAirplayHotkeys((const char *)q->value(0).toString(),err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<203)&&(set_schema>cur_schema)) {
    DropColumn("REPLICATORS","NORMALIZATION_LEVEL");
    sql=
      "alter table REPLICATORS add column NORMALIZATION_LEVEL int default 0 after ENABLE_METADATA";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPL_CART_STATE add column REPOST enum('N','Y') default 'N' after ITEM_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPL_CART_STATE add column POSTED_FILENAME char(255) after CART_NUMBER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="drop index REPLICATOR_NAME_IDX on REPL_CART_STATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="create unique index REPLICATOR_NAME_IDX on REPL_CART_STATE (REPLICATOR_NAME,CART_NUMBER,POSTED_FILENAME)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="delete from REPL_CART_STATE where POSTED_FILENAME is null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<204)&&(set_schema>cur_schema)) {
    sql=QString("insert into IMPORT_TEMPLATES set ")+
      "NAME=\"The Traffic Light\","+
      "CART_OFFSET=10,"+
      "CART_LENGTH=6,"+
      "TITLE_OFFSET=25,"+
      "TITLE_LENGTH=34,"+
      "HOURS_OFFSET=0,"+
      "HOURS_LENGTH=2,"+
      "MINUTES_OFFSET=3,"+
      "MINUTES_LENGTH=2,"+
      "SECONDS_OFFSET=6,"+
      "SECONDS_LENGTH=2,"+
      "LEN_HOURS_OFFSET=60,"+
      "LEN_HOURS_LENGTH=2,"+
      "LEN_MINUTES_OFFSET=63,"+
      "LEN_MINUTES_LENGTH=2,"+
      "LEN_SECONDS_OFFSET=66,"+
      "LEN_SECONDS_LENGTH=2,"+
      "DATA_OFFSET=69,"+
      "DATA_LENGTH=32";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<205)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column START_JACK enum('N','Y') default 'N' after FILTER_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table STATIONS add column JACK_SERVER_NAME char(64) after START_JACK";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<206)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists REPORT_GROUPS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "REPORT_NAME char(64) not null,"+
      "GROUP_NAME char(10),"+
      "index IDX_REPORT_NAME (REPORT_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table REPORTS add column FILTER_GROUPS enum('N','Y') default 'N' after FILTER_ONAIR_FLAG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<207)&&(set_schema>cur_schema)) {
    //
    // Removed as redundant with update 183 [call to InsertImportFormats()].
    //

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<208)&&(set_schema>cur_schema)) {
    sql="alter table RDAIRPLAY add column HOUR_SELECTOR_ENABLED enum('N','Y') default 'N' after DEFAULT_SERVICE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<209)&&(set_schema>cur_schema)) {
    sql="alter table STATIONS add column JACK_COMMAND_LINE char(255) after JACK_SERVER_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists JACK_CLIENTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "DESCRIPTION char(64),"+
      "COMMAND_LINE char(255) not null,"+
      "index IDX_STATION_NAME (STATION_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<210)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists CARTSLOTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "SLOT_NUMBER int unsigned not null,"+
      "MODE int not null default 0,"+
      "DEFAULT_MODE int not null default -1,"+
      "STOP_ACTION int not null default 0,"+
      "DEFAULT_STOP_ACTION int not null default -1,"+
      "CART_NUMBER int default 0,"+
      "DEFAULT_CART_NUMBER int not null default 0,"+
      "SERVICE_NAME char(10),"+
      "CARD int not null default 0,"+
      "INPUT_PORT int not null default 0,"+
      "OUTPUT_PORT int not null default 0,"+
      "index STATION_NAME_IDX(STATION_NAME,SLOT_NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<211)&&(set_schema>cur_schema)) {
    sql=QString("alter table SYSTEM add column TEMP_CART_GROUP char(10) ")+
      "default \"TEMP\" after ISCI_XREFERENCE_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<212)&&(set_schema>cur_schema)) {
    sql=QString("alter table CARTSLOTS add column HOOK_MODE int default 0 ")+
      "after DEFAULT_CART_NUMBER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CARTSLOTS add column DEFAULT_HOOK_MODE int ")+
      "default -1 after HOOK_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<213)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add column CUE_CARD int default 0 ")+
      "after JACK_COMMAND_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STATIONS add column CUE_PORT int default 0 ")+
      "after CUE_CARD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STATIONS add column CARTSLOT_COLUMNS int ")+
      "default 1 after CUE_PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STATIONS add column CARTSLOT_ROWS int ")+
      "default 8 after CARTSLOT_COLUMNS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select STATION_NAME,CARD_NUMBER,PORT_NUMBER from DECKS ")+
      "where CHANNEL=0";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update STATIONS set ")+
	QString().sprintf("CUE_CARD=%d,",q->value(1).toInt())+
	QString().sprintf("CUE_PORT=%d ",q->value(2).toInt())+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<214)&&(set_schema>cur_schema)) {
    sql=QString("alter table SERVICES add column AUTOSPOT_GROUP char(10) ")+
      "after TRACK_GROUP";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<215)&&(set_schema>cur_schema)) {
    sql=QString("alter table GROUPS add column DELETE_EMPTY_CARTS ")+
      "enum('N','Y') default 'N' after CUT_SHELFLIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<216)&&(set_schema>cur_schema)) {
    sql="alter table CUTS add index ISCI_IDX(ISCI)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CUTS add index ISRC_IDX(ISRC)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<217)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists LIVEWIRE_GPIO_SLOTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "SLOT int not null,"+
      "SOURCE_NUMBER int,"+
      "index STATION_NAME_IDX(STATION_NAME,MATRIX))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<218)&&(set_schema>cur_schema)) {
    sql=QString("alter table LIVEWIRE_GPIO_SLOTS ")+
      "add column IP_ADDRESS char(15) after SLOT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<219)&&(set_schema>cur_schema)) {
    //
    // RDAirPlay Channels
    //
    sql=QString("create table if not exists RDAIRPLAY_CHANNELS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "INSTANCE int unsigned not null,"+
      "CARD int not null default 0,"+
      "PORT int not null default 0,"+
      "START_RML char(255),"+
      "STOP_RML char(255),"+
      "START_GPI_MATRIX int not null default -1,"+
      "START_GPI_LINE int not null default -1,"+
      "START_GPO_MATRIX int not null default -1,"+
      "START_GPO_LINE int not null default -1,"+
      "STOP_GPI_MATRIX int not null default -1,"+
      "STOP_GPI_LINE int not null default -1,"+
      "STOP_GPO_MATRIX int not null default -1,"+
      "STOP_GPO_LINE int not null default -1,"+
      "index STATION_NAME_IDX(STATION_NAME,INSTANCE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(unsigned i=0;i<10;i++) {
      sql=QString().
	sprintf("select STATION,CARD%u,PORT%u,START_RML%u,STOP_RML%u ",
		i,i,i,i)+
	"from RDAIRPLAY";
      q=new RDSqlQuery(sql,false);
      while(q->next()) {
	sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("INSTANCE=%u,",i)+
	  QString().sprintf("CARD=%d,",q->value(1).toInt())+
	  QString().sprintf("PORT=%d,",q->value(2).toInt())+
	  "START_RML=\""+RDEscapeString(q->value(3).toString())+"\","+
	  "STOP_RML=\""+RDEscapeString(q->value(4).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q;
    }

    //
    // RDPanel Channels
    //
    sql=QString("create table if not exists RDPANEL_CHANNELS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "INSTANCE int unsigned not null,"+
      "CARD int not null default 0,"+
      "PORT int not null default 0,"+
      "START_RML char(255),"+
      "STOP_RML char(255),"+
      "START_GPI_MATRIX int not null default -1,"+
      "START_GPI_LINE int not null default -1,"+
      "START_GPO_MATRIX int not null default -1,"+
      "START_GPO_LINE int not null default -1,"+
      "STOP_GPI_MATRIX int not null default -1,"+
      "STOP_GPI_LINE int not null default -1,"+
      "STOP_GPO_MATRIX int not null default -1,"+
      "STOP_GPO_LINE int not null default -1,"+
      "index STATION_NAME_IDX(STATION_NAME,INSTANCE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(unsigned i=0;i<10;i++) {
      if((i==2)||(i==3)||(i==6)||(i==7)||(i==8)||(i==9)) {
	sql=QString().
	  sprintf("select STATION,CARD%u,PORT%u,START_RML%u,STOP_RML%u ",
		  i,i,i,i)+
	  "from RDPANEL";
	q=new RDSqlQuery(sql,false);
	while(q->next()) {
	  sql=QString("insert into RDPANEL_CHANNELS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("INSTANCE=%u,",i)+
	    QString().sprintf("CARD=%d,",q->value(1).toInt())+
	    QString().sprintf("PORT=%d,",q->value(2).toInt())+
	    "START_RML=\""+RDEscapeString(q->value(3).toString())+"\","+
	    "STOP_RML=\""+RDEscapeString(q->value(4).toString())+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q;
      }
    }

    //
    // Clean Up RDAirPlay
    //
    DropColumn("RDAIRPLAY","INSTANCE");
    for(unsigned i=0;i<10;i++) {
      DropColumn("RDAIRPLAY",QString().sprintf("CARD%u",i));
      DropColumn("RDAIRPLAY",QString().sprintf("PORT%u",i));
      DropColumn("RDAIRPLAY",QString().sprintf("START_RML%u",i));
      DropColumn("RDAIRPLAY",QString().sprintf("STOP%u",i));
    }
    
    //
    // Clean Up RDPanel
    //
    DropColumn("RDPANEL","INSTANCE");
    for(unsigned i=0;i<10;i++) {
      if((i==2)||(i==3)||(i==6)||(i==7)||(i==8)||(i==9)) {
	DropColumn("RDPANEL",QString().sprintf("CARD%u",i));
	DropColumn("RDPANEL",QString().sprintf("PORT%u",i));
	DropColumn("RDPANEL",QString().sprintf("START_RML%u",i));
	DropColumn("RDPANEL",QString().sprintf("STOP_RML%u",i));
      }
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<220)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY_CHANNELS add column GPIO_TYPE ")+
      "int unsigned default 0 after STOP_RML";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDPANEL_CHANNELS add column GPIO_TYPE ")+
      "int unsigned default 0 after STOP_RML";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<221)&&(set_schema>cur_schema)) {
    sql="alter table RDLIBRARY modify column TRIM_THRESHOLD int default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<222)&&(set_schema>cur_schema)) {
    //
    // Removed as redundant with update 183 [call to InsertImportFormats()].
    //

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<223)&&(set_schema>cur_schema)) {
    sql="alter table CART add column CONDUCTOR char(64) after LABEL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add index CONDUCTOR_IDX(CONDUCTOR)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add column SONG_ID char(32) after USER_DEFINED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add index SONG_ID_IDX(SONG_ID)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="alter table CART add column BPM int unsigned default 0 after SONG_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<224)&&(set_schema>cur_schema)) {
    sql=QString("alter table DROPBOXES add column SET_USER_DEFINED char(255) ")+
      "after CREATE_ENDDATE_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<225)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column TITLE_TEMPLATE char(64) ")+
      "default '%t' after HOUR_SELECTOR_ENABLED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY add column ARTIST_TEMPLATE char(64) ")+
      "default '%a' after TITLE_TEMPLATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY add column OUTCUE_TEMPLATE char(64) ")+
      "default '%o' after ARTIST_TEMPLATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY add column DESCRIPTION_TEMPLATE char(64) ")+
      "default '%i' after HOUR_SELECTOR_ENABLED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<226)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLOGEDIT add column ")+
      "ENABLE_SECOND_START enum('N','Y') default 'Y' after BITRATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<227)&&(set_schema>cur_schema)) {
    sql="alter table LOGS add index TYPE_IDX(TYPE,LOG_EXISTS)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<228)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLIBRARY add column ")+
      "LIMIT_SEARCH int default 1 after SRC_CONVERTER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add column ")+
      "SEARCH_LIMITED enum('N','Y') default 'Y' after LIMIT_SEARCH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<229)&&(set_schema>cur_schema)) {
    sql=QString("alter table SERVICES add column ")+
      "DESCRIPTION_TEMPLATE char(255) after NAME_TEMPLATE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update SERVICES set DESCRIPTION_TEMPLATE=\"")+
	RDEscapeString(q->value(0).toString())+" log for %m/%d/%Y\" "+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<230)&&(set_schema>cur_schema)) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql="alter table `"+tablename+"_LOG`"+
	" add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql="alter table `"+tablename+"_PRE`"+
	" add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql="alter table `"+tablename+"_POST`"+
	" add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<231)&&(set_schema>cur_schema)) {
    sql=QString("alter table CART add column ")+
      "USE_EVENT_LENGTH enum('N','Y') default 'N' after METADATA_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<232)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add column ")+
      "ENABLE_DRAGDROP enum('N','Y') default 'Y' after CARTSLOT_ROWS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STATIONS add column ")+
      "ENFORCE_PANEL_SETUP enum('N','Y') default 'N' after ENABLE_DRAGDROP";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<233)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column ")+
      "LOG_MODE_STYLE int default 0 after START_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists LOG_MODES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MACHINE int unsigned not null,"+
      "START_MODE int not null default 0,"+
      "OP_MODE int not null default 2,"+
      "index STATION_NAME_IDX(STATION_NAME,MACHINE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select STATION,START_MODE,OP_MODE from RDAIRPLAY");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<3;i++) {
	sql=QString("insert into LOG_MODES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%u,",i)+
	  QString().sprintf("START_MODE=%d,",q->value(1).toInt())+
	  QString().sprintf("OP_MODE=%d",q->value(2).toInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<234)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add column ")+
      "CUE_START_CART int unsigned after CUE_PORT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STATIONS add column ")+
      "CUE_STOP_CART int unsigned after CUE_START_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<235)&&(set_schema>cur_schema)) {
    // Lock Locking Changes, Superceded

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<236)&&(set_schema>cur_schema)) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT`"+
	" add column CONDUCTOR char(64) after LABEL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_SRT`"+
	" add column USER_DEFINED char(255) after COMPOSER";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_SRT`"+
	" add column SONG_ID char(32) after USER_DEFINED";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<237)&&(set_schema>cur_schema)) {
    sql=QString("alter table REPORTS add column ")+
      "START_TIME time after FILTER_GROUPS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORTS add column ")+
      "END_TIME time after START_TIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<238)&&(set_schema>cur_schema)) {
    sql=QString("alter table CART add column ")+
      "PENDING_STATION char(64) after USE_EVENT_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART add column ")+
      "PENDING_PID int after PENDING_STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART add column ")+
      "PENDING_DATETIME datetime after PENDING_PID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART add index ")+
      "PENDING_STATION_IDX(PENDING_STATION)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART add index ")+
      "PENDING_PID_IDX(PENDING_STATION,PENDING_PID)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART add index ")+
      "PENDING_DATETIME_IDX(PENDING_DATETIME)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<239)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists DROPBOX_SCHED_CODES(")+
      "ID int auto_increment not null primary key,"+
      "DROPBOX_ID int not null,"+
      "SCHED_CODE char(11) not null,"
      "index DROPBOX_ID_IDX(DROPBOX_ID),"+
      "index SCHED_CODE_IDX(SCHED_CODE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<240)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists GPIO_EVENTS(")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "TYPE int not null,"+
      "EDGE int not null,"+
      "EVENT_DATETIME datetime not null,"+
      "index STATION_NAME_IDX(STATION_NAME,MATRIX,TYPE,EVENT_DATETIME,EDGE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<241)&&(set_schema>cur_schema)) {
    sql=QString("alter table EVENTS add column ")+
      "HAVE_CODE2 varchar(10) after HAVE_CODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<242)&&(set_schema>cur_schema)) {
    sql=QString("alter table REPORTS add column ")+
      "POST_EXPORT_CMD text after EXPORT_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORTS add column ")+
      "WIN_POST_EXPORT_CMD text after WIN_EXPORT_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<243)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add column ")+
      "HAVE_MP4_DECODE enum('N','Y') default 'N' after HAVE_MPG321";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<244)&&(set_schema>cur_schema)) {
    sql=QString("alter table JACK_CLIENTS modify column ")+
      "COMMAND_LINE text not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<245)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLIBRARY add column ")+
      "READ_ISRC enum('N','Y') default 'Y' after CDDB_SERVER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<246)&&(set_schema>cur_schema)) {
  // Versions 246 - 253 are reserved

    cur_schema+=8;
  }

  if((cur_schema<254)&&(set_schema>cur_schema)) {
    sql=QString("alter table CART add column ")+
      "USE_WEIGHTING enum('N','Y') default 'Y' after ENFORCE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CUTS add column PLAY_ORDER int after WEIGHT");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select CUT_NAME from CUTS order by CUT_NAME");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update CUTS set ")+
	"PLAY_ORDER="+q->value(0).toString().right(3)+" "+
	"where CUT_NAME=\""+q->value(0).toString()+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<255)&&(set_schema>cur_schema)) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT`"+
	" add column DESCRIPTION char(64) after USAGE_CODE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table `")+tablename+"_SRT`"+
	" add column OUTCUE char(64) after DESCRIPTION";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<256)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists CUT_EVENTS(")+
      "ID int auto_increment not null primary key,"+
      "CUT_NAME char(12) not null,"+
      "NUMBER int not null,"+
      "POINT int not null,"+
      "index CUT_NAME_IDX(CUT_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists DECK_EVENTS(")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CHANNEL int unsigned not null,"+
      "NUMBER int not null,"+
      "CART_NUMBER int unsigned not null default 0,"+
      "index STATION_NAME_IDX(STATION_NAME,CHANNEL))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i++) {
	for(unsigned j=0;j<MAX_DECKS;j++) {
	  sql=QString("insert into DECK_EVENTS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CHANNEL=%u,",j+129)+
	    QString().sprintf("NUMBER=%u",i+1);
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<257)&&(set_schema>cur_schema)) {
    sql=QString("alter table LOGS modify column LINK_DATETIME datetime");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS modify column START_DATE date");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS modify column END_DATE date");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<258)&&(set_schema>cur_schema)) {
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table ")+
	"`"+tablename+"_LOG` "+
	"modify column CART_NUMBER int unsigned not null default 0";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<259)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists SERVICE_CLOCKS(")+
      "ID int auto_increment not null primary key,"+
      "SERVICE_NAME char(10) not null,"+
      "HOUR int not null,"+
      "CLOCK_NAME char(64) default null,"+
      "index SERVICE_NAME_IDX(SERVICE_NAME,HOUR),"+
      "index CLOCK_NAME_IDX(CLOCK_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME,");
    for(int i=0;i<168;i++) {
      sql+=QString().sprintf("CLOCK%i,",i);
    }
    sql=sql.left(sql.length()-1);
    sql+=" from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<168;i++) {
	sql=QString("insert into SERVICE_CLOCKS set ")+
	  "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("HOUR=%d,",i);
	if(q->value(i+1).isNull()) {
	  sql+="CLOCK_NAME=null";
	}
	else {
	  sql+="CLOCK_NAME=\""+RDEscapeString(q->value(i+1).toString())+"\"";
	}
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    for(int i=0;i<168;i++) {
      DropColumn("SERVICES",QString().sprintf("CLOCK%d",i));
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<260)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists WEBAPI_AUTHS(")+
      "TICKET char(41) not null primary key,"+
      "LOGIN_NAME char(255) not null,"+
      "IPV4_ADDRESS char(16) not null,"+
      "EXPIRATION_DATETIME datetime not null,"+
      "index TICKET_IDX(TICKET,IPV4_ADDRESS,EXPIRATION_DATETIME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USERS add column ")+
      "WEBAPI_AUTH_TIMEOUT int not null default 3600 after PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<261)&&(set_schema>cur_schema)) {
    sql=QString("alter table CUTS add column ")+
      "SHA1_HASH char(40) after LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql="create index SHA1_HASH_IDX on CUTS(SHA1_HASH)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<262)&&(set_schema>cur_schema)) {
    sql=QString("alter table USERS add column ")+
      "LOCAL_AUTH enum('N','Y') default 'Y' after ENABLE_WEB";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USERS add column ")+
      "PAM_SERVICE char(32) default \"rivendell\" after LOCAL_AUTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create index IPV4_ADDRESS_IDX on STATIONS (IPV4_ADDRESS)");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<263)&&(set_schema>cur_schema)) { 
    // Add missing LOG_MODES records
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<3;i++) {
	sql=QString("select ID from LOG_MODES where ")+
	  "(STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\")&&"+
	  QString().sprintf("(MACHINE=%d)",i);
	q1=new RDSqlQuery(sql,false);
	if(!q1->first()) {
	  sql=QString("insert into LOG_MODES set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("MACHINE=%d",i);
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<264)&&(set_schema>cur_schema)) { 
    sql=QString("alter table SYSTEM add column ")+
      "FIX_DUP_CART_TITLES enum('N','Y') not null default 'Y' after "+
      "DUP_CART_TITLES";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<265)&&(set_schema>cur_schema)) { 
    sql=QString("alter table SYSTEM add column ")+
      "SHOW_USER_LIST enum('N','Y') not null default 'Y' "+
      "after TEMP_CART_GROUP";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<266)&&(set_schema>cur_schema)) { 
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("select `")+tablename+"_LOG`"+
	".ID from "+
	"`"+tablename+"_LOG` left join CART "+
	"on `"+tablename+"_LOG`"+
	".CART_NUMBER=CART.NUMBER where "+
	"CART.OWNER is not null";
      q1=new RDSqlQuery(sql,false);
      int completed=q1->size();
      delete q1;

      sql=QString("select ID from `")+tablename+"_LOG` where TYPE=6";
      q1=new RDSqlQuery(sql,false);
      int scheduled=q1->size()+completed;
      delete q1;

      sql=QString("update LOGS set ")+
	QString().sprintf("SCHEDULED_TRACKS=%d,",scheduled)+
	QString().sprintf("COMPLETED_TRACKS=%u ",completed)+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<267)&&(set_schema>cur_schema)) { 
    sql=QString("alter table CUTS add column ORIGIN_LOGIN_NAME char(255) ")+
      "after ORIGIN_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CUTS add column SOURCE_HOSTNAME char(255) ")+
      "after ORIGIN_LOGIN_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<268)&&(set_schema>cur_schema)) { 
    sql=QString("alter table DROPBOXES add column ")+
      "FORCE_TO_MONO enum('N','Y') default 'N' after TO_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<269)&&(set_schema>cur_schema)) { 
    sql=QString("alter table GROUPS add column ")+
      "DEFAULT_CUT_LIFE int default -1 after DEFAULT_HIGH_CART";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<270)&&(set_schema>cur_schema)) { 
    sql=QString("alter table STATIONS add column ")+
      "SHORT_NAME char(64) after NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update STATIONS set ")+
	"SHORT_NAME=\""+RDEscapeString(q->value(0).toString())+"\" where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<271)&&(set_schema>cur_schema)) { 
    sql=QString("alter table DROPBOXES ")+
      "add column SEGUE_LEVEL int(11) default 1 after FORCE_TO_MONO, "+
      "add column SEGUE_LENGTH int(11) default 0 after SEGUE_LEVEL";
    q=new RDSqlQuery(sql);
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<272)&&(set_schema>cur_schema)) { 
    sql=QString("create table if not exists USER_SERVICE_PERMS (")+
      "ID int auto_increment not null primary key,"+
      "USER_NAME char(255) not null,"+
      "SERVICE_NAME char(10) not null,"+
      "index USER_NAME_IDX(USER_NAME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select LOGIN_NAME from USERS");
     q=new RDSqlQuery(sql,false);
     while(q->next()) {
       sql=QString("select NAME from SERVICES");
       q1=new RDSqlQuery(sql,false);
       while(q1->next()) {
	 sql=QString("insert into USER_SERVICE_PERMS set ")+
	   "USER_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	   "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	 if(!RDSqlQuery::apply(sql,err_msg)) {
	   return false;
	 }
       }
       delete q1;
     }
     delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<273)&&(set_schema>cur_schema)) { 
    sql=QString("alter table LOGS ")+
      "add column LOCK_USER_NAME char(255) after NEXT_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS ")+
      "add column LOCK_STATION_NAME char(64) after LOCK_USER_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS ")+
      "add column LOCK_IPV4_ADDRESS char(16) after LOCK_STATION_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS ")+
      "add column LOCK_DATETIME datetime after LOCK_IPV4_ADDRESS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<274)&&(set_schema>cur_schema)) { 
    sql=QString("alter table LOGS ")+
      "add column LOCK_GUID char(82) after LOCK_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS add index LOCK_GUID_IDX(LOCK_GUID)");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<275)&&(set_schema>cur_schema)) { 
    sql=QString("alter table SERVICES ")+
      "add column LOG_SHELFLIFE_ORIGIN int default 0 "+
      "after DEFAULT_LOG_SHELFLIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<276)&&(set_schema>cur_schema)) { 
    sql=QString("alter table SYSTEM ")+
      "add column NOTIFICATION_ADDRESS char(15) default \""+
      RD_NOTIFICATION_ADDRESS+"\" after SHOW_USER_LIST";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<277)&&(set_schema>cur_schema)) { 
    sql=QString("alter table USERS ")+
      "add column WEBGET_LOGIN_PRIV enum('N','Y') not null default 'N' "+
      "after EDIT_AUDIO_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<278)&&(set_schema>cur_schema)) { 
    sql=QString("create table if not exists LOG_MACHINES (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MACHINE int not null,"+
      "START_MODE int not null default 0,"+
      "AUTO_RESTART enum('N','Y') not null default 'N',"+
      "LOG_NAME char(64),"+
      "CURRENT_LOG char(64),"+
      "RUNNING enum('N','Y') not null default 'N',"+
      "LOG_ID int not null default -1,"+
      "LOG_LINE int not null default -1,"+
      "NOW_CART int unsigned not null default 0,"+
      "NEXT_CART int unsigned not null default 0,"+
      "UDP_ADDR char(255),"+
      "UDP_PORT int unsigned,"+
      "UDP_STRING char(255),"+
      "LOG_RML char(255),"+
      "index STATION_NAME_IDX(STATION_NAME,MACHINE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(int i=0;i<3;i++) {
      sql=QString("select ")+
	"STATION,"+                                  // 00
	QString().sprintf("LOG%d_START_MODE,",i)+    // 01
	QString().sprintf("LOG%d_AUTO_RESTART,",i)+  // 02
	QString().sprintf("LOG%d_LOG_NAME,",i)+      // 03
	QString().sprintf("LOG%d_CURRENT_LOG,",i)+   // 04
	QString().sprintf("LOG%d_RUNNING,",i)+       // 05
	QString().sprintf("LOG%d_LOG_ID,",i)+        // 06
	QString().sprintf("LOG%d_LOG_LINE,",i)+      // 07
	QString().sprintf("LOG%d_NOW_CART,",i)+      // 08
	QString().sprintf("LOG%d_NEXT_CART,",i)+     // 09
	QString().sprintf("UDP_ADDR%d,",i)+          // 10
	QString().sprintf("UDP_PORT%d,",i)+          // 11
	QString().sprintf("UDP_STRING%d,",i)+        // 12
	QString().sprintf("LOG_RML%d ",i)+           // 13
	"from RDAIRPLAY order by STATION";
      q=new RDSqlQuery(sql,false);
      while(q->next()) {
	sql=QString("insert into LOG_MACHINES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%d,",i)+
	  QString().sprintf("START_MODE=%d,",q->value(1).toInt())+
	  "AUTO_RESTART=\""+q->value(2).toString()+"\","+
	  "LOG_NAME=\""+RDEscapeString(q->value(3).toString())+"\","+
	  "CURRENT_LOG=\""+RDEscapeString(q->value(4).toString())+"\","+
	  "RUNNING=\""+q->value(5).toString()+"\","+
	  QString().sprintf("LOG_ID=%d,",q->value(6).toInt())+
	  QString().sprintf("LOG_LINE=%d,",q->value(7).toInt())+
	  QString().sprintf("NOW_CART=%u,",q->value(8).toUInt())+
	  QString().sprintf("NEXT_CART=%u,",q->value(9).toUInt())+
	  "UDP_ADDR=\""+RDEscapeString(q->value(10).toString())+"\","+
	  QString().sprintf("UDP_PORT=%u,",q->value(11).toUInt())+
	  "UDP_STRING=\""+RDEscapeString(q->value(12).toString())+"\","+
	  "LOG_RML=\""+RDEscapeString(q->value(13).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q;
    }

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=RD_RDVAIRPLAY_LOG_BASE;i<(RD_RDVAIRPLAY_LOG_BASE+RD_RDVAIRPLAY_LOG_QUAN);i++) {
	sql=QString("insert into LOG_MACHINES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    for(int i=0;i<3;i++) {
      DropColumn("RDAIRPLAY",QString().sprintf("UDP_ADDR%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("UDP_PORT%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("UDP_STRING%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG_RML%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_START_MODE",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_AUTO_RESTART",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_LOG_NAME",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_CURRENT_LOG",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_RUNNING",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_LOG_ID",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_LOG_LINE",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_NOW_CART",i));
      DropColumn("RDAIRPLAY",QString().sprintf("LOG%d_NEXT_CART",i));
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<279)&&(set_schema>cur_schema)) { 
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
	sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("INSTANCE=%u",i+RD_RDVAIRPLAY_LOG_BASE);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<280)&&(set_schema>cur_schema)) { 
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
	sql=QString("insert into LOG_MODES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%u",i+RD_RDVAIRPLAY_LOG_BASE);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<281)&&(set_schema>cur_schema)) { 
    sql=QString("alter table RDAIRPLAY add column ")+
      "VIRTUAL_EXIT_CODE int default 0 after EXIT_CODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<282)&&(set_schema>cur_schema)) { 
    sql=QString("create table if not exists AUDIO_CARDS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "DRIVER int not null default 0,"+
      "NAME char(64),"+
      "INPUTS int not null default -1,"+
      "OUTPUTS int not null default -1,"+
      "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<RD_MAX_CARDS;i++) {
	sql=QString("insert into AUDIO_CARDS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("CARD_NUMBER=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    for(int i=0;i<8;i++) {
      DropColumn("STATIONS",QString().sprintf("CARD%d_DRIVER",i));
      DropColumn("STATIONS",QString().sprintf("CARD%d_NAME",i));
      DropColumn("STATIONS",QString().sprintf("CARD%d_INPUTS",i));
      DropColumn("STATIONS",QString().sprintf("CARD%d_OUTPUTS",i));
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<283)&&(set_schema>cur_schema)) { 
    sql=QString("alter table AUDIO_CARDS add column ")+
      "CLOCK_SOURCE int not null default 0 after OUTPUTS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "STATION_NAME,"+
      "CARD_NUMBER,"+
      "CLOCK_SOURCE "+
      "from AUDIO_PORTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_CARDS set ")+
	QString().sprintf("CLOCK_SOURCE=%d where ",q->value(2).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("create table if not exists AUDIO_INPUTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "PORT_NUMBER int not null,"+
      "LEVEL int not null default 0,"+
      "TYPE int not null default 0,"
      "MODE int not null default 0,"+
      "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER,PORT_NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<24;i++) {
	for(int j=0;j<24;j++) {
	  sql=QString("insert into AUDIO_INPUTS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CARD_NUMBER=%d,",i)+
	    QString().sprintf("PORT_NUMBER=%d",j);
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
      }
    }
    delete q;
    for(int i=0;i<8;i++) {
      sql=QString("select ")+
	"STATION_NAME,"+                        // 00
	"CARD_NUMBER,"+                         // 01
	QString().sprintf("INPUT_%d_LEVEL,",i)+  // 02
	QString().sprintf("INPUT_%d_TYPE,",i)+   // 03
	QString().sprintf("INPUT_%d_MODE ",i)+   // 04
	"from AUDIO_PORTS";
      q=new RDSqlQuery(sql,false);
      while(q->next()) {
	sql=QString("update AUDIO_INPUTS set ")+
	  QString().sprintf("LEVEL=%d,",q->value(2).toInt())+
	  QString().sprintf("TYPE=%d,",q->value(3).toInt())+
	  QString().sprintf("MODE=%d where ",q->value(4).toInt())+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	  QString().sprintf("CARD_NUMBER=%d && ",q->value(1).toInt())+
	  QString().sprintf("PORT_NUMBER=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q;
    }

    sql=QString("create table if not exists AUDIO_OUTPUTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "PORT_NUMBER int not null,"+
      "LEVEL int not null default 0,"+
      "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER,PORT_NUMBER))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<24;i++) {
	for(int j=0;j<24;j++) {
	  sql=QString("insert into AUDIO_OUTPUTS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CARD_NUMBER=%d,",i)+
	    QString().sprintf("PORT_NUMBER=%d",j);
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
      }
    }
    for(int i=0;i<8;i++) {
      sql=QString("select ")+
	"STATION_NAME,"+                          // 00
	"CARD_NUMBER,"+                           // 01
	QString().sprintf("OUTPUT_%d_LEVEL ",i)+  // 02
	"from AUDIO_PORTS";
      q=new RDSqlQuery(sql,false);
      while(q->next()) {
	sql=QString("update AUDIO_OUTPUTS set ")+
	  QString().sprintf("LEVEL=%d where ",q->value(2).toInt())+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	  QString().sprintf("CARD_NUMBER=%d && ",q->value(1).toInt())+
	  QString().sprintf("PORT_NUMBER=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q;
    }

    if(!DropTable("AUDIO_PORTS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<284)&&(set_schema>cur_schema)) { 
    for(int i=0;i<10;i++) {
      DropColumn("RDAIRPLAY",QString().sprintf("CARD%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("PORT%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("START_RML%d",i));
      DropColumn("RDAIRPLAY",QString().sprintf("STOP_RML%d",i));
    }
    DropColumn("RDAIRPLAY","OP_MODE");
    DropColumn("RDAIRPLAY","START_MODE");
    DropColumn("RDPANEL","INSTANCE");
    for(int i=2;i<4;i++) {
      DropColumn("RDPANEL",QString().sprintf("CARD%d",i));
      DropColumn("RDPANEL",QString().sprintf("PORT%d",i));
      DropColumn("RDPANEL",QString().sprintf("START_RML%d",i));
      DropColumn("RDPANEL",QString().sprintf("STOP_RML%d",i));
    }

    for(int i=6;i<10;i++) {
      DropColumn("RDPANEL",QString().sprintf("CARD%d",i));
      DropColumn("RDPANEL",QString().sprintf("PORT%d",i));
      DropColumn("RDPANEL",QString().sprintf("START_RML%d",i));
      DropColumn("RDPANEL",QString().sprintf("STOP_RML%d",i));
    }

    sql=QString("alter table MATRICES alter column PORT set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column PORT_2 set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column INPUTS set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column OUTPUTS set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column GPIS set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column GPOS set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPLICATORS alter column TYPE_ID set default 0");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<285)&&(set_schema>cur_schema)) {
    DropColumn("CART","ISRC");
    DropColumn("DECKS","DEFAULT_SAMPRATE");
    DropColumn("RDLIBRARY","INPUT_STREAM");
    DropColumn("RDLIBRARY","OUTPUT_STREAM");
    DropColumn("RDLIBRARY","RECORD_GPI");
    DropColumn("RDLIBRARY","PLAY_GPI");
    DropColumn("RDLIBRARY","STOP_GPI");
    DropColumn("RDLIBRARY","DEFAULT_SAMPRATE");
    DropColumn("RDLOGEDIT","SAMPRATE");
    DropColumn("SERVICES","TFC_START_OFFSET");
    DropColumn("SERVICES","TFC_START_LENGTH");
    DropColumn("SERVICES","MUS_START_OFFSET");
    DropColumn("SERVICES","MUS_START_LENGTH");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<286)&&(set_schema>cur_schema)) { 
    DropColumn("SERVICES","TFC_LENGTH_OFFSET");
    DropColumn("SERVICES","TFC_LENGTH_LENGTH");
    DropColumn("SERVICES","MUS_LENGTH_OFFSET");
    DropColumn("SERVICES","MUS_LENGTH_LENGTH");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<287)&&(set_schema>cur_schema)) { 
    sql=QString("alter table STATIONS add column ")+
      "JACK_PORTS int not null default 8 after JACK_COMMAND_LINE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<288)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists LOG_LINES (")+
      "ID int auto_increment not null primary key,"+
      "LOG_NAME char(64) not null,"+
      "LINE_ID int not null,"+
      "COUNT int not null,"+
      "TYPE int default 0,"+
      "SOURCE int not null,"+
      "START_TIME int,"+
      "GRACE_TIME int default 0,"+
      "CART_NUMBER int UNSIGNED not null default 0,"+
      "TIME_TYPE int not null,"+
      "POST_POINT enum('N','Y') default 'N',"+
      "TRANS_TYPE int not null,"+
      "START_POINT int not null default -1,"+
      "END_POINT int not null default -1,"+
      "FADEUP_POINT int default -1,"+
      QString().sprintf("FADEUP_GAIN int default %d,",RD_FADE_DEPTH)+
      "FADEDOWN_POINT int default -1,"+
      QString().sprintf("FADEDOWN_GAIN int default %d,",RD_FADE_DEPTH)+
      "SEGUE_START_POINT int not null default -1,"+
      "SEGUE_END_POINT int not null default -1,"+
      QString().sprintf("SEGUE_GAIN int default %d,",RD_FADE_DEPTH)+
      "DUCK_UP_GAIN int default 0,"+
      "DUCK_DOWN_GAIN int default 0,"+
      "COMMENT CHAR(255),"+
      "LABEL CHAR(64),"+
      "ORIGIN_USER char(255),"+
      "ORIGIN_DATETIME datetime,"+
      "EVENT_LENGTH int default -1,"+
      "LINK_EVENT_NAME char(64),"+
      "LINK_START_TIME int,"+
      "LINK_LENGTH int default 0,"+
      "LINK_START_SLOP int default 0,"+
      "LINK_END_SLOP int default 0,"+
      "LINK_ID int default -1,"+
      "LINK_EMBEDDED enum('N','Y') default 'N',"+
      "EXT_START_TIME time,"+
      "EXT_LENGTH int,"+
      "EXT_CART_NAME char(32),"+
      "EXT_DATA char(32),"+
      "EXT_EVENT_ID char(32),"+
      "EXT_ANNC_TYPE char(8),"+
      "unique index LOG_NAME_IDX (LOG_NAME,COUNT),"+
      "index CART_NUMBER_IDX (CART_NUMBER),"+
      "index LABEL_IDX (LABEL))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_LOG";
      tablename.replace(" ","_");
      if(!TableExists(tablename)) {
	fprintf(stderr,"rddbmgr: missing LOG table for log \"%s\"\n",
		(const char *)q->value(0).toString().toUtf8());
      }
      else {
	sql=QString("select ")+
	  "ID,"+                 // 00
	  "COUNT,"+              // 01
	  "TYPE,"+               // 02
	  "SOURCE,"+             // 03
	  "START_TIME,"+         // 04
	  "GRACE_TIME,"+         // 05
	  "CART_NUMBER,"+        // 06
	  "TIME_TYPE,"+          // 07
	  "POST_POINT,"+         // 08
	  "TRANS_TYPE,"+         // 09
	  "START_POINT,"+        // 10
	  "END_POINT,"+          // 11
	  "FADEUP_POINT,"+       // 12
	  "FADEUP_GAIN,"+        // 13
	  "FADEDOWN_POINT,"+     // 14
	  "FADEDOWN_GAIN,"+      // 15
	  "SEGUE_START_POINT,"+  // 16
	  "SEGUE_END_POINT,"+    // 17
	  "SEGUE_GAIN,"+         // 18
	  "DUCK_UP_GAIN,"+       // 19
	  "DUCK_DOWN_GAIN,"+     // 20
	  "COMMENT,"+            // 21
	  "LABEL,"+              // 22
	  "ORIGIN_USER,"+        // 23
	  "ORIGIN_DATETIME,"+    // 24
	  "EVENT_LENGTH,"+       // 25
	  "LINK_EVENT_NAME,"+    // 26
	  "LINK_START_TIME,"+    // 27
	  "LINK_LENGTH,"+        // 28
	  "LINK_START_SLOP,"+    // 29
	  "LINK_END_SLOP,"+      // 30
	  "LINK_ID,"+            // 31
	  "LINK_EMBEDDED,"+      // 32
	  "EXT_START_TIME,"+     // 33
	  "EXT_LENGTH,"+         // 34
	  "EXT_CART_NAME,"+      // 35
	  "EXT_DATA,"+           // 36
	  "EXT_EVENT_ID,"+       // 37
	  "EXT_ANNC_TYPE "+      // 38
	  "from `"+tablename+"` order by COUNT";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into LOG_LINES set ")+
	    "LOG_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("LINE_ID=%d,",q1->value(0).toInt())+
	    QString().sprintf("COUNT=%d,",q1->value(1).toInt())+
	    QString().sprintf("TYPE=%d,",q1->value(2).toInt())+
	    QString().sprintf("SOURCE=%d,",q1->value(3).toInt())+
	    QString().sprintf("START_TIME=%d,",q1->value(4).toInt())+
	    QString().sprintf("GRACE_TIME=%d,",q1->value(5).toInt())+
	    QString().sprintf("CART_NUMBER=%u,",q1->value(6).toUInt())+
	    QString().sprintf("TIME_TYPE=%d,",q1->value(7).toInt())+
	    "POST_POINT=\""+RDEscapeString(q1->value(8).toString())+"\","+
	    QString().sprintf("TRANS_TYPE=%d,",q1->value(9).toInt())+
	    QString().sprintf("START_POINT=%d,",q1->value(10).toInt())+
	    QString().sprintf("END_POINT=%d,",q1->value(11).toInt())+
	    QString().sprintf("FADEUP_POINT=%d,",q1->value(12).toInt())+
	    QString().sprintf("FADEUP_GAIN=%d,",q1->value(13).toInt())+
	    QString().sprintf("FADEDOWN_POINT=%d,",q1->value(14).toInt())+
	    QString().sprintf("FADEDOWN_GAIN=%d,",q1->value(15).toInt())+
	    QString().sprintf("SEGUE_START_POINT=%d,",q1->value(16).toInt())+
	    QString().sprintf("SEGUE_END_POINT=%d,",q1->value(17).toInt())+
	    QString().sprintf("SEGUE_GAIN=%d,",q1->value(18).toInt())+
	    QString().sprintf("DUCK_UP_GAIN=%d,",q1->value(19).toInt())+
	    QString().sprintf("DUCK_DOWN_GAIN=%d,",q1->value(20).toInt())+
	    "COMMENT=\""+RDEscapeString(q1->value(21).toString())+"\","+
	    "LABEL=\""+RDEscapeString(q1->value(22).toString())+"\","+
	    "ORIGIN_USER=\""+RDEscapeString(q1->value(23).toString())+"\","+
	    "ORIGIN_DATETIME="+RDCheckDateTime(q1->value(24).toDateTime(),
	    "yyyy-MM-dd hh:mm:ss")+","+
	    QString().sprintf("EVENT_LENGTH=%d,",q1->value(25).toInt())+
	    "LINK_EVENT_NAME=\""+RDEscapeString(q1->value(26).toString())+"\","+
	    QString().sprintf("LINK_START_TIME=%d,",q1->value(27).toInt())+
	    QString().sprintf("LINK_LENGTH=%d,",q1->value(28).toInt())+
	    QString().sprintf("LINK_START_SLOP=%d,",q1->value(29).toInt())+
	    QString().sprintf("LINK_END_SLOP=%d,",q1->value(30).toInt())+
	    QString().sprintf("LINK_ID=%d,",q1->value(31).toInt())+
	    "LINK_EMBEDDED=\""+RDEscapeString(q1->value(32).toString())+"\","+
	    "EXT_START_TIME="+
	    RDCheckDateTime(q1->value(33).toTime(),"hh:mm:ss")+","+
	    QString().sprintf("EXT_LENGTH=%d,",q1->value(34).toInt())+
	    "EXT_CART_NAME=\""+RDEscapeString(q1->value(35).toString())+"\","+
	    "EXT_DATA=\""+RDEscapeString(q1->value(36).toString())+"\","+
	    "EXT_EVENT_ID=\""+RDEscapeString(q1->value(37).toString())+"\","+
	    "EXT_ANNC_TYPE=\""+RDEscapeString(q1->value(38).toString())+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    //
    // Delete orphaned log tables
    //
    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_LOG\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned LOG table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<289)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists ELR_LINES (")+
      "ID int unsigned auto_increment primary key,"+
      "SERVICE_NAME char(10) not null,"+
      "EVENT_DATETIME datetime not null,"+
      "LENGTH int,"+
      "LOG_NAME char(64),"+
      "LOG_ID int,"+
      "CART_NUMBER int unsigned,"+
      "CUT_NUMBER int,"+
      "TITLE char(255),"+
      "ARTIST char(255),"+
      "PUBLISHER char(64),"+
      "COMPOSER char(64),"+
      "USER_DEFINED char(255),"+
      "SONG_ID char(32),"+
      "ALBUM char(255),"+
      "LABEL char(64),"+
      "CONDUCTOR char(64),"+
      "USAGE_CODE int,"+
      "DESCRIPTION char(64),"+
      "OUTCUE char(64),"+
      "ISRC char(12),"+
      "ISCI char(32),"+
      "STATION_NAME char(64),"+
      "SCHEDULED_TIME time,"+
      "EVENT_TYPE int,"+
      "EVENT_SOURCE int,"+
      "PLAY_SOURCE int,"+
      "START_SOURCE int default 0,"+
      "ONAIR_FLAG enum('N','Y') default 'N',"+
      "EXT_START_TIME time,"+
      "EXT_LENGTH int,"+
      "EXT_CART_NAME char(32),"+
      "EXT_DATA char(32),"+
      "EXT_EVENT_ID char(8),"+
      "EXT_ANNC_TYPE char(8),"+
      "index SERVICE_NAME_EVENT_DATETIME_IDX(SERVICE_NAME,EVENT_DATETIME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_SRT";
      tablename.replace(" ","_");
      sql=QString("select ")+
	"LENGTH,"+          // 00
	"LOG_NAME,"+        // 01
	"LOG_ID,"+          // 02
	"CART_NUMBER,"+     // 03
	"CUT_NUMBER,"+      // 04
	"TITLE,"+           // 05
	"ARTIST,"+          // 06
	"PUBLISHER,"+       // 07
	"COMPOSER,"+        // 08
	"USER_DEFINED,"+    // 09
	"SONG_ID,"+         // 10
	"ALBUM,"+           // 11
	"LABEL,"+           // 12
	"CONDUCTOR,"+       // 13
	"USAGE_CODE,"+      // 14
	"DESCRIPTION,"+     // 15
	"OUTCUE,"+          // 16
	"ISRC,"+            // 17
	"ISCI,"+            // 18
	"STATION_NAME,"+    // 19
	"EVENT_DATETIME,"+  // 20
	"SCHEDULED_TIME,"+  // 21
	"EVENT_TYPE,"+      // 22
	"EVENT_SOURCE,"+    // 23
	"PLAY_SOURCE,"+     // 24
	"START_SOURCE,"+    // 25
	"ONAIR_FLAG,"+      // 26
	"EXT_START_TIME,"+  // 27
	"EXT_LENGTH,"+      // 28
	"EXT_CART_NAME,"+   // 29
	"EXT_DATA,"+        // 30
	"EXT_EVENT_ID,"+    // 31
	"EXT_ANNC_TYPE "+   // 32
	"from `"+tablename+"`";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into ELR_LINES set ")+
	  "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("LENGTH=%d,",q1->value(0).toInt())+
	  "LOG_NAME=\""+RDEscapeString(q1->value(1).toString())+"\","+
	  QString().sprintf("LOG_ID=%d,",q1->value(2).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(3).toUInt())+
	  QString().sprintf("CUT_NUMBER=%d,",q1->value(4).toInt())+
	  "TITLE=\""+RDEscapeString(q1->value(5).toString())+"\","+
	  "ARTIST=\""+RDEscapeString(q1->value(6).toString())+"\","+
	  "PUBLISHER=\""+RDEscapeString(q1->value(7).toString())+"\","+
	  "COMPOSER=\""+RDEscapeString(q1->value(8).toString())+"\","+
	  "USER_DEFINED=\""+RDEscapeString(q1->value(9).toString())+"\","+
	  "SONG_ID=\""+RDEscapeString(q1->value(10).toString())+"\","+
	  "ALBUM=\""+RDEscapeString(q1->value(11).toString())+"\","+
	  "LABEL=\""+RDEscapeString(q1->value(12).toString())+"\","+
	  "CONDUCTOR=\""+RDEscapeString(q1->value(13).toString())+"\","+
	  "USAGE_CODE=\""+RDEscapeString(q1->value(14).toString())+"\","+
	  "DESCRIPTION=\""+RDEscapeString(q1->value(15).toString())+"\","+
	  "OUTCUE=\""+RDEscapeString(q1->value(16).toString())+"\","+
	  "ISRC=\""+RDEscapeString(q1->value(17).toString())+"\","+
	  "ISCI=\""+RDEscapeString(q1->value(18).toString())+"\","+
	  "STATION_NAME=\""+RDEscapeString(q1->value(19).toString())+"\","+
	  "EVENT_DATETIME=\""+RDEscapeString(q1->value(20).toDateTime().
				     toString("yyyy-MM-dd hh:mm:ss"))+"\","+
	  "SCHEDULED_TIME="+
	  RDCheckDateTime(q1->value(21).toTime(),"hh:mm:ss")+","+
	  QString().sprintf("EVENT_TYPE=%d,",q1->value(22).toInt())+
	  QString().sprintf("EVENT_SOURCE=%d,",q1->value(23).toInt())+
	  QString().sprintf("PLAY_SOURCE=%d,",q1->value(24).toInt())+
	  QString().sprintf("START_SOURCE=%d,",q1->value(25).toInt())+
	  "ONAIR_FLAG=\""+RDEscapeString(q1->value(26).toString())+"\","+
	  "EXT_START_TIME="+
	  RDCheckDateTime(q1->value(27).toTime(),"hh:mm:ss")+","+
	  QString().sprintf("EXT_LENGTH=%d,",q1->value(28).toInt())+
	  "EXT_CART_NAME=\""+RDEscapeString(q1->value(29).toString())+"\","+
	  "EXT_DATA=\""+RDEscapeString(q1->value(30).toString())+"\","+
	  "EXT_EVENT_ID=\""+RDEscapeString(q1->value(31).toString())+"\","+
	  "EXT_ANNC_TYPE=\""+RDEscapeString(q1->value(32).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
      if(!DropTable(tablename,err_msg)) {
	return false;
      }
    }
    delete q;

    //
    // Delete orphaned elr tables
    //
    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_SRT\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned SRT table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<290)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists EVENT_LINES (")+
      "ID int unsigned auto_increment primary key,"+
      "EVENT_NAME char(64) not null,"+
      "TYPE int not null,"+
      "COUNT int not null,"+
      "EVENT_TYPE int not null,"+
      "CART_NUMBER int unsigned,"+
      "TRANS_TYPE int not null,"+
      "MARKER_COMMENT char(255),"+
      "unique index EVENT_NAME_TYPE_COUNT_IDX (EVENT_NAME,TYPE,COUNT))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from EVENTS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      //
      // PreImport Events
      //
      QString tablename=q->value(0).toString()+"_PRE";
      tablename.replace(" ","_");
      if(!TableExists(tablename)) {
	fprintf(stderr,"rddbmgr: missing PRE table for event \"%s\"\n",
		(const char *)q->value(0).toString().toUtf8());
      }
      else {
	sql=QString("select ")+
	  "COUNT,"+        // 00
	  "TYPE,"+         // 01
	  "CART_NUMBER,"+  // 02
	  "TRANS_TYPE,"+   // 03
	  "COMMENT "+      // 05
	  "from `"+tablename+"`"+
	  "order by COUNT";
	q1=new RDSqlQuery(sql,false);
	while(q1->next()) {
	  sql=QString("insert into EVENT_LINES set ")+
	    "EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    "TYPE=0,"+
	    QString().sprintf("COUNT=%d,",q1->value(0).toInt())+
	    QString().sprintf("EVENT_TYPE=%d,",q1->value(1).toInt())+
	    QString().sprintf("CART_NUMBER=%u,",q1->value(2).toUInt())+
	    QString().sprintf("TRANS_TYPE=%d,",q1->value(3).toInt())+
	    "MARKER_COMMENT=\""+RDEscapeString(q1->value(4).toString())+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename)) {
	  return false;
	}
      }

      //
      // PostImport Events
      //
      tablename=q->value(0).toString()+"_POST";
      tablename.replace(" ","_");
      if(!TableExists(tablename)) {
	fprintf(stderr,"rddbmgr: missing POST table for event \"%s\"\n",
		(const char *)q->value(0).toString().toUtf8());
      }
      else {
	sql=QString("select ")+
	  "COUNT,"+        // 00
	  "TYPE,"+         // 01
	  "CART_NUMBER,"+  // 02
	  "TRANS_TYPE,"+   // 03
	  "COMMENT "+      // 05
	  "from `"+tablename+"`"+
	  "order by COUNT";
	q1=new RDSqlQuery(sql,false);
	while(q1->next()) {
	  sql=QString("insert into EVENT_LINES set ")+
	    "EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    "TYPE=1,"+
	    QString().sprintf("COUNT=%d,",q1->value(0).toInt())+
	    QString().sprintf("EVENT_TYPE=%d,",q1->value(1).toInt())+
	    QString().sprintf("CART_NUMBER=%u,",q1->value(2).toUInt())+
	    QString().sprintf("TRANS_TYPE=%d,",q1->value(3).toInt())+
	    "MARKER_COMMENT=\""+RDEscapeString(q1->value(4).toString())+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    //
    // Delete orphaned event tables
    //
    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_PRE\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned PRE table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_POST\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned POST table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<291)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists CLOCK_LINES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "CLOCK_NAME char(64) not null,"+
      "EVENT_NAME char(64) not null,"+
      "START_TIME int not null,"+
      "LENGTH int not null,"+
      "unique index CLOCK_NAME_START_TIME_IDX (CLOCK_NAME,START_TIME))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from CLOCKS");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_CLK";
      tablename.replace(" ","_");
      if(!TableExists(tablename)) {
	fprintf(stderr,"rddbmgr: missing CLK table for clock \"%s\"\n",
		(const char *)q->value(0).toString().toUtf8());
      }
      else {
	/* *** HACK WARNING *** HACK WARNING ***
	 *
	 * This check works around a bug in Rivendell v2.x that permitted
	 * clocks to have events with the same start time. It deletes all
	 * such 'conflicting' events except the one with the lowest ID number.
	 */
	QList<unsigned> deleted_ids;
	sql=QString("select ")+
	  "ID,"+          // 00
	  "EVENT_NAME,"+  // 01
	  "START_TIME,"+  // 02
	  "LENGTH "+      // 03
	  "from `"+tablename+"` "+
	  "order by ID";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("select ")+
	    "ID,"+          // 00
	    "EVENT_NAME "+  // 01
	    "from `"+tablename+"` where "+
	    QString().sprintf("START_TIME=%d && ",q1->value(2).toInt())+
	    QString().sprintf("ID!=%u ",q1->value(0).toUInt())+
	    "order by ID";
	  q2=new RDSqlQuery(sql);
	  while(q2->next()) {
	    if(!deleted_ids.contains(q1->value(0).toUInt())) {
	      fprintf(stderr,
		      "WARNING: deleted conflicting event \"%s\" from clock \"%s\"\n",
		      (const char *)q2->value(1).toString().toUtf8(),
		      (const char *)q->value(0).toString().toUtf8());
	      sql=QString("delete ")+
		"from `"+tablename+"` where "+
		QString().sprintf("ID=%u",q2->value(0).toUInt());
	      RDSqlQuery::apply(sql);
	      deleted_ids.push_back(q2->value(0).toUInt());
	    }
	  }
	  delete q2;
	}
	delete q1;
	/* *** END OF HACK WARNING *** END OF HACK WARNING ***/

	sql=QString("select ")+
	  "EVENT_NAME,"+  // 00
	  "START_TIME,"+  // 01
	  "LENGTH "+      // 02
	  "from `"+tablename+"` "+
	  "order by START_TIME";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into CLOCK_LINES set ")+
	    "CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    "EVENT_NAME=\""+RDEscapeString(q1->value(0).toString())+"\","+
	    QString().sprintf("START_TIME=%d,",q1->value(1).toInt())+
	    QString().sprintf("LENGTH=%d",q1->value(2).toInt());
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_CLK\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned CLK table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<292)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists RULE_LINES (")+
      "ID int unsigned auto_increment primary key,"+
      "CLOCK_NAME char(64) not null,"+
      "CODE varchar(10) not null,"+
      "MAX_ROW int unsigned,"+
      "MIN_WAIT int unsigned,"+
      "NOT_AFTER varchar(10),"+
      "OR_AFTER varchar(10),"+
      "OR_AFTER_II varchar(10),"+
      "unique index CLOCK_NAME_CODE_IDX(CLOCK_NAME,CODE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select NAME from CLOCKS");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_RULES";
      tablename.replace(" ","_");
      if(!TableExists(tablename)) {
	fprintf(stderr,"rddbmgr: missing RULES table for clock \"%s\"\n",
		(const char *)q->value(0).toString().toUtf8());
      }
      else {
	sql=QString("select ")+
	  "CODE,"+         // 00
	  "MAX_ROW,"+      // 01
	  "MIN_WAIT,"+     // 02
	  "NOT_AFTER,"+    // 03
	  "OR_AFTER,"+     // 04
	  "OR_AFTER_II "+  // 05
	  "from `"+tablename+"` "+
	  "order by CODE";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into RULE_LINES set ")+
	    "CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    "CODE=\""+RDEscapeString(q1->value(0).toString())+"\","+
	    QString().sprintf("MAX_ROW=%u,",q1->value(1).toUInt())+
	    QString().sprintf("MIN_WAIT=%u,",q1->value(2).toUInt())+
	    "NOT_AFTER=\""+RDEscapeString(q1->value(3).toString())+"\","+
	    "OR_AFTER=\""+RDEscapeString(q1->value(4).toString())+"\","+
	    "OR_AFTER_II=\""+RDEscapeString(q1->value(5).toString())+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_RULES\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned RULES table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<293)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists STACK_LINES (")+
      "ID int unsigned not null auto_increment primary key,"+
      "SCHED_STACK_ID int unsigned not null,"+
      "SERVICE_NAME char(10) not null,"+
      "CART int unsigned not null,"+
      "ARTIST varchar(255),"+
      "SCHED_CODES varchar(255),"+
      "SCHEDULED_AT datetime default '1752-09-14 00:00:00',"+
      "index SERVICE_NAME_IDX(SERVICE_NAME),"+
      "index SCHED_STACK_ID_IDX(SERVICE_NAME,SCHED_STACK_ID,SCHED_CODES))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_STACK";
      tablename.replace(" ","_");

      if(TableExists(tablename)) {
	//
	// 9/14/1752 is the earliest valid QDate
	//
	sql=QString("update `")+tablename+"` set "+
	  "SCHEDULED_AT=\"1752-09-14 00:00:00\" where "+
	  "SCHEDULED_AT<\"1752-09-14 00:00:00\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
	sql=QString("select ")+
	  "SCHED_STACK_ID,"+  // 00
	  "CART,"+            // 01
	  "ARTIST,"+          // 02
	  "SCHED_CODES,"+     // 03
	  "SCHEDULED_AT "+    // 04
	  "from `"+tablename+"` "+
	  "order by SCHEDULED_AT";
	q1=new RDSqlQuery(sql,false);
	while(q1->next()) {	
	  sql=QString("insert into STACK_LINES set ")+
	    "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("SCHED_STACK_ID=%u,",q1->value(0).toUInt())+
	    QString().sprintf("CART=%u,",q1->value(1).toUInt())+
	    "ARTIST=\""+RDEscapeString(q1->value(2).toString())+"\","+
	    "SCHED_CODES=\""+RDEscapeString(q1->value(3).toString())+"\","+
	    "SCHEDULED_AT=\""+RDEscapeString(q1->value(4).toDateTime().
					  toString("yyyy-MM-dd hh:mm:ss"))+"\"";
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    sql=QString("show tables where ")+
      "Tables_in_"+db_config->mysqlDbname()+" like \"%_STACK\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if(DropTable(q->value(0).toString(),err_msg)) {
	fprintf(stderr,"rddbmgr: dropping orphaned STACK table \"%s\"\n",
		(const char *)q->value(0).toString());
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<294)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists CAST_DOWNLOADS (")+
      "ID int unsigned not null auto_increment primary key,"+
      "FEED_KEY_NAME char(8) not null,"+
      "CAST_ID int unsigned not null,"+
      "ACCESS_DATE date not null,"+
      "ACCESS_COUNT int unsigned not null default 0,"+
      "unique index KEY_NAME_CAST_ID_DATE_IDX(FEED_KEY_NAME,CAST_ID,ACCESS_DATE))"+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select KEY_NAME from FEEDS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_FLG";
      tablename.replace(" ","_");
      if(!TableExists(tablename)) {
	fprintf(stderr,"rddbmgr: missing FLG table for feed \"%s\"\n",
		(const char *)q->value(0).toString().toUtf8());
      }
      else {
	sql=QString("select ")+
	  "CAST_ID,"+       // 00
	  "ACCESS_DATE,"+   // 01
	  "ACCESS_COUNT "+  // 02
	  "from `"+tablename+"` "+
	  "order by ACCESS_DATE";
	q1=new RDSqlQuery(sql,false);
	while(q1->next()) {
	  sql=QString("insert into CAST_DOWNLOADS set ")+
	    "FEED_KEY_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CAST_ID=%u,",q1->value(0).toUInt())+
	    "ACCESS_DATE=\""+
	    RDEscapeString(q1->value(1).toDate().toString("yyyy-MM-dd"))+"\","+
	    QString().sprintf("ACCESS_COUNT=%u",q1->value(2).toUInt());
	  if(!RDSqlQuery::apply(sql,err_msg)) {
	    return false;
	  }
	}
	delete q1;
	if(!DropTable(tablename,err_msg)) {
	  return false;
	}
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<295)&&(set_schema>cur_schema)) {
    sql=QString("create table IMPORTER_LINES (")+
      "ID int not null primary key auto_increment,"+
      "STATION_NAME char(64) not null,"+
      "PROCESS_ID int unsigned not null,"
      "LINE_ID int unsigned not null,"+
      "START_HOUR int not null,"+
      "START_SECS int not null,"+
      "CART_NUMBER int unsigned,"+
      "TITLE char(255),"+
      "LENGTH int,"+
      "INSERT_BREAK enum('N','Y') default 'N',"+
      "INSERT_TRACK enum('N','Y') default 'N',"+
      "INSERT_FIRST int unsigned default 0,"+
      "TRACK_STRING char(255),"+
      "EXT_DATA char(32),"+
      "EXT_EVENT_ID char(32),"+
      "EXT_ANNC_TYPE char(8),"+
      "EXT_CART_NAME char(32),"+
      "LINK_START_TIME time default NULL,"+
      "LINK_LENGTH int default NULL,"+
      "EVENT_USED enum('N','Y') default 'N',"+
      "index STATION_NAME_PROCESS_ID_IDX (STATION_NAME,PROCESS_ID),"+
      "unique index START_TIME_IDX (STATION_NAME,PROCESS_ID,START_HOUR,START_SECS,LINE_ID)) "+
      " charset latin1 collate latin1_swedish_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<296)&&(set_schema>cur_schema)) {
    DropColumn("STATIONS","BACKUP_DIR");
    DropColumn("STATIONS","BACKUP_LIFE");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<297)&&(set_schema>cur_schema)) {
    sql=QString("alter table AUDIO_CARDS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table AUDIO_CARDS ")+
      "modify column NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table AUDIO_INPUTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table AUDIO_OUTPUTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table AUDIO_PERMS ")+
      "modify column GROUP_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table AUDIO_PERMS ")+
      "modify column SERVICE_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table AUTOFILLS ")+
      "modify column SERVICE varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table AUX_METADATA ")+
      "modify column VAR_NAME varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table AUX_METADATA ")+
      "modify column CAPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART ")+
      "modify column GROUP_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column TITLE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column ARTIST varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column ALBUM varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column CONDUCTOR varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column LABEL varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column CLIENT varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column AGENCY varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column PUBLISHER varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column COMPOSER varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column USER_DEFINED varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column SONG_ID varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column OWNER varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CART ")+
      "modify column PENDING_STATION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CARTSLOTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CARTSLOTS ")+
      "modify column SERVICE_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CAST_DOWNLOADS ")+
      "modify column FEED_KEY_NAME varchar(8) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CLIPBOARD ")+
      "modify column CUT_NAME varchar(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLIPBOARD ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLIPBOARD ")+
      "modify column OUTCUE varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLIPBOARD ")+
      "modify column ORIGIN_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CLOCKS ")+
      "modify column NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLOCKS ")+
      "modify column SHORT_NAME varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLOCKS ")+
      "modify column COLOR varchar(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLOCKS ")+
      "modify column REMARKS text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CLOCK_LINES ")+
      "modify column CLOCK_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLOCK_LINES ")+
      "modify column EVENT_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CLOCK_PERMS ")+
      "modify column CLOCK_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CLOCK_PERMS ")+
      "modify column SERVICE_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CUTS ")+
      "modify column CUT_NAME varchar(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column OUTCUE varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column ISRC varchar(12)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column ISCI varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column SHA1_HASH varchar(40)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column ORIGIN_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column ORIGIN_LOGIN_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table CUTS ")+
      "modify column SOURCE_HOSTNAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CUT_EVENTS ")+
      "modify column CUT_NAME varchar(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DECKS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table DECKS ")+
      "modify column SWITCH_STATION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DECK_EVENTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DROPBOXES ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table DROPBOXES ")+
      "modify column GROUP_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table DROPBOXES ")+
      "modify column PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table DROPBOXES ")+
      "modify column METADATA_PATTERN varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table DROPBOXES ")+
      "modify column LOG_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table DROPBOXES ")+
      "modify column SET_USER_DEFINED varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DROPBOX_PATHS ")+
      "modify column FILE_PATH varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DROPBOX_SCHED_CODES ")+
      "modify column SCHED_CODE varchar(11) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table ELR_LINES ")+
      "modify column SERVICE_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column LOG_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column TITLE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column ARTIST varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column PUBLISHER varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column COMPOSER varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column USER_DEFINED varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column SONG_ID varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column ALBUM varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column LABEL varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column CONDUCTOR varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column OUTCUE varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column ISRC varchar(12)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column ISCI varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column EXT_CART_NAME varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column EXT_DATA varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column EXT_EVENT_ID varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ELR_LINES ")+
      "modify column EXT_ANNC_TYPE varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table ENCODERS ")+
      "modify column NAME varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ENCODERS ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ENCODERS ")+
      "modify column COMMAND_LINE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ENCODERS ")+
      "modify column DEFAULT_EXTENSION varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table EVENTS ")+
      "modify column NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column PROPERTIES varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column DISPLAY_TEXT varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column NOTE_TEXT varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column COLOR varchar(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column SCHED_GROUP varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column HAVE_CODE varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column HAVE_CODE2 varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column NESTED_EVENT varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENTS ")+
      "modify column REMARKS text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table EVENT_LINES ")+
      "modify column EVENT_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENT_LINES ")+
      "modify column MARKER_COMMENT varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table EVENT_PERMS ")+
      "modify column EVENT_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EVENT_PERMS ")+
      "modify column SERVICE_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table EXTENDED_PANELS ")+
      "modify column OWNER varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EXTENDED_PANELS ")+
      "modify column LABEL varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EXTENDED_PANELS ")+
      "modify column DEFAULT_COLOR varchar(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table EXTENDED_PANEL_NAMES ")+
      "modify column OWNER varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table EXTENDED_PANEL_NAMES ")+
      "modify column NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table FEEDS ")+
      "modify column KEY_NAME varchar(8) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_TITLE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_DESCRIPTION text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_CATEGORY varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_LINK varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_COPYRIGHT varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_WEBMASTER varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_LANGUAGE varchar(5) default \"en-us\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column BASE_URL varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column BASE_PREAMBLE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column PURGE_URL varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column PURGE_USERNAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column PURGE_PASSWORD varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column UPLOAD_EXTENSION varchar(16) default \"mp3\"";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "modify column REDIRECT_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table FEED_PERMS ")+
      "modify column USER_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEED_PERMS ")+
      "modify column KEY_NAME varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table GPIO_EVENTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table GPIS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table GPOS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table GROUPS ")+
      "modify column NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table GROUPS ")+
      "modify column DESCRIPTION varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table GROUPS ")+
      "modify column DEFAULT_TITLE varchar(191) default 'Imported from %f.%e'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table GROUPS ")+
      "modify column COLOR varchar(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table HOSTVARS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table HOSTVARS ")+
      "modify column NAME varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table HOSTVARS ")+
      "modify column VARVALUE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table HOSTVARS ")+
      "modify column REMARK varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table IMPORTER_LINES ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column TITLE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column TRACK_STRING varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column EXT_DATA varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column EXT_EVENT_ID varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column EXT_ANNC_TYPE varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column EXT_CART_NAME varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table IMPORT_TEMPLATES ")+
      "modify column NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table INPUTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table INPUTS ")+
      "modify column NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table INPUTS ")+
      "modify column FEED_NAME varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table INPUTS ")+
      "modify column NODE_HOSTNAME varchar(176)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column ISCI varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column FILENAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column TYPE varchar(1) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column ADVERTISER_NAME varchar(30)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column PRODUCT_NAME varchar(35)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column CREATIVE_TITLE varchar(30)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table ISCI_XREFERENCE ")+
      "modify column REGION_NAME varchar(80)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table JACK_CLIENTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table JACK_CLIENTS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table JACK_CLIENTS ")+
      "modify column COMMAND_LINE text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LIVEWIRE_GPIO_SLOTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LIVEWIRE_GPIO_SLOTS ")+
      "modify column IP_ADDRESS varchar(15)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS ")+
      "modify column NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column SERVICE varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column ORIGIN_USER varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column LOCK_USER_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column LOCK_STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column LOCK_IPV4_ADDRESS varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOGS ")+
      "modify column LOCK_GUID varchar(82)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOG_LINES ")+
      "modify column LOG_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column COMMENT varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column LABEL varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column ORIGIN_USER varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column LINK_EVENT_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column EXT_CART_NAME varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column EXT_DATA varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column EXT_EVENT_ID varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_LINES ")+
      "modify column EXT_ANNC_TYPE varchar(8)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOG_MACHINES ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_MACHINES ")+
      "modify column LOG_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_MACHINES ")+
      "modify column CURRENT_LOG varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_MACHINES ")+
      "modify column UDP_ADDR varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_MACHINES ")+
      "modify column UDP_STRING varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table LOG_MACHINES ")+
      "modify column LOG_RML varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOG_MODES ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column IP_ADDRESS varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column IP_ADDRESS_2 varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column USERNAME varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column USERNAME_2 varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column PASSWORD varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column PASSWORD_2 varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table MATRICES ")+
      "modify column GPIO_DEVICE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table NOWNEXT_PLUGINS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table NOWNEXT_PLUGINS ")+
      "modify column PLUGIN_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table NOWNEXT_PLUGINS ")+
      "modify column PLUGIN_ARG varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table OUTPUTS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table OUTPUTS ")+
      "modify column NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table OUTPUTS ")+
      "modify column NODE_HOSTNAME varchar(176)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table PANELS ")+
      "modify column OWNER varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PANELS ")+
      "modify column LABEL varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PANELS ")+
      "modify column DEFAULT_COLOR varchar(7)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table PANEL_NAMES ")+
      "modify column OWNER varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PANEL_NAMES ")+
      "modify column NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_TITLE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_DESCRIPTION text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_CATEGORY varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_LINK varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_COMMENTS varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_AUTHOR varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_SOURCE_TEXT varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column ITEM_SOURCE_URL varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "modify column AUDIO_FILENAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY ")+
      "modify column STATION varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column BUTTON_LABEL_TEMPLATE varchar(32) default '%t'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column DEFAULT_SERVICE varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column TITLE_TEMPLATE varchar(64) default '%t'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column ARTIST_TEMPLATE varchar(64) default '%a'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column OUTCUE_TEMPLATE varchar(64) default '%o'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column DESCRIPTION_TEMPLATE varchar(64) default '%i'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column EXIT_PASSWORD varchar(41)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY ")+
      "modify column SKIN_PATH varchar(191) default '/usr/share/pixmaps/rivendell/rdairplay_skin.png'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY_CHANNELS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY_CHANNELS ")+
      "modify column START_RML varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDAIRPLAY_CHANNELS ")+
      "modify column STOP_RML varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDCATCH ")+
      "modify column STATION varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDCATCH ")+
      "modify column ERROR_RML varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDHOTKEYS ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDHOTKEYS ")+
      "modify column MODULE_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDHOTKEYS ")+
      "modify column KEY_VALUE varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDHOTKEYS ")+
      "modify column KEY_LABEL varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY ")+
      "modify column STATION varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDLIBRARY ")+
      "modify column RIPPER_DEVICE varchar(64) default '/dev/cdrom'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDLIBRARY ")+
      "modify column CDDB_SERVER varchar(64) default 'freedb.freedb.org'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLOGEDIT ")+
      "modify column STATION varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDPANEL ")+
      "modify column STATION varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDPANEL ")+
      "modify column BUTTON_LABEL_TEMPLATE varchar(32) default '%t'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDPANEL ")+
      "modify column DEFAULT_SERVICE varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDPANEL ")+
      "modify column SKIN_PATH varchar(191) default '/usr/share/pixmaps/rivendell/rdpanel_skin.png'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDPANEL_CHANNELS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDPANEL_CHANNELS ")+
      "modify column START_RML varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RDPANEL_CHANNELS ")+
      "modify column STOP_RML varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RECORDINGS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS ")+
      "modify column CUT_NAME varchar(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS ")+
      "modify column URL varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS ")+
      "modify column URL_USERNAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table RECORDINGS ")+
      "modify column URL_PASSWORD varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPLICATORS ")+
      "modify column NAME varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPLICATORS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPLICATORS ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPLICATORS ")+
      "modify column URL varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPLICATORS ")+
      "modify column URL_USERNAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPLICATORS ")+
      "modify column URL_PASSWORD varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPLICATOR_MAP ")+
      "modify column REPLICATOR_NAME varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPLICATOR_MAP ")+
      "modify column GROUP_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPL_CART_STATE ")+
      "modify column REPLICATOR_NAME varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPL_CART_STATE ")+
      "modify column POSTED_FILENAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPL_CUT_STATE ")+
      "modify column REPLICATOR_NAME varchar(32) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPL_CUT_STATE ")+
      "modify column CUT_NAME varchar(12) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORTS ")+
      "modify column NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORTS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORTS ")+
      "modify column EXPORT_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORTS ")+
      "modify column WIN_EXPORT_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORTS ")+
      "modify column STATION_ID varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORTS ")+
      "modify column SERVICE_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORTS ")+
      "modify column STATION_FORMAT varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORT_GROUPS ")+
      "modify column REPORT_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORT_GROUPS ")+
      "modify column GROUP_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORT_SERVICES ")+
      "modify column REPORT_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORT_SERVICES ")+
      "modify column SERVICE_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORT_SERVICES ")+
      "modify column REPORT_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORT_SERVICES ")+
      "modify column SERVICE_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPORT_STATIONS ")+
      "modify column REPORT_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table REPORT_STATIONS ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES ")+
      "modify column NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column DESCRIPTION varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column NAME_TEMPLATE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column DESCRIPTION_TEMPLATE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column PROGRAM_CODE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TRACK_GROUP varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column AUTOSPOT_GROUP varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_WIN_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_IMPORT_TEMPLATE varchar(64) default 'Rivendell Standard Import'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_LABEL_CART varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_TRACK_CART varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_BREAK_STRING varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column TFC_TRACK_STRING varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES ")+
      "modify column MUS_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column MUS_WIN_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column MUS_IMPORT_TEMPLATE varchar(64) default 'Rivendell Standard Import'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column MUS_LABEL_CART varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column MUS_TRACK_CART varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column MUS_BREAK_STRING varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICES ")+
      "modify column MUS_TRACK_STRING varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICE_CLOCKS ")+
      "modify column SERVICE_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICE_CLOCKS ")+
      "modify column CLOCK_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICE_PERMS ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SERVICE_PERMS ")+
      "modify column SERVICE_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STACK_LINES ")+
      "modify column SERVICE_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STACK_LINES ")+
      "modify column ARTIST varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STACK_LINES ")+
      "modify column SCHED_CODES varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table STATIONS ")+
      "modify column NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column SHORT_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column DESCRIPTION varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column USER_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column DEFAULT_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column IPV4_ADDRESS varchar(15) default '127.0.0.2'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column HTTP_STATION varchar(64) default 'localhost'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column CAE_STATION varchar(64) default 'localhost'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column EDITOR_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column JACK_SERVER_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column JACK_COMMAND_LINE varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column HPI_VERSION varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column JACK_VERSION varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table STATIONS ")+
      "modify column ALSA_VERSION varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SWITCHER_NODES ")+
      "modify column STATION_NAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SWITCHER_NODES ")+
      "modify column HOSTNAME varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SWITCHER_NODES ")+
      "modify column PASSWORD varchar(64)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SWITCHER_NODES ")+
      "modify column DESCRIPTION varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SYSTEM ")+
      "modify column ISCI_XREFERENCE_PATH varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SYSTEM ")+
      "modify column TEMP_CART_GROUP varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table SYSTEM ")+
      "modify column NOTIFICATION_ADDRESS varchar(15) default '"+
      RD_NOTIFICATION_ADDRESS+"'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table TRIGGERS ")+
      "modify column CUT_NAME varchar(12)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table TTYS ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table TTYS ")+
      "modify column PORT varchar(20)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USERS ")+
      "modify column LOGIN_NAME varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USERS ")+
      "modify column FULL_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USERS ")+
      "modify column PHONE_NUMBER varchar(20)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USERS ")+
      "modify column DESCRIPTION varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USERS ")+
      "modify column PASSWORD varchar(32)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USERS ")+
      "modify column PAM_SERVICE varchar(32) default 'rivendell'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USER_PERMS ")+
      "modify column USER_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USER_PERMS ")+
      "modify column GROUP_NAME varchar(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USER_SERVICE_PERMS ")+
      "modify column USER_NAME varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USER_SERVICE_PERMS ")+
      "modify column SERVICE_NAME varchar(10) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table VGUEST_RESOURCES ")+
      "modify column STATION_NAME varchar(64) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table WEBAPI_AUTHS ")+
      "modify column TICKET varchar(41) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table WEBAPI_AUTHS ")+
      "modify column LOGIN_NAME varchar(191) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table WEBAPI_AUTHS ")+
      "modify column IPV4_ADDRESS varchar(16) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table WEB_CONNECTIONS ")+
      "modify column LOGIN_NAME varchar(191)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table WEB_CONNECTIONS ")+
      "modify column IP_ADDRESS varchar(16)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    ModifyCharset("utf8mb4","utf8mb4_general_ci");

    WriteSchemaVersion(++cur_schema);
  }

  //
  // Maintainer's Note:
  //
  // All tables created below this point should use a character set
  // of 'utf8mb4' and a collation of 'utf8mb4_general_ci'.
  //

  if((cur_schema<298)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLOGEDIT ")+
      "modify column INPUT_CARD int(11) default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLOGEDIT ")+
      "modify column OUTPUT_CARD int(11) default 0";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<299)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists CART_SCHED_CODES (")+
      "ID int auto_increment not null primary key,"+
      "CART_NUMBER int unsigned not null default 0,"+
      "SCHED_CODE varchar(11) not null,"+
      "index SCHED_CODE_IDX (CART_NUMBER,SCHED_CODE))"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    q=new RDSqlQuery("select NUMBER,SCHED_CODES from CART where TYPE=1",false);
    while(q->next()) {
      for(int i=0;i<255;i+=11) {
        QString code=q->value(1).toString().mid(i,11).stripWhiteSpace();
        if((!code.isEmpty())&&(code!=".")) {
          sql=QString("insert into CART_SCHED_CODES set ")+
            "CART_NUMBER="+q->value(0).toString()+","+
            "SCHED_CODE=\""+code+"\"";
          if(!RDSqlQuery::apply(sql,err_msg)) {
            return false;
          }
        }
      }
    }
    delete q;
    DropColumn("CART","SCHED_CODES");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<300)&&(set_schema>cur_schema)) {
    DropColumn("SERVICES","MUS_WIN_PATH");
    DropColumn("SERVICES","MUS_WIN_PREIMPORT_CMD");
    DropColumn("SERVICES","TFC_WIN_PATH");
    DropColumn("SERVICES","TFC_WIN_PREIMPORT_CMD");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<301)&&(set_schema>cur_schema)) {
    sql=QString("alter table SERVICES add column ")+
      "INCLUDE_IMPORT_MARKERS enum('N','Y') default 'Y' after ELR_SHELFLIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS add column ")+
      "INCLUDE_IMPORT_MARKERS enum('N','Y') default 'Y' after COMPLETED_TRACKS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<302)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add column ")+
      "REPORT_EDITOR_PATH varchar(191) after EDITOR_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<303)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists PYPAD_INSTANCES (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME varchar(64) not null,"+
      "SCRIPT_PATH varchar(191) not null,"+
      "DESCRIPTION varchar(191) default '[new]',"+
      "CONFIG text not null,"+
      "index STATION_NAME_IDX(STATION_NAME))"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<304)&&(set_schema>cur_schema)) {
    sql=QString("alter table PYPAD_INSTANCES add column ")+
      "IS_RUNNING enum('N','Y') not null default 'N' after CONFIG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PYPAD_INSTANCES add column ")+
      "EXIT_CODE int not null default 0 after IS_RUNNING";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PYPAD_INSTANCES add column ")+
      "ERROR_TEXT text after EXIT_CODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<305)&&(set_schema>cur_schema)) {
    DropTable("NOWNEXT_PLUGINS");
    DropColumn("LOG_MACHINES","UDP_ADDR");
    DropColumn("LOG_MACHINES","UDP_PORT");
    DropColumn("LOG_MACHINES","UDP_STRING");
    DropColumn("LOG_MACHINES","LOG_RML");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<306)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLIBRARY drop index STATION_IDX");
    RDSqlQuery::apply(sql);
    DropColumn("RDLIBRARY","INSTANCE");
    sql=QString("create index STATION_IDX on RDLIBRARY (STATION)"); 
    RDSqlQuery::apply(sql);

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<307)&&(set_schema>cur_schema)) {
    sql=QString("alter table EVENTS add column ")+
      "ARTIST_SEP int(10) after SCHED_GROUP";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table EVENTS modify column TITLE_SEP int(10)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    if (!ConvertArtistSep307(err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<308)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists STACK_SCHED_CODES (")+
      "ID int auto_increment not null primary key,"+
      "STACK_LINES_ID int unsigned,"+
      "SCHED_CODE varchar(10),"+
      "index SCHED_CODE_IDX(SCHED_CODE))"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select ")+
      "ID,"+          // 00
      "SCHED_CODES "  // 01
      "from STACK_LINES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QStringList f0=q->value(1).toString().split(" ",QString::SkipEmptyParts);
      for(int i=0;i<f0.size();i++) {
	if((!f0.at(i).trimmed().isEmpty())&&(f0.at(i).trimmed()!=".")) {
	  sql=QString("insert into STACK_SCHED_CODES set ")+
	    QString().sprintf("STACK_LINES_ID=%u,",q->value(0).toUInt())+
	    "SCHED_CODE=\""+RDEscapeString(f0.at(i).trimmed())+"\"";
	  RDSqlQuery::apply(sql,err_msg);
	}
      }
    }
    delete q;
    DropColumn("STACK_LINES","SCHED_CODES",err_msg);

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<309)&&(set_schema>cur_schema)) {
    sql=QString("alter table DROPBOXES add column ")+
      "LOG_TO_SYSLOG enum('N','Y') not null "+
      "default 'Y' after FIX_BROKEN_FORMATS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("update DROPBOXES set LOG_TO_SYSLOG='N' where ")+
      "LOG_PATH is not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<310)&&(set_schema>cur_schema)) {
    sql="alter table CART add column MINIMUM_TALK_LENGTH int unsigned default 0 after AVERAGE_HOOK_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql="alter table CART add column MAXIMUM_TALK_LENGTH int unsigned default 0 after MINIMUM_TALK_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists NEXUS_FIELDS (")+
      "STATION varchar(255) NOT NULL,"+
      "RD_NAME varchar(255),"+
      "NEXUS_NAME varchar(255),"+
      "NEXUS_ID int(11))"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists NEXUS_QUEUE (")+
      "ID int auto_increment not null primary key,"+
      "NOTIFICATION varchar(255),"+
      "ERROR varchar(255),"+
      "CREATED timestamp default current_timestamp)"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists NEXUS_SERVER (")+
      "ENABLED enum('N','Y') default 'N',"+
      "ADDRESS varchar(50) NOT NULL,"+
      "PORT int(11) NOT NULL DEFAULT 8080,"+
      "STATION_NAME varchar(64))"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table if not exists NEXUS_STATIONS (")+
      "STATION varchar(255) not null,"+
      "CATEGORY varchar(10) not null,"+
      "AUTO_MERGE enum('N','Y') default 'N',"+
      "RD_SERVICE varchar(10) not null,"+
      "RD_GROUP_NAME varchar(10) not null,"+
      "RD_SCHED_CODE varchar(11) not null)"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    q=new RDSqlQuery("select NAME from SERVICES",false);
    while(q->next()) {
      sql=QString("insert into NEXUS_STATIONS set ")+
	  "RD_SERVICE=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
    delete q;

    sql=QString("insert into IMPORT_TEMPLATES set ")+
      "NAME='MusicMaster Nexus',"+
      "HOURS_OFFSET=0,"+
      "HOURS_LENGTH=2,"+
      "MINUTES_OFFSET=3,"+
      "MINUTES_LENGTH=2,"+
      "SECONDS_OFFSET=6,"+
      "SECONDS_LENGTH=2,"+
      "DATA_OFFSET=10,"+
      "DATA_LENGTH=6,"+
      "CART_OFFSET=20,"+
      "CART_LENGTH=6,"+
      "TITLE_OFFSET=30,"+
      "TITLE_LENGTH=60,"+
      "LEN_HOURS_OFFSET=0,"+
      "LEN_HOURS_LENGTH=0,"+
      "LEN_MINUTES_OFFSET=95,"+
      "LEN_MINUTES_LENGTH=2,"+
      "LEN_SECONDS_OFFSET=98,"+
      "LEN_SECONDS_LENGTH=2,"+
      "EVENT_ID_OFFSET=101,"+
      "EVENT_ID_LENGTH=10";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    length_update_required=true;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<311)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLOGEDIT add column ")+
      "WAVEFORM_CAPTION varchar(64) not null default '%t - %a' "+
      "after TAIL_PREROLL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<312)&&(set_schema>cur_schema)) {
    sql=QString("alter table RDLIBRARY add column ")+
      "CD_SERVER_TYPE int unsigned not null default 2 after RIPPER_LEVEL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("update RDLIBRARY set CD_SERVER_TYPE=1");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add column ")+
      "MB_SERVER varchar(64) default 'musicbrainz.org' after CDDB_SERVER";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<313)&&(set_schema>cur_schema)) {
    sql=QString("alter table CUTS add column ")+
      "RECORDING_MBID varchar(40) after ISCI";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CUTS add column ")+
      "RELEASE_MBID varchar(40) after RECORDING_MBID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<314)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add column ")+
      "BROWSER_PATH varchar(191) default 'firefox' after REPORT_EDITOR_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<315)&&(set_schema>cur_schema)) {
    DropColumn("EVENTS","POST_POINT");
    DropColumn("LOG_LINES","POST_POINT");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<316)&&(set_schema>cur_schema)) {
    DropColumn("EVENTS","PROPERTIES");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<316)&&(set_schema>cur_schema)) {
    DropColumn("EVENTS","PROPERTIES");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<317)&&(set_schema>cur_schema)) {
    sql=QString("create index STACK_LINES_ID_IDX on ")+
      "STACK_SCHED_CODES(STACK_LINES_ID)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<318)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS add column ")+
      "IS_SUPERFEED enum('N','Y') default 'N' after KEY_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS add index IS_SUPERFEED_IDX(IS_SUPERFEED)");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("create table SUPERFEED_MAPS (")+
      "ID int unsigned primary key auto_increment,"+
      "FEED_ID int unsigned not null,"+
      "MEMBER_FEED_ID int unsigned not null,"+
      "KEY_NAME varchar(8) not null,"+
      "MEMBER_KEY_NAME varchar(8) not null,"+
      "index FEED_ID_IDX(FEED_ID),"+
      "index MEMBER_FEED_ID_IDX(MEMBER_FEED_ID),"+
      "index KEY_NAME_IDX(KEY_NAME),"+
      "index MEMBER_KEY_NAME_IDX(MEMBER_KEY_NAME)) "+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<319)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS add column ")+
      "AUDIENCE_METRICS enum('N','Y') default 'N' after IS_SUPERFEED";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("update FEEDS set AUDIENCE_METRICS='Y'");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  ///
  if((cur_schema<320)&&(set_schema>cur_schema)) {
    sql=QString("alter table USERS add column ")+
      "EMAIL_ADDRESS varchar(191) after FULL_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<321)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS add column ")+
      "CHANNEL_EDITOR varchar(64) after CHANNEL_COPYRIGHT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<322)&&(set_schema>cur_schema)) {
    sql=QString("create table RSS_SCHEMAS (")+
      "ID int unsigned primary key,"+
      "NAME varchar(64) unique not null,"+
      "HEADER_XML text,"+
      "CHANNEL_XML text,"+
      "ITEM_XML text,"+
      "index NAME_IDX(NAME)) "+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table FEEDS add column ")+
      "RSS_SCHEMA int unsigned not null default 0 after PURGE_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<323)&&(set_schema>cur_schema)) {
    sql=QString("create table FEED_IMAGES (")+
      "ID int unsigned primary key auto_increment,"+
      "FEED_ID int unsigned not null,"+
      "FEED_KEY_NAME varchar(8) not null,"+
      "WIDTH int not null,"+\
      "HEIGHT int not null,"+
      "DEPTH int not null,"+
      "DESCRIPTION varchar(191) not null,"+
      "FILE_EXTENSION varchar(10) not null,"+
      "DATA mediumblob not null,"+
      "index FEED_ID_IDX (FEED_ID),"+
      "index FEED_KEY_NAME_IDX (FEED_KEY_NAME)) "+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<324)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS add column CHANNEL_IMAGE_ID ")+
      "int not null default -1 after CHANNEL_LANGUAGE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS add column DEFAULT_ITEM_IMAGE_ID ")+
      "int not null default -1 after KEEP_METADATA";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS add column ITEM_IMAGE_ID ")+
      "int not null default -1 after ITEM_SOURCE_URL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<325)&&(set_schema>cur_schema)) {
    DropTable("RSS_SCHEMAS");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<326)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS add column CHANNEL_AUTHOR varchar(64) ")+
      "after CHANNEL_EDITOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS add column CHANNEL_OWNER_NAME varchar(64) ")+
      "after CHANNEL_AUTHOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS add column ")+
      "CHANNEL_OWNER_EMAIL varchar(64) after CHANNEL_OWNER_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS add column CHANNEL_EXPLICIT enum('N','Y') ")+
      "not null default 'N' after CHANNEL_LANGUAGE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS add column ITEM_EXPLICIT enum('N','Y') ")+
      "not null default 'N' after ITEM_SOURCE_URL";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<327)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS ")+
      "add column CHANNEL_SUB_CATEGORY varchar(64) "+
      "after CHANNEL_CATEGORY";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<328)&&(set_schema>cur_schema)) {
    DropColumn("FEEDS","AUDIENCE_METRICS");
    DropColumn("FEEDS","KEEP_METADATA");
    DropColumn("FEEDS","MEDIA_LINK_MODE");
    DropColumn("FEEDS","REDIRECT_PATH");
    DropTable("CAST_DOWNLOADS");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<329)&&(set_schema>cur_schema)) {
    sql=QString("alter table SYSTEM ")+
      "add column RSS_PROCESSOR_STATION varchar(64) "+
      "after NOTIFICATION_ADDRESS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select NAME from STATIONS where SYSTEM_MAINT='Y'");
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("update SYSTEM set ")+
	"RSS_PROCESSOR_STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    sql=QString("alter table PODCASTS ")+
      "add column EXPIRATION_DATETIME datetime after EFFECTIVE_DATETIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select ")+
      "ID,"+                  // 00
      "ORIGIN_DATETIME,"+     // 01
      "SHELF_LIFE "+          // 02
      "from PODCASTS where "+
      "SHELF_LIFE>0";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update PODCASTS set ")+
	"EXPIRATION_DATETIME=\""+
	q->value(1).toDateTime().addDays(q->value(2).toInt()).
	toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
	QString().sprintf("ID=%u",q->value(0).toUInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    DropColumn("PODCASTS","SHELF_LIFE");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<330)&&(set_schema>cur_schema)) {
    sql=QString("alter table PODCASTS ")+
      "add column ORIGIN_LOGIN_NAME varchar(191) after AUDIO_TIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "add column ORIGIN_STATION varchar(64) after ORIGIN_LOGIN_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<331)&&(set_schema>cur_schema)) {
    DropTable("ENCODER_SAMPLERATES");
    DropTable("ENCODER_BITRATES");
    DropTable("ENCODER_CHANNELS");
    DropTable("ENCODERS");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<332)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS ")+
      "add column CHANNEL_AUTHOR_IS_DEFAULT enum('N','Y') default 'N' "+
      "after CHANNEL_AUTHOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<333)&&(set_schema>cur_schema)) {
    sql=QString("alter table STATIONS add ")+
      "SSH_IDENTITY_FILE text after BROWSER_PATH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RECORDINGS ")+
      "add column URL_USE_ID_FILE enum('N','Y') default 'N' after URL_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table FEEDS add ")+
      "column PURGE_USE_ID_FILE enum('N','Y') default 'N' after PURGE_PASSWORD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<334)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS modify column ")+
      "PURGE_PASSWORD text";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select ")+
      "KEY_NAME,"+        // 00
      "PURGE_PASSWORD "+  // 01
      "from FEEDS";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update FEEDS set ")+
	"PURGE_PASSWORD=\""+
	RDEscapeString(q->value(1).toString().toUtf8().toBase64())+"\" where "+
	"KEY_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<335)&&(set_schema>cur_schema)) {
    sql=QString("alter table PODCASTS add column ")+
      "SHA1_HASH varchar(40) after AUDIO_TIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS add index SHA1_HASH_IDX(SHA1_HASH)");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<336)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEED_IMAGES modify column ")+
      "DATA longblob not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<337)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS ")+
      "add index CHANNEL_IMAGE_ID_IDX(CHANNEL_IMAGE_ID)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table FEEDS ")+
      "add index DEFAULT_ITEM_IMAGE_ID_IDX(DEFAULT_ITEM_IMAGE_ID)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table PODCASTS ")+
      "add index ITEM_IMAGE_ID_IDX(ITEM_IMAGE_ID)";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<338)&&(set_schema>cur_schema)) {
    sql=QString("alter table FEEDS ")+
      "modify column CHANNEL_LANGUAGE varchar(8) default 'en-us'";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<339)&&(set_schema>cur_schema)) {
    sql=QString("alter table SERVICES ")+
      "add column SUB_EVENT_INHERITANCE int not null default 0 "+
      "after CHAIN_LOG";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<340)&&(set_schema>cur_schema)) {
    sql=QString("delete from IMPORTER_LINES");  // Purge stale lines first
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "add column TYPE int unsigned not null "+
      "after LINE_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column START_HOUR int";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "modify column START_SECS int";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    DropColumn("IMPORTER_LINES","INSERT_BREAK");
    DropColumn("IMPORTER_LINES","INSERT_TRACK");
    DropColumn("IMPORTER_LINES","INSERT_FIRST");
    DropColumn("IMPORTER_LINES","LINK_START_TIME");
    DropColumn("IMPORTER_LINES","LINK_LENGTH");
    DropColumn("IMPORTER_LINES","TRACK_STRING");

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<341)&&(set_schema>cur_schema)) {
    sql=QString("delete from IMPORTER_LINES");  // Purge stale lines first
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "add column FILE_LINE int unsigned not null after PROCESS_ID";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "add column LINK_START_TIME time after EXT_CART_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table IMPORTER_LINES ")+
      "add column LINK_LENGTH int after LINK_START_TIME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<342)&&(set_schema>cur_schema)) {
    sql=
     QString("create index EVENT_DATETIME_IDX on GPIO_EVENTS(EVENT_DATETIME)"); 
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<343)&&(set_schema>cur_schema)) {
    sql=QString("create table if not exists ENCODER_PRESETS (")+
      "ID int auto_increment not null primary key,"+
      "NAME varchar(64) not null,"+
      "FORMAT int unsigned not null,"+
      "CHANNELS int unsigned not null,"+
      "SAMPLE_RATE int unsigned not null,"+
      "BIT_RATE int unsigned not null,"+
      "QUALITY int unsigned not null,"+
      "NORMALIZATION_LEVEL int not null,"+
      "AUTOTRIM_LEVEL int not null,"+
      "unique index NAME_IDX (NAME))"+
      " charset utf8mb4 collate utf8mb4_general_ci"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<344)&&(set_schema>cur_schema)) {
    sql=QString("alter table SYSTEM add column ")+
      "ORIGIN_EMAIL_ADDRESS varchar(64) not null "+
      "default 'Rivendell <noreply@example.com>' "+
      "after RSS_PROCESSOR_STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table GROUPS add column ")+
      "NOTIFY_EMAIL_ADDRESS text after COLOR";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<345)&&(set_schema>cur_schema)) {
    sql=QString("alter table DROPBOXES add column ")+
      "SEND_EMAIL enum('N','Y') not null default 'N' after DELETE_SOURCE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    WriteSchemaVersion(++cur_schema);
  }

  if((cur_schema<346)&&(set_schema>cur_schema)) {
    sql=QString("alter table USERS drop column ADMIN_USERS_PRIV");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("alter table USERS add column ")+
      "ADMIN_RSS_PRIV enum('N','Y') not null default 'N' "+
      "after ADMIN_CONFIG_PRIV";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(++cur_schema);
  }



  // NEW SCHEMA UPDATES GO HERE...

  //
  // Maintainer's Note:
  //
  // When adding a schema update here, be sure also to implement the
  // corresponding reversion in revertschema.cpp!
  //
  
  // **** End of version updates ****

  //
  // Update Cart Lengths, If Required
  //
  if(length_update_required) {
    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Audio);
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }

  *err_msg="ok";
  return true;
}


void MainObject::AverageCuts89(unsigned cartnum) const
{
  unsigned total=0;
  unsigned count=0;
  unsigned high=0;
  unsigned low=0xFFFFFFFF;
  unsigned avg=0;
  unsigned max_dev=0;
  unsigned weight;
  QDateTime end_datetime;
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "LENGTH,"+
    "WEIGHT,"+
    "END_DATETIME "+
    "from CUTS where "+
    QString().sprintf("(CART_NUMBER=%u)&&(LENGTH>0)",cartnum);
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    weight = q->value(1).toUInt();
    end_datetime = q->value(2).toDateTime();
    if (end_datetime.isValid() && (end_datetime <QDateTime::currentDateTime ())){
      // This cut has expired, it is no more, set its weight to zero.
      weight = 0;
    }
    total+=(q->value(0).toUInt() * weight);
    if((weight) && (q->value(0).toUInt()>high)) {
      high=q->value(0).toUInt();
    }
    if((weight) && (q->value(0).toUInt()<low)) {
      low=q->value(0).toUInt();
    }
    count += weight;    
  }
  delete q;
  if(count==0) {
    avg=0;
    low=0;
    high=0;
  }
  else {
    avg=total/count;
  }
  if((high-avg)>(avg-low)) {
    max_dev=high-avg;
  }
  else {
    max_dev=avg-low;
  }
  sql=QString("update CART set ")+
    QString().sprintf("LENGTH_DEVIATION=%u,",max_dev)+
    QString().sprintf("AVERAGE_LENGTH=%u where ",avg)+
    QString().sprintf("NUMBER=%u",cartnum);
  q=new RDSqlQuery(sql,false);
  delete q;
}


void MainObject::TotalMacros89(unsigned cartnum) const
{
  int len=0;

  QString sql=QString("select MACROS from CART where ")+
    QString().sprintf("NUMBER=%d",cartnum);
  RDSqlQuery *q=new RDSqlQuery(sql,false);
  if(q->first()) {
    QStringList f0=q->value(0).toString().split("!");
    if(f0.size()==2) {
      if(f0[0]=="SP") {
	len+=f0[1].toInt();
      }
    }
  }
  delete q;
  sql=QString("update CART set ")+
    QString().sprintf("AVERAGE_LENGTH=%u,",len)+
    QString().sprintf("FORCED_LENGTH=%u ",len)+
    QString().sprintf("where NUMBER=%u",cartnum);
  q=new RDSqlQuery(sql,false);
  delete q;
}


void MainObject::CreateAuxFieldsTable143(const QString &key_name) const
{
  QString keyname=key_name;
  keyname.replace(" ","_");
  QString sql=QString("create table if not exists `")+
    keyname+"_FIELDS` (CAST_ID int unsigned not null primary key) "+
    " charset latin1 collate latin1_swedish_ci"+
    db_table_create_postfix;
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


void MainObject::CreateFeedLog151(const QString &key_name) const
{
  QString sql;
  RDSqlQuery *q;

  QString keyname=key_name;

  keyname.replace(" ","_");
  sql=QString("create table if not exists `")+keyname+"_FLG` ("+
    "ID int unsigned primary key auto_increment,"+
    "CAST_ID int unsigned,"+
    "ACCESS_DATE date,"+
    "ACCESS_COUNT int unsigned default 0,"+
    "index CAST_ID_IDX(CAST_ID,ACCESS_DATE))"+
    " charset latin1 collate latin1_swedish_ci"+
    db_table_create_postfix;
  q=new RDSqlQuery(sql);
  delete q;
}


bool MainObject::UpdateLogTable186(const QString &table,QString *err_msg) const
{
  QString sql;

  //
  // Drop POST_TIME
  //
  sql=QString("alter table `")+table+"` drop column POST_TIME";
  if(!RDSqlQuery::apply(sql,err_msg)) {
    return false;
  }

  //
  // Convert Fields
  //
  if(!ConvertTimeField186(table,"START_TIME",err_msg)) {
    return false;
  }
  if(!ConvertTimeField186(table,"LINK_START_TIME",err_msg)) {
    return false;
  }

  return true;
}


bool MainObject::ConvertTimeField186(const QString &table,const QString &field,
				     QString *err_msg) const
{
  QString sql;
  RDSqlQuery *q;

  //
  // Create temporary field
  //
  sql=QString("alter table `")+table+
  "` add column "+field+"_TEMP int after "+field;
  if(!RDSqlQuery::apply(sql,err_msg)) {
    return false;
  }

  //
  // Copy data to temporary field
  //
  sql=QString("select ID,")+field+" from `"+table+"`";
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    if(!q->value(1).isNull()) {
      sql=QString("update `")+table+
	"` set "+
	field+QString().sprintf("_TEMP=%d where ID=%d",
				QTime().msecsTo(q->value(1).toTime()),
				q->value(0).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
  }
  delete q;

  //
  // Convert primary field
  //
  sql=QString("alter table `")+table+"` modify column "+field+" int";
  if(!RDSqlQuery::apply(sql,err_msg)) {
    return false;
  }

  //
  // Copy data back to primary field
  //
  sql=QString("select ID,")+field+"_TEMP from `"+table+"`";
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    if(!q->value(1).isNull()) {
      sql=QString("update `")+table+
	"` set "+field+
	QString().sprintf("=%d where ID=%d",
			  q->value(1).toInt(),
			  q->value(0).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }
  }
  delete q;

  //
  // Delete Temporary field
  //
  sql=QString("alter table `")+table+"` drop column "+field+"_TEMP";
  if(!RDSqlQuery::apply(sql,err_msg)) {
    return false;
  }

  return true;
}

bool MainObject::ConvertArtistSep307(QString *err_msg) const
{
  QString sql;
  RDSqlQuery *q;
  int max=-1;

  sql=QString("select ARTISTSEP from CLOCKS");
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    if(q->value(0).toInt()>max) {
      max=q->value(0).toInt();
    }
  }
  delete q;

  sql=QString().sprintf("update EVENTS set ARTIST_SEP=%d",max);
  if(!RDSqlQuery::apply(sql,err_msg)) {
    return false;
  }

  return true;
}
