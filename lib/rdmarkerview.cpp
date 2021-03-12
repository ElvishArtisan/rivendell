// rdmarkerview.cpp
//
// Widget for displaying/editing cut markers
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

#include "rdmarkerview.h"

RDMarkerView::RDMarkerView(int width,int height,QWidget *parent)
  : QWidget(parent)
{
  d_width=width;
  d_height=height;
  d_cut=NULL;
  d_scene=NULL;
  clear();

  d_view=new QGraphicsView(this);

  d_wave_factory=new RDWaveFactory(RDWaveFactory::MultiTrack);
}


RDMarkerView::~RDMarkerView()
{
  if(d_cut!=NULL) {
    delete d_cut;
  }
  if(d_scene!=NULL) {
    delete d_scene;
  }
  delete d_wave_factory;
  delete d_view;
}


QSize RDMarkerView::sizeHint() const
{
  return QSize(1000,d_height);
}


QSizePolicy RDMarkerView::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
}


int RDMarkerView::audioGain() const
{
  return d_audio_gain;
}


int RDMarkerView::shrinkFactor() const
{
  return d_shrink_factor;
}


void RDMarkerView::setAudioGain(int lvl)
{
  if(d_audio_gain!=lvl) {
    d_audio_gain=lvl;
    WriteWave();
  }
}


void RDMarkerView::setShrinkFactor(int sf)
{
  if((d_shrink_factor!=sf)&&(sf<=d_max_shrink_factor)) {
    d_shrink_factor=sf;
    WriteWave();
  }
}


void RDMarkerView::setMaximumShrinkFactor()
{
  setShrinkFactor(d_max_shrink_factor);
}


bool RDMarkerView::setCut(QString *err_msg,unsigned cartnum,int cutnum)
{
  if(d_cut!=NULL) {
    delete d_cut;
  }
  d_cut=new RDCut(cartnum,cutnum);
  if(!d_cut->exists()) {
    *err_msg=tr("No such cart/cut!");
    return false;
  }
  if(!d_wave_factory->setCut(err_msg,cartnum,cutnum)) {
    return false;
  }
  d_max_shrink_factor=1;
  while(d_wave_factory->energySize()>=(d_width*d_max_shrink_factor)) {
    d_max_shrink_factor=d_max_shrink_factor*2;
  }
  d_pad_size=(d_width*d_max_shrink_factor-d_wave_factory->energySize())/d_max_shrink_factor-1;
  d_shrink_factor=d_max_shrink_factor;
  WriteWave();

  return true;
}


void RDMarkerView::clear()
{
  if(d_cut!=NULL) {
    delete d_cut;
    d_cut=NULL;
  }
  if(d_scene!=NULL) {
    delete d_scene;
    d_scene=NULL;
  }
  d_shrink_factor=1;
  d_max_shrink_factor=1;
  d_pad_size=0;
  d_audio_gain=900;
}


void RDMarkerView::resizeEvent(QResizeEvent *e)
{
  d_view->setGeometry(0,0,size().width(),size().height());
}


void RDMarkerView::WriteWave()
{
  QPixmap wavemap=
    d_wave_factory->generate(d_height-20,d_shrink_factor,d_audio_gain,true);

  if(d_scene!=NULL) {
    d_scene->deleteLater();
  }
  d_scene=new QGraphicsScene(0,0,wavemap.width()+d_pad_size,d_height-20,this);

  d_scene->addPixmap(wavemap);
  d_scene->addRect(wavemap.width(),0,d_pad_size,d_height-20,QPen(Qt::gray),QBrush(Qt::gray));
  d_view->setScene(d_scene);
}
