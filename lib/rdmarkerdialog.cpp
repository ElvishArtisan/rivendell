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
  d_player=new RDMarkerPlayer(card,port,this);
  connect(d_player,SIGNAL(cursorPositionChanged(unsigned)),
	  d_marker_view,SLOT(setCursorPosition(unsigned)));
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_player,SLOT(setPointerValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_player,SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));

  //
  // Marker Readouts
  //
  d_cut_readout=new RDMarkerReadout(RDMarkerHandle::CutStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_cut_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_cut_readout,SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));
  d_cut_readout->setEnabled(true);

  d_talk_readout=new RDMarkerReadout(RDMarkerHandle::TalkStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_talk_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_talk_readout,SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));

  d_segue_readout=new RDMarkerReadout(RDMarkerHandle::SegueStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_segue_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_segue_readout,SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));

  d_hook_readout=new RDMarkerReadout(RDMarkerHandle::HookStart,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_hook_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_hook_readout,SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));

  d_fadeup_readout=new RDMarkerReadout(RDMarkerHandle::FadeUp,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_fadeup_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_fadeup_readout,
	  SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));

  d_fadedown_readout=new RDMarkerReadout(RDMarkerHandle::FadeDown,this);
  connect(d_marker_view,
	  SIGNAL(pointerValueChanged(RDMarkerHandle::PointerRole,int)),
	  d_fadedown_readout,SLOT(setValue(RDMarkerHandle::PointerRole,int)));
  connect(d_marker_view,
	  SIGNAL(selectedMarkerChanged(RDMarkerHandle::PointerRole)),
	  d_fadedown_readout,
	  SLOT(setSelectedMarker(RDMarkerHandle::PointerRole)));

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
  delete d_player;
  delete d_marker_view;
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
  if(!d_player->setCut(cartnum,cutnum)) {
    QMessageBox::critical(this,d_caption+" - "+tr("Error"),
			  tr("Unable to open cut in audio player!"));
    return false;
  }
  for(int i=0;i<RDMarkerHandle::LastRole;i++) {
    RDMarkerHandle::PointerRole role=(RDMarkerHandle::PointerRole)i;
    d_cut_readout->setValue(role,d_marker_view->pointerValue(role));
    d_cut_readout->setSelectedMarker(RDMarkerHandle::LastRole);
    d_talk_readout->setValue(role,d_marker_view->pointerValue(role));
    d_talk_readout->setSelectedMarker(RDMarkerHandle::LastRole);
    d_segue_readout->setValue(role,d_marker_view->pointerValue(role));
    d_segue_readout->setSelectedMarker(RDMarkerHandle::LastRole);
    d_hook_readout->setValue(role,d_marker_view->pointerValue(role));
    d_hook_readout->setSelectedMarker(RDMarkerHandle::LastRole);
    d_fadeup_readout->setValue(role,d_marker_view->pointerValue(role));
    d_fadeup_readout->setSelectedMarker(RDMarkerHandle::LastRole);
    d_fadedown_readout->setValue(role,d_marker_view->pointerValue(role));
    d_fadedown_readout->setSelectedMarker(RDMarkerHandle::LastRole);
    d_player->setPointerValue(role,d_marker_view->pointerValue(role));
    d_player->setSelectedMarker(RDMarkerHandle::LastRole);
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
  d_player->clearCut();
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
      d_player->clearCut();
      done(false);
      break;

    default:
      return;
    }
  }
  d_player->clearCut();
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
  d_player->setGeometry(2,2+d_marker_view->sizeHint().height(),
			d_player->sizeHint().width(),
			d_player->sizeHint().height());

  d_ok_button->setGeometry(w-180,h-60,80,50);
  d_cancel_button->setGeometry(w-90,h-60,80,50);
}
