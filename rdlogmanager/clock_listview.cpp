//   clock_listview.cpp
//
//   The Clock Carts ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <q3dragobject.h>
#include <q3header.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <QMouseEvent>

#include <rdcart.h>

#include <edit_note.h>
#include <clock_listview.h>

ClockListView::ClockListView(QWidget *parent)
  : RDListView(parent)
{
  clock_parent=parent;

  //
  // Right Button Menu
  //
  clock_menu=new Q3PopupMenu(this);
  connect(clock_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
  clock_menu->
    insertItem(tr("Edit Event"),this,SLOT(editEventData()),0,0);
}


void ClockListView::aboutToShowData()
{
  clock_menu->setItemEnabled(0,(clock_menu_item!=NULL)&&
			     (!clock_menu_item->text(4).isEmpty()));
}


void ClockListView::editEventData()
{
  emit editLine(clock_menu_item->text(4).toInt());
}


void ClockListView::contentsMousePressEvent(QMouseEvent *e)
{
  Q3ListView::contentsMousePressEvent(e);
  clock_menu_item=(RDListViewItem *)selectedItem();
  switch(e->button()) {
      case Qt::RightButton:
	clock_menu->setGeometry(clock_parent->geometry().x()+
				geometry().x()+e->pos().x()+2,
				clock_parent->geometry().y()+
				geometry().y()+e->pos().y()+
				header()->geometry().height()+2-
				contentsY(),
				clock_menu->sizeHint().width(),
				clock_menu->sizeHint().height());
	clock_menu->exec();
	break;

      default:
	e->ignore();
	break;
  }
}
