// list_casts.h
//
// List Rivendell Casts
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCheckBox>
#include <QProgressDialog>

#include <rdcut_dialog.h>
#include <rddialog.h>
#include <rdfeed.h>
#include <rdlist_logs.h>
#include <rdpodcastlistmodel.h>
#include <rdpodcastfilter.h>
#include <rdtableview.h>

#include "render_dialog.h"

class ListCasts : public RDDialog
{
  Q_OBJECT
 public:
  ListCasts(unsigned feed_id,QWidget *parent=0);
  ~ListCasts();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addCartData();
  void addFileData();
  void addLogData();
  void editData();
  void deleteData();
  void doubleClickedData(const QModelIndex &index);
  void userChangedData();
  void filterChangedData(const QString &str);
  void modelResetData();
  void rowsInsertedData(const QModelIndex &parent,int start,int end);
  void postProgressChangedData(int step);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDCutDialog *list_cut_dialog;
  QLabel *list_casts_label;
  RDTableView *list_casts_view;
  RDPodcastListModel *list_casts_model;
  QPushButton *list_cart_button;
  QPushButton *list_file_button;
  QPushButton *list_log_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
  unsigned list_feed_id;
  RDPodcastFilter *list_casts_filter;
  QProgressDialog *list_progress_dialog;
  RenderDialog *list_render_dialog;
  RDListLogs *list_listlogs_dialog;
  RDFeed *list_feed;
};


#endif  // LIST_CASTS_H
