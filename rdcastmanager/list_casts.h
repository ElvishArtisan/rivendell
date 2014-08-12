// list_casts.h
//
// List Rivendell Casts
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_casts.h,v 1.9 2010/07/29 19:32:36 cvs Exp $
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

#ifndef LIST_CASTS_H
#define LIST_CASTS_H

#include <qdialog.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qprogressdialog.h>

#include <rdfeed.h>
#include <rdlistviewitem.h>


class ListCasts : public QDialog
{
  Q_OBJECT
 public:
  ListCasts(unsigned feed_id,QWidget *parent=0,const char *name=0);
  ~ListCasts();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addCartData();
  void addFileData();
  void editData();
  void deleteData();
  void reportData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void userChangedData();
  void filterChangedData(const QString &str);
  void notexpiredToggledData(bool state);
  void activeToggledData(bool state);
  void postProgressChangedData(int step);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  void GetEncoderId();
  RDListView *list_casts_view;
  QPushButton *list_cart_button;
  QPushButton *list_file_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_report_button;
  QPushButton *list_close_button;
  QPixmap *list_redball_map;
  QPixmap *list_greenball_map;
  QPixmap *list_whiteball_map;
  unsigned list_feed_id;
  int list_encoder_id;
  QLabel *list_filter_label;
  QLineEdit *list_filter_edit;
  QLabel *list_unexpired_label;
  QCheckBox *list_unexpired_check;
  QLabel *list_active_label;
  QCheckBox *list_active_check;
  QProgressDialog *list_progress_dialog;
  RDFeed *list_feed;
};


#endif  // LIST_CASTS_H
