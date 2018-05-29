// rdairplay.cpp
//
// The On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2010,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qsqlpropertymap.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qsignalmapper.h>
#include <qtimer.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qpainter.h>

#include <dbversion.h>
#include <rd.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rdaudio_port.h>
#include <rdcart_search_text.h>
#include <rdmixer.h>
#include <rdcheck_daemons.h>
#include <rdcart_dialog.h>
#include <rdmacro.h>
#include <rdcmd_switch.h>
#include <rdgetpasswd.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdhotkeys.h>
#include <rdhotkeylist.h>

#include "globals.h"
#include "rdairplay.h"
#include "wall_clock.h"

//
// Global Resources
//
RDAudioPort *rdaudioport_conf;
RDEventPlayer *rdevent_player;
RDCartDialog *rdcart_dialog;
MainWidget *prog_ptr;
RDHotKeyList *rdkeylist;
RDHotkeys *rdhotkeys;

//
// Icons
//
#include "../icons/rdairplay-128x128.xpm"
#include "../icons/rdairplay-22x22.xpm"

//
// Prototypes
//
void SigHandler(int signo);

void MainWidget::logLine(RDConfig::LogPriority prio,const QString &s)
{
  LogLine(prio,s);
}

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  prog_ptr=this;
  QString str;
  int cards[3];
  int ports[3];
  QString start_rmls[3];
  QString stop_rmls[3];
  QPixmap *mainmap=NULL;
  QPixmap *pm=NULL;
  QPainter *pd=NULL;
  QString err_msg;

  air_refresh_pixmap=NULL;
  air_panel=NULL;

  //
  // Ensure Single Instance
  //
  air_lock=new RDInstanceLock(QString().sprintf("%s/.rdairplaylock",
					       (const char *)RDHomeDir()));
  if(!air_lock->lock()) {
    QMessageBox::information(this,tr("RDAirPlay"),
			     tr("Multiple instances not allowed!"));
    exit(1);
  }

  //
  // Splash Screen
  //
  air_splash_screen=new QSplashScreen(QPixmap(rdairplay_128x128_xpm));
  air_splash_screen->hide();
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(clearSplashData()));
  timer->start(AIR_PLAY_SPLASH_TIME,true);

  //
  // Get the Startup Date/Time
  //
  air_startup_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());

  //
  // Ensure that system daemons are running
  //
  RDInitializeDaemons();

  //
  // Open the Database
  //
  rda=new RDApplication("RDAirPlay","rdairplay",RDAIRPLAY_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDAirPlay - "+tr("Error"),err_msg);
    exit(1);
  }
  connect(RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
	  this,SLOT(logLine(RDConfig::LogPriority,const QString &))); 


  //
  // Read Command Options
  //
  QString lineno;
  for(unsigned i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_start_line[i]=0;
    air_start_start[i]=false;
    for(unsigned j=0;j<rda->cmdSwitch()->keys();j++) {
      if(rda->cmdSwitch()->key(j)==QString().sprintf("--log%u",i+1)) {
	air_start_logname[i]=rda->cmdSwitch()->value(j);
	for(unsigned k=0;k<rda->cmdSwitch()->value(j).length();k++) {
	  if(rda->cmdSwitch()->value(j).at(k)==QChar(':')) {
	    air_start_logname[i]=
	      RDDateTimeDecode(rda->cmdSwitch()->value(j).left(k),
			       air_startup_datetime,
			       rda->station(),rda->config());
	    lineno=rda->cmdSwitch()->value(j).right(rda->cmdSwitch()->value(j).
						    length()-(k+1));
	    if(lineno.right(1)=="+") {
	      air_start_start[i]=true;
	      lineno=lineno.left(lineno.length()-1);
	    }
	    air_start_line[i]=lineno.toInt();
	  }
	}
	rda->cmdSwitch()->setProcessed(j,true);
      }
    }
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDAirPlay - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Fix the Window Size
  //
#ifndef RESIZABLE
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
#endif  // RESIZABLE

  //
  // Initialize the Random Number Generator
  //
  srandom(QTime::currentTime().msec());

  //
  // Generate Fonts
  //
  QFont default_font("Helvetica",12,QFont::Normal);
  default_font.setPixelSize(12);
  qApp->setFont(default_font);
  QFont button_font=QFont("Helvetica",16,QFont::Bold);
  button_font.setPixelSize(16);
  for(unsigned i=0;i<AIR_MESSAGE_FONT_QUANTITY;i++) {
    air_message_fonts[i]=QFont("helvetica",12+2*i,QFont::Normal);
    air_message_fonts[i].setPixelSize(12+2*i);
    air_message_metrics[i]=new QFontMetrics(air_message_fonts[i]);
  }

  //
  // Create And Set Icon
  //
  air_rivendell_map=new QPixmap(rdairplay_22x22_xpm);
  setIcon(*air_rivendell_map);

  air_start_next=false;
  air_next_button=0;
  air_action_mode=StartButton::Play;

  logfile=rda->config()->airplayLogname();

  str=QString("RDAirPlay")+" v"+VERSION+" - "+tr("Host:");
  setCaption(str+" "+rda->config()->stationName());

  //
  // Master Clock Timer
  //
  air_master_timer=new QTimer(this);
  connect(air_master_timer,SIGNAL(timeout()),this,SLOT(masterTimerData()));
  air_master_timer->start(MASTER_TIMER_INTERVAL);

  //
  // Allocate Global Resources
  //
  rdairplay_previous_exit_code=rda->airplayConf()->exitCode();
  rda->airplayConf()->setExitCode(RDAirPlayConf::ExitDirty);
  air_clear_filter=rda->airplayConf()->clearFilter();
  air_bar_action=rda->airplayConf()->barAction();
  air_op_mode_style=rda->airplayConf()->opModeStyle();
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_op_mode[i]=RDAirPlayConf::Previous;
  }
  air_editor_cmd=rda->station()->editorPath();
  mainmap=new QPixmap(rda->airplayConf()->skinPath());
  if(mainmap->isNull()||(mainmap->width()<1024)||(mainmap->height()<738)) {
    delete mainmap;
    mainmap=NULL;
  }
  else {
    setErasePixmap(*mainmap);
  }

  //
  // Load GPIO Channel Configuration
  //
  for(unsigned i=0;i<RDAirPlayConf::LastChannel;i++) {
    RDAirPlayConf::Channel chan=(RDAirPlayConf::Channel)i;
    air_start_gpi_matrices[i]=rda->airplayConf()->startGpiMatrix(chan);
    air_start_gpi_lines[i]=rda->airplayConf()->startGpiLine(chan)-1;
    air_start_gpo_matrices[i]=rda->airplayConf()->startGpoMatrix(chan);
    air_start_gpo_lines[i]=rda->airplayConf()->startGpoLine(chan)-1;
    air_stop_gpi_matrices[i]=rda->airplayConf()->stopGpiMatrix(chan);
    air_stop_gpi_lines[i]=rda->airplayConf()->stopGpiLine(chan)-1;
    air_stop_gpo_matrices[i]=rda->airplayConf()->stopGpoMatrix(chan);
    air_stop_gpo_lines[i]=rda->airplayConf()->stopGpoLine(chan)-1;
    air_channel_gpio_types[i]=rda->airplayConf()->gpioType(chan);
    air_audio_channels[i]=
      AudioChannel(rda->airplayConf()->card(chan),rda->airplayConf()->port(chan));

    if((rda->airplayConf()->card(chan)>=0)&&(rda->airplayConf()->port(chan)>=0)) {
      int achan=
	AudioChannel(rda->airplayConf()->card(chan),rda->airplayConf()->port(chan));
      if(air_channel_timers[0][achan]==NULL) {
	air_channel_timers[0][achan]=new QTimer(this);
	air_channel_timers[1][achan]=new QTimer(this);
      }
    }
  }

  //
  // Fixup Main Log GPIO Channel Assignments
  //
  if(((rda->airplayConf()->card(RDAirPlayConf::MainLog1Channel)==
      rda->airplayConf()->card(RDAirPlayConf::MainLog2Channel))&&
     (rda->airplayConf()->port(RDAirPlayConf::MainLog1Channel)==
      rda->airplayConf()->port(RDAirPlayConf::MainLog2Channel)))||
     rda->airplayConf()->card(RDAirPlayConf::MainLog2Channel)<0) {
    air_start_gpi_matrices[RDAirPlayConf::MainLog2Channel]=-1;
    air_start_gpo_matrices[RDAirPlayConf::MainLog2Channel]=-1;
    air_stop_gpi_matrices[RDAirPlayConf::MainLog2Channel]=
      air_stop_gpi_matrices[RDAirPlayConf::MainLog1Channel];
    air_stop_gpo_matrices[RDAirPlayConf::MainLog2Channel]=-1;
  }

  //
  // CAE Connection
  //
  rda->cae()->connectHost();

  //
  // Set Audio Assignments
  //
  air_segue_length=rda->airplayConf()->segueLength()+1;
  RDSetMixerPorts(rda->config()->stationName(),rda->cae());

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),this,SLOT(ripcConnected(bool)));
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));
  connect(rda->ripc(),SIGNAL(gpiStateChanged(int,int,bool)),
	  this,SLOT(gpiStateChangedData(int,int,bool)));

  //
  // Macro Player
  //
  rdevent_player=new RDEventPlayer(rda->ripc(),this);

  //
  // User
  //
  //  rduser=NULL;

  //
  // UDP Transmission Socket
  //
  air_nownext_socket=new QSocketDevice(QSocketDevice::Datagram);

  //
  // Log Machines
  //
  QSignalMapper *reload_mapper=new QSignalMapper(this);
  connect(reload_mapper,SIGNAL(mapped(int)),this,SLOT(logReloadedData(int)));
  QSignalMapper *rename_mapper=new QSignalMapper(this);
  connect(rename_mapper,SIGNAL(mapped(int)),this,SLOT(logRenamedData(int)));
  QString default_svcname=rda->airplayConf()->defaultSvc();
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log[i]=new RDLogPlay(i,rdevent_player,air_nownext_socket,"",
			     &air_plugin_hosts);
    air_log[i]->setDefaultServiceName(default_svcname);
    air_log[i]->setNowCart(rda->airplayConf()->logNowCart(i));
    air_log[i]->setNextCart(rda->airplayConf()->logNextCart(i));
    reload_mapper->setMapping(air_log[i],i);
    connect(air_log[i],SIGNAL(reloaded()),reload_mapper,SLOT(map()));
    rename_mapper->setMapping(air_log[i],i);
    connect(air_log[i],SIGNAL(renamed()),rename_mapper,SLOT(map()));
    connect(air_log[i],SIGNAL(refreshStatusChanged(bool)),
	    this,SLOT(refreshStatusChangedData(bool)));
    connect(air_log[i],SIGNAL(channelStarted(int,int,int,int)),
	    this,SLOT(logChannelStartedData(int,int,int,int)));
    connect(air_log[i],SIGNAL(channelStopped(int,int,int,int)),
	    this,SLOT(logChannelStoppedData(int,int,int,int)));
  }
  connect(air_log[0],SIGNAL(transportChanged()),
	  this,SLOT(transportChangedData()));

  //
  // Audio Channel Assignments
  //
  air_cue_card=rda->airplayConf()->card(RDAirPlayConf::CueChannel);
  air_cue_port=rda->airplayConf()->port(RDAirPlayConf::CueChannel);
  for(int i=0;i<3;i++) {
    air_meter_card[i]=rda->airplayConf()->card((RDAirPlayConf::Channel)i);
    air_meter_port[i]=rda->airplayConf()->port((RDAirPlayConf::Channel)i);
    cards[i]=rda->airplayConf()->card((RDAirPlayConf::Channel)i);
    ports[i]=rda->airplayConf()->port((RDAirPlayConf::Channel)i);
    start_rmls[i]=rda->airplayConf()->startRml((RDAirPlayConf::Channel)i);
    stop_rmls[i]=rda->airplayConf()->stopRml((RDAirPlayConf::Channel)i);
  }
  if((air_meter_card[1]<0)||(air_meter_port[1]<0)) {  // Fixup disabled main log port 2 playout
    air_meter_card[1]=air_meter_card[0];
    air_meter_port[1]=air_meter_port[0];
    cards[1]=cards[0];
    ports[1]=ports[0];
  }
  air_log[0]->setChannels(cards,ports,start_rmls,stop_rmls);

  for(int i=0;i<2;i++) {
    cards[i]=rda->airplayConf()->card(RDAirPlayConf::AuxLog1Channel);
    ports[i]=rda->airplayConf()->port(RDAirPlayConf::AuxLog1Channel);
    start_rmls[i]=rda->airplayConf()->startRml(RDAirPlayConf::AuxLog1Channel);
    stop_rmls[i]=rda->airplayConf()->stopRml(RDAirPlayConf::AuxLog1Channel);
  }
  air_log[1]->setChannels(cards,ports,start_rmls,stop_rmls);

  for(int i=0;i<2;i++) {
    cards[i]=rda->airplayConf()->card(RDAirPlayConf::AuxLog2Channel);
    ports[i]=rda->airplayConf()->port(RDAirPlayConf::AuxLog2Channel);
    start_rmls[i]=rda->airplayConf()->startRml(RDAirPlayConf::AuxLog2Channel);
    stop_rmls[i]=rda->airplayConf()->stopRml(RDAirPlayConf::AuxLog2Channel);
  }
  air_log[2]->setChannels(cards,ports,start_rmls,stop_rmls);

  //
  // Cart Picker
  //
  rdcart_dialog=
    new RDCartDialog(&air_add_filter,&air_add_group,&air_add_schedcode,this);

  //
  // Wall Clock
  //
  WallClock *clock=new WallClock(this);
  clock->
    setGeometry(10,5,clock->sizeHint().width(),clock->sizeHint().height());
  clock->setCheckSyncEnabled(rda->airplayConf()->checkTimesync());
  connect(air_master_timer,SIGNAL(timeout()),clock,SLOT(tickClock()));
 clock->setFocusPolicy(QWidget::NoFocus);
  connect(clock,SIGNAL(timeModeChanged(RDAirPlayConf::TimeMode)),
	  this,SLOT(timeModeData(RDAirPlayConf::TimeMode)));

  //
  // Post Counter
  //
  air_post_counter=new PostCounter(this);
  air_post_counter->setGeometry(220,5,air_post_counter->sizeHint().width(),
				air_post_counter->sizeHint().height());
  air_post_counter->setPostPoint(QTime(),0,false,false);
  air_post_counter->setFocusPolicy(QWidget::NoFocus);
  connect(air_master_timer,SIGNAL(timeout()),
	  air_post_counter,SLOT(tickCounter()));
  connect(air_log[0],SIGNAL(postPointChanged(QTime,int,bool,bool)),
	  air_post_counter,SLOT(setPostPoint(QTime,int,bool,bool)));

  //
  // Pie Counter
  //
  air_pie_counter=new PieCounter(rda->airplayConf()->pieCountLength(),this);
  air_pie_counter->setGeometry(426,5,air_pie_counter->sizeHint().width(),
				air_pie_counter->sizeHint().height());
  air_pie_counter->setCountLength(rda->airplayConf()->pieCountLength());
  air_pie_end=rda->airplayConf()->pieEndPoint();
  air_pie_counter->setOpMode(air_op_mode[0]);
  air_pie_counter->setFocusPolicy(QWidget::NoFocus);
  if(mainmap!=NULL) {
    pm=new QPixmap(1024,738);
    pd=new QPainter(pm);
    pd->drawPixmap(-426,-5,*mainmap);
    pd->end();
    air_pie_counter->setErasePixmap(*pm);
    delete pd;
    delete pm;
  }
  connect(air_master_timer,SIGNAL(timeout()),
	  air_pie_counter,SLOT(tickCounter()));
  connect(rda->ripc(),SIGNAL(onairFlagChanged(bool)),
	  air_pie_counter,SLOT(setOnairFlag(bool)));

  //
  // Audio Meter
  //
  air_stereo_meter=new RDStereoMeter(this);
  air_stereo_meter->setGeometry(50,70,air_stereo_meter->sizeHint().width(),
				air_stereo_meter->sizeHint().height());
  air_stereo_meter->setMode(RDSegMeter::Peak);
 air_stereo_meter->setFocusPolicy(QWidget::NoFocus);
  if(rda->config()->useStreamMeters()) {
    air_stereo_meter->hide();
  }

  //
  // Message Label
  //
  air_message_label=new RDLabel(this);
  air_message_label->setGeometry(sizeHint().width()-425,70,
		MESSAGE_WIDGET_WIDTH,air_stereo_meter->sizeHint().height());
  air_message_label->setWordWrapEnabled(true);
  air_message_label->setLineWidth(1);
  air_message_label->setMidLineWidth(1);
  air_message_label->setFrameStyle(QFrame::Box|QFrame::Raised);
  air_message_label->setAlignment(AlignCenter);
 air_message_label->setFocusPolicy(QWidget::NoFocus);

  //
  // Stop Counter
  //
  air_stop_counter=new StopCounter(this);
  air_stop_counter->setGeometry(600,5,air_stop_counter->sizeHint().width(),
				air_stop_counter->sizeHint().height());
  air_stop_counter->setTime(QTime(0,0,0));
 air_stop_counter->setFocusPolicy(QWidget::NoFocus);
  connect(air_master_timer,SIGNAL(timeout()),
	  air_stop_counter,SLOT(tickCounter()));
  connect(air_log[0],SIGNAL(nextStopChanged(QTime)),
	  air_stop_counter,SLOT(setTime(QTime)));

  //
  // Mode Display/Button
  //
  air_mode_display=new ModeDisplay(this);
  air_mode_display->
    setGeometry(sizeHint().width()-air_mode_display->sizeHint().width()-10,
		5,air_mode_display->sizeHint().width(),
		air_mode_display->sizeHint().height());
  air_mode_display->setFocusPolicy(QWidget::NoFocus);
  air_mode_display->setOpModeStyle(air_op_mode_style);
  connect(air_mode_display,SIGNAL(clicked()),this,SLOT(modeButtonData()));

  //
  // Create Palettes
  //
  auto_color=
    QPalette(QColor(BUTTON_MODE_AUTO_COLOR),backgroundColor());
  manual_color=
    QPalette(QColor(BUTTON_MODE_MANUAL_COLOR),backgroundColor());
  active_color=palette();
  active_color.setColor(QPalette::Active,QColorGroup::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  active_color.setColor(QPalette::Active,QColorGroup::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  active_color.setColor(QPalette::Active,QColorGroup::Background,
			backgroundColor());
  active_color.setColor(QPalette::Inactive,QColorGroup::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  active_color.setColor(QPalette::Inactive,QColorGroup::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  active_color.setColor(QPalette::Inactive,QColorGroup::Background,
			backgroundColor());

  //
  // Add Button
  //
  air_add_button=new RDPushButton(this);
  air_add_button->setGeometry(10,sizeHint().height()-65,80,60);
  air_add_button->setFont(button_font);
  air_add_button->setText(tr("ADD"));
 air_add_button->setFocusPolicy(QWidget::NoFocus);
  connect(air_add_button,SIGNAL(clicked()),this,SLOT(addButtonData()));

  //
  // Delete Button
  //
  air_delete_button=new RDPushButton(this);
  air_delete_button->setGeometry(100,sizeHint().height()-65,80,60);
  air_delete_button->setFont(button_font);
  air_delete_button->setText(tr("DEL"));
  air_delete_button->setFlashColor(AIR_FLASH_COLOR);
 air_delete_button->setFocusPolicy(QWidget::NoFocus);
  connect(air_delete_button,SIGNAL(clicked()),this,SLOT(deleteButtonData()));

  //
  // Move Button
  //
  air_move_button=new RDPushButton(this);
  air_move_button->setGeometry(190,sizeHint().height()-65,80,60);
  air_move_button->setFont(button_font);
  air_move_button->setText(tr("MOVE"));
  air_move_button->setFlashColor(AIR_FLASH_COLOR);
 air_move_button->setFocusPolicy(QWidget::NoFocus);
  connect(air_move_button,SIGNAL(clicked()),this,SLOT(moveButtonData()));

  //
  // Copy Button
  //
  air_copy_button=new RDPushButton(this);
  air_copy_button->setGeometry(280,sizeHint().height()-65,80,60);
  air_copy_button->setFont(button_font);
  air_copy_button->setText(tr("COPY"));
  air_copy_button->setFlashColor(AIR_FLASH_COLOR);
  air_copy_button->setFocusPolicy(QWidget::NoFocus);
  connect(air_copy_button,SIGNAL(clicked()),this,SLOT(copyButtonData()));

  //
  // Refresh Indicator
  //
  air_refresh_label=new RDLabel(this);
  air_refresh_label->setGeometry(390,sizeHint().height()-65,120,60);
  air_refresh_label->setFont(button_font);
  QPalette p=palette();
  p.setColor(QColorGroup::Foreground,red);
  air_refresh_label->setPalette(p);
  air_refresh_label->setAlignment(AlignCenter);
  if(mainmap!=NULL) {
    air_refresh_pixmap=new QPixmap(1024,738);
    pd=new QPainter(air_refresh_pixmap);
    pd->drawPixmap(-390,-sizeHint().height()+65,*mainmap);
    pd->end();
    air_refresh_label->setErasePixmap(*air_refresh_pixmap);
    delete pd;
  }

  //
  // Meter Timer
  //
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(meterData()));
  timer->start(RD_METER_UPDATE_INTERVAL);

  //
  // Sound Panel Array
  //
  if (rda->airplayConf()->panels(RDAirPlayConf::StationPanel) || 
      rda->airplayConf()->panels(RDAirPlayConf::UserPanel)){
    int card=-1;
    air_panel=
      new RDSoundPanel(AIR_PANEL_BUTTON_COLUMNS,AIR_PANEL_BUTTON_ROWS,
		       rda->airplayConf()->panels(RDAirPlayConf::StationPanel),
		       rda->airplayConf()->panels(RDAirPlayConf::UserPanel),
		       rda->airplayConf()->flashPanel(),
		       rda->airplayConf()->buttonLabelTemplate(),false,
		       rdevent_player,rdcart_dialog,this);
    air_panel->setLogfile(rda->config()->airplayLogname());
    air_panel->setGeometry(510,140,air_panel->sizeHint().width(),
			 air_panel->sizeHint().height());
    if(mainmap!=NULL) {
      pm=new QPixmap(1024,738);
      pd=new QPainter(pm);
      pd->drawPixmap(-510,-140,*mainmap);
      pd->end();
      air_panel->setErasePixmap(*pm);
      delete pd;
      delete pm;
    }
    air_panel->setPauseEnabled(rda->airplayConf()->panelPauseEnabled());
    air_panel->setCard(0,rda->airplayConf()->card(RDAirPlayConf::SoundPanel1Channel));
    air_panel->setPort(0,rda->airplayConf()->port(RDAirPlayConf::SoundPanel1Channel));
    air_panel->setFocusPolicy(QWidget::NoFocus);
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel2Channel))<0) {
      air_panel->setCard(1,air_panel->card(RDAirPlayConf::MainLog1Channel));
      air_panel->setPort(1,air_panel->port(RDAirPlayConf::MainLog1Channel));
    }
    else {
      air_panel->setCard(1,card);
      air_panel->setPort(1,rda->airplayConf()->port(RDAirPlayConf::SoundPanel2Channel));
    }
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel3Channel))<0) {
      air_panel->setCard(2,air_panel->card(RDAirPlayConf::MainLog2Channel));
      air_panel->setPort(2,air_panel->port(RDAirPlayConf::MainLog2Channel));
    }
    else {
      air_panel->setCard(2,card);
      air_panel->setPort(2,rda->airplayConf()->port(RDAirPlayConf::SoundPanel3Channel));
    }
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel4Channel))<0) {
      air_panel->setCard(3,air_panel->card(RDAirPlayConf::SoundPanel1Channel));
      air_panel->setPort(3,air_panel->port(RDAirPlayConf::SoundPanel1Channel));
    }
    else {
      air_panel->setCard(3,card);
      air_panel->setPort(3,rda->airplayConf()->port(RDAirPlayConf::SoundPanel4Channel));
    }
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel5Channel))<0) {
      air_panel->setCard(4,air_panel->card(RDAirPlayConf::CueChannel));
      air_panel->setPort(4,air_panel->port(RDAirPlayConf::CueChannel));
    }
    else {
      air_panel->setCard(4,card);
      air_panel->setPort(4,rda->airplayConf()->
			 port(RDAirPlayConf::SoundPanel5Channel));
    }
    air_panel->setRmls(0,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel1Channel),
		  rda->airplayConf()->stopRml(RDAirPlayConf::SoundPanel1Channel));
    air_panel->setRmls(1,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel2Channel),
		  rda->airplayConf()->stopRml(RDAirPlayConf::SoundPanel2Channel));
    air_panel->setRmls(2,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel3Channel),
		  rda->airplayConf()->stopRml(RDAirPlayConf::SoundPanel3Channel));
    air_panel->setRmls(3,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel4Channel),
		  rda->airplayConf()->stopRml(RDAirPlayConf::SoundPanel4Channel));
    air_panel->setRmls(4,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel5Channel),
		  rda->airplayConf()->stopRml(RDAirPlayConf::SoundPanel5Channel));
    int next_output=0;
    int channum[2];
    bool assigned=false;
    if((air_log[0]->card(0)==air_log[0]->card(RDAirPlayConf::MainLog2Channel))&&
       (air_log[0]->port(0)==air_log[0]->port(RDAirPlayConf::MainLog2Channel))) {
      next_output=2;
      channum[0]=1;
      channum[1]=1;
    }
    else {
      next_output=3;
      channum[0]=1;
      channum[1]=2;
    }
    for(int i=0;i<PANEL_MAX_OUTPUTS;i++) {
      air_panel->setOutputText(i,QString().sprintf("%d",next_output++));
      assigned=false;
      for(int j=0;j<2;j++) {
	if((air_panel->card((RDAirPlayConf::Channel)i)==air_log[0]->card(j))&&
	   (air_panel->port((RDAirPlayConf::Channel)i)==air_log[0]->port(j))) {
	  air_panel->setOutputText(i,QString().sprintf("%d",channum[j]));
	  next_output--;
	  assigned=true;
	  j=2;
	}
      }
      if(!assigned) {
	for(int j=0;j<i;j++) {
	  if((i!=j)&&(air_panel->card((RDAirPlayConf::Channel)i)==air_panel->card(j))&&
	     (air_panel->port((RDAirPlayConf::Channel)i)==air_panel->port(j))) {
	    air_panel->setOutputText(i,air_panel->outputText(j));
	    next_output--;
	    j=PANEL_MAX_OUTPUTS;
	  }
	}
      }
    }

    air_panel->setSvcName(rda->airplayConf()->defaultSvc());
    connect(rda->ripc(),SIGNAL(userChanged()),air_panel,SLOT(changeUser()));
    connect(air_master_timer,SIGNAL(timeout()),air_panel,SLOT(tickClock()));
    connect(air_panel,SIGNAL(selectClicked(unsigned,int,int)),
	    this,SLOT(selectClickedData(unsigned,int,int)));
    connect(air_panel,SIGNAL(channelStarted(int,int,int)),
	    this,SLOT(panelChannelStartedData(int,int,int)));
    connect(air_panel,SIGNAL(channelStopped(int,int,int)),
	    this,SLOT(panelChannelStoppedData(int,int,int)));
  }

  //
  // Full Log List
  //
  air_pause_enabled=rda->airplayConf()->pauseEnabled();
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_list[i]=new ListLog(air_log[i],rda->cae(),i,air_pause_enabled,this);
    air_log_list[i]->setGeometry(510,140,air_log_list[i]->sizeHint().width(),
			      air_log_list[i]->sizeHint().height());
    air_log_list[i]->hide();
    connect(air_log_list[i],SIGNAL(selectClicked(int,int,RDLogLine::Status)),
	    this,SLOT(selectClickedData(int,int,RDLogLine::Status)));
    connect(air_log_list[i],SIGNAL(cartDropped(int,int,RDLogLine *)),
	    this,SLOT(cartDroppedData(int,int,RDLogLine *)));
  }

  //
  // Full Log Buttons
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(fullLogButtonData(int)));
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_button[i]=new QPushButton(this);
    air_log_button[i]->setGeometry(647+i*123,sizeHint().height()-65,118,60);
    air_log_button[i]->setFont(button_font);
    air_log_button[i]->setFocusPolicy(QWidget::NoFocus);
    mapper->setMapping(air_log_button[i],i);
    connect(air_log_button[i],SIGNAL(clicked()),mapper,SLOT(map()));
  }
  air_log_button[0]->setText(tr("Main Log\n[--]"));
    air_log_button[1]->setText(tr("Aux 1 Log\n[--]"));
    if((!rda->airplayConf()->showAuxButton(0))||
       (!air_log[1]->channelsValid())) {
    air_log_button[1]->hide();
  }
    air_log_button[2]->setText(tr("Aux 2 Log\n[--]"));
    if((!rda->airplayConf()->showAuxButton(1))||
       (!air_log[2]->channelsValid())) {
    air_log_button[2]->hide();
  }

  //
  // Empty Cart
  //
  air_empty_cart=new RDEmptyCart(this);
  air_empty_cart->setGeometry(520,sizeHint().height()-51,32,32);
  if(!rda->station()->enableDragdrop()) {
    air_empty_cart->hide();
  }

  //
  // SoundPanel Button
  //
  air_panel_button=new QPushButton(this);
  air_panel_button->setGeometry(562,sizeHint().height()-65,80,60);
  air_panel_button->setFont(button_font);
  air_panel_button->setText(tr("Sound\nPanel"));
  air_panel_button->setPalette(active_color);
  air_panel_button->setFocusPolicy(QWidget::NoFocus);
  connect(air_panel_button,SIGNAL(clicked()),this,SLOT(panelButtonData()));
  if (rda->airplayConf()->panels(RDAirPlayConf::StationPanel) || 
      rda->airplayConf()->panels(RDAirPlayConf::UserPanel)){
  } 
  else {
    air_panel_button->hide();
    air_log_button[0]->setPalette (active_color);
    air_log_list[0]->show();
  }	  


  //
  // Button Log
  //
  air_button_list=new ButtonLog(air_log[0],rda->cae(),0,rda->airplayConf(),
				air_pause_enabled,this);
  air_button_list->setGeometry(10,140,air_button_list->sizeHint().width(),
			       air_button_list->sizeHint().height());
  if(mainmap!=NULL) {
    pm=new QPixmap(1024,738);
    pd=new QPainter(pm);
    pd->drawPixmap(-10,-140,*mainmap);
    pd->end();
    air_button_list->setErasePixmap(*pm);
    delete pd;
    delete pm;
  }
  connect(air_button_list,SIGNAL(selectClicked(int,int,RDLogLine::Status)),
	  this,SLOT(selectClickedData(int,int,RDLogLine::Status)));
  connect(air_button_list,SIGNAL(cartDropped(int,int,RDLogLine *)),
	  this,SLOT(cartDroppedData(int,int,RDLogLine *)));

  //
  // Set Startup Mode
  //
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    switch(rda->airplayConf()->logStartMode(i)) {
      case RDAirPlayConf::Manual:
	SetManualMode(i);
	break;
	
      case RDAirPlayConf::LiveAssist:
	SetLiveAssistMode(i);
	break;
	
      case RDAirPlayConf::Auto:
	SetAutoMode(i);
	break;
	
      case RDAirPlayConf::Previous:
	if(air_op_mode_style==RDAirPlayConf::Unified) {
	  SetMode(i,rda->airplayConf()->opMode(0));
	}
	else {
	  SetMode(i,rda->airplayConf()->opMode(i));
	}
	break;
    }
  }

  //
  // Load Plugins
  //
  QString sql;
  RDSqlQuery *q;

  //
  // Load Plugins
  //
  sql=QString().sprintf("select PLUGIN_PATH,PLUGIN_ARG from NOWNEXT_PLUGINS \
                         where (STATION_NAME=\"%s\")&&(LOG_MACHINE=0)",
			(const char *)
			RDEscapeString(rda->config()->stationName()));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    air_plugin_hosts.
      push_back(new RDRLMHost(q->value(0).toString(),q->value(1).toString(),
			      air_nownext_socket,this));
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("Loading RLM \"%s\"",
		    (const char *)q->value(0).toString()));
    if(!air_plugin_hosts.back()->load()) {
      LogLine(RDConfig::LogWarning,QString().
	      sprintf("Failed to load RLM \"%s\"",
		      (const char *)q->value(0).toString()));
    }
  }
  delete q;

  // Create the HotKeyList object
  //
  rdkeylist=new RDHotKeyList();
  rdhotkeys=new RDHotkeys(rda->config()->stationName(),"rdairplay");
  AltKeyHit=false;
  CtrlKeyHit=false;

  //
  // Set Signal Handlers
  //
  signal(SIGCHLD,SigHandler);

  //
  // Start the RIPC Connection
  //
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // (Perhaps) Lock Memory
  //
  if(rda->config()->lockRdairplayMemory()) {
    if(mlockall(MCL_CURRENT|MCL_FUTURE)<0) {
      QMessageBox::warning(this,"RDAirPlay - "+tr("Memory Warning"),
			   tr("Unable to lock all memory")+
			   " ["+strerror(errno)+"].");
    }
  }

  LogLine(RDConfig::LogInfo,"RDAirPlay started");
}


QSize MainWidget::sizeHint() const
{
  return QSize(1024,738);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::ripcConnected(bool state)
{
  QString logname;
  QHostAddress addr;
  QString sql;
  RDSqlQuery *q;
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  addr.setAddress("127.0.0.1");
  rml.setAddress(addr);
  rml.setEchoRequested(false);

  //
  // Check Channel Assignments
  //
  if(!air_log[0]->channelsValid()) {
    QMessageBox::warning(this,"RDAirPlay - "+tr("Warning"),
			 tr("Main Log channel assignments are invalid!"));
  }

  //
  // Get Onair Flag State
  //
  rda->ripc()->sendOnairFlag();

  //
  // Load Initial Logs
  //
  for(unsigned i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    if(air_start_logname[i].isNull()) {
      switch(rda->airplayConf()->startMode(i)) {
	  case RDAirPlayConf::StartEmpty:
	    break;
	    
	  case RDAirPlayConf::StartPrevious:
	    air_start_logname[i]=
	      RDDateTimeDecode(rda->airplayConf()->currentLog(i),
			       air_startup_datetime,rda->station(),rda->config());
	    if(!air_start_logname[i].isEmpty()) {
	      if(rdairplay_previous_exit_code==RDAirPlayConf::ExitDirty) {
		if((air_start_line[i]=rda->airplayConf()->logCurrentLine(i))>=0) {
		  air_start_start[i]=rda->airplayConf()->autoRestart(i)&&
		    rda->airplayConf()->logRunning(i);
		}
	      }
	      else {
		air_start_line[i]=0;
		air_start_start[i]=false;
	      }
	    }
	    break;

	  case RDAirPlayConf::StartSpecified:
	    air_start_logname[i]=
	      RDDateTimeDecode(rda->airplayConf()->logName(i),
			       air_startup_datetime,rda->station(),rda->config());
	    if(!air_start_logname[i].isEmpty()) {
	      if(rdairplay_previous_exit_code==RDAirPlayConf::ExitDirty) {
		if(air_start_logname[i]==rda->airplayConf()->currentLog(i)) {
		  if((air_start_line[i]=rda->airplayConf()->logCurrentLine(i))>=
		     0) {
		    air_start_start[i]=rda->airplayConf()->autoRestart(i)&&
		      rda->airplayConf()->logRunning(i);
		  }
		  else {
		    air_start_line[i]=0;
		    air_start_start[i]=false;
		  }
		}
	      }
	    }
	    break;
      }
    }
    if(!air_start_logname[i].isEmpty()) {
      sql=QString().sprintf("select NAME from LOGS where NAME=\"%s\"",
			    (const char *)air_start_logname[i]);
      q=new RDSqlQuery(sql);
      if(q->first()) {
	rml.setCommand(RDMacro::LL);  // Load Log
	rml.setArgQuantity(2);
	rml.setArg(0,i+1);
	rml.setArg(1,air_start_logname[i]);
	rda->ripc()->sendRml(&rml);
      }
      else {
	fprintf(stderr,"rdairplay: log \"%s\" doesn't exist\n",
		(const char *)air_start_logname[i]);
      }
      delete q;
    }
  }
}


void MainWidget::rmlReceivedData(RDMacro *rml)
{
  RunLocalMacros(rml);
}


void MainWidget::gpiStateChangedData(int matrix,int line,bool state)
{
  //
  // Main Logs
  //
  for(unsigned i=0;i<2;i++) {
    if(state) {
      if((air_start_gpi_matrices[i]==matrix)&&
	 (air_start_gpi_lines[i]==line)) {
	if(AssertChannelLock(1,air_audio_channels[i])) {
	  air_log[0]->channelPlay(i);
	}
      }
    }
    else {
      if((air_stop_gpi_matrices[i]==matrix)&&
	 (air_stop_gpi_lines[i]==line)) {
	air_log[0]->channelStop(i);
      }
    }
  }

  //
  // Aux Logs
  //
  for(unsigned i=4;i<6;i++) {
    if(state) {
      if((air_start_gpi_matrices[i]==matrix)&&
	 (air_start_gpi_lines[i]==line)) {
	if(air_channel_timers[0][air_audio_channels[i]]->isActive()) {
	  air_channel_timers[0][air_audio_channels[i]]->stop();
	}
	else {
	  air_channel_timers[1][air_audio_channels[i]]->
	    start(AIR_CHANNEL_LOCKOUT_INTERVAL,true);
	  air_log[i-3]->channelPlay(0);
	}
      }
    }
    else {
      if((air_stop_gpi_matrices[i]==matrix)&&
	 (air_stop_gpi_lines[i]==line)) {
	air_log[i-3]->channelStop(0);
      }
    }
  }

  //
  // Sound Panel
  //
  if(!state) {
    if((air_stop_gpi_matrices[RDAirPlayConf::SoundPanel1Channel]==matrix)&&
       (air_stop_gpi_lines[RDAirPlayConf::SoundPanel1Channel]==line)) {
      air_panel->channelStop(0);
    }
    for(unsigned i=6;i<10;i++) {
      if((air_stop_gpi_matrices[i]==matrix)&&
	 (air_stop_gpi_lines[i]==line)) {
	air_panel->channelStop(i-5);
      }
    }
  }
}


void MainWidget::logChannelStartedData(int id,int mport,int card,int port)
{
  if(!AssertChannelLock(0,card,port)) {
    return;
  }
  switch(id) {
  case 0:  // Main Log
    switch(mport) {
    case 0:
      if(air_start_gpo_matrices[RDAirPlayConf::MainLog1Channel]>=0) {
	switch(air_channel_gpio_types[RDAirPlayConf::MainLog1Channel]) {
	case RDAirPlayConf::LevelGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::MainLog1Channel],
		      air_start_gpo_lines[RDAirPlayConf::MainLog1Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 1 300!",
		      air_start_gpo_matrices[RDAirPlayConf::MainLog1Channel],
		      air_start_gpo_lines[RDAirPlayConf::MainLog1Channel]+1));
	  break;
	}
      }
      break;

    case 1:
      if(air_start_gpo_matrices[RDAirPlayConf::MainLog2Channel]>=0) {
	switch(air_channel_gpio_types[RDAirPlayConf::MainLog2Channel]) {
	case RDAirPlayConf::LevelGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::MainLog2Channel],
		      air_start_gpo_lines[RDAirPlayConf::MainLog2Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 1 300!",
		      air_start_gpo_matrices[RDAirPlayConf::MainLog2Channel],
		      air_start_gpo_lines[RDAirPlayConf::MainLog2Channel]+1));
	  break;
	}
      }
      break;
    }
    break;

  case 1:  // Aux Log 1
    if(air_start_gpo_matrices[RDAirPlayConf::AuxLog1Channel]>=0) {
      switch(air_channel_gpio_types[RDAirPlayConf::AuxLog1Channel]) {
      case RDAirPlayConf::LevelGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::AuxLog1Channel],
		      air_start_gpo_lines[RDAirPlayConf::AuxLog1Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 1 300!",
		      air_start_gpo_matrices[RDAirPlayConf::AuxLog1Channel],
		      air_start_gpo_lines[RDAirPlayConf::AuxLog1Channel]+1));
	break;
      }
    }
    break;
    
  case 2:  // Aux Log 2
    if(air_start_gpo_matrices[RDAirPlayConf::AuxLog2Channel]>=0) {
      switch(air_channel_gpio_types[RDAirPlayConf::AuxLog2Channel]) {
      case RDAirPlayConf::LevelGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::AuxLog2Channel],
		      air_start_gpo_lines[RDAirPlayConf::AuxLog2Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 1 300!",
		      air_start_gpo_matrices[RDAirPlayConf::AuxLog2Channel],
		      air_start_gpo_lines[RDAirPlayConf::AuxLog2Channel]+1));
	break;
      }
    }
    break;
  }
}


void MainWidget::logChannelStoppedData(int id,int mport,int card,int port)
{
  switch(id) {
  case 0:  // Main Log
    switch(mport) {
    case 0:
      if(air_stop_gpo_matrices[RDAirPlayConf::MainLog1Channel]>=0) {
	switch(air_channel_gpio_types[RDAirPlayConf::MainLog1Channel]) {
	case RDAirPlayConf::LevelGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::MainLog1Channel],
		      air_stop_gpo_lines[RDAirPlayConf::MainLog1Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 1 300!",
		      air_stop_gpo_matrices[RDAirPlayConf::MainLog1Channel],
		      air_stop_gpo_lines[RDAirPlayConf::MainLog1Channel]+1));
	  break;
	}
      }
      break;

    case 1:
      if(air_stop_gpo_matrices[RDAirPlayConf::MainLog2Channel]>=0) {
	switch(air_channel_gpio_types[RDAirPlayConf::MainLog2Channel]) {
	case RDAirPlayConf::LevelGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::MainLog2Channel],
		      air_stop_gpo_lines[RDAirPlayConf::MainLog2Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  rdevent_player->
	    exec(QString().sprintf("GO %d %d 1 300!",
		      air_stop_gpo_matrices[RDAirPlayConf::MainLog2Channel],
		      air_stop_gpo_lines[RDAirPlayConf::MainLog2Channel]+1));
	  break;
	}
      }
      break;
    }
    break;

  case 1:  // Aux Log 1
    if(air_stop_gpo_matrices[RDAirPlayConf::AuxLog1Channel]>=0) {
      switch(air_channel_gpio_types[RDAirPlayConf::AuxLog1Channel]) {
      case RDAirPlayConf::LevelGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::AuxLog1Channel],
		      air_stop_gpo_lines[RDAirPlayConf::AuxLog1Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 1 300!",
		      air_stop_gpo_matrices[RDAirPlayConf::AuxLog1Channel],
		      air_stop_gpo_lines[RDAirPlayConf::AuxLog1Channel]+1));
	break;
      }
    }
    break;

  case 2:  // Aux Log 2
    if(air_stop_gpo_matrices[RDAirPlayConf::AuxLog2Channel]>=0) {
      switch(air_channel_gpio_types[RDAirPlayConf::AuxLog2Channel]) {
      case RDAirPlayConf::LevelGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::AuxLog2Channel],
		      air_stop_gpo_lines[RDAirPlayConf::AuxLog2Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	rdevent_player->
	  exec(QString().sprintf("GO %d %d 1 300!",
		      air_stop_gpo_matrices[RDAirPlayConf::AuxLog2Channel],
		      air_stop_gpo_lines[RDAirPlayConf::AuxLog2Channel]+1));
	break;
      }
    }
    break;
  }
}


void MainWidget::panelChannelStartedData(int mport,int card,int port)
{
  if(!AssertChannelLock(0,card,port)) {
    return;
  }
  RDAirPlayConf::Channel chan=PanelChannel(mport);
  if(air_start_gpo_matrices[chan]>=0) {
    switch(air_channel_gpio_types[chan]) {
    case RDAirPlayConf::LevelGpio:
      rdevent_player->
	exec(QString().sprintf("GO %d %d 1 0!",
			       air_start_gpo_matrices[chan],
			       air_start_gpo_lines[chan]+1));
      break;

    case RDAirPlayConf::EdgeGpio:
      rdevent_player->
	exec(QString().sprintf("GO %d %d 1 300!",
			       air_start_gpo_matrices[chan],
			       air_start_gpo_lines[chan]+1));
      break;
    }
  }
}


void MainWidget::panelChannelStoppedData(int mport,int card,int port)
{
  RDAirPlayConf::Channel chan=PanelChannel(mport);

  if(air_stop_gpo_matrices[chan]>=0) {
    switch(air_channel_gpio_types[chan]) {
    case RDAirPlayConf::LevelGpio:
      rdevent_player->
	exec(QString().sprintf("GO %d %d 0 0!",
			       air_stop_gpo_matrices[chan],
			       air_stop_gpo_lines[chan]+1));
      break;

    case RDAirPlayConf::EdgeGpio:
      rdevent_player->
	exec(QString().sprintf("GO %d %d 1 300!",
			       air_stop_gpo_matrices[chan],
			       air_stop_gpo_lines[chan]+1));
      break;
    }
  }
}


void MainWidget::logRenamedData(int log)
{
  QString str;
  QString logname=
    air_log[log]->logName().left(air_log[log]->logName().length()-4);
  QString labelname=logname;
  if(logname.isEmpty()) {
    labelname="--";
  }
  switch(log) {
      case 0:
	str=QString(tr("Main Log"));
	air_log_button[0]->
	  setText(QString().sprintf("%s\n[%s]",(const char *)str,
				    (const char *)labelname));
	SetCaption();
	if(air_panel) {
	  air_panel->setLogName(logname);
	}
	break;

      case 1:
	str=QString(tr("Aux 1 Log"));
	air_log_button[1]->
	  setText(QString().sprintf("%s\n[%s]",(const char *)str,
				    (const char *)labelname));
	break;
	
      case 2:
	str=QString(tr("Aux 2 Log"));
	air_log_button[2]->
	  setText(QString().sprintf("%s\n[%s]",(const char *)str,
				    (const char *)labelname));
	break;
  }
}


void MainWidget::logReloadedData(int log)
{
  QString str;
  QHostAddress addr;
  QString labelname=
    air_log[log]->logName().left(air_log[log]->logName().length()-4);
  if(labelname.isEmpty()) {
    labelname="--";
  }

  switch(log) {
      case 0:
	str=QString(tr("Main Log"));
	air_log_button[0]->
	  setText(QString().sprintf("%s\n[%s]",(const char *)str,
				    (const char *)labelname));
	LogLine(RDConfig::LogInfo,QString().
		sprintf("loaded log '%s' in Main Log",
			(const char *)air_log[0]->logName().
			left(air_log[0]->logName().length()-4)));
	if(air_log[0]->logName().isEmpty()) {
	  if(air_panel!=NULL) {
	    air_panel->setSvcName(rda->airplayConf()->defaultSvc());
	  }
	}
	else {
	  if(air_panel!=NULL) {
	    air_panel->setSvcName(air_log[0]->serviceName());
	  }
	}
	break;

      case 1:
	str=QString(tr("Aux 1 Log"));
	air_log_button[1]->
	  setText(QString().sprintf("%s\n[%s]",(const char *)str,
				    (const char *)labelname));
	LogLine(RDConfig::LogInfo,QString().
		sprintf("loaded log '%s' in Aux 1 Log",
			(const char *)air_log[1]->logName().
			left(air_log[1]->logName().length()-4)));
	break;
	
      case 2:
	str=QString(tr("Aux 2 Log"));
	air_log_button[2]->
	  setText(QString().sprintf("%s\n[%s]",(const char *)str,
				    (const char *)labelname));
	LogLine(RDConfig::LogInfo,QString().
		sprintf("loaded log '%s' in Aux 2 Log",
			(const char *)air_log[2]->logName().
			left(air_log[2]->logName().length()-4)));
	break;
  }
  SetCaption();

  //
  // Load Initial Log
  //
  if(air_start_logname[log].isEmpty()) {
    return;
  }
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  addr.setAddress("127.0.0.1");
  rml.setAddress(addr);
  rml.setEchoRequested(false);

  if(air_start_line[log]<air_log[log]->size()) {
    rml.setCommand(RDMacro::MN);  // Make Next
    rml.setArgQuantity(2);
    rml.setArg(0,log+1);
    rml.setArg(1,air_start_line[log]);
    rda->ripc()->sendRml(&rml);
    
    if(air_start_start[log]) {
      rml.setCommand(RDMacro::PN);  // Start Next
      rml.setArgQuantity(1);
      rml.setArg(0,log+1);
      rda->ripc()->sendRml(&rml);
    }
  }
  else {
    fprintf(stderr,"rdairplay: line %d doesn't exist in log \"%s\"\n",
	    air_start_line[log],(const char *)air_start_logname[log]);
  }
  air_start_logname[log]="";
}


void MainWidget::userData()
{
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("user changed to '%s'",(const char *)rda->ripc()->user()));
  SetCaption();

  //
  // Set Control Perms
  //
  bool add_allowed=rda->user()->addtoLog();
  bool delete_allowed=rda->user()->removefromLog();
  bool arrange_allowed=rda->user()->arrangeLog();
  bool playout_allowed=rda->user()->playoutLog();

  air_add_button->setEnabled(add_allowed&&arrange_allowed&&playout_allowed);
  air_move_button->setEnabled(arrange_allowed&&playout_allowed);
  air_delete_button->
    setEnabled(delete_allowed&&arrange_allowed&&playout_allowed);
  air_copy_button->setEnabled(add_allowed&&arrange_allowed&&playout_allowed);
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_list[i]->userChanged(add_allowed,delete_allowed,
				 arrange_allowed,playout_allowed);
  }
}


void MainWidget::addButtonData()
{
  if((air_action_mode==StartButton::AddFrom)||
     (air_action_mode==StartButton::AddTo)) {
    SetActionMode(StartButton::Stop);
  }
  else {
    SetActionMode(StartButton::AddFrom);
  }
}


void MainWidget::deleteButtonData()
{
  if(air_action_mode==StartButton::DeleteFrom) {
    SetActionMode(StartButton::Stop);
  }
  else {
    SetActionMode(StartButton::DeleteFrom);
  }
}


void MainWidget::moveButtonData()
{
  if((air_action_mode==StartButton::MoveFrom)||
    (air_action_mode==StartButton::MoveTo)) {
    SetActionMode(StartButton::Stop);
  }
  else {
    SetActionMode(StartButton::MoveFrom);
  }
}


void MainWidget::copyButtonData()
{
  if((air_action_mode==StartButton::CopyFrom)||
    (air_action_mode==StartButton::CopyTo)) {
    SetActionMode(StartButton::Stop);
  }
  else {
    SetActionMode(StartButton::CopyFrom);
  }
}


void MainWidget::fullLogButtonData(int id)
{
#ifdef SHOW_SLOTS
  printf("fullLogButtonData()\n");
#endif
  if(air_log_list[id]->isVisible()) {
    return;
  }
  else {
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      if(air_log_list[i]->isVisible()) {
	air_log_list[i]->hide();
	air_log_button[i]->setPalette(palette());
      }
    }
    air_log_list[id]->show();
    air_log_button[id]->setPalette(active_color);
    if (air_panel_button) air_panel_button->setPalette(palette());
  }
}


void MainWidget::panelButtonData()
{
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    if(air_log_list[i]->isVisible()) {
      air_log_list[i]->hide();
      air_log_button[i]->setPalette(palette());
    }
  }
  air_panel_button->setPalette(active_color);
}


void MainWidget::modeButtonData()
{
  int mach=-1;
  switch(air_op_mode_style) {
  case RDAirPlayConf::Unified:
    mach=-1;
    break;

  case RDAirPlayConf::Independent:
    mach=0;
    break;
  }
  switch(air_op_mode[0]) {
      case RDAirPlayConf::Manual:
	SetMode(mach,RDAirPlayConf::LiveAssist);
	break;

      case RDAirPlayConf::LiveAssist:
	SetMode(mach,RDAirPlayConf::Auto);
	break;

      case RDAirPlayConf::Auto:
	SetMode(mach,RDAirPlayConf::Manual);
	break;

      default:
	break;
  }
}


void MainWidget::selectClickedData(int id,int line,RDLogLine::Status status)
{
  RDLogLine *logline;

  switch(air_action_mode) {
      case StartButton::AddTo:
	if(line<0) {
	  air_log[id]->
	    insert(air_log[id]->size(),air_add_cart,RDLogLine::Play);
	  air_log[id]->logLine(air_log[id]->size()-1)->
	    setTransType(rda->airplayConf()->defaultTransType());
	  air_log_list[id]->refresh(air_log[id]->size()-1);
	}
	else {
	  air_log[id]->
	    insert(line,air_add_cart,air_log[id]->nextTransType(line));
	  air_log[id]->logLine(line)->
	    setTransType(rda->airplayConf()->defaultTransType());
	  air_log_list[id]->refresh(line);
	}
	SetActionMode(StartButton::Stop);
	break;

      case StartButton::DeleteFrom:
	if(status==RDLogLine::Finished) {
	  return;
	}
	air_log[id]->remove(line,1);
	SetActionMode(StartButton::Stop);
	break;

      case StartButton::MoveFrom:
	if((logline=air_log[id]->logLine(line))!=NULL) {
	  air_copy_line=line;
	  air_add_cart=logline->cartNumber();
	  air_source_id=id;
	  SetActionMode(StartButton::MoveTo);
	}
	else {
	  SetActionMode(StartButton::Stop);
	}
	break;

      case StartButton::MoveTo:
	if(air_source_id==id) {
	  if(line<0) {
	    air_log[id]->move(air_copy_line,air_log[id]->size());
	    air_log_list[id]->refresh(air_log[id]->size()-1);
	  }
	  else {
	    if(line>air_copy_line) {
	      line--;
	    }
	    air_log[id]->move(air_copy_line,line);
	    air_log_list[id]->refresh(line);
	  }
	}
	else {
	  air_log[air_source_id]->remove(air_copy_line,1);
	  if(line<0) {
	    air_log[id]->
	      insert(air_log[id]->size(),air_add_cart,RDLogLine::Play);
	    air_log_list[id]->refresh(air_log[id]->size()-1);
	    
	  }
	  else {
	    air_log[id]->
	      insert(line,air_add_cart,air_log[id]->nextTransType(line));
	    air_log_list[id]->refresh(line);
	  }
	}
	SetActionMode(StartButton::Stop);
	break;

      case StartButton::CopyFrom:
	if((logline=air_log[id]->logLine(line))!=NULL) {
	  air_copy_line=line;
	  air_add_cart=logline->cartNumber();
	  air_source_id=id;
	  SetActionMode(StartButton::CopyTo);
	}
	else {
	  SetActionMode(StartButton::Stop);
	}
	break;

      case StartButton::CopyTo:
	if(air_source_id==id) {
	  if(line<0) {
	    air_log[id]->copy(air_copy_line,air_log[id]->size());
	  }
	  else {
	    air_log[id]->copy(air_copy_line,line);
	  }
	}
	else {
	  if(line<0) {
	    air_log[id]->insert(air_log[id]->size(),air_add_cart,
				rda->airplayConf()->defaultTransType(),
				rda->airplayConf()->defaultTransType());
	    air_log[id]->logLine(air_log[id]->size()-1)->
	      setTransType(rda->airplayConf()->defaultTransType());
	    air_log_list[id]->refresh(air_log[id]->size()-1);
	  }
	  else {
	    air_log[id]->
	      insert(line,air_add_cart,air_log[id]->nextTransType(line),
		     rda->airplayConf()->defaultTransType());
	    air_log[id]->logLine(line)->
	      setTransType(rda->airplayConf()->defaultTransType());
	    air_log_list[id]->refresh(line);
	  }
	}
	SetActionMode(StartButton::Stop);
	break;

      default:
	break;
  }
}


void MainWidget::selectClickedData(unsigned cartnum,int row,int col)
{
  switch(air_action_mode) {
      case StartButton::CopyFrom:
	air_copy_line=-1;
	air_add_cart=cartnum;
	air_source_id=-1;
	SetActionMode(StartButton::CopyTo);
	break;

      case StartButton::CopyTo:
	if(air_panel!=NULL) {
	  air_panel->setButton(air_panel->currentType(),
			       air_panel->currentNumber(),row,col,air_add_cart);
	}
	SetActionMode(StartButton::Stop);
	break;

      case StartButton::AddTo:
	if(air_panel!=NULL) {
	  air_panel->setButton(air_panel->currentType(),
			       air_panel->currentNumber(),row,col,air_add_cart);
	}
	SetActionMode(StartButton::Stop);
	break;

      case StartButton::DeleteFrom:
	if(air_panel!=NULL) {
	  air_panel->setButton(air_panel->currentType(),
			       air_panel->currentNumber(),row,col,0);
	}
	SetActionMode(StartButton::Stop);
	break;

      default:
	break;
  }
}


void MainWidget::cartDroppedData(int id,int line,RDLogLine *ll)
{
  if(ll->cartNumber()==0) {
    air_log[id]->remove(line,1);
  }
  else {
    if(line<0) {
      air_log[id]->
	insert(air_log[id]->size(),ll->cartNumber(),RDLogLine::Play);
      air_log[id]->logLine(air_log[id]->size()-1)->
	setTransType(rda->airplayConf()->defaultTransType());
      air_log_list[id]->refresh(air_log[id]->size()-1);
    }
    else {
      air_log[id]->
	insert(line,ll->cartNumber(),air_log[id]->nextTransType(line));
      air_log[id]->logLine(line)->
	setTransType(rda->airplayConf()->defaultTransType());
      air_log_list[id]->refresh(line);
    }
  }
}


void MainWidget::meterData()
{
#ifdef SHOW_METER_SLOTS
  printf("meterData()\n");
#endif
  double ratio[2]={0.0,0.0};
  short level[2];

  for(int i=0;i<AIR_TOTAL_PORTS;i++) {
    if(FirstPort(i)) {
      rda->cae()->outputMeterUpdate(air_meter_card[i],air_meter_port[i],level);
      for(int j=0;j<2;j++) {
	ratio[j]+=pow(10.0,((double)level[j])/1000.0);
      }
    }
  }
  air_stereo_meter->setLeftPeakBar((int)(log10(ratio[0])*1000.0));
  air_stereo_meter->setRightPeakBar((int)(log10(ratio[1])*1000.0));
}


void MainWidget::masterTimerData()
{
  static unsigned counter=0;
  static QTime last_time=QTime::currentTime();

  if(counter++>=5) {
    QTime current_time=QTime::currentTime();
    if(current_time<last_time) {
      for(unsigned i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	air_log[i]->resync();
      }
    }
    last_time=current_time;
    counter=0;
  }
}


void MainWidget::transportChangedData()
{
  int lines[TRANSPORT_QUANTITY];
  int line=0;
  int count;
  QTime end_time;
  RDLogLine *logline;
  RDLogLine *next_logline;
  RDAirPlayConf::PieEndPoint pie_end=RDAirPlayConf::CartEnd;

  if((count=air_log[0]->runningEvents(lines,false))>0) {
    for(int i=0;i<count;i++) {
      if((logline=air_log[0]->logLine(lines[i]))!=NULL) {
	switch(logline->type()) {
	    case RDLogLine::Cart:
	      if(logline->startTime(RDLogLine::Actual).
		 addMSecs(logline->effectiveLength()-
			  ((RDPlayDeck *)logline->playDeck())->
			  lastStartPosition())>end_time) {
		end_time=logline->startTime(RDLogLine::Actual).
		  addMSecs(logline->effectiveLength()-
			   ((RDPlayDeck *)logline->playDeck())->
			   lastStartPosition());
		line=lines[i];
	      }
	      break;

	    case RDLogLine::Macro:
	      line=lines[i];
	      break;

	    default:
	      break;
	}
      }
    }

    logline=air_log[0]->logLine(line);
    switch(air_op_mode[0]) {
	case RDAirPlayConf::Manual:
	case RDAirPlayConf::LiveAssist:
	  pie_end=RDAirPlayConf::CartEnd;
	  break;

	case RDAirPlayConf::Auto:
	  pie_end=air_pie_end;
	  break;

	default:
	  break;
    }
    if(logline->effectiveLength()>0) {
      if((air_pie_counter->line()!=logline->id())) {
	switch(pie_end) {
	    case RDAirPlayConf::CartEnd:
	      air_pie_counter->setTime(logline->effectiveLength());
	      break;
	      
	    case RDAirPlayConf::CartTransition:
	      if((next_logline=air_log[0]->
		  logLine(air_log[0]->nextLine(line)))!=NULL) {
		if((unsigned)logline->startTime(RDLogLine::Actual).
		   msecsTo(QTime::currentTime())<
		   logline->segueLength(next_logline->transType())-
		   logline->playPosition()) {
		  air_pie_counter->
		    setTime(logline->segueLength(next_logline->transType()));
		}
		else {
		  air_pie_counter->setTime(logline->effectiveLength());
		}
	      }
	      else {
		air_pie_counter->setTime(logline->effectiveLength());
	      }
	      break;
	}
	if(logline->talkStartPoint()==0) {
          air_pie_counter->setTalkStart(0);
  	  air_pie_counter->setTalkEnd(logline->talkEndPoint());
        }
        else {
	air_pie_counter->
	  setTalkStart(logline->talkStartPoint()-logline->
		         startPoint());
	air_pie_counter->
	  setTalkEnd(logline->talkEndPoint()-logline->
		         startPoint());
        }
	air_pie_counter->setTransType(air_log[0]->nextTrans(line));
	if(logline->playDeck()==NULL) {
	  air_pie_counter->setLogline(NULL);
	  air_pie_counter->start(rda->station()->timeOffset());
	}
	else {
	  air_pie_counter->setLogline(logline);
	  air_pie_counter->start(((RDPlayDeck *)logline->playDeck())->
				 currentPosition()+
				 rda->station()->timeOffset());
	}
      }
    }
    else {
      air_pie_counter->stop();
      air_pie_counter->resetTime();
      air_pie_counter->setLine(-1);
    }
  }
  else {
    air_pie_counter->stop();
    air_pie_counter->resetTime();
    air_pie_counter->setLine(-1);
  }
}


void MainWidget::timeModeData(RDAirPlayConf::TimeMode mode)
{
  air_button_list->setTimeMode(mode);
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_list[i]->setTimeMode(mode);
  }
  air_stop_counter->setTimeMode(mode);
  air_post_counter->setTimeMode(mode);
}


void MainWidget::refreshStatusChangedData(bool active)
{
  if(active) {
    air_refresh_label->setErasePixmap(QPixmap());
    air_refresh_label->setText(tr("LOG\nREFRESHING"));
  }
  else {
    if(air_refresh_pixmap!=NULL) {
      air_refresh_label->setErasePixmap(*air_refresh_pixmap);
    }
    air_refresh_label->setText("");
  }
  //
  // FIXME: Disabled in 2.10.3caefix05 due to segfault problems.
  //        Do we really need this?
  //
  //  qApp->processEvents();
}


void MainWidget::clearSplashData()
{
  air_splash_screen->hide();
  delete air_splash_screen;
  air_splash_screen=NULL;
}


void MainWidget::keyPressEvent(QKeyEvent *e)
{
 switch(e->key()) {
      case Qt::Key_Space:
	break;

      case Qt::Key_X:
	if(((e->state()&AltButton)!=0)&&
	   ((e->state()&ShiftButton)==0)&&((e->state()&ControlButton)==0)) {
	  QCloseEvent *ce=new QCloseEvent();
	  closeEvent(ce);
	  delete ce;
	}
	break;

      case Qt::Key_Alt:
        keystrokecount++;
        AltKeyHit = true;
        break;

      case Qt::Key_Control:
        keystrokecount++;
        CtrlKeyHit = true;
        break;

      default:
        QWidget::keyPressEvent(e);
	break;
  }
}

void MainWidget::keyReleaseEvent(QKeyEvent *e)
{
  int keyhit = e->key();
  QString mystring=(*rdkeylist).GetKeyCode(keyhit);
  QString hotkeystrokes;
  QString hot_label;
  QString temp_string;

  switch(e->key()) {
      case Qt::Key_Space:
	switch(air_bar_action) {
	    case RDAirPlayConf::StartNext:
	      if(!e->isAutoRepeat()){
		air_log[0]->
		  play(air_log[0]->nextLine(),RDLogLine::StartManual);
	      }
	      break;

	    default:
	      break;
	}
	break;
  }
//  Try to figure out if this is a hot key combination
  if ( (e->key() == Qt::Key_Shift) || 
       (e->key() == Qt::Key_Up) || 
       (e->key() == Qt::Key_Left) ||
       (e->key() == Qt::Key_Right) ||
       (e->key() == Qt::Key_Down) )    {
      QWidget::keyReleaseEvent(e);
      keystrokecount = 0;
      hotkeystrokes = QString ("");
      return;
  }

  if ((e->key() == Qt::Key_Alt) ||
      (e->key() == Qt::Key_Control)) {
      if (keystrokecount != 0 ) hotkeystrokes = QString ("");
      if (AltKeyHit) {
          AltKeyHit = false;
          if (keystrokecount > 0) keystrokecount--;
      }
      if (CtrlKeyHit) {
          CtrlKeyHit = false;
          if (keystrokecount > 0) keystrokecount--;
      }
      return;
  }

  if (!e->isAutoRepeat()) {
      if (keystrokecount == 0)
          hotkeystrokes = QString ("");
      if (AltKeyHit) {
          hotkeystrokes =  (*rdkeylist).GetKeyCode(Qt::Key_Alt);
          hotkeystrokes +=  QString(" + ");
      }
      if (CtrlKeyHit) {
          if (AltKeyHit) {
              hotkeystrokes +=  (*rdkeylist).GetKeyCode(Qt::Key_Control);
              hotkeystrokes += QString (" + ");
          }
          else {
              hotkeystrokes =  (*rdkeylist).GetKeyCode(Qt::Key_Control);
              hotkeystrokes += QString (" + ");
          }
      }

      hotkeystrokes += mystring; 
      keystrokecount = 0 ;
  }
  
      // Have any Hot Key Combinations now...

  if (hotkeystrokes.length() > 0)  {

    hot_label=(*rdhotkeys).GetRowLabel(RDEscapeString(rda->config()->stationName()),
                        (const char *)"airplay",(const char *)hotkeystrokes);

    if (hot_label.length()>0) {

        // "we found a keystroke label
 
        if (strcmp(hot_label,"Add") == 0)
        {
            addButtonData();
            return;
        }

        if (strcmp(hot_label,"Delete") == 0)
        {
            deleteButtonData();
            return;
        }

        if (strcmp(hot_label,"Copy") == 0)
        {
            copyButtonData();
            return;
        }

        if (strcmp(hot_label,"Move") == 0)
        {
            moveButtonData();
            return;
        }
    
        if (strcmp(hot_label,"Sound Panel") == 0)
        {
            panelButtonData();
            return;
        }
    
        if (strcmp(hot_label,"Main Log") == 0)
        {
            fullLogButtonData(0);
            return;
        }
    
        if ((strcmp(hot_label,"Aux Log 1") == 0) &&
             (rda->airplayConf()->showAuxButton(0) ) )
        {
            fullLogButtonData(1);
            return;
        }
    
        if ( (strcmp(hot_label,"Aux Log 2") == 0) &&
             (rda->airplayConf()->showAuxButton(1) ) )
        {
            fullLogButtonData(2);
            return;
        }
    
        for (int i = 1; i < 8 ; i++)
        {
            temp_string = QString().sprintf("Start Line %d",i);
            if (strcmp(hot_label,temp_string) == 0)
                air_button_list->startButton(i-1);
    
            temp_string = QString().sprintf("Stop Line %d",i);
            if (strcmp(hot_label,temp_string) == 0)
                air_button_list->stopButtonHotkey(i-1);
    
            temp_string = QString().sprintf("Pause Line %d",i);
            if (strcmp(hot_label,temp_string) == 0)
                air_button_list->pauseButtonHotkey(i-1);
        }
      }
  }
  QWidget::keyReleaseEvent(e);
}

void MainWidget::closeEvent(QCloseEvent *e)
{
  if(!rda->airplayConf()->exitPasswordValid("")) {
    QString passwd;
    RDGetPasswd *gw=new RDGetPasswd(&passwd,this);
    gw->exec();
    if(!rda->airplayConf()->exitPasswordValid(passwd)) {
      return;
    }
    rda->airplayConf()->setExitCode(RDAirPlayConf::ExitClean);
    LogLine(RDConfig::LogInfo,"RDAirPlay exiting");
    air_lock->unlock();
    exit(0);
  }
  if(QMessageBox::question(this,"",tr("Exit RDAirPlay?"),
			   QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  for(unsigned i=0;i<air_plugin_hosts.size();i++) {
    air_plugin_hosts[i]->unload();
  }
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    delete air_log[i];
  }
  rda->airplayConf()->setExitCode(RDAirPlayConf::ExitClean);
  LogLine(RDConfig::LogInfo,"RDAirPlay exiting");
  air_lock->unlock();
  exit(0);
}


void MainWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(black);
  p->fillRect(10,70,410,air_stereo_meter->sizeHint().height(),black);
  p->end();
  delete p;
}


void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
  case SIGCHLD:
    pLocalPid=waitpid(-1,NULL,WNOHANG);
    while(pLocalPid>0) {
      pLocalPid=waitpid(-1,NULL,WNOHANG);
    }
    signal(SIGCHLD,SigHandler);
    return;
  }
}


void MainWidget::SetCaption()
{
  QString log=air_log[0]->logName();
  if(log.isEmpty()) {
    log="--    ";
  }
  setCaption(QString("RDAirPlay")+" v"+VERSION+" - "+tr("Host")+": "+
	     rda->config()->stationName()+" "+
	     tr("User:")+" "+rda->ripc()->user()+" "+
	     tr("Log:")+" "+log.left(log.length()-4)+" "+
	     tr("Service:")+" "+air_log[0]->serviceName());
}


void MainWidget::SetMode(int mach,RDAirPlayConf::OpMode mode)
{
  if(mach<0) {
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      SetMode(i,mode);
    }
    return;
  }
  if(air_op_mode[mach]==mode) {
    return;
  }
  switch(mode) {
      case RDAirPlayConf::Manual:
	SetManualMode(mach);
	break;

      case RDAirPlayConf::LiveAssist:
	SetLiveAssistMode(mach);
	break;

      case RDAirPlayConf::Auto:
	SetAutoMode(mach);
	break;

      default:
	break;
  }
}


void MainWidget::SetManualMode(int mach)
{
  if(mach<0) {
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      SetManualMode(i);
    }
    return;
  }
  if(mach==0) {
    air_pie_counter->setOpMode(RDAirPlayConf::Manual);
  }
  air_mode_display->setOpMode(mach,RDAirPlayConf::Manual);
  air_op_mode[mach]=RDAirPlayConf::Manual;
  rda->airplayConf()->setOpMode(mach,RDAirPlayConf::Manual);
  air_log[mach]->setOpMode(RDAirPlayConf::Manual);
  air_log_list[mach]->setOpMode(RDAirPlayConf::Manual);
  if(mach==0) {
    air_button_list->setOpMode(RDAirPlayConf::Manual);
    air_post_counter->setDisabled(true);
  }
  LogLine(RDConfig::LogInfo,
	  QString().sprintf("log machine %d mode set to MANUAL",mach+1));
}


void MainWidget::SetAutoMode(int mach)
{
  if(mach<0) {
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      SetAutoMode(i);
    }
    return;
  }
  if(mach==0) {
    air_pie_counter->setOpMode(RDAirPlayConf::Auto);
  }
  air_mode_display->setOpMode(mach,RDAirPlayConf::Auto);
  air_op_mode[mach]=RDAirPlayConf::Auto;
  rda->airplayConf()->setOpMode(mach,RDAirPlayConf::Auto);
  air_log[mach]->setOpMode(RDAirPlayConf::Auto);
  air_log_list[mach]->setOpMode(RDAirPlayConf::Auto);
  if(mach==0) {
    air_button_list->setOpMode(RDAirPlayConf::Auto);
    air_post_counter->setEnabled(true);
  }
  LogLine(RDConfig::LogInfo,
	  QString().sprintf("log machine %d mode set to AUTO",mach+1));
}


void MainWidget::SetLiveAssistMode(int mach)
{
  if(mach<0) {
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      SetLiveAssistMode(i);
    }
    return;
  }
  if(mach==0) {
    air_pie_counter->setOpMode(RDAirPlayConf::LiveAssist);
  }
  air_mode_display->setOpMode(mach,RDAirPlayConf::LiveAssist);
  air_op_mode[mach]=RDAirPlayConf::LiveAssist;
  rda->airplayConf()->setOpMode(mach,RDAirPlayConf::LiveAssist);
  air_log[mach]->setOpMode(RDAirPlayConf::LiveAssist);
  air_log_list[mach]->setOpMode(RDAirPlayConf::LiveAssist);
  if(mach==0) {
    air_button_list->setOpMode(RDAirPlayConf::LiveAssist); 
    air_post_counter->setDisabled(true);
  }
  LogLine(RDConfig::LogInfo,
	  QString().sprintf("log machine %d mode set to LIVE ASSIST",mach+1));
}


void MainWidget::SetActionMode(StartButton::Mode mode)
{
  QString svc_name[RD_MAX_DEFAULT_SERVICES];
  int svc_quan=0;
  QString sql;
  RDSqlQuery *q;
  QStringList services_list;

  if(air_action_mode==mode) {
    return;
  }
  air_action_mode=mode;
  switch(mode) {
      case StartButton::Stop:
	air_add_button->setFlashingEnabled(false);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(false);
	air_copy_button->setFlashingEnabled(false);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::Normal);
	}
	air_button_list->setActionMode(RDAirPlayConf::Normal);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::Normal);
	}

	break;

      case StartButton::AddFrom:
	if(air_clear_filter) {
	  air_add_filter="";
	}
	air_add_cart=0;
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  svc_name[i]=air_log[i]->serviceName();
	  if(!svc_name[i].isEmpty()) {
	    svc_quan=RDAIRPLAY_LOG_QUANTITY;
	  }
	}
	if(svc_quan==0) {
	  sql=QString().sprintf("select SERVICE_NAME from SERVICE_PERMS \
                                   where STATION_NAME=\"%s\"",
				(const char *)rda->station()->name());
	  q=new RDSqlQuery(sql);
	  while(q->next()) {
	    services_list.append( q->value(0).toString() );
	  }
	  delete q;
          for ( QStringList::Iterator it = services_list.begin(); 
                it != services_list.end()&&svc_quan<(RD_MAX_DEFAULT_SERVICES-1);
                ++it ) {
            svc_name[svc_quan++]=*it;
          }

	}
	air_add_button->setFlashColor(BUTTON_FROM_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(true);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(false);
	air_copy_button->setFlashingEnabled(false);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::Normal);
	}
	air_button_list->setActionMode(RDAirPlayConf::Normal);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::Normal);
	}
	if(rdcart_dialog->exec(&air_add_cart,RDCart::All,0,0,
			       rda->user()->name(),rda->user()->password())==0) {
	  SetActionMode(StartButton::AddTo);
	}
	else {
	  SetActionMode(StartButton::Stop);
	}
	break;
	
      case StartButton::AddTo:
	air_add_button->setFlashColor(BUTTON_TO_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(true);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(false);
	air_copy_button->setFlashingEnabled(false);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::AddTo,&air_add_cart);
	}
	air_button_list->setActionMode(RDAirPlayConf::AddTo);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::AddTo);
	}
	break;

      case StartButton::DeleteFrom:
	air_delete_button->setFlashColor(BUTTON_FROM_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(false);
	air_delete_button->setFlashingEnabled(true);
	air_move_button->setFlashingEnabled(false);
	air_copy_button->setFlashingEnabled(false);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::DeleteFrom);
	}
	air_button_list->setActionMode(RDAirPlayConf::DeleteFrom);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::DeleteFrom);
	}
	break;

      case StartButton::MoveFrom:
	air_move_button->setFlashColor(BUTTON_FROM_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(false);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(true);
	air_copy_button->setFlashingEnabled(false);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::MoveFrom);
	}
	air_button_list->setActionMode(RDAirPlayConf::MoveFrom);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::MoveFrom);
	}
	break;

      case StartButton::MoveTo:
	air_move_button->setFlashColor(BUTTON_TO_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(false);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(true);
	air_copy_button->setFlashingEnabled(false);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::MoveTo);
	}
	air_button_list->setActionMode(RDAirPlayConf::MoveTo);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::MoveTo);
	}
	break;

      case StartButton::CopyFrom:
	air_copy_button->setFlashColor(BUTTON_FROM_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(false);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(false);
	air_copy_button->setFlashingEnabled(true);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::CopyFrom);
	}
	air_button_list->setActionMode(RDAirPlayConf::CopyFrom);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::CopyFrom);
	}
	break;

      case StartButton::CopyTo:
	air_move_button->setFlashColor(BUTTON_TO_BACKGROUND_COLOR);
	air_add_button->setFlashingEnabled(false);
	air_delete_button->setFlashingEnabled(false);
	air_move_button->setFlashingEnabled(false);
	air_copy_button->setFlashingEnabled(true);
	for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
	  air_log_list[i]->setActionMode(RDAirPlayConf::CopyTo);
	}
	air_button_list->setActionMode(RDAirPlayConf::CopyTo);
	if(air_panel!=NULL) {
	  air_panel->setActionMode(RDAirPlayConf::CopyTo);
	}
	break;

      default:
	break;
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
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
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdairplay_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}


QString logfile;


void LogLine(RDConfig::LogPriority prio,const QString &line)
{
  FILE *file;

  rda->config()->log("rdairplay",prio,line);

  if(logfile.isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();
  if((file=fopen(logfile,"a"))==NULL) {
    return;
  }
  chmod(logfile,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(file,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line);
  fclose(file);
}


bool MainWidget::FirstPort(int index)
{
  for(int i=0;i<index;i++) {
    if((air_meter_card[index]==air_meter_card[i])&&
       (air_meter_port[index]==air_meter_port[i])) {
      return false;
    }
  }
  return true;
}


bool MainWidget::AssertChannelLock(int dir,int card,int port)
{
  return AssertChannelLock(dir,AudioChannel(card,port));
}


bool MainWidget::AssertChannelLock(int dir,int achan)
{
  if(achan>=0) {
    int odir=!dir;
    if(air_channel_timers[odir][achan]->isActive()) {
      air_channel_timers[odir][achan]->stop();
      return false;
    }
    air_channel_timers[dir][achan]->start(AIR_CHANNEL_LOCKOUT_INTERVAL,true);
    return true;
  }
  return false;
}


int MainWidget::AudioChannel(int card,int port) const
{
  return RD_MAX_PORTS*card+port;
}


RDAirPlayConf::Channel MainWidget::PanelChannel(int mport) const
{
  RDAirPlayConf::Channel chan=RDAirPlayConf::SoundPanel1Channel;
  switch(mport) {
  case 0:
    chan=RDAirPlayConf::SoundPanel1Channel;
    break;

  case 1:
    chan=RDAirPlayConf::SoundPanel2Channel;
    break;

  case 2:
    chan=RDAirPlayConf::SoundPanel3Channel;
    break;

  case 3:
    chan=RDAirPlayConf::SoundPanel4Channel;
    break;

  case 4:
    chan=RDAirPlayConf::SoundPanel5Channel;
    break;
  }
  return chan;
}
