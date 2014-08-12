// rdwavepainter.h
//
// A Painter Class for Drawing Audio Waveforms
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdwavepainter.h,v 1.10.6.1 2013/11/13 23:36:34 cvs Exp $
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

#ifndef RDWAVEPAINTER_H
#define RDWAVEPAINTER_H

#include <qpainter.h>

#include <rdconfig.h>
#include <rdpeaksexport.h>
#include <rdstation.h>
#include <rduser.h>
#include <rdcut.h>

class RDWavePainter : public QPainter
{
 public:
  enum Channel {Mono=0,Left=1,Right=2};
  RDWavePainter(const QPaintDevice *pd,RDCut *cut,RDStation *station,
		RDUser *user,RDConfig *config);
  RDWavePainter(RDStation *station,RDUser *user,RDConfig *config);
  ~RDWavePainter();
  void nameWave(RDCut *cut);
  bool begin(const QPaintDevice *pd,RDCut *cut);
  bool begin(const QPaintDevice *pd);
  void drawWaveBySamples(int x,int w,int startsamp,int endsamp,int gain,
			 Channel channel,const QColor &color,
			 int startclip=-1,int endclip=-1);
  void drawWaveByMsecs(int x,int w,int startmsecs,int endmsecs,int gain,
		       Channel channel,const QColor &color,
		       int startclip=-1,int endclip=-1);

 private:
  void LoadWave();
  RDCut *wave_cut;
  RDStation *wave_station;
  RDUser *wave_user;
  RDConfig *wave_config;
  RDPeaksExport *wave_peaks;
  unsigned wave_sample_rate;
  unsigned wave_channels;
};


#endif  // RDWAVEPAINTER_H
