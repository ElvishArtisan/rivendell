//   logtableview.cpp
//
//   TableView widget for RDLogEdit
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "logtableview.h"
#include "logmodel.h"

LogTableView::LogTableView(QWidget *parent)
  : RDTableView(parent)
{
  d_mouse_row=-1;

  setAcceptDrops(true);

  //
  // Mouse menu
  //
  d_mouse_menu=new QMenu(this);
  d_play_action=d_mouse_menu->
    addAction(tr("PLAY Transition"),this,SLOT(setPlayData()));
  d_play_action->setCheckable(true);
  d_segue_action=d_mouse_menu->
    addAction(tr("SEGUE Transition"),this,SLOT(setSegueData()));
  d_segue_action->setCheckable(true);
  d_stop_action=d_mouse_menu->
    addAction(tr("STOP Transition"),this,SLOT(setStopData()));
  d_stop_action->setCheckable(true);
  connect(d_mouse_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));

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


void LogTableView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::RightButton) {
    d_mouse_row=indexAt(e->pos()).row();
    if((d_mouse_row>=0)&&(d_mouse_row<(model()->rowCount()-1))) {
      d_mouse_menu->popup(e->globalPos());
    }
    else {
      d_mouse_row=-1;
    }
  }
  QTableView::mousePressEvent(e);
}


void LogTableView::aboutToShowMenuData()
{
  RDLogLine *ll=((LogModel *)model())->logLine(d_mouse_row);
  if(ll!=NULL) {
    d_play_action->setChecked(ll->transType()==RDLogLine::Play);
    d_segue_action->setChecked(ll->transType()==RDLogLine::Segue);
    d_stop_action->setChecked(ll->transType()==RDLogLine::Stop);
  }
}


void LogTableView::setPlayData()
{
  ((LogModel *)model())->setTransition(d_mouse_row,RDLogLine::Play);
}


void LogTableView::setSegueData()
{
  ((LogModel *)model())->setTransition(d_mouse_row,RDLogLine::Segue);
}


void LogTableView::setStopData()
{
  ((LogModel *)model())->setTransition(d_mouse_row,RDLogLine::Stop);
}
