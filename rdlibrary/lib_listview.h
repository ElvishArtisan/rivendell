//   lib_listview.h
//
//   A drag & drop QListView widget for Rivendell's RDLibrary
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIB_LISTVIEW_H
#define LIB_LISTVIEW_H

#include <rdlistview.h>

#include "notebubble.h"

class LibListView : public RDListView
{
  Q_OBJECT
 public:
  LibListView(QWidget *parent);
  bool noteBubblesEnabled() const;

 public slots:
  void enableNoteBubbles(bool state);

 protected:
  void leaveEvent(QEvent *e);
  void contentsMousePressEvent(QMouseEvent *e);
  void contentsMouseMoveEvent(QMouseEvent *e);
  void contentsMouseReleaseEvent(QMouseEvent *e);

 private:
  NoteBubble *list_note_bubble;
  bool list_note_bubbles_enabled;
  int list_move_count;
};


#endif  // LIB_LISTVIEW_H
