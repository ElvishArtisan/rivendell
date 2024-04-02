// rdairplay.cpp
//
// The On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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
#include <sys/mman.h>
#include <sys/wait.h>

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>

#include <rdconf.h>
#include <rdgetpasswd.h>
#include <rddatedecode.h>
#include <rdescape_string.h>

#include "rdairplay.h"

//
// Graphics
//
#include "../icons/bug-138x60.xpm"

//
// Prototypes
//
void SigHandler(int signo);

MainWidget::MainWidget(RDConfig *config,QWidget *parent)
  : RDMainWindow("rdairplay",config)
{
  QString str;
  int cards[3];
  int ports[3];
  QString start_rmls[3];
  QString stop_rmls[3];
  QPixmap bgmap;
  QString err_msg;
  bool dump_panel_updates=false;

  air_panel=NULL;
  air_tracker=NULL;

  //
  // Splash Screen
  //
  air_splash_screen=new SplashScreen(this);
  air_splash_screen->show();
  air_splash_screen->showMessage(tr("Opening database..."));

  //
  // Get the Startup Date/Time
  //
  air_startup_datetime=QDateTime(QDate::currentDate(),QTime::currentTime());

  //
  // Open the Database
  //
  rda=new RDApplication("RDAirPlay","rdairplay",RDAIRPLAY_USAGE,true,this);
  if(!rda->open(&err_msg,NULL,true,true)) {
    QMessageBox::critical(this,"RDAirPlay - "+tr("Error"),err_msg);
    exit(1);
  }

  air_splash_screen->showMessage(tr("Reading command line options..."));

  //
  // Read Command Options
  //
  QString lineno;
  for(unsigned i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_start_line[i]=0;
    air_start_start[i]=false;
    for(unsigned j=0;j<rda->cmdSwitch()->keys();j++) {
      if(rda->cmdSwitch()->key(j)=="--dump-panel-updates") {
	dump_panel_updates=true;
	rda->cmdSwitch()->setProcessed(j,true);
      }
      if(rda->cmdSwitch()->key(j)==QString::asprintf("--log%u",i+1)) {
	air_start_logname[i]=rda->cmdSwitch()->value(j);
	for(int k=0;k<rda->cmdSwitch()->value(j).length();k++) {
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
  setMinimumSize(sizeHint());

  //
  // Initialize the Random Number Generator
  //
  srandom(QTime::currentTime().msec());

  //
  // Create And Set Icon
  //
  setWindowIcon(rda->iconEngine()->applicationIcon(RDIconEngine::RdAirPlay,22));

  air_start_next=false;
  air_next_button=0;
  air_action_mode=StartButton::Play;

  str=QString("RDAirPlay")+" v"+VERSION+" - "+tr("Host:");
  setWindowTitle(str+" "+rda->config()->stationName());

  //
  // Master Clock Timer
  //
  air_master_timer=new QTimer(this);
  connect(air_master_timer,SIGNAL(timeout()),this,SLOT(masterTimerData()));
  air_master_timer->start(MASTER_TIMER_INTERVAL);

  air_splash_screen->showMessage(tr("Initializing global resources..."));

  //
  // Allocate Global Resources
  //
  rdairplay_previous_exit_code=rda->airplayConf()->exitCode();
  rda->airplayConf()->setExitCode(RDAirPlayConf::ExitDirty);
  air_default_trans_type=rda->airplayConf()->defaultTransType();
  air_clear_filter=rda->airplayConf()->clearFilter();
  air_bar_action=rda->airplayConf()->barAction();
  air_op_mode_style=rda->airplayConf()->opModeStyle();
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_op_mode[i]=RDAirPlayConf::Previous;
  }
  setBackgroundPixmap(QPixmap(rda->airplayConf()->skinPath()));

  //
  // Top Strip
  //
  air_top_strip=new TopStrip(this);
  connect(air_master_timer,SIGNAL(timeout()),
	  air_top_strip->wallClockWidget(),SLOT(tickClock()));
  connect(air_top_strip->modeDisplayWidget(),SIGNAL(clicked()),
	  this,SLOT(modeButtonData()));
  connect(rda->ripc(),SIGNAL(onairFlagChanged(bool)),
	  air_top_strip,SLOT(setOnairFlag(bool)));

  LoadMeters();

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
      AudioChannel(rda->airplayConf()->card(chan),
		   rda->airplayConf()->port(chan));

    if((rda->airplayConf()->card(chan)>=0)&&
       (rda->airplayConf()->port(chan)>=0)) {
      int achan=
	AudioChannel(rda->airplayConf()->card(chan),
		     rda->airplayConf()->port(chan));
      if(air_channel_timers[0][achan]==NULL) {
	air_channel_timers[0][achan]=new QTimer(this);
	air_channel_timers[0][achan]->setSingleShot(true);
	air_channel_timers[1][achan]=new QTimer(this);
	air_channel_timers[1][achan]->setSingleShot(true);
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
  connect(rda->cae(),SIGNAL(isConnected(bool)),
	  this,SLOT(caeConnectedData(bool)));
  if(!rda->cae()->connectHost(&err_msg)) {
    QMessageBox::warning(this,"RDAirPlay - "+tr("Error"),err_msg);
    exit(RDCoreApplication::ExitInternalError);
  }

  //
  // Set Audio Assignments
  //
  air_segue_length=rda->airplayConf()->segueLength()+1;

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));
  connect(rda->ripc(),SIGNAL(gpiStateChanged(int,int,bool)),
	  this,SLOT(gpiStateChangedData(int,int,bool)));

  //
  // Macro Player
  //
  air_event_player=new RDEventPlayer(rda->ripc(),this);

  air_splash_screen->showMessage(tr("Initializing widgets..."));

  //
  // Log Machines
  //
  QSignalMapper *reload_mapper=new QSignalMapper(this);
  connect(reload_mapper,SIGNAL(mapped(int)),this,SLOT(logReloadedData(int)));
  QSignalMapper *rename_mapper=new QSignalMapper(this);
  connect(rename_mapper,SIGNAL(mapped(int)),this,SLOT(logRenamedData(int)));
  QString default_svcname=rda->airplayConf()->defaultSvc();
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log[i]=new RDLogPlay(i,air_event_player,true,this);
    air_log[i]->setDefaultServiceName(default_svcname);
    air_log[i]->setNowCart(rda->airplayConf()->logNowCart(i));
    air_log[i]->setNextCart(rda->airplayConf()->logNextCart(i));
    reload_mapper->setMapping(air_log[i],i);
    connect(air_log[i],SIGNAL(reloaded()),reload_mapper,SLOT(map()));
    rename_mapper->setMapping(air_log[i],i);
    connect(air_log[i],SIGNAL(renamed()),rename_mapper,SLOT(map()));
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
  QString labels[3];
  for(int i=0;i<2;i++) {
    cards[i]=rda->airplayConf()->card((RDAirPlayConf::Channel)i);
    ports[i]=rda->airplayConf()->port((RDAirPlayConf::Channel)i);
    start_rmls[i]=rda->airplayConf()->startRml((RDAirPlayConf::Channel)i);
    stop_rmls[i]=rda->airplayConf()->stopRml((RDAirPlayConf::Channel)i);
    if((cards[i]>=0)&&(ports[i]>=0)) {
      labels[i]=rda->portNames()->portName(cards[i],ports[i]);
    }
  }
  if((cards[1]<0)||(ports[1]<0)) {  // Fixup disabled main log port 2 playout
    cards[1]=cards[0];
    ports[1]=ports[0];
    labels[1]=labels[0];
  }
  air_log[0]->setChannels(cards,ports,labels,start_rmls,stop_rmls);

  for(int i=0;i<2;i++) {
    cards[i]=rda->airplayConf()->card(RDAirPlayConf::AuxLog1Channel);
    ports[i]=rda->airplayConf()->port(RDAirPlayConf::AuxLog1Channel);
    labels[i]=rda->portNames()->portName(cards[i],ports[i]);
    start_rmls[i]=rda->airplayConf()->startRml(RDAirPlayConf::AuxLog1Channel);
    stop_rmls[i]=rda->airplayConf()->stopRml(RDAirPlayConf::AuxLog1Channel);
  }
  air_log[1]->setChannels(cards,ports,labels,start_rmls,stop_rmls);

  for(int i=0;i<2;i++) {
    cards[i]=rda->airplayConf()->card(RDAirPlayConf::AuxLog2Channel);
    ports[i]=rda->airplayConf()->port(RDAirPlayConf::AuxLog2Channel);
    labels[i]=rda->portNames()->portName(cards[i],ports[i]);
    start_rmls[i]=rda->airplayConf()->startRml(RDAirPlayConf::AuxLog2Channel);
    stop_rmls[i]=rda->airplayConf()->stopRml(RDAirPlayConf::AuxLog2Channel);
  }
  air_log[2]->setChannels(cards,ports,labels,start_rmls,stop_rmls);

  //
  // Cart Picker
  //
  air_cart_dialog=new RDCartDialog(&air_add_filter,&air_add_group,
				   &air_add_schedcode,"RDAirPlay",false,this);

  //
  // Create Palettes
  //
  auto_color=QPalette(QColor(BUTTON_MODE_AUTO_COLOR),
		      palette().color(QPalette::Background));
  manual_color=QPalette(QColor(BUTTON_MODE_MANUAL_COLOR),
			palette().color(QPalette::Background));
  active_color=palette();
  active_color.setColor(QPalette::Active,QPalette::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  active_color.setColor(QPalette::Active,QPalette::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  active_color.setColor(QPalette::Active,QPalette::Background,
			palette().color(QPalette::Background));
  active_color.setColor(QPalette::Inactive,QPalette::ButtonText,
			BUTTON_LOG_ACTIVE_TEXT_COLOR);
  active_color.setColor(QPalette::Inactive,QPalette::Button,
			BUTTON_LOG_ACTIVE_BACKGROUND_COLOR);
  active_color.setColor(QPalette::Inactive,QPalette::Background,
			palette().color(QPalette::Background));

  //
  // Add Button
  //
  air_add_button=new RDPushButton(this);
  air_add_button->setFont(bigButtonFont());
  air_add_button->setText(tr("ADD"));
 air_add_button->setFocusPolicy(Qt::NoFocus);
  connect(air_add_button,SIGNAL(clicked()),this,SLOT(addButtonData()));

  //
  // Delete Button
  //
  air_delete_button=new RDPushButton(this);
  air_delete_button->setFont(bigButtonFont());
  air_delete_button->setText(tr("DEL"));
  air_delete_button->setFlashColor(AIR_FLASH_COLOR);
 air_delete_button->setFocusPolicy(Qt::NoFocus);
  connect(air_delete_button,SIGNAL(clicked()),this,SLOT(deleteButtonData()));

  //
  // Move Button
  //
  air_move_button=new RDPushButton(this);
  air_move_button->setFont(bigButtonFont());
  air_move_button->setText(tr("MOVE"));
  air_move_button->setFlashColor(AIR_FLASH_COLOR);
 air_move_button->setFocusPolicy(Qt::NoFocus);
  connect(air_move_button,SIGNAL(clicked()),this,SLOT(moveButtonData()));

  //
  // Copy Button
  //
  air_copy_button=new RDPushButton(this);
  air_copy_button->setFont(bigButtonFont());
  air_copy_button->setText(tr("COPY"));
  air_copy_button->setFlashColor(AIR_FLASH_COLOR);
  air_copy_button->setFocusPolicy(Qt::NoFocus);
  connect(air_copy_button,SIGNAL(clicked()),this,SLOT(copyButtonData()));

  //
  // Bug
  //
  QPixmap *bug=new QPixmap(bug_138x60_xpm);
  air_bug_label=new QLabel(this);
  air_bug_label->setScaledContents(true);
  air_bug_label->setPixmap(*bug);
  delete bug;
  
  air_splash_screen->showMessage(tr("Initializing sound panel array..."));

  //
  // Sound Panel Array
  //
  if (rda->airplayConf()->panels(RDAirPlayConf::StationPanel) || 
      rda->airplayConf()->panels(RDAirPlayConf::UserPanel)){
    int card=-1;
    air_panel=
      new SoundPanel(air_event_player,air_cart_dialog,dump_panel_updates,this);
    air_panel->soundPanelWidget()->
      setPauseEnabled(rda->airplayConf()->panelPauseEnabled());
    air_panel->soundPanelWidget()->setCard(0,rda->airplayConf()->
		       card(RDAirPlayConf::SoundPanel1Channel));
    air_panel->soundPanelWidget()->setPort(0,rda->airplayConf()->
		       port(RDAirPlayConf::SoundPanel1Channel));
    air_panel->soundPanelWidget()->setFocusPolicy(Qt::NoFocus);
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel2Channel))<0) {
      air_panel->soundPanelWidget()->
	setCard(1,air_panel->
		soundPanelWidget()->card(RDAirPlayConf::MainLog1Channel));
      air_panel->soundPanelWidget()->
	setPort(1,air_panel->
		soundPanelWidget()->port(RDAirPlayConf::MainLog1Channel));
    }
    else {
      air_panel->soundPanelWidget()->setCard(1,card);
      air_panel->soundPanelWidget()->setPort(1,rda->airplayConf()->
			 port(RDAirPlayConf::SoundPanel2Channel));
    }
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel3Channel))<0) {
      air_panel->soundPanelWidget()->
	setCard(2,air_panel->
		soundPanelWidget()->card(RDAirPlayConf::MainLog2Channel));
      air_panel->soundPanelWidget()->
	setPort(2,air_panel->
		soundPanelWidget()->port(RDAirPlayConf::MainLog2Channel));
    }
    else {
      air_panel->soundPanelWidget()->setCard(2,card);
      air_panel->soundPanelWidget()->setPort(2,rda->airplayConf()->
			 port(RDAirPlayConf::SoundPanel3Channel));
    }
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel4Channel))<0) {
      air_panel->soundPanelWidget()->
	setCard(3,air_panel->
		soundPanelWidget()->card(RDAirPlayConf::SoundPanel1Channel));
      air_panel->soundPanelWidget()->
	setPort(3,air_panel->
		soundPanelWidget()->port(RDAirPlayConf::SoundPanel1Channel));
    }
    else {
      air_panel->soundPanelWidget()->setCard(3,card);
      air_panel->soundPanelWidget()->setPort(3,rda->airplayConf()->
			 port(RDAirPlayConf::SoundPanel4Channel));
    }
    if((card=rda->airplayConf()->card(RDAirPlayConf::SoundPanel5Channel))<0) {
      air_panel->soundPanelWidget()->
	setCard(4,air_panel->
		soundPanelWidget()->card(RDAirPlayConf::CueChannel));
      air_panel->soundPanelWidget()->
	setPort(4,air_panel->
		soundPanelWidget()->port(RDAirPlayConf::CueChannel));
    }
    else {
      air_panel->soundPanelWidget()->setCard(4,card);
      air_panel->soundPanelWidget()->setPort(4,rda->airplayConf()->
			 port(RDAirPlayConf::SoundPanel5Channel));
    }
    air_panel->soundPanelWidget()->setRmls(0,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel1Channel),
		  rda->airplayConf()->
		       stopRml(RDAirPlayConf::SoundPanel1Channel));
    air_panel->soundPanelWidget()->setRmls(1,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel2Channel),
		  rda->airplayConf()->
		       stopRml(RDAirPlayConf::SoundPanel2Channel));
    air_panel->soundPanelWidget()->setRmls(2,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel3Channel),
		  rda->airplayConf()->
		       stopRml(RDAirPlayConf::SoundPanel3Channel));
    air_panel->soundPanelWidget()->setRmls(3,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel4Channel),
		  rda->airplayConf()->
		       stopRml(RDAirPlayConf::SoundPanel4Channel));
    air_panel->soundPanelWidget()->setRmls(4,rda->airplayConf()->
		  startRml(RDAirPlayConf::SoundPanel5Channel),
		  rda->airplayConf()->
		       stopRml(RDAirPlayConf::SoundPanel5Channel));
    int next_output=0;
    bool assigned=false;
    if((air_log[0]->card(0)==air_log[0]->card(RDAirPlayConf::MainLog2Channel))&&
      (air_log[0]->port(0)==air_log[0]->port(RDAirPlayConf::MainLog2Channel))) {
      next_output=2;
    }
    else {
      next_output=3;
    }
    for(int i=0;i<RD_SOUNDPANEL_MAX_OUTPUTS;i++) {
      assigned=false;
      for(int j=0;j<2;j++) {
	if((air_panel->soundPanelWidget()->
	    card((RDAirPlayConf::Channel)i)==air_log[0]->card(j))&&
	   (air_panel->soundPanelWidget()->
	    port((RDAirPlayConf::Channel)i)==air_log[0]->port(j))) {
	  next_output--;
	  assigned=true;
	  j=2;
	}
      }
      if(!assigned) {
	for(int j=0;j<i;j++) {
	  if((i!=j)&&(air_panel->soundPanelWidget()->
		      card((RDAirPlayConf::Channel)i)==
		      air_panel->soundPanelWidget()->card(j))&&
	     (air_panel->soundPanelWidget()->
	      port((RDAirPlayConf::Channel)i)==air_panel->soundPanelWidget()->port(j))) {
	    next_output--;
	    j=RD_SOUNDPANEL_MAX_OUTPUTS;
	  }
	}
      }
    }
    for(int i=0;i<RD_SOUNDPANEL_MAX_OUTPUTS;i++) {
      air_panel->soundPanelWidget()->
	setOutputText(i,rda->portNames()->
		      portName(air_panel->soundPanelWidget()->card(i),
			       air_panel->soundPanelWidget()->port(i)));
    }

    air_panel->soundPanelWidget()->setSvcName(rda->airplayConf()->defaultSvc());
    connect(rda->ripc(),SIGNAL(userChanged()),
	    air_panel->soundPanelWidget(),SLOT(changeUser()));
    connect(air_master_timer,SIGNAL(timeout()),
	    air_panel->soundPanelWidget(),SLOT(tickClock()));
    connect(air_panel->soundPanelWidget(),
	    SIGNAL(selectClicked(unsigned,int,int)),
	    this,SLOT(selectClickedData(unsigned,int,int)));
    connect(air_panel->soundPanelWidget(),SIGNAL(channelStarted(int,int,int)),
	    this,SLOT(panelChannelStartedData(int,int,int)));
    connect(air_panel->soundPanelWidget(),SIGNAL(channelStopped(int,int,int)),
	    this,SLOT(panelChannelStoppedData(int,int,int)));
  }

  //
  // Voice Tracker
  //
  air_tracker=new VoiceTracker(this);
  air_tracker->hide();
  
  //
  // Full Log List
  //
  air_pause_enabled=rda->airplayConf()->pauseEnabled();
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_list[i]=new ListLog(air_log[i],i,air_pause_enabled,this);
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
    air_log_button[i]->setFont(bigButtonFont());
    air_log_button[i]->setFocusPolicy(Qt::NoFocus);
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
  if(!rda->station()->enableDragdrop()) {
    air_empty_cart->hide();
  }

  //
  // SoundPanel Button
  //
  air_panel_button=new QPushButton(this);
  air_panel_button->setFont(bigButtonFont());
  air_panel_button->setText(tr("Sound\nPanel"));
  air_panel_button->setPalette(active_color);
  air_panel_button->setFocusPolicy(Qt::NoFocus);
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
  // Voice Tracker Button
  //
  air_tracker_button=new QPushButton(this);
  air_tracker_button->setFont(bigButtonFont());
  air_tracker_button->setText(tr("Voice\nTracker"));
  air_tracker_button->setFocusPolicy(Qt::NoFocus);
  connect(air_tracker_button,SIGNAL(clicked()),this,SLOT(trackerButtonData()));

  //
  // Button Log
  //
  air_button_list=
    new ButtonLog(air_log[0],0,rda->airplayConf(),air_pause_enabled,this);
  air_button_list->pieCounterWidget()->setOpMode(air_op_mode[0]);
  air_pie_end=rda->airplayConf()->pieEndPoint();
  connect(air_button_list,SIGNAL(selectClicked(int,int,RDLogLine::Status)),
	  this,SLOT(selectClickedData(int,int,RDLogLine::Status)));
  connect(air_button_list,SIGNAL(cartDropped(int,int,RDLogLine *)),
	  this,SLOT(cartDroppedData(int,int,RDLogLine *)));
  connect(air_master_timer,SIGNAL(timeout()),
	  air_button_list->postCounterWidget(),SLOT(tickCounter()));
  connect(air_log[0],SIGNAL(postPointChanged(QTime,int,bool,bool)),
	  air_button_list->postCounterWidget(),
	  SLOT(setPostPoint(QTime,int,bool,bool)));
  connect(air_master_timer,SIGNAL(timeout()),
	  air_button_list->pieCounterWidget(),SLOT(tickCounter()));
  connect(air_master_timer,SIGNAL(timeout()),
	  air_button_list->stopCounterWidget(),SLOT(tickCounter()));
  connect(air_log[0],SIGNAL(nextStopChanged(QTime)),
	  air_button_list->stopCounterWidget(),SLOT(setTime(QTime)));
  connect(air_button_list,SIGNAL(slotQuantityChanged(int)),
	  air_log[0],SLOT(setSlotQuantity(int)));

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
			   " ["+QString(strerror(errno))+"].");
    }
  }

  rda->syslog(LOG_INFO,"RDAirPlay started");

  if(!loadSettings(true)) {
    showMaximized();
  }

  air_splash_screen->showMessage(tr("Connecting to Rivendell services..."));
}


QSize MainWidget::sizeHint() const
{
  return QSize(1280,870);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::caeConnectedData(bool state)
{
  QList<int> cards;

  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::MainLog1Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::MainLog2Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::AuxLog1Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::AuxLog2Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::SoundPanel1Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::SoundPanel2Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::SoundPanel3Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::SoundPanel4Channel));
  cards.push_back(rda->airplayConf()->card(RDAirPlayConf::SoundPanel5Channel));
  rda->cae()->enableMetering(&cards);
}


void MainWidget::ripcConnectedData(bool state)
{
  QString logname;
  QHostAddress addr;
  QString sql;
  RDSqlQuery *q;
  RDMacro rml;

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
	  if((air_start_line[i]=rda->airplayConf()->logCurrentLine(i))>=0) {
	    air_start_start[i]=rda->airplayConf()->autoRestart(i)&&
	      rda->airplayConf()->logRunning(i)&&
	      (rdairplay_previous_exit_code==RDAirPlayConf::ExitDirty);
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
	air_start_line[i]=0;
	air_start_start[i]=false;
	break;
      }
    }
    if(!air_start_logname[i].isEmpty()) {
      sql=QString("select `NAME` from `LOGS` where ")+
	"`NAME`='"+RDEscapeString(air_start_logname[i])+"'";
      q=new RDSqlQuery(sql);
      if(q->first()) {
	rml.clear();
	rml.setRole(RDMacro::Cmd);
	addr.setAddress("127.0.0.1");
	rml.setAddress(addr);
	rml.setEchoRequested(false);
	rml.setCommand(RDMacro::LL);  // Load Log
	rml.addArg(i+1);
	rml.addArg(air_start_logname[i]);
	rda->ripc()->sendRml(&rml);
      }
      else {
	fprintf(stderr,"rdairplay: log \"%s\" doesn't exist\n",
		air_start_logname[i].toUtf8().constData());
      }
      delete q;
    }
  }

  QTimer *timer=new QTimer();
  timer->setSingleShot(true);
  connect(timer,SIGNAL(timeout()),air_splash_screen,SLOT(finish()));
  timer->start(0);
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
	    start(AIR_CHANNEL_LOCKOUT_INTERVAL);
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
      air_panel->soundPanelWidget()->channelStop(0);
    }
    for(unsigned i=6;i<10;i++) {
      if((air_stop_gpi_matrices[i]==matrix)&&
	 (air_stop_gpi_lines[i]==line)) {
	air_panel->soundPanelWidget()->channelStop(i-5);
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
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::MainLog1Channel],
		      air_start_gpo_lines[RDAirPlayConf::MainLog1Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 1 300!",
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
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::MainLog2Channel],
		      air_start_gpo_lines[RDAirPlayConf::MainLog2Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 1 300!",
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
	air_event_player->
	  exec(QString::asprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::AuxLog1Channel],
		      air_start_gpo_lines[RDAirPlayConf::AuxLog1Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	air_event_player->
	  exec(QString::asprintf("GO %d %d 1 300!",
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
	air_event_player->
	  exec(QString::asprintf("GO %d %d 1 0!",
		      air_start_gpo_matrices[RDAirPlayConf::AuxLog2Channel],
		      air_start_gpo_lines[RDAirPlayConf::AuxLog2Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	air_event_player->
	  exec(QString::asprintf("GO %d %d 1 300!",
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
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::MainLog1Channel],
		      air_stop_gpo_lines[RDAirPlayConf::MainLog1Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 1 300!",
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
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::MainLog2Channel],
		      air_stop_gpo_lines[RDAirPlayConf::MainLog2Channel]+1));
	  break;

	case RDAirPlayConf::EdgeGpio:
	  air_event_player->
	    exec(QString::asprintf("GO %d %d 1 300!",
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
	air_event_player->
	  exec(QString::asprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::AuxLog1Channel],
		      air_stop_gpo_lines[RDAirPlayConf::AuxLog1Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	air_event_player->
	  exec(QString::asprintf("GO %d %d 1 300!",
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
	air_event_player->
	  exec(QString::asprintf("GO %d %d 0 0!",
		      air_stop_gpo_matrices[RDAirPlayConf::AuxLog2Channel],
		      air_stop_gpo_lines[RDAirPlayConf::AuxLog2Channel]+1));
	break;

      case RDAirPlayConf::EdgeGpio:
	air_event_player->
	  exec(QString::asprintf("GO %d %d 1 300!",
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
      air_event_player->
	exec(QString::asprintf("GO %d %d 1 0!",
			       air_start_gpo_matrices[chan],
			       air_start_gpo_lines[chan]+1));
      break;

    case RDAirPlayConf::EdgeGpio:
      air_event_player->
	exec(QString::asprintf("GO %d %d 1 300!",
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
      air_event_player->
	exec(QString::asprintf("GO %d %d 0 0!",
			       air_stop_gpo_matrices[chan],
			       air_stop_gpo_lines[chan]+1));
      break;

    case RDAirPlayConf::EdgeGpio:
      air_event_player->
	exec(QString::asprintf("GO %d %d 1 300!",
			       air_stop_gpo_matrices[chan],
			       air_stop_gpo_lines[chan]+1));
      break;
    }
  }
}


void MainWidget::logRenamedData(int log)
{
  QString str;
  QString logname=air_log[log]->logName();
  QString labelname=logname;
  if(logname.isEmpty()) {
    labelname="--";
  }
  switch(log) {
  case 0:
    air_log_button[0]->setText(tr("Main Log")+"\n["+labelname+"]");
    SetCaption();
    break;

  case 1:
    air_log_button[1]->setText(tr("Aux 1 Log")+"\n["+labelname+"]");
    break;
	
  case 2:
    air_log_button[2]->setText(tr("Aux 2 Log")+"\n["+labelname+"]");
    break;
  }
}


void MainWidget::logReloadedData(int log)
{
  QString str;
  QHostAddress addr;
  QString labelname=air_log[log]->logName();
  if(labelname.isEmpty()) {
    labelname="--";
  }

  switch(log) {
  case 0:
    air_log_button[0]->setText(tr("Main Log")+"\n["+labelname+"]");
    rda->syslog(LOG_INFO,"loaded log '%s' in Main Log",
	   (const char *)air_log[0]->logName().toUtf8());
    if(air_log[0]->logName().isEmpty()) {
      if(air_panel!=NULL) {
	air_panel->soundPanelWidget()->
	  setSvcName(rda->airplayConf()->defaultSvc());
      }
    }
    else {
      if(air_panel!=NULL) {
	air_panel->soundPanelWidget()->setSvcName(air_log[0]->serviceName());
      }
    }
    break;

  case 1:
    air_log_button[1]->setText(tr("Aux 1 Log")+"\n["+labelname+"]");
    rda->syslog(LOG_INFO,"loaded log '%s' in Aux 1 Log",
	   (const char *)air_log[1]->logName().toUtf8());
    break;
	
  case 2:
    air_log_button[2]->setText(tr("Aux 2 Log")+"\n["+labelname+"]");
    rda->syslog(LOG_INFO,"loaded log '%s' in Aux Log 2",
	   (const char *)air_log[2]->logName().toUtf8());
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

  if(air_start_line[log]<air_log[log]->lineCount()) {
    rml.setCommand(RDMacro::MN);  // Make Next
    rml.addArg(log+1);
    rml.addArg(air_start_line[log]);
    rda->ripc()->sendRml(&rml);
    
    if(air_start_start[log]) {
      rml.clear();
      rml.setRole(RDMacro::Cmd);
      addr.setAddress("127.0.0.1");
      rml.setAddress(addr);
      rml.setEchoRequested(false);
      rml.setCommand(RDMacro::PN);  // Start Next
      rml.addArg(log+1);
      rda->ripc()->sendRml(&rml);
    }
  }
  else {
    fprintf(stderr,"rdairplay: line %d doesn't exist in log \"%s\"\n",
	    air_start_line[log],air_start_logname[log].toUtf8().constData());
  }
  air_start_logname[log]="";
}


void MainWidget::userData()
{
  rda->syslog(LOG_INFO,"user changed to '%s'",
	      (const char *)rda->ripc()->user().toUtf8());
  SetCaption();

  //
  // Set Control Perms
  //
  bool add_allowed=rda->user()->addtoLog();
  bool delete_allowed=rda->user()->removefromLog();
  bool arrange_allowed=rda->user()->arrangeLog();
  bool playout_allowed=rda->user()->playoutLog();
  bool tracking_allowed=rda->user()->voicetrackLog();

  air_add_button->setEnabled(add_allowed&&arrange_allowed&&playout_allowed);
  air_move_button->setEnabled(arrange_allowed&&playout_allowed);
  air_delete_button->
    setEnabled(delete_allowed&&arrange_allowed&&playout_allowed);
  air_copy_button->setEnabled(add_allowed&&arrange_allowed&&playout_allowed);
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_list[i]->userChanged(add_allowed,delete_allowed,
				 arrange_allowed,playout_allowed);
  }
  air_tracker_button->setEnabled(tracking_allowed);
  if((!tracking_allowed)&&air_tracker->isVisible()) {
    fullLogButtonData(0);
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
    air_panel->hide();
    air_tracker->hide();
    air_tracker_button->setPalette(palette());
    for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
      if(air_log_list[i]->isVisible()) {
	air_log_list[i]->hide();
	air_log_button[i]->setPalette(palette());
      }
    }
    air_log_list[id]->show();
    air_log_button[id]->setPalette(active_color);
    if(air_panel_button) {
      air_panel_button->setPalette(palette());
    }
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
  air_tracker->hide();
  air_tracker_button->setPalette(palette());
  air_panel->show();
  air_panel_button->setPalette(active_color);
}


void MainWidget::trackerButtonData()
{
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    if(air_log_list[i]->isVisible()) {
      air_log_list[i]->hide();
      air_log_button[i]->setPalette(palette());
    }
  }
  air_panel->hide();
  air_panel_button->setPalette(palette());
  air_tracker->show();
  air_tracker_button->setPalette(active_color);
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
	insert(air_log[id]->lineCount(),air_add_cart,RDLogLine::Play,
	       rda->airplayConf()->defaultTransType());
      air_log[id]->logLine(air_log[id]->lineCount()-1)->
	setTransType(rda->airplayConf()->defaultTransType());
      air_log[id]->update(air_log[id]->lineCount()-1);
    }
    else {
      air_log[id]->
	insert(line,air_add_cart,air_log[id]->nextTransType(line),
	       rda->airplayConf()->defaultTransType());
      air_log[id]->logLine(line)->
	setTransType(rda->airplayConf()->defaultTransType());
      air_log[id]->update(line);
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
	air_log[id]->move(air_copy_line,air_log[id]->lineCount());
	air_log[id]->update(air_log[id]->lineCount()-1);
      }
      else {
	if(line>air_copy_line) {
	  line--;
	}
	air_log[id]->move(air_copy_line,line);
	air_log[id]->update(line);
      }
    }
    else {
      air_log[air_source_id]->remove(air_copy_line,1);
      if(line<0) {
	air_log[id]->
	  insert(air_log[id]->lineCount(),air_add_cart,RDLogLine::Play);
	air_log[id]->update(air_log[id]->lineCount()-1);
      }
      else {
	air_log[id]->
	  insert(line,air_add_cart,air_log[id]->nextTransType(line));
	air_log[id]->update(line);
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
	air_log[id]->copy(air_copy_line,air_log[id]->lineCount(),
			  rda->airplayConf()->defaultTransType());
      }
      else {
	air_log[id]->
	  copy(air_copy_line,line,rda->airplayConf()->defaultTransType());
      }
    }
    else {
      if(line<0) {
	air_log[id]->insert(air_log[id]->lineCount(),air_add_cart,
			    rda->airplayConf()->defaultTransType(),
			    rda->airplayConf()->defaultTransType());
	air_log[id]->logLine(air_log[id]->lineCount()-1)->
	  setTransType(rda->airplayConf()->defaultTransType());
	air_log[id]->update(air_log[id]->lineCount()-1);
      }
      else {
	air_log[id]->
	  insert(line,air_add_cart,air_log[id]->nextTransType(line),
		 rda->airplayConf()->defaultTransType());
	air_log[id]->logLine(line)->
	  setTransType(rda->airplayConf()->defaultTransType());
	air_log[id]->update(line);
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
      air_panel->soundPanelWidget()->
	setButton(air_panel->soundPanelWidget()->currentType(),
		  air_panel->soundPanelWidget()->currentNumber(),
		  row,col,air_add_cart);
    }
    SetActionMode(StartButton::Stop);
    break;

  case StartButton::AddTo:
    if(air_panel!=NULL) {
      air_panel->soundPanelWidget()->
	setButton(air_panel->soundPanelWidget()->currentType(),
		  air_panel->soundPanelWidget()->currentNumber(),
		  row,col,air_add_cart);
    }
    SetActionMode(StartButton::Stop);
    break;

  case StartButton::DeleteFrom:
    if(air_panel!=NULL) {
      air_panel->soundPanelWidget()->
	setButton(air_panel->soundPanelWidget()->currentType(),
		  air_panel->soundPanelWidget()->currentNumber(),row,col,0);
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
	insert(air_log[id]->lineCount(),ll->cartNumber(),RDLogLine::Play,
	       rda->airplayConf()->defaultTransType());
      air_log[id]->logLine(air_log[id]->lineCount()-1)->
	setTransType(rda->airplayConf()->defaultTransType());
      air_log[id]->update(air_log[id]->lineCount()-1);
    }
    else {
      air_log[id]->
       insert(line,ll->cartNumber(),air_log[id]->nextTransType(line),
	       rda->airplayConf()->defaultTransType());
      air_log[id]->logLine(line)->
	setTransType(rda->airplayConf()->defaultTransType());
      air_log[id]->update(line);
    }
  }
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
      if((air_button_list->pieCounterWidget()->line()!=logline->id())) {
	switch(pie_end) {
	case RDAirPlayConf::CartEnd:
	  air_button_list->pieCounterWidget()->setTime(logline->effectiveLength());
	  break;
	      
	case RDAirPlayConf::CartTransition:
	  if((next_logline=air_log[0]->
	      logLine(air_log[0]->nextLine(line)))!=NULL) {
	    //
	    // Are we not past the segue point?
	    //
	    if((logline->playPosition()>
		(unsigned)logline->segueLength(next_logline->transType()))||
	       ((unsigned)logline->startTime(RDLogLine::Actual).
	       msecsTo(QTime::currentTime())<
	       logline->segueLength(next_logline->transType())-
	       logline->playPosition())) {
	      air_button_list->pieCounterWidget()->
		setTime(logline->segueLength(next_logline->transType()));
	    }
	  }
	  else {
	    air_button_list->pieCounterWidget()->setTime(logline->effectiveLength());
	  }
	  break;
	}
	if(logline->talkStartPoint()==0) {
          air_button_list->pieCounterWidget()->setTalkStart(0);
  	  air_button_list->pieCounterWidget()->setTalkEnd(logline->talkEndPoint());
        }
        else {
	air_button_list->pieCounterWidget()->
	  setTalkStart(logline->talkStartPoint()-logline->
		         startPoint());
	air_button_list->pieCounterWidget()->
	  setTalkEnd(logline->talkEndPoint()-logline->
		         startPoint());
        }
	air_button_list->pieCounterWidget()->setTransType(air_log[0]->nextTrans(line));
	if(logline->playDeck()==NULL) {
	  air_button_list->pieCounterWidget()->setLogline(NULL);
	  air_button_list->pieCounterWidget()->start(rda->station()->timeOffset());
	}
	else {
	  air_button_list->pieCounterWidget()->setLogline(logline);
	  air_button_list->pieCounterWidget()->start(((RDPlayDeck *)logline->playDeck())->
				 currentPosition()+
				 rda->station()->timeOffset());
	}
      }
    }
    else {
      air_button_list->pieCounterWidget()->stop();
      air_button_list->pieCounterWidget()->resetTime();
      air_button_list->pieCounterWidget()->setLine(-1);
    }
  }
  else {
    air_button_list->pieCounterWidget()->stop();
    air_button_list->pieCounterWidget()->resetTime();
    air_button_list->pieCounterWidget()->setLine(-1);
  }
}


void MainWidget::keyReleaseEvent(QKeyEvent *e)
{
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

  case Qt::Key_X:
    if(((e->modifiers()&Qt::AltModifier)!=0)&&
       ((e->modifiers()&Qt::ShiftModifier)==0)&&
       ((e->modifiers()&Qt::ControlModifier)==0)) {
      QCloseEvent *ce=new QCloseEvent();
      closeEvent(ce);
      delete ce;
    }
    break;
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
      e->ignore();
      return;
    }
    rda->airplayConf()->setExitCode(RDAirPlayConf::ExitClean);
    rda->syslog(LOG_INFO,"RDAirPlay exiting");
    saveSettings();
    exit(0);
  }
  if(QMessageBox::question(this,"RDAirPlay",tr("Exit RDAirPlay?"),
			   QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    e->setAccepted(false);
    return;
  }
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    delete air_log[i];
  }
  rda->airplayConf()->setExitCode(RDAirPlayConf::ExitClean);
  rda->syslog(LOG_INFO,"RDAirPlay exiting");
  saveSettings();
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  int w=width();
  int h=height();

  //
  // Top Row
  //
  air_top_strip->setGeometry(0,0,w,10+RD_RDAIRPLAY_LOGO_HEIGHT);

  //
  // Button Log
  //
  air_button_list->setGeometry(10,140,
			       air_button_list->sizeHint().width(),
			       h-215);
  
  //
  // Sound Panel
  //
  if(air_panel!=NULL) {
    air_panel->setGeometry(550,140,w-560,h-215);
  }

  //
  // Voice Tracker
  //
  air_tracker->setGeometry(550,140,w-555,h-215);

  //
  // Full Log Widgets
  //
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_list[i]->setGeometry(550,140,w-560,h-215);
  }

  //
  // Bottom Button Row
  //
  air_add_button->setGeometry(10,h-65,80,60);
  air_delete_button->setGeometry(100,h-65,80,60);
  air_move_button->setGeometry(190,h-65,80,60);
  air_copy_button->setGeometry(280,h-65,80,60);

  air_bug_label->setGeometry(370,h-65,138,60);
  air_empty_cart->setGeometry(520,h-51,32,32);

  int xpos=562;
  air_panel_button->setGeometry(xpos,h-65,80,60);
  xpos+=85;

  air_tracker_button->setGeometry(xpos,h-65,80,60);
  xpos+=85;
  air_tracker_button->show();

  int log_button_w=(w-xpos-5)/3;
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    air_log_button[i]->setGeometry(xpos+i*log_button_w,h-65,log_button_w-5,60);
  }
}


void MainWidget::wheelEvent(QWheelEvent *e)
{
  if((air_panel!=NULL)&&(e->orientation()==Qt::Vertical)) {
    if(e->delta()>0) {
      air_panel->soundPanelWidget()->panelDown();
    }
    if(e->delta()<0) {
      air_panel->soundPanelWidget()->panelUp();
    }
  }
  e->accept();
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
  setWindowTitle(QString("RDAirPlay")+" v"+VERSION+" - "+tr("Host")+": "+
		 rda->config()->stationName()+" "+
		 tr("User:")+" "+rda->ripc()->user()+" "+
		 tr("Log:")+" "+log+" "+
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
    air_button_list->pieCounterWidget()->setOpMode(RDAirPlayConf::Manual);
  }
  air_top_strip->modeDisplayWidget()->setOpMode(mach,RDAirPlayConf::Manual);
  air_op_mode[mach]=RDAirPlayConf::Manual;
  rda->airplayConf()->setOpMode(mach,RDAirPlayConf::Manual);
  air_log[mach]->setOpMode(RDAirPlayConf::Manual);
  air_log_list[mach]->setOpMode(RDAirPlayConf::Manual);
  if(mach==0) {
    air_button_list->setOpMode(RDAirPlayConf::Manual);
    air_button_list->postCounterWidget()->setDisabled(true);
  }
  rda->syslog(LOG_INFO,"log machine %d mode set to MANUAL",mach+1);
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
    air_button_list->pieCounterWidget()->setOpMode(RDAirPlayConf::Auto);
  }
  air_top_strip->modeDisplayWidget()->setOpMode(mach,RDAirPlayConf::Auto);
  air_op_mode[mach]=RDAirPlayConf::Auto;
  rda->airplayConf()->setOpMode(mach,RDAirPlayConf::Auto);
  air_log[mach]->setOpMode(RDAirPlayConf::Auto);
  air_log_list[mach]->setOpMode(RDAirPlayConf::Auto);
  if(mach==0) {
    air_button_list->setOpMode(RDAirPlayConf::Auto);
    air_button_list->postCounterWidget()->setEnabled(true);
  }
  rda->syslog(LOG_INFO,"log machine %d mode set to AUTO",mach+1);
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
    air_button_list->pieCounterWidget()->setOpMode(RDAirPlayConf::LiveAssist);
  }
  air_top_strip->modeDisplayWidget()->setOpMode(mach,RDAirPlayConf::LiveAssist);
  air_op_mode[mach]=RDAirPlayConf::LiveAssist;
  rda->airplayConf()->setOpMode(mach,RDAirPlayConf::LiveAssist);
  air_log[mach]->setOpMode(RDAirPlayConf::LiveAssist);
  air_log_list[mach]->setOpMode(RDAirPlayConf::LiveAssist);
  if(mach==0) {
    air_button_list->setOpMode(RDAirPlayConf::LiveAssist); 
    air_button_list->postCounterWidget()->setDisabled(true);
  }
  rda->syslog(LOG_INFO,"log machine %d mode set to LIVE ASSIST",mach+1);
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::Normal);
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
      sql=QString("select `SERVICE_NAME` from `SERVICE_PERMS` where ")+
	"`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"'";
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::Normal);
    }
    if(air_cart_dialog->
       exec(&air_add_cart,RDCart::All,air_log[0]->serviceName(),NULL)) {
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::AddTo);
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::DeleteFrom);
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::MoveFrom);
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::MoveTo);
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::CopyFrom);
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
      air_panel->soundPanelWidget()->setActionMode(RDAirPlayConf::CopyTo);
    }
    break;

  default:
    break;
  }
}


int main(int argc,char *argv[])
{
  bool user_style=false;
  for(int i=0;i<argc;i++) {
    if(QString(argv[i])=="-style") {
      user_style=true;
      break;
    }
  }
  if(!user_style) {
    QApplication::setStyle(RD_GUI_STYLE);
  }
  QApplication a(argc,argv);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();

  return a.exec();
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
    air_channel_timers[dir][achan]->start(AIR_CHANNEL_LOCKOUT_INTERVAL);
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


void MainWidget::LoadMeters()
{
  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Add Meters
  //
  QList<int> strip_cards;
  QList<int> strip_ports;
  QStringList strip_labels;
  QList<int> strip_index;
  bool strip_changed=true;
  sql=QString("select ")+
    "`CARD`,"+  // 00
    "`PORT` "+  // 01
    "from `RDAIRPLAY_CHANNELS` where "+
    "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"' && "+
    QString::asprintf("`INSTANCE`<%u",RDAirPlayConf::LastChannel);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(((q->value(0).toInt()>=0)&&(q->value(1).toInt()>=0))&&
       ((!strip_cards.contains(q->value(0).toInt()))||
	(!strip_ports.contains(q->value(1).toInt())))) {
      strip_cards.push_back(q->value(0).toInt());
      strip_ports.push_back(q->value(1).toInt());
      strip_labels.
	push_back(rda->portNames()->
		  portName(q->value(0).toInt(),q->value(1).toInt()));
      strip_index.push_back(strip_index.size());
    }
  }
  delete q;

  while(strip_changed) {
    strip_changed=false;
    for(int i=0;i<(strip_index.size()-1);i++) {
      if(strip_labels.at(strip_index.at(i))>
	 strip_labels.at(strip_index.at(i+1))) {
	int index=strip_index.at(i);
	strip_index[i]=strip_index.at(i+1);
	strip_index[i+1]=index;
	strip_changed=true;
      }
    }
  }
  air_meter_strip=new RDMeterStrip(this);
  for(int i=0;i<strip_cards.size();i++) {
    air_top_strip->meterWidget()->
      addOutputMeter(strip_cards.at(strip_index.at(i)),
		     strip_ports.at(strip_index.at(i)),
		     strip_labels.at(strip_index.at(i)));
  }
}
