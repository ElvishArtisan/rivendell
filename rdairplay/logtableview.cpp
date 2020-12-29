//   logtableview.cpp
//
//   TableView widget for RDAirPlay
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdlogplay.h>

#include "logtableview.h"

LogTableView::LogTableView(QWidget *parent)
  : QTableView(parent)
{
  setAcceptDrops(true);
}


void LogTableView::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e));
}


void LogTableView::dragMoveEvent(QDragMoveEvent *e)
{
  e->accept(RDCartDrag::canDecode(e));
}


void LogTableView::dropEvent(QDropEvent *e)
{
  RDLogLine ll;
  int line=-1;
  int y_pos=e->pos().y();

  if(RDCartDrag::decode(e,&ll)) {
    line=rowAt(y_pos);
    emit cartDropped(line,&ll);
  }
}
