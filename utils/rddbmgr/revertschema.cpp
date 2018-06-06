// revertschema.cpp
//
// Revert Rivendell DB schema
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdescape_string.h>

#include "rddbmgr.h"

bool MainObject::RevertSchema(int cur_schema,int set_schema,QString *err_msg) const
{
  QString sql;
  RDSqlQuery *q; 
  QString tablename;

  //
  // Revert 286
  //
  if((cur_schema==286)&&(set_schema<cur_schema)) {
    sql=QString("alter table SERVICES add ")+
      "column TFC_LENGTH_OFFSET int after TFC_LEN_SECONDS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column TFC_LENGTH_LENGTH int after TFC_LENGTH_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_LENGTH_OFFSET int after MUS_LEN_SECONDS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_LENGTH_LENGTH int after MUS_LENGTH_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 285
  //
  if((cur_schema==285)&&(set_schema<cur_schema)) {
    sql=QString("alter table CART add column ISRC char(12) after YEAR");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DECKS add ")+
      "column DEFAULT_SAMPRATE int default 44100 after DEFAULT_CHANNELS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column INPUT_STREAM int default 0 after INPUT_CARD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column OUTPUT_STREAM int default 0 after OUTPUT_CARD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column RECORD_GPI int default -1 after TRIM_THRESHOLD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column PLAY_GPI int default -1 after RECORD_GPI";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column STOP_GPI int default -1 after PLAY_GPI";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column DEFAULT_SAMPRATE int default 44100 after DEFAULT_CHANNELS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLOGEDIT add ")+
      "column SAMPRATE int unsigned default 44100 after FORMAT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column TFC_START_OFFSET int after TFC_TITLE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column TFC_START_LENGTH int after TFC_START_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_START_OFFSET int after MUS_TITLE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_START_LENGTH int after MUS_START_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 284
  //
  if((cur_schema==284)&&(set_schema<cur_schema)) {
    sql=QString("alter table RDAIRPLAY add column INSTANCE int unsigned ")+
      "not null default 0 after STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(int i=9;i>=0;i--) {
      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("START_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("PORT%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("CARD%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("alter table RDAIRPLAY add column ")+
      "OP_MODE int default 2 after TRANS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY add column ")+
      "START_MODE int default 0 after OP_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDPANEL add column INSTANCE int unsigned ")+
      "not null default 0 after STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(int i=9;i>=6;i--) {
      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("START_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("PORT%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("CARD%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    for(int i=3;i>=2;i--) {
      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("START_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("PORT%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("CARD%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("alter table MATRICES alter column PORT drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column PORT_2 drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column INPUTS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column OUTPUTS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column GPIS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column GPOS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPLICATORS alter column TYPE_ID drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 283
  //
  if((cur_schema==283)&&(set_schema<cur_schema)) {
    sql=QString("create table if not exists AUDIO_PORTS (")+
      "ID int unsigned not null primary key AUTO_INCREMENT,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "CLOCK_SOURCE int default 0,"+
      "INPUT_0_LEVEL int default 0,"+
      "INPUT_0_TYPE int default 0,"+
      "INPUT_0_MODE int default 0,"+
      "INPUT_1_LEVEL int default 0,"+
      "INPUT_1_TYPE int default 0,"+
      "INPUT_1_MODE int default 0,"+
      "INPUT_2_LEVEL int default 0,"+
      "INPUT_2_TYPE int default 0,"+
      "INPUT_2_MODE int default 0,"+
      "INPUT_3_LEVEL int default 0,"+
      "INPUT_3_TYPE int default 0,"+
      "INPUT_3_MODE int default 0,"+
      "INPUT_4_LEVEL int default 0,"+
      "INPUT_4_TYPE int default 0,"+
      "INPUT_4_MODE int default 0,"+
      "INPUT_5_LEVEL int default 0,"+
      "INPUT_5_TYPE int default 0,"+
      "INPUT_5_MODE int default 0,"+
      "INPUT_6_LEVEL int default 0,"+
      "INPUT_6_TYPE int default 0,"+
      "INPUT_6_MODE int default 0,"+
      "INPUT_7_LEVEL int default 0,"+
      "INPUT_7_TYPE int default 0,"+
      "INPUT_7_MODE int default 0,"+
      "OUTPUT_0_LEVEL int default 0,"+
      "OUTPUT_1_LEVEL int default 0,"+
      "OUTPUT_2_LEVEL int default 0,"+
      "OUTPUT_3_LEVEL int default 0,"+
      "OUTPUT_4_LEVEL int default 0,"+
      "OUTPUT_5_LEVEL int default 0,"+
      "OUTPUT_6_LEVEL int default 0,"+
      "OUTPUT_7_LEVEL int default 0,"+
      "index STATION_NAME_IDX (STATION_NAME),"+
      "index CARD_NUMBER_IDX (CARD_NUMBER))"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<8;i++) {
	sql=QString("insert into AUDIO_PORTS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("CARD_NUMBER=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;
    sql=QString("select ")+
      "STATION_NAME,"+  // 00
      "CARD_NUMBER,"+   // 01
      "PORT_NUMBER,"+   // 02
      "LEVEL,"+         // 03
      "TYPE,"+          // 04
      "MODE "+          // 05
      "from AUDIO_INPUTS where PORT_NUMBER<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_PORTS set ")+
	QString().sprintf("INPUT_%d_LEVEL=%d,",
			  q->value(2).toInt(),q->value(3).toInt())+
	QString().sprintf("INPUT_%d_TYPE=%d,",
			  q->value(2).toInt(),q->value(4).toInt())+
	QString().sprintf("INPUT_%d_MODE=%d  where ",
			  q->value(2).toInt(),q->value(5).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
    }
    delete q;
    sql=QString("drop table AUDIO_INPUTS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "STATION_NAME,"+  // 00
      "CARD_NUMBER,"+   // 01
      "PORT_NUMBER,"+   // 02
      "LEVEL "+         // 03
      "from AUDIO_OUTPUTS where PORT_NUMBER<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_PORTS set ")+
	QString().sprintf("OUTPUT_%d_LEVEL=%d where ",
			  q->value(2).toInt(),q->value(3).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    sql=QString("drop table AUDIO_OUTPUTS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "STATION_NAME,"+  // 00
      "CARD_NUMBER,"+   // 01
      "CLOCK_SOURCE "+  // 02
      "from AUDIO_CARDS where CARD_NUMBER<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_PORTS set ")+
	QString().sprintf("CLOCK_SOURCE=%d where ",q->value(2).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    sql=QString("alter table AUDIO_CARDS drop column CLOCK_SOURCE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 282
  //
  if((cur_schema==282)&&(set_schema<cur_schema)) {
    for(int i=7;i>=0;i--) {
      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_OUTPUTS int default -1 after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_INPUTS int default -1 after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_NAME char(64) after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_DRIVER int default 0 after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("drop table AUDIO_CARDS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 281
  //
  if((cur_schema==281)&&(set_schema<cur_schema)) {
    sql=QString("alter table RDAIRPLAY drop column VIRTUAL_EXIT_CODE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 280
  //
  if((cur_schema==280)&&(set_schema<cur_schema)) {
    sql=QString("delete from LOG_MODES where MACHINE>=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 279
  //
  if((cur_schema==279)&&(set_schema<cur_schema)) {
    sql=QString("delete from RDAIRPLAY_CHANNELS where INSTANCE>=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 278
  //
  if((cur_schema==278)&&(set_schema<cur_schema)) {
    for(int i=2;i>=0;i--) {
      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG_RML%d char(255) after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("UDP_STRING%d char(255) after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("UDP_PORT%d int unsigned after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("UDP_ADDR%d char(255) after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_START_MODE int default 0 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_NEXT_CART int unsigned default 0 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_NOW_CART int unsigned default 0 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_LOG_LINE int default -1 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_LOG_ID int default -1 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_RUNNING enum('N','Y') default 'N' ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_CURRENT_LOG char(64) ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_LOG_NAME char(64) ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_AUTO_RESTART enum('N','Y') default 'N' ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("select ")+
      "STATION_NAME,"+   // 00
      "MACHINE,"+        // 01
      "START_MODE,"+     // 02
      "AUTO_RESTART,"+   // 03
      "LOG_NAME,"+       // 04
      "CURRENT_LOG,"+    // 05
      "RUNNING,"+        // 06
      "LOG_ID,"+         // 07
      "LOG_LINE,"+       // 08
      "NOW_CART,"+       // 09
      "NEXT_CART,"+      // 10
      "UDP_ADDR,"+       // 11
      "UDP_PORT,"+       // 12
      "UDP_STRING,"+     // 13
      "LOG_RML "+        // 14
      "from LOG_MACHINES where "+
      "MACHINE<3";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update RDAIRPLAY set ")+
	QString().sprintf("UDP_ADDR%d=\"",q->value(1).toInt())+
	RDEscapeString(q->value(11).toString())+"\","+
	QString().sprintf("UDP_PORT%d=%u,",
			  q->value(1).toInt(),q->value(12).toUInt())+
	QString().sprintf("UDP_STRING%d=\"",q->value(1).toInt())+
	RDEscapeString(q->value(13).toString())+"\","+
	QString().sprintf("LOG_RML%d=\"",q->value(1).toInt())+
	RDEscapeString(q->value(14).toString())+"\","+
	QString().sprintf("LOG%d_START_MODE=%d,",
			  q->value(1).toInt(),q->value(2).toInt())+
	QString().sprintf("LOG%d_AUTO_RESTART=\"",q->value(1).toInt())+
	RDEscapeString(q->value(3).toString())+"\","+
	QString().sprintf("LOG%d_LOG_NAME=\"",q->value(1).toInt())+
	RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("LOG%d_CURRENT_LOG=\"",q->value(1).toInt())+
	RDEscapeString(q->value(5).toString())+"\","+
	QString().sprintf("LOG%d_RUNNING=\"",q->value(1).toInt())+
	RDEscapeString(q->value(6).toString())+"\","+
	QString().sprintf("LOG%d_LOG_ID=%d,",
			  q->value(1).toInt(),q->value(7).toInt())+
	QString().sprintf("LOG%d_LOG_LINE=%d,",
			  q->value(1).toInt(),q->value(8).toInt())+
	QString().sprintf("LOG%d_NOW_CART=%d,",
			  q->value(1).toInt(),q->value(9).toInt())+
	QString().sprintf("LOG%d_NEXT_CART=%d ",
			  q->value(1).toInt(),q->value(10).toInt())+
	"where STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    sql=QString("drop table LOG_MACHINES");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 277
  //
  if((cur_schema==277)&&(set_schema<cur_schema)) {
    sql=QString("alter table USERS drop column WEBGET_LOGIN_PRIV");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 276
  //
  if((cur_schema==276)&&(set_schema<cur_schema)) {
    sql=QString("alter table SYSTEM drop column NOTIFICATION_ADDRESS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 275
  //
  if((cur_schema==275)&&(set_schema<cur_schema)) {
    sql=QString("alter table SERVICES drop column LOG_SHELFLIFE_ORIGIN");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 274
  //
  if((cur_schema==274)&&(set_schema<cur_schema)) {
    sql=QString("alter table LOGS drop index LOCK_GUID_IDX");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_GUID");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 273
  //
  if((cur_schema==273)&&(set_schema<cur_schema)) {
    sql=QString("alter table LOGS drop column LOCK_DATETIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_IPV4_ADDRESS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_STATION_NAME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_USER_NAME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 272
  //
  if((cur_schema==272)&&(set_schema<cur_schema)) {
    sql=QString("drop table USER_SERVICE_PERMS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 271
  //
  if((cur_schema==271)&&(set_schema<cur_schema)) {
    sql=QString("alter table DROPBOXES drop column SEGUE_LEVEL, ")+
      "drop column SEGUE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 270
  //
  if((cur_schema==270)&&(set_schema<cur_schema)) {
    sql="alter table STATIONS drop column SHORT_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 269
  //
  if((cur_schema==269)&&(set_schema<cur_schema)) {
    sql="alter table GROUPS drop column DEFAULT_CUT_LIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 268
  //
  if((cur_schema==268)&&(set_schema<cur_schema)) {
    sql="alter table DROPBOXES drop column FORCE_TO_MONO";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 267
  //
  if((cur_schema==267)&&(set_schema<cur_schema)) {
    sql="alter table CUTS drop column ORIGIN_LOGIN_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    sql="alter table CUTS drop column SOURCE_HOSTNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 266
  //
  if((cur_schema==266)&&(set_schema<cur_schema)) {
    // Nothing to do here as this is a pseudo-schema change.

    cur_schema--;
  }

  //
  // Revert 265
  //
  if((cur_schema==265)&&(set_schema<cur_schema)) {
    sql="alter table SYSTEM drop column SHOW_USER_LIST";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 264
  //
  if((cur_schema==264)&&(set_schema<cur_schema)) {
    sql="alter table SYSTEM drop column FIX_DUP_CART_TITLES";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 263
  //
  if((cur_schema==263)&&(set_schema<cur_schema)) {
    // Nothing to do here as this is a pseudo-schema change.

    cur_schema--;
  }

  //
  // Revert 262
  //
  if((cur_schema==262)&&(set_schema<cur_schema)) {
    sql=QString("alter table USERS drop column LOCAL_AUTH");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USERS drop column PAM_SERVICE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("drop index IPV4_ADDRESS_IDX on STATIONS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    cur_schema--;
  }

  //
  // Revert 261
  //
  if((cur_schema==261)&&(set_schema<cur_schema)) {
    sql=QString("alter table CUTS drop column SHA1_HASH");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 260
  //
  if((cur_schema==260)&&(set_schema<cur_schema)) {
    sql=QString("alter table USERS drop column WEBAPI_AUTH_TIMEOUT");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("drop table WEBAPI_AUTHS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 259
  //
  if((cur_schema==259)&&(set_schema<cur_schema)) {
    for(int i=0;i<168;i++) {
      sql=QString().sprintf("alter table SERVICES add column CLOCK%d char(64)",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("select SERVICE_NAME,HOUR,CLOCK_NAME from SERVICE_CLOCKS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update SERVICES set ")+
	QString().sprintf("CLOCK%d=",q->value(1).toInt());
      if(q->value(2).isNull()) {
	sql+="null ";
      }
      else {
	sql+="\""+RDEscapeString(q->value(2).toString())+"\" ";
      }
      sql+="where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    sql=QString("drop table SERVICE_CLOCKS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 258
  //
  if((cur_schema==258)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table ")+
	"`"+tablename+"_LOG` "+
	"modify column CART_NUMBER int unsigned not null";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    cur_schema--;
  }

  //
  // Revert 257
  //
  if((cur_schema==257)&&(set_schema<cur_schema)) {
    sql=QString("alter table LOGS modify column LINK_DATETIME datetime not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS modify column START_DATE date not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS modify column END_DATE date not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 256
  //
  if((cur_schema==256)&&(set_schema<cur_schema)) {
    sql=QString("drop table CUT_EVENTS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("drop table DECK_EVENTS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 255
  //
  if((cur_schema==255)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT` drop column DESCRIPTION";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("alter table `")+tablename+"_SRT` drop column OUTCUE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    cur_schema--;
  }

  //
  // Revert 254
  //
  if((cur_schema==254)&&(set_schema<cur_schema)) {
    sql=QString("alter table CUTS drop column PLAY_ORDER");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART drop column USE_WEIGHTING");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 253
  //
  if((cur_schema==253)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 252
  //
  if((cur_schema==252)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 251
  //
  if((cur_schema==251)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 250
  //
  if((cur_schema==250)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 249
  //
  if((cur_schema==249)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 248
  //
  if((cur_schema==248)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 247
  //
  if((cur_schema==247)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 246
  //
  if((cur_schema==246)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    cur_schema--;
  }

  //
  // Revert 245
  //
  if((cur_schema==245)&&(set_schema<cur_schema)) {
    sql=QString("alter table RDLIBRARY drop column READ_ISRC");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 244
  //
  if((cur_schema==244)&&(set_schema<cur_schema)) {
    sql=QString("alter table JACK_CLIENTS modify column COMMAND_LINE ")+
      "char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Revert 243
  //
  if((cur_schema==243)&&(set_schema<cur_schema)) {
    sql=QString("alter table STATIONS drop column HAVE_MP4_DECODE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    cur_schema--;
  }

  //
  // Write the new schema version
  //
  sql=QString().sprintf("update VERSION set DB=%d",cur_schema);
  if(!RDSqlQuery::apply(sql,err_msg)) {
    return false;
  }

  *err_msg="ok";
  return true;
}
