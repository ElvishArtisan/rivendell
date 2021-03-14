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

#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

#include <rdcut.h>
#include <rdwavefactory.h>

class RDMarkerHandle : public QGraphicsPolygonItem
{
 public:
  enum PointerType {Start=0,End=1};
  enum PointerRole {CutStart=0,CutEnd=1,
		    TalkStart=2,TalkEnd=3,
		    SegueStart=4,SegueEnd=5,
		    HookStart=6,HookEnd=7,
		    FadeUp=8,FadeDown=9,
		    LastRole=10};
  RDMarkerHandle(RDMarkerHandle::PointerRole role,PointerType type,
		 void *mkrview,QGraphicsItem *parent=nullptr);
  QString name() const;
  PointerRole role() const;
  int minimum() const;
  void setMinimum(int pos);
  int maximum() const;
  void setMaximum(int pos);
  void setRange(int min,int max);
  static QString pointerRoleText(PointerRole role);
  static QString pointerRoleTypeText(PointerRole role);
  static QColor pointerRoleColor(PointerRole role);

 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *e);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

 private:
  QString d_name;
  PointerRole d_role;
  QList<QGraphicsItem *> d_peers;
  void *d_marker_view;
  int d_minimum;
  int d_maximum;
};


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
  int pointerValue(RDMarkerHandle::PointerRole role);
  bool hasUnsavedChanges() const;
  void updatePosition(RDMarkerHandle::PointerRole role, int offset);

 public slots:
  void setAudioGain(int lvl);
  void setShrinkFactor(int sf);
  void setMaximumShrinkFactor();
  bool setCut(QString *err_msg,unsigned cartnum,int cutnum);
  void save();
  void clear();

 signals:
  void pointerValueChanged(RDMarkerHandle::PointerRole role,int msec);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  int Frame(int msec) const;
  bool LoadCutData();
  void WriteWave();
  void DrawMarker(RDMarkerHandle::PointerType type,
		  RDMarkerHandle::PointerRole role,int handle_pos);
  QGraphicsView *d_view;
  QGraphicsScene *d_scene;
  unsigned d_cart_number;
  int d_cut_number;
  int d_width;
  int d_height;
  int d_shrink_factor;
  int d_max_shrink_factor;
  int d_pad_size;
  int d_audio_gain;
  unsigned d_sample_rate;
  unsigned d_channels;
  QStringList d_pointer_fields;
  RDWaveFactory *d_wave_factory;
  bool d_has_unsaved_changes;
  int d_pointers[RDMarkerHandle::LastRole];
};


#endif  // RDMARKERVIEW_H
