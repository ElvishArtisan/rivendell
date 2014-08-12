// rdmarker_bar.h
//
// A marker widget for the RDCueEdit widget.
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmarker_bar.h,v 1.1.2.1 2013/07/05 21:07:28 cvs Exp $
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


#ifndef RDMARKER_BAR_H
#define RDMARKER_BAR_H

#include <qlabel.h>
#include <qpixmap.h>

#include <rd.h>

class RDMarkerBar : public QLabel
{
  Q_OBJECT
 public:
  enum Marker {Play=0,Start=1,End=2,MaxSize=3};
  RDMarkerBar(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int length() const;
  void setLength(int msecs);
  int marker(Marker marker) const;
  void setMarker(Marker marker,int msecs);

 private:
  void DrawMap();
  int marker_pos[RDMarkerBar::MaxSize];
  int marker_length;
};


#endif 
