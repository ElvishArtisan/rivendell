// rdcatch.cpp
//
// The Event Schedule Manager for Rivendell.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <unistd.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <q3sqlpropertymap.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlabel.h>
#include <q3listview.h>
#include <qsignalmapper.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QPixmap>
#include <QCloseEvent>

#include <rdprofile.h>
#include <rd.h>
#include <rdapplication.h>
#include <rdaudio_port.h>
#include <rdcatch.h>
#include <rdconf.h>
#include <rdcut.h>
#include <rdcut_path.h>
#include <rddeck.h>
#include <rdedit_audio.h>
#include <rdescape_string.h>
#include <rdmixer.h>
#include <rdripc.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rduser.h>
#include <dbversion.h>

#include "add_recording.h"
#include "colors.h"
#include "deckmon.h"
#include "edit_cartevent.h"
#include "edit_download.h"
#include "edit_playout.h"
#include "edit_recording.h"
#include "globals.h"
#include "list_reports.h"
#include "edit_switchevent.h"
#include "edit_upload.h"

//
// Global Resources
//
RDAudioPort *rdaudioport_conf;
RDCartDialog *catch_cart_dialog;
int catch_audition_card=-1;
int catch_audition_port=-1;

//
// Icons
//
#include "../icons/record.xpm"
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/switch3.xpm"
#include "../icons/download.xpm"
#include "../icons/upload.xpm"
#include "../icons/rdcatch-22x22.xpm"

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




MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
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

  //
  // Generate Fonts
  //
  QFont default_font("Helvetica",12,QFont::Normal);
  default_font.setPixelSize(12);
  qApp->setFont(default_font);
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",10,QFont::Bold);
  label_font.setPixelSize(10);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont clock_font=QFont("Helvetica",18,QFont::Bold);
  clock_font.setPixelSize(18);

  //
  // Create Icons
  //
  catch_type_maps[RDRecording::Recording]=new QPixmap(record_xpm);
  catch_type_maps[RDRecording::Playout]=new QPixmap(play_xpm);
  catch_type_maps[RDRecording::MacroEvent]=new QPixmap(rml5_xpm);
  catch_type_maps[RDRecording::SwitchEvent]=new QPixmap(switch3_xpm);
  catch_type_maps[RDRecording::Download]=new QPixmap(download_xpm);
  catch_type_maps[RDRecording::Upload]=new QPixmap(upload_xpm);
  catch_rivendell_map=new QPixmap(rdcatch_22x22_xpm);
  setIcon(*catch_rivendell_map);

  //
  // Generate Palettes
  //
  catch_scroll_color[0]=palette();
  catch_scroll_color[0].setColor(QPalette::Active,QColorGroup::ButtonText,
			BUTTON_ACTIVE_TEXT_COLOR);
  catch_scroll_color[0].setColor(QPalette::Active,QColorGroup::Button,
			BUTTON_ACTIVE_BACKGROUND_COLOR);
  catch_scroll_color[0].setColor(QPalette::Active,QColorGroup::Background,
			backgroundColor());
  catch_scroll_color[0].setColor(QPalette::Inactive,QColorGroup::ButtonText,
			BUTTON_ACTIVE_TEXT_COLOR);
  catch_scroll_color[0].setColor(QPalette::Inactive,QColorGroup::Button,
			BUTTON_ACTIVE_BACKGROUND_COLOR);
  catch_scroll_color[0].setColor(QPalette::Inactive,QColorGroup::Background,
			backgroundColor());
  catch_scroll_color[1]=QPalette(backgroundColor(),backgroundColor());

  str=QString("RDCatch")+" v"+VERSION+" - "+tr("Host")+":";
  setCaption(str+" "+rda->config()->stationName());

  //  connect(RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
  //	  this,SLOT(log(RDConfig::LogPriority,const QString &)));

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
  // Set Audio Assignments
  //
  RDSetMixerPorts(rda->station()->name(),rda->cae());

  //
  // Deck Monitors
  //
  catch_monitor_view=new Q3ScrollView(this,"",Qt::WNoAutoErase);
  catch_monitor_vbox=new VBox(catch_monitor_view);
  catch_monitor_vbox->setSpacing(2);
  catch_monitor_view->addChild(catch_monitor_vbox);

  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(abortData(int)));
  QSignalMapper *mon_mapper=new QSignalMapper(this);
  connect(mon_mapper,SIGNAL(mapped(int)),this,SLOT(monitorData(int)));
  QString sql;
  RDSqlQuery *q1;
  RDSqlQuery *q=
    new RDSqlQuery("select NAME,IPV4_ADDRESS from STATIONS\
                   where NAME!=\"DEFAULT\"");
  while(q->next()) {
    catch_connect.push_back(new CatchConnector(new RDCatchConnect(catch_connect.size(),this),q->value(0).toString().lower()));
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
      "CHANNEL,"+          // 00
      "MON_PORT_NUMBER "+  // 01
      "from DECKS where "+
      "(CARD_NUMBER!=-1)&&(PORT_NUMBER!=-1)&&(CHANNEL>0)&&"+
      "(STATION_NAME=\""+RDEscapeString(q->value(0).toString().lower())+"\") "+
      "order by CHANNEL";
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
			    (rda->config()->stationName().lower()==
			     q->value(0).toString().lower()));
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
    catch_monitor_view->hide();
  }

  //
  // Filter Selectors
  //
  catch_show_active_box=new QCheckBox(this,"catch_show_active_box");
  catch_show_active_label=new QLabel(catch_show_active_box,
				     tr("Show Only Active Events"),
				     this,"catch_show_active_label");
  catch_show_active_label->setFont(label_font);
  catch_show_active_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(catch_show_active_box,SIGNAL(toggled(bool)),
	  this,SLOT(filterChangedData(bool)));
  catch_show_today_box=new QCheckBox(this);
  catch_show_today_label=
    new QLabel(catch_show_active_box,tr("Show Only Today's Events"),this);
  catch_show_today_label->setFont(label_font);
  catch_show_today_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(catch_show_today_box,SIGNAL(toggled(bool)),
	  this,SLOT(filterChangedData(bool)));

  catch_dow_box=new QComboBox(this);
  catch_dow_label=new QLabel(catch_dow_box,tr("Show DayOfWeek:"),this);
  catch_dow_label->setFont(label_font);
  catch_dow_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  catch_dow_box->insertItem(tr("All"));
  catch_dow_box->insertItem(tr("Weekdays"));
  catch_dow_box->insertItem(tr("Sunday"));
  catch_dow_box->insertItem(tr("Monday"));
  catch_dow_box->insertItem(tr("Tuesday"));
  catch_dow_box->insertItem(tr("Wednesday"));
  catch_dow_box->insertItem(tr("Thursday"));
  catch_dow_box->insertItem(tr("Friday"));
  catch_dow_box->insertItem(tr("Saturday"));
  connect(catch_dow_box,SIGNAL(activated(int)),this,SLOT(filterActivatedData(int)));

  catch_type_box=new QComboBox(this);
  connect(catch_type_box,SIGNAL(activated(int)),this,SLOT(filterActivatedData(int)));
  catch_type_label=new QLabel(catch_type_box,tr("Show Event Type")+":",this);
  catch_type_label->setFont(label_font);
  catch_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  for(int i=0;i<RDRecording::LastType;i++) {
    catch_type_box->insertItem(*(catch_type_maps[i]),
			       RDRecording::typeString((RDRecording::Type)i));
  }
  catch_type_box->insertItem(tr("All Types"));
  catch_type_box->setCurrentItem(catch_type_box->count()-1);

  //
  // Cart Picker
  //
  catch_cart_dialog=
    new RDCartDialog(&catch_filter,&catch_group,&catch_schedcode,this);

  //
  // Cart List
  //
  catch_recordings_list=new CatchListView(this);
  catch_recordings_list->setAllColumnsShowFocus(true);
  catch_recordings_list->setItemMargin(5);
  catch_recordings_list->setFont(list_font);
  connect(catch_recordings_list,SIGNAL(selectionChanged(Q3ListViewItem *)),
	  this,SLOT(selectionChangedData(Q3ListViewItem *)));
  connect(catch_recordings_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(0,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("DESCRIPTION"));
  catch_recordings_list->setColumnAlignment(1,Qt::AlignLeft);
  catch_recordings_list->addColumn(tr("LOCATION"));
  catch_recordings_list->setColumnAlignment(2,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("START"));
  catch_recordings_list->setColumnAlignment(3,Qt::AlignLeft);
  catch_recordings_list->addColumn(tr("END"));
  catch_recordings_list->setColumnAlignment(4,Qt::AlignLeft);
  catch_recordings_list->addColumn(tr("SOURCE"));
  catch_recordings_list->setColumnAlignment(5,Qt::AlignLeft);
  catch_recordings_list->addColumn(tr("DESTINATION"));
  catch_recordings_list->setColumnAlignment(6,Qt::AlignLeft);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(7,Qt::AlignHCenter);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(8,Qt::AlignHCenter);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(9,Qt::AlignHCenter);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(10,Qt::AlignHCenter);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(11,Qt::AlignHCenter);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(12,Qt::AlignHCenter);
  catch_recordings_list->addColumn("");
  catch_recordings_list->setColumnAlignment(13,Qt::AlignHCenter);
  catch_recordings_list->addColumn("RSS FEED");
  catch_recordings_list->setColumnAlignment(14,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("ORIGIN"));
  catch_recordings_list->setColumnAlignment(15,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("ONE SHOT"));
  catch_recordings_list->setColumnAlignment(16,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("TRIM THRESHOLD"));
  catch_recordings_list->setColumnAlignment(17,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("STARTDATE OFFSET"));
  catch_recordings_list->setColumnAlignment(18,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("ENDDATE OFFSET"));
  catch_recordings_list->setColumnAlignment(19,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("FORMAT"));
  catch_recordings_list->setColumnAlignment(20,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("CHANNELS"));
  catch_recordings_list->setColumnAlignment(21,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("SAMPLE RATE"));
  catch_recordings_list->setColumnAlignment(22,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("BIT RATE"));
  catch_recordings_list->setColumnAlignment(23,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("STATION"));
  catch_recordings_list->setColumnAlignment(24,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("DECK"));
  catch_recordings_list->setColumnAlignment(25,Qt::AlignRight);
  catch_recordings_list->addColumn(tr("CUT"));
  catch_recordings_list->setColumnAlignment(26,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("CART"));
  catch_recordings_list->setColumnAlignment(27,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("ID"));
  catch_recordings_list->setColumnAlignment(28,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("TYPE"));
  catch_recordings_list->setColumnAlignment(29,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("STATUS"));
  catch_recordings_list->setColumnAlignment(30,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("EXIT CODE"));
  catch_recordings_list->setColumnAlignment(31,Qt::AlignHCenter);
  catch_recordings_list->addColumn(tr("STATE"));
  catch_recordings_list->setColumnAlignment(32,Qt::AlignHCenter);
  catch_recordings_list->setSorting(3);  // Start Time

  //
  // Add Button
  //
  catch_add_button=new QPushButton(this);
  catch_add_button->setFont(button_font);
  catch_add_button->setText(tr("&Add"));
  connect(catch_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  catch_edit_button=new QPushButton(this);
  catch_edit_button->setFont(button_font);
  catch_edit_button->setText(tr("&Edit"));
  connect(catch_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  catch_delete_button=new QPushButton(this);
  catch_delete_button->setFont(button_font);
  catch_delete_button->setText(tr("&Delete"));
  connect(catch_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Scroll Button
  //
  catch_scroll_button=new QPushButton(this);
  catch_scroll_button->setFont(button_font);
  catch_scroll_button->setText(tr("Scroll"));
  connect(catch_scroll_button,SIGNAL(clicked()),this,SLOT(scrollButtonData()));

  //
  // Reports Button
  //
  catch_reports_button=new QPushButton(this);
  catch_reports_button->setFont(button_font);
  catch_reports_button->setText(tr("Reports"));
  connect(catch_reports_button,SIGNAL(clicked()),this,SLOT(reportsButtonData()));

  //
  // Wall Clock
  //
  catch_clock_label=new QLabel("00:00:00",this);
  catch_clock_label->setFont(clock_font);
  catch_clock_label->setAlignment(Qt::AlignCenter);
  catch_clock_timer=new QTimer(this);
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
  catch_close_button->setFont(button_font);
  catch_close_button->setText(tr("&Close"));
  catch_close_button->setFocus();
  catch_close_button->setDefault(true);
  connect(catch_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  //
  // Next Event Timer
  //
  catch_next_timer=new QTimer(this);
  connect(catch_next_timer,SIGNAL(timeout()),this,SLOT(nextEventData()));

  //
  // Midnight Timer
  //
  catch_midnight_timer=new QTimer(this);
  connect(catch_midnight_timer,SIGNAL(timeout()),this,SLOT(midnightData()));
  midnightData();
  LoadGeometry();

  RefreshList();

  QTime current_time=QTime::currentTime().addMSecs(catch_time_offset);
  QDate current_date=QDate::currentDate();
  QTime next_time;
  if(ShowNextEvents(current_date.dayOfWeek(),current_time,&next_time)>0) {
    catch_next_timer->start(current_time.msecsTo(next_time),true);
  }
  nextEventData();

  //
  // Silly Resize Workaround
  // (so that the deck monitors get laid out properly)
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(resizeData()));
  timer->start(1,true);

  catch_resize=true;
}

void MainWidget::log(RDConfig::LogPriority prio,const QString &msg)
{
  rda->config()->log("RDCatch",prio,msg);
}

QSize MainWidget::sizeHint() const
{
  return QSize(940,600);
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
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->firstChild();
  if(item!=NULL) {
    do {
      if(item->backgroundColor()==EVENT_NEXT_COLOR) {
	item->setBackgroundColor(catch_recordings_list->palette().color(QPalette::Active,QColorGroup::Base));
      }
    } while((item=(RDListViewItem *)item->nextSibling())!=NULL);
  }
  QTime current_time=QTime::currentTime().addMSecs(catch_time_offset);
  QDate current_date=QDate::currentDate();
  if(ShowNextEvents(current_date.dayOfWeek(),current_time,&next_time)>0) {
    catch_next_timer->start(current_time.msecsTo(next_time),true);
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
	QTime().msecsTo(next_time);
      catch_next_timer->start(interval,true);
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
	QTime().msecsTo(next_time);
      catch_next_timer->start(interval,true);
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
  RDSqlQuery *q;
  RDListViewItem *item;
  int conn;

  if(!rda->user()->editCatches()) {
    return;
  }
  EnableScroll(false);
  int n=AddRecord();
  AddRecording *recording=new AddRecording(n,&catch_filter,this);
  switch((RDRecording::Type)recording->exec()) {
      case RDRecording::Recording:
      case RDRecording::Playout:
      case RDRecording::MacroEvent:
      case RDRecording::SwitchEvent:
      case RDRecording::Download:
      case RDRecording::Upload:
	item=new RDListViewItem(catch_recordings_list);
	item->setBackgroundColor(catch_recordings_list->palette().color(QPalette::Active,QColorGroup::Base));
	item->setText(28,QString().sprintf("%d",n));
	RefreshLine(item);
	conn=GetConnection(item->text(24));
	if(conn<0) {
	  fprintf(stderr,"rdcatch: invalid connection index!\n");
	  return;
	}
	catch_recordings_list->setSelected(item,true);
	catch_recordings_list->ensureItemVisible(item);
	catch_connect[conn]->connector()->addEvent(n);
	nextEventData();
	break;

      default:
	q=new RDSqlQuery(QString().
			sprintf("delete from RECORDINGS where ID=%d",n));
	delete q;
	break;
  }
  delete recording;
}


void MainWidget::editData()
{
  int old_conn;
  int new_conn;
  std::vector<int> new_events;

  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->selectedItem();
  EditRecording *recording;
  EditPlayout *playout;
  EditCartEvent *event;
  EditSwitchEvent *switch_event;
  EditDownload *download;
  EditUpload *upload;

  if(!rda->user()->editCatches()) {
    return;
  }
  if(item==NULL) {
    return;
  }
  switch((RDRecording::ExitCode)item->text(31).toUInt()) {
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
  int id=item->text(28).toInt();
  old_conn=GetConnection(item->text(24));
  if(old_conn<0) {
    fprintf(stderr,"rdcatch: invalid connection index!\n");
    return;
  }
  switch((RDRecording::Type)item->text(29).toInt()) {
  case RDRecording::Recording:
    recording=new EditRecording(id,&new_events,&catch_filter,this);
    if(recording->exec()>=0) {
      RefreshLine(item);
      new_conn=GetConnection(item->text(24));
      if(new_conn<0) {
	fprintf(stderr,"rdcatch: invalid connection index!\n");
	return;
      }
      catch_connect[old_conn]->connector()->removeEvent(id);
      catch_connect[new_conn]->connector()->addEvent(id);
      nextEventData();
    }
    delete recording;
    break;

  case RDRecording::Playout:
    playout=new EditPlayout(id,&new_events,&catch_filter,this);
    if(playout->exec()>=0) {
      RefreshLine(item);
      new_conn=GetConnection(item->text(24));
      if(new_conn<0) {
	fprintf(stderr,"rdcatch: invalid connection index!\n");
	return;
      }
      catch_connect[old_conn]->connector()->removeEvent(id);
      catch_connect[new_conn]->connector()->addEvent(id);
      nextEventData();
    }
    delete playout;
    break;

  case RDRecording::MacroEvent:
    event=new EditCartEvent(id,&new_events,this);
    if(event->exec()>=0) {
      RefreshLine(item);
      new_conn=GetConnection(item->text(24));
      if(new_conn<0) {
	fprintf(stderr,"rdcatch: invalid connection index!\n");
	return;
      }
      catch_connect[old_conn]->connector()->removeEvent(id);
      catch_connect[new_conn]->connector()->addEvent(id);
      nextEventData();
    }
    delete event;
    break;

  case RDRecording::SwitchEvent:
    switch_event=new EditSwitchEvent(id,&new_events,this);
    if(switch_event->exec()>=0) {
      RefreshLine(item);
      new_conn=GetConnection(item->text(24));
      if(new_conn<0) {
	fprintf(stderr,"rdcatch: invalid connection index!\n");
	return;
      }
      catch_connect[old_conn]->connector()->removeEvent(id);
      catch_connect[new_conn]->connector()->addEvent(id);
      nextEventData();
    }
    delete switch_event;
    break;

  case RDRecording::Download:
    download=new EditDownload(id,&new_events,&catch_filter,this);
    if(download->exec()>=0) {
      RefreshLine(item);
      new_conn=GetConnection(item->text(24));
      if(new_conn<0) {
	fprintf(stderr,"rdcatch: invalid connection index!\n");
	return;
      }
      catch_connect[old_conn]->connector()->removeEvent(id);
      catch_connect[new_conn]->connector()->addEvent(id);
      nextEventData();
    }
    delete download;
    break;

  case RDRecording::Upload:
    upload=new EditUpload(id,&new_events,&catch_filter,this);
    if(upload->exec()>=0) {
      RefreshLine(item);
      new_conn=GetConnection(item->text(24));
      if(new_conn<0) {
	fprintf(stderr,"rdcatch: invalid connection index!\n");
	return;
      }
      catch_connect[old_conn]->connector()->removeEvent(id);
      catch_connect[new_conn]->connector()->addEvent(id);
      nextEventData();
    }
    delete upload;
    break;

  case RDRecording::LastType:
    break;
  }
  ProcessNewRecords(&new_events);
}


void MainWidget::deleteData()
{
  QString warning;
  QString filename;
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->selectedItem();
  int conn;

  if(!rda->user()->editCatches()||(item==NULL)) {
    return;
  }
  EnableScroll(false);
  if(item->text(1).isEmpty()) {
    warning=tr("Are you sure you want to delete event")+"\n"+
      tr("at")+" "+item->text(3);
  }
  else {
    warning=tr("Are you sure you want to delete event")+
      "\n\""+item->text(3)+"\"?";
  }
  if(QMessageBox::warning(this,tr("Delete Event"),warning,
			  QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  conn=GetConnection(item->text(24));
  if(conn<0) {
    fprintf(stderr,"rdcatch: invalid connection index!\n");
    return;
  }
  catch_connect[conn]->connector()->removeEvent(item->text(28).toInt());
  sql=QString("delete from RECORDINGS where ")+
    "ID="+item->text(28);
  q=new RDSqlQuery(sql);
  delete q;
  RDListViewItem *next=(RDListViewItem *)item->nextSibling();
  catch_recordings_list->removeItem(item);
  if(next!=NULL) {
    catch_recordings_list->setSelected(next,true);
  }
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
  setCaption(str+" "+rda->config()->stationName()+", "+tr("User")+": "+
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
  if(id>0) {
    RDListViewItem *item=GetItem(id);
    if(item!=NULL) {
      switch(status) {
	case RDDeck::Offline:
	  item->setBackgroundColor(EVENT_ERROR_COLOR);
	  break;
	  
	case RDDeck::Idle:
	  item->setBackgroundColor(catch_recordings_list->palette().
				   color(QPalette::Active,QColorGroup::Base));
	  break;
	  
	case RDDeck::Ready:
	  item->setBackgroundColor(EVENT_READY_COLOR);
	  break;
	  
	case RDDeck::Waiting:
	  item->setBackgroundColor(EVENT_WAITING_COLOR);
	  break;
	  
	case RDDeck::Recording:
	  item->setBackgroundColor(EVENT_ACTIVE_COLOR);
	  break;
      }
      item->setText(32,QString().sprintf("%u",status));
      UpdateExitCode(item);
    }
    else {
      if(id<RDCATCHD_DYNAMIC_BASE_ID) {
	fprintf(stderr,
		"rdcatch: received status update for nonexistent ID %d\n",id);
	return;
      }
    }
  }
  if(mon>=0) {
    int waiting_count=0;
    int active_count=0;
    int waiting_id=0;
    RDListViewItem *item=
      (RDListViewItem *)catch_recordings_list->firstChild();
    while(item!=NULL) {
      if(item->text(25).toUInt()==chan) {
	switch((RDDeck::Status)item->text(32).toUInt()) {
	    case RDDeck::Waiting:
	      active_count++;
	      waiting_count++;
	      waiting_id=item->text(28).toInt();
	      break;

	    case RDDeck::Ready:
	    case RDDeck::Recording:
	      active_count++;
	      break;

	    default:
	      break;
	}
      }
      item=(RDListViewItem *)item->nextSibling();
    }
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
				  catch_dow_box->currentItem(),this);
  lr->exec();
  delete lr;
}


void MainWidget::headButtonData()
{
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->selectedItem();
  if(item==NULL) {
    return;
  }
  EnableScroll(false);
  if((!head_playing)&&(!tail_playing)) {  // Start Head Play
    RDCut *cut=new RDCut(item->text(26));
    rda->cae()->loadPlay(catch_audition_card,item->text(26),
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
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->selectedItem();
  if(item==NULL) {
    return;
  }
  EnableScroll(false);
  if((!head_playing)&&(!tail_playing)) {  // Start Tail Play
    RDCut *cut=new RDCut(item->text(26));
    rda->cae()->loadPlay(catch_audition_card,item->text(26),
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
  if(!state) {
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


void MainWidget::selectionChangedData(Q3ListViewItem *item)
{
  if(item==NULL) {
    catch_head_button->setDisabled(true);
    catch_tail_button->setDisabled(true);
    catch_stop_button->setDisabled(true);
    catch_edit_button->setDisabled(true);
    return;
  }
  if(((item->text(29).toInt()==RDRecording::Recording)||
      (item->text(29).toInt()==RDRecording::Playout)||
      (item->text(29).toInt()==RDRecording::Upload)||
      (item->text(29).toInt()==RDRecording::Download))) {
    catch_head_button->
      setEnabled((catch_audition_card>=0)&&(catch_audition_port>=0));
    catch_tail_button->
      setEnabled((catch_audition_card>=0)&&(catch_audition_port>=0));
    catch_stop_button->setEnabled(true);
  }
  else {
    catch_head_button->setDisabled(true);
    catch_tail_button->setDisabled(true);
    catch_stop_button->setDisabled(true);
  }
}


void MainWidget::doubleClickedData(Q3ListViewItem *,const QPoint &,int)
{
  editData();
}


void MainWidget::eventUpdatedData(int id)
{
  // printf("eventUpdatedData(%d)\n",id);
  RDListViewItem *item=GetItem(id);
  if(item==NULL) {  // New Event
    item=new RDListViewItem(catch_recordings_list);
    item->setText(28,QString().sprintf("%d",id));
  }
  RefreshLine(item);
  nextEventData();
}


void MainWidget::eventPurgedData(int id)
{
  RDListViewItem *item=GetItem(id);
  if(item==NULL) {
    return;
  }
  catch_recordings_list->removeItem(item);
}


void MainWidget::heartbeatFailedData(int id)
{
  if(!catch_host_warnings) {
    return;
  }
  QString str;

  str=QString(tr("Control connection timed out to host"));
  QString msg=tr("Control connection timed out to host")+
    " `"+catch_connect[id]->stationName()+"'?";
  QMessageBox::warning(this,"RDCatch - "+tr("Connection Error"),msg);
}


void MainWidget::quitMainWidget()
{
  catch_db->removeDatabase(rda->config()->mysqlDbname());
  SaveGeometry();
  exit(0);
}


void MainWidget::filterChangedData(bool)
{
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->firstChild();
  int day=QDate::currentDate().dayOfWeek();
  int day_column=0;
  switch(day) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    day_column=day+7;
    break;

  case 7:
    day_column=7;
  }
  RDRecording::Type filter_type=
    (RDRecording::Type)catch_type_box->currentItem();
  if(catch_show_active_box->isChecked()) {
    if(catch_show_today_box->isChecked()) {
      while(item!=NULL) {
	RDRecording::Type event_type=
	  (RDRecording::Type)item->text(29).toInt();
	if((item->textColor(1)==EVENT_ACTIVE_TEXT_COLOR)&&
	  (!item->text(day_column).isEmpty())) {
	  if((event_type==filter_type)||(filter_type==RDRecording::LastType)) {
	    ShowEvent(item);
	  }
	}
	else {
	  item->setVisible(false);
	}
	item=(RDListViewItem *)item->nextSibling();
      }
    }
    else {
      if(catch_show_active_box->isChecked()) {
	while(item!=NULL) {
	  RDRecording::Type event_type=
	    (RDRecording::Type)item->text(29).toInt();
	  if((item->textColor(1)==EVENT_ACTIVE_TEXT_COLOR)&&
	     ((event_type==filter_type)||(filter_type==RDRecording::LastType))) {
	    ShowEvent(item);
	  }
	  else {
	    item->setVisible(false);
	  }
	  item=(RDListViewItem *)item->nextSibling();
	}
      }
    }
  }
  else {
    if(catch_show_today_box->isChecked()) {
      while(item!=NULL) {
	RDRecording::Type event_type=
	  (RDRecording::Type)item->text(29).toInt();
	if((!item->text(day_column).isEmpty())&&
	   ((event_type==filter_type)||(filter_type==RDRecording::LastType))) {
	  ShowEvent(item);
	}
	else {
	  item->setVisible(false);
	}
	item=(RDListViewItem *)item->nextSibling();
      }
    }
    else {
      while(item!=NULL) {
	RDRecording::Type event_type=
	  (RDRecording::Type)item->text(29).toInt();
	if((event_type==filter_type)||(filter_type==RDRecording::LastType)) {
	  ShowEvent(item);
	}
	else {
	  item->setVisible(false);
	}
	item=(RDListViewItem *)item->nextSibling();
      }
    }
  }
}


void MainWidget::filterActivatedData(int id)
{
  filterChangedData(false);
}


void MainWidget::clockData()
{
  QTime current_time=QTime::currentTime().addMSecs(catch_time_offset);
  catch_clock_label->setText(current_time.toString("hh:mm:ss"));
  catch_clock_timer->start(1000-current_time.msec(),true);
}


void MainWidget::midnightData()
{
  filterChangedData(false);
  catch_midnight_timer->
    start(86400000+QTime::currentTime().addMSecs(catch_time_offset).
	  msecsTo(QTime()),true);
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
  assert (catch_monitor_view);
  if(catch_monitor.size()<=RDCATCH_MAX_VISIBLE_MONITORS) {
    catch_monitor_view->
      setGeometry(10,10,e->size().width()-20,32*catch_monitor.size()+4);
    catch_monitor_vbox->
      setGeometry(0,0,e->size().width()-25,32*catch_monitor.size());
  }
  else {
    catch_monitor_view->
      setGeometry(10,10,e->size().width()-20,32*RDCATCH_MAX_VISIBLE_MONITORS);
    catch_monitor_vbox->
      setGeometry(0,0,e->size().width()-
		  catch_monitor_view->verticalScrollBar()->geometry().width()-
		  25,32*catch_monitor.size());
  }
  int deck_height=0;  
  if (catch_monitor.size()>0){
    deck_height=catch_monitor_view->geometry().y()+
      catch_monitor_view->geometry().height();
  }
  catch_show_active_label->setGeometry(35,deck_height+5,140,20);
  catch_show_active_box->setGeometry(15,deck_height+7,15,15);
  catch_show_today_label->setGeometry(205,deck_height+5,145,20);
  catch_show_today_box->setGeometry(185,deck_height+7,15,15);
  catch_dow_label->setGeometry(370,deck_height+5,125,20);
  catch_dow_box->setGeometry(500,deck_height+4,120,20);
  catch_type_label->setGeometry(630,deck_height+5,125,20);
  catch_type_box->setGeometry(760,deck_height+4,140,20);
  catch_recordings_list->
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


void MainWidget::ShowEvent(RDListViewItem *item)
{
  switch(catch_dow_box->currentItem()) {
  case 0:   // All Days
    item->setVisible(true);
    break;
	
  case 1:   // Weekdays
    if(item->text(8).isEmpty()&&
       item->text(9).isEmpty()&&
       item->text(10).isEmpty()&&
       item->text(11).isEmpty()&&
       item->text(12).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 2:   // Sunday
    if(item->text(7).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 3:   // Monday
    if(item->text(8).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 4:   // Tuesday
    if(item->text(9).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 5:   // Wednesday
    if(item->text(10).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 6:   // Thursday
    if(item->text(11).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 7:   // Friday
    if(item->text(12).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
	
  case 8:   // Saturday
    if(item->text(13).isEmpty()) {
      item->setVisible(false);
    }
    else {
      item->setVisible(true);
    }
    break;
  }
}


int MainWidget::ShowNextEvents(int day,QTime time,QTime *next)
{
  RDListViewItem *item=NULL;
  QString sql;
  int count=0;
  if(time.isNull()) {
    sql=QString("select ")+
      "ID,"+          // 00
      "START_TIME "+  // 01
      "from RECORDINGS where "+
      "(IS_ACTIVE=\"Y\")&&"+
      "("+RDGetShortDayNameEN(day).upper()+"=\"Y\") "+
      "order by START_TIME";
  }
  else {
    sql=QString("select ")+
      "ID,"+
      "START_TIME "+
      "from RECORDINGS where "+
      "(IS_ACTIVE=\"Y\")&&"+
      "(time_to_sec(START_TIME)>time_to_sec(\""+
      RDEscapeString(time.toString("hh:mm:ss"))+"\"))&&"+
      "("+RDGetShortDayNameEN(day).upper()+"=\"Y\")"+
      "order by START_TIME";
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return count;
  }
  *next=q->value(1).toTime();
  if((item=GetItem(q->value(0).toInt()))!=NULL) {
    if((item->backgroundColor()!=EVENT_ACTIVE_COLOR)&&
       (item->backgroundColor()!=EVENT_WAITING_COLOR)){
      item->setBackgroundColor(QColor(EVENT_NEXT_COLOR));
    }
    count++;
  }
  while(q->next()&&(q->value(1).toTime()==*next)) {
    if((item=GetItem(q->value(0).toInt()))!=NULL) {
      if((item->backgroundColor()!=EVENT_ACTIVE_COLOR)&&
	 (item->backgroundColor()!=EVENT_WAITING_COLOR)){
	item->setBackgroundColor(QColor(EVENT_NEXT_COLOR));
      }
      count++;
    }
  }
  delete q;
  return count;
}


int MainWidget::AddRecord()
{
  QString sql;
  RDSqlQuery *q;
  int n;

  sql=QString("select ID from RECORDINGS order by ID desc limit 1");
  q=new RDSqlQuery(sql);
  if(q->first()) {
    n=q->value(0).toInt()+1;
  }
  else {
    n=1;
  }
  delete q;
  sql=QString("insert into RECORDINGS set ")+
    QString().sprintf("ID=%d,",n)+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  return n;
}


void MainWidget::ProcessNewRecords(std::vector<int> *adds)
{
  int conn=0;
  RDListViewItem *item;

  for(unsigned i=0;i<adds->size();i++) {
    item=new RDListViewItem(catch_recordings_list);
    item->setBackgroundColor(catch_recordings_list->palette().color(QPalette::Active,QColorGroup::Base));
    item->setText(28,QString().sprintf("%d",adds->at(i)));
    RefreshLine(item);
    conn=GetConnection(item->text(24));
    if(conn<0) {
      fprintf(stderr,"rdcatch: invalid connection index!\n");
      return;
    }
    catch_connect[conn]->connector()->addEvent(adds->at(i));
  }
  nextEventData();
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
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->firstChild();

  //
  // Search for active event
  //
  while(item!=NULL) {
    if(item->backgroundColor()==EVENT_ACTIVE_COLOR) {
      catch_recordings_list->
	ensureVisible(0,catch_recordings_list->itemPos(item),
		      0,catch_recordings_list->size().height()/2);
      catch_recordings_list->setCurrentItem(item);
      catch_recordings_list->clearSelection();
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
  }

  //
  // Search for next event
  //
  item=(RDListViewItem *)catch_recordings_list->firstChild();
  while(item!=NULL) {
    if(item->backgroundColor()==EVENT_NEXT_COLOR) {
      catch_recordings_list->
	ensureVisible(0,catch_recordings_list->itemPos(item),
		      0,catch_recordings_list->size().height()/2);
      catch_recordings_list->setCurrentItem(item);
      catch_recordings_list->clearSelection();
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
}


QString MainWidget::RefreshSql() const
{
  QString sql=QString("select ")+
    "RECORDINGS.ID,"+                // 00
    "RECORDINGS.DESCRIPTION,"+       // 01
    "RECORDINGS.IS_ACTIVE,"+         // 02
    "RECORDINGS.STATION_NAME,"+      // 03
    "RECORDINGS.START_TIME,"+        // 04
    "RECORDINGS.LENGTH,"+            // 05
    "RECORDINGS.CUT_NAME,"+          // 06
    "RECORDINGS.SUN,"+               // 07
    "RECORDINGS.MON,"+               // 08
    "RECORDINGS.TUE,"+               // 09
    "RECORDINGS.WED,"+               // 10
    "RECORDINGS.THU,"+               // 11
    "RECORDINGS.FRI,"+               // 12
    "RECORDINGS.SAT,"+               // 13
    "RECORDINGS.SWITCH_INPUT,"+      // 14
    "RECORDINGS.START_GPI,"+         // 15
    "RECORDINGS.END_GPI,"+           // 16
    "RECORDINGS.TRIM_THRESHOLD,"+    // 17
    "RECORDINGS.STARTDATE_OFFSET,"+  // 18
    "RECORDINGS.ENDDATE_OFFSET,"+    // 19
    "RECORDINGS.FORMAT,"+            // 20
    "RECORDINGS.CHANNELS,"+          // 21
    "RECORDINGS.SAMPRATE,"+          // 22
    "RECORDINGS.BITRATE,"+           // 23
    "RECORDINGS.CHANNEL,"+           // 24
    "RECORDINGS.MACRO_CART,"+        // 25
    "RECORDINGS.TYPE,"+              // 26
    "RECORDINGS.SWITCH_OUTPUT,"+     // 27
    "RECORDINGS.EXIT_CODE,"+         // 28
    "RECORDINGS.ONE_SHOT,"+          // 29
    "RECORDINGS.START_TYPE,"+        // 30
    "RECORDINGS.START_LENGTH,"+      // 31
    "RECORDINGS.START_MATRIX,"+      // 32
    "RECORDINGS.START_LINE,"+        // 33
    "RECORDINGS.START_OFFSET,"+      // 34
    "RECORDINGS.END_TYPE,"+          // 35
    "RECORDINGS.END_TIME,"+          // 36
    "RECORDINGS.END_LENGTH,"+        // 37
    "RECORDINGS.END_MATRIX,"+        // 38
    "RECORDINGS.END_LINE,"+          // 39
    "CUTS.ORIGIN_NAME,"+             // 40
    "CUTS.ORIGIN_DATETIME,"+         // 41
    "RECORDINGS.URL,"+               // 42
    "RECORDINGS.QUALITY,"+           // 43
    "FEEDS.KEY_NAME,"+               // 44
    "EXIT_TEXT "+                    // 45
    "from RECORDINGS left join CUTS "+
    "on (RECORDINGS.CUT_NAME=CUTS.CUT_NAME) left join FEEDS "+
    "on (RECORDINGS.FEED_ID=FEEDS.ID) ";

  return sql;
}


void MainWidget::RefreshRow(RDSqlQuery *q,RDListViewItem *item)
{
  RDCut *cut=NULL;
  QString sql;
  RDSqlQuery *q1;

  item->setBackgroundColor(catch_recordings_list->palette().color(QPalette::Active,QColorGroup::Base));
  if(RDBool(q->value(2).toString())) {
    item->setTextColor(QColor(EVENT_ACTIVE_TEXT_COLOR));
  }
  else {
    item->setTextColor(QColor(EVENT_INACTIVE_TEXT_COLOR));
  }
  item->setText(1,q->value(1).toString());     // Description
  if(RDBool(q->value(7).toString())) {       // Sun
    item->setText(7,tr("Su"));
  }
  if(RDBool(q->value(8).toString())) {       // Mon
    item->setText(8,tr("Mo"));
  }
  if(RDBool(q->value(9).toString())) {       // Tue
    item->setText(9,tr("Tu"));
  }
  if(RDBool(q->value(10).toString())) {       // Wed
    item->setText(10,tr("We"));
  }
  if(RDBool(q->value(11).toString())) {      // Thu
    item->setText(11,tr("Th"));
  }
  if(RDBool(q->value(12).toString())) {      // Fri
    item->setText(12,tr("Fr"));
  }
  if(RDBool(q->value(13).toString())) {      // Sat
    item->setText(13,tr("Sa"));
  }
  switch((RDRecording::Type)q->value(26).toInt()) {
  case RDRecording::Recording:
  case RDRecording::Playout:
  case RDRecording::Download:
  case RDRecording::Upload:
    item->setText(15,q->value(40).toString()+" - "+
	       q->value(41).toDateTime().
	       toString("M/dd/yyyy hh:mm:ss"));
    break;
    
  default:
    item->setText(15,"");
    break;
  }
  item->setText(16,q->value(29).toString());   // One Shot
  item->setText(17,QString().sprintf("%d ",  // Trim Threshold
				     -q->value(17).toInt())+tr("dB"));
  item->setText(18,q->value(18).toString());   // Startdate Offset
  item->setText(19,q->value(19).toString());   // Enddate Offset
  item->setText(24,q->value(3).toString());    // Station
  item->setText(25,q->value(24).toString());   // Deck
  item->setText(26,q->value(6).toString());    // Cut Name
  if(q->value(24).toInt()>=0) {
    item->setText(27,q->value(25).toString()); // Macro Cart
  }
  else {
    item->setText(27,"");
  }
  item->setText(28,q->value(0).toString());   // Id
  item->setText(29,q->value(26).toString());   // Type
  item->setText(32,QString().sprintf("%u",RDDeck::Idle));
  item->setPixmap(0,*(catch_type_maps[q->value(26).toInt()]));
  switch((RDRecording::Type)q->value(26).toInt()) {
  case RDRecording::Recording:
    item->setText(2,q->value(3).toString()+
	       QString().sprintf(" : %dR",q->value(24).toInt()));
    switch((RDRecording::StartType)q->value(30).toUInt()) {
    case RDRecording::HardStart:
      item->setText(3,tr("Hard")+": "+q->value(4).toTime().toString("hh:mm:ss"));
      break;

    case RDRecording::GpiStart:
      item->setText(3,tr("Gpi")+": "+q->value(4).toTime().toString("hh:mm:ss")+
		 ","+q->value(4).toTime().addMSecs(q->value(31).toInt()).
		 toString("hh:mm:ss")+","+
		 QString().sprintf("%d:%d,",q->value(32).toInt(),q->value(33).toInt())+
		 QTime().addMSecs(q->value(34).toUInt()).toString("mm:ss"));
      break;
    }
    switch((RDRecording::EndType)q->value(35).toUInt()) {
    case RDRecording::LengthEnd:
      item->setText(4,tr("Len")+": "+
		 RDGetTimeLength(q->value(5).toUInt(),false,false));
      break;

    case RDRecording::HardEnd:
      item->setText(4,tr("Hard")+": "+
		 q->value(36).toTime().toString("hh:mm:ss"));
      break;

    case RDRecording::GpiEnd:
      item->setText(4,tr("Gpi")+": "+q->value(36).toTime().toString("hh:mm:ss")+
		 ","+q->value(36).toTime().addMSecs(q->value(37).toInt()).
		 toString("hh:mm:ss")+
		 QString().sprintf(",%d:%d",q->value(38).toInt(),
				   q->value(39).toInt()));
      break;
    }
    item->setText(6,tr("Cut")+" "+q->value(6).toString());
    sql=QString("select ")+
      "SWITCH_STATION,"+  // 00
      "SWITCH_MATRIX "+   // 01
      "from DECKS where "+
      "(STATION_NAME=\""+RDEscapeString(q->value(3).toString())+"\")&&"+
      QString().sprintf("(CHANNEL=%d)",q->value(24).toInt());
    q1=new RDSqlQuery(sql);
    if(q1->first()) {  // Source
      item->setText(5,GetSourceName(q1->value(0).toString(),  
				 q1->value(1).toInt(),
				 q->value(14).toInt()));
    }
    delete q1;
    switch((RDSettings::Format)q->value(20).toInt()) {    // Format
    case RDSettings::Pcm16:
      item->setText(20,tr("PCM16"));
      break;

    case RDSettings::Pcm24:
      item->setText(20,tr("PCM24"));
      break;

    case RDSettings::MpegL1:
      item->setText(20,tr("MPEG Layer 1"));
      break;

    case RDSettings::MpegL2:
    case RDSettings::MpegL2Wav:
      item->setText(20,tr("MPEG Layer 2"));
      break;

    case RDSettings::MpegL3:
      item->setText(20,tr("MPEG Layer 3"));
      break;

    case RDSettings::Flac:
      item->setText(20,tr("FLAC"));
      break;

    case RDSettings::OggVorbis:
      item->setText(20,tr("OggVorbis"));
      break;
    }
    item->setText(21,q->value(21).toString());   // Channels
    item->setText(22,q->value(22).toString());   // Sample Rate
    item->setText(23,q->value(23).toString());   // Bit Rate
    break;

  case RDRecording::Playout:
    item->setText(2,q->value(3).toString()+QString().sprintf(" : %dP",
							  q->value(24).toInt()-128));
    item->setText(3,tr("Hard")+": "+q->value(4).toTime().toString("hh:mm:ss"));
    cut=new RDCut(q->value(6).toString());
    if(cut->exists()) {
      item->setText(4,tr("Len")+": "+RDGetTimeLength(cut->length(),false,false));
    }
    delete cut;
    item->setText(5,tr("Cut")+" "+q->value(6).toString());
    break;

  case RDRecording::MacroEvent:
    item->setText(2,q->value(3).toString());
    item->setText(3,tr("Hard")+": "+q->value(4).toTime().
	       toString(QString().sprintf("hh:mm:ss")));
    item->setText(5,tr("Cart")+QString().sprintf(" %06d",q->value(25).toInt()));
    break;

  case RDRecording::SwitchEvent:
    item->setText(2,q->value(3).toString());
    item->setText(3,tr("Hard")+": "+q->value(4).toTime().toString("hh:mm:ss"));
    item->setText(5,GetSourceName(q->value(3).toString(),  // Source
			       q->value(24).toInt(),
			       q->value(14).toInt()));
    item->setText(6,GetDestinationName(q->value(3).toString(),  // Dest
				    q->value(24).toInt(),
				    q->value(27).toInt()));
    break;

  case RDRecording::Download:
    item->setText(2,q->value(3).toString());
    item->setText(3,tr("Hard")+": "+q->value(4).toTime().toString("hh:mm:ss"));
    item->setText(5,q->value(42).toString());
    item->setText(6,tr("Cut")+" "+q->value(6).toString());
    break;

  case RDRecording::Upload:
    item->setText(2,q->value(3).toString());
    item->setText(3,tr("Hard")+": "+q->value(4).toTime().toString("hh:mm:ss"));
    item->setText(5,tr("Cut")+" "+q->value(6).toString());
    item->setText(6,q->value(42).toString());
    switch((RDSettings::Format)q->value(20).toInt()) {    // Format
    case RDSettings::Pcm16:
      item->setText(20,tr("PCM16"));
      break;

    case RDSettings::Pcm24:
      item->setText(20,tr("PCM24"));
      break;

    case RDSettings::MpegL1:
      item->setText(20,tr("MPEG Layer 1"));
      break;

    case RDSettings::MpegL2:
    case RDSettings::MpegL2Wav:
      item->setText(20,tr("MPEG Layer 2"));
      break;

    case RDSettings::MpegL3:
      item->setText(20,tr("MPEG Layer 3"));
      break;

    case RDSettings::Flac:
      item->setText(20,tr("FLAC"));
      break;

    case RDSettings::OggVorbis:
      item->setText(20,tr("OggVorbis"));
      break;
    }
    if(q->value(44).toString().isEmpty()) {
      item->setText(14,tr("[none]"));
    }
    else {
      item->setText(14,q->value(44).toString());    // Feed Key Name
    }
    item->setText(21,q->value(21).toString());   // Channels
    item->setText(22,q->value(22).toString());   // Sample Rate
    if(q->value(23).toInt()==0) {     // Bit Rate/Quality
      item->setText(23,QString().sprintf("Qual %d",q->value(43).toInt()));
    }
    else {
      item->setText(23,QString().sprintf("%d kb/sec",
				      q->value(23).toInt()/1000));
    }
    break;

  case RDRecording::LastType:
    break;
  }
  DisplayExitCode(item,(RDRecording::ExitCode)q->value(28).toInt(),
		  q->value(45).toString());
}


void MainWidget::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *l;

  catch_recordings_list->clear();
  sql=RefreshSql();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new RDListViewItem(catch_recordings_list);
    RefreshRow(q,l);
  }
  delete q;
}


void MainWidget::RefreshLine(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  int id=item->text(28).toInt();
  sql=RefreshSql()+"where "+
    QString().sprintf("RECORDINGS.ID=%d",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    RefreshRow(q,item);
  }
  else {  // Event removed
    delete item;
  }
  delete q;
}


void MainWidget::UpdateExitCode(RDListViewItem *item)
{
  RDRecording::ExitCode code=RDRecording::InternalError;
  QString err_text=tr("Unknown");
  QString sql=QString().sprintf("select RECORDINGS.EXIT_CODE,\
                                 CUTS.ORIGIN_NAME,CUTS.ORIGIN_DATETIME,\
                                 RECORDINGS.EXIT_TEXT \
                                 from RECORDINGS left join CUTS \
                                 on RECORDINGS.CUT_NAME=CUTS.CUT_NAME \
                                 where RECORDINGS.ID=%d",
				item->text(28).toInt());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    code=(RDRecording::ExitCode)q->value(0).toInt();
    err_text=q->value(3).toString();
    item->setText(15,q->value(1).toString()+" - "+q->value(2).toDateTime().
		  toString("M/dd/yyyy hh:mm:ss"));
  }
  else {
    item->setText(15,"");
  }
  delete q; 
  DisplayExitCode(item,code,err_text);
}


void MainWidget::DisplayExitCode(RDListViewItem *item,
				 RDRecording::ExitCode code,
				 const QString &err_text)
{
  item->setText(31,QString().sprintf("%u",code));
  switch(code) {
      case RDRecording::Ok:
      case RDRecording::Downloading:
      case RDRecording::Uploading:
      case RDRecording::RecordActive:
      case RDRecording::PlayActive:
      case RDRecording::Waiting:
	item->setText(30,RDRecording::exitString(code));
	break;
	
      case RDRecording::Short:
      case RDRecording::LowLevel:
      case RDRecording::HighLevel:
      case RDRecording::Interrupted:
      case RDRecording::DeviceBusy:
      case RDRecording::NoCut:
      case RDRecording::UnknownFormat:
	item->setText(30,RDRecording::exitString(code));
	item->setBackgroundColor(EVENT_ERROR_COLOR);
	break;
	
      case RDRecording::ServerError:
      case RDRecording::InternalError:
	item->setText(30,RDRecording::exitString(code)+": "+err_text);
	item->setBackgroundColor(EVENT_ERROR_COLOR);
	break;
  }
}


QString MainWidget::GetSourceName(QString station,int matrix,int input)
{
  QString input_name;
  QString sql=QString("select NAME from INPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(station)+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrix)+
    QString().sprintf("(NUMBER=%d)",input);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    input_name=q->value(0).toString();
  }
  delete q;
  return input_name;
}


QString MainWidget::GetDestinationName(QString station,int matrix,int output)
{
  QString output_name;
  QString sql=QString("select NAME from OUTPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(station)+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrix)+
    QString().sprintf("(NUMBER=%d)",output);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output_name=q->value(0).toString();
  }
  delete q;
  return output_name;
}


RDListViewItem *MainWidget::GetItem(int id)
{
  RDListViewItem *item=(RDListViewItem *)catch_recordings_list->firstChild();
  if(item==NULL) {
    return NULL;
  }
  do {
    if(item->text(28).toInt()==id) {
      return item;
    }
  } while ((item=(RDListViewItem *)item->nextSibling())!=NULL);
  return NULL;
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
    if(catch_connect[i]->stationName()==station.lower()) {
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

void MainWidget::LoadGeometry()
{
  QString geometry_file = GeometryFile();
  if(geometry_file==NULL) {
    return;
  }
  RDProfile *profile=new RDProfile();
  profile->setSource(geometry_file);
  resize(profile->intValue("RDCatch","Width",sizeHint().width()),
	 profile->intValue("RDCatch","Height",sizeHint().height()));

  delete profile;
}


void MainWidget::SaveGeometry()
{
  QString geometry_file = GeometryFile();
  if(geometry_file==NULL) {
    return;
  }
  FILE *file=fopen(geometry_file,"w");
  if(file==NULL) {
    return;
  }
  fprintf(file,"[RDCatch]\n");
  fprintf(file,"Width=%d\n",geometry().width());
  fprintf(file,"Height=%d\n",geometry().height());
  fclose(file);
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString("/usr/share/qt4/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdcatch_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->show();
  return a.exec();
}
