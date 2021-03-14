// rdwavefactory.cpp
//
// Factory for generating audio waveform pixmaps
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

#include <QObject>
#include <QPainter>

#include "rdapplication.h"
#include "rdconf.h"
#include "rdcut.h"
#include "rdpeaksexport.h"
#include "rdwavefactory.h"

RDWaveFactory::RDWaveFactory(RDWaveFactory::TrackMode mode)
{
  d_track_mode=mode;
  d_cart_number=0;
  d_cut_number=-1;

  d_font_engine=new RDFontEngine();
}


RDWaveFactory::~RDWaveFactory()
{
  delete d_font_engine;
}


RDWaveFactory::TrackMode RDWaveFactory::trackMode() const
{
  return d_track_mode;
}


unsigned RDWaveFactory::cartNumber() const
{
  return d_cart_number;
}


int RDWaveFactory::cutNumber() const
{
  return d_cut_number;
}


QPixmap RDWaveFactory::generate(int height,int x_shrink,int gain,
				bool incl_scale)
{
  QPixmap pix(d_energy.size()/(x_shrink*d_energy_channels),height);
  pix.fill(Qt::white);  // FIXME: make the background transparent
  QPainter *p=new QPainter(&pix);
  p->setFont(d_font_engine->defaultFont());

  //
  // Time Scale
  //
  if(incl_scale) {
    int interval=2*rda->system()->sampleRate()/1152;
    int msec=2000;
    for(int i=interval;i<(d_energy.size()/x_shrink);i+=interval) {
      p->setPen(Qt::green);
      p->drawLine(i,0,i,height);
      p->setPen(Qt::red);
      for(unsigned j=0;j<d_energy_channels;j++) {
	p->drawText(i+5,(j+1)*height/d_energy_channels-2,
		    RDGetTimeLength(msec*x_shrink,false,false));
      }
      msec+=2000;
    }
  }

  //
  // Gain Ratio
  //
  double ratio=exp10((double)gain/2000.0);

  //
  // Waveform
  //
  p->setPen(Qt::black);
  int ref_line=exp10((double)(-REFERENCE_LEVEL)/2000.00)*height*ratio/
    ((double)d_energy_channels*2.0);
  int clip_line=height/(2*d_energy_channels);
  for(unsigned i=0;i<d_energy_channels;i++) {
    int zero_line=height/(d_energy_channels*2)+i*height/(d_energy_channels);
    if(incl_scale) {
      if(ref_line<clip_line) {
	p->setPen(Qt::red);
	p->drawLine(0,zero_line+ref_line,
		    d_energy.size()/x_shrink,zero_line+ref_line);
	p->drawLine(0,zero_line-ref_line,
		    d_energy.size()/x_shrink,zero_line-ref_line);
	p->setPen(Qt::black);
      }
    }
    p->drawLine(0,zero_line,d_energy.size()/x_shrink,zero_line);
    for(int j=i;j<d_energy.size();j+=(d_energy_channels*x_shrink)) {
      uint16_t lvl=d_energy.at(j);
      for(int k=1;k<x_shrink;k++) {
	if(((j+k)<d_energy.size())&&(d_energy.at(j+k))>lvl) {
	  lvl=d_energy.at(j+k);
	}
      }
      int rlvl=(int)(ratio*(double)lvl*(double)height/
		     (65534.0*(double)d_energy_channels));
      if(rlvl>clip_line) {
	rlvl=clip_line;
      }
      // Bottom half
      p->fillRect(j/(x_shrink*d_energy_channels),zero_line,1,rlvl,Qt::black);

      // Top half
      p->fillRect(j/(x_shrink*d_energy_channels),zero_line,1,-rlvl,Qt::black);
    }
  }

  //
  // Dividing Line
  //
  p->setPen(Qt::gray);
  for(unsigned i=1;i<d_energy_channels;i++) {
    p->drawLine(0,i*height/d_energy_channels,
		d_energy.size()/x_shrink,i*height/d_energy_channels);
  }

  p->end();
  delete p;

  return pix;
}


bool RDWaveFactory::setCut(QString *err_msg,unsigned cartnum,int cutnum)
{
  d_energy.clear();
  d_cart_number=cartnum;
  d_cut_number=cutnum;

  //
  // Get Cut Info
  //
  RDCut *cut=new RDCut(cartnum,cutnum);
  if(!cut->exists()) {
    *err_msg=QObject::tr("No such cart/cut!");
    delete cut;
    return false;
  }
  d_channels=cut->channels();
  delete cut;
  d_energy_channels=d_channels;
  if(d_track_mode==RDWaveFactory::SingleTrack) {
    d_energy_channels=1;
  }

  //
  // Get Cut Energy Data
  //
  RDPeaksExport::ErrorCode err_code;
  RDPeaksExport *conv=new RDPeaksExport();

  conv->setCartNumber(cartnum);
  conv->setCutNumber(cutnum);
  if((err_code=conv->runExport(rda->user()->name(),rda->user()->password()))!=
     RDPeaksExport::ErrorOk) {
    *err_msg=QObject::tr("Energy export failed")+": "+
      RDPeaksExport::errorText(err_code);
    delete conv;
    return false;
  }
  if((d_track_mode==RDWaveFactory::SingleTrack)&&(d_channels==2)) { // Mix-down
    for(unsigned i=0;i<conv->energySize();i+=2) {
      uint32_t frame=
	((uint32_t)conv->energy(i)+(uint32_t)conv->energy(i+1))/2;
      d_energy.push_back(frame);
    }    
  }
  else {  // Pass-through
    for(unsigned i=0;i<conv->energySize();i++) {
      d_energy.push_back(conv->energy(i));
    }
  }
  delete conv;

  return true;
}


int RDWaveFactory::energySize() const
{
  return d_energy.size()/d_energy_channels;
}


int RDWaveFactory::referenceHeight(int height,int gain)
{
  return (int)((double)height*32767.0*
	       exp10((double)(gain-REFERENCE_LEVEL)/2000.0));
}
