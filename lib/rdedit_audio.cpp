// rdedit_audio.cpp
//
// Edit Rivendell Audio
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdedit_audio.cpp,v 1.26.6.3 2014/01/16 02:44:59 cvs Exp $
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

#include <stdlib.h>
#include <math.h>
#include <qpainter.h>
#include <qsignalmapper.h>
#include <qmessagebox.h>

#include <rdconf.h>
#include <rd.h>
#include <rdmixer.h>
#include <rdcut.h>
#include <rdedit_audio.h>
#include <rdaudioinfo.h>
#include <rdtrimaudio.h>

RDEditAudio::RDEditAudio(RDCart *cart,QString cut_name,RDCae *cae,RDUser *user,
			 RDStation *station,RDConfig *config,int card,
			 int port,int preroll,
			 int trim_level,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_cae=cae;
  edit_station=station;
  edit_user=user;
  edit_config=config;
  edit_card=card;
  edit_port=port;
  edit_stream=-1;
  
  bool editing_allowed=user->editAudio()&&cart->owner().isEmpty();

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Hevetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Hevetica",12,QFont::Normal);
  label_font.setPixelSize(12);
  QFont small_font("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  edit_cursors[RDEditAudio::Play]=-1;
  edit_cursors[RDEditAudio::SegueStart]=-1;
  edit_cursors[RDEditAudio::SegueEnd]=-1;
  edit_cursors[RDEditAudio::TalkStart]=-1;
  edit_cursors[RDEditAudio::TalkEnd]=-1;
  edit_cursors[RDEditAudio::Start]=-1;
  edit_cursors[RDEditAudio::End]=-1;
  edit_cursors[RDEditAudio::FadeUp]=-1;
  edit_cursors[RDEditAudio::FadeDown]=-1;
  edit_cursors[RDEditAudio::HookStart]=-1;
  edit_cursors[RDEditAudio::HookEnd]=-1;
  is_playing=false;
  is_paused=false;
  is_stopped=false;
  is_looping=false;
  edit_cue_point=RDEditAudio::Play;
  edit_play_mode=RDEditAudio::FromStart;
  left_button_pressed=false;
  center_button_pressed=false;
  energy_data=NULL;
  energy_size=0;
  edit_gain_mode=RDEditAudio::GainNone;
  edit_gain_count=0;
  use_looping=false;
  ignore_pause=false;
  delete_marker=false;
  pause_mode=false;
  played_cursor=0;

  //
  // The Cut
  //
  edit_cut=new RDCut(cut_name);
  setCaption(QString().sprintf("Edit Markers - %s\n",
			       (const char *)edit_cut->description()));

  //
  // The Audio
  //
  connect(edit_cae,SIGNAL(playing(int)),this,SLOT(playedData(int)));
  connect(edit_cae,SIGNAL(playStopped(int)),this,SLOT(pausedData(int)));
  connect(edit_cae,SIGNAL(playPositionChanged(int,unsigned)),
	  this,SLOT(positionData(int,unsigned)));
  edit_cae->loadPlay(edit_card,edit_cut->cutName(),&edit_stream,&edit_handle);
  RDSetMixerOutputPort(edit_cae,edit_card,edit_stream,edit_port);
  RDAudioInfo *info=new RDAudioInfo(station,edit_config,this);
  RDAudioInfo::ErrorCode audio_err;
  info->setCartNumber(RDCut::cartNumber(cut_name));
  info->setCutNumber(RDCut::cutNumber(cut_name));

  if((audio_err=info->runInfo(user->name(),user->password()))==
     RDAudioInfo::ErrorOk) {
    edit_sample_rate=info->sampleRate();
    edit_sample_length=info->frames();
    edit_channels=info->channels();
  }
  else {
    fprintf(stderr,"unable to download cut data, error was: \"%s\".\n",
	    (const char *)RDAudioInfo::errorText(audio_err));
    edit_sample_rate=RD_DEFAULT_SAMPLE_RATE;
    edit_sample_length=info->frames();
    edit_channels=2;
  }
  delete info;
  edit_gain=EDITAUDIO_DEFAULT_GAIN;
  edit_preroll=edit_sample_rate*preroll/1000;
  for(unsigned i=0;i<(8*sizeof(unsigned));i++) {
    if((edit_sample_length/(1<<i))<896256) {
      edit_max_factor_x=1<<i;
      i=8*sizeof(unsigned);
    }
  }
  edit_factor_x=edit_max_factor_x;

  //
  //  Save Button
  //
  QPushButton *button=new QPushButton(this,"save_button");
  button->setGeometry(EDITAUDIO_WIDGET_WIDTH-90,EDITAUDIO_WIDGET_HEIGHT-120,
		      80,50);
  button->setDefault(true);
  button->setFont(button_font);
  button->setText(tr("&Save"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(EDITAUDIO_WIDGET_WIDTH-90,
			     EDITAUDIO_WIDGET_HEIGHT-60,80,50);
  cancel_button->setDefault(true);
  cancel_button->setFont(button_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Transport Buttons
  //
  edit_play_cursor_button=new RDTransportButton(RDTransportButton::PlayBetween,
					this,"edit_play_cursor_button");
  edit_play_cursor_button->setGeometry(20,425,65,45);
  edit_play_cursor_button->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_play_cursor_button->setEnabled((edit_card>=0)&&(edit_port>=0));
  connect(edit_play_cursor_button,SIGNAL(clicked()),
	  this,SLOT(playCursorData()));

  edit_play_start_button=new RDTransportButton(RDTransportButton::Play,
					this,"edit_play_start_button");
  edit_play_start_button->setGeometry(90,425,65,45);
  edit_play_start_button->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_play_start_button->setEnabled((edit_card>=0)&&(edit_port>=0));
  connect(edit_play_start_button,SIGNAL(clicked()),
	  this,SLOT(playStartData()));

  edit_pause_button=new RDTransportButton(RDTransportButton::Pause,
					 this,"edit_pause_button");
  edit_pause_button->setGeometry(160,425,65,45);
  edit_pause_button->setOnColor(QColor(red));
  edit_pause_button->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_pause_button->setEnabled((edit_card>=0)&&(edit_port>=0));
  connect(edit_pause_button,SIGNAL(clicked()),this,SLOT(pauseData()));

  edit_stop_button=new RDTransportButton(RDTransportButton::Stop,
					 this,"edit_stop_button");
  edit_stop_button->setGeometry(230,425,65,45);
  edit_stop_button->on();
  edit_stop_button->setOnColor(QColor(red));
  edit_stop_button->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_stop_button->setEnabled((edit_card>=0)&&(edit_port>=0));
  connect(edit_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  edit_loop_button=new RDTransportButton(RDTransportButton::Loop,
					this,"edit_loop_button");
  edit_loop_button->setGeometry(300,425,65,45);
  edit_loop_button->off();
  edit_loop_button->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_loop_button->setEnabled((edit_card>=0)&&(edit_port>=0));
  connect(edit_loop_button,SIGNAL(clicked()),this,SLOT(loopData()));

  //
  // Time Origin Scroll Bar
  //
  edit_hscroll=new QScrollBar(0,0,EDITAUDIO_PAN_SIZE/10,
			      EDITAUDIO_PAN_SIZE,0,Qt::Horizontal,
			      this,"edit_hscroll");
  edit_hscroll->setGeometry(10,10+EDITAUDIO_WAVEFORM_HEIGHT,
			    EDITAUDIO_WAVEFORM_WIDTH,20);
  connect(edit_hscroll,SIGNAL(valueChanged(int)),this,SLOT(hscrollData(int)));

  //
  // Amplitude Buttons
  //
  QLabel *amp_label=new QLabel(this,"amp_label");
  amp_label->setGeometry(742,5,80,16);
  amp_label->setAlignment(AlignHCenter|AlignVCenter);
  amp_label->setFont(button_font);
  amp_label->setText(tr("Amplitude"));

  RDTransportButton *y_up_button=new RDTransportButton(RDTransportButton::Up,
						     this,"y_up_button");
  y_up_button->setGeometry(747,22,70,50);
  y_up_button->setFont(QFont("Helvetica",12,QFont::Bold));
  y_up_button->setText(tr("Zoom\nIn"));
  connect(y_up_button,SIGNAL(clicked()),this,SLOT(yUp()));

  RDTransportButton *y_down_button=new RDTransportButton(RDTransportButton::Down,
						       this,"y_down_button");
  y_down_button->setGeometry(747,72,70,50);
  y_down_button->setFont(QFont("Helvetica",12,QFont::Bold));
  y_down_button->setText(tr("Zoom\nOut"));
  connect(y_down_button,SIGNAL(clicked()),this,SLOT(yDown()));

  //
  // Time Buttons
  //
  QLabel *time_label=new QLabel(this,"time_label");
  time_label->setGeometry(760,143,40,16);
  time_label->setAlignment(AlignHCenter|AlignVCenter);
  time_label->setFont(button_font);
  time_label->setText(tr("Time"));

  QPushButton *x_full_in_button=new QPushButton(this,"x_full_in_button");
  x_full_in_button->setGeometry(747,160,70,50);
  x_full_in_button->setFont(button_font);
  x_full_in_button->setText(tr("Full\nIn"));
  connect(x_full_in_button,SIGNAL(clicked()),this,SLOT(xFullIn()));

  RDTransportButton *x_up_button=new RDTransportButton(RDTransportButton::Up,
						     this,"x_up_button");
  x_up_button->setGeometry(747,212,70,50);
  x_up_button->setFont(button_font);
  x_up_button->setText(tr("Zoom\nIn"));
  connect(x_up_button,SIGNAL(clicked()),this,SLOT(xUp()));

  RDTransportButton *x_down_button=new RDTransportButton(RDTransportButton::Down,
						       this,"x_down_button");
  x_down_button->setGeometry(747,262,70,50);
  x_down_button->setFont(button_font);
  x_down_button->setText(tr("Zoom\nOut"));
  connect(x_down_button,SIGNAL(clicked()),this,SLOT(xDown()));

  QPushButton *x_full_button=new QPushButton(this,"x_full_button");
  x_full_button->setGeometry(747,312,70,50);
  x_full_button->setFont(button_font);
  x_full_button->setText(tr("Full\nOut"));
  connect(x_full_button,SIGNAL(clicked()),this,SLOT(xFullOut()));

  //
  // GoTo Buttons
  //
  QLabel *goto_label=new QLabel(this,"goto_label");
  goto_label->setGeometry(760,378,40,16);
  goto_label->setAlignment(AlignHCenter|AlignVCenter);
  goto_label->setFont(button_font);
  goto_label->setText(tr("Goto"));

  QPushButton *goto_cursor_button=new QPushButton(this,"goto_cursor_button");
  goto_cursor_button->setGeometry(747,393,70,50);
  goto_cursor_button->setFont(button_font);
  goto_cursor_button->setText(tr("Cursor"));
  connect(goto_cursor_button,SIGNAL(clicked()),this,SLOT(gotoCursorData()));

  QPushButton *goto_home_button=new QPushButton(this,"goto_home_button");
  goto_home_button->setGeometry(747,443,70,50);
  goto_home_button->setFont(button_font);
  goto_home_button->setText(tr("Home"));
  connect(goto_home_button,SIGNAL(clicked()),this,SLOT(gotoHomeData()));

  QPushButton *goto_end_button=new QPushButton(this,"goto_end_button");
  goto_end_button->setGeometry(747,493,70,50);
  goto_end_button->setFont(button_font);
  goto_end_button->setText(tr("End"));
  connect(goto_end_button,SIGNAL(clicked()),this,SLOT(gotoEndData()));

  //
  // Cursor Readouts
  //
  QSignalMapper *button_mapper=new QSignalMapper(this,"button_mapper");
  connect(button_mapper,SIGNAL(mapped(int)),this,SLOT(cuePointData(int)));
  QSignalMapper *edit_mapper=new QSignalMapper(this,"edit_mapper");
  connect(edit_mapper,SIGNAL(mapped(int)),this,SLOT(cueEditData(int)));
  QSignalMapper *esc_mapper=new QSignalMapper(this,"esc_mapper");
  connect(esc_mapper,SIGNAL(mapped(int)),this,SLOT(cueEscData(int)));

  edit_cursor_edit[RDEditAudio::Start]=new RDMarkerEdit(this,"edit_start_edit");
  edit_cursor_edit[RDEditAudio::Start]->setGeometry(88,496,70,21);
  edit_cursor_edit[RDEditAudio::Start]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::Start]->setDragEnabled(false);
  edit_cursor_edit[RDEditAudio::Start]->
    setFont(label_font);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::Start],(int)RDEditAudio::Start);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::Start],(int)RDEditAudio::Start);
  connect(edit_cursor_edit[RDEditAudio::Start],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::Start],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));

  edit_cue_button[RDEditAudio::Start]=new RDMarkerButton(this,"StartButton");
  edit_cue_button[RDEditAudio::Start]->setToggleButton(true);
  edit_cue_button[RDEditAudio::Start]->setGeometry(20,485,66,45);
  edit_cue_button[RDEditAudio::Start]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::Start]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::Start]->
    setPalette(QPalette(QColor(RD_START_END_MARKER_COLOR),backgroundColor()));
  edit_cue_button[RDEditAudio::Start]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::Start]->setText(tr("Cut\nStart"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::Start],(int)RDEditAudio::Start);
  connect(edit_cue_button[RDEditAudio::Start],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::End]=new RDMarkerEdit(this,"edit_end_edit");
  edit_cursor_edit[RDEditAudio::End]->setGeometry(88,541,70,21);
  edit_cursor_edit[RDEditAudio::End]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::End]->setDragEnabled(false);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::End],(int)RDEditAudio::End);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::End],(int)RDEditAudio::End);
  connect(edit_cursor_edit[RDEditAudio::End],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::End],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::End]->
    setFont(label_font);
  edit_cue_button[RDEditAudio::End]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::End]->setToggleButton(true);
  edit_cue_button[RDEditAudio::End]->setGeometry(20,530,66,45);
  edit_cue_button[RDEditAudio::End]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::End]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::End]->
    setPalette(QPalette(QColor(RD_START_END_MARKER_COLOR),
			      backgroundColor()));
  edit_cue_button[RDEditAudio::End]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::End]->setText(tr("Cut\nEnd"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::End],(int)RDEditAudio::End);
  connect(edit_cue_button[RDEditAudio::End],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::TalkStart]=
    new RDMarkerEdit(this,"edit_talk_start_edit");
  edit_cursor_edit[RDEditAudio::TalkStart]->setGeometry(243,596,70,21);
  edit_cursor_edit[RDEditAudio::TalkStart]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::TalkStart]->setDragEnabled(false);
  edit_cursor_edit[RDEditAudio::TalkStart]->
    setFont(label_font);
  edit_mapper->setMapping(edit_cursor_edit[RDEditAudio::TalkStart],
			  (int)RDEditAudio::TalkStart);
  esc_mapper->setMapping(edit_cursor_edit[RDEditAudio::TalkStart],
			  (int)RDEditAudio::TalkStart);
  connect(edit_cursor_edit[RDEditAudio::TalkStart],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::TalkStart],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cue_button[RDEditAudio::TalkStart]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::TalkStart]->setToggleButton(true);
  edit_cue_button[RDEditAudio::TalkStart]->setGeometry(175,585,66,45);
  edit_cue_button[RDEditAudio::TalkStart]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::TalkStart]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::TalkStart]->
    setPalette(QPalette(QColor(RD_TALK_MARKER_COLOR),
			      backgroundColor()));
  edit_cue_button[RDEditAudio::TalkStart]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::TalkStart]->setText(tr("Talk\nStart"));
  button_mapper->
   setMapping(edit_cue_button[RDEditAudio::TalkStart],(int)RDEditAudio::TalkStart);
  connect(edit_cue_button[RDEditAudio::TalkStart],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::TalkEnd]=
    new RDMarkerEdit(this,"edit_talk_end_edit");
  edit_cursor_edit[RDEditAudio::TalkEnd]->setGeometry(243,641,70,21);
  edit_cursor_edit[RDEditAudio::TalkEnd]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::TalkEnd]->setDragEnabled(false);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::TalkEnd],(int)RDEditAudio::TalkEnd);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::TalkEnd],(int)RDEditAudio::TalkEnd);
  connect(edit_cursor_edit[RDEditAudio::TalkEnd],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::TalkEnd],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::TalkEnd]->
    setFont(label_font);
  edit_cue_button[RDEditAudio::TalkEnd]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::TalkEnd]->setToggleButton(true);
  edit_cue_button[RDEditAudio::TalkEnd]->setGeometry(175,630,66,45);
  edit_cue_button[RDEditAudio::TalkEnd]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::TalkEnd]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::TalkEnd]->
    setPalette(QPalette(QColor(RD_TALK_MARKER_COLOR),
			      backgroundColor()));
  edit_cue_button[RDEditAudio::TalkEnd]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::TalkEnd]->setText("Talk\nEnd");
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::TalkEnd],(int)RDEditAudio::TalkEnd);
  connect(edit_cue_button[RDEditAudio::TalkEnd],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::SegueStart]=
    new RDMarkerEdit(this,"edit_segue_start_edit");
  edit_cursor_edit[RDEditAudio::SegueStart]->setGeometry(398,596,70,21);
  edit_cursor_edit[RDEditAudio::SegueStart]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::SegueStart]->setDragEnabled(false);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::SegueStart],
	       (int)RDEditAudio::SegueStart);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::SegueStart],
	       (int)RDEditAudio::SegueStart);
  connect(edit_cursor_edit[RDEditAudio::SegueStart],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::SegueStart],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::SegueStart]->
    setFont(label_font);
  edit_cue_button[RDEditAudio::SegueStart]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::SegueStart]->setToggleButton(true);
  edit_cue_button[RDEditAudio::SegueStart]->setGeometry(330,585,66,45);
  edit_cue_button[RDEditAudio::SegueStart]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::SegueStart]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::SegueStart]->
    setPalette(QPalette(QColor(RD_SEGUE_MARKER_COLOR),
			      backgroundColor()));
  edit_cue_button[RDEditAudio::SegueStart]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::SegueStart]->setText(tr("Segue\nStart"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::SegueStart],
	       (int)RDEditAudio::SegueStart);
  connect(edit_cue_button[RDEditAudio::SegueStart],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::SegueEnd]=
    new RDMarkerEdit(this,"edit_segue_end_edit");
  edit_cursor_edit[RDEditAudio::SegueEnd]->setGeometry(398,641,70,21);
  edit_cursor_edit[RDEditAudio::SegueEnd]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::SegueEnd]->setDragEnabled(false);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::SegueEnd],(int)RDEditAudio::SegueEnd);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::SegueEnd],(int)RDEditAudio::SegueEnd);
  connect(edit_cursor_edit[RDEditAudio::SegueEnd],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::SegueEnd],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::SegueEnd]->
    setFont(label_font);
  edit_cue_button[RDEditAudio::SegueEnd]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::SegueEnd]->setToggleButton(true);
  edit_cue_button[RDEditAudio::SegueEnd]->setGeometry(330,630,66,45);
  edit_cue_button[RDEditAudio::SegueEnd]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::SegueEnd]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::SegueEnd]->
    setPalette(QPalette(QColor(RD_SEGUE_MARKER_COLOR),backgroundColor()));
  edit_cue_button[RDEditAudio::SegueEnd]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::SegueEnd]->setText(tr("Segue\nEnd"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::SegueEnd],(int)RDEditAudio::SegueEnd);
  connect(edit_cue_button[RDEditAudio::SegueEnd],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::FadeUp]=new RDMarkerEdit(this,"edit_fadeup_edit");
  edit_cursor_edit[RDEditAudio::FadeUp]->setGeometry(88,596,70,21);
  edit_cursor_edit[RDEditAudio::FadeUp]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::FadeUp]->setDragEnabled(false);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::FadeUp],(int)RDEditAudio::FadeUp);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::FadeUp],(int)RDEditAudio::FadeUp);
  connect(edit_cursor_edit[RDEditAudio::FadeUp],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::FadeUp],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::FadeUp]->
    setFont(label_font);
  edit_cue_button[RDEditAudio::FadeUp]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::FadeUp]->setToggleButton(true);
  edit_cue_button[RDEditAudio::FadeUp]->setGeometry(20,585,66,45);
  edit_cue_button[RDEditAudio::FadeUp]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::FadeUp]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::FadeUp]->
    setPalette(QPalette(QColor(RD_FADE_MARKER_COLOR),backgroundColor()));
  edit_cue_button[RDEditAudio::FadeUp]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::FadeUp]->setText(tr("Fade\nUp"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::FadeUp],(int)RDEditAudio::FadeUp);
  connect(edit_cue_button[RDEditAudio::FadeUp],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::FadeDown]=
    new RDMarkerEdit(this,"edit_fadedown_edit");
  edit_cursor_edit[RDEditAudio::FadeDown]->setGeometry(88,641,70,21);
  edit_cursor_edit[RDEditAudio::FadeDown]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::FadeDown]->setDragEnabled(false);
  edit_mapper->setMapping(edit_cursor_edit[RDEditAudio::FadeDown],
			  (int)RDEditAudio::FadeDown);
  esc_mapper->setMapping(edit_cursor_edit[RDEditAudio::FadeDown],
			  (int)RDEditAudio::FadeDown);
  connect(edit_cursor_edit[RDEditAudio::FadeDown],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::FadeDown],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::FadeDown]->
    setFont(label_font);
  edit_cue_button[RDEditAudio::FadeDown]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::FadeDown]->setToggleButton(true);
  edit_cue_button[RDEditAudio::FadeDown]->setGeometry(20,630,66,45);
  edit_cue_button[RDEditAudio::FadeDown]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::FadeDown]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::FadeDown]->
    setPalette(QPalette(QColor(RD_FADE_MARKER_COLOR),backgroundColor()));
  edit_cue_button[RDEditAudio::FadeDown]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::FadeDown]->setText("Fade\nDown");
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::FadeDown],(int)RDEditAudio::FadeDown);
  connect(edit_cue_button[RDEditAudio::FadeDown],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::HookStart]=new RDMarkerEdit(this,"edit_hook_start_edit");
  edit_cursor_edit[RDEditAudio::HookStart]->setGeometry(553,596,70,21);
  edit_cursor_edit[RDEditAudio::HookStart]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::HookStart]->setDragEnabled(false);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::HookStart],
	       (int)RDEditAudio::HookStart);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::HookStart],
	       (int)RDEditAudio::HookStart);
  connect(edit_cursor_edit[RDEditAudio::HookStart],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::HookStart],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cursor_edit[RDEditAudio::HookStart]->
    setFont(label_font);
  edit_cursor_edit[RDEditAudio::HookStart]->setReadOnly(true);
  edit_cue_button[RDEditAudio::HookStart]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::HookStart]->setToggleButton(true);
  edit_cue_button[RDEditAudio::HookStart]->setGeometry(485,585,66,45);
  edit_cue_button[RDEditAudio::HookStart]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::HookStart]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::HookStart]->
    setPalette(QPalette(QColor(RD_HOOK_MARKER_COLOR),
			      backgroundColor()));
  edit_cue_button[RDEditAudio::HookStart]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::HookStart]->setText(tr("Hook\nStart"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::HookStart],
	       (int)RDEditAudio::HookStart);
  connect(edit_cue_button[RDEditAudio::HookStart],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  edit_cursor_edit[RDEditAudio::HookEnd]=
    new RDMarkerEdit(this,"edit_hook_end_edit");
  edit_cursor_edit[RDEditAudio::HookEnd]->setGeometry(553,641,70,21);
  edit_cursor_edit[RDEditAudio::HookEnd]->setReadOnly(true);
  edit_cursor_edit[RDEditAudio::HookEnd]->setDragEnabled(false);
  edit_cursor_edit[RDEditAudio::HookEnd]->
    setFont(label_font);
  edit_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::HookEnd],(int)RDEditAudio::HookEnd);
  esc_mapper->
    setMapping(edit_cursor_edit[RDEditAudio::HookEnd],(int)RDEditAudio::HookEnd);
  connect(edit_cursor_edit[RDEditAudio::HookEnd],SIGNAL(returnPressed()),
	  edit_mapper,SLOT(map()));
  connect(edit_cursor_edit[RDEditAudio::HookEnd],SIGNAL(escapePressed()),
	  esc_mapper,SLOT(map()));
  edit_cue_button[RDEditAudio::HookEnd]=new RDMarkerButton(this,"button");
  edit_cue_button[RDEditAudio::HookEnd]->setToggleButton(true);
  edit_cue_button[RDEditAudio::HookEnd]->setGeometry(485,630,66,45);
  edit_cue_button[RDEditAudio::HookEnd]->setFlashColor(backgroundColor());
  edit_cue_button[RDEditAudio::HookEnd]->
    setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_cue_button[RDEditAudio::HookEnd]->
    setPalette(QPalette(QColor(RD_HOOK_MARKER_COLOR),
			      backgroundColor()));
  edit_cue_button[RDEditAudio::HookEnd]->
    setFont(button_font);
  edit_cue_button[RDEditAudio::HookEnd]->setText(tr("Hook\nEnd"));
  button_mapper->
    setMapping(edit_cue_button[RDEditAudio::HookEnd],(int)RDEditAudio::HookEnd);
  connect(edit_cue_button[RDEditAudio::HookEnd],SIGNAL(clicked()),
	  button_mapper,SLOT(map()));

  //
  // AutoTrim Buttons
  //
  edit_trim_box=new QSpinBox(this,"edit_head_trim_box");
  edit_trim_box->setGeometry(243,529,70,21);
  edit_trim_box->setAcceptDrops(false);
  edit_trim_box->setValidator(0);
  edit_trim_box->setSuffix(tr(" dB"));
  edit_trim_box->setRange(-99,0);
  edit_trim_box->
    setValue((trim_level+REFERENCE_LEVEL)/100);
  QLabel *label=new QLabel(tr("Threshold"),this,"label");
  label->setGeometry(238,513,70,15);
  label->setAlignment(AlignHCenter);
  label->setFont(QFont(small_font));
  QPushButton *trim_start_button=new QPushButton(this,"trim_start_button");
  trim_start_button->setGeometry(175,485,66,45);
  trim_start_button->
    setPalette(QPalette(QColor(RD_START_END_MARKER_COLOR),backgroundColor()));
  trim_start_button->setFont(button_font);
  trim_start_button->setText(tr("Trim\nStart"));
  connect(trim_start_button,SIGNAL(clicked()),this,SLOT(trimHeadData()));

  QPushButton *trim_end_button=new QPushButton(this,"trim_end_button");
  trim_end_button->setGeometry(175,530,66,45);
  trim_end_button->
    setPalette(QPalette(QColor(RD_START_END_MARKER_COLOR),backgroundColor()));
  trim_end_button->setFont(button_font);
  trim_end_button->setText(tr("Trim\nEnd"));
  connect(trim_end_button,SIGNAL(clicked()),this,SLOT(trimTailData()));

  //
  // Cut Gain Control
  //
  edit_gain_control=new QRangeControl();
  edit_gain_control->setRange(-1000,1000);
  edit_gain_control->setSteps(10,10);
  edit_gain_edit=new RDMarkerEdit(this,"edit_gain_edit");
  edit_gain_edit->setGeometry(398,529,70,21);
  edit_gain_edit->setAcceptDrops(false);
  connect(edit_gain_edit,SIGNAL(returnPressed()),this,SLOT(gainChangedData()));
  label=new QLabel(tr("Cut Gain"),this,"label");
  label->setGeometry(388,513,70,15);
  label->setAlignment(AlignHCenter);
  label->setFont(QFont(small_font));
  RDTransportButton *gain_up_button=new 
    RDTransportButton(RDTransportButton::Up,this,"button");
  gain_up_button->setGeometry(330,485,66,45);
  gain_up_button->off();
  connect(gain_up_button,SIGNAL(pressed()),this,SLOT(gainUpPressedData()));
  connect(gain_up_button,SIGNAL(released()),this,SLOT(gainReleasedData()));
  
  RDTransportButton *gain_down_button=
    new RDTransportButton(RDTransportButton::Down,this,"button");
  gain_down_button->setGeometry(330,530,66,45);
  gain_down_button->off();
  connect(gain_down_button,SIGNAL(pressed()),this,SLOT(gainDownPressedData()));
  connect(gain_down_button,SIGNAL(released()),this,SLOT(gainReleasedData()));
  edit_gain_timer=new QTimer(this,"edit_gain_timer");
  connect(edit_gain_timer,SIGNAL(timeout()),this,SLOT(gainTimerData()));

  //
  // Marker Remove Button
  //
  edit_remove_button=new RDPushButton(this,"edit_remove_button");
  edit_remove_button->setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_remove_button->setGeometry(485,510,66,45);
  edit_remove_button->setFont(button_font);
  edit_remove_button->setText(tr("Remove\nMarker"));
  edit_remove_button->setToggleButton(true);
  edit_remove_button->setFlashColor(QColor(EDITAUDIO_REMOVE_FLASH_COLOR));
  connect(edit_remove_button,SIGNAL(clicked()),this,SLOT(removeButtonData()));

  //
  // Segue Fade Box
  //
  edit_overlap_box=new QCheckBox(this,"edit_overlap_box");
  edit_overlap_box->setGeometry(570,510,15,15);
  label=new QLabel(edit_overlap_box,tr("No Fade on Segue Out"),
		   this,"label");
  label->setGeometry(590,508,130,20);
  label->setFont(button_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  
  //
  // Time Counters
  //
  label=new QLabel(tr("Position"),this,"label");
  label->setGeometry(60,385,70,20);
  label->setFont(QFont(small_font));
  label->setAlignment(Qt::AlignHCenter);
  label->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_overall_edit=new QLineEdit(this,"edit_overall_edit");
  edit_overall_edit->setAcceptDrops(false);
  edit_overall_edit->setGeometry(60,400,70,21);
  edit_overall_edit->setFont(label_font);
  edit_overall_edit->setReadOnly(true);

  edit_region_edit_label=new QLabel("Region",this,"label");
  edit_region_edit_label->setGeometry(158,385,70,20);
  edit_region_edit_label->setFont(QFont(small_font));
  edit_region_edit_label->setAlignment(Qt::AlignHCenter);
  edit_region_edit_label->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_region_edit=new QLineEdit(this,"edit_region_edit");
  edit_region_edit->setAcceptDrops(false);
  edit_region_edit->setGeometry(158,400,70,21);
  edit_region_edit->setFont(label_font);
  edit_region_edit->setReadOnly(true);

  label=new QLabel(tr("Length"),this,"label");
  label->setGeometry(256,385,70,20);
  label->setFont(QFont(small_font));
  label->setAlignment(Qt::AlignHCenter);
  label->
    setPalette(QPalette(backgroundColor(),QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
  edit_size_edit=new QLineEdit(this,"edit_size_edit");
  edit_size_edit->setAcceptDrops(false);
  edit_size_edit->setGeometry(256,400,70,21);
  edit_size_edit->setFont(label_font);
  edit_size_edit->setReadOnly(true);

  //
  // The Audio Meter
  //
  edit_meter=new RDStereoMeter(this,"edit_meter");
  edit_meter->setGeometry(380,398,edit_meter->geometry().width(),
			  edit_meter->geometry().height());
  edit_meter->setSegmentSize(5);
  edit_meter->setMode(RDSegMeter::Peak);
  edit_meter_timer=new QTimer(this,"meter_timer");
  connect(edit_meter_timer,SIGNAL(timeout()),this,SLOT(meterData()));

  //
  // The Wave Forms
  //
  edit_peaks=new RDPeaksExport(station,config,this);
  RDPeaksExport::ErrorCode conv_err;
  edit_peaks->setCartNumber(RDCut::cartNumber(cut_name));
  edit_peaks->setCutNumber(RDCut::cutNumber(cut_name));
  if((conv_err=edit_peaks->runExport(user->name(),user->password()))!=
     RDPeaksExport::ErrorOk) {
    QMessageBox::warning(this,tr("Rivendell Web Service"),
			 tr("Unable to download peak data, error was:\n\"")+
			 RDPeaksExport::errorText(conv_err)+"\".");
  }
  edit_wave_array=new QPointArray(EDITAUDIO_WAVEFORM_WIDTH-2);
  DrawMaps();

  //
  // The Edit Menu
  //
  edit_menu=new QPopupMenu(this,"edit_menu");
  connect(edit_menu,SIGNAL(aboutToShow()),this,SLOT(updateMenuData()));
  edit_menu->insertItem(tr("Delete Talk Markers"),this,
			SLOT(deleteTalkData()),0,RDEditAudio::TalkStart);
  edit_menu->insertItem(tr("Delete Segue Markers"),this,
			SLOT(deleteSegueData()),0,RDEditAudio::SegueStart);
  edit_menu->insertItem(tr("Delete Hook Markers"),this,
			SLOT(deleteHookData()),0,RDEditAudio::HookStart);
  edit_menu->insertItem(tr("Delete Fade Up Marker"),this,
			SLOT(deleteFadeupData()),0,RDEditAudio::FadeUp);
  edit_menu->insertItem(tr("Delete Fade Down Marker"),this,
			SLOT(deleteFadedownData()),0,RDEditAudio::FadeDown);

  //
  // Populate Counter Fields
  //
  edit_cursor_edit[RDEditAudio::Start]->
	  setText(RDGetTimeLength(edit_cut->startPoint(true),true));
  edit_cursors[RDEditAudio::Start]=(int)(((double)edit_cut->startPoint(true)*
					(double)edit_sample_rate)
				       /1152000.0);
  edit_cursors[RDEditAudio::Play]=edit_cursors[RDEditAudio::Start];
  baseline=edit_cursors[RDEditAudio::Start];
  edit_cursor_edit[RDEditAudio::End]->
	  setText(RDGetTimeLength(edit_cut->endPoint(true),true));
  edit_cursors[RDEditAudio::End]=(int)(((double)edit_cut->endPoint(true)*
				      (double)edit_sample_rate)
				     /1152000.0);
  if(edit_cut->talkStartPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::TalkStart]->
      setText(RDGetTimeLength(edit_cut->talkStartPoint(),true));
    edit_cursors[RDEditAudio::TalkStart]=
      (int)(((double)edit_cut->talkStartPoint()*
	     (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::TalkStart]=-1;
  }
  if(edit_cut->talkEndPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::TalkEnd]->
      setText(RDGetTimeLength(edit_cut->talkEndPoint(),true));
    edit_cursors[RDEditAudio::TalkEnd]=(int)(((double)edit_cut->talkEndPoint()*
			   (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::TalkEnd]=-1;
  }
  if(edit_cut->segueStartPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::SegueStart]->
      setText(RDGetTimeLength(edit_cut->segueStartPoint(),true));
    edit_cursors[RDEditAudio::SegueStart]=
      (int)(((double)edit_cut->segueStartPoint()*
	     (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::SegueStart]=-1;
  }
  if(edit_cut->segueEndPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::SegueEnd]->
      setText(RDGetTimeLength(edit_cut->segueEndPoint(),true));
    edit_cursors[RDEditAudio::SegueEnd]=(int)(((double)edit_cut->segueEndPoint()*
			     (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::SegueEnd]=-1;
  }
  if(edit_cut->fadeupPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::FadeUp]->setText(RDGetTimeLength(edit_cut->fadeupPoint(),true));
    edit_cursors[RDEditAudio::FadeUp]=(int)(((double)edit_cut->fadeupPoint()*
			 (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::FadeUp]=-1;
  }
  if(edit_cut->fadedownPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::FadeDown]->
      setText(RDGetTimeLength(edit_cut->fadedownPoint(),true));
    edit_cursors[RDEditAudio::FadeDown]=(int)(((double)edit_cut->fadedownPoint()*
			   (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::FadeDown]=-1;
  }
  if(edit_cut->hookStartPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::HookStart]->
      setText(RDGetTimeLength(edit_cut->hookStartPoint(),true));
    edit_cursors[RDEditAudio::HookStart]=(int)(((double)edit_cut->hookStartPoint()*
			     (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::HookStart]=-1;
  }
  if(edit_cut->hookEndPoint()!=-1) {
    edit_cursor_edit[RDEditAudio::HookEnd]->
      setText(RDGetTimeLength(edit_cut->hookEndPoint(),true));
    edit_cursors[RDEditAudio::HookEnd]=(int)(((double)edit_cut->hookEndPoint()*
			   (double)edit_sample_rate)/1152000.0);
  }
  else {
    edit_cursors[RDEditAudio::HookEnd]=-1;
  }
  edit_gain_control->setValue(edit_cut->playGain());
  edit_gain_edit->setText(QString().sprintf("%4.1f dB",
			     (double)edit_gain_control->value()/100.0));
  edit_trim_box->setValue(trim_level/100);
  if(edit_cut->segueGain()==0) {
    edit_overlap_box->setChecked(true);
  }
  else {
    edit_overlap_box->setChecked(false);
  }
    
  //
  // The Mouse Pointers
  //
  DrawPointers();
  setCursor(*edit_arrow_cursor);
  setMouseTracking(true);
  setFocusPolicy(StrongFocus);

  UpdateCursors();
  UpdateCounters();
  gainChangedData();
  //
  // Set Control Perms
  //
  for(int i=1;i<RDEditAudio::LastMarker;i++) {
    edit_cursor_edit[i]->setReadOnly(!editing_allowed);
    edit_cue_button[i]->setEnabled(editing_allowed);
  }
  edit_remove_button->setEnabled(editing_allowed);
  gain_up_button->setEnabled(editing_allowed);
  gain_down_button->setEnabled(editing_allowed);
  edit_gain_edit->setReadOnly(!editing_allowed);
  trim_start_button->setEnabled(editing_allowed);
  trim_end_button->setEnabled(editing_allowed);
  edit_overlap_box->setEnabled(editing_allowed);
}


RDEditAudio::~RDEditAudio()
{
  delete edit_peaks;
}


QSize RDEditAudio::sizeHint() const
{
  return QSize(EDITAUDIO_WIDGET_WIDTH,EDITAUDIO_WIDGET_HEIGHT);
} 


QSizePolicy RDEditAudio::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDEditAudio::saveData()
{
  edit_cae->stopPlay(edit_handle);
  edit_cae->unloadPlay(edit_handle);
  if(!SaveMarkers()) {
    return;
  }
  done(0);
}


void RDEditAudio::cancelData()
{
  edit_cae->stopPlay(edit_handle);
  edit_cae->unloadPlay(edit_handle);
  done(1);
}


void RDEditAudio::xFullIn()
{
  if(edit_factor_x!=0.125) {
    edit_factor_x=0.125;
    edit_hscroll->setRange(0,(int)((double)edit_peaks->energySize()/
				   (double)edit_channels*
				   (1.0-edit_factor_x/edit_max_factor_x)));
    CenterDisplay();
    DrawMaps();
    repaint(false);
  }
}


void RDEditAudio::xUp()
{
  if(edit_factor_x>(1.0/8.0)) {
    edit_factor_x/=2.0;
    if(edit_factor_x==edit_max_factor_x) {
      edit_hscroll->setRange(0,0);
    }
    else {
      edit_hscroll->setRange(0,(int)((double)edit_peaks->energySize()/
				     (double)edit_channels*
				     (1.0-edit_factor_x/edit_max_factor_x)));
    }
    CenterDisplay();
    DrawMaps();
    repaint(false);
  }
}


void RDEditAudio::xDown()
{
  if(edit_factor_x<edit_max_factor_x) {
    edit_factor_x*=2;
    if(edit_factor_x==edit_max_factor_x) {
      edit_hscroll->setRange(0,0);
    }
    else {
      edit_hscroll->setRange(0,(int)((double)edit_peaks->energySize()/
				     (double)edit_channels*
				     (1.0-edit_factor_x/edit_max_factor_x)));
    }
    CenterDisplay();
    DrawMaps();
    repaint(false);
  }
}


void RDEditAudio::xFullOut()
{
  if(edit_factor_x!=edit_max_factor_x) {
    edit_factor_x=edit_max_factor_x;
    edit_hscroll->setRange(0,0);
    CenterDisplay();
    DrawMaps();
    repaint(false);
  }
}


void RDEditAudio::yUp()
{
  if(edit_gain>-21) {
    edit_gain-=3;
    DrawMaps();
    repaint(false);
  }
}


void RDEditAudio::yDown()
{
  if(edit_gain<0) {
    edit_gain+=3;
    DrawMaps();
    repaint(false);
  }

}


void RDEditAudio::gotoCursorData()
{
  edit_hscroll->setValue(edit_cursors[RDEditAudio::Play]-
    (int)(edit_factor_x*(double)EDITAUDIO_WAVEFORM_WIDTH/2.0));
}


void RDEditAudio::gotoHomeData()
{
  edit_hscroll->setValue(edit_hscroll->minValue());
}


void RDEditAudio::gotoEndData()
{
  edit_hscroll->setValue(edit_hscroll->maxValue());
}


void RDEditAudio::hscrollData(int value)
{
  DrawMaps();
  repaint(false);
}


void RDEditAudio::playStartData()
{
  if(is_playing) {
    return;
  }
  edit_cae->
    positionPlay(edit_handle,GetTime(edit_cursors[RDEditAudio::Start]*1152));
  switch(edit_cue_point) {
      case RDEditAudio::End:
      case RDEditAudio::SegueEnd:
      case RDEditAudio::TalkEnd:
      case RDEditAudio::HookEnd:
	played_cursor=edit_cursors[edit_cue_point]-edit_preroll/1152;
	break;

      default:
	played_cursor=edit_cursors[edit_cue_point];
	break;
  }
  if(!is_playing) {
    edit_cae->setPlayPortActive(edit_card,edit_port,edit_stream);
    edit_cae->
      setOutputVolume(edit_card,edit_stream,edit_port,0+edit_gain_control->value());
    edit_cae->play(edit_handle,(int)(1000.0*(double)
				     ((edit_cursors[RDEditAudio::End]-
			    edit_cursors[RDEditAudio::Start])*1152)/
				     (double)edit_sample_rate),
		   RD_TIMESCALE_DIVISOR,0);
  }
  if(use_looping) {
    is_looping=true;
  }
  edit_play_mode=RDEditAudio::FromStart;
  edit_play_start_button->on();
  edit_play_cursor_button->off();
  edit_pause_button->off();
}


void RDEditAudio::playCursorData()
{
  int length=0;

  if(is_playing) {
    return;
  }
  switch(edit_cue_point) {
      case RDEditAudio::Play:
      case RDEditAudio::Start:
      case RDEditAudio::End:
      case RDEditAudio::FadeDown:
	length=(int)(1000.0*(double)((edit_cursors[RDEditAudio::End]-
			   edit_cursors[RDEditAudio::Play])*1152)/
			   (double)edit_sample_rate);
	break;

      case RDEditAudio::SegueStart:
      case RDEditAudio::SegueEnd:
	length=(int)(1000.0*(double)((edit_cursors[RDEditAudio::SegueEnd]-
		     edit_cursors[RDEditAudio::Play])*1152)/
		     (double)edit_sample_rate);
	break;

      case RDEditAudio::TalkStart:
      case RDEditAudio::TalkEnd:
	length=(int)(1000.0*(double)((edit_cursors[RDEditAudio::TalkEnd]-
		     edit_cursors[RDEditAudio::Play])*1152)/
		     (double)edit_sample_rate);
	break;

      case RDEditAudio::HookStart:
      case RDEditAudio::HookEnd:
	length=(int)(1000.0*(double)((edit_cursors[RDEditAudio::HookEnd]-
		     edit_cursors[RDEditAudio::Play])*1152)/
		     (double)edit_sample_rate);
	break;

      case RDEditAudio::FadeUp:
	length=(int)(1000.0*(double)((edit_cursors[RDEditAudio::FadeUp]-
		     edit_cursors[RDEditAudio::Play])*1152)/
		     (double)edit_sample_rate);
	break;

      default:
	break;
  }
  played_cursor=edit_cursors[RDEditAudio::Play];

  pause_mode=false;
  if(!is_playing) {
    edit_cae->setPlayPortActive(edit_card,edit_port,edit_stream);
    edit_cae->
      setOutputVolume(edit_card,edit_stream,edit_port,0+edit_gain_control->value());
    edit_cae->play(edit_handle,length,RD_TIMESCALE_DIVISOR,0);
  }
  if(use_looping) {
    is_looping=true;
  }
  edit_play_mode=RDEditAudio::FromCursor;
  edit_play_start_button->off();
  edit_play_cursor_button->on();
  edit_pause_button->off();
}


void RDEditAudio::pauseData()
{
  if(!is_paused) {
    is_looping=false;
    pause_mode=true;
    edit_cae->stopPlay(edit_handle);
  }
}


void RDEditAudio::stopData()
{
  if(!is_paused) {
    is_looping=false;
    pause_mode=false;
    edit_cae->stopPlay(edit_handle);
  }
}


void RDEditAudio::loopData()
{
  if(use_looping) {
    use_looping=false;
    is_looping=false;
    edit_loop_button->off();
  }
  else {
    use_looping=true;
    if(is_playing) {
      is_looping=true;
    }
    edit_loop_button->flash();
  }
}


void RDEditAudio::playedData(int handle)
{
  edit_pause_button->off();
  edit_stop_button->off();
  is_playing=true;
  is_paused=false;
  is_stopped=false;
  edit_meter_timer->start(RD_METER_UPDATE_INTERVAL);
}


void RDEditAudio::pausedData(int handle)
{
  if(ignore_pause) {
    return;
  }
  if(is_looping) {
    switch(edit_play_mode) {
	case RDEditAudio::FromStart:
	  LoopRegion(edit_cursors[RDEditAudio::Start],
		     edit_cursors[RDEditAudio::End]);
	  break;
	  
	case RDEditAudio::FromCursor:
	case RDEditAudio::Region:
	  switch(edit_cue_point) {
	      case RDEditAudio::Start:
	      case RDEditAudio::End:
		LoopRegion(edit_cursors[RDEditAudio::Start],
			   edit_cursors[RDEditAudio::End]);
		break;
		
	      case RDEditAudio::SegueStart:
	      case RDEditAudio::SegueEnd:
		LoopRegion(edit_cursors[RDEditAudio::SegueStart],
			   edit_cursors[RDEditAudio::SegueEnd]);
		break;
		
	      case RDEditAudio::TalkStart:
	      case RDEditAudio::TalkEnd:
		LoopRegion(edit_cursors[RDEditAudio::TalkStart],
			   edit_cursors[RDEditAudio::TalkEnd]);
		break;
		
	      case RDEditAudio::HookStart:
	      case RDEditAudio::HookEnd:
		LoopRegion(edit_cursors[RDEditAudio::HookStart],
			   edit_cursors[RDEditAudio::HookEnd]);
		break;
		
	      case RDEditAudio::FadeUp:
		LoopRegion(edit_cursors[RDEditAudio::Start],
			   edit_cursors[RDEditAudio::FadeUp]);
		break;
		
	      case RDEditAudio::FadeDown:
		LoopRegion(edit_cursors[RDEditAudio::FadeDown],
			   edit_cursors[RDEditAudio::End]);
		break;

	      default:
		break;
	  }
	  break;
    }
    return;
  }
  else {
    edit_play_start_button->off();
    edit_play_cursor_button->off();
    if(pause_mode) {
      edit_pause_button->on();
    }
    else {
      edit_stop_button->on();
      edit_cae->positionPlay(edit_handle,GetTime(played_cursor*1152));
    }
    is_playing=false;
    is_paused=true;
    is_stopped=false;  
  }
  if(edit_meter_timer->isActive()) {
    edit_meter_timer->stop();
    edit_meter->setLeftPeakBar(-10000);
    edit_meter->setRightPeakBar(-10000);
  }
}

  
void RDEditAudio::positionData(int handle,unsigned int pos)
{ //pos is in miliseconds
  edit_cursors[RDEditAudio::Play]=
    (unsigned int)((double)pos*edit_sample_rate/1152000.0);
  if((edit_sample_rate%8000)!=0) { //Account for the MPEG padding!
    edit_cursors[RDEditAudio::Play]++;
  }
  UpdateCursors();
  UpdateCounters();
}


void RDEditAudio::cuePointData(int id)
{
  if(is_playing) {
    edit_cue_button[id]->toggle();
    return;
  }
  if(delete_marker) {
    switch(id) {
	case RDEditAudio::SegueStart:
	case RDEditAudio::SegueEnd:
	  deleteSegueData();
	  break;

	case RDEditAudio::TalkStart:
	case RDEditAudio::TalkEnd:
	  deleteTalkData();
	  break;

	case RDEditAudio::HookStart:
	case RDEditAudio::HookEnd:
	  deleteHookData();
	  break;

	case RDEditAudio::FadeUp:
	  deleteFadeupData();
	  break;

	case RDEditAudio::FadeDown:
	  deleteFadedownData();
	  break;
    }
    for(int i=1;i<11;i++) {
      edit_cue_button[i]->setToggleButton(true);
    }
    edit_remove_button->setFlashingEnabled(false);
    edit_remove_button->setOn(false);
    delete_marker=false;
  }
  else {
    if(edit_cue_button[id]->isOn()) {
      edit_cue_button[id]->setFlashingEnabled(true);
      if(edit_cue_point!=RDEditAudio::Play) {
	cueEditData(edit_cue_point);
	edit_cue_button[edit_cue_point]->setOn(false);
	edit_cue_button[edit_cue_point]->setFlashingEnabled(false);
	edit_cursor_edit[edit_cue_point]->setReadOnly(true);
      }
      edit_cue_point=(RDEditAudio::CuePoints)id;
      edit_cue_string=edit_cursor_edit[edit_cue_point]->text();
      edit_cursor_edit[edit_cue_point]->setReadOnly(false);
      switch(id) {
	  case RDEditAudio::Start:
	  case RDEditAudio::SegueStart:
	  case RDEditAudio::TalkStart:
	  case RDEditAudio::HookStart:
	  case RDEditAudio::FadeDown:
	    edit_cae->positionPlay(edit_handle,GetTime(edit_cursors[id]*1152));
	    break;

	  case RDEditAudio::End:
	  case RDEditAudio::SegueEnd:
	  case RDEditAudio::TalkEnd:
	  case RDEditAudio::HookEnd:
	  case RDEditAudio::FadeUp:
	    PreRoll(edit_cursors[id]*1152,(RDEditAudio::CuePoints)id);
	    break;
      }
    }
    else {
      edit_cue_button[id]->setFlashingEnabled(false);
      edit_cue_point=RDEditAudio::Play;
      edit_cae->positionPlay(edit_handle,0);
    }
  }
  UpdateCounters();
}


void RDEditAudio::cueEditData(int id)
{
  int cursor;
  int old_cursor;

  old_cursor=1152*edit_cursors[id];
  if((cursor=(int)((double)(RDSetTimeLength(edit_cursor_edit[id]->text())*
      (double)edit_sample_rate/1000.0)))<0) {
    cursor=1152*edit_cursors[id];
    edit_cursor_edit[id]->setText(edit_cue_string);
    return;
  }
  ignore_pause=true;
  if(!PositionCursor(cursor)) {
    PositionCursor(old_cursor);
  }
  ignore_pause=false;
  edit_cursor_edit[id]->setFocus();
  edit_cursor_edit[id]->selectAll();
  return;
}


void RDEditAudio::cueEscData(int id)
{
  edit_cursor_edit[id]->setText(edit_cue_string);
  edit_cursor_edit[id]->selectAll();
}


void RDEditAudio::updateMenuData()
{
  if(edit_cursors[RDEditAudio::Start]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::Start,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::Start,false);
  } 
  if(edit_cursors[RDEditAudio::End]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::End,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::End,false);
  }
  if(edit_cursors[RDEditAudio::SegueStart]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::SegueStart,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::SegueStart,false);
  }
  if(edit_cursors[RDEditAudio::SegueEnd]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::SegueEnd,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::SegueEnd,false);
  }
  if(edit_cursors[RDEditAudio::TalkStart]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::TalkStart,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::TalkStart,false);
  }
  if(edit_cursors[RDEditAudio::TalkEnd]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::TalkEnd,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::TalkEnd,false);
  }
  if(edit_cursors[RDEditAudio::FadeUp]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::FadeUp,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::FadeUp,false);
  }
  if(edit_cursors[RDEditAudio::FadeDown]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::FadeDown,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::FadeDown,false);
  }
  if(edit_cursors[RDEditAudio::HookStart]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::HookStart,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::HookStart,false);
  }
  if(edit_cursors[RDEditAudio::HookEnd]!=-1) {
    edit_menu->setItemEnabled(RDEditAudio::HookEnd,true);
  }
  else {
    edit_menu->setItemEnabled(RDEditAudio::HookEnd,false);
  }
}


void RDEditAudio::deleteSegueData()
{
  DeleteMarkerData(RDEditAudio::SegueStart);
}


void RDEditAudio::deleteFadeupData()
{
  DeleteMarkerData(RDEditAudio::FadeUp);
}


void RDEditAudio::deleteFadedownData()
{
  DeleteMarkerData(RDEditAudio::FadeDown);
}


void RDEditAudio::deleteTalkData()
{
  DeleteMarkerData(RDEditAudio::TalkStart);
}


void RDEditAudio::deleteHookData()
{
  DeleteMarkerData(RDEditAudio::HookStart);
}


void RDEditAudio::trimHeadData()
{
  RDEditAudio::CuePoints point;
  RDTrimAudio::ErrorCode conv_err;
  RDTrimAudio *conv=new RDTrimAudio(edit_station,edit_config,this);
  conv->setCartNumber(edit_cut->cartNumber());
  conv->setCutNumber(edit_cut->cutNumber());
  conv->setTrimLevel(100*edit_trim_box->value());
  switch(conv_err=conv->runTrim(edit_user->name(),edit_user->password())) {
  case RDTrimAudio::ErrorOk:
    if(conv->startPoint()>=0) {
      point=edit_cue_point;
      edit_cue_point=RDEditAudio::Start;
      PositionCursor((double)conv->startPoint()*
		     (double)edit_sample_rate/1000.0);
      UpdateCounters();
      edit_cue_point=point;
    }
    break;

  default:
    QMessageBox::warning(this,tr("Edit Audio"),
			 RDTrimAudio::errorText(conv_err));
    break;
  }
  delete conv;
}


void RDEditAudio::trimTailData()
{
  RDEditAudio::CuePoints point;
  RDTrimAudio::ErrorCode conv_err;
  RDTrimAudio *conv=new RDTrimAudio(edit_station,edit_config,this);
  conv->setCartNumber(edit_cut->cartNumber());
  conv->setCutNumber(edit_cut->cutNumber());
  conv->setTrimLevel(100*edit_trim_box->value());
  switch(conv_err=conv->runTrim(edit_user->name(),edit_user->password())) {
  case RDTrimAudio::ErrorOk:
    if(conv->endPoint()>=0) {
      point=edit_cue_point;
      edit_cue_point=RDEditAudio::End;
      PositionCursor((double)conv->endPoint()*(double)edit_sample_rate/1000.0);
      UpdateCounters();
      edit_cue_point=point;
    }
    break;

  default:
    QMessageBox::warning(this,tr("Edit Audio"),
			 RDTrimAudio::errorText(conv_err));
    break;
  }
  delete conv;
}


void RDEditAudio::gainUpPressedData()
{
  edit_gain_mode=RDEditAudio::GainUp;
  gainTimerData();
  edit_gain_timer->start(TYPO_RATE_1);
}


void RDEditAudio::gainDownPressedData()
{
  edit_gain_mode=RDEditAudio::GainDown;
  gainTimerData();
  edit_gain_timer->start(TYPO_RATE_1);  
}


void RDEditAudio::gainChangedData()
{
  int gain;
  QString str;

  if(sscanf((const char *)edit_gain_edit->text(),"%d",&gain)==1) {
    edit_gain_control->setValue(gain*100);
  }
  str=QString(tr("dB"));
  edit_gain_edit->setText(QString().sprintf("%4.1f %s",
			  (double)edit_gain_control->value()/100.0,
			  (const char *)str));
  DrawMaps();
  repaint(false);
}


void RDEditAudio::gainReleasedData()
{
  edit_gain_timer->stop();
  edit_gain_mode=RDEditAudio::GainNone;
  edit_gain_count=0;
}


void RDEditAudio::gainTimerData()
{
  QString str;

  switch(edit_gain_mode) {
      case RDEditAudio::GainUp:
	edit_gain_control->addLine();
	if(edit_gain_count++==1) {
	  edit_gain_timer->changeInterval(TYPO_RATE_2);
	}
	break;

      case RDEditAudio::GainDown:
	edit_gain_control->subtractLine();
	if(edit_gain_count++==1) {
	  edit_gain_timer->changeInterval(TYPO_RATE_2);
	}
	break;

      default:
	break;
  }
  str=QString(tr("dB"));
  edit_gain_edit->setText(QString().sprintf("%4.1f %s",
			     (double)edit_gain_control->value()/100.0,
			     (const char *)str));
  DrawMaps();
  repaint(false);
}


void RDEditAudio::removeButtonData()
{
  if(edit_remove_button->isOn()) {
    if(edit_cue_point!=RDEditAudio::Play) {
      edit_cue_button[edit_cue_point]->setOn(false);
      edit_cue_button[edit_cue_point]->setFlashingEnabled(false);
      edit_cue_point=RDEditAudio::Play;
    }
    for(int i=1;i<11;i++) {
      edit_cue_button[i]->setToggleButton(false);
    }
    edit_remove_button->setFlashingEnabled(true);
    delete_marker=true;
  }
  else {
    for(int i=1;i<11;i++) {
      edit_cue_button[i]->setToggleButton(true);
    }
    edit_remove_button->setFlashingEnabled(false);
    delete_marker=false;
  }
  UpdateCounters();
}


void RDEditAudio::meterData()
{
  short levels[2];

  edit_cae->outputMeterUpdate(edit_card,edit_port,levels);
  edit_meter->setLeftPeakBar(levels[0]);
  edit_meter->setRightPeakBar(levels[1]);
}


void RDEditAudio::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  //
  // Waveforms
  //
  p->setPen(QColor(black));
  if(edit_channels==1) {
    p->drawImage(11,11,edit_left_image);
  }
  if(edit_channels==2) {
    p->drawImage(11,11,edit_left_image);
    p->drawImage(11,11+EDITAUDIO_WAVEFORM_HEIGHT/2,edit_right_image);
  }

  //
  // Highlights
  //
  p->drawRect(739,12,85,120);    // Amplitude Buttons
  p->drawRect(739,150,85,220);   // Time Buttons
  p->drawRect(739,385,85,165);   // Goto Buttons

  //
  // Transport Control Area
  //
  p->setPen(QColor(colorGroup().shadow()));
  p->fillRect(11,30+EDITAUDIO_WAVEFORM_HEIGHT,EDITAUDIO_WAVEFORM_WIDTH,92,
	      QColor(EDITAUDIO_HIGHLIGHT_COLOR));
  p->drawRect(11,30+EDITAUDIO_WAVEFORM_HEIGHT,EDITAUDIO_WAVEFORM_WIDTH,92);

  //
  // Marker Control Area
  //
  p->drawRect(11,130+EDITAUDIO_WAVEFORM_HEIGHT,717,197);

  p->end();
  delete p;
  UpdateCursors();
}


void RDEditAudio::mouseMoveEvent(QMouseEvent *e)
{
  int cursor;

  if((e->x()>10)&&(e->x()<(10+EDITAUDIO_WAVEFORM_WIDTH))&&
     (e->y()>10)&&(e->y()<(EDITAUDIO_WAVEFORM_HEIGHT+6))) {
    setCursor(*edit_cross_cursor);
    if(left_button_pressed) {
      cursor=(int)((((double)e->x()-10.0)*edit_factor_x+
		    (double)edit_hscroll->value())*1152.0);
      if(edit_cue_point!=RDEditAudio::Play) {
	ignore_pause=true;
	PositionCursor(cursor);
	ignore_pause=false;
      }
      else {
	ignore_pause=true;
	edit_cae->positionPlay(edit_handle,GetTime(cursor));
	ignore_pause=false;
      }
    }
    if(center_button_pressed) {
      cursor=(int)((((double)e->x()-10.0)*edit_factor_x+
		    (double)edit_hscroll->value())*1152.0);
      ignore_pause=true;
      edit_cae->positionPlay(edit_handle,GetTime(cursor));
      ignore_pause=false;
    }
  }
  else {
    setCursor(*edit_arrow_cursor);
  }
}


void RDEditAudio::mousePressEvent(QMouseEvent *e)
{
  int cursor;

  if((e->x()>10)&&(e->x()<788)&&(e->y()>10)&&(e->y()<400)) {
    cursor=(int)((((double)e->x()-10.0)*edit_factor_x+
		  (double)edit_hscroll->value())*1152.0);
    switch(e->button()) {
	case QMouseEvent::LeftButton:
	  left_button_pressed=true;
	  if(edit_cue_point!=RDEditAudio::Play) {
	    ignore_pause=true;
	    PositionCursor(cursor);
	    ignore_pause=false;
	  }
	  else {
	    ignore_pause=true;
	    edit_cae->positionPlay(edit_handle,GetTime(cursor));
	    ignore_pause=false;
	  }
	  break;

	case QMouseEvent::MidButton:
	  center_button_pressed=true;
	  ignore_pause=true;
	  edit_cae->positionPlay(edit_handle,GetTime(cursor));
	  ignore_pause=false;
	  break;

	case QMouseEvent::RightButton:
	  edit_menu->setGeometry(e->x(),e->y()+53,
				 edit_menu->sizeHint().width(),
				 edit_menu->sizeHint().height());
	  edit_menu->exec();
	  break;

	default:
	  break;
    }
  }
}


void RDEditAudio::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()) {
      case QMouseEvent::LeftButton:
	left_button_pressed=false;
	break;

      case QMouseEvent::MidButton:
	center_button_pressed=false;
	break;

      default:
	break;
  }
}


void RDEditAudio::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Key_Space:
	if(is_playing) {
	  stopData();
	}
	else {
	  if(e->state()==0) {
	    playCursorData();
	  }
	  if((e->state()&ControlButton)!=0) {
	    playStartData();
	  }
	}
	e->accept();
	break;

      case Key_Left:
	PositionCursor(-(edit_sample_rate/10),true);
	e->accept();
	break;

      case Key_Right:
	PositionCursor(edit_sample_rate/10,true);
	e->accept();
	break;

      case Key_Plus:
	xUp();
	break;
	
      case Key_Minus:
	xDown();
	break;
	
      case Key_Home:
	gotoHomeData();
	break;

      case Key_End:
	gotoEndData();
	break;

      case Key_Delete:
	DeleteMarkerData(edit_cue_point);
	break;

      default:
	e->ignore();
	break;
  }
}


void RDEditAudio::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void RDEditAudio::DeleteMarkerData(int id)
{
  if((id==RDEditAudio::Play)||(id==RDEditAudio::Start)||(id==RDEditAudio::End)) {
    return;
  }
  switch(id) {
      case RDEditAudio::Start:
      case RDEditAudio::TalkStart:
      case RDEditAudio::SegueStart:
      case RDEditAudio::HookStart:
	edit_cursors[id]=-1;
	edit_cursors[id+1]=-1;
	edit_cursor_edit[id]->clear();
	edit_cursor_edit[id+1]->clear();
	break;

      case RDEditAudio::End:
      case RDEditAudio::TalkEnd:
      case RDEditAudio::SegueEnd:
      case RDEditAudio::HookEnd:
	edit_cursors[id]=-1;
	edit_cursors[id-1]=-1;
	edit_cursor_edit[id]->clear();
	edit_cursor_edit[id-1]->clear();
	break;

      case RDEditAudio::FadeUp:
      case RDEditAudio::FadeDown:
	edit_cursors[id]=-1;
	edit_cursor_edit[id]->clear();
	break;
  }
  UpdateCursors();
  UpdateCounters();
}


void RDEditAudio::PreRoll(int cursor,RDEditAudio::CuePoints point)
{
  int prepoint=cursor-edit_preroll;
  switch(point) {
      case RDEditAudio::SegueEnd:
	if(prepoint>1152*edit_cursors[RDEditAudio::SegueStart]) {
	  edit_cae->positionPlay(edit_handle,GetTime(prepoint));
	}
	else {
	  edit_cae->positionPlay(edit_handle,
			   GetTime(1152*edit_cursors[RDEditAudio::SegueStart]));
	}
	break;
      case RDEditAudio::End:
	if(prepoint>1152*edit_cursors[RDEditAudio::Start]) {
	  edit_cae->positionPlay(edit_handle,GetTime(prepoint));
	}
	else {
	  edit_cae->positionPlay(edit_handle,
			      GetTime(1152*edit_cursors[RDEditAudio::Start]));
	}
	break;
      case RDEditAudio::TalkEnd:
	if(prepoint>1152*edit_cursors[RDEditAudio::TalkStart]) {
	  edit_cae->positionPlay(edit_handle,GetTime(prepoint));
	}
	else {
	  edit_cae->positionPlay(edit_handle,
			     GetTime(1152*edit_cursors[RDEditAudio::TalkStart]));
	}
	break;
      case RDEditAudio::HookEnd:
	if(prepoint>1152*edit_cursors[RDEditAudio::HookStart]) {
	  edit_cae->positionPlay(edit_handle,GetTime(prepoint));
	}
	else {
	  edit_cae->positionPlay(edit_handle,
			     GetTime(1152*edit_cursors[RDEditAudio::HookStart]));
	}
	break;

      default:
	break;
  }
}


bool RDEditAudio::PositionCursor(int cursor,bool relative)
{
  switch(edit_cue_point) {
      case RDEditAudio::Start:
      case RDEditAudio::TalkStart:
      case RDEditAudio::HookStart:
      case RDEditAudio::SegueStart:
	if((edit_cursors[edit_cue_point+1]==-1)&&(cursor!=-1)) {
	  edit_cursors[edit_cue_point+1]=edit_cursors[RDEditAudio::End];
	  edit_cursor_edit[edit_cue_point+1]->
	    setText(RDGetTimeLength(
		     (int)(1152000.0*(double)edit_cursors[edit_cue_point+1]/
		 (double)edit_sample_rate),true));
	}
	if(relative) {
	  if((edit_cursors[edit_cue_point]+cursor/1152)>
	     edit_cursors[edit_cue_point+1]) {
	    return false;
	  }
	  if(((edit_cursors[edit_cue_point]+cursor/1152)<
	      edit_cursors[RDEditAudio::Start])&&
	     (edit_cue_point!=RDEditAudio::Start)) {
	    return false;
	  }
	  edit_cursors[edit_cue_point]+=cursor/1152;
	  cursor=edit_cursors[edit_cue_point]*1152;
	}
	else {
	  if((cursor/1152)>edit_cursors[edit_cue_point+1]) {
	    return false;
	  }
	  if(((cursor/1152)<edit_cursors[RDEditAudio::Start])&&
	    (edit_cue_point!=RDEditAudio::Start)) {
	    return false;
	  }
	  edit_cursors[edit_cue_point]=cursor/1152;
	}
	edit_cursor_edit[edit_cue_point]->
	  setText(RDGetTimeLength((int)(1000.0*(double)cursor/
			       (double)edit_sample_rate),true));
	edit_cae->positionPlay(edit_handle,GetTime(cursor));
	break;
	
      case RDEditAudio::End:
      case RDEditAudio::TalkEnd:
      case RDEditAudio::HookEnd:
      case RDEditAudio::SegueEnd:
	if((edit_cursors[edit_cue_point-1]==-1)&&(cursor!=-1)) {
	  edit_cursors[edit_cue_point-1]=edit_cursors[RDEditAudio::Start];
	  edit_cursor_edit[edit_cue_point-1]->
	    setText(RDGetTimeLength(
		  (int)(1152000.0*(double)edit_cursors[edit_cue_point-1]/
		 (double)edit_sample_rate),true));
	}
	if(relative) {
	  if((edit_cursors[edit_cue_point]+cursor/1152)<
	     edit_cursors[edit_cue_point-1]) {
	    return false;
	  }
	  if(((edit_cursors[edit_cue_point]+cursor/1152)>
	      edit_cursors[RDEditAudio::End])&&
	     (edit_cue_point!=RDEditAudio::End)) {
	    return false;
	  }
	  if((edit_cue_point==RDEditAudio::End)&&
	     ((1152*edit_cursors[edit_cue_point]+cursor)
	      >(int)edit_sample_length)) {
	    cursor=edit_sample_length-
	      1152*edit_cursors[edit_cue_point];;
	  }
	  edit_cursors[edit_cue_point]+=cursor/1152;
	  cursor=edit_cursors[edit_cue_point]*1152;
	}
	else {
	  if((cursor/1152)<edit_cursors[edit_cue_point-1]) {
	    return false;
	  }
	  if(((cursor/1152)>edit_cursors[RDEditAudio::End])&&
	    (edit_cue_point!=RDEditAudio::End)) {
	    return false;
	  }
	  if((edit_cue_point==RDEditAudio::End)&&
	     (cursor>(int)edit_sample_length)) {
	    cursor=edit_sample_length;
	  }
	  edit_cursors[edit_cue_point]=cursor/1152;
	}
	if(((edit_play_mode==RDEditAudio::Region)&&
	    ((edit_cue_point==edit_cue_point-1)||
	     (edit_cue_point==edit_cue_point)))) {
	}
	edit_cursor_edit[edit_cue_point]->
	  setText(RDGetTimeLength((int)(1000.0*(double)cursor/
			       (double)edit_sample_rate),true));
	PreRoll(cursor,edit_cue_point);
	break;
	
      case RDEditAudio::FadeUp:
	if(relative) {
	  if(((edit_cursors[RDEditAudio::FadeUp]+cursor/1152)>
	      edit_cursors[RDEditAudio::FadeDown])&&
	     edit_cursors[RDEditAudio::FadeDown]==-1) {
	    return false;
	  }
	  if((edit_cursors[edit_cue_point]+cursor/1152)<
	     edit_cursors[RDEditAudio::Start]) {
	    return false;
	  }
	  edit_cursors[RDEditAudio::FadeUp]+=cursor/1152;
	  cursor=edit_cursors[RDEditAudio::FadeUp]*1152;
	}
	else {
	  if(((cursor/1152)>edit_cursors[RDEditAudio::FadeDown])&&
	     (edit_cursors[RDEditAudio::FadeDown]!=-1)) {
	    return false;
	  }
	  if(((cursor/1152)<edit_cursors[RDEditAudio::Start])||
	    ((cursor/1152)>edit_cursors[RDEditAudio::End])) {
	    return false;
	  }
	  edit_cursors[RDEditAudio::FadeUp]=cursor/1152;
	}
	if(((edit_play_mode==RDEditAudio::Region)&&
	    ((edit_cue_point==RDEditAudio::FadeUp)))) {
	}
	edit_cursor_edit[RDEditAudio::FadeUp]->
	  setText(RDGetTimeLength((int)(1000.0*(double)cursor/
			       (double)edit_sample_rate),true));
	edit_cae->positionPlay(edit_handle,
			    GetTime(edit_cursors[RDEditAudio::Start]*1152));
	break;
	
      case RDEditAudio::FadeDown:
	if(relative) {
	  if((edit_cursors[RDEditAudio::FadeDown]+cursor/1152)<
	     edit_cursors[RDEditAudio::FadeUp]) {
	    return false;
	  }
	  if(((edit_cursors[RDEditAudio::FadeDown]+cursor/1152)<
	      edit_cursors[RDEditAudio::Start])||
	    ((edit_cursors[RDEditAudio::FadeDown]+cursor/1152)>
	     edit_cursors[RDEditAudio::End])) {
	    return false;
	  }
	  edit_cursors[RDEditAudio::FadeDown]+=cursor/1152;
	  cursor=edit_cursors[RDEditAudio::FadeDown]*1152;
	}
	else {
	  if(((cursor/1152)<edit_cursors[RDEditAudio::FadeUp])) {
	    return false;
	  }
	  if(((cursor/1152)<edit_cursors[RDEditAudio::Start])||
	    ((cursor/1152)>edit_cursors[RDEditAudio::End])) {
	    return false;
	  }
	  edit_cursors[RDEditAudio::FadeDown]=cursor/1152;
	}
	edit_cursor_edit[RDEditAudio::FadeDown]->
	  setText(RDGetTimeLength((int)(1000.0*(double)cursor/
			       (double)edit_sample_rate),true));
	edit_cae->positionPlay(edit_handle,GetTime(cursor));
	break;	

      default:
	break;
  }
  ValidateMarkers();
  UpdateCursors();
  UpdateCounters();
  update(0,0,11,399);
  update(11+EDITAUDIO_WAVEFORM_WIDTH,0,11,399);
  return true;
}


void RDEditAudio::ValidateMarkers()
{
  for(int i=RDEditAudio::SegueStart;i<RDEditAudio::FadeUp;i+=2) {
    if(edit_cursors[i]!=-1) {
      if(edit_cursors[i]<edit_cursors[RDEditAudio::Start]) {
	if((edit_cursors[i+1]>=edit_cursors[RDEditAudio::Start])&&
	   (edit_cursors[i+1]<=edit_cursors[RDEditAudio::End])) {
	  edit_cursors[i]=edit_cursors[RDEditAudio::Start];
	  edit_cursor_edit[i]->setText(
	    RDGetTimeLength((int)((double)edit_cursors[i]*
		 1152000.0/(double)edit_sample_rate),true));
	}
	else {
	  edit_cursors[i]=-1;
	  edit_cursors[i+1]=-1;
	  edit_cursor_edit[i]->clear();
	  edit_cursor_edit[i+1]->clear();
	}
      }
    }
    if(edit_cursors[i+1]!=-1) {
      if(edit_cursors[i+1]>edit_cursors[RDEditAudio::End]) {
	if((edit_cursors[i]>=edit_cursors[RDEditAudio::Start])&&
	   (edit_cursors[i]<=edit_cursors[RDEditAudio::End])) {
	  edit_cursors[i+1]=edit_cursors[RDEditAudio::End];
	  edit_cursor_edit[i+1]->setText(
	    RDGetTimeLength((int)((double)edit_cursors[i+1]*
		 1152000.0/(double)edit_sample_rate),true));
	}
	else {
	  edit_cursors[i]=-1;
	  edit_cursors[i+1]=-1;
	  edit_cursor_edit[i]->clear();
	  edit_cursor_edit[i+1]->clear();
	}
      }
    }
  }
  for(int i=RDEditAudio::FadeUp;i<RDEditAudio::LastMarker;i++) {
    if(edit_cursors[i]!=-1) {
      if(edit_cursors[i]<edit_cursors[RDEditAudio::Start]) {
	edit_cursors[i]=-1;
	edit_cursor_edit[i]->clear();
      }
      if(edit_cursors[i]>edit_cursors[RDEditAudio::End]) {
	edit_cursors[i]=-1;
	edit_cursor_edit[i]->clear();
      }
    }
  }
}


bool RDEditAudio::SaveMarkers()
{
  //
  // Sanity Checks
  //
  int start_point=(int)((double)(edit_cursors[RDEditAudio::Start])*
			1152000.0/(double)edit_sample_rate);
  int end_point=(int)((double)(edit_cursors[RDEditAudio::End])*
		      1152000.0/(double)edit_sample_rate)+26;
  int len=(int)(1000.0*(double)edit_sample_length/(double)edit_sample_rate);
  if((2*(end_point-start_point))<len) {
    if(QMessageBox::question(this,tr("Marker Warning"),
			     tr("Less than half of the audio is playable with these marker settings.\nAre you sure you want to save?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      return false;
    }
  }

  if(edit_cursors[RDEditAudio::SegueStart]!=-1) {
    len=end_point-start_point;
    start_point=(int)((double)(edit_cursors[RDEditAudio::SegueStart])*
		      1152000.0/(double)edit_sample_rate);
    end_point=(int)((double)(edit_cursors[RDEditAudio::SegueEnd])*
		    1152000.0/(double)edit_sample_rate);
    if((2*(end_point-start_point))>len) {
      if(QMessageBox::question(this,tr("Marker Warning"),
			       tr("More than half of the audio will be faded with these marker settings.\nAre you sure you want to save?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
	return false;
      }
    }
  }

  //
  // Save Settings
  //
  edit_cut->setPlayGain(10*edit_gain_control->value());
  edit_cut->setStartPoint((int)((double)(edit_cursors[RDEditAudio::Start])*
				1152000.0/
				(double)edit_sample_rate));
  edit_cut->setEndPoint((int)((double)(edit_cursors[RDEditAudio::End])*
			      1152000.0/
			      (double)edit_sample_rate)+26);
  if(edit_cursors[RDEditAudio::TalkStart]!=-1) {
    edit_cut->
      setTalkStartPoint((int)((double)(edit_cursors[RDEditAudio::TalkStart])*
			      1152000.0/
			      (double)edit_sample_rate));
  }
  else {
    edit_cut->setTalkStartPoint(-1);
  }
  if(edit_cursors[RDEditAudio::TalkEnd]!=-1) {
    edit_cut->setTalkEndPoint((int)((double)(edit_cursors[RDEditAudio::TalkEnd])*
				    1152000.0/
				(double)edit_sample_rate));
  }
  else {
    edit_cut->setTalkEndPoint(-1);
  }
  if(edit_cursors[RDEditAudio::SegueStart]!=-1) {
    edit_cut->setSegueStartPoint((int)((double)(edit_cursors[RDEditAudio::SegueStart])*1152000.0/
				  (double)edit_sample_rate));
  }
  else {
    edit_cut->setSegueStartPoint(-1);
  }
  if(edit_cursors[RDEditAudio::SegueEnd]!=-1) {
    edit_cut->
      setSegueEndPoint((int)((double)(edit_cursors[RDEditAudio::SegueEnd])*
			     1152000.0/(double)edit_sample_rate));
  }
  else {
    edit_cut->setSegueEndPoint(-1);
  }
  if(edit_cursors[RDEditAudio::FadeUp]!=-1) {
    edit_cut->setFadeupPoint((int)((double)(edit_cursors[RDEditAudio::FadeUp])*1152000.0/
				  (double)edit_sample_rate));
  }
  else {
    edit_cut->setFadeupPoint(-1);
  }
  if(edit_cursors[RDEditAudio::FadeDown]!=-1) {
    edit_cut->
      setFadedownPoint((int)((double)(edit_cursors[RDEditAudio::FadeDown])*
			     1152000.0/(double)edit_sample_rate));
  }
  else {
    edit_cut->setFadedownPoint(-1);
  }
  if(edit_cursors[RDEditAudio::HookStart]!=-1) {
    edit_cut->setHookStartPoint((int)((double)(edit_cursors[RDEditAudio::HookStart])*1152000.0/
				  (double)edit_sample_rate));
  }
  else {
    edit_cut->setHookStartPoint(-1);
  }
  if(edit_cursors[RDEditAudio::HookEnd]!=-1) {
    edit_cut->setHookEndPoint((int)((double)(edit_cursors[RDEditAudio::HookEnd])*
				    1152000.0/
				    (double)edit_sample_rate));
  }
  else {
    edit_cut->setHookEndPoint(-1);
  }
  edit_cut->setLength(edit_cut->endPoint(true)-edit_cut->startPoint(true));
  edit_cut->setPlayGain(edit_gain_control->value());
  if(edit_overlap_box->isChecked()) {
    edit_cut->setSegueGain(0);
  }
  else {
    edit_cut->setSegueGain(RD_FADE_DEPTH);
  }
  return true;
}


void RDEditAudio::LoopRegion(int cursor0,int cursor1)
{
  int length=0;

  if(cursor1!=-1) {
    length=(int)(1000.0*(double)((cursor1-cursor0)*
				 1152)/(double)edit_sample_rate);
  }
  if(cursor0==-1) {
    edit_cae->positionPlay(edit_handle,0);
    edit_cae->
      setOutputVolume(edit_card,edit_stream,edit_port,0+edit_gain_control->value());
    edit_cae->play(edit_handle,length,RD_TIMESCALE_DIVISOR,0);
  }
  else {
    edit_cae->positionPlay(edit_handle,GetTime(cursor0*1152));
    edit_cae->
      setOutputVolume(edit_card,edit_stream,edit_port,0+edit_gain_control->value());
    edit_cae->play(edit_handle,length,RD_TIMESCALE_DIVISOR,0);
  }
}


void RDEditAudio::UpdateCounters()
{
  int cursor0=0;
  int cursor1=0;
  bool null_region=false;
  static RDEditAudio::CuePoints prev_cue_point=RDEditAudio::Play;

  edit_overall_edit->setText(RDGetTimeLength(
	  (int)(1000.0*(double)((edit_cursors[RDEditAudio::Play]-baseline)*1152)/
	  (double)edit_sample_rate),true,true));
  if(prev_cue_point!=RDEditAudio::Play) {
    edit_cursor_edit[prev_cue_point]->clearFocus();
    edit_cursor_edit[prev_cue_point]->deselect();
  }

  switch(edit_cue_point) {
      case RDEditAudio::Play:
	edit_play_cursor_button->
	    setAccentColor(EDITAUDIO_WAVEFORM_COLOR);
	edit_region_edit_label->setText(tr("<none>"));
	edit_region_edit_label->
	  setPalette(QPalette(backgroundColor(),
			      QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
	break;

      case RDEditAudio::Start:
	cursor0=edit_cursors[RDEditAudio::Start];
	cursor1=edit_cursors[RDEditAudio::End];
	edit_play_cursor_button->
	    setAccentColor(QColor(RD_START_END_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Cut"));
	edit_region_edit_label->setPalette(RD_START_END_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::Start]->setFocus();
	edit_cursor_edit[RDEditAudio::Start]->selectAll();
	break;

      case RDEditAudio::End:
	cursor0=edit_cursors[RDEditAudio::Start];
	cursor1=edit_cursors[RDEditAudio::End];
	edit_play_cursor_button->
	    setAccentColor(QColor(RD_START_END_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Cut"));
	edit_region_edit_label->setPalette(RD_START_END_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::End]->setFocus();
	edit_cursor_edit[RDEditAudio::End]->selectAll();
	break;

      case RDEditAudio::TalkStart:
	cursor0=edit_cursors[RDEditAudio::TalkStart];
	cursor1=edit_cursors[RDEditAudio::TalkEnd];
	edit_play_cursor_button->setAccentColor(QColor(RD_TALK_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Talk"));
	edit_region_edit_label->setPalette(RD_TALK_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::TalkStart]->setFocus();
	edit_cursor_edit[RDEditAudio::TalkStart]->selectAll();
	break;

      case RDEditAudio::TalkEnd:
	cursor0=edit_cursors[RDEditAudio::TalkStart];
	cursor1=edit_cursors[RDEditAudio::TalkEnd];
	edit_play_cursor_button->setAccentColor(QColor(RD_TALK_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Talk"));
	edit_region_edit_label->setPalette(RD_TALK_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::TalkEnd]->setFocus();
	edit_cursor_edit[RDEditAudio::TalkEnd]->selectAll();
	break;

      case RDEditAudio::SegueStart:
	cursor0=edit_cursors[RDEditAudio::SegueStart];
	cursor1=edit_cursors[RDEditAudio::SegueEnd];
	edit_play_cursor_button->setAccentColor(QColor(RD_SEGUE_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Segue"));
	edit_region_edit_label->setPalette(RD_SEGUE_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::SegueStart]->setFocus();
	edit_cursor_edit[RDEditAudio::SegueStart]->selectAll();
	break;

      case RDEditAudio::SegueEnd:
	cursor0=edit_cursors[RDEditAudio::SegueStart];
	cursor1=edit_cursors[RDEditAudio::SegueEnd];
	edit_play_cursor_button->setAccentColor(QColor(RD_SEGUE_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Segue"));
	edit_region_edit_label->setPalette(RD_SEGUE_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::SegueEnd]->setFocus();
	edit_cursor_edit[RDEditAudio::SegueEnd]->selectAll();
	break;

      case RDEditAudio::HookStart:
	cursor0=edit_cursors[RDEditAudio::HookStart];
	cursor1=edit_cursors[RDEditAudio::HookEnd];
	edit_play_cursor_button->setAccentColor(QColor(RD_HOOK_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Hook"));
	edit_region_edit_label->setPalette(RD_HOOK_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::HookStart]->setFocus();
	edit_cursor_edit[RDEditAudio::HookStart]->selectAll();
	break;

      case RDEditAudio::HookEnd:
	cursor0=edit_cursors[RDEditAudio::HookStart];
	cursor1=edit_cursors[RDEditAudio::HookEnd];
	edit_play_cursor_button->setAccentColor(QColor(RD_HOOK_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Hook"));
	edit_region_edit_label->setPalette(RD_HOOK_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::HookEnd]->setFocus();
	edit_cursor_edit[RDEditAudio::HookEnd]->selectAll();
	break;

      case RDEditAudio::FadeUp:
	cursor0=edit_cursors[RDEditAudio::Start];
	cursor1=edit_cursors[RDEditAudio::FadeUp];
	edit_play_cursor_button->setAccentColor(QColor(RD_FADE_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Fade Up"));
	edit_region_edit_label->setPalette(RD_FADE_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::FadeUp]->setFocus();
	edit_cursor_edit[RDEditAudio::FadeUp]->selectAll();
	break;

      case RDEditAudio::FadeDown:
	cursor0=edit_cursors[RDEditAudio::FadeDown];
	cursor1=edit_cursors[RDEditAudio::End];
	edit_play_cursor_button->setAccentColor(QColor(RD_FADE_MARKER_COLOR));
	edit_region_edit_label->setText(tr("Fade Down"));
	edit_region_edit_label->setPalette(RD_FADE_MARKER_COLOR);
	edit_cursor_edit[RDEditAudio::FadeDown]->setFocus();
	edit_cursor_edit[RDEditAudio::FadeDown]->selectAll();
	break;

      case RDEditAudio::LastMarker:
	break;
  }
  if(cursor0==-1) {
    cursor0=0;
    null_region=true;
  }
  if(cursor1==-1) {
    cursor1=edit_sample_length/1152;
    null_region=true;
  }
  if(null_region||(edit_cue_point==RDEditAudio::Play)) {
    edit_region_edit->setText("0:00:00.0");
    edit_region_edit->setDisabled(true);
  }
  else {
    edit_region_edit->setText(RDGetTimeLength(
			    (int)(1000.0*(double)((cursor1-cursor0)*1152)/
			    (double)edit_sample_rate),true,true));
    edit_region_edit->setEnabled(true);
  }
  edit_size_edit->setText(RDGetTimeLength(
			 (int)(1000.0*(double)((edit_cursors[RDEditAudio::End]-
					edit_cursors[RDEditAudio::Start])*1152)/
		         (double)edit_sample_rate),true,true));
  prev_cue_point=edit_cue_point;
}


void RDEditAudio::DrawMaps()
{
  QPixmap *pix=NULL;

  if(edit_channels==1) {
    pix=new QPixmap(EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT);
    DrawWave(EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT,0,"",pix);
    edit_left_image=pix->convertToImage();
    delete pix;
  }
  if(edit_channels==2) {
    pix=new QPixmap(EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT/2);
    DrawWave(EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT/2,0,tr("L"),
	     pix);
    edit_left_image=pix->convertToImage();
    DrawWave(EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT/2,1,tr("R"),
	     pix);
    edit_right_image=pix->convertToImage();
    delete pix;
  }
}


void RDEditAudio::UpdateCursors()
{
  if(edit_channels==1) {
    DrawCursors(11,11,EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT,0);
  }
  if(edit_channels==2) {
    DrawCursors(11,11,EDITAUDIO_WAVEFORM_WIDTH,EDITAUDIO_WAVEFORM_HEIGHT/2,0);
    DrawCursors(11,11+EDITAUDIO_WAVEFORM_HEIGHT/2,EDITAUDIO_WAVEFORM_WIDTH,
		EDITAUDIO_WAVEFORM_HEIGHT/2,1);
  }

}


void RDEditAudio::DrawCursors(int xpos,int ypos,int xsize,int ysize,int chan)
{
  static int prev_x[2][12]={{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
			    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::Play],prev_x[chan][RDEditAudio::Play],
	      QColor(EDITAUDIO_PLAY_COLOR),RDEditAudio::None,20);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::SegueStart],
	      prev_x[chan][RDEditAudio::SegueStart],
	      QColor(RD_SEGUE_MARKER_COLOR),
	      RDEditAudio::Right,30);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::SegueEnd],
	      prev_x[chan][RDEditAudio::SegueEnd],
	      QColor(RD_SEGUE_MARKER_COLOR),
	      RDEditAudio::Left,30);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::Start],
	      prev_x[chan][RDEditAudio::Start],
	      QColor(RD_START_END_MARKER_COLOR),
	      RDEditAudio::Right,10);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::End],
	      prev_x[chan][RDEditAudio::End],
	      QColor(RD_START_END_MARKER_COLOR),
	      RDEditAudio::Left,10);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::TalkStart],
	      prev_x[chan][RDEditAudio::TalkStart],
	      QColor(RD_TALK_MARKER_COLOR),
	      RDEditAudio::Right,20);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::TalkEnd],
	      prev_x[chan][RDEditAudio::TalkEnd],
	      QColor(RD_TALK_MARKER_COLOR),
	      RDEditAudio::Left,20);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::FadeUp],
	      prev_x[chan][RDEditAudio::FadeUp],
	      QColor(RD_FADE_MARKER_COLOR),
	      RDEditAudio::Left,40);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::FadeDown],
	      prev_x[chan][RDEditAudio::FadeDown],
	      QColor(RD_FADE_MARKER_COLOR),
	      RDEditAudio::Right,40);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::HookStart],
	      prev_x[chan][RDEditAudio::HookStart],
	      QColor(RD_HOOK_MARKER_COLOR),
	      RDEditAudio::Right,50);

  EraseCursor(xpos,ypos,xsize,ysize,chan,
	      edit_cursors[RDEditAudio::HookEnd],
	      prev_x[chan][RDEditAudio::HookEnd],
	      QColor(RD_HOOK_MARKER_COLOR),
	      RDEditAudio::Left,50);


  prev_x[chan][RDEditAudio::Play]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					   edit_cursors[RDEditAudio::Play],
					   prev_x[chan][RDEditAudio::Play],
					   QColor(EDITAUDIO_PLAY_COLOR),
					   RDEditAudio::None,20,RDEditAudio::Play,
					   Qt::XorROP);
  
  prev_x[chan][RDEditAudio::SegueStart]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					   edit_cursors[RDEditAudio::SegueStart],
					   prev_x[chan][RDEditAudio::SegueStart],
					   QColor(RD_SEGUE_MARKER_COLOR),
					   RDEditAudio::Right,30,
					   RDEditAudio::SegueStart);

  prev_x[chan][RDEditAudio::SegueEnd]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					   edit_cursors[RDEditAudio::SegueEnd],
					   prev_x[chan][RDEditAudio::SegueEnd],
					   QColor(RD_SEGUE_MARKER_COLOR),
					   RDEditAudio::Left,30,
					   RDEditAudio::SegueEnd);

  prev_x[chan][RDEditAudio::Start]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					    edit_cursors[RDEditAudio::Start],
					    prev_x[chan][RDEditAudio::Start],
					    QColor(RD_START_END_MARKER_COLOR),
					    RDEditAudio::Right,10,
					    RDEditAudio::Start);

  prev_x[chan][RDEditAudio::End]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					  edit_cursors[RDEditAudio::End],
					  prev_x[chan][RDEditAudio::End],
					  QColor(RD_START_END_MARKER_COLOR),
					  RDEditAudio::Left,10,
					  RDEditAudio::End);

  prev_x[chan][RDEditAudio::TalkStart]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					  edit_cursors[RDEditAudio::TalkStart],
					  prev_x[chan][RDEditAudio::TalkStart],
					  QColor(RD_TALK_MARKER_COLOR),
					  RDEditAudio::Right,20,
					  RDEditAudio::TalkStart);

  prev_x[chan][RDEditAudio::TalkEnd]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					      edit_cursors[RDEditAudio::TalkEnd],
					      prev_x[chan][RDEditAudio::TalkEnd],
					      QColor(RD_TALK_MARKER_COLOR),
					      RDEditAudio::Left,20,
					      RDEditAudio::TalkEnd);

  prev_x[chan][RDEditAudio::FadeUp]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					     edit_cursors[RDEditAudio::FadeUp],
					     prev_x[chan][RDEditAudio::FadeUp],
					     QColor(RD_FADE_MARKER_COLOR),
					     RDEditAudio::Left,40,
					     RDEditAudio::FadeUp);

  prev_x[chan][RDEditAudio::FadeDown]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					     edit_cursors[RDEditAudio::FadeDown],
					     prev_x[chan][RDEditAudio::FadeDown],
					     QColor(RD_FADE_MARKER_COLOR),
					     RDEditAudio::Right,40,
					     RDEditAudio::FadeDown);

  prev_x[chan][RDEditAudio::HookStart]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					    edit_cursors[RDEditAudio::HookStart],
					    prev_x[chan][RDEditAudio::HookStart],
					    QColor(RD_HOOK_MARKER_COLOR),
					    RDEditAudio::Right,50,
					    RDEditAudio::HookStart);

  prev_x[chan][RDEditAudio::HookEnd]=DrawCursor(xpos,ypos,xsize,ysize,chan,
					      edit_cursors[RDEditAudio::HookEnd],
					      prev_x[chan][RDEditAudio::HookEnd],
					      QColor(RD_HOOK_MARKER_COLOR),
					      RDEditAudio::Left,50,
					      RDEditAudio::HookEnd);
}


int RDEditAudio::DrawCursor(int xpos,int ypos,int xsize,int ysize,int chan,
			  int samp,int prev,QColor color,Arrow arrow,int apos,
			  RDEditAudio::CuePoints pt,Qt::RasterOp op)
{
  int x;
  QPointArray *point;

  if(samp<0) {
    return 0;
  }
  x=(int)((double)(samp-edit_hscroll->value())/edit_factor_x);
  if((x!=prev)||(pt!=RDEditAudio::Play)) {
    QPainter *p=new QPainter(this);
    p->setClipRect(xpos,ypos,xsize,ysize);
    p->setRasterOp(op);
    p->translate(xpos,ypos);
    if((x>=0)&(x<EDITAUDIO_WAVEFORM_WIDTH)) {
      p->setPen(color);
      p->moveTo(x,0);
      p->lineTo(x,ysize);
      if(arrow==RDEditAudio::Left) {
	p->setClipRect(0,0,xsize+xpos+10,ysize+ypos);
	p->setBrush(color);
	point=new QPointArray(3);
	point->setPoint(0,x,apos);
	point->setPoint(1,x+10,apos-5);
	point->setPoint(2,x+10,apos+5);
	p->drawPolygon(*point);
	point->setPoint(0,x,ysize-apos);
	point->setPoint(1,x+10,ysize-apos-5);
	point->setPoint(2,x+10,ysize-apos+5);
	p->drawPolygon(*point);
	delete point;
      }
      if(arrow==RDEditAudio::Right) {
	p->setClipRect(-10,0,xsize+10,ysize+ypos);
	p->setBrush(color);
	point=new QPointArray(3);
	point->setPoint(0,x,apos);
	point->setPoint(1,x-10,apos-5);
	point->setPoint(2,x-10,apos+5);
	p->drawPolygon(*point);
	point->setPoint(0,x,ysize-apos);
	point->setPoint(1,x-10,ysize-apos-5);
	point->setPoint(2,x-10,ysize-apos+5);
	p->drawPolygon(*point);
	delete point;
      }
    }
    p->end();
    delete p;
  }
  return x;
}


void RDEditAudio::EraseCursor(int xpos,int ypos,int xsize,int ysize,int chan,
			   int samp,int prev,QColor color,Arrow arrow,int apos)
{
  int x;

  if(edit_hscroll==NULL) {
    return;
  }
  x=(int)((double)(samp-edit_hscroll->value())/edit_factor_x);
  if(x!=prev) {
    QPainter *p=new QPainter(this);
    p->translate(xpos,ypos);
    if((prev>=0)&&(prev<EDITAUDIO_WAVEFORM_WIDTH)&&(prev!=x)) {
      if(chan==0) {
	p->drawImage(prev,0,edit_left_image,prev,0,1,ysize);
	if(arrow==RDEditAudio::Left) {
	  p->drawImage(prev,apos-5,edit_left_image,prev,apos-5,11,25);
	  p->drawImage(prev,ysize-apos-5,edit_left_image,prev,ysize-apos-5,
		       11,25);
	  p->fillRect(xsize,0,10,ysize,QBrush(backgroundColor()));
	}
	if(arrow==RDEditAudio::Right) {
	  p->drawImage(prev-11,apos-5,edit_left_image,prev-11,apos-5,11,25);
	  p->drawImage(prev-11,ysize-apos-5,edit_left_image,prev-11,ysize-apos-5,
		       11,25);
	  p->fillRect(-10,0,10,ysize,QBrush(backgroundColor()));
	}
      }
      if(chan==1) {
	p->drawImage(prev,0,edit_right_image,prev,0,1,ysize);
	if(arrow==RDEditAudio::Left) {
	  p->drawImage(prev,apos-5,edit_right_image,prev,apos-5,11,25);
	  p->drawImage(prev,ysize-apos-5,edit_right_image,prev,
		       ysize-apos-5,11,25);
	  p->fillRect(xsize,0,10,ysize,QBrush(backgroundColor()));
	}
	if(arrow==RDEditAudio::Right) {
	  p->drawImage(prev-11,apos-5,edit_right_image,prev-11,apos-5,11,25);
	  p->drawImage(prev-11,ysize-apos-5,edit_right_image,prev-11,
		       ysize-apos-5,11,25);
	  p->fillRect(-10,0,10,ysize,QBrush(backgroundColor()));
	}
      }
    }
    p->end();
    delete p;
  }
}


void RDEditAudio::DrawWave(int xsize,int ysize,int chan,QString label,
			 QPixmap *pix)
{
  unsigned offset;
  unsigned origin_x;
  int ref_line;

  if(edit_factor_x>1) {
    origin_x=(edit_hscroll->value()/(unsigned)edit_factor_x)*
      (unsigned)edit_factor_x;
  }
  else {
    origin_x=edit_hscroll->value();;
  }
  QPainter *p=new QPainter(pix);
  p->eraseRect(0,0,xsize,ysize);
  p->drawRect(0,0,xsize,ysize);

  int vert=ysize/2;
  double size_y=pow(10,(-((double)edit_gain)/20.0));
  ref_line=int(size_y*ysize*pow(10.0,-(double)REFERENCE_LEVEL/2000.0-
			      (double)edit_gain_control->value()/2000.0)/2.0);

  //
  // Grayed-Out Area
  //
  for(int i=1;i<(xsize-3);i++) {
    offset=(unsigned)((double)i*edit_factor_x*
		      (double)edit_channels+
		      (double)edit_channels*
		      (double)origin_x);
    if(offset>=edit_peaks->energySize()) {
      //    if(offset>=edit_wave->energySize()) {
      p->fillRect(i,1,xsize-i,ysize-2,
		  QBrush(QColor(EDITAUDIO_HIGHLIGHT_COLOR)));
      continue;
    }
  }

  //
  // Reference Level Lines
  //
  p->setPen(QColor(red));
  p->moveTo(0,vert+ref_line);
  p->lineTo(xsize,vert+ref_line);
  p->moveTo(0,vert-ref_line);
  p->lineTo(xsize,vert-ref_line);

  p->translate(1,ysize/2);
  if(edit_peaks->energySize()>0) {
    //  if(edit_wave->energySize()>0) {

    //
    // Time Tick Marks
    //
    p->setFont(QFont("Helvetica",8,QFont::Normal));
    for(unsigned i=0;i<2*edit_peaks->energySize();
	i+=(int)(edit_factor_x*(double)edit_sample_rate/576.0)) {
      offset=(int)((double)(i-origin_x)/edit_factor_x);
      if((offset>0)&&(offset<(EDITAUDIO_WAVEFORM_WIDTH-2))) {
	p->setPen(QColor(green));
	p->moveTo(offset,-ysize/2);
	p->lineTo(offset,ysize/2);
	p->setPen(QColor(red));
	p->drawText(offset+3,ysize/2-4,
		    RDGetTimeLength((int)((1152000.0*(double)i)/
	       		  (double)edit_sample_rate+1000.0),
				   false,edit_factor_x<0.5));
      }
    }
    
    double size_y=pow(10,(-((double)edit_gain)/20.0));

    //
    // Waveform
    //
    p->setPen(QColor(EDITAUDIO_WAVEFORM_COLOR));
    p->setBrush(QColor(EDITAUDIO_WAVEFORM_COLOR));
    edit_wave_array->setPoint(0,0,0);
    for(int i=1;i<(xsize-3);i++) {
      offset=(unsigned)((double)i*edit_factor_x*
			(double)edit_channels+
			(double)edit_channels*
			(double)origin_x+(double)chan);
      if(offset<edit_peaks->energySize()) {
	edit_wave_array->setPoint(i,i+(int)((double)chan/(2.0*edit_factor_x)),
				  (int)(edit_peaks->energy(offset)*ysize*
					size_y/65534));
      }
      else {
	edit_wave_array->setPoint(i,i,0);
      }
    }
    edit_wave_array->setPoint(xsize-3,xsize-3,0);
    p->drawPolygon(*edit_wave_array);

    edit_wave_array->setPoint(0,0,0);
    for(int i=1;i<(xsize-3);i++) {
      offset=(unsigned)((double)i*edit_factor_x*
			(double)edit_channels+
			(double)edit_channels*
			(double)origin_x+(double)chan);
      if(offset<edit_peaks->energySize()) {
	edit_wave_array->setPoint(i,i+(int)((double)chan/(2.0*edit_factor_x)),
			      (int)(-edit_peaks->energy(offset)*
				    ysize*size_y/65534));
      }
      else {
	edit_wave_array->setPoint(i,i,0);
      }
    }
    edit_wave_array->setPoint(xsize-3,xsize-3,0);
    p->drawPolygon(*edit_wave_array);

    p->setPen(QColor(red));
    if(!label.isEmpty()) {
      p->setFont(QFont("Helvetica",24,QFont::Normal));
      p->drawText(10,28-ysize/2,label);
    }
    p->setPen(QColor(black));
    p->moveTo(0,0);
    p->lineTo(xsize-3,0);
  }
  else {
    p->setFont(QFont("Helvetica",24,QFont::Bold));
    p->drawText(270,0,"No Energy Data");
  }
  p->end();
  delete p;
}


void RDEditAudio::DrawPointers()
{
  edit_arrow_cursor=new QCursor(Qt::ArrowCursor);
  edit_cross_cursor=new QCursor(Qt::CrossCursor);
}


void RDEditAudio::CenterDisplay()
{
  edit_hscroll->
    setValue((int)(edit_cursors[RDEditAudio::Play]-EDITAUDIO_WAVEFORM_WIDTH/2*edit_factor_x));
}


int RDEditAudio::GetTime(int samples)
{
  if(samples>(int)edit_sample_length) {
    return -1;
  }
  return (int)(1000.0f*(double)samples/(double)edit_sample_rate);
}
