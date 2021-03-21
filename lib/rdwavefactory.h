// rdwavefactory.h
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

#ifndef RDWAVEFACTORY_H
#define RDWAVEFACTORY_H

#include <QList>
#include <QPixmap>

#include <rdfontengine.h>

class RDWaveFactory
{
 public:
  enum TrackMode {SingleTrack=0,MultiTrack=1};
  RDWaveFactory(TrackMode mode);
  ~RDWaveFactory();
  TrackMode trackMode() const;
  unsigned cartNumber() const;
  int cutNumber() const;
  QPixmap generate(int height,int x_shrink,int gain,bool incl_scale);
  bool setCut(QString *err_msg,unsigned cartnum,int cutnum);
  QList<uint16_t> energy() const;
  int energySize() const;
  static int referenceHeight(int height,int gain);

 private:
  TrackMode d_track_mode;
  unsigned d_cart_number;
  int d_cut_number;
  QList<uint16_t> d_energy;
  unsigned d_channels;
  unsigned d_energy_channels;
  RDFontEngine *d_font_engine;
};


#endif  // RDWAVEFACTORY_H
