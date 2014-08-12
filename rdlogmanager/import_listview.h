//   import_listview.h
//
//   The Import Carts ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: import_listview.h,v 1.14 2010/07/29 19:32:37 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
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

#ifndef IMPORT_LISTVIEW_H
#define IMPORT_LISTVIEW_H

#include <qlistview.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

#include <rdlog_event.h>

class ImportListView : public QListView
{
  Q_OBJECT

 public:
  ImportListView(QWidget *parent,const char *name=0);
  void setForceTrans(RDLogLine::TransType trans);
  void setAllowStop(bool state);
  void setAllowFirstTrans(bool state);
  RDLogEvent *logEvent();
  void refreshList(int line=-1);
  void validateTransitions();

 signals:
  void sizeChanged(int size);
  void lengthChanged(int msecs);

 private slots:
  void aboutToShowData();
  void insertNoteMenuData();
  void editNoteMenuData();
  void insertTrackMenuData();
  void editTrackMenuData();
  void playMenuData();
  void segueMenuData();
  void stopMenuData();
  void deleteMenuData();

 protected:
  void contentsMousePressEvent(QMouseEvent *e);
  void contentsMouseDoubleClickEvent(QMouseEvent *e);
  void focusOutEvent(QFocusEvent *e);
  void dragEnterEvent(QDragEnterEvent *e);
  void dropEvent(QDropEvent *e);

 private:
  RDLogEvent *import_log;
  QPixmap *import_playout_map;
  QPixmap *import_macro_map;
  QPixmap *import_notemarker_map;
  QPixmap *import_mic16_map;
  QPopupMenu *import_menu;
  int import_menu_line;
  RDLogLine *import_menu_logline;
  QListViewItem *import_menu_item;
  RDLogLine::TransType import_force_trans;
  bool import_allow_stop;
  bool import_allow_first_trans;
  QWidget *import_parent;
};


#endif  // IMPORT_LISTVIEW_H
