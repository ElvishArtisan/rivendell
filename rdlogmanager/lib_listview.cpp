//   lib_listview.cpp
//
//   The Library ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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
//

#include <rdcartdrag.h>

#include <lib_listview.h>

LibListView::LibListView(QWidget *parent)
  : Q3ListView(parent)
{
}


void LibListView::focusOutEvent(QFocusEvent *e)
{
  Q3ListViewItem *item=selectedItem();
  if(item==NULL) {
    return;
  }
  setSelected(item,false);
}


void LibListView::contentsMousePressEvent(QMouseEvent *e)
{
  Q3ListView::contentsMousePressEvent(e);
  Q3ListViewItem *item=selectedItem();
  if(item==NULL) {
    return;
  }
  RDCartDrag *d=new RDCartDrag(item->text(1).toUInt(),item->pixmap(0),
			       Qt::lightGray,this);
  d->dragCopy();

  emit clicked(item);
}
