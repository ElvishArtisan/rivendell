// rdlogedit.h
//
// The Log Editor Utility for Rivendell.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <vector>

#include <q3mainwindow.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <q3listview.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qlabel.h>

#include <rdlog_line.h>
#include <rdlogfilter.h>
#include <rdnotification.h>

#include "list_listviewitem.h"

#define RDLOGEDIT_USAGE "\n"

class MainWidget : public Q3MainWindow
{
 Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void connectedData(bool state);
  void userData();
  void recentData(bool state);
  void addData();
  void editData();
  void deleteData();
  void trackData();
  void reportData();
  void filterChangedData(const QString &str);
  void logSelectionChangedData();
  void logDoubleclickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void notificationReceivedData(RDNotification *notify);
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshItem(ListListViewItem *item);
  void RefreshList();
  unsigned SelectedLogs(std::vector<ListListViewItem *> *items,
			int *tracks=NULL) const;
  void SendNotification(RDNotification::Action action,const QString &logname);
  void LockList();
  void UnlockList();
  QString log_filename;
  QString log_import_path;
  QLabel *log_user_label;
  int log_card_no;
  int log_stream_no;
  RDLogFilter *log_filter_widget;
  Q3ListView *log_log_list;
  std::vector<RDLogLine> log_clipboard;
  QPushButton *log_add_button;
  QPushButton *log_edit_button;
  QPushButton *log_delete_button;
  QPushButton *log_track_button;
  QPushButton *log_report_button;
  QPushButton *log_close_button;
  QPixmap *log_rivendell_map;
  QPixmap *log_greencheckmark_map;
  QPixmap *log_redx_map;
  QPixmap *log_whiteball_map;
  QPixmap *log_greenball_map;
  QPixmap *log_redball_map;
  QString log_filter;
  QString log_group;
  QString log_schedcode;
  bool log_resize;
  bool log_list_locked;
  QStringList log_deleted_logs;
};


#endif  // RDLOGEDIT_H
