// rdwavewidget.cpp
//
// Widget for displaying audio waveforms
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

#include <math.h>

#include "rdapplication.h"
#include "rdcut.h"
#include "rdpeaksexport.h"
#include "rdwavewidget.h"

RDWaveWidget::RDWaveWidget(RDWaveWidget::TrackMode mode,int height,
			   QWidget *parent)
  : QWidget(parent)
{
  d_channels=0;
  d_audio_gain=0.0;
  d_x_shrink=1;
  d_track_mode=mode;
  d_height=(qreal)height;

  d_view=new QGraphicsView(this);
  d_scene=NULL;
}


RDWaveWidget::~RDWaveWidget()
{
  delete d_view;
  if(d_scene!=NULL) {
    delete d_scene;
  }
}


QSize RDWaveWidget::sizeHint() const
{
  return QSize(1000,d_height+20);
  //  return QSize(1000,d_height+42);
}


QSizePolicy RDWaveWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
}


QGraphicsScene *RDWaveWidget::scene() const
{
  return d_scene;
}


qreal RDWaveWidget::audioGain() const
{
  return d_audio_gain;
}


int RDWaveWidget::shrinkFactor() const
{
  return d_x_shrink;
}


void RDWaveWidget::setAudioGain(qreal gain)
{
  if(gain!=d_audio_gain) {
    d_audio_gain=gain;
    WriteWave();
  }
}


void RDWaveWidget::setShrinkFactor(int sf)
{
  if(sf!=d_x_shrink) {
    d_x_shrink=sf;
    WriteWave();
  }
}


bool RDWaveWidget::setCut(QString *err_msg,unsigned cartnum,int cutnum)
{
  clear();

  //
  // Get Cut Info
  //
  RDCut *cut=new RDCut(cartnum,cutnum);
  if(!cut->exists()) {
    *err_msg=tr("No such cart/cut!");
    delete cut;
    return false;
  }
  d_channels=cut->channels();
  delete cut;

  //
  // Get Cut Energy Data
  //
  RDPeaksExport::ErrorCode err_code;
  RDPeaksExport *conv=new RDPeaksExport(this);

  conv->setCartNumber(cartnum);
  conv->setCutNumber(cutnum);
  if((err_code=conv->runExport(rda->user()->name(),rda->user()->password()))!=
     RDPeaksExport::ErrorOk) {
    *err_msg=tr("Energy export failed")+": "+RDPeaksExport::errorText(err_code);
    delete conv;
    return false;
  }
  if((d_track_mode==RDWaveWidget::SingleTrack)&&(d_channels==2)) { // Mix-down
    for(unsigned i=0;i<conv->energySize();i+=2) {
      uint32_t frame=
	((uint32_t)conv->energy(i)+(uint32_t)conv->energy(i+1))/2;
      d_energy_data.push_back(frame);
    }    
  }
  else {  // Pass-through
    for(unsigned i=0;i<conv->energySize();i++) {
      d_energy_data.push_back(conv->energy(i));
    }
  }
  delete conv;

  return loadCut();
}


void RDWaveWidget::clear()
{
  d_view->setScene(NULL);
  delete d_scene;
  d_scene=NULL;
  d_energy_data.clear();
  d_x_shrink=1;
}


QList<uint16_t> RDWaveWidget::energyData() const
{
  return d_energy_data;
}


bool RDWaveWidget::loadCut()
{
  /*
  switch(d_track_mode) {
  case RDWaveWidget::SingleTrack:
    d_scene=new RDWaveScene(d_energy_data,d_x_shrink,0.0,d_height,this);
    break;

  case RDWaveWidget::MultiTrack:
    switch(d_channels) {
    case 1:
      d_scene=new RDWaveScene(d_energy_data,d_x_shrink,0.0,d_height,this);
      break;

    case 2:
      d_scenes.push_back(new RDWaveScene(d_energy_datas.front(),d_x_shrink,0.0,
					 d_height/2,this));
      d_views.front()->setScene(d_scenes.back());
      d_scenes.push_back(new RDWaveScene(d_energy_datas.back(),d_x_shrink,0.0,
					 d_height/2,this));
      d_views.back()->setScene(d_scenes.back());
      d_views.front()->show();
      d_views.back()->show();
      break;
    }
    break;
  }
  d_view->setScene(d_scene);
  */
  WriteWave();
  
  return true;
}


void RDWaveWidget::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  d_view->setGeometry(0,0,w,h);
}


void RDWaveWidget::WriteWave()
{
  //
  // Calculate Effective Channels
  //
  int chans=d_channels;
  if(d_track_mode==RDWaveWidget::SingleTrack) {
    chans=1;
  }

  //
  // Get New Scene
  if(d_scene!=NULL) {
    delete d_scene;
  }
  d_scene=new QGraphicsScene(0,0,d_energy_data.size()/d_x_shrink,d_height,this);

  //
  // Gain Ratio
  //
  qreal ratio=exp10(d_audio_gain/20.0);

  //
  // Waveform
  //
  for(int i=0;i<chans;i++) {
    for(int j=i;j<d_energy_data.size();j+=(chans*d_x_shrink)) {
      uint16_t lvl=d_energy_data.at(j);
      for(int k=1;k<d_x_shrink;k++) {
	if(((j+k)<d_energy_data.size())&&(d_energy_data.at(j+k))>lvl) {
	  lvl=d_energy_data.at(j+k);
	}
      }
      qreal rlvl=ratio*(qreal)lvl*d_height/(65534.0*(qreal)chans);
      // Bottom half
      d_scene->addRect((qreal)(j/d_x_shrink),
		       d_height/((qreal)chans*2.0)+i*d_height/((qreal)chans),
		       (qreal)1,
		       rlvl,
		       QPen(Qt::black),QBrush(Qt::black));
      
      // Top half
      d_scene->addRect((qreal)(j/d_x_shrink),
		       d_height/((qreal)chans*2.0)+i*d_height/((qreal)chans),
		       (qreal)1,
		       -rlvl,
		       QPen(Qt::black),QBrush(Qt::black));
    }
  }
  for(int i=1;i<chans;i++) {
    // Track Divider
    d_scene->addLine(0.0,i*d_height/((qreal)chans),
		     d_energy_data.size()/d_x_shrink,i*d_height/((qreal)chans),
		     QPen(Qt::gray));
  }
  d_view->setScene(d_scene);
}


/*
void RDWaveWidget::WriteWave()
{
  //
  // Calculate Effective Channels
  //
  int chans=d_channels;
  if(d_track_mode==RDWaveWidget::SingleTrack) {
    chans=1;
  }

  //
  // Get New Scene
  if(d_scene!=NULL) {
    delete d_scene;
  }
  d_scene=new QGraphicsScene(0,0,d_energy_data.size()/d_x_shrink,d_height,this);

  //
  // Gain Ratio
  //
  qreal ratio=exp10(d_audio_gain/20.0);

  //
  // Waveform
  //
  for(int j=0;j<d_energy_data.size();j+=d_x_shrink) {
    uint16_t lvl=d_energy_data.at(j);
    for(int k=1;k<d_x_shrink;k++) {
      if(((j+k)<d_energy_data.size())&&(d_energy_data.at(j+k))>lvl) {
	lvl=d_energy_data.at(j+k);
      }
    }
    qreal rlvl=ratio*(qreal)lvl*d_height/65534.0;
    // Bottom half
    d_scene->addRect((qreal)(j/d_x_shrink),d_height/2.0,
		     (qreal)1,rlvl,
		     QPen(Qt::black),QBrush(Qt::black));
      
    // Top half
    d_scene->addRect((qreal)(j/d_x_shrink),d_height/2.0,
		     (qreal)1,-rlvl,
		     QPen(Qt::black),QBrush(Qt::black));
  }
  d_view->setScene(d_scene);
}
*/
