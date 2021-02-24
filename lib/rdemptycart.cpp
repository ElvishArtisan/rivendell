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

#include <QDrag>

#include "rdemptycart.h"

//
// Icons
//
#include "../icons/trashcan-16x16.xpm"
#include "../icons/trashcan-32x32.xpm"

#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>

RDEmptyCart::RDEmptyCart(QWidget *parent)
  : QWidget(parent)
{
  empty_label=new QLabel(this);
  empty_label->setPixmap(QPixmap(trashcan_32x32_xpm));
  empty_label->setGeometry(0,0,32,32);
}


RDEmptyCart::~RDEmptyCart()
{
  delete empty_label;
}


QSize RDEmptyCart::sizeHint() const
{
  return QSize(32,32);
}


QSizePolicy RDEmptyCart::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDEmptyCart::mousePressEvent(QMouseEvent *e)
{
  QWidget::mousePressEvent(e);
  QDrag *drag=new QDrag(this);
  RDCartDrag *cd=new RDCartDrag(0,"",Qt::lightGray);
  drag->setMimeData(cd);
  drag->setPixmap(QPixmap(trashcan_16x16_xpm));
  drag->exec();
}
