//   import_listview.h
//
//   The Import Carts ListView widget for RDLogManager.
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

#ifndef IMPORT_LISTVIEW_H
#define IMPORT_LISTVIEW_H

#include <q3listview.h>
#include <qpixmap.h>
#include <qmenu.h>

#include <rdeventimportlist.h>

class ImportListView : public Q3ListView
{
  Q_OBJECT

 public:
  ImportListView(QWidget *parent);
  RDEventImportList *eventImportList() const;
  void setAllowFirstTrans(bool state);
  void move(int from_line,int to_line);
  void setEventName(const QString &str);
  bool load(const QString &event_name,RDEventImportList::ImportType type);
  void save(RDLogLine::TransType first_trans);
  void refreshList(int line=-1);
  void fixupTransitions(RDLogLine::TransType repl_trans);

 signals:
  void sizeChanged(int size);
  void lengthChanged(int msecs);
  void validationNeeded();

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
  void dragEnterEvent(QDragEnterEvent *e);
  void dropEvent(QDropEvent *e);

 private:
  RDEventImportList *import_list;
  QPixmap *import_playout_map;
  QPixmap *import_macro_map;
  QPixmap *import_notemarker_map;
  QPixmap *import_mic16_map;
  QMenu *import_menu;
  int import_menu_line;
  RDEventImportItem *import_menu_i_item;
  Q3ListViewItem *import_menu_item;
  bool import_allow_first_trans;
  QWidget *import_parent;
};


#endif  // IMPORT_LISTVIEW_H
