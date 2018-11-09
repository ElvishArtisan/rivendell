//   lib_listview.cpp
//
//   A drag & drop QListView widget for Rivendell's RDLibrary
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
#include <rdlibrary.h>
#include <rdlistviewitem.h>

#include "globals.h"
#include "lib_listview.h"

LibListView::LibListView(QWidget *parent)
  : RDListView(parent)
{
  list_move_count=-1;
  list_note_bubbles_enabled=false;

  list_note_bubble=new NoteBubble(this);
  list_note_bubble->hide();
}


bool LibListView::noteBubblesEnabled() const
{
  return list_note_bubbles_enabled;
}


void LibListView::enableNoteBubbles(bool state)
{
  list_note_bubbles_enabled=state;
}


void LibListView::leaveEvent(QEvent *e)
{
  list_note_bubble->hide();
}


void LibListView::contentsMousePressEvent(QMouseEvent *e)
{
  list_move_count=3;
  Q3ListView::contentsMousePressEvent(e);
}


void LibListView::contentsMouseMoveEvent(QMouseEvent *e)
{
  RDListViewItem *item=NULL;

  Q3ListView::contentsMouseMoveEvent(e);

  //
  // Note Bubbles
  //
  if(list_note_bubbles_enabled) {
    if((item=(RDListViewItem *)itemAt(contentsToViewport(e->pos())))!=NULL) {
      unsigned cartnum=item->text(1).left(6).toUInt();
      if(cartnum!=list_note_bubble->cartNumber()) {
	list_note_bubble->setCartNumber(cartnum);
	QRect box(contentsToViewport(e->pos()).x(),
		  itemRect(item).y()+itemRect(item).height(),
		  list_note_bubble->sizeHint().width(),
		  list_note_bubble->sizeHint().height());
	if((box.x()+box.width())>width()) {
	  box.moveLeft(width()-box.width()-20);
	}
	if((box.y()+box.height())>height()) {
	  box.moveTop(height()-box.height()-20);
	}
	list_note_bubble->setGeometry(box);
      }
    }
  }

  //
  // Drag-n-Drop
  //
  list_move_count--;
  if(list_move_count==0) {
    item=(RDListViewItem *)selectedItem();

    if(item==NULL) {
      return;
    }
    if(item->text(MainWidget::OwnedBy).isEmpty()&&!item->parent()) {  // Voice tracks and cuts cannot be dragged
      RDCartDrag *d=
        new RDCartDrag(item->text(MainWidget::Cart).left(6).toUInt(),item->text(MainWidget::Title),
                       item->textColor(MainWidget::Group), this);
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
