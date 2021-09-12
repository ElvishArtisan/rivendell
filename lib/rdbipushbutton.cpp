//   rdbipushbutton.cpp
//
//   QPushButton with a two-part legend
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

#include <QEvent>
#include <QPainter>

#include "rdbipushbutton.h"

RDBiPushButton::RDBiPushButton(QWidget *parent,RDConfig *c)
  : QPushButton(parent), RDFontEngine(font(),c)
{
}


RDBiPushButton::RDBiPushButton(const QString &top_text,
			       const QString &bottom_text,QWidget *parent,
			       RDConfig *c)
  : QPushButton(parent), RDFontEngine(font(),c)
{
  d_top_text=top_text;
  d_bottom_text=bottom_text;
}


QString RDBiPushButton::topText() const
{
  return d_top_text;
}


void RDBiPushButton::setTopText(const QString &str)
{
  d_top_text=str;
  update();
}


QString RDBiPushButton::bottomText() const
{
  return d_bottom_text;
}


void RDBiPushButton::setBottomText(const QString &str)
{
  d_bottom_text=str;
  update();
}


void RDBiPushButton::changeEvent(QEvent *e)
{
  if(e->type()==QEvent::EnabledChange) {
    update();
    e->accept();
  }
}


void RDBiPushButton::paintEvent(QPaintEvent *e)
{
  int w=size().width();
  int h=size().height();
  QFontMetrics *m=buttonFontMetrics();

  QPushButton::paintEvent(e);

  QPainter *p=new QPainter(this);
  if(isEnabled()) {
    p->setPen(palette().color(QPalette::Inactive,QPalette::ButtonText));
  }
  else {
    p->setPen(palette().color(QPalette::Disabled,QPalette::ButtonText));
  }
  p->setFont(buttonFont());
  p->drawText((w-m->width(d_top_text))/2,h/2-5,d_top_text);
  p->drawLine(10,h/2,w-10,h/2);
  p->drawText((w-m->width(d_bottom_text))/2,h/2+m->height(),d_bottom_text);
  p->end();
  delete p;
}
