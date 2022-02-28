// rdlogedit.h
//
// The Log Editor Utility for Rivendell.
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

#ifndef RDLOGEDIT_H
#define RDLOGEDIT_H

#include <QList>

#include <rdlog_line.h>
#include <rdlogfilter.h>
#include <rdloglistmodel.h>
#include <rdmainwindow.h>
#include <rdnotification.h>
#include <rdtableview.h>
#include <rdwidget.h>

#include "edit_log.h"
#include "voice_tracker.h"

#define RDLOGEDIT_DEFAULT_WIDTH 640
#define RDLOGEDIT_DEFAULT_HEIGHT 480
#define RDLOGEDIT_POSITION_FILENAME ".rdlogedit"
#define RDLOGEDIT_USAGE "\n"

class MainWidget : public RDMainWindow
{
 Q_OBJECT
 public:
 MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void caeConnectedData(bool state);
  void userData();
  void recentData(bool state);
  void addData();
  void editData();
  void deleteData();
  void trackData();
  void reportData();
  void filterChangedData(const QString &str);
  void selectionChangedData(const QItemSelection &selected,
			    const QItemSelection &deselected);
  void doubleClickedData(const QModelIndex &index);
  void notificationReceivedData(RDNotification *notify);
  void quitMainWidget();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void SendNotification(RDNotification::Action action,const QString &logname);
  void LockList();
  void UnlockList();
  QModelIndex SingleSelectedRow() const;
  QString log_filename;
  QString log_import_path;
  int log_card_no;
  int log_stream_no;
  RDLogFilter *log_filter_widget;
  RDTableView *log_log_view;
  RDLogListModel *log_log_model;
  QList<RDLogLine> log_clipboard;
  QPushButton *log_add_button;
  QPushButton *log_edit_button;
  QPushButton *log_delete_button;
  QPushButton *log_track_button;
  QPushButton *log_report_button;
  QPushButton *log_close_button;
  QString log_filter;
  QString log_group;
  QString log_schedcode;
  bool log_resize;
  bool log_list_locked;
  QStringList log_deleted_logs;
  EditLog *log_edit_dialog;
  VoiceTracker *log_tracker_dialog;
};


#endif  // RDLOGEDIT_H
