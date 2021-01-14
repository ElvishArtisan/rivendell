// rd3emptycart.h
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

#ifndef RD3EMPTYCART_H
#define RD3EMPTYCART_H

#include <qlabel.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QMouseEvent>

#include <rd3cartdrag.h>

class RD3EmptyCart : public QWidget
{
  Q_OBJECT
 public:
  RD3EmptyCart(QWidget *parent=0);
  ~RD3EmptyCart();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  QLabel *empty_label;
};


#endif  // RD3EMPTYCART_H
