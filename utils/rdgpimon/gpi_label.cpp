// gpi_label.cpp
//
// A Qt-based application for testing general purpose input (GPI) devices.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "gpi_label.h"

GpiLabel::GpiLabel(QWidget *parent)
  : RDWidget(parent)
{
  gpi_line=-1;

  //
  // Line Label
  //
  gpi_line_label=new QLabel(this);
  gpi_line_label->setGeometry(0,0,59,33);
  gpi_line_label->setFont(bigButtonFont());
  gpi_line_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  gpi_line_label->setPalette(Qt::gray);
  gpi_line_label->setStyleSheet("background-color:"+QColor(Qt::gray).name());

  //
  // On Cart Label
  //
  QPalette p=palette();
  p.setColor(QPalette::Active,QPalette::Foreground,Qt::darkGreen);
  p.setColor(QPalette::Inactive,QPalette::Foreground,Qt::darkGreen);
  gpi_oncart_label=new QLabel(this);
  gpi_oncart_label->setGeometry(0,33,59,16);
  gpi_oncart_label->setFont(labelFont());
  gpi_oncart_label->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
  gpi_oncart_label->setPalette(p);

  //
  // Off Cart Label
  //
  p.setColor(QPalette::Active,QPalette::Foreground,Qt::darkRed);
  p.setColor(QPalette::Inactive,QPalette::Foreground,Qt::darkRed);
  gpi_offcart_label=new QLabel(this);
  gpi_offcart_label->setGeometry(0,49,59,16);
  gpi_offcart_label->setFont(labelFont());
  gpi_offcart_label->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
  gpi_offcart_label->setPalette(p);
}


QSize GpiLabel::sizeHint() const
{
  return QSize(59,69);
}


QSizePolicy GpiLabel::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int GpiLabel::line() const
{
  return gpi_line;
}


void GpiLabel::setLine(int line)
{
  gpi_line=line;
  gpi_line_label->setText(QString::asprintf("%d",line+1));
}


void GpiLabel::setState(bool state)
{
  if(state) {
    gpi_line_label->setPalette(Qt::green);
    gpi_line_label->
      setStyleSheet("background-color:"+QColor(Qt::green).name());
  }
  else {
    gpi_line_label->setPalette(Qt::gray);
    gpi_line_label->
      setStyleSheet("background-color:"+QColor(Qt::gray).name());
  }
}


void GpiLabel::setCart(unsigned off_cartnum,unsigned on_cartnum)
{
  if(off_cartnum==0) {
    gpi_offcart_label->setText("");
  }
  else {
    gpi_offcart_label->setText(QString::asprintf("%06u",off_cartnum));
  }
  if(on_cartnum==0) {
    gpi_oncart_label->setText("");
  }
  else {
    gpi_oncart_label->setText(QString::asprintf("%06u",on_cartnum));
  }
}


void GpiLabel::setMask(bool state)
{
  gpi_offcart_label->setEnabled(state);
  gpi_oncart_label->setEnabled(state);
}


void GpiLabel::setGeometry(int x,int y,int w,int h)
{
  QWidget::setGeometry(x,y,w,h);
}
