// rdmarker_bar.cpp
//
// A marker widget for the RDCueEdit widget.
//
//   (C) Copyright 2002-2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpainter.h>
#include <q3pointarray.h>
//Added by qt3to4:
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>

#include <rdmarker_bar.h>

RDMarkerBar::RDMarkerBar(QWidget *parent)
  : QLabel(parent)
{
  for(int i=0;i<RDMarkerBar::MaxSize;i++) {
    marker_pos[i]=0;
  }
  setLineWidth(1);
  setMidLineWidth(0);
  setFrameStyle(Q3Frame::Box|Q3Frame::Plain);
}


QSize RDMarkerBar::sizeHint() const
{
  return QSize(425,14);
}


QSizePolicy RDMarkerBar::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDMarkerBar::length() const
{
  return marker_length;
}


void RDMarkerBar::setLength(int msecs)
{
  marker_length=msecs;
  DrawMap();
}


int RDMarkerBar::marker(Marker marker) const
{
  if(marker>=RDMarkerBar::MaxSize) {
    return 0;
  }
  return marker_pos[marker];
}


void RDMarkerBar::setMarker(Marker marker,int msecs)
{
  if(marker>=RDMarkerBar::MaxSize) {
    return;
  }
  marker_pos[marker]=msecs;
  DrawMap();
}


void RDMarkerBar::DrawMap()
{
  QPixmap *pix=new QPixmap(size());
  QPainter *p=new QPainter(pix);
  Q3PointArray *pt;
  p->fillRect(0,0,size().width(),size().height(),backgroundColor());
  if(marker_length>0) {
    p->setPen(RD_CUEEDITOR_START_MARKER);
    p->setBrush(RD_CUEEDITOR_START_MARKER);
    p->fillRect(size().width()*marker_pos[RDMarkerBar::Start]/marker_length-2,0,
		4,size().height(),RD_CUEEDITOR_START_MARKER);
    pt=new Q3PointArray(3);
    pt->setPoint(0,size().width()*marker_pos[RDMarkerBar::Start]/marker_length-2,
		 size().height()/2-1);
    pt->setPoint(1,size().width()*marker_pos[RDMarkerBar::Start]/marker_length-12,
		 size().height()-2);
    pt->setPoint(2,size().width()*marker_pos[RDMarkerBar::Start]/marker_length-12,
		 1);
    p->drawPolygon(*pt);

    p->fillRect(size().width()*marker_pos[RDMarkerBar::End]/marker_length-2,0,
		4,size().height(),RD_CUEEDITOR_START_MARKER);
    pt->setPoint(0,size().width()*marker_pos[RDMarkerBar::End]/marker_length+2,
		 size().height()/2-1);
    pt->setPoint(1,size().width()*marker_pos[RDMarkerBar::End]/marker_length+12,
		 size().height()-2);
    pt->setPoint(2,size().width()*marker_pos[RDMarkerBar::End]/marker_length+12,
		 1);
    p->drawPolygon(*pt);
    delete pt;

    p->setPen(RD_CUEEDITOR_PLAY_MARKER);
    p->setBrush(RD_CUEEDITOR_PLAY_MARKER);
    p->fillRect(size().width()*marker_pos[RDMarkerBar::Play]/marker_length-1,0,
		2,size().height(),RD_CUEEDITOR_PLAY_MARKER);
  }
  p->end();
  setPixmap(*pix);
  delete p;
  delete pix;
}

