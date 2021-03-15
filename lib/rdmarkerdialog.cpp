// rdmarkerdialog.cpp
//
// Rivendell Audio Marker Editor
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include "rdaudioinfo.h"
#include "rdmarkerdialog.h"
#include "rdmixer.h"

RDMarkerDialog::RDMarkerDialog(const QString &caption,int card,int port,
			       QWidget *parent)
  : RDDialog(parent)
{
  d_caption=caption;

  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  d_marker_view=new RDMarkerView(sizeHint().width()-104,374,this);

  /**************************************************************************
   * Waveform Section
   **************************************************************************/
  //
  // Amplitude
  //
  d_amplitude_box=new QGroupBox(tr("Amplitude"),this);
  d_amplitude_box->setFont(labelFont());
  d_amplitude_up_button=
    new RDTransportButton(RDTransportButton::Up,d_amplitude_box);
  connect(d_amplitude_up_button,SIGNAL(clicked()),
	  this,SLOT(amplitudeUpData()));
  d_amplitude_down_button=
    new RDTransportButton(RDTransportButton::Down,d_amplitude_box);
  connect(d_amplitude_down_button,SIGNAL(clicked()),
	  this,SLOT(amplitudeDownData()));

  //
  // Time
  //
  d_time_box=new QGroupBox(tr("Time"),this);
  d_time_box->setFont(labelFont());
  d_time_fullin_button=new QPushButton(tr("Full\nIn"),d_time_box);
  d_time_fullin_button->setFont(buttonFont());
  connect(d_time_fullin_button,SIGNAL(clicked()),this,SLOT(timeFullInData()));

  d_time_in_button=new RDTransportButton(RDTransportButton::Up,d_time_box);
  connect(d_time_in_button,SIGNAL(clicked()),this,SLOT(timeInData()));

  d_time_out_button=new RDTransportButton(RDTransportButton::Down,d_time_box);
  connect(d_time_out_button,SIGNAL(clicked()),this,SLOT(timeOutData()));

  d_time_fullout_button=new QPushButton(tr("Full\nOut"),d_time_box);
  d_time_fullout_button->setFont(buttonFont());
  connect(d_time_fullout_button,SIGNAL(clicked()),
	  d_marker_view,SLOT(setMaximumShrinkFactor()));

  /**************************************************************************
   * Transport Section
   **************************************************************************/
  //
  // Time Counters
  //
  d_overall_label=new QLabel(tr("Position"),this);
  d_overall_label->setFont(subLabelFont());
  d_overall_label->setAlignment(Qt::AlignHCenter);
  d_overall_label->
    setPalette(QPalette(palette().color(QPalette::Background),
			QColor(RDMARKERDIALOG_HIGHLIGHT_COLOR)));
  d_overall_edit=new QLineEdit(this);
  d_overall_edit->setAcceptDrops(false);
  d_overall_edit->setReadOnly(true);

  d_region_edit_label=new QLabel("Region",this);
  d_region_edit_label->setFont(subLabelFont());
  d_region_edit_label->setAlignment(Qt::AlignHCenter);
  d_region_edit_label->
    setPalette(QPalette(palette().color(QPalette::Background),QColor(RDMARKERDIALOG_HIGHLIGHT_COLOR)));
  d_region_edit=new QLineEdit(this);
  d_region_edit->setAcceptDrops(false);
  d_region_edit->setReadOnly(true);

  d_size_label=new QLabel(tr("Length"),this);
  d_size_label->setFont(subLabelFont());
  d_size_label->setAlignment(Qt::AlignHCenter);
  d_size_label->
    setPalette(QPalette(palette().color(QPalette::Background),QColor(RDMARKERDIALOG_HIGHLIGHT_COLOR)));
  d_size_edit=new QLineEdit(this);
  d_size_edit->setAcceptDrops(false);
  d_size_edit->setReadOnly(true);

  //
  // Transport Buttons
  //
  d_play_cursor_button=
    new RDTransportButton(RDTransportButton::PlayBetween,this);
  d_play_cursor_button->setFocusPolicy(Qt::NoFocus);
  //  d_play_cursor_button->setEnabled((d_card>=0)&&(d_port>=0));
  //  connect(d_play_cursor_button,SIGNAL(clicked()),
  //	  this,SLOT(playCursorData()));

  d_play_start_button=
    new RDTransportButton(RDTransportButton::Play,this);
  d_play_start_button->setFocusPolicy(Qt::NoFocus);
  //  d_play_start_button->setEnabled((d_card>=0)&&(d_port>=0));
  //  connect(d_play_start_button,SIGNAL(clicked()),
  //	  this,SLOT(playStartData()));

  d_pause_button=new RDTransportButton(RDTransportButton::Pause,this);
  d_pause_button->setFocusPolicy(Qt::NoFocus);
  d_pause_button->setOnColor(QColor(Qt::red));
  //  d_pause_button->setEnabled((d_card>=0)&&(d_port>=0));
  //  connect(d_pause_button,SIGNAL(clicked()),this,SLOT(pauseData()));

  d_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  d_stop_button->setFocusPolicy(Qt::NoFocus);
  d_stop_button->on();
  d_stop_button->setOnColor(QColor(Qt::red));
  //  d_stop_button->setEnabled((d_card>=0)&&(d_port>=0));
  //  connect(d_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  d_loop_button=new RDTransportButton(RDTransportButton::Loop,this);
  d_loop_button->off();
  //  d_loop_button->setEnabled((d_card>=0)&&(d_port>=0));
  //  connect(d_loop_button,SIGNAL(clicked()),this,SLOT(loopData()));

  //
  // The Audio Meter
  //
  d_meter=new RDStereoMeter(this);
  d_meter->setSegmentSize(5);
  d_meter->setMode(RDSegMeter::Peak);
  //  d_meter_timer=new QTimer(this);
  //  connect(d_meter_timer,SIGNAL(timeout()),this,SLOT(meterData()));

  //
  // Marker Readouts
  //
  d_cut_readout=new RDMarkerReadout(RDMarkerHandle::CutStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_cut_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  d_cut_readout->setEnabled(true);

  d_talk_readout=new RDMarkerReadout(RDMarkerHandle::TalkStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_talk_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));

  d_segue_readout=new RDMarkerReadout(RDMarkerHandle::SegueStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_segue_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));

  d_hook_readout=new RDMarkerReadout(RDMarkerHandle::HookStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_hook_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));

  d_fadeup_readout=new RDMarkerReadout(RDMarkerHandle::FadeUp,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_fadeup_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));

  d_fadedown_readout=new RDMarkerReadout(RDMarkerHandle::FadeDown,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_fadedown_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));

  /**************************************************************************
   * Navigation Section
   **************************************************************************/
  //
  // OK Button
  //
  d_ok_button=new QPushButton(tr("OK"),this);
  d_ok_button->setFont(buttonFont());
  connect(d_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  d_cancel_button=new QPushButton(tr("Cancel"),this);
  d_cancel_button->setFont(buttonFont());
  connect(d_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDMarkerDialog::~RDMarkerDialog()
{
}


QSize RDMarkerDialog::sizeHint() const
{
  return QSize(1000,600);
}


int RDMarkerDialog::exec(unsigned cartnum,int cutnum)
{
  QString err_msg;

  d_cart_number=cartnum;
  d_cut_number=cutnum;

  setWindowTitle(d_caption+" - "+tr("Edit Audio"));

  if(!d_marker_view->setCut(&err_msg,cartnum,cutnum)) {
    QMessageBox::critical(this,d_caption+" - "+tr("Error"),err_msg);
    return false;
  }
  for(int i=0;i<RDMarkerHandle::LastRole;i++) {
    RDMarkerHandle::PointerRole role=(RDMarkerHandle::PointerRole)i;
    d_cut_readout->setValue(role,d_marker_view->pointerValue(role));
    d_talk_readout->setValue(role,d_marker_view->pointerValue(role));
    d_segue_readout->setValue(role,d_marker_view->pointerValue(role));
    d_hook_readout->setValue(role,d_marker_view->pointerValue(role));
    d_fadeup_readout->setValue(role,d_marker_view->pointerValue(role));
    d_fadedown_readout->setValue(role,d_marker_view->pointerValue(role));
  }
  return QDialog::exec();
}


void RDMarkerDialog::amplitudeUpData()
{
  d_marker_view->setAudioGain(d_marker_view->audioGain()+300);
}


void RDMarkerDialog::amplitudeDownData()
{
  d_marker_view->setAudioGain(d_marker_view->audioGain()-300);
}


void RDMarkerDialog::timeFullInData()
{
  d_marker_view->setShrinkFactor(1);
}


void RDMarkerDialog::timeInData()
{
  int sf=d_marker_view->shrinkFactor();
  if(sf>1) {
    d_marker_view->setShrinkFactor(sf/2);
  }
}


void RDMarkerDialog::timeOutData()
{
  d_marker_view->setShrinkFactor(2*d_marker_view->shrinkFactor());
}


void RDMarkerDialog::okData()
{
  d_marker_view->save();
  done(true);
}


void RDMarkerDialog::cancelData()
{
  if(d_marker_view->hasUnsavedChanges()) {
    switch(QMessageBox::question(this,d_caption+" - "+tr("Unsaved Changes"),
				 tr("There are unsaved changes!")+"\n"+
				 tr("Do you want to save them?"),
				 QMessageBox::Cancel,QMessageBox::No,
				 QMessageBox::Yes)) {
    case QMessageBox::Yes:
      okData();
      break;

    case QMessageBox::No:
      done(false);
      break;

    default:
      return;
    }
  }
  done(false);
}


void RDMarkerDialog::closeEvent(QCloseEvent *e)
{
  e->ignore();
  cancelData();
}


void RDMarkerDialog::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  //
  // Waveform Section
  //
  d_marker_view->setGeometry(2,2,w-104,d_marker_view->sizeHint().height());

  d_amplitude_box->setGeometry(w-100,2,90,130);
  d_amplitude_up_button->setGeometry(5,24,80,50);
  d_amplitude_down_button->setGeometry(5,74,80,50);

  d_time_box->setGeometry(w-100,142,90,230);
  d_time_fullin_button->setGeometry(5,24,80,50);
  d_time_in_button->setGeometry(5,74,80,50);
  d_time_out_button->setGeometry(5,124,80,50);
  d_time_fullout_button->setGeometry(5,174,80,50);

  d_meter->setGeometry(380,398,
		     d_meter->sizeHint().width(),d_meter->sizeHint().height());

  //
  // Readout Section
  //
  d_cut_readout->setGeometry(10,30+RDMARKERDIALOG_WAVEFORM_HEIGHT+94,
			     d_cut_readout->sizeHint().width(),
			     d_cut_readout->sizeHint().height());

  d_talk_readout->setGeometry(10+1*(d_talk_readout->sizeHint().width()-2),
			      30+RDMARKERDIALOG_WAVEFORM_HEIGHT+94,
			      d_talk_readout->sizeHint().width(),
			      d_talk_readout->sizeHint().height());

  d_segue_readout->setGeometry(10+2*(d_segue_readout->sizeHint().width()-2),
			       30+RDMARKERDIALOG_WAVEFORM_HEIGHT+94,
			       d_segue_readout->sizeHint().width(),
			       d_segue_readout->sizeHint().height());

  d_hook_readout->setGeometry(10+3*(d_hook_readout->sizeHint().width()-2),
			      30+RDMARKERDIALOG_WAVEFORM_HEIGHT+94,
			      d_hook_readout->sizeHint().width(),
			      d_hook_readout->sizeHint().height());

  d_fadeup_readout->setGeometry(10,
				30+RDMARKERDIALOG_WAVEFORM_HEIGHT+90+
				d_hook_readout->sizeHint().height(),
				2*d_fadeup_readout->sizeHint().width()-2,
				d_fadeup_readout->sizeHint().height());

  d_fadedown_readout->setGeometry(6+2*d_fadedown_readout->sizeHint().width(),
				  30+RDMARKERDIALOG_WAVEFORM_HEIGHT+90+
				  d_hook_readout->sizeHint().height(),
				  2*d_fadedown_readout->sizeHint().width()-2,
				  d_fadedown_readout->sizeHint().height());

  //
  // Transport Section
  //
  d_overall_label->setGeometry(60,385,70,20);
  d_overall_edit->setGeometry(60,400,70,21);
  d_region_edit_label->setGeometry(158,385,70,20);
  d_region_edit->setGeometry(158,400,70,21);
  d_size_label->setGeometry(256,385,70,20);
  d_size_edit->setGeometry(256,400,70,21);

  d_play_cursor_button->setGeometry(20,425,65,45);
  d_play_start_button->setGeometry(90,425,65,45);
  d_pause_button->setGeometry(160,425,65,45);
  d_stop_button->setGeometry(230,425,65,45);
  d_loop_button->setGeometry(300,425,65,45);

  d_ok_button->setGeometry(w-180,h-60,80,50);
  d_cancel_button->setGeometry(w-90,h-60,80,50);
}


void RDMarkerDialog::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  //
  // Transport Control Area
  //
  p->setPen(QColor(palette().shadow().color()));
  p->fillRect(11,30+RDMARKERDIALOG_WAVEFORM_HEIGHT,RDMARKERDIALOG_WAVEFORM_WIDTH,92,
	      QColor(RDMARKERDIALOG_HIGHLIGHT_COLOR));
  p->drawRect(11,30+RDMARKERDIALOG_WAVEFORM_HEIGHT,RDMARKERDIALOG_WAVEFORM_WIDTH,92);

  delete p;
}
