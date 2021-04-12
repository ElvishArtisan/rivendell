// rdwavescene.cpp
//
// GraphicsScene Class for Drawing Audio Waveforms
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

#include "rdwavescene.h"

RDWaveScene::RDWaveScene(const QList<uint16_t> &energy,int x_shrink,qreal gain,
			 int height,QObject *parent)
  : QGraphicsScene(0,0,energy.size()/x_shrink,height,parent)
{
  d_energy_data=energy;
  d_x_shrink_factor=x_shrink;
  d_audio_gain=gain;
  d_height=(qreal)height;

  //
  // Gain Ratio
  //
  qreal ratio=exp10(d_audio_gain/20.0);

  //
  // Waveform
  //
  for(int i=0;i<energy.size();i+=x_shrink) {
    //    printf("energy[%d]: %u\n",i,0xFFFF&energy.at(i));
    uint16_t lvl=energy.at(i);
    for(int j=1;j<x_shrink;j++) {
      if(((i+j)<energy.size())&&(energy.at(i+j))>lvl) {
	lvl=energy.at(i+j);
      }
    }
    //    qreal rlvl=ratio*(qreal)lvl/327.67;
    qreal rlvl=ratio*(qreal)lvl*d_height/65534.0;
    // Bottom half
    addRect((qreal)(i/x_shrink),d_height/2.0,
	    (qreal)1,rlvl,
	    QPen(Qt::black),QBrush(Qt::black));

    // Top half
    addRect((qreal)(i/x_shrink),d_height/2.0,
	    (qreal)1,-rlvl,
	    QPen(Qt::black),QBrush(Qt::black));
  }
}


RDWaveScene::~RDWaveScene()
{
}
