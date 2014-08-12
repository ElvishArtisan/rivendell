// voice_tracker.cpp
//
// A Rivendell Voice Tracker
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: voice_tracker.cpp,v 1.84.2.4.2.2 2014/05/21 18:19:43 cvs Exp $
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

#include <qdialog.h>
#include <qpushbutton.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qapplication.h>

#include <rdconf.h>
#include <rdcart.h>
#include <rd.h>
#include <rdlog.h>
#include <rdsvc.h>
#include <rdlogedit_conf.h>
#include <rdlibrary_conf.h>
#include <rdedit_audio.h>
#include <rdimport_audio.h>
#include <rdwavedata.h>

#include <globals.h>
#include <voice_tracker.h>
#include <import_track.h>
#include <edit_track.h>

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/marker.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/music.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/traffic.xpm"
#include "../icons/mic16.xpm"


VoiceTracker::VoiceTracker(const QString &logname,QString *import_path,
			   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_log_name=logname;
  edit_import_path=import_path;
  edit_coding=RDCae::Pcm16;
  edit_track_cart=NULL;
  edit_sliding=false;
  edit_scrolling=false;
  track_line=-1;
  track_loaded=false;
  track_offset=false;
  segue_loaded=false;
  track_recording=false;
  track_changed=false;
  track_recording_pos=0;
  track_record_ran=false;
  track_aborting=false;
  track_block_valid=false;
  track_time_remaining=0;
  track_time_counter=0;
  track_start_time=QTime(0,0,0);
  track_size_altered=false;
  for(unsigned i=0;i<3;i++) {
    edit_scroll_pos[i]=-1;
    edit_track_line[i]=-1;
    edit_segue_start_offset[i]=0;
    edit_track_cuts[i]=NULL;
    wpg[i]=NULL;
  }
  menu_clicked_point=-1;
  edit_shift_pressed=false;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumWidth(sizeHint().width());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Voice Tracker"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont label_font=QFont("Hevetica",12,QFont::Normal);
  label_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);
  QFont timer_font=QFont("Helvetica",20,QFont::Bold);
  timer_font.setPixelSize(20);

  //
  // Create Icons
  //
  edit_playout_map=new QPixmap(play_xpm);
  edit_macro_map=new QPixmap(rml5_xpm);
  edit_marker_map=new QPixmap(marker_xpm);
  edit_chain_map=new QPixmap(chain_xpm);
  edit_track_cart_map=new QPixmap(track_cart_xpm);
  edit_music_map=new QPixmap(music_xpm);
  edit_mic16_map=new QPixmap(mic16_xpm);
  edit_notemarker_map=new QPixmap(notemarker_xpm);
  edit_traffic_map=new QPixmap(traffic_xpm);

  //
  // Create Palettes
  //
  track_record_palette=QPalette(TRACKER_RECORD_BUTTON_COLOR,backgroundColor());
  track_start_palette=QPalette(TRACKER_START_BUTTON_COLOR,backgroundColor());
  track_done_palette=QPalette(TRACKER_DONE_BUTTON_COLOR,backgroundColor());
  track_abort_palette=QPalette(TRACKER_ABORT_BUTTON_COLOR,backgroundColor());

  //
  // Create Track and Target Region
  //
  track_track_rect=new QRect(0,0,TRACKER_X_WIDTH,3*TRACKER_Y_HEIGHT);
  for(int i=0;i<3;i++) {
  track_trackzones_rect[i]=
    new QRect(0,i*TRACKER_Y_HEIGHT,TRACKER_X_WIDTH,TRACKER_Y_HEIGHT);
  }
  for(unsigned i=0;i<VoiceTracker::TargetSize;i++) {
    track_target_rect[i]=new QRect();
  }
  track_current_target=VoiceTracker::TargetSize;

  //
  // Create Cursors
  //
  track_arrow_cursor=new QCursor(Qt::ArrowCursor);
  track_hand_cursor=new QCursor(Qt::PointingHandCursor);
  track_cross_cursor=new QCursor(Qt::CrossCursor);
  track_current_cursor=track_arrow_cursor;
  setMouseTracking(true);

  //
  // Macro Event Player
  //
  track_event_player=new RDEventPlayer(rdripc,this,"track_event_player");

  //
  // Waveform Pixmaps
  //
  for(int i=0;i<3;i++) {
    edit_wave_map[i]=new QPixmap(TRACKER_X_WIDTH,77);
  }
  edit_previous_point=new QPoint(-1,-1);
  edit_current_track=-1;

  //
  // Logline Dummies
  //
  track_dummy0_logline=new RDLogLine();
  track_dummy2_logline=new RDLogLine();
  for(unsigned i=0;i<3;i++) {
    edit_saved_logline[i]=new RDLogLine();
  }

  //
  // Audio Parameters
  //
  RDLogeditConf *conf=new RDLogeditConf(log_config->stationName());
  edit_input_card=conf->inputCard();
  edit_input_port=conf->inputPort();
  edit_output_card=conf->outputCard();
  edit_output_port=conf->outputPort();
  edit_format=conf->format();
  edit_samprate=rdsystem->sampleRate();
  edit_bitrate=conf->bitrate();
  edit_chans=conf->defaultChannels();
  play_start_macro=conf->startCart();
  play_end_macro=conf->endCart();
  record_start_macro=conf->recStartCart();
  record_end_macro=conf->recEndCart();
  track_preroll=conf->tailPreroll();
  edit_scroll_threshold=TRACKER_X_WIDTH-track_preroll/TRACKER_MSECS_PER_PIXEL;
  edit_settings=new RDSettings();
  conf->getSettings(edit_settings);
  delete conf;

  RDLibraryConf *lconf=new RDLibraryConf(log_config->stationName(),0);
  edit_tail_preroll=lconf->tailPreroll();
  edit_threshold_level=lconf->trimThreshold();
  delete lconf;
  
  //
  // Voicetrack Group
  //
  RDLog *log=new RDLog(logname);
  RDSvc *svc=new RDSvc(log->service());
  track_group=new RDGroup(svc->trackGroup());
  track_tracks=log->scheduledTracks()-log->completedTracks();
  delete svc;
  delete log;

  //
  // Play Decks
  //
  for(int i=0;i<3;i++) {
    edit_deck[i]=new RDPlayDeck(rdcae,i);
    edit_deck[i]->setCard(edit_output_card);
    edit_deck[i]->setPort(edit_output_port);
    connect(edit_deck[i],SIGNAL(stateChanged(int,RDPlayDeck::State)),
	    this,SLOT(stateChangedData(int,RDPlayDeck::State)));
    connect(edit_deck[i],SIGNAL(segueStart(int)),
	    this,SLOT(segueStartData(int)));
  }

  //
  // Record Slot Connections
  //
  connect(rdcae,SIGNAL(recordLoaded(int,int)),
	  this,SLOT(recordLoadedData(int,int)));
  connect(rdcae,SIGNAL(recording(int,int)),
	  this,SLOT(recordingData(int,int)));
  connect(rdcae,SIGNAL(recordStopped(int,int)),
	  this,SLOT(recordStoppedData(int,int)));
  connect(rdcae,SIGNAL(recordUnloaded(int,int,unsigned)),
	  this,SLOT(recordUnloadedData(int,int,unsigned)));

  //
  // Log Machine
  //
  track_log=new RDLog(edit_log_name,false);
  track_log_event=new RDLogEvent(RDLog::tableName(edit_log_name));
  track_log_event->load();

  //
  // Right-Click Menu
  //
  track_menu=new QPopupMenu(this,"track_menu");
  connect(track_menu,SIGNAL(aboutToShow()),this,SLOT(updateMenuData()));
  connect(track_menu,SIGNAL(aboutToHide()),this,SLOT(hideMenuData()));
  track_menu->
    insertItem(tr("Edit Cue Markers"),this,SLOT(editAudioData()),0,0);
  track_menu->
    insertItem(tr("Undo Segue Changes"),this,SLOT(undoChangesData()),0,1);
  track_menu->
    insertItem(tr("Set Start Point Here"),this,SLOT(setStartPointData()),0,2);
  track_menu->
    insertItem(tr("Set End Point Here"),this,SLOT(setEndPointData()),0,3);
  track_menu->
    insertItem(tr("Set to Hook Markers"),this,SLOT(setHookPointData()),0,4);

  //
  // Track 1 Button
  //
  track_track1_button=new QPushButton(this,"track_track1_button");
  track_track1_button->setGeometry(sizeHint().width()-90,15,70,70);
  track_track1_button->setPalette(track_start_palette);
  track_track1_button->setFont(font);
  track_track1_button->setText(tr("Start"));
  connect(track_track1_button,SIGNAL(clicked()),this,SLOT(track1Data()));

  //
  // Record Button
  //
  track_record_button=new QPushButton(this,"track_record_button");
  track_record_button->setGeometry(sizeHint().width()-90,95,70,70);
  track_record_button->setPalette(track_record_palette);
  track_record_button->setFont(font);
  track_record_button->setText(tr("Record"));
  connect(track_record_button,SIGNAL(clicked()),this,SLOT(recordData()));

  //
  // Track 2 Button
  //
  track_track2_button=new QPushButton(this,"track_track2_button");
  track_track2_button->setGeometry(sizeHint().width()-90,175,70,70);
  track_track2_button->setPalette(track_start_palette);
  track_track2_button->setFont(font);
  track_track2_button->setText(tr("Start"));
  connect(track_track2_button,SIGNAL(clicked()),this,SLOT(track2Data()));
  if(!rdlogedit_conf->enableSecondStart()) {
    track_track2_button->hide();
  }

  //
  // Finished Button
  //
  track_finished_button=new QPushButton(this,"track_finished_button");
  if(rdlogedit_conf->enableSecondStart()) {
    track_finished_button->setGeometry(sizeHint().width()-90,255,70,70);
  }
  else {
    track_finished_button->setGeometry(sizeHint().width()-90,175,70,70);
  }
  track_finished_button->setPalette(track_done_palette);
  track_finished_button->setFont(font);
  track_finished_button->setText(tr("Save"));
  connect(track_finished_button,SIGNAL(clicked()),this,SLOT(finishedData()));

  //
  // Previous Button
  //
  track_previous_button=new QPushButton(this,"track_previous_button");
  track_previous_button->
    setGeometry(sizeHint().width()-290,sizeHint().height()-60,80,50);
  track_previous_button->setFont(font);
  track_previous_button->setText(tr("&Previous\nTrack"));
  connect(track_previous_button,SIGNAL(clicked()),this,SLOT(previousData()));

  //
  // Next Button
  //
  track_next_button=new QPushButton(this,"track_next_button");
  track_next_button->
    setGeometry(sizeHint().width()-200,sizeHint().height()-60,80,50);
  track_next_button->setFont(font);
  track_next_button->setText(tr("&Next\nTrack"));
  connect(track_next_button,SIGNAL(clicked()),this,SLOT(nextData()));

  //
  // Play Button
  //
  track_play_button=new RDTransportButton(RDTransportButton::Play,this,
					 "track_play_button");
  track_play_button->setGeometry(20,265,80,50);
  track_play_button->
    setPalette(QPalette(backgroundColor(),colorGroup().mid()));
  connect(track_play_button,SIGNAL(clicked()),
	  this,SLOT(playData()));

  //
  // Stop Button
  //
  track_stop_button=new RDTransportButton(RDTransportButton::Stop,this,
					 "track_stop_button");
  track_stop_button->setGeometry(110,265,80,50);
  track_stop_button->
    setPalette(QPalette(backgroundColor(),colorGroup().mid()));
  track_stop_button->setOnColor(red);
  track_stop_button->on();
  connect(track_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  //
  // Audio Meter
  //
  track_meter=new RDStereoMeter(this,"track_meter");
  track_meter->setGeometry(205,260,track_meter->sizeHint().width(),
			   track_meter->sizeHint().height());
  track_meter->setMode(RDSegMeter::Peak);
  track_meter_timer=new QTimer(this,"track_meter_timer");
  connect(track_meter_timer,SIGNAL(timeout()),this,SLOT(meterData()));

  //
  // Track Length Readout
  //
  edit_length_label=new QLabel(this,"edit_length_label");
  edit_length_label->setText("-:--:--.-");
  edit_length_label->setGeometry(565,255,110,25);
  edit_length_label->setBackgroundColor(white);
  edit_length_label->setAlignment(AlignCenter);
  edit_length_label->setFont(timer_font);

  //
  // Tracks Remaining Readout
  //
  QLabel *label=new QLabel(tr("Remaining"),this,"label");
  label->setGeometry(555,288,116,14);
  label->setFont(small_font);
  label->setAlignment(Qt::AlignHCenter);
  label->setPalette(QPalette(backgroundColor(),colorGroup().mid()));  
  edit_tracks_remaining_label=new QLabel(this,"edit_tracks_remaining_label");
  edit_tracks_remaining_label->setText("0");
  edit_tracks_remaining_label->setGeometry(565,313,40,18);
  edit_tracks_remaining_label->setBackgroundColor(white);
  edit_tracks_remaining_label->setAlignment(AlignCenter);
  edit_tracks_remaining_label->setFont(label_font);
  label=new QLabel(tr("Tracks"),this,"label");
  label->setGeometry(565,300,40,14);
  label->setFont(small_font);
  label->setAlignment(Qt::AlignHCenter);
  label->setPalette(QPalette(backgroundColor(),colorGroup().mid()));  

  edit_time_remaining_label=new QLabel(this,"edit_time_remaining_label");
  edit_time_remaining_label->setText("0:00:00.0");
  edit_time_remaining_label->setGeometry(615,313,60,18);
  edit_time_remaining_label->setBackgroundColor(white);
  edit_time_remaining_label->setAlignment(AlignCenter);
  edit_time_remaining_label->setFont(label_font);
  edit_time_remaining_palette[0]=edit_time_remaining_label->palette();
  edit_time_remaining_palette[1]=edit_time_remaining_label->palette();
  edit_time_remaining_palette[1].
    setColor(QPalette::Active,QColorGroup::Foreground,red);
  edit_time_remaining_palette[1].
    setColor(QPalette::Inactive,QColorGroup::Foreground,red);
  label=new QLabel(tr("Time"),this,"label");
  label->setGeometry(615,300,60,14);
  label->setFont(small_font);
  label->setAlignment(Qt::AlignHCenter);
  label->setPalette(QPalette(backgroundColor(),colorGroup().mid()));  

  //
  // Log List
  //
  track_log_list=new LogListView(this,"track_log_list");
  track_log_list->
    setGeometry(10,335,sizeHint().width()-120,sizeHint().height()-405);
  track_log_list->setAllColumnsShowFocus(true);
  track_log_list->setItemMargin(5);
  connect(track_log_list,SIGNAL(clicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(logClickedData(QListViewItem *,const QPoint &,int)));
  connect(track_log_list,SIGNAL(transitionChanged(int,RDLogLine::TransType)),
	  this,SLOT(transitionChangedData(int,RDLogLine::TransType)));
  track_log_list->addColumn(tr(" "));
  track_log_list->setColumnAlignment(0,AlignCenter);
  track_log_list->addColumn(tr("TIME"));
  track_log_list->setColumnAlignment(1,AlignCenter);
  track_log_list->addColumn(tr("TRANS"));
  track_log_list->setColumnAlignment(2,AlignCenter);
  track_log_list->addColumn(tr("CART"));
  track_log_list->setColumnAlignment(3,AlignCenter);
  track_log_list->addColumn(tr("GROUP"));
  track_log_list->setColumnAlignment(4,AlignCenter);
  track_log_list->addColumn(tr("LENGTH"));
  track_log_list->setColumnAlignment(5,AlignRight);
  track_log_list->addColumn(tr("TITLE"));
  track_log_list->setColumnAlignment(6,AlignLeft);
  track_log_list->addColumn(tr("ARTIST"));
  track_log_list->setColumnAlignment(7,AlignLeft);
  track_log_list->addColumn(tr("ALBUM"));
  track_log_list->setColumnAlignment(8,AlignLeft);
  track_log_list->addColumn(tr("LABEL"));
  track_log_list->setColumnAlignment(9,AlignLeft);
  for(int i=0;i<track_log_list->columns();i++) {
    track_log_list->setColumnSortType(i,RDListView::LineSort);
  }

  //
  // Reset Button
  //
  track_reset_button=new QPushButton(this,"track_reset_button");
  track_reset_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-160,70,70);
  track_reset_button->setFont(font);
  track_reset_button->setText(tr("Do Over"));
  connect(track_reset_button,SIGNAL(clicked()),this,SLOT(resetData()));

  //
  // Hit Post Button
  //
  track_post_button=new QPushButton(this,"track_post_button");
  track_post_button->setGeometry(sizeHint().width()-90,360,70,70);
  track_post_button->setFont(font);
  track_post_button->setText(tr("Hit Post"));
  connect(track_post_button,SIGNAL(clicked()),this,SLOT(postData()));

  //
  // Insert Track Button
  //
  track_insert_button=new QPushButton(this,"track_insert_button");
  track_insert_button->setGeometry(20,sizeHint().height()-60,80,50);
  track_insert_button->setFont(font);
  track_insert_button->setText(tr("Insert\nTrack"));
  connect(track_insert_button,SIGNAL(clicked()),this,SLOT(insertData()));

  //
  // Delete Track Button
  //
  track_delete_button=new QPushButton(this,"track_delete_button");
  track_delete_button->setGeometry(110,sizeHint().height()-60,80,50);
  track_delete_button->setFont(font);
  track_delete_button->setText(tr("Delete\nTrack"));
  connect(track_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  track_close_button=new QPushButton(this,"track_close_button");
  track_close_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  track_close_button->setFont(font);
  track_close_button->setText(tr("&Close"));
  connect(track_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  for(int i=0;i<track_log_event->size();i++) {
    if(track_log_event->logLine(i)->type()==RDLogLine::Track) {
      track_line=i;
      track_loaded=true;
      LoadTrack(track_line);
      LoadBlockLength(track_line);
      i=track_log_event->size();
    }
  }
  RefreshList();
  track_log_list->ensureVisible(0,track_log_list->
     itemPos(track_log_list->selectedItem()),0,track_log_list->size().height()/2);
  UpdateControls();
  UpdateRemaining();

  if(track_group->name().isEmpty()) {
    QMessageBox::warning(this,tr("No VoiceTrack Group"),
			 tr("No voicetracking group has been defined for this service,\ntherefore only existing transitions will be editable."));
  }
}


VoiceTracker::~VoiceTracker()
{
  for(int i=0;i<3;i++) {
    delete wpg[i];
    wpg[i]=NULL;
  }
}


QSize VoiceTracker::sizeHint() const
{
  return QSize(800,700);
} 


QSizePolicy VoiceTracker::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void VoiceTracker::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Qt::Key_Less:
        if(!edit_wave_name[0].isEmpty() && !TransportActive())
	  DragTrack(0,400);
        else
          if(!edit_wave_name[1].isEmpty() && !TransportActive())
  	    DragTrack(1,400);
          else
            if(!edit_wave_name[2].isEmpty() && !TransportActive())
    	      DragTrack(2,400);

	break;

      case Qt::Key_Greater:
        if(!edit_wave_name[0].isEmpty() && !TransportActive())
	  DragTrack(0,-400);
        else
          if(!edit_wave_name[1].isEmpty() && !TransportActive())
  	    DragTrack(1,-400);
          else
            if(!edit_wave_name[2].isEmpty() && !TransportActive())
    	      DragTrack(2,-400);
	break;

       case Qt::Key_Shift:
          edit_shift_pressed=true;
  	break;
	
      default:
	QWidget::keyPressEvent(e);
	break;
  }
}


void VoiceTracker::keyReleaseEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Qt::Key_Shift:
        edit_shift_pressed=false;
  	break;

      default:
	QWidget::keyPressEvent(e);
	break;
  }
}


void VoiceTracker::wheelEvent(QWheelEvent *e)
{
  if(edit_shift_pressed) {
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
        if(!edit_wave_name[0].isEmpty() && !TransportActive())
	  DragTrack(0,e->delta());
        else
          if(!edit_wave_name[1].isEmpty() && !TransportActive())
  	    DragTrack(1,e->delta());
          else
            if(!edit_wave_name[2].isEmpty() && !TransportActive())
    	      DragTrack(2,e->delta());
  }
}


void VoiceTracker::updateMenuData()
{
  if(!edit_wave_name[edit_rightclick_track].isEmpty()) {
    if(edit_rightclick_track==1 && track_loaded) {
      track_menu->setItemEnabled(0,false);
    }
    else {
      track_menu->setItemEnabled(0,true);
    }
  }  
  else {
    track_menu->setItemEnabled(0,false);
  }
  track_menu->setItemEnabled(1,track_changed);
  track_menu->setItemEnabled(2,
                         (!edit_wave_name[edit_rightclick_track].isEmpty()));
  track_menu->setItemEnabled(3,
                         (!edit_wave_name[edit_rightclick_track].isEmpty()));

  if(!edit_wave_name[edit_rightclick_track].isEmpty()) {
    RDCut hook_cut=RDCut(edit_logline[edit_rightclick_track]->cartNumber(),
                edit_logline[edit_rightclick_track]->cutNumber());
    if(hook_cut.hookStartPoint()>=0 && hook_cut.hookEndPoint()>=0) {
      track_menu->setItemEnabled(4,true);
    }
    else {
      track_menu->setItemEnabled(4,false);
    }
  }
  else {
    track_menu->setItemEnabled(4,false);
  }
//  if(edit_wave_name[edit_rightclick_track].isEmpty() 
//      || edit_logline[edit_rightclick_track]->segueGain()==0) {
//    track_menu->setItemEnabled(5,false);
//  }
//  else {
//    track_menu->setItemEnabled(5,true);
//  }
  menu_clicked_point=edit_rightclick_pos;
  DrawTrackMap(edit_rightclick_track);
  WriteTrackMap(edit_rightclick_track);
}


void VoiceTracker::hideMenuData()
{
  menu_clicked_point=-1;
  DrawTrackMap(edit_rightclick_track);
  WriteTrackMap(edit_rightclick_track);
}


void VoiceTracker::playData()
{
  if(TransportActive()) {
    return;
  }
  track_redraw_count=0;
  for(int i=0;i<3;i++) {
    track_redraw[i]=false;
  }
  int start=
    edit_wave_origin[0]-edit_logline[0]->startPoint();
  if(start<0) {
    start=0;
  }
  if((start<=(edit_logline[0]->segueEndPoint()-
	edit_logline[0]->startPoint()))&&
      (!edit_wave_name[0].isEmpty())) {
    if(start>(edit_logline[0]->segueStartPoint()-
	      edit_logline[0]->startPoint())) {
      edit_segue_start_offset[1]=start-
	(edit_logline[0]->segueStartPoint()-
	 edit_logline[0]->startPoint());
      if(edit_wave_name[1].isEmpty()) {
        edit_segue_start_offset[2]=edit_segue_start_offset[1];
        }
      else {
	if(start>(edit_logline[1]->segueStartPoint()-
		  edit_logline[1]->startPoint())) {
        edit_segue_start_offset[2]=edit_segue_start_offset[1]-
                            edit_logline[1]->segueStartPoint()-
                            edit_logline[1]->startPoint();
	}
	else {
	  edit_segue_start_offset[2]=0;
	}
      }
    }
    else {
      edit_segue_start_offset[1]=0;
    }
    edit_deck[0]->setCart(edit_logline[0],false);
    edit_deck[0]->
      play(start,edit_logline[0]->segueStartPoint(),
	   edit_logline[0]->segueEndPoint());
  }
  else {
    if(edit_wave_name[1].isEmpty()) {
      start=edit_wave_origin[2]-
	edit_logline[2]->startPoint();
      if(start<=(edit_logline[2]->segueEndPoint()-
	   edit_logline[2]->startPoint())) {
	start=edit_wave_origin[2]-edit_logline[2]->
	  startPoint(RDLogLine::CartPointer);
	if(start<0) {
	  start=0;
	}
      }
      edit_deck[2]->setCart(edit_logline[2],false);
      edit_deck[2]->
	play(start,edit_logline[2]->segueStartPoint(),
	     edit_logline[2]->segueEndPoint());
    }
    else {
      start=edit_wave_origin[1]-
	edit_logline[1]->startPoint();
      if(start<=(edit_logline[1]->segueEndPoint()-
	   edit_logline[1]->startPoint())) {
	if(start<0) {
	  start=0;
	}
	if(start>(edit_logline[1]->segueStartPoint()-
		  edit_logline[1]->startPoint())) {
	  edit_segue_start_offset[2]=start-
	    (edit_logline[1]->segueStartPoint()-
	     edit_logline[1]->startPoint());
	}
	else {
	  edit_segue_start_offset[2]=0;
	}
	edit_deck[1]->setCart(edit_logline[1],false);
	edit_deck[1]->
	  play(start,edit_logline[1]->segueStartPoint(),
	       edit_logline[1]->segueEndPoint());
      }
      else {
	start=edit_wave_origin[2]-
	  edit_logline[2]->startPoint();
	if(start<=(edit_logline[2]->segueEndPoint()-
	     edit_logline[2]->startPoint())) {
	  start=edit_wave_origin[2]-
	    edit_logline[2]->startPoint();
	  if(start<0) {
	    start=0;
	  }
	}
	edit_deck[2]->setCart(edit_logline[2],false);
	edit_deck[2]->
	  play(start,edit_logline[2]->segueStartPoint(),
	       edit_logline[2]->segueEndPoint());
      }
    }
  }
}


void VoiceTracker::stopData()
{
  edit_deck[0]->stop();
  edit_deck[1]->stop();
  edit_deck[2]->stop();
}


void VoiceTracker::track1Data()
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if(track_track1_button->text()==tr("Import")) {
    if(!ImportTrack(item)) {
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
  rdcae->loadRecord(edit_input_card,edit_input_port,
		    edit_track_cuts[1]->cutName(),
		    edit_coding,edit_chans,edit_samprate,edit_bitrate);
  playData();
  UpdateControls();
}


void VoiceTracker::recordData()
{
  if(edit_deck_state==VoiceTracker::DeckIdle) {
    RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
    if(item==NULL) {
      return;
    }
    if(track_record_button->text()==tr("Import")) {
      if(!ImportTrack(item)) {
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
    rdcae->loadRecord(edit_input_card,edit_input_port,
		      edit_track_cuts[1]->cutName(),
		      edit_coding,edit_chans,edit_samprate,edit_bitrate);
    edit_sliding=true;
    edit_cursor_pos=-edit_wave_origin[2]/TRACKER_MSECS_PER_PIXEL;
  }
  if(!edit_sliding) {
    edit_wave_origin[2]=edit_wave_origin[0]-edit_deck[0]->currentPosition()-
              edit_logline[0]->startPoint()+
              edit_logline[2]->startPoint();
    DrawTrackMap(2);
    WriteTrackMap(2);
    edit_sliding=true;
  }
  track_event_player->exec(record_start_macro);
  edit_wave_name[1]=RDCut::pathName(edit_track_cuts[1]->cutName());
  wpg[1]=new RDWavePainter(edit_wave_map[1],edit_track_cuts[1],
			   rdstation_conf,rduser,log_config);
  wpg[1]->end();
  rdcae->record(edit_input_card,edit_input_port,0,0);
  track_record_ran=true;
  track_record_start_time=GetCurrentTime();
  if(edit_deck_state==VoiceTracker::DeckTrack1) {
    track_time_remaining+=edit_deck[0]->currentPosition()+
      edit_logline[0]->startPoint()-
      edit_logline[0]->segueStartPoint();
  }
  track_time_remaining_start=track_time_remaining;
  edit_deck[0]->duckDown(edit_logline[0]->endPoint()-
               edit_logline[0]->startPoint()-
               edit_deck[0]->currentPosition());
  edit_deck_state=VoiceTracker::DeckTrack2;
  track_start_time=track_log_event->blockStartTime(track_line);
  if(!edit_wave_name[0].isEmpty() && track_start_time>QTime(0,0,0)){
    track_start_time=track_start_time.addMSecs(-edit_logline[0]->
                         segueLength(RDLogLine::Segue));
    track_start_time=track_start_time.addMSecs(edit_deck[0]->currentPosition());
  }
  DrawTrackMap(1);
  UpdateControls();
}


void VoiceTracker::track2Data()
{
  if((edit_deck_state!=VoiceTracker::DeckTrack1)&&
     (edit_deck_state!=VoiceTracker::DeckTrack2)) {
    return;
  }
  if(!edit_sliding) {
    edit_wave_origin[2]=edit_wave_origin[0]-edit_deck[0]->currentPosition();
    DrawTrackMap(2);
    WriteTrackMap(2);
  }
  else {
    edit_sliding=false;
  }
  if(edit_wave_name[1].isEmpty()) {
    edit_segue_start_point[0]=edit_deck[0]->currentPosition();
  }
  else {
    edit_segue_start_point[1]=track_recording_pos;
  }
  edit_deck_state=VoiceTracker::DeckTrack3;

  int new_end=edit_deck[0]->currentPosition()+
              edit_logline[0]->startPoint()+TRACKER_FORCED_SEGUE;
  if(!edit_wave_name[0].isEmpty() && 
      edit_logline[0]->endPoint()>new_end && 
      edit_deck[0]->state()==RDPlayDeck::Playing) {
    if(edit_logline[0]->fadedownPoint()>(new_end-TRACKER_FORCED_SEGUE) ||
       edit_logline[0]->fadedownGain()==0) { 
      edit_logline[0]->setFadedownPoint(new_end-TRACKER_FORCED_SEGUE,
                               RDLogLine::LogPointer);
      edit_logline[0]->setEndPoint(new_end,
                             RDLogLine::LogPointer);
      edit_logline[0]->setSegueEndPoint(new_end,
                             RDLogLine::LogPointer);
      edit_logline[0]->setFadedownGain(RD_FADE_DEPTH);
      edit_deck[0]->stop(TRACKER_FORCED_SEGUE,RD_FADE_DEPTH);
    }                               
  }

  StartNext(0,2);
  UpdateControls();
}


void VoiceTracker::finishedData()
{
  switch(edit_deck_state) {
      case VoiceTracker::DeckIdle:
	if(track_changed) {
	  SaveTrack(track_line);
	}
	break;
	
      case VoiceTracker::DeckTrack1:
	track_aborting=true;
	stopData();
	rdcae->unloadRecord(edit_input_card,edit_input_port);
	edit_deck_state=VoiceTracker::DeckIdle;
	resetData();
	break;
	
      case VoiceTracker::DeckTrack2:
	if(rdlogedit_conf->enableSecondStart()) {
	  if(edit_wave_name[2].isEmpty()||
	     ((edit_logline[2]->transType()!=RDLogLine::Segue))) {
	    FinishTrack();
	  }
	  else {
	    track_aborting=true;
	    stopData();
	    rdcae->stopRecord(edit_input_card,edit_input_port);
	    edit_deck_state=VoiceTracker::DeckIdle;
	    resetData();
	  }
	}
	else {
	  FinishTrack();
	}
	break;

      case VoiceTracker::DeckTrack3:
	FinishTrack();
	break;
  }
  UpdateRemaining();
  UpdateControls();
}


void VoiceTracker::postData()
{
  if(edit_wave_name[2].isEmpty()) {
    return;
  }
  if(!track_changed) {
    PushSegues();
    track_changed=true;
  }
  int talk_end=edit_logline[2]->talkEndPoint();
  if(talk_end<edit_logline[2]->startPoint()) {
    talk_end=edit_logline[2]->startPoint();
  }
  int segue_start=edit_logline[1]->endPoint()-
    edit_logline[1]->startPoint()-
    (talk_end-edit_logline[2]->startPoint());
  if(segue_start<edit_logline[1]->startPoint()) {
    segue_start=edit_logline[1]->startPoint()+10;
  }
  if(segue_start>edit_logline[1]->endPoint()) {
    segue_start=edit_logline[1]->endPoint()-10;
  }
  edit_logline[1]->setSegueStartPoint(segue_start,RDLogLine::LogPointer);
  edit_logline[1]->setSegueGain(0);
  edit_logline[1]->
    setAverageSegueLength(edit_logline[1]->
			  segueStartPoint()-
			  edit_logline[1]->startPoint());
  edit_wave_origin[2]=edit_wave_origin[1]-
    (segue_start-edit_logline[1]->startPoint());

  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  RenderTransition(item->line());
  UpdateControls();
  UpdateRemaining();
}


void VoiceTracker::resetData()
{
  if(track_loaded&&(edit_logline[1]->type()==RDLogLine::Cart)) {
    if(edit_track_cart!=NULL) {
      delete edit_track_cart;
    }
    edit_track_cart=new RDCart(edit_logline[1]->cartNumber());
    edit_logline[1]->setCartNumber(0);
    edit_logline[1]->setType(RDLogLine::Track);
    edit_logline[1]->setSource(RDLogLine::Manual);
    edit_logline[1]->setOriginUser("");
    edit_logline[1]->setOriginDateTime(QDateTime());
  //  printf("CART: %u  TITLE: %s\n",edit_track_cart->number(),(const char *)edit_track_cart->title());
    edit_logline[1]->setMarkerComment(edit_track_cart->title());
    edit_logline[1]->setForcedLength(0);
    edit_logline[1]->clearTrackData(RDLogLine::AllTrans);
    track_log_event->removeCustomTransition(edit_track_line[1]);
    if(!edit_track_cart->remove(rdstation_conf,rduser,log_config)) {
      QMessageBox::warning(this,tr("RDLogEdit"),tr("Audio Deletion Error!"));
    }
    delete edit_track_cart;
    edit_track_cart=NULL;
    if(edit_track_cuts[1]!=NULL) {
      delete edit_track_cuts[1];
      edit_track_cuts[1]=NULL;
    }
    edit_wave_name[1]="";
    delete wpg[1];
    wpg[1]=NULL;
    if(!edit_wave_name[2].isEmpty()) {
      track_log_event->removeCustomTransition(edit_track_line[2]);
    }
  }
  else {
    track_log_event->removeCustomTransition(edit_track_line[1]);
  }
  SaveTrack(track_line);
  LoadTrack(track_line);
  for(int i=0;i<3;i++) {
    DrawTrackMap(i);
    WriteTrackMap(i);
  }
  RDListViewItem *real_item=(RDListViewItem *)track_log_list->selectedItem();
  RDListViewItem *item=NULL;
  if(track_offset) {
    item=GetItemByLine(real_item->line()-1);
  }
  else {
    item=real_item;
  }
  if(item==NULL) {
    return;
  }
  if(track_loaded) {
    item->setPixmap(0,*edit_mic16_map);
    item->setText(3,tr("TRACK"));
    item->setText(4,"");
    item->setText(5,"");
    track_tracks++;
  }
  LoadBlockLength(track_line);
  RefreshLine(real_item);
  if(!track_offset) {
    item=(RDListViewItem *)real_item->nextSibling();
  }
  if((real_item!=item)&&(item!=NULL)) {
    RefreshLine(item);
  }
  UpdateRemaining();
  UpdateControls();
}


void VoiceTracker::insertData()
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  int line=item->line();
  SaveTrack(line);
  if(line==TRACKER_MAX_LINENO) {
    line=track_log_event->size();
  }
  track_log_event->insert(line,1,true);
  track_log_event->logLine(line)->setType(RDLogLine::Track);
  track_log_event->logLine(line)->setTransType(RDLogLine::Segue);
  track_log_event->logLine(line)->setMarkerComment(tr("Voice Track"));
  EditTrack *edit=new EditTrack(track_log_event->logLine(line),this);
  if(edit->exec()>=0) {
    while(item!=NULL) {
      if(item->line()!=TRACKER_MAX_LINENO) {
	item->setLine(item->line()+1);
      }
      item=(RDListViewItem *)item->nextSibling();
    }
    item=new RDListViewItem(track_log_list);
    item->setLine(line);
    RefreshLine(item);
    track_log_list->setSelected(item,true);
    track_log_list->ensureVisible(0,track_log_list->itemPos(item),
				  0,track_log_list->size().height()/2);
    track_tracks++;
    track_size_altered=true;
    track_line=-1;
    logClickedData(item,QPoint(),0);
    UpdateRemaining();
    UpdateControls();
  }
  else {
    track_log_event->remove(line,1);
  }
  delete edit;
}


void VoiceTracker::insertData(int line,RDLogLine *logline,bool warn)
{
  SaveTrack(line);
  track_log_event->insert(line,1,true);
  *track_log_event->logLine(line)=*logline;
  RDListViewItem *item=GetItemByLine(line);
  while(item!=NULL) {
    if(item->line()!=TRACKER_MAX_LINENO) {
      item->setLine(item->line()+1);
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  item=new RDListViewItem(track_log_list);
  item->setLine(line);
  RefreshLine(item);
  track_log_list->ensureVisible(0,track_log_list->itemPos(item),
				0,track_log_list->size().height()/2);
  track_tracks++;
  track_size_altered=true;
  track_line=-1;
  track_changed=false;
  logClickedData(item,QPoint(),0);
  UpdateRemaining();
  UpdateControls();
}


void VoiceTracker::deleteData()
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(track_offset) {
    item=GetItemByLine(item->line()-1);
  }
  if(item==NULL) {
    return;
  }
  deleteData(item->line(),true);
}


void VoiceTracker::deleteData(int line,bool warn)
{
  SaveTrack(line);
  track_log_event->remove(line,1,true);
  RDListViewItem *l=(RDListViewItem *)GetItemByLine(line)->nextSibling();
  while(l!=NULL) {
    if(l->line()!=TRACKER_MAX_LINENO) {
      l->setLine(l->line()-1);
    }
    l=(RDListViewItem *)l->nextSibling();
  }
  if((l=(RDListViewItem *)GetItemByLine(line)->nextSibling())!=NULL) {
    track_log_list->setSelected(l,true);
  }
  delete GetItemByLine(line);
  track_line=-1;
  logClickedData(l,QPoint(),0);
  track_tracks--;
  track_size_altered=true;
  UpdateControls();
  UpdateRemaining();
}


void VoiceTracker::previousData()
{
  RDListViewItem *current=(RDListViewItem *)track_log_list->selectedItem();
  if(current==NULL) {
    return;
  }
  RDListViewItem *item=(RDListViewItem *)track_log_list->firstChild();
  RDListViewItem *previous=NULL;
  while(item!=current) {
    if(item->line()!=TRACKER_MAX_LINENO) {
      if((track_log_event->logLine(item->line())->type()==RDLogLine::Track)||
	 (track_log_event->logLine(item->line())->source()==
	  RDLogLine::Tracker)) {
	previous=item;
      }
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  if(previous==NULL) {
    QMessageBox::information(this,tr("Track List"),tr("No more tracks!"));
    return;
  }
  track_log_list->setSelected(previous,true);
  track_log_list->ensureVisible(0,track_log_list->itemPos(previous),
				0,track_log_list->size().height()/2);
  logClickedData(previous,QPoint(),0);
  track_loaded=true;
}


void VoiceTracker::nextData()
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  item=(RDListViewItem *)item->nextSibling();
  while(item!=NULL) {
    if(item->line()!=TRACKER_MAX_LINENO) {
      if((track_log_event->logLine(item->line())->type()==RDLogLine::Track)||
	 (track_log_event->logLine(item->line())->source()==
	  RDLogLine::Tracker)) {
	track_log_list->setSelected(item,true);
	track_log_list->ensureVisible(0,track_log_list->itemPos(item),
				      0,track_log_list->size().height()/2);
	logClickedData(item,QPoint(),0);
	track_loaded=true;
	return;
      }
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  QMessageBox::information(this,tr("Track List"),
			   tr("No more tracks!"));
}


void VoiceTracker::editAudioData()
{
  RDCart *rdcart=new RDCart(edit_logline[edit_rightclick_track]->cartNumber());
  RDEditAudio *edit=
    new RDEditAudio(rdcart,edit_logline[edit_rightclick_track]->cutName(),
		    rdcae,rduser,rdstation_conf,log_config,edit_output_card,
		    edit_output_port,edit_tail_preroll,
		    edit_threshold_level,this);
  if(edit->exec()!=-1) {
    rdcart->updateLength();
    edit_logline[edit_rightclick_track]->refreshPointers();
  if(edit_logline[edit_rightclick_track]->fadeupPoint()<
     edit_logline[edit_rightclick_track]->startPoint() &&
     edit_logline[edit_rightclick_track]->fadeupPoint()>=0) {
   edit_logline[edit_rightclick_track]->setFadeupPoint(
       edit_logline[edit_rightclick_track]->startPoint(),RDLogLine::LogPointer);
  }    
  if(edit_logline[edit_rightclick_track]->fadeupPoint()>
     edit_logline[edit_rightclick_track]->endPoint()) {
   edit_logline[edit_rightclick_track]->setFadeupPoint(
       edit_logline[edit_rightclick_track]->endPoint(),RDLogLine::LogPointer);
  }    
  if(edit_logline[edit_rightclick_track]->fadedownPoint()<
     edit_logline[edit_rightclick_track]->startPoint() &&
     edit_logline[edit_rightclick_track]->fadedownPoint()>=0) {
   edit_logline[edit_rightclick_track]->setFadedownPoint(
       edit_logline[edit_rightclick_track]->startPoint(),RDLogLine::LogPointer);
  }    
  if(edit_logline[edit_rightclick_track]->fadedownPoint()>
     edit_logline[edit_rightclick_track]->endPoint()) {
   edit_logline[edit_rightclick_track]->setFadedownPoint(
       edit_logline[edit_rightclick_track]->endPoint(),RDLogLine::LogPointer);
  }    
    DrawTrackMap(edit_rightclick_track);
    WriteTrackMap(edit_rightclick_track);
  }
  delete edit;
  delete rdcart;
}


void VoiceTracker::setStartPointData()
{
    if(!track_changed) {
      PushSegues();
      track_changed=true;
    }
    
    int fadeup_diff=edit_logline[edit_rightclick_track]->fadeupPoint()-
             edit_logline[edit_rightclick_track]->startPoint();

    int newpoint=edit_rightclick_pos*TRACKER_MSECS_PER_PIXEL+
                 edit_wave_origin[edit_rightclick_track];
    if(newpoint<0)
      newpoint=0;
    if(newpoint>edit_logline[edit_rightclick_track]->endPoint())
      newpoint=edit_logline[edit_rightclick_track]->endPoint();
    edit_logline[edit_rightclick_track]->
         setStartPoint(newpoint,RDLogLine::LogPointer);

    if(edit_logline[edit_rightclick_track]->
                     endPoint(RDLogLine::LogPointer)<0)
      edit_logline[edit_rightclick_track]->
           setEndPoint(edit_logline[edit_rightclick_track]->
                     endPoint(RDLogLine::CartPointer),
                     RDLogLine::LogPointer);
    
    edit_logline[edit_rightclick_track]->
         setFadeupPoint(edit_logline[edit_rightclick_track]->startPoint()+
         fadeup_diff,RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadeupPoint(RDLogLine::CartPointer)>
          edit_logline[edit_rightclick_track]->fadeupPoint())
      edit_logline[edit_rightclick_track]->
         setFadeupPoint(edit_logline[edit_rightclick_track]->
         fadeupPoint(RDLogLine::CartPointer),RDLogLine::LogPointer);
          

    if(edit_logline[edit_rightclick_track]->fadeupPoint()<
       edit_logline[edit_rightclick_track]->startPoint())
      edit_logline[edit_rightclick_track]->
         setFadeupPoint(edit_logline[edit_rightclick_track]->startPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadeupPoint()>
       edit_logline[edit_rightclick_track]->endPoint())
      edit_logline[edit_rightclick_track]->
         setFadeupPoint(edit_logline[edit_rightclick_track]->endPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadedownPoint()<
       edit_logline[edit_rightclick_track]->startPoint() &&
       edit_logline[edit_rightclick_track]->fadedownPoint()>=0)
      edit_logline[edit_rightclick_track]->
         setFadedownPoint(edit_logline[edit_rightclick_track]->startPoint(),
          RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->segueStartPoint()<
       edit_logline[edit_rightclick_track]->startPoint() &&
       edit_logline[edit_rightclick_track]->segueStartPoint()>=0)
      edit_logline[edit_rightclick_track]->
         setSegueStartPoint(edit_logline[edit_rightclick_track]->startPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadeupPoint(RDLogLine::CartPointer)>=0)
       edit_logline[edit_rightclick_track]->setFadeupGain(RD_FADE_DEPTH);    

    edit_logline[edit_rightclick_track]->setForcedLength(
      edit_logline[edit_rightclick_track]->endPoint()-
      edit_logline[edit_rightclick_track]->startPoint());
    
    edit_logline[edit_rightclick_track]->setHasCustomTransition(true);

    RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
    RenderTransition(item->line());
    UpdateControls();
    UpdateRemaining();
    RefreshList();
    item=(RDListViewItem *)track_log_list->selectedItem();
    track_log_list->ensureVisible(0,track_log_list->itemPos(item),0,track_log_list->size().height()/2);
}


void VoiceTracker::setEndPointData()
{
    if(!track_changed) {
      PushSegues();
      track_changed=true;
    }

    int fadedown_diff=edit_logline[edit_rightclick_track]->endPoint()-
             edit_logline[edit_rightclick_track]->fadedownPoint();

    int newpoint=edit_rightclick_pos*TRACKER_MSECS_PER_PIXEL+
                 edit_wave_origin[edit_rightclick_track];
    if(newpoint>
       edit_logline[edit_rightclick_track]->endPoint(RDLogLine::CartPointer))
       newpoint=edit_logline[edit_rightclick_track]->
                endPoint(RDLogLine::CartPointer);
    if(newpoint<
       edit_logline[edit_rightclick_track]->startPoint())
       newpoint=edit_logline[edit_rightclick_track]->
                startPoint();
    edit_logline[edit_rightclick_track]->
         setEndPoint(newpoint,RDLogLine::LogPointer);

    if(edit_logline[edit_rightclick_track]->
                     startPoint(RDLogLine::LogPointer)<0)
      edit_logline[edit_rightclick_track]->
           setStartPoint(edit_logline[edit_rightclick_track]->
                     startPoint(RDLogLine::CartPointer),
                     RDLogLine::LogPointer);

    edit_logline[edit_rightclick_track]->
         setFadedownPoint(edit_logline[edit_rightclick_track]->endPoint()-
         fadedown_diff,RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadedownPoint(RDLogLine::CartPointer)>=0
       && edit_logline[edit_rightclick_track]->fadedownPoint(RDLogLine::CartPointer)<
       edit_logline[edit_rightclick_track]->fadedownPoint())
      edit_logline[edit_rightclick_track]->
           setFadedownPoint(edit_logline[edit_rightclick_track]->
           fadedownPoint(RDLogLine::CartPointer),RDLogLine::LogPointer);

    if(edit_logline[edit_rightclick_track]->fadedownPoint()>
       edit_logline[edit_rightclick_track]->endPoint())
      edit_logline[edit_rightclick_track]->
         setFadedownPoint(edit_logline[edit_rightclick_track]->endPoint(),
          RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->segueStartPoint()>
       edit_logline[edit_rightclick_track]->endPoint())
      edit_logline[edit_rightclick_track]->
         setSegueStartPoint(edit_logline[edit_rightclick_track]->endPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadeupPoint()>
       edit_logline[edit_rightclick_track]->endPoint())
      edit_logline[edit_rightclick_track]->
         setFadeupPoint(edit_logline[edit_rightclick_track]->endPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->segueStartPoint()<
       edit_logline[edit_rightclick_track]->startPoint() &&
       edit_logline[edit_rightclick_track]->segueStartPoint()>=0)
      edit_logline[edit_rightclick_track]->
         setSegueStartPoint(edit_logline[edit_rightclick_track]->startPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadedownPoint()<
       edit_logline[edit_rightclick_track]->startPoint() &&
       edit_logline[edit_rightclick_track]->fadedownPoint()>=0)
      edit_logline[edit_rightclick_track]->
         setFadedownPoint(edit_logline[edit_rightclick_track]->startPoint(),
         RDLogLine::LogPointer);

    if(edit_logline[edit_rightclick_track]->fadedownPoint(RDLogLine::CartPointer)>=0)
       edit_logline[edit_rightclick_track]->setFadedownGain(RD_FADE_DEPTH);    


    edit_logline[edit_rightclick_track]->
       setSegueEndPoint(edit_logline[edit_rightclick_track]->endPoint(),
       RDLogLine::LogPointer);
    edit_logline[edit_rightclick_track]->setForcedLength(
      edit_logline[edit_rightclick_track]->endPoint()-
      edit_logline[edit_rightclick_track]->startPoint());
  
    if(edit_logline[edit_rightclick_track+1]!=NULL) {
      edit_logline[edit_rightclick_track+1]->setHasCustomTransition(true);
    }  
    RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
    RenderTransition(item->line());
    UpdateControls();
    UpdateRemaining();
    RefreshList();
    item=(RDListViewItem *)track_log_list->selectedItem();
    track_log_list->ensureVisible(0,track_log_list->itemPos(item),0,track_log_list->size().height()/2);
}


void VoiceTracker::setHookPointData()
{
    if(!track_changed) {
      PushSegues();
      track_changed=true;
    }

    RDCut hook_cut=RDCut(edit_logline[edit_rightclick_track]->cartNumber(),
                edit_logline[edit_rightclick_track]->cutNumber());
    if(hook_cut.hookStartPoint()>=0 && hook_cut.hookEndPoint()>=0 &&
       hook_cut.hookStartPoint()<hook_cut.hookEndPoint()){
      edit_logline[edit_rightclick_track]->setStartPoint(
                hook_cut.hookStartPoint(),RDLogLine::LogPointer);
      edit_logline[edit_rightclick_track]->setFadeupPoint(
                hook_cut.hookStartPoint(),RDLogLine::LogPointer);
      edit_logline[edit_rightclick_track]->setEndPoint(
                hook_cut.hookEndPoint(),RDLogLine::LogPointer);
      edit_logline[edit_rightclick_track]->setSegueEndPoint(
                hook_cut.hookEndPoint(),RDLogLine::LogPointer);
      edit_logline[edit_rightclick_track]->setSegueStartPoint(
                hook_cut.hookEndPoint()-500,RDLogLine::LogPointer);
      edit_logline[edit_rightclick_track]->setFadedownPoint(
                hook_cut.hookEndPoint()-500,RDLogLine::LogPointer);
      edit_logline[edit_rightclick_track]->setFadedownGain(RD_FADE_DEPTH);
    }
    else {
      return;
    }

    if(edit_logline[edit_rightclick_track]->endPoint()>
       edit_logline[edit_rightclick_track]->endPoint(RDLogLine::CartPointer))
      edit_logline[edit_rightclick_track]->setEndPoint(
       edit_logline[edit_rightclick_track]->endPoint(RDLogLine::CartPointer),
        RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->endPoint()<
       edit_logline[edit_rightclick_track]->startPoint())
      edit_logline[edit_rightclick_track]->setEndPoint(
       edit_logline[edit_rightclick_track]->startPoint(),
        RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->segueStartPoint()<
       edit_logline[edit_rightclick_track]->startPoint() &&
       edit_logline[edit_rightclick_track]->segueStartPoint()>=0)
      edit_logline[edit_rightclick_track]->
         setSegueStartPoint(edit_logline[edit_rightclick_track]->startPoint(),
         RDLogLine::LogPointer);
    if(edit_logline[edit_rightclick_track]->fadedownPoint()<
       edit_logline[edit_rightclick_track]->startPoint() &&
       edit_logline[edit_rightclick_track]->fadedownPoint()>=0)
      edit_logline[edit_rightclick_track]->
         setFadedownPoint(edit_logline[edit_rightclick_track]->startPoint(),
         RDLogLine::LogPointer);

    edit_logline[edit_rightclick_track]->setForcedLength(
      edit_logline[edit_rightclick_track]->endPoint()-
      edit_logline[edit_rightclick_track]->startPoint());
    
    edit_logline[edit_rightclick_track]->setHasCustomTransition(true);
    if(edit_logline[edit_rightclick_track+1]!=NULL) {
      edit_logline[edit_rightclick_track+1]->setHasCustomTransition(true);
    }  

    RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
    RenderTransition(item->line());
    UpdateControls();
    UpdateRemaining();
    RefreshList();
    item=(RDListViewItem *)track_log_list->selectedItem();
    track_log_list->ensureVisible(0,track_log_list->itemPos(item),0,track_log_list->size().height()/2);
}


void VoiceTracker::undoChangesData()
{
  PopSegues();
  track_changed=false;
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  RenderTransition(item->line());
  RefreshLine(item);
  if(GetItemByLine(item->line()+1)!=NULL){
    RefreshLine(GetItemByLine(item->line()+1));
  }
}


void VoiceTracker::stateChangedData(int id,RDPlayDeck::State state)
{
  // printf("stateChangeData(%d,%d)\n",id,state);

  if(state==RDPlayDeck::Playing) {
    track_play_button->on();
    track_stop_button->off();
    track_event_player->exec(play_start_macro);
  }
  else {
    if((edit_deck_state==VoiceTracker::DeckTrack1)&&
       (state==RDPlayDeck::Finished)&&(!track_record_ran)) {
      finishedData();
      return;
    }
    else {
      for(int i=0;i<3;i++) {
	if(edit_deck[i]->state()==RDPlayDeck::Playing) {
	  return;
	}
      }
      if(track_recording) {
	return;
      }
      track_play_button->off();
      track_stop_button->on();
      track_event_player->exec(play_end_macro);
      edit_deck_state=VoiceTracker::DeckIdle;
      positionData(id,-1);
    }
  }
  switch(state) {
      case RDPlayDeck::Playing:
      case RDPlayDeck::Stopping:
	if(!track_meter_timer->isActive()) {
	  track_meter_timer->start(RD_METER_UPDATE_INTERVAL);
	}
	UpdateControls();
	break;

      default:
	if(!TransportActive()) {
	  track_meter_timer->stop();
	  track_meter->setLeftPeakBar(-10000);
	  track_meter->setRightPeakBar(-10000);
	  edit_scrolling=false;
	  for(unsigned i=0;i<3;i++) {
	    edit_scroll_pos[i]=-1;
	    edit_segue_start_offset[i]=0;
	  }
	}
	RenderTransition(((RDListViewItem *)track_log_list->selectedItem())->
			 line());
	UpdateControls();
	break;
  }
}


void VoiceTracker::positionData(int id,int msecs)
{
  int edit_scroll_diff;
  if(msecs<=0) {
    QPainter *p=new QPainter(this);
    ClearCursor(p);
    delete p;
    return;
  }

  if((id==1)&&track_recording) {
    edit_length_label->setText(RDGetTimeLength(msecs,true));
    track_time_counter=track_time_remaining_start-msecs;
    if(edit_deck_state==VoiceTracker::DeckTrack2) {
      track_time_remaining=
	track_time_remaining_start-msecs;
      UpdateRemaining();
    }
  }
  for(int i=id-1;i>=0;i--) {
    if(edit_deck[i]->state()==RDPlayDeck::Playing) {
      return;
    }
  }
  if(edit_scroll_pos[id]>=0)
    edit_scroll_diff=msecs-edit_scroll_pos[id];
  else
    edit_scroll_diff=0;
  edit_scroll_pos[id]=msecs;
  msecs+=edit_logline[id]->startPoint();
  switch(edit_deck_state) {
      case VoiceTracker::DeckIdle:
	break;

      case VoiceTracker::DeckTrack1:
	if(edit_sliding) {
 	  edit_wave_origin[0]=edit_wave_origin[2]-
                               edit_logline[2]->startPoint()+msecs;
	  track_redraw[0]=true;
	}
	break;

      case VoiceTracker::DeckTrack2:
	switch(id) {
	    case 0:
	      if(!edit_scrolling) {
                edit_wave_origin[0]=edit_wave_origin[2]-
                                    edit_logline[2]->startPoint()+msecs;
	      }
	      edit_wave_origin[1]=edit_wave_origin[0]-
		edit_segue_start_point[0];
	      break;
	      
	    case 1:
	      if(!edit_scrolling) {
		edit_wave_origin[1]=edit_wave_origin[2]+msecs-
		  edit_logline[2]->startPoint();
	      }
	      edit_wave_origin[0]=
	      edit_wave_origin[1]+edit_segue_start_point[0];
	      break;
	}
	track_redraw[0]=true;
	track_redraw[1]=true;
	break;

      case VoiceTracker::DeckTrack3:
	if((id==0)||(id==1)) {
	  track_redraw[0]=true;
	  track_redraw[1]=true;
	}
	else {
	  return;
	}
	break;
  }

  if(edit_scrolling&&(edit_scroll_diff>=0)) {
    edit_wave_origin[0]+=edit_scroll_diff;
    track_redraw[0]=true;
    if(!edit_wave_name[1].isEmpty()) {
      edit_wave_origin[1]+=edit_scroll_diff;
      track_redraw[1]=true;
    }
    if(!edit_sliding) {
      edit_wave_origin[2]+=edit_scroll_diff;
      track_redraw[2]=true;
    }
  }
  if(track_recording && (edit_deck[2]->state()==RDPlayDeck::Playing ||
          edit_deck[2]->state()==RDPlayDeck::Stopping)) {
  track_redraw[2]=true;
  }
  if(track_redraw_count++==TRACKER_SCROLL_SCALE) {
    for(unsigned i=0;i<3;i++) {
      if(track_redraw[i]) {
	DrawTrackMap(i);
	WriteTrackMap(i);
	track_redraw[i]=false;
      }
    }
    track_redraw_count=0;
  }
  QPainter *p=new QPainter(this);
  p->setPen(black);
  p->setBrush(black);
  ClearCursor(p);
  int x=-1;
  if(msecs>=0) {
    if((msecs>edit_wave_origin[id])&&
       (msecs<(edit_wave_origin[id]+edit_wave_width))) {
      x=(int)(((double)(msecs-edit_wave_origin[id]))*
	      ((double)(edit_wave_map[id]->size().width()))/
	      ((double)TRACKER_START_WIDTH))+10;
      p->moveTo(x,10);
      p->lineTo(x,248);
      if(x>edit_scroll_threshold) {
	edit_scrolling=true;
      }
    }
  }
  p->end();
  delete p;
  edit_cursor_pos=x;
//  edit_wave_pos[id]=msecs;
}


void VoiceTracker::segueStartData(int id)
{
  switch(edit_deck_state) {
      case VoiceTracker::DeckIdle:
	for(int i=id+1;i<3;i++) {
	  if(!edit_wave_name[i].isEmpty()) {
	    if(edit_logline[i]->transType()==RDLogLine::Stop) {
	      stopData();
	    }
	    else {
	      StartNext(id);
	    }
	    return;
	  }
	}
	break;

      case VoiceTracker::DeckTrack1:
	edit_sliding=true;

	if(!edit_wave_name[2].isEmpty()) {
	  edit_wave_origin[2]=
	    edit_wave_origin[0]-edit_deck[0]->currentPosition()-
            edit_logline[0]->startPoint()+
            edit_logline[2]->startPoint();
	}

	break;

      case VoiceTracker::DeckTrack2:
	break;

      case VoiceTracker::DeckTrack3:
	break;
  }
}


void VoiceTracker::logClickedData(QListViewItem *item,const QPoint &pt,
					int col)
{
  CheckChanges();
  if(item==NULL) {
    track_loaded=false;
    segue_loaded=false;
    edit_length_label->setText("-:--:--.-");
    return;
  }
  RefreshLine(((RDListViewItem *)item));
  RenderTransition(((RDListViewItem *)item)->line());
}


void VoiceTracker::transitionChangedData(int line,RDLogLine::TransType trans)
{
  track_log_event->logLine(line)->setTransType(trans);
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  RefreshLine(item);
  if((trans==RDLogLine::Segue)&&(edit_wave_name[1].isEmpty())) {
    resetData();
  }
  else {
    RenderTransition(item->line());
  }
  SaveTrack(track_line);
}


void VoiceTracker::meterData()
{
  double ratio[2]={0.0000000001,0.0000000001};
  short level[2];

  if(track_recording) {
    track_recording_pos=
      (int)((GetCurrentTime()-track_record_start_time)*1000.0);
    positionData(1,track_recording_pos);
  }

  for(int i=0;i<3;i++) {
    if((edit_deck[i]->state()==RDPlayDeck::Playing)||
       (edit_deck[i]->state()==RDPlayDeck::Stopping)) {
	positionData(i,edit_deck[i]->currentPosition());
      rdcae->
	outputMeterUpdate(edit_deck[i]->card(),edit_deck[i]->port(),level);
      for(int j=0;j<2;j++) {
	ratio[j]+=pow(10.0,((double)level[j])/1000.0);
      }
    }
    if(track_recording) {
      rdcae->
	inputMeterUpdate(edit_input_card,edit_input_port,level);
      for(int j=0;j<2;j++) {
	ratio[j]+=pow(10.0,((double)level[j])/1000.0);
      }
    }
  }
  track_meter->setLeftPeakBar((int)(log10(ratio[0])*1000.0));
  track_meter->setRightPeakBar((int)(log10(ratio[1])*1000.0));
}


void VoiceTracker::recordLoadedData(int card,int stream)
{
  // printf("recordLoadedData(%d,%d)\n",card,stream);
  if((card!=edit_input_card)||(stream!=edit_input_port)) {
    return;
  }
}


void VoiceTracker::recordingData(int card,int stream)
{
   //printf("recordingData(%d,%d)\n",card,stream);
  if((card!=edit_input_card)||(stream!=edit_input_port)) {
    return;
  }
  edit_segue_start_point[0]=edit_logline[0]->startPoint()+
                   edit_deck[0]->currentPosition();
  if(!edit_wave_name[1].isEmpty()) {
    if((edit_logline[1]->transType()!=RDLogLine::Segue)) {
      edit_segue_start_point[0]=
	edit_logline[0]->endPoint();
    }
  }
//  edit_segue_gain[0]=0;
  edit_wave_origin[1]=edit_wave_origin[0]+
                      edit_deck[0]->currentPosition();
   // rdcae->fadeOutputVolume(edit_deck[0]->card(),edit_deck[0]->stream(),
//			  edit_deck[0]->port(),edit_segue_gain[0],
//			  edit_logline[0]->endPoint(RDLogLine::CartPointer)-
//			  edit_segue_start_point[0]);
  if(!track_meter_timer->isActive()) {
    track_meter_timer->start(RD_METER_UPDATE_INTERVAL);
  }
  track_recording=true;
}


void VoiceTracker::recordStoppedData(int card,int stream)
{
  // printf("recordStoppedData(%d,%d)\n",card,stream);
  if((card!=edit_input_card)||(stream!=edit_input_port)) {
    return;
  }
  rdcae->unloadRecord(edit_input_card,edit_input_port);
  track_event_player->exec(record_end_macro);
}


void VoiceTracker::recordUnloadedData(int card,int stream,unsigned msecs)
{
  // printf("recordUnloadedData(%d,%d)\n",card,stream);
  if((card!=edit_input_card)||(stream!=edit_input_port)) {
    return;
  }
  track_recording=false;
  if(!track_aborting) {
    edit_track_cuts[1]->
      checkInRecording(rdstation_conf->name(),edit_settings,msecs);
    edit_track_cuts[1]->setSampleRate(rdsystem->sampleRate());
    edit_track_cart->updateLength();
    edit_track_cart->resetRotation();
    edit_logline[1]->
      loadCart(edit_track_cart->number(),RDLogLine::Segue,0,false);
    edit_logline[1]->setEvent(0,RDLogLine::Segue,false);
    edit_logline[1]->setType(RDLogLine::Cart);
    edit_logline[1]->setSource(RDLogLine::Tracker);
    edit_logline[1]->setOriginUser(rduser->name());
    edit_logline[1]->setOriginDateTime(QDateTime(QDate::currentDate(),
						 QTime::currentTime()));
    edit_logline[1]->
      setSegueStartPoint(edit_segue_start_point[1],RDLogLine::LogPointer);
    edit_logline[1]->
      setSegueEndPoint(edit_logline[1]->endPoint(),
		       RDLogLine::LogPointer);
    edit_logline[1]->setSegueGain(0);
    edit_logline[1]->setSegueEndPoint(edit_logline[1]->endPoint(),RDLogLine::LogPointer);
    edit_logline[1]->
      setFadeupPoint(edit_track_cuts[1]->startPoint(),RDLogLine::LogPointer);
    edit_logline[1]->setFadeupGain(RD_FADE_DEPTH);
    edit_logline[1]->
      setFadedownPoint(edit_track_cuts[1]->endPoint(),RDLogLine::LogPointer);
    edit_logline[1]->setFadedownGain(RD_FADE_DEPTH);
    edit_logline[1]->setHasCustomTransition(true);
    if(!edit_wave_name[2].isEmpty()) {
      edit_logline[2]->setHasCustomTransition(true);
    }
    if(!edit_wave_name[2].isEmpty()) {
      edit_logline[1]->
	setAverageSegueLength(edit_logline[1]->
			      segueStartPoint()-
			      edit_logline[1]->
			      startPoint());
    }
    edit_wave_origin[1]=edit_logline[1]->startPoint()-
      (edit_logline[0]->segueStartPoint()-
       edit_wave_origin[0]);
    DrawTrackMap(1);
    WriteTrackMap(1);
    RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
    if(track_offset) {
      item=GetItemByLine(item->line()-1);
    }
    if(item==NULL) {
      return;
    }
    item->setPixmap(0,*edit_track_cart_map);
    item->setText(3,QString().sprintf("%06u",edit_track_cart->number()));
    item->setText(4,track_group->name());
    item->setText(5,RDGetTimeLength(edit_track_cart->forcedLength()));
    SaveTrack(track_line);
    RefreshLine(item);
    if(!edit_wave_name[2].isEmpty()) {
      RefreshLine((RDListViewItem *)item->nextSibling());
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
}


void VoiceTracker::closeData()
{
  stopData();
  CheckChanges();
  if(track_size_altered) {
    track_log_event->save();
  }
  done(0);
}


void VoiceTracker::paintEvent(QPaintEvent *e)
{
  QPainter *p=NULL;

  if(track_line<0) {
    p=new QPainter();
    for(int i=0;i<3;i++) {
      p->begin(edit_wave_map[i],"");
      p->setPen(TRACKER_TEXT_COLOR);
      p->setBackgroundColor(gray);
      p->eraseRect(0,0,edit_wave_map[i]->size().width(),
		   edit_wave_map[i]->size().height());
      p->end();
    }
    delete p;
  }
  p=new QPainter(this);

  p->setPen(black);

  p->fillRect(TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN,TRACKER_X_WIDTH-1-2,238,
	      white);
  p->fillRect(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1,
	      TRACKER_X_WIDTH+2,TRACKER_Y_HEIGHT+6,colorGroup().mid());

  p->moveTo(TRACKER_X_ORIGIN-2,TRACKER_Y_ORIGIN-2);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-2);
  p->moveTo(TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN-1);

  p->moveTo(TRACKER_X_ORIGIN-2,TRACKER_Y_ORIGIN-2);
  p->lineTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->moveTo(TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_ORIGIN-1,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);

  p->moveTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	    TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);
  p->moveTo(TRACKER_X_ORIGIN+TRACKER_X_ORIGIN-1,
	    TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN,
	    TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);

  p->moveTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN-2);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN,
	    TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->moveTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	    TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-2);

  p->moveTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-2);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	    TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-2);
  p->moveTo(TRACKER_X_ORIGIN-1,TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	    TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN-1);

  p->moveTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-2);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN,
	    TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-2);
  p->moveTo(TRACKER_X_ORIGIN-1,TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH+TRACKER_X_ORIGIN-1,
	    TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN-1);

  p->moveTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH-1+TRACKER_X_ORIGIN+1,
	    TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);
  p->lineTo(TRACKER_X_WIDTH-1+TRACKER_X_ORIGIN+1,
	    TRACKER_Y_HEIGHT*4+TRACKER_Y_ORIGIN-5+10);
  p->lineTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*4+TRACKER_Y_ORIGIN-5+10);
  p->lineTo(TRACKER_X_ORIGIN-2,TRACKER_Y_HEIGHT*3+TRACKER_Y_ORIGIN-1);

  WriteTrackMap(0);
  WriteTrackMap(1);
  WriteTrackMap(2);

  delete p;
}


void VoiceTracker::mousePressEvent(QMouseEvent *e)
{
  if(TransportActive()) {
    return;
  }
  switch(e->button()) {
      case QMouseEvent::LeftButton:
	edit_current_track=GetClick(e,edit_previous_point);
	break;

      case QMouseEvent::RightButton:
	edit_rightclick_track=GetClick(e,edit_previous_point);
        edit_rightclick_pos=edit_previous_point->x();
	if(edit_rightclick_track>=0) {
	  track_menu->setGeometry(e->globalX(),e->globalY()+20,
				  track_menu->sizeHint().width(),
				  track_menu->sizeHint().height());
	  track_menu->exec();
	}
	break;

      default:
	break;
  }
}


void VoiceTracker::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()) {
      case QMouseEvent::LeftButton:
        if(edit_current_track>=0) {
          track_meter->setLeftPeakBar(-10000);
          track_meter->setRightPeakBar(-10000);
        }  
	edit_current_track=-1;
	break;

      case QMouseEvent::RightButton:
	edit_rightclick_track=-1;
	break;

      case QMouseEvent::MidButton:
        if(e->y()<TRACKER_Y_ORIGIN+TRACKER_Y_HEIGHT) {
		DragTrack(0,((edit_wave_origin[0]-edit_logline[0]->startPoint())/TRACKER_MSECS_PER_PIXEL)+250);
        }
        else {
          if(e->y()<(TRACKER_Y_ORIGIN+2*TRACKER_Y_HEIGHT)) {
		  DragTrack(0,((edit_wave_origin[1]-edit_logline[1]->startPoint())/TRACKER_MSECS_PER_PIXEL)+250);
	  }
          else {
            if(e->y()<(TRACKER_Y_ORIGIN+3*TRACKER_Y_HEIGHT)) {
		    DragTrack(0,((edit_wave_origin[2]-edit_logline[2]->startPoint())/TRACKER_MSECS_PER_PIXEL)+250);
	    }
          }
        }
	break;

      default:
	break;
  }
}


void VoiceTracker::mouseMoveEvent(QMouseEvent *e)
{
  if(TransportActive()) {
    return;
  }
  QPoint pt;
  int trackno=GetClick(e,&pt);
  if(edit_current_track<0) {
    QCursor *cursor=track_arrow_cursor;
    if((trackno>=0)&&(track_track_rect->contains(pt))) {
      if(!edit_wave_name[trackno].isEmpty()) {
	track_current_target=VoiceTracker::TargetSize;
	for(unsigned i=0;i<VoiceTracker::TargetSize;i++) {
	  if(track_target_rect[i]->contains(pt)&&
	     (track_target_track[i]==trackno)) {
	    cursor=track_cross_cursor;
	    track_current_target=(VoiceTracker::Target)i;
	  }
	}
	if(track_current_target==VoiceTracker::TargetSize) {
	  switch(edit_logline[trackno]->transType()) {
	      case RDLogLine::Segue:
		cursor=track_hand_cursor;
		break;
		
	      case RDLogLine::Play:
	      case RDLogLine::Stop:
	      case RDLogLine::NoTrans:
		if(trackno==0) {
		  cursor=track_hand_cursor;
		}
		else {
		  cursor=track_arrow_cursor;
		}
		break;
	  }
	}
      }
    }
    if(track_current_cursor!=cursor) {
      setCursor(*cursor);
      track_current_cursor=cursor;
    }
    return;
  }
  if(trackno!=edit_current_track) {
    return;
  }
  if(track_current_cursor==track_hand_cursor) {
    DragTrack(edit_current_track,pt.x()-edit_previous_point->x());
    *edit_previous_point=pt;
  }
  if(track_current_cursor==track_cross_cursor) {
    DragTarget(edit_current_track,pt);
  }
}


void VoiceTracker::LoadTrack(int line)
{
  edit_deck_state=DeckIdle;
  edit_cursor_pos=-1;

  if(line<0) {
    for(int i=0;i<3;i++) {
      if(!(edit_wave_name[i].isEmpty())) {
	delete wpg[i];
        wpg[i]=NULL;
      }
      edit_wave_name[i]="";
      edit_logline[i]=NULL;
      edit_wave_origin[i]=0;
      edit_wave_width=0;
    }
  }
  else {
    //
    // Track Mappings
    //
    if(track_loaded) {
      if(track_offset) {
	edit_track_line[0]=line-2;
	edit_track_line[1]=line-1;
	edit_track_line[2]=line;
      }
      else {
	edit_track_line[0]=line-1;
	edit_track_line[1]=line;
	edit_track_line[2]=line+1;
      }
    }
    else {
      if(segue_loaded) {
//	edit_track_line[0]=line-1;
//	edit_track_line[1]=-1;
//	edit_track_line[2]=line;
	edit_track_line[0]=line-1;
	edit_track_line[1]=line;
	edit_track_line[2]=line+1;
      }
      else {
	edit_track_line[0]=-1;
	edit_track_line[1]=-1;
	edit_track_line[2]=-1;
      }
    }

    //
    // Audio Filenames
    //
    for(unsigned i=0;i<3;i++) {
      edit_wave_name[i]=GetCutName(edit_track_line[i],&edit_track_cuts[i]);
      if(!(edit_wave_name[i].isEmpty())) {
        if(wpg[i]!=NULL) {
           delete wpg[i];
        }
	wpg[i]=new RDWavePainter(edit_wave_map[i],edit_track_cuts[i],
				 rdstation_conf,rduser,log_config);
	wpg[i]->end();
      }
    }

    //
    // Track 0 Parameters
    //
    if(edit_wave_name[0].isEmpty()) {
      edit_logline[0]=track_dummy0_logline;  // No initial track, so fake it
      edit_logline[0]->clear();
      edit_logline[0]->setStartPoint(0,RDLogLine::CartPointer);
      edit_logline[0]->setStartPoint(-1,RDLogLine::LogPointer);
      edit_logline[0]->setEndPoint(0,RDLogLine::CartPointer);
      edit_logline[0]->setEndPoint(-1,RDLogLine::LogPointer);
      edit_wave_origin[0]=0;
    }
    else {
      edit_logline[0]=track_log_event->logLine(edit_track_line[0]);
      edit_logline[0]->refreshPointers();
      edit_wave_origin[0]=edit_logline[0]->segueStartPoint()-track_preroll;
    }

    //
    // Track 1 Parameters
    //
    edit_logline[1]=track_log_event->logLine(edit_track_line[1]);
    if(!edit_wave_name[1].isEmpty()) {
      edit_logline[1]->refreshPointers();
    }  
    if((track_log_event->logLine(edit_track_line[1])!=NULL)&&
       (!edit_wave_name[1].isEmpty())) {
      edit_wave_origin[1]=edit_logline[1]->startPoint()-track_preroll;
      edit_length_label->
	setText(RDGetTimeLength(edit_logline[1]->forcedLength(),true));
      if(((edit_logline[1]->transType()!=RDLogLine::Segue))&&
	 (!edit_wave_name[0].isEmpty())) {
	edit_wave_origin[0]=edit_logline[0]->endPoint()-track_preroll;
      }
    }
    else {
      edit_wave_origin[1]=0;
      edit_length_label->setText("0:00:00.0");
    }
    
    //
    // Track 2 Parameters
    //
    if(edit_wave_name[2].isEmpty()) {
      edit_logline[2]=track_dummy2_logline;  // No final track, so fake it
      edit_logline[2]->clear();
      edit_logline[2]->setStartPoint(0,RDLogLine::CartPointer);
      edit_logline[2]->setStartPoint(-1,RDLogLine::LogPointer);
      edit_logline[2]->setEndPoint(0,RDLogLine::CartPointer);
      edit_logline[2]->setEndPoint(-1,RDLogLine::LogPointer);
      edit_wave_origin[2]=0;
    }
    else {
      edit_logline[2]=track_log_event->logLine(edit_track_line[2]);
      edit_logline[2]->refreshPointers();
      if(edit_logline[2]->transType()!=RDLogLine::Segue) {
	if(!edit_wave_name[1].isEmpty()) {  
	   edit_wave_origin[2]=edit_logline[2]->startPoint()-
	     (edit_logline[1]->endPoint()-edit_logline[1]->startPoint()+
	     track_preroll);
	}
	else
	{
	  edit_wave_origin[2]=edit_logline[2]->startPoint()-track_preroll;
  	  edit_wave_origin[0]=edit_logline[0]->endPoint()-track_preroll;
	}
      }
      else {
        if(edit_wave_name[1].isEmpty()) {
	  edit_wave_origin[2]=edit_logline[2]->startPoint()-track_preroll;
        }
        else {
	   edit_wave_origin[2]=edit_logline[2]->startPoint()-
	     (edit_logline[1]->segueStartPoint()-edit_logline[1]->startPoint()+
	     track_preroll);
        }
      }
    }
    edit_wave_width=TRACKER_START_WIDTH;
  }
  track_start_time=track_log_event->blockStartTime(line);
  DrawTrackMap(0);
  DrawTrackMap(1);
  DrawTrackMap(2);
  UpdateControls();
}


void VoiceTracker::SaveTrack(int line)
{
  if((line<0)||(line==TRACKER_MAX_LINENO)) {
    return;
  }
  if((line>0)&&(track_log_event->logLine(line-1)->type()==RDLogLine::Track)) {
    line--;
  }
  track_log_event->save();
  track_log->
    setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
  track_changed=false;
  track_size_altered=false;
}


bool VoiceTracker::ImportTrack(RDListViewItem *item)
{
  bool metadata=false;

  if(!InitTrack()) {
    return false;
  }
  RDWaveData *wdata=new RDWaveData();
  RDImportAudio *import=
    new RDImportAudio(edit_track_cuts[1]->cutName(),edit_import_path,
		      edit_settings,&metadata,wdata,NULL,rdstation_conf,rduser,
		      &import_running,log_config,this);
  if(import->exec(true,false)<0) {
    delete import;
    delete wdata;
    resetData();
    return true;
  }
  delete import;
  delete wdata;

  if(!edit_wave_name[0].isEmpty()) {
    edit_logline[0]->
      setSegueStartPoint(edit_logline[0]->
			 segueStartPoint(RDLogLine::CartPointer),
			 RDLogLine::LogPointer);
    edit_logline[0]->setSegueGain(0);
    edit_logline[0]->setSegueEndPoint(edit_logline[0]->endPoint(),RDLogLine::LogPointer);
    edit_logline[1]->setTransType(RDLogLine::Segue);
    edit_logline[0]->
      setAverageSegueLength(edit_logline[0]->
			    segueStartPoint()-
			    edit_logline[0]->
			    startPoint());
  }

  edit_track_cart->updateLength();
  edit_track_cart->resetRotation();
  edit_logline[1]->
    loadCart(edit_track_cart->number(),RDLogLine::Segue,0,false);
  edit_logline[1]->setEvent(0,RDLogLine::Segue,false);
  edit_logline[1]->setType(RDLogLine::Cart);
  edit_logline[1]->setSource(RDLogLine::Tracker);
  edit_logline[1]->setOriginUser(rduser->name());
  edit_logline[1]->setOriginDateTime(QDateTime(QDate::currentDate(),
					       QTime::currentTime()));
  edit_logline[1]->setFadeupPoint(edit_track_cuts[1]->startPoint(),
				  RDLogLine::LogPointer);
  edit_logline[1]->setFadeupGain(RD_FADE_DEPTH);
  edit_logline[1]->setFadedownPoint(edit_track_cuts[1]->endPoint(),
				    RDLogLine::LogPointer);
  edit_logline[1]->setFadedownGain(RD_FADE_DEPTH);
  edit_logline[1]->setEndPoint(-1,RDLogLine::LogPointer);
  edit_logline[1]->setSegueEndPoint(edit_track_cuts[1]->endPoint(),
				    RDLogLine::LogPointer);
  edit_logline[1]->setHasCustomTransition(true);
  if(!edit_wave_name[2].isEmpty()) {
    edit_logline[2]->setHasCustomTransition(true);
    edit_logline[1]->
      setAverageSegueLength(edit_logline[1]->
			    segueStartPoint()-
			    edit_logline[1]->
			    startPoint());
  }
  item->setPixmap(0,*edit_track_cart_map);
  item->setText(3,QString().sprintf("%06u",edit_track_cart->number()));
  item->setText(4,track_group->name());
  item->setText(5,RDGetTimeLength(edit_track_cart->forcedLength()));
  postData();
  SaveTrack(track_line);
  LoadTrack(track_line);
  WriteTrackMap(0);
  WriteTrackMap(1);
  WriteTrackMap(2);
  RefreshLine(item);
  if(!edit_wave_name[2].isEmpty()) {
    RefreshLine((RDListViewItem *)item->nextSibling());
  }

  return true;
}


void VoiceTracker::RenderTransition(int line)
{
  if(line==TRACKER_MAX_LINENO) {
    track_loaded=false;
    segue_loaded=false;
    edit_length_label->setText("-:--:--.-");
  }
  else {
    if(IsTrack(line,&track_offset)) {
      track_loaded=true;
    }
    else {
      track_loaded=false;
      edit_length_label->setText("-:--:--.-");
    }
    segue_loaded=true;
  }
  track_line=line;
  LoadTrack(track_line);
  LoadBlockLength(line);
  UpdateRemaining();
  update();
  UpdateControls();
}


void VoiceTracker::LoadBlockLength(int line)
{
  int nominal_length=0;
  int actual_length=0;

  track_block_valid=track_log_event->
    blockLength(&nominal_length,&actual_length,line);
  track_time_remaining=nominal_length-actual_length;
}


void VoiceTracker::RefreshList()
{
  RDListViewItem *item=NULL;
  track_log_list->clear();
  item=new RDListViewItem(track_log_list);
  item->setLine(TRACKER_MAX_LINENO);
  item->setText(6,tr("[end of log]"));
  for(int i=track_log_event->size()-1;i>=0;i--) {
    item=new RDListViewItem(track_log_list);
    item->setId(track_log_event->logLine(i)->id());
    item->setLine(i);
    RefreshLine(item);
    if(track_line==i) {
      track_log_list->setSelected(item,true);
      track_log_list->ensureVisible(0,track_log_list->itemPos(item),0,track_log_list->size().height()/2);
    }
  }
}


void VoiceTracker::RefreshLine(RDListViewItem *item)
{
  RDLogLine *logline=track_log_event->logLine(item->line());
  if(logline==NULL) {
    return;
  }
  switch(logline->type()) {
      case RDLogLine::Cart:
	switch(logline->source()) {
	    case RDLogLine::Tracker:
	      item->setPixmap(0,*edit_track_cart_map);
	      break;

	    default:
	      item->setPixmap(0,*edit_playout_map);
	      break;
	}
	item->setText(3,QString().sprintf("%06u",logline->cartNumber()));
	item->setText(5,RDGetTimeLength(logline->forcedLength()));
	if(logline->title().isEmpty()) {
	  item->setText(4,"");
	  item->setText(6,tr("[cart not found]"));
	}
	else {
	  item->setText(4,logline->groupName());
	  if(logline->originUser().isEmpty()||
	     (!logline->originDateTime().isValid())) {
	    item->setText(6,logline->title());
	  }
	  else {
	    item->setText(6,QString().
			  sprintf("%s -- %s %s",
				  (const char *)logline->title(),
				  (const char *)logline->originUser(),
				  (const char *)logline->originDateTime().
				  toString("M/d hh:mm")));
	  }
	}
	item->setText(7,logline->artist());
	item->setText(8,logline->album());
	item->setText(9,logline->label());
	break;
	
      case RDLogLine::Macro:
	item->setPixmap(0,*edit_macro_map);
	item->setText(3,QString().sprintf("%06u",logline->cartNumber()));
	item->setText(5,RDGetTimeLength(logline->forcedLength()));
	if(logline->title().isEmpty()) {
	  item->setText(4,"");
	  item->setText(6,tr("[cart not found]"));
	}
	else {
	  item->setText(4,logline->groupName());
	  item->setText(6,logline->title());
	}
	item->setText(7,logline->artist());
	item->setText(8,logline->album());
	item->setText(9,logline->label());
	break;
	
      case RDLogLine::Marker:
	item->setPixmap(0,*edit_notemarker_map);
	item->setText(3,tr("MARKER"));
	item->setText(6,RDTruncateAfterWord(logline->markerComment(),5,true));
	break;
	
      case RDLogLine::Chain:
	item->setPixmap(0,*edit_chain_map);
	item->setText(3,tr("LOG CHAIN"));
	item->setText(6,logline->markerLabel());
	item->setText(7,RDTruncateAfterWord(logline->markerComment(),5,true));
	break;
	
      case RDLogLine::Track:
	item->setPixmap(0,*edit_mic16_map);
	item->setText(3,tr("TRACK"));
	item->setText(6,RDTruncateAfterWord(logline->markerComment(),5,true));
	break;
	
      case RDLogLine::MusicLink:
	item->setPixmap(0,*edit_music_map);
	item->setText(3,tr("LINK"));
	item->setText(6,tr("[music import]"));
	break;
	
      case RDLogLine::TrafficLink:
	item->setPixmap(0,*edit_traffic_map);
	item->setText(3,tr("LINK"));
	item->setText(6,tr("[traffic import]"));
	break;
	
      default:
	break;
  }
  if(!logline->startTime(RDLogLine::Logged).isNull()) {
    if(logline->timeType()==RDLogLine::Hard) {
      item->setText(1,logline->startTime(RDLogLine::Imported).
		    toString("Hhh:mm:ss.zzz").left(11));
    }
    else {
      item->setText(1,logline->startTime(RDLogLine::Imported).
		    toString("hh:mm:ss.zzz").left(10));
    }
  }
  switch(logline->transType()) {
      case RDLogLine::Play:
	item->setText(2,tr("PLAY"));
	item->setTextColor(2,item->textColor(1),QFont::Normal);
	break;
	
      case RDLogLine::Segue:
	item->setText(2,tr("SEGUE"));
	if(logline->hasCustomTransition()) {
	  item->setTextColor(2,RD_CUSTOM_TRANSITION_COLOR,QFont::Bold);
	}
	else {
	  item->setTextColor(2,item->textColor(1),QFont::Normal);
	}
	break;
	
      case RDLogLine::Stop:
	item->setText(2,tr("STOP"));
	item->setTextColor(2,item->textColor(1),QFont::Normal);
	break;
	
      default:
	break;
  }
}


void VoiceTracker::StartNext(int finishing_id,int next_id)
{
  int duckin;
  if(next_id==-1) {
    next_id=finishing_id+1;
  }
  for(int i=next_id;i<3;i++) {
    if(!edit_wave_name[i].isEmpty()) {
      edit_deck[i]->setCart(edit_logline[i],true);
      if(edit_wave_name[i-1].isEmpty()) {
        duckin=edit_logline[i-2]->segueEndPoint()-
                   edit_logline[i-2]->segueStartPoint();
	edit_deck[i-2]->stop(duckin);
      }
      else {
        duckin=edit_logline[i-1]->segueEndPoint()-
                   edit_logline[i-1]->segueStartPoint();
	edit_deck[i-1]->stop(duckin);
      }
      if(track_recording)
        duckin=-1;
      edit_deck[i]->
	     play(edit_segue_start_offset[i],
             edit_logline[i]->segueStartPoint(),
	     edit_logline[i]->segueEndPoint(),duckin);
      return;
    }
  }
}


QString VoiceTracker::GetCutName(int line,RDCut **cut)
{
  if((line<0)||(line>=track_log_event->size())) {
    return QString();
  }
  QString wavname;
  QString pathname;
  RDLogLine *logline=track_log_event->logLine(line);
  if(*cut!=NULL) {
    delete *cut;
    *cut=NULL;
  }
  if(line==(track_log_event->size()-1)) {
    logline->setEvent(0,RDLogLine::Stop,false);
  }
  else {
    logline->setEvent(0,track_log_event->logLine(line+1)->transType(),false);
  }
  if(!logline->cutName().isEmpty()) {
    *cut=new RDCut(logline->cutName());
    pathname=RDCut::pathName(logline->cutName());
    /*
    if(!QFile::exists(pathname)) {
      return QString();
    }
    */
  }
  return pathname;
}


int VoiceTracker::GetClick(QMouseEvent *e,QPoint *p)
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


void VoiceTracker::DragTrack(int trackno,int xdiff)
{
  if(edit_wave_name[trackno].isEmpty()) {
    return;
  }
//  if((!track_changed)&&(trackno>0)&&(track_line!=0)) {
  if((!track_changed)&&(trackno>0)) {
    PushSegues();
  }

  //
  // Calculate the Offset
  //
  int tdiff=xdiff*TRACKER_MSECS_PER_PIXEL;
  int tend=0;
  switch(trackno) {
      case 1:
	if(!edit_wave_name[0].isEmpty()) {
	  if((edit_wave_origin[1]-edit_logline[1]->startPoint()-tdiff)>
            (edit_wave_origin[0]-edit_logline[0]->startPoint())) {
	    tdiff=(edit_wave_origin[1]-edit_logline[1]->startPoint())-
                   (edit_wave_origin[0]-edit_logline[0]->startPoint());
	  }
	  tend=edit_wave_origin[0]-
	    edit_logline[0]->segueEndPoint()+
	    edit_logline[1]->startPoint()+100;
	  if((edit_wave_origin[1]-tdiff)<tend) {
	    tdiff=edit_wave_origin[1]-tend;
	  }
	  edit_logline[0]->
	    setSegueStartPoint(edit_logline[0]->segueStartPoint()+tdiff,
			       RDLogLine::LogPointer);
//	  edit_logline[0]->setSegueGain(0);
	  edit_logline[0]->
	    setAverageSegueLength(edit_logline[0]->
				  segueStartPoint()-
				  edit_logline[0]->
				  startPoint());
	}
	break;

      case 2:
	if(edit_wave_name[1].isEmpty()) {
	  if(!edit_wave_name[0].isEmpty()) {
  	    if((edit_wave_origin[2]-edit_logline[2]->startPoint()-tdiff)>
                (edit_wave_origin[0]-edit_logline[0]->startPoint())) {
	      tdiff=(edit_wave_origin[2]-edit_logline[2]->startPoint())-
                     (edit_wave_origin[0]-edit_logline[0]->startPoint());
	    }
	    tend=edit_wave_origin[0]-
	      edit_logline[0]->segueEndPoint()+
	      edit_logline[2]->startPoint()+100;
	    if((edit_wave_origin[2]-tdiff)<tend) {
	      tdiff=edit_wave_origin[2]-tend;
	    }
	    edit_logline[0]->
	      setSegueStartPoint(edit_logline[0]->segueStartPoint()+tdiff,
				 RDLogLine::LogPointer);
//	    edit_logline[0]->setSegueGain(0);
	    edit_logline[0]->
	      setAverageSegueLength(edit_logline[0]->
				    segueStartPoint()-
				    edit_logline[0]->
				    startPoint());
	  }
	}
	else {
	  if((edit_wave_origin[2]-edit_logline[2]->startPoint()-tdiff)>
            (edit_wave_origin[1]-edit_logline[1]->startPoint())) {
	    tdiff=(edit_wave_origin[2]-edit_logline[2]->startPoint())-
                   (edit_wave_origin[1]-edit_logline[1]->startPoint());
	  }
	  tend=edit_wave_origin[1]-
	    edit_logline[1]->segueEndPoint()+
	    edit_logline[2]->startPoint()+100;
	  if((edit_wave_origin[2]-tdiff)<tend) {
	    tdiff=edit_wave_origin[2]-tend;
	  }
	  edit_logline[1]->
	    setSegueStartPoint(edit_logline[1]->segueStartPoint()+tdiff,
			       RDLogLine::LogPointer);
	//  edit_logline[1]->setSegueGain(0);
	  edit_logline[1]->
	    setAverageSegueLength(edit_logline[1]->
				  segueStartPoint()-
				  edit_logline[1]->
				  startPoint());
	}
	break;
  }

  //
  // Ignore Meaningless Edits
  //
  bool state=false;
  for(int i=0;i<trackno;i++) {
    state|=!edit_wave_name[i].isEmpty();
  }
  for(int i=trackno;i<3;i++) {
    edit_wave_origin[i]-=tdiff;
  }
  track_start_time=track_log_event->blockStartTime(track_line);
  for(int i=0;i<3;i++) {
    DrawTrackMap(i);
    WriteTrackMap(i);
  }
  if((!state)||(tdiff==0)) {
    return;
  }

  //
  // Apply the Offset
  //
  track_changed=true;
  edit_logline[trackno]->setHasCustomTransition(true);

  if(edit_wave_name[2].isEmpty()) {
    if((edit_wave_origin[0]-
	edit_logline[0]->endPoint())>0) {
      track_track1_button->setDisabled(true);
      track_record_button->setDisabled(true);
    }
  }
  else {
    if(edit_wave_origin[2]>0) {
      track_track1_button->setDisabled(true);
      track_record_button->setDisabled(true);
    }
  }
  LoadBlockLength(track_line);
  RDListViewItem *item=NULL;
  if(track_log_event->logLine(track_line)->type()==RDLogLine::Track) {
    item=GetItemByLine(track_line+1);
  }
  else {
    item=GetItemByLine(track_line);
  }
  if(item!=NULL) {
    RefreshLine(item);
    if(GetItemByLine(item->line()+1)!=NULL) {
      RefreshLine(GetItemByLine(item->line()+1));
    }  
  }
  UpdateRemaining();
  UpdateControls();
}


void VoiceTracker::DragTarget(int trackno,const QPoint &pt)
{
  //  printf("DragTarget(%d,(%d,%d))  TARGET: %d\n",trackno,pt.x(),pt.y(),track_current_target);  

  if(!track_changed) {
    PushSegues();
    track_changed=true;
    UpdateControls();
  }
  switch(track_current_target) {
      case VoiceTracker::FadedownPoint:
        edit_logline[trackno]->
	  setDuckDownGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
	if(edit_logline[trackno]->duckDownGain()>TRACKER_MAX_GAIN) {
	  edit_logline[trackno]->setDuckDownGain(TRACKER_MAX_GAIN);
	}
	if(edit_logline[trackno]->duckDownGain()<TRACKER_MIN_GAIN) {
	  edit_logline[trackno]->setDuckDownGain(TRACKER_MIN_GAIN);
        }   
      case VoiceTracker::TrackFadedownPoint:
	if((edit_logline[trackno]->fadedownPoint()<=0 || 
	   edit_logline[trackno]->fadedownPoint()>=edit_logline[trackno]->endPoint())
	   && edit_logline[trackno]->fadedownGain()==0) {
	   edit_logline[trackno]->setFadedownGain(RD_FADE_DEPTH);
	}
        edit_logline[trackno]->setSegueGain(0);
        if(edit_logline[trackno]->fadedownPoint(RDLogLine::LogPointer)<0) {
          edit_logline[trackno]->setFadedownGain(RD_FADE_DEPTH);
        }
        if(trackno<2) {
          if((edit_logline[trackno+1]->transType()==RDLogLine::Segue)
                 && (!track_loaded)) {
             edit_logline[trackno]->setEndPoint(
                edit_logline[trackno]->segueEndPoint(),RDLogLine::LogPointer);
          }
        } 
	edit_logline[trackno]->
	  setFadedownPoint(pt.x()*TRACKER_MSECS_PER_PIXEL+
			   edit_wave_origin[trackno],RDLogLine::LogPointer);
	if(edit_logline[trackno]->fadedownPoint()>edit_logline[trackno]->
	   endPoint()) {
	  edit_logline[trackno]->setFadedownPoint(edit_logline[trackno]->
					    endPoint(),RDLogLine::LogPointer);
	}
	if(edit_logline[trackno]->fadedownPoint()<
	   edit_logline[trackno]->fadeupPoint()) {
	  edit_logline[trackno]->
	    setFadedownPoint(edit_logline[trackno]->fadeupPoint(),
            RDLogLine::LogPointer);;
	}
	if(edit_logline[trackno]->fadedownPoint(RDLogLine::CartPointer)>=0 
	   &&  edit_logline[trackno]->fadedownPoint()>
	   edit_logline[trackno]->fadedownPoint(RDLogLine::CartPointer)) {
	  edit_logline[trackno]->
	    setFadedownPoint(edit_logline[trackno]->fadedownPoint(RDLogLine::CartPointer),
            RDLogLine::LogPointer);
	}
	DrawTrackMap(trackno);
	WriteTrackMap(trackno);
        track_meter->setLeftPeakBar(edit_logline[trackno]->duckDownGain()-1600);
        track_meter->setRightPeakBar(edit_logline[trackno]->duckDownGain()-1600);
        if(edit_logline[trackno+1]!=NULL) {
          edit_logline[trackno+1]->setHasCustomTransition(true);
        }  
	break;

      case VoiceTracker::FadedownGain:
      case VoiceTracker::TrackFadedownGain:
        if(edit_logline[trackno]->fadedownPoint()>edit_logline[trackno]->endPoint()  
              || edit_logline[trackno]->fadedownPoint()<0) {
          edit_logline[trackno]->setFadedownPoint(edit_logline[trackno]->endPoint(),RDLogLine::LogPointer);
          }
	edit_logline[trackno]->
	  setFadedownGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
	if(edit_logline[trackno]->fadedownGain()>TRACKER_MAX_GAIN) {
	  edit_logline[trackno]->setFadedownGain(TRACKER_MAX_GAIN);
	}
	if(edit_logline[trackno]->fadedownGain()<TRACKER_MIN_GAIN) {
	  edit_logline[trackno]->setFadedownGain(TRACKER_MIN_GAIN);
	}
	if(edit_logline[trackno]->fadedownPoint(RDLogLine::CartPointer)>=0
	   && edit_logline[trackno]->fadedownPoint(RDLogLine::CartPointer)<
	      edit_logline[trackno]->endPoint()
	    ) {
	  edit_logline[trackno]->setFadedownGain(RD_FADE_DEPTH);
	}  
	DrawTrackMap(trackno);
	WriteTrackMap(trackno);
        track_meter->setLeftPeakBar(edit_logline[trackno]->fadedownGain()-1600);
        track_meter->setRightPeakBar(edit_logline[trackno]->fadedownGain()-1600);
        if(edit_logline[trackno+1]!=NULL) {
          edit_logline[trackno+1]->setHasCustomTransition(true);
        }  
	break;

      case VoiceTracker::FadeupPoint:
        edit_logline[trackno]->
	  setDuckUpGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
	if(edit_logline[trackno]->duckUpGain()>TRACKER_MAX_GAIN) {
	  edit_logline[trackno]->setDuckUpGain(TRACKER_MAX_GAIN);
	}
	if(edit_logline[trackno]->duckUpGain()<TRACKER_MIN_GAIN) {
	  edit_logline[trackno]->setDuckUpGain(TRACKER_MIN_GAIN);
        }
      case VoiceTracker::TrackFadeupPoint:
	if(edit_logline[trackno]->fadeupPoint()<=edit_logline[trackno]->startPoint()
	   && edit_logline[trackno]->fadeupGain()==0) {
	   edit_logline[trackno]->setFadeupGain(RD_FADE_DEPTH);
	}
        if(edit_logline[trackno]->fadeupPoint(RDLogLine::LogPointer)<0) {
          edit_logline[trackno]->setFadeupGain(RD_FADE_DEPTH);
        }
	edit_logline[trackno]->
	  setFadeupPoint(pt.x()*TRACKER_MSECS_PER_PIXEL+
			 edit_wave_origin[trackno],RDLogLine::LogPointer);
	if(edit_logline[trackno]->fadeupPoint()>
	   edit_logline[trackno]->fadedownPoint()
           && edit_logline[trackno]->fadedownPoint()>=0) {
	  edit_logline[trackno]->setFadeupPoint(edit_logline[trackno]->
						fadedownPoint(),RDLogLine::LogPointer);
	}
	if(edit_logline[trackno]->fadeupPoint()<edit_logline[trackno]->
	   startPoint()) {
	  edit_logline[trackno]->setFadeupPoint(edit_logline[trackno]->
					  startPoint(),RDLogLine::LogPointer);
	}
	if(edit_logline[trackno]->fadeupPoint()<
	   edit_logline[trackno]->fadeupPoint(RDLogLine::CartPointer)) {
	  edit_logline[trackno]->
	    setFadeupPoint(edit_logline[trackno]->fadeupPoint(RDLogLine::CartPointer),
            RDLogLine::LogPointer);
	}
	DrawTrackMap(trackno);
	WriteTrackMap(trackno);
        track_meter->setLeftPeakBar(edit_logline[trackno]->duckUpGain()-1600);
        track_meter->setRightPeakBar(edit_logline[trackno]->duckUpGain()-1600);
        edit_logline[trackno]->setHasCustomTransition(true);
	break;

      case VoiceTracker::FadeupGain:
      case VoiceTracker::TrackFadeupGain:
        if(edit_logline[trackno]->fadeupPoint()<edit_logline[trackno]->startPoint()) {
          edit_logline[trackno]->setFadeupPoint(edit_logline[trackno]->startPoint(),RDLogLine::LogPointer);
          }
	edit_logline[trackno]->
	  setFadeupGain((TRACKER_GAIN_MARGIN-pt.y())*TRACKER_MB_PER_PIXEL);
	if(edit_logline[trackno]->fadeupGain()>TRACKER_MAX_GAIN) {
	  edit_logline[trackno]->setFadeupGain(TRACKER_MAX_GAIN);
	}
	if(edit_logline[trackno]->fadeupGain()<TRACKER_MIN_GAIN) {
	  edit_logline[trackno]->setFadeupGain(TRACKER_MIN_GAIN);
	}
	if(edit_logline[trackno]->fadeupPoint(RDLogLine::CartPointer)>
	      edit_logline[trackno]->startPoint()) {
	  edit_logline[trackno]->setFadeupGain(RD_FADE_DEPTH);
	}  
	DrawTrackMap(trackno);
	WriteTrackMap(trackno);
        track_meter->setLeftPeakBar(edit_logline[trackno]->fadeupGain()-1600);
        track_meter->setRightPeakBar(edit_logline[trackno]->fadeupGain()-1600);
        edit_logline[trackno]->setHasCustomTransition(true);
	break;

      case VoiceTracker::TargetSize:
	break;
  }
  track_changed=true;
  RDListViewItem *item=NULL;
  if(track_log_event->logLine(track_line)->type()==RDLogLine::Track) {
    item=GetItemByLine(track_line+1);
  }
  else {
    item=GetItemByLine(track_line);
  }
  if(item!=NULL) {
    RefreshLine(item);
    if(GetItemByLine(item->line()+1)!=NULL) {
      RefreshLine(GetItemByLine(item->line()+1));
    }  
  }
  UpdateControls();
}


void VoiceTracker::DrawTrackMap(int trackno)
{
  QTime track_time;
  QPainter *p=NULL;
  int xpos=0;
  QColor back_color;
  switch(trackno) {
  case 0:
    if(edit_wave_name[0].isEmpty()) {
      p=new QPainter(edit_wave_map[0]);
      p->setBackgroundColor(gray);
      p->eraseRect(0,0,edit_wave_map[0]->size().width(),
		   edit_wave_map[0]->size().height());
      p->end();
      delete p;
    }
    else {
      wpg[0]->begin(edit_wave_map[0]);
      wpg[0]->setFont(QFont("Helvetica",12,QFont::Bold));
      wpg[0]->setPen(TRACKER_TEXT_COLOR);
      wpg[0]->setBackgroundColor(backgroundColor());
      wpg[0]->eraseRect(0,0,edit_wave_map[0]->size().width(),
			edit_wave_map[0]->size().height());
      if(!edit_wave_name[0].isEmpty()) {
	wpg[0]->drawWaveByMsecs(0,edit_wave_map[0]->width(),
				edit_wave_origin[0],
				edit_wave_origin[0]+edit_wave_width,800,
				RDWavePainter::Mono,black,
				edit_logline[0]->
				startPoint(RDLogLine::CartPointer),
				edit_logline[0]->
				endPoint(RDLogLine::CartPointer));

	//
	// Draw Segue Markers
	//
	if(edit_logline[0]->segueStartPoint(RDLogLine::CartPointer)>=0) {
	  xpos=(edit_logline[0]->segueStartPoint(RDLogLine::CartPointer)-
		edit_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(wpg[0],edit_wave_map[0]->height(),xpos,
		     RD_SEGUE_MARKER_COLOR,20,true);
	  xpos=(edit_logline[0]->segueEndPoint(RDLogLine::CartPointer)-
		edit_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(wpg[0],edit_wave_map[0]->height(),xpos,
		     RD_SEGUE_MARKER_COLOR,20,false);
	}

	//
	// Draw Start Marker
	//
	xpos=(edit_logline[0]->startPoint(RDLogLine::CartPointer)-
	      edit_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	DrawCursor(wpg[0],edit_wave_map[0]->height(),xpos,
		   RD_START_END_MARKER_COLOR,10,true);

	//
	// Draw End Marker
	//
	xpos=(edit_logline[0]->endPoint(RDLogLine::CartPointer)-
	      edit_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	DrawCursor(wpg[0],edit_wave_map[0]->height(),xpos,
		   RD_START_END_MARKER_COLOR,10,false);

	//
	// Draw Fadedown Marker
	//
	if(edit_logline[0]->fadedownPoint(RDLogLine::CartPointer)>=0) {
	  xpos=(edit_logline[0]->fadedownPoint(RDLogLine::CartPointer)-
		edit_wave_origin[0])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(wpg[0],edit_wave_map[0]->height(),xpos,
		     RD_FADE_MARKER_COLOR,30,true);
	}

	//
	// Draw Rubber Bands
	//
	DrawRubberBand(wpg[0],0);
   
	//
	// Draw Menu Marker
	//
        if(menu_clicked_point>=0 && edit_rightclick_track==0) {
	  DrawCursor(wpg[0],edit_wave_map[0]->height(),menu_clicked_point,
		     RD_START_END_MARKER_COLOR,10,true);
	  DrawCursor(wpg[0],edit_wave_map[0]->height(),menu_clicked_point,
		     RD_START_END_MARKER_COLOR,10,false);
        }	    
      }
      wpg[0]->setPen(TRACKER_TEXT_COLOR);
      wpg[0]->drawText(5,14,QString().sprintf("%s - %s",
					      (const char *)edit_logline[0]->title(),
					      (const char *)edit_logline[0]->artist()));
      wpg[0]->end();
    }
    break;

  case 1:
    if(edit_wave_name[1].isEmpty()) {
      p=new QPainter(edit_wave_map[1]);
      if(track_loaded) {
	p->setBackgroundColor(backgroundColor());
	p->setFont(QFont("Helvetica",12,QFont::Bold));
	p->setPen(TRACKER_TEXT_COLOR);
	p->eraseRect(0,0,edit_wave_map[1]->size().width(),
		     edit_wave_map[1]->size().height());
	p->drawText(5,14,edit_logline[1]->markerComment());
      }
      else {
	p->setBackgroundColor(gray);
	p->eraseRect(0,0,edit_wave_map[1]->size().width(),
		     edit_wave_map[1]->size().height());
      }
      if(track_start_time>QTime(0,0,0)) {
        p->setFont(QFont("Helvetica",12,QFont::Bold));
	p->setPen(TRACKER_TEXT_COLOR);
        p->drawText(550,75,QString().sprintf("Start %s", 
					     (const char*)track_start_time.toString("h:mm:ss")));
      }           
      p->end();
      delete p;
    }
    else {
      if((edit_logline[1]->transType()==RDLogLine::Segue)) {
	back_color=backgroundColor();
      }
      else {
	back_color=lightGray;
      }
      switch(edit_deck_state) {
      case VoiceTracker::DeckTrack2:
      case VoiceTracker::DeckTrack3:
	p=new QPainter(edit_wave_map[1]);
	p->setPen(TRACKER_RECORD_COLOR);
	p->setBrush(TRACKER_RECORD_COLOR);
	p->setBackgroundColor(back_color);
	p->eraseRect(0,0,edit_wave_map[1]->size().width(),
		     edit_wave_map[1]->size().height());
	p->fillRect(-edit_wave_origin[1]/
		    TRACKER_MSECS_PER_PIXEL,
		    TRACKER_Y_HEIGHT/4,
		    track_recording_pos/TRACKER_MSECS_PER_PIXEL,
		    TRACKER_Y_HEIGHT/2,TRACKER_RECORD_COLOR);
	p->setFont(QFont("Helvetica",12,QFont::Bold));
        if(track_start_time>QTime(0,0,0)) {
	  p->setPen(TRACKER_TEXT_COLOR);
	  track_time=track_start_time;
	  track_time=track_time.addMSecs(
					 track_time_remaining_start-track_time_counter);
          p->drawText(550,75,QString().sprintf("Time %s", 
					       (const char*)track_time.toString("h:mm:ss")));
        }                  
	p->end();
	delete p;
	break;

      default:
	wpg[1]->begin(edit_wave_map[1]);
	wpg[1]->setFont(QFont("Helvetica",12,QFont::Bold));
	wpg[1]->setPen(TRACKER_TEXT_COLOR);
	wpg[1]->setBackgroundColor(back_color);
	wpg[1]->eraseRect(0,0,edit_wave_map[1]->size().width(),
			  edit_wave_map[1]->size().height());
	if(!edit_wave_name[1].isEmpty()) {
	  wpg[1]->drawWaveByMsecs(0,edit_wave_map[1]->width(),
				  edit_wave_origin[1],
				  edit_wave_origin[1]+edit_wave_width,800,
				  RDWavePainter::Mono,black,
				  edit_logline[1]->
				  startPoint(RDLogLine::CartPointer),
				  edit_logline[1]->
				  endPoint(RDLogLine::CartPointer));
	}
	if(track_line>=0) {

	  //
	  // Draw Segue Markers
	  //
	  if(edit_logline[1]->segueStartPoint(RDLogLine::CartPointer)>=0) {
	    xpos=(edit_logline[1]->segueStartPoint(RDLogLine::CartPointer)-
		  edit_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(wpg[1],edit_wave_map[1]->height(),xpos,
		       RD_SEGUE_MARKER_COLOR,20,true);
	    xpos=(edit_logline[1]->segueEndPoint(RDLogLine::CartPointer)-
		  edit_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(wpg[1],edit_wave_map[1]->height(),xpos,
		       RD_SEGUE_MARKER_COLOR,20,false);
	    }

	  //
	  // Draw Fadeup Marker
	  //
	  if(edit_logline[1]->fadeupPoint(RDLogLine::CartPointer)>=0) {
	    xpos=(edit_logline[1]->fadeupPoint(RDLogLine::CartPointer)-
		  edit_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(wpg[1],edit_wave_map[1]->height(),xpos,
		       RD_FADE_MARKER_COLOR,30,false);
	  }
               
	  //
	  // Draw Start Marker
	  //
	  xpos=(edit_logline[1]->startPoint(RDLogLine::CartPointer)-
		edit_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(wpg[1],edit_wave_map[1]->height(),xpos,
		     RD_START_END_MARKER_COLOR,10,true);

	  //
	  // Draw Fadedown Marker
	  //
	  if(edit_logline[1]->fadedownPoint(RDLogLine::CartPointer)>=0) {
	    xpos=(edit_logline[1]->fadedownPoint(RDLogLine::CartPointer)-
		  edit_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	    DrawCursor(wpg[1],edit_wave_map[1]->height(),xpos,
		       RD_FADE_MARKER_COLOR,30,true);
	  }
              
	  //
	  // Draw End Marker
	  //
	  xpos=(edit_logline[1]->endPoint(RDLogLine::CartPointer)-
		edit_wave_origin[1])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(wpg[1],edit_wave_map[1]->height(),xpos,
		     RD_START_END_MARKER_COLOR,10,false);

	  //
	  // Draw Rubber Bands
	  //
	  DrawRubberBand(wpg[1],1);

	  //
	  // Draw Menu Marker
	  //
          if(menu_clicked_point>=0 && edit_rightclick_track==1) {
	    DrawCursor(wpg[1],edit_wave_map[1]->height(),menu_clicked_point,
		       RD_START_END_MARKER_COLOR,10,true);
	    DrawCursor(wpg[1],edit_wave_map[1]->height(),menu_clicked_point,
		       RD_START_END_MARKER_COLOR,10,false);
          }       
	  wpg[1]->setPen(TRACKER_TEXT_COLOR);
	  wpg[1]->drawText(5,14,edit_logline[1]->title());
	}
        if(track_start_time>QTime(0,0,0)) {
	  wpg[1]->drawText(550,75,QString().sprintf("Start %s", 
						    (const char*)track_start_time.toString("h:mm:ss")));
        }
	wpg[1]->end();
	break;
      }
    }
    break;

  case 2:
    if(edit_wave_name[2].isEmpty()) {
      p=new QPainter(edit_wave_map[2]);
      p->setBackgroundColor(gray);
      p->eraseRect(0,0,edit_wave_map[2]->size().width(),
		   edit_wave_map[2]->size().height());
      p->end();
      delete p;
    }
    else {
      if((edit_logline[2]->transType()==RDLogLine::Segue)) {
	back_color=backgroundColor();
      }
      else {
	back_color=lightGray;
      }
      wpg[2]->begin(edit_wave_map[2]);
      wpg[2]->setFont(QFont("Helvetica",12,QFont::Bold));
      wpg[2]->setBackgroundColor(back_color);
      wpg[2]->eraseRect(0,0,edit_wave_map[2]->size().width(),
			edit_wave_map[2]->size().height());
      if(!edit_wave_name[2].isEmpty()) {
	wpg[2]->drawWaveByMsecs(0,edit_wave_map[2]->width(),
				edit_wave_origin[2],
				edit_wave_origin[2]+edit_wave_width,800,
				RDWavePainter::Mono,black,
				edit_logline[2]->startPoint(RDLogLine::CartPointer),
                                edit_logline[2]->endPoint(RDLogLine::CartPointer));

	//
	// Draw Talk Markers
	//
	if(edit_logline[2]->talkEndPoint()>0) {
	  int tsxpos;
	  int texpos;
	  if(edit_logline[2]->talkStartPoint()==0){
	    tsxpos=(edit_logline[2]->startPoint()-
		    edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	  }
	  else {
	    tsxpos=(edit_logline[2]->talkStartPoint()-
		    edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	  }
	  if(edit_logline[2]->talkEndPoint()==0) {
	    texpos=(edit_logline[2]->startPoint()-
		    edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	  }
	  else {
	    if(edit_logline[2]->talkStartPoint()==0) {
	      texpos=(edit_logline[2]->startPoint()+
                      edit_logline[2]->talkEndPoint()-
		      edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	    }
	    else {
	      texpos=(edit_logline[2]->talkEndPoint()-
		      edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	    }
	  }
	  if(tsxpos!=texpos){
	    DrawCursor(wpg[2],edit_wave_map[2]->height(),tsxpos,
		       RD_TALK_MARKER_COLOR,20,true);
	    DrawCursor(wpg[2],edit_wave_map[2]->height(),texpos,
		       RD_TALK_MARKER_COLOR,20,false);
	  }
	}

	//
	// Draw Fadeup Marker
	//
	if(edit_logline[2]->fadeupPoint(RDLogLine::CartPointer)>=0) {
	  xpos=(edit_logline[2]->fadeupPoint(RDLogLine::CartPointer)-
		edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	  DrawCursor(wpg[2],edit_wave_map[2]->height(),xpos,
		     RD_FADE_MARKER_COLOR,30,false);
	}
	      	         
	//
	// Draw Start Marker
	//
	xpos=(edit_logline[2]->startPoint(RDLogLine::CartPointer)-
	      edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	DrawCursor(wpg[2],edit_wave_map[2]->height(),xpos,
		   RD_START_END_MARKER_COLOR,10,true);

	//
	// Draw End Marker
	//
	xpos=(edit_logline[2]->endPoint(RDLogLine::CartPointer)-
	      edit_wave_origin[2])/TRACKER_MSECS_PER_PIXEL;
	DrawCursor(wpg[2],edit_wave_map[2]->height(),xpos,
		   RD_START_END_MARKER_COLOR,10,false);

	//
	// Draw Menu Marker
	//
        if(menu_clicked_point>=0 && edit_rightclick_track==2) {
	  DrawCursor(wpg[2],edit_wave_map[2]->height(),menu_clicked_point,
		     RD_START_END_MARKER_COLOR,10,true);
	  DrawCursor(wpg[2],edit_wave_map[2]->height(),menu_clicked_point,
		     RD_START_END_MARKER_COLOR,10,false);
        }       
 
	//
	// Draw Rubber Bands
	//
	DrawRubberBand(wpg[2],2);
      }
      wpg[2]->setPen(TRACKER_TEXT_COLOR);
      wpg[2]->drawText(5,14,QString().sprintf("%s - %s",
					      (const char *)edit_logline[2]->title(),
					      (const char *)edit_logline[2]->artist()));
      if(track_recording && (edit_deck[2]->state()==RDPlayDeck::Playing ||
			     edit_deck[2]->state()==RDPlayDeck::Stopping)) {
	int talk_len=edit_logline[2]->talkLength();
	if(talk_len>0 && edit_deck[2]->currentPosition()>
	   edit_logline[2]->talkStartPoint()){
	  talk_len-=edit_deck[2]->currentPosition()-
	    edit_logline[2]->talkStartPoint();
	}
	wpg[2]->drawText(550,75,QString().sprintf("Talk :%d", 
						  (talk_len+500)/1000));
      }
      else {
	wpg[2]->drawText(550,75,QString().sprintf("Talk :%d", 
						  (edit_logline[2]->talkLength()+500)/1000));
      }

      wpg[2]->end();
    }
    break;
  }
}


void VoiceTracker::DrawCursor(RDWavePainter *wp,int height,int xpos,
			      const QColor &color,int arrow_offset,
			      bool left_arrow)
{
  wp->setPen(color);
  wp->setBrush(color);
  wp->moveTo(xpos,0);
  wp->lineTo(xpos,height);
  int arrow_x=8;
  if(left_arrow) {
    arrow_x=-8;
  }
  QPointArray *pa=new QPointArray(3);
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


void VoiceTracker::DrawRubberBand(RDWavePainter *wp,int trackno)
{
  int xpos=0;
  int ypos=0;

  wp->setPen(TRACKER_RUBBERBAND_COLOR);
  wp->setBrush(TRACKER_RUBBERBAND_COLOR);
  
  int draw_fadeup_gain=edit_logline[trackno]->fadeupGain();
  int draw_fadedown_gain=edit_logline[trackno]->fadedownGain();
  int draw_fadedown_point=edit_logline[trackno]->fadedownPoint();
  int draw_duckup_gain=edit_logline[trackno]->duckUpGain();
  int draw_duckdown_gain=edit_logline[trackno]->duckDownGain();
  int draw_end_point=edit_logline[trackno]->endPoint();
  if(edit_logline[trackno]->fadedownPoint(RDLogLine::LogPointer)<0) {
     draw_fadedown_gain=RD_FADE_DEPTH;
  }
  if(edit_logline[trackno]->fadeupPoint(RDLogLine::LogPointer)<0) {
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
  if(edit_logline[trackno]->fadeupPoint()<=edit_logline[trackno]->startPoint()
     && draw_fadeup_gain==0)
    draw_fadeup_gain=TRACKER_MIN_GAIN;
  if(edit_logline[trackno]->fadedownPoint()<0 && draw_fadedown_gain==0)
    draw_fadedown_gain=TRACKER_MIN_GAIN;
  if(edit_logline[trackno]->fadedownPoint()>=edit_logline[trackno]->endPoint() 
     && draw_fadedown_gain==0)
    draw_fadedown_gain=TRACKER_MIN_GAIN;
  if(edit_logline[trackno]->segueGain()<0
        && edit_logline[trackno]->segueStartPoint()>=0
        && edit_logline[trackno]->segueStartPoint()<draw_fadedown_point
        && (!track_loaded)) {
    if(trackno<2) {
      if(edit_logline[trackno+1]->transType()==RDLogLine::Segue) {
        draw_fadedown_point=edit_logline[trackno]->segueStartPoint();
        draw_fadedown_gain=edit_logline[trackno]->segueGain();
      }
    }
  }      
  if(trackno<2) {
    if((edit_logline[trackno+1]->transType()==RDLogLine::Segue)
        && (!track_loaded)) {
       draw_end_point=edit_logline[trackno]->segueEndPoint();
       if(draw_fadedown_point>draw_end_point) {
         draw_fadedown_point=draw_end_point;
       }
     } 
   }
  
  //
  // Fade Up
  //
  xpos=(edit_logline[trackno]->startPoint()-
	edit_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
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
  wp->moveTo(xpos,ypos);
  switch(trackno) {
      case 1:
	DrawTarget(wp,VoiceTracker::TrackFadeupGain,trackno,xpos,ypos);
	break;
	
      case 2:
	DrawTarget(wp,VoiceTracker::FadeupGain,trackno,xpos,ypos);
	break;
  }
  xpos=(edit_logline[trackno]->fadeupPoint()-
	edit_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
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
  wp->lineTo(xpos,ypos);
  switch(trackno) {
      case 1:
	DrawTarget(wp,VoiceTracker::TrackFadeupPoint,trackno,xpos,ypos);
	break;
	
      case 2:
	DrawTarget(wp,VoiceTracker::FadeupPoint,trackno,xpos,ypos);
	break;
  }

  //
  // Body of cut
  //
  xpos=(edit_logline[trackno]->fadeupPoint()-
	edit_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  wp->moveTo(xpos,ypos);
  if(draw_fadedown_point>=
      edit_logline[trackno]->fadeupPoint())
  xpos=(draw_fadedown_point-
	edit_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  else
    xpos=(draw_end_point-
	 edit_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;

  wp->lineTo(xpos,ypos);

  //
  // Fade Down
  switch(trackno) {
      case 0:
	DrawTarget(wp,VoiceTracker::FadedownPoint,trackno,xpos,ypos);
	break;
	
      case 1:
	DrawTarget(wp,VoiceTracker::TrackFadedownPoint,trackno,xpos,ypos);
	break;
  }
  xpos=(draw_end_point-
	edit_wave_origin[trackno])/TRACKER_MSECS_PER_PIXEL;
  if(trackno==2) {
  ypos=TRACKER_GAIN_MARGIN-
        draw_duckup_gain/TRACKER_MB_PER_PIXEL-
        (int)((double)draw_fadedown_gain*
        (1-(double)draw_duckup_gain/TRACKER_MIN_GAIN))
         /TRACKER_MB_PER_PIXEL;
  }
  else {
    ypos=TRACKER_GAIN_MARGIN-
        edit_logline[trackno]->duckDownGain()/TRACKER_MB_PER_PIXEL-
        (int)((double)draw_fadedown_gain*
        (1-(double)draw_duckdown_gain/TRACKER_MIN_GAIN))
         /TRACKER_MB_PER_PIXEL;
  }
  wp->lineTo(xpos,ypos);
  switch(trackno) {
      case 0:
	DrawTarget(wp,VoiceTracker::FadedownGain,trackno,xpos,ypos);
	break;
	
      case 1:
	DrawTarget(wp,VoiceTracker::TrackFadedownGain,trackno,xpos,ypos);
	break;
  }
}


void VoiceTracker::DrawTarget(RDWavePainter *wp,VoiceTracker::Target target,
			      int trackno,int xpos,int ypos)
{
  track_target_rect[target]->setX(xpos-TRACKER_GAIN_MARGIN);
  track_target_rect[target]->setY(ypos-TRACKER_GAIN_MARGIN);
  track_target_rect[target]->setWidth(TRACKER_GAIN_MARGIN*2);
  track_target_rect[target]->setHeight(TRACKER_GAIN_MARGIN*2);
  track_target_track[target]=trackno;
  wp->drawRect(*(track_target_rect[target]));
}


void VoiceTracker::WriteTrackMap(int trackno)
{
  QPainter *p=new QPainter(this);
  switch(trackno) {
      case 0:
	p->drawPixmap(TRACKER_X_ORIGIN,TRACKER_Y_ORIGIN,*edit_wave_map[0]);
	break;

      case 1:
	p->drawPixmap(TRACKER_X_ORIGIN,TRACKER_Y_HEIGHT+TRACKER_Y_ORIGIN,
		      *edit_wave_map[1]);
	break;

      case 2:
	p->drawPixmap(TRACKER_X_ORIGIN,TRACKER_Y_HEIGHT*2+TRACKER_Y_ORIGIN,
		      *edit_wave_map[2]);
	break;
  }
  p->end();
  delete p;
}


bool VoiceTracker::TransportActive()
{
  return track_recording||PlayoutActive();
}


bool VoiceTracker::PlayoutActive()
{
  for(int i=0;i<3;i++) {
    if((edit_deck[i]->state()==RDPlayDeck::Playing)||
       (edit_deck[i]->state()==RDPlayDeck::Stopping)) {
      return true;
    }
  }
  return false;
}


void VoiceTracker::UpdateControls()
{
  bool transport_idle=!TransportActive();

  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if((item==NULL)||(item->line()==TRACKER_MAX_LINENO)) {
    track_track1_button->setDisabled(true);
    track_track1_button->setText(tr("Start"));
    track_track1_button->setPalette(track_start_palette);
    track_record_button->setDisabled(true);
    track_record_button->setText(tr("Record"));
    track_track2_button->setDisabled(true);
    track_finished_button->setDisabled(true);
    track_reset_button->setDisabled(true);
    track_post_button->setDisabled(true);
    track_play_button->setDisabled(true);
    track_stop_button->setDisabled(true);
    track_next_button->setEnabled(transport_idle);
    track_previous_button->setEnabled(transport_idle);
    track_insert_button->setEnabled(transport_idle&&CanInsertTrack());
    track_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
    track_close_button->setEnabled(true);
    track_log_list->setEnabled(transport_idle);
    return;
  }
  RDLogLine *real_logline=track_log_event->logLine(item->line());
  RDLogLine *logline=NULL;
  if(track_offset) {
    logline=track_log_event->logLine(item->line()-1);
  }
  else {
    logline=real_logline;
  }
  if(track_loaded) {
    if(logline->type()==RDLogLine::Track) {  // Unfinished Track
      switch(edit_deck_state) {
	  case VoiceTracker::DeckIdle:
	    if(edit_wave_name[0].isEmpty()) {
	      track_track1_button->setEnabled(!track_group->name().isEmpty());
	      track_track1_button->setText(tr("Import"));
	      track_track1_button->setPalette(track_record_palette);
	      track_record_button->setEnabled(!track_group->name().isEmpty());
	      track_record_button->setText(tr("Record"));
	      track_record_button->setFocus();
	    }
	    else {
		    if((logline->transType()==RDLogLine::Segue)) {
		track_track1_button->
		  setEnabled(!track_group->name().isEmpty());
		track_track1_button->setText(tr("Start"));
		track_track1_button->setPalette(track_start_palette);
		track_track1_button->setFocus();
		track_record_button->
		  setEnabled(!track_group->name().isEmpty());
		track_record_button->setText(tr("Import"));
	      }
	      else {
		track_track1_button->
		  setEnabled(!track_group->name().isEmpty());
		track_track1_button->setText(tr("Import"));
		track_track1_button->setPalette(track_record_palette);
		track_record_button->
		  setEnabled(!track_group->name().isEmpty());
		track_record_button->setText(tr("Record"));
		track_record_button->setFocus();
	      }
	    }
	    track_track2_button->setDisabled(true);
	    track_finished_button->setPalette(track_done_palette);
	    track_finished_button->setText(tr("Save"));
	    track_finished_button->setEnabled(track_changed);
	    track_reset_button->
	      setEnabled(real_logline->hasCustomTransition());
	    track_post_button->setDisabled(true);
	    track_play_button->setEnabled(true);
	    track_stop_button->setEnabled(true);
	    track_next_button->setEnabled(transport_idle);
	    track_previous_button->setEnabled(transport_idle);
	    track_insert_button->setEnabled(transport_idle&&CanInsertTrack());
	    track_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
	    track_close_button->setEnabled(true);
	    track_log_list->setEnabled(transport_idle);
	    break;
	    
	  case VoiceTracker::DeckTrack1:
	    track_track1_button->setDisabled(true);
	    track_track1_button->setText(tr("Start"));
	    track_track1_button->setPalette(track_start_palette);
	    track_record_button->setEnabled(true);
	    track_record_button->setText(tr("Record"));
	    track_record_button->setFocus();
	    track_track2_button->setDisabled(true);
	    track_finished_button->setPalette(track_abort_palette);
	    track_finished_button->setText(tr("Abort"));
	    track_finished_button->setEnabled(true);
	    track_reset_button->setDisabled(true);
	    track_post_button->setDisabled(true);
	    track_play_button->setDisabled(true);
	    track_stop_button->setDisabled(true);
	    track_next_button->setDisabled(true);
	    track_previous_button->setDisabled(true);
	    track_insert_button->setDisabled(true);
	    track_delete_button->setDisabled(true);
	    track_close_button->setDisabled(true);
	    track_log_list->setDisabled(true);
	    break;
	    
	  case VoiceTracker::DeckTrack2:
	    track_track1_button->setDisabled(true);
	    track_track1_button->setText(tr("Start"));
	    track_track1_button->setPalette(track_start_palette);
	    track_record_button->setDisabled(true);
	    track_record_button->setText(tr("Record"));
	    if(edit_wave_name[2].isEmpty()) {
	      track_finished_button->setPalette(track_done_palette);
	      track_finished_button->setText(tr("Save"));
	      track_finished_button->setFocus();
	      track_track2_button->setDisabled(true);
	    }
	    else {
	      if((edit_logline[2]->transType()==RDLogLine::Segue)) {
		if(rdlogedit_conf->enableSecondStart()) {
		  track_finished_button->setPalette(track_abort_palette);
		  track_finished_button->setText(tr("Abort"));
		}
		else {
		  track_finished_button->setPalette(track_done_palette);
		  track_finished_button->setText(tr("Save"));
		}
		track_track2_button->setEnabled(true);
	      }
	      else {
		track_finished_button->setText(tr("Save"));
		track_finished_button->setPalette(track_done_palette);
		track_track2_button->setDisabled(true);
	      }
	      track_track2_button->setFocus();
	    }
	    track_finished_button->setEnabled(true);
	    track_reset_button->setDisabled(true);
	    track_insert_button->setDisabled(true);
	    track_delete_button->setDisabled(true);
	    track_post_button->setDisabled(true);
	    track_play_button->setDisabled(true);
	    track_stop_button->setDisabled(true);
	    track_next_button->setDisabled(true);
	    track_previous_button->setDisabled(true);
	    track_close_button->setDisabled(true);
	    track_log_list->setDisabled(true);
	    break;
	    
	  case VoiceTracker::DeckTrack3:
	    track_track1_button->setDisabled(true);
	    track_track1_button->setText(tr("Start"));
	    track_track1_button->setPalette(track_start_palette);
	    track_record_button->setDisabled(true);
	    track_record_button->setText(tr("Record"));
	    track_track2_button->setDisabled(true);
	    track_finished_button->setPalette(track_done_palette);
	    track_finished_button->setText(tr("Save"));
	    track_finished_button->setEnabled(true);
	    track_finished_button->setFocus();
	    track_reset_button->setDisabled(true);
	    track_post_button->setDisabled(true);
	    track_insert_button->setDisabled(true);
	    track_delete_button->setDisabled(true);
	    track_play_button->setDisabled(true);
	    track_stop_button->setDisabled(true);
	    track_next_button->setDisabled(true);
	    track_previous_button->setDisabled(true);
	    track_close_button->setDisabled(true);
	    track_log_list->setDisabled(true);
	    break;
      }
    }
    else {         // Completed Track
      track_track1_button->setDisabled(true);
      track_track1_button->setText(tr("Start"));
      track_track1_button->setPalette(track_start_palette);
      track_record_button->setDisabled(true);
      track_record_button->setText(tr("Record"));
      track_track2_button->setDisabled(true);
      track_finished_button->setEnabled(track_changed);
      track_reset_button->setEnabled(transport_idle);
      track_post_button->
	setEnabled(transport_idle&&(!edit_wave_name[2].isEmpty()));
      track_play_button->setEnabled(true);
      track_stop_button->setEnabled(true);
      track_next_button->setEnabled(transport_idle);
      track_previous_button->setEnabled(transport_idle);
      if(transport_idle) {
	track_next_button->setFocus();
      }
      track_insert_button->setEnabled(transport_idle&&CanInsertTrack());
      track_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
      track_close_button->setEnabled(true);
      track_log_list->setEnabled(transport_idle);
    }
  }
  else {             // Straight Segue
    track_track1_button->setDisabled(true);
    track_track1_button->setText(tr("Start"));
    track_track1_button->setPalette(track_start_palette);
    track_record_button->setDisabled(true);
    track_record_button->setText(tr("Record"));
    track_track2_button->setDisabled(true);
    track_finished_button->setEnabled(track_changed);
    track_reset_button->setEnabled(real_logline->hasCustomTransition());
    track_post_button->setDisabled(true);
    track_play_button->setEnabled(true);
    track_stop_button->setEnabled(true);
    track_next_button->setEnabled(transport_idle);
    track_previous_button->setEnabled(transport_idle);
    if(transport_idle) {
      track_next_button->setFocus();
    }
    track_insert_button->setEnabled(transport_idle&&CanInsertTrack());
    track_delete_button->setEnabled(transport_idle&&CanDeleteTrack());
    track_close_button->setEnabled(true);
    track_log_list->setEnabled(transport_idle);
  }
}


void VoiceTracker::UpdateRemaining()
{
  edit_tracks_remaining_label->setText(QString().sprintf("%d",track_tracks));
  if(track_block_valid) {
    edit_time_remaining_label->
      setText(QString().sprintf("%s",(const char *)
				RDGetTimeLength(track_time_remaining,
					       true,true)));
    if(track_time_remaining>=0) {
      edit_time_remaining_label->setPalette(edit_time_remaining_palette[0]);
    }
    else {
      edit_time_remaining_label->setPalette(edit_time_remaining_palette[1]);
    }
  }
  else {
    edit_time_remaining_label->setText("-:--:--.-");
    edit_time_remaining_label->setPalette(edit_time_remaining_palette[0]);
  }
}


bool VoiceTracker::TrackAvailable()
{
  if(!track_group->exists()) {
    return false;
  }
  if((track_group->freeCartQuantity()<=0)&&edit_wave_name[1].isEmpty()) {
    return false;
  }
  return true;
}


void VoiceTracker::LogLine(const QString &line)
{
  fprintf(stderr,"%s: %s\n",
	  (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	  (const char *)line);
}


bool VoiceTracker::InitTrack()
{
  int cutnum;

  track_recording=false;
  track_record_ran=false;
  track_recording_pos=0;
  track_aborting=false;
  if(edit_track_cart!=NULL) {
    delete edit_track_cart;
    edit_track_cart=NULL;
  }
  bool ok=false;
  unsigned next_cart=0;
  RDCart *cart=NULL;
  while(!ok) {
    if((next_cart=track_group->nextFreeCart())==0) {
      return false;
    }
    cart=new RDCart(next_cart);
    ok=cart->create(track_group->name(),RDCart::Audio);
    delete cart;
  }
  edit_track_cart=new RDCart(next_cart);
  edit_track_cart->setOwner(track_log->name());
  edit_track_cart->setTitle(edit_logline[1]->markerComment());
  if(edit_track_cuts[1]!=NULL) {
    delete edit_track_cuts[1];
  }
  if((cutnum=edit_track_cart->addCut(edit_format,edit_bitrate,edit_chans))<0) {
    QMessageBox::warning(this,tr("RDLogEdit - Voice Tracker"),
			 tr("This cart cannot contain any additional cuts!"));
    return false;
  }
  edit_track_cuts[1]=new RDCut(edit_track_cart->number(),cutnum);
  switch(edit_format) {
      case 0:
	edit_coding=RDCae::Pcm16;
	break;
	
      case 1:
	edit_coding=RDCae::MpegL2;
	break;
	
      default:
	edit_coding=RDCae::Pcm16;
	break;
  }
  edit_deck_state=VoiceTracker::DeckTrack1;
  edit_sliding=false;
  for(unsigned i=0;i<3;i++) {
    edit_segue_start_point[i]=-1;
//    edit_segue_gain[i]=-1;
  }
  edit_logline[1]->
    setFadeupPoint(edit_logline[1]->startPoint(),RDLogLine::LogPointer);
  edit_logline[1]->setFadedownGain(RD_FADE_DEPTH);
  edit_logline[1]->
    setFadedownPoint(edit_logline[1]->endPoint(),RDLogLine::LogPointer);
  edit_logline[1]->setFadeupGain(RD_FADE_DEPTH);
  track_tracks--;

  return true;
}


void VoiceTracker::FinishTrack()
{
  edit_logline[0]->
	setSegueStartPoint(edit_segue_start_point[0],RDLogLine::LogPointer);
  edit_logline[0]->setSegueGain(0);
  edit_logline[0]->
    setAverageSegueLength(edit_logline[0]->segueStartPoint()-
			  edit_logline[0]->startPoint());
  if(!edit_wave_name[1].isEmpty()) {
    rdcae->stopRecord(edit_input_card,edit_input_port);
    edit_logline[1]->setEndPoint(-1,RDLogLine::LogPointer);
  }
  stopData();
  edit_deck_state=VoiceTracker::DeckIdle;
}


double VoiceTracker::GetCurrentTime()
{
  struct timeval tv;

  memset(&tv,0,sizeof(tv));
  gettimeofday(&tv,NULL);
  return (double)tv.tv_sec+(double)tv.tv_usec/1000000.0;
}


bool VoiceTracker::IsTrack(int line,bool *offset)
{
  *offset=false;
  if(track_log_event->logLine(line)==NULL) {
    return false;
  }
  if((track_log_event->logLine(line)->type()==RDLogLine::Track)||
     (track_log_event->logLine(line)->source()==RDLogLine::Tracker)) {
    return true;
  }
  if(track_log_event->logLine(line-1)==NULL) {
    return false;
  }
/*  if((track_log_event->logLine(line-1)->type()==RDLogLine::Track)||
     (track_log_event->logLine(line-1)->source()==RDLogLine::Tracker)) {
    *offset=true;
    return true;
  }*/
  return false;
}


bool VoiceTracker::CanInsertTrack()
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(item==NULL) {
    return false;
  }
  if(item->line()==TRACKER_MAX_LINENO) {
    if(track_log_event->size()<=0) {
      return true;
    }
    return track_log_event->logLine(track_log_event->size()-1)->type()
      !=RDLogLine::Track;
  }
  bool state=track_log_event->logLine(item->line())->type()==RDLogLine::Track;
  if(item->line()>0) {
    state=state||
      (track_log_event->logLine(item->line()-1)->type()==RDLogLine::Track);
  }

  return !state;
}


bool VoiceTracker::CanDeleteTrack()
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->selectedItem();
  if(track_offset) {
    item=GetItemByLine(item->line()-1);
  }
  if((item==NULL)||(item->line()==TRACKER_MAX_LINENO)) {
    return false;
  }
  return track_log_event->logLine(item->line())->type()==RDLogLine::Track;
}


void VoiceTracker::ClearCursor(QPainter *p)
{
  if(edit_cursor_pos>=0) {
    for(int i=0;i<3;i++) {
      p->drawPixmap(edit_cursor_pos,12+79*i,*edit_wave_map[i],
		    edit_cursor_pos-12,0,1,
		    edit_wave_map[i]->size().height());
    }
  }
}


RDListViewItem *VoiceTracker::GetItemByLine(int line)
{
  RDListViewItem *item=(RDListViewItem *)track_log_list->firstChild();
  while(item!=NULL) {
    if(item->line()==line) {
      return item;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  return NULL;
}


void VoiceTracker::CheckChanges()
{
  if(track_changed) {
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
  track_changed=false;
}


void VoiceTracker::PushSegues()
{
  for(unsigned i=0;i<3;i++) {
    if(edit_logline[i]!=NULL) {
      *(edit_saved_logline[i])=*(edit_logline[i]);
    }
  }
}


void VoiceTracker::PopSegues()
{
  for(unsigned i=0;i<3;i++) {
    if(edit_logline[i]!=NULL) {
      *(edit_logline[i])=*(edit_saved_logline[i]);
    }
  }
}
