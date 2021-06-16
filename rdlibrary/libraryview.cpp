// libraryview.cpp
//
// QTreeView widget that supports cart dragging.
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

#include <rdcartdrag.h>
#include <rdlibrarymodel.h>

#include "libraryview.h"

LibraryView::LibraryView(QWidget *parent)
  : RDTreeView(parent)
{
}


void LibraryView::mousePressEvent(QMouseEvent *e)
{
  QTreeView::mousePressEvent(e);

  if(dragEnabled()&&(e->button()==Qt::LeftButton)) {
    RDLibraryModel *mod=(RDLibraryModel *)model();
    QModelIndex index=indexAt(e->pos());
    QDrag *drag=new QDrag(this);
    RDCartDrag *cd=
      new RDCartDrag(mod->cartNumber(index),
		     mod->data(mod->index(index.row(),4)).toString(),
		     mod->data(mod->index(index.row(),1),Qt::TextColorRole).
		     value<QColor>());
    drag->setMimeData(cd);
    drag->setPixmap(mod->data(mod->index(index.row(),0),Qt::DecorationRole).
		    value<QPixmap>());
    drag->exec();
  }
}
