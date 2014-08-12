// rdemptycart.h
//
// A drag source for an empty cart.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdemptycart.h,v 1.1.2.1 2013/12/30 20:37:01 cvs Exp $
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


#ifndef RDEMPTYCART_H
#define RDEMPTYCART_H

#include <qlabel.h>
#include <qwidget.h>

#include <rdcartdrag.h>

class RDEmptyCart : public QWidget
{
  Q_OBJECT
 public:
  RDEmptyCart(QWidget *parent=0,const char *name=0);
  ~RDEmptyCart();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  QLabel *empty_label;
};


#endif  // RDEMPTYCART_H
