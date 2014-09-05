// createdb.cpp
//
// Create, Initialize and/or Update a Rivendell Database
//
//   (C) Copyright 2002-2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: createdb.cpp,v 1.195.2.32.2.5 2014/06/05 19:04:25 cvs Exp $
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

#include <rddb.h>
#include <rd.h>
#include <rdevent.h>
#include <rdlog.h>
#include <dbversion.h>
#include <rdcart.h>
#include <rdcreate_log.h>
#include <rdrecording.h>
#include <rdmacro_event.h>
#include <rdpaths.h>
#include <rdsvc.h>
#include <rdlog_line.h>
#include <rdcreateauxfieldstable.h>
#include <rdfeedlog.h>
#include <rdescape_string.h>
#include "rdconfig.h"
#include <createdb.h>
#include <globals.h>
#include <rdconf.h>

//
// NOTE TO MAINTAINERS:
// Be sure to use the QSqlQuery class to run queries throughout this file, 
// *not* RDSqlQuery!  RDSqlQuery will kill outer sql 'select' loops when 
// schema updates are reapplied!
//

bool RunQuery(QString sql)
{
  QSqlQuery *q=new QSqlQuery(sql);
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

  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=1,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 1"));
  
  if(!RunQuery(sql)) {
    return false;
  }

  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",					       \
      KEY_ID=2,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 1"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=3,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 1"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=4,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 2"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=5,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 2"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=6,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 2"));
  if(!RunQuery(sql)) {
    return false;
  }
  
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=7,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 3"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=8,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 3"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=9,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 3"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=10,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 4"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=11,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 4"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=12,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 4"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=13,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 5"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=14,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 5"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=15,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 5"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=16,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 6"));
  if(!RunQuery(sql)) {
    return false;
  }
      
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=17,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 6"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=18,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 6"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=19,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Start Line 7"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=20,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Stop Line 7"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=21,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Pause Line 7"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=22,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Add"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=23,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Delete"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=24,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Copy"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=25,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Move"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=26,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Sound Panel"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=27,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Main Log"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=28,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Aux Log 1"));
  if(!RunQuery(sql)) {
    return false;
  }
 
  sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
      MODULE_NAME=\"airplay\",\
      KEY_ID=29,\
      KEY_LABEL=\"%s\" ",
			(const char *)current_station,
			(const char *)QObject::tr("Aux Log 2"));
  if(!RunQuery(sql)) {
    return false;
  }
return true;
}

void UpdateImportFormats()
{
  QString sql;
  QSqlQuery *q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"Rivendell Standard Import\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=69,\
         DATA_LENGTH=32";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"PowerGold Music Scheduling\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=69,\
         DATA_LENGTH=32";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"RadioTraffic.com\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=69,\
         DATA_LENGTH=32";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"CounterPoint Traffic\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         EVENT_ID_OFFSET=69,\
         EVENT_ID_LENGTH=32,\
         DATA_OFFSET=102,\
         DATA_LENGTH=32";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"WideOrbit Traffic\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         EVENT_ID_OFFSET=69,\
         EVENT_ID_LENGTH=32,\
         DATA_OFFSET=102,\
         DATA_LENGTH=32";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"Visual Traffic\",\
         CART_OFFSET=14,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=39,\
         HOURS_OFFSET=5,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=8,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=11,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=0,\
         LEN_HOURS_LENGTH=0,\
         LEN_MINUTES_OFFSET=97,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=99,\
         LEN_SECONDS_LENGTH=2,\
         EVENT_ID_OFFSET=0,\
         EVENT_ID_LENGTH=0,\
         DATA_OFFSET=0,\
         DATA_LENGTH=0";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"Music 1\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=69,\
         DATA_LENGTH=32";
  q=new QSqlQuery(sql);
  delete q;

  sql="insert into IMPORT_TEMPLATES set\
         NAME=\"NaturalLog\",\
         CART_OFFSET=9,\
         CART_LENGTH=6,\
         TITLE_OFFSET=19,\
         TITLE_LENGTH=40,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=61,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=64,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=67,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=0,\
         DATA_LENGTH=0";
  q=new QSqlQuery(sql);
  delete q;
}


bool CreateDb(QString name,QString pwd)
{
  QString sql;

//
// Create USERS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS USERS (\
    LOGIN_NAME CHAR(255) NOT NULL PRIMARY KEY,\
    FULL_NAME CHAR(255),\
    PHONE_NUMBER CHAR(20),\
    DESCRIPTION CHAR(255),\
    PASSWORD CHAR(32),\
    ENABLE_WEB enum('N','Y') default 'N',\
    ADMIN_USERS_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    ADMIN_CONFIG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    CREATE_CARTS_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    DELETE_CARTS_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    MODIFY_CARTS_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    EDIT_AUDIO_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    ASSIGN_CART_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    CREATE_LOG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    DELETE_LOG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    DELETE_REC_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    PLAYOUT_LOG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    ARRANGE_LOG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    MODIFY_TEMPLATE_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    ADDTO_LOG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    REMOVEFROM_LOG_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    CONFIG_PANELS_PRIV enum('N','Y') not null default 'N',\
    VOICETRACK_LOG_PRIV enum('N','Y') not null default 'N',\
    EDIT_CATCHES_PRIV ENUM('N','Y') NOT NULL DEFAULT 'N',\
    ADD_PODCAST_PRIV enum('N','Y') not null default 'N',\
    EDIT_PODCAST_PRIV enum('N','Y') not null default 'N',\
    DELETE_PODCAST_PRIV enum('N','Y') not null default 'N',\
    INDEX FULL_NAME_IDX (FULL_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create STATIONS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS STATIONS (\
      NAME CHAR(64) PRIMARY KEY NOT NULL,\
      DESCRIPTION CHAR(64),\
      USER_NAME CHAR(255),\
      DEFAULT_NAME CHAR(255),\
      IPV4_ADDRESS CHAR(15) default \"127.0.0.2\",\
      HTTP_STATION char(64) default \"localhost\",\
      CAE_STATION char(64) default \"localhost\",\
      TIME_OFFSET int default 0,\
      BACKUP_DIR char(255),\
      BACKUP_LIFE int default 0,\
      BROADCAST_SECURITY int unsigned default 0,\
      HEARTBEAT_CART int unsigned default 0,\
      HEARTBEAT_INTERVAL int unsigned default 0,\
      STARTUP_CART int unsigned default 0,\
      EDITOR_PATH char(255) default \"\",\
      FILTER_MODE int default 0,\
      START_JACK enum('N','Y') default 'N',\
      JACK_SERVER_NAME char(64),\
      JACK_COMMAND_LINE char(255),\
      CUE_CARD int default 0,\
      CUE_PORT int default 0,\
      CUE_START_CART int unsigned,\
      CUE_STOP_CART int unsigned,\
      CARTSLOT_COLUMNS int default 1,\
      CARTSLOT_ROWS int default 8,\
      ENABLE_DRAGDROP enum('N','Y') default 'Y',\
      ENFORCE_PANEL_SETUP enum('N','Y') default 'N',\
      SYSTEM_MAINT enum('N','Y') default 'Y',\
      STATION_SCANNED enum('N','Y') default 'N',\
      HAVE_OGGENC enum('N','Y') default 'N',\
      HAVE_OGG123 enum('N','Y') default 'N',\
      HAVE_FLAC enum('N','Y') default 'N',\
      HAVE_TWOLAME enum('N','Y') default 'N',\
      HAVE_LAME enum('N','Y') default 'N',\
      HAVE_MPG321 enum('N','Y') default 'N',\
      HPI_VERSION char(16),\
      JACK_VERSION char(16),\
      ALSA_VERSION char(16),\
      CARD0_DRIVER int(11) default 0,\
      CARD0_NAME char(64),\
      CARD0_INPUTS int default -1,\
      CARD0_OUTPUTS int default -1,\
      CARD1_DRIVER int(11) default 0,\
      CARD1_NAME char(64),\
      CARD1_INPUTS int default -1,\
      CARD1_OUTPUTS int default -1,\
      CARD2_DRIVER int(11) default 0,\
      CARD2_NAME char(64),\
      CARD2_INPUTS int default -1,\
      CARD2_OUTPUTS int default -1,\
      CARD3_DRIVER int(11) default 0,\
      CARD3_NAME char(64),\
      CARD3_INPUTS int default -1,\
      CARD3_OUTPUTS int default -1,\
      CARD4_DRIVER int(11) default 0,\
      CARD4_NAME char(64),\
      CARD4_INPUTS int default -1,\
      CARD4_OUTPUTS int default -1,\
      CARD5_DRIVER int(11) default 0,\
      CARD5_NAME char(64),\
      CARD5_INPUTS int default -1,\
      CARD5_OUTPUTS int default -1,\
      CARD6_DRIVER int(11) default 0,\
      CARD6_NAME char(64),\
      CARD6_INPUTS int default -1,\
      CARD6_OUTPUTS int default -1,\
      CARD7_DRIVER int(11) default 0,\
      CARD7_NAME char(64),\
      CARD7_INPUTS int default -1,\
      CARD7_OUTPUTS int default -1,\
      INDEX DESCRIPTION_IDX (DESCRIPTION))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create CART table
//
  sql=QString("CREATE TABLE IF NOT EXISTS CART (\
      NUMBER INT UNSIGNED NOT NULL PRIMARY KEY,\
      TYPE INT UNSIGNED NOT NULL,\
      GROUP_NAME CHAR(10) NOT NULL,\
      TITLE CHAR(255),\
      ARTIST CHAR(255),\
      ALBUM CHAR(255),\
      YEAR DATE,\
      ISRC CHAR(12),\
      CONDUCTOR CHAR(64),\
      LABEL CHAR(64),\
      CLIENT CHAR(64),\
      AGENCY CHAR(64),\
      PUBLISHER CHAR(64),\
      COMPOSER CHAR(64),\
      USER_DEFINED CHAR(255),\
      SONG_ID CHAR(32),\
      BPM int unsigned default 0,\
      USAGE_CODE int default 0,\
      FORCED_LENGTH INT UNSIGNED,\
      AVERAGE_LENGTH int unsigned,\
      LENGTH_DEVIATION int unsigned default 0,\
      AVERAGE_SEGUE_LENGTH int unsigned,\
      AVERAGE_HOOK_LENGTH int unsigned default 0,\
      CUT_QUANTITY INT UNSIGNED,\
      LAST_CUT_PLAYED INT UNSIGNED,\
      PLAY_ORDER INT UNSIGNED,\
      VALIDITY int unsigned default 2,\
      START_DATETIME DATETIME,\
      END_DATETIME DATETIME,\
      ENFORCE_LENGTH ENUM('N','Y') DEFAULT 'N',\
      PRESERVE_PITCH ENUM('N','Y') DEFAULT 'N',\
      ASYNCRONOUS enum('N','Y') default 'N',\
      OWNER char(64),\
      MACROS text,\
      SCHED_CODES VARCHAR( 255 ) NULL DEFAULT NULL,\
      NOTES text,\
      METADATA_DATETIME datetime,\
      USE_EVENT_LENGTH enum('N','Y') default 'N',\
      PENDING_STATION char(64),\
      PENDING_PID int,\
      PENDING_DATETIME datetime,\
      INDEX GROUP_NAME_IDX (GROUP_NAME),\
      INDEX TITLE_IDX (TITLE),\
      INDEX ARTIST_IDX (ARTIST),\
      INDEX ALBUM_IDX (ALBUM),\
      INDEX CONDUCTOR_IDX (CONDUCTOR),\
      INDEX LABEL_IDX (LABEL),\
      INDEX CLIENT_IDX (CLIENT),\
      INDEX AGENCY_IDX (AGENCY),\
      INDEX PUBLISHER_IDX (PUBLISHER),\
      INDEX COMPOSER_IDX (COMPOSER),\
      INDEX USER_DEFINED_IDX (USER_DEFINED),\
      INDEX SONG_ID_IDX (SONG_ID),\
      INDEX OWNER_IDX (OWNER),\
      index METADATA_DATETIME_IDX (METADATA_DATETIME),\
      index PENDING_STATION_IDX(PENDING_STATION),\
      index PENDING_PID_IDX(PENDING_STATION,PENDING_PID),\
      index PENDING_DATETIME_IDX(PENDING_DATETIME))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create CUTS table
//
  sql=QString().sprintf("CREATE TABLE IF NOT EXISTS CUTS (\
      CUT_NAME CHAR(12) PRIMARY KEY NOT NULL,\
      CART_NUMBER INT UNSIGNED NOT NULL,\
      EVERGREEN enum('N','Y') default 'N',\
      DESCRIPTION CHAR(64),\
      OUTCUE CHAR(64),\
      ISRC char(12),\
      ISCI char(32),\
      LENGTH INT UNSIGNED,\
      ORIGIN_DATETIME DATETIME,\
      START_DATETIME DATETIME,\
      END_DATETIME DATETIME,\
      SUN enum('N','Y') default 'Y',\
      MON enum('N','Y') default 'Y',\
      TUE enum('N','Y') default 'Y',\
      WED enum('N','Y') default 'Y',\
      THU enum('N','Y') default 'Y',\
      FRI enum('N','Y') default 'Y',\
      SAT enum('N','Y') default 'Y',\
      START_DAYPART TIME,\
      END_DAYPART TIME,\
      ORIGIN_NAME CHAR(64),\
      WEIGHT INT UNSIGNED DEFAULT 1,\
      LAST_PLAY_DATETIME DATETIME,\
      UPLOAD_DATETIME datetime,\
      PLAY_COUNTER INT UNSIGNED DEFAULT 0,\
      LOCAL_COUNTER int unsigned default 0,\
      VALIDITY int unsigned default 2,\
      CODING_FORMAT INT UNSIGNED,\
      SAMPLE_RATE INT UNSIGNED,\
      BIT_RATE INT UNSIGNED,\
      CHANNELS INT UNSIGNED,\
      PLAY_GAIN INT DEFAULT 0,\
      START_POINT INT DEFAULT -1,\
      END_POINT INT DEFAULT -1,\
      FADEUP_POINT INT DEFAULT -1,\
      FADEDOWN_POINT INT DEFAULT -1,\
      SEGUE_START_POINT INT DEFAULT -1,\
      SEGUE_END_POINT INT DEFAULT -1,\
      SEGUE_GAIN INT DEFAULT %d,\
      HOOK_START_POINT INT DEFAULT -1,\
      HOOK_END_POINT INT DEFAULT -1,\
      TALK_START_POINT INT DEFAULT -1,\
      TALK_END_POINT INT DEFAULT -1,\
      index CART_NUMBER_IDX (CART_NUMBER),\
      index DESCRIPTION_IDX (DESCRIPTION),\
      index OUTCUE_IDX (OUTCUE),\
      index ORIGIN_DATETIME_IDX (ORIGIN_DATETIME),\
      index START_DATETIME_IDX (START_DATETIME),\
      index END_DATETIME_IDX (END_DATETIME),\
      index ISCI_IDX (ISCI),\
      index ISRC_IDX (ISRC))",RD_FADE_DEPTH);
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create CLIPBOARD table
//
  sql=QString("CREATE TABLE IF NOT EXISTS CLIPBOARD (\
      CUT_NAME CHAR(12) PRIMARY KEY NOT NULL,\
      CART_NUMBER INT UNSIGNED NOT NULL,\
      DESCRIPTION CHAR(64),\
      OUTCUE CHAR(64),\
      LENGTH INT UNSIGNED,\
      ORIGIN_DATETIME DATETIME,\
      ORIGIN_NAME CHAR(64),\
      WEIGHT INT UNSIGNED DEFAULT 1,\
      LAST_PLAY_DATETIME DATETIME,\
      PLAY_COUNTER INT UNSIGNED DEFAULT 0,\
      CODING_FORMAT INT UNSIGNED,\
      SAMPLE_RATE INT UNSIGNED,\
      BIT_RATE INT UNSIGNED,\
      CHANNELS INT UNSIGNED,\
      PLAY_GAIN INT DEFAULT 0,\
      START_POINT INT DEFAULT -1,\
      END_POINT INT DEFAULT -1,\
      FADEUP_POINT INT DEFAULT -1,\
      FADEDOWN_POINT INT DEFAULT -1,\
      SEGUE_START_POINT INT DEFAULT -1,\
      SEGUE_END_POINT INT DEFAULT -1,\
      HOOK_START_POINT INT DEFAULT -1,\
      HOOK_END_POINT INT DEFAULT -1,\
      TALK_START_POINT INT DEFAULT -1,\
      TALK_END_POINT INT DEFAULT -1,\
      INDEX CART_NUMBER_IDX (CART_NUMBER),\
      INDEX DESCRIPTION_IDX (DESCRIPTION),\
      INDEX OUTCUE_IDX (OUTCUE))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create SERVICES table
//
  sql=QString("CREATE TABLE IF NOT EXISTS SERVICES (\
      NAME CHAR(10) NOT NULL PRIMARY KEY,\
      DESCRIPTION CHAR(255),\
      NAME_TEMPLATE char(255),\
      DESCRIPTION_TEMPLATE char(255),\
      PROGRAM_CODE char(255),\
      CHAIN_LOG enum('N','Y') default 'N',\
      TRACK_GROUP char(10),\
      AUTOSPOT_GROUP char(10),\
      AUTO_REFRESH enum('N','Y') default 'N',\
      DEFAULT_LOG_SHELFLIFE int default -1,\
      ELR_SHELFLIFE int default -1,\
      TFC_PATH char(255),\
      TFC_PREIMPORT_CMD text,\
      TFC_WIN_PATH char(255),\
      TFC_WIN_PREIMPORT_CMD text,\
      TFC_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\",\
      TFC_LABEL_CART char(32),\
      TFC_TRACK_CART char(32),\
      TFC_BREAK_STRING char(64),\
      TFC_TRACK_STRING char(64),\
      TFC_CART_OFFSET int,\
      TFC_CART_LENGTH int,\
      TFC_TITLE_OFFSET int,\
      TFC_TITLE_LENGTH int,\
      TFC_START_OFFSET int,\
      TFC_START_LENGTH int,\
      TFC_HOURS_OFFSET int,\
      TFC_HOURS_LENGTH int,\
      TFC_MINUTES_OFFSET int,\
      TFC_MINUTES_LENGTH int,\
      TFC_SECONDS_OFFSET int,\
      TFC_SECONDS_LENGTH int,\
      TFC_LEN_HOURS_OFFSET int,\
      TFC_LEN_HOURS_LENGTH int,\
      TFC_LEN_MINUTES_OFFSET int,\
      TFC_LEN_MINUTES_LENGTH int,\
      TFC_LEN_SECONDS_OFFSET int,\
      TFC_LEN_SECONDS_LENGTH int,\
      TFC_LENGTH_OFFSET int,\
      TFC_LENGTH_LENGTH int,\
      TFC_DATA_OFFSET int,\
      TFC_DATA_LENGTH int,\
      TFC_EVENT_ID_OFFSET int,\
      TFC_EVENT_ID_LENGTH int,\
      TFC_ANNC_TYPE_OFFSET int,\
      TFC_ANNC_TYPE_LENGTH int,\
      MUS_PATH char(255),\
      MUS_PREIMPORT_CMD text,\
      MUS_WIN_PATH char(255),\
      MUS_WIN_PREIMPORT_CMD text,\
      MUS_IMPORT_TEMPLATE char(64) default \"Rivendell Standard Import\",\
      MUS_LABEL_CART char(32),\
      MUS_TRACK_CART char(32),\
      MUS_BREAK_STRING char(64),\
      MUS_TRACK_STRING char(64),\
      MUS_CART_OFFSET int,\
      MUS_CART_LENGTH int,\
      MUS_TITLE_OFFSET int,\
      MUS_TITLE_LENGTH int,\
      MUS_START_OFFSET int,\
      MUS_START_LENGTH int,\
      MUS_HOURS_OFFSET int,\
      MUS_HOURS_LENGTH int,\
      MUS_MINUTES_OFFSET int,\
      MUS_MINUTES_LENGTH int,\
      MUS_SECONDS_OFFSET int,\
      MUS_SECONDS_LENGTH int,\
      MUS_LEN_HOURS_OFFSET int,\
      MUS_LEN_HOURS_LENGTH int,\
      MUS_LEN_MINUTES_OFFSET int,\
      MUS_LEN_MINUTES_LENGTH int,\
      MUS_LEN_SECONDS_OFFSET int,\
      MUS_LEN_SECONDS_LENGTH int,\
      MUS_LENGTH_OFFSET int,\
      MUS_LENGTH_LENGTH int,\
      MUS_DATA_OFFSET int,\
      MUS_DATA_LENGTH int,\
      MUS_EVENT_ID_OFFSET int,\
      MUS_EVENT_ID_LENGTH int,\
      MUS_ANNC_TYPE_OFFSET int,\
      MUS_ANNC_TYPE_LENGTH int, ");

  for(int i=0;i<167;i++) {
    sql+=QString().sprintf("CLOCK%d char(64),",i);
  }
  sql+=QString("CLOCK167 char(64))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create GROUPS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS GROUPS (\
      NAME CHAR(10) NOT NULL PRIMARY KEY,\
      DESCRIPTION CHAR(255),\
      DEFAULT_CART_TYPE int unsigned default 1,\
      DEFAULT_LOW_CART int unsigned default 0,\
      DEFAULT_HIGH_CART int unsigned default 0,\
      CUT_SHELFLIFE int default -1,\
      DELETE_EMPTY_CARTS enum('N','Y') default 'N',\
      DEFAULT_TITLE char(255) default \"Imported from %f.%e\",\
      ENFORCE_CART_RANGE enum('N','Y') default 'N',\
      REPORT_TFC enum('N','Y') default 'Y',\
      REPORT_MUS enum('N','Y') default 'Y',\
      ENABLE_NOW_NEXT enum('N','Y') default 'N',\
      COLOR char(7),\
      index IDX_REPORT_TFC (REPORT_TFC),\
      index IDX_REPORT_MUS (REPORT_MUS))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create AUDIO_PERMS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS AUDIO_PERMS (\
      ID INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
      GROUP_NAME CHAR(10),\
      SERVICE_NAME CHAR(10),\
      INDEX GROUP_IDX (GROUP_NAME),\
      INDEX SERVICE_IDX (SERVICE_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create RDLIBRARY table
//
  sql=QString("CREATE TABLE IF NOT EXISTS RDLIBRARY (\
      ID INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,\
      STATION CHAR(40) NOT NULL,\
      INSTANCE INT UNSIGNED NOT NULL,\
      INPUT_CARD INT DEFAULT 0,\
      INPUT_STREAM INT DEFAULT 0,\
      INPUT_PORT INT DEFAULT 0,\
      INPUT_TYPE ENUM('A','D') DEFAULT 'A',\
      OUTPUT_CARD INT DEFAULT 0,\
      OUTPUT_STREAM INT DEFAULT 0,\
      OUTPUT_PORT INT DEFAULT 0,\
      VOX_THRESHOLD INT DEFAULT -5000,\
      TRIM_THRESHOLD INT DEFAULT 0,\
      RECORD_GPI INT DEFAULT -1,\
      PLAY_GPI INT DEFAULT -1,\
      STOP_GPI INT DEFAULT -1,\
      DEFAULT_FORMAT INT UNSIGNED DEFAULT 0,\
      DEFAULT_CHANNELS INT UNSIGNED DEFAULT 2,\
      DEFAULT_SAMPRATE INT UNSIGNED DEFAULT 44100,\
      DEFAULT_LAYER INT UNSIGNED DEFAULT 0,\
      DEFAULT_BITRATE INT UNSIGNED DEFAULT 0,\
      DEFAULT_RECORD_MODE INT UNSIGNED DEFAULT 0,\
      DEFAULT_TRIM_STATE ENUM('N','Y') DEFAULT 'N',\
      MAXLENGTH INT,\
      TAIL_PREROLL INT UNSIGNED DEFAULT 1500,\
      RIPPER_DEVICE CHAR(64) DEFAULT \"/dev/cdrom\",\
      PARANOIA_LEVEL INT DEFAULT 0,\
      RIPPER_LEVEL INT DEFAULT -1300,\
      CDDB_SERVER CHAR(64) DEFAULT \"freedb.freedb.org\",\
      ENABLE_EDITOR enum('N','Y') default 'N',\
      SRC_CONVERTER int default 1,\
      LIMIT_SEARCH int default 1,\
      SEARCH_LIMITED enum('N','Y') default 'Y',\
      INDEX STATION_IDX (STATION,INSTANCE))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create TRIGGERS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS TRIGGERS (\
      ID INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
      CUT_NAME CHAR(12),\
      TRIGGER_CODE INT UNSIGNED,\
      OFFSET INT UNSIGNED,\
      INDEX CUT_NAME_IDX (CUT_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create TTYS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS TTYS (\
      ID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,\
      PORT_ID INT UNSIGNED NOT NULL,\
      ACTIVE ENUM('N','Y') NOT NULL DEFAULT 'N',\
      STATION_NAME CHAR(64) NOT NULL,\
      PORT CHAR(20),\
      BAUD_RATE INT DEFAULT 9600,\
      DATA_BITS INT DEFAULT 8,\
      STOP_BITS INT DEFAULT 1,\
      PARITY INT DEFAULT 0,\
      TERMINATION INT DEFAULT 0,\
      INDEX STATION_NAME_IDX (STATION_NAME),\
      INDEX ACTIVE_IDX (ACTIVE),\
      INDEX PORT_ID_IDX (PORT_ID))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create DECKS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS DECKS (\
      ID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,\
      STATION_NAME CHAR(64) NOT NULL,\
      CHANNEL INT UNSIGNED NOT NULL,\
      CARD_NUMBER INT DEFAULT -1,\
      STREAM_NUMBER INT DEFAULT -1,\
      PORT_NUMBER INT DEFAULT -1,\
      MON_PORT_NUMBER int default -1,\
      DEFAULT_MONITOR_ON enum('N','Y') default 'N',\
      PORT_TYPE ENUM('A','D') DEFAULT 'A',\
      DEFAULT_FORMAT INT DEFAULT 0,\
      DEFAULT_CHANNELS INT DEFAULT 2,\
      DEFAULT_SAMPRATE INT DEFAULT 44100,\
      DEFAULT_BITRATE INT DEFAULT 0,\
      DEFAULT_THRESHOLD INT DEFAULT 0,\
      SWITCH_STATION char(64),\
      SWITCH_MATRIX int default -1,\
      SWITCH_OUTPUT int default -1,\
      SWITCH_DELAY int default 0,\
      INDEX STATION_NAME_IDX (STATION_NAME),\
      INDEX CHANNEL_IDX (CHANNEL))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create RECORDINGS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS RECORDINGS (\
      ID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,\
      IS_ACTIVE enum('N','Y') default 'Y',\
      STATION_NAME CHAR(64) NOT NULL,\
      TYPE int default 0,\
      CHANNEL INT UNSIGNED NOT NULL,\
      CUT_NAME CHAR(12) NOT NULL,\
      SUN ENUM('N','Y') DEFAULT 'N',\
      MON ENUM('N','Y') DEFAULT 'N',\
      TUE ENUM('N','Y') DEFAULT 'N',\
      WED ENUM('N','Y') DEFAULT 'N',\
      THU ENUM('N','Y') DEFAULT 'N',\
      FRI ENUM('N','Y') DEFAULT 'N',\
      SAT ENUM('N','Y') DEFAULT 'N',\
      DESCRIPTION CHAR(64),\
      START_TYPE int unsigned default 0,\
      START_TIME time,\
      START_LENGTH int default 0,\
      START_MATRIX int default -1,\
      START_LINE int default -1,\
      START_OFFSET int default 0,\
      END_TYPE int default 0,\
      END_TIME time,\
      END_LENGTH int default 0,\
      END_MATRIX int default -1,\
      END_LINE int default -1,\
      LENGTH INT UNSIGNED,\
      START_GPI INT DEFAULT -1,\
      END_GPI INT DEFAULT -1,\
      ALLOW_MULT_RECS enum('N','Y') default 'N',\
      MAX_GPI_REC_LENGTH int unsigned default 3600000,\
      TRIM_THRESHOLD int,\
      NORMALIZE_LEVEL int default -1300,\
      STARTDATE_OFFSET INT UNSIGNED DEFAULT 0,\
      ENDDATE_OFFSET INT UNSIGNED DEFAULT 0,\
      EVENTDATE_OFFSET int default 0,\
      FORMAT INT DEFAULT 0,\
      CHANNELS INT DEFAULT 2,\
      SAMPRATE INT DEFAULT 44100,\
      BITRATE INT DEFAULT 0,\
      QUALITY int default 0,\
      MACRO_CART int default -1,\
      SWITCH_INPUT int default -1,\
      SWITCH_OUTPUT int default -1,\
      EXIT_CODE int default 0,\
      EXIT_TEXT text,\
      ONE_SHOT enum('N','Y') default 'N',\
      URL char(255),\
      URL_USERNAME char(64),\
      URL_PASSWORD char(64),\
      ENABLE_METADATA enum('N','Y') default 'N',\
      FEED_ID int default -1,\
      INDEX STATION_NAME_IDX (STATION_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create AUDIO_PORTS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS AUDIO_PORTS (\
      ID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,\
      STATION_NAME CHAR(64) NOT NULL,\
      CARD_NUMBER INT NOT NULL,\
      CLOCK_SOURCE INT DEFAULT 0,\
      INPUT_0_LEVEL INT DEFAULT 0,\
      INPUT_0_TYPE INT DEFAULT 0,\
      INPUT_0_MODE INT DEFAULT 0,\
      INPUT_1_LEVEL INT DEFAULT 0,\
      INPUT_1_TYPE INT DEFAULT 0,\
      INPUT_1_MODE INT DEFAULT 0,\
      INPUT_2_LEVEL INT DEFAULT 0,\
      INPUT_2_TYPE INT DEFAULT 0,\
      INPUT_2_MODE INT DEFAULT 0,\
      INPUT_3_LEVEL INT DEFAULT 0,\
      INPUT_3_TYPE INT DEFAULT 0,\
      INPUT_3_MODE INT DEFAULT 0,\
      INPUT_4_LEVEL INT DEFAULT 0,\
      INPUT_4_TYPE INT DEFAULT 0,\
      INPUT_4_MODE INT DEFAULT 0,\
      INPUT_5_LEVEL INT DEFAULT 0,\
      INPUT_5_TYPE INT DEFAULT 0,\
      INPUT_5_MODE INT DEFAULT 0,\
      INPUT_6_LEVEL INT DEFAULT 0,\
      INPUT_6_TYPE INT DEFAULT 0,\
      INPUT_6_MODE INT DEFAULT 0,\
      INPUT_7_LEVEL INT DEFAULT 0,\
      INPUT_7_TYPE INT DEFAULT 0,\
      INPUT_7_MODE INT DEFAULT 0,\
      OUTPUT_0_LEVEL INT DEFAULT 0,\
      OUTPUT_1_LEVEL INT DEFAULT 0,\
      OUTPUT_2_LEVEL INT DEFAULT 0,\
      OUTPUT_3_LEVEL INT DEFAULT 0,\
      OUTPUT_4_LEVEL INT DEFAULT 0,\
      OUTPUT_5_LEVEL INT DEFAULT 0,\
      OUTPUT_6_LEVEL INT DEFAULT 0,\
      OUTPUT_7_LEVEL INT DEFAULT 0,\
      INDEX STATION_NAME_IDX (STATION_NAME),\
      INDEX CARD_NUMBER_IDX (CARD_NUMBER))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create LOGS table
//
  sql=QString("CREATE TABLE IF NOT EXISTS LOGS (\
      NAME CHAR(64) NOT NULL PRIMARY KEY,\
      LOG_EXISTS enum('N','Y') default 'Y',\
      TYPE int not null default 0,\
      SERVICE CHAR(10) NOT NULL,\
      DESCRIPTION CHAR(64),\
      ORIGIN_USER CHAR(255) NOT NULL,\
      ORIGIN_DATETIME DATETIME NOT NULL,\
      LINK_DATETIME DATETIME NOT NULL,\
      MODIFIED_DATETIME DATETIME NOT NULL,\
      AUTO_REFRESH enum('N','Y') default 'N',\
      START_DATE DATE NOT NULL,\
      END_DATE DATE NOT NULL,\
      PURGE_DATE date,\
      IMPORT_DATE date,\
      SCHEDULED_TRACKS int unsigned default 0,\
      COMPLETED_TRACKS int unsigned default 0,\
      MUSIC_LINKS int default 0,\
      MUSIC_LINKED enum('N','Y') default 'N',\
      TRAFFIC_LINKS int default 0,\
      TRAFFIC_LINKED enum('N','Y') default 'N',\
      NEXT_ID int default 0,\
      index NAME_IDX (NAME,LOG_EXISTS),\
      INDEX SERVICE_IDX (SERVICE),\
      INDEX DESCRIPTION_IDX (DESCRIPTION),\
      INDEX ORIGIN_USER_IDX (ORIGIN_USER),\
      INDEX START_DATE_IDX (START_DATE),\
      INDEX END_DATE_IDX (END_DATE),\
      index TYPE_IDX(TYPE,LOG_EXISTS))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create VERSION table
//
  sql="create table if not exists VERSION (\
       DB INT NOT NULL PRIMARY KEY,\
       LAST_MAINT_DATETIME datetime default \"1970-01-01 00:00:00\",\
       LAST_ISCI_XREFERENCE datetime default \"1970-01-01 00:00:00\")";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDAIRPLAY Table
  //
  sql="create table if not exists RDAIRPLAY (\
      ID int not null primary key auto_increment,\
      STATION char(40) not null,\
      INSTANCE int unsigned not null,\
      CARD0 int default 0,\
      PORT0 int default 0,\
      START_RML0 char(255),\
      STOP_RML0 char(255),\
      CARD1 int default 0,\
      PORT1 int default 0,\
      START_RML1 char(255),\
      STOP_RML1 char(255),\
      CARD2 int default 0,\
      PORT2 int default 0,\
      START_RML2 char(255),\
      STOP_RML2 char(255),\
      CARD3 int default 0,\
      PORT3 int default 0,\
      START_RML3 char(255),\
      STOP_RML3 char(255),\
      CARD4 int default 0,\
      PORT4 int default 0,\
      START_RML4 char(255),\
      STOP_RML4 char(255),\
      CARD5 int default 0,\
      PORT5 int default 0,\
      START_RML5 char(255),\
      STOP_RML5 char(255),\
      CARD6 int default 0,\
      PORT6 int default 0,\
      START_RML6 char(255),\
      STOP_RML6 char(255),\
      CARD7 int default 0,\
      PORT7 int default 0,\
      START_RML7 char(255),\
      STOP_RML7 char(255),\
      CARD8 int default 0,\
      PORT8 int default 0,\
      START_RML8 char(255),\
      STOP_RML8 char(255),\
      CARD9 int default 0,\
      PORT9 int default 0,\
      START_RML9 char(255),\
      STOP_RML9 char(255),\
      SEGUE_LENGTH int default 250,\
      TRANS_LENGTH int default 50,\
      OP_MODE int default 2,\
      START_MODE int default 0,\
      LOG_MODE_STYLE int default 0,\
      PIE_COUNT_LENGTH int default 15000,\
      PIE_COUNT_ENDPOINT int default 0,\
      CHECK_TIMESYNC enum('N','Y') default 'N',\
      STATION_PANELS int default 3,\
      USER_PANELS int default 3,\
      SHOW_AUX_1 enum('N','Y') default 'Y',\
      SHOW_AUX_2 enum('N','Y') default 'Y',\
      CLEAR_FILTER enum('N','Y') default 'N',\
      DEFAULT_TRANS_TYPE int default 0,\
      BAR_ACTION int unsigned default 0,\
      FLASH_PANEL enum('N','Y') default 'N',\
      PANEL_PAUSE_ENABLED enum('N','Y') default 'N',\
      BUTTON_LABEL_TEMPLATE char(32) default \"%t\",\
      PAUSE_ENABLED enum('N','Y'),\
      DEFAULT_SERVICE char(10),\
      HOUR_SELECTOR_ENABLED enum('N','Y') default 'N',\
      TITLE_TEMPLATE char(64) default '%t',\
      ARTIST_TEMPLATE char(64) default '%a',\
      OUTCUE_TEMPLATE char(64) default '%o',\
      DESCRIPTION_TEMPLATE char(64) default '%i',\
      UDP_ADDR0 char(255),\
      UDP_PORT0 int unsigned,\
      UDP_STRING0 char(255),\
      LOG_RML0 char(255),\
      UDP_ADDR1 char(255),\
      UDP_PORT1 int unsigned,\
      UDP_STRING1 char(255),\
      LOG_RML1 char(255),\
      UDP_ADDR2 char(255),\
      UDP_PORT2 int unsigned,\
      UDP_STRING2 char(255),\
      LOG_RML2 char(255),\
      EXIT_CODE int default 0,\
      EXIT_PASSWORD char(41) default \"\",\
      SKIN_PATH char(255) default \"";
  sql+=RD_DEFAULT_RDAIRPLAY_SKIN;
  sql+="\",\
      SHOW_COUNTERS enum('N','Y') default 'N',\
      AUDITION_PREROLL int default 10000,\
      LOG0_START_MODE int default 0,\
      LOG0_AUTO_RESTART enum('N','Y') default 'N',\
      LOG0_LOG_NAME char(64),\
      LOG0_CURRENT_LOG char(64),\
      LOG0_RUNNING enum('N','Y') default 'N',\
      LOG0_LOG_ID int default -1,\
      LOG0_LOG_LINE int default -1,\
      LOG0_NOW_CART int unsigned default 0,\
      LOG0_NEXT_CART int unsigned default 0,\
      LOG1_START_MODE int default 0,\
      LOG1_AUTO_RESTART enum('N','Y') default 'N',\
      LOG1_LOG_NAME char(64),\
      LOG1_CURRENT_LOG char(64),\
      LOG1_RUNNING enum('N','Y') default 'N',\
      LOG1_LOG_ID int default -1,\
      LOG1_LOG_LINE int default -1,\
      LOG1_NOW_CART int unsigned default 0,\
      LOG1_NEXT_CART int unsigned default 0,\
      LOG2_START_MODE int default 0,\
      LOG2_AUTO_RESTART enum('N','Y') default 'N',\
      LOG2_LOG_NAME char(64),\
      LOG2_CURRENT_LOG char(64),\
      LOG2_RUNNING enum('N','Y') default 'N',\
      LOG2_LOG_ID int default -1,\
      LOG2_LOG_LINE int default -1,\
      LOG2_NOW_CART int unsigned default 0,\
      LOG2_NEXT_CART int unsigned default 0,\
      index STATION_IDX (STATION,INSTANCE))"; 
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create PANELS Table
  //
  sql="create table if not exists PANELS (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      ROW_NO int not null,\
      COLUMN_NO int not null,\
      LABEL char(64),\
      CART int,\
      DEFAULT_COLOR char(7),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO),\
      index SAVE_IDX (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO))"; 
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create MATRICES Table
  //
  sql="create table if not exists MATRICES (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      NAME char(64),\
      MATRIX int not null,\
      TYPE int not null,\
      LAYER int default 86,\
      PORT_TYPE int default 0,\
      PORT_TYPE_2 int default 0,\
      CARD int default -1,\
      PORT int not null,\
      PORT_2 int not null,\
      IP_ADDRESS char(16),\
      IP_ADDRESS_2 char(16),\
      IP_PORT int,\
      IP_PORT_2 int,\
      USERNAME char(32),\
      USERNAME_2 char(32),\
      PASSWORD char(32),\
      PASSWORD_2 char(32),\
      START_CART int unsigned,\
      STOP_CART int unsigned,\
      START_CART_2 int unsigned,\
      STOP_CART_2 int unsigned,\
      GPIO_DEVICE char(255),\
      INPUTS int not null,\
      OUTPUTS int not null,\
      GPIS int not null,\
      GPOS int not null,\
      FADERS int default 0,\
      DISPLAYS int default 0,\
      index MATRIX_IDX (STATION_NAME,MATRIX))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create INPUTS Table
  //
  sql="create table if not exists INPUTS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      MATRIX int not null,\
      NUMBER int not null,\
      NAME char(64),\
      FEED_NAME char(8),\
      CHANNEL_MODE int,\
      ENGINE_NUM int default -1,\
      DEVICE_NUM int default -1,\
      NODE_HOSTNAME char(255),\
      NODE_TCP_PORT int,\
      NODE_SLOT int,\
      index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER),\
      index NODE_IDX (STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create OUTPUTS Table
  //
  sql="create table if not exists OUTPUTS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      MATRIX int not null,\
      NUMBER int not null,\
      NAME char(64),\
      ENGINE_NUM int default -1,\
      DEVICE_NUM int default -1,\
      NODE_HOSTNAME char(255),\
      NODE_TCP_PORT int,\
      NODE_SLOT int,\
      index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER),\
      index NODE_IDX (STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create GPIS Table
  //
  sql="create table if not exists GPIS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      MATRIX int not null,\
      NUMBER int not null,\
      MACRO_CART int default 0,\
      OFF_MACRO_CART int default 0,\
      index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))";
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create EVENTS table
//
  sql=QString("create table if not exists EVENTS (\
      NAME char(64) not null primary key,\
      PROPERTIES char(64),\
      DISPLAY_TEXT char(64),\
      NOTE_TEXT char(255),\
      PREPOSITION int default -1,\
      TIME_TYPE int default 0,\
      GRACE_TIME int default 0,\
      POST_POINT enum('N','Y') default 'N',\
      USE_AUTOFILL enum('N','Y') default 'N',\
      AUTOFILL_SLOP int default -1,\
      USE_TIMESCALE enum('N','Y') default 'N',\
      IMPORT_SOURCE int default 0,\
      START_SLOP int default 0,\
      END_SLOP int default 0,\
      FIRST_TRANS_TYPE int default 0,\
      DEFAULT_TRANS_TYPE int default 0,\
      COLOR char(7),\
      SCHED_GROUP VARCHAR(10),\
      TITLE_SEP INT(10) UNSIGNED,\
      HAVE_CODE VARCHAR(10),\
      HOR_SEP INT(10) UNSIGNED,\
      HOR_DIST INT(10) UNSIGNED,\
      NESTED_EVENT char(64),\
      REMARKS char(255))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create CLOCKS table
//
  sql=QString("create table if not exists CLOCKS (\
      NAME char(64) not null primary key,\
      SHORT_NAME char(8),\
      ARTISTSEP int(10) unsigned,\
      COLOR char(7),\
      REMARKS char(255))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create AUTOFILLS table
//
  sql=QString("create table if not exists AUTOFILLS (\
      ID int not null primary key auto_increment,\
      SERVICE char(10),\
      CART_NUMBER int unsigned,\
      index SERVICE_IDX (SERVICE))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create HOSTVARS table
//
  sql=QString("create table if not exists HOSTVARS (\
      ID int not null primary key auto_increment,\
      STATION_NAME char(64) not null,\
      NAME char(32) not null,\
      VARVALUE char(255),\
      REMARK char(255),\
      index NAME_IDX (STATION_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SERVICE_PERMS Table
  //
  sql=QString("create table if not exists SERVICE_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      STATION_NAME char(64),\
      SERVICE_NAME char(10),\
      index STATION_IDX (STATION_NAME),\
      index SERVICE_IDX (SERVICE_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORTS Table
  //
  sql=QString("create table if not exists REPORTS (\
               ID int unsigned auto_increment not null primary key,\
               NAME char(64) not null unique,\
               DESCRIPTION char(64),\
               EXPORT_FILTER int,\
               EXPORT_PATH char(255),\
               WIN_EXPORT_PATH char(255),\
               EXPORT_TFC enum('N','Y') default 'N',\
               FORCE_TFC enum('N','Y') default 'N',\
               EXPORT_MUS enum('N','Y') default 'N',\
               FORCE_MUS enum('N','Y') default 'N',\
               EXPORT_GEN enum('N','Y') default 'N',\
               STATION_ID char(16),\
               CART_DIGITS int unsigned default 6,\
               USE_LEADING_ZEROS enum('N','Y') default 'N',\
               LINES_PER_PAGE int default 66,\
               SERVICE_NAME char(64),\
               STATION_TYPE int default 0,\
               STATION_FORMAT char(64),\
               FILTER_ONAIR_FLAG enum('N','Y') default 'N',\
               FILTER_GROUPS enum('N','Y') default 'N',\
               START_TIME time,\
               END_TIME time,\
               index IDX_NAME (NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORT_SERVICES Table
  //
  sql=QString("create table if not exists REPORT_SERVICES (\
               ID int unsigned auto_increment not null primary key,\
               REPORT_NAME char(64) not null,\
               SERVICE_NAME char(10),\
               index IDX_REPORT_NAME (REPORT_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORT_STATIONS Table
  //
  sql=QString("create table if not exists REPORT_STATIONS (\
               ID int unsigned auto_increment not null primary key,\
               REPORT_NAME char(64) not null,\
               STATION_NAME char(64),\
               index IDX_REPORT_NAME (REPORT_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPORT_GROUPS Table
  //
  sql=QString("create table if not exists REPORT_GROUPS (\
               ID int unsigned auto_increment not null primary key,\
               REPORT_NAME char(64) not null,\
               GROUP_NAME char(10),\
               index IDX_REPORT_NAME (REPORT_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create CLOCK_PERMS Table
  //
  sql=QString("create table if not exists CLOCK_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      CLOCK_NAME char(64),\
      SERVICE_NAME char(10),\
      index STATION_IDX (CLOCK_NAME),\
      index SERVICE_IDX (SERVICE_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create EVENT_PERMS Table
  //
  sql=QString("create table if not exists EVENT_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      EVENT_NAME char(64),\
      SERVICE_NAME char(10),\
      index STATION_IDX (EVENT_NAME),\
      index SERVICE_IDX (SERVICE_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create USER_PERMS table
  //
  sql=QString("create table if not exists USER_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      USER_NAME char(255),\
      GROUP_NAME char(10),\
      index USER_IDX (USER_NAME),\
      index GROUP_IDX (GROUP_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create VGUEST_RESOURCES Table
  //
  sql="create table if not exists VGUEST_RESOURCES (\
       ID int unsigned auto_increment not null primary key,\
       STATION_NAME char(64) not null,\
       MATRIX_NUM int not null,\
       VGUEST_TYPE int not null,\
       NUMBER int not null,\
       ENGINE_NUM int default -1,\
       DEVICE_NUM int default -1,\
       SURFACE_NUM int default 0,\
       RELAY_NUM int default -1,\
       BUSS_NUM int default -1,\
       index STATION_MATRIX_IDX (STATION_NAME,MATRIX_NUM,VGUEST_TYPE))";
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create RDLOGEDIT table
//
  sql=QString("create table if not exists RDLOGEDIT (\
      ID int unsigned primary key auto_increment,\
      STATION char(64) not null,\
      INPUT_CARD int default -1,\
      INPUT_PORT int default 0,\
      OUTPUT_CARD int default -1,\
      OUTPUT_PORT int default 0,\
      FORMAT int unsigned default 0,\
      SAMPRATE int unsigned default 44100,\
      LAYER int unsigned default 0,\
      BITRATE int unsigned default 0,\
      ENABLE_SECOND_START enum('N','Y') default 'Y',\
      DEFAULT_CHANNELS int unsigned default 2,\
      MAXLENGTH int default 0,\
      TAIL_PREROLL int unsigned default 2000,\
      START_CART int unsigned default 0,\
      END_CART int unsigned default 0,\
      REC_START_CART int unsigned default 0,\
      REC_END_CART int unsigned default 0,\
      TRIM_THRESHOLD int default -3000,\
      RIPPER_LEVEL int default -1300,\
      DEFAULT_TRANS_TYPE int default 0,\
      index STATION_IDX (STATION))");
  if(!RunQuery(sql)) {
    return false;
  }

//
// Create RDCATCH table
//
  sql="create table if not exists RDCATCH (\
       ID int unsigned primary key auto_increment,\
       STATION char(64) not null,\
       ERROR_RML char(255),\
       index STATION_IDX (STATION))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SCHED_CODES Table
  //
  sql="create table if not exists SCHED_CODES\
		  (CODE varchar(10) not null primary key,\
		  DESCRIPTION varchar(255))";
  if(!RunQuery(sql)) {
	  return false;
  }

  //
  // Create DROPBOXES Table
  //
  sql="create table if not exists DROPBOXES \
       (ID int auto_increment not null primary key,\
        STATION_NAME char(64),\
        GROUP_NAME char(10),\
        PATH char(255),\
        NORMALIZATION_LEVEL int default 1,\
        AUTOTRIM_LEVEL int default 1,\
        SINGLE_CART enum('N','Y') default 'N',\
        TO_CART int unsigned default 0,\
        USE_CARTCHUNK_ID enum('N','Y') default 'N',\
        TITLE_FROM_CARTCHUNK_ID enum('N','Y') default 'N',\
        DELETE_CUTS enum('N','Y') default 'N',\
        DELETE_SOURCE enum('N','Y') default 'Y',\
        METADATA_PATTERN char(64),\
        STARTDATE_OFFSET int default 0,\
        ENDDATE_OFFSET int default 0,\
        FIX_BROKEN_FORMATS enum('N','Y') default 'N',\
        LOG_PATH char(255),\
        IMPORT_CREATE_DATES enum('N','Y') default 'N',\
        CREATE_STARTDATE_OFFSET int default 0,\
        CREATE_ENDDATE_OFFSET int default 0,\
        SET_USER_DEFINED char(255),\
        index STATION_NAME_IDX (STATION_NAME))";
  if(!RunQuery(sql)) {
	  return false;
  }

  //
  // Create RDPANEL Table
  //
  sql="create table if not exists RDPANEL (\
      ID int not null primary key auto_increment,\
      STATION char(40) not null,\
      INSTANCE int unsigned not null,\
      CARD2 int default 0,\
      PORT2 int default 0,\
      START_RML2 char(255),\
      STOP_RML2 char(255),\
      CARD3 int default 0,\
      PORT3 int default 0,\
      START_RML3 char(255),\
      STOP_RML3 char(255),\
      CARD6 int default 0,\
      PORT6 int default 0,\
      START_RML6 char(255),\
      STOP_RML6 char(255),\
      CARD7 int default 0,\
      PORT7 int default 0,\
      START_RML7 char(255),\
      STOP_RML7 char(255),\
      CARD8 int default 0,\
      PORT8 int default 0,\
      START_RML8 char(255),\
      STOP_RML8 char(255),\
      CARD9 int default 0,\
      PORT9 int default 0,\
      START_RML9 char(255),\
      STOP_RML9 char(255),\
      STATION_PANELS int default 3,\
      USER_PANELS int default 3,\
      CLEAR_FILTER enum('N','Y') default 'N',\
      FLASH_PANEL enum('N','Y') default 'N',\
      PANEL_PAUSE_ENABLED enum('N','Y') default 'N',\
      BUTTON_LABEL_TEMPLATE char(32) default \"%t\",\
      DEFAULT_SERVICE char(10),\
      SKIN_PATH char(255) default \"";
  sql+=RD_DEFAULT_RDPANEL_SKIN;
  sql+="\",\
      index STATION_IDX (STATION,INSTANCE))"; 
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create EXTENDED_PANELS Table
  //
  sql="create table if not exists EXTENDED_PANELS (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      ROW_NO int not null,\
      COLUMN_NO int not null,\
      LABEL char(64),\
      CART int,\
      DEFAULT_COLOR char(7),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO),\
      index SAVE_IDX (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO))"; 
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create PANEL_NAMES Table
  //
  sql="create table if not exists PANEL_NAMES (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      NAME char(64),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO))"; 
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create EXTENDED_PANEL_NAMES Table
  //
  sql="create table if not exists EXTENDED_PANEL_NAMES (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      NAME char(64),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO))"; 
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create FEEDS Table
  //
    sql="create table if not exists FEEDS (\
         ID int unsigned auto_increment not null primary key,\
         KEY_NAME char(8) unique not null,\
         CHANNEL_TITLE char(255),\
         CHANNEL_DESCRIPTION text,\
         CHANNEL_CATEGORY char(64),\
         CHANNEL_LINK char(255),\
         CHANNEL_COPYRIGHT char(64),\
         CHANNEL_WEBMASTER char(64),\
         CHANNEL_LANGUAGE char(5) default \"en-us\",\
         BASE_URL char(255),\
         BASE_PREAMBLE char(255),\
         PURGE_URL char(255),\
         PURGE_USERNAME char(64),\
         PURGE_PASSWORD char(64),\
         HEADER_XML text,\
         CHANNEL_XML text,\
         ITEM_XML text,\
         CAST_ORDER enum('N','Y') default 'N',\
         MAX_SHELF_LIFE int,\
         LAST_BUILD_DATETIME datetime,\
         ORIGIN_DATETIME datetime,\
         ENABLE_AUTOPOST enum('N','Y') default 'N',\
         KEEP_METADATA enum('N','Y') default 'Y',\
         UPLOAD_FORMAT int default 2,\
         UPLOAD_CHANNELS int default 2,\
         UPLOAD_SAMPRATE int default 44100,\
         UPLOAD_BITRATE int default 32000,\
         UPLOAD_QUALITY int default 0,\
         UPLOAD_EXTENSION char(16) default \"mp3\",\
         NORMALIZE_LEVEL int default -100,\
         REDIRECT_PATH char(255),\
         MEDIA_LINK_MODE int default 0,\
         index KEY_NAME_IDX(KEY_NAME))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create PODCASTS Table
  //
  sql="create table if not exists PODCASTS (\
       ID int unsigned auto_increment not null primary key,\
       FEED_ID int unsigned not null,\
       STATUS int unsigned default 1,\
       ITEM_TITLE char(255),\
       ITEM_DESCRIPTION text,\
       ITEM_CATEGORY char(64),\
       ITEM_LINK char(255),\
       ITEM_COMMENTS char(255),\
       ITEM_AUTHOR char(255),\
       ITEM_SOURCE_TEXT char(64),\
       ITEM_SOURCE_URL char(255),\
       AUDIO_FILENAME char(255),\
       AUDIO_LENGTH int unsigned,\
       AUDIO_TIME int unsigned,\
       SHELF_LIFE int,\
       ORIGIN_DATETIME datetime,\
       EFFECTIVE_DATETIME datetime,\
       index FEED_ID_IDX(FEED_ID,ORIGIN_DATETIME))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create AUX_METADATA Table
  //
  sql="create table if not exists AUX_METADATA (\
       ID int unsigned auto_increment not null primary key,\
       FEED_ID int unsigned,\
       VAR_NAME char(16),\
       CAPTION char(64),\
       index FEED_ID_IDX(FEED_ID))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create FEED_PERMS table
  //
  sql=QString("create table if not exists FEED_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      USER_NAME char(255),\
      KEY_NAME char(8),\
      index USER_IDX (USER_NAME),\
      index KEYNAME_IDX (KEY_NAME))");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create WEB_CONNECTIONS Table
  //
  sql=QString("create table if not exists WEB_CONNECTIONS (\
      SESSION_ID int unsigned not null primary key,\
      LOGIN_NAME char(255),\
      IP_ADDRESS char(16),\
      TIME_STAMP datetime)");
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SWITCHER_NODES Table
  //
  sql="create table if not exists SWITCHER_NODES (\
       ID int not null auto_increment primary key,\
       STATION_NAME char(64),\
       MATRIX int,\
       BASE_OUTPUT int default 0,\
       HOSTNAME char(64),\
       PASSWORD char(64),\
       TCP_PORT int,\
       DESCRIPTION char(255),\
       index STATION_IDX (STATION_NAME,MATRIX))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create ENCODERS Table
  //
  sql="create table if not exists ENCODERS (\
       ID int not null auto_increment primary key,\
       NAME char(32) not null,\
       STATION_NAME char(64),\
       COMMAND_LINE char(255),\
       DEFAULT_EXTENSION char(16),\
       index NAME_IDX(NAME,STATION_NAME))";
  if(!RunQuery(sql)) {
    return false;
  }
  // Ensure that dynamic format IDs start after 100
  sql="insert into ENCODERS set ID=100,NAME=\"dummy\"";
  if(!RunQuery(sql)) {
    return false;
  }
  sql="delete from ENCODERS where ID=100";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create ENCODER_BITRATES Table
  //
  sql="create table if not exists ENCODER_BITRATES (\
       ID int not null auto_increment primary key,\
       ENCODER_ID int not null,\
       BITRATES int not null,\
       index ENCODER_ID_IDX(ENCODER_ID))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create ENCODER_CHANNELS Table
  //
  sql="create table if not exists ENCODER_CHANNELS (\
       ID int not null auto_increment primary key,\
       ENCODER_ID int not null,\
       CHANNELS int not null,\
       index ENCODER_ID_IDX(ENCODER_ID))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create ENCODER_SAMPLERATES Table
  //
  sql="create table if not exists ENCODER_SAMPLERATES (\
       ID int not null auto_increment primary key,\
       ENCODER_ID int not null,\
       SAMPLERATES int not null,\
       index ENCODER_ID_IDX(ENCODER_ID))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create GPOS Table
  //
  sql="create table if not exists GPOS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      MATRIX int not null,\
      NUMBER int not null,\
      MACRO_CART int default 0,\
      OFF_MACRO_CART int default 0,\
      index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // DROPBOX_PATHS Table
  //
  sql="create table if not exists DROPBOX_PATHS (\
      ID int auto_increment not null primary key,\
      DROPBOX_ID int not null,\
      FILE_PATH char(255) not null,\
      FILE_DATETIME datetime,\
      index FILE_PATH_IDX (DROPBOX_ID,FILE_PATH))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create NOWNEXT_PLUGINS Table
  //
  sql="create table if not exists NOWNEXT_PLUGINS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      LOG_MACHINE int unsigned not null default 0,\
      PLUGIN_PATH char(255),\
      PLUGIN_ARG char(255),\
      index STATION_IDX (STATION_NAME,LOG_MACHINE))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create SYSTEM Table
  //
  sql=QString().sprintf("create table if not exists SYSTEM (\
                         ID int auto_increment not null primary key,\
                         SAMPLE_RATE int unsigned default %d,\
                         DUP_CART_TITLES enum('N','Y') not null default 'Y',\
                         MAX_POST_LENGTH int unsigned default %u,\
                         ISCI_XREFERENCE_PATH char(255),\
                         TEMP_CART_GROUP char(10))",
			RD_DEFAULT_SAMPLE_RATE,
			RD_DEFAULT_MAX_POST_LENGTH);
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create IMPORT_TEMPLATES table
  //
  sql="create table if not exists IMPORT_TEMPLATES (\
       NAME char(64) not null primary key,\
       CART_OFFSET int,\
       CART_LENGTH int,\
       TITLE_OFFSET int,\
       TITLE_LENGTH int,\
       HOURS_OFFSET int,\
       HOURS_LENGTH int,\
       MINUTES_OFFSET int,\
       MINUTES_LENGTH int,\
       SECONDS_OFFSET int,\
       SECONDS_LENGTH int,\
       LEN_HOURS_OFFSET int,\
       LEN_HOURS_LENGTH int,\
       LEN_MINUTES_OFFSET int,\
       LEN_MINUTES_LENGTH int,\
       LEN_SECONDS_OFFSET int,\
       LEN_SECONDS_LENGTH int,\
       LENGTH_OFFSET int,\
       LENGTH_LENGTH int,\
       DATA_OFFSET int,\
       DATA_LENGTH int,\
       EVENT_ID_OFFSET int,\
       EVENT_ID_LENGTH int,\
       ANNC_TYPE_OFFSET int,\
       ANNC_TYPE_LENGTH int)";
  if(!RunQuery(sql)) {
    return false;
  }
  UpdateImportFormats();

  //
  // Create REPLICATORS Table
  //
  sql=QString().sprintf("create table if not exists REPLICATORS (\
                         NAME char(32) not null primary key,\
                         DESCRIPTION char(64),\
                         TYPE_ID int unsigned not null,\
                         STATION_NAME char(64),\
                         FORMAT int unsigned default 0,\
                         CHANNELS int unsigned default 2,\
                         SAMPRATE int unsigned default %u,\
                         BITRATE int unsigned default 0,\
                         QUALITY int unsigned default 0,\
                         URL char(255),\
                         URL_USERNAME char(64),\
                         URL_PASSWORD char(64),\
                         ENABLE_METADATA enum('N','Y') default 'N',\
                         NORMALIZATION_LEVEL int default 0,\
                         index TYPE_ID_IDX (TYPE_ID))",
			RD_DEFAULT_SAMPLE_RATE);
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPLICATOR_MAP Table
  //
    sql="create table if not exists REPLICATOR_MAP (\
        ID int unsigned not null auto_increment primary key,\
        REPLICATOR_NAME char(32) not null,\
        GROUP_NAME char(10) not null,\
        index REPLICATOR_NAME_IDX(REPLICATOR_NAME),\
        index GROUP_NAME_IDX(GROUP_NAME))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPL_CART_STATE Table
  //
  sql="create table if not exists REPL_CART_STATE (\
       ID int unsigned not null auto_increment primary key,	\
       REPLICATOR_NAME char(32) not null,\
       CART_NUMBER int unsigned not null,\
       POSTED_FILENAME char(255),\
       ITEM_DATETIME datetime not null,\
       REPOST enum('N','Y') default 'N',\
       unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CART_NUMBER,POSTED_FILENAME))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create REPL_CUT_STATE Table
  //
  sql="create table if not exists REPL_CUT_STATE (\
       ID int unsigned not null auto_increment primary key,\
       REPLICATOR_NAME char(32) not null,\
       CUT_NAME char(12) not null,\
       ITEM_DATETIME datetime not null,\
       unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CUT_NAME))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create ISCI_XREFERENCE Table
  //
    sql="create table if not exists ISCI_XREFERENCE (\
        ID int unsigned not null auto_increment primary key,\
        CART_NUMBER int unsigned not null,\
        ISCI char(32) not null,\
        FILENAME char(64) not null,\
        LATEST_DATE date not null,\
        TYPE char(1) not null,\
        ADVERTISER_NAME char(30),\
        PRODUCT_NAME char(35),\
        CREATIVE_TITLE char(30),\
        REGION_NAME char(80),\
        index CART_NUMBER_IDX(CART_NUMBER),\
        index TYPE_IDX(TYPE,LATEST_DATE),\
        index LATEST_DATE_IDX(LATEST_DATE))";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create RDHOTKEYS table
  //
  sql=QString("CREATE TABLE IF NOT EXISTS RDHOTKEYS (\
      ID int unsigned not null auto_increment primary key,	\
      STATION_NAME         CHAR(64),   \
      MODULE_NAME          CHAR(64),   \
      KEY_ID               int,        \
      KEY_VALUE            CHAR(64),   \
      KEY_LABEL            CHAR(64)) ");
  if(!RunQuery(sql)) {
     return false;
  }

  //
  // Create JACK_CLIENTS Table
  //
  sql=QString("create table if not exists JACK_CLIENTS (\
               ID int unsigned auto_increment not null primary key,	\
               STATION_NAME char(64) not null,\
               DESCRIPTION char(64),\
               COMMAND_LINE char(255) not null,\
               index IDX_STATION_NAME (STATION_NAME))");
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
    "index STATION_NAME_IDX(STATION_NAME,SLOT_NUMBER))";
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
    "index STATION_NAME_IDX(STATION_NAME,MATRIX))";
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
    "index STATION_NAME_IDX(STATION_NAME,INSTANCE))";
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
    "index STATION_NAME_IDX(STATION_NAME,INSTANCE))";
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
    "index STATION_NAME_IDX(STATION_NAME,MACHINE))";
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
      "index SCHED_CODE_IDX(SCHED_CODE))";
  if(!RunQuery(sql)) {
     return false;
  }
  
  return true;
}


bool InitDb(QString name,QString pwd,QString station_name)
{
  QString sql;

  //
  // Create Default Admin Account
  //
  sql=QString().sprintf("insert into USERS \
     (LOGIN_NAME,PASSWORD,FULL_NAME,DESCRIPTION,ADMIN_USERS_PRIV,\
      ADMIN_CONFIG_PRIV)\
     values (\"%s\",PASSWORD(\"%s\"),\"%s\",\"%s\",\"Y\",\"Y\")",
			RDA_LOGIN_NAME,
			RDA_PASSWORD,
			RDA_FULLNAME,
			RDA_DESCRIPTION);
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default User Account
  //
  sql=QString().sprintf("insert into USERS (LOGIN_NAME,PASSWORD,FULL_NAME,\
      DESCRIPTION,CREATE_CARTS_PRIV,DELETE_CARTS_PRIV,MODIFY_CARTS_PRIV,\
      EDIT_AUDIO_PRIV,ASSIGN_CART_PRIV,CREATE_LOG_PRIV,DELETE_LOG_PRIV,\
      DELETE_REC_PRIV,PLAYOUT_LOG_PRIV,ARRANGE_LOG_PRIV,ADDTO_LOG_PRIV,\
      REMOVEFROM_LOG_PRIV,CONFIG_PANELS_PRIV,VOICETRACK_LOG_PRIV,\
      EDIT_CATCHES_PRIV,MODIFY_TEMPLATE_PRIV,\
      ADD_PODCAST_PRIV,EDIT_PODCAST_PRIV,DELETE_PODCAST_PRIV)\
      values (\"%s\",PASSWORD(\"%s\"),\"%s\",\"%s\",\
      'Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y')",
			RD_USER_LOGIN_NAME,
			RD_USER_PASSWORD,
			RD_USER_FULL_NAME,
			RD_USER_DESCRIPTION);
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
  struct hostent *hostent=gethostbyname((const char *)station_name);
  if(hostent==NULL) {
    sql=QString().sprintf("insert into STATIONS \
     (NAME,DESCRIPTION,USER_NAME,DEFAULT_NAME) \
     VALUES (\"%s\",\"%s\",\"%s\",\"%s\")",
			  (const char *)RDEscapeString(station_name),
			  RD_STATION_DESCRIPTION,
			  RD_USER_LOGIN_NAME,
			  RD_USER_LOGIN_NAME);
  }
  else {
    sql=QString().sprintf("insert into STATIONS \
     (NAME,DESCRIPTION,USER_NAME,DEFAULT_NAME,IPV4_ADDRESS) \
     VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%d.%d.%d.%d\")",
			  (const char *)RDEscapeString(station_name),
			  RD_STATION_DESCRIPTION,
			  RD_USER_LOGIN_NAME,
			  RD_USER_LOGIN_NAME,
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

  //
  // Create Test Tone Cart
  //
  sql="insert into CART(TYPE,NUMBER,GROUP_NAME,TITLE,ARTIST,CUT_QUANTITY,\
      FORCED_LENGTH,METADATA_DATETIME)\
     values (1,999999,\"TEST\",\"Test Tone\",\"Rivendell Radio Automation\",1,10000,now())";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default Audio Cut
  //
  sql=QString().sprintf("insert into CUTS set CUT_NAME=\"999999_001\",\
                         CART_NUMBER=999999,\
                         DESCRIPTION=\"1 kHz at Reference Level [-16 dBFS]\",\
                         OUTCUE=\"[tone]\",CODING_FORMAT=0,\
                         SAMPLE_RATE=%d,BIT_RATE=0,\
                         CHANNELS=2,LENGTH=10000,START_POINT=0,END_POINT=10000,\
                         ORIGIN_DATETIME=NOW(),ORIGIN_NAME=\"RDGen\"",
			RD_DEFAULT_SAMPLE_RATE);
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Clipboard Entry
  //
  sql="insert into CLIPBOARD set CUT_NAME=\"clip\",CART_NUMBER=0,\
      DESCRIPTION=\"Default Clipboard\"";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Create Default Service
  //
  RDSvc *svc=new RDSvc(RD_SERVICE_NAME);
  svc->create("");
  svc->setDescription(RD_SERVICE_DESCRIPTION);
  delete svc;

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
    sql=QString().sprintf ("insert into GROUPS (NAME,DESCRIPTION,DEFAULT_CART_TYPE,\
      DEFAULT_LOW_CART,DEFAULT_HIGH_CART,REPORT_TFC,REPORT_MUS,ENABLE_NOW_NEXT) \
      values (\"%s\",\"%s\",%d,%d,%d,\'%s\',\'%s\',\'%s\')",
			   g->group,
			   g->description, 
			   g->macro ? 2:1,
			   g->start, 
			   g->end,
			   g->rpt_traffic ? "Y" :"N",
			   g->rpt_music ? "Y" :"N",
			   g->now_next ? "Y" :"N");
    if(!RunQuery(sql)) {
      return false;
    }
    // Add it to the user permissions table for the default user
    sql=QString().sprintf("insert into USER_PERMS (USER_NAME,GROUP_NAME) \
                         values (\"%s\",\"%s\")",RD_USER_LOGIN_NAME,g->group);
    if(!RunQuery(sql)) {
      return false;
    }
    // Add it to the audio permsmissions table
    sql=QString().sprintf("insert into AUDIO_PERMS (GROUP_NAME,SERVICE_NAME) \
                         values (\"%s\",\"%s\")",g->group,RD_SERVICE_NAME);
    if(!RunQuery(sql)) {
      return false;
    }
  }

  //
  // Create Sample Log
  //
  sql=RDCreateLogTableSql("SAMPLE_LOG");
  if(!RunQuery(sql)) {
    return false;
  }
  sql="insert into LOGS (NAME,SERVICE,DESCRIPTION,ORIGIN_USER,ORIGIN_DATETIME)\
      values (\"SAMPLE\",\"Production\",\"Sample Log\",\"user\",NOW())";
  if(!RunQuery(sql)) {
    return false;
  }

  //
  // Generate Hotkey Definitions
  //
  if (!UpdateRDAirplayHotkeys(station_name)) {
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
  QSqlQuery *q;
  QSqlQuery *q1;

  //
  // Create temporary field
  //
  sql=QString().sprintf("alter table %s add column %s_TEMP int after %s",
			(const char *)table,
			(const char *)field,
			(const char *)field);
  q=new QSqlQuery(sql);
  delete q;

  //
  // Copy data to temporary field
  //
  sql=QString().sprintf("select ID,%s from %s",
			(const char *)field,
			(const char *)table);
  q=new QSqlQuery(sql);
  while(q->next()) {
    if(!q->value(1).isNull()) {
      sql=QString().sprintf("update %s set %s_TEMP=%d where ID=%d",
			    (const char *)table,
			    (const char *)field,
			    QTime().msecsTo(q->value(1).toTime()),
			    q->value(0).toInt());
      q1=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
  delete q;

  //
  // Copy data back to primary field
  //
  sql=QString().sprintf("select ID,%s_TEMP from %s",
			(const char *)field,
			(const char *)table);
  q=new QSqlQuery(sql);
  while(q->next()) {
    if(!q->value(1).isNull()) {
      sql=QString().sprintf("update %s set %s=%d where ID=%d",
			    (const char *)table,
			    (const char *)field,
			    q->value(1).toInt(),
			    q->value(0).toInt());
      q1=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
  delete q;
}


void UpdateLogTable(const QString &table)
{
  QString sql;
  QSqlQuery *q;

  //
  // Drop POST_TIME
  //
  sql=QString().sprintf("alter table %s drop column POST_TIME",
			(const char *)table);
  q=new QSqlQuery(sql);
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
int UpdateDb(int ver)
{
  QString cmd;
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;
  QSqlQuery *q2;
  QSqlQuery *q3;
  RDCart *cart;
  unsigned dev;
  QString tablename;

  //
  // Create backup
  //
  if(!admin_skip_backup) {
    if(admin_backup_filename.isEmpty()) {
      bool home_found = false;
      admin_backup_filename = RDGetHomeDir(&home_found);
      if (!home_found) {
        admin_backup_filename = RDTempDir();
      }
      admin_backup_filename+=
	QString().sprintf("/rdbackup-%s-%d.sql.gz",
       				       (const char *)QDate::currentDate().
			  toString("yyyyMMdd"),ver);
    }
    cmd=QString().sprintf("mysqldump -h %s -u %s -p%s %s | gzip -q -c - > %s",
			  (const char *)admin_config->mysqlHostname(),
			  (const char *)admin_config->mysqlUsername(),
			  (const char *)admin_config->mysqlPassword(),
			  (const char *)admin_config->mysqlDbname(),
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
    sql="create table if not exists RDAIRPLAY (\
      ID int not null primary key,\
      STATION char(40) not null,\
      INSTANCE int unsigned not null,\
      AUTO_CARD0 int default -1,\
      AUTO_STREAM0 int default -1,\
      AUTO_PORT0 int default -1,\
      AUTO_CARD1 int default -1,\
      AUTO_STREAM1 int default -1,\
      AUTO_PORT1 int default -1,\
      PANEL_CARD0 int default -1,\
      PANEL_STREAM0 int default -1,\
      PANEL_PORT0 int default -1,\
      PANEL_CARD1 int default -1,\
      PANEL_STREAM1 int default -1,\
      PANEL_PORT1 int default -1,\
      index STATION_IDX (STATION,INSTANCE))"; 
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create Default RDAirPlay Configuration
    //
    sql="insert into RDAIRPLAY (STATION,INSTANCE) \
      values (\"DEFAULT\",0)";
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
    q=new QSqlQuery("select NAME from LOGS");
    while(q->next()) {
      RunQuery(QString().
	      sprintf("alter table %s_LOG add SOURCE int not null after COUNT",
		     (const char *)q->value(0).toString()));
    }
    delete q;
  }

  if(ver<6) {
    // 
    // Update RDAIRPLAY Structure
    //
    q=new QSqlQuery("alter table RDAIRPLAY add column CARD int default -1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column PORT0 int default -1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column PORT1 int default -1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column PORT2 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM0 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM1 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM2 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM3 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM4 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM5 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM6 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column STREAM7 int default -1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column AUTO_CARD0");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column AUTO_CARD1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column AUTO_STREAM0");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column AUTO_STREAM1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column AUTO_PORT0");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column AUTO_PORT1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column PANEL_CARD0");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column PANEL_CARD1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column PANEL_STREAM0");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column PANEL_STREAM1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column PANEL_PORT0");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY drop column PANEL_PORT1");
    delete q;
  }

  if(ver<7) {
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column SEGUE_LENGTH int default 0");
    delete q;
  }

  if(ver<8) {
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column PORT3 int default -1");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column OP_MODE int default 0");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column START_MODE int default 0");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column PIE_COUNT_LENGTH int default 15000");
    delete q;
    q=new 
      QSqlQuery("alter table RDAIRPLAY add column PIE_COUNT_ENDPOINT int default 0");
    delete q;
  }

  if(ver<9) {
    q=new QSqlQuery("alter table RDAIRPLAY add column PORT4 int default -1");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column PORT5 int default -1");
    delete q;
  }

  if(ver<10) {
    q=new QSqlQuery("select NAME from LOGS");
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
    q=new QSqlQuery("alter table RDAIRPLAY add column CHECK_TIMESYNC enum('N','Y') default 'N'");
    delete q;    

  //
  // Create PANELS Table
  //
  sql="create table if not exists PANELS (\
      ID int not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      ROW_NO int not null,\
      COLUMN_NO int not null,\
      LABEL char(64),\
      CART int,\
      DEFAULT_COLOR char(6),\
      index OWNER_IDX (OWNER))"; 
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<12) {
    q=new QSqlQuery("alter table RDAIRPLAY add column STATION_PANELS int default 3");
    delete q;

    q=new QSqlQuery("alter table RDAIRPLAY add column USER_PANELS int default 3");
    delete q;
  }

  if(ver<13) {
    sql="drop table PANELS";
    q=new QSqlQuery(sql);
    delete q;

    sql="create table if not exists PANELS (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      ROW_NO int not null,\
      COLUMN_NO int not null,\
      LABEL char(64),\
      CART int,\
      DEFAULT_COLOR char(6),\
      index OWNER_IDX (OWNER))"; 
    q=new QSqlQuery(sql);
    delete q;
  }
    
  if(ver<14) {
    sql="drop table PANELS";
    q=new QSqlQuery(sql);
    delete q;

    sql="create table if not exists PANELS (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      ROW_NO int not null,\
      COLUMN_NO int not null,\
      LABEL char(64),\
      CART int,\
      DEFAULT_COLOR char(6),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO),\
      index SAVE_IDX (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO))"; 
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<15) {
    q=new QSqlQuery("alter table PANELS drop column DEFAULT_COLOR");
    delete q;

    q=new QSqlQuery("alter table PANELS add column DEFAULT_COLOR char(7)");
    delete q;
  }

  if(ver<16) {
    //
    // Create MATRICES Table
    //
    sql="create table if not exists MATRICES (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      NAME char(64),\
      MATRIX int not null,\
      TYPE int not null,\
      PORT int not null,\
      GPIO_DEVICE char(255),\
      INPUTS int not null,\
      OUTPUTS int not null,\
      GPIS int not null,\
      GPOS int not null,\
      index MATRIX_IDX (STATION_NAME,MATRIX))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    
    //
    // Create INPUTS Table
    //
    sql="create table if not exists INPUTS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      MATRIX int not null,\
      NUMBER int not null,\
      NAME char(64),\
      FEED_NAME char(8),\
      index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    
    //
    // Create OUTPUTS Table
    //
    sql="create table if not exists OUTPUTS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      MATRIX int not null,\
      NUMBER int not null,\
      NAME char(64),\
      index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }
  
  if(ver<17) {
    q=new QSqlQuery("alter table INPUTS add column CHANNEL_MODE int");
    delete q;
  }

  if(ver<18) {
    q=new QSqlQuery("alter table STATIONS add column IPV4_ADDRESS char(15)");
    delete q;
  }

  if(ver<19) {
    q=new QSqlQuery("create table if not exists EVENTS (\
      ID int auto_increment not null primary key,\
      STATION_NAME char(64) not null,\
      SUN enum('N','Y') not null,\
      MON enum('N','Y') not null,\
      TUE enum('N','Y') not null,\
      WED enum('N','Y') not null,\
      THU enum('N','Y') not null,\
      FRI enum('N','Y') not null,\
      SAT enum('N','Y') not null,\
      TIME time not null,\
      DESCRIPTION char(64),\
      COMMAND char(255),\
      index STATION_IDX (STATION_NAME),\
      index SUN_IDX (STATION_NAME,SUN),\
      index MON_IDX (STATION_NAME,MON),\
      index TUE_IDX (STATION_NAME,TUE),\
      index WED_IDX (STATION_NAME,WED),\
      index THU_IDX (STATION_NAME,THU),\
      index FRI_IDX (STATION_NAME,FRI),\
      index SAT_IDX (STATION_NAME,SAT))");
    delete q;
  }

  if(ver<20) {
    q=new QSqlQuery("alter table CART add column MACROS text");
    delete q;
  }

  if(ver<21) {
    q=new 
      QSqlQuery("alter table RECORDINGS add column MACRO_CART int default -1");
    delete q;
    q=new 
      QSqlQuery("drop table EVENTS");
    delete q;
  }

  if(ver<22) {
    q=new QSqlQuery("alter table DECKS drop column SWITCH_TYPE");
    delete q;
    q=new QSqlQuery("alter table DECKS drop column TTY_ID");
    delete q;
    q=new QSqlQuery("alter table DECKS add column SWITCH_STATION char(64)");
    delete q;
    q=new
      QSqlQuery("alter table DECKS add column SWITCH_MATRIX int default -1");
    delete q;
    q=new
      QSqlQuery("alter table DECKS add column SWITCH_OUTPUT int default -1");
    delete q;
    q=new
      QSqlQuery("alter table DECKS add column SWITCH_DELAY int default 0");
    delete q;
  }

  if(ver<23) {
    q=new QSqlQuery("alter table RECORDINGS drop column SOURCE_NAME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column \
                 SWITCH_INPUT int default -1");
    delete q;
    q=new QSqlQuery("drop table SOURCES");
    delete q;
  }

  if(ver<24) {
    q=new QSqlQuery("alter table RECORDINGS add column \
                     TYPE int default 0 after STATION_NAME");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS add column \
                     SWITCH_OUTPUT int default -1");
    delete q;
    q=new QSqlQuery("update RECORDINGS set TYPE=1 where MACRO_CART!=-1");
    delete q;
  }

  if(ver<25) {
    q=new QSqlQuery("alter table RECORDINGS drop index SUN_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index MON_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index TUE_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index WED_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index THU_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index FRI_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index SAT_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS add column IS_ACTIVE \
                     enum('N','Y') default 'Y' after ID");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS add index \
                     SUN_IDX (STATION_NAME,SUN,IS_ACTIVE)");
    q=new QSqlQuery("alter table RECORDINGS add index \
                     MON_IDX (STATION_NAME,MON,IS_ACTIVE)");
    q=new QSqlQuery("alter table RECORDINGS add index \
                     TUE_IDX (STATION_NAME,TUE,IS_ACTIVE)");
    q=new QSqlQuery("alter table RECORDINGS add index \
                     WED_IDX (STATION_NAME,WED,IS_ACTIVE)");
    q=new QSqlQuery("alter table RECORDINGS add index \
                     THU_IDX (STATION_NAME,THU,IS_ACTIVE)");
    q=new QSqlQuery("alter table RECORDINGS add index \
                     FRI_IDX (STATION_NAME,FRI,IS_ACTIVE)");
    q=new QSqlQuery("alter table RECORDINGS add index \
                     SAT_IDX (STATION_NAME,SAT,IS_ACTIVE)");
    delete q;
  }

  if(ver<26) {
    q=new QSqlQuery("alter table RECORDINGS drop index SUN_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index MON_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index TUE_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index WED_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index THU_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index FRI_IDX");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS drop index SAT_IDX");
    delete q;
  }

  //
  // Create GPIS Table
  //
  if(ver<27) {
    sql="create table if not exists GPIS (\
         ID int auto_increment not null primary key,\
         STATION_NAME char(64) not null,\
         MATRIX int not null,\
         NUMBER int not null,\
         MACRO_CART int default -1,\
         index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<28) {
    sql="alter table CUTS alter column ORIGIN_DATETIME set default NULL";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CUTS alter column START_DATETIME set default NULL";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CUTS alter column END_DATETIME set default NULL";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CUTS alter column START_DAYPART set default NULL";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CUTS alter column END_DAYPART set default NULL";
    q=new QSqlQuery(sql);
    delete q;
    sql="update CUTS set ORIGIN_DATETIME=NULL where \
         ORIGIN_DATETIME=\"0000-00-00 00:00:00\"";
    q=new QSqlQuery(sql);
    delete q;
    sql="update CUTS set START_DATETIME=NULL where \
         START_DATETIME=\"0000-00-00 00:00:00\"";
    q=new QSqlQuery(sql);
    delete q;
    sql="update CUTS set END_DATETIME=NULL where \
         END_DATETIME=\"0000-00-00 00:00:00\"";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("alter table %s_LOG drop index START_TIME_IDX",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG alter column START_TIME \
                             set default NULL",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG modify column START_TIME int",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG alter column POST_TIME \
                             set default NULL",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("update %s_LOG set START_TIME=NULL where \
                             START_TIME=\"00:00:00\"",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("update %s_LOG set POST_TIME=NULL where \
                             POST_TIME=\"00:00:00\"",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<29) {
    q=new QSqlQuery("alter table RECORDINGS add column \
                     EXIT_CODE int default 0");
    delete q;
  }

  if(ver<30) {
    q=new QSqlQuery("alter table RECORDINGS add column \
                     ONE_SHOT enum('N','Y') default 'N'");
    delete q;
  }

  if(ver<31) {
    q=new QSqlQuery("alter table STATIONS add column \
                     TIME_OFFSET int default 0");
    delete q;
  }

  if(ver<32) {
    q=new QSqlQuery("alter table GROUPS add column \
                     DEFAULT_LOW_CART int unsigned default 0");
    q=new QSqlQuery("alter table GROUPS add column \
                     DEFAULT_HIGH_CART int unsigned default 0");
    delete q;
  }

  if(ver<33) {
    q=new QSqlQuery("alter table CUTS add column \
                     SUN enum('N','Y') default 'Y' after END_DAYPART");
    q=new QSqlQuery("alter table CUTS add column \
                     MON enum('N','Y') default 'Y' after END_DAYPART");
    q=new QSqlQuery("alter table CUTS add column \
                     TUE enum('N','Y') default 'Y' after END_DAYPART");
    q=new QSqlQuery("alter table CUTS add column \
                     WED enum('N','Y') default 'Y' after END_DAYPART");
    q=new QSqlQuery("alter table CUTS add column \
                     THU enum('N','Y') default 'Y' after END_DAYPART");
    q=new QSqlQuery("alter table CUTS add column \
                     FRI enum('N','Y') default 'Y' after END_DAYPART");
    q=new QSqlQuery("alter table CUTS add column \
                     SAT enum('N','Y') default 'Y' after END_DAYPART");
  }

  if(ver<34) {
    q=new QSqlQuery("select NAME from LOGS");
    while(q->next()) {
      RunQuery(QString().sprintf("alter table %s_LOG \
                      add GRACE_TIME int default 0 after START_TIME",
		     (const char *)q->value(0).toString()));
    }
    delete q;
  }

  if(ver<35) {
    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     TRANS_LENGTH int default 0 after SEGUE_LENGTH");
  }

  if(ver<36) {
    q=new QSqlQuery("select NAME from LOGS");
    while(q->next()) {
      RunQuery(QString().sprintf("alter table %s_LOG \
                      add POST_POINT enum('N','Y') default 'N'\
                      after TIME_TYPE",
		     (const char *)q->value(0).toString()));
    }
    delete q;
  }

  if(ver<37) {
    q=new QSqlQuery("alter table LOGS add column \
                     TYPE int not null default 0 after NAME");
    delete q;
  }

  if(ver<38) {
    sql=QString("create table if not exists EVENTS (\
      NAME char(64) not null primary key,\
      PROPERTIES char(64),\
      DISPLAY_TEXT char(64),\
      NOTE_TEXT char(255),\
      PREPOSITION int default -1,\
      TIME_TYPE int default 0,\
      GRACE_TIME int default 0,\
      POST_POINT enum('N','Y') default 'N',\
      USE_AUTOFILL enum('N','Y') default 'N',\
      USE_TIMESCALE enum('N','Y') default 'N',\
      IMPORT_SOURCE int default 0,\
      START_SLOP int default 0,\
      END_SLOP int default 0,\
      FIRST_TRANS_TYPE int default 0,\
      DEFAULT_TRANS_TYPE int default 0,\
      COLOR char(7))");
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<39) {   // Transpose RDLogLine::Stop and RDLogLine::Segue
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().
	sprintf("update %s_LOG set TRANS_TYPE=100 where TRANS_TYPE=1",
		(const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().
	sprintf("update %s_LOG set TRANS_TYPE=1 where TRANS_TYPE=2",
		(const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().
	sprintf("update %s_LOG set TRANS_TYPE=2 where TRANS_TYPE=100",
		(const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<40) {
    sql=QString("create table if not exists CLOCKS (\
      NAME char(64) not null primary key,\
      SHORT_NAME char(8),\
      COLOR char(7))");
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<41) {
    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     CARD0 int default -1 after INSTANCE");
    delete q;

    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     CARD1 int default -1 after PORT0");
    delete q;

    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     CARD2 int default -1 after PORT1");
    delete q;

    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     CARD3 int default -1 after PORT2");
    delete q;

    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     CARD4 int default -1 after PORT3");
    delete q;

    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     CARD5 int default -1 after PORT4");
    delete q;
  }

  if(ver<42) {
    q=new QSqlQuery("alter table SERVICES add column CLOCK0 char(64) \
                     after DESCRIPTION");
    delete q;
    for(int i=1;i<168;i++) {
      q=new QSqlQuery(QString().sprintf("alter table SERVICES \
                   add column CLOCK%d char(64) after CLOCK%d",i,i-1));
      delete q;
    }
  }

  if(ver<43) {
    q=new QSqlQuery("alter table RDAIRPLAY \
                     add column SHOW_AUX_1 enum('N','Y') default 'Y' \
                     after USER_PANELS");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY \
                     add column SHOW_AUX_2 enum('N','Y') default 'Y' \
                     after SHOW_AUX_1");
    delete q;
  }

  if(ver<44) {
    q=new QSqlQuery("alter table CUTS \
                     add column LOCAL_COUNTER int unsigned default 0 \
                     after PLAY_COUNTER");
    delete q;
  }

  if(ver<45) {
    q=new QSqlQuery("alter table CUTS \
                     add column EVERGREEN enum('N','Y') default 'N' \
                     after CART_NUMBER");
    delete q;
  }

  if(ver<46) {
    q=new QSqlQuery("alter table CART \
                     add column LENGTH_DEVIATION int unsigned default 0 \
                     after FORCED_LENGTH");
    delete q;
    q=new QSqlQuery("select NUMBER from CART where TYPE=1");
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->calculateAverageLength(&dev);
      cart->setLengthDeviation(dev);
      delete cart;
    }
    delete q;
  }

  if(ver<47) {
    q=new QSqlQuery("alter table SERVICES \
                     add column NAME_TEMPLATE char(255)\
                     after DESCRIPTION");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_PATH char(255)\
                     after NAME_TEMPLATE");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_WIN_PATH char(255)\
                     after TFC_PATH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_CART_OFFSET int\
                     after TFC_WIN_PATH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_CART_LENGTH int\
                     after TFC_CART_OFFSET");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_START_OFFSET int\
                     after TFC_CART_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_START_LENGTH int\
                     after TFC_START_OFFSET");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_PATH char(255)\
                     after TFC_START_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_WIN_PATH char(255)\
                     after MUS_PATH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_CART_OFFSET int\
                     after MUS_PATH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_CART_LENGTH int\
                     after MUS_CART_OFFSET");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_START_OFFSET int\
                     after MUS_CART_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_START_LENGTH int\
                     after MUS_START_OFFSET");
    delete q;
  }

  if(ver<48) {
    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_LENGTH_OFFSET int\
                     after TFC_START_LENGTH");
    delete q;

    q=new QSqlQuery("alter table SERVICES \
                     add column TFC_LENGTH_LENGTH int\
                     after TFC_LENGTH_OFFSET");
    delete q;

    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_LENGTH_OFFSET int\
                     after MUS_START_LENGTH");
    delete q;

    q=new QSqlQuery("alter table SERVICES \
                     add column MUS_LENGTH_LENGTH int\
                     after MUS_LENGTH_OFFSET");
    delete q;
  }

  if(ver<49) {
    q=new QSqlQuery("create table if not exists AUTOFILLS (\
                     ID int not null primary key auto_increment,\
                     SERVICE char(10),\
                     CART_NUMBER int unsigned,\
                     index SERVICE_IDX (SERVICE))");
    delete q;
  }

  if(ver<50) {
    q=new QSqlQuery("alter table SERVICES \
                     add column CHAIN_LOG enum('N','Y') default 'N'\
                     after NAME_TEMPLATE");
    delete q;
  }

  if(ver<51) {
    q=new QSqlQuery("alter table USERS \
                     modify column PASSWORD char(32)\
                     after DESCRIPTION");
    delete q;
  }

  if(ver<52) {
    q=new QSqlQuery("create table if not exists HOSTVARS (\
      ID int not null primary key auto_increment,\
      STATION_NAME char(64) not null,\
      NAME char(32) not null,\
      VARVALUE char(255),\
      REMARK char(255),\
      index NAME_IDX (STATION_NAME))");
    delete q;
  }

  if(ver<53) {
    q=new QSqlQuery("alter table STATIONS add column BACKUP_DIR char(255)");
    delete q;
    q=new QSqlQuery("alter table STATIONS\
                       add column BACKUP_LIFE int default 0");
    delete q;
  }
  
  if(ver<54) {
    q=new QSqlQuery("alter table RDAIRPLAY add column \
                       CLEAR_FILTER enum(\'N\',\'Y\') default \'N\'");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column \
                       BAR_ACTION int unsigned default 0");
    delete q;
  }

  if(ver<55) {
    q=new QSqlQuery("alter table RDAIRPLAY add column \
                       FLASH_PANEL enum(\'N\',\'Y\') default \'N\'");
    delete q;
  }

  if(ver<56) {
    q=new QSqlQuery("alter table STATIONS add column \
                       HEARTBEAT_CART int unsigned default 0");
    delete q;
    q=new QSqlQuery("alter table STATIONS add column \
                       HEARTBEAT_INTERVAL int unsigned default 0");
    delete q;
  }

  if(ver<57) {
    q=new QSqlQuery("create table if not exists SERVICE_PERMS (\
                     ID int unsigned auto_increment not null primary key,\
                     STATION_NAME char(64),\
                     SERVICE_NAME char(10),\
                     index STATION_IDX (STATION_NAME),\
                     index SERVICE_IDX (SERVICE_NAME))");
    delete q;
    q=new QSqlQuery("select NAME from STATIONS");
    while(q->next()) {
      q1=new QSqlQuery("select NAME from SERVICES");
      while(q1->next()) {
	q2=new 
	  QSqlQuery(QString().sprintf("insert into SERVICE_PERMS set \
                                       STATION_NAME=\"%s\",\
                                       SERVICE_NAME=\"%s\"",
				      (const char *)q->value(0).toString(),
				      (const char *)q1->value(0).toString()));
	delete q2;
      }
      delete q1;
    }
    delete q;
  }

  if(ver<58) {
    q=new QSqlQuery("select NAME from LOGS");
    while(q->next()) {
      q1=new 
	QSqlQuery(QString().sprintf("alter table %s_LOG add column\
                                     EXT_START_TIME time",
				    (const char *)q->value(0).toString()));
      delete q1;
      q1=new 
	QSqlQuery(QString().sprintf("alter table %s_LOG add column\
                                     EXT_LENGTH int",
				    (const char *)q->value(0).toString()));
      delete q1;
      q1=new 
	QSqlQuery(QString().sprintf("alter table %s_LOG add column\
                                     EXT_DATA char(32)",
				    (const char *)q->value(0).toString()));
      delete q1;
      q1=new 
	QSqlQuery(QString().sprintf("alter table %s_LOG add column\
                                     EXT_EVENT_ID char(8)",
				    (const char *)q->value(0).toString()));
      delete q1;
    }
    delete q;
  }

  if(ver<59) {
    q=new QSqlQuery("alter table SERVICES add column TFC_DATA_OFFSET int\
                     after TFC_LENGTH_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column TFC_DATA_LENGTH int\
                     after TFC_DATA_OFFSET");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column TFC_EVENT_ID_OFFSET int\
                     after TFC_DATA_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column TFC_EVENT_ID_LENGTH int\
                     after TFC_EVENT_ID_OFFSET");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column MUS_DATA_OFFSET int\
                     after MUS_LENGTH_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column MUS_DATA_LENGTH int\
                     after MUS_DATA_OFFSET");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column MUS_EVENT_ID_OFFSET int\
                     after MUS_DATA_LENGTH");
    delete q;
    q=new QSqlQuery("alter table SERVICES add column MUS_EVENT_ID_LENGTH int\
                     after MUS_EVENT_ID_OFFSET");
    delete q;
  }

  //
  // Version 60 and 61 code removed, as per-log reconciliation data is no 
  // longer used.   FFG 11/08/2005
  //

  if(ver<62) {
    q=new QSqlQuery("alter table GROUPS add column \
                     REPORT_TFC enum('N','Y') default 'N'");
    delete q;
    q=new QSqlQuery("alter table GROUPS add column \
                     REPORT_MUS enum('N','Y') default 'N'");
    delete q;
    q=new QSqlQuery("alter table GROUPS add index \
                     IDX_REPORT_TFC (REPORT_TFC)");
    delete q;
    q=new QSqlQuery("alter table GROUPS add index \
                     IDX_REPORT_MUS (REPORT_MUS)");
    delete q;
  }

  if(ver<63) {
    sql=QString("create table if not exists REPORTS (\
                 ID int unsigned auto_increment not null primary key,\
                 NAME char(64) not null unique,\
                 DESCRIPTION char(64),\
                 EXPORT_FILTER int,\
                 EXPORT_PATH char(255),\
                 WIN_EXPORT_PATH char(255),\
                 index IDX_NAME (NAME))");
    q=new QSqlQuery(sql);
    delete q;
    sql=QString("create table if not exists REPORT_SERVICES (\
                 ID int unsigned auto_increment not null primary key,\
                 REPORT_NAME char(64) not null,\
                 SERVICE_NAME char(10),\
                 index IDX_REPORT_NAME (REPORT_NAME))");
    q=new QSqlQuery(sql);
    delete q;
    sql=QString("create table if not exists REPORT_STATIONS (\
                 ID int unsigned auto_increment not null primary key,\
                 REPORT_NAME char(64) not null,\
                 STATION_NAME char(64),\
                 index IDX_REPORT_NAME (REPORT_NAME))");
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<64) {
    q=new QSqlQuery("alter table REPORTS add column\
                     EXPORT_TFC enum('N','Y') default 'N'");
    delete q;
    q=new QSqlQuery("alter table REPORTS add column\
                     EXPORT_MUS enum('N','Y') default 'N'");
    delete q;
    q=new QSqlQuery("alter table REPORTS add column\
                     EXPORT_GEN enum('N','Y') default 'N'");
    delete q;
  }

  if(ver<65) {
    q=new QSqlQuery("alter table REPORTS add column STATION_ID char(16)");
    delete q;
  }

  if(ver<66) {
    q=new 
      QSqlQuery("alter table RDAIRPLAY alter column OP_MODE set default 2");
    delete q;
  }

  if(ver<67) {
    q=new 
      QSqlQuery("alter table RDAIRPLAY \
                 add column PAUSE_ENABLED enum('N','Y') default 'N'");
    delete q;
  }

  if(ver<68) {
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_ADDR0 char(255)");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_PORT0 int unsigned");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_STRING0 char(255)");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_ADDR1 char(255)");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_PORT1 int unsigned");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_STRING1 char(255)");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_ADDR2 char(255)");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_PORT2 int unsigned");
    delete q;
    q=new QSqlQuery("alter table RDAIRPLAY add column UDP_STRING2 char(255)");
    delete q;
    q=new QSqlQuery("alter table GROUPS \
                     add column ENABLE_NOW_NEXT enum('N','Y') default 'N'");
    delete q;
  }

  if(ver<69) {
    q=new QSqlQuery("alter table MATRICES add column PORT_TYPE int default 0\
                     after TYPE");
    delete q;
    q=new QSqlQuery("alter table MATRICES add column IP_ADDRESS char(16)\
                     after PORT");
    delete q;
    q=new QSqlQuery("alter table MATRICES add column IP_PORT int\
                     after IP_ADDRESS");
    delete q;
  }

  if(ver<70) {
    q=new QSqlQuery("select NAME from LOGS");
    while(q->next()) {
      q1=new 
	QSqlQuery(QString().sprintf("alter table %s_REC\
                                     add column PLAY_SOURCE int default 0\
                                     after EVENT_TYPE",
				    (const char *)q->value(0).toString()));
      delete q1;
      q1=new 
	QSqlQuery(QString().sprintf("alter table %s_REC\
                                     add column CUT_NUMBER int default 0\
                                     after CART_NUMBER",
				    (const char *)q->value(0).toString()));
      delete q1;
    }
    delete q;
  }

  if(ver<71) {
    q=new QSqlQuery("alter table RECORDINGS add column END_LINE int default -1\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column END_MATRIX int default -1\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column END_LENGTH int default 0\
                     after START_TIME");
    delete q;
    q=new QSqlQuery("alter table RECORDINGS add column END_TIME time\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add \
                 column END_TYPE int unsigned default 2 after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column START_OFFSET int default 0\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column START_LINE int default -1\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column START_MATRIX int default -1\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column START_LENGTH int default 0\
                     after START_TIME");
    delete q;
    q=new 
      QSqlQuery("alter table RECORDINGS add column START_TYPE int default 0\
                     after DESCRIPTION");
    delete q;
  }

  if(ver<72) {
    q=new QSqlQuery("alter table GROUPS add column \
                     DEFAULT_CART_TYPE int unsigned default 1\
                     after DESCRIPTION");
    delete q;
  }

  if(ver<73) {
    q=new QSqlQuery("alter table RDAIRPLAY add column \
                     DEFAULT_TRANS_TYPE int unsigned default 0\
                     after CLEAR_FILTER");
    delete q;
  }

  if(ver<74) {
    //
    // Create CLOCK_PERMS Table
    //
    sql=QString("create table if not exists CLOCK_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      CLOCK_NAME char(64),\
      SERVICE_NAME char(10),\
      index CLOCK_IDX (CLOCK_NAME),\
      index SERVICE_IDX (SERVICE_NAME))");
    printf("SQL: %s\n",(const char *)sql);
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    sql="select NAME from CLOCKS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql="select NAME from SERVICES";
      q1=new QSqlQuery(sql);
      while(q1->next()) {
	sql=QString().sprintf("insert into CLOCK_PERMS set CLOCK_NAME=\"%s\",\
                               SERVICE_NAME=\"%s\"",
			      (const char *)q->value(0).toString(),
			      (const char *)q1->value(0).toString());
	q2=new QSqlQuery(sql);
	delete q2;
      }
      delete q1;
    }
    delete q;

    //
    // Create EVENT_PERMS Table
    //
    sql=QString("create table if not exists EVENT_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      EVENT_NAME char(64),\
      SERVICE_NAME char(10),\
      index EVENT_IDX (EVENT_NAME),\
      index SERVICE_IDX (SERVICE_NAME))");
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql="select NAME from SERVICES";
      q1=new QSqlQuery(sql);
      while(q1->next()) {
	sql=QString().sprintf("insert into EVENT_PERMS set EVENT_NAME=\"%s\",\
                               SERVICE_NAME=\"%s\"",
			      (const char *)q->value(0).toString(),
			      (const char *)q1->value(0).toString());
	q2=new QSqlQuery(sql);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }

  if(ver<75) {
    q=new QSqlQuery("alter table MATRICES add column \
                     CARD int default -1 after PORT_TYPE");
    delete q;
  }

  if(ver<76) {
    q=new QSqlQuery("alter table DECKS add column \
                     MON_PORT_NUMBER int default -1 after PORT_NUMBER");
    delete q;
  }

  if(ver<77) {
    //
    // Create USER_PERMS table
    //
    sql=QString("create table if not exists USER_PERMS (\
      ID int unsigned auto_increment not null primary key,\
      USER_NAME char(8),\
      GROUP_NAME char(10),\
      index USER_IDX (USER_NAME),\
      index GROUP_IDX (GROUP_NAME))");
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    sql=QString("select LOGIN_NAME from USERS");
    q=new QSqlQuery(sql);
    sql=QString("select NAME from GROUPS");
    q1=new QSqlQuery(sql);
    while(q->next()) {
      q1->seek(-1);
      while(q1->next()) {
	sql=QString().sprintf("insert into USER_PERMS set USER_NAME=\"%s\",\
                               GROUP_NAME=\"%s\"",
			      (const char *)q->value(0).toString(),
			      (const char *)q1->value(0).toString());
	q2=new QSqlQuery(sql);
	delete q2;
      }
    }
    delete q1;
    delete q;

  }

  if(ver<78) {
    sql="alter table USERS add column \
         MODIFY_TEMPLATE_PRIV enum('N','Y') not null default 'N'\
         after ARRANGE_LOG_PRIV";
    q=new QSqlQuery(sql);
    delete q;
    sql="update USERS set MODIFY_TEMPLATE_PRIV=\"Y\" where \
         CREATE_LOG_PRIV=\"Y\"";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<79) {
    sql="alter table GROUPS add column \
         ENFORCE_CART_RANGE enum('N','Y') default 'N'\
         after DEFAULT_HIGH_CART";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<80) {
    sql="alter table SERVICES add column\
         TFC_ANNC_TYPE_OFFSET int after TFC_EVENT_ID_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column\
         TFC_ANNC_TYPE_LENGTH int after TFC_ANNC_TYPE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column\
         MUS_ANNC_TYPE_OFFSET int after MUS_EVENT_ID_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column\
         MUS_ANNC_TYPE_LENGTH int after MUS_ANNC_TYPE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPORTS add column\
         FORCE_TFC enum('N','Y') default 'N' after EXPORT_TFC";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table REPORTS add column\
         FORCE_MUS enum('N','Y') default 'N' after EXPORT_MUS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table GROUPS alter column REPORT_TFC set default 'Y'";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table GROUPS alter column REPORT_MUS set default 'Y'";
    q=new QSqlQuery(sql);
    delete q;
   }

  if(ver<81) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_REC add column\
                             EVENT_SOURCE int default 0 after EVENT_TYPE",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_REC add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_PRE add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column\
                             EVENT_SOURCE int default 0 after EVENT_TYPE",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EVENT_SOURCE int default 0 after EVENT_TYPE",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<82) {
    sql="alter table INPUTS add column ENGINE_NUM int default -1\
         after CHANNEL_MODE";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table INPUTS add column DEVICE_NUM int default -1\
         after ENGINE_NUM";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table OUTPUTS add column ENGINE_NUM int default -1\
         after NAME";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table OUTPUTS add column DEVICE_NUM int default -1\
         after ENGINE_NUM";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column USERNAME char(32) after IP_PORT";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table MATRICES add column PASSWORD char(32) after USERNAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="create table if not exists VGUEST_RESOURCES (\
         ID int unsigned auto_increment not null primary key,\
         STATION_NAME char(64) not null,\
         MATRIX_NUM int not null,\
         VGUEST_TYPE int not null,\
         NUMBER int not null,\
         ENGINE_NUM int default -1,\
         DEVICE_NUM int default -1,\
         SURFACE_NUM int default -1,\
         RELAY_NUM int default -1,\
         BUSS_NUM int default -1,\
         index STATION_MATRIX_IDX (STATION_NAME,MATRIX_NUM,VGUEST_TYPE))";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column FADERS int default 0 after GPOS";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table MATRICES add column DISPLAYS int default 0 after FADERS";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<83) {
    sql="alter table RECORDINGS add column URL char(255) after ONE_SHOT";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table RECORDINGS add column URL_USERNAME char(64) after URL";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table RECORDINGS add column URL_PASSWORD char(64)\
         after URL_USERNAME";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<84) {
    sql=
      "alter table STATIONS add column STATION_SCANNED enum('N','Y')\
       default 'N'";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column HPI_VERSION char(16)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column JACK_VERSION char(16)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column ALSA_VERSION char(16)";
    q=new QSqlQuery(sql);
    delete q;

    sql=
      "alter table STATIONS add column HAVE_OGGENC enum('N','Y') default 'N'";
    q=new QSqlQuery(sql);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_OGG123 enum('N','Y') default 'N'";
    q=new QSqlQuery(sql);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_FLAC enum('N','Y') default 'N'";
    q=new QSqlQuery(sql);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_LAME enum('N','Y') default 'N'";
    q=new QSqlQuery(sql);
    delete q;
    sql=
      "alter table STATIONS add column HAVE_MPG321 enum('N','Y') default 'N'";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD0_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD0_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD0_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD0_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD1_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD1_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD1_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD1_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD2_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD2_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD2_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD2_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD3_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD3_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD3_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD3_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD4_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD4_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD4_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD4_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD5_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD5_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD5_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD5_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD6_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD6_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD6_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD6_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column CARD7_DRIVER int default 0";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD7_NAME char(64)";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD7_INPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table STATIONS add column CARD7_OUTPUTS int default -1";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<85) {
    sql="alter table RECORDINGS add column NORMALIZE_LEVEL int default -1300\
         after TRIM_THRESHOLD";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RECORDINGS add column QUALITY int default 0\
         after BITRATE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<86) {
    sql="alter table RECORDINGS alter column END_TYPE set default 0";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<87) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG add column\
                             EXT_CART_NAME char(32) after EXT_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_REC add column\
                             EXT_CART_NAME char(32) after EXT_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_PRE add column\
                             EXT_CART_NAME char(32) after EXT_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EXT_CART_NAME char(32) after EXT_LENGTH",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<88) {
    sql="alter table RECORDINGS add column\
         ALLOW_MULT_RECS enum('N','Y') default 'N' after END_GPI";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RECORDINGS add column\
         MAX_GPI_REC_LENGTH int unsigned default 3600000\
         after ALLOW_MULT_RECS";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString().
      sprintf("select ID,START_TIME,END_TIME,END_LENGTH from RECORDINGS\
               where END_TYPE=%d",RDRecording::GpiEnd);
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().
	sprintf("update RECORDINGS set MAX_GPI_REC_LENGTH=%u where ID=%u",
		QTime().msecsTo(q->value(2).toTime())+q->value(3).toUInt()-
		QTime().msecsTo(q->value(1).toTime()),q->value(0).toUInt());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<89) {
    sql="alter table CART add column AVERAGE_LENGTH int unsigned\
         after FORCED_LENGTH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add column ASYNCRONOUS enum('N','Y') default 'N'\
         after PRESERVE_PITCH";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Audio);
    q=new QSqlQuery(sql);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      sql=QString().sprintf("update CART set AVERAGE_LENGTH=%u\
                             where NUMBER=%u",cart->calculateAverageLength(),
			    cart->number());
      q1=new QSqlQuery(sql);
      delete q1;
      delete cart;
    }
    delete q;

    RDMacroEvent *macro_event;
    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Macro);
    q=new QSqlQuery(sql);
    while(q->next()) {
      macro_event=new RDMacroEvent();
      macro_event->load(q->value(0).toUInt());
      sql=QString().sprintf("update CART set AVERAGE_LENGTH=%u,\
                             FORCED_LENGTH=%u where NUMBER=%u",
			    macro_event->length(),
			    macro_event->length(),
			    q->value(0).toUInt());
      q1=new QSqlQuery(sql);
      delete q1;
      delete macro_event;
    }
    delete q;
  }

  if(ver<90) {
    sql="alter table REPORTS add column CART_DIGITS int unsigned default 6\
         after STATION_ID";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPORTS add column USE_LEADING_ZEROS enum('N','Y')\
         default 'N' after CART_DIGITS";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<91) {
    sql="alter table DECKS add column DEFAULT_MONITOR_ON enum('N','Y')\
         default 'N' after MON_PORT_NUMBER";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<92) {
    sql="alter table EVENTS add column AUTOFILL_SLOP int default -1\
         after USE_AUTOFILL";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<93) {
    sql="alter table LOGS add column IMPORT_DATE DATE after END_DATE";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG add column\
                             FADEUP_POINT int default -1 after END_POINT",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG add column\
                             FADEUP_GAIN int default %d after FADEUP_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG add column\
                             FADEDOWN_POINT int default -1 after FADEUP_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG add column\
                            FADEDOWN_GAIN int default %d after FADEDOWN_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG add column\
                             SEGUE_GAIN int default %d after SEGUE_END_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");

      sql=QString().sprintf("alter table %s_PRE add column\
                             FADEUP_POINT int default -1 after END_POINT",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column\
                             FADEUP_GAIN int default %d after FADEUP_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column\
                             FADEDOWN_POINT int default -1 after FADEUP_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column\
                            FADEDOWN_GAIN int default %d after FADEDOWN_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column\
                             SEGUE_GAIN int default %d after SEGUE_END_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             FADEUP_POINT int default -1 after END_POINT",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             FADEUP_GAIN int default %d after FADEUP_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             FADEDOWN_POINT int default -1 after FADEUP_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                            FADEDOWN_GAIN int default %d after FADEDOWN_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             SEGUE_GAIN int default %d after SEGUE_END_POINT",
			    (const char *)tablename,RD_FADE_DEPTH);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<94) {
    sql="alter table CART add column OWNER char(64) after ASYNCRONOUS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add index OWNER_IDX (OWNER)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TRACK_GROUP char(10) after CHAIN_LOG";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column SCHEDULED_TRACKS int unsigned default 0\
         after IMPORT_DATE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column COMPLETED_TRACKS int unsigned default 0\
         after SCHEDULED_TRACKS";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<95) {
    sql="alter table USERS add column VOICETRACK_LOG_PRIV enum('N','Y') \
         not null default 'N' after ADDTO_LOG_PRIV";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<96) {
    sql=QString("create table if not exists RDLOGEDIT (\
                 ID int unsigned primary key auto_increment,\
                 STATION char(64) not null,\
                 INPUT_CARD int default -1,\
                 INPUT_PORT int default 0,\
                 OUTPUT_CARD int default -1,\
                 OUTPUT_PORT int default 0,\
                 FORMAT int unsigned default 0,\
                 SAMPRATE int unsigned default 44100,\
                 LAYER int unsigned default 0,\
                 BITRATE int unsigned default 0,\
                 DEFAULT_CHANNELS int unsigned default 2,\
                 MAXLENGTH int default 0,\
                 TAIL_PREROLL int unsigned default 2000,\
                 START_CART int unsigned default 0,\
                 END_CART int unsigned default 0,\
                 index STATION_IDX (STATION))");
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from STATIONS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into RDLOGEDIT set STATION=\"%s\"",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<97) {
    sql="alter table LOGS add column LOG_EXISTS enum('N','Y') default 'Y'\
         after NAME";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table LOGS add index NAME_IDX (NAME,LOG_EXISTS)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table USERS add column DELETE_REC_PRIV enum('N','Y')\
         default 'N' after DELETE_LOG_PRIV";
    q=new QSqlQuery(sql);
    delete q;
    sql="update USERS set DELETE_REC_PRIV=\"Y\" where DELETE_LOG_PRIV=\"Y\"";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<98) {
    QString tablename;
    sql="select NAME from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("create table %s_SRT (\
                             ID int unsigned auto_increment primary key,\
                             LENGTH int,\
                             LOG_NAME char(64),\
                             LOG_ID int,\
                             CART_NUMBER int unsigned,\
                             CUT_NUMBER int,\
                             TITLE char(255),\
                             ARTIST char(255),\
                             STATION_NAME char(64),\
                             EVENT_DATETIME datetime,\
                             SCHEDULED_TIME time,\
                             EVENT_TYPE int,\
                             EVENT_SOURCE int,\
                             PLAY_SOURCE int,\
                             START_SOURCE int default 0,\
                             EXT_START_TIME time,\
                             EXT_LENGTH int,\
                             EXT_CART_NAME char(32),\
                             EXT_DATA char(32),\
                             EXT_EVENT_ID char(8),\
                             EXT_ANNC_TYPE char(8),\
                             index EVENT_DATETIME_IDX(EVENT_DATETIME))",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("select NAME from LOGS where SERVICE=\"%s\"",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      while(q1->next()) {
	tablename=q1->value(0).toString();
	tablename.replace(" ","_");
	sql=QString().sprintf("select %s_REC.LENGTH,\
                               %s_REC.LOG_ID,\
                               %s_REC.CART_NUMBER,\
                               %s_REC.CUT_NUMBER,\
                               %s_REC.STATION_NAME,\
                               %s_REC.EVENT_DATETIME,\
                               %s_REC.EVENT_TYPE,\
                               %s_REC.EVENT_SOURCE,\
                               %s_REC.PLAY_SOURCE,\
                               %s_REC.EXT_START_TIME,\
                               %s_REC.EXT_LENGTH,\
                               %s_REC.EXT_CART_NAME,\
                               %s_REC.EXT_DATA,\
                               %s_REC.EXT_EVENT_ID,\
                               %s_REC.EXT_ANNC_TYPE,\
                               CART.TITLE,\
                               CART.ARTIST \
                               from CART right join %s_REC on \
                               CART.NUMBER=%s_REC.CART_NUMBER",
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename,
			      (const char *)tablename);
	q2=new QSqlQuery(sql);
	while(q2->next()) {
	  sql=QString().sprintf("insert into %s_SRT set\
                                 LENGTH=%d,\
                                 LOG_NAME=\"%s\",\
                                 LOG_ID=%d,\
                                 CART_NUMBER=%u,\
                                 CUT_NUMBER=%d,\
                                 TITLE=\"%s\",\
                                 ARTIST=\"%s\",\
                                 STATION_NAME=\"%s\",\
                                 EVENT_DATETIME=\"%s\",\
                                 SCHEDULED_TIME=\"%s\",\
                                 EVENT_TYPE=%d,\
                                 EVENT_SOURCE=%d,\
                                 PLAY_SOURCE=%d,\
                                 EXT_START_TIME=\"%s\",\
                                 EXT_LENGTH=%d,\
                                 EXT_CART_NAME=\"%s\",\
                                 EXT_DATA=\"%s\",\
                                 EXT_EVENT_ID=\"%s\",\
                                 EXT_ANNC_TYPE=\"%s\"",
				(const char *)q->value(0).toString(),
				q2->value(0).toInt(),
				(const char *)q1->value(0).toString(),
				q2->value(1).toInt(),
				q2->value(2).toUInt(),
				q2->value(3).toInt(),
				(const char *)q2->value(15).toString(),
				(const char *)q2->value(16).toString(),
				(const char *)q2->value(4).toString(),
				(const char *)q2->value(5).toDateTime().
				toString("yyyy-MM-dd hh:mm:ss"),
				"00:00:00",
				q2->value(6).toInt(),
				q2->value(7).toInt(),
				q2->value(8).toInt(),
				(const char *)q2->value(9).toTime().
				toString("hh:mm:ss"),
				q2->value(10).toInt(),
				(const char *)q2->value(11).toString(),
				(const char *)q2->value(12).toString(),
				(const char *)q2->value(13).toString(),
				(const char *)q2->value(14).toString());
	  q3=new QSqlQuery(sql);
	  delete q3;
	}
	delete q2;
      }
      delete q1;
    }
    delete q;

    sql="alter table RDAIRPLAY add column DEFAULT_SERVICE char(10)\
         after PAUSE_ENABLED";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<99) {
    sql="alter table USERS add column CONFIG_PANELS_PRIV enum('N','Y')\
         default 'N' after REMOVEFROM_LOG_PRIV";
    q=new QSqlQuery(sql);
    delete q;
    sql="update USERS set CONFIG_PANELS_PRIV=\"Y\"";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<100) {
    sql="alter table CUTS add column ISRC char(12) after OUTCUE";
    q=new QSqlQuery(sql);
    delete q;
    sql="select NUMBER,ISRC from CART";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update CUTS set ISRC=\"%s\" where CART_NUMBER=%u",
			    (const char *)q->value(1).toString(),
			    q->value(0).toUInt());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<101) {
    for(int i=0;i<RD_MAX_CARDS;i++) {
      sql=QString().sprintf("alter table AUDIO_PORTS \
                             add column INPUT_%d_MODE INT DEFAULT 0 \
                             after INPUT_%d_TYPE", i, i);
      q=new QSqlQuery(sql);
      delete q;
    }
  }

  if(ver<102) {
    sql="alter table CART add column PUBLISHER char(64) after AGENCY";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CART add index PUBLISHER_IDX (PUBLISHER)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add column COMPOSER char(64) after PUBLISHER";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CART add index COMPOSER_IDX (COMPOSER)";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<104) {
    sql="alter table CART add column USAGE_CODE int default 0\
         after USER_DEFINED";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPORTS add column LINES_PER_PAGE int default 66\
         after USE_LEADING_ZEROS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPORTS add column STATION_TYPE int default 0\
         after LINES_PER_PAGE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPORTS add column STATION_FORMAT char(64)\
         after STATION_TYPE";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_SRT add column PUBLISHER char(64)\
                             after ARTIST",(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_SRT add column COMPOSER char(64)\
                             after PUBLISHER",(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_SRT add column ISRC char(12)\
                             after PUBLISHER",(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_SRT add column USAGE_CODE int\
                             default 0 after ISRC",(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<105) {
    for(int i=0;i<6;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column START_RML%d\
                           char(255) after PORT%d",i,i);
      q=new QSqlQuery(sql);
      delete q;
      sql=QString().sprintf("alter table RDAIRPLAY add column STOP_RML%d\
                           char(255) after START_RML%d",i,i);
      q=new QSqlQuery(sql);
      delete q;
    }
  }

  if(ver<106) {
    for(int i=0;i<3;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column LOG_RML%d\
                           char(255) after UDP_STRING%d",i,i);
      q=new QSqlQuery(sql);
      delete q;
    }
  }

  if(ver<107) {
    sql="alter table RDLOGEDIT add column REC_START_CART int unsigned\
         default 0 after END_CART";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table RDLOGEDIT add column REC_END_CART int unsigned\
         default 0 after REC_START_CART";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<108) {
    sql="alter table CART add column AVERAGE_SEGUE_LENGTH int unsigned\
         after LENGTH_DEVIATION";
    q=new QSqlQuery(sql);
    delete q;
    sql="select NUMBER from CART";
    q=new QSqlQuery(sql);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }

  if(ver<109) {
    sql="alter table EVENTS add column NESTED_EVENT char(64) after COLOR";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<110) {
    for(unsigned i=6;i<10;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY add column CARD%d int\
                             default -1 after STOP_RML%d",i,i-1);
      q=new QSqlQuery(sql);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY add column PORT%d int\
                             default -1 after CARD%d",i,i);
      q=new QSqlQuery(sql);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY add column START_RML%d\
                             char(255) after PORT%d",i,i);
      q=new QSqlQuery(sql);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY add column STOP_RML%d\
                             char(255) after START_RML%d",i,i);
      q=new QSqlQuery(sql);
      delete q;
    }
    sql="select ID,CARD2,PORT2,START_RML2,STOP_RML2 from RDAIRPLAY";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update RDAIRPLAY set \
                             CARD6=%d,PORT6=%d,\
                             START_RML6=\"%s\",STOP_RML6=\"%s\",\
                             CARD7=%d,PORT7=%d,\
                             START_RML7=\"%s\",STOP_RML7=\"%s\",\
                             CARD8=%d,PORT8=%d,\
                             START_RML8=\"%s\",STOP_RML8=\"%s\",\
                             CARD9=%d,PORT9=%d,\
                             START_RML9=\"%s\",STOP_RML9=\"%s\"\
                             where ID=%d",
			    q->value(1).toInt(),q->value(2).toInt(),
			    (const char *)q->value(3).toString(),
			    (const char *)q->value(4).toString(),
			    q->value(1).toInt(),q->value(2).toInt(),
			    (const char *)q->value(3).toString(),
			    (const char *)q->value(4).toString(),
			    q->value(1).toInt(),q->value(2).toInt(),
			    (const char *)q->value(3).toString(),
			    (const char *)q->value(4).toString(),
			    q->value(1).toInt(),q->value(2).toInt(),
			    (const char *)q->value(3).toString(),
			    (const char *)q->value(4).toString(),
			    q->value(0).toInt());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<111) {
    sql="alter table CART add column VALIDITY int unsigned default 2\
         after PLAY_ORDER";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table CUTS add column VALIDITY int unsigned default 2\
         after LOCAL_COUNTER";
    q=new QSqlQuery(sql);
    delete q;
    sql="select NUMBER from CART";
    q=new QSqlQuery(sql);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }

  if(ver<112) {
    sql="alter table RDLOGEDIT add column TRIM_THRESHOLD int default -3000 \
         after REC_END_CART";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table RDLOGEDIT add column RIPPER_LEVEL int default -1300 \
         after TRIM_THRESHOLD";
    q=new QSqlQuery(sql);
    delete q;
    sql="select STATION,TRIM_THRESHOLD,RIPPER_LEVEL from RDLIBRARY";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update RDLOGEDIT set TRIM_THRESHOLD=%d,\
                             RIPPER_LEVEL=%d where STATION=\"%s\"",
			    q->value(1).toInt(),q->value(2).toInt(),
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="alter table RDLIBRARY modify RIPPER_LEVEL int default -1300";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDLOGEDIT modify DEFAULT_CHANNELS int unsigned default 1";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<113) {
    sql="alter table VGUEST_RESOURCES modify SURFACE_NUM int default 0";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_BREAK_STRING char(64) \
         after MUS_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_TRACK_STRING char(64) \
         after MUS_BREAK_STRING";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<114) {
    sql="alter table LOGS add column MUSIC_LINKS int default 0 \
         after COMPLETED_TRACKS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column MUSIC_LINKED enum('N','Y') default 'N' \
         after MUSIC_LINKS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column TRAFFIC_LINKS int default 0 \
         after MUSIC_LINKED";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column TRAFFIC_LINKED enum('N','Y') default 'N' \
         after TRAFFIC_LINKS";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG add column\
                             LINK_EVENT_NAME char(64) after POST_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_LOG add column\
                             LINK_START_TIME time after LINK_EVENT_NAME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_LOG add column\
                             LINK_LENGTH int default 0 after LINK_START_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_LOG add column\
                             LINK_ID int default -1 after LINK_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q; 

   sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_PRE add column\
                             LINK_EVENT_NAME char(64) after POST_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_PRE add column\
                             LINK_START_TIME time after LINK_EVENT_NAME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_PRE add column\
                             LINK_LENGTH int default 0 after LINK_START_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_PRE add column\
                             LINK_ID int default -1 after LINK_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST add column\
                             LINK_EVENT_NAME char(64) after POST_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST add column\
                             LINK_START_TIME time after LINK_EVENT_NAME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST add column\
                             LINK_LENGTH int default 0 after LINK_START_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST add column\
                             LINK_ID int default -1 after LINK_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<115) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG modify column LABEL char(64)",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_PRE modify column LABEL char(64)",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST modify column LABEL char(64)",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="alter table SERVICES add column TRACK_GROUP char(10) \
         after CHAIN_LOG";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<116) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG add column LINK_EMBEDDED \
                             enum('N','Y') default 'N' after LINK_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_PRE add column LINK_EMBEDDED \
                             enum('N','Y') default 'N' after LINK_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST add column LINK_EMBEDDED \
                             enum('N','Y') default 'N' after LINK_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<117) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_LOG add column ORIGIN_USER char(8)\
                             after POST_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_LOG add column ORIGIN_DATETIME \
                             datetime after ORIGIN_USER",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;


      sql=QString().sprintf("alter table %s_LOG drop column ORIGIN_NAME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;


    }
    delete q;

    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_PRE add column ORIGIN_USER char(8)\
                             after POST_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_PRE add column ORIGIN_DATETIME \
                             datetime after ORIGIN_USER",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table %s_POST add column ORIGIN_USER \
                             char(8) after POST_TIME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column ORIGIN_DATETIME \
                             datetime after ORIGIN_USER",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;




      sql=QString().sprintf("alter table %s_PRE drop column ORIGIN_NAME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST drop column ORIGIN_NAME",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;


    }
    delete q;
  }

  if(ver<118) {
    sql="alter table SERVICES add column TFC_LABEL_CART char(32) \
         after TFC_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_TRACK_CART char(32) \
         after TFC_LABEL_CART";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_LABEL_CART char(32) \
         after MUS_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_TRACK_CART char(32) \
         after MUS_LABEL_CART";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_TITLE_OFFSET int \
         after TFC_CART_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_TITLE_LENGTH int \
         after TFC_TITLE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_TITLE_OFFSET int \
         after MUS_CART_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_TITLE_LENGTH int \
         after MUS_TITLE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<119) {
    sql="alter table SERVICES add column TFC_HOURS_OFFSET int \
         after TFC_START_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_HOURS_LENGTH int \
         after TFC_HOURS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TFC_MINUTES_OFFSET int \
         after TFC_HOURS_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_MINUTES_LENGTH int \
         after TFC_MINUTES_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TFC_SECONDS_OFFSET int \
         after TFC_MINUTES_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_SECONDS_LENGTH int \
         after TFC_SECONDS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME,TFC_START_OFFSET from SERVICES where TFC_START_LENGTH=8";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update SERVICES set\
                             TFC_HOURS_OFFSET=%d,TFC_HOURS_LENGTH=2,\
                             TFC_MINUTES_OFFSET=%d,TFC_MINUTES_LENGTH=2,\
                             TFC_SECONDS_OFFSET=%d,TFC_SECONDS_LENGTH=2 \
                             where NAME=\"%s\"",
			    q->value(1).toInt(),
			    q->value(1).toInt()+3,
			    q->value(1).toInt()+6,
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;


    sql="alter table SERVICES add column MUS_HOURS_OFFSET int \
         after MUS_START_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_HOURS_LENGTH int \
         after MUS_HOURS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_MINUTES_OFFSET int \
         after MUS_HOURS_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_MINUTES_LENGTH int \
         after MUS_MINUTES_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_SECONDS_OFFSET int \
         after MUS_MINUTES_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_SECONDS_LENGTH int \
         after MUS_SECONDS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;


    sql="select NAME,MUS_START_OFFSET from SERVICES where MUS_START_LENGTH=8";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update SERVICES set\
                             MUS_HOURS_OFFSET=%d,MUS_HOURS_LENGTH=2,\
                             MUS_MINUTES_OFFSET=%d,MUS_MINUTES_LENGTH=2,\
                             MUS_SECONDS_OFFSET=%d,MUS_SECONDS_LENGTH=2 \
                             where NAME=\"%s\"",
			    q->value(1).toInt(),
			    q->value(1).toInt()+3,
			    q->value(1).toInt()+6,
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<120) {
    sql="alter table GROUPS add column COLOR char(7) default \"#000000\" \
         after ENABLE_NOW_NEXT";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDLOGEDIT add column DEFAULT_TRANS_TYPE int default 0 \
         after RIPPER_LEVEL";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<121) {
    sql="alter table LOGS add column LINK_DATETIME datetime not null \
         after ORIGIN_DATETIME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column NEXT_ID int default 0 \
         after TRAFFIC_LINKED";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME,ORIGIN_DATETIME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update LOGS set LINK_DATETIME=\"%s\" \
                             where NAME=\"%s\"",
			    (const char *)q->value(1).toDateTime().
			    toString("yyyy-MM-dd hh:mm:ss"),
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("select ID from `%s_LOG` order by ID",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      if(q1->last()) {
	sql=QString().sprintf("update LOGS set NEXT_ID=%d where NAME=\"%s\"",
			      q1->value(0).toInt()+1,
			      (const char *)q->value(0).toString());
	q2=new QSqlQuery(sql);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }

  if(ver<122) {
    sql="alter table LOGS add column MODIFIED_DATETIME datetime not null \
         after LINK_DATETIME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column AUTO_REFRESH enum('N','Y') default 'N' \
         after MODIFIED_DATETIME";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME,LINK_DATETIME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update LOGS set MODIFIED_DATETIME=\"%s\" \
                             where NAME=\"%s\"",
			    (const char *)q->value(1).toDateTime().
			    toString("yyyy-MM-dd hh:mm:ss"),
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }

    sql="alter table SERVICES add column AUTO_REFRESH enum('N','Y') \
         default 'N' after TRACK_GROUP";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<123) {
    sql="alter table REPORTS add column FILTER_ONAIR_FLAG enum('N','Y') \
         default 'N' after STATION_FORMAT";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("alter table `%s_SRT` add column ONAIR_FLAG \
                             enum('N','Y') default 'N' after START_SOURCE",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<124) {
    sql="alter table SERVICES add column TFC_LEN_HOURS_OFFSET int \
         after TFC_SECONDS_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_LEN_HOURS_LENGTH int \
         after TFC_LEN_HOURS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TFC_LEN_MINUTES_OFFSET int \
         after TFC_LEN_HOURS_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_LEN_MINUTES_LENGTH int \
         after TFC_LEN_MINUTES_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TFC_LEN_SECONDS_OFFSET int \
         after TFC_LEN_MINUTES_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column TFC_LEN_SECONDS_LENGTH int \
         after TFC_LEN_SECONDS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME,TFC_LENGTH_OFFSET,TFC_LENGTH_LENGTH from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      if(q->value(2).toInt()==5) {
	sql=QString().sprintf("update SERVICES set TFC_LEN_MINUTES_OFFSET=%d,\
                               TFC_LEN_MINUTES_LENGTH=2,\
                               TFC_LEN_SECONDS_OFFSET=%d,\
                               TFC_LEN_SECONDS_LENGTH=2 where NAME=\"%s\"",
			      q->value(1).toInt(),q->value(1).toInt()+3,
			      (const char *)q->value(0).toString());
      }
      else {
	sql=QString().sprintf("update SERVICES set TFC_LEN_SECONDS_OFFSET=%d,\
                             TFC_LEN_SECONDS_LENGTH=%d where NAME=\"%s\"",
			      q->value(1).toInt(),q->value(2).toInt(),
			      (const char *)q->value(0).toString());
      }
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="alter table SERVICES add column MUS_LEN_HOURS_OFFSET int \
         after MUS_SECONDS_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_LEN_HOURS_LENGTH int \
         after MUS_LEN_HOURS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_LEN_MINUTES_OFFSET int \
         after MUS_LEN_HOURS_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_LEN_MINUTES_LENGTH int \
         after MUS_LEN_MINUTES_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_LEN_SECONDS_OFFSET int \
         after MUS_LEN_MINUTES_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table SERVICES add column MUS_LEN_SECONDS_LENGTH int \
         after MUS_LEN_SECONDS_OFFSET";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME,MUS_LENGTH_OFFSET,MUS_LENGTH_LENGTH from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      if(q->value(2).toInt()==5) {
	sql=QString().sprintf("update SERVICES set MUS_LEN_MINUTES_OFFSET=%d,\
                               MUS_LEN_MINUTES_LENGTH=2,\
                               MUS_LEN_SECONDS_OFFSET=%d,\
                               MUS_LEN_SECONDS_LENGTH=2 where NAME=\"%s\"",
			      q->value(1).toInt(),q->value(1).toInt()+3,
			      (const char *)q->value(0).toString());
      }
      else {
	sql=QString().sprintf("update SERVICES set MUS_LEN_SECONDS_OFFSET=%d,\
                             MUS_LEN_SECONDS_LENGTH=%d where NAME=\"%s\"",
			      q->value(1).toInt(),q->value(2).toInt(),
			      (const char *)q->value(0).toString());
      }
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<125) {
    sql="alter table REPORTS add column SERVICE_NAME char(64) \
         after LINES_PER_PAGE";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("alter table `%s_SRT` add column ALBUM char(255) \
                             after COMPOSER",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table `%s_SRT` add column LABEL char(64) \
                             after ALBUM",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<126) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("alter table `%s_LOG` add column \
                             LINK_START_SLOP int default 0 after LINK_LENGTH",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table `%s_LOG` add column \
                            LINK_END_SLOP int default 0 after LINK_START_SLOP",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<127) {
    sql="alter table RDAIRPLAY add column \
         PANEL_PAUSE_ENABLED enum('N','Y') default 'N' after FLASH_PANEL";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<128) {
    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table `%s_PRE` add column \
                             LINK_START_SLOP int default 0 after LINK_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table `%s_PRE` add column \
                            LINK_END_SLOP int default 0 after LINK_START_SLOP",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table `%s_POST` add column \
                             LINK_START_SLOP int default 0 after LINK_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString().sprintf("alter table `%s_POST` add column \
                            LINK_END_SLOP int default 0 after LINK_START_SLOP",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<129) {
    sql="alter table RDAIRPLAY add column EXIT_CODE int default 0 \
         after LOG_RML2";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column EXIT_PASSWORD char(41) default \"\" \
         after EXIT_CODE";
    q=new QSqlQuery(sql);
    delete q;


    sql="alter table RDAIRPLAY add column LOG0_START_MODE int default 0 \
         after EXIT_PASSWORD";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_AUTO_RESTART enum('N','Y') \
         default 'N' after LOG0_START_MODE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_LOG_NAME char(64) \
         after LOG0_AUTO_RESTART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_CURRENT_LOG char(64) \
         after LOG0_LOG_NAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_RUNNING enum('N','Y') \
         default 'N' after LOG0_CURRENT_LOG";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_LOG_ID int default -1 \
         after LOG0_RUNNING";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_LOG_LINE int default -1 \
         after LOG0_LOG_ID";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_START_MODE int default 0 \
         after LOG0_LOG_LINE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_AUTO_RESTART enum('N','Y') \
         default 'N' after LOG1_START_MODE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_LOG_NAME char(64) \
         after LOG1_AUTO_RESTART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_CURRENT_LOG char(64) \
         after LOG1_LOG_NAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_RUNNING enum('N','Y') \
         default 'N' after LOG1_CURRENT_LOG";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_LOG_ID int default -1 \
         after LOG1_RUNNING";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_LOG_LINE int default -1 \
         after LOG1_LOG_ID";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_START_MODE int default 0 \
         after LOG1_LOG_LINE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_AUTO_RESTART enum('N','Y') \
         default 'N' after LOG2_START_MODE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_LOG_NAME char(64) \
         after LOG2_AUTO_RESTART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_CURRENT_LOG char(64) \
         after LOG2_LOG_NAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_RUNNING enum('N','Y') \
         default 'N' after LOG2_CURRENT_LOG";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_LOG_ID int default -1 \
         after LOG2_RUNNING";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_LOG_LINE int default -1 \
         after LOG2_LOG_ID";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<130) {
    sql="create table if not exists RDCATCH (\
         ID int unsigned primary key auto_increment,\
         STATION char(64) not null,\
         ERROR_RML char(255),\
         index STATION_IDX (STATION))";
    q=new QSqlQuery(sql);
    delete q;
  }

  if (ver<131) {
    sql="ALTER TABLE `EVENTS` ADD `SCHED_GROUP` VARCHAR(10)";
    q=new QSqlQuery(sql);
    delete q;
    sql="ALTER TABLE `EVENTS` ADD `TITLE_SEP` INT(10) UNSIGNED";
    q=new QSqlQuery(sql);
    delete q;
    sql="ALTER TABLE `EVENTS` ADD `HAVE_CODE` VARCHAR(10)";
    q=new QSqlQuery(sql);
    delete q;
    sql="ALTER TABLE `EVENTS` ADD `HOR_SEP` INT(10) UNSIGNED";
    q=new QSqlQuery(sql);
    delete q;
    sql="ALTER TABLE `EVENTS` ADD `HOR_DIST` INT(10) UNSIGNED";
    q=new QSqlQuery(sql);
    delete q;
    sql="ALTER TABLE `CLOCKS` ADD `ARTISTSEP` INT(10) UNSIGNED";
    q=new QSqlQuery(sql);
    delete q;
    sql="ALTER TABLE `CART` ADD `SCHED_CODES` VARCHAR( 255 ) NULL DEFAULT NULL";
    q=new QSqlQuery(sql);
    delete q;
    sql="create table if not exists SCHED_CODES\
    (CODE varchar(10) not null primary key,\
    DESCRIPTION varchar(255))";
    q=new QSqlQuery(sql);
    delete q;
    sql="drop table SCHED_STACK";
    q=new QSqlQuery(sql);
    delete q;
    sql="select NAME from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      QString svc=q->value(0).toString();
      sql=QString().sprintf("drop table %s_STACK",(const char *)svc.replace(" ","_"));
      printf("%s\n",(const char*)sql);
      q1=new QSqlQuery(sql);
      delete q1;
      }
    delete q;
    sql="select NAME from LOGS";  
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("alter table `%s_LOG` add column \
                             DUCK_UP_GAIN int default 0 after SEGUE_GAIN",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_LOG` add column \
                            DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
    sql="select NAME from EVENTS";  
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("alter table `%s_PRE` add column \
                             DUCK_UP_GAIN int default 0 after SEGUE_GAIN",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_PRE` add column \
                            DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_POST` add column \
                             DUCK_UP_GAIN int default 0 after SEGUE_GAIN",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_POST` add column \
                            DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN",
			    (const char *)q->value(0).toString());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if (ver<132) {
    sql="create table if not exists DROPBOXES \
         (ID int auto_increment not null primary key,\
          STATION_NAME char(64),\
          GROUP_NAME char(10),\
          PATH char(255),\
          NORMALIZATION_LEVEL int default 1,\
          AUTOTRIM_LEVEL int default 1,\
          SINGLE_CART enum('N','Y') default 'N',\
          TO_CART int unsigned default 0,\
          USE_CARTCHUNK_ID enum('N','Y') default 'N',\
          DELETE_CUTS enum('N','Y') default 'N',\
          METADATA_PATTERN char(64),\
          FIX_BROKEN_FORMATS enum('N','Y') default 'N',\
          LOG_PATH char(255),\
          index STATION_NAME_IDX (STATION_NAME))";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<133) {
    sql="select NAME from LOGS";  
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table `%s_LOG` add column \
                             DUCK_UP_GAIN int default 0 after SEGUE_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_LOG` add column \
                            DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
    sql="select NAME from EVENTS";  
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table `%s_PRE` add column \
                             DUCK_UP_GAIN int default 0 after SEGUE_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_PRE` add column \
                            DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_POST` add column \
                             DUCK_UP_GAIN int default 0 after SEGUE_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table `%s_POST` add column \
                            DUCK_DOWN_GAIN int default 0 after DUCK_UP_GAIN",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<134) {
    sql="create table if not exists RDPANEL (\
         ID int not null primary key auto_increment,\
         STATION char(40) not null,\
         INSTANCE int unsigned not null,\
         CARD2 int default -1,\
         PORT2 int default -1,\
         START_RML2 char(255),\
         STOP_RML2 char(255),\
         CARD6 int default -1,\
         PORT6 int default -1,\
         START_RML6 char(255),\
         STOP_RML6 char(255),\
         CARD7 int default -1,\
         PORT7 int default -1,\
         START_RML7 char(255),\
         STOP_RML7 char(255),\
         CARD8 int default -1,\
         PORT8 int default -1,\
         START_RML8 char(255),\
         STOP_RML8 char(255),\
         CARD9 int default -1,\
         PORT9 int default -1,\
         START_RML9 char(255),\
         STOP_RML9 char(255),\
         STATION_PANELS int default 3,\
         USER_PANELS int default 3,\
         CLEAR_FILTER enum('N','Y') default 'N',\
         FLASH_PANEL enum('N','Y') default 'N',\
         PANEL_PAUSE_ENABLED enum('N','Y') default 'N',\
         DEFAULT_SERVICE char(10),\
         index STATION_IDX (STATION,INSTANCE))"; 
    q=new QSqlQuery(sql);
    delete q;

    //
    // Create EXTENDED_PANELS Table
    //
    sql="create table if not exists EXTENDED_PANELS (\
        ID int auto_increment not null primary key,\
        TYPE int not null,\
        OWNER char(64) not null,\
        PANEL_NO int not null,\
        ROW_NO int not null,\
        COLUMN_NO int not null,\
        LABEL char(64),\
        CART int,\
        DEFAULT_COLOR char(7),\
        index LOAD_IDX (TYPE,OWNER,PANEL_NO),\
        index SAVE_IDX (TYPE,OWNER,PANEL_NO,ROW_NO,COLUMN_NO))"; 
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<135) {
    sql="alter table STATIONS add column STARTUP_CART int unsigned default 0 \
         after HEARTBEAT_INTERVAL";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<136) {
    //
    // Create PANEL_NAMES Table
    //
    sql="create table if not exists PANEL_NAMES (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      NAME char(64),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO))"; 
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create EXTENDED_PANEL_NAMES Table
    //
    sql="create table if not exists EXTENDED_PANEL_NAMES (\
      ID int auto_increment not null primary key,\
      TYPE int not null,\
      OWNER char(64) not null,\
      PANEL_NO int not null,\
      NAME char(64),\
      index LOAD_IDX (TYPE,OWNER,PANEL_NO))"; 
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  } 

  if(ver<137) {
    sql="alter table CART add column AVERAGE_HOOK_LENGTH int unsigned \
         default 0 after AVERAGE_SEGUE_LENGTH";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    sql="select NUMBER from CART";
    q=new QSqlQuery(sql);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }
  
  if(ver<138) {
    sql="alter table RDAIRPLAY add column BUTTON_LABEL_TEMPLATE char(32) \
         default \"%t\" after PANEL_PAUSE_ENABLED";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDPANEL add column BUTTON_LABEL_TEMPLATE char(32) \
         default \"%t\" after PANEL_PAUSE_ENABLED";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<139) {
    sql="alter table RDAIRPLAY modify EXIT_PASSWORD char(41) default \"\""; 
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<140) {
    sql="alter table SERVICES add column TFC_BREAK_STRING char(64) \
         after TFC_TRACK_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TFC_TRACK_STRING char(64) \
         after TFC_TRACK_CART";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<141) {
    sql="alter table DROPBOXES add column TITLE_FROM_CARTCHUNK_ID enum('N','Y') \
         default 'N' after USE_CARTCHUNK_ID"; 
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<142) {
    sql="create table if not exists FEEDS (\
         ID int unsigned auto_increment not null primary key,\
         KEY_NAME char(8) unique not null,\
         CHANNEL_TITLE char(255),\
         CHANNEL_DESCRIPTION text,\
         CHANNEL_CATEGORY char(64),\
         CHANNEL_LINK char(255),\
         CHANNEL_COPYRIGHT char(64),\
         CHANNEL_WEBMASTER char(64),\
         CHANNEL_LANGUAGE char(5) default \"en-us\",\
         BASE_URL char(255),\
         PURGE_URL char(255),\
         PURGE_USERNAME char(64),\
         PURGE_PASSWORD char(64),\
         HEADER_XML text,\
         CHANNEL_XML text,\
         ITEM_XML text,\
         MAX_SHELF_LIFE int,\
         LAST_BUILD_DATETIME datetime,\
         ORIGIN_DATETIME datetime,\
         ENABLE_AUTOPOST enum('N','Y') default 'N',\
         index KEY_NAME_IDX(KEY_NAME))";
    q=new QSqlQuery(sql);
    delete q;

    sql="create table if not exists PODCASTS (\
         ID int unsigned auto_increment not null primary key,\
         FEED_ID int unsigned not null,\
         STATUS int unsigned default 0,\
         ITEM_TITLE char(255),\
         ITEM_DESCRIPTION text,\
         ITEM_CATEGORY char(64),\
         ITEM_LINK char(255),\
         AUDIO_FILENAME char(255),\
         AUDIO_LENGTH int unsigned,\
         SHELF_LIFE int,\
         ORIGIN_DATETIME datetime,\
         index FEED_ID_IDX(FEED_ID,ORIGIN_DATETIME))";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RECORDINGS add column FEED_ID int default -1 \
         after URL_PASSWORD";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table VERSION add column LAST_MAINT_DATETIME datetime \
         default \"1970-01-01 00:00:00\" after DB";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<143) {
    sql="create table if not exists AUX_METADATA (\
         ID int unsigned auto_increment not null primary key,\
         FEED_ID int unsigned,\
         VAR_NAME char(16),\
         CAPTION char(64),\
         index FEED_ID_IDX(FEED_ID))";
    q=new QSqlQuery(sql);
    delete q;

    sql="select KEY_NAME from FEEDS"; 
    q=new QSqlQuery(sql);
    while(q->next()) {
      RDCreateAuxFieldsTable(q->value(0).toString());
    }
    delete q;
  }

  if(ver<144) {
    sql="alter table FEEDS add column UPLOAD_FORMAT int default 2 \
         after ENABLE_AUTOPOST";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table FEEDS add column UPLOAD_CHANNELS int default 2 \
         after UPLOAD_FORMAT";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table FEEDS add column UPLOAD_SAMPRATE int default 44100 \
         after UPLOAD_CHANNELS";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table FEEDS add column UPLOAD_BITRATE int default 32000 \
         after UPLOAD_CHANNELS";
    q=new QSqlQuery(sql);
    delete q;
    sql="alter table FEEDS add column UPLOAD_QUALITY int default 0 \
         after UPLOAD_BITRATE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table FEEDS add column NORMALIZE_LEVEL int default -100 \
         after UPLOAD_QUALITY";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table PODCASTS add column ITEM_COMMENTS char(255) \
         after ITEM_LINK";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table PODCASTS add column ITEM_AUTHOR char(255) \
         after ITEM_COMMENTS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table PODCASTS add column ITEM_SOURCE_TEXT char(64) \
         after ITEM_AUTHOR";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table PODCASTS add column ITEM_SOURCE_URL char(255) \
         after ITEM_SOURCE_TEXT";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<145) {
    sql=QString("create table if not exists FEED_PERMS (\
        ID int unsigned auto_increment not null primary key,\
        USER_NAME char(8),\
        KEY_NAME char(8),\
        index USER_IDX (USER_NAME),\
        index KEYNAME_IDX (KEY_NAME))");
    q=new QSqlQuery(sql);
    delete q;

    sql="select LOGIN_NAME from USERS \
         where (ADMIN_USERS_PRIV='N')&&(ADMIN_CONFIG_PRIV='N')";
    q=new QSqlQuery(sql);
    sql="select KEY_NAME from FEEDS";
    q1=new QSqlQuery(sql);
    while(q->next()) {
      while(q1->next()) {
	sql=QString().sprintf("insert into FEED_PERMS set USER_NAME=\"%s\",\
                               KEY_NAME=\"%s\"",
			      (const char *)q->value(0).toString(),
			      (const char *)q1->value(0).toString());
	q2=new QSqlQuery(sql);
	delete q2;
      }
      q1->seek(-1);
    }
    delete q1;
    delete q;


    sql="alter table USERS add column ADD_PODCAST_PRIV enum('N','Y') \
         not null default 'N' after EDIT_CATCHES_PRIV";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table USERS add column EDIT_PODCAST_PRIV enum('N','Y') \
         not null default 'N' after ADD_PODCAST_PRIV";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table USERS add column DELETE_PODCAST_PRIV enum('N','Y') \
         not null default 'N' after EDIT_PODCAST_PRIV";
    q=new QSqlQuery(sql);
    delete q;

    sql="update USERS set ADD_PODCAST_PRIV='Y',EDIT_PODCAST_PRIV='Y',\
         DELETE_PODCAST_PRIV='Y' \
         where (ADMIN_CONFIG_PRIV='N')&&(ADMIN_USERS_PRIV='N')";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<146) {
   sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table %s_POST add column\
                             EXT_CART_NAME char(32) after EXT_LENGTH",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EVENT_SOURCE int default 0 after EVENT_TYPE",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().sprintf("alter table %s_POST add column\
                             EXT_ANNC_TYPE char(8) after EXT_EVENT_ID",
			    (const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<147) {
    sql="alter table USERS add column ENABLE_WEB enum('N','Y') default 'N' \
         after PASSWORD";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("create table if not exists WEB_CONNECTIONS (\
        SESSION_ID int unsigned not null primary key,\
        LOGIN_NAME char(8),\
        IP_ADDRESS char(16),\
        TIME_STAMP datetime)");
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<148) {
    sql=QString().sprintf("alter table CUTS add column\
                           SEGUE_GAIN int default %d after SEGUE_END_POINT",RD_FADE_DEPTH);
    q=new QSqlQuery(sql);
    delete q;
  }
  
  if(ver<148) {
    sql="alter table PODCASTS add column AUDIO_TIME int unsigned \
         after AUDIO_LENGTH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<149) {
  sql="create table if not exists SWITCHER_NODES (\
       ID int not null auto_increment primary key,\
       STATION_NAME char(64),\
       MATRIX int,\
       BASE_OUTPUT int default 0,\
       HOSTNAME char(64),\
       PASSWORD char(64),\
       TCP_PORT int,\
       DESCRIPTION char(255),\
       index STATION_IDX (STATION_NAME,MATRIX))";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table INPUTS add column NODE_HOSTNAME char(255) \
         after DEVICE_NUM";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table INPUTS add column NODE_TCP_PORT int \
         after NODE_HOSTNAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table INPUTS add column NODE_SLOT int \
         after NODE_TCP_PORT";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table add index \
         NODE_IDX(STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table OUTPUTS add column NODE_HOSTNAME char(255) \
         after DEVICE_NUM";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table OUTPUTS add column NODE_TCP_PORT int \
         after NODE_HOSTNAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table OUTPUTS add column NODE_SLOT int \
         after NODE_TCP_PORT";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table add index \
         NODE_IDX(STATION_NAME,MATRIX,NUMBER,NODE_HOSTNAME,NODE_TCP_PORT)";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<150) {
    sql="alter table MATRICES add column PORT_2 int not null after PORT";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column PORT_TYPE_2 int default 0 \
         after PORT_TYPE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column IP_ADDRESS_2 char(16) \
         after IP_ADDRESS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column IP_PORT_2 int after IP_PORT";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column USERNAME_2 char(32) after USERNAME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column PASSWORD_2 char(32) after PASSWORD";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<151) {
    sql="alter table FEEDS add column KEEP_METADATA enum('N','Y') default 'Y' \
         after ENABLE_AUTOPOST";
    q=new QSqlQuery(sql);
    delete q;

    sql="select KEY_NAME from FEEDS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      RDCreateFeedLog(q->value(0).toString());
    }
    delete q;
  }

  if(ver<152) {
    sql="alter table STATIONS add column EDITOR_PATH char(255) default \"\"\
         after STARTUP_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDPANEL add column CARD3 int default -1 \
         after STOP_RML2";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDPANEL add column PORT3 int default -1 \
         after CARD3";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDPANEL add column START_RML3 char(255) \
         after PORT3";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDPANEL add column STOP_RML3 char(255) \
         after START_RML3";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<153) {
    sql="alter table STATIONS add column FILTER_MODE int default 0 \
         after EDITOR_PATH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<154) {
    sql=QString().sprintf("alter table CUTS add column\
                           SEGUE_GAIN int default %d after SEGUE_END_POINT",RD_FADE_DEPTH);
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<155) {
    sql="alter table RDLIBRARY add column ENABLE_EDITOR enum('N','Y') \
         default 'N' after CDDB_SERVER";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<156) {
    sql="alter table MATRICES add column LAYER int default 86 after TYPE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<157) {
    sql="alter table STATIONS \
            add column BROADCAST_SECURITY int unsigned \
            default 0 after BACKUP_LIFE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<158) {  // Convert OVERLAP to SEGUE
    sql="select NAME from LOGS;";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("update %s_LOG set TRANS_TYPE=1,SEGUE_GAIN=0 \
                             where TRANS_TYPE=3",(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<159) {
    sql="alter table RDAIRPLAY add column SKIN_PATH char(255) \
         default \"";
    sql+=RD_DEFAULT_RDAIRPLAY_SKIN;
    sql+="\" after EXIT_PASSWORD";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDPANEL add column SKIN_PATH char(255) \
         default \"";
    sql+=RD_DEFAULT_RDPANEL_SKIN;
    sql+="\" after DEFAULT_SERVICE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<160) {
    //
    // Create ENCODERS Table
    //
    sql="create table if not exists ENCODERS (\
         ID int not null auto_increment primary key,\
         NAME char(32) not null,\
         STATION_NAME char(64),\
         COMMAND_LINE char(255),\
         DEFAULT_EXTENSION char(16),\
         index NAME_IDX(NAME,STATION_NAME))";
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
    sql="create table if not exists ENCODER_BITRATES (\
         ID int not null auto_increment primary key,\
         ENCODER_ID int not null,\
         BITRATES int not null,\
         index ENCODER_ID_IDX(ENCODER_ID))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
    
    //
    // Create ENCODER_CHANNELS Table
    //
    sql="create table if not exists ENCODER_CHANNELS (\
         ID int not null auto_increment primary key,\
         ENCODER_ID int not null,\
         CHANNELS int not null,\
         index ENCODER_ID_IDX(ENCODER_ID))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create ENCODER_SAMPLERATES Table
    //
    sql="create table if not exists ENCODER_SAMPLERATES (\
         ID int not null auto_increment primary key,	  \
         ENCODER_ID int not null,				  \
         SAMPLERATES int not null,\
         index ENCODER_ID_IDX(ENCODER_ID))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<161) {
    sql="alter table FEEDS add column UPLOAD_EXTENSION char(16) default \"mp3\"\
         after UPLOAD_QUALITY";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<162) {
    sql="alter table GPIS alter column MACRO_CART set default 0";
    q=new QSqlQuery(sql);
    delete q;

    sql="create table if not exists GPOS (\
        ID int auto_increment not null primary key,\
        STATION_NAME char(64) not null,\
        MATRIX int not null,\
        NUMBER int not null,\
        MACRO_CART int default 0,\
        index MATRIX_IDX (STATION_NAME,MATRIX,NUMBER))";
    q=new QSqlQuery(sql);
    delete q;

    sql="select STATION_NAME,MATRIX,GPOS from MATRICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      for(int i=0;i<q->value(2).toInt();i++) {
	sql=QString().sprintf("insert into GPOS set \
                               STATION_NAME=\"%s\",\
                               MATRIX=%d,\
                               NUMBER=%d,\
                               MACRO_CART=0",
			      (const char *)q->value(0).toString(),
			      q->value(1).toInt(),
			      i+1);
	q1=new QSqlQuery(sql);
	delete q1;
      }
    }
    delete q;
  }

  if(ver<163) {
    sql="alter table RECORDINGS add column EVENTDATE_OFFSET int default 0 \
         after ENDDATE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<164) {
    sql="create table if not exists NOWNEXT_PLUGINS (\
         ID int auto_increment not null primary key,    \
         STATION_NAME char(64) not null,		     \
         LOG_MACHINE int unsigned not null default 0,   \
         PLUGIN_PATH char(255),\
         PLUGIN_ARG char(255),\
         index STATION_IDX (STATION_NAME,LOG_MACHINE))";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<165) {
    sql="create table if not exists DROPBOX_PATHS (\
        ID int auto_increment not null primary key,\
        DROPBOX_ID int not null,\
        FILE_PATH char(255) not null,\
        FILE_DATETIME datetime,\
        index FILE_PATH_IDX (DROPBOX_ID,FILE_PATH))";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table DROPBOXES add DELETE_SOURCE enum('N','Y') default 'Y' \
         after DELETE_CUTS";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table DROPBOXES add column STARTDATE_OFFSET int default 0 \
         after METADATA_PATTERN";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table DROPBOXES add column ENDDATE_OFFSET int default 0 \
         after STARTDATE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<166) {
    sql="alter table GROUPS add column CUT_SHELFLIFE int default -1 \
         after DEFAULT_HIGH_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table GROUPS add column DEFAULT_TITLE char(255) \
         default \"Imported from %f.%e\" after CUT_SHELFLIFE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column SYSTEM_MAINT enum('N','Y') \
         default 'Y' after FILTER_MODE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column ELR_SHELFLIFE int default -1 \
         after AUTO_REFRESH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table EVENTS add column REMARKS char(255) after NESTED_EVENT";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CLOCKS add column REMARKS char(255) after COLOR";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<167) {
    sql="alter table RDAIRPLAY add column LOG0_NOW_CART int unsigned default 0 \
         after LOG0_LOG_LINE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG0_NEXT_CART int unsigned default 0\
         after LOG0_NOW_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_NOW_CART int unsigned default 0 \
         after LOG1_LOG_LINE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG1_NEXT_CART int unsigned default 0\
         after LOG1_NOW_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_NOW_CART int unsigned default 0 \
         after LOG2_LOG_LINE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column LOG2_NEXT_CART int unsigned default 0\
         after LOG2_NOW_CART";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<168) {
    sql="alter table GPIS add column OFF_MACRO_CART int default 0 \
         after MACRO_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table GPOS add column OFF_MACRO_CART int default 0 \
         after MACRO_CART";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<169) {
    sql="alter table SERVICES add column DEFAULT_LOG_SHELFLIFE int default -1 \
         after AUTO_REFRESH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS add column PURGE_DATE date after END_DATE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<170) {
    sql="alter table USERS modify column LOGIN_NAME char(255) not null";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table USERS modify column FULL_NAME char(255)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table FEED_PERMS modify column USER_NAME char(255)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table LOGS modify column ORIGIN_USER char(255) not null";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().
	sprintf("alter table %s_LOG modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().
	sprintf("alter table %s_PRE modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString().
	sprintf("alter table %s_POST modify column ORIGIN_USER char(255)",
		(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="alter table STATIONS modify column USER_NAME char(255)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS modify column DEFAULT_NAME char(255)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table USER_PERMS modify column USER_NAME char(255)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table WEB_CONNECTIONS modify column LOGIN_NAME char(255)";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<171) {
    sql="alter table FEEDS add column CAST_ORDER enum('N','Y') default 'N' \
         after ITEM_XML";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table PODCASTS add column EFFECTIVE_DATETIME datetime \
         after ORIGIN_DATETIME";
    q=new QSqlQuery(sql);
    delete q;

    sql="select ID,ORIGIN_DATETIME from PODCASTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("update PODCASTS set EFFECTIVE_DATETIME=\"%s\" \
                             where ID=%u",
			    (const char *)q->value(1).toDateTime().
			    toString("yyyy-MM-dd hh:mm:ss"),
			    q->value(0).toUInt());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<172) {
    sql="alter table FEEDS add column REDIRECT_PATH char(255) \
         after NORMALIZE_LEVEL";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<173) {
    sql="alter table FEEDS add column BASE_PREAMBLE char(255) after BASE_URL";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<174) {
    sql="alter table MATRICES add column START_CART int unsigned \
         after PASSWORD_2";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column STOP_CART int unsigned \
         after START_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column START_CART_2 int unsigned \
         after STOP_CART";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table MATRICES add column STOP_CART_2 int unsigned \
         after START_CART_2";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<175) {
    sql="create table if not exists SYSTEM (\
         ID int auto_increment not null primary key,		\
         DUP_CART_TITLES enum('N','Y') not null default 'Y')";
    q=new QSqlQuery(sql);
    delete q;
    sql="insert into SYSTEM set DUP_CART_TITLES=\"Y\"";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<176) {
    sql="alter table RDAIRPLAY add column \
         SHOW_COUNTERS enum('N','Y') default 'N' after SKIN_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table RDAIRPLAY add column AUDITION_PREROLL int default 10000 \
         after SHOW_COUNTERS";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<177) {
    sql="alter table RDPANEL add column SKIN_PATH char(255) \
         default \"";
    sql+=RD_DEFAULT_RDPANEL_SKIN;
    sql+="\" after DEFAULT_SERVICE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table PANELS drop column SKIN_PATH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<178) {
    sql="alter table PODCASTS modify column STATUS int unsigned default 1";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString().sprintf("alter table SYSTEM add column \
                           MAX_POST_LENGTH int unsigned default %u \
                           after DUP_CART_TITLES",
			  RD_DEFAULT_MAX_POST_LENGTH);
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<179) {
    sql="alter table FEEDS add column MEDIA_LINK_MODE int default 0 \
         after REDIRECT_PATH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<180) {
    sql="alter table RDLIBRARY add column SRC_CONVERTER int default 1 \
         after ENABLE_EDITOR";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<181) {
    sql=QString().sprintf("select NUMBER from CART where TYPE=%u",
			  RDCart::Audio);
    q=new QSqlQuery(sql);
    while(q->next()) {
      cart=new RDCart(q->value(0).toUInt());
      cart->updateLength();
      delete cart;
    }
    delete q;
  }

  if(ver<182) {
    sql="alter table CART add column NOTES text after MACROS";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<183) {
    sql="create table if not exists IMPORT_TEMPLATES (\
         NAME char(64) not null primary key,\
         CART_OFFSET int,\
         CART_LENGTH int,\
         TITLE_OFFSET int,\
         TITLE_LENGTH int,\
         HOURS_OFFSET int,\
         HOURS_LENGTH int,\
         MINUTES_OFFSET int,\
         MINUTES_LENGTH int,\
         SECONDS_OFFSET int,\
         SECONDS_LENGTH int,\
         LEN_HOURS_OFFSET int,\
         LEN_HOURS_LENGTH int,\
         LEN_MINUTES_OFFSET int,\
         LEN_MINUTES_LENGTH int,\
         LEN_SECONDS_OFFSET int,\
         LEN_SECONDS_LENGTH int,\
         LENGTH_OFFSET int,\
         LENGTH_LENGTH int,\
         DATA_OFFSET int,\
         DATA_LENGTH int,\
         EVENT_ID_OFFSET int,\
         EVENT_ID_LENGTH int,\
         ANNC_TYPE_OFFSET int,\
         ANNC_TYPE_LENGTH int)";
    q=new QSqlQuery(sql);
    delete q;
    UpdateImportFormats();

    sql="alter table SERVICES add column TFC_IMPORT_TEMPLATE char(64)\
         default \"Rivendell Standard Import\" after TFC_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_IMPORT_TEMPLATE char(64)\
         default \"Rivendell Standard Import\" after MUS_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="update SERVICES set TFC_IMPORT_TEMPLATE=\"\"";
    q=new QSqlQuery(sql);
    delete q;

    sql="update SERVICES set MUS_IMPORT_TEMPLATE=\"\"";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<184) {
    UpdateImportFormats();
  }  

  if(ver<185) {
    sql="alter table SERVICES add column TFC_PREIMPORT_CMD text \
         after TFC_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column TFC_WIN_PREIMPORT_CMD text \
         after TFC_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_PREIMPORT_CMD text \
         after MUS_PATH";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table SERVICES add column MUS_WIN_PREIMPORT_CMD text \
         after MUS_WIN_PATH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<186) {
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      UpdateLogTable(tablename+"_LOG");
    }
    delete q;
    
    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      UpdateLogTable(tablename+"_PRE");
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      UpdateLogTable(tablename+"_POST");
    }

    sql="select NAME from CLOCKS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      ConvertTimeField(tablename+"_CLK","START_TIME");
    }
    delete q;
  }

  if(ver<187) {
    sql="alter table CUTS add column ISCI char(32) after ISRC";
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from SERVICES";
    q=new QSqlQuery(sql);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString().sprintf("alter table `%s_SRT` add column ISCI char(32)\
                             after ISRC",(const char *)tablename);
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<188) {
  }

  if(ver<189) {
    sql="alter table CUTS add column UPLOAD_DATETIME datetime \
         after LAST_PLAY_DATETIME";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<190) {
    sql="alter table STATIONS add column HAVE_TWOLAME enum('N','Y') default 'N'\
         after HAVE_FLAC";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<191) {
    sql="alter table SERVICES add column PROGRAM_CODE char(255) \
         after NAME_TEMPLATE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<192) {
    sql="alter table RECORDINGS add column EXIT_TEXT text after EXIT_CODE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<193) {
    sql="alter table STATIONS alter column IPV4_ADDRESS \
         set default \"127.0.0.2\"";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<194) {
    sql=QString().sprintf("alter table SYSTEM add column \
                           SAMPLE_RATE int unsigned default %d after ID",
			  RD_DEFAULT_SAMPLE_RATE);;
    q=new QSqlQuery(sql);
    delete q;
    sql="select DEFAULT_SAMPRATE from RDLIBRARY";
    q=new QSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("update SYSTEM set SAMPLE_RATE=%u",
			    q->value(0).toUInt());
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }
  if(ver<195) {
    sql="alter table RECORDINGS add column ENABLE_METADATA enum('N','Y') \
         default 'N' after URL_PASSWORD";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<196) {
  sql="alter table DROPBOXES add column IMPORT_CREATE_DATES enum('N','Y') default 'N' \
       after LOG_PATH";
  q=new QSqlQuery(sql);
  delete q;
  sql="alter table DROPBOXES add column CREATE_STARTDATE_OFFSET int default 0 \
       after IMPORT_CREATE_DATES";
  q=new QSqlQuery(sql);
  delete q;
  sql="alter table DROPBOXES add column CREATE_ENDDATE_OFFSET int default 0 \
       after CREATE_STARTDATE_OFFSET";
  q=new QSqlQuery(sql);
  delete q;
  }

  if(ver<197) {
    sql=QString().sprintf("create table if not exists REPLICATORS (\
                           NAME char(32) not null primary key,\
                           DESCRIPTION char(64),\
                           TYPE_ID int unsigned not null,\
                           STATION_NAME char(64),\
                           FORMAT int unsigned default 0,\
                           CHANNELS int unsigned default 2,\
                           SAMPRATE int unsigned default %u,\
                           BITRATE int unsigned default 0,\
                           QUALITY int unsigned default 0,\
                           URL char(255),\
                           URL_USERNAME char(64),\
                           URL_PASSWORD char(64),\
                           ENABLE_METADATA enum('N','Y') default 'N',\
                           NORMALIZATION_LEVEL int unsigned default 0,\
                           index TYPE_ID_IDX (TYPE_ID))",
			  RD_DEFAULT_SAMPLE_RATE);
    q=new QSqlQuery(sql);
    delete q;

    sql="create table if not exists REPLICATOR_MAP (\
        ID int unsigned not null auto_increment primary key,\
        REPLICATOR_NAME char(32) not null,\
        GROUP_NAME char(10) not null,\
        index REPLICATOR_NAME_IDX(REPLICATOR_NAME),\
        index GROUP_NAME_IDX(GROUP_NAME))";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add column METADATA_DATETIME datetime after NOTES";
    q=new QSqlQuery(sql);
    delete q;
    sql="update CART set METADATA_DATETIME=now()";
    q=new QSqlQuery(sql);
    delete q;

    //
    // Create REPL_CART_STATE Table
    //
    sql="create table if not exists REPL_CART_STATE (\
         ID int unsigned not null auto_increment primary key,\
         REPLICATOR_NAME char(32) not null,\
         CART_NUMBER int unsigned not null,\
         ITEM_DATETIME datetime not null,\
         unique REPLICATOR_NAME_IDX(REPLICATOR_NAME,CART_NUMBER))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    //
    // Create REPL_CUT_STATE Table
    //
    sql="create table if not exists REPL_CUT_STATE (\
         ID int unsigned not null auto_increment primary key,\
         REPLICATOR_NAME char(32) not null,\
         CUT_NAME char(12) not null,\
         ITEM_DATETIME datetime not null,\
         index REPLICATOR_NAME_IDX(REPLICATOR_NAME),\
         index CUT_NAME_IDX(CUT_NAME))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<198) {
    sql="create table if not exists ISCI_XREFERENCE (\
         ID int unsigned not null auto_increment primary key,\
         CART_NUMBER int unsigned not null,\
         ISCI char(32) not null,\
         FILENAME char(64) not null,\
         LATEST_DATE date not null,\
         TYPE char(1) not null,\
         ADVERTISER_NAME char(30),\
         PRODUCT_NAME char(35),\
         CREATIVE_TITLE char(30),\
         REGION_NAME char(80),\
         index CART_NUMBER_IDX(CART_NUMBER))";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    sql="alter table VERSION add column LAST_ISCI_XREFERENCE datetime \
         default \"1970-01-01 00:00:00\" after LAST_MAINT_DATETIME";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    sql="alter table SYSTEM add column ISCI_XREFERENCE_PATH char(255) \
         after MAX_POST_LENGTH";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<199) {
    sql="create index TYPE_IDX on ISCI_XREFERENCE (TYPE,LATEST_DATE)";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    sql="create index LATEST_DATE_IDX on ISCI_XREFERENCE (LATEST_DATE)";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<200) {
  }

  if(ver<201) {
    sql="alter table STATIONS add column HTTP_STATION char(64) \
         default \"localhost\" after IPV4_ADDRESS";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }

    sql="alter table STATIONS add column CAE_STATION char(64) \
         default \"localhost\" after HTTP_STATION";
    if(!RunQuery(sql)) {
      return UPDATEDB_QUERY_FAILED;
    }
  }

  if(ver<202) {
    sql=QString("CREATE TABLE IF NOT EXISTS RDHOTKEYS (\
          ID int unsigned not null auto_increment primary key,	\
          STATION_NAME         CHAR(64),   \
          MODULE_NAME          CHAR(64),   \
          KEY_ID               int,        \
          KEY_VALUE            CHAR(64),   \
          KEY_LABEL            CHAR(64)) ");
    q=new QSqlQuery(sql);
    delete q;

    sql="select NAME from STATIONS";
    q=new QSqlQuery(sql);
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
    q=new QSqlQuery(sql);
    delete q;
    sql=
      "alter table REPLICATORS add column NORMALIZATION_LEVEL int default 0 \
       after ENABLE_METADATA";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPL_CART_STATE add column REPOST enum('N','Y') \
         default 'N' after ITEM_DATETIME";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPL_CART_STATE add column POSTED_FILENAME char(255) \
         after CART_NUMBER";
    q=new QSqlQuery(sql);
    delete q;

    sql="drop index REPLICATOR_NAME_IDX on REPL_CART_STATE";
    q=new QSqlQuery(sql);
    delete q;

    sql="create unique index REPLICATOR_NAME_IDX on REPL_CART_STATE \
         (REPLICATOR_NAME,CART_NUMBER,POSTED_FILENAME)";
    q=new QSqlQuery(sql);
    delete q;

    sql="delete from REPL_CART_STATE where POSTED_FILENAME is null";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<204) {
    sql="insert into IMPORT_TEMPLATES set\
         NAME=\"The Traffic Light\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=69,\
         DATA_LENGTH=32";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<205) {
    sql="alter table STATIONS add column START_JACK enum('N','Y') default 'N' \
         after FILTER_MODE";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table STATIONS add column JACK_SERVER_NAME char(64) \
         after START_JACK";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<206) {
    sql=QString("create table if not exists REPORT_GROUPS (\
                 ID int unsigned auto_increment not null primary key,\
                 REPORT_NAME char(64) not null,\
                 GROUP_NAME char(10),\
                 index IDX_REPORT_NAME (REPORT_NAME))");
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table REPORTS add column FILTER_GROUPS enum('N','Y') \
         default 'N' after FILTER_ONAIR_FLAG";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<207) {
    sql="insert into IMPORT_TEMPLATES set\
         NAME=\"WideOrbit Traffic\",\
         CART_OFFSET=10,\
         CART_LENGTH=6,\
         TITLE_OFFSET=25,\
         TITLE_LENGTH=34,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=60,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=63,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=66,\
         LEN_SECONDS_LENGTH=2,\
         EVENT_ID_OFFSET=69,\
         EVENT_ID_LENGTH=32,\
         DATA_OFFSET=102,\
         DATA_LENGTH=32";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<208) {
    sql="alter table RDAIRPLAY add column HOUR_SELECTOR_ENABLED enum('N','Y') \
         default 'N' after DEFAULT_SERVICE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<209) {
    sql="alter table STATIONS add column JACK_COMMAND_LINE char(255) \
         after JACK_SERVER_NAME";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("create table if not exists JACK_CLIENTS (\
                 ID int unsigned auto_increment not null primary key,\
                 STATION_NAME char(64) not null,\
                 DESCRIPTION char(64),\
                 COMMAND_LINE char(255) not null,\
                 index IDX_STATION_NAME (STATION_NAME))");
    q=new QSqlQuery(sql);
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
      "index STATION_NAME_IDX(STATION_NAME,SLOT_NUMBER))";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<211) {
    sql=QString("alter table SYSTEM add column TEMP_CART_GROUP char(10) ")+
      "default \"TEMP\" after ISCI_XREFERENCE_PATH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<212) {
    sql=QString("alter table CARTSLOTS add column HOOK_MODE int default 0 ")+
      "after DEFAULT_CART_NUMBER";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CARTSLOTS add column DEFAULT_HOOK_MODE int ")+
      "default -1 after HOOK_MODE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<213) {
    sql=QString("alter table STATIONS add column CUE_CARD int default 0 ")+
      "after JACK_COMMAND_LINE";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table STATIONS add column CUE_PORT int default 0 ")+
      "after CUE_CARD";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table STATIONS add column CARTSLOT_COLUMNS int ")+
      "default 1 after CUE_PORT";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table STATIONS add column CARTSLOT_ROWS int ")+
      "default 8 after CARTSLOT_COLUMNS";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("select STATION_NAME,CARD_NUMBER,PORT_NUMBER from DECKS ")+
      "where CHANNEL=0";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString("update STATIONS set ")+
	QString().sprintf("CUE_CARD=%d,",q->value(1).toInt())+
	QString().sprintf("CUE_PORT=%d ",q->value(2).toInt())+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<214) {
    sql=QString("alter table SERVICES add column AUTOSPOT_GROUP char(10) ")+
      "after TRACK_GROUP";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<215) {
    sql=QString("alter table GROUPS add column DELETE_EMPTY_CARTS ")+
      "enum('N','Y') default 'N' after CUT_SHELFLIFE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<216) {
    sql="alter table CUTS add index ISCI_IDX(ISCI)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CUTS add index ISRC_IDX(ISRC)";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<217) {
    sql=QString("create table if not exists LIVEWIRE_GPIO_SLOTS (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MATRIX int not null,"+
      "SLOT int not null,"+
      "SOURCE_NUMBER int,"+
      "index STATION_NAME_IDX(STATION_NAME,MATRIX))";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<218) {
    sql=QString("alter table LIVEWIRE_GPIO_SLOTS ")+
      "add column IP_ADDRESS char(15) after SLOT";
    q=new QSqlQuery(sql);
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
      "index STATION_NAME_IDX(STATION_NAME,INSTANCE))";
    q=new QSqlQuery(sql);
    delete q;

    for(unsigned i=0;i<10;i++) {
      sql=QString().
	sprintf("select STATION,CARD%u,PORT%u,START_RML%u,STOP_RML%u ",
		i,i,i,i)+
	"from RDAIRPLAY";
      q=new QSqlQuery(sql);
      while(q->next()) {
	sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("INSTANCE=%u,",i)+
	  QString().sprintf("CARD=%d,",q->value(1).toInt())+
	  QString().sprintf("PORT=%d,",q->value(2).toInt())+
	  "START_RML=\""+RDEscapeString(q->value(3).toString())+"\","+
	  "STOP_RML=\""+RDEscapeString(q->value(4).toString())+"\"";
	q1=new QSqlQuery(sql);
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
      "index STATION_NAME_IDX(STATION_NAME,INSTANCE))";
    q=new QSqlQuery(sql);
    delete q;

    for(unsigned i=0;i<10;i++) {
      if((i==2)||(i==3)||(i==6)||(i==7)||(i==8)||(i==9)) {
	sql=QString().
	  sprintf("select STATION,CARD%u,PORT%u,START_RML%u,STOP_RML%u ",
		  i,i,i,i)+
	  "from RDPANEL";
	q=new QSqlQuery(sql);
	while(q->next()) {
	  sql=QString("insert into RDPANEL_CHANNELS set ")+
	    "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	    QString().sprintf("INSTANCE=%u,",i)+
	    QString().sprintf("CARD=%d,",q->value(1).toInt())+
	    QString().sprintf("PORT=%d,",q->value(2).toInt())+
	    "START_RML=\""+RDEscapeString(q->value(3).toString())+"\","+
	    "STOP_RML=\""+RDEscapeString(q->value(4).toString())+"\"";
	  q1=new QSqlQuery(sql);
	  delete q1;
	}
	delete q;
      }
    }

    //
    // Clean Up RDAirPlay
    //
    sql="alter table RDAIRPLAY drop column INSTANCE";
    q=new QSqlQuery(sql);
    delete q;
    for(unsigned i=0;i<10;i++) {
      sql=QString().sprintf("alter table RDAIRPLAY drop column CARD%u",i);
      q=new QSqlQuery(sql);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column PORT%u",i);
      q=new QSqlQuery(sql);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column START_RML%u",i);
      q=new QSqlQuery(sql);
      delete q;

      sql=QString().sprintf("alter table RDAIRPLAY drop column STOP_RML%u",i);
      q=new QSqlQuery(sql);
      delete q;
    }
    
    //
    // Clean Up RDPanel
    //
    sql="alter table RDPANEL drop column INSTANCE";
    q=new QSqlQuery(sql);
    delete q;
    for(unsigned i=0;i<10;i++) {
      if((i==2)||(i==3)||(i==6)||(i==7)||(i==8)||(i==9)) {
	sql=QString().sprintf("alter table RDPANEL drop column CARD%u",i);
	q=new QSqlQuery(sql);
	delete q;

	sql=QString().sprintf("alter table RDPANEL drop column PORT%u",i);
	q=new QSqlQuery(sql);
	delete q;

	sql=QString().sprintf("alter table RDPANEL drop column START_RML%u",i);
	q=new QSqlQuery(sql);
	delete q;

	sql=QString().sprintf("alter table RDPANEL drop column STOP_RML%u",i);
	q=new QSqlQuery(sql);
	delete q;
      }
    }
    
  }

  if(ver<220) {
    sql=QString("alter table RDAIRPLAY_CHANNELS add column GPIO_TYPE ")+
      "int unsigned default 0 after STOP_RML";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table RDPANEL_CHANNELS add column GPIO_TYPE ")+
      "int unsigned default 0 after STOP_RML";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<221) {
    sql="alter table RDLIBRARY modify column TRIM_THRESHOLD int default 0";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<222) {
    sql="insert into IMPORT_TEMPLATES set\
         NAME=\"NaturalLog\",\
         CART_OFFSET=9,\
         CART_LENGTH=6,\
         TITLE_OFFSET=19,\
         TITLE_LENGTH=40,\
         HOURS_OFFSET=0,\
         HOURS_LENGTH=2,\
         MINUTES_OFFSET=3,\
         MINUTES_LENGTH=2,\
         SECONDS_OFFSET=6,\
         SECONDS_LENGTH=2,\
         LEN_HOURS_OFFSET=61,\
         LEN_HOURS_LENGTH=2,\
         LEN_MINUTES_OFFSET=64,\
         LEN_MINUTES_LENGTH=2,\
         LEN_SECONDS_OFFSET=67,\
         LEN_SECONDS_LENGTH=2,\
         DATA_OFFSET=0,\
         DATA_LENGTH=0";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<223) {
    sql="alter table CART add column CONDUCTOR char(64) after LABEL";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add index CONDUCTOR_IDX(CONDUCTOR)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add column SONG_ID char(32) after USER_DEFINED";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add index SONG_ID_IDX(SONG_ID)";
    q=new QSqlQuery(sql);
    delete q;

    sql="alter table CART add column BPM int unsigned default 0 after SONG_ID";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<224) {
    sql=QString("alter table DROPBOXES add column SET_USER_DEFINED char(255) ")+
      "after CREATE_ENDDATE_OFFSET";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<225) {
    sql=QString("alter table RDAIRPLAY add column TITLE_TEMPLATE char(64) ")+
      "default '%t' after HOUR_SELECTOR_ENABLED";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ARTIST_TEMPLATE char(64) ")+
      "default '%a' after TITLE_TEMPLATE";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table RDAIRPLAY add column OUTCUE_TEMPLATE char(64) ")+
      "default '%o' after ARTIST_TEMPLATE";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table RDAIRPLAY add column DESCRIPTION_TEMPLATE char(64) ")+
      "default '%i' after HOUR_SELECTOR_ENABLED";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<226) {
    sql=QString("alter table RDLOGEDIT add column ")+
      "ENABLE_SECOND_START enum('N','Y') default 'Y' after BITRATE";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<227) {
    sql="alter table LOGS add index TYPE_IDX(TYPE,LOG_EXISTS)";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<228) {
    sql=QString("alter table RDLIBRARY add column ")+
      "LIMIT_SEARCH int default 1 after SRC_CONVERTER";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table RDLIBRARY add column ")+
      "SEARCH_LIMITED enum('N','Y') default 'Y' after LIMIT_SEARCH";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<229) {
    sql=QString("alter table SERVICES add column ")+
      "DESCRIPTION_TEMPLATE char(255) after NAME_TEMPLATE";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("select NAME from SERVICES");
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString("update SERVICES set DESCRIPTION_TEMPLATE=\"")+
	RDEscapeString(q->value(0).toString())+" log for %m/%d/%Y\" "+
	"where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<230) { 
    sql="select NAME from LOGS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql="alter table `"+RDLog::tableName(q->value(0).toString())+
	"` add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql="select NAME from EVENTS";
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql="alter table `"+RDEvent::preimportTableName(q->value(0).toString())+
	"` add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      q1=new QSqlQuery(sql);
      delete q1;

      sql="alter table `"+RDEvent::postimportTableName(q->value(0).toString())+
	"` add column EVENT_LENGTH int default -1 after ORIGIN_DATETIME";
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
 }

  if(ver<231) { 
    sql=QString("alter table CART add column ")+
      "USE_EVENT_LENGTH enum('N','Y') default 'N' after METADATA_DATETIME";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<232) { 
    sql=QString("alter table STATIONS add column ")+
      "ENABLE_DRAGDROP enum('N','Y') default 'Y' after CARTSLOT_ROWS";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      "ENFORCE_PANEL_SETUP enum('N','Y') default 'N' after ENABLE_DRAGDROP";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<233) { 
    sql=QString("alter table RDAIRPLAY add column ")+
      "LOG_MODE_STYLE int default 0 after START_MODE";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("create table if not exists LOG_MODES (")+
      "ID int unsigned auto_increment not null primary key,"+
      "STATION_NAME char(64) not null,"+
      "MACHINE int unsigned not null,"+
      "START_MODE int not null default 0,"+
      "OP_MODE int not null default 2,"+
      "index STATION_NAME_IDX(STATION_NAME,MACHINE))";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("select STATION,START_MODE,OP_MODE from RDAIRPLAY");
    q=new QSqlQuery(sql);
    while(q->next()) {
      for(unsigned i=0;i<3;i++) {
	sql=QString("insert into LOG_MODES set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("MACHINE=%u,",i)+
	  QString().sprintf("START_MODE=%d,",q->value(1).toInt())+
	  QString().sprintf("OP_MODE=%d",q->value(2).toInt());
	q1=new QSqlQuery(sql);
	delete q1;
      }
    }
    delete q;
  }

  if(ver<234) { 
    sql=QString("alter table STATIONS add column ")+
      "CUE_START_CART int unsigned after CUE_PORT";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      "CUE_STOP_CART int unsigned after CUE_START_CART";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<235) { 
    // Lock Locking Changes, Superceded
  }

  if(ver<236) {
    sql=QString("select NAME from SERVICES");
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column CONDUCTOR char(64) after LABEL";
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column USER_DEFINED char(255) after COMPOSER";
      q1=new QSqlQuery(sql);
      delete q1;

      sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
	"` add column SONG_ID char(32) after USER_DEFINED";
      q1=new QSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  if(ver<237) {
    sql=QString("alter table REPORTS add column ")+
      "START_TIME time after FILTER_GROUPS";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table REPORTS add column ")+
      "END_TIME time after START_TIME";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<238) {
    sql=QString("alter table CART add column ")+
      "PENDING_STATION char(64) after USE_EVENT_LENGTH";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CART add column ")+
      "PENDING_PID int after PENDING_STATION";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CART add column ")+
      "PENDING_DATETIME datetime after PENDING_PID";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CART add index ")+
      "PENDING_STATION_IDX(PENDING_STATION)";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CART add index ")+
      "PENDING_PID_IDX(PENDING_STATION,PENDING_PID)";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CART add index ")+
      "PENDING_DATETIME_IDX(PENDING_DATETIME)";
    q=new QSqlQuery(sql);
    delete q;
  }

  if(ver<239) {
    sql=QString("create table if not exists DROPBOX_SCHED_CODES(")+
      "ID int auto_increment not null primary key,"+
      "DROPBOX_ID int not null,"+
      "SCHED_CODE char(11) not null,"
      "index DROPBOX_ID_IDX(DROPBOX_ID),"+
      "index SCHED_CODE_IDX(SCHED_CODE))";
    q=new QSqlQuery(sql);
    delete q;
  }


      
  // **** End of version updates ****
  
  //
  // Update Version Field
  //
  q=new QSqlQuery(QString().sprintf("update VERSION set DB=%d",
				    RD_VERSION_DATABASE));
  delete q;
  return UPDATEDB_SUCCESS;
}
