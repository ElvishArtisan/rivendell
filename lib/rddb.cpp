// rddb.cpp
//
//   Database driver with automatic reconnect
//
//   (C) Copyright 2007 Dan Mills <dmills@exponent.myzen.co.uk>
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QSqlError>
#include <QStringList>
#include <QVariant>

#include "dbversion.h"
#include "rdcart.h"
#include "rdcreate_log.h"
#include "rdcut.h"
#include "rddb.h"
#include "rddbheartbeat.h"
#include "rdescape_string.h"
#include "rdgroup.h"
#include "rdpaths.h"
#include "rdstation.h"
#include "rdsvc.h"
#include "rduser.h"

RDSqlQuery::RDSqlQuery (const QString &query):
  QSqlQuery(query)
{
  sql_columns=0;

  if(isActive()) {
    QStringList f0=query.split(" ");
    if(f0[0].toLower()=="select") {
      for(int i=1;i<f0.size();i++) {
	if(f0[i].toLower()=="from") {
	  QString fields;
	  for(int j=1;j<i;j++) {
	    fields+=f0[j];
	  }
	  QStringList f1=fields.split(",");
	  sql_columns=f1.size();
	  continue;
	}
      }
    }
  }
  else {
    QString err=QObject::tr("invalid SQL or failed DB connection")+
      +"["+lastError().text()+"]: "+query;

    fprintf(stderr,"%s\n",(const char *)err);
#ifndef WIN32
    syslog(LOG_ERR,(const char *)err);
#endif  // WIN32
  }
}


int RDSqlQuery::columns() const
{
  return sql_columns;
}


bool RDOpenDb (int *schema,QString *err_str,RDConfig *config)
{
  QSqlDatabase db;
  QString sql;
  QSqlQuery *q;

  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(config->mysqlDriver());
    if(!db.isValid()) {
      *err_str+= QString(QObject::tr("Couldn't initialize QSql driver!"));
      return false;
    }
    db.setHostName(config->mysqlHostname());
    db.setDatabaseName(config->mysqlDbname());
    db.setUserName(config->mysqlUsername());
    db.setPassword(config->mysqlPassword());
    if(!db.open()) {
      *err_str+=QString(QObject::tr("Couldn't open mySQL connection on"))+
	" \""+config->mysqlHostname()+"\".";
      db.removeDatabase(config->mysqlDbname());
      db.close();
      return false;
    }
  }
  new RDDbHeartbeat(config->mysqlHeartbeatInterval());
  sql=QString("set NAMES '")+config->mysqlCharset()+"'"+
    "COLLATE '"+config->mysqlCollation()+"'";
  q=new QSqlQuery(sql);
  delete q;

  *schema=-1;
  sql=QString("show tables where ")+
    "Tables_in_"+config->mysqlDbname()+"=\"VERSION\"";
  q=new QSqlQuery(sql);
  if(q->first()) {
    delete q;
    q=new QSqlQuery("select DB from VERSION");
    if(q->first()) {
      *schema=q->value(0).toUInt();
    }
  }
  else {
    delete q;
    sql=QString("show tables");
    q=new QSqlQuery(sql);
    if(!q->first()) {
      *schema=0;
    }
  }
  delete q;

  return true;
}


bool RunQuery(QString *err_str,QString sql)
{
  QSqlQuery *q=new QSqlQuery(sql);
  if(!q->isActive()) {
    *err_str=q->lastError().databaseText()+" ["+sql+"]";
    delete q;
    return false;
  }
  delete q;
  return true;
}


bool InsertRDAirPlayHotkey(QString *err_str,const QString &station,
			   const QString &label,int id)
{
  QString sql=QString("insert into RDHOTKEYS set ")+
    "STATION_NAME=\""+RDEscapeString(station)+"\","+
    "MODULE_NAME=\"airplay\","+
    QString().sprintf("KEY_ID=%d,",id)+
    "KEY_LABEL=\""+RDEscapeString(label)+"\"";
  if(!RunQuery(err_str,sql)) {
    return false;
  }
  return true;
}


bool UpdateRDAirplayHotkeys(QString *err_str,QString station)
{
  bool ret=true;
  QString sql;

  for(int i=0;i<7;i++) {
    ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Start Line")+
				  QString().sprintf(" %d",i+1),3*i+1);
    ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Stop Line")+
				  QString().sprintf(" %d",i+1),3*i+2);
    ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Pause Line")+
				  QString().sprintf(" %d",i+1),3*i+3);
  }
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Add"),22);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Delete"),23);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Copy"),24);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Move"),25);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Sound Panel"),26);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Main Log"),27);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Aux Log 1"),28);
  ret=ret&&InsertRDAirPlayHotkey(err_str,station,QObject::tr("Aux Log 2"),29);

  return ret;
}


void UpdateImportFormats()
{
  QString sql;
  QSqlQuery *q;

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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
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
  q=new QSqlQuery(sql);
  delete q;
}


bool RDInitDb(QString *err_str,RDConfig *config)
{
  QString sql;
  RDUser *user=NULL;

  //
  // Create Default Admin Account
  //
  if(!RDUser::create(RDA_LOGIN_NAME)) {
    return false;
  }
  user=new RDUser(RDA_LOGIN_NAME);
  user->setPassword(RDA_PASSWORD);
  user->setFullName(RDA_FULLNAME);
  user->setDescription(RDA_DESCRIPTION);
  user->setAdminConfig(true);
  delete user;

  //
  // Create Default User Account
  //
  if(!RDUser::create(RD_USER_LOGIN_NAME)) {
    return false;
  }
  user=new RDUser(RD_USER_LOGIN_NAME);
  user->setPassword(RD_USER_PASSWORD);
  user->setFullName(RD_USER_FULL_NAME);
  user->setDescription(RD_USER_DESCRIPTION);
  user->setCreateCarts(true);
  user->setDeleteCarts(true);
  user->setModifyCarts(true);
  user->setEditAudio(true);
  user->setCreateLog(true);
  user->setDeleteLog(true);
  user->setDeleteRec(true);
  user->setPlayoutLog(true);
  user->setArrangeLog(true);
  user->setAddtoLog(true);
  user->setRemovefromLog(true);
  user->setConfigPanels(true);
  user->setVoicetrackLog(true);
  user->setEditCatches(true);
  user->setModifyTemplate(true);
  user->setAddPodcast(true);
  user->setEditPodcast(true);
  user->setDeletePodcast(true);
  delete user;

  //
  // Create Default Workstation
  //
  struct hostent *hostent=gethostbyname(config->stationName());
  if(!RDStation::create(config->stationName())) {
    return false;
  }
  RDStation *station=new RDStation(config->stationName());
  station->setDescription(RD_STATION_DESCRIPTION);
  station->setUserName(RD_USER_LOGIN_NAME);
  if(hostent!=NULL) {
    station->
      setAddress(QHostAddress(QString().sprintf("%d.%d.%d.%d",
						0xFF&hostent->h_addr[0],
						0xFF&hostent->h_addr[1],
						0xFF&hostent->h_addr[2],
						0xFF&hostent->h_addr[3])));
  }
  delete station;

  //
  // Create Test Tone Cart/Cut
  //
  QString filename=
    QString().sprintf("%s/999999_001.%s",
		      config->audioRoot().ascii(),
		      config->audioExtension().ascii());

  QString cmd=QString().sprintf("rdgen -t 10 -l 16 %s",
				(const char *)filename);
  system((const char *)cmd);
  if(getuid()==0) {
    chown(filename,config->uid(),config->gid());
    chmod (filename,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  }

  RDCart *cart=new RDCart(999999);
  if(!cart->create("TEST",RDCart::Audio)) {
    return false;
  }
  cart->setTitle(QObject::tr("Test Tone"));
  cart->setArtist(QObject::tr("Rivendell Radio Automation"));
  cart->setForcedLength(10000);

  cart->addCut(0,0,2,"",QObject::tr("1 kHz at Reference Level [-16 dBFS]"));
  RDCut *cut=new RDCut(999999,1);
  cut->setOutcue("["+QObject::tr("tone")+"]");
  RDSettings *s=new RDSettings();
  s->setFormat(RDSettings::Pcm16);
  s->setSampleRate(RD_DEFAULT_SAMPLE_RATE);
  s->setChannels(2);
  cut->checkInRecording("RDGen",s,10000);
  delete s;
  delete cut;
  delete cart;

  //
  // Create Clipboard Entry
  //
  // FIXME: Make this per-host!
  //
  sql=QString("insert into CLIPBOARD set ")+
    "CUT_NAME=\"clip\","+
    "CART_NUMBER=0,"+
    "DESCRIPTION=\"Default Clipboard\"";
  if(!RunQuery(err_str,sql)) {
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

  
  for(const struct Group *g=group;g->group!= NULL; g++) {
    RDGroup::create(g->group,true,true);
    RDGroup *group=new RDGroup(g->group);
    group->setDescription(g->description);
    if(g->macro) {
      group->setDefaultCartType(RDCart::Macro);
    }
    else {
      group->setDefaultCartType(RDCart::Audio);
    }
    group->setDefaultLowCart(g->start);
    group->setDefaultHighCart(g->end);
    group->setExportReport(RDGroup::Traffic,g->rpt_traffic);
    group->setExportReport(RDGroup::Music,g->rpt_music);
    group->setEnableNowNext(g->now_next);
    delete group;
    // Add it to the user permissions table for the default user
    sql=QString().sprintf("insert into USER_PERMS (USER_NAME,GROUP_NAME) \
                         values (\"%s\",\"%s\")",RD_USER_LOGIN_NAME,g->group);
    if(!RunQuery(err_str,sql)) {
      return false;
    }
    // Add it to the audio permsmissions table
    sql=QString().sprintf("insert into AUDIO_PERMS (GROUP_NAME,SERVICE_NAME) \
                         values (\"%s\",\"%s\")",g->group,RD_SERVICE_NAME);
    if(!RunQuery(err_str,sql)) {
      return false;
    }
  }

  //
  // Create Sample Log
  //
  sql=RDCreateLogTableSql("SAMPLE_LOG");
  if(!RunQuery(err_str,sql)) {
    return false;
  }
  sql=QString("insert into LOGS ")+
    "(NAME,SERVICE,DESCRIPTION,ORIGIN_USER,ORIGIN_DATETIME,MODIFIED_DATETIME) "+
    "values (\"SAMPLE\",\"Production\",\"Sample Log\",\"user\",now(),now())";
  if(!RunQuery(err_str,sql)) {
    return false;
  }

  //
  // Generate Hotkey Definitions
  //
  if (!UpdateRDAirplayHotkeys(err_str,RD_STATION_NAME)) {
      return false;
  }


  return true;
}


bool RDMakeDb(QString *err_str,RDConfig *config)
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
    "ENABLE_WEB enum('N','Y') default 'N',"+
    "ADMIN_USERS_PRIV enum('N','Y') not null default 'N',"+
    "ADMIN_CONFIG_PRIV enum('N','Y') not null default 'N',"+
    "CREATE_CARTS_PRIV enum('N','Y') not null default 'N',"+
    "DELETE_CARTS_PRIV enum('N','Y') not null default 'N',"+
    "MODIFY_CARTS_PRIV enum('N','Y') not null default 'N',"+
    "EDIT_AUDIO_PRIV enum('N','Y') not null default 'N',"+
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
    "index FULL_NAME_IDX (FULL_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create STATIONS table
//
  sql=QString("create table if not exists STATIONS (")+
    "NAME char(64) primary key not null,"+
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
    "CARD0_DRIVER int(11) default 0,"+
    "CARD0_NAME char(64),"+
    "CARD0_INPUTS int default -1,"+
    "CARD0_OUTPUTS int default -1,"+
    "CARD1_DRIVER int(11) default 0,"+
    "CARD1_NAME char(64),"+
    "CARD1_INPUTS int default -1,"+
    "CARD1_OUTPUTS int default -1,"+
    "CARD2_DRIVER int(11) default 0,"+
    "CARD2_NAME char(64),"+
    "CARD2_INPUTS int default -1,"+
    "CARD2_OUTPUTS int default -1,"+
    "CARD3_DRIVER int(11) default 0,"+
    "CARD3_NAME char(64),"+
    "CARD3_INPUTS int default -1,"+
    "CARD3_OUTPUTS int default -1,"+
    "CARD4_DRIVER int(11) default 0,"+
    "CARD4_NAME char(64),"+
    "CARD4_INPUTS int default -1,"+
    "CARD4_OUTPUTS int default -1,"+
    "CARD5_DRIVER int(11) default 0,"+
    "CARD5_NAME char(64),"+
    "CARD5_INPUTS int default -1,"+
    "CARD5_OUTPUTS int default -1,"+
    "CARD6_DRIVER int(11) default 0,"+
    "CARD6_NAME char(64),"+
    "CARD6_INPUTS int default -1,"+
    "CARD6_OUTPUTS int default -1,"+
    "CARD7_DRIVER int(11) default 0,"+
    "CARD7_NAME char(64),"+
    "CARD7_INPUTS int default -1,"+
    "CARD7_OUTPUTS int default -1,"+
    "index DESCRIPTION_IDX (DESCRIPTION))"+ 
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
    "ISRC char(12),"+
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
    "SCHED_CODES varchar( 255 ) NULL default NULL,"+
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
  if(!RunQuery(err_str,sql)) {
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
    "SEGUE_GAIN int default "+QString().sprintf("%d",RD_FADE_DEPTH)+","+
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
    "TFC_START_OFFSET int,"+
    "TFC_START_LENGTH int,"+
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
    "MUS_START_OFFSET int,"+
    "MUS_START_LENGTH int,"+
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create AUDIO_PERMS table
//
  sql=QString("create table if not exists AUDIO_PERMS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "GROUP_NAME char(10),"+
    "SERVICE_NAME char(10),"+
    "index GROUP_IDX (GROUP_NAME),"+
    "index SERVICE_IDX (SERVICE_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create RDLIBRARY table
//
  sql=QString("create table if not exists RDLIBRARY (")+
    "ID int unsigned primary key auto_increment,"+
    "STATION char(40) not null,"+
    "INSTANCE int unsigned not null,"+
    "INPUT_CARD int default 0,"+
    "INPUT_STREAM int default 0,"+
    "INPUT_PORT int default 0,"+
    "INPUT_TYPE enum('A','D') default 'A',"+
    "OUTPUT_CARD int default 0,"+
    "OUTPUT_STREAM int default 0,"+
    "OUTPUT_PORT int default 0,"+
    "VOX_THRESHOLD int default -5000,"+
    "TRIM_THRESHOLD int default 0,"+
    "RECORD_GPI int default -1,"+
    "PLAY_GPI int default -1,"+
    "STOP_GPI int default -1,"+
    "DEFAULT_FORMAT int unsigned default 0,"+
    "DEFAULT_CHANNELS int unsigned default 2,"+
    "DEFAULT_SAMPRATE int unsigned default 44100,"+
    "DEFAULT_LAYER int unsigned default 0,"+
    "DEFAULT_BITRATE int unsigned default 0,"+
    "DEFAULT_RECORD_MODE int unsigned default 0,"+
    "DEFAULT_TRIM_STATE enum('N','Y') default 'N',"+
    "MAXLENGTH INT,"+
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create TRIGGERS table
//
  sql=QString("create table if not exists TRIGGERS (")+
    "ID int unsigned auto_increment not null primary key,"+
    "CUT_NAME char(12),"+
    "TRIGGER_CODE int unsigned,"+
    "OFFSET int unsigned,"+
    "index CUT_NAME_IDX (CUT_NAME))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create TTYS table
//
  sql=QString("create table if not exists TTYS (")+
    "ID int unsigned not null primary key auto_increment,"+
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create DECKS table
//
  sql=QString("create table if not exists DECKS (")+
    "ID int unsigned not null primary key auto_increment,"+
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
    "DEFAULT_SAMPRATE int default 44100,"+
    "DEFAULT_BITRATE int default 0,"+
    "DEFAULT_THRESHOLD int default 0,"+
    "SWITCH_STATION char(64),"+
    "SWITCH_MATRIX int default -1,"+
    "SWITCH_OUTPUT int default -1,"+
    "SWITCH_DELAY int default 0,"+
    "index STATION_NAME_IDX (STATION_NAME),"+
    "index CHANNEL_IDX (CHANNEL))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create RECORDINGS table
//
  sql=QString("create table if not exists RECORDINGS (")+
    "ID int unsigned not null primary key auto_increment,"+
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
    "SAMPRATE int default 44100,"+
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create AUDIO_PORTS table
//
  sql=QString("create table if not exists AUDIO_PORTS (")+
    "ID int unsigned not null primary key auto_increment,"+
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

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
    "index NAME_IDX (NAME,LOG_EXISTS),"+
    "index SERVICE_IDX (SERVICE),"+
    "index DESCRIPTION_IDX (DESCRIPTION),"+
    "index ORIGIN_USER_IDX (ORIGIN_USER),"+
    "index START_DATE_IDX (START_DATE),"+
    "index END_DATE_IDX (END_DATE),"+
    "index TYPE_IDX(TYPE,LOG_EXISTS))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

//
// Create VERSION table
//
  sql=QString("create table if not exists VERSION (")+
  " DB int not null primary key,"+
  " LAST_MAINT_DATETIME datetime default \"1970-01-01 00:00:00\","+
  " LAST_ISCI_XREFERENCE datetime default \"1970-01-01 00:00:00\")"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

  //
  // Create RDAIRPLAY Table
  //
  sql=QString("create table if not exists RDAIRPLAY (")+
    "ID int not null primary key auto_increment,"+
    "STATION char(40) not null,"+
    "CARD0 int default 0,"+
    "PORT0 int default 0,"+
    "START_RML0 char(255),"+
    "STOP_RML0 char(255),"+
    "CARD1 int default 0,"+
    "PORT1 int default 0,"+
    "START_RML1 char(255),"+
    "STOP_RML1 char(255),"+
    "CARD2 int default 0,"+
    "PORT2 int default 0,"+
    "START_RML2 char(255),"+
    "STOP_RML2 char(255),"+
    "CARD3 int default 0,"+
    "PORT3 int default 0,"+
    "START_RML3 char(255),"+
    "STOP_RML3 char(255),"+
    "CARD4 int default 0,"+
    "PORT4 int default 0,"+
    "START_RML4 char(255),"+
    "STOP_RML4 char(255),"+
    "CARD5 int default 0,"+
    "PORT5 int default 0,"+
    "START_RML5 char(255),"+
    "STOP_RML5 char(255),"+
    "CARD6 int default 0,"+
    "PORT6 int default 0,"+
    "START_RML6 char(255),"+
    "STOP_RML6 char(255),"+
    "CARD7 int default 0,"+
    "PORT7 int default 0,"+
    "START_RML7 char(255),"+
    "STOP_RML7 char(255),"+
    "CARD8 int default 0,"+
    "PORT8 int default 0,"+
    "START_RML8 char(255),"+
    "STOP_RML8 char(255),"+
    "CARD9 int default 0,"+
    "PORT9 int default 0,"+
    "START_RML9 char(255),"+
    "STOP_RML9 char(255),"+
    "SEGUE_LENGTH int default 250,"+
    "TRANS_LENGTH int default 50,"+
    "OP_MODE int default 2,"+
    "START_MODE int default 0,"+
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
    "BUTTON_LABEL_TEMPLATE char(32) default \"%t\","+
    "PAUSE_ENABLED enum('N','Y'),"+
    "DEFAULT_SERVICE char(10),"+
    "HOUR_SELECTOR_ENABLED enum('N','Y') default 'N',"+
    "TITLE_TEMPLATE char(64) default '%t',"+
    "ARTIST_TEMPLATE char(64) default '%a',"+
    "OUTCUE_TEMPLATE char(64) default '%o',"+
    "DESCRIPTION_TEMPLATE char(64) default '%i',"+
    "UDP_ADDR0 char(255),"+
    "UDP_PORT0 int unsigned,"+
    "UDP_STRING0 char(255),"+
    "LOG_RML0 char(255),"+
    "UDP_ADDR1 char(255),"+
    "UDP_PORT1 int unsigned,"+
    "UDP_STRING1 char(255),"+
    "LOG_RML1 char(255),"+
    "UDP_ADDR2 char(255),"+
    "UDP_PORT2 int unsigned,"+
    "UDP_STRING2 char(255),"+
    "LOG_RML2 char(255),"+
    "EXIT_CODE int default 0,"+
    "EXIT_PASSWORD char(41) default \"\","+
    "SKIN_PATH char(255) default \""+
    RDEscapeString(RD_DEFAULT_RDAIRPLAY_SKIN)+"\","+
    "SHOW_COUNTERS enum('N','Y') default 'N',"+
    "AUDITION_PREROLL int default 10000,"+
    "LOG0_START_MODE int default 0,"+
    "LOG0_AUTO_RESTART enum('N','Y') default 'N',"+
    "LOG0_LOG_NAME char(64),"+
    "LOG0_CURRENT_LOG char(64),"+
    "LOG0_RUNNING enum('N','Y') default 'N',"+
    "LOG0_LOG_ID int default -1,"+
    "LOG0_LOG_LINE int default -1,"+
    "LOG0_NOW_CART int unsigned default 0,"+
    "LOG0_NEXT_CART int unsigned default 0,"+
    "LOG1_START_MODE int default 0,"+
    "LOG1_AUTO_RESTART enum('N','Y') default 'N',"+
    "LOG1_LOG_NAME char(64),"+
    "LOG1_CURRENT_LOG char(64),"+
    "LOG1_RUNNING enum('N','Y') default 'N',"+
    "LOG1_LOG_ID int default -1,"+
    "LOG1_LOG_LINE int default -1,"+
    "LOG1_NOW_CART int unsigned default 0,"+
    "LOG1_NEXT_CART int unsigned default 0,"+
    "LOG2_START_MODE int default 0,"+
    "LOG2_AUTO_RESTART enum('N','Y') default 'N',"+
    "LOG2_LOG_NAME char(64),"+
    "LOG2_CURRENT_LOG char(64),"+
    "LOG2_RUNNING enum('N','Y') default 'N',"+
    "LOG2_LOG_ID int default -1,"+
    "LOG2_LOG_LINE int default -1,"+
    "LOG2_NOW_CART int unsigned default 0,"+
    "LOG2_NEXT_CART int unsigned default 0,"+
    "index STATION_IDX (STATION))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
    "PORT int not null,"+
    "PORT_2 int not null,"+
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
    "INPUTS int not null,"+
    "OUTPUTS int not null,"+
    "GPIS int not null,"+
    "GPOS int not null,"+
    "FADERS int default 0,"+
    "DISPLAYS int default 0,"+
    "index MATRIX_IDX (STATION_NAME,MATRIX))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
    "SCHED_GROUP varchar(10),"+
    "TITLE_SEP INT(10) UNSIGNED,"+
    "HAVE_CODE varchar(10),"+
    "HAVE_CODE2 varchar(10),"+
    "HOR_SEP INT(10) UNSIGNED,"+
    "HOR_DIST INT(10) UNSIGNED,"+
    "NESTED_EVENT char(64),"+
    "REMARKS char(255))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
    "SAMPRATE int unsigned default 44100,"+
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

  //
  // Create SCHED_CODES Table
  //
  sql=QString("create table if not exists SCHED_CODES (")+
    "CODE varchar(10) not null primary key,"+
    "DESCRIPTION varchar(255))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
    return false;
  }

  //
  // Create DROPBOXES Table
  //
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
  if(!RunQuery(err_str,sql)) {
	  return false;
  }

  //
  // Create RDPANEL Table
  //
  sql=QString("create table if not exists RDPANEL (")+
    "ID int not null primary key auto_increment,"+
    "STATION char(40) not null,"+
    "INSTANCE int unsigned not null,"+
    "CARD2 int default 0,"+
    "PORT2 int default 0,"+
    "START_RML2 char(255),"+
    "STOP_RML2 char(255),"+
    "CARD3 int default 0,"+
    "PORT3 int default 0,"+
    "START_RML3 char(255),"+
    "STOP_RML3 char(255),"+
    "CARD6 int default 0,"+
    "PORT6 int default 0,"+
    "START_RML6 char(255),"+
    "STOP_RML6 char(255),"+
    "CARD7 int default 0,"+
    "PORT7 int default 0,"+
    "START_RML7 char(255),"+
    "STOP_RML7 char(255),"+
    "CARD8 int default 0,"+
    "PORT8 int default 0,"+
    "START_RML8 char(255),"+
    "STOP_RML8 char(255),"+
    "CARD9 int default 0,"+
    "PORT9 int default 0,"+
    "START_RML9 char(255),"+
    "STOP_RML9 char(255),"+
    "STATION_PANELS int default 3,"+
    "USER_PANELS int default 3,"+
    "CLEAR_FILTER enum('N','Y') default 'N',"+
    "FLASH_PANEL enum('N','Y') default 'N',"+
    "PANEL_PAUSE_ENABLED enum('N','Y') default 'N',"+
    "BUTTON_LABEL_TEMPLATE char(32) default \"%t\","+
    "DEFAULT_SERVICE char(10),"+
    "SKIN_PATH char(255) default \""+RD_DEFAULT_RDPANEL_SKIN+"\","+
    "index STATION_IDX (STATION,INSTANCE))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }
  // Ensure that dynamic format IDs start after 100
  sql="insert into ENCODERS set ID=100,NAME=\"dummy\"";
  if(!RunQuery(err_str,sql)) {
    return false;
  }
  sql="delete from ENCODERS where ID=100";
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
    QString().sprintf("MAX_POST_LENGTH int unsigned default %u,",
    RD_DEFAULT_MAX_POST_LENGTH)+
    "ISCI_XREFERENCE_PATH char(255),"+
    "TEMP_CART_GROUP char(10))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
    return false;
  }

  //
  // Create RDHOTKEYS table
  //
  sql=QString("create table if not exists RDHOTKEYS (")+
    "ID int unsigned not null auto_increment primary key,"+
    "STATION_NAME char(64),"+
    "MODULE_NAME char(64),"+
    "KEY_ID int,"+
    "KEY_VALUE char(64),"+
    "KEY_LABEL char(64))"+
    config->createTablePostfix();
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
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
  if(!RunQuery(err_str,sql)) {
     return false;
  }

  //
  // Write Schema Version
  //
  sql=QString().sprintf("insert into VERSION set DB=%u",
			RD_VERSION3_BASE_DATABASE);
  if(!RunQuery(err_str,sql)) {
     return false;
  }

  //
  // Create System Entry
  //
  sql=QString("insert into SYSTEM set ID=1");
  if(!RunQuery(err_str,sql)) {
     return false;
  }

  return true;
}


bool RDUpdateDb(QString *err_str,int schema,RDConfig *config)
{
  return true;
}
