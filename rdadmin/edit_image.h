// edit_image.h
//
// View a pixmap image and modify its metadata
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

#ifndef EDIT_IMAGE_H
#define EDIT_IMAGE_H

#include <qimage.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <rddialog.h>

#define EDIT_IMAGE_WIDTH_OFFSET 20
#define EDIT_IMAGE_HEIGHT_OFFSET 95

class EditImage : public RDDialog
{
 Q_OBJECT
 public:
  EditImage(QWidget *parent=0);
  ~EditImage();
  QSize sizeHint() const;
  
 public slots:
  int exec(int img_id);

 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  QSize FittedSize(const QSize &img_size) const;
  QSize MaxFriendlyImageSize() const;
  QLabel *c_image_label;
  QLabel *c_description_label;
  QLineEdit *c_description_edit;
  QLabel *c_url_label;
  QLineEdit *c_url_edit;
  QLabel *c_size_label;
  QLabel *c_size_value_label;
  QLabel *c_extension_label;
  QLabel *c_extension_value_label;
  QPushButton *c_ok_button;
  QPushButton *c_cancel_button;
  QImage c_image;
  int c_image_id;
};


#endif  // EDIT_IMAGE_H
