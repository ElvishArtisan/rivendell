// rdcastmanager.h
//
// A RSS Feed Management Utility for Rivendell.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastmanager.h,v 1.6 2010/07/29 19:32:36 cvs Exp $
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

#include <vector>

#include <qmainwindow.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qsqldatabase.h>
#include <qpushbutton.h>
#include <qpixmap.h>

#include <rdlistview.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdlibrary_conf.h>
#include <rdlog_line.h>
#include <rdconfig.h>


class MainWidget : public QMainWindow
{
 Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0,WFlags f=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void openData();
  void userChangedData();
  void feedDoubleclickedData(QListViewItem *item,const QPoint &pt,int col);
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshItem(RDListViewItem *item);
  void RefreshList();
  RDListView *cast_feed_list;
  QPixmap *cast_rivendell_map;
  QPixmap *cast_greencheckmark_map;
  QPixmap *cast_redx_map;
  QPushButton *cast_open_button;
  QPushButton *cast_close_button;
};


#endif  // RDCASTMANAGER_H
