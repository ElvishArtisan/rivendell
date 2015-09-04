// rdedit_audio.cpp
//
// Edit Rivendell Audio Markers
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdcut.h>
#include <rdedit_audio.h>
#include <rdaudioinfo.h>
#include <rdtrimaudio.h>
#include <rdwavepainter.h>

RDEditAudio::RDEditAudio(RDCart *cart,QString cut_name,RDCae *cae,RDUser *user,
			 RDStation *station,RDConfig *config,int card,
			 int port,int preroll,
			 int trim_level,QWidget *parent)
  : QDialog(parent,"",true)
{
  trans_preroll=preroll;
  edit_station=station;
  edit_config=config;
  edit_user=user;
  edit_gain_mode=RDEditAudio::GainNone;
  edit_gain_count=0;

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

  //
  // Audio Cut
  //
  edit_cut=new RDCut(cut_name);

  //
  // Waveform
  //
  switch(edit_cut->channels()) {
  case 1:
    edit_waveform[0]=new RDMarkerWaveform(edit_cut,user,station,config,
					  RDWavePainter::Mono,this);
    edit_waveform[1]=new RDMarkerWaveform(edit_cut,user,station,config,
					  RDWavePainter::Mono,this);
    edit_waveform[1]->hide();
    break;

  case 2:
    edit_waveform[0]=new RDMarkerWaveform(edit_cut,user,station,config,
					  RDWavePainter::Left,this);
    edit_waveform[1]=new RDMarkerWaveform(edit_cut,user,station,config,
					  RDWavePainter::Right,this);
    break;
  }
  connect(edit_waveform[0],SIGNAL(clicked(int)),this,
	  SLOT(waveformClickedData(int)));
  connect(edit_waveform[1],SIGNAL(clicked(int)),this,
	  SLOT(waveformClickedData(int)));
  edit_waveform_scroll=new QScrollBar(QScrollBar::Horizontal,this);

  //
  // Audio Transport Controls
  //
  edit_marker_transport=
    new RDMarkerTransport(edit_cut,cae,card,port,preroll,this);
  for(int i=0;i<2;i++) {
    connect(edit_marker_transport,SIGNAL(positionChanged(int)),
	    edit_waveform[i],SLOT(setPlayCursor(int)));
    connect(edit_waveform[i],SIGNAL(clicked(int)),
	    edit_marker_transport,SLOT(setPosition(int)));
  }

  //
  // Marker Widgets
  //
  edit_marker_widget[RDMarkerWaveform::Play]=
    new RDMarkerWidget(tr("Play"),Qt::black,this);
  edit_marker_widget[RDMarkerWaveform::Play]->hide();  // "Virtual" for playback operations

  edit_marker_widget[RDMarkerWaveform::Start]=
    new RDMarkerWidget(tr("Cut\nStart"),RD_START_END_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::End]=
    new RDMarkerWidget(tr("Cut\nEnd"),RD_START_END_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::FadeUp]=
    new RDMarkerWidget(tr("Fade\nUp"),RD_FADE_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::FadeDown]=
    new RDMarkerWidget(tr("Fade\nDown"),RD_FADE_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::TalkStart]=
    new RDMarkerWidget(tr("Talk\nStart"),RD_TALK_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::TalkEnd]=
    new RDMarkerWidget(tr("Talk\nEnd"),RD_TALK_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::SegueStart]=
    new RDMarkerWidget(tr("Segue\nStart"),RD_SEGUE_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::SegueEnd]=
    new RDMarkerWidget(tr("Segue\nEnd"),RD_SEGUE_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::HookStart]=
    new RDMarkerWidget(tr("Hook\nStart"),RD_HOOK_MARKER_COLOR,this);

  edit_marker_widget[RDMarkerWaveform::HookEnd]=
    new RDMarkerWidget(tr("Hook\nEnd"),RD_HOOK_MARKER_COLOR,this);

  QSignalMapper *enabled_mapper=new QSignalMapper(this);
  connect(enabled_mapper,SIGNAL(mapped(int)),
	  this,SLOT(markerButtonEnabledData(int)));
  QSignalMapper *value_mapper=new QSignalMapper(this);
  connect(value_mapper,SIGNAL(mapped(int)),
	  this,SLOT(markerValueChangedData(int)));
 
  for(int i=0;i<RDMarkerWaveform::LastMarker;i++) {
    enabled_mapper->
      setMapping(edit_marker_widget[i],(int)i);
    connect(edit_marker_widget[i],SIGNAL(selectionChanged()),
	    enabled_mapper,SLOT(map()));
    value_mapper->
      setMapping(edit_marker_widget[i],(int)i);
    connect(edit_marker_widget[i],SIGNAL(valueChanged()),
	    value_mapper,SLOT(map()));
  }

  //
  // AutoTrim Buttons
  //
  edit_trim_box=new QSpinBox(this);
  edit_trim_box->setAcceptDrops(false);
  edit_trim_box->setValidator(0);
  edit_trim_box->setSuffix(tr(" dB"));
  edit_trim_box->setRange(-99,0);
  edit_trim_box->
    setValue((trim_level-REFERENCE_LEVEL)/100);
  edit_trim_label=new QLabel(tr("Threshold"),this);
  edit_trim_label->setAlignment(AlignHCenter);
  edit_trim_label->setFont(QFont(small_font));
  edit_trim_start_button=new QPushButton(this);
  edit_trim_start_button->
    setPalette(QPalette(QColor(RD_START_END_MARKER_COLOR),backgroundColor()));
  edit_trim_start_button->setFont(button_font);
  edit_trim_start_button->setText(tr("Trim\nStart"));
  connect(edit_trim_start_button,SIGNAL(clicked()),this,SLOT(trimHeadData()));

  edit_trim_end_button=new QPushButton(this);
  edit_trim_end_button->
    setPalette(QPalette(QColor(RD_START_END_MARKER_COLOR),backgroundColor()));
  edit_trim_end_button->setFont(button_font);
  edit_trim_end_button->setText(tr("Trim\nEnd"));
  connect(edit_trim_end_button,SIGNAL(clicked()),this,SLOT(trimTailData()));

  //
  // Cut Gain Control
  //
  edit_gain_control=new QRangeControl();
  edit_gain_control->setRange(-1000,1000);
  edit_gain_control->setSteps(10,10);
  edit_gain_edit=new RDMarkerEdit(this);
  edit_gain_edit->setAcceptDrops(false);
  connect(edit_gain_edit,SIGNAL(returnPressed()),this,SLOT(gainChangedData()));
  edit_gain_label=new QLabel(tr("Cut Gain"),this);
  edit_gain_label->setAlignment(AlignHCenter);
  edit_gain_label->setFont(QFont(small_font));
  gain_up_button=new RDTransportButton(RDTransportButton::Up,this);
  gain_up_button->off();
  connect(gain_up_button,SIGNAL(pressed()),this,SLOT(gainUpPressedData()));
  connect(gain_up_button,SIGNAL(released()),this,SLOT(gainReleasedData()));
  
  gain_down_button=new RDTransportButton(RDTransportButton::Down,this);
  gain_down_button->off();
  connect(gain_down_button,SIGNAL(pressed()),this,SLOT(gainDownPressedData()));
  connect(gain_down_button,SIGNAL(released()),this,SLOT(gainReleasedData()));
  edit_gain_timer=new QTimer(this);
  connect(edit_gain_timer,SIGNAL(timeout()),this,SLOT(gainTimerData()));

  //
  // Marker Remove Button
  //
  edit_remove_button=new RDPushButton(this);
  edit_remove_button->setFlashPeriod(EDITAUDIO_BUTTON_FLASH_PERIOD);
  edit_remove_button->setFont(button_font);
  edit_remove_button->setText(tr("Remove\nMarker"));
  edit_remove_button->setToggleButton(true);
  edit_remove_button->setFlashColor(QColor(EDITAUDIO_REMOVE_FLASH_COLOR));
  connect(edit_remove_button,SIGNAL(clicked()),this,SLOT(removeButtonData()));

  //
  // Segue Fade Box
  //
  edit_overlap_box=new QCheckBox(this);
  edit_overlap_label=
    new QLabel(edit_overlap_box,tr("No Fade on Segue Out"),this);
  edit_overlap_label->setFont(small_font);
  edit_overlap_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  
  //
  // Amplitude Buttons
  //
  edit_amp_label=new QLabel(this);
  edit_amp_label->setAlignment(AlignHCenter|AlignVCenter);
  edit_amp_label->setFont(button_font);
  edit_amp_label->setText(tr("Amplitude"));

  edit_ampup_button=new RDTransportButton(RDTransportButton::Up,this);
  edit_ampup_button->setFont(QFont("Helvetica",12,QFont::Bold));
  edit_ampup_button->setText(tr("Zoom\nIn"));
  connect(edit_ampup_button,SIGNAL(clicked()),edit_waveform[0],SLOT(ampUp()));
  connect(edit_ampup_button,SIGNAL(clicked()),edit_waveform[1],SLOT(ampUp()));

  edit_ampdown_button=new RDTransportButton(RDTransportButton::Down,this);
  edit_ampdown_button->setFont(QFont("Helvetica",12,QFont::Bold));
  edit_ampdown_button->setText(tr("Zoom\nOut"));
  connect(edit_ampdown_button,SIGNAL(clicked()),
	  edit_waveform[0],SLOT(ampDown()));
  connect(edit_ampdown_button,SIGNAL(clicked()),
	  edit_waveform[1],SLOT(ampDown()));

  //
  // Time Buttons
  //
  edit_time_label=new QLabel(this);
  edit_time_label->setAlignment(AlignHCenter|AlignVCenter);
  edit_time_label->setFont(button_font);
  edit_time_label->setText(tr("Time"));

  edit_fullin_button=new QPushButton(this);
  edit_fullin_button->setFont(button_font);
  edit_fullin_button->setText(tr("Full\nIn"));
  connect(edit_fullin_button,SIGNAL(clicked()),edit_waveform[0],SLOT(fullIn()));
  connect(edit_fullin_button,SIGNAL(clicked()),edit_waveform[1],SLOT(fullIn()));

  edit_zoomin_button=new RDTransportButton(RDTransportButton::Up,this);
  edit_zoomin_button->setFont(button_font);
  edit_zoomin_button->setText(tr("Zoom\nIn"));
  connect(edit_zoomin_button,SIGNAL(clicked()),edit_waveform[0],SLOT(zoomIn()));
  connect(edit_zoomin_button,SIGNAL(clicked()),edit_waveform[1],SLOT(zoomIn()));

  edit_zoomout_button=new RDTransportButton(RDTransportButton::Down,this);
  edit_zoomout_button->setFont(button_font);
  edit_zoomout_button->setText(tr("Zoom\nOut"));
  connect(edit_zoomout_button,SIGNAL(clicked()),
	  edit_waveform[0],SLOT(zoomOut()));
  connect(edit_zoomout_button,SIGNAL(clicked()),
	  edit_waveform[1],SLOT(zoomOut()));

  edit_fullout_button=new QPushButton(this);
  edit_fullout_button->setFont(button_font);
  edit_fullout_button->setText(tr("Full\nOut"));
  connect(edit_fullout_button,SIGNAL(clicked()),
	  edit_waveform[0],SLOT(fullOut()));
  connect(edit_fullout_button,SIGNAL(clicked()),
	  edit_waveform[1],SLOT(fullOut()));

  //
  // GoTo Buttons
  //
  edit_goto_label=new QLabel(tr("Goto"),this);
  edit_goto_label->setAlignment(AlignHCenter|AlignVCenter);
  edit_goto_label->setFont(button_font);

  edit_goto_cursor_button=new QPushButton(this);
  edit_goto_cursor_button->setFont(button_font);
  edit_goto_cursor_button->setText(tr("Cursor"));
  connect(edit_goto_cursor_button,SIGNAL(clicked()),
	  this,SLOT(gotoCursorData()));

  edit_goto_home_button=new QPushButton(this);
  edit_goto_home_button->setFont(button_font);
  edit_goto_home_button->setText(tr("Home"));
  connect(edit_goto_home_button,SIGNAL(clicked()),this,SLOT(gotoHomeData()));

  edit_goto_end_button=new QPushButton(this);
  edit_goto_end_button->setFont(button_font);
  edit_goto_end_button->setText(tr("End"));
  connect(edit_goto_end_button,SIGNAL(clicked()),this,SLOT(gotoEndData()));

  //
  //  Save Button
  //
  edit_save_button=new QPushButton(this);
  edit_save_button->setDefault(true);
  edit_save_button->setFont(button_font);
  edit_save_button->setText(tr("&Save"));
  connect(edit_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setDefault(true);
  edit_cancel_button->setFont(button_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  edit_marker_widget[RDMarkerWaveform::Start]->setValue(edit_cut->startPoint());
  edit_marker_widget[RDMarkerWaveform::Start]->setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::Start]->
    setRange(NULL,edit_marker_widget[RDMarkerWaveform::End]);

  edit_marker_widget[RDMarkerWaveform::End]->setValue(edit_cut->endPoint());
  edit_marker_widget[RDMarkerWaveform::End]->setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::End]->
    setRange(edit_marker_widget[RDMarkerWaveform::Start],NULL);

  edit_marker_widget[RDMarkerWaveform::TalkStart]->
    setValue(edit_cut->talkStartPoint());
  edit_marker_widget[RDMarkerWaveform::TalkStart]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::TalkStart]->
    setRange(edit_marker_widget[RDMarkerWaveform::Start],
	     edit_marker_widget[RDMarkerWaveform::TalkEnd]);

  edit_marker_widget[RDMarkerWaveform::TalkEnd]->
    setValue(edit_cut->talkEndPoint());
  edit_marker_widget[RDMarkerWaveform::TalkEnd]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::TalkEnd]->
    setRange(edit_marker_widget[RDMarkerWaveform::TalkStart],
	     edit_marker_widget[RDMarkerWaveform::End]);

  edit_marker_widget[RDMarkerWaveform::SegueStart]->
    setValue(edit_cut->segueStartPoint());
  edit_marker_widget[RDMarkerWaveform::SegueStart]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::SegueStart]->
    setRange(edit_marker_widget[RDMarkerWaveform::Start],
	     edit_marker_widget[RDMarkerWaveform::SegueEnd]);

  edit_marker_widget[RDMarkerWaveform::SegueEnd]->
    setValue(edit_cut->segueEndPoint());
  edit_marker_widget[RDMarkerWaveform::SegueEnd]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::SegueEnd]->
    setRange(edit_marker_widget[RDMarkerWaveform::SegueStart],
	     edit_marker_widget[RDMarkerWaveform::End]);


  edit_marker_widget[RDMarkerWaveform::HookStart]->
    setValue(edit_cut->hookStartPoint());
  edit_marker_widget[RDMarkerWaveform::HookStart]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::HookStart]->
    setRange(edit_marker_widget[RDMarkerWaveform::Start],
	     edit_marker_widget[RDMarkerWaveform::HookEnd]);

  edit_marker_widget[RDMarkerWaveform::HookEnd]->
    setValue(edit_cut->hookEndPoint());
  edit_marker_widget[RDMarkerWaveform::HookEnd]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::HookEnd]->
    setRange(edit_marker_widget[RDMarkerWaveform::HookStart],
	     edit_marker_widget[RDMarkerWaveform::End]);

  edit_marker_widget[RDMarkerWaveform::FadeUp]->
    setValue(edit_cut->fadeupPoint());
  edit_marker_widget[RDMarkerWaveform::FadeUp]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::FadeUp]->
    setRange(edit_marker_widget[RDMarkerWaveform::Start],
	     edit_marker_widget[RDMarkerWaveform::FadeDown]);

  edit_marker_widget[RDMarkerWaveform::FadeDown]->
    setValue(edit_cut->fadedownPoint());
  edit_marker_widget[RDMarkerWaveform::FadeDown]->
    setRange(0,edit_cut->length());
  edit_marker_widget[RDMarkerWaveform::FadeDown]->
    setRange(edit_marker_widget[RDMarkerWaveform::FadeUp],
	     edit_marker_widget[RDMarkerWaveform::End]);

  edit_gain_control->setValue(edit_cut->playGain());
  gainChangedData();
}


RDEditAudio::~RDEditAudio()
{
  delete edit_cancel_button;
  delete edit_save_button;
  for(int i=0;i<2;i++) {
    delete edit_waveform[i];
  }
  delete edit_cut;
  delete edit_marker_transport;
  for(int i=0;i<RDMarkerWaveform::LastMarker;i++) {
    delete edit_marker_widget[i];
  }
}


QSize RDEditAudio::sizeHint() const
{
  return QSize(EDITAUDIO_WIDGET_WIDTH,EDITAUDIO_WIDGET_HEIGHT);
} 


QSizePolicy RDEditAudio::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDEditAudio::resizeEvent(QResizeEvent *e)
{
  //
  // Waveforms
  //
  switch(edit_cut->channels()) {
  case 1:
    edit_waveform[0]->setGeometry(0,10,edit_waveform[0]->sizeHint().width(),
			       edit_waveform[0]->sizeHint().height());
    break;

  case 2:
    edit_waveform[0]->setGeometry(0,10,edit_waveform[0]->sizeHint().width(),
			       edit_waveform[0]->sizeHint().height()/2);
    edit_waveform[1]->setGeometry(0,10+edit_waveform[0]->sizeHint().height()/2,
				  edit_waveform[0]->sizeHint().width(),
				  edit_waveform[0]->sizeHint().height()/2);
    break;
  }
  connect(edit_waveform[0],SIGNAL(viewportWidthChanged(int)),
	  this,SLOT(viewportWidthChangedData(int)));
  edit_waveform_scroll->setGeometry(10,edit_waveform[0]->sizeHint().height()+10,
				    edit_waveform[0]->sizeHint().width()-20,20);
  edit_waveform_scroll->setMinValue(0);
  edit_waveform_scroll->setMaxValue(edit_cut->length());
  connect(edit_waveform_scroll,SIGNAL(valueChanged(int)),
	  edit_waveform[0],SLOT(setViewportStart(int)));
  connect(edit_waveform_scroll,SIGNAL(valueChanged(int)),
	  edit_waveform[1],SLOT(setViewportStart(int)));
  viewportWidthChangedData(edit_waveform[0]->viewportWidth());

  //
  // Audio Transport Controls
  //
  edit_marker_transport->
    setGeometry(11,30+edit_waveform[0]->sizeHint().height(),
		edit_waveform[0]->sizeHint().width()-20,92);

  //
  // Marker Widgets
  //
  edit_marker_widget[RDMarkerWaveform::Start]->setGeometry(20,485,165,45);
  edit_marker_widget[RDMarkerWaveform::End]->setGeometry(20,530,165,45);
  edit_marker_widget[RDMarkerWaveform::FadeUp]->setGeometry(20,585,165,45);
  edit_marker_widget[RDMarkerWaveform::FadeDown]->setGeometry(20,630,165,45);
  edit_marker_widget[RDMarkerWaveform::TalkStart]->setGeometry(195,585,165,45);
  edit_marker_widget[RDMarkerWaveform::TalkEnd]->setGeometry(195,630,165,45);
  edit_marker_widget[RDMarkerWaveform::SegueStart]->setGeometry(370,585,165,45);
  edit_marker_widget[RDMarkerWaveform::SegueEnd]->setGeometry(370,630,165,45);
  edit_marker_widget[RDMarkerWaveform::HookStart]->setGeometry(545,585,165,45);
  edit_marker_widget[RDMarkerWaveform::HookEnd]->setGeometry(545,630,165,45);

  //
  // Autotrim Buttons
  //
  edit_trim_box->setGeometry(263,529,65,21);
  edit_trim_label->setGeometry(258,513,65,15);
  edit_trim_start_button->setGeometry(195,485,66,45);
  edit_trim_end_button->setGeometry(195,530,66,45);

  //
  // Gain Control Buttons
  //
  edit_gain_edit->setGeometry(410,529,65,21);
  edit_gain_label->setGeometry(400,513,65,15);
  gain_up_button->setGeometry(342,485,66,45);
  gain_down_button->setGeometry(342,530,66,45);

  //
  // Marker Remove Button
  //
  edit_remove_button->setGeometry(490,510,66,45);

  //
  // Segue Fade Box
  //
  edit_overlap_box->setGeometry(570,515,15,15);
  edit_overlap_label->setGeometry(590,513,130,20);


  //
  // Amplitude Buttons
  //
  edit_amp_label->setGeometry(742,5,80,16);
  edit_ampup_button->setGeometry(747,22,70,50);
  edit_ampdown_button->setGeometry(747,72,70,50);

  //
  // Time Zoom Buttons
  //
  edit_time_label->setGeometry(760,143,40,16);
  edit_fullin_button->setGeometry(747,160,70,50);
  edit_zoomin_button->setGeometry(747,212,70,50);
  edit_zoomout_button->setGeometry(747,262,70,50);
  edit_fullout_button->setGeometry(747,312,70,50);

  //
  // Goto Buttons
  //
  edit_goto_label->setGeometry(760,378,40,16);
  edit_goto_cursor_button->setGeometry(747,393,70,50);
  edit_goto_home_button->setGeometry(747,443,70,50);
  edit_goto_end_button->setGeometry(747,493,70,50);

  //
  // Save/Cancel Buttons
  //
  edit_save_button->
    setGeometry(EDITAUDIO_WIDGET_WIDTH-90,EDITAUDIO_WIDGET_HEIGHT-120,80,50);
  edit_cancel_button->
    setGeometry(EDITAUDIO_WIDGET_WIDTH-90,EDITAUDIO_WIDGET_HEIGHT-60,80,50);
}


void RDEditAudio::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  //
  // Highlights
  //
  p->drawRect(739,12,85,120);    // Amplitude Buttons
  p->drawRect(739,150,85,220);   // Time Buttons
  p->drawRect(739,385,85,165);   // Goto Buttons

  //
  // Marker Control Area
  //
  p->drawRect(11,130+edit_waveform[0]->sizeHint().height(),717,197);

  p->end();
  delete p;
}


void RDEditAudio::viewportWidthChangedData(int msecs)
{
  edit_waveform_scroll->setLineStep(msecs/10);
  edit_waveform_scroll->setPageStep(msecs);  
}


void RDEditAudio::waveformClickedData(int msecs)
{
  for(int i=1;i<RDMarkerWaveform::LastMarker;i++) {
    if(edit_marker_widget[i]->isSelected()) {
      edit_marker_widget[i]->setValue(msecs);
    }
  }
}


void RDEditAudio::markerButtonEnabledData(int id)
{
  for(int i=1;i<RDMarkerWaveform::LastMarker;i++) {
    if(i!=id) {
      edit_marker_widget[i]->setSelected(false);
    }
  }
  if(edit_marker_widget[id]->isSelected()) {
    switch(id) {
    case RDMarkerWaveform::Start:
    case RDMarkerWaveform::TalkStart:
    case RDMarkerWaveform::SegueStart:
    case RDMarkerWaveform::HookStart:
      edit_marker_transport->setActiveMarker((RDMarkerWaveform::CuePoints)id,
					     edit_marker_widget[id]->value(),
					     edit_marker_widget[id+1]->value());
      break;

    case RDMarkerWaveform::End:
    case RDMarkerWaveform::TalkEnd:
    case RDMarkerWaveform::SegueEnd:
    case RDMarkerWaveform::HookEnd:
      edit_marker_transport->
	setActiveMarker((RDMarkerWaveform::CuePoints)id,
			edit_marker_widget[id]->value()-trans_preroll,
			edit_marker_widget[id]->value());
      break;

    case RDMarkerWaveform::FadeUp:
      edit_marker_transport->
	setActiveMarker((RDMarkerWaveform::CuePoints)id,
			edit_marker_widget[RDMarkerWaveform::Start]->value(),
			edit_marker_widget[id]->value());
      break;

    case RDMarkerWaveform::FadeDown:
      edit_marker_transport->
	setActiveMarker((RDMarkerWaveform::CuePoints)id,
			edit_marker_widget[id]->value(),
			edit_marker_widget[RDMarkerWaveform::End]->value());
      break;

    case RDMarkerWaveform::Play:
    case RDMarkerWaveform::LastMarker:
      break;
    }
  }
  else {
      edit_marker_transport->
	setActiveMarker(RDMarkerWaveform::Play,
			edit_marker_widget[RDMarkerWaveform::Start]->value(),
			edit_marker_widget[RDMarkerWaveform::End]->value());
  }
}


void RDEditAudio::markerValueChangedData(int id)
{
  int value=edit_marker_widget[id]->value();

  edit_waveform[0]->setCursor((RDMarkerWaveform::CuePoints)id,value);
  edit_waveform[1]->setCursor((RDMarkerWaveform::CuePoints)id,value);

  switch((RDMarkerWaveform::CuePoints)id) {
  case RDMarkerWaveform::TalkStart:
  case RDMarkerWaveform::SegueStart:
  case RDMarkerWaveform::HookStart:
    if(value<0) {
      if(edit_marker_widget[id+1]->value()>=0) {
	edit_marker_widget[id+1]->setValue(-1);
      }
      SetDeleteMode(false);
    }
    else {
      if(edit_marker_widget[id+1]->value()<0) {
	edit_marker_widget[id+1]->
	  setValue(edit_marker_widget[id+1]->highLimit());
      }
    }
    break;

  case RDMarkerWaveform::TalkEnd:
  case RDMarkerWaveform::SegueEnd:
  case RDMarkerWaveform::HookEnd:
    if(value<0) {
      if(edit_marker_widget[id-1]->value()>=0) {
	edit_marker_widget[id-1]->setValue(-1);
      }
      SetDeleteMode(false);
    }
    else {
      if(edit_marker_widget[id-1]->value()<0) {
	edit_marker_widget[id-1]->
	  setValue(edit_marker_widget[id-1]->lowLimit());
      }
    }
    break;

  case RDMarkerWaveform::Start:
    edit_marker_transport->setStartPosition(value);
    edit_marker_transport->
      setLength(edit_marker_widget[RDMarkerWaveform::End]->value()-
		edit_marker_widget[RDMarkerWaveform::Start]->value());
    for(int i=3;i<RDMarkerWaveform::LastMarker;i++) {
      if((edit_marker_widget[i]->value()>=0)&&
	 (edit_marker_widget[i]->value()<value)) {
	edit_marker_widget[i]->setValue(value);
      }
    }
    break;

  case RDMarkerWaveform::End:
    edit_marker_transport->setEndPosition(value);
    edit_marker_transport->
      setLength(edit_marker_widget[RDMarkerWaveform::End]->value()-
		edit_marker_widget[RDMarkerWaveform::Start]->value());
    for(int i=3;i<RDMarkerWaveform::LastMarker;i++) {
      if((edit_marker_widget[i]->value()>=0)&&
	 (edit_marker_widget[i]->value()>value)) {
	edit_marker_widget[i]->setValue(value);
      }
    }
    break;

  case RDMarkerWaveform::FadeUp:
  case RDMarkerWaveform::FadeDown:
  case RDMarkerWaveform::Play:
  case RDMarkerWaveform::LastMarker:
    break;
  }
}


void RDEditAudio::trimHeadData()
{
  RDTrimAudio::ErrorCode conv_err;
  RDTrimAudio *conv=new RDTrimAudio(edit_station,edit_config,this);
  conv->setCartNumber(edit_cut->cartNumber());
  conv->setCutNumber(edit_cut->cutNumber());
  conv->setTrimLevel(100*edit_trim_box->value());
  switch(conv_err=conv->runTrim(edit_user->name(),edit_user->password())) {
  case RDTrimAudio::ErrorOk:
    if(conv->startPoint()>=0) {
      edit_marker_widget[RDMarkerWaveform::Start]->setValue(conv->startPoint());
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
  RDTrimAudio::ErrorCode conv_err;
  RDTrimAudio *conv=new RDTrimAudio(edit_station,edit_config,this);
  conv->setCartNumber(edit_cut->cartNumber());
  conv->setCutNumber(edit_cut->cutNumber());
  conv->setTrimLevel(100*edit_trim_box->value());
  switch(conv_err=conv->runTrim(edit_user->name(),edit_user->password())) {
  case RDTrimAudio::ErrorOk:
    if(conv->endPoint()>=0) {
      edit_marker_widget[RDMarkerWaveform::End]->setValue(conv->endPoint());
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
  edit_marker_transport->setGain(edit_gain_control->value());
  edit_waveform[0]->setReferenceLevel(edit_gain_control->value());
  edit_waveform[1]->setReferenceLevel(edit_gain_control->value());
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

  case RDEditAudio::GainNone:
    break;
  }
  str=QString(tr("dB"));
  edit_gain_edit->setText(QString().sprintf("%4.1f %s",
			     (double)edit_gain_control->value()/100.0,
			     (const char *)str));
  edit_marker_transport->setGain(edit_gain_control->value());
  edit_waveform[0]->setReferenceLevel(edit_gain_control->value());
  edit_waveform[1]->setReferenceLevel(edit_gain_control->value());
}


void RDEditAudio::removeButtonData()
{
  SetDeleteMode(!edit_marker_widget[3]->deleteModeActive());
}


void RDEditAudio::gotoCursorData()
{
  edit_waveform_scroll->setValue(edit_waveform[0]->playCursor()-
				 edit_waveform_scroll->pageStep()/2);
}


void RDEditAudio::gotoHomeData()
{
  edit_waveform_scroll->setValue(0);
}


void RDEditAudio::gotoEndData()
{
  edit_waveform_scroll->setValue(edit_waveform_scroll->maxValue()-
				 edit_waveform_scroll->pageStep()+
				 edit_waveform_scroll->lineStep());
}


void RDEditAudio::saveData()
{
  done(0);
}


void RDEditAudio::cancelData()
{
  done(1);
}


void RDEditAudio::SetDeleteMode(bool state)
{
  for(int i=1;i<RDMarkerWaveform::LastMarker;i++) {
    edit_marker_widget[i]->setSelected(false);
  }
  for(int i=3;i<RDMarkerWaveform::LastMarker;i++) {
    edit_marker_widget[i]->setDeleteMode(state);
  }
  edit_remove_button->setFlashingEnabled(state);
  edit_remove_button->setDown(state);
}


RDMarkerWaveform::CuePoints RDEditAudio::CurrentMarker() const
{
  RDMarkerWaveform::CuePoints ret=RDMarkerWaveform::Play;

  for(int i=0;i<RDMarkerWaveform::LastMarker;i++) {
    if(edit_marker_widget[i]->isSelected()) {
      ret=(RDMarkerWaveform::CuePoints)i;
    }
  }

  return ret;
}
