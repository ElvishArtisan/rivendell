//   rdtrackertableview.h
//
//   The Log TableView widget for RDTrackerWidget
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

#ifndef RDTRACKERTABLEVIEW_H
#define RDTRACKERTABLEVIEW_H

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMenu>

#include <rdlog_line.h>
#include <rdtableview.h>

class RDTrackerTableView : public RDTableView
{
  Q_OBJECT
 public:
  RDTrackerTableView(QWidget *parent);

 signals:
  void cartDropped(int line,RDLogLine *ll);

 protected:
  void dragEnterEvent(QDragEnterEvent *e);
  void dragMoveEvent(QDragMoveEvent *e);
  void dropEvent(QDropEvent *e);
  void mousePressEvent(QMouseEvent *e);

 private slots:
  void aboutToShowMenuData();
  void setPlayData();
  void setSegueData();
  void setStopData();

 private:
  int d_mouse_row;
  QMenu *d_mouse_menu;
  QAction *d_play_action;
  QAction *d_segue_action;
  QAction *d_stop_action;
};


#endif  // RDTRACKERTABLEVIEW_H
