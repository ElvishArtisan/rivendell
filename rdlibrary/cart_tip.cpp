// cart_tip.cpp
//
// Custom ToolTip for RDLibrary's Cart List
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cart_tip.cpp,v 1.2 2010/07/29 19:32:36 cvs Exp $
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

#include <rdcart.h>

#include <cart_tip.h>

CartTip::CartTip(QWidget *widget,QToolTipGroup *group)
  : QToolTip(widget,group)
{
}


void CartTip::setCartNumber(const QRect &item_rect,unsigned cartnum)
{
  RDCart *cart=new RDCart(cartnum);
  tip_notes=cart->notes();
  delete cart;
  tip_item_rect=item_rect;
  tip_cart_number=cartnum;
}


void CartTip::maybeTip(const QPoint &pt)
{
  tip(tip_item_rect,tip_notes);
}
