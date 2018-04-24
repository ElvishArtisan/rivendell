//   lib_listview.cpp
//
//   A drag & drop QListView widget for Rivendell's RDLibrary
//
//   (C) Copyright 2002-2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <globals.h>
#include <rdlistviewitem.h>
#include <lib_listview.h>
//Added by qt3to4:
#include <QMouseEvent>

LibListView::LibListView(QWidget *parent)
  : RDListView(parent)
{
  list_move_count=-1;
}


void LibListView::contentsMousePressEvent(QMouseEvent *e)
{
  list_move_count=3;
  Q3ListView::contentsMousePressEvent(e);
}


void LibListView::contentsMouseMoveEvent(QMouseEvent *e)
{
  Q3ListView::contentsMouseMoveEvent(e);
  list_move_count--;
  if(list_move_count==0) {
    RDListViewItem *item=(RDListViewItem *)selectedItem();

    if(item==NULL) {
      return;
    }
    if(item->text(21).isEmpty()) {  // Voice tracks cannot be dragged
      RDCartDrag *d=
	new RDCartDrag(item->text(1).toUInt(),item->text(4),item->textColor(2),
		       this);
      d->dragCopy();
      emit clicked(item);
    }
  }
}


void LibListView::contentsMouseReleaseEvent(QMouseEvent *e)
{
  list_move_count=-1;
  Q3ListView::contentsMouseReleaseEvent(e);
}
