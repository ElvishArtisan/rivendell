// clocklistview.cpp
//
// RDTableView widget with consistent Rivendell defaults
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDragEnterEvent>
#include <QHeaderView>

#include <rdcartdrag.h>
#include <rdclockmodel.h>

#include "edit_note.h"
#include "edit_track.h"
#include "clocklistview.h"

ClockListView::ClockListView(QWidget *parent)
  :RDTableView(parent)
{
  d_mouse_row=-1;

  //
  // Mouse menu
  //
  d_mouse_menu=new QMenu(this);

  d_edit_event_action=d_mouse_menu->
    addAction(tr("Edit Event"),this,SLOT(editEventData()));
  d_edit_event_action->setCheckable(false);

  connect(d_mouse_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));
}


void ClockListView::aboutToShowMenuData()
{
  RDClockModel *mod=(RDClockModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->rowCount())) {
    d_edit_event_action->setEnabled(false);
    return;
  }
  d_edit_event_action->setEnabled(true);
}


void ClockListView::editEventData()
{
  emit editEventAtLine(d_mouse_row);
}


void ClockListView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::RightButton) {
    d_mouse_row=indexAt(e->pos()).row();
    if((d_mouse_row>=0)&&(d_mouse_row<model()->rowCount())) {
      d_mouse_menu->popup(e->globalPos());
    }
    else {
      d_mouse_row=-1;
    }
  }
  QTableView::mousePressEvent(e);
}
