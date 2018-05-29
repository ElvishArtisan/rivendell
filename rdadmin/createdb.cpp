// createdb.cpp
//
// Create, Initialize and/or Update a Rivendell Database
//
//   (C) Copyright 2002-2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <qapplication.h>

#include <dbversion.h>
#include <rd.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rdconfig.h>
#include <rdcreate_log.h>
#include <rdcreateauxfieldstable.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdevent.h>
#include <rdfeedlog.h>
#include <rdlog.h>
#include <rdlog_line.h>
#include <rdmacro_event.h>
#include <rdpaths.h>
#include <rdrecording.h>
#include <rdsvc.h>
#include <rdtempdirectory.h>

#include "createdb.h"
#include "globals.h"

//
// NOTE TO MAINTAINERS:
//
// Be sure to invoke RDSqlQuery with automatic DB reconnection
// disabled throughout this file (by providing 'false' as the second argument
// to the constructor), otherwise the automatic reconnects will kill outer
// sql 'select' loops when schema updates are reapplied!
//

bool RunQuery(QString sql)
{
  RDSqlQuery *q=new RDSqlQuery(sql,false);
  if(!q->isActive()) {
    fprintf(stderr,"SQL: %s\n",(const char *)sql);
    fprintf(stderr,"SQL Error: %s\n",(const char *)q->lastError().databaseText());
    delete q;
    return false;
  }
  delete q;
  return true;
}


bool UpdateRDAirplayHotkeys(QString current_station)
{
  QString sql;
  QStringList labels;

  labels.push_back(QObject::tr("Start Line 1"));
  labels.push_back(QObject::tr("Stop Line 1"));
  labels.push_back(QObject::tr("Pause Line 1"));
  labels.push_back(QObject::tr("Start Line 2"));
  labels.push_back(QObject::tr("Stop Line 2"));
  labels.push_back(QObject::tr("Pause Line 2"));
  labels.push_back(QObject::tr("Start Line 3"));
  labels.push_back(QObject::tr("Stop Line 3"));
  labels.push_back(QObject::tr("Pause Line 3"));
  labels.push_back(QObject::tr("Start Line 4"));
  labels.push_back(QObject::tr("Stop Line 4"));
  labels.push_back(QObject::tr("Pause Line 4"));
  labels.push_back(QObject::tr("Start Line 5"));
  labels.push_back(QObject::tr("Stop Line 5"));
  labels.push_back(QObject::tr("Pause Line 5"));
  labels.push_back(QObject::tr("Start Line 6"));
  labels.push_back(QObject::tr("Stop Line 6"));
  labels.push_back(QObject::tr("Pause Line 6"));
  labels.push_back(QObject::tr("Start Line 7"));
  labels.push_back(QObject::tr("Stop Line 7"));
  labels.push_back(QObject::tr("Pause Line 7"));
  labels.push_back(QObject::tr("Add"));
  labels.push_back(QObject::tr("Delete"));
  labels.push_back(QObject::tr("Copy"));
  labels.push_back(QObject::tr("Move"));
  labels.push_back(QObject::tr("Sound Panel"));
  labels.push_back(QObject::tr("Main Log"));
  labels.push_back(QObject::tr("Aux Log 1"));
  labels.push_back(QObject::tr("Aux Log 2"));

  for(unsigned i=0;i<labels.size();i++) {
    sql=QString("insert into RDHOTKEYS set ")+
      "STATION_NAME=\""+RDEscapeString(current_station)+"\","+
      "MODULE_NAME=\"airplay\","+
      QString().sprintf("KEY_ID=%u,",i+1)+
      "KEY_LABEL=\""+RDEscapeString(labels[i])+"\"";
    if(!RunQuery(sql)) {
      return false;
    }
  }
  return true;
}

void UpdateImportFormats()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"Rivendell Standard Import\","+
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
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"PowerGold Music Scheduling\","+
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
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"RadioTraffic.com\","+
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
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"CounterPoint Traffic\","+
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
    "EVENT_ID_OFFSET=69,"+
    "EVENT_ID_LENGTH=32,"+
    "DATA_OFFSET=102,"+
    "DATA_LENGTH=32";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"WideOrbit Traffic\","+
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
    "EVENT_ID_OFFSET=69,"+
    "EVENT_ID_LENGTH=32,"+
    "DATA_OFFSET=102,"+
    "DATA_LENGTH=32";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"Visual Traffic\","+
    "CART_OFFSET=14,"+
    "CART_LENGTH=6,"+
    "TITLE_OFFSET=25,"+
    "TITLE_LENGTH=39,"+
    "HOURS_OFFSET=5,"+
    "HOURS_LENGTH=2,"+
    "MINUTES_OFFSET=8,"+
    "MINUTES_LENGTH=2,"+
    "SECONDS_OFFSET=11,"+
    "SECONDS_LENGTH=2,"+
    "LEN_HOURS_OFFSET=0,"+
    "LEN_HOURS_LENGTH=0,"+
    "LEN_MINUTES_OFFSET=97,"+
    "LEN_MINUTES_LENGTH=2,"+
    "LEN_SECONDS_OFFSET=99,"+
    "LEN_SECONDS_LENGTH=2,"+
    "EVENT_ID_OFFSET=0,"+
    "EVENT_ID_LENGTH=0,"+
    "DATA_OFFSET=0,"+
    "DATA_LENGTH=0";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"Music 1\","+
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
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("insert into IMPORT_TEMPLATES set ")+
    "NAME=\"NaturalLog\","+
    "CART_OFFSET=9,"+
    "CART_LENGTH=6,"+
    "TITLE_OFFSET=19,"+
    "TITLE_LENGTH=40,"+
    "HOURS_OFFSET=0,"+
    "HOURS_LENGTH=2,"+
    "MINUTES_OFFSET=3,"+
    "MINUTES_LENGTH=2,"+
    "SECONDS_OFFSET=6,"+
    "SECONDS_LENGTH=2,"+
    "LEN_HOURS_OFFSET=61,"+
    "LEN_HOURS_LENGTH=2,"+
    "LEN_MINUTES_OFFSET=64,"+
    "LEN_MINUTES_LENGTH=2,"+
    "LEN_SECONDS_OFFSET=67,"+
    "LEN_SECONDS_LENGTH=2,"+
    "DATA_OFFSET=0,"+
    "DATA_LENGTH=0";
  q=new RDSqlQuery(sql,false);
  delete q;
}


bool CreateDb(QString name,QString pwd,RDConfig *config)
{
  QString sql;

  //
  // Create USERS table
  //
  sql=QString("create table if not exists USERS (")+
    "LOGIN_NAME char(255) not null primary key,"+
    "FULL_NAME char(255),"+
    "PHONE_NUMBER char(20),"+
    "DESCRIPTION char(255),"+
    "PASSWORD char(32),"+
    "WEBAPI_AUTH_TIMEOUT int not null default 3600,"+
    "ENABLE_WEB enum('N','Y') default 'N',"+
    "LOCAL_AUTH enum('N','Y') default 'Y',"+
    "PAM_SERVICE char(32) default \"rivendell\","+
    "ADMIN_USERS_PRIV enum('N','Y') not null default 'N',"+
    "ADMIN_CONFIG_PRIV enum('N','Y') not null default 'N',"+
    "CREATE_CARTS_PRIV enum('N','Y') not null default 'N',"+
    "DELETE_CARTS_PRIV enum('N','Y') not null default 'N',"+
    "MODIFY_CARTS_PRIV enum('N','Y') not null default 'N',"+
    "EDIT_AUDIO_PRIV enum('N','Y') not null default 'N',"+
    "WEBGET_LOGIN_PRIV enum('N','Y') not null default 'N',"+
    "ASSIGN_CART_PRIV enum('N','Y') not null default 'N',"+
    "CREATE_LOG_PRIV enum('N','Y') not null default 'N',"+
    "DELETE_LOG_PRIV enum('N','Y') not null default 'N',"+
    "DELETE_REC_PRIV enum('N','Y') not null default 'N',"+
    "PLAYOUT_LOG_PRIV enum('N','Y') not null default 'N',"+
    "ARRANGE_LOG_PRIV enum('N','Y') not null default 'N',"+
    "MODIFY_TEMPLATE_PRIV enum('N','Y') not null default 'N',"+
    "ADDTO_LOG_PRIV enum('N','Y') not null default 'N',"+
    "REMOVEFROM_LOG_PRIV enum('N','Y') not null default 'N',"+
    "CONFIG_PANELS_PRIV enum('N','Y') not null default 'N',"+
    "VOICETRACK_LOG_PRIV enum('N','Y') not null default 'N',"+
    "EDIT_CATCHES_PRIV enum('N','Y') not null default 'N',"+
    "ADD_PODCAST_PRIV enum('N','Y') not null default 'N',"+
    "EDIT_PODCAST_PRIV enum('N','Y') not null default 'N',"+
    "DELETE_PODCAST_PRIV enum('N','Y') not null default 'N',"+
    "INDEX FULL_NAME_IDX (FULL_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create STATIONS table
  //
  sql=QString("create table if not exists STATIONS (")+
    "NAME char(64) primary key not null,"+
    "SHORT_NAME char(64),"+
    "DESCRIPTION char(64),"+
    "USER_NAME char(255),"+
    "DEFAULT_NAME char(255),"+
    "IPV4_ADDRESS char(15) default \"127.0.0.2\","+
    "HTTP_STATION char(64) default \"localhost\","+
    "CAE_STATION char(64) default \"localhost\","+
    "TIME_OFFSET int default 0,"+
    "BACKUP_DIR char(255),"+
    "BACKUP_LIFE int default 0,"+
    "BROADCAST_SECURITY int unsigned default 0,"+
    "HEARTBEAT_CART int unsigned default 0,"+
    "HEARTBEAT_INTERVAL int unsigned default 0,"+
    "STARTUP_CART int unsigned default 0,"+
    "EDITOR_PATH char(255) default \"\","+
    "FILTER_MODE int default 0,"+
    "START_JACK enum('N','Y') default 'N',"+
    "JACK_SERVER_NAME char(64),"+
    "JACK_COMMAND_LINE char(255),"+
    "CUE_CARD int default 0,"+
    "CUE_PORT int default 0,"+
    "CUE_START_CART int unsigned,"+
    "CUE_STOP_CART int unsigned,"+
    "CARTSLOT_COLUMNS int default 1,"+
    "CARTSLOT_ROWS int default 8,"+
    "ENABLE_DRAGDROP enum('N','Y') default 'Y',"+
    "ENFORCE_PANEL_SETUP enum('N','Y') default 'N',"+
    "SYSTEM_MAINT enum('N','Y') default 'Y',"+
    "STATION_SCANNED enum('N','Y') default 'N',"+
    "HAVE_OGGENC enum('N','Y') default 'N',"+
    "HAVE_OGG123 enum('N','Y') default 'N',"+
    "HAVE_FLAC enum('N','Y') default 'N',"+
    "HAVE_TWOLAME enum('N','Y') default 'N',"+
    "HAVE_LAME enum('N','Y') default 'N',"+
    "HAVE_MPG321 enum('N','Y') default 'N',"+
    "HAVE_MP4_DECODE enum('N','Y') default 'N',"+
    "HPI_VERSION char(16),"+
    "JACK_VERSION char(16),"+
    "ALSA_VERSION char(16),"+
    "INDEX DESCRIPTION_IDX (DESCRIPTION),"+
    "index IPV4_ADDRESS_IDX (IPV4_ADDRESS))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create CART table
  //
  sql=QString("create table if not exists CART (")+
    "NUMBER int unsigned not null primary key,"+
    "TYPE int unsigned not null,"+
    "GROUP_NAME char(10) not null,"+
    "TITLE char(255),"+
    "ARTIST char(255),"+
    "ALBUM char(255),"+
    "YEAR date,"+
    "CONDUCTOR char(64),"+
    "LABEL char(64),"+
    "CLIENT char(64),"+
    "AGENCY char(64),"+
    "PUBLISHER char(64),"+
    "COMPOSER char(64),"+
    "USER_DEFINED char(255),"+
    "SONG_ID char(32),"+
    "BPM int unsigned default 0,"+
    "USAGE_CODE int default 0,"+
    "FORCED_LENGTH int unsigned,"+
    "AVERAGE_LENGTH int unsigned,"+
    "LENGTH_DEVIATION int unsigned default 0,"+
    "AVERAGE_SEGUE_LENGTH int unsigned,"+
    "AVERAGE_HOOK_LENGTH int unsigned default 0,"+
    "CUT_QUANTITY int unsigned,"+
    "LAST_CUT_PLAYED int unsigned,"+
    "PLAY_ORDER int unsigned,"+
    "VALIDITY int unsigned default 2,"+
    "START_DATETIME datetime,"+
    "END_DATETIME datetime,"+
    "ENFORCE_LENGTH enum('N','Y') default 'N',"+
    "PRESERVE_PITCH enum('N','Y') default 'N',"+
    "USE_WEIGHTING enum('N','Y') default 'Y',"+
    "ASYNCRONOUS enum('N','Y') default 'N',"+
    "OWNER char(64),"+
    "MACROS text,"+
    "SCHED_CODES VARCHAR( 255 ) NULL default NULL,"+
    "NOTES text,"+
    "METADATA_DATETIME datetime,"+
    "USE_EVENT_LENGTH enum('N','Y') default 'N',"+
    "PENDING_STATION char(64),"+
    "PENDING_PID int,"+
    "PENDING_DATETIME datetime,"+
    "index GROUP_NAME_IDX (GROUP_NAME),"+
    "index TITLE_IDX (TITLE),"+
    "index ARTIST_IDX (ARTIST),"+
    "index ALBUM_IDX (ALBUM),"+
    "index CONDUCTOR_IDX (CONDUCTOR),"+
    "index LABEL_IDX (LABEL),"+
    "index CLIENT_IDX (CLIENT),"+
    "index AGENCY_IDX (AGENCY),"+
    "index PUBLISHER_IDX (PUBLISHER),"+
    "index COMPOSER_IDX (COMPOSER),"+
    "index USER_DEFINED_IDX (USER_DEFINED),"+
    "index SONG_ID_IDX (SONG_ID),"+
    "index OWNER_IDX (OWNER),"+
    "index METADATA_DATETIME_IDX (METADATA_DATETIME),"+
    "index PENDING_STATION_IDX(PENDING_STATION),"+
    "index PENDING_PID_IDX(PENDING_STATION,PENDING_PID),"+
    "index PENDING_DATETIME_IDX(PENDING_DATETIME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create CUTS table
  //
  sql=QString("create table if not exists CUTS (")+
    "CUT_NAME char(12) primary key not null,"+
    "CART_NUMBER int unsigned not null,"+
    "EVERGREEN enum('N','Y') default 'N',"+
    "DESCRIPTION char(64),"+
    "OUTCUE char(64),"+
    "ISRC char(12),"+
    "ISCI char(32),"+
    "LENGTH int unsigned,"+
    "SHA1_HASH char(40),"+
    "ORIGIN_DATETIME datetime,"+
    "START_DATETIME datetime,"+
    "END_DATETIME datetime,"+
    "SUN enum('N','Y') default 'Y',"+
    "MON enum('N','Y') default 'Y',"+
    "TUE enum('N','Y') default 'Y',"+
    "WED enum('N','Y') default 'Y',"+
    "THU enum('N','Y') default 'Y',"+
    "FRI enum('N','Y') default 'Y',"+
    "SAT enum('N','Y') default 'Y',"+
    "START_DAYPART time,"+
    "END_DAYPART time,"+
    "ORIGIN_NAME char(64),"+
    "ORIGIN_LOGIN_NAME char(255),"+
    "SOURCE_HOSTNAME char(255),"+
    "WEIGHT int unsigned default 1,"+
    "PLAY_ORDER int,"+
    "LAST_PLAY_DATETIME datetime,"+
    "UPLOAD_DATETIME datetime,"+
    "PLAY_COUNTER int unsigned default 0,"+
    "LOCAL_COUNTER int unsigned default 0,"+
    "VALIDITY int unsigned default 2,"+
    "CODING_FORMAT int unsigned,"+
    "SAMPLE_RATE int unsigned,"+
    "BIT_RATE int unsigned,"+
    "CHANNELS int unsigned,"+
    "PLAY_GAIN int default 0,"+
    "START_POINT int default -1,"+
    "END_POINT int default -1,"+
    "FADEUP_POINT int default -1,"+
    "FADEDOWN_POINT int default -1,"+
    "SEGUE_START_POINT int default -1,"+
    "SEGUE_END_POINT int default -1,"+
    QString().sprintf("SEGUE_GAIN int default %d,",RD_FADE_DEPTH)+
    "HOOK_START_POINT int default -1,"+
    "HOOK_END_POINT int default -1,"+
    "TALK_START_POINT int default -1,"+
    "TALK_END_POINT int default -1,"+
    "index CART_NUMBER_IDX (CART_NUMBER),"+
    "index DESCRIPTION_IDX (DESCRIPTION),"+
    "index OUTCUE_IDX (OUTCUE),"+
    "index ORIGIN_DATETIME_IDX (ORIGIN_DATETIME),"+
    "index START_DATETIME_IDX (START_DATETIME),"+
    "index END_DATETIME_IDX (END_DATETIME),"+
    "index ISCI_IDX (ISCI),"+
    "index ISRC_IDX (ISRC))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create CLIPBOARD table
  //
  sql=QString("create table if not exists CLIPBOARD (")+
    "CUT_NAME char(12) primary key not null,"+
    "CART_NUMBER int unsigned not null,"+
    "DESCRIPTION char(64),"+
    "OUTCUE char(64),"+
    "LENGTH int unsigned,"+
    "ORIGIN_DATETIME datetime,"+
    "ORIGIN_NAME char(64),"+
    "WEIGHT int unsigned default 1,"+
    "LAST_PLAY_DATETIME datetime,"+
    "PLAY_COUNTER int unsigned default 0,"+
    "CODING_FORMAT int unsigned,"+
    "SAMPLE_RATE int unsigned,"+
    "BIT_RATE int unsigned,"+
    "CHANNELS int unsigned,"+
    "PLAY_GAIN int default 0,"+
    "START_POINT int default -1,"+
    "END_POINT int default -1,"+
    "FADEUP_POINT int default -1,"+
    "FADEDOWN_POINT int default -1,"+
    "SEGUE_START_POINT int default -1,"+
    "SEGUE_END_POINT int default -1,"+
    "HOOK_START_POINT int default -1,"+
    "HOOK_END_POINT int default -1,"+
    "TALK_START_POINT int default -1,"+
    "TALK_END_POINT int default -1,"+
    "index CART_NUMBER_IDX (CART_NUMBER),"+
    "index DESCRIPTION_IDX (DESCRIPTION),"+
    "index OUTCUE_IDX (OUTCUE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SERVICES table
  //
  sql=QString("create table if not exists SERVICES (")+
    "NAME char(10) not null primary key,"+
    "DESCRIPTION char(255),"+
    "NAME_TEMPLATE char(255),"+
    "DESCRIPTION_TEMPLATE char(255),"+
    "PROGRAM_CODE char(255),"+
    "CHAIN_LOG enum('N','Y') default 'N',"+
    "TRACK_GROUP char(10),"+
    "AUTOSPOT_GROUP char(10),"+
    "AUTO_REFRESH enum('N','Y') default 'N',"+
    "DEFAULT_LOG_SHELFLIFE int default -1,"+
    "LOG_SHELFLIFE_ORIGIN int default 0,"+
    "ELR_SHELFLIFE int default -1,"+
    "TFC_PATH char(255),"+
    "TFC_PREIMPORT_CMD text,"+
    "TFC_WIN_PATH char(255),"+
    "TFC_WIN_PREIMPORT_CMD text,"+
    "TFC_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\","+
    "TFC_LABEL_CART char(32),"+
    "TFC_TRACK_CART char(32),"+
    "TFC_BREAK_STRING char(64),"+
    "TFC_TRACK_STRING char(64),"+
    "TFC_CART_OFFSET int,"+
    "TFC_CART_LENGTH int,"+
    "TFC_TITLE_OFFSET int,"+
    "TFC_TITLE_LENGTH int,"+
    "TFC_HOURS_OFFSET int,"+
    "TFC_HOURS_LENGTH int,"+
    "TFC_MINUTES_OFFSET int,"+
    "TFC_MINUTES_LENGTH int,"+
    "TFC_SECONDS_OFFSET int,"+
    "TFC_SECONDS_LENGTH int,"+
    "TFC_LEN_HOURS_OFFSET int,"+
    "TFC_LEN_HOURS_LENGTH int,"+
    "TFC_LEN_MINUTES_OFFSET int,"+
    "TFC_LEN_MINUTES_LENGTH int,"+
    "TFC_LEN_SECONDS_OFFSET int,"+
    "TFC_LEN_SECONDS_LENGTH int,"+
    "TFC_LENGTH_OFFSET int,"+
    "TFC_LENGTH_LENGTH int,"+
    "TFC_DATA_OFFSET int,"+
    "TFC_DATA_LENGTH int,"+
    "TFC_EVENT_ID_OFFSET int,"+
    "TFC_EVENT_ID_LENGTH int,"+
    "TFC_ANNC_TYPE_OFFSET int,"+
    "TFC_ANNC_TYPE_LENGTH int,"+
    "MUS_PATH char(255),"+
    "MUS_PREIMPORT_CMD text,"+
    "MUS_WIN_PATH char(255),"+
    "MUS_WIN_PREIMPORT_CMD text,"+
    "MUS_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\","+
    "MUS_LABEL_CART char(32),"+
    "MUS_TRACK_CART char(32),"+
    "MUS_BREAK_STRING char(64),"+
    "MUS_TRACK_STRING char(64),"+
    "MUS_CART_OFFSET int,"+
    "MUS_CART_LENGTH int,"+
    "MUS_TITLE_OFFSET int,"+
    "MUS_TITLE_LENGTH int,"+
    "MUS_HOURS_OFFSET int,"+
    "MUS_HOURS_LENGTH int,"+
    "MUS_MINUTES_OFFSET int,"+
    "MUS_MINUTES_LENGTH int,"+
    "MUS_SECONDS_OFFSET int,"+
    "MUS_SECONDS_LENGTH int,"+
    "MUS_LEN_HOURS_OFFSET int,"+
    "MUS_LEN_HOURS_LENGTH int,"+
    "MUS_LEN_MINUTES_OFFSET int,"+
    "MUS_LEN_MINUTES_LENGTH int,"+
    "MUS_LEN_SECONDS_OFFSET int,"+
    "MUS_LEN_SECONDS_LENGTH int,"+
    "MUS_LENGTH_OFFSET int,"+
    "MUS_LENGTH_LENGTH int,"+
    "MUS_DATA_OFFSET int,"+
    "MUS_DATA_LENGTH int,"+
    "MUS_EVENT_ID_OFFSET int,"+
    "MUS_EVENT_ID_LENGTH int,"+
    "MUS_ANNC_TYPE_OFFSET int,"+
    "MUS_ANNC_TYPE_LENGTH int)"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create GROUPS table
  //
  sql=QString("create table if not exists GROUPS (")+
    "NAME char(10) not null primary key,"+
    "DESCRIPTION char(255),"+
    "DEFAULT_CART_TYPE int unsigned default 1,"+
    "DEFAULT_LOW_CART int unsigned default 0,"+
    "DEFAULT_HIGH_CART int unsigned default 0,"+
    "DEFAULT_CUT_LIFE int default -1,"+
    "CUT_SHELFLIFE int default -1,"+
    "DELETE_EMPTY_CARTS enum('N','Y') default 'N',"+
    "DEFAULT_TITLE char(255) default \"Imported from %f.%e\","+
    "ENFORCE_CART_RANGE enum('N','Y') default 'N',"+
    "REPORT_TFC enum('N','Y') default 'Y',"+
    "REPORT_MUS enum('N','Y') default 'Y',"+
    "ENABLE_NOW_NEXT enum('N','Y') default 'N',"+
    "COLOR char(7),"+
    "index IDX_REPORT_TFC (REPORT_TFC),"+
    "index IDX_REPORT_MUS (REPORT_MUS))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create AUDIO_PERMS table
  //
  sql=QString("create table if not exists AUDIO_PERMS (")+
    "ID int unsigned AUTO_INCREMENT not null primary key,"+
    "GROUP_NAME char(10),"+
    "SERVICE_NAME char(10),"+
    "index GROUP_IDX (GROUP_NAME),"+
    "index SERVICE_IDX (SERVICE_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDLIBRARY table
  //
  sql=QString("create table if not exists RDLIBRARY (")+
    "ID int unsigned primary key AUTO_INCREMENT,"+
    "STATION char(40) not null,"+
    "INSTANCE int unsigned not null,"+
    "INPUT_CARD int default 0,"+
    "INPUT_PORT int default 0,"+
    "INPUT_TYPE enum('A','D') default 'A',"+
    "OUTPUT_CARD int default 0,"+
    "OUTPUT_PORT int default 0,"+
    "VOX_THRESHOLD int default -5000,"+
    "TRIM_THRESHOLD int default 0,"+
    "DEFAULT_FORMAT int unsigned default 0,"+
    "DEFAULT_CHANNELS int unsigned default 2,"+
    "DEFAULT_LAYER int unsigned default 0,"+
    "DEFAULT_BITRATE int unsigned default 0,"+
    "DEFAULT_RECORD_MODE int unsigned default 0,"+
    "DEFAULT_TRIM_STATE enum('N','Y') default 'N',"+
    "MAXLENGTH int,"+
    "TAIL_PREROLL int unsigned default 1500,"+
    "RIPPER_DEVICE char(64) default \"/dev/cdrom\","+
    "PARANOIA_LEVEL int default 0,"+
    "RIPPER_LEVEL int default -1300,"+
    "CDDB_SERVER char(64) default \"freedb.freedb.org\","+
    "READ_ISRC enum('N','Y') default 'Y',"+
    "ENABLE_EDITOR enum('N','Y') default 'N',"+
    "SRC_CONVERTER int default 1,"+
    "LIMIT_SEARCH int default 1,"+
    "SEARCH_LIMITED enum('N','Y') default 'Y',"+
    "index STATION_IDX (STATION,INSTANCE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create TRIGGERS table
  //
  sql=QString("create table if not exists TRIGGERS (")+
    "ID int unsigned AUTO_INCREMENT not null primary key,"+
    "CUT_NAME char(12),"+
    "TRIGGER_CODE int unsigned,"+
    "OFFSET int unsigned,"+
    "index CUT_NAME_IDX (CUT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create TTYS table
  //
  sql=QString("create table if not exists TTYS (")+
    "ID int unsigned not null primary key AUTO_INCREMENT,"+
    "PORT_ID int unsigned not null,"+
    "ACTIVE enum('N','Y') not null default 'N',"+
    "STATION_NAME char(64) not null,"+
    "PORT char(20),"+
    "BAUD_RATE int default 9600,"+
    "DATA_BITS int default 8,"+
    "STOP_BITS int default 1,"+
    "PARITY int default 0,"+
    "TERMINATION int default 0,"+
    "index STATION_NAME_IDX (STATION_NAME),"+
    "index ACTIVE_IDX (ACTIVE),"+
    "index PORT_ID_IDX (PORT_ID))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create DECKS table
  //
  sql=QString("create table if not exists DECKS (")+
    "ID int unsigned not null primary key AUTO_INCREMENT,"+
    "STATION_NAME char(64) not null,"+
    "CHANNEL int unsigned not null,"+
    "CARD_NUMBER int default -1,"+
    "STREAM_NUMBER int default -1,"+
    "PORT_NUMBER int default -1,"+
    "MON_PORT_NUMBER int default -1,"+
    "DEFAULT_MONITOR_ON enum('N','Y') default 'N',"+
    "PORT_TYPE enum('A','D') default 'A',"+
    "DEFAULT_FORMAT int default 0,"+
    "DEFAULT_CHANNELS int default 2,"+
    "DEFAULT_BITRATE int default 0,"+
    "DEFAULT_THRESHOLD int default 0,"+
    "SWITCH_STATION char(64),"+
    "SWITCH_MATRIX int default -1,"+
    "SWITCH_OUTPUT int default -1,"+
    "SWITCH_DELAY int default 0,"+
    "index STATION_NAME_IDX (STATION_NAME),"+
    "index CHANNEL_IDX (CHANNEL))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RECORDINGS table
  //
  sql=QString("create table if not exists RECORDINGS (")+
    "ID int unsigned not null primary key AUTO_INCREMENT,"+
    "IS_ACTIVE enum('N','Y') default 'Y',"+
    "STATION_NAME char(64) not null,"+
    "TYPE int default 0,"+
    "CHANNEL int unsigned not null,"+
    "CUT_NAME char(12) not null,"+
    "SUN enum('N','Y') default 'N',"+
    "MON enum('N','Y') default 'N',"+
    "TUE enum('N','Y') default 'N',"+
    "WED enum('N','Y') default 'N',"+
    "THU enum('N','Y') default 'N',"+
    "FRI enum('N','Y') default 'N',"+
    "SAT enum('N','Y') default 'N',"+
    "DESCRIPTION char(64),"+
    "START_TYPE int unsigned default 0,"+
    "START_TIME time,"+
    "START_LENGTH int default 0,"+
    "START_MATRIX int default -1,"+
    "START_LINE int default -1,"+
    "START_OFFSET int default 0,"+
    "END_TYPE int default 0,"+
    "END_TIME time,"+
    "END_LENGTH int default 0,"+
    "END_MATRIX int default -1,"+
    "END_LINE int default -1,"+
    "LENGTH int unsigned,"+
    "START_GPI int default -1,"+
    "END_GPI int default -1,"+
    "ALLOW_MULT_RECS enum('N','Y') default 'N',"+
    "MAX_GPI_REC_LENGTH int unsigned default 3600000,"+
    "TRIM_THRESHOLD int,"+
    "NORMALIZE_LEVEL int default -1300,"+
    "STARTDATE_OFFSET int unsigned default 0,"+
    "ENDDATE_OFFSET int unsigned default 0,"+
    "EVENTDATE_OFFSET int default 0,"+
    "FORMAT int default 0,"+
    "CHANNELS int default 2,"+
    "BITRATE int default 0,"+
    "QUALITY int default 0,"+
    "MACRO_CART int default -1,"+
    "SWITCH_INPUT int default -1,"+
    "SWITCH_OUTPUT int default -1,"+
    "EXIT_CODE int default 0,"+
    "EXIT_TEXT text,"+
    "ONE_SHOT enum('N','Y') default 'N',"+
    "URL char(255),"+
    "URL_USERNAME char(64),"+
    "URL_PASSWORD char(64),"+
    "ENABLE_METADATA enum('N','Y') default 'N',"+
    "FEED_ID int default -1,"+
    "index STATION_NAME_IDX (STATION_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create AUDIO_PORTS table
  //
  /*
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }
  */
  //
  // Create LOGS table
  //
  sql=QString("create table if not exists LOGS (")+
    "NAME char(64) not null primary key,"+
    "LOG_EXISTS enum('N','Y') default 'Y',"+
    "TYPE int not null default 0,"+
    "SERVICE char(10) not null,"+
    "DESCRIPTION char(64),"+
    "ORIGIN_USER char(255) not null,"+
    "ORIGIN_DATETIME datetime not null,"+
    "LINK_DATETIME datetime,"+
    "MODIFIED_DATETIME datetime not null,"+
    "AUTO_REFRESH enum('N','Y') default 'N',"+
    "START_DATE date,"+
    "END_DATE date,"+
    "PURGE_DATE date,"+
    "IMPORT_DATE date,"+
    "SCHEDULED_TRACKS int unsigned default 0,"+
    "COMPLETED_TRACKS int unsigned default 0,"+
    "MUSIC_LINKS int default 0,"+
    "MUSIC_LINKED enum('N','Y') default 'N',"+
    "TRAFFIC_LINKS int default 0,"+
    "TRAFFIC_LINKED enum('N','Y') default 'N',"+
    "NEXT_ID int default 0,"+
    "LOCK_USER_NAME char(255),"+
    "LOCK_STATION_NAME char(64),"+
    "LOCK_IPV4_ADDRESS char(16),"+
    "LOCK_DATETIME datetime,"+
    "LOCK_GUID char(82),"+
    "index NAME_IDX (NAME,LOG_EXISTS),"+
    "index SERVICE_IDX (SERVICE),"+
    "index DESCRIPTION_IDX (DESCRIPTION),"+
    "index ORIGIN_USER_IDX (ORIGIN_USER),"+
    "index START_DATE_IDX (START_DATE),"+
    "index END_DATE_IDX (END_DATE),"+
    "index TYPE_IDX(TYPE,LOG_EXISTS),"+
    "index LOCK_GUID_IDX(LOCK_GUID))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create VERSION table
  //
  sql=QString("create table if not exists VERSION (")+
    "DB int not null primary key,"+
    "LAST_MAINT_DATETIME datetime default \"1970-01-01 00:00:00\","+
    "LAST_ISCI_XREFERENCE datetime default \"1970-01-01 00:00:00\")"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDAIRPLAY Table
  //
  sql=QString("create table if not exists RDAIRPLAY (")+
    "ID int not null primary key auto_increment,"+
    "STATION char(40) not null,"+
    "SEGUE_LENGTH int default 250,"+
    "TRANS_LENGTH int default 50,"+
    "LOG_MODE_STYLE int default 0,"+
    "PIE_COUNT_LENGTH int default 15000,"+
    "PIE_COUNT_ENDPOINT int default 0,"+
    "CHECK_TIMESYNC enum('N','Y') default 'N',"+
    "STATION_PANELS int default 3,"+
    "USER_PANELS int default 3,"+
    "SHOW_AUX_1 enum('N','Y') default 'Y',"+
    "SHOW_AUX_2 enum('N','Y') default 'Y',"+
    "CLEAR_FILTER enum('N','Y') default 'N',"+
    "DEFAULT_TRANS_TYPE int default 0,"+
    "BAR_ACTION int unsigned default 0,"+
    "FLASH_PANEL enum('N','Y') default 'N',"+
    "PANEL_PAUSE_ENABLED enum('N','Y') default 'N',"+
    "BUTTON_LABEL_TEMPLATE char(32) default '%t',"+
    "PAUSE_ENABLED enum('N','Y'),"+
    "DEFAULT_SERVICE char(10),"+
    "HOUR_SELECTOR_ENABLED enum('N','Y') default 'N',"+
    "TITLE_TEMPLATE char(64) default '%t',"+
    "ARTIST_TEMPLATE char(64) default '%a',"+
    "OUTCUE_TEMPLATE char(64) default '%o',"+
    "DESCRIPTION_TEMPLATE char(64) default '%i',"+
    "EXIT_CODE int default 0,"+
    "VIRTUAL_EXIT_CODE int default 0,"+
    "EXIT_PASSWORD char(41) default \"\","+
    "SKIN_PATH char(255) default \""+
    RDEscapeString(RD_DEFAULT_RDAIRPLAY_SKIN)+"\","+
    "SHOW_COUNTERS enum('N','Y') default 'N',"+
    "AUDITION_PREROLL int default 10000,"+
    "index STATION_IDX (STATION))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create PANELS Table
  //
  sql=QString("create table if not exists PANELS (")+
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create MATRICES Table
  //
  sql=QString("create table if not exists MATRICES (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "NAME char(64),"+
    "MATRIX int not null,"+
    "TYPE int not null,"+
    "LAYER int default 86,"+
    "PORT_TYPE int default 0,"+
    "PORT_TYPE_2 int default 0,"+
    "CARD int default -1,"+
    "PORT int not null default 0,"+
    "PORT_2 int not null default 0,"+
    "IP_ADDRESS char(16),"+
    "IP_ADDRESS_2 char(16),"+
    "IP_PORT int,"+
    "IP_PORT_2 int,"+
    "USERNAME char(32),"+
    "USERNAME_2 char(32),"+
    "PASSWORD char(32),"+
    "PASSWORD_2 char(32),"+
    "START_CART int unsigned,"+
    "STOP_CART int unsigned,"+
    "START_CART_2 int unsigned,"+
    "STOP_CART_2 int unsigned,"+
    "GPIO_DEVICE char(255),"+
    "INPUTS int not null default 0,"+
    "OUTPUTS int not null default 0,"+
    "GPIS int not null default 0,"+
    "GPOS int not null default 0,"+
    "FADERS int default 0,"+
    "DISPLAYS int default 0,"+
    "index MATRIX_IDX (STATION_NAME,MATRIX))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    "CHANNEL_MODE int,"+
    "ENGINE_NUM int default -1,"+
    "DEVICE_NUM int default -1,"+
    "NODE_HOSTNAME char(255),"+
    "NODE_TCP_PORT int,"+
    "NODE_SLOT int,"+
    "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER),"+
    "index NODE_IDX (STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    "ENGINE_NUM int default -1,"+
    "DEVICE_NUM int default -1,"+
    "NODE_HOSTNAME char(255),"+
    "NODE_TCP_PORT int,"+
    "NODE_SLOT int,"+
    "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER),"+
    "index NODE_IDX (STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create GPIS Table
  //
  sql=QString("create table if not exists GPIS (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "MATRIX int not null,"+
    "NUMBER int not null,"+
    "MACRO_CART int default 0,"+
    "OFF_MACRO_CART int default 0,"+
    "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create EVENTS table
  //
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
    "AUTOFILL_SLOP int default -1,"+
    "USE_TIMESCALE enum('N','Y') default 'N',"+
    "IMPORT_SOURCE int default 0,"+
    "START_SLOP int default 0,"+
    "END_SLOP int default 0,"+
    "FIRST_TRANS_TYPE int default 0,"+
    "DEFAULT_TRANS_TYPE int default 0,"+
    "COLOR char(7),"+
    "SCHED_GROUP VARCHAR(10),"+
    "TITLE_SEP INT(10) unsigned,"+
    "HAVE_CODE VARCHAR(10),"+
    "HAVE_CODE2 VARCHAR(10),"+
    "HOR_SEP INT(10) unsigned,"+
    "HOR_DIST INT(10) unsigned,"+
    "NESTED_EVENT char(64),"+
    "REMARKS char(255))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create CLOCKS table
  //
  sql=QString("create table if not exists CLOCKS (")+
    "NAME char(64) not null primary key,"+
    "SHORT_NAME char(8),"+
    "ARTISTSEP int(10) unsigned,"+
    "COLOR char(7),"+
    "REMARKS char(255))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create AUTOFILLS table
  //
  sql=QString("create table if not exists AUTOFILLS (")+
    "ID int not null primary key auto_increment,"+
    "SERVICE char(10),"+
    "CART_NUMBER int unsigned,"+
    "index SERVICE_IDX (SERVICE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create HOSTVARS table
  //
  sql=QString("create table if not exists HOSTVARS (")+
    "ID int not null primary key auto_increment,"+
    "STATION_NAME char(64) not null,"+
    "NAME char(32) not null,"+
    "VARVALUE char(255),"+
    "REMARK char(255),"+
    "index NAME_IDX (STATION_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SERVICE_PERMS Table
  //
  sql=QString("create table if not exists SERVICE_PERMS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64),"+
    "SERVICE_NAME char(10),"+
    "index STATION_IDX (STATION_NAME),"+
    "index SERVICE_IDX (SERVICE_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORTS Table
  //
  sql=QString("create table if not exists REPORTS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "NAME char(64) not null unique,"+
    "DESCRIPTION char(64),"+
    "EXPORT_FILTER int,"+
    "EXPORT_PATH char(255),"+
    "POST_EXPORT_CMD text,"+
    "WIN_EXPORT_PATH char(255),"+
    "WIN_POST_EXPORT_CMD text,"+
    "EXPORT_TFC enum('N','Y') default 'N',"+
    "FORCE_TFC enum('N','Y') default 'N',"+
    "EXPORT_MUS enum('N','Y') default 'N',"+
    "FORCE_MUS enum('N','Y') default 'N',"+
    "EXPORT_GEN enum('N','Y') default 'N',"+
    "STATION_ID char(16),"+
    "CART_DIGITS int unsigned default 6,"+
    "USE_LEADING_ZEROS enum('N','Y') default 'N',"+
    "LINES_PER_PAGE int default 66,"+
    "SERVICE_NAME char(64),"+
    "STATION_TYPE int default 0,"+
    "STATION_FORMAT char(64),"+
    "FILTER_ONAIR_FLAG enum('N','Y') default 'N',"+
    "FILTER_GROUPS enum('N','Y') default 'N',"+
    "START_TIME time,"+
    "END_TIME time,"+
    "index IDX_NAME (NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORT_SERVICES Table
  //
  sql=QString("create table if not exists REPORT_SERVICES (")+
    "ID int unsigned auto_increment not null primary key,"+
    "REPORT_NAME char(64) not null,"+
    "SERVICE_NAME char(10),"+
    "index IDX_REPORT_NAME (REPORT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORT_STATIONS Table
  //
  sql=QString("create table if not exists REPORT_STATIONS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "REPORT_NAME char(64) not null,"+
    "STATION_NAME char(64),"+
    "index IDX_REPORT_NAME (REPORT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORT_GROUPS Table
  //
  sql=QString("create table if not exists REPORT_GROUPS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "REPORT_NAME char(64) not null,"+
    "GROUP_NAME char(10),"+
    "index IDX_REPORT_NAME (REPORT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create CLOCK_PERMS Table
  //
  sql=QString("create table if not exists CLOCK_PERMS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "CLOCK_NAME char(64),"+
    "SERVICE_NAME char(10),"+
    "index STATION_IDX (CLOCK_NAME),"+
    "index SERVICE_IDX (SERVICE_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create EVENT_PERMS Table
  //
  sql=QString("create table if not exists EVENT_PERMS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "EVENT_NAME char(64),"+
    "SERVICE_NAME char(10),"+
    "index STATION_IDX (EVENT_NAME),"+
    "index SERVICE_IDX (SERVICE_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create USER_PERMS table
  //
  sql=QString("create table if not exists USER_PERMS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "USER_NAME char(255),"+
    "GROUP_NAME char(10),"+
    "index USER_IDX (USER_NAME),"+
    "index GROUP_IDX (GROUP_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create VGUEST_RESOURCES Table
  //
  sql=QString("create table if not exists VGUEST_RESOURCES (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "MATRIX_NUM int not null,"+
    "VGUEST_TYPE int not null,"+
    "NUMBER int not null,"+
    "ENGINE_NUM int default -1,"+
    "DEVICE_NUM int default -1,"+
    "SURFACE_NUM int default 0,"+
    "RELAY_NUM int default -1,"+
    "BUSS_NUM int default -1,"+
    "index STATION_MATRIX_IDX (STATION_NAME,MATRIX_NUM,VGUEST_TYPE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDLOGEDIT table
  //
  sql=QString("create table if not exists RDLOGEDIT (")+
    "ID int unsigned primary key auto_increment,"+
    "STATION char(64) not null,"+
    "INPUT_CARD int default -1,"+
    "INPUT_PORT int default 0,"+
    "OUTPUT_CARD int default -1,"+
    "OUTPUT_PORT int default 0,"+
    "FORMAT int unsigned default 0,"+
    "LAYER int unsigned default 0,"+
    "BITRATE int unsigned default 0,"+
    "ENABLE_SECOND_START enum('N','Y') default 'Y',"+
    "DEFAULT_CHANNELS int unsigned default 2,"+
    "MAXLENGTH int default 0,"+
    "TAIL_PREROLL int unsigned default 2000,"+
    "START_CART int unsigned default 0,"+
    "END_CART int unsigned default 0,"+
    "REC_START_CART int unsigned default 0,"+
    "REC_END_CART int unsigned default 0,"+
    "TRIM_THRESHOLD int default -3000,"+
    "RIPPER_LEVEL int default -1300,"+
    "DEFAULT_TRANS_TYPE int default 0,"+
    "index STATION_IDX (STATION))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDCATCH table
  //
  sql=QString("create table if not exists RDCATCH (")+
    "ID int unsigned primary key auto_increment,"+
    "STATION char(64) not null,"+
    "ERROR_RML char(255),"+
    "index STATION_IDX (STATION))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SCHED_CODES Table
  //
  sql=QString("create table if not exists SCHED_CODES")+
    "(CODE varchar(10) not null primary key,"+
    "DESCRIPTION varchar(255))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create DROPBOXES Table
  //
  sql=QString("create table if not exists DROPBOXES ")+
    "(ID int auto_increment not null primary key,"+
    "STATION_NAME char(64),"+
    "GROUP_NAME char(10),"+
    "PATH char(255),"+
    "NORMALIZATION_LEVEL int default 1,"+
    "AUTOTRIM_LEVEL int default 1,"+
    "SINGLE_CART enum('N','Y') default 'N',"+
    "TO_CART int unsigned default 0,"+
    "FORCE_TO_MONO enum('N','Y') default 'N',"+
    "SEGUE_LEVEL int(11) default 1,"+
    "SEGUE_LENGTH int(11) default 0,"+
    "USE_CARTCHUNK_ID enum('N','Y') default 'N',"+
    "TITLE_FROM_CARTCHUNK_ID enum('N','Y') default 'N',"+
    "DELETE_CUTS enum('N','Y') default 'N',"+
    "DELETE_SOURCE enum('N','Y') default 'Y',"+
    "METADATA_PATTERN char(64),"+
    "STARTDATE_OFFSET int default 0,"+
    "ENDDATE_OFFSET int default 0,"+
    "FIX_BROKEN_FORMATS enum('N','Y') default 'N',"+
    "LOG_PATH char(255),"+
    "IMPORT_CREATE_DATES enum('N','Y') default 'N',"+
    "CREATE_STARTDATE_OFFSET int default 0,"+
    "CREATE_ENDDATE_OFFSET int default 0,"+
    "SET_USER_DEFINED char(255),"+
    "index STATION_NAME_IDX (STATION_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDPANEL Table
  //
  sql=QString("create table if not exists RDPANEL (")+
    "ID int not null primary key auto_increment,"+
    "STATION char(40) not null,"+
    "STATION_PANELS int default 3,"+
    "USER_PANELS int default 3,"+
    "CLEAR_FILTER enum('N','Y') default 'N',"+
    "FLASH_PANEL enum('N','Y') default 'N',"+
    "PANEL_PAUSE_ENABLED enum('N','Y') default 'N',"+
    "BUTTON_LABEL_TEMPLATE char(32) default \"%t\","+
    "DEFAULT_SERVICE char(10),"+
    "SKIN_PATH char(255) default \""+
    RDEscapeString(RD_DEFAULT_RDPANEL_SKIN)+"\","+
    "index STATION_IDX (STATION,INSTANCE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create FEEDS Table
  //
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
    "BASE_PREAMBLE char(255),"+
    "PURGE_URL char(255),"+
    "PURGE_USERNAME char(64),"+
    "PURGE_PASSWORD char(64),"+
    "HEADER_XML text,"+
    "CHANNEL_XML text,"+
    "ITEM_XML text,"+
    "CAST_ORDER enum('N','Y') default 'N',"+
    "MAX_SHELF_LIFE int,"+
    "LAST_BUILD_DATETIME datetime,"+
    "ORIGIN_DATETIME datetime,"+
    "ENABLE_AUTOPOST enum('N','Y') default 'N',"+
    "KEEP_METADATA enum('N','Y') default 'Y',"+
    "UPLOAD_FORMAT int default 2,"+
    "UPLOAD_CHANNELS int default 2,"+
    "UPLOAD_SAMPRATE int default 44100,"+
    "UPLOAD_BITRATE int default 32000,"+
    "UPLOAD_QUALITY int default 0,"+
    "UPLOAD_EXTENSION char(16) default \"mp3\","+
    "NORMALIZE_LEVEL int default -100,"+
    "REDIRECT_PATH char(255),"+
    "MEDIA_LINK_MODE int default 0,"+
    "index KEY_NAME_IDX(KEY_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create PODCASTS Table
  //
  sql=QString("create table if not exists PODCASTS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "FEED_ID int unsigned not null,"+
    "STATUS int unsigned default 1,"+
    "ITEM_TITLE char(255),"+
    "ITEM_DESCRIPTION text,"+
    "ITEM_CATEGORY char(64),"+
    "ITEM_LINK char(255),"+
    "ITEM_COMMENTS char(255),"+
    "ITEM_AUTHOR char(255),"+
    "ITEM_SOURCE_TEXT char(64),"+
    "ITEM_SOURCE_URL char(255),"+
    "AUDIO_FILENAME char(255),"+
    "AUDIO_LENGTH int unsigned,"+
    "AUDIO_TIME int unsigned,"+
    "SHELF_LIFE int,"+
    "ORIGIN_DATETIME datetime,"+
    "EFFECTIVE_DATETIME datetime,"+
    "index FEED_ID_IDX(FEED_ID,ORIGIN_DATETIME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create AUX_METADATA Table
  //
  sql=QString("create table if not exists AUX_METADATA (")+
    "ID int unsigned auto_increment not null primary key,"+
    "FEED_ID int unsigned,"+
    "VAR_NAME char(16),"+
    "CAPTION char(64),"+
    "index FEED_ID_IDX(FEED_ID))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create FEED_PERMS table
  //
  sql=QString("create table if not exists FEED_PERMS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "USER_NAME char(255),"+
    "KEY_NAME char(8),"+
    "index USER_IDX (USER_NAME),"+
    "index KEYNAME_IDX (KEY_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create WEB_CONNECTIONS Table
  //
  sql=QString("create table if not exists WEB_CONNECTIONS (")+
    "SESSION_ID int unsigned not null primary key,"+
    "LOGIN_NAME char(255),"+
    "IP_ADDRESS char(16),"+
    "TIME_STAMP datetime)"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SWITCHER_NODES Table
  //
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }
  // Ensure that dynamic format IDs start after 100
  sql=QString("insert into ENCODERS set ID=100,NAME=\"dummy\"");
  if(!RunQuery(sql)) {
    return false;
  }
  sql=QString("delete from ENCODERS where ID=100");
  if(!RunQuery(sql)) {
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create GPOS Table
  //
  sql=QString("create table if not exists GPOS (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "MATRIX int not null,"+
    "NUMBER int not null,"+
    "MACRO_CART int default 0,"+
    "OFF_MACRO_CART int default 0,"+
    "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // DROPBOX_PATHS Table
  //
  sql=QString("create table if not exists DROPBOX_PATHS (")+
    "ID int auto_increment not null primary key,"+
    "DROPBOX_ID int not null,"+
    "FILE_PATH char(255) not null,"+
    "FILE_DATETIME datetime,"+
    "index FILE_PATH_IDX (DROPBOX_ID,FILE_PATH))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create NOWNEXT_PLUGINS Table
  //
  sql=QString("create table if not exists NOWNEXT_PLUGINS (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "LOG_MACHINE int unsigned not null default 0,"+
    "PLUGIN_PATH char(255),"+
    "PLUGIN_ARG char(255),"+
    "index STATION_IDX (STATION_NAME,LOG_MACHINE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SYSTEM Table
  //
  sql=QString("create table if not exists SYSTEM (")+
    "ID int auto_increment not null primary key,"+
    QString().sprintf("SAMPLE_RATE int unsigned default %d,",
		      RD_DEFAULT_SAMPLE_RATE)+
    "DUP_CART_TITLES enum('N','Y') not null default 'Y',"+
    "FIX_DUP_CART_TITLES enum('N','Y') not null default 'Y',"+
    QString().sprintf("MAX_POST_LENGTH int unsigned default %u,",
		      RD_DEFAULT_MAX_POST_LENGTH)+
    "ISCI_XREFERENCE_PATH char(255),"+
    "TEMP_CART_GROUP char(10),"+
    "SHOW_USER_LIST enum('N','Y') not null default 'Y',"+
    "NOTIFICATION_ADDRESS char(15) default \""+RD_NOTIFICATION_ADDRESS+"\")"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create IMPORT_TEMPLATES table
  //
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }
  UpdateImportFormats();

  //
  // Create REPLICATORS Table
  //
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
    "NORMALIZATION_LEVEL int default 0,"+
    "index TYPE_ID_IDX (TYPE_ID))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPLICATOR_MAP Table
  //
  sql=QString("create table if not exists REPLICATOR_MAP (")+
    "ID int unsigned not null auto_increment primary key,"+
    "REPLICATOR_NAME char(32) not null,"+
    "GROUP_NAME char(10) not null,"+
    "index REPLICATOR_NAME_IDX(REPLICATOR_NAME),"+
    "index GROUP_NAME_IDX(GROUP_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPL_CART_STATE Table
  //
  sql=QString("create table if not exists REPL_CART_STATE (")+
    "ID int unsigned not null auto_increment primary key,"+
    "REPLICATOR_NAME char(32) not null,"+
    "CART_NUMBER int unsigned not null,"+
    "POSTED_FILENAME char(255),"+
    "ITEM_DATETIME datetime not null,"+
    "REPOST enum('N','Y') default 'N',"+
    "unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CART_NUMBER,POSTED_FILENAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
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
    "unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CUT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create ISCI_XREFERENCE Table
  //
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
    "index CART_NUMBER_IDX(CART_NUMBER),"+
    "index TYPE_IDX(TYPE,LATEST_DATE),"+
    "index LATEST_DATE_IDX(LATEST_DATE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDHOTKEYS table
  //
  sql=QString("create table if not exists RDHOTKEYS (")+
    "ID int unsigned not null auto_increment primary key,"+
    "STATION_NAME         char(64),"+
    "MODULE_NAME          char(64),"+
    "KEY_ID               int,"+
    "KEY_VALUE            char(64),"+
    "KEY_LABEL            char(64))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create JACK_CLIENTS Table
  //
  sql=QString("create table if not exists JACK_CLIENTS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "DESCRIPTION char(64),"+
    "COMMAND_LINE text not null,"+
    "index IDX_STATION_NAME (STATION_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create CARTSLOTS Table
  //
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
    "HOOK_MODE int default 0,"+
    "DEFAULT_HOOK_MODE int not null default -1,"+
    "SERVICE_NAME char(10),"+
    "CARD int not null default 0,"+
    "INPUT_PORT int not null default 0,"+
    "OUTPUT_PORT int not null default 0,"+
    "index STATION_NAME_IDX(STATION_NAME,SLOT_NUMBER))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create LIVEWIRE_GPIO_SLOTS table
  //
  sql=QString("create table if not exists LIVEWIRE_GPIO_SLOTS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "MATRIX int not null,"+
    "SLOT int not null,"+
    "IP_ADDRESS char(15),"+
    "SOURCE_NUMBER int,"+
    "index STATION_NAME_IDX(STATION_NAME,MATRIX))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create RDAIRPLAY_CHANNELS table
  //
  sql=QString("create table if not exists RDAIRPLAY_CHANNELS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "INSTANCE int unsigned not null,"+
    "CARD int not null default 0,"+
    "PORT int not null default 0,"+
    "START_RML char(255),"+
    "STOP_RML char(255),"+
    "GPIO_TYPE int unsigned default 0,"+
    "START_GPI_MATRIX int not null default -1,"+
    "START_GPI_LINE int not null default -1,"+
    "START_GPO_MATRIX int not null default -1,"+
    "START_GPO_LINE int not null default -1,"+
    "STOP_GPI_MATRIX int not null default -1,"+
    "STOP_GPI_LINE int not null default -1,"+
    "STOP_GPO_MATRIX int not null default -1,"+
    "STOP_GPO_LINE int not null default -1,"+
    "index STATION_NAME_IDX(STATION_NAME,INSTANCE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create RDPANEL_CHANNELS table
  //
  sql=QString("create table if not exists RDPANEL_CHANNELS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "INSTANCE int unsigned not null,"+
    "CARD int not null default 0,"+
    "PORT int not null default 0,"+
    "START_RML char(255),"+
    "STOP_RML char(255),"+
    "GPIO_TYPE int unsigned default 0,"+
    "START_GPI_MATRIX int not null default -1,"+
    "START_GPI_LINE int not null default -1,"+
    "START_GPO_MATRIX int not null default -1,"+
    "START_GPO_LINE int not null default -1,"+
    "STOP_GPI_MATRIX int not null default -1,"+
    "STOP_GPI_LINE int not null default -1,"+
    "STOP_GPO_MATRIX int not null default -1,"+
    "STOP_GPO_LINE int not null default -1,"+
    "index STATION_NAME_IDX(STATION_NAME,INSTANCE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create LOG_MODES table
  //
  sql=QString("create table if not exists LOG_MODES (")+
    "ID int unsigned auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "MACHINE int unsigned not null,"+
    "START_MODE int not null default 0,"+
    "OP_MODE int not null default 2,"+
    "index STATION_NAME_IDX(STATION_NAME,MACHINE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create DROPBOX_SCHED_CODES table
  //
  sql=QString("create table if not exists DROPBOX_SCHED_CODES(")+
    "ID int auto_increment not null primary key,"+
    "DROPBOX_ID int not null,"+
    "SCHED_CODE char(11) not null,"
    "index DROPBOX_ID_IDX(DROPBOX_ID),"+
    "index SCHED_CODE_IDX(SCHED_CODE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create GPIO_EVENTS table
  //
  sql=QString("create table if not exists GPIO_EVENTS(")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "MATRIX int not null,"+
    "NUMBER int not null,"+
    "TYPE int not null,"+
    "EDGE int not null,"+
    "EVENT_DATETIME datetime not null,"+
    "index STATION_NAME_IDX(STATION_NAME,MATRIX,TYPE,EVENT_DATETIME,EDGE))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create CUT_EVENTS table
  //
  sql=QString("create table if not exists CUT_EVENTS(")+
    "ID int auto_increment not null primary key,"+
    "CUT_NAME char(12) not null,"+
    "NUMBER int not null,"+
    "POINT int not null,"+
    "index CUT_NAME_IDX(CUT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create DECK_EVENTS table
  //
  sql=QString("create table if not exists DECK_EVENTS(")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "CHANNEL int unsigned not null,"+
    "NUMBER int not null,"+
    "CART_NUMBER int unsigned not null default 0,"+
    "index STATION_NAME_IDX(STATION_NAME,CHANNEL))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create SERVICE_CLOCKS table
  //
  sql=QString("create table if not exists SERVICE_CLOCKS(")+
    "ID int auto_increment not null primary key,"+
    "SERVICE_NAME char(10) not null,"+
    "HOUR int not null,"+
    "CLOCK_NAME char(64) default null,"+
    "index SERVICE_NAME_IDX(SERVICE_NAME,HOUR),"+
    "index CLOCK_NAME_IDX(CLOCK_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create WEBAPI_AUTH table
  //
  sql=QString("create table if not exists WEBAPI_AUTHS(")+
    "TICKET char(41) not null primary key,"+
    "LOGIN_NAME char(255) not null,"+
    "IPV4_ADDRESS char(16) not null,"+
    "EXPIRATION_DATETIME datetime not null,"+
    "index TICKET_IDX(TICKET,IPV4_ADDRESS,EXPIRATION_DATETIME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create USER_SERVICE_PERMS table
  //
  sql=QString("create table if not exists USER_SERVICE_PERMS (")+
    "ID int auto_increment not null primary key,"+
    "USER_NAME char(255) not null,"+
    "SERVICE_NAME char(10) not null,"+
    "index USER_NAME_IDX(USER_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create LOG_MACHINES table
  //
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
    config->createTablePostfix();
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create AUDIO_CARDS table
  //
  sql=QString("create table if not exists AUDIO_CARDS (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "CARD_NUMBER int not null,"+
    "DRIVER int not null default 0,"+
    "NAME char(64),"+
    "INPUTS int not null default -1,"+
    "OUTPUTS int not null default -1,"+
    "CLOCK_SOURCE int not null default 0,"+
    "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER))";
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create AUDIO_INPUTS table
  //
  sql=QString("create table if not exists AUDIO_INPUTS (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "CARD_NUMBER int not null,"+
    "PORT_NUMBER int not null,"+
    "LEVEL int not null default 0,"+
    "TYPE int not null default 0,"
    "MODE int not null default 0,"+
    "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER,PORT_NUMBER))";
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create AUDIO_OUTPUTS table
  //
  sql=QString("create table if not exists AUDIO_OUTPUTS (")+
    "ID int auto_increment not null primary key,"+
    "STATION_NAME char(64) not null,"+
    "CARD_NUMBER int not null,"+
    "PORT_NUMBER int not null,"+
    "LEVEL int not null default 0,"+
    "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER,PORT_NUMBER))";
  if(!RunQuery(sql)) {
     return false;
  }

  return true;
}


bool InitDb(QString name,QString pwd,QString station_name,RDConfig *config)
{
  QString sql;
  QString err_msg;

  //
  // Create Default Admin Account
  //
  sql=QString("insert into USERS set ")+
    "LOGIN_NAME=\""+RDEscapeString(RDA_LOGIN_NAME)+"\","+
    "PASSWORD=\""+RDEscapeString(RDA_PASSWORD)+"\","+
    "FULL_NAME=\""+RDEscapeString(RDA_FULLNAME)+"\","+
    "DESCRIPTION=\""+RDEscapeString(RDA_DESCRIPTION)+"\","+
    "ADMIN_USERS_PRIV=\"Y\","+
    "ADMIN_CONFIG_PRIV=\"Y\"";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default User Account
  //
  sql=QString("insert into USERS set ")+
    "LOGIN_NAME=\""+RDEscapeString(RD_USER_LOGIN_NAME)+"\","+
    "PASSWORD=\""+RDEscapeString(RD_USER_PASSWORD)+"\","+
    "FULL_NAME=\""+RDEscapeString(RD_USER_FULL_NAME)+"\","+
    "DESCRIPTION=\""+RDEscapeString(RD_USER_DESCRIPTION)+"\","+
    "CREATE_CARTS_PRIV=\"Y\","+
    "DELETE_CARTS_PRIV=\"Y\","+
    "MODIFY_CARTS_PRIV=\"Y\","+
    "EDIT_AUDIO_PRIV=\"Y\","+
    "ASSIGN_CART_PRIV=\"Y\","+
    "CREATE_LOG_PRIV=\"Y\","+
    "DELETE_LOG_PRIV=\"Y\","+
    "DELETE_REC_PRIV=\"Y\","+
    "PLAYOUT_LOG_PRIV=\"Y\","+
    "ARRANGE_LOG_PRIV=\"Y\","+
    "MODIFY_TEMPLATE_PRIV=\"Y\","+
    "ADDTO_LOG_PRIV=\"Y\","+
    "REMOVEFROM_LOG_PRIV=\"Y\","+
    "CONFIG_PANELS_PRIV=\"Y\","+
    "VOICETRACK_LOG_PRIV=\"Y\","+
    "EDIT_CATCHES_PRIV=\"Y\","+
    "ADD_PODCAST_PRIV=\"Y\","+
    "EDIT_PODCAST_PRIV=\"Y\","+
    "DELETE_PODCAST_PRIV=\"Y\"";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default System-wide Settings Record
  //
  sql="insert into SYSTEM set ID=1";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default Workstation
  //
  sql=QString("insert into STATIONS set ")+
    "NAME=\""+RDEscapeString(station_name)+"\","+
    "DESCRIPTION=\""+RDEscapeString(RD_STATION_DESCRIPTION)+"\","+
    "USER_NAME=\""+RDEscapeString(RD_USER_LOGIN_NAME)+"\","+
    "DEFAULT_NAME=\""+RDEscapeString(RD_USER_LOGIN_NAME)+"\"";
  struct hostent *hostent=gethostbyname((const char *)station_name);
  if(hostent!=NULL) {
    sql+=QString().sprintf(",IPV4_ADDRESS=\"%d.%d.%d.%d\"",
			   0xFF&hostent->h_addr[0],0xFF&hostent->h_addr[1],
			   0xFF&hostent->h_addr[2],0xFF&hostent->h_addr[3]);
  }
  if(!RunQuery(sql)) {
    return false;
  }
  for(unsigned i=0;i<10;i++) {
    sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
      "STATION_NAME=\""+RDEscapeString(station_name)+"\","+
      QString().sprintf("INSTANCE=%u",i);
    if(!RunQuery(sql)) {
      return false;
    }
  }
  for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
      "STATION_NAME=\""+RDEscapeString(station_name)+"\","+
      QString().sprintf("INSTANCE=%u",i+RD_RDVAIRPLAY_LOG_BASE);
    if(!RunQuery(sql)) {
      return false;
    }
  }
  for(unsigned i=0;i<10;i++) {
    sql=QString("insert into RDPANEL_CHANNELS set ")+
      "STATION_NAME=\""+RDEscapeString(station_name)+"\","+
      QString().sprintf("INSTANCE=%u",i);
    if(!RunQuery(sql)) {
      return false;
    }
  }
  for(unsigned i=0;i<3;i++) {
    sql=QString("insert into LOG_MODES set ")+
      "STATION_NAME=\""+RDEscapeString(station_name)+"\","+
      QString().sprintf("MACHINE=%u",i);
    if(!RunQuery(sql)) {
      return false;
    }
  }
  for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
    sql=QString("insert into LOG_MODES set ")+
      "STATION_NAME=\""+RDEscapeString(station_name)+"\","+
      QString().sprintf("MACHINE=%u",i+RD_RDVAIRPLAY_LOG_BASE);
    if(!RunQuery(sql)) {
      return false;
    }
  }
  for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i++) {
    for(unsigned j=0;j<MAX_DECKS;j++) {
      sql=QString("insert into DECK_EVENTS set ")+
	"STATION_NAME=\""+RDEscapeString(station_name)+"\","+
	QString().sprintf("CHANNEL=%u,",j+129)+
	QString().sprintf("NUMBER=%u",i+1);
      if(!RunQuery(sql)) {
	return false;
      }
    }
  }

  //
  // Create Test Tone Cart
  //
  sql=QString("insert into CART set ")+
    QString().sprintf("TYPE=%u,",RDCart::Audio)+
    "NUMBER=999999,"+
    "GROUP_NAME=\"TEST\","+
    "TITLE=\"Test Tone\","+
    "ARTIST=\"Rivendell Radio Automation\","+
    "CUT_QUANTITY=1,"+
    "FORCED_LENGTH=10000,"+
    "METADATA_DATETIME=now()";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default Audio Cut
  //
  sql=QString("insert into CUTS set ")+
    "CUT_NAME=\""+RDCut::cutName(999999,1)+"\","+
    "CART_NUMBER=999999,"+
    "DESCRIPTION=\"1 kHz at Reference Level [-16 dBFS]\","+
    "OUTCUE=\"[tone]\","+
    "CODING_FORMAT=0,"+
    "BIT_RATE=0,"+
    "CHANNELS=2,"+
    "LENGTH=10000,"+
    "START_POINT=0,"+
    "END_POINT=10000,"+
    "ORIGIN_DATETIME=now(),"+
    "ORIGIN_NAME=\"+RDGen\"";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Clipboard Entry
  //
  sql=QString("insert into CLIPBOARD set ")+
    "CUT_NAME=\"clip\","+
    "CART_NUMBER=0,"+
    "DESCRIPTION=\""+RDEscapeString("Default Clipboard")+"\"";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default Service
  //
  if(!RDSvc::create(RD_SERVICE_NAME,&err_msg,"",config)) {
    return false;
  }
  sql=QString("update SERVICES set ")+
    "DESCRIPTION=\""+RDEscapeString(RD_SERVICE_DESCRIPTION)+"\" where "+
    "NAME=\""+RDEscapeString(RD_SERVICE_NAME)+"\"";
  if(!RunQuery(sql)) {
    return false;
  }
  sql=QString("insert into USER_SERVICE_PERMS set ")+
    "USER_NAME=\""+RDEscapeString(RD_USER_LOGIN_NAME)+"\","+
    "SERVICE_NAME=\""+RDEscapeString(RD_SERVICE_NAME)+"\"";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default Groups
  //
  struct Group
  {
    const char *group;
    const char *description;
    int start;
    int end;
    bool now_next;
    bool rpt_traffic;
    bool rpt_music;
    bool macro;
  };

  static const struct Group group[] = {
    {"TEMP","Temporary Carts",0,0,false,false,false,false},
    {"TEST","Test Carts",0,0,false,false,false,false},
    {"TRAFFIC","Traffic Carts",1,10000,false,true,false,false},
    {"MUSIC","Music Carts",10001,40000,true,false,true,false},
    {"BEDS","Music Bed Carts",40001,45000,false,false,false,false},
    {"STINGS","Short stingers",45001,50000,false,false,false,false},
    {"MACROS","Macro Carts",50001,60000,false,false,false,true},
    {"LEGAL","Legal IDs",60001,60100,false,false,false,false},
    {"IDENTS","General IDs",60101,60500,false,false,false,false},
    {NULL,NULL,0,0,false,false,false,false},
  };

  for (const struct Group *g = group; g->group != NULL; g++){
    // Create the group
    sql=QString("insert into GROUPS set ")+
      "NAME=\""+RDEscapeString(g->group)+"\","+
      "DESCRIPTION=\""+RDEscapeString(g->description)+"\","+
      QString().sprintf("DEFAULT_CART_TYPE=%d,",g->macro?2:1)+
      QString().sprintf("DEFAULT_LOW_CART=%d,",g->start)+
      QString().sprintf("DEFAULT_HIGH_CART=%d,",g->end)+
      "REPORT_TFC=\""+RDYesNo(g->rpt_traffic)+"\","+
      "REPORT_MUS=\""+RDYesNo(g->rpt_music)+"\","+
      "ENABLE_NOW_NEXT=\""+RDYesNo(g->now_next)+"\"";
    if(!RunQuery(sql)) {
      return false;
    }
    // Add it to the user permissions table for the default user
    sql=QString("insert into USER_PERMS set ")+
      "USER_NAME=\""+RDEscapeString(RD_USER_LOGIN_NAME)+"\","+
      "GROUP_NAME=\""+RDEscapeString(g->group)+"\"";
    if(!RunQuery(sql)) {
      return false;
    }
    // Add it to the audio permsmissions table
    sql=QString("insert into AUDIO_PERMS set ")+
      "GROUP_NAME=\""+RDEscapeString(g->group)+"\","+
      "SERVICE_NAME=\""+RDEscapeString(RD_SERVICE_NAME)+"\"";
    if(!RunQuery(sql)) {
      return false;
    }
  }

  //
  // Create Sample Log
  //
  sql=RDCreateLogTableSql("SAMPLE_LOG",config);
  if(!RunQuery(sql)) {
    return false;
  }
  sql=QString("insert into LOGS set ")+
    "NAME=\"SAMPLE\","+
    "SERVICE=\""+RDEscapeString(RD_SERVICE_NAME)+"\","+
    "DESCRIPTION=\"Sample Log\","+
    "ORIGIN_USER=\""+RDEscapeString(RD_USER_LOGIN_NAME)+"\","+
    "ORIGIN_DATETIME=now(),"
    "MODIFIED_DATETIME=now()";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Generate Hotkey Definitions
  //
  if(!UpdateRDAirplayHotkeys(station_name)) {
    return false;
  }

  //
  // Generate Version Number
  //
  sql=QString().sprintf("insert into VERSION (DB) values (%d)",
			RD_VERSION_DATABASE);
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Generate Audio
  //
  QString filename=
    QString().sprintf("%s/999999_001.%s",
		      RDConfiguration()->audioRoot().ascii(),
		      RDConfiguration()->audioExtension().ascii());

  QString cmd=QString().sprintf("rdgen -t 10 -l 16 %s",
				(const char *)filename);
  system((const char *)cmd);
  if(getuid()==0) {
    chown(filename,RDConfiguration()->uid(),RDConfiguration()->gid());
    chmod (filename,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  }

  return true;
}


//
// The following two methods are called by the 185=>186 schema update
//
void ConvertTimeField(const QString &table,const QString &field)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  //
  // Create temporary field
  //
  sql=QString().sprintf("alter table %s add column %s_TEMP int after %s",
			(const char *)table,
			(const char *)field,
			(const char *)field);
  q=new RDSqlQuery(sql,false);
  delete q;

  //
  // Copy data to temporary field
  //
  sql=QString().sprintf("select ID,%s from %s",
			(const char *)field,
			(const char *)table);
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    if(!q->value(1).isNull()) {
      sql=QString().sprintf("update %s set %s_TEMP=%d where ID=%d",
			    (const char *)table,
			    (const char *)field,
			    QTime().msecsTo(q->value(1).toTime()),
			    q->value(0).toInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
  }
  delete q;

  //
  // Convert primary field
  //
  sql=QString().sprintf("alter table %s modify column %s int",
			(const char *)table,
			(const char *)field);
  q=new RDSqlQuery(sql,false);
  delete q;

  //
  // Copy data back to primary field
  //
  sql=QString().sprintf("select ID,%s_TEMP from %s",
			(const char *)field,
			(const char *)table);
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    if(!q->value(1).isNull()) {
      sql=QString().sprintf("update %s set %s=%d where ID=%d",
			    (const char *)table,
			    (const char *)field,
			    q->value(1).toInt(),
			    q->value(0).toInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
  }
  delete q;

  //
  // Delete Temporary field
  //
  sql=QString().sprintf("alter table %s drop column %s_TEMP",
			(const char *)table,
			(const char *)field);
  q=new RDSqlQuery(sql,false);
  delete q;
}


void UpdateLogTable(const QString &table)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Drop POST_TIME
  //
  sql=QString().sprintf("alter table %s drop column POST_TIME",
			(const char *)table);
  q=new RDSqlQuery(sql,false);
  delete q;

  //
  // Convert Fields
  //
  ConvertTimeField(table,"START_TIME");
  ConvertTimeField(table,"LINK_START_TIME");
}


//
// Main Schema Update Routine
//
int UpdateDb(int ver,RDConfig *config)
{
  QString cmd;
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDSqlQuery *q2;
  RDSqlQuery *q3;
  RDCart *cart;
  unsigned dev;
  QString tablename;
  bool length_update_required=false;

  //
  // Create backup
  //
  if(!admin_skip_backup) {
    if(admin_backup_filename.isEmpty()) {
      bool home_found = false;
      admin_backup_filename = RDGetHomeDir(&home_found);
      if (!home_found) {
        admin_backup_filename = RDTempDirectory::basePath();
      }
      admin_backup_filename+=
	QString().sprintf("/rdbackup-%s-%d.sql.gz",
       				       (const char *)QDate::currentDate().
			  toString("yyyyMMdd"),ver);
    }
    cmd=QString().sprintf("mysqldump -h %s -u %s -p%s %s | gzip -q -c - > %s",
			  (const char *)config->mysqlHostname(),
			  (const char *)config->mysqlUsername(),
			  (const char *)config->mysqlPassword(),
			  (const char *)config->mysqlDbname(),
			  (const char *)admin_backup_filename);
    if(system(cmd)!=0) {
      return UPDATEDB_BACKUP_FAILED;
    }
  }

  // **** Start of version updates ****

  if(ver<3) {
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create Default RDAirPlay Configuration
    //
    sql=QString("insert into RDAIRPLAY (STATION,INSTANCE) ")+
      "values (\"DEFAULT\",0)";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<4) {
    if(!RunQuery(
	 "alter table RDAIRPLAY modify ID int not null auto_increment")) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<5) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add SOURCE int not null after COUNT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<6) {
    // 
    // Update RDAIRPLAY Structure
    //
    q=new RDSqlQuery("alter table RDAIRPLAY add column CARD int default -1",
		     false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column PORT0 int default -1",
		     false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column PORT1 int default -1",
		     false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column PORT2 int default -1",
		     false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM0 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM1 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM2 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM3 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM4 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM5 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM6 int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column STREAM7 int default -1",
		 false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column AUTO_CARD0",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column AUTO_CARD1",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column AUTO_STREAM0",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column AUTO_STREAM1",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column AUTO_PORT0",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column AUTO_PORT1",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column PANEL_CARD0",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column PANEL_CARD1",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column PANEL_STREAM0",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column PANEL_STREAM1",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column PANEL_PORT0",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY drop column PANEL_PORT1",false);
    delete q;
  }

  if(ver<7) {
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column SEGUE_LENGTH int default 0",
		 false);
    delete q;
  }

  if(ver<8) {
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column PORT3 int default -1",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column OP_MODE int default 0",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column START_MODE int default 0",
		 false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column PIE_COUNT_LENGTH int default 15000",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column PIE_COUNT_ENDPOINT int default 0",false);
    delete q;
  }

  if(ver<9) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column PORT4 int default -1",
		     false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column PORT5 int default -1",
		     false);
    delete q;
  }

  if(ver<10) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      RunQuery(QString().
	      sprintf("alter table %s_LOG add TYPE int default 0 after COUNT",
		     (const char *)q->value(0).toString()));
      RunQuery(QString().
	      sprintf("alter table %s_LOG add COMMENT char(255)",
		     (const char *)q->value(0).toString()));
      RunQuery(QString().
	      sprintf("alter table %s_LOG add LABEL char(10)",
		     (const char *)q->value(0).toString()));
      RunQuery(QString().
	    sprintf("alter table %s_LOG add POST_TIME time default '24:00:00'",
		     (const char *)q->value(0).toString()));
      RunQuery(QString().
	      sprintf("alter table %s_LOG add index LABEL_IDX (LABEL)",
		     (const char *)q->value(0).toString()));
    }
    delete q;
  }


  if(ver<11) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<12) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column STATION_PANELS int default 3",false);
    delete q;

    q=new RDSqlQuery("alter table RDAIRPLAY add column USER_PANELS int default 3",false);
    delete q;
  }

  if(ver<13) {
    sql="drop table PANELS";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }
    
  if(ver<14) {
    sql="drop table PANELS";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<15) {
    q=new RDSqlQuery("alter table PANELS drop column DEFAULT_COLOR",false);
    delete q;

    q=new RDSqlQuery("alter table PANELS add column DEFAULT_COLOR char(7)",
		     false);
    delete q;
  }

  if(ver<16) {
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<17) {
    q=new RDSqlQuery("alter table INPUTS add column CHANNEL_MODE int",false);
    delete q;
  }

  if(ver<18) {
    q=new RDSqlQuery("alter table STATIONS add column IPV4_ADDRESS char(15)",
		     false);
    delete q;
  }

  if(ver<19) {
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
      config->createTablePostfix();
  q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<20) {
    q=new RDSqlQuery("alter table CART add column MACROS text",false);
    delete q;
  }

  if(ver<21) {
    q=new 
      RDSqlQuery("alter table RECORDINGS add column MACRO_CART int default -1",
		 false);
    delete q;
    q=new 
      RDSqlQuery("drop table EVENTS",false);
    delete q;
  }

  if(ver<22) {
    q=new RDSqlQuery("alter table DECKS drop column SWITCH_TYPE",false);
    delete q;
    q=new RDSqlQuery("alter table DECKS drop column TTY_ID",false);
    delete q;
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
  }

  if(ver<23) {
    q=new RDSqlQuery("alter table RECORDINGS drop column SOURCE_NAME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column SWITCH_INPUT int default -1",false);
    delete q;
    q=new RDSqlQuery("drop table SOURCES",false);
    delete q;
  }

  if(ver<24) {
    q=new RDSqlQuery("alter table RECORDINGS add column TYPE int default 0 after STATION_NAME",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS add column SWITCH_OUTPUT int default -1",false);
    delete q;
    q=new RDSqlQuery("update RECORDINGS set TYPE=1 where MACRO_CART!=-1",false);
    delete q;
  }

  if(ver<25) {
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
  }

  if(ver<26) {
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
  }

  //
  // Create GPIS Table
  //
  if(ver<27) {
    sql=QString("create table if not exists GPIS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "MACRO_CART int default -1,"+
      "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<28) {
    sql="alter table CUTS alter column ORIGIN_DATETIME set default NULL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CUTS alter column START_DATETIME set default NULL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CUTS alter column END_DATETIME set default NULL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CUTS alter column START_DAYPART set default NULL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CUTS alter column END_DAYPART set default NULL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update CUTS set ORIGIN_DATETIME=NULL where ORIGIN_DATETIME=\"0000-00-00 00:00:00\"";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update CUTS set START_DATETIME=NULL where START_DATETIME=\"0000-00-00 00:00:00\"";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update CUTS set END_DATETIME=NULL where END_DATETIME=\"0000-00-00 00:00:00\"";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"drop index START_TIME_IDX";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"alter column START_TIME set default NULL";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"modify column START_TIME int";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"alter column POST_TIME set default NULL";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("update `")+q->value(0).toString()+"_LOG` "+
	"set START_TIME=NULL where START_TIME=\"00:00:00\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("update `")+q->value(0).toString()+"_LOG` "+
	"set POST_TIME=NULL where POST_TIME=\"00:00:00\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<29) {
    q=new RDSqlQuery("alter table RECORDINGS add column EXIT_CODE int default 0",false);
    delete q;
  }

  if(ver<30) {
    q=new RDSqlQuery("alter table RECORDINGS add column ONE_SHOT enum('N','Y') default 'N'",false);
    delete q;
  }

  if(ver<31) {
    q=new RDSqlQuery("alter table STATIONS add column TIME_OFFSET int default 0",false);
    delete q;
  }

  if(ver<32) {
    q=new RDSqlQuery("alter table GROUPS add column DEFAULT_LOW_CART int unsigned default 0",false);
    q=new RDSqlQuery("alter table GROUPS add column DEFAULT_HIGH_CART int unsigned default 0",false);
    delete q;
  }

  if(ver<33) {
    q=new RDSqlQuery("alter table CUTS add column SUN enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column MON enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column TUE enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column WED enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column THU enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column FRI enum('N','Y') default 'Y' after END_DAYPART",false);
    q=new RDSqlQuery("alter table CUTS add column SAT enum('N','Y') default 'Y' after END_DAYPART",false);
  }

  if(ver<34) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG "+
	"add GRACE_TIME int default 0 after START_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<35) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column TRANS_LENGTH int default 0 after SEGUE_LENGTH",false);
  }

  if(ver<36) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add POST_POINT enum('N','Y') default 'N' after TIME_TYPE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<37) {
    q=new RDSqlQuery("alter table LOGS add column TYPE int not null default 0 after NAME",false);
    delete q;
  }

  if(ver<38) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<39) {   // Transpose RDLogLine::Stop and RDLogLine::Segue
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update `")+q->value(0).toString()+"_LOG` set "+
	"TRANS_TYPE=100 where TRANS_TYPE=1";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("update `")+q->value(0).toString()+"_LOG` set "+
	"TRANS_TYPE=1 where TRANS_TYPE=2";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("update `")+q->value(0).toString()+"_LOG` set "+
	"TRANS_TYPE=2 where TRANS_TYPE=100";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<40) {
    sql=QString("create table if not exists CLOCKS (")+
      "NAME char(64) not null primary key,"+
      "SHORT_NAME char(8),"+
      "COLOR char(7))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<41) {
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
  }

  if(ver<42) {
    q=new RDSqlQuery("alter table SERVICES add column CLOCK0 char(64) after DESCRIPTION",false);
    delete q;
    for(int i=1;i<168;i++) {
      sql=QString("alter table SERVICES add column ")+
	QString().sprintf("CLOCK%d char(64) after CLOCK%d",i,i-1);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
  }

  if(ver<43) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column SHOW_AUX_1 enum('N','Y') default 'Y' after USER_PANELS",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column SHOW_AUX_2 enum('N','Y') default 'Y' after SHOW_AUX_1",false);
    delete q;
  }

  if(ver<44) {
    q=new RDSqlQuery("alter table CUTS add column LOCAL_COUNTER int unsigned default 0 after PLAY_COUNTER",false);
    delete q;
  }

  if(ver<45) {
    q=new RDSqlQuery("alter table CUTS add column EVERGREEN enum('N','Y') default 'N' after CART_NUMBER",false);
    delete q;
  }

  if(ver<46) {
    q=new RDSqlQuery("alter table CART add column LENGTH_DEVIATION int unsigned default 0 after FORCED_LENGTH",false);
    delete q;
    q=new RDSqlQuery("select NUMBER from CART where TYPE=1",false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->calculateAverageLength(&dev);
      cart->setLengthDeviation(dev);
      delete cart;
    }
    delete q;
  }

  if(ver<47) {
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
  }

  if(ver<48) {
    q=new RDSqlQuery("alter table SERVICES add column TFC_LENGTH_OFFSET int after TFC_START_LENGTH",false);
    delete q;

    q=new RDSqlQuery("alter table SERVICES add column TFC_LENGTH_LENGTH int after TFC_LENGTH_OFFSET",false);
    delete q;

    q=new RDSqlQuery("alter table SERVICES add column MUS_LENGTH_OFFSET int after MUS_START_LENGTH",false);
    delete q;

    q=new RDSqlQuery("alter table SERVICES add column MUS_LENGTH_LENGTH int after MUS_LENGTH_OFFSET",false);
    delete q;
  }

  if(ver<49) {
    sql=QString("create table if not exists AUTOFILLS (")+
      "ID int not null primary key auto_increment,"+
      "SERVICE char(10),"+
      "CART_NUMBER int unsigned,"+
      "index SERVICE_IDX (SERVICE))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<50) {
    sql=QString("alter table SERVICES ")+
      "add column CHAIN_LOG enum('N','Y') default 'N'"+
      "after NAME_TEMPLATE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<51) {
    sql=QString("alter table USERS ")+
      "modify column PASSWORD char(32)"+
      "after DESCRIPTION";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<52) {
    sql=QString("create table if not exists HOSTVARS (")+
      "ID int not null primary key auto_increment,"+
      "STATION_NAME char(64) not null,"+
      "NAME char(32) not null,"+
      "VARVALUE char(255),"+
      "REMARK char(255),"+
      "index NAME_IDX (STATION_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<53) {
    q=new RDSqlQuery("alter table STATIONS add column BACKUP_DIR char(255)",false);
    delete q;
    q=new RDSqlQuery("alter table STATIONS add column BACKUP_LIFE int default 0",false);
    delete q;
  }
  
  if(ver<54) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column CLEAR_FILTER enum(\'N\',\'Y\') default \'N\'",false);
    delete q;
    q=new RDSqlQuery("alter table RDAIRPLAY add column BAR_ACTION int unsigned default 0",false);
    delete q;
  }

  if(ver<55) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column FLASH_PANEL enum(\'N\',\'Y\') default \'N\'",false);
    delete q;
  }

  if(ver<56) {
    q=new RDSqlQuery("alter table STATIONS add column HEARTBEAT_CART int unsigned default 0",false);
    delete q;
    q=new RDSqlQuery("alter table STATIONS add column HEARTBEAT_INTERVAL int unsigned default 0",false);
    delete q;
  }

  if(ver<57) {
    sql=QString("create table if not exists SERVICE_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64),"+
      "SERVICE_NAME char(10),"+
      "index STATION_IDX (STATION_NAME),"+
      "index SERVICE_IDX (SERVICE_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
    q=new RDSqlQuery("select NAME from STATIONS",false);
    while(q->next()) {
      q1=new RDSqlQuery("select NAME from SERVICES",false);
      while(q1->next()) {
	sql=QString("insert into SERVICE_PERMS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	q2=new RDSqlQuery(sql,false);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }

  if(ver<58) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column EXT_START_TIME time";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column EXT_LENGTH int";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table '")+q->value(0).toString()+"_LOG` "+
	"add column EXT_DATA char(32)";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column EXT_EVENT_ID char(8)";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<59) {
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
  }

  //
  // Version 60 and 61 code removed, as per-log reconciliation data is no 
  // longer used.   FFG 11/08/2005
  //

  if(ver<62) {
    q=new RDSqlQuery("alter table GROUPS add column REPORT_TFC enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add column REPORT_MUS enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add index IDX_REPORT_TFC (REPORT_TFC)",false);
    delete q;
    q=new RDSqlQuery("alter table GROUPS add index IDX_REPORT_MUS (REPORT_MUS)",false);
    delete q;
  }

  if(ver<63) {
    sql=QString("create table if not exists REPORTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "NAME char(64) not null unique,"+
      "DESCRIPTION char(64),"+
      "EXPORT_FILTER int,"+
      "EXPORT_PATH char(255),"+
      "WIN_EXPORT_PATH char(255),"+
      "index IDX_NAME (NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=QString("create table if not exists REPORT_SERVICES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "REPORT_NAME char(64) not null,"+
      "SERVICE_NAME char(10),"+
      "index IDX_REPORT_NAME (REPORT_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=QString("create table if not exists REPORT_STATIONS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "REPORT_NAME char(64) not null,"+
      "STATION_NAME char(64),"+
      "index IDX_REPORT_NAME (REPORT_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<64) {
    q=new RDSqlQuery("alter table REPORTS add column EXPORT_TFC enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table REPORTS add column EXPORT_MUS enum('N','Y') default 'N'",false);
    delete q;
    q=new RDSqlQuery("alter table REPORTS add column EXPORT_GEN enum('N','Y') default 'N'",false);
    delete q;
  }

  if(ver<65) {
    q=new RDSqlQuery("alter table REPORTS add column STATION_ID char(16)",false);
    delete q;
  }

  if(ver<66) {
    q=new 
      RDSqlQuery("alter table RDAIRPLAY alter column OP_MODE set default 2",false);
    delete q;
  }

  if(ver<67) {
    q=new 
      RDSqlQuery("alter table RDAIRPLAY add column PAUSE_ENABLED enum('N','Y') default 'N'",false);
    delete q;
  }

  if(ver<68) {
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
  }

  if(ver<69) {
    q=new RDSqlQuery("alter table MATRICES add column PORT_TYPE int default 0 after TYPE",false);
    delete q;
    q=new RDSqlQuery("alter table MATRICES add column IP_ADDRESS char(16) after PORT",false);
    delete q;
    q=new RDSqlQuery("alter table MATRICES add column IP_PORT int after IP_ADDRESS",false);
    delete q;
  }

  if(ver<70) {
    q=new RDSqlQuery("select NAME from LOGS",false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_REC`"+
	"add column PLAY_SOURCE int default 0 after EVENT_TYPE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_REC` "+
	"add column CUT_NUMBER int default 0 after CART_NUMBER";
      delete q1;
    }
    delete q;
  }

  if(ver<71) {
    q=new RDSqlQuery("alter table RECORDINGS add column END_LINE int default -1 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column END_MATRIX int default -1 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column END_LENGTH int default 0 after START_TIME",false);
    delete q;
    q=new RDSqlQuery("alter table RECORDINGS add column END_TIME time after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column END_TYPE int unsigned default 2 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column START_OFFSET int default 0 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column START_LINE int default -1 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column START_MATRIX int default -1 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column START_LENGTH int default 0 after START_TIME",false);
    delete q;
    q=new 
      RDSqlQuery("alter table RECORDINGS add column START_TYPE int default 0 after DESCRIPTION",false);
    delete q;
  }

  if(ver<72) {
    q=new RDSqlQuery("alter table GROUPS add column DEFAULT_CART_TYPE int unsigned default 1 after DESCRIPTION",false);
    delete q;
  }

  if(ver<73) {
    q=new RDSqlQuery("alter table RDAIRPLAY add column DEFAULT_TRANS_TYPE int unsigned default 0 after CLEAR_FILTER",false);
    delete q;
  }

  if(ver<74) {
    //
    // Create CLOCK_PERMS Table
    //
    sql=QString("create table if not exists CLOCK_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "CLOCK_NAME char(64),"+
      "SERVICE_NAME char(10),"+
      "index CLOCK_IDX (CLOCK_NAME),"+
      "index SERVICE_IDX (SERVICE_NAME))"+
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
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
	q2=new RDSqlQuery(sql,false);
	delete q2;
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
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
	q2=new RDSqlQuery(sql,false);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }

  if(ver<75) {
    q=new RDSqlQuery("alter table MATRICES add column CARD int default -1 after PORT_TYPE",false);
    delete q;
  }

  if(ver<76) {
    q=new RDSqlQuery("alter table DECKS add column MON_PORT_NUMBER int default -1 after PORT_NUMBER",false);
    delete q;
  }

  if(ver<77) {
    //
    // Create USER_PERMS table
    //
    sql=QString("create table if not exists USER_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "USER_NAME char(8),"+
      "GROUP_NAME char(10),"+
      "index USER_IDX (USER_NAME),"+
      "index GROUP_IDX (GROUP_NAME))"+
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
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
	q2=new RDSqlQuery(sql,false);
	delete q2;
      }
    }
    delete q1;
    delete q;

  }

  if(ver<78) {
    sql="alter table USERS add column MODIFY_TEMPLATE_PRIV enum('N','Y') not null default 'N' after ARRANGE_LOG_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update USERS set MODIFY_TEMPLATE_PRIV=\"Y\" where CREATE_LOG_PRIV=\"Y\"";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<79) {
    sql="alter table GROUPS add column ENFORCE_CART_RANGE enum('N','Y') default 'N' after DEFAULT_HIGH_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<80) {
    sql="alter table SERVICES add column TFC_ANNC_TYPE_OFFSET int after TFC_EVENT_ID_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_ANNC_TYPE_LENGTH int after TFC_ANNC_TYPE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_ANNC_TYPE_OFFSET int after MUS_EVENT_ID_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_ANNC_TYPE_LENGTH int after MUS_ANNC_TYPE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPORTS add column FORCE_TFC enum('N','Y') default 'N' after EXPORT_TFC";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table REPORTS add column FORCE_MUS enum('N','Y') default 'N' after EXPORT_MUS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table GROUPS alter column REPORT_TFC set default 'Y'";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table GROUPS alter column REPORT_MUS set default 'Y'";
    q=new RDSqlQuery(sql,false);
    delete q;
   }

  if(ver<81) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_REC` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_REC` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<82) {
    sql="alter table INPUTS add column ENGINE_NUM int default -1 after CHANNEL_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table INPUTS add column DEVICE_NUM int default -1 after ENGINE_NUM";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table OUTPUTS add column ENGINE_NUM int default -1 after NAME";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table OUTPUTS add column DEVICE_NUM int default -1 after ENGINE_NUM";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column USERNAME char(32) after IP_PORT";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table MATRICES add column PASSWORD char(32) after USERNAME";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column FADERS int default 0 after GPOS";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table MATRICES add column DISPLAYS int default 0 after FADERS";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<83) {
    sql="alter table RECORDINGS add column URL char(255) after ONE_SHOT";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table RECORDINGS add column URL_USERNAME char(64) after URL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table RECORDINGS add column URL_PASSWORD char(64) after URL_USERNAME";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<84) {
    sql=
      "alter table STATIONS add column STATION_SCANNED enum('N','Y') default 'N'";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column HPI_VERSION char(16)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column JACK_VERSION char(16)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column ALSA_VERSION char(16)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=
      "alter table STATIONS add column HAVE_OGGENC enum('N','Y') default 'N'";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_OGG123 enum('N','Y') default 'N'";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_FLAC enum('N','Y') default 'N'";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_LAME enum('N','Y') default 'N'";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_MPG321 enum('N','Y') default 'N'";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD0_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD0_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD0_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD0_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD1_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD1_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD1_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD1_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD2_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD2_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD2_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD2_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD3_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD3_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD3_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD3_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD4_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD4_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD4_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD4_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD5_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD5_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD5_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD5_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD6_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD6_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD6_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD6_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CARD7_DRIVER int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD7_NAME char(64)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD7_INPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table STATIONS add column CARD7_OUTPUTS int default -1";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<85) {
    sql="alter table RECORDINGS add column NORMALIZE_LEVEL int default -1300 after TRIM_THRESHOLD";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RECORDINGS add column QUALITY int default 0 after BITRATE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<86) {
    sql="alter table RECORDINGS alter column END_TYPE set default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<87) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_REC` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<88) {
    sql="alter table RECORDINGS add column ALLOW_MULT_RECS enum('N','Y') default 'N' after END_GPI";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RECORDINGS add column MAX_GPI_REC_LENGTH int unsigned default 3600000 after ALLOW_MULT_RECS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select ")+
      "ID,"+
      "START_TIME,"+
      "END_TIME,"+
      "END_LENGTH "+
      "from RECORDINGS where "+
      QString().sprintf("END_TYPE=%d",RDRecording::GpiEnd);
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString().
	sprintf("update RECORDINGS set MAX_GPI_REC_LENGTH=%u where ID=%u",
		QTime().msecsTo(q->value(2).toTime())+q->value(3).toUInt()-
		QTime().msecsTo(q->value(1).toTime()),q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<89) {
    sql="alter table CART add column AVERAGE_LENGTH int unsigned after FORCED_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add column ASYNCRONOUS enum('N','Y') default 'N' after PRESERVE_PITCH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Audio);
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      sql=QString("update CART set ")+
	QString().sprintf("AVERAGE_LENGTH=%u where ",
			  cart->calculateAverageLength())+
	QString().sprintf("NUMBER=%u",cart->number());
      q1=new RDSqlQuery(sql,false);
      delete q1;
      delete cart;
    }
    delete q;

    RDMacroEvent *macro_event;
    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Macro);
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      macro_event=new RDMacroEvent();
      macro_event->load(q->value(0).toUInt());
      sql=QString("update CART set ")+
	QString().sprintf("AVERAGE_LENGTH=%u,",macro_event->length())+
	QString().sprintf("FORCED_LENGTH=%u ",macro_event->length())+
	QString().sprintf("where NUMBER=%u",q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
      delete macro_event;
    }
    delete q;
  }

  if(ver<90) {
    sql="alter table REPORTS add column CART_DIGITS int unsigned default 6 after STATION_ID";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPORTS add column USE_LEADING_ZEROS enum('N','Y') default 'N' after CART_DIGITS";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<91) {
    sql="alter table DECKS add column DEFAULT_MONITOR_ON enum('N','Y') default 'N' after MON_PORT_NUMBER";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<92) {
    sql="alter table EVENTS add column AUTOFILL_SLOP int default -1 after USE_AUTOFILL";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<93) {
    sql="alter table LOGS add column IMPORT_DATE date after END_DATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column FADEUP_POINT int default -1 after END_POINT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_LOG` "+
	QString().sprintf("add column FADEUP_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEUP_POINT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column FADEDOWN_POINT int default -1 after FADEUP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_LOG` "+
	QString().sprintf("add column FADEDOWN_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEDOWN_POINT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_LOG` "+
	QString().sprintf("add column SEGUE_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after SEGUE_END_POINT",
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column FADEUP_POINT int default -1 after END_POINT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	QString().sprintf("add column FADEUP_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEUP_POINT",
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column FADEDOWN_POINT int default -1 after FADEUP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table '")+tablename+"_PRE' "+
	QString().sprintf("add column FADEDOWN_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEDOWN_POINT",
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	QString().sprintf("add column SEGUE_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after SEGUE_END_POINT",
	q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table '")+tablename+"_POST' "+
	"add column FADEUP_POINT int default -1 after END_POINT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table '")+tablename+"_POST' "+
	QString().sprintf("add column FADEUP_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEUP_POINT";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column FADEDOWN_POINT int default -1 after FADEUP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	QString().sprintf("add column FADEDOWN_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after FADEDOWN_POINT",
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	QString().sprintf("add column SEGUE_GAIN int default %d ",
			  RD_FADE_DEPTH)+
	"after SEGUE_END_POINT",
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<94) {
    sql="alter table CART add column OWNER char(64) after ASYNCRONOUS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add index OWNER_IDX (OWNER)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TRACK_GROUP char(10) after CHAIN_LOG";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column SCHEDULED_TRACKS int unsigned default 0 after IMPORT_DATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column COMPLETED_TRACKS int unsigned default 0 after SCHEDULED_TRACKS";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<95) {
    sql="alter table USERS add column VOICETRACK_LOG_PRIV enum('N','Y') not null default 'N' after ADDTO_LOG_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<96) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from STATIONS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("insert into RDLOGEDIT set ")+
	"STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<97) {
    sql="alter table LOGS add column LOG_EXISTS enum('N','Y') default 'Y' after NAME";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table LOGS add index NAME_IDX (NAME,LOG_EXISTS)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table USERS add column DELETE_REC_PRIV enum('N','Y') default 'N' after DELETE_LOG_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update USERS set DELETE_REC_PRIV=\"Y\" where DELETE_LOG_PRIV=\"Y\"";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<98) {
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
	config->createTablePostfix();
      q1=new RDSqlQuery(sql,false);
      delete q1;

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
	  q3=new RDSqlQuery(sql,false);
	  delete q3;
	}
	delete q2;
      }
      delete q1;
    }
    delete q;

    sql="alter table RDAIRPLAY add column DEFAULT_SERVICE char(10) after PAUSE_ENABLED";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<99) {
    sql="alter table USERS add column CONFIG_PANELS_PRIV enum('N','Y') default 'N' after REMOVEFROM_LOG_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update USERS set CONFIG_PANELS_PRIV=\"Y\"";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<100) {
    sql="alter table CUTS add column ISRC char(12) after OUTCUE";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="select NUMBER,ISRC from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update CUTS set ")+
	"ISRC=\""+RDEscapeString(q->value(1).toString())+"\" where "+
	QString().sprintf("CART_NUMBER=%u",q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<101) {
    for(int i=0;i<RD_MAX_CARDS;i++) {
      sql=QString("alter table AUDIO_PORTS ")+
	QString().sprintf("add column INPUT_%d_MODE int default 0 ",i)+
	QString().sprintf("after INPUT_%d_TYPE",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
  }

  if(ver<102) {
    sql="alter table CART add column PUBLISHER char(64) after AGENCY";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CART add index PUBLISHER_IDX (PUBLISHER)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add column COMPOSER char(64) after PUBLISHER";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CART add index COMPOSER_IDX (COMPOSER)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<104) {
    sql="alter table CART add column USAGE_CODE int default 0\
         after USER_DEFINED";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPORTS add column LINES_PER_PAGE int default 66\
         after USE_LEADING_ZEROS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPORTS add column STATION_TYPE int default 0\
         after LINES_PER_PAGE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPORTS add column STATION_FORMAT char(64)\
         after STATION_TYPE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column PUBLISHER char(64) after ARTIST";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column COMPOSER char(64) after PUBLISHER";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column ISRC char(12) after PUBLISHER";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column USAGE_CODE int default 0 after ISRC";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<105) {
    for(int i=0;i<6;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column START_RML%d char(255) after PORT%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
      sql=QString().sprintf("alter table RDAIRPLAY add column STOP_RML%d char(255) after START_RML%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
  }

  if(ver<106) {
    for(int i=0;i<3;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column LOG_RML%d char(255) after UDP_STRING%d",i,i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
  }

  if(ver<107) {
    sql="alter table RDLOGEDIT add column REC_START_CART int unsigned default 0 after END_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table RDLOGEDIT add column REC_END_CART int unsigned default 0 after REC_START_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<108) {
    sql="alter table CART add column AVERAGE_SEGUE_LENGTH int unsigned after LENGTH_DEVIATION";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="select NUMBER from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }

  if(ver<109) {
    sql="alter table EVENTS add column NESTED_EVENT char(64) after COLOR";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<110) {
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
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<111) {
    sql="alter table CART add column VALIDITY int unsigned default 2 after PLAY_ORDER";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table CUTS add column VALIDITY int unsigned default 2 after LOCAL_COUNTER";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="select NUMBER from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }

  if(ver<112) {
    sql="alter table RDLOGEDIT add column TRIM_THRESHOLD int default -3000 after REC_END_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table RDLOGEDIT add column RIPPER_LEVEL int default -1300 after TRIM_THRESHOLD";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="select STATION,TRIM_THRESHOLD,RIPPER_LEVEL from RDLIBRARY";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update RDLOGEDIT set ")+
	QString().sprintf("TRIM_THRESHOLD=%d,",q->value(1).toInt())+
	QString().sprintf("RIPPER_LEVEL=%d where ",q->value(2).toInt())+
	"STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="alter table RDLIBRARY modify RIPPER_LEVEL int default -1300";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDLOGEDIT modify DEFAULT_CHANNELS int unsigned default 1";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<113) {
    sql="alter table VGUEST_RESOURCES modify SURFACE_NUM int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_BREAK_STRING char(64) after MUS_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_TRACK_STRING char(64) after MUS_BREAK_STRING";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<114) {
    sql="alter table LOGS add column MUSIC_LINKS int default 0 after COMPLETED_TRACKS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column MUSIC_LINKED enum('N','Y') default 'N' after MUSIC_LINKS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column TRAFFIC_LINKS int default 0 after MUSIC_LINKED";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column TRAFFIC_LINKED enum('N','Y') default 'N' after TRAFFIC_LINKS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_EVENT_NAME char(64) after POST_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_START_TIME time after LINK_EVENT_NAME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString().sprintf("alter table `")+tablename+"_LOG` "+
	"add column LINK_LENGTH int default 0 after LINK_START_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_ID int default -1 after LINK_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_EVENT_NAME char(64) after POST_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_START_TIME time after LINK_EVENT_NAME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_LENGTH int default 0 after LINK_START_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_ID int default -1 after LINK_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_EVENT_NAME char(64) after POST_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_START_TIME time after LINK_EVENT_NAME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_LENGTH int default 0 after LINK_START_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_ID int default -1 after LINK_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<115) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"modify column LABEL char(64)";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"modify column LABEL char(64)";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"modify column LABEL char(64)";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="alter table SERVICES add column TRACK_GROUP char(10) after CHAIN_LOG";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<116) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column LINK_EMBEDDED enum('N','Y') default 'N' after LINK_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_EMBEDDED enum('N','Y') default 'N' after LINK_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_EMBEDDED enum('N','Y') default 'N' after LINK_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<117) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column ORIGIN_USER char(8) after POST_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column ORIGIN_DATETIME datetime after ORIGIN_USER";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_LOG` drop column ORIGIN_NAME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column ORIGIN_USER char(8) after POST_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column ORIGIN_DATETIME datetime after ORIGIN_USER";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column ORIGIN_USER char(8) after POST_TIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column ORIGIN_DATETIME datetime after ORIGIN_USER";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_PRE` "+
	"drop column ORIGIN_NAME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"drop column ORIGIN_NAME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<118) {
    sql="alter table SERVICES add column TFC_LABEL_CART char(32) after TFC_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_TRACK_CART char(32) after TFC_LABEL_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_LABEL_CART char(32) after MUS_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_TRACK_CART char(32) after MUS_LABEL_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_TITLE_OFFSET int after TFC_CART_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_TITLE_LENGTH int after TFC_TITLE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_TITLE_OFFSET int after MUS_CART_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_TITLE_LENGTH int after MUS_TITLE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<119) {
    sql="alter table SERVICES add column TFC_HOURS_OFFSET int after TFC_START_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_HOURS_LENGTH int after TFC_HOURS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TFC_MINUTES_OFFSET int after TFC_HOURS_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_MINUTES_LENGTH int after TFC_MINUTES_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TFC_SECONDS_OFFSET int after TFC_MINUTES_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_SECONDS_LENGTH int after TFC_SECONDS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="alter table SERVICES add column MUS_HOURS_OFFSET int after MUS_START_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_HOURS_LENGTH int after MUS_HOURS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_MINUTES_OFFSET int after MUS_HOURS_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_MINUTES_LENGTH int after MUS_MINUTES_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_SECONDS_OFFSET int after MUS_MINUTES_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_SECONDS_LENGTH int after MUS_SECONDS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<120) {
    sql="alter table GROUPS add column COLOR char(7) default \"#000000\" after ENABLE_NOW_NEXT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDLOGEDIT add column DEFAULT_TRANS_TYPE int default 0 after RIPPER_LEVEL";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<121) {
    sql="alter table LOGS add column LINK_DATETIME datetime not null after ORIGIN_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column NEXT_ID int default 0 after TRAFFIC_LINKED";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME,ORIGIN_DATETIME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update LOGS set ")+
	"LINK_DATETIME=\""+q->value(1).toDateTime().
	toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("select ID from `")+q->value(0).toString()+"_LOG` "+
	"order by ID";
      q1=new RDSqlQuery(sql,false);
      if(q1->last()) {
	sql=QString("update LOGS set ")+
	  QString().sprintf("NEXT_ID=%d where ",q1->value(0).toInt()+1)+
	  "NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
	q2=new RDSqlQuery(sql,false);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }

  if(ver<122) {
    sql="alter table LOGS add column MODIFIED_DATETIME datetime not null after LINK_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column AUTO_REFRESH enum('N','Y') default 'N' after MODIFIED_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME,LINK_DATETIME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update LOGS set ")+
	"MODIFIED_DATETIME=\""+RDEscapeString(q->value(1).toDateTime().
			      toString("yyyy-MM-dd hh:mm:ss"))+"\" where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }

    sql="alter table SERVICES add column AUTO_REFRESH enum('N','Y') default 'N' after TRACK_GROUP";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<123) {
    sql="alter table REPORTS add column FILTER_ONAIR_FLAG enum('N','Y') default 'N' after STATION_FORMAT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_SRT` "+
	"add column ONAIR_FLAG enum('N','Y') default 'N' after START_SOURCE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<124) {
    sql="alter table SERVICES add column TFC_LEN_HOURS_OFFSET int after TFC_SECONDS_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_LEN_HOURS_LENGTH int after TFC_LEN_HOURS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TFC_LEN_MINUTES_OFFSET int after TFC_LEN_HOURS_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_LEN_MINUTES_LENGTH int after TFC_LEN_MINUTES_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TFC_LEN_SECONDS_OFFSET int after TFC_LEN_MINUTES_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column TFC_LEN_SECONDS_LENGTH int after TFC_LEN_SECONDS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="alter table SERVICES add column MUS_LEN_HOURS_OFFSET int after MUS_SECONDS_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_LEN_HOURS_LENGTH int after MUS_LEN_HOURS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_LEN_MINUTES_OFFSET int after MUS_LEN_HOURS_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_LEN_MINUTES_LENGTH int after MUS_LEN_MINUTES_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_LEN_SECONDS_OFFSET int after MUS_LEN_MINUTES_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table SERVICES add column MUS_LEN_SECONDS_LENGTH int after MUS_LEN_SECONDS_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<125) {
    sql="alter table REPORTS add column SERVICE_NAME char(64) after LINES_PER_PAGE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_SRT` "+
	"add column ALBUM char(255) after COMPOSER";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+q->value(0).toString()+"_SRT` "+
	"add column LABEL char(64) after ALBUM";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<126) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column LINK_START_SLOP int default 0 after LINK_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column LINK_END_SLOP int default 0 after LINK_START_SLOP";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<127) {
    sql="alter table RDAIRPLAY add column PANEL_PAUSE_ENABLED enum('N','Y') default 'N' after FLASH_PANEL";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<128) {
    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_START_SLOP int default 0 after LINK_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column LINK_END_SLOP int default 0 after LINK_START_SLOP";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_START_SLOP int default 0 after LINK_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+tablename+"_POST` "+
	"add column LINK_END_SLOP int default 0 after LINK_START_SLOP";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<129) {
    sql="alter table RDAIRPLAY add column EXIT_CODE int default 0 after LOG_RML2";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column EXIT_PASSWORD char(41) default \"\" after EXIT_CODE";
    q=new RDSqlQuery(sql,false);
    delete q;


    sql="alter table RDAIRPLAY add column LOG0_START_MODE int default 0 after EXIT_PASSWORD";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_AUTO_RESTART enum('N','Y') default 'N' after LOG0_START_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_LOG_NAME char(64) after LOG0_AUTO_RESTART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_CURRENT_LOG char(64) after LOG0_LOG_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_RUNNING enum('N','Y') default 'N' after LOG0_CURRENT_LOG";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_LOG_ID int default -1 after LOG0_RUNNING";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_LOG_LINE int default -1 after LOG0_LOG_ID";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_START_MODE int default 0 after LOG0_LOG_LINE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_AUTO_RESTART enum('N','Y') default 'N' after LOG1_START_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_LOG_NAME char(64) after LOG1_AUTO_RESTART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_CURRENT_LOG char(64) after LOG1_LOG_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_RUNNING enum('N','Y') default 'N' after LOG1_CURRENT_LOG";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_LOG_ID int default -1 after LOG1_RUNNING";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_LOG_LINE int default -1 after LOG1_LOG_ID";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_START_MODE int default 0 after LOG1_LOG_LINE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_AUTO_RESTART enum('N','Y') default 'N' after LOG2_START_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_LOG_NAME char(64) after LOG2_AUTO_RESTART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_CURRENT_LOG char(64) after LOG2_LOG_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_RUNNING enum('N','Y') default 'N' after LOG2_CURRENT_LOG";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_LOG_ID int default -1 after LOG2_RUNNING";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_LOG_LINE int default -1 after LOG2_LOG_ID";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<130) {
    sql=QString("create table if not exists RDCATCH (")+
      "ID int unsigned primary key auto_increment,"+
      "STATION char(64) not null,"+
      "ERROR_RML char(255),"+
      "index STATION_IDX (STATION))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if (ver<131) {
    sql="alter table `EVENTS` add column `SCHED_GROUP` varchar(10)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table `EVENTS` add column `TITLE_SEP` int(10) unsigned";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table `EVENTS` add column `HAVE_CODE` varchar(10)";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table `EVENTS` add column `HOR_SEP` int(10) unsigned";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table `EVENTS` add column `HOR_DIST` int(10) unsigned";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table `CLOCKS` add column `ARTISTSEP` int(10) unsigned";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table `CART` add column `SCHED_CODES` varchar( 255 ) NULL default NULL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=QString("create table if not exists SCHED_CODES (")+
      "CODE varchar(10) not null primary key,"+
      "DESCRIPTION varchar(255))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="drop table SCHED_STACK";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString svc=q->value(0).toString();
      sql=QString("drop table `")+svc.replace(" ","_")+"_STACK`";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      }
    delete q;
    sql="select NAME from LOGS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_LOG` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
    sql="select NAME from EVENTS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+q->value(0).toString()+"_PRE` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_PRE` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+q->value(0).toString()+"_POST` "+
		  "add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if (ver<132) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<133) {
    sql="select NAME from LOGS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_LOG` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
    sql="select NAME from EVENTS";  
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_PRE` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column DUCK_UP_GAIN int default 0 after SEGUE_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<134) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<135) {
    sql="alter table STATIONS add column STARTUP_CART int unsigned default 0 after HEARTBEAT_INTERVAL";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<136) {
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  } 

  if(ver<137) {
    sql="alter table CART add column AVERAGE_HOOK_LENGTH int unsigned default 0 after AVERAGE_SEGUE_LENGTH";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    sql="select NUMBER from CART";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }
  
  if(ver<138) {
    sql=QString("alter table RDAIRPLAY ")+
      "add column BUTTON_LABEL_TEMPLATE char(32) "+
      "default \"%t\" after PANEL_PAUSE_ENABLED";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDPANEL add column BUTTON_LABEL_TEMPLATE char(32) default \"%t\" after PANEL_PAUSE_ENABLED";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<139) {
    sql="alter table RDAIRPLAY modify EXIT_PASSWORD char(41) default \"\""; 
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<140) {
    sql="alter table SERVICES add column TFC_BREAK_STRING char(64) after TFC_TRACK_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TFC_TRACK_STRING char(64) after TFC_TRACK_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<141) {
    sql="alter table DROPBOXES add column TITLE_FROM_CARTCHUNK_ID enum('N','Y') default 'N' after USE_CARTCHUNK_ID"; 
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<142) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RECORDINGS add column FEED_ID int default -1 after URL_PASSWORD";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table VERSION add column LAST_MAINT_DATETIME datetime default \"1970-01-01 00:00:00\" after DB";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<143) {
    sql=QString("create table if not exists AUX_METADATA (")+
      "ID int unsigned auto_increment not null primary key,"+
      "FEED_ID int unsigned,"+
      "VAR_NAME char(16),"+
      "CAPTION char(64),"+
      "index FEED_ID_IDX(FEED_ID))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select KEY_NAME from FEEDS"; 
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      RDCreateAuxFieldsTable(q->value(0).toString(),config);
    }
    delete q;
  }

  if(ver<144) {
    sql="alter table FEEDS add column UPLOAD_FORMAT int default 2 after ENABLE_AUTOPOST";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table FEEDS add column UPLOAD_CHANNELS int default 2 after UPLOAD_FORMAT";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table FEEDS add column UPLOAD_SAMPRATE int default 44100 after UPLOAD_CHANNELS";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table FEEDS add column UPLOAD_BITRATE int default 32000 after UPLOAD_CHANNELS";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="alter table FEEDS add column UPLOAD_QUALITY int default 0 after UPLOAD_BITRATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table FEEDS add column NORMALIZE_LEVEL int default -100 after UPLOAD_QUALITY";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table PODCASTS add column ITEM_COMMENTS char(255) after ITEM_LINK";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table PODCASTS add column ITEM_AUTHOR char(255) after ITEM_COMMENTS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table PODCASTS add column ITEM_SOURCE_TEXT char(64) after ITEM_AUTHOR";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table PODCASTS add column ITEM_SOURCE_URL char(255) after ITEM_SOURCE_TEXT";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<145) {
    sql=QString("create table if not exists FEED_PERMS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "USER_NAME char(8),"+
      "KEY_NAME char(8),"+
      "index USER_IDX (USER_NAME),"+
      "index KEYNAME_IDX (KEY_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
	q2=new RDSqlQuery(sql,false);
	delete q2;
      }
      q1->seek(-1);
    }
    delete q1;
    delete q;


    sql="alter table USERS add column ADD_PODCAST_PRIV enum('N','Y') not null default 'N' after EDIT_CATCHES_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table USERS add column EDIT_PODCAST_PRIV enum('N','Y') not null default 'N' after ADD_PODCAST_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table USERS add column DELETE_PODCAST_PRIV enum('N','Y') not null default 'N' after EDIT_PODCAST_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="update USERS set ADD_PODCAST_PRIV='Y',EDIT_PODCAST_PRIV='Y', DELETE_PODCAST_PRIV='Y' where (ADMIN_CONFIG_PRIV='N')&&(ADMIN_USERS_PRIV='N')";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<146) {
   sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EXT_CART_NAME char(32) after EXT_LENGTH";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EVENT_SOURCE int default 0 after EVENT_TYPE";
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString("alter table `")+tablename+"_POST` "+
	"add column EXT_ANNC_TYPE char(8) after EXT_EVENT_ID";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<147) {
    sql="alter table USERS add column ENABLE_WEB enum('N','Y') default 'N' after PASSWORD";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("create table if not exists WEB_CONNECTIONS (")+
      "SESSION_ID int unsigned not null primary key,"+
      "LOGIN_NAME char(8),"+
      "IP_ADDRESS char(16),"+
      "TIME_STAMP datetime)"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<148) {
    sql=QString("alter table CUTS add column ")+
      QString().sprintf("SEGUE_GAIN int default %d after SEGUE_END_POINT",
			RD_FADE_DEPTH);
    q=new RDSqlQuery(sql,false);
    delete q;
  }
  
  if(ver<148) {
    sql="alter table PODCASTS add column AUDIO_TIME int unsigned after AUDIO_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<149) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table INPUTS add column NODE_HOSTNAME char(255) after DEVICE_NUM";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table INPUTS add column NODE_TCP_PORT int after NODE_HOSTNAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table INPUTS add column NODE_SLOT int after NODE_TCP_PORT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table add index NODE_IDX(STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table OUTPUTS add column NODE_HOSTNAME char(255) after DEVICE_NUM";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table OUTPUTS add column NODE_TCP_PORT int after NODE_HOSTNAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table OUTPUTS add column NODE_SLOT int after NODE_TCP_PORT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table add index NODE_IDX(STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<150) {
    sql="alter table MATRICES add column PORT_2 int not null after PORT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column PORT_TYPE_2 int default 0 after PORT_TYPE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column IP_ADDRESS_2 char(16) after IP_ADDRESS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column IP_PORT_2 int after IP_PORT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column USERNAME_2 char(32) after USERNAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column PASSWORD_2 char(32) after PASSWORD";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<151) {
    sql="alter table FEEDS add column KEEP_METADATA enum('N','Y') default 'Y' after ENABLE_AUTOPOST";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select KEY_NAME from FEEDS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      RDCreateFeedLog(q->value(0).toString());
    }
    delete q;
  }

  if(ver<152) {
    sql="alter table STATIONS add column EDITOR_PATH char(255) default \"\"\
         after STARTUP_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDPANEL add column CARD3 int default -1 after STOP_RML2";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDPANEL add column PORT3 int default -1 after CARD3";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDPANEL add column START_RML3 char(255) after PORT3";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDPANEL add column STOP_RML3 char(255) after START_RML3";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<153) {
    sql="alter table STATIONS add column FILTER_MODE int default 0 after EDITOR_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<154) {
    sql=QString().sprintf("alter table CUTS add column SEGUE_GAIN int default %d after SEGUE_END_POINT",RD_FADE_DEPTH);
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<155) {
    sql="alter table RDLIBRARY add column ENABLE_EDITOR enum('N','Y') default 'N' after CDDB_SERVER";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<156) {
    sql="alter table MATRICES add column LAYER int default 86 after TYPE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<157) {
    sql="alter table STATIONS add column BROADCAST_SECURITY int unsigned default 0 after BACKUP_LIFE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<158) {  // Convert OVERLAP to SEGUE
    sql="select NAME from LOGS;";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("update `")+tablename+"_LOG` set TRANS_TYPE=1,SEGUE_GAIN=0 where TRANS_TYPE=3";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<159) {
    sql="alter table RDAIRPLAY add column SKIN_PATH char(255) default \"";
    sql+=RD_DEFAULT_RDAIRPLAY_SKIN;
    sql+="\" after EXIT_PASSWORD";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDPANEL add column SKIN_PATH char(255) default \"";
    sql+=RD_DEFAULT_RDPANEL_SKIN;
    sql+="\" after DEFAULT_SERVICE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<160) {
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
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    // Ensure that dynamic format IDs start after 100
    sql="insert into ENCODERS set ID=100,NAME=\"dummy\"";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    sql="delete from ENCODERS where ID=100";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create ENCODER_BITRATES Table
    //
    sql=QString("create table if not exists ENCODER_BITRATES (")+
      "ID int not null auto_increment primary key,"+
      "ENCODER_ID int not null,"+
      "BITRATES int not null,"+
      "index ENCODER_ID_IDX(ENCODER_ID))"+
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    
    //
    // Create ENCODER_CHANNELS Table
    //
    sql=QString("create table if not exists ENCODER_CHANNELS (")+
      "ID int not null auto_increment primary key,"+
      "ENCODER_ID int not null,"+
      "CHANNELS int not null,"+
      "index ENCODER_ID_IDX(ENCODER_ID))"+
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create ENCODER_SAMPLERATES Table
    //
    sql=QString("create table if not exists ENCODER_SAMPLERATES (")+
      "ID int not null auto_increment primary key,"+
      "ENCODER_ID int not null,"+
      "SAMPLERATES int not null,"+
      "index ENCODER_ID_IDX(ENCODER_ID))"+
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<161) {
    sql="alter table FEEDS add column UPLOAD_EXTENSION char(16) default \"mp3\" after UPLOAD_QUALITY";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<162) {
    sql="alter table GPIS alter column MACRO_CART set default 0";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("create table if not exists GPOS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "MACRO_CART int default 0,"+
      "index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select STATION_NAME,MATRIX,GPOS from MATRICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<q->value(2).toInt();i++) {
	sql=QString("insert into GPOS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MATRIX=%d,",q->value(1).toInt())+
	  QString().sprintf("NUMBER=%d,",i+1)+
	  "MACRO_CART=0";
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
    }
    delete q;
  }

  if(ver<163) {
    sql="alter table RECORDINGS add column EVENTDATE_OFFSET int default 0 after ENDDATE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<164) {
    sql=QString("create table if not exists NOWNEXT_PLUGINS (")+
      "ID int auto_increment not null primary key,    "+
      "STATION_NAME char(64) not null,		     "+
      "LOG_MACHINE int unsigned not null default 0,   "+
      "PLUGIN_PATH char(255),"+
      "PLUGIN_ARG char(255),"+
      "index STATION_IDX (STATION_NAME,LOG_MACHINE))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<165) {
    sql=QString("create table if not exists DROPBOX_PATHS (")+
      "ID int auto_increment not null primary key,"+
      "DROPBOX_ID int not null,"+
      "FILE_PATH char(255) not null,"+
      "FILE_DATETIME datetime,"+
      "index FILE_PATH_IDX (DROPBOX_ID,FILE_PATH))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table DROPBOXES add DELETE_SOURCE enum('N','Y') default 'Y' after DELETE_CUTS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table DROPBOXES add column STARTDATE_OFFSET int default 0 after METADATA_PATTERN";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table DROPBOXES add column ENDDATE_OFFSET int default 0 after STARTDATE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<166) {
    sql="alter table GROUPS add column CUT_SHELFLIFE int default -1 after DEFAULT_HIGH_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table GROUPS add column DEFAULT_TITLE char(255) default \"Imported from %f.%e\" after CUT_SHELFLIFE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column SYSTEM_MAINT enum('N','Y') default 'Y' after FILTER_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column ELR_SHELFLIFE int default -1 after AUTO_REFRESH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table EVENTS add column REMARKS char(255) after NESTED_EVENT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CLOCKS add column REMARKS char(255) after COLOR";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<167) {
    sql="alter table RDAIRPLAY add column LOG0_NOW_CART int unsigned default 0 after LOG0_LOG_LINE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_NEXT_CART int unsigned default 0 after LOG0_NOW_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_NOW_CART int unsigned default 0 after LOG1_LOG_LINE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_NEXT_CART int unsigned default 0 after LOG1_NOW_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_NOW_CART int unsigned default 0 after LOG2_LOG_LINE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_NEXT_CART int unsigned default 0 after LOG2_NOW_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<168) {
    sql="alter table GPIS add column OFF_MACRO_CART int default 0 after MACRO_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table GPOS add column OFF_MACRO_CART int default 0 after MACRO_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<169) {
    sql="alter table SERVICES add column DEFAULT_LOG_SHELFLIFE int default -1 after AUTO_REFRESH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS add column PURGE_DATE date after END_DATE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<170) {
    sql="alter table USERS modify column LOGIN_NAME char(255) not null";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table USERS modify column FULL_NAME char(255)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table FEED_PERMS modify column USER_NAME char(255)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table LOGS modify column ORIGIN_USER char(255) not null";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().
	sprintf("alter table %s_LOG modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      q1=new RDSqlQuery(sql,false);
      delete q1;
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
      q1=new RDSqlQuery(sql,false);
      delete q1;
      sql=QString().
	sprintf("alter table %s_POST modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="alter table STATIONS modify column USER_NAME char(255)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS modify column DEFAULT_NAME char(255)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table USER_PERMS modify column USER_NAME char(255)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table WEB_CONNECTIONS modify column LOGIN_NAME char(255)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<171) {
    sql="alter table FEEDS add column CAST_ORDER enum('N','Y') default 'N' after ITEM_XML";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table PODCASTS add column EFFECTIVE_DATETIME datetime after ORIGIN_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select ID,ORIGIN_DATETIME from PODCASTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update PODCASTS set ")+
	"EFFECTIVE_DATETIME=\""+q->value(1).toDateTime().
	toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
        QString().sprintf("ID=%u",q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<172) {
    sql="alter table FEEDS add column REDIRECT_PATH char(255) after NORMALIZE_LEVEL";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<173) {
    sql="alter table FEEDS add column BASE_PREAMBLE char(255) after BASE_URL";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<174) {
    sql="alter table MATRICES add column START_CART int unsigned after PASSWORD_2";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column STOP_CART int unsigned after START_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column START_CART_2 int unsigned after STOP_CART";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table MATRICES add column STOP_CART_2 int unsigned after START_CART_2";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<175) {
    sql=QString("create table if not exists SYSTEM (")+
      "ID int auto_increment not null primary key,"+
      "DUP_CART_TITLES enum('N','Y') not null default 'Y')"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="insert into SYSTEM set DUP_CART_TITLES=\"Y\"";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<176) {
    sql="alter table RDAIRPLAY add column SHOW_COUNTERS enum('N','Y') default 'N' after SKIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table RDAIRPLAY add column AUDITION_PREROLL int default 10000 after SHOW_COUNTERS";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<177) {
    sql=QString("alter table RDPANEL ")+
      "add column SKIN_PATH char(255) default \""+RD_DEFAULT_RDPANEL_SKIN+"\" "+
      "after DEFAULT_SERVICE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table PANELS drop column SKIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<178) {
    sql="alter table PODCASTS modify column STATUS int unsigned default 1";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table SYSTEM ")+
      "add column MAX_POST_LENGTH "+
      QString().sprintf("int unsigned default %u after DUP_CART_TITLES",
			RD_DEFAULT_MAX_POST_LENGTH);
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<179) {
    sql="alter table FEEDS add column MEDIA_LINK_MODE int default 0 after REDIRECT_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<180) {
    sql="alter table RDLIBRARY add column SRC_CONVERTER int default 1 after ENABLE_EDITOR";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<181) {
    length_update_required=true;
    /*
    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Audio);
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
    */
  }

  if(ver<182) {
    sql="alter table CART add column NOTES text after MACROS";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<183) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
    UpdateImportFormats();

    sql="alter table SERVICES add column TFC_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\" after TFC_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\" after MUS_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="update SERVICES set TFC_IMPORT_TEMPLATE=\"\"";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="update SERVICES set MUS_IMPORT_TEMPLATE=\"\"";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<184) {
    //
    // Removed as redundant with update 183 [call to UpdateImportFormats()].
    //
  }  

  if(ver<185) {
    sql="alter table SERVICES add column TFC_PREIMPORT_CMD text after TFC_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column TFC_WIN_PREIMPORT_CMD text after TFC_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_PREIMPORT_CMD text after MUS_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SERVICES add column MUS_WIN_PREIMPORT_CMD text after MUS_WIN_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<186) {
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      UpdateLogTable(tablename+"_LOG");
    }
    delete q;
    
    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      UpdateLogTable(tablename+"_PRE");
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      UpdateLogTable(tablename+"_POST");
    }

    sql="select NAME from CLOCKS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      ConvertTimeField(tablename+"_CLK","START_TIME");
    }
    delete q;
  }

  if(ver<187) {
    sql="alter table CUTS add column ISCI char(32) after ISRC";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT` "+
	"add column ISCI char(32) after ISRC";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<188) {
  }

  if(ver<189) {
    sql="alter table CUTS add column UPLOAD_DATETIME datetime after LAST_PLAY_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<190) {
    sql="alter table STATIONS add column HAVE_TWOLAME enum('N','Y') default 'N' after HAVE_FLAC";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<191) {
    sql="alter table SERVICES add column PROGRAM_CODE char(255) after NAME_TEMPLATE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<192) {
    sql="alter table RECORDINGS add column EXIT_TEXT text after EXIT_CODE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<193) {
    sql="alter table STATIONS alter column IPV4_ADDRESS set default \"127.0.0.2\"";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<194) {
    sql=QString().sprintf("alter table SYSTEM ")+
      "add column SAMPLE_RATE "+
      QString().sprintf("int unsigned default %d after ID",
			RD_DEFAULT_SAMPLE_RATE);
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="select DEFAULT_SAMPRATE from RDLIBRARY";
    q=new RDSqlQuery(sql,false);
    if(q->first()) {
      sql=QString().sprintf("update SYSTEM set SAMPLE_RATE=%u",
			    q->value(0).toUInt());
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }
  if(ver<195) {
    sql="alter table RECORDINGS add column ENABLE_METADATA enum('N','Y') default 'N' after URL_PASSWORD";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<196) {
  sql="alter table DROPBOXES add column IMPORT_CREATE_DATES enum('N','Y') default 'N' after LOG_PATH";
  q=new RDSqlQuery(sql,false);
  delete q;
  sql="alter table DROPBOXES add column CREATE_STARTDATE_OFFSET int default 0 after IMPORT_CREATE_DATES";
  q=new RDSqlQuery(sql,false);
  delete q;
  sql="alter table DROPBOXES add column CREATE_ENDDATE_OFFSET int default 0 after CREATE_STARTDATE_OFFSET";
  q=new RDSqlQuery(sql,false);
  delete q;
  }

  if(ver<197) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("create table if not exists REPLICATOR_MAP (")+
      "ID int unsigned not null auto_increment primary key,"+
      "REPLICATOR_NAME char(32) not null,"+
      "GROUP_NAME char(10) not null,"+
      "index REPLICATOR_NAME_IDX(REPLICATOR_NAME),"+
      "index GROUP_NAME_IDX(GROUP_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add column METADATA_DATETIME datetime after NOTES";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql="update CART set METADATA_DATETIME=now()";
    q=new RDSqlQuery(sql,false);
    delete q;

    //
    // Create REPL_CART_STATE Table
    //
    sql=QString("create table if not exists REPL_CART_STATE (")+
      "ID int unsigned not null auto_increment primary key,"+
      "REPLICATOR_NAME char(32) not null,"+
      "CART_NUMBER int unsigned not null,"+
      "ITEM_DATETIME datetime not null,"+
      "unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CART_NUMBER))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<198) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table VERSION add column LAST_ISCI_XREFERENCE datetime default \"1970-01-01 00:00:00\" after LAST_MAINT_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table SYSTEM add column ISCI_XREFERENCE_PATH char(255) after MAX_POST_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<199) {
    sql="create index TYPE_IDX on ISCI_XREFERENCE (TYPE,LATEST_DATE)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="create index LATEST_DATE_IDX on ISCI_XREFERENCE (LATEST_DATE)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<200) {
  }

  if(ver<201) {
    sql="alter table STATIONS add column HTTP_STATION char(64) default \"localhost\" after IPV4_ADDRESS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column CAE_STATION char(64) \
         default \"localhost\" after HTTP_STATION";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<202) {
    sql=QString("create table if not exists RDHOTKEYS (")+
      "ID int unsigned not null auto_increment primary key,"+
      "STATION_NAME char(64),"+
      "MODULE_NAME char(64),"+
      "KEY_ID int,"+
      "KEY_VALUE char(64),"+
      "KEY_LABEL char(64))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="select NAME from STATIONS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      if (!UpdateRDAirplayHotkeys((const char *)q->value(0).toString())) {
	return UPDATEDB_QUERY_FAILED;
      }
    }
    delete q;
  }

  if(ver<203) {
    sql=
      "alter table REPLICATORS drop column NORMALIZATION_LEVEL";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=
      "alter table REPLICATORS add column NORMALIZATION_LEVEL int default 0 after ENABLE_METADATA";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPL_CART_STATE add column REPOST enum('N','Y') default 'N' after ITEM_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPL_CART_STATE add column POSTED_FILENAME char(255) after CART_NUMBER";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="drop index REPLICATOR_NAME_IDX on REPL_CART_STATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="create unique index REPLICATOR_NAME_IDX on REPL_CART_STATE (REPLICATOR_NAME,CART_NUMBER,POSTED_FILENAME)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="delete from REPL_CART_STATE where POSTED_FILENAME is null";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<204) {
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
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<205) {
    sql="alter table STATIONS add column START_JACK enum('N','Y') default 'N' after FILTER_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table STATIONS add column JACK_SERVER_NAME char(64) after START_JACK";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<206) {
    sql=QString("create table if not exists REPORT_GROUPS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "REPORT_NAME char(64) not null,"+
      "GROUP_NAME char(10),"+
      "index IDX_REPORT_NAME (REPORT_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table REPORTS add column FILTER_GROUPS enum('N','Y') default 'N' after FILTER_ONAIR_FLAG";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<207) {
    //
    // Removed as redundant with update 183 [call to UpdateImportFormats()].
    //
  }

  if(ver<208) {
    sql="alter table RDAIRPLAY add column HOUR_SELECTOR_ENABLED enum('N','Y') default 'N' after DEFAULT_SERVICE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<209) {
    sql="alter table STATIONS add column JACK_COMMAND_LINE char(255) after JACK_SERVER_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("create table if not exists JACK_CLIENTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "DESCRIPTION char(64),"+
      "COMMAND_LINE char(255) not null,"+
      "index IDX_STATION_NAME (STATION_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<210) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<211) {
    sql=QString("alter table SYSTEM add column TEMP_CART_GROUP char(10) ")+
      "default \"TEMP\" after ISCI_XREFERENCE_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<212) {
    sql=QString("alter table CARTSLOTS add column HOOK_MODE int default 0 ")+
      "after DEFAULT_CART_NUMBER";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CARTSLOTS add column DEFAULT_HOOK_MODE int ")+
      "default -1 after HOOK_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<213) {
    sql=QString("alter table STATIONS add column CUE_CARD int default 0 ")+
      "after JACK_COMMAND_LINE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column CUE_PORT int default 0 ")+
      "after CUE_CARD";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column CARTSLOT_COLUMNS int ")+
      "default 1 after CUE_PORT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column CARTSLOT_ROWS int ")+
      "default 8 after CARTSLOT_COLUMNS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select STATION_NAME,CARD_NUMBER,PORT_NUMBER from DECKS ")+
      "where CHANNEL=0";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update STATIONS set ")+
	QString().sprintf("CUE_CARD=%d,",q->value(1).toInt())+
	QString().sprintf("CUE_PORT=%d ",q->value(2).toInt())+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<214) {
    sql=QString("alter table SERVICES add column AUTOSPOT_GROUP char(10) ")+
      "after TRACK_GROUP";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<215) {
    sql=QString("alter table GROUPS add column DELETE_EMPTY_CARTS ")+
      "enum('N','Y') default 'N' after CUT_SHELFLIFE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<216) {
    sql="alter table CUTS add index ISCI_IDX(ISCI)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CUTS add index ISRC_IDX(ISRC)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<217) {
    sql=QString("create table if not exists LIVEWIRE_GPIO_SLOTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "SLOT int not null,"+
      "SOURCE_NUMBER int,"+
      "index STATION_NAME_IDX(STATION_NAME,MATRIX))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<218) {
    sql=QString("alter table LIVEWIRE_GPIO_SLOTS ")+
      "add column IP_ADDRESS char(15) after SLOT";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<219) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
	q1=new RDSqlQuery(sql,false);
	delete q1;
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
	  q1=new RDSqlQuery(sql,false);
	  delete q1;
	}
	delete q;
      }
    }

    //
    // Clean Up RDAirPlay
    //
    sql="alter table RDAIRPLAY drop column INSTANCE";
    q=new RDSqlQuery(sql,false);
    delete q;
    for(unsigned i=0;i<10;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY drop column CARD%u",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column PORT%u",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column START_RML%u",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column STOP_RML%u",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
    
    //
    // Clean Up RDPanel
    //
    sql="alter table RDPANEL drop column INSTANCE";
    q=new RDSqlQuery(sql,false);
    delete q;
    for(unsigned i=0;i<10;i++) {
      if((i==2)||(i==3)||(i==6)||(i==7)||(i==8)||(i==9)) {
	sql=QString().sprintf("alter table RDPANEL drop column CARD%u",i);
	q=new RDSqlQuery(sql,false);
	delete q;

	sql=QString().sprintf("alter table RDPANEL drop column PORT%u",i);
	q=new RDSqlQuery(sql,false);
	delete q;

	sql=QString().sprintf("alter table RDPANEL drop column START_RML%u",i);
	q=new RDSqlQuery(sql,false);
	delete q;

	sql=QString().sprintf("alter table RDPANEL drop column STOP_RML%u",i);
	q=new RDSqlQuery(sql,false);
	delete q;
      }
    }
    
  }

  if(ver<220) {
    sql=QString("alter table RDAIRPLAY_CHANNELS add column GPIO_TYPE ")+
      "int unsigned default 0 after STOP_RML";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL_CHANNELS add column GPIO_TYPE ")+
      "int unsigned default 0 after STOP_RML";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<221) {
    sql="alter table RDLIBRARY modify column TRIM_THRESHOLD int default 0";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<222) {
    //
    // Removed as redundant with update 183 [call to UpdateImportFormats()].
    //
  }

  if(ver<223) {
    sql="alter table CART add column CONDUCTOR char(64) after LABEL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add index CONDUCTOR_IDX(CONDUCTOR)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add column SONG_ID char(32) after USER_DEFINED";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add index SONG_ID_IDX(SONG_ID)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql="alter table CART add column BPM int unsigned default 0 after SONG_ID";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<224) {
    sql=QString("alter table DROPBOXES add column SET_USER_DEFINED char(255) ")+
      "after CREATE_ENDDATE_OFFSET";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<225) {
    sql=QString("alter table RDAIRPLAY add column TITLE_TEMPLATE char(64) ")+
      "default '%t' after HOUR_SELECTOR_ENABLED";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ARTIST_TEMPLATE char(64) ")+
      "default '%a' after TITLE_TEMPLATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column OUTCUE_TEMPLATE char(64) ")+
      "default '%o' after ARTIST_TEMPLATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column DESCRIPTION_TEMPLATE char(64) ")+
      "default '%i' after HOUR_SELECTOR_ENABLED";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<226) {
    sql=QString("alter table RDLOGEDIT add column ")+
      "ENABLE_SECOND_START enum('N','Y') default 'Y' after BITRATE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<227) {
    sql="alter table LOGS add index TYPE_IDX(TYPE,LOG_EXISTS)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<228) {
    sql=QString("alter table RDLIBRARY add column ")+
      "LIMIT_SEARCH int default 1 after SRC_CONVERTER";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY add column ")+
      "SEARCH_LIMITED enum('N','Y') default 'Y' after LIMIT_SEARCH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<229) {
    sql=QString("alter table SERVICES add column ")+
      "DESCRIPTION_TEMPLATE char(255) after NAME_TEMPLATE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update SERVICES set DESCRIPTION_TEMPLATE=\"")+
	RDEscapeString(q->value(0).toString())+" log for %m/%d/%Y\" "+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<230) { 
    sql="select NAME from LOGS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql="alter table `"+RDLog::tableName(q->value(0).toString())+
	"` add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql="alter table `"+RDEvent::preimportTableName(q->value(0).toString())+
	"` add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql="alter table `"+RDEvent::postimportTableName(q->value(0).toString())+
	"` add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
 }

  if(ver<231) { 
    sql=QString("alter table CART add column ")+
      "USE_EVENT_LENGTH enum('N','Y') default 'N' after METADATA_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<232) { 
    sql=QString("alter table STATIONS add column ")+
      "ENABLE_DRAGDROP enum('N','Y') default 'Y' after CARTSLOT_ROWS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      "ENFORCE_PANEL_SETUP enum('N','Y') default 'N' after ENABLE_DRAGDROP";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<233) { 
    sql=QString("alter table RDAIRPLAY add column ")+
      "LOG_MODE_STYLE int default 0 after START_MODE";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("create table if not exists LOG_MODES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MACHINE int unsigned not null,"+
      "START_MODE int not null default 0,"+
      "OP_MODE int not null default 2,"+
      "index STATION_NAME_IDX(STATION_NAME,MACHINE))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select STATION,START_MODE,OP_MODE from RDAIRPLAY");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<3;i++) {
	sql=QString("insert into LOG_MODES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%u,",i)+
	  QString().sprintf("START_MODE=%d,",q->value(1).toInt())+
	  QString().sprintf("OP_MODE=%d",q->value(2).toInt());
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
    }
    delete q;
  }

  if(ver<234) { 
    sql=QString("alter table STATIONS add column ")+
      "CUE_START_CART int unsigned after CUE_PORT";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      "CUE_STOP_CART int unsigned after CUE_START_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<235) { 
    // Lock Locking Changes, Superceded
  }

  if(ver<236) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column CONDUCTOR char(64) after LABEL";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column USER_DEFINED char(255) after COMPOSER";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column SONG_ID char(32) after USER_DEFINED";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<237) {
    sql=QString("alter table REPORTS add column ")+
      "START_TIME time after FILTER_GROUPS";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table REPORTS add column ")+
      "END_TIME time after START_TIME";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<238) {
    sql=QString("alter table CART add column ")+
      "PENDING_STATION char(64) after USE_EVENT_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CART add column ")+
      "PENDING_PID int after PENDING_STATION";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CART add column ")+
      "PENDING_DATETIME datetime after PENDING_PID";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CART add index ")+
      "PENDING_STATION_IDX(PENDING_STATION)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CART add index ")+
      "PENDING_PID_IDX(PENDING_STATION,PENDING_PID)";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CART add index ")+
      "PENDING_DATETIME_IDX(PENDING_DATETIME)";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<239) {
    sql=QString("create table if not exists DROPBOX_SCHED_CODES(")+
      "ID int auto_increment not null primary key,"+
      "DROPBOX_ID int not null,"+
      "SCHED_CODE char(11) not null,"
      "index DROPBOX_ID_IDX(DROPBOX_ID),"+
      "index SCHED_CODE_IDX(SCHED_CODE))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<240) {
    sql=QString("create table if not exists GPIO_EVENTS(")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "NUMBER int not null,"+
      "TYPE int not null,"+
      "EDGE int not null,"+
      "EVENT_DATETIME datetime not null,"+
      "index STATION_NAME_IDX(STATION_NAME,MATRIX,TYPE,EVENT_DATETIME,EDGE))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<241) {
    sql=QString("alter table EVENTS add column ")+
      "HAVE_CODE2 varchar(10) after HAVE_CODE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<242) {
    sql=QString("alter table REPORTS add column ")+
      "POST_EXPORT_CMD text after EXPORT_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table REPORTS add column ")+
      "WIN_POST_EXPORT_CMD text after WIN_EXPORT_PATH";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<243) {
    sql=QString("alter table STATIONS add column ")+
      "HAVE_MP4_DECODE enum('N','Y') default 'N' after HAVE_MPG321";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<244) {
    sql=QString("alter table JACK_CLIENTS modify column ")+
      "COMMAND_LINE text not null";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<245) {
    sql=QString("alter table RDLIBRARY add column ")+
      "READ_ISRC enum('N','Y') default 'Y' after CDDB_SERVER";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  /*
   * Versions 246 - 253 are reserved
   */

  if(ver<254) {
    sql=QString("alter table CART add column ")+
      "USE_WEIGHTING enum('N','Y') default 'Y' after ENFORCE_LENGTH";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CUTS add column PLAY_ORDER int after WEIGHT");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select CUT_NAME from CUTS order by CUT_NAME");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update CUTS set ")+
	"PLAY_ORDER="+q->value(0).toString().right(3)+" "+
	"where CUT_NAME=\""+q->value(0).toString()+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<255) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column DESCRIPTION char(64) after USAGE_CODE";
      q1=new RDSqlQuery(sql,false);
      delete q1;

      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column OUTCUE char(64) after DESCRIPTION";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<256) {
    sql=QString("create table if not exists CUT_EVENTS(")+
      "ID int auto_increment not null primary key,"+
      "CUT_NAME char(12) not null,"+
      "NUMBER int not null,"+
      "POINT int not null,"+
      "index CUT_NAME_IDX(CUT_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("create table if not exists DECK_EVENTS(")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CHANNEL int unsigned not null,"+
      "NUMBER int not null,"+
      "CART_NUMBER int unsigned not null default 0,"+
      "index STATION_NAME_IDX(STATION_NAME,CHANNEL))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i++) {
	for(unsigned j=0;j<MAX_DECKS;j++) {
	  sql=QString("insert into DECK_EVENTS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CHANNEL=%u,",j+129)+
	    QString().sprintf("NUMBER=%u",i+1);
	  q1=new RDSqlQuery(sql,false);
	  delete q1;
	}
      }
    }
    delete q;
  }

  if(ver<257) {
    sql=QString("alter table LOGS modify column LINK_DATETIME datetime");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table LOGS modify column START_DATE date");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table LOGS modify column END_DATE date");
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<258) {
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("alter table ")+
	"`"+RDLog::tableName(q->value(0).toString())+"` "+
	"modify column CART_NUMBER int unsigned not null default 0";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<259) {
    sql=QString("create table if not exists SERVICE_CLOCKS(")+
      "ID int auto_increment not null primary key,"+
      "SERVICE_NAME char(10) not null,"+
      "HOUR int not null,"+
      "CLOCK_NAME char(64) default null,"+
      "index SERVICE_NAME_IDX(SERVICE_NAME,HOUR),"+
      "index CLOCK_NAME_IDX(CLOCK_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
    }
    delete q;

    for(int i=0;i<168;i++) {
      sql=QString().sprintf("alter table SERVICES drop column CLOCK%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
  }

  if(ver<260) {
    sql=QString("create table if not exists WEBAPI_AUTHS(")+
      "TICKET char(41) not null primary key,"+
      "LOGIN_NAME char(255) not null,"+
      "IPV4_ADDRESS char(16) not null,"+
      "EXPIRATION_DATETIME datetime not null,"+
      "index TICKET_IDX(TICKET,IPV4_ADDRESS,EXPIRATION_DATETIME))"+
      config->createTablePostfix();
    if(!RunQuery(sql)) {
      return false;
    }

    sql=QString("alter table USERS add column ")+
      "WEBAPI_AUTH_TIMEOUT int not null default 3600 after PASSWORD";
    if(!RunQuery(sql)) {
      return false;
    }
  }

  if(ver<261) {
    sql=QString("alter table CUTS add column ")+
      "SHA1_HASH char(40) after LENGTH";
    if(!RunQuery(sql)) {
      return false;
    }

    sql="create index SHA1_HASH_IDX on CUTS(SHA1_HASH)";
    if(!RunQuery(sql)) {
      return false;
    }
  }

  if(ver<262) {
    sql=QString("alter table USERS add column ")+
      "LOCAL_AUTH enum('N','Y') default 'Y' after ENABLE_WEB";
    if(!RunQuery(sql)) {
      return false;
    }

    sql=QString("alter table USERS add column ")+
      "PAM_SERVICE char(32) default \"rivendell\" after LOCAL_AUTH";
    if(!RunQuery(sql)) {
      return false;
    }

    sql=QString("create index IPV4_ADDRESS_IDX on STATIONS (IPV4_ADDRESS)");
    if(!RunQuery(sql)) {
      return false;
    }
  }

  if(ver<263) {  // Add missing LOG_MODES records
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
	  q2=new RDSqlQuery(sql,false);
	  delete q2;
	}
	delete q1;
      }
    }
    delete q;
  }

  if(ver<264) {
    sql=QString("alter table SYSTEM add column ")+
      "FIX_DUP_CART_TITLES enum('N','Y') not null default 'Y' after "+
      "DUP_CART_TITLES";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<265) {
    sql=QString("alter table SYSTEM add column ")+
      "SHOW_USER_LIST enum('N','Y') not null default 'Y' "+
      "after TEMP_CART_GROUP";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<266) {
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("select `")+RDLog::tableName(q->value(0).toString())+
	"`.ID from "+
	"`"+RDLog::tableName(q->value(0).toString())+"` left join CART "+
	"on `"+RDLog::tableName(q->value(0).toString())+
	"`.CART_NUMBER=CART.NUMBER where "+
	"CART.OWNER is not null";
      q1=new RDSqlQuery(sql,false);
      int completed=q1->size();
      delete q1;

      sql=QString("select ID from `")+RDLog::tableName(q->value(0).toString())+
	"` where "+QString().sprintf("TYPE=%d",RDLogLine::Track);
      q1=new RDSqlQuery(sql,false);
      int scheduled=q1->size()+completed;
      delete q1;

      sql=QString("update LOGS set ")+
	QString().sprintf("SCHEDULED_TRACKS=%d,",scheduled)+
	QString().sprintf("COMPLETED_TRACKS=%u ",completed)+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<267) {
    sql=QString("alter table CUTS add column ORIGIN_LOGIN_NAME char(255) ")+
      "after ORIGIN_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table CUTS add column SOURCE_HOSTNAME char(255) ")+
      "after ORIGIN_LOGIN_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<268) {
    sql=QString("alter table DROPBOXES add column ")+
      "FORCE_TO_MONO enum('N','Y') default 'N' after TO_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<269) {
    sql=QString("alter table GROUPS add column ")+
      "DEFAULT_CUT_LIFE int default -1 after DEFAULT_HIGH_CART";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<270) {
    sql=QString("alter table STATIONS add column ")+
      "SHORT_NAME char(64) after NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update STATIONS set ")+
	"SHORT_NAME=\""+RDEscapeString(q->value(0).toString())+"\" where "+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }
    delete q;
  }

  if(ver<271) {
    sql=QString("alter table DROPBOXES ")+
      "add column SEGUE_LEVEL int(11) default 1 after FORCE_TO_MONO, "+
      "add column SEGUE_LENGTH int(11) default 0 after SEGUE_LEVEL";
    q=new RDSqlQuery(sql);
    delete q;
  }

  if(ver<272) {
    sql=QString("create table if not exists USER_SERVICE_PERMS (")+
      "ID int auto_increment not null primary key,"+
      "USER_NAME char(255) not null,"+
      "SERVICE_NAME char(10) not null,"+
      "index USER_NAME_IDX(USER_NAME))"+
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select LOGIN_NAME from USERS");
     q=new RDSqlQuery(sql,false);
     while(q->next()) {
       sql=QString("select NAME from SERVICES");
       q1=new RDSqlQuery(sql,false);
       while(q1->next()) {
	 sql=QString("insert into USER_SERVICE_PERMS set ")+
	   "USER_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	   "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	 q2=new RDSqlQuery(sql,false);
	 delete q2;
       }
       delete q1;
     }
     delete q;
  }

  if(ver<273) {
    sql=QString("alter table LOGS ")+
      "add column LOCK_USER_NAME char(255) after NEXT_ID";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table LOGS ")+
      "add column LOCK_STATION_NAME char(64) after LOCK_USER_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table LOGS ")+
      "add column LOCK_IPV4_ADDRESS char(16) after LOCK_STATION_NAME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table LOGS ")+
      "add column LOCK_DATETIME datetime after LOCK_IPV4_ADDRESS";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<274) {
    sql=QString("alter table LOGS ")+
      "add column LOCK_GUID char(82) after LOCK_DATETIME";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table LOGS add index LOCK_GUID_IDX(LOCK_GUID)");
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<275) {
    sql=QString("alter table SERVICES ")+
      "add column LOG_SHELFLIFE_ORIGIN int default 0 "+
      "after DEFAULT_LOG_SHELFLIFE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<276) {
    sql=QString("alter table SYSTEM ")+
      "add column NOTIFICATION_ADDRESS char(15) default \""+
      RD_NOTIFICATION_ADDRESS+"\" after SHOW_USER_LIST";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<277) {
    sql=QString("alter table USERS ")+
      "add column WEBGET_LOGIN_PRIV enum('N','Y') not null default 'N' "+
      "after EDIT_AUDIO_PRIV";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<278) {
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
      config->createTablePostfix();
    q=new RDSqlQuery(sql,false);
    delete q;

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
	q1=new RDSqlQuery(sql,false);
	delete q1;
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
	q1=new RDSqlQuery(sql);
	delete q1;
      }
    }
    delete q;

    for(int i=0;i<3;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY drop column UDP_ADDR%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column UDP_PORT%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column UDP_STRING%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_START_MODE",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_AUTO_RESTART",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_LOG_NAME",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_CURRENT_LOG",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_RUNNING",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_LOG_ID",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_LOG_LINE",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_NOW_CART",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column LOG%d_NEXT_CART",i);
      q=new RDSqlQuery(sql,false);
      delete q;
   }
  }

  if(ver<279) {
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
	sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("INSTANCE=%u",i+RD_RDVAIRPLAY_LOG_BASE);
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
    }
    delete q;
  }

  if(ver<280) {
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(unsigned i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
	sql=QString("insert into LOG_MODES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%u",i+RD_RDVAIRPLAY_LOG_BASE);
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
    }
  }

  if(ver<281) {
    sql=QString("alter table RDAIRPLAY add column ")+
      "VIRTUAL_EXIT_CODE int default 0 after EXIT_CODE";
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<282) {
    sql=QString("create table if not exists AUDIO_CARDS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "DRIVER int not null default 0,"+
      "NAME char(64),"+
      "INPUTS int not null default -1,"+
      "OUTPUTS int not null default -1,"+
      "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER))";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<RD_MAX_CARDS;i++) {
	sql=QString("insert into AUDIO_CARDS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("CARD_NUMBER=%d",i);
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
    }
    delete q;

    for(int i=0;i<8;i++) {
      sql=QString("alter table STATIONS drop column ")+
	QString().sprintf("CARD%d_DRIVER",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table STATIONS drop column ")+
	QString().sprintf("CARD%d_NAME",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table STATIONS drop column ")+
	QString().sprintf("CARD%d_INPUTS",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table STATIONS drop column ")+
	QString().sprintf("CARD%d_OUTPUTS",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }
  }

  if(ver<283) {
    sql=QString("alter table AUDIO_CARDS add column ")+
      "CLOCK_SOURCE int not null default 0 after OUTPUTS";
    q=new RDSqlQuery(sql,false);
    delete q;

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
      q1=new RDSqlQuery(sql,false);
      delete q1;
    }

    sql=QString("create table if not exists AUDIO_INPUTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "PORT_NUMBER int not null,"+
      "LEVEL int not null default 0,"+
      "TYPE int not null default 0,"
      "MODE int not null default 0,"+
      "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER,PORT_NUMBER))";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<24;i++) {
	for(int j=0;j<24;j++) {
	  sql=QString("insert into AUDIO_INPUTS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CARD_NUMBER=%d,",i)+
	    QString().sprintf("PORT_NUMBER=%d",j);
	  q1=new RDSqlQuery(sql,false);
	  delete q1;
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
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
      delete q;
    }

    sql=QString("create table if not exists AUDIO_OUTPUTS (")+
      "ID int auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "PORT_NUMBER int not null,"+
      "LEVEL int not null default 0,"+
      "unique index STATION_NAME_IDX(STATION_NAME,CARD_NUMBER,PORT_NUMBER))";
    q=new RDSqlQuery(sql,false);
    delete q;
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<24;i++) {
	for(int j=0;j<24;j++) {
	  sql=QString("insert into AUDIO_OUTPUTS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("CARD_NUMBER=%d,",i)+
	    QString().sprintf("PORT_NUMBER=%d",j);
	  q1=new RDSqlQuery(sql,false);
	  delete q1;
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
	q1=new RDSqlQuery(sql,false);
	delete q1;
      }
      delete q;
    }

    sql=QString("drop table AUDIO_PORTS");
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<284) {
    sql=QString("alter table RDAIRPLAY drop column INSTANCE");
    q=new RDSqlQuery(sql,false);
    delete q;

    for(int i=0;i<10;i++) {
      sql=QString("alter table RDAIRPLAY ")+
	QString().sprintf("drop column CARD%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDAIRPLAY ")+
	QString().sprintf("drop column PORT%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDAIRPLAY ")+
	QString().sprintf("drop column START_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDAIRPLAY ")+
	QString().sprintf("drop column STOP_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    sql=QString("alter table RDAIRPLAY drop column OP_MODE");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY drop column START_MODE");
    q=new RDSqlQuery(sql,false);
    delete q;


    sql=QString("alter table RDPANEL drop column INSTANCE");
    q=new RDSqlQuery(sql,false);
    delete q;

    for(int i=2;i<4;i++) {
      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column CARD%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column PORT%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column START_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column STOP_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    for(int i=6;i<10;i++) {
      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column CARD%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column PORT%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column START_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;

      sql=QString("alter table RDPANEL ")+
	QString().sprintf("drop column STOP_RML%d",i);
      q=new RDSqlQuery(sql,false);
      delete q;
    }

    sql=QString("alter table MATRICES alter column PORT set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table MATRICES alter column PORT_2 set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table MATRICES alter column INPUTS set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table MATRICES alter column OUTPUTS set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table MATRICES alter column GPIS set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table MATRICES alter column GPOS set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table REPLICATORS alter column TYPE_ID set default 0");
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  if(ver<285) {
    sql=QString("alter table CART drop column ISRC");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table DECKS drop column DEFAULT_SAMPRATE");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY drop column INPUT_STREAM");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY drop column OUTPUT_STREAM");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY drop column RECORD_GPI");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY drop column PLAY_GPI");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY drop column STOP_GPI");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLIBRARY drop column DEFAULT_SAMPRATE");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDLOGEDIT drop column SAMPRATE");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RECORDINGS drop column SAMPRATE");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table SERVICES drop column TFC_START_OFFSET");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table SERVICES drop column TFC_START_LENGTH");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table SERVICES drop column MUS_START_OFFSET");
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table SERVICES drop column MUS_START_LENGTH");
    q=new RDSqlQuery(sql,false);
    delete q;
  }



  //
  // Maintainer's Note:
  //
  // When adding a schema update here, be sure also to implement the
  // corresponding reversion in rdrevert(8)!
  //
  
  // **** End of version updates ****

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

  //
  // Update Version Field
  //
  q=new RDSqlQuery(QString().sprintf("update VERSION set DB=%d",
				     RD_VERSION_DATABASE),false);
  delete q;

  return UPDATEDB_SUCCESS;
}
