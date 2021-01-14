// libraryview.cpp
//
// QTreeView widget that supports cart dragging.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdlibrarymodel.h"

#include "libraryview.h"

LibraryView::LibraryView(QWidget *parent)
  : QTreeView(parent)
{
}


void LibraryView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::LeftButton) {
    QDrag *drag=new QDrag(this);
    QMimeData *mime=new QMimeData();

    RDLibraryModel *mod=(RDLibraryModel *)model();
    QModelIndex index=indexAt(e->pos());

    QString str="[Rivendell-Cart]\n";
    str+="Number="+QString().sprintf("%06u",mod->cartNumber(index))+"\n";
    QColor color=mod->data(mod->index(index.row(),1),Qt::TextColorRole).value<QColor>();
    if(color.isValid()) {
      str+="Color="+color.name()+"\n";
    }

    QString title=mod->data(mod->index(index.row(),4),Qt::DisplayRole).toString();
    if(!title.isEmpty()) {
      str+="ButtonText="+title+"\n";
    }
    printf("DRAG: %s\n",str.toUtf8().constData());
    mime->setText(str);

  }
  QTreeView::mousePressEvent(e);

}
