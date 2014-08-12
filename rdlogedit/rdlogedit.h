// rdlogedit.h
//
// The Log Editor Utility for Rivendell.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogedit.h,v 1.28.4.1 2014/01/08 23:32:50 cvs Exp $
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

#include <qmainwindow.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qlabel.h>

#include <rduser.h>
#include <rdripc.h>
#include <rdlibrary_conf.h>
#include <rdlog_line.h>

#include <list_listviewitem.h>


class MainWidget : public QMainWindow
{
 Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0,WFlags f=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void connectedData(bool state);
  void userData();
  void addData();
  void editData();
  void deleteData();
  void trackData();
  void reportData();
  void filterChangedData(const QString &str);
  void filterClearedData();
  void logDoubleclickedData(QListViewItem *item,const QPoint &pt,int col);
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshItem(ListListViewItem *item);
  void RefreshList();
  QSqlDatabase *log_db;
  QString log_filename;
  QString log_import_path;
  QLabel *log_user_label;
  int log_card_no;
  int log_stream_no;
  QLabel *log_service_label;
  QComboBox *log_service_box;
  QLabel *log_filter_label;
  QLineEdit *log_filter_edit;
  QPushButton *log_filter_button;
  QListView *log_log_list;
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
};


#endif  // RDLOGEDIT_H
