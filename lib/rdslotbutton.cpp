// rdslotbutton.cpp
//
//   Start/stop button for rdcartslots(1).
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QBitmap>
#include <QPainter>

#include "rdslotbutton.h"

RDSlotButton::RDSlotButton(int slotnum,QWidget *parent,RDConfig *c)
  : QPushButton(parent), RDFontEngine(c)
{
  d_slot_number=slotnum;

  //
  // Palettes
  //
  d_ready_color=
    QPalette(QColor(BUTTON_STOPPED_BACKGROUND_COLOR),
	     palette().color(QPalette::Background));
  d_playing_color=
    QPalette(QColor(BUTTON_PLAY_BACKGROUND_COLOR),
	     palette().color(QPalette::Background));
  setFocusPolicy(Qt::NoFocus);
}


int RDSlotButton::slotNumber() const
{
  return d_slot_number;
}


QString RDSlotButton::portLabel() const
{
  return d_port_label;
}


void RDSlotButton::setState(int id,RDPlayDeck::State state)
{
  switch(state) {
  case RDPlayDeck::Playing:
    setPalette(d_playing_color);
    WriteKeycap();
    break;

  case RDPlayDeck::Stopped:
  case RDPlayDeck::Finished:
    setPalette(d_ready_color);
    WriteKeycap();
    break;

  case RDPlayDeck::Stopping:
  case RDPlayDeck::Paused:
    break;
  }
}


void RDSlotButton::setPortLabel(const QString &str)
{
  if(str!=d_port_label) {
    d_port_label=str;
    WriteKeycap();
  }
}


void RDSlotButton::resizeEvent(QResizeEvent *e)
{
  setIconSize(QSize(size().width()-4,size().height()-4));
  WriteKeycap();
}


void RDSlotButton::WriteKeycap()
{
  int w=size().width();
  int h=size().height();

  QPixmap *pix=new QBitmap(w-4,h-4);
  QPainter *p=new QPainter(pix);
  p->setRenderHint(QPainter::Antialiasing,true);
  p->setRenderHint(QPainter::SmoothPixmapTransform,true);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);
  p->fillRect(0,0,w,h,palette().color(QPalette::Background));

  p->setFont(hugeButtonFont());
  p->drawText((w-p->fontMetrics().width(QString().
					sprintf("%d",1+d_slot_number)))/2,
	      p->fontMetrics().height(),
	      QString().sprintf("%d",1+d_slot_number));

  p->setFont(bigLabelFont());
  p->drawText((w-p->fontMetrics().width(d_port_label))/2,
	      3*h/4,
	      d_port_label);

  p->end();
  setIcon(*pix);
  
  delete p;
  delete pix;
}
