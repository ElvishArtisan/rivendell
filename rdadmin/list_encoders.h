// list_encoders.h
//
// List Rivendell Encoder Profiles
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_ENCODERS_H
#define LIST_ENCODERS_H

#include <QListWidget>
#include <QPushButton>

#include <rddialog.h>
#include <rdexport_settings_dialog.h>
#include <rdlistview.h>

class ListEncoders : public RDDialog
{
  Q_OBJECT
 public:
  ListEncoders(QWidget *parent=0);
  ~ListEncoders();
  QSize sizeHint() const;
  
 public slots:
  int exec();

 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  QLabel *c_list_label;
  RDListView *c_list;
  QPushButton *c_add_button;
  QPushButton *c_edit_button;
  QPushButton *c_delete_button;
  QPushButton *c_close_button;
  RDExportSettingsDialog *c_settings_dialog;
};


#endif  // LIST_ENCODERS_H


