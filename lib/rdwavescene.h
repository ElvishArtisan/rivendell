// rdwavescene.h
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

#ifndef RDWAVESCENE_H
#define RDWAVESCENE_H

#include <QGraphicsScene>
#include <QList>

#include <rdcut.h>

class RDWaveScene : public QGraphicsScene
{
  Q_OBJECT;
 public:
  enum Channel {Mono=0,Left=1,Right=2};
  RDWaveScene(const QList<uint16_t> &energy,int x_shrink,qreal gain,int height,
	      QObject *parent=0);
  ~RDWaveScene();

 private:
  QList<uint16_t> d_energy_data;
  int d_x_shrink_factor;
  qreal d_audio_gain;
  qreal d_height;
};


#endif  // RDWAVESCENE_H
