//   rdmarkerwaveform.cpp
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

#include <qpointarray.h>

#include "rdconf.h"
#include "rdmarkerwaveform.h"

RDMarkerWaveform::RDMarkerWaveform(RDCut *cut,RDUser *user,RDStation *station,
				   RDConfig *config,RDWavePainter::Channel chan,
				   QWidget *parent)
  : QWidget(parent)
{
  wave_cut=cut;
  wave_user=user;
  wave_station=station;
  wave_config=config;
  wave_channel=chan;

  //
  // Arrow Offset Table
  //
  wave_cursor_arrow_offsets[RDMarkerWaveform::Play]=0;
  wave_cursor_arrow_offsets[RDMarkerWaveform::Start]=10;
  wave_cursor_arrow_offsets[RDMarkerWaveform::End]=10;
  wave_cursor_arrow_offsets[RDMarkerWaveform::FadeUp]=40;
  wave_cursor_arrow_offsets[RDMarkerWaveform::FadeDown]=40;
  wave_cursor_arrow_offsets[RDMarkerWaveform::TalkStart]=20;
  wave_cursor_arrow_offsets[RDMarkerWaveform::TalkEnd]=20;
  wave_cursor_arrow_offsets[RDMarkerWaveform::SegueStart]=30;
  wave_cursor_arrow_offsets[RDMarkerWaveform::SegueEnd]=30;
  wave_cursor_arrow_offsets[RDMarkerWaveform::HookStart]=50;
  wave_cursor_arrow_offsets[RDMarkerWaveform::HookEnd]=50;

  //
  // Arrow Direction Table
  //
  wave_cursor_arrow_dirs[RDMarkerWaveform::Play]=RDMarkerWaveform::None;
  wave_cursor_arrow_dirs[RDMarkerWaveform::Start]=RDMarkerWaveform::Left;
  wave_cursor_arrow_dirs[RDMarkerWaveform::End]=RDMarkerWaveform::Right;
  wave_cursor_arrow_dirs[RDMarkerWaveform::FadeUp]=RDMarkerWaveform::Left;
  wave_cursor_arrow_dirs[RDMarkerWaveform::FadeDown]=RDMarkerWaveform::Right;
  wave_cursor_arrow_dirs[RDMarkerWaveform::TalkStart]=RDMarkerWaveform::Left;
  wave_cursor_arrow_dirs[RDMarkerWaveform::TalkEnd]=RDMarkerWaveform::Right;
  wave_cursor_arrow_dirs[RDMarkerWaveform::SegueStart]=RDMarkerWaveform::Left;
  wave_cursor_arrow_dirs[RDMarkerWaveform::SegueEnd]=RDMarkerWaveform::Right;
  wave_cursor_arrow_dirs[RDMarkerWaveform::HookStart]=RDMarkerWaveform::Left;
  wave_cursor_arrow_dirs[RDMarkerWaveform::HookEnd]=RDMarkerWaveform::Right;

  wave_start=0;
  wave_width=(double)cut->length()*1.05;
  wave_width_max=cut->length();

  wave_gain=1024;

  for(int i=0;i<RDMarkerWaveform::LastMarker;i++) {
    wave_cursors[i]=-1;
  }
}


RDMarkerWaveform::~RDMarkerWaveform()
{
}


QSize RDMarkerWaveform::sizeHint() const
{
  return QSize(RDMARKERWAVEFORM_WIDTH+20,RDMARKERWAVEFORM_HEIGHT);
}


QSizePolicy RDMarkerWaveform::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDMarkerWaveform::viewportWidth() const
{
  return wave_width;
}


void RDMarkerWaveform::setCursor(CuePoints pt,int msecs)
{
  if(msecs!=wave_cursors[pt]) {
    if(pt==RDMarkerWaveform::Play) {
      SetPlayCursor(msecs);
      wave_cursors[pt]=msecs;
    }
    else {
      wave_cursors[pt]=msecs;
      update();
    }
  }
}


QColor RDMarkerWaveform::markerColor(CuePoints pt)
{
  QColor ret;

  switch(pt) {
  case RDMarkerWaveform::Play:
    ret=Qt::black;
    break;

  case RDMarkerWaveform::Start:
  case RDMarkerWaveform::End:
    ret=RD_START_END_MARKER_COLOR;
    break;

  case RDMarkerWaveform::FadeUp:
  case RDMarkerWaveform::FadeDown:
    ret=RD_FADE_MARKER_COLOR;
    break;

  case RDMarkerWaveform::TalkStart:
  case RDMarkerWaveform::TalkEnd:
    ret=RD_TALK_MARKER_COLOR;
    break;

  case RDMarkerWaveform::SegueStart:
  case RDMarkerWaveform::SegueEnd:
    ret=RD_SEGUE_MARKER_COLOR;
    break;

  case RDMarkerWaveform::HookStart:
  case RDMarkerWaveform::HookEnd:
    ret=RD_HOOK_MARKER_COLOR;
    break;

  case RDMarkerWaveform::LastMarker:
    break;
  }

  return ret;
}


QString RDMarkerWaveform::markerName(CuePoints pt)
{
  QString ret;

  switch(pt) {
  case RDMarkerWaveform::Play:
    ret="<"+tr("none")+">";
    break;

  case RDMarkerWaveform::Start:
  case RDMarkerWaveform::End:
    ret=tr("Cut");
    break;

  case RDMarkerWaveform::FadeUp:
    ret=tr("Fade Up");
    break;

  case RDMarkerWaveform::FadeDown:
    ret=tr("Fade Down");
    break;

  case RDMarkerWaveform::TalkStart:
  case RDMarkerWaveform::TalkEnd:
    ret=tr("Talk");
    break;

  case RDMarkerWaveform::SegueStart:
  case RDMarkerWaveform::SegueEnd:
    ret=tr("Segue");
    break;

  case RDMarkerWaveform::HookStart:
  case RDMarkerWaveform::HookEnd:
    ret=tr("Hook");
    break;

  case RDMarkerWaveform::LastMarker:
    break;
  }

  return ret;
}


void RDMarkerWaveform::setPlayCursor(int msecs)
{
  setCursor(RDMarkerWaveform::Play,msecs);
}


void RDMarkerWaveform::setViewportStart(int msecs)
{
  if(((unsigned)msecs<wave_cut->length())&&(msecs!=wave_start)) {
    wave_start=msecs;
    repaint();
  }
}


void RDMarkerWaveform::zoomIn()
{
  if(wave_width>RDMARKERWAVEFORM_MIN_WAVE_WIDTH) {
    wave_width/=2;
    repaint();
    emit viewportWidthChanged(wave_width);
  }
}


void RDMarkerWaveform::zoomOut()
{
  if(wave_width<wave_width_max) {
    wave_width*=2;
    repaint();
    emit viewportWidthChanged(wave_width);
  }
}


void RDMarkerWaveform::fullIn()
{
  wave_width=(double)wave_cut->length()*1.05;
  while(wave_width>RDMARKERWAVEFORM_MIN_WAVE_WIDTH) {
    wave_width/=2;
  }
  repaint();
}


void RDMarkerWaveform::fullOut()
{
  wave_width=(double)wave_cut->length()*1.05;
  repaint();
}


void RDMarkerWaveform::ampUp()
{
  if(wave_gain<RDMARKERWAVEFORM_MAX_WAVE_GAIN) {
    wave_gain*=2;
    repaint();
  }
}


void RDMarkerWaveform::ampDown()
{
  if(wave_gain>RDMARKERWAVEFORM_MIN_WAVE_GAIN) {
    wave_gain/=2;
    repaint();
  }
}


void RDMarkerWaveform::paintEvent(QPaintEvent *e)
{
  int h=size().height();
  int w=size().width();
  QPixmap *pix=new QPixmap(size());
  RDWavePainter *p=
    new RDWavePainter(pix,wave_cut,wave_station,wave_user,wave_config);
  p->fillRect(0,0,size().width(),size().height(),Qt::white);

  //
  // Dead Zone
  //
  p->setPen(colorGroup().mid());
  p->setBrush(colorGroup().mid());
  p->drawRect(XCoordinate(wave_cut->length())+10,0,w,h);

  //
  // Time Ticks
  //
  p->setFont(QFont("helvetica",8,QFont::Normal));
  for(unsigned i=GridIncrement();i<(2*wave_cut->length());i+=GridIncrement()) {
    p->setPen(Qt::green);
    p->moveTo(XCoordinate(i)+10,0);
    p->lineTo(XCoordinate(i)+10,h);
    p->setPen(Qt::red);
    p->drawText(XCoordinate(i)+5+10,h-5,RDGetTimeLength(i,false,false));
  }

  //
  // Reference Level
  //
  p->setPen(Qt::red);
  p->setBrush(QBrush());
  p->moveTo(10,h/2+wave_gain*5/88);
  p->lineTo(w-10,h/2+wave_gain*5/88);
  p->moveTo(10,h/2-wave_gain*5/88);
  p->lineTo(w-10,h/2-wave_gain*5/88);

  //
  // Waveform
  //
  p->drawWaveByMsecs(10,size().width()-20,wave_start,wave_start+wave_width,
		     wave_gain,wave_channel,Qt::black);

  //
  // Faux End Pieces
  //
  p->setPen(palette().active().background());
  p->setBrush(palette().active().background());
  p->drawRect(0,0,10,h);
  p->drawRect(w-10,0,10,h);

  //
  // Border
  //
  p->setPen(Qt::black);
  p->setBrush(QBrush());
  p->drawRect(10,0,size().width()-20,size().height());

  //
  // Markers
  //
  for(int i=1;i<RDMarkerWaveform::LastMarker;i++) {
    if(wave_cursors[i]>=0) {
      DrawCursor(p,(RDMarkerWaveform::CuePoints)i);
    }
  }

  delete p;

  QPainter *pa=new QPainter(this);
  pa->drawPixmap(0,0,*pix);
  wave_image=pix->convertToImage();

  delete pix;
  delete pa;
}


void RDMarkerWaveform::mouseMoveEvent(QMouseEvent *e)
{
  if((e->x()>=0)&&(e->y()>=0)) {
    emit clicked(e->x()*(double)wave_width/(double)RDMARKERWAVEFORM_WIDTH+
		 wave_start);
  }
}


void RDMarkerWaveform::mouseReleaseEvent(QMouseEvent *e)
{
  if((e->x()>=10)&&(e->y()>=0)) {
    emit clicked((e->x()-10)*(double)wave_width/(double)RDMARKERWAVEFORM_WIDTH+
		 wave_start);
  }
}


void RDMarkerWaveform::DrawCursor(QPainter *p,RDMarkerWaveform::CuePoints pt)
{
  int h=size().height();
  QPointArray array(3);

  //
  // Line
  //
  int x=XCoordinate(wave_cursors[pt])+10;
  if((x>=10)&&(x<(size().width()-10))) {
    p->setPen(RDMarkerWaveform::markerColor(pt));
    p->setBrush(RDMarkerWaveform::markerColor(pt));
    p->moveTo(x,0);
    p->lineTo(x,size().height());

    switch(wave_cursor_arrow_dirs[pt]) {
    case RDMarkerWaveform::None:
      break;

    case RDMarkerWaveform::Left:
      array.setPoint(0,x,wave_cursor_arrow_offsets[pt]);
      array.setPoint(1,x-10,wave_cursor_arrow_offsets[pt]+5);
      array.setPoint(2,x-10,wave_cursor_arrow_offsets[pt]-5);
      p->drawPolygon(array);

      array.setPoint(0,x,h-wave_cursor_arrow_offsets[pt]);
      array.setPoint(1,x-10,h-(wave_cursor_arrow_offsets[pt]+5));
      array.setPoint(2,x-10,h-(wave_cursor_arrow_offsets[pt]-5));
      p->drawPolygon(array);
      break;

    case RDMarkerWaveform::Right:
      array.setPoint(0,x,wave_cursor_arrow_offsets[pt]);
      array.setPoint(1,x+10,wave_cursor_arrow_offsets[pt]+5);
      array.setPoint(2,x+10,wave_cursor_arrow_offsets[pt]-5);
      p->drawPolygon(array);

      array.setPoint(0,x,h-wave_cursor_arrow_offsets[pt]);
      array.setPoint(1,x+10,h-(wave_cursor_arrow_offsets[pt]+5));
      array.setPoint(2,x+10,h-(wave_cursor_arrow_offsets[pt]-5));
      p->drawPolygon(array);
      break;
    }
  }
}


void RDMarkerWaveform::SetPlayCursor(int msecs)
{
  int old_x=XCoordinate(wave_cursors[RDMarkerWaveform::Play])+10;
  int x=XCoordinate(msecs)+10;
  if((x>=10)&&(x<(size().width()-10))) {
    QPainter *p=new QPainter(this);
    p->drawImage(old_x,0,wave_image,old_x,0,1,size().height());
    p->setRasterOp(Qt::NotXorROP);
    p->setPen(Qt::black);
    p->moveTo(x,0);
    p->lineTo(x,size().height());
    p->end();
    delete p;
  }
}


int RDMarkerWaveform::XCoordinate(int msecs) const
{
  return (msecs-wave_start)*(double)RDMARKERWAVEFORM_WIDTH/(double)wave_width;
}


int RDMarkerWaveform::GridIncrement() const
{
  int ret=10;
  bool odd=false;

  while((wave_width/ret)>10) {
    if(odd) {
      ret*=5;
      odd=false;
    }
    else {
      ret*=2;
      odd=true;
    }
  }
  return ret;
}
