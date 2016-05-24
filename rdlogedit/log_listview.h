//   log_listview.h
//
//   The Log ListView widget for RDLogEdit.
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

#ifndef LOG_LISTVIEW_H
#define LOG_LISTVIEW_H

#include <qlistview.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

#include <rdlistview.h>
#include <rdlog_event.h>
#include <rdlog_line.h>

#define TRACKER_MAX_LINENO 2147483647

class LogListView : public RDListView
{
  Q_OBJECT

 public:
  LogListView(QWidget *parent);

 private slots:
  void aboutToShowData();
  void playData();
  void segueData();
  void stopData();

 signals:
  void transitionChanged(int line,RDLogLine::TransType trans);

 protected:
  void contentsMousePressEvent(QMouseEvent *e);

 private:
  QPopupMenu *log_menu;
  RDListViewItem *log_menu_item;
  QWidget *log_parent;
  QString log_transition_type;
};


#endif  // LOG_LISTVIEW_H
