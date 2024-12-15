// rdtrackerwidget.cpp
//
// Rivendell Voice Tracker
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

#include <math.h>
#include <sys/time.h>

#include <QMessageBox>

#include "rdconf.h"
#include "rdsvc.h"
#include "rdimport_audio.h"
#include "rdlogeventdialog.h"
#include "rdrehash.h"
#include "rdtrackereditdialog.h"
#include "rdtrackerwidget.h"

RDTrackerWidget::RDTrackerWidget(QString *import_path,QWidget *parent)
  : RDWidget(parent)
{
  d_cursor_xpos=-1;
  d_log=NULL;
  d_svc=NULL;
  d_group=NULL;
  d_tracks=0;

  d_import_path=import_path;
  d_coding=RDCae::Pcm16;
  d_track_cart=NULL;
  d_sliding=false;
  d_scrolling=false;
  d_track_line=-1;
  d_loaded=false;
  d_offset=false;
  d_segue_loaded=false;
  d_recording=false;
  d_changed=false;
  d_recording_pos=0;
  d_record_ran=false;
  d_aborting=false;
  d_block_valid=false;
  d_time_remaining=0;
  d_time_counter=0;
  d_start_time=QTime(0,0,0);
  d_size_altered=false;
  for(unsigned i=0;i<3;i++) {
    d_scroll_pos[i]=-1;
    d_track_lines[i]=-1;
    d_segue_start_offset[i]=0;
    d_track_cuts[i]=NULL;
    d_wpg[i]=NULL;
  }
  d_menu_clicked_point=-1;
  d_shift_pressed=false;
  d_active=false;
  d_focus_policy=Qt::WheelFocus;

  //
  // Create Palettes
  //
  d_record_palette=QPalette(TRACKER_RECORD_BUTTON_COLOR,palette().color(QPalette::Background));
  d_start_palette=QPalette(TRACKER_START_BUTTON_COLOR,palette().color(QPalette::Background));
  d_done_palette=QPalette(TRACKER_DONE_BUTTON_COLOR,palette().color(QPalette::Background));
  d_abort_palette=QPalette(TRACKER_ABORT_BUTTON_COLOR,palette().color(QPalette::Background));
  QColor system_mid_color = palette().mid().color();
  QColor system_button_color = palette().button().color();

  //
  // Create Track and Target Region
  //
  d_track_rect=new QRect(0,0,TRACKER_X_WIDTH,3*TRACKER_Y_HEIGHT);
  for(unsigned i=0;i<RDTrackerWidget::TargetSize;i++) {
    d_target_rect[i]=new QRect();
  }
  d_current_target=RDTrackerWidget::TargetSize;

  //
  // Create Cursors
  //
  d_arrow_cursor=new QCursor(Qt::ArrowCursor);
  d_hand_cursor=new QCursor(Qt::PointingHandCursor);
  d_cross_cursor=new QCursor(Qt::CrossCursor);
  d_current_cursor=d_arrow_cursor;
  setMouseTracking(true);

  //
  // Macro Event Player
  //
  d_event_player=new RDEventPlayer(rda->ripc(),this);

  //
  // Waveform Pixmaps
  //
  for(int i=0;i<3;i++) {
    d_wave_map[i]=new QPixmap();
  }
  d_previous_point=new QPoint(-1,-1);
  d_current_track=-1;

  //
  // Logline Dummies
  //
  d_dummy0_logline=new RDLogLine();
  d_dummy2_logline=new RDLogLine();
  for(unsigned i=0;i<3;i++) {
    d_saved_loglines[i]=new RDLogLine();
  }

  //
  // Audio Parameters
  //
  RDLogeditConf *conf=new RDLogeditConf(rda->config()->stationName());
  d_input_card=conf->inputCard();
  d_input_port=conf->inputPort();
  d_output_card=conf->outputCard();
  d_output_port=conf->outputPort();
  d_format=conf->format();
  d_samprate=rda->system()->sampleRate();
  d_bitrate=conf->bitrate();
  d_chans=conf->defaultChannels();
  d_play_start_macro=conf->startCart();
  d_play_end_macro=conf->endCart();
  d_record_start_macro=conf->recStartCart();
  d_record_end_macro=conf->recEndCart();
  d_preroll=conf->tailPreroll();
  d_scroll_threshold=TRACKER_X_WIDTH-d_preroll/TRACKER_MSECS_PER_PIXEL;
  d_settings=new RDSettings();
  conf->getSettings(d_settings);
  delete conf;

  RDLibraryConf *lconf=new RDLibraryConf(rda->config()->stationName());
  d_tail_preroll=lconf->tailPreroll();
  d_threshold_level=lconf->trimThreshold();
  delete lconf;
  
  //
  // Dialogs
  //
  d_marker_dialog=
    new RDMarkerDialog("RDLogEdit",d_output_card,d_output_port,this);

  //
  // Play Decks
  //
  for(int i=0;i<3;i++) {
    d_deck[i]=new RDPlayDeck(rda->cae(),i);
    d_deck[i]->setCard(d_output_card);
    d_deck[i]->setPort(d_output_port);
    connect(d_deck[i],SIGNAL(stateChanged(int,RDPlayDeck::State)),
	    this,SLOT(stateChangedData(int,RDPlayDeck::State)));
    connect(d_deck[i],SIGNAL(segueStart(int)),this,SLOT(segueStartData(int)));
  }

  //
  // Record Slot Connections
  //
  connect(rda->cae(),SIGNAL(recordLoaded(int,int)),
	  this,SLOT(recordLoadedData(int,int)));
  connect(rda->cae(),SIGNAL(recording(int,int)),
	  this,SLOT(recordingData(int,int)));
  connect(rda->cae(),SIGNAL(recordStopped(int,int)),
	  this,SLOT(recordStoppedData(int,int)));
  connect(rda->cae(),SIGNAL(recordUnloaded(int,int,unsigned)),
	  this,SLOT(recordUnloadedData(int,int,unsigned)));

  //
  // Right-Click Menu
  //
  d_mouse_menu=new QMenu(this);
  d_edit_cue_action=d_mouse_menu->
    addAction(tr("Edit Cue Markers"),this,SLOT(editAudioData()));
  d_edit_cue_action->setCheckable(false);
  d_undo_segue_action=d_mouse_menu->
    addAction(tr("Undo Segue Changes"),this,SLOT(undoChangesData()));
  d_undo_segue_action->setCheckable(false);
  d_set_start_action=d_mouse_menu->
    addAction(tr("Set Start Point Here"),this,SLOT(setStartPointData()));
  d_set_start_action->setCheckable(false);
  d_set_end_action=d_mouse_menu->
    addAction(tr("Set End Point Here"),this,SLOT(setEndPointData()));
  d_set_end_action->setCheckable(false);
  d_set_hook_action=d_mouse_menu->
    addAction(tr("Set to Hook Markers"),this,SLOT(setHookPointData()));
  d_set_hook_action->setCheckable(false);
  connect(d_mouse_menu,SIGNAL(aboutToShow()),this,SLOT(updateMenuData()));
  connect(d_mouse_menu,SIGNAL(aboutToHide()),this,SLOT(hideMenuData()));

  //
  // Track 1 Button
  //
  d_track1_button=new QPushButton(this);
  d_track1_button->setPalette(d_start_palette);
  d_track1_button->setFont(buttonFont());
  d_track1_button->setText(tr("Start"));
  connect(d_track1_button,SIGNAL(clicked()),this,SLOT(track1Data()));

  //
  // Record Button
  //
  d_record_button=new QPushButton(this);
  d_record_button->setPalette(d_record_palette);
  d_record_button->setFont(buttonFont());
  d_record_button->setText(tr("Record"));
  connect(d_record_button,SIGNAL(clicked()),this,SLOT(recordData()));

  //
  // Track 2 Button
  //
  d_track2_button=new QPushButton(this);
  d_track2_button->setPalette(d_start_palette);
  d_track2_button->setFont(buttonFont());
  d_track2_button->setText(tr("Start"));
  connect(d_track2_button,SIGNAL(clicked()),this,SLOT(track2Data()));
  if(!rda->logeditConf()->enableSecondStart()) {
    d_track2_button->hide();
  }

  //
  // Finished Button
  //
  d_finished_button=new QPushButton(this);
  d_finished_button->setPalette(d_done_palette);
  d_finished_button->setFont(buttonFont());
  d_finished_button->setText(tr("Save"));
  connect(d_finished_button,SIGNAL(clicked()),this,SLOT(finishedData()));

  //
  // Previous Button
  //
  d_previous_button=new QPushButton(this);
  d_previous_button->setFont(buttonFont());
  d_previous_button->setText(tr("Previous\nTrack"));
  connect(d_previous_button,SIGNAL(clicked()),this,SLOT(previousData()));

  //
  // Next Button
  //
  d_next_button=new QPushButton(this);
  d_next_button->setFont(buttonFont());
  d_next_button->setText(tr("Next\nTrack"));
  connect(d_next_button,SIGNAL(clicked()),this,SLOT(nextData()));

  //
  // Play Button
  //
  d_play_button=new RDTransportButton(RDTransportButton::Play,this);
  d_play_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  connect(d_play_button,SIGNAL(clicked()),
	  this,SLOT(playData()));

  //
  // Stop Button
  //
  d_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  d_stop_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  d_stop_button->setOnColor(Qt::red);
  d_stop_button->on();
  connect(d_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  //
  // Audio Meter
  //
  d_meter=new RDStereoMeter(this);
  d_meter->setMode(RDSegMeter::Peak);
  d_meter_timer=new QTimer(this);
  connect(d_meter_timer,SIGNAL(timeout()),this,SLOT(meterData()));

  //
  // Track Length Readout
  //
  d_length_label=new QLabel(this);
  d_length_label->setText("-:--:--.-");
  d_length_label->
    setStyleSheet("background-color: "+
		  palette().color(QPalette::Background).name());
  d_length_label->setAlignment(Qt::AlignCenter);
  d_length_label->setFont(timerFont());

  //
  // Tracks Remaining Readout
  //
  d_tracks_remaining_label_label=new QLabel(tr("Remaining"),this);
  d_tracks_remaining_label_label->setFont(subLabelFont());
  d_tracks_remaining_label_label->setAlignment(Qt::AlignHCenter);
  d_tracks_remaining_label_label->
    setPalette(QPalette(palette().color(QPalette::Background),palette().mid().
			color()));  
  d_tracks_remaining_label=new QLabel(this);
  d_tracks_remaining_label->setText("0");
  d_tracks_remaining_label->
    setStyleSheet("background-color: "+
		  palette().color(QPalette::Background).name());
  d_tracks_remaining_label->setAlignment(Qt::AlignCenter);
  d_tracks_remaining_label->setFont(labelFont());
  d_time_remaining_label_label=new QLabel(tr("Tracks"),this);
  d_time_remaining_label_label->setFont(subLabelFont());
  d_time_remaining_label_label->setAlignment(Qt::AlignHCenter);
  d_time_remaining_label_label->
    setPalette(QPalette(palette().color(QPalette::Background),palette().mid().
			color()));  

  d_time_remaining_label=new QLabel(this);
  d_time_remaining_label->setText("0:00:00.0");
  d_time_remaining_label->
    setStyleSheet("background-color: "+
		  palette().color(QPalette::Background).name());
  d_time_remaining_label->setAlignment(Qt::AlignCenter);
  d_time_remaining_label->setFont(labelFont());
  d_time_remaining_palette[0]=d_time_remaining_label->palette();
  d_time_remaining_palette[1]=d_time_remaining_label->palette();
  d_time_remaining_palette[1].
    setColor(QPalette::Active,QPalette::Foreground,Qt::red);
  d_time_remaining_palette[1].
    setColor(QPalette::Inactive,QPalette::Foreground,Qt::red);
  d_time_label=new QLabel(tr("Time"),this);
  d_time_label->setFont(subLabelFont());
  d_time_label->setAlignment(Qt::AlignHCenter);
  d_time_label->setPalette(QPalette(palette().color(QPalette::Background),
				    palette().mid().color()));

  //
  // Log List
  //
  d_log_view=new RDTrackerTableView(this);
  d_log_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  d_log_view->setSelectionMode(QAbstractItemView::SingleSelection);
  d_log_view->setShowGrid(false);
  d_log_view->setSortingEnabled(false);
  d_log_view->setWordWrap(false);
  d_log_view->setAcceptDrops(false);
  d_log_model=new RDTrackerModel(this);
  d_log_model->setFont(defaultFont());
  d_log_model->setPalette(palette());
  d_log_view->setModel(d_log_model);
  d_log_view->resizeColumnsToContents();
  connect(d_log_model,SIGNAL(modelReset()),
	  d_log_view,SLOT(resizeColumnsToContents()));
  connect(d_log_view->selectionModel(),
     SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
     this,
     SLOT(selectionChangedData(const QItemSelection &,const QItemSelection &)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  d_log_model,SLOT(processNotification(RDNotification *)));
  d_log_model->setStartTimeStyle(RDLogModel::Estimated);

  //
  // Reset Button
  //
  d_reset_button=new QPushButton(this);
  d_reset_button->setFont(buttonFont());
  d_reset_button->setText(tr("Do Over"));
  connect(d_reset_button,SIGNAL(clicked()),this,SLOT(resetData()));

  //
  // Hit Post Button
  //
  d_post_button=new QPushButton(this);
  d_post_button->setFont(buttonFont());
  d_post_button->setText(tr("Hit Post"));
  connect(d_post_button,SIGNAL(clicked()),this,SLOT(postData()));

  //
  // Insert Track Button
  //
  d_insert_button=new QPushButton(this);
  d_insert_button->setFont(buttonFont());
  d_insert_button->setText(tr("Insert\nTrack"));
  connect(d_insert_button,SIGNAL(clicked()),this,SLOT(insertData()));

  //
  // Delete Track Button
  //
  d_delete_button=new QPushButton(this);
  d_delete_button->setFont(buttonFont());
  d_delete_button->setText(tr("Delete\nTrack"));
  connect(d_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  setDisabled(true);
}


RDTrackerWidget::~RDTrackerWidget()
{
  for(int i=0;i<3;i++) {
    delete d_wpg[i];
    d_wpg[i]=NULL;
  }
  delete d_marker_dialog;
  delete d_log_lock;
  delete d_svc;
  delete d_log;
  delete d_group;
}


QSize RDTrackerWidget::sizeHint() const
{
  return QSize(715,645);
} 


QSizePolicy RDTrackerWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,
		     QSizePolicy::MinimumExpanding);
}


bool RDTrackerWidget::isActive() const
{
  return d_active;
}


void RDTrackerWidget::setFocusPolicy(Qt::FocusPolicy pol)
{
  d_focus_policy=pol;
  d_play_button->setFocusPolicy(pol);
  d_stop_button->setFocusPolicy(pol);
  d_track1_button->setFocusPolicy(pol);
  d_record_button->setFocusPolicy(pol);
  d_track2_button->setFocusPolicy(pol);
  d_finished_button->setFocusPolicy(pol);
  d_post_button->setFocusPolicy(pol);
  d_reset_button->setFocusPolicy(pol);
  d_previous_button->setFocusPolicy(pol);
  d_next_button->setFocusPolicy(pol);
  d_insert_button->setFocusPolicy(pol);
  d_delete_button->setFocusPolicy(pol);
  d_log_view->setFocusPolicy(pol);
}


bool RDTrackerWidget::load(const QString &logname)
{
  QString username;
  QString stationname;
  QHostAddress addr;
  
  //
  // Attempt to get a log lock
  //
  d_log_lock=new RDLogLock(logname,rda->user(),rda->station(),this);
  if(!d_log_lock->tryLock(&username,&stationname,&addr)) {
    QMessageBox::warning(this,"RDLogEdit - "+tr("Log Locked"),
			 tr("Log already being edited by")+" "+
			 username+"@"+stationname+" ["+
			 addr.toString()+"].");
    delete d_log_lock;
    d_log_lock=NULL;
    return false;
  }
  
  //
  // Voicetrack Group
  //
  d_log=new RDLog(logname);
  d_svc=new RDSvc(d_log->service(),rda->station(),rda->config());
  d_group=new RDGroup(d_svc->trackGroup());
  d_tracks=d_log->scheduledTracks()-d_log->completedTracks();
  d_log_model->setLogName(logname);
  d_log_model->setServiceName(d_log->service());
  d_log_model->load(true);

  //
  // Select first track
  //
  for(int i=0;i<d_log_model->lineCount();i++) {
    if(d_log_model->logLine(i)->type()==RDLogLine::Track) {
      d_track_line=i;
      d_loaded=true;
      LoadTrack(d_track_line);
      LoadBlockLength(d_track_line);
      d_log_view->selectRow(i);
      i=d_log_model->lineCount();
    }
  }
  UpdateControls();
  UpdateRemaining();

  if(d_group->name().isEmpty()) {
    QMessageBox::warning(this,tr("No VoiceTrack Group"),
			 tr("No voicetracking group has been defined for this service,\ntherefore only existing transitions will be editable."));
  }

  setEnabled(true);

  return true;
}


void RDTrackerWidget::unload()
{
  stopData();
  CheckChanges();
  if(d_size_altered) {
    d_log_model->save(rda->config());
  }
  d_log_model->clear();
  if(d_log_lock!=NULL) {
    delete d_log_lock;
    d_log_lock=NULL;
  }
  if(d_group!=NULL) {
    delete d_group;
    d_group=NULL;
  }
  if(d_svc!=NULL) {
    delete d_svc;
    d_svc=NULL;
  }
  if(d_log!=NULL) {
    delete d_log;
    d_log=NULL;
  }
  
  d_tracks=0;
  LoadTrack(-1);
  setDisabled(true);
}


void RDTrackerWidget::updateMenuData()
{
  if(!d_wave_name[d_rightclick_track].isEmpty()) {
    if(d_rightclick_track==1 && d_loaded) {
      d_edit_cue_action->setEnabled(false);
    }
    else {
      d_edit_cue_action->setEnabled(true);
    }
  }  
  else {
    d_edit_cue_action->setEnabled(false);
  }
  d_undo_segue_action->setEnabled(d_changed);
  d_set_start_action->setEnabled(!d_wave_name[d_rightclick_track].isEmpty());
  d_set_end_action->setEnabled(!d_wave_name[d_rightclick_track].isEmpty());

  if(!d_wave_name[d_rightclick_track].isEmpty()) {
    RDCut hook_cut=RDCut(d_loglines[d_rightclick_track]->cartNumber(),
                d_loglines[d_rightclick_track]->cutNumber());
    if(hook_cut.hookStartPoint()>=0 && hook_cut.hookEndPoint()>=0) {
      d_set_hook_action->setEnabled(true);
    }
    else {
      d_set_hook_action->setEnabled(false);
    }
  }
  else {
    d_set_hook_action->setEnabled(false);
  }
  d_menu_clicked_point=d_rightclick_pos;
  DrawTrackMap(d_rightclick_track);
  update();
}


void RDTrackerWidget::hideMenuData()
{
  d_menu_clicked_point=-1;
  DrawTrackMap(d_rightclick_track);
  update();
}


void RDTrackerWidget::playData()
{
  if(TransportActive()) {
    return;
  }
  d_redraw_count=0;
  for(int i=0;i<3;i++) {
    d_redraw[i]=false;
  }
  int start=
    d_wave_origin[0]-d_loglines[0]->startPoint();
  if(start<0) {
    start=0;
  }
  if((start<=(d_loglines[0]->segueEndPoint()-d_loglines[0]->startPoint()))&&
     (!d_wave_name[0].isEmpty())) {
    if(start>(d_loglines[0]->segueStartPoint()-d_loglines[0]->startPoint())) {
      d_segue_start_offset[1]=start-
	(d_loglines[0]->segueStartPoint()-d_loglines[0]->startPoint());
      if(d_wave_name[1].isEmpty()) {
        d_segue_start_offset[2]=d_segue_start_offset[1];
      }
      else {
	if(start>(d_loglines[1]->segueStartPoint()-
		  d_loglines[1]->startPoint())) {
	  d_segue_start_offset[2]=d_segue_start_offset[1]-
	    d_loglines[1]->segueStartPoint()-d_loglines[1]->startPoint();
	}
	else {
	  d_segue_start_offset[2]=0;
	}
      }
    }
    else {
      d_segue_start_offset[1]=0;
    }
    d_deck[0]->setCart(d_loglines[0],false);
    d_deck[0]->play(start,d_loglines[0]->segueStartPoint(),
		    d_loglines[0]->segueEndPoint());
  }
  else {
    if(d_wave_name[1].isEmpty()) {
      start=d_wave_origin[2]-
	d_loglines[2]->startPoint();
      if(start<=(d_loglines[2]->segueEndPoint()-d_loglines[2]->startPoint())) {
	start=d_wave_origin[2]-d_loglines[2]->
	  startPoint(RDLogLine::CartPointer);
	if(start<0) {
	  start=0;
	}
      }
      d_deck[2]->setCart(d_loglines[2],false);
      d_deck[2]->play(start,d_loglines[2]->segueStartPoint(),
		      d_loglines[2]->segueEndPoint());
    }
    else {
      start=d_wave_origin[1]-
	d_loglines[1]->startPoint();
      if(start<=(d_loglines[1]->segueEndPoint()-d_loglines[1]->startPoint())) {
	if(start<0) {
	  start=0;
	}
	if(start>(d_loglines[1]->segueStartPoint()-
		  d_loglines[1]->startPoint())) {
	  d_segue_start_offset[2]=start-(d_loglines[1]->segueStartPoint()-
					 d_loglines[1]->startPoint());
	}
	else {
	  d_segue_start_offset[2]=0;
	}
	d_deck[1]->setCart(d_loglines[1],false);
	d_deck[1]->play(start,d_loglines[1]->segueStartPoint(),
			d_loglines[1]->segueEndPoint());
      }
      else {
	start=d_wave_origin[2]-d_loglines[2]->startPoint();
	if(start<=(d_loglines[2]->segueEndPoint()-
		   d_loglines[2]->startPoint())) {
	  start=d_wave_origin[2]-d_loglines[2]->startPoint();
	  if(start<0) {
	    start=0;
	  }
	}
	d_deck[2]->setCart(d_loglines[2],false);
	d_deck[2]->play(start,d_loglines[2]->segueStartPoint(),
			d_loglines[2]->segueEndPoint());
      }
    }
  }
}


void RDTrackerWidget::stopData()
{
  d_deck[0]->stop();
  d_deck[1]->stop();
  d_deck[2]->stop();
}


void RDTrackerWidget::track1Data()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }
  if(d_track1_button->text()==tr("Import")) {
    if(!ImportTrack(line)) {
      QMessageBox::warning(this,tr("Cart Creation Failure"),
			   tr("Unable to create new cart for voice track!"));
      return;
    }
    UpdateRemaining();
    UpdateControls();
    return;
  }
  if(!InitTrack()) {
    QMessageBox::warning(this,tr("Cart Creation Failure"),
			 tr("Unable to create new cart for voice track!"));
    return;
  }
  rda->cae()->loadRecord(d_input_card,d_input_port,d_track_cuts[1]->cutName(),
			 d_coding,d_chans,d_samprate,d_bitrate);
  playData();
  UpdateControls();
}


void RDTrackerWidget::recordData()
{
  int line;

  if(d_deck_state==RDTrackerWidget::DeckIdle) {
    if((line=SingleSelectionLine())<0) {
      return;
    }
    if(d_record_button->text()==tr("Import")) {
      if(!ImportTrack(line)) {
	QMessageBox::warning(this,tr("Cart Creation Failure"),
			     tr("Unable to create new cart for voice track!"));
	return;
      }
      UpdateRemaining();
      UpdateControls();
      return;
    }
    if(!InitTrack()) {
      QMessageBox::warning(this,tr("Cart Creation Failure"),
			   tr("Unable to create new cart for voice track!"));
      return;
    }
    rda->cae()->loadRecord(d_input_card,d_input_port,
			   d_track_cuts[1]->cutName(),
			   d_coding,d_chans,d_samprate,d_bitrate);
    d_sliding=true;
  }
  if(!d_sliding) {
    d_wave_origin[2]=d_wave_origin[0]-d_deck[0]->currentPosition()-
      d_loglines[0]->startPoint()+d_loglines[2]->startPoint();
    DrawTrackMap(2);
    d_sliding=true;
  }
  d_event_player->exec(d_record_start_macro);
  d_wave_name[1]=RDCut::pathName(d_track_cuts[1]->cutName());
  d_wpg[1]=new RDWavePainter(d_wave_map[1],d_track_cuts[1],
			     rda->station(),rda->user(),rda->config());
  d_wpg[1]->end();
  rda->cae()->record(d_input_card,d_input_port,0,0);
  d_record_ran=true;
  d_record_start_time=GetCurrentTime();
  if(d_deck_state==RDTrackerWidget::DeckTrack1) {
    d_time_remaining+=d_deck[0]->currentPosition()+d_loglines[0]->startPoint()-
      d_loglines[0]->segueStartPoint();
  }
  d_time_remaining_start=d_time_remaining;
  d_deck[0]->duckDown(d_loglines[0]->endPoint()-d_loglines[0]->startPoint()-
		      d_deck[0]->currentPosition());
  d_deck_state=RDTrackerWidget::DeckTrack2;
  d_start_time=d_log_model->blockStartTime(d_track_line);
  if(!d_wave_name[0].isEmpty() && d_start_time>QTime(0,0,0)){
    d_start_time=
      d_start_time.addMSecs(-d_loglines[0]->segueLength(RDLogLine::Segue));
    d_start_time=d_start_time.addMSecs(d_deck[0]->currentPosition());
  }
  DrawTrackMap(1);
  UpdateControls();
}


void RDTrackerWidget::track2Data()
{
  if((d_deck_state!=RDTrackerWidget::DeckTrack1)&&
     (d_deck_state!=RDTrackerWidget::DeckTrack2)) {
    return;
  }
  if(!d_sliding) {
    d_wave_origin[2]=d_wave_origin[0]-d_deck[0]->currentPosition();
    DrawTrackMap(2);
  }
  else {
    d_sliding=false;
  }
  if(d_wave_name[1].isEmpty()) {
    d_segue_start_point[0]=d_deck[0]->currentPosition();
  }
  else {
    d_segue_start_point[1]=d_recording_pos;
  }
  d_deck_state=RDTrackerWidget::DeckTrack3;

  int new_end=d_deck[0]->currentPosition()+d_loglines[0]->startPoint()+
    TRACKER_FORCED_SEGUE;
  if(!d_wave_name[0].isEmpty() && 
      d_loglines[0]->endPoint()>new_end && 
      d_deck[0]->state()==RDPlayDeck::Playing) {
    if(d_loglines[0]->fadedownPoint()>(new_end-TRACKER_FORCED_SEGUE) ||
       d_loglines[0]->fadedownGain()==0) { 
      d_loglines[0]->setFadedownPoint(new_end-TRACKER_FORCED_SEGUE,
				      RDLogLine::LogPointer);
      d_loglines[0]->setEndPoint(new_end,RDLogLine::LogPointer);
      d_loglines[0]->setSegueEndPoint(new_end,RDLogLine::LogPointer);
      d_loglines[0]->setFadedownGain(RD_FADE_DEPTH);
      d_deck[0]->stop(TRACKER_FORCED_SEGUE,RD_FADE_DEPTH);
    }                               
  }

  StartNext(0,2);
  UpdateControls();
}


void RDTrackerWidget::finishedData()
{
  switch(d_deck_state) {
  case RDTrackerWidget::DeckIdle:
    if(d_changed) {
      SaveTrack(d_track_line);
    }
    break;
	
  case RDTrackerWidget::DeckTrack1:
    d_aborting=true;
    stopData();
    rda->cae()->unloadRecord(d_input_card,d_input_port);
    d_deck_state=RDTrackerWidget::DeckIdle;
    resetData();
    break;
	
  case RDTrackerWidget::DeckTrack2:
    if(rda->logeditConf()->enableSecondStart()) {
      if(d_wave_name[2].isEmpty()||
	 ((d_loglines[2]->transType()!=RDLogLine::Segue))) {
	FinishTrack();
      }
      else {
	d_aborting=true;
	stopData();
	rda->cae()->stopRecord(d_input_card,d_input_port);
	d_deck_state=RDTrackerWidget::DeckIdle;
	resetData();
      }
    }
    else {
      FinishTrack();
    }
    break;

  case RDTrackerWidget::DeckTrack3:
    FinishTrack();
    break;
  }
  UpdateRemaining();
  UpdateControls();
}


void RDTrackerWidget::postData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }

  if(d_wave_name[2].isEmpty()) {
    return;
  }
  if(!d_changed) {
    PushSegues();
    d_changed=true;
  }
  int talk_end=d_loglines[2]->talkEndPoint();
  if(talk_end<d_loglines[2]->startPoint()) {
    talk_end=d_loglines[2]->startPoint();
  }
  int segue_start=d_loglines[1]->endPoint()-d_loglines[1]->startPoint()-
    (talk_end-d_loglines[2]->startPoint());
  if(segue_start<d_loglines[1]->startPoint()) {
    segue_start=d_loglines[1]->startPoint()+10;
  }
  if(segue_start>d_loglines[1]->endPoint()) {
    segue_start=d_loglines[1]->endPoint()-10;
  }
  d_loglines[1]->setSegueStartPoint(segue_start,RDLogLine::LogPointer);
  d_loglines[1]->setSegueGain(0);
  d_loglines[1]->
    setAverageSegueLength(d_loglines[1]->segueStartPoint()-
			  d_loglines[1]->startPoint());
  d_wave_origin[2]=d_wave_origin[1]-
    (segue_start-d_loglines[1]->startPoint());

  RenderTransition(line);
  UpdateControls();
  UpdateRemaining();
}


void RDTrackerWidget::resetData()
{
  if(d_loaded&&(d_loglines[1]->type()==RDLogLine::Cart)) {
    if(d_track_cart!=NULL) {
      delete d_track_cart;
    }
    d_track_cart=new RDCart(d_loglines[1]->cartNumber());
    d_loglines[1]->setCartNumber(0);
    d_loglines[1]->setType(RDLogLine::Track);
    d_loglines[1]->setSource(RDLogLine::Manual);
    d_loglines[1]->setOriginUser("");
    d_loglines[1]->setOriginDateTime(QDateTime());
    d_loglines[1]->setMarkerComment(d_track_cart->title());
    d_loglines[1]->setForcedLength(0);
    d_loglines[1]->clearTrackData(RDLogLine::AllTrans);
    d_log_model->removeCustomTransition(d_track_lines[1]);
    if(!d_track_cart->remove(rda->station(),rda->user(),rda->config())) {
      QMessageBox::warning(this,tr("RDLogEdit"),tr("Audio Deletion Error!"));
    }
    SendNotification(RDNotification::DeleteAction,d_track_cart->number());
    delete d_track_cart;
    d_track_cart=NULL;
    if(d_track_cuts[1]!=NULL) {
      delete d_track_cuts[1];
      d_track_cuts[1]=NULL;
    }
    d_wave_name[1]="";
    delete d_wpg[1];
    d_wpg[1]=NULL;
    if(!d_wave_name[2].isEmpty()) {
      d_log_model->removeCustomTransition(d_track_lines[2]);
    }
  }
  else {
    d_log_model->removeCustomTransition(d_track_lines[1]);
  }
  SaveTrack(d_track_line);
  LoadTrack(d_track_line);
  for(int i=0;i<3;i++) {
    DrawTrackMap(i);
  }

  int real_line=SingleSelectionLine();
  int line=-1;
  if(d_offset) {
    line=real_line-1;
  }
  else {
    line=real_line;
  }
  if(line<0) {
    return;
  }
  if(d_loaded) {
    d_log_model->update(line);
    d_tracks++;
  }
  LoadBlockLength(d_track_line);
  RefreshLine(real_line);
  line++;
  if((real_line!=line)&&(line>=0)) {
    RefreshLine(line);
  }
  UpdateRemaining();
  UpdateControls();
}


void RDTrackerWidget::insertData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }
  SaveTrack(line);
  if(line==TRACKER_MAX_LINENO) {
    line=d_log_model->lineCount();
  }
  d_log_model->insert(line,1,true);
  d_log_model->logLine(line)->setType(RDLogLine::Track);
  d_log_model->logLine(line)->setTransType(RDLogLine::Segue);
  d_log_model->logLine(line)->setMarkerComment(tr("Voice Track"));
  RDTrackerEditDialog *edit=
    new RDTrackerEditDialog(d_log_model->logLine(line),this);
  if(edit->exec()>=0) {
    RefreshLine(line);
    d_log_view->selectRow(line);
    d_tracks++;
    d_size_altered=true;
    d_track_line=-1;
    UpdateRemaining();
    UpdateControls();
  }
  else {
    d_log_model->remove(line,1);
  }
  QItemSelection select(d_log_model->index(line,0),
			d_log_model->index(line,d_log_model->columnCount()-1));
  selectionChangedData(select,select);
  delete edit;
}


void RDTrackerWidget::insertData(int line,RDLogLine *logline,bool warn)
{
  SaveTrack(line);
  d_log_model->insert(line,1,true);
  *d_log_model->logLine(line)=*logline;
  d_log_model->insert(line,1);
  d_log_model->logLine(line)->setType(RDLogLine::Track);
  d_log_model->logLine(line)->setTransType(RDLogLine::Segue);
  d_log_model->logLine(line)->setMarkerComment(tr("Voice Track"));
  d_log_model->update(line);

  d_tracks++;
  d_size_altered=true;
  d_track_line=-1;
  d_changed=false;
  UpdateRemaining();
  UpdateControls();
}


void RDTrackerWidget::deleteData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }
  if(d_offset) {
    line--;
  }
  deleteData(line,true);
}


void RDTrackerWidget::deleteData(int line,bool warn)
{
  SaveTrack(line);
  d_log_view->selectRow(line+1);
  d_log_model->remove(line,1);
  d_track_line=-1;
  d_tracks--;
  d_size_altered=true;
  UpdateControls();
  UpdateRemaining();
}


void RDTrackerWidget::previousData()
{
  RDLogLine *ll=NULL;
  int line=SingleSelectionLine();

  for(int i=line-1;i>=0;i--) {
    if((ll=d_log_model->logLine(i))!=NULL) {
      if((ll->type()==RDLogLine::Track)||(ll->source()==RDLogLine::Tracker)) {
	d_log_view->selectRow(i);
	d_loaded=true;
	return;
      }
    }
  }
  QMessageBox::information(this,tr("Track List"),tr("No more tracks!"));
}


void RDTrackerWidget::nextData()
{
  RDLogLine *ll=NULL;
  int line=SingleSelectionLine();

  for(int i=line+1;i<d_log_model->lineCount();i++) {
    if((ll=d_log_model->logLine(i))!=NULL) {
      if((ll->type()==RDLogLine::Track)||(ll->source()==RDLogLine::Tracker)) {
	d_log_view->selectRow(i);
	d_loaded=true;
	return;
      }
    }
  }
  QMessageBox::information(this,tr("Track List"),tr("No more tracks!"));
}


void RDTrackerWidget::editAudioData()
{
  RDCart *rdcart=new RDCart(d_loglines[d_rightclick_track]->cartNumber());

  if(d_marker_dialog->
     exec(RDCut::cartNumber(d_loglines[d_rightclick_track]->cutName()),
	  RDCut::cutNumber(d_loglines[d_rightclick_track]->cutName()),
	  !rda->user()->editAudio())) {
    rdcart->updateLength();
    d_loglines[d_rightclick_track]->refreshPointers();
    if(d_loglines[d_rightclick_track]->fadeupPoint()<
       d_loglines[d_rightclick_track]->startPoint() &&
       d_loglines[d_rightclick_track]->fadeupPoint()>=0) {
      d_loglines[d_rightclick_track]->
	setFadeupPoint(d_loglines[d_rightclick_track]->startPoint(),
		       RDLogLine::LogPointer);
    }    
    if(d_loglines[d_rightclick_track]->fadeupPoint()>
       d_loglines[d_rightclick_track]->endPoint()) {
      d_loglines[d_rightclick_track]->
	setFadeupPoint(d_loglines[d_rightclick_track]->endPoint(),
		       RDLogLine::LogPointer);
    }    
    if(d_loglines[d_rightclick_track]->fadedownPoint()<
       d_loglines[d_rightclick_track]->startPoint() &&
       d_loglines[d_rightclick_track]->fadedownPoint()>=0) {
      d_loglines[d_rightclick_track]->
	setFadedownPoint(d_loglines[d_rightclick_track]->startPoint(),
			 RDLogLine::LogPointer);
    }    
    if(d_loglines[d_rightclick_track]->fadedownPoint()>
       d_loglines[d_rightclick_track]->endPoint()) {
      d_loglines[d_rightclick_track]->
	setFadedownPoint(d_loglines[d_rightclick_track]->endPoint(),
			 RDLogLine::LogPointer);
    }    
    DrawTrackMap(d_rightclick_track);
    update();
  }

  delete rdcart;
}


void RDTrackerWidget::setStartPointData()
{
  if(!d_changed) {
    PushSegues();
    d_changed=true;
  }
    
  int fadeup_diff=d_loglines[d_rightclick_track]->fadeupPoint()-
    d_loglines[d_rightclick_track]->startPoint();

  int newpoint=d_rightclick_pos*TRACKER_MSECS_PER_PIXEL+
    d_wave_origin[d_rightclick_track];
  if(newpoint<0) {
    newpoint=0;
  }
  if(newpoint>d_loglines[d_rightclick_track]->endPoint()) {
    newpoint=d_loglines[d_rightclick_track]->endPoint();
  }
  d_loglines[d_rightclick_track]->
    setStartPoint(newpoint,RDLogLine::LogPointer);

  if(d_loglines[d_rightclick_track]->endPoint(RDLogLine::LogPointer)<0) {
    d_loglines[d_rightclick_track]->
      setEndPoint(d_loglines[d_rightclick_track]->
		  endPoint(RDLogLine::CartPointer),
		  RDLogLine::LogPointer);
  }
  d_loglines[d_rightclick_track]->
    setFadeupPoint(d_loglines[d_rightclick_track]->startPoint()+
		   fadeup_diff,RDLogLine::LogPointer);
  if(d_loglines[d_rightclick_track]->fadeupPoint(RDLogLine::CartPointer)>
     d_loglines[d_rightclick_track]->fadeupPoint()) {
    d_loglines[d_rightclick_track]->
      setFadeupPoint(d_loglines[d_rightclick_track]->
		     fadeupPoint(RDLogLine::CartPointer),RDLogLine::LogPointer);
  }

  if(d_loglines[d_rightclick_track]->fadeupPoint()<
     d_loglines[d_rightclick_track]->startPoint()) {
    d_loglines[d_rightclick_track]->
      setFadeupPoint(d_loglines[d_rightclick_track]->startPoint(),
		     RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->fadeupPoint()>
     d_loglines[d_rightclick_track]->endPoint()) {
    d_loglines[d_rightclick_track]->
      setFadeupPoint(d_loglines[d_rightclick_track]->endPoint(),
		     RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->fadedownPoint()<
     d_loglines[d_rightclick_track]->startPoint() &&
     d_loglines[d_rightclick_track]->fadedownPoint()>=0) {
    d_loglines[d_rightclick_track]->
      setFadedownPoint(d_loglines[d_rightclick_track]->startPoint(),
		       RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->segueStartPoint()<
     d_loglines[d_rightclick_track]->startPoint() &&
     d_loglines[d_rightclick_track]->segueStartPoint()>=0) {
    d_loglines[d_rightclick_track]->
      setSegueStartPoint(d_loglines[d_rightclick_track]->startPoint(),
			 RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->fadeupPoint(RDLogLine::CartPointer)>=0) {
    d_loglines[d_rightclick_track]->setFadeupGain(RD_FADE_DEPTH);    
  }
  d_loglines[d_rightclick_track]->
    setForcedLength(d_loglines[d_rightclick_track]->endPoint()-
		    d_loglines[d_rightclick_track]->startPoint());
    
  d_loglines[d_rightclick_track]->setHasCustomTransition(true);

  RenderTransition(SingleSelectionLine());
  UpdateControls();
  UpdateRemaining();
}


void RDTrackerWidget::setEndPointData()
{
  if(!d_changed) {
    PushSegues();
    d_changed=true;
  }

  int fadedown_diff=d_loglines[d_rightclick_track]->endPoint()-
    d_loglines[d_rightclick_track]->fadedownPoint();

  int newpoint=d_rightclick_pos*TRACKER_MSECS_PER_PIXEL+
    d_wave_origin[d_rightclick_track];
  if(newpoint>
     d_loglines[d_rightclick_track]->endPoint(RDLogLine::CartPointer)) {
    newpoint=d_loglines[d_rightclick_track]->endPoint(RDLogLine::CartPointer);
  }
  if(newpoint<
     d_loglines[d_rightclick_track]->startPoint()) {
    newpoint=d_loglines[d_rightclick_track]->startPoint();
  }
  d_loglines[d_rightclick_track]->
    setEndPoint(newpoint,RDLogLine::LogPointer);

  if(d_loglines[d_rightclick_track]->startPoint(RDLogLine::LogPointer)<0) {
    d_loglines[d_rightclick_track]->
      setStartPoint(d_loglines[d_rightclick_track]->
		    startPoint(RDLogLine::CartPointer),RDLogLine::LogPointer);
  }

  d_loglines[d_rightclick_track]->
    setFadedownPoint(d_loglines[d_rightclick_track]->endPoint()-
		     fadedown_diff,RDLogLine::LogPointer);
  if(d_loglines[d_rightclick_track]->fadedownPoint(RDLogLine::CartPointer)>=0
     && d_loglines[d_rightclick_track]->fadedownPoint(RDLogLine::CartPointer)<
     d_loglines[d_rightclick_track]->fadedownPoint()) {
    d_loglines[d_rightclick_track]->
      setFadedownPoint(d_loglines[d_rightclick_track]->
		       fadedownPoint(RDLogLine::CartPointer),
		       RDLogLine::LogPointer);
  }

  if(d_loglines[d_rightclick_track]->fadedownPoint()>
     d_loglines[d_rightclick_track]->endPoint()) {
    d_loglines[d_rightclick_track]->
      setFadedownPoint(d_loglines[d_rightclick_track]->endPoint(),
		       RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->segueStartPoint()>
     d_loglines[d_rightclick_track]->endPoint()) {
    d_loglines[d_rightclick_track]->
      setSegueStartPoint(d_loglines[d_rightclick_track]->endPoint(),
			 RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->fadeupPoint()>
     d_loglines[d_rightclick_track]->endPoint()) {
    d_loglines[d_rightclick_track]->
      setFadeupPoint(d_loglines[d_rightclick_track]->endPoint(),
		     RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->segueStartPoint()<
     d_loglines[d_rightclick_track]->startPoint() &&
     d_loglines[d_rightclick_track]->segueStartPoint()>=0) {
    d_loglines[d_rightclick_track]->
      setSegueStartPoint(d_loglines[d_rightclick_track]->startPoint(),
			 RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->fadedownPoint()<
     d_loglines[d_rightclick_track]->startPoint() &&
     d_loglines[d_rightclick_track]->fadedownPoint()>=0) {
    d_loglines[d_rightclick_track]->
      setFadedownPoint(d_loglines[d_rightclick_track]->startPoint(),
		       RDLogLine::LogPointer);
  }

  if(d_loglines[d_rightclick_track]->fadedownPoint(RDLogLine::CartPointer)>=0) {
    d_loglines[d_rightclick_track]->setFadedownGain(RD_FADE_DEPTH);    
  }

  d_loglines[d_rightclick_track]->
    setSegueEndPoint(d_loglines[d_rightclick_track]->endPoint(),
		     RDLogLine::LogPointer);
  d_loglines[d_rightclick_track]->
    setForcedLength(d_loglines[d_rightclick_track]->endPoint()-
		    d_loglines[d_rightclick_track]->startPoint());
  
  if(d_loglines[d_rightclick_track+1]!=NULL) {
    d_loglines[d_rightclick_track+1]->setHasCustomTransition(true);
  }
  RenderTransition(SingleSelectionLine());
  UpdateControls();
  UpdateRemaining();
}


void RDTrackerWidget::setHookPointData()
{
  if(!d_changed) {
    PushSegues();
    d_changed=true;
  }

  RDCut hook_cut=RDCut(d_loglines[d_rightclick_track]->cartNumber(),
		       d_loglines[d_rightclick_track]->cutNumber());
  if(hook_cut.hookStartPoint()>=0 && hook_cut.hookEndPoint()>=0 &&
     hook_cut.hookStartPoint()<hook_cut.hookEndPoint()) {
    d_loglines[d_rightclick_track]->
      setStartPoint(hook_cut.hookStartPoint(),RDLogLine::LogPointer);
    d_loglines[d_rightclick_track]->
      setFadeupPoint(hook_cut.hookStartPoint(),RDLogLine::LogPointer);
    d_loglines[d_rightclick_track]->
      setEndPoint(hook_cut.hookEndPoint(),RDLogLine::LogPointer);
    d_loglines[d_rightclick_track]->
      setSegueEndPoint(hook_cut.hookEndPoint(),RDLogLine::LogPointer);
    d_loglines[d_rightclick_track]->
      setSegueStartPoint(hook_cut.hookEndPoint()-500,RDLogLine::LogPointer);
    d_loglines[d_rightclick_track]->
      setFadedownPoint(hook_cut.hookEndPoint()-500,RDLogLine::LogPointer);
    d_loglines[d_rightclick_track]->setFadedownGain(RD_FADE_DEPTH);
    }
    else {
      return;
    }

  if(d_loglines[d_rightclick_track]->endPoint()>
     d_loglines[d_rightclick_track]->endPoint(RDLogLine::CartPointer)) {
    d_loglines[d_rightclick_track]->
      setEndPoint(d_loglines[d_rightclick_track]->
		  endPoint(RDLogLine::CartPointer),RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->endPoint()<
     d_loglines[d_rightclick_track]->startPoint()) {
    d_loglines[d_rightclick_track]->
      setEndPoint(d_loglines[d_rightclick_track]->startPoint(),
		  RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->segueStartPoint()<
     d_loglines[d_rightclick_track]->startPoint() &&
     d_loglines[d_rightclick_track]->segueStartPoint()>=0) {
    d_loglines[d_rightclick_track]->
      setSegueStartPoint(d_loglines[d_rightclick_track]->startPoint(),
			 RDLogLine::LogPointer);
  }
  if(d_loglines[d_rightclick_track]->fadedownPoint()<
     d_loglines[d_rightclick_track]->startPoint() &&
     d_loglines[d_rightclick_track]->fadedownPoint()>=0) {
    d_loglines[d_rightclick_track]->
      setFadedownPoint(d_loglines[d_rightclick_track]->startPoint(),
		       RDLogLine::LogPointer);
  }
  d_loglines[d_rightclick_track]->
    setForcedLength(d_loglines[d_rightclick_track]->endPoint()-
		    d_loglines[d_rightclick_track]->startPoint());
    
  d_loglines[d_rightclick_track]->setHasCustomTransition(true);
  if(d_loglines[d_rightclick_track+1]!=NULL) {
    d_loglines[d_rightclick_track+1]->setHasCustomTransition(true);
  }  
  RenderTransition(SingleSelectionLine());
  UpdateControls();
  UpdateRemaining();
}


void RDTrackerWidget::undoChangesData()
{
  PopSegues();
  d_changed=false;
  int line=SingleSelectionLine();
  RenderTransition(line);
  RefreshLine(line);
  if(line<(d_log_model->lineCount()-1)) {
    RefreshLine(line+1);
  }
}


void RDTrackerWidget::stateChangedData(int id,RDPlayDeck::State state)
{
  // printf("stateChangeData(%d,%d)\n",id,state);

  if(state==RDPlayDeck::Playing) {
    d_play_button->on();
    d_stop_button->off();
    d_event_player->exec(d_play_start_macro);
  }
  else {
    if((d_deck_state==RDTrackerWidget::DeckTrack1)&&
       (state==RDPlayDeck::Finished)&&(!d_record_ran)) {
      finishedData();
      return;
    }
    else {
      for(int i=0;i<3;i++) {
	if(d_deck[i]->state()==RDPlayDeck::Playing) {
	  return;
	}
      }
      if(d_recording) {
	return;
      }
      d_play_button->off();
      d_stop_button->on();
      d_event_player->exec(d_play_end_macro);
      d_deck_state=RDTrackerWidget::DeckIdle;
      positionData(id,-1);
    }
  }
  switch(state) {
  case RDPlayDeck::Playing:
  case RDPlayDeck::Stopping:
    if(!d_meter_timer->isActive()) {
      d_meter_timer->start(RD_METER_UPDATE_INTERVAL);
    }
    UpdateControls();
    break;

  default:
    if(!TransportActive()) {
      d_meter_timer->stop();
      d_meter->setLeftPeakBar(-10000);
      d_meter->setRightPeakBar(-10000);
      d_scrolling=false;
      for(unsigned i=0;i<3;i++) {
	d_scroll_pos[i]=-1;
	d_segue_start_offset[i]=0;
      }
    }
    RenderTransition(SingleSelectionLine());
    UpdateControls();
    break;
  }
  if((d_deck[0]->state()!=RDPlayDeck::Stopped)||
     (d_deck[1]->state()!=RDPlayDeck::Stopped)||
     (d_deck[2]->state()!=RDPlayDeck::Stopped)!=d_active) {
    d_active=(d_deck[0]->state()!=RDPlayDeck::Stopped)||
      (d_deck[1]->state()!=RDPlayDeck::Stopped)||
      (d_deck[2]->state()!=RDPlayDeck::Stopped);
    emit activeChanged(d_active);
  }
}


void RDTrackerWidget::positionData(int id,int msecs)
{
  int edit_scroll_diff;
  if(msecs<=0) {
    d_cursor_xpos=-1;
    update();
    return;
  }

  if((id==1)&&d_recording) {
    d_length_label->setText(RDGetTimeLength(msecs,true));
    d_time_counter=d_time_remaining_start-msecs;
    if(d_deck_state==RDTrackerWidget::DeckTrack2) {
      d_time_remaining=
	d_time_remaining_start-msecs;
      UpdateRemaining();
    }
  }
  for(int i=id-1;i>=0;i--) {
    if(d_deck[i]->state()==RDPlayDeck::Playing) {
      update();
      return;
    }
  }
  if(d_scroll_pos[id]>=0)
    edit_scroll_diff=msecs-d_scroll_pos[id];
  else
    edit_scroll_diff=0;
  d_scroll_pos[id]=msecs;
  msecs+=d_loglines[id]->startPoint();
  switch(d_deck_state) {
  case RDTrackerWidget::DeckIdle:
    break;

  case RDTrackerWidget::DeckTrack1:
    if(d_sliding) {
      d_wave_origin[0]=d_wave_origin[2]-
	d_loglines[2]->startPoint()+msecs;
      d_redraw[0]=true;
    }
    break;

  case RDTrackerWidget::DeckTrack2:
    switch(id) {
    case 0:
      if(!d_scrolling) {
	d_wave_origin[0]=d_wave_origin[2]-
	  d_loglines[2]->startPoint()+msecs;
      }
      d_wave_origin[1]=d_wave_origin[0]-
	d_segue_start_point[0];
      break;
	      
    case 1:
      if(!d_scrolling) {
	d_wave_origin[1]=d_wave_origin[2]+msecs-
	  d_loglines[2]->startPoint();
      }
      d_wave_origin[0]=
	d_wave_origin[1]+d_segue_start_point[0];
      break;
    }
    d_redraw[0]=true;
    d_redraw[1]=true;
    break;

  case RDTrackerWidget::DeckTrack3:
    if((id==0)||(id==1)) {
      d_redraw[0]=true;
      d_redraw[1]=true;
    }
    else {
      update();
      return;
    }
    break;
  }

  if(d_scrolling&&(edit_scroll_diff>=0)) {
    d_wave_origin[0]+=edit_scroll_diff;
    d_redraw[0]=true;
    if(!d_wave_name[1].isEmpty()) {
      d_wave_origin[1]+=edit_scroll_diff;
      d_redraw[1]=true;
    }
    if(!d_sliding) {
      d_wave_origin[2]+=edit_scroll_diff;
      d_redraw[2]=true;
    }
  }
  if(d_recording && (d_deck[2]->state()==RDPlayDeck::Playing ||
          d_deck[2]->state()==RDPlayDeck::Stopping)) {
  d_redraw[2]=true;
  }
  if(d_redraw_count++==TRACKER_SCROLL_SCALE) {
    for(unsigned i=0;i<3;i++) {
      if(d_redraw[i]) {
	DrawTrackMap(i);
	d_redraw[i]=false;
      }
    }
    d_redraw_count=0;
  }

  int x=-1;
  if(msecs>=0) {
    if((msecs>=d_wave_origin[id])&&
       (msecs<(d_wave_origin[id]+d_time_width))) {
      x=(int)(((double)(msecs-d_wave_origin[id]))*
	      ((double)(d_wave_map[id]->size().width()))/
	      ((double)size().width()*(double)TRACKER_START_WIDTH/800.0))+10;
      if(x>d_scroll_threshold) {
	d_scrolling=true;
      }
    }
  }
  d_cursor_xpos=x;

  update();
}


void RDTrackerWidget::segueStartData(int id)
{
  switch(d_deck_state) {
  case RDTrackerWidget::DeckIdle:
    for(int i=id+1;i<3;i++) {
      if(!d_wave_name[i].isEmpty()) {
	if(d_loglines[i]->transType()==RDLogLine::Stop) {
	  stopData();
	}
	else {
	  StartNext(id);
	}
	return;
      }
    }
    break;

  case RDTrackerWidget::DeckTrack1:
    d_sliding=true;
    if(!d_wave_name[2].isEmpty()) {
      d_wave_origin[2]=
	d_wave_origin[0]-d_deck[0]->currentPosition()-
	d_loglines[0]->startPoint()+
	d_loglines[2]->startPoint();
    }
    break;

  case RDTrackerWidget::DeckTrack2:
    break;

  case RDTrackerWidget::DeckTrack3:
    break;
  }
}


void RDTrackerWidget::selectionChangedData(const QItemSelection &selected,
					const QItemSelection &deselected)
{
  CheckChanges();
  if(selected.indexes().size()==0) {
    d_loaded=false;
    d_segue_loaded=false;
    d_length_label->setText("-:--:--.-");
    return;
  }
  if(selected.indexes().size()>=1) {
    RefreshLine(selected.indexes().at(0).row());
    RenderTransition(selected.indexes().at(0).row());
  }
}


void RDTrackerWidget::meterData()
{
  double ratio[2]={0.0000000001,0.0000000001};
  short level[2];

  if(d_recording) {
    d_recording_pos=
      (int)((GetCurrentTime()-d_record_start_time)*1000.0);
    positionData(1,d_recording_pos);
  }

  for(int i=0;i<3;i++) {
    if((d_deck[i]->state()==RDPlayDeck::Playing)||
       (d_deck[i]->state()==RDPlayDeck::Stopping)) {
	positionData(i,d_deck[i]->currentPosition());
      rda->cae()->
	outputMeterUpdate(d_deck[i]->card(),d_deck[i]->port(),level);
      for(int j=0;j<2;j++) {
	ratio[j]=pow(10.0,((double)level[j])/1000.0);
      }
    }
    if(d_recording) {
      rda->cae()->
	inputMeterUpdate(d_input_card,d_input_port,level);
      for(int j=0;j<2;j++) {
	ratio[j]=pow(10.0,((double)level[j])/1000.0);
      }
    }
  }
  d_meter->setLeftPeakBar((int)(log10(ratio[0])*1000.0));
  d_meter->setRightPeakBar((int)(log10(ratio[1])*1000.0));
}


void RDTrackerWidget::recordLoadedData(int card,int stream)
{
  // printf("recordLoadedData(%d,%d)\n",card,stream);
  if((card!=d_input_card)||(stream!=d_input_port)) {
    return;
  }
}


void RDTrackerWidget::recordingData(int card,int stream)
{
   //printf("recordingData(%d,%d)\n",card,stream);
  if((card!=d_input_card)||(stream!=d_input_port)) {
    return;
  }
  d_segue_start_point[0]=d_loglines[0]->startPoint()+
                   d_deck[0]->currentPosition();
  if(!d_wave_name[1].isEmpty()) {
    if((d_loglines[1]->transType()!=RDLogLine::Segue)) {
      d_segue_start_point[0]=
	d_loglines[0]->endPoint();
    }
  }
  d_wave_origin[1]=d_wave_origin[0]+
                      d_deck[0]->currentPosition();
  if(!d_meter_timer->isActive()) {
    d_meter_timer->start(RD_METER_UPDATE_INTERVAL);
  }
  d_recording=true;
}


void RDTrackerWidget::recordStoppedData(int card,int stream)
{
  // printf("recordStoppedData(%d,%d)\n",card,stream);
  if((card!=d_input_card)||(stream!=d_input_port)) {
    return;
  }
  rda->cae()->unloadRecord(d_input_card,d_input_port);
  d_event_player->exec(d_record_end_macro);
}


void RDTrackerWidget::recordUnloadedData(int card,int stream,unsigned msecs)
{
  // printf("recordUnloadedData(%d,%d)\n",card,stream);

  if((card!=d_input_card)||(stream!=d_input_port)) {
    return;
  }
  d_recording=false;
  if(!d_aborting) {
    d_track_cuts[1]->
      checkInRecording(rda->station()->name(),rda->user()->name(),
		       rda->station()->name(),d_settings,msecs);
    RDRehash::rehash(rda->station(),rda->user(),rda->config(),
		     d_track_cuts[1]->cartNumber(),
		     d_track_cuts[1]->cutNumber());
    d_track_cuts[1]->setSampleRate(rda->system()->sampleRate());
    d_track_cart->updateLength();
    d_track_cart->resetRotation();
    d_loglines[1]->
      loadCart(d_track_cart->number(),RDLogLine::Segue,0,false);
    d_loglines[1]->setEvent(0,RDLogLine::Segue,false);
    d_loglines[1]->setType(RDLogLine::Cart);
    d_loglines[1]->setSource(RDLogLine::Tracker);
    d_loglines[1]->setOriginUser(rda->user()->name());
    d_loglines[1]->setOriginDateTime(QDateTime(QDate::currentDate(),
						 QTime::currentTime()));
    d_loglines[1]->
      setSegueStartPoint(d_segue_start_point[1],RDLogLine::LogPointer);
    d_loglines[1]->
      setSegueEndPoint(d_loglines[1]->endPoint(),
		       RDLogLine::LogPointer);
    d_loglines[1]->setSegueGain(0);
    d_loglines[1]->
      setSegueEndPoint(d_loglines[1]->endPoint(),RDLogLine::LogPointer);
    d_loglines[1]->
      setFadeupPoint(d_track_cuts[1]->startPoint(),RDLogLine::LogPointer);
    d_loglines[1]->setFadeupGain(RD_FADE_DEPTH);
    d_loglines[1]->
      setFadedownPoint(d_track_cuts[1]->endPoint(),RDLogLine::LogPointer);
    d_loglines[1]->setFadedownGain(RD_FADE_DEPTH);
    d_loglines[1]->setHasCustomTransition(true);
    if(!d_wave_name[2].isEmpty()) {
      d_loglines[2]->setHasCustomTransition(true);
    }
    if(!d_wave_name[2].isEmpty()) {
      d_loglines[1]->
	setAverageSegueLength(d_loglines[1]->
			      segueStartPoint()-
			      d_loglines[1]->
			      startPoint());
    }
    d_wave_origin[1]=d_loglines[1]->startPoint()-
      (d_loglines[0]->segueStartPoint()-
       d_wave_origin[0]);
    DrawTrackMap(1);
    update();
    int line=SingleSelectionLine();
    if(d_offset) {
      line--;
    }
    if(line<0) {
      return;
    }
    SaveTrack(d_track_line);
    RefreshLine(line);
    if(!d_wave_name[2].isEmpty()) {
      RefreshLine(line);
    }
  }
  else {
    positionData(1,-1);
    stateChangedData(1,RDPlayDeck::Finished);
  }
  if(!TransportActive()) {
    stateChangedData(2,RDPlayDeck::Finished);
  }
  UpdateControls();
  SendNotification(RDNotification::ModifyAction,d_loglines[1]->cartNumber());
}


void RDTrackerWidget::resizeEvent(QResizeEvent *e)
{
  //  printf("RDTrackerWidget::resizeEvent(): %dx%d\n",size().width(),size().height());

  if(d_wave_map[0]->width()!=(size().width()-88)) {
    for(int i=0;i<3;i++) {
      delete d_wave_map[i];
      d_wave_map[i]=new QPixmap(size().width()-90,77);
    }
    LoadTrack(d_track_line);
  }
  
  d_track1_button->setGeometry(size().width()-80,4,70,70);
  d_record_button->setGeometry(size().width()-80,84,70,70);
  d_track2_button->setGeometry(size().width()-80,164,70,70);
  if(rda->logeditConf()->enableSecondStart()) {
    d_finished_button->setGeometry(size().width()-80,244,70,70);
  }
  else {
    d_finished_button->setGeometry(size().width()-80,164,70,70);
  }

  d_play_button->setGeometry(9,254,70,50);
  d_stop_button->setGeometry(89,254,70,50);
  d_meter->setGeometry(169,249,d_meter->sizeHint().width(),
		       d_meter->sizeHint().height());
  d_length_label->setGeometry(518,244,100,25);

  d_tracks_remaining_label_label->setGeometry(511,277,110,14);
  d_tracks_remaining_label->setGeometry(509,302,35,18);

  d_time_remaining_label_label->setGeometry(511,289,34,14);
  d_time_remaining_label->setGeometry(549,302,74,18);

  d_time_label->setGeometry(556,289,60,14);

  d_log_view->
    setGeometry(0,324,size().width()-88,size().height()-384);
  d_post_button->setGeometry(size().width()-80,349,70,70);
  d_reset_button->
    setGeometry(size().width()-80,size().height()-159,70,70);

  d_insert_button->setGeometry(9,size().height()-50,80,50);
  d_delete_button->setGeometry(101,size().height()-50,80,50);

  d_previous_button->
    setGeometry(size().width()-279,size().height()-50,80,50);
  d_next_button->
    setGeometry(size().width()-189,size().height()-50,80,50);
}


void RDTrackerWidget::paintEvent(QPaintEvent *e)
{
  //  printf("RDTrackerWidget::paintEvent(): %dx%d\n",size().width(),size().height());
  d_track_rect->setSize(QSize(size().width()-111,3*TRACKER_Y_HEIGHT));

  
  QPainter *p=NULL;

  if(d_track_line<0) {
    p=new QPainter();
    for(int i=0;i<3;i++) {
      if(!d_wave_map[i]->isNull()) {
	p->begin(d_wave_map[i]);
	p->setPen(TRACKER_TEXT_COLOR);
	p->setBackground(Qt::gray);
	p->eraseRect(0,0,d_wave_map[i]->size().width(),
		     d_wave_map[i]->size().height());
	p->end();
      }
    }
    delete p;
  }
  p=new QPainter(this);

  p->setPen(Qt::black);

  p->fillRect(TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN,TRACKER_X_WIDTH-1-2,238,
	      Qt::white);
  p->fillRect(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+2,TRACKER_Y_HEIGHT+6,palette().mid());

  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_ORIGIN-2,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-2);
  p->drawLine(TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN-1);

  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_ORIGIN-2,
	      TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->drawLine(TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-1,
	      TRACKER_X_ORIGIN-1,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);

  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);
  p->drawLine(TRACKER_X_ORIGIN+TRACKER_X_ORIGIN-1,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);

  p->drawLine(TRACKER_X_WIDTH+TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN-2,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->drawLine(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);

  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-2,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	      TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-2);
  p->drawLine(TRACKER_X_ORIGIN-1,TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	      TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-1);

  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-2,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN,
	      TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-2);
  p->drawLine(TRACKER_X_ORIGIN-1,TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	      TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-1);

  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH-1+TRACKER_X_ORIGIN+1,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->drawLine(TRACKER_X_WIDTH-1+TRACKER_X_ORIGIN+1,
	      TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH-1+TRACKER_X_ORIGIN+1,
	      TRACKER_Y_HEIGHT*4+TRACKER_Y_ORIGIN-5+10);
  p->drawLine(TRACKER_X_WIDTH-1+TRACKER_X_ORIGIN+1,
	      TRACKER_Y_HEIGHT*4+TRACKER_Y_ORIGIN-5+10,TRACKER_X_ORIGIN-2,
	      TRACKER_Y_HEIGHT*4+TRACKER_Y_ORIGIN-5+10);
  p->drawLine(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*4+TRACKER_Y_ORIGIN-5+10,
	      TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);

  //
  // Draw the tracks
  //
  p->drawPixmap(TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN,*d_wave_map[0]);
  p->drawPixmap(TRACKER_X_ORIGIN,TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN,
		*d_wave_map[1]);
  p->drawPixmap(TRACKER_X_ORIGIN,TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN,
		  *d_wave_map[2]);

  //
  // Draw the cursor
  //
  if(d_cursor_xpos>=0) {
    p->setPen(Qt::green);
    p->drawLine(d_cursor_xpos,10,d_cursor_xpos,248);
  }

  delete p;
}


void RDTrackerWidget::mousePressEvent(QMouseEvent *e)
{
  if(TransportActive()) {
    return;
  }
  if(e->button()==Qt::LeftButton) {
    d_current_track=GetClick(e,d_previous_point);
  }
  if(e->button()==Qt::RightButton) {
    d_rightclick_track=GetClick(e,d_previous_point);
    d_rightclick_pos=d_previous_point->x();
    if(d_rightclick_track>=0) {
      d_mouse_menu->setGeometry(e->globalX(),e->globalY(),
			      d_mouse_menu->sizeHint().width(),
			      d_mouse_menu->sizeHint().height());
      d_mouse_menu->exec();
    }
  }
}


void RDTrackerWidget::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()) {
  case Qt::LeftButton:
    if(d_current_track>=0) {
      d_meter->setLeftPeakBar(-10000);
      d_meter->setRightPeakBar(-10000);
    }  
    d_current_track=-1;
    break;

  case Qt::RightButton:
    d_rightclick_track=-1;
    break;

  case Qt::MidButton:
    if(e->y()<TRACKER_Y_ORIGIN+TRACKER_Y_HEIGHT) {
      DragTrack(0,((d_wave_origin[0]-d_loglines[0]->
		    startPoint())/TRACKER_MSECS_PER_PIXEL)+250);
    }
    else {
      if(e->y()<(TRACKER_Y_ORIGIN+2*TRACKER_Y_HEIGHT)) {
	DragTrack(0,((d_wave_origin[1]-d_loglines[1]->
		      startPoint())/TRACKER_MSECS_PER_PIXEL)+250);
      }
      else {
	if(e->y()<(TRACKER_Y_ORIGIN+3*TRACKER_Y_HEIGHT)) {
	  DragTrack(0,((d_wave_origin[2]-d_loglines[2]->
			startPoint())/TRACKER_MSECS_PER_PIXEL)+250);
	}
      }
    }
    break;

  default:
    break;
  }
}


void RDTrackerWidget::mouseMoveEvent(QMouseEvent *e)
{
  if(TransportActive()) {
    return;
  }
  QPoint pt;
  int trackno=GetClick(e,&pt);
  if(d_current_track<0) {
    QCursor *cursor=d_arrow_cursor;
    if((trackno>=0)&&(d_track_rect->contains(pt))) {
      if(!d_wave_name[trackno].isEmpty()) {
	d_current_target=RDTrackerWidget::TargetSize;
	for(unsigned i=0;i<RDTrackerWidget::TargetSize;i++) {
	  if(d_target_rect[i]->contains(pt)&&
	     (d_target_track[i]==trackno)) {
	    cursor=d_cross_cursor;
	    d_current_target=(RDTrackerWidget::Target)i;
	  }
	}
	if(d_current_target==RDTrackerWidget::TargetSize) {
	  switch(d_loglines[trackno]->transType()) {
	  case RDLogLine::Segue:
	    cursor=d_hand_cursor;
	    break;
		
	  case RDLogLine::Play:
	  case RDLogLine::Stop:
	  case RDLogLine::NoTrans:
	    if(trackno==0) {
	      cursor=d_hand_cursor;
	    }
	    else {
	      cursor=d_arrow_cursor;
	    }
	    break;
	  }
	}
      }
    }
    if(d_current_cursor!=cursor) {
      setCursor(*cursor);
      d_current_cursor=cursor;
    }
    return;
  }
  if(trackno!=d_current_track) {
    return;
  }
  if(d_current_cursor==d_hand_cursor) {
    DragTrack(d_current_track,pt.x()-d_previous_point->x());
    *d_previous_point=pt;
  }
  if(d_current_cursor==d_cross_cursor) {
    DragTarget(d_current_track,pt);
  }
}


void RDTrackerWidget::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
  case Qt::Key_Less:
    if(!d_wave_name[0].isEmpty() && !TransportActive()) {
      DragTrack(0,400);
    }
    else {
      if(!d_wave_name[1].isEmpty() && !TransportActive()) {
	DragTrack(1,400);
      }
      else {
	if(!d_wave_name[2].isEmpty() && !TransportActive())  {
	  DragTrack(2,400);
	}
      }
    }    
    break;

  case Qt::Key_Greater:
    if(!d_wave_name[0].isEmpty() && !TransportActive()) {
      DragTrack(0,-400);
    }
    else {
      if(!d_wave_name[1].isEmpty() && !TransportActive()) {
	DragTrack(1,-400);
      }
      else {
	if(!d_wave_name[2].isEmpty() && !TransportActive()) {
	  DragTrack(2,-400);
	}
      }
    }
    break;
    
  case Qt::Key_Shift:
    d_shift_pressed=true;
    break;
	
  default:
    QWidget::keyPressEvent(e);
    break;
  }
}


void RDTrackerWidget::keyReleaseEvent(QKeyEvent *e)
{
  switch(e->key()) {
  case Qt::Key_Shift:
    d_shift_pressed=false;
    break;

  default:
    QWidget::keyPressEvent(e);
    break;
  }
}


void RDTrackerWidget::wheelEvent(QWheelEvent *e)
{
  if(d_shift_pressed) {
    if(e->y()<TRACKER_Y_ORIGIN+TRACKER_Y_HEIGHT) {
      DragTrack(0,e->delta());
    }
    else {
      if(e->y()<(TRACKER_Y_ORIGIN+2*TRACKER_Y_HEIGHT)) {
        DragTrack(1,e->delta());
      }
      else {
        if(e->y()<(TRACKER_Y_ORIGIN+3*TRACKER_Y_HEIGHT)) {
          DragTrack(2,e->delta());
        }
      }
    }
  }
  else {
    if(!d_wave_name[0].isEmpty() && !TransportActive()) {
      DragTrack(0,e->delta());
    }
    else {
      if(!d_wave_name[1].isEmpty() && !TransportActive()) {
	DragTrack(1,e->delta());
      }
      else {
	if(!d_wave_name[2].isEmpty() && !TransportActive()) {
	  DragTrack(2,e->delta());
	}
      }
    }
  }
}


void RDTrackerWidget::LoadTrack(int line)
{
  d_deck_state=DeckIdle;

  if(line<0) {
    for(int i=0;i<3;i++) {
      if(!(d_wave_name[i].isEmpty())) {
	delete d_wpg[i];
        d_wpg[i]=NULL;
      }
      d_wave_name[i]="";
      d_loglines[i]=NULL;
      d_wave_origin[i]=0;
      d_time_width=0;
    }
  }
  else {
    //
    // Track Mappings
    //
    if(d_loaded) {
      if(d_offset) {
	d_track_lines[0]=line-2;
	d_track_lines[1]=line-1;
	d_track_lines[2]=line;
      }
      else {
	d_track_lines[0]=line-1;
	d_track_lines[1]=line;
	d_track_lines[2]=line+1;
      }
    }
    else {
      if(d_segue_loaded) {
	d_track_lines[0]=line-1;
	d_track_lines[1]=line;
	d_track_lines[2]=line+1;
      }
      else {
	d_track_lines[0]=-1;
	d_track_lines[1]=-1;
	d_track_lines[2]=-1;
      }
    }

    //
    // Audio Filenames
    //
    for(unsigned i=0;i<3;i++) {
      d_wave_name[i]=GetCutName(d_track_lines[i],&d_track_cuts[i]);
      if(!(d_wave_name[i].isEmpty())) {
        if(d_wpg[i]!=NULL) {
           delete d_wpg[i];
        }
	if(!d_wave_map[i]->isNull()) {
	  d_wpg[i]=new RDWavePainter(d_wave_map[i],d_track_cuts[i],
				     rda->station(),rda->user(),rda->config());
	  d_wpg[i]->end();
	}
      }
    }

    //
    // Track 0 Parameters
    //
    if(d_wave_name[0].isEmpty()) {
      d_loglines[0]=d_dummy0_logline;  // No initial track, so fake it
      d_loglines[0]->clear();
      d_loglines[0]->setStartPoint(0,RDLogLine::CartPointer);
      d_loglines[0]->setStartPoint(-1,RDLogLine::LogPointer);
      d_loglines[0]->setEndPoint(0,RDLogLine::CartPointer);
      d_loglines[0]->setEndPoint(-1,RDLogLine::LogPointer);
      d_wave_origin[0]=0;
    }
    else {
      d_loglines[0]=d_log_model->logLine(d_track_lines[0]);
      d_loglines[0]->refreshPointers();
      d_wave_origin[0]=d_loglines[0]->segueStartPoint()-d_preroll;
    }

    //
    // Track 1 Parameters
    //
    d_loglines[1]=d_log_model->logLine(d_track_lines[1]);
    if(!d_wave_name[1].isEmpty()) {
      d_loglines[1]->refreshPointers();
    }  
    if((d_log_model->logLine(d_track_lines[1])!=NULL)&&
       (!d_wave_name[1].isEmpty())) {
      d_wave_origin[1]=d_loglines[1]->startPoint()-d_preroll;
      d_length_label->
	setText(RDGetTimeLength(d_loglines[1]->forcedLength(),true));
      if(((d_loglines[1]->transType()!=RDLogLine::Segue))&&
	 (!d_wave_name[0].isEmpty())) {
	d_wave_origin[0]=d_loglines[0]->endPoint()-d_preroll;
      }
    }
    else {
      d_wave_origin[1]=0;
      d_length_label->setText("0:00:00.0");
    }
    
    //
    // Track 2 Parameters
    //
    if(d_wave_name[2].isEmpty()) {
      d_loglines[2]=d_dummy2_logline;  // No final track, so fake it
      d_loglines[2]->clear();
      d_loglines[2]->setStartPoint(0,RDLogLine::CartPointer);
      d_loglines[2]->setStartPoint(-1,RDLogLine::LogPointer);
      d_loglines[2]->setEndPoint(0,RDLogLine::CartPointer);
      d_loglines[2]->setEndPoint(-1,RDLogLine::LogPointer);
      d_wave_origin[2]=0;
    }
    else {
      d_loglines[2]=d_log_model->logLine(d_track_lines[2]);
      d_loglines[2]->refreshPointers();
      if(d_loglines[2]->transType()!=RDLogLine::Segue) {
	if(!d_wave_name[1].isEmpty()) {  
	   d_wave_origin[2]=d_loglines[2]->startPoint()-
	     (d_loglines[1]->endPoint()-d_loglines[1]->startPoint()+d_preroll);
	}
	else
	{
	  d_wave_origin[2]=d_loglines[2]->startPoint()-d_preroll;
  	  d_wave_origin[0]=d_loglines[0]->endPoint()-d_preroll;
	}
      }
      else {
        if(d_wave_name[1].isEmpty()) {
	  d_wave_origin[2]=d_loglines[2]->startPoint()-d_preroll;
        }
        else {
	   d_wave_origin[2]=d_loglines[2]->startPoint()-
	     (d_loglines[1]->segueStartPoint()-d_loglines[1]->startPoint()+
	     d_preroll);
        }
      }
    }
    d_time_width=d_wave_map[2]->width()*TRACKER_MSECS_PER_PIXEL;
  }
  d_start_time=d_log_model->blockStartTime(line);
  DrawTrackMap(0);
  DrawTrackMap(1);
  DrawTrackMap(2);

  UpdateControls();
}


void RDTrackerWidget::SaveTrack(int line)
{
  if((line<0)||(line==TRACKER_MAX_LINENO)) {
    return;
  }
  if((line>0)&&(d_log_model->logLine(line-1)->type()==RDLogLine::Track)) {
    line--;
  }
  if(d_size_altered) {
     d_log_model->save(rda->config());
  }
  else {
     d_log_model->saveModified(rda->config());
  }

  d_log->
    setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
  d_changed=false;
  d_size_altered=false;
  SendNotification(RDNotification::ModifyAction,d_log->name());
}


bool RDTrackerWidget::ImportTrack(int line)
{
  bool metadata=false;
  bool import_running=false;
  
  if(!InitTrack()) {
    return false;
  }
  RDWaveData *wdata=new RDWaveData();
  RDImportAudio *import=
    new RDImportAudio(d_track_cuts[1]->cutName(),d_import_path,
		      d_settings,&metadata,wdata,NULL,&import_running,
		      "RDLogEdit",this);
  if(import->exec(true,false)<0) {
    delete import;
    delete wdata;
    resetData();
    return true;
  }
  delete import;
  delete wdata;

  if(!d_wave_name[0].isEmpty()) {
    d_loglines[0]->
      setSegueStartPoint(d_loglines[0]->
			 segueStartPoint(RDLogLine::CartPointer),
			 RDLogLine::LogPointer);
    d_loglines[0]->setSegueGain(0);
    d_loglines[0]->
      setSegueEndPoint(d_loglines[0]->endPoint(),RDLogLine::LogPointer);
    d_loglines[1]->setTransType(RDLogLine::Segue);
    d_loglines[0]->
      setAverageSegueLength(d_loglines[0]->segueStartPoint()-
			    d_loglines[0]->startPoint());
  }

  d_track_cart->updateLength();
  d_track_cart->resetRotation();
  d_loglines[1]->
    loadCart(d_track_cart->number(),RDLogLine::Segue,0,false);
  d_loglines[1]->setEvent(0,RDLogLine::Segue,false);
  d_loglines[1]->setType(RDLogLine::Cart);
  d_loglines[1]->setSource(RDLogLine::Tracker);
  d_loglines[1]->setOriginUser(rda->user()->name());
  d_loglines[1]->setOriginDateTime(QDateTime(QDate::currentDate(),
					       QTime::currentTime()));
  d_loglines[1]->setFadeupPoint(d_track_cuts[1]->startPoint(),
				  RDLogLine::LogPointer);
  d_loglines[1]->setFadeupGain(RD_FADE_DEPTH);
  d_loglines[1]->setFadedownPoint(d_track_cuts[1]->endPoint(),
				    RDLogLine::LogPointer);
  d_loglines[1]->setFadedownGain(RD_FADE_DEPTH);
  d_loglines[1]->setEndPoint(-1,RDLogLine::LogPointer);
  d_loglines[1]->setSegueEndPoint(d_track_cuts[1]->endPoint(),
				    RDLogLine::LogPointer);
  d_loglines[1]->setHasCustomTransition(true);
  if(!d_wave_name[2].isEmpty()) {
    d_loglines[2]->setHasCustomTransition(true);
    d_loglines[1]->
      setAverageSegueLength(d_loglines[1]->segueStartPoint()-
			    d_loglines[1]->startPoint());
  }
  postData();
  SaveTrack(d_track_line);
  LoadTrack(d_track_line);
  update();

  RefreshLine(line);
  if(!d_wave_name[2].isEmpty()) {
    RefreshLine(line+1);
  }

  return true;
}


void RDTrackerWidget::RenderTransition(int line)
{
  if(line==TRACKER_MAX_LINENO) {
    d_loaded=false;
    d_segue_loaded=false;
    d_length_label->setText("-:--:--.-");
  }
  else {
    if(IsTrack(line,&d_offset)) {
      d_loaded=true;
    }
    else {
      d_loaded=false;
      d_length_label->setText("-:--:--.-");
    }
    d_segue_loaded=true;
  }
  d_track_line=line;
  LoadTrack(d_track_line);
  LoadBlockLength(line);
  UpdateRemaining();
  UpdateControls();
}


void RDTrackerWidget::LoadBlockLength(int line)
{
  int nominal_length=0;
  int actual_length=0;

  d_block_valid=d_log_model->
    blockLength(&nominal_length,&actual_length,line);
  d_time_remaining=nominal_length-actual_length;
}


void RDTrackerWidget::RefreshLine(int line)
{
  RDLogLine *logline=d_log_model->logLine(line);
  if(logline==NULL) {
    return;
  }
  logline->refreshCart();
}


void RDTrackerWidget::StartNext(int finishing_id,int next_id)
{
  int duckin;
  if(next_id==-1) {
    next_id=finishing_id+1;
  }
  for(int i=next_id;i<3;i++) {
    if(!d_wave_name[i].isEmpty()) {
      d_deck[i]->setCart(d_loglines[i],true);
      if(d_wave_name[i-1].isEmpty()) {
        duckin=
	  d_loglines[i-2]->segueEndPoint()-d_loglines[i-2]->segueStartPoint();
	d_deck[i-2]->stop(duckin);
      }
      else {
        duckin=
	  d_loglines[i-1]->segueEndPoint()-d_loglines[i-1]->segueStartPoint();
	d_deck[i-1]->stop(duckin);
      }
      if(d_recording)
        duckin=-1;
      d_deck[i]->
	     play(d_segue_start_offset[i],
             d_loglines[i]->segueStartPoint(),
	     d_loglines[i]->segueEndPoint(),duckin);
      return;
    }
  }
}


QString RDTrackerWidget::GetCutName(int line,RDCut **cut)
{
  if((line<0)||(line>=d_log_model->lineCount())) {
    return QString();
  }
  QString wavname;
  QString pathname;
  RDLogLine *logline=d_log_model->logLine(line);
  if(*cut!=NULL) {
    delete *cut;
    *cut=NULL;
  }
  if(line==(d_log_model->lineCount()-1)) {
    logline->setEvent(0,RDLogLine::Stop,false);
  }
  else {
    logline->setEvent(0,d_log_model->logLine(line+1)->transType(),false);
  }
  if(!logline->cutName().isEmpty()) {
    *cut=new RDCut(logline->cutName());
    pathname=RDCut::pathName(logline->cutName());
  }
  return pathname;
}


int RDTrackerWidget::GetClick(QMouseEvent *e,QPoint *p)
{
  p->setX(e->x()-TRACKER_X_ORIGIN);
  if((e->x()<=TRACKER_X_ORIGIN)||
     (e->x()>=(TRACKER_X_ORIGIN+TRACKER_X_WIDTH))||
     (e->y()<=TRACKER_Y_ORIGIN)||
     (e->y()>=(TRACKER_Y_ORIGIN+3*TRACKER_Y_HEIGHT))) {
    return -1;
  }
  if(e->y()<TRACKER_Y_ORIGIN+TRACKER_Y_HEIGHT) {
    p->setY(e->y()-TRACKER_Y_ORIGIN);
    return 0;
  }
  if(e->y()<(TRACKER_Y_ORIGIN+2*TRACKER_Y_HEIGHT)) {
    p->setY(e->y()-(TRACKER_Y_ORIGIN+TRACKER_Y_HEIGHT));
    return 1;
  }
  p->setY(e->y()-(TRACKER_Y_ORIGIN+2*TRACKER_Y_HEIGHT));
  return 2;
}


void RDTrackerWidget::DragTrack(int trackno,int xdiff)
{
  if(d_wave_name[trackno].isEmpty()) {
    return;
  }
  if((!d_changed)&&(trackno>0)) {
    PushSegues();
  }

  //
  // Calculate the Offset
  //
  int tdiff=xdiff*TRACKER_MSECS_PER_PIXEL;
  int tend=0;
  switch(trackno) {
  case 1:
    if(!d_wave_name[0].isEmpty()) {
      if((d_wave_origin[1]-d_loglines[1]->startPoint()-tdiff)>
	 (d_wave_origin[0]-d_loglines[0]->startPoint())) {
	tdiff=(d_wave_origin[1]-d_loglines[1]->startPoint())-
	  (d_wave_origin[0]-d_loglines[0]->startPoint());
      }
      tend=d_wave_origin[0]-
	d_loglines[0]->segueEndPoint()+
	d_loglines[1]->startPoint()+100;
      if((d_wave_origin[1]-tdiff)<tend) {
	tdiff=d_wave_origin[1]-tend;
      }
      d_loglines[0]->
	setSegueStartPoint(d_loglines[0]->segueStartPoint()+tdiff,
			   RDLogLine::LogPointer);
      d_loglines[0]->
	setAverageSegueLength(d_loglines[0]->
			      segueStartPoint()-
			      d_loglines[0]->
			      startPoint());
    }
    break;

  case 2:
    if(d_wave_name[1].isEmpty()) {
      if(!d_wave_name[0].isEmpty()) {
	if((d_wave_origin[2]-d_loglines[2]->startPoint()-tdiff)>
	   (d_wave_origin[0]-d_loglines[0]->startPoint())) {
	  tdiff=(d_wave_origin[2]-d_loglines[2]->startPoint())-
	    (d_wave_origin[0]-d_loglines[0]->startPoint());
	}
	tend=d_wave_origin[0]-
	  d_loglines[0]->segueEndPoint()+
	  d_loglines[2]->startPoint()+100;
	if((d_wave_origin[2]-tdiff)<tend) {
	  tdiff=d_wave_origin[2]-tend;
	}
	d_loglines[0]->
	  setSegueStartPoint(d_loglines[0]->segueStartPoint()+tdiff,
			     RDLogLine::LogPointer);
	d_loglines[0]->
	  setAverageSegueLength(d_loglines[0]->segueStartPoint()-
				 d_loglines[0]->startPoint());
      }
    }
    else {
      if((d_wave_origin[2]-d_loglines[2]->startPoint()-tdiff)>
	 (d_wave_origin[1]-d_loglines[1]->startPoint())) {
	tdiff=(d_wave_origin[2]-d_loglines[2]->startPoint())-
	  (d_wave_origin[1]-d_loglines[1]->startPoint());
      }
      tend=d_wave_origin[1]-
	d_loglines[1]->segueEndPoint()+
	d_loglines[2]->startPoint()+100;
      if((d_wave_origin[2]-tdiff)<tend) {
	tdiff=d_wave_origin[2]-tend;
      }
      d_loglines[1]->
	setSegueStartPoint(d_loglines[1]->segueStartPoint()+tdiff,
			   RDLogLine::LogPointer);
      d_loglines[1]->
	setAverageSegueLength(d_loglines[1]->segueStartPoint()-
			      d_loglines[1]->startPoint());
    }
    break;
  }

  //
  // Ignore Meaningless Edits
  //
  bool state=false;
  for(int i=0;i<trackno;i++) {
    state|=!d_wave_name[i].isEmpty();
  }
  for(int i=trackno;i<3;i++) {
    d_wave_origin[i]-=tdiff;
  }
  d_start_time=d_log_model->blockStartTime(d_track_line);
  for(int i=0;i<3;i++) {
    DrawTrackMap(i);
  }
  update();
  if((!state)||(tdiff==0)) {
    return;
  }

  //
  // Apply the Offset
  //
  d_changed=true;
  d_loglines[trackno]->setHasCustomTransition(true);

  if(d_wave_name[2].isEmpty()) {
    if((d_wave_origin[0]-
	d_loglines[0]->endPoint())>0) {
      d_track1_button->setDisabled(true);
      d_record_button->setDisabled(true);
    }
  }
  else {
    if(d_wave_origin[2]>0) {
      d_track1_button->setDisabled(true);
      d_record_button->setDisabled(true);
    }
  }
  LoadBlockLength(d_track_line);
  int line=-1;
  if(d_log_model->logLine(d_track_line)->type()==RDLogLine::Track) {
    line=d_track_line+1;
  }
  else {
    line=d_track_line;
  }
  if(line>=0) {
    RefreshLine(line);
    RefreshLine(line+1);
  }
  UpdateRemaining();
  UpdateControls();
}


void RDTrackerWidget::DragTarget(int trackno,const QPoint &pt)
{
  //  printf("DragTarget(%d,(%d,%d))  TARGET: %d\n",trackno,pt.x(),pt.y(),d_current_target);  

  if(!d_changed) {
    PushSegues();
    d_changed=true;
    UpdateControls();
  }
  switch(d_current_target) {
  case RDTrackerWidget::FadedownPoint:
    d_loglines[trackno]->
      setDuckDownGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
    if(d_loglines[trackno]->duckDownGain()>TRACKER_MAX_GAIN) {
      d_loglines[trackno]->setDuckDownGain(TRACKER_MAX_GAIN);
    }
    if(d_loglines[trackno]->duckDownGain()<TRACKER_MIN_GAIN) {
      d_loglines[trackno]->setDuckDownGain(TRACKER_MIN_GAIN);
    }
   
  case RDTrackerWidget::TrackFadedownPoint:
    if((d_loglines[trackno]->fadedownPoint()<=0 || 
	d_loglines[trackno]->fadedownPoint()>=d_loglines[trackno]->
	endPoint())
       && d_loglines[trackno]->fadedownGain()==0) {
      d_loglines[trackno]->setFadedownGain(RD_FADE_DEPTH);
    }
    d_loglines[trackno]->setSegueGain(0);
    if(d_loglines[trackno]->fadedownPoint(RDLogLine::LogPointer)<0) {
      d_loglines[trackno]->setFadedownGain(RD_FADE_DEPTH);
    }
    if(trackno<2) {
      if((d_loglines[trackno+1]->transType()==RDLogLine::Segue)
	 && (!d_loaded)) {
	d_loglines[trackno]->
	  setEndPoint(d_loglines[trackno]->segueEndPoint(),
		      RDLogLine::LogPointer);
      }
    } 
    d_loglines[trackno]->
      setFadedownPoint(pt.x()*TRACKER_MSECS_PER_PIXEL+
		       d_wave_origin[trackno],RDLogLine::LogPointer);
    if(d_loglines[trackno]->fadedownPoint()>d_loglines[trackno]->
       endPoint()) {
      d_loglines[trackno]->setFadedownPoint(d_loglines[trackno]->
					      endPoint(),RDLogLine::LogPointer);
    }
    if(d_loglines[trackno]->fadedownPoint()<
       d_loglines[trackno]->fadeupPoint()) {
      d_loglines[trackno]->
	setFadedownPoint(d_loglines[trackno]->fadeupPoint(),
			 RDLogLine::LogPointer);;
    }
    if(d_loglines[trackno]->fadedownPoint(RDLogLine::CartPointer)>=0 
       &&  d_loglines[trackno]->fadedownPoint()>
       d_loglines[trackno]->fadedownPoint(RDLogLine::CartPointer)) {
      d_loglines[trackno]->
	setFadedownPoint(d_loglines[trackno]->
			 fadedownPoint(RDLogLine::CartPointer),
			 RDLogLine::LogPointer);
    }
    DrawTrackMap(trackno);
    d_meter->setLeftPeakBar(d_loglines[trackno]->duckDownGain()-1600);
    d_meter->setRightPeakBar(d_loglines[trackno]->duckDownGain()-1600);
    if(d_loglines[trackno+1]!=NULL) {
      d_loglines[trackno+1]->setHasCustomTransition(true);
    }  
    break;

  case RDTrackerWidget::FadedownGain:
  case RDTrackerWidget::TrackFadedownGain:
    if(d_loglines[trackno]->fadedownPoint()>d_loglines[trackno]->endPoint()
       || d_loglines[trackno]->fadedownPoint()<0) {
      d_loglines[trackno]->setFadedownPoint(d_loglines[trackno]->endPoint(),
					      RDLogLine::LogPointer);
    }
    d_loglines[trackno]->
      setFadedownGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
    if(d_loglines[trackno]->fadedownGain()>TRACKER_MAX_GAIN) {
      d_loglines[trackno]->setFadedownGain(TRACKER_MAX_GAIN);
    }
    if(d_loglines[trackno]->fadedownGain()<TRACKER_MIN_GAIN) {
      d_loglines[trackno]->setFadedownGain(TRACKER_MIN_GAIN);
    }
    if(d_loglines[trackno]->fadedownPoint(RDLogLine::CartPointer)>=0
       && d_loglines[trackno]->fadedownPoint(RDLogLine::CartPointer)<
       d_loglines[trackno]->endPoint()
       ) {
      d_loglines[trackno]->setFadedownGain(RD_FADE_DEPTH);
    }  
    DrawTrackMap(trackno);
       d_meter->setLeftPeakBar(d_loglines[trackno]->fadedownGain()-1600);
       d_meter->setRightPeakBar(d_loglines[trackno]->fadedownGain()-1600);
    if(d_loglines[trackno+1]!=NULL) {
      d_loglines[trackno+1]->setHasCustomTransition(true);
    }  
    break;

  case RDTrackerWidget::FadeupPoint:
    d_loglines[trackno]->
      setDuckUpGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
    if(d_loglines[trackno]->duckUpGain()>TRACKER_MAX_GAIN) {
      d_loglines[trackno]->setDuckUpGain(TRACKER_MAX_GAIN);
    }
    if(d_loglines[trackno]->duckUpGain()<TRACKER_MIN_GAIN) {
      d_loglines[trackno]->setDuckUpGain(TRACKER_MIN_GAIN);
    }

  case RDTrackerWidget::TrackFadeupPoint:
    if(d_loglines[trackno]->fadeupPoint()<=d_loglines[trackno]->startPoint()
       && d_loglines[trackno]->fadeupGain()==0) {
      d_loglines[trackno]->setFadeupGain(RD_FADE_DEPTH);
    }
    if(d_loglines[trackno]->fadeupPoint(RDLogLine::LogPointer)<0) {
      d_loglines[trackno]->setFadeupGain(RD_FADE_DEPTH);
    }
    d_loglines[trackno]->
      setFadeupPoint(pt.x()*TRACKER_MSECS_PER_PIXEL+
		     d_wave_origin[trackno],RDLogLine::LogPointer);
    if(d_loglines[trackno]->fadeupPoint()>
       d_loglines[trackno]->fadedownPoint()
       && d_loglines[trackno]->fadedownPoint()>=0) {
      d_loglines[trackno]->
	setFadeupPoint(d_loglines[trackno]->
		       fadedownPoint(),RDLogLine::LogPointer);
    }
    if(d_loglines[trackno]->fadeupPoint()<d_loglines[trackno]->
       startPoint()) {
      d_loglines[trackno]->setFadeupPoint(d_loglines[trackno]->
					    startPoint(),RDLogLine::LogPointer);
    }
    if(d_loglines[trackno]->fadeupPoint()<
       d_loglines[trackno]->fadeupPoint(RDLogLine::CartPointer)) {
      d_loglines[trackno]->
	setFadeupPoint(d_loglines[trackno]->
		       fadeupPoint(RDLogLine::CartPointer),
		       RDLogLine::LogPointer);
    }
    DrawTrackMap(trackno);
    d_meter->setLeftPeakBar(d_loglines[trackno]->duckUpGain()-1600);
    d_meter->setRightPeakBar(d_loglines[trackno]->duckUpGain()-1600);
    d_loglines[trackno]->setHasCustomTransition(true);
    break;

  case RDTrackerWidget::FadeupGain:
  case RDTrackerWidget::TrackFadeupGain:
    if(d_loglines[trackno]->fadeupPoint()<
       d_loglines[trackno]->startPoint()) {
      d_loglines[trackno]->setFadeupPoint(d_loglines[trackno]->
					    startPoint(),RDLogLine::LogPointer);
    }
    d_loglines[trackno]->
      setFadeupGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
    if(d_loglines[trackno]->fadeupGain()>TRACKER_MAX_GAIN) {
      d_loglines[trackno]->setFadeupGain(TRACKER_MAX_GAIN);
    }
    if(d_loglines[trackno]->fadeupGain()<TRACKER_MIN_GAIN) {
      d_loglines[trackno]->setFadeupGain(TRACKER_MIN_GAIN);
    }
    if(d_loglines[trackno]->fadeupPoint(RDLogLine::CartPointer)>
       d_loglines[trackno]->startPoint()) {
      d_loglines[trackno]->setFadeupGain(RD_FADE_DEPTH);
    }  
    DrawTrackMap(trackno);
    d_meter->setLeftPeakBar(d_loglines[trackno]->fadeupGain()-1600);
    d_meter->setRightPeakBar(d_loglines[trackno]->fadeupGain()-1600);
    d_loglines[trackno]->setHasCustomTransition(true);
    break;

  case RDTrackerWidget::TargetSize:
    break;
  }
  d_changed=true;
  int line=-1;
  if(d_log_model->logLine(d_track_line)->type()==RDLogLine::Track) {
    line=d_track_line+1;
  }
  else {
    line=d_track_line;
  }
  if(line>=0) {
    RefreshLine(line);
    RefreshLine(line);
  }
  UpdateControls();
}


void RDTrackerWidget::DrawTrackMap(int trackno)
{
  QTime track_time;
  QPainter *p=NULL;
  int xpos=0;
  QColor back_color;
  switch(trackno) {
  case 0:
    if(d_wave_name[0].isEmpty()&&(!d_wave_map[0]->isNull())) {
      p=new QPainter(d_wave_map[0]);
      p->setBackground(Qt::gray);
      p->eraseRect(0,0,d_wave_map[0]->size().width(),
		   d_wave_map[0]->size().height());
      p->end();
      delete p;
    }
    else {
      if(!d_wave_map[0]->isNull()&&(!d_wave_map[0]->isNull())) {
	d_wpg[0]->begin(d_wave_map[0]);
	d_wpg[0]->setFont(labelFont());
	d_wpg[0]->setPen(TRACKER_TEXT_COLOR);
	d_wpg[0]->setBackground(palette().color(QPalette::Background));
	d_wpg[0]->eraseRect(0,0,d_wave_map[0]->size().width(),
			    d_wave_map[0]->size().height());
	if(!d_wave_name[0].isEmpty()) {
	  d_wpg[0]->drawWaveByMsecs(0,d_wave_map[0]->width(),
				    d_wave_origin[0],
				    d_wave_origin[0]+d_time_width,800,
				    RDWavePainter::Mono,Qt::black,
				    d_loglines[0]->
				    startPoint(RDLogLine::CartPointer),
				    d_loglines[0]->
				    endPoint(RDLogLine::CartPointer));

	  //
	  // Draw Segue Markers
	  //
	  if(d_loglines[0]->segueStartPoint(RDLogLine::CartPointer)>=0) {
	    xpos=(d_loglines[0]->segueStartPoint(RDLogLine::CartPointer)-
		  d_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(d_wpg[0],d_wave_map[0]->height(),xpos,
		       RD_SEGUE_MARKER_COLOR,20,true);
	    xpos=(d_loglines[0]->segueEndPoint(RDLogLine::CartPointer)-
		  d_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(d_wpg[0],d_wave_map[0]->height(),xpos,
		       RD_SEGUE_MARKER_COLOR,20,false);
	  }

	  //
	  // Draw Start Marker
	  //
	  xpos=(d_loglines[0]->startPoint(RDLogLine::CartPointer)-
		d_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(d_wpg[0],d_wave_map[0]->height(),xpos,
		     RD_START_END_MARKER_COLOR,10,true);

	  //
	  // Draw End Marker
	  //
	  xpos=(d_loglines[0]->endPoint(RDLogLine::CartPointer)-
		d_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(d_wpg[0],d_wave_map[0]->height(),xpos,
		     RD_START_END_MARKER_COLOR,10,false);

	  //
	  // Draw Fadedown Marker
	  //
	  if(d_loglines[0]->fadedownPoint(RDLogLine::CartPointer)>=0) {
	    xpos=(d_loglines[0]->fadedownPoint(RDLogLine::CartPointer)-
		  d_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(d_wpg[0],d_wave_map[0]->height(),xpos,
		       RD_FADE_MARKER_COLOR,30,true);
	  }

	  //
	  // Draw Rubber Bands
	  //
	  DrawRubberBand(d_wpg[0],0);
   
	  //
	  // Draw Menu Marker
	  //
	  if(d_menu_clicked_point>=0 && d_rightclick_track==0) {
	    DrawCursor(d_wpg[0],d_wave_map[0]->height(),d_menu_clicked_point,
		       RD_START_END_MARKER_COLOR,10,true);
	    DrawCursor(d_wpg[0],d_wave_map[0]->height(),d_menu_clicked_point,
		       RD_START_END_MARKER_COLOR,10,false);
	  }	    
	}
	d_wpg[0]->setPen(TRACKER_TEXT_COLOR);
	d_wpg[0]->drawText(5,14,d_loglines[0]->
		     resolveWildcards(rda->logeditConf()->waveformCaption()));
	d_wpg[0]->end();
      }
    }
    break;

  case 1:
    if(d_wave_name[1].isEmpty()) {
      if(!d_wave_map[1]->isNull()) {
	p=new QPainter(d_wave_map[1]);
	if(d_loaded) {
	  p->setBackground(palette().color(QPalette::Background));
	  p->setFont(labelFont());
	  p->setPen(TRACKER_TEXT_COLOR);
	  p->eraseRect(0,0,d_wave_map[1]->size().width(),
		       d_wave_map[1]->size().height());
	}
	else {
	  p->setBackground(Qt::gray);
	  p->eraseRect(0,0,d_wave_map[1]->size().width(),
		       d_wave_map[1]->size().height());
	}
	if(d_start_time>QTime(0,0,0)) {
	  p->setFont(labelFont());
	  p->setPen(TRACKER_TEXT_COLOR);
	  p->drawText(550,75,tr("Start")+" "+
		      d_start_time.toString("h:mm:ss"));
	}           
	p->end();
	delete p;
      }
    }
    else {
      if((d_loglines[1]->transType()==RDLogLine::Segue)) {
	back_color=palette().color(QPalette::Background);
      }
      else {
	back_color=Qt::lightGray;
      }
      switch(d_deck_state) {
      case RDTrackerWidget::DeckTrack2:
      case RDTrackerWidget::DeckTrack3:
	if(!d_wave_map[1]->isNull()) {
	  p=new QPainter(d_wave_map[1]);
	  p->setPen(TRACKER_RECORD_COLOR);
	  p->setBrush(TRACKER_RECORD_COLOR);
	  p->setBackground(back_color);
	  p->eraseRect(0,0,d_wave_map[1]->size().width(),
		       d_wave_map[1]->size().height());
	  p->fillRect(-d_wave_origin[1]/
		      TRACKER_MSECS_PER_PIXEL,
		      TRACKER_Y_HEIGHT/4,
		      d_recording_pos/TRACKER_MSECS_PER_PIXEL,
		      TRACKER_Y_HEIGHT/2,TRACKER_RECORD_COLOR);
	  p->setFont(labelFont());
	  if(d_start_time>QTime(0,0,0)) {
	    p->setPen(TRACKER_TEXT_COLOR);
	    track_time=d_start_time;
	    track_time=track_time.
	      addMSecs(d_time_remaining_start-d_time_counter);
	    p->drawText(550,75,tr("Time")+" "+track_time.toString("h:mm:ss"));
	  }                  
	  p->end();
	  delete p;
	}
	break;

      default:
	if(!d_wave_map[1]->isNull()) {
	  d_wpg[1]->begin(d_wave_map[1]);
	  d_wpg[1]->setFont(labelFont());
	  d_wpg[1]->setPen(TRACKER_TEXT_COLOR);
	  d_wpg[1]->setBackground(back_color);
	  d_wpg[1]->eraseRect(0,0,d_wave_map[1]->size().width(),
			      d_wave_map[1]->size().height());
	  if(!d_wave_name[1].isEmpty()) {
	    d_wpg[1]->drawWaveByMsecs(0,d_wave_map[1]->width(),
				      d_wave_origin[1],
				      d_wave_origin[1]+d_time_width,800,
				      RDWavePainter::Mono,Qt::black,
				      d_loglines[1]->
				      startPoint(RDLogLine::CartPointer),
				      d_loglines[1]->
				      endPoint(RDLogLine::CartPointer));
	  }
	  if(d_track_line>=0) {

	    //
	    // Draw Segue Markers
	    //
	    if(d_loglines[1]->segueStartPoint(RDLogLine::CartPointer)>=0) {
	      xpos=(d_loglines[1]->segueStartPoint(RDLogLine::CartPointer)-
		    d_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	      DrawCursor(d_wpg[1],d_wave_map[1]->height(),xpos,
			 RD_SEGUE_MARKER_COLOR,20,true);
	      xpos=(d_loglines[1]->segueEndPoint(RDLogLine::CartPointer)-
		    d_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	      DrawCursor(d_wpg[1],d_wave_map[1]->height(),xpos,
			 RD_SEGUE_MARKER_COLOR,20,false);
	    }

	    //
	    // Draw Fadeup Marker
	    //
	    if(d_loglines[1]->fadeupPoint(RDLogLine::CartPointer)>=0) {
	      xpos=(d_loglines[1]->fadeupPoint(RDLogLine::CartPointer)-
		    d_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	      DrawCursor(d_wpg[1],d_wave_map[1]->height(),xpos,
			 RD_FADE_MARKER_COLOR,30,false);
	    }
               
	    //
	    // Draw Start Marker
	    //
	    xpos=(d_loglines[1]->startPoint(RDLogLine::CartPointer)-
		  d_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(d_wpg[1],d_wave_map[1]->height(),xpos,
		       RD_START_END_MARKER_COLOR,10,true);

	    //
	    // Draw Fadedown Marker
	    //
	    if(d_loglines[1]->fadedownPoint(RDLogLine::CartPointer)>=0) {
	      xpos=(d_loglines[1]->fadedownPoint(RDLogLine::CartPointer)-
		    d_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	      DrawCursor(d_wpg[1],d_wave_map[1]->height(),xpos,
			 RD_FADE_MARKER_COLOR,30,true);
	    }
              
	    //
	    // Draw End Marker
	    //
	    xpos=(d_loglines[1]->endPoint(RDLogLine::CartPointer)-
		  d_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(d_wpg[1],d_wave_map[1]->height(),xpos,
		       RD_START_END_MARKER_COLOR,10,false);

	    //
	    // Draw Rubber Bands
	    //
	    DrawRubberBand(d_wpg[1],1);

	    //
	    // Draw Menu Marker
	    //
	    if(d_menu_clicked_point>=0 && d_rightclick_track==1) {
	      DrawCursor(d_wpg[1],d_wave_map[1]->height(),d_menu_clicked_point,
			 RD_START_END_MARKER_COLOR,10,true);
	      DrawCursor(d_wpg[1],d_wave_map[1]->height(),d_menu_clicked_point,
			 RD_START_END_MARKER_COLOR,10,false);
	    }       
	    d_wpg[1]->setPen(TRACKER_TEXT_COLOR);
	    d_wpg[1]->drawText(5,14,d_loglines[1]->title());
	  }
	  if(d_start_time>QTime(0,0,0)) {
	    d_wpg[1]->drawText(550,75,tr("Start")+" "+
			       d_start_time.toString("h:mm:ss"));
	  }
	  d_wpg[1]->end();
	}
	break;
      }
    }
    break;

  case 2:
    if(d_wave_name[2].isEmpty()&&(!d_wave_map[2]->isNull())) {
      p=new QPainter(d_wave_map[2]);
      p->setBackground(Qt::gray);
      p->eraseRect(0,0,d_wave_map[2]->size().width(),
		   d_wave_map[2]->size().height());
      p->end();
      delete p;
    }
    else {
      if((d_loglines[2]->transType()==RDLogLine::Segue)) {
	back_color=palette().color(QPalette::Background);
      }
      else {
	back_color=Qt::lightGray;
      }
      if(!d_wave_map[2]->isNull()) {
	d_wpg[2]->begin(d_wave_map[2]);
	d_wpg[2]->setFont(labelFont());
	d_wpg[2]->setBackground(back_color);
	d_wpg[2]->eraseRect(0,0,d_wave_map[2]->size().width(),
			    d_wave_map[2]->size().height());
	if(!d_wave_name[2].isEmpty()) {
	  d_wpg[2]->
	    drawWaveByMsecs(0,d_wave_map[2]->width(),d_wave_origin[2],
			    d_wave_origin[2]+d_time_width,800,
			    RDWavePainter::Mono,Qt::black,
			    d_loglines[2]->startPoint(RDLogLine::CartPointer),
			    d_loglines[2]->endPoint(RDLogLine::CartPointer));
	  //
	  // Draw Talk Markers
	  //
	  if(d_loglines[2]->talkEndPoint()>0) {
	    int tsxpos;
	    int texpos;
	    if(d_loglines[2]->talkStartPoint()==0){
	      tsxpos=(d_loglines[2]->startPoint()-
		      d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	    }
	    else {
	      tsxpos=(d_loglines[2]->talkStartPoint()-
		      d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	    }
	    if(d_loglines[2]->talkEndPoint()==0) {
	      texpos=(d_loglines[2]->startPoint()-
		      d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	    }
	    else {
	      if(d_loglines[2]->talkStartPoint()==0) {
		texpos=(d_loglines[2]->startPoint()+
			d_loglines[2]->talkEndPoint()-
			d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	      }
	      else {
		texpos=(d_loglines[2]->talkEndPoint()-
			d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	      }
	    }
	    if(tsxpos!=texpos){
	      DrawCursor(d_wpg[2],d_wave_map[2]->height(),tsxpos,
			 RD_TALK_MARKER_COLOR,20,true);
	      DrawCursor(d_wpg[2],d_wave_map[2]->height(),texpos,
			 RD_TALK_MARKER_COLOR,20,false);
	    }
	  }

	  //
	  // Draw Fadeup Marker
	  //
	  if(d_loglines[2]->fadeupPoint(RDLogLine::CartPointer)>=0) {
	    xpos=(d_loglines[2]->fadeupPoint(RDLogLine::CartPointer)-
		  d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(d_wpg[2],d_wave_map[2]->height(),xpos,
		       RD_FADE_MARKER_COLOR,30,false);
	  }
	      	         
	  //
	  // Draw Start Marker
	  //
	  xpos=(d_loglines[2]->startPoint(RDLogLine::CartPointer)-
		d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(d_wpg[2],d_wave_map[2]->height(),xpos,
		     RD_START_END_MARKER_COLOR,10,true);

	  //
	  // Draw End Marker
	  //
	  xpos=(d_loglines[2]->endPoint(RDLogLine::CartPointer)-
		d_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(d_wpg[2],d_wave_map[2]->height(),xpos,
		     RD_START_END_MARKER_COLOR,10,false);

	  //
	  // Draw Menu Marker
	  //
	  if(d_menu_clicked_point>=0 && d_rightclick_track==2) {
	    DrawCursor(d_wpg[2],d_wave_map[2]->height(),d_menu_clicked_point,
		       RD_START_END_MARKER_COLOR,10,true);
	    DrawCursor(d_wpg[2],d_wave_map[2]->height(),d_menu_clicked_point,
		       RD_START_END_MARKER_COLOR,10,false);
	  }       
 
	  //
	  // Draw Rubber Bands
	  //
	  DrawRubberBand(d_wpg[2],2);
	}
	d_wpg[2]->setPen(TRACKER_TEXT_COLOR);
	d_wpg[2]->drawText(5,14,d_loglines[2]->
			   resolveWildcards(rda->logeditConf()->waveformCaption()));
	if(d_recording && (d_deck[2]->state()==RDPlayDeck::Playing ||
			   d_deck[2]->state()==RDPlayDeck::Stopping)) {
	  int talk_len=d_loglines[2]->talkLength();
	  if(talk_len>0 && d_deck[2]->currentPosition()>
	     d_loglines[2]->talkStartPoint()){
	    talk_len-=d_deck[2]->currentPosition()-
	      d_loglines[2]->talkStartPoint();
	  }
	  d_wpg[2]->drawText(550,75,tr("Talk")+
			     QString::asprintf(" :%d",(talk_len+500)/1000));
	}
	else {
	  d_wpg[2]->drawText(550,75,tr("Talk")+QString().
			     sprintf(" :%d",(d_loglines[2]->talkLength()+500)/1000));
	}
	d_wpg[2]->end();
      }
    }
    break;
  }
}


void RDTrackerWidget::DrawCursor(RDWavePainter *wp,int height,int xpos,
			      const QColor &color,int arrow_offset,
			      bool left_arrow)
{
  wp->setPen(color);
  wp->setBrush(color);
  wp->drawLine(xpos,0,xpos,height);
  int arrow_x=8;
  if(left_arrow) {
    arrow_x=-8;
  }
  QPolygon *pa=new QPolygon(3);
  pa->setPoint(0,xpos,arrow_offset);
  pa->setPoint(1,xpos+arrow_x,arrow_offset-5);
  pa->setPoint(2,xpos+arrow_x,arrow_offset+5);
  wp->drawPolygon(*pa);
  pa->setPoint(0,xpos,height-arrow_offset);
  pa->setPoint(1,xpos+arrow_x,height-(arrow_offset-5));
  pa->setPoint(2,xpos+arrow_x,height-(arrow_offset+5));
  wp->drawPolygon(*pa);
  delete pa;
}


void RDTrackerWidget::DrawRubberBand(RDWavePainter *wp,int trackno)
{
  int xpos=0;
  int xstart=0;
  int ypos=0;
  int ystart=0;

  wp->setPen(TRACKER_RUBBERBAND_COLOR);
  wp->setBrush(TRACKER_RUBBERBAND_COLOR);
  
  int draw_fadeup_gain=d_loglines[trackno]->fadeupGain();
  int draw_fadedown_gain=d_loglines[trackno]->fadedownGain();
  int draw_fadedown_point=d_loglines[trackno]->fadedownPoint();
  int draw_duckup_gain=d_loglines[trackno]->duckUpGain();
  int draw_duckdown_gain=d_loglines[trackno]->duckDownGain();
  int draw_end_point=d_loglines[trackno]->endPoint();
  if(d_loglines[trackno]->fadedownPoint(RDLogLine::LogPointer)<0) {
     draw_fadedown_gain=RD_FADE_DEPTH;
  }
  if(d_loglines[trackno]->fadeupPoint(RDLogLine::LogPointer)<0) {
     draw_fadeup_gain=RD_FADE_DEPTH;
  }

  if(draw_fadeup_gain<TRACKER_MIN_GAIN)
    draw_fadeup_gain=TRACKER_MIN_GAIN;
  if(draw_fadedown_gain<TRACKER_MIN_GAIN)
    draw_fadedown_gain=TRACKER_MIN_GAIN;
  if(draw_duckup_gain<TRACKER_MIN_GAIN)
    draw_duckup_gain=TRACKER_MIN_GAIN;
  if(draw_duckdown_gain<TRACKER_MIN_GAIN)
    draw_duckdown_gain=TRACKER_MIN_GAIN;
  if(d_loglines[trackno]->fadeupPoint()<=d_loglines[trackno]->startPoint()
     && draw_fadeup_gain==0)
    draw_fadeup_gain=TRACKER_MIN_GAIN;
  if(d_loglines[trackno]->fadedownPoint()<0 && draw_fadedown_gain==0)
    draw_fadedown_gain=TRACKER_MIN_GAIN;
  if(d_loglines[trackno]->fadedownPoint()>=d_loglines[trackno]->endPoint() 
     && draw_fadedown_gain==0)
    draw_fadedown_gain=TRACKER_MIN_GAIN;
  if(d_loglines[trackno]->segueGain()<0
        && d_loglines[trackno]->segueStartPoint()>=0
        && d_loglines[trackno]->segueStartPoint()<draw_fadedown_point
        && (!d_loaded)) {
    if(trackno<2) {
      if((d_loglines[trackno+1]!=NULL)&&
	 (d_loglines[trackno+1]->transType()==RDLogLine::Segue)) {
        draw_fadedown_point=d_loglines[trackno]->segueStartPoint();
        draw_fadedown_gain=d_loglines[trackno]->segueGain();
      }
    }
  }      
  if(trackno<2) {
    if((d_loglines[trackno+1]!=NULL)&&
       (d_loglines[trackno+1]->transType()==RDLogLine::Segue)
        && (!d_loaded)) {
       draw_end_point=d_loglines[trackno]->segueEndPoint();
       if(draw_fadedown_point>draw_end_point) {
         draw_fadedown_point=draw_end_point;
       }
     } 
   }
  
  //
  // Fade Up
  //
  xpos=(d_loglines[trackno]->startPoint()-
	d_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  if(trackno==0) {
    ypos=TRACKER_GAIN_MARGIN-
        draw_duckdown_gain/TRACKER_MB_PER_PIXEL-
        (int)((double)draw_fadeup_gain*
        (1-(double)draw_duckdown_gain/TRACKER_MIN_GAIN))
         /TRACKER_MB_PER_PIXEL;
  }
  else {
    ypos=TRACKER_GAIN_MARGIN-
        draw_duckup_gain/TRACKER_MB_PER_PIXEL-
        (int)((double)draw_fadeup_gain*
        (1-(double)draw_duckup_gain/TRACKER_MIN_GAIN))
         /TRACKER_MB_PER_PIXEL;
  }
  xstart=xpos;
  ystart=ypos;
  switch(trackno) {
  case 1:
    DrawTarget(wp,RDTrackerWidget::TrackFadeupGain,trackno,xpos,ypos);
    break;
	
  case 2:
    DrawTarget(wp,RDTrackerWidget::FadeupGain,trackno,xpos,ypos);
    break;
  }
  xpos=(d_loglines[trackno]->fadeupPoint()-
	d_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  switch(trackno) {
  case 0:
    ypos=TRACKER_GAIN_MARGIN-
      draw_duckdown_gain/TRACKER_MB_PER_PIXEL;
    break;

  case 1:
    ypos=TRACKER_GAIN_MARGIN;
    break;

  case 2:
    ypos=TRACKER_GAIN_MARGIN-
      draw_duckup_gain/TRACKER_MB_PER_PIXEL;
    break;
  }
  wp->drawLine(xstart,ystart,xpos,ypos);
  switch(trackno) {
  case 1:
    DrawTarget(wp,RDTrackerWidget::TrackFadeupPoint,trackno,xpos,ypos);
    break;
	
  case 2:
    DrawTarget(wp,RDTrackerWidget::FadeupPoint,trackno,xpos,ypos);
    break;
  }

  //
  // Body of cut
  //
  xpos=(d_loglines[trackno]->fadeupPoint()-
	d_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  xstart=xpos;
  ystart=ypos;
  if(draw_fadedown_point>=d_loglines[trackno]->fadeupPoint())
  xpos=(draw_fadedown_point-
	d_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  else
    xpos=(draw_end_point-
	 d_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;

  wp->drawLine(xstart,ystart,xpos,ypos);
  xstart=xpos;
  ystart=ypos;

  //
  // Fade Down
  switch(trackno) {
  case 0:
    DrawTarget(wp,RDTrackerWidget::FadedownPoint,trackno,xpos,ypos);
    break;
	
  case 1:
    DrawTarget(wp,RDTrackerWidget::TrackFadedownPoint,trackno,xpos,ypos);
    break;
  }
  xpos=(draw_end_point-
	d_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  if(trackno==2) {
  ypos=TRACKER_GAIN_MARGIN-
        draw_duckup_gain/TRACKER_MB_PER_PIXEL-
        (int)((double)draw_fadedown_gain*
        (1-(double)draw_duckup_gain/TRACKER_MIN_GAIN))
         /TRACKER_MB_PER_PIXEL;
  }
  else {
    ypos=TRACKER_GAIN_MARGIN-
        d_loglines[trackno]->duckDownGain()/TRACKER_MB_PER_PIXEL-
        (int)((double)draw_fadedown_gain*
        (1-(double)draw_duckdown_gain/TRACKER_MIN_GAIN))
         /TRACKER_MB_PER_PIXEL;
  }
  wp->drawLine(xstart,ystart,xpos,ypos);
  switch(trackno) {
  case 0:
    DrawTarget(wp,RDTrackerWidget::FadedownGain,trackno,xpos,ypos);
    break;
	
  case 1:
    DrawTarget(wp,RDTrackerWidget::TrackFadedownGain,trackno,xpos,ypos);
    break;
  }
}


void RDTrackerWidget::DrawTarget(RDWavePainter *wp,RDTrackerWidget::Target target,
			      int trackno,int xpos,int ypos)
{
  d_target_rect[target]->setX(xpos-TRACKER_GAIN_MARGIN);
  d_target_rect[target]->setY(ypos-TRACKER_GAIN_MARGIN);
  d_target_rect[target]->setWidth(TRACKER_GAIN_MARGIN*2);
  d_target_rect[target]->setHeight(TRACKER_GAIN_MARGIN*2);
  d_target_track[target]=trackno;
  wp->drawRect(*(d_target_rect[target]));
}


bool RDTrackerWidget::TransportActive()
{
  return d_recording||PlayoutActive();
}


bool RDTrackerWidget::PlayoutActive()
{
  for(int i=0;i<3;i++) {
    if((d_deck[i]->state()==RDPlayDeck::Playing)||
       (d_deck[i]->state()==RDPlayDeck::Stopping)) {
      return true;
    }
  }
  return false;
}


void RDTrackerWidget::UpdateControls()
{
  int line=SingleSelectionLine();
  bool transport_idle=!TransportActive();

  if((line<0)||(line==TRACKER_MAX_LINENO)) {
    d_track1_button->setDisabled(true);
    d_track1_button->setText(tr("Start"));
    d_track1_button->setPalette(d_start_palette);
    d_record_button->setDisabled(true);
    d_record_button->setText(tr("Record"));
    d_track2_button->setDisabled(true);
    d_finished_button->setDisabled(true);
    d_reset_button->setDisabled(true);
    d_post_button->setDisabled(true);
    d_play_button->setDisabled(true);
    d_stop_button->setDisabled(true);
    d_next_button->setEnabled(transport_idle);
    d_previous_button->setEnabled(transport_idle);
    d_insert_button->setEnabled(transport_idle&&CanInsertTrack());
    d_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
    d_log_view->setEnabled(transport_idle);
    return;
  }
  RDLogLine *real_logline=d_log_model->logLine(line);
  RDLogLine *logline=NULL;
  if(d_offset) {
    logline=d_log_model->logLine(line-1);
  }
  else {
    logline=real_logline;
  }
  if(d_loaded) {
    if(logline->type()==RDLogLine::Track) {  // Unfinished Track
      switch(d_deck_state) {
      case RDTrackerWidget::DeckIdle:
	if(d_wave_name[0].isEmpty()) {
	  d_track1_button->setEnabled(!d_group->name().isEmpty());
	  d_track1_button->setText(tr("Import"));
	  d_track1_button->setPalette(d_record_palette);
	  d_record_button->setEnabled(!d_group->name().isEmpty());
	  d_record_button->setText(tr("Record"));
	  if(d_focus_policy!=Qt::NoFocus) {
	    d_record_button->setFocus();
	  }
	}
	else {
	  if((logline->transType()==RDLogLine::Segue)) {
	    d_track1_button->
	      setEnabled(!d_group->name().isEmpty());
	    d_track1_button->setText(tr("Start"));
	    d_track1_button->setPalette(d_start_palette);
	    if(d_focus_policy!=Qt::NoFocus) {
	      d_track1_button->setFocus();
	    }
	    d_record_button->
	      setEnabled(!d_group->name().isEmpty());
	    d_record_button->setText(tr("Import"));
	  }
	  else {
	    d_track1_button->
	      setEnabled(!d_group->name().isEmpty());
	    d_track1_button->setText(tr("Import"));
	    d_track1_button->setPalette(d_record_palette);
	    d_record_button->
	      setEnabled(!d_group->name().isEmpty());
	    d_record_button->setText(tr("Record"));
	    if(d_focus_policy!=Qt::NoFocus) {
	      d_record_button->setFocus();
	    }
	  }
	}
	d_track2_button->setDisabled(true);
	d_finished_button->setPalette(d_done_palette);
	d_finished_button->setText(tr("Save"));
	d_finished_button->setEnabled(d_changed);
	d_reset_button->
	  setEnabled(real_logline->hasCustomTransition());
	d_post_button->setDisabled(true);
	d_play_button->setEnabled(true);
	d_stop_button->setEnabled(true);
	d_next_button->setEnabled(transport_idle);
	d_previous_button->setEnabled(transport_idle);
	d_insert_button->setEnabled(transport_idle&&CanInsertTrack());
	d_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
	d_log_view->setEnabled(transport_idle);
	break;

      case RDTrackerWidget::DeckTrack1:
	d_track1_button->setDisabled(true);
	d_track1_button->setText(tr("Start"));
	d_track1_button->setPalette(d_start_palette);
	d_record_button->setEnabled(true);
	d_record_button->setText(tr("Record"));
	if(d_focus_policy!=Qt::NoFocus) {
	  d_record_button->setFocus();
	}
	d_track2_button->setDisabled(true);
	d_finished_button->setPalette(d_abort_palette);
	d_finished_button->setText(tr("Abort"));
	d_finished_button->setEnabled(true);
	d_reset_button->setDisabled(true);
	d_post_button->setDisabled(true);
	d_play_button->setDisabled(true);
	d_stop_button->setDisabled(true);
	d_next_button->setDisabled(true);
	d_previous_button->setDisabled(true);
	d_insert_button->setDisabled(true);
	d_delete_button->setDisabled(true);
	d_log_view->setDisabled(true);
	break;
	    
      case RDTrackerWidget::DeckTrack2:
	d_track1_button->setDisabled(true);
	d_track1_button->setText(tr("Start"));
	d_track1_button->setPalette(d_start_palette);
	d_record_button->setDisabled(true);
	d_record_button->setText(tr("Record"));
	if(d_wave_name[2].isEmpty()) {
	  d_finished_button->setPalette(d_done_palette);
	  d_finished_button->setText(tr("Save"));
	  if(d_focus_policy!=Qt::NoFocus) {
	    d_finished_button->setFocus();
	  }
	  d_track2_button->setDisabled(true);
	}
	else {
	  if((d_loglines[2]->transType()==RDLogLine::Segue)) {
	    if(rda->logeditConf()->enableSecondStart()) {
	      d_finished_button->setPalette(d_abort_palette);
	      d_finished_button->setText(tr("Abort"));
	    }
	    else {
	      d_finished_button->setPalette(d_done_palette);
	      d_finished_button->setText(tr("Save"));
	    }
	    d_track2_button->setEnabled(true);
	  }
	  else {
	    d_finished_button->setText(tr("Save"));
	    d_finished_button->setPalette(d_done_palette);
	    d_track2_button->setDisabled(true);
	  }
	  if(d_focus_policy!=Qt::NoFocus) {
	    d_track2_button->setFocus();
	  }
	}
	d_finished_button->setEnabled(true);
	d_reset_button->setDisabled(true);
	d_insert_button->setDisabled(true);
	d_delete_button->setDisabled(true);
	d_post_button->setDisabled(true);
	d_play_button->setDisabled(true);
	d_stop_button->setDisabled(true);
	d_next_button->setDisabled(true);
	d_previous_button->setDisabled(true);
	d_log_view->setDisabled(true);
	break;
	    
      case RDTrackerWidget::DeckTrack3:
	d_track1_button->setDisabled(true);
	d_track1_button->setText(tr("Start"));
	d_track1_button->setPalette(d_start_palette);
	d_record_button->setDisabled(true);
	d_record_button->setText(tr("Record"));
	d_track2_button->setDisabled(true);
	d_finished_button->setPalette(d_done_palette);
	d_finished_button->setText(tr("Save"));
	d_finished_button->setEnabled(true);
	if(d_focus_policy!=Qt::NoFocus) {
	  d_finished_button->setFocus();
	}
	d_reset_button->setDisabled(true);
	d_post_button->setDisabled(true);
	d_insert_button->setDisabled(true);
	d_delete_button->setDisabled(true);
	d_play_button->setDisabled(true);
	d_stop_button->setDisabled(true);
	d_next_button->setDisabled(true);
	d_previous_button->setDisabled(true);
	d_log_view->setDisabled(true);
	break;
      }
    }
    else {         // Completed Track
      d_track1_button->setDisabled(true);
      d_track1_button->setText(tr("Start"));
      d_track1_button->setPalette(d_start_palette);
      d_record_button->setDisabled(true);
      d_record_button->setText(tr("Record"));
      d_track2_button->setDisabled(true);
      d_finished_button->setEnabled(d_changed);
      d_reset_button->setEnabled(transport_idle);
      d_post_button->
	setEnabled(transport_idle&&(!d_wave_name[2].isEmpty()));
      d_play_button->setEnabled(true);
      d_stop_button->setEnabled(true);
      d_next_button->setEnabled(transport_idle);
      d_previous_button->setEnabled(transport_idle);
      if(transport_idle&&(d_focus_policy!=Qt::NoFocus)) {
	d_next_button->setFocus();
      }
      d_insert_button->setEnabled(transport_idle&&CanInsertTrack());
      d_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
      d_log_view->setEnabled(transport_idle);
    }
  }
  else {             // Straight Segue
    d_track1_button->setDisabled(true);
    d_track1_button->setText(tr("Start"));
    d_track1_button->setPalette(d_start_palette);
    d_record_button->setDisabled(true);
    d_record_button->setText(tr("Record"));
    d_track2_button->setDisabled(true);
    d_finished_button->setEnabled(d_changed);
    if(real_logline!=NULL) {
      d_reset_button->setEnabled(real_logline->hasCustomTransition());
    }
    d_post_button->setDisabled(true);
    d_play_button->setEnabled(true);
    d_stop_button->setEnabled(true);
    d_next_button->setEnabled(transport_idle);
    d_previous_button->setEnabled(transport_idle);
    if(transport_idle&&(d_focus_policy!=Qt::NoFocus)) {
      d_next_button->setFocus();
    }
    d_insert_button->setEnabled(transport_idle&&CanInsertTrack());
    d_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
    d_log_view->setEnabled(transport_idle);
  }
  update();
}


void RDTrackerWidget::UpdateRemaining()
{
  d_tracks_remaining_label->setText(QString::asprintf("%d",d_tracks));
  if(d_block_valid) {
    d_time_remaining_label->
      setText(RDGetTimeLength(d_time_remaining,true,true));
    if(d_time_remaining>=0) {
      d_time_remaining_label->setPalette(d_time_remaining_palette[0]);
    }
    else {
      d_time_remaining_label->setPalette(d_time_remaining_palette[1]);
    }
  }
  else {
    d_time_remaining_label->setText("-:--:--.-");
    d_time_remaining_label->setPalette(d_time_remaining_palette[0]);
  }
}


bool RDTrackerWidget::TrackAvailable()
{
  if(!d_group->exists()) {
    return false;
  }
  if((d_group->freeCartQuantity()<=0)&&d_wave_name[1].isEmpty()) {
    return false;
  }
  return true;
}


void RDTrackerWidget::LogLine(const QString &line)
{
  fprintf(stderr,"%s: %s\n",
	  QTime::currentTime().toString("hh:mm:ss.zzz").toUtf8().constData(),
	  line.toUtf8().constData());
}


bool RDTrackerWidget::InitTrack()
{
  int cutnum;
  QString err_msg;

  d_recording=false;
  d_record_ran=false;
  d_recording_pos=0;
  d_aborting=false;
  if(d_track_cart!=NULL) {
    delete d_track_cart;
    d_track_cart=NULL;
  }
  unsigned next_cart=0;

  if((next_cart=RDCart::create(d_group->name(),RDCart::Audio,&err_msg))==0) {
    return false;
  }

  d_track_cart=new RDCart(next_cart);
  d_track_cart->setOwner(d_log->name());
  d_track_cart->setTitle(d_loglines[1]->markerComment().trimmed());
  if(d_track_cuts[1]!=NULL) {
    delete d_track_cuts[1];
  }
  if((cutnum=d_track_cart->addCut(d_format,d_bitrate,d_chans))<0) {
    QMessageBox::warning(this,tr("RDLogEdit - Voice Tracker"),
			 tr("This cart cannot contain any additional cuts!"));
    return false;
  }
  d_track_cuts[1]=new RDCut(d_track_cart->number(),cutnum);
  switch(d_format) {
  case 0:
    d_coding=RDCae::Pcm16;
    break;
	
  case 1:
    d_coding=RDCae::MpegL2;
    break;
	
  case 2:
    d_coding=RDCae::Pcm24;
    break;
	
  default:
    d_coding=RDCae::Pcm16;
    break;
  }
  d_deck_state=RDTrackerWidget::DeckTrack1;
  d_sliding=false;
  for(unsigned i=0;i<3;i++) {
    d_segue_start_point[i]=-1;
  }
  d_loglines[1]->
    setFadeupPoint(d_loglines[1]->startPoint(),RDLogLine::LogPointer);
  d_loglines[1]->setFadedownGain(RD_FADE_DEPTH);
  d_loglines[1]->
    setFadedownPoint(d_loglines[1]->endPoint(),RDLogLine::LogPointer);
  d_loglines[1]->setFadeupGain(RD_FADE_DEPTH);
  d_tracks--;
  SendNotification(RDNotification::AddAction,next_cart);

  return true;
}


void RDTrackerWidget::FinishTrack()
{
  d_loglines[0]->
	setSegueStartPoint(d_segue_start_point[0],RDLogLine::LogPointer);
  d_loglines[0]->setSegueGain(0);
  d_loglines[0]->
    setAverageSegueLength(d_loglines[0]->segueStartPoint()-
			  d_loglines[0]->startPoint());
  if(!d_wave_name[1].isEmpty()) {
    rda->cae()->stopRecord(d_input_card,d_input_port);
    d_loglines[1]->setEndPoint(-1,RDLogLine::LogPointer);
  }
  stopData();
  d_deck_state=RDTrackerWidget::DeckIdle;
}


double RDTrackerWidget::GetCurrentTime()
{
  struct timeval tv;

  memset(&tv,0,sizeof(tv));
  gettimeofday(&tv,NULL);
  return (double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
}


bool RDTrackerWidget::IsTrack(int line,bool *offset)
{
  *offset=false;
  if(d_log_model->logLine(line)==NULL) {
    return false;
  }
  if((d_log_model->logLine(line)->type()==RDLogLine::Track)||
     (d_log_model->logLine(line)->source()==RDLogLine::Tracker)) {
    return true;
  }
  if(d_log_model->logLine(line-1)==NULL) {
    return false;
  }
  return false;
}


bool RDTrackerWidget::CanInsertTrack()
{
  int line;
  bool state=false;

  if((line=SingleSelectionLine())<0) {
    return false;
  }
  if(line==TRACKER_MAX_LINENO) {
    if(d_log_model->lineCount()<=0) {
      return true;
    }
    return d_log_model->logLine(d_log_model->lineCount()-1)->type()
      !=RDLogLine::Track;
  }
  if(d_log_model->logLine(line)!=NULL) {
    state=d_log_model->logLine(line)->type()==RDLogLine::Track;
  }
  if((d_log_model->logLine(line-1)!=NULL)&&(line>0)) {
    state=state||
      (d_log_model->logLine(line-1)->type()==RDLogLine::Track);
  }

  return !state;
}


bool RDTrackerWidget::CanDeleteTrack()
{
  int line=SingleSelectionLine();
  bool ret=false;

  if((line<0)||(line==TRACKER_MAX_LINENO)) {
    return false;
  }
  if(d_log_model->logLine(line)!=NULL) {
    ret=d_log_model->logLine(line)->type()==RDLogLine::Track;
  }

  return ret;
}


void RDTrackerWidget::CheckChanges()
{
  if(d_changed) {
    switch(QMessageBox::question(this,tr("Segue Changed"),
				 tr("Save segue changes?"),
				 QMessageBox::Yes,QMessageBox::No)) {
    case QMessageBox::Yes:
      finishedData();
      break;

    case QMessageBox::No:
    case QMessageBox::NoButton:
      PopSegues();
      break;
    }
  }
  d_changed=false;
}


void RDTrackerWidget::PushSegues()
{
  for(unsigned i=0;i<3;i++) {
    if(d_loglines[i]!=NULL) {
      *(d_saved_loglines[i])=*(d_loglines[i]);
    }
  }
}


void RDTrackerWidget::PopSegues()
{
  for(unsigned i=0;i<3;i++) {
    if(d_loglines[i]!=NULL) {
      *(d_loglines[i])=*(d_saved_loglines[i]);
    }
  }
}


int RDTrackerWidget::SingleSelectionLine(bool incl_end_handle)
{
  int offset=-1;
  if(incl_end_handle) {
    offset=0;
  }
  QItemSelectionModel *sel=d_log_view->selectionModel();
  if((sel->selectedRows().size()==1)&&
     (sel->selectedRows().at(0).row()<(d_log_model->rowCount()-offset))) {
    return sel->selectedRows().at(0).row();
  }
  return -1;
}


void RDTrackerWidget::SendNotification(RDNotification::Action action,
				    const QString &log_name)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,
					    action,QVariant(log_name));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}


void RDTrackerWidget::SendNotification(RDNotification::Action action,
				    unsigned cartnum)
{
  RDNotification *notify=new RDNotification(RDNotification::CartType,
					    action,QVariant(cartnum));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}
