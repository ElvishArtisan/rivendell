// rdcastmanager.h
//
// A RSS Feed Management Utility for Rivendell.
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

#ifndef RDCASTMANAGER_H
#define RDCASTMANAGER_H

#include <QPushButton>

#include <rdconfig.h>
#include <rdfeedlistmodel.h>
#include <rdlog_line.h>
#include <rdmainwindow.h>
#include <rdtreeview.h>
#include <rdwidget.h>

#define RDCASTMANAGER_USAGE "\n"

class MainWidget : public RDMainWindow
{
 Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void openData();
  void copyData();
  void userChangedData();
  void modelResetData();
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void feedDoubleClickedData(const QModelIndex &index);
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  
 private:
  RDTreeView *cast_feed_view;
  RDFeedListModel *cast_feed_model;
  QPushButton *cast_open_button;
  QPushButton *cast_copy_button;
  QPushButton *cast_close_button;
  bool cast_resize;
};


#endif  // RDCASTMANAGER_H
