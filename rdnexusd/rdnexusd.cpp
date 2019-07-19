// rdnexusd.cpp
//
// The Rivendell MusicMaster Nexus Daemon
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>

#include <QApplication>
#include <QFileInfo>
#include <QFile>

#include "rdnexusd.h"
#include "rdapplication.h"
#include "rdconf.h"
#include "rdcart.h"
#include "rdhttpserver.h"
#include "rddatedecode.h"
#include "rdescape_string.h"
#include "rdlog.h"
#include "rdlog_event.h"

void SigHandler(int signum)
{
  pid_t local_pid;

  switch(signum) {
    case SIGINT:
    case SIGTERM:
      syslog(LOG_NOTICE,"exiting");
      RDDeletePid(RD_PID_DIR,RDNEXUSD_PID);
      exit(0);
      break;

    case SIGCHLD:
      local_pid=waitpid(-1,NULL,WNOHANG);
      while(local_pid>0) {
        local_pid=waitpid(-1,NULL,WNOHANG);
      }
      signal(SIGCHLD,SigHandler);
      return;

    default:
      break;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;

  nexusd_app=QFileInfo(qApp->arguments().at(0)).fileName();

  nexusd_debug=false;
  nexusd_automerge=false;
  nexusd_name="";
  nexusd_server="";
  nexusd_port=RDNEXUS_DEFAULT_PORT;
  nexusd_station="";
  nexusd_category="";
  nexusd_service="";
  nexusd_group="";
  nexusd_schedcode="";
  nexusd_svc=NULL;
  nexusd_enable_http_server=false;
  nexusd_http_port=RDNEXUS_DEFAULT_PORT;

  //
  // Open the Database
  //
  rda=new RDApplication("rdnexusd","rdnexusd",RDNEXUSD_USAGE,this);
  if(!rda->open(&err_msg,&err_type,false)) {
    fprintf(stderr,"%s: %s\n",qPrintable(nexusd_app),qPrintable(err_msg));
    exit(1);
  }

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),nexusd_app,RDNEXUSD_USAGE);

  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--auto-merge") {
      nexusd_automerge=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--debug") {
      nexusd_debug=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--name") {
      nexusd_server=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--server") {
      nexusd_server=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--port") {
      nexusd_port=cmd->value(i).toInt();
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--service") {
      nexusd_service=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--station") {
      nexusd_station=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--group") {
      nexusd_group=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--schedcode") {
      nexusd_schedcode=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--category") {
      nexusd_category=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--enable-http-server") {
      nexusd_enable_http_server=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--http-port") {
      nexusd_http_port=cmd->value(i).toInt();
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"%s: unknown command option \"%s\"\n",
              qPrintable(nexusd_app),
	      qPrintable(rda->cmdSwitch()->key(i)));
      exit(2);
    }
  }

  if(nexusd_server.isEmpty()) {
    qFatal("%s: aborting - no Nexus server specified",qPrintable(nexusd_app));
    exit(1);
  }

#if 0
  if(nexusd_station.isEmpty()) {
    qFatal("%s: aborting - no Nexus station specified",qPrintable(nexusd_app));
    exit(1);
  }
#endif

  if(nexusd_name.isEmpty()) {
    nexusd_name=nexusd_server;
  }

#if 0
  if(nexusd_enable_http_server&&nexusd_group.isEmpty()) {
    qFatal("%s: aborting - '--enable-http-server' requires '--group'",qPrintable(nexusd_app));
    exit(1);
  }
#endif

  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGCHLD,SigHandler);

#if 0
  if(!RDWritePid(RD_PID_DIR,RDNEXUSD_PID)) {
    qFatal("%s: aborting - can't write pid file",qPrintable(nexusd_app));
    exit(1);
  }
#endif

#if 0
  //
  // Rivendell Service
  //
  if(!nexusd_service.isEmpty()) {
    nexusd_svc=new RDSvc(nexusd_service,rda->station(),rda->config(),this);

    if(!nexusd_svc->exists()) {
      qFatal("%s: aborting - service '%s' does not exist",qPrintable(nexusd_app),qPrintable(nexusd_service));
    }
  }
#endif

  //
  // Subscribe to receive notifications
  //
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
          this,SLOT(notificationReceivedSlot(RDNotification *)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Connect to Nexus
  //
  nexus=new RDNexus();

  nexus->setDebug(nexusd_debug);

  nexus->setServer(nexusd_server,nexusd_port);
#if 0
  nexus->setStation(nexusd_station);
#endif

  //
  // Start Nexus HTTP Server for Push Requests
  //
  if(nexusd_enable_http_server) {
    nexus->startHttpServer(nexusd_http_port);
    connect(nexus,SIGNAL(publishMetaData(QString &,RDNexusIdList &)),this,SLOT(publishMetaDataSlot(QString &,RDNexusIdList &)));
    connect(nexus,SIGNAL(publishSchedule(QString &,RDNexusElementList &)),this,SLOT(publishScheduleSlot(QString &,RDNexusElementList &)));
    syslog(LOG_INFO,QString("Started HTTP server on port %1").arg(nexusd_http_port));
  }

  //
  // Start publish timer
  //
  nexusd_timer = new QTimer(this);
  connect(nexusd_timer, SIGNAL(timeout()), this, SLOT(queueTimerSlot()));
  nexusd_timer->setSingleShot(true);
  nexusd_timer->start(RDNEXUSD_QUEUE_TIMER);

  syslog(LOG_NOTICE,"started");
}

//
// Run timer events
//
void MainObject::queueTimerSlot()
{
  fprintf(stderr,"qTimerSlot()\n");

  processNotificationQueue();

  nexusd_timer->start(RDNEXUSD_QUEUE_TIMER);
}

//
// Handles "publishMetadata" XML commands from Nexus
//
// Receives a nexus station and list of songids that have
// been updated in MusicMaster that need to be
// applied to Rivendell carts.
//
void MainObject::publishMetaDataSlot(QString &station,RDNexusIdList &list)
{
  RDNexusSongInfoList songlist;
  RDNexusStationMap stationmap;

  syslog(LOG_INFO,QString("%1:%2 has %3 metadata update(s) available").arg(nexusd_name).arg(station).arg(list.size()));
  fputs(qPrintable(QString("%1: %2 has %3 metadata update(s) available\n").arg(nexusd_name).arg(station).arg(list.size())),stderr);

  nexus->stationMapForStation(station,stationmap);

  if(stationmap.service.isEmpty()) {
    syslog(LOG_ERR,QString("No Rivendell service configured for Nexus station '%1'").arg(station));
    return;
  }

  nexusd_station=station;
  nexusd_service=stationmap.service;
  nexusd_group=stationmap.group;
  nexusd_schedcode=stationmap.schedcode;

  fprintf(stderr,"%s\n",
    qPrintable(QString("nexusd_station='%1' nexusd_service='%2' nexusd_group='%3' nexusd_schedcode='%4'")
      .arg(nexusd_station)
      .arg(nexusd_service)
      .arg(nexusd_group)
      .arg(nexusd_schedcode)));

  //
  // Set Rivendell Service and Nexus Station
  //
  nexus->setStation(nexusd_station);
#if 0
  nexus->setService(nexusd_service);
#endif

  nexus->getSongInfo(list,RDNexus::SongId,songlist);

  for(int i=0;i<songlist.size();i++) {
    RDNexusSongInfo info=songlist.at(i);
    updateCart(info);
  }
}

//
// Handles "publishSchedule" XML commands from Nexus
//
void MainObject::publishScheduleSlot(QString &station,RDNexusElementList &list)
{
  RDNexusStationMap stationmap;

  syslog(LOG_INFO,QString("%1:%2 has %3 log element update(s) available").arg(nexusd_name).arg(station).arg(list.size()));
  fputs(qPrintable(QString("%1:%2 has %3 log element update(s) available\n").arg(nexusd_name).arg(station).arg(list.size())),stderr);

  if (!list.size()) {
    return;
  }

  nexus->stationMapForStation(station,stationmap);

  if(stationmap.service.isEmpty()) {
    syslog(LOG_ERR,QString("No Rivendell service configured for Nexus station '%1'").arg(station));
    return;
  }

  nexusd_station=station;
  nexusd_service=stationmap.service;
  nexusd_group=stationmap.group;
  nexusd_schedcode=stationmap.schedcode;
  nexusd_automerge=stationmap.automerge;

  fprintf(stderr,"%s\n",
    qPrintable(QString("nexusd_station='%1' nexusd_service='%2' nexusd_group='%3' nexusd_schedcode='%4'")
      .arg(nexusd_station)
      .arg(nexusd_service)
      .arg(nexusd_group)
      .arg(nexusd_schedcode)));

  //
  // Obtain Rivendell Service
  //
  RDSvc *svc=new RDSvc(nexusd_service,rda->station(),rda->config(),this);

  if(!svc->exists()) {
    syslog(LOG_ERR,QString("%1: service '%s' does not exist").arg(nexusd_app).arg(nexusd_service));
    return;
  }

  //
  // Set Rivendell Service and Nexus Station
  //
  nexus->setStation(nexusd_station);
#if 0
  nexus->setService(nexusd_service);
#endif

  //
  // Obtain log date from first element
  //
  QDate logdate=QDate::fromString(list.at(0).airdate,RDNEXUS_DATE);
  QString logname=RDDateDecode(svc->nameTemplate(),logdate,rda->station(),rda->config(),svc->name());

  //
  // Generate new music import file
  //
  generateLogFile(svc,logdate,nexusd_service,list);

  RDLog *log=new RDLog(logname);

  if(log->exists()) {
    if(log->linkState(RDLog::SourceMusic)==RDLog::LinkDone) {
      syslog(LOG_INFO,QString("logname='%1' has been merged.").arg(logname));
      if(nexusd_automerge) {
        syslog(LOG_INFO,"Auto reconcile enabled.");
        reconcileLog(svc,log,logdate,list);
      }
    }
    else {
      syslog(LOG_INFO,QString("logname='%1' has not been merged.").arg(logname));
      if(nexusd_automerge) {
        syslog(LOG_INFO,"Auto merge enabled.");
        mergeLog(svc,logdate,logname);
      }
    }
  }

  delete log;
  delete svc;
}

//
// Handles notifications from RIPCD
//
void MainObject::notificationReceivedSlot(RDNotification *notify)
{
  syslog(LOG_INFO,QString("%1").arg(notify->write()));

  switch(notify->type()) {
    case RDNotification::CartType:
      cartNotification(notify);
      break;

    case RDNotification::PlayoutExtIdType:
      playoutNotification(notify);
      break;

    default:
      break;
  }
}

//
// Handle RDNotification::CartType Notifications
//
// Returns:
//    true - success
//   false - failure
//
bool MainObject::cartNotification(RDNotification *notify,bool queue)
{
  unsigned cartnum;
  int r=false;

  cartnum=notify->id().toInt();

  if (!cartnum) {
    return r;
  }

  RDCart *cart=new RDCart(cartnum);

  //
  // If the cart does not exist, force action to Delete
  //
  if(notify->action()!=RDNotification::DeleteAction&&!cart->exists()) {
    syslog(LOG_NOTICE,QString("Cart %1 does not exist. Forcing delete.").arg(cartnum));
    notify->setAction(RDNotification::DeleteAction);
  }

  switch(notify->action()) {
    case RDNotification::AddAction:
    case RDNotification::ModifyAction:
    {
      //
      // Add/Modify Nexus Stations for Rivendell Group
      //
      RDNexusStationMapList stationmaplist;
      nexus->groupStations(cart->groupName(),stationmaplist);

      for(int i=0;i<stationmaplist.size();i++) {
        nexus->setStation(stationmaplist.at(i).station);

        RDNexusSongInfo info;
        setSongInfo(cart,info);

        int action=notify->action();

        if(action==RDNotification::ModifyAction) {
          if(nexus->exists(cartnum,RDNexus::CutId)) {
            syslog(LOG_INFO,QString("Updating cart %1 on %2:%3").arg(cartnum).arg(nexusd_name).arg(nexus->station()));
            r=nexus->updateSong(info);
          }
          else {
            syslog(LOG_INFO,QString("Cart %1 does not exist on %2:%3").arg(cartnum).arg(nexusd_name).arg(nexus->station()));
            action=RDNotification::AddAction;
          }
        }
        if(action==RDNotification::AddAction) {
          syslog(LOG_INFO,QString("Adding cart %1 on %2:%3").arg(cartnum).arg(nexusd_name).arg(nexus->station()));
          info.category=nexus->stationCategory();  // Set default category
          r=nexus->importSong(info);
        }
        if(!r&&queue) {
          queueNotification(notify,nexus->responseError());
          return r;
        }
      }
      break;
    }
  
    case RDNotification::DeleteAction:
    {
      //
      // Delete cart from all Nexus Stations.
      //
      RDNexusStationMapList stationmaplist;
      nexus->stationMapList(stationmaplist);

      for(int i=0;i<stationmaplist.size();i++) {
        nexus->setStation(stationmaplist.at(i).station);
        syslog(LOG_INFO,QString("Deleting cart %1 from %2:%3").arg(cartnum).arg(nexusd_name).arg(nexus->station()));
        if(!(r=nexus->deleteSong(cartnum,RDNexus::CutId))) {
          if(queue) {
            queueNotification(notify,nexus->responseError());
            return r;
          }
        }
      }
      break;
    }

    default:
      break;
  }

  return r;
}


//
// Handle RDNotification::PlayoutExtIdType Notifications
//
// Returns:
//    true - success
//   false - failure
//
bool MainObject::playoutNotification(RDNotification *notify,bool queue)
{
  unsigned historyid=notify->id().toUInt();
  QString mode;
  int r=false;

  fprintf(stderr,"Playout notification: historyid=%d queue=%d\n",historyid,queue);

  if (!historyid) {
    return r;
  }

  switch(notify->action()) {
    case RDNotification::StartAction:
      mode="playStart";
      break;

    case RDNotification::StopAction:
      mode="playEnd";
      break;

    default:
      return r;
  }

  QString airtime=notify->dateTime().toString(RDNEXUS_DATETIME);

  if(!(r=nexus->reconcileElement(historyid,mode,airtime))) {
    syslog(LOG_ERR,QString("Failed %1 reconciliation for historyid %2 to %3: %4").arg(mode).arg(historyid).arg(nexusd_name).arg(nexus->responseError()));

    if(queue) {
      queueNotification(notify,nexus->responseError());
    }
  }
  else {
    syslog(LOG_INFO,QString("Sent %1 reconciliation for historyid %2 to %3").arg(mode).arg(historyid).arg(nexusd_name));
  }

  return r;
}

void MainObject::updateCart(RDNexusSongInfo &info)
{
  bool created=false;
  RDCart *cart;

  int cartnum=info.cutid;

  fprintf(stderr,"updateCart: info.cutid=%d\n",info.cutid);

  if(!cartnum) {
    QString err_msg;
    cartnum=RDCart::create(nexusd_group,RDCart::Audio,&err_msg);
    if(!cartnum) {
      syslog(LOG_ERR,QString("Could not create new cart %1: %2").arg(nexusd_name).arg(err_msg));
      return;
    }
    created=true;
  }

  cart=new RDCart(cartnum);

  cart->setArtist(info.artist);
  cart->setTitle(info.title);
  cart->setYear(info.year.toInt());
  cart->setNotes(info.notes);

  //
  // If this is a new cart set scheduler code and send new cart number to Nexus
  //
  if(created) {
    if(!nexusd_schedcode.isEmpty()) {
      cart->addSchedCode(nexusd_schedcode);
    }

    info.cutid=cartnum;
    nexus->updateSong(info);
  }

  //
  // Send notification to RIPCD
  //
  if(created) {
    sendCartNotification(RDNotification::AddAction,cartnum);
    syslog(LOG_ERR,QString("Created new cart %1 from %2").arg(cartnum).arg(nexusd_name));
  }
  else {
    sendCartNotification(RDNotification::ModifyAction,cartnum);
    syslog(LOG_NOTICE,QString("Updated cart %1 metadata from %2").arg(cartnum).arg(nexusd_name));
  }

  delete cart;
}

//
// Generate Rivendell Music Data Import File
//
bool MainObject::generateLogFile(RDSvc *svc,QDate logdate,QString &service,RDNexusElementList &list)
{
  QString path=RDDateDecode(svc->importPath(RDSvc::Music),logdate,rda->station(),rda->config(),service);

  syslog(LOG_INFO,QString("Attempting to open '%1'").arg(path));

  QFile file(path);

  if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    syslog(LOG_ERR,QString("Could not open '%1': %2").arg(path).arg(file.errorString()));
    return false;
  }

  QTextStream out(&file);

  for(int i=0;i<list.size();i++) {
    out << QString("%1").arg(list.at(i).airtime,-10);
    out << QString("%1").arg(list.at(i).songid,-10);
    out << QString("%1").arg(list.at(i).cutid,-10);
    out << QString("%1").arg(list.at(i).description,-65);
    out << QString("%1").arg(list.at(i).runtime,-6);
    out << QString("%1").arg(list.at(i).historyid,-10);
    out << endl;
  }

  file.close();

  syslog(LOG_INFO,QString("Log created in '%1'").arg(path));

  return true;
}


void MainObject::reconcileLog(RDSvc *svc,RDLog *log,QDate &logdate,RDNexusElementList &list)
{
  QString logname=log->name();
  bool log_modified=false;

  RDLogEvent *log_event=new RDLogEvent(logname);

  if (!log_event->exists()) {
    syslog(LOG_ERR,tr(QString("Unable to reconcile music log: '1' does not exist").arg(logname)));
    return;
  }

  log_event->load();

  fprintf(stderr,"Loaded %d lines from %s\n",log_event->size(),(const char *)logname);

  for(int line=0;line<log_event->size();line++) {
    RDLogLine *ll=log_event->logLine(line);

    int cutid=ll->cartNumber();
    int historyid=ll->extEventId().toUInt();

    if(historyid) {

      //
      // Locate historyid in Nexus schedule event list
      //
      int i=nexus->indexOfHistoryId(historyid,list);

      //
      // If the historyid does not exist in the new log, remove it
      //
      if(i<0) {
        fprintf(stderr,"Removing log line %d\n",line);
        log_event->remove(line,1);
        log_modified=true;
        line--;
        continue;
      }

      //
      // If the cutid does not match, it was changed
      //
      if(list.at(i).cutid!=cutid) {
        fprintf(stderr,"Changing log line %d from cart %d to %d\n",line,cutid,list.at(i).cutid);
        ll->setCartNumber(list.at(i).cutid);
        log_event->refresh(line);
        log_modified=true;
      }
    }
  }

  if(log_modified) {
    log_event->save(rda->config());
    log->setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    sendLogNotification(RDNotification::ModifyAction,logname);
  }
}


void MainObject::mergeLog(RDSvc *svc,QDate &logdate,QString &logname)
{
  QString err_msg;
  QString report;

  if(!svc->linkLog(RDSvc::Music,logdate,logname,&report,rda->user(),&err_msg)) {

    syslog(LOG_ERR,tr(QString("Unable to merge music log: '%1'").arg(err_msg)));
    return;
  }

  sendLogNotification(RDNotification::ModifyAction,logname);

  if(!report.isEmpty()) {
//    RDTextFile(report);
    qDebug() << report;
  }

  syslog(LOG_INFO,tr(QString("Merged with music log: '%1'").arg(logname)));
}


void MainObject::setSongInfo(RDCart *cart,RDNexusSongInfo &info)
{
  info.clear();

  info.cutid=cart->number();
  info.artist=cart->artist();
  info.title=cart->title();
  if(int year=cart->year()) {
    info.year=QString::number(year);
  }
  info.group=cart->groupName();
  info.notes=cart->notes();
  info.runtime=RDGetTimeLength(cart->forcedLength(),false,false);
  info.intro=RDGetTimeLength(cart->minimumTalkLength(),false,false);
  info.ending=RDGetTimeLength(cart->averageSegueLength(),false,false);
}

void MainObject::sendCartNotification(RDNotification::Action action,unsigned cartnum)
{
  RDNotification *notify=
    new RDNotification(RDNotification::CartType,action,QVariant(cartnum));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}

void MainObject::sendLogNotification(RDNotification::Action action,QString &logname)
{
  RDNotification *notify=
    new RDNotification(RDNotification::LogType,action,QVariant(logname));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}

void MainObject::queueNotification(RDNotification *notify,QString error)
{
  QString sql;

  QString notification=notify->write();

  sql=QString("insert into NEXUS_QUEUE set ");
  sql+=QString("NOTIFICATION='%1',").arg(RDEscapeString(notification));
  sql+=QString("ERROR='%1'").arg(RDEscapeString(error));
  RDSqlQuery::apply(sql);
}

void MainObject::removeNotificationQueue(int id)
{
  QString sql;

  sql=QString("delete from NEXUS_QUEUE ");
  sql+=QString("where id=%1").arg(id);
  RDSqlQuery::apply(sql);
}

void MainObject::processNotificationQueue()
{
  RDSqlQuery *q;
  QString sql;

  //
  // Delete stale notifications
  //
  sql="delete from NEXUS_QUEUE where created < (now() - interval 2 minute)";
  RDSqlQuery::apply(sql);

  //
  // Process notification queue
  //
  sql=QString("select ID,NOTIFICATION from NEXUS_QUEUE order by ID");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    RDNotification *notify=new RDNotification();
    notify->read(q->value(1).toString());
    
    switch(notify->type()) {
      case RDNotification::CartType:
        if(cartNotification(notify,false)) {
          removeNotificationQueue(q->value(0).toInt());
        }
        break;

      case RDNotification::PlayoutExtIdType:
        if(playoutNotification(notify,false)) {
          removeNotificationQueue(q->value(0).toInt());
        }
        break;

      default:
        break;
    }

    delete notify;
  }
  delete q;
}

int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
