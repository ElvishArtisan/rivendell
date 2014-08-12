// list_logs.h
//
// Select a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_logs.h,v 1.5 2010/07/29 19:32:37 cvs Exp $
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

#ifndef LIST_LOGS_H
#define LIST_LOGS_H

#include <qsqldatabase.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qpushbutton.h>


class ListLogs : public QDialog
{
  Q_OBJECT

 public:
  ListLogs(QString *logname,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void doubleClickedData(QListViewItem *,const QPoint &,int);
  void closeEvent(QCloseEvent *);
  void okButtonData();
  void cancelButtonData();

 private:
  void RefreshList();
  QListView *list_log_list;
  QString *list_logname;
};


#endif 
