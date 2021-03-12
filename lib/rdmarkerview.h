// rdmarkerview.h
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

#ifndef RDMARKERVIEW_H
#define RDMARKERVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

#include <rdcut.h>
#include <rdwavefactory.h>

class RDMarkerView : public QWidget
{
  Q_OBJECT;
 public:
  RDMarkerView(int width,int height,QWidget *parent=0);
  ~RDMarkerView();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  int audioGain() const;
  int shrinkFactor() const;

 public slots:
  void setAudioGain(int lvl);
  void setShrinkFactor(int sf);
  void setMaximumShrinkFactor();
  bool setCut(QString *err_msg,unsigned cartnum,int cutnum);
  void clear();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void WriteWave();
  QGraphicsView *d_view;
  QGraphicsScene *d_scene;
  RDCut *d_cut;
  int d_width;
  int d_height;
  int d_shrink_factor;
  int d_max_shrink_factor;
  int d_pad_size;
  int d_audio_gain;
  RDWaveFactory *d_wave_factory;
};


#endif  // RDMARKERVIEW_H
