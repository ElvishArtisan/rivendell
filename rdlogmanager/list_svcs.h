// list_svcs.h
//
// List Rivendell Services and Report Ages
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_svcs.h,v 1.6 2010/07/29 19:32:37 cvs Exp $
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

#ifndef LIST_SVCS_H
#define LIST_SVCS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlistview.h>
#include <qpushbutton.h>


class ListSvcs : public QDialog
{
 Q_OBJECT
 public:
  ListSvcs(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void generateData();
  void purgeData();
  void listDoubleClickedData(QListViewItem *item,const QPoint &pt,int c);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void RefreshLine(QListViewItem *item);
  QListView *list_log_list;
  QPushButton *list_generate_button;
  QPushButton *list_purge_button;
  QPushButton *list_close_button;
};


#endif

