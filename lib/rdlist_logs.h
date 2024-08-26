// rdlist_logs.h
//
// Select a Rivendell Log
//
//   (C) Copyright 2007-2021 Fred Gleason <fredg@paravelsystems.com>
//
// The RDListLogs class creates a basic dialog that displays a list of logs
// (log name, description, and service) and allows the user to select one.  If
// user security is enabled (by passing a an RDUser object to the constructor)
// then the list of logs is filtered based on the users permissions.
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

#ifndef RDLIST_LOGS_H
#define RDLIST_LOGS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdlogfilter.h>
#include <rdloglistmodel.h>
#include <rdtableview.h>

class RDListLogs : public RDDialog
{
  Q_OBJECT
 public:
  RDListLogs(RDLogFilter::FilterMode mode,const QString &caption,
	     QWidget *parent);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *logname);

 private slots:
  void doubleClickedData(const QModelIndex &index);
  void closeEvent(QCloseEvent *);
  void modelResetData();
  void okButtonData();
  void cancelButtonData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *list_log_view;
  RDLogListModel *list_log_model;
  QString *list_logname;
  QPushButton *list_ok_button;
  QPushButton *list_cancel_button;
  RDLogFilter *list_filter_widget;
};


#endif  // RDLIST_LOGS_H
