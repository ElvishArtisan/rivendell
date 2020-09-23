// list_images.h
//
// Manage a collection of pixmap images
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

#ifndef LIST_IMAGES_H
#define LIST_IMAGES_H

#include <qlistview.h>
#include <qpushbutton.h>

#include <rddelete.h>
#include <rddialog.h>
#include <rdfeed.h>
#include <rdimagepickermodel.h>

#include "edit_image.h"

class ListImages : public RDDialog
{
 Q_OBJECT
 public:
  ListImages(RDImagePickerModel *model,QWidget *parent=0);
  ~ListImages();
  QSize sizeHint() const;
  
 public slots:
  int exec(RDFeed *feed);

 private slots:
  void addData();
  void viewData();
  void deleteData();
  void clickedData(const QModelIndex &index);
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  int SelectedRow() const;
  EditImage *list_edit_image_dialog;
  QListView *list_view;
  RDImagePickerModel *list_model;
  RDFeed *list_feed;
  QPushButton *list_add_button;
  QPushButton *list_view_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
  QString list_file_dir;
};


#endif  // LIST_IMAGES_H
