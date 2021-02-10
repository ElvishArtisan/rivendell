// clocklistview.h
//
// RDTableView widget for clock events in rdlogmanager.
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

#ifndef CLOCKLISTVIEW_H
#define CLOCKLISTVIEW_H

#include <QAction>
#include <QMenu>

#include <rdtableview.h>

class ClockListView : public RDTableView
{
  Q_OBJECT
 public:
  ClockListView(QWidget *parent=0);

 signals:
  void editEventAtLine(int line);

 private slots:
  void aboutToShowMenuData();
  void editEventData();

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  int d_mouse_row;
  QMenu *d_mouse_menu;
  QAction *d_edit_event_action;
};


#endif  // CLOCKLISTVIEW_H
