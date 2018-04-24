//   catch_listview.h
//
//   Events List Widget for RDCatch
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CATCH_LISTVIEW_H
#define CATCH_LISTVIEW_H

#include <q3popupmenu.h>
//Added by qt3to4:
#include <QMouseEvent>

#include <rdlistview.h>

class CatchListView : public RDListView
{
  Q_OBJECT

 public:
  CatchListView(QWidget *parent);

 private slots:
  void aboutToShowData();
  void editAudioMenuData();

 protected:
  void contentsMousePressEvent(QMouseEvent *e);
  void contentsMouseDoubleClickEvent(QMouseEvent *e);

 private:
  Q3PopupMenu *catch_menu;
  Q3ListViewItem *catch_menu_item;
  QString catch_cutname;
  QWidget *catch_parent;
};


#endif  // CATCH_LISTVIEW_H
