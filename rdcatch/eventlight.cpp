// eventlight.h
//
// Indicator light for cut events in rdcatch(1);
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "eventlight.h"

EventLight::EventLight(QWidget *parent)
  : QLabel("--",parent)
{
  setAlignment(Qt::AlignCenter);
  setFrameStyle(QFrame::Panel|QFrame::Sunken);

  d_timer=new QTimer(this);
  d_timer->setSingleShot(true);
  connect(d_timer,SIGNAL(timeout()),this,SLOT(reset()));
}


QSize EventLight::sizeHint() const
{
  return QSize(20,18);
}


QSizePolicy EventLight::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EventLight::trigger(int num)
{
  setText(QString::asprintf("%d",num));
  setStyleSheet("color: "+palette().color(QPalette::HighlightedText).name()+";"+
		"background-color: "+
		palette().color(QPalette::Highlight).name()+";");
  d_timer->start(3000);
}


void EventLight::setEnabled(bool state)
{
  QLabel::setEnabled(state);
}


void EventLight::setDisabled(bool state)
{
  setText("--");
  setStyleSheet("");
  QLabel::setDisabled(state);
}


void EventLight::reset()
{
  setStyleSheet("");
}
