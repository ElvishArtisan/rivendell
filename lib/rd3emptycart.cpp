// rdemptycart.cpp
//
// A drag source for an empty cart.
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rd3emptycart.h"

//
// Icons
//
#include "../icons/trashcan-32x32.xpm"
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>

RD3EmptyCart::RD3EmptyCart(QWidget *parent)
  : QWidget(parent)
{
  empty_label=new QLabel(this);
  empty_label->setPixmap(QPixmap(trashcan_xpm));
  empty_label->setGeometry(0,0,32,32);
}


RD3EmptyCart::~RD3EmptyCart()
{
  delete empty_label;
}


QSize RD3EmptyCart::sizeHint() const
{
  return QSize(32,32);
}


QSizePolicy RD3EmptyCart::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RD3EmptyCart::mousePressEvent(QMouseEvent *e)
{
  QWidget::mousePressEvent(e);
  RD3CartDrag *d=new RD3CartDrag(0,"",Qt::lightGray,this);
  d->dragCopy();
}
