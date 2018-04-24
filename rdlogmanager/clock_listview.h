//   clock_listview.h
//
//   The Clock ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CLOCK_LISTVIEW_H
#define CLOCK_LISTVIEW_H

#include <q3listview.h>
#include <qpixmap.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QMouseEvent>

#include <rdlistview.h>
#include <rdlog_event.h>

class ClockListView : public RDListView
{
  Q_OBJECT

 public:
  ClockListView(QWidget *parent);

 signals:
  void editLine(int count);

 private slots:
  void aboutToShowData();
  void editEventData();

 protected:
  void contentsMousePressEvent(QMouseEvent *e);

 private:
  Q3PopupMenu *clock_menu;
  RDListViewItem *clock_menu_item;
  QWidget *clock_parent;
};


#endif  // CLOCK_LISTVIEW_H
