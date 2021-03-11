// rdwavewidget.h
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

#ifndef RDWAVEWIDGET_H
#define RDWAVEWIDGET_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

class RDWaveWidget : public QWidget
{
  Q_OBJECT;
 public:
  enum TrackMode {SingleTrack=0,MultiTrack=1};
  RDWaveWidget(TrackMode mode,int height,QWidget *parent=0);
  ~RDWaveWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QGraphicsScene *scene() const;
  qreal audioGain() const;
  int shrinkFactor() const;

 public slots:
  void setAudioGain(qreal gain);
  void setShrinkFactor(int sf);
  bool setCut(QString *err_msg,unsigned cartnum,int cutnum);
  void clear();

 protected:
  QList<uint16_t> energyData() const;
  virtual bool loadCut();
  void resizeEvent(QResizeEvent *e);

 private:
  void WriteWave();
  unsigned d_channels;
  RDWaveWidget::TrackMode d_track_mode;
  qreal d_height;
  QGraphicsView *d_view;
  QGraphicsScene *d_scene;
  QList<uint16_t> d_energy_data;
  int d_x_shrink;
  qreal d_audio_gain;
};


#endif  // RDWAVEWIDGET_H
