// rdcatch.cpp
//
// The Event Schedule Manager for Rivendell.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <assert.h>

#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QTranslator>

#include <rdprofile.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdmixer.h>

#include "add_recording.h"
#include "colors.h"
#include "deckmon.h"
#include "globals.h"
#include "list_reports.h"
#include "rdcatch.h"

//
// Global Resources
//
RDAudioPort *rdaudioport_conf;
RDCartDialog *catch_cart_dialog;
RDCutDialog *catch_cut_dialog;
EditCartEvent *catch_editcartevent_dialog;
EditDownload *catch_editdownload_dialog;
EditPlayout *catch_editplayout_dialog;
EditRecording *catch_editrecording_dialog;
EditSwitchEvent *catch_editswitchevent_dialog;
EditUpload *catch_editupload_dialog;
int catch_audition_card=-1;
int catch_audition_port=-1;

CatchConnector::CatchConnector(RDCatchConnect *conn,const QString &station_name)
{
  catch_connect=conn;
  catch_station_name=station_name;
}


RDCatchConnect *CatchConnector::connector() const
{
  return catch_connect;
}


QString CatchConnector::stationName()
{
  return catch_station_name;
}




MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDMainWindow("rdcatch",c)
{
  QString str;
  QString err_msg;

  catch_resize=false;
  catch_host_warnings=false;
  catch_audition_stream=-1;

  catch_scroll=false;

  //
  // Open the Database
  //
  rda=new RDApplication("RDCatch","rdcatch",RDCATCH_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDCatch - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--offline-host-warnings") {
      catch_host_warnings=RDBool(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDCatch - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setWindowIcon(rda->iconEngine()->applicationIcon(RDIconEngine::RdCatch,22));

  //
  // Generate Palettes
  //
  catch_scroll_color[0]=palette();
  catch_scroll_color[0].setColor(QPalette::Active,QPalette::ButtonText,
			BUTTON_ACTIVE_TEXT_COLOR);
  catch_scroll_color[0].setColor(QPalette::Active,QPalette::Button,
			BUTTON_ACTIVE_BACKGROUND_COLOR);
  catch_scroll_color[0].setColor(QPalette::Active,QPalette::Background,
			palette().color(QPalette::Background));
  catch_scroll_color[0].setColor(QPalette::Inactive,QPalette::ButtonText,
			BUTTON_ACTIVE_TEXT_COLOR);
  catch_scroll_color[0].setColor(QPalette::Inactive,QPalette::Button,
			BUTTON_ACTIVE_BACKGROUND_COLOR);
  catch_scroll_color[0].setColor(QPalette::Inactive,QPalette::Background,
			palette().color(QPalette::Background));
  catch_scroll_color[1]=QPalette(palette().color(QPalette::Background),palette().color(QPalette::Background));

  str=QString("RDCatch")+" v"+VERSION+" - "+tr("Host")+":";
  setWindowTitle(str+" "+rda->config()->stationName());

  //
  // Allocate Global Resources
  //
  catch_audition_card=rda->station()->cueCard();
  catch_audition_port=rda->station()->cuePort();
  catch_time_offset=rda->station()->timeOffset();

  //
  // Load Audio Settings
  //
  RDDeck *deck=new RDDeck(rda->config()->stationName(),0);
  delete deck;
  head_playing=false;
  tail_playing=false;
  rdaudioport_conf=new RDAudioPort(rda->station()->name(),catch_audition_card);

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  connect(rda,SIGNAL(userChanged()),this,SLOT(ripcUserData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),this,SLOT(initData(bool)));
  connect(rda->cae(),SIGNAL(playing(int)),this,SLOT(playedData(int)));
  connect(rda->cae(),SIGNAL(playStopped(int)),
	  this,SLOT(playStoppedData(int)));
  rda->cae()->connectHost();

  //
  // Deck Monitors
  //
  catch_monitor_area=new QScrollArea(this);
  catch_monitor_vbox=new VBox(catch_monitor_area);
  catch_monitor_vbox->setSpacing(2);
  catch_monitor_area->setWidget(catch_monitor_vbox);

  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(abortData(int)));
  QSignalMapper *mon_mapper=new QSignalMapper(this);
  connect(mon_mapper,SIGNAL(mapped(int)),this,SLOT(monitorData(int)));
  QString sql;
  RDSqlQuery *q1;
  sql=QString("select `NAME`,`IPV4_ADDRESS` from `STATIONS` where ")+
    "`NAME`!='DEFAULT'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    catch_connect.push_back(new CatchConnector(new RDCatchConnect(catch_connect.size(),this),q->value(0).toString().toLower()));
    connect(catch_connect.back()->connector(),
      SIGNAL(statusChanged(int,unsigned,RDDeck::Status,int,const QString &)),
      this,
      SLOT(statusChangedData(int,unsigned,RDDeck::Status,int,const QString &)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(monitorChanged(int,unsigned,bool)),
	    this,SLOT(monitorChangedData(int,unsigned,bool)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(connected(int,bool)),
	    this,SLOT(connectedData(int,bool)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(meterLevel(int,int,int,int)),
	    this,SLOT(meterLevelData(int,int,int,int)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(eventUpdated(int)),
	    this,SLOT(eventUpdatedData(int)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(eventPurged(int)),
	    this,SLOT(eventPurgedData(int)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(deckEventSent(int,int,int)),
	    this,SLOT(deckEventSentData(int,int,int)));
    connect(catch_connect.back()->connector(),
	    SIGNAL(heartbeatFailed(int)),
	    this,SLOT(heartbeatFailedData(int)));
    catch_connect.back()->connector()->
      connectHost(q->value(1).toString(),RDCATCHD_TCP_PORT,
		  rda->config()->password());
    sql=QString("select ")+
      "`CHANNEL`,"+          // 00
      "`MON_PORT_NUMBER` "+  // 01
      "from `DECKS` where "+
      "(`CARD_NUMBER`!=-1)&&(`PORT_NUMBER`!=-1)&&(`CHANNEL`>0)&&"+
      "(`STATION_NAME`='"+
      RDEscapeString(q->value(0).toString().toLower())+"') "+
      "order by `CHANNEL`";
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      catch_connect.back()->chan.push_back(q1->value(0).toUInt());
      catch_connect.back()->mon_id.push_back(catch_monitor.size());


      catch_monitor.push_back(new CatchMonitor());
      catch_monitor.back()->setDeckMon(new DeckMon(q->value(0).toString(),
						   q1->value(0).toUInt(),
						   catch_monitor_vbox));
      catch_monitor.back()->setSerialNumber(catch_connect.size()-1);
      catch_monitor.back()->setChannelNumber(q1->value(0).toUInt());
      catch_monitor_vbox->addWidget(catch_monitor.back()->deckMon());

      catch_monitor.back()->deckMon()->
	enableMonitorButton((q1->value(1).toInt()>=0)&&
			    (rda->config()->stationName().toLower()==
			     q->value(0).toString().toLower()));
      catch_monitor.back()->deckMon()->show();
      mapper->setMapping(catch_monitor.back()->deckMon(),
			 catch_monitor.size()-1);
      connect(catch_monitor.back()->deckMon(),SIGNAL(abortClicked()),
	      mapper,SLOT(map()));
      mon_mapper->setMapping(catch_monitor.back()->deckMon(),
			     catch_monitor.size()-1);
      connect(catch_monitor.back()->deckMon(),SIGNAL(monitorClicked()),
	      mon_mapper,SLOT(map()));
    }
    delete q1;
  }
  delete q;
  if(catch_monitor.size()==0) {
    catch_monitor_area->hide();
  }

  //
  // Dialogs
  //
  catch_cart_dialog=
    new RDCartDialog(&catch_filter,&catch_group,&catch_schedcode,"RDCatch",
		     true,this);
  catch_cut_dialog=
    new RDCutDialog(&catch_filter,&catch_group,&catch_schedcode,
		    false,true,false,"RDCatch",true,this);
  catch_editcartevent_dialog=new EditCartEvent(this);
  catch_editdownload_dialog=new EditDownload(&catch_filter,this);
  catch_editplayout_dialog=new EditPlayout(&catch_filter,this);
  catch_editrecording_dialog=new EditRecording(&catch_filter,this);
  catch_editswitchevent_dialog=new EditSwitchEvent(this);
  catch_editupload_dialog=new EditUpload(&catch_filter,this);
  catch_add_recording_dialog=new AddRecording(&catch_filter,this);

  //
  // Filter Selectors
  //
  catch_show_active_box=new QCheckBox(this);
  catch_show_active_label=new QLabel(tr("Show Only Active Events"),this);
  catch_show_active_label->setFont(labelFont());
  catch_show_active_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(catch_show_active_box,SIGNAL(toggled(bool)),
	  this,SLOT(filterChangedData(bool)));
  catch_show_today_box=new QCheckBox(this);
  catch_show_today_label=new QLabel(tr("Show Only Today's Events"),this);
  catch_show_today_label->setFont(labelFont());
  catch_show_today_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(catch_show_today_box,SIGNAL(toggled(bool)),
	  this,SLOT(filterChangedData(bool)));

  catch_dow_box=new QComboBox(this);
  catch_dow_label=new QLabel(tr("Show DayOfWeek:"),this);
  catch_dow_label->setFont(labelFont());
  catch_dow_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  catch_dow_box->insertItem(0,tr("All"));
  catch_dow_box->insertItem(1,tr("Weekdays"));
  catch_dow_box->insertItem(2,tr("Sunday"));
  catch_dow_box->insertItem(3,tr("Monday"));
  catch_dow_box->insertItem(4,tr("Tuesday"));
  catch_dow_box->insertItem(5,tr("Wednesday"));
  catch_dow_box->insertItem(6,tr("Thursday"));
  catch_dow_box->insertItem(7,tr("Friday"));
  catch_dow_box->insertItem(8,tr("Saturday"));
  connect(catch_dow_box,SIGNAL(activated(int)),this,SLOT(filterActivatedData(int)));

  catch_type_box=new QComboBox(this);
  connect(catch_type_box,SIGNAL(activated(int)),this,SLOT(filterActivatedData(int)));
  catch_type_label=new QLabel(tr("Show Event Type")+":",this);
  catch_type_label->setFont(labelFont());
  catch_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  for(int i=0;i<RDRecording::LastType;i++) {
    catch_type_box->insertItem(catch_type_box->count(),rda->iconEngine()->
			       catchIcon((RDRecording::Type)i),
			       RDRecording::typeString((RDRecording::Type)i));
  }
  catch_type_box->insertItem(catch_type_box->count(),tr("All Types"));
  catch_type_box->setCurrentIndex(catch_type_box->count()-1);

  //
  // Cart List
  //
  catch_recordings_view=new CatchTableView(this);
  catch_recordings_model=new RecordListModel(this);
  catch_recordings_model->setFont(defaultFont());
  catch_recordings_model->setPalette(palette());
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  catch_recordings_model
	  ,SLOT(notificationReceivedData(RDNotification *)));
  catch_recordings_view->setModel(catch_recordings_model);
  catch_recordings_view->resizeColumnsToContents();
  connect(catch_recordings_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(catch_recordings_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,
				  const QItemSelection &)),
	  this,
	  SLOT(selectionChangedData(const QItemSelection &,
				    const QItemSelection &)));
  connect(catch_recordings_model,SIGNAL(updateNextEvents()),
	  this,SLOT(nextEventData()));
  catch_recordings_view->resizeColumnsToContents();

  //
  // Add Button
  //
  catch_add_button=new QPushButton(this);
  catch_add_button->setFont(buttonFont());
  catch_add_button->setText(tr("Add"));
  connect(catch_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  catch_edit_button=new QPushButton(this);
  catch_edit_button->setFont(buttonFont());
  catch_edit_button->setText(tr("Edit"));
  connect(catch_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  catch_delete_button=new QPushButton(this);
  catch_delete_button->setFont(buttonFont());
  catch_delete_button->setText(tr("Delete"));
  connect(catch_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Scroll Button
  //
  catch_scroll_button=new QPushButton(this);
  catch_scroll_button->setFont(buttonFont());
  catch_scroll_button->setText(tr("Scroll"));
  connect(catch_scroll_button,SIGNAL(clicked()),this,SLOT(scrollButtonData()));

  //
  // Reports Button
  //
  catch_reports_button=new QPushButton(this);
  catch_reports_button->setFont(buttonFont());
  catch_reports_button->setText(tr("Reports"));
  connect(catch_reports_button,SIGNAL(clicked()),this,SLOT(reportsButtonData()));

  //
  // Wall Clock
  //
  catch_clock_label=new QLabel("00:00:00",this);
  catch_clock_label->setFont(progressFont());
  catch_clock_label->setAlignment(Qt::AlignCenter);
  catch_clock_timer=new QTimer(this);
  catch_clock_timer->setSingleShot(true);
  connect(catch_clock_timer,SIGNAL(timeout()),this,SLOT(clockData()));
  clockData();

  //
  // Play Head Button
  //
  catch_head_button=new RDTransportButton(RDTransportButton::PlayFrom,this);
  catch_head_button->setDisabled(true);
  connect(catch_head_button,SIGNAL(clicked()),this,SLOT(headButtonData()));

  //
  // Play Tail Button
  //
  catch_tail_button=new RDTransportButton(RDTransportButton::PlayTo,this);
  catch_tail_button->setDisabled(true);
  connect(catch_tail_button,SIGNAL(clicked()),this,SLOT(tailButtonData()));

  //
  // Play Stop Button
  //
  catch_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  catch_stop_button->setDisabled(true);
  catch_stop_button->setOnColor(Qt::red);
  connect(catch_stop_button,SIGNAL(clicked()),this,SLOT(stopButtonData()));
  catch_stop_button->on();

  //
  // Close Button
  //
  catch_close_button=new QPushButton(this);
  catch_close_button->setFont(buttonFont());
  catch_close_button->setText(tr("Close"));
  catch_close_button->setFocus();
  catch_close_button->setDefault(true);
  connect(catch_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  //
  // Next Event Timer
  //
  catch_next_timer=new QTimer(this);
  catch_next_timer->setSingleShot(true);
  connect(catch_next_timer,SIGNAL(timeout()),this,SLOT(nextEventData()));

  //
  // Midnight Timer
  //
  catch_midnight_timer=new QTimer(this);
  catch_midnight_timer->setSingleShot(true);
  connect(catch_midnight_timer,SIGNAL(timeout()),this,SLOT(midnightData()));
  midnightData();

  loadSettings(true);

  QTime current_time=QTime::currentTime().addMSecs(catch_time_offset);
  QDate current_date=QDate::currentDate();
  QTime next_time;
  if(ShowNextEvents(current_date.dayOfWeek(),current_time,&next_time)>0) {
    catch_next_timer->start(current_time.msecsTo(next_time));
  }

  //
  // Silly Resize Workaround
  // (so that the deck monitors get laid out properly)
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(resizeData()));
  timer->start(1);

  catch_resize=true;
}

QSize MainWidget::sizeHint() const
{
  return QSize(980,600);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::resizeData()
{
  QResizeEvent *e=new QResizeEvent(QSize(geometry().width(),
					 geometry().height()),
				   QSize(geometry().width(),
					 geometry().height()));
  resizeEvent(e);
  delete e;
}


void MainWidget::connectedData(int serial,bool state)
{
  if(state) {
    catch_connect[serial]->connector()->enableMetering(true);
  }
}


void MainWidget::nextEventData()
{
  QTime next_time;

  catch_recordings_model->clearNextRecords();
  QTime current_time=QTime::currentTime().addMSecs(catch_time_offset);
  QDate current_date=QDate::currentDate();
  if(ShowNextEvents(current_date.dayOfWeek(),current_time,&next_time)>0) {
    catch_next_timer->start(current_time.msecsTo(next_time));
    if(catch_scroll) {
      UpdateScroll();
    }
    return;
  }
  int extra_day=0;
  for(int i=current_date.dayOfWeek()+1;i<8;i++) {
    if(ShowNextEvents(i,QTime(),&next_time)>0) {
      int interval=current_time.msecsTo(QTime(23,59,59))+1000+
	86400000*extra_day+
	QTime(0,0,0).msecsTo(next_time);
      catch_next_timer->start(interval);
      if(catch_scroll) {
	UpdateScroll();
      }
      return;
    }
    extra_day++;
  }
  for(int i=1;i<(current_date.dayOfWeek()+1);i++) {
    if(ShowNextEvents(i,QTime(),&next_time)>0) {
      int interval=current_time.msecsTo(QTime(23,59,59))+1000+
	86400000*extra_day+
	QTime(0,0,0).msecsTo(next_time);
      catch_next_timer->start(interval);
      if(catch_scroll) {
	UpdateScroll();
      }
      return;
    }
    extra_day++;
  }
}


void MainWidget::addData()
{
  int conn;
  RDNotification *notify=NULL;
  QModelIndex row;
  RDRecording::Type type=RDRecording::Recording;

  if(!rda->user()->editCatches()) {
    return;
  }
  EnableScroll(false);
  unsigned rec_id=0;
  if(catch_add_recording_dialog->exec(&rec_id,&type)) {
    notify=new RDNotification(RDNotification::CatchEventType,
			      RDNotification::AddAction,rec_id);
    rda->ripc()->sendNotification(*notify);
    delete notify;
    row=catch_recordings_model->addRecord(rec_id);
    if(row.isValid()) {
      catch_recordings_view->selectRow(row.row());
    }
    conn=GetConnection(catch_recordings_model->hostName(row));
    if(conn<0) {
      fprintf(stderr,"rdcatch: invalid connection index!\n");
      return;
    }
    nextEventData();
  }
}


void MainWidget::editData()
{
  std::vector<int> new_events;
  bool updated=false;
  QModelIndexList rows=catch_recordings_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  if(!rda->user()->editCatches()) {
    return;
  }
  switch(catch_recordings_model->recordExitCode(rows.first())) {
      case RDRecording::Downloading:
      case RDRecording::Uploading:
      case RDRecording::RecordActive:
      case RDRecording::PlayActive:
      case RDRecording::Waiting:
	QMessageBox::information(this,tr("Event Active"),
				 tr("You cannot edit an active event!"));
	return;

      default:
	break;
  }
  EnableScroll(false);
  int id=catch_recordings_model->recordId(rows.first());
  switch(catch_recordings_model->recordType(rows.first())) {
  case RDRecording::Recording:
    updated=catch_editrecording_dialog->exec(id,&new_events);
    break;

  case RDRecording::Playout:
    updated=catch_editplayout_dialog->exec(id,&new_events);
    break;

  case RDRecording::MacroEvent:
    updated=catch_editcartevent_dialog->exec(id,&new_events);
    break;

  case RDRecording::SwitchEvent:
    updated=catch_editswitchevent_dialog->exec(id,&new_events);
    break;

  case RDRecording::Download:
    updated=catch_editdownload_dialog->exec(id,&new_events);
    break;

  case RDRecording::Upload:
    updated=catch_editupload_dialog->exec(id,&new_events);
    break;

  case RDRecording::LastType:
    break;
  }
  if(updated) {
    RDNotification *notify=new RDNotification(RDNotification::CatchEventType,
					      RDNotification::ModifyAction,id);
    rda->ripc()->sendNotification(*notify);
    delete notify;
    catch_recordings_model->refresh(rows.first());
  }
  ProcessNewRecords(&new_events);
  nextEventData();
}


void MainWidget::deleteData()
{
  QString warning;
  QString filename;
  int conn;
  QString sql;
  QModelIndexList rows=catch_recordings_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(!rda->user()->editCatches()) {
    return;
  }
  EnableScroll(false);
  warning=tr("Are you sure you want to delete this event?");
  if(QMessageBox::warning(this,tr("Delete Event"),warning,
			  QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  conn=GetConnection(catch_recordings_model->hostName(rows.first()));
  if(conn<0) {
    fprintf(stderr,"rdcatch: invalid connection index!\n");
    return;
  }
  sql=QString("delete from `RECORDINGS` where ")+
    QString::asprintf("`ID`=%u",catch_recordings_model->recordId(rows.first()));
  RDSqlQuery::apply(sql);
  RDNotification *notify=new RDNotification(RDNotification::CatchEventType,
					    RDNotification::DeleteAction,
					    catch_recordings_model->recordId(rows.first()));
  rda->ripc()->sendNotification(*notify);
  delete notify;
  catch_recordings_model->removeRecord(rows.first());

  nextEventData();
}


void MainWidget::ripcConnectedData(bool state)
{
  if(!state) {
    QMessageBox::warning(this,"Can't Connect","Unable to connect to ripcd!");
    exit(0);
  }
}


void MainWidget::ripcUserData()
{
  QString str;

  str=QString("RDCatch")+" v"+VERSION+" - "+tr("Host")+":";
  setWindowTitle(str+" "+rda->config()->stationName()+", "+tr("User")+": "+
		 rda->ripc()->user());

  //
  // Set Control Perms
  //
  bool modification_allowed=rda->user()->editCatches();
  catch_add_button->setEnabled(modification_allowed);
  catch_edit_button->setEnabled(modification_allowed);
  catch_delete_button->setEnabled(modification_allowed);
}


void MainWidget::statusChangedData(int serial,unsigned chan,
				   RDDeck::Status status,int id,
				   const QString &cutname)
{
  // printf("statusChangedData(%d,%u,%d,%d)\n",serial,chan,status,id);
  int mon=GetMonitor(serial,chan);
  catch_recordings_model->setRecordStatus(id,status);
  if(mon>=0) {
    int waiting_count=0;
    int active_count=0;
    unsigned waiting_id=0;
    catch_recordings_model->
      channelCounts(chan,&waiting_count,&active_count,&waiting_id);
    if(waiting_count>1) {
      catch_monitor[mon]->deckMon()->setStatus(status,-1,cutname);
    }
    else {
      if((active_count==0)||(status!=RDDeck::Idle)) {
	catch_monitor[mon]->deckMon()->setStatus(status,id,cutname);
      }
      else {
	catch_monitor[mon]->deckMon()->
	  setStatus(RDDeck::Waiting,waiting_id,cutname);
      }
    }
  }
  nextEventData();
}


void MainWidget::monitorChangedData(int serial,unsigned chan,bool state)
{
  // printf("monitorChangedData(%d,%u,%d)\n",serial,chan,state);
  int mon=GetMonitor(serial,chan);
  if(mon>=0) {
    catch_monitor[mon]->deckMon()->setMonitor(state);
  }
}


void MainWidget::deckEventSentData(int serial,int chan,int number)
{
  int mon=GetMonitor(serial,chan);
  if(mon>=0) {
    catch_monitor[mon]->deckMon()->setEvent(number);
  }
}


void MainWidget::scrollButtonData()
{
  EnableScroll(!catch_scroll);
}


void MainWidget::reportsButtonData()
{
  ListReports *lr=new ListReports(catch_show_today_box->isChecked(),
				  catch_show_active_box->isChecked(),
				  catch_dow_box->currentIndex(),this);
  lr->exec();
  delete lr;
}


void MainWidget::headButtonData()
{
  QModelIndexList rows=catch_recordings_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EnableScroll(false);
  if((!head_playing)&&(!tail_playing)) {  // Start Head Play
    RDCut *cut=new RDCut(catch_recordings_model->cutName(rows.first()));
    rda->cae()->loadPlay(catch_audition_card,cut->cutName(),
			&catch_audition_stream,&catch_play_handle);
    if(catch_audition_stream<0) {
      return;
    }
    RDSetMixerOutputPort(rda->cae(),catch_audition_card,catch_audition_stream,
			 catch_audition_port);
    rda->cae()->positionPlay(catch_play_handle,cut->startPoint());
    rda->cae()->setPlayPortActive(catch_audition_card,catch_audition_port,catch_audition_stream);
    rda->cae()->setOutputVolume(catch_audition_card,catch_audition_stream,catch_audition_port,
           0+cut->playGain());
    rda->cae()->play(catch_play_handle,RDCATCH_AUDITION_LENGTH,
		    RD_TIMESCALE_DIVISOR,false);
    head_playing=true;
    delete cut;
  }
}


void MainWidget::tailButtonData()
{
  QModelIndexList rows=catch_recordings_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EnableScroll(false);
  if((!head_playing)&&(!tail_playing)) {  // Start Tail Play
    RDCut *cut=new RDCut(catch_recordings_model->cutName(rows.first()));
    rda->cae()->loadPlay(catch_audition_card,cut->cutName(),
			&catch_audition_stream,&catch_play_handle);
    if(catch_audition_stream<0) {
      return;
    }
    RDSetMixerOutputPort(rda->cae(),catch_audition_card,catch_audition_stream,
			 catch_audition_port);
    if((cut->endPoint()-cut->startPoint()-RDCATCH_AUDITION_LENGTH)>0) {
      rda->cae()->positionPlay(catch_play_handle,
			      cut->endPoint()-RDCATCH_AUDITION_LENGTH);
    }
    else {
      rda->cae()->positionPlay(catch_play_handle,cut->startPoint());
    }
    rda->cae()->setPlayPortActive(catch_audition_card,catch_audition_port,catch_audition_stream);
    rda->cae()->setOutputVolume(catch_audition_card,catch_audition_stream,catch_audition_port,
           0+cut->playGain());
    rda->cae()->play(catch_play_handle,RDCATCH_AUDITION_LENGTH,
		    RD_TIMESCALE_DIVISOR,false);
    tail_playing=true;
    delete cut;
  }
}


void MainWidget::stopButtonData()
{
  if(head_playing||tail_playing) {  // Stop Play
    rda->cae()->stopPlay(catch_play_handle);
    rda->cae()->unloadPlay(catch_play_handle);
  }
}


void MainWidget::initData(bool state)
{
  if(state) {
    QList<int> cards;
    cards.push_back(rda->station()->cueCard());
    rda->cae()->enableMetering(&cards);
  }
  else {
    QMessageBox::warning(this,tr("Can't Connect"),
			 tr("Unable to connect to Core AudioEngine"));
    exit(1);
  }
}


void MainWidget::playedData(int handle)
{
  if(head_playing) {
    catch_head_button->on();
  }
  if(tail_playing) {
    catch_tail_button->on();
  }
  catch_stop_button->off();
}


void MainWidget::playStoppedData(int handle)
{
  head_playing=false;
  tail_playing=false;
  catch_head_button->off();
  catch_tail_button->off();
  catch_stop_button->on();
  rda->cae()->unloadPlay(catch_play_handle);
}


void MainWidget::meterLevelData(int serial,int deck,int l_r,int level)
{
  DeckMon *monitor;

  for(unsigned i=0;i<catch_connect[serial]->chan.size();i++) {
    if(catch_connect[serial]->chan[i]==(unsigned)deck) {
       monitor=catch_monitor[catch_connect[serial]->mon_id[i]]->deckMon();
       if(l_r==0) {
	 monitor->setLeftMeter(level);
       }
       if(l_r==1) {
	 monitor->setRightMeter(level);
       }
       return;
    }
  }
}


void MainWidget::abortData(int id)
{
  catch_connect[catch_monitor[id]->serialNumber()]->connector()->
    stop(catch_monitor[id]->channelNumber());
}


void MainWidget::monitorData(int id)
{
  catch_connect[catch_monitor[id]->serialNumber()]->connector()->
    toggleMonitor(catch_monitor[id]->channelNumber());
}


void MainWidget::selectionChangedData(const QItemSelection &before,
				      const QItemSelection &after)
{
  QModelIndexList rows=catch_recordings_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    catch_head_button->setDisabled(true);
    catch_tail_button->setDisabled(true);
    catch_stop_button->setDisabled(true);
    catch_edit_button->setDisabled(true);
    return;
  }
  switch(catch_recordings_model->recordType(rows.first())) {
  case RDRecording::Recording:
  case RDRecording::Playout:
  case RDRecording::Upload:
  case RDRecording::Download:
    catch_head_button->
      setEnabled((catch_audition_card>=0)&&(catch_audition_port>=0));
    catch_tail_button->
      setEnabled((catch_audition_card>=0)&&(catch_audition_port>=0));
    catch_stop_button->setEnabled(true);
    break;

  case RDRecording::MacroEvent:
  case RDRecording::SwitchEvent:
  case RDRecording::LastType:
    catch_head_button->setDisabled(true);
    catch_tail_button->setDisabled(true);
    catch_stop_button->setDisabled(true);
    break;
  }
}


void MainWidget::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void MainWidget::eventUpdatedData(int id)
{
  // printf("eventUpdatedData(%d)\n",id);
  if(!catch_recordings_model->refresh(id)) {
    catch_recordings_model->addRecord(id);
  }
  nextEventData();
}


void MainWidget::eventPurgedData(int id)
{
  catch_recordings_model->removeRecord(id);
}


void MainWidget::heartbeatFailedData(int id)
{
  if(!catch_host_warnings) {
    return;
  }
  QString msg=tr("Control connection timed out to host")+
    " `"+catch_connect[id]->stationName()+"'?";
  QMessageBox::warning(this,"RDCatch - "+tr("Connection Error"),msg);
}


void MainWidget::quitMainWidget()
{
  catch_db->removeDatabase(rda->config()->mysqlDbname());

  saveSettings();
  
  exit(0);
}


void MainWidget::filterChangedData(bool state)
{
  //  printf("filterChangedData(%d)\n",state);

  QTime next_time;
  QString sql;

  if(catch_show_active_box->isChecked()) {
    sql+="(`RECORDINGS`.`IS_ACTIVE`='Y')&&";
  }
  if(catch_show_today_box->isChecked()) {
    QDate today=QDate::currentDate();

    switch(today.dayOfWeek()) {
    case 1:
      sql+="(`RECORDINGS`.`MON`='Y')&&";
      break;

    case 2:
      sql+="(`RECORDINGS`.`TUE`='Y')&&";
      break;

    case 3:
      sql+="(`RECORDINGS`.`WED`='Y')&&";
      break;

    case 4:
      sql+="(`RECORDINGS`.`THU`='Y')&&";
      break;

    case 5:
      sql+="(`RECORDINGS`.`FRI`='Y')&&";
      break;

    case 6:
      sql+="(`RECORDINGS`.`SAT`='Y')&&";
      break;

    case 7:
      sql+="(`RECORDINGS`.`SUN`='Y')&&";
      break;
    }
  }
  switch(catch_dow_box->currentIndex()) {
  case 0:  // All
    break;

  case 1:  // Weekdays
    sql+=QString("((`RECORDINGS`.`MON`='Y')||")+
      "(`RECORDINGS`.`TUE`='Y')||"+
      "(`RECORDINGS`.`WED`='Y')||"+
      "(`RECORDINGS`.`THU`='Y')||"+
      "(`RECORDINGS`.`FRI`='Y'))&&";
    break;

  case 2:  // Sunday
    sql+="(`RECORDINGS`.`SUN`='Y')&&";
    break;

  case 3:  // Monday
    sql+="(`RECORDINGS`.`MON`='Y')&&";
    break;

  case 4:  // Tuesday
    sql+="(`RECORDINGS`.`TUE`='Y')&&";
    break;

  case 5:  // Wednesday
    sql+="(`RECORDINGS`.`WED`='Y')&&";
    break;

  case 6:  // Thursday
    sql+="(`RECORDINGS`.`THU`='Y')&&";
    break;

  case 7:  // Friday
    sql+="(`RECORDINGS`.`FRI`='Y')&&";
    break;

  case 8:  // Saturday
    sql+="(`RECORDINGS`.`SAT`='Y')&&";
    break;
  }
  if(catch_type_box->currentIndex()<RDRecording::LastType) {
    sql+=QString::asprintf("(`RECORDINGS`.`TYPE`=%d)&&",
			   catch_type_box->currentIndex());
  }

  if(sql.isEmpty()) {
    catch_recordings_model->setFilterSql("");
  }
  else {
    sql=sql.left(sql.length()-2);
    catch_recordings_model->setFilterSql("where "+sql);
  }
  nextEventData();
}


void MainWidget::filterActivatedData(int id)
{
  filterChangedData(false);
}


void MainWidget::clockData()
{
  QTime current_time=QTime::currentTime().addMSecs(catch_time_offset);
  catch_clock_label->setText(rda->timeString(current_time));
  catch_clock_timer->start(1000-current_time.msec());
}


void MainWidget::midnightData()
{
  filterChangedData(false);
  catch_midnight_timer->
    start(86400000+QTime::currentTime().addMSecs(catch_time_offset).
	  msecsTo(QTime(0,0,0)));
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  quitMainWidget();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if(!catch_resize) {
    return;
  }
  assert (e);
  assert (catch_monitor_area);
  if(catch_monitor.size()<=RDCATCH_MAX_VISIBLE_MONITORS) {
    catch_monitor_area->
      setGeometry(10,10,e->size().width()-20,32*catch_monitor.size()+4);
    catch_monitor_vbox->
      setGeometry(0,0,e->size().width()-25,32*catch_monitor.size());
  }
  else {
    catch_monitor_area->
      setGeometry(10,10,e->size().width()-20,32*RDCATCH_MAX_VISIBLE_MONITORS);
    catch_monitor_vbox->
      setGeometry(0,0,e->size().width()-
		  catch_monitor_area->verticalScrollBar()->geometry().width()-
		  25,32*catch_monitor.size());
  }
  int deck_height=0;  
  if (catch_monitor.size()>0){
    deck_height=catch_monitor_area->geometry().y()+
      catch_monitor_area->geometry().height();
  }
  catch_show_active_label->setGeometry(35,deck_height+4,155,20);
  catch_show_active_box->setGeometry(15,deck_height+7,15,15);
  catch_show_today_label->setGeometry(225,deck_height+4,170,20);
  catch_show_today_box->setGeometry(205,deck_height+7,15,15);
  catch_dow_label->setGeometry(400,deck_height+4,125,20);
  catch_dow_box->setGeometry(530,deck_height+4,120,20);
  catch_type_label->setGeometry(660,deck_height+4,125,20);
  catch_type_box->setGeometry(790,deck_height+4,140,20);
  catch_recordings_view->
    setGeometry(10,deck_height+25,e->size().width()-20,
		e->size().height()-90-deck_height);
  catch_add_button->setGeometry(10,e->size().height()-55,80,50);
  catch_edit_button->setGeometry(100,e->size().height()-55,80,50);
  catch_delete_button->setGeometry(190,e->size().height()-55,80,50);
  catch_scroll_button->setGeometry(290,e->size().height()-55,80,50);
  catch_reports_button->setGeometry(380,e->size().height()-55,80,50);
  catch_clock_label->setGeometry(470,e->size().height()-38,
				 e->size().width()-850,20);
  catch_head_button->
    setGeometry(e->size().width()-370,e->size().height()-55,80,50);
  catch_tail_button->
    setGeometry(e->size().width()-280,e->size().height()-55,80,50);
  catch_stop_button->
    setGeometry(e->size().width()-190,e->size().height()-55,80,50);
  catch_close_button->
    setGeometry(e->size().width()-90,e->size().height()-55,80,50);
}


int MainWidget::ShowNextEvents(int day,QTime time,QTime *next)
{
  QString sql;
  int count=0;
  if(time.isNull()) {
    sql=QString("select ")+
      "`ID`,"+          // 00
      "`START_TIME` "+  // 01
      "from `RECORDINGS` where "+
      "(`IS_ACTIVE`='Y')&&"+
      "("+RDGetShortDayNameEN(day).toUpper()+"='Y') "+
      "order by `START_TIME`";
  }
  else {
    sql=QString("select ")+
      "`ID`,"+
      "`START_TIME` "+
      "from `RECORDINGS` where "+
      "(`IS_ACTIVE`='Y')&&"+
      "(time_to_sec(`START_TIME`)>time_to_sec('"+
      RDEscapeString(time.toString("hh:mm:ss"))+"'))&&"+
      "("+RDGetShortDayNameEN(day).toUpper()+"='Y')"+
      "order by `START_TIME`";
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return count;
  }
  *next=q->value(1).toTime();

  catch_recordings_model->setRecordIsNext(q->value(0).toUInt(),true);
  count++;
  while(q->next()&&(q->value(1).toTime()==*next)) {
    catch_recordings_model->setRecordIsNext(q->value(0).toInt(),true);
    count++;
  }
  delete q;
  return count;
}


void MainWidget::ProcessNewRecords(std::vector<int> *adds)
{
  for(unsigned i=0;i<adds->size();i++) {
    catch_recordings_model->addRecord(adds->at(i));
    RDNotification *notify=
      new RDNotification(RDNotification::CatchEventType,
			 RDNotification::AddAction,adds->at(i));
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
}


void MainWidget::EnableScroll(bool state)
{
  if(state) {
    catch_scroll_button->setPalette(catch_scroll_color[0]);
    catch_scroll=true;
    UpdateScroll();
  }
  else {
    catch_scroll_button->setPalette(catch_scroll_color[1]);
    catch_scroll=false;
  }
}


void MainWidget::UpdateScroll()
{
  //
  // Current Event
  //
  for(int i=0;i<catch_recordings_model->rowCount();i++) {
    QModelIndex row=catch_recordings_model->index(i,0);
    if(catch_recordings_model->recordStatus(row)==RDDeck::Recording) {
      catch_recordings_view->scrollTo(row,QAbstractItemView::PositionAtCenter);
      return;
    }
  }

  //
  // Next Event
  //
  for(int i=0;i<catch_recordings_model->rowCount();i++) {
    QModelIndex row=catch_recordings_model->index(i,0);
    if(catch_recordings_model->recordIsNext(row)) {
      catch_recordings_view->scrollTo(row,QAbstractItemView::PositionAtCenter);
      return;
    }
  }
}


int MainWidget::GetMonitor(int serial,int chan)
{
  for(unsigned i=0;i<catch_monitor.size();i++) {
    if((catch_monitor[i]->serialNumber()==serial)&&
       (catch_monitor[i]->channelNumber()==chan)) {
      return i;
    }
  }
  return -1;
}


int MainWidget::GetConnection(QString station,unsigned chan)
{
  for(unsigned i=0;i<catch_connect.size();i++) {
    if(catch_connect[i]->stationName()==station.toLower()) {
      if(chan==0) {
	return i;
      }
      for(unsigned j=0;j<catch_connect[i]->chan.size();j++) {
	if(catch_connect[i]->chan[j]==chan) {
	  return i;
	}
      }
      return -1;
    }
  }
  printf("  No connection found!\n");
  return -1;
}

QString MainWidget::GeometryFile() {
  bool home_found = false;
  QString home = RDGetHomeDir(&home_found);
  if (home_found) {
    return home + "/" + RDCATCH_GEOMETRY_FILE;
  } else {
    return NULL;
  }
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString loc=RDApplication::locale();
  if(!loc.isEmpty()) {
    QTranslator qt(0);
    qt.load(QString("/usr/share/qt4/translations/qt_")+loc,".");
    a.installTranslator(&qt);

    QTranslator rd(0);
    rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+loc,".");
    a.installTranslator(&rd);

    QTranslator rdhpi(0);
    rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+loc,".");
    a.installTranslator(&rdhpi);

    QTranslator tr(0);
    tr.load(QString(PREFIX)+QString("/share/rivendell/rdcatch_")+loc,".");
    a.installTranslator(&tr);
  }

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();
  return a.exec();
}
