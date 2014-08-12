// gpi_label.cpp
//
// A Qt-based application for testing general purpose input (GPI) devices.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_label.cpp,v 1.5 2011/05/27 21:28:25 cvs Exp $
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


#include <gpi_label.h>


GpiLabel::GpiLabel(QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  gpi_line=-1;

  //
  // Generate Fonts
  //
  QFont line_font("Helvetica",18,QFont::Bold);
  line_font.setPixelSize(18);
  QFont cart_font("Helvetica",12,QFont::Bold);
  cart_font.setPixelSize(12);

  //
  // Line Label
  //
  gpi_line_label=new QLabel(this);
  gpi_line_label->setGeometry(0,0,59,33);
  gpi_line_label->setFont(line_font);
  gpi_line_label->setAlignment(AlignHCenter|AlignVCenter);
  gpi_line_label->setPalette(gray);

  //
  // On Cart Label
  //
  QPalette p=palette();
  p.setColor(QPalette::Active,QColorGroup::Foreground,darkGreen);
  p.setColor(QPalette::Inactive,QColorGroup::Foreground,darkGreen);
  p.setColor(QPalette::Disabled,QColorGroup::Foreground,darkGreen);
  gpi_oncart_label=new QLabel(this);
  gpi_oncart_label->setGeometry(0,33,59,16);
  gpi_oncart_label->setFont(cart_font);
  gpi_oncart_label->setAlignment(AlignHCenter|AlignTop);
  gpi_oncart_label->setPalette(p);

  //
  // Off Cart Label
  //
  p.setColor(QPalette::Active,QColorGroup::Foreground,darkRed);
  p.setColor(QPalette::Inactive,QColorGroup::Foreground,darkRed);
  p.setColor(QPalette::Disabled,QColorGroup::Foreground,darkRed);
  gpi_offcart_label=new QLabel(this);
  gpi_offcart_label->setGeometry(0,49,59,16);
  gpi_offcart_label->setFont(cart_font);
  gpi_offcart_label->setAlignment(AlignHCenter|AlignTop);
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
  gpi_line_label->setText(QString().sprintf("%d",line+1));
}


void GpiLabel::setState(bool state)
{
  if(state) {
    gpi_line_label->setPalette(green);
  }
  else {
    gpi_line_label->setPalette(gray);
  }
}


void GpiLabel::setCart(unsigned off_cartnum,unsigned on_cartnum)
{
  if(off_cartnum==0) {
    gpi_offcart_label->setText("");
  }
  else {
    gpi_offcart_label->setText(QString().sprintf("%06u",off_cartnum));
  }
  if(on_cartnum==0) {
    gpi_oncart_label->setText("");
  }
  else {
    gpi_oncart_label->setText(QString().sprintf("%06u",on_cartnum));
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
