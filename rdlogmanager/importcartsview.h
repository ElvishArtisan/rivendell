// importcartsview.h
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

#ifndef IMPORTCARTSVIEW_H
#define IMPORTCARTSVIEW_H

#include <QAction>
#include <QMenu>

#include <rdlog_line.h>
#include <rdtableview.h>

class ImportCartsView : public RDTableView
{
  Q_OBJECT
 public:
  ImportCartsView(QWidget *parent=0);

 signals:
  void cartDropped(int line,RDLogLine *ll);

 private slots:
  void aboutToShowMenuData();
  void insertNoteData();
  void insertTrackData();
  void editData();
  void deleteData();
  void setPlayData();
  void setSegueData();

 protected:
  void dragEnterEvent(QDragEnterEvent *e);
  void dragMoveEvent(QDragMoveEvent *e);
  void dropEvent(QDropEvent *e);
  void mousePressEvent(QMouseEvent *e);

 private:
  int d_mouse_row;
  QMenu *d_mouse_menu;
  QAction *d_insert_note_action;
  QAction *d_insert_track_action;
  QAction *d_edit_action;
  QAction *d_delete_action;
  QAction *d_set_play_action;
  QAction *d_set_segue_action;
};


#endif  // IMPORTCARTSVIEW_H
