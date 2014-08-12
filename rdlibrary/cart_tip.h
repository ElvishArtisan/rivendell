// cart_tip.h
//
// Custom ToolTip for RDLibrary's Cart List
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cart_tip.h,v 1.2 2010/07/29 19:32:36 cvs Exp $
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

#ifndef CART_TIP_H
#define CART_TIP_H

#include <qtooltip.h>

class CartTip : public QToolTip
{
 public:
  CartTip(QWidget *widget,QToolTipGroup *group=0);
  void setCartNumber(const QRect &item_rect,unsigned cartnum);

 protected:
  void maybeTip(const QPoint &pt);

 private:
  unsigned tip_cart_number;
  QRect tip_item_rect;
  QString tip_notes;
};

#endif  // CART_TIP_H
