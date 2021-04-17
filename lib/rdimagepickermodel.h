// rdimagepickermodel.h
//
// One-dimensional model for picking images
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

#ifndef RDIMAGEPICKERMODEL_H
#define RDIMAGEPICKERMODEL_H

#include <QAbstractListModel>
#include <qlist.h>
#include <qpixmap.h>
#include <qstringlist.h>

class RDImagePickerModel : public QAbstractListModel
{
  Q_OBJECT;
 public:
  RDImagePickerModel(const QString &tbl_name,const QString &cat_id_col,
		     QObject *parent=0);
  ~RDImagePickerModel();
  int categoryId() const;
  int imageId(int row) const;
  int imageRowOfId(int img_id) const;
  void rescaleImages(const QSize &size);
  void update(int row);
  void refresh();
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole);

 public slots:
  void setCategoryId(int id);


 private:
  void LoadRows(int cat_id,const QSize &img_size);
  QString c_table_name;
  QString c_category_column;
  int c_category_id;
  QSize c_image_size;
  QList<QPixmap *> c_images;
  QStringList c_descriptions;
  QList<int> c_image_ids;
};


#endif  // RDIMAGEPICKERMODEL_H
