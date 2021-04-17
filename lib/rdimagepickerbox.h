// rdimagepickerbox.h
//
// ComboBox for selecting images
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDIMAGEPICKERBOX_H
#define RDIMAGEPICKERBOX_H

#include <qcombobox.h>

#include <rdimagepickermodel.h>

class RDImagePickerBox : public QComboBox
{
  Q_OBJECT;
 public:
  RDImagePickerBox(const QString &tbl_name,const QString &cat_id_col,
		   QWidget *parent=0);
  ~RDImagePickerBox();
  int currentImageId() const;
  void refresh();

 public slots:
  void setCurrentImageId(int img_id);
  void setCategoryId(int id);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDImagePickerModel *c_model;
};


#endif  // RDIMAGEPICKERBOX_H
