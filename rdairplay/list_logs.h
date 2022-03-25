// list_logs.h
//
// Select a Rivendell Log
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPushButton>

#include <rddialog.h>
#include <rdlogfilter.h>
#include <rdloglistmodel.h>
#include <rdloglock.h>
#include <rdlogplay.h>
#include <rdtableview.h>

class ListLogs : public RDDialog
{
  Q_OBJECT

 public:
  enum Operation {Load=0,Cancel=1,Save=2,SaveAs=3,Unload=4};
  ListLogs(RDLogPlay *log,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
   int exec(QString *logname,QString *svcname,RDLogLock **loglock);

 private slots:
  void doubleClickedData(const QModelIndex &index);
  void closeEvent(QCloseEvent *);
  void loadButtonData();
  void saveButtonData();
  void saveAsButtonData();
  void unloadButtonData();
  void cancelButtonData();
  void modelResetData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  bool TryLock(RDLogLock *lock);
  RDLogFilter *list_filter_widget;
  RDTableView *list_log_view;
  RDLogListModel *list_log_model;
  QString *list_logname;
  QString *list_svcname;
  QPushButton *list_load_button;
  QPushButton *list_unload_button;
  QPushButton *list_save_button;
  QPushButton *list_saveas_button;
  QPushButton *list_cancel_button;
  RDLogPlay *list_log;
  RDLogLock **list_log_lock;
};


#endif  // LIST_LOGS_H
