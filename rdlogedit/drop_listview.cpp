//   drop_listview.cpp
//
//   The Log ListView widget for RDLogEdit.
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: drop_listview.cpp,v 1.1.2.1 2013/12/27 22:12:28 cvs Exp $
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

#include <qheader.h>

#include <rdcartdrag.h>

#include <drop_listview.h>

DropListView::DropListView(QWidget *parent,const char *name)
  : RDListView(parent,name)
{
  setAcceptDrops(true);
}


void DropListView::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e));
}


void DropListView::dropEvent(QDropEvent *e)
{
  RDLogLine ll;
  int line=-1;
  QPoint pos(e->pos().x(),e->pos().y()-header()->sectionRect(0).height());

  if(RDCartDrag::decode(e,&ll)) {
    RDListViewItem *item=(RDListViewItem *)itemAt(pos);
    if(item!=NULL) {
      line=item->text(13).toInt();
    }
    emit cartDropped(line,&ll);
  }
}
