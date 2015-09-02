//   rdmarkerwaveform.h
//
//   Wavform widget for Rivendell's Marker Editor
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDMARKERWAVEFORM_H
#define RDMARKERWAVEFORM_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qcolor.h>

#include <rdconfig.h>
#include <rdcut.h>
#include <rdstation.h>
#include <rduser.h>
#include <rdwavepainter.h>

#define RDMARKERWAVEFORM_WIDTH 717
#define RDMARKERWAVEFORM_HEIGHT 352
#define RDMARKERWAVEFORM_MIN_WAVE_WIDTH 6000
#define RDMARKERWAVEFORM_MIN_WAVE_GAIN 128
#define RDMARKERWAVEFORM_MAX_WAVE_GAIN 16384

class RDMarkerWaveform : public QWidget
{
  Q_OBJECT
 public:
  enum CuePoints {Play=0,Start=1,End=2,SegueStart=3,SegueEnd=4,
		  TalkStart=5,TalkEnd=6,HookStart=7,HookEnd=8,
		  FadeUp=9,FadeDown=10,LastMarker=11};
  enum Arrow {None=0,Left=1,Right=2};
  RDMarkerWaveform(RDCut *cut,RDUser *user,RDStation *station,RDConfig *config,
		   RDWavePainter::Channel chan,QWidget *parent);
  ~RDMarkerWaveform();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  int viewportWidth() const;
  void setCursor(CuePoints pt,int msecs);

 signals:
  void viewportWidthChanged(int msecs);
  void clicked(int msecs);

 public slots:
  void setViewportStart(int msecs);
  void zoomIn();
  void zoomOut();
  void fullIn();
  void fullOut();
  void ampUp();
  void ampDown();

 protected:
  void paintEvent(QPaintEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

 private:
  void DrawCursor(QPainter *p,RDMarkerWaveform::CuePoints pt);
  int XCoordinate(int msecs) const;
  int GridIncrement() const;
  RDCut *wave_cut;
  RDUser *wave_user;
  RDStation *wave_station;
  RDConfig *wave_config;
  RDWavePainter::Channel wave_channel;
  int wave_start;
  int wave_width;
  int wave_width_max;
  int wave_gain;
  int wave_cursors[RDMarkerWaveform::LastMarker];
  QColor wave_cursor_colors[RDMarkerWaveform::LastMarker];
  int wave_cursor_arrow_offsets[RDMarkerWaveform::LastMarker];
  RDMarkerWaveform::Arrow wave_cursor_arrow_dirs[RDMarkerWaveform::LastMarker];
};


#endif  // RDMARKERWAVEFORM_H
