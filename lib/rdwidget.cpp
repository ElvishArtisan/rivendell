// rdwidget.cpp
//
// Base class for Rivendell modal widgets.
//
//   (C) Copyright 2019-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPainter>

#include "rdwidget.h"

RDWidget::RDWidget(QWidget *parent,Qt::WindowFlags f)
  : QWidget(parent,f), RDFontEngine(font())
{
  setFont(defaultFont());
}


RDWidget::RDWidget(RDConfig *config,QWidget *parent,Qt::WindowFlags f)
  : QWidget(parent,f), RDFontEngine(font(),config)
{
  setFont(defaultFont());
}


QPixmap RDWidget::backgroundPixmap() const
{
  return d_background_pixmap;
}


void RDWidget::setBackgroundPixmap(const QPixmap &pix)
{
  d_background_pixmap=pix;
  update();
}


void RDWidget::paintEvent(QPaintEvent *e)
{
  int w=size().width();
  int h=size().height();

  if(!d_background_pixmap.isNull()) {
    QPainter *p=new QPainter(this);
    for(int i=0;i<h;i+=d_background_pixmap.height()) {
      for(int j=0;j<w;j+=d_background_pixmap.width()) {
	p->drawPixmap(j,i,d_background_pixmap);
      }
    }
    delete p;
  }
}
