// rdimagepickermodel.cpp
//
// One-dimensional model for picking images
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

#include <qimage.h>

#include "rddb.h"
#include "rdimagepickermodel.h"

RDImagePickerModel::RDImagePickerModel(const QString &tbl_name,
				       const QString &cat_id_col,
				       const QString &img_id_col,
				       QObject *parent)
  : QAbstractListModel(parent)
{
  c_table_name=tbl_name;
  c_category_column=cat_id_col;
  c_image_column=img_id_col;
  c_category_id=-1;;
  c_image_size=QSize(100,100);
}


RDImagePickerModel::~RDImagePickerModel()
{
  for(int i=0;i<c_images.size();i++) {
    delete c_images.at(i);
  }
  c_images.clear();
}


int RDImagePickerModel::categoryId() const
{
  return c_category_id;
}


int RDImagePickerModel::imageId(int row) const
{
  if(row<0) {
    return -1;
  }
  return c_image_ids.at(row);
}


int RDImagePickerModel::imageRowOfId(int img_id) const
{
  if(img_id<0) {
    return -1;
  }
  return c_image_ids.indexOf(img_id);
}


void RDImagePickerModel::rescaleImages(const QSize &size)
{
  if(size!=c_image_size) {
    LoadRows(c_category_id,size);
    c_image_size=size;
  }
}


void RDImagePickerModel::update(int row)
{
  QString sql;
  RDSqlQuery *q=NULL;

  sql=QString("select ")+
    "DESCRIPTION,"+     // 00
    "FILE_EXTENSION,"+  // 01
    "WIDTH,"+           // 02
    "HEIGHT "+          // 03
    "from FEED_IMAGES where "+
    QString().sprintf("ID=%d",c_image_ids.at(row));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    c_descriptions[row]=q->value(0).toString()+"\n"+
      +"["+q->value(1).toString().toUpper()+", "+
      QString().sprintf("%dx%d]",q->value(2).toInt(),q->value(3).toInt());
    emit dataChanged(createIndex(row,0),createIndex(row,0));
  }
  delete q;
}


void RDImagePickerModel::refresh()
{
  LoadRows(c_category_id,c_image_size);
}


int RDImagePickerModel::rowCount(const QModelIndex &parent) const
{
  return c_images.size();
}


QVariant RDImagePickerModel::data(const QModelIndex &index,int role) const
{
  if(index.column()==0) {
    if(role==Qt::DisplayRole) {
      return QVariant(c_descriptions.at(index.row()));
    }
    if(role==Qt::DecorationRole) {
      if(c_images.at(index.row())!=NULL) {
	return QVariant(*(c_images.at(index.row())));
      }
    }
    if(role==Qt::SizeHintRole) {
      return QVariant(QSize(200,50));
    }
  }

  return QVariant();
}


QVariant RDImagePickerModel::headerData(int section,Qt::Orientation orient,
					int role)
{
  if((orient==Qt::Horizontal)&&(section==0)) {
    if(role==Qt::DisplayRole) {
      return QVariant(tr("Image"));
    }
  }

  return QVariant();
}


void RDImagePickerModel::setCategoryId(int id)
{
  if(id!=c_category_id) {
    LoadRows(id,c_image_size);
    c_category_id=id;
  }
}


void RDImagePickerModel::LoadRows(int cat_id,const QSize &img_size)
{
  QString sql;
  RDSqlQuery *q=NULL;
  QImage img;

  //
  // Clear stale data
  //
  if(c_images.size()>0) {
    beginRemoveRows(QModelIndex(),0,c_images.size()-1);
    for(int i=0;i<c_images.size();i++) {
      delete c_images.at(i);
    }
    c_images.clear();
    c_descriptions.clear();
    c_image_ids.clear();
    endRemoveRows();
  }

  //
  // Load new data
  //
  sql=QString("select ")+
    "ID,"+              // 00
    "DESCRIPTION,"+     // 01
    "FILE_EXTENSION,"+  // 02
    "WIDTH,"+           // 02
    "HEIGHT,"+          // 03
    "DATA "+            // 04
    "from "+c_table_name+" where "+
    c_category_column+QString().sprintf("=%d ",cat_id)+
    "order by DESCRIPTION";
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    beginInsertRows(QModelIndex(),0,q->size()-1);
    while(q->next()) {
      c_image_ids.push_back(q->value(0).toUInt());
      c_descriptions.
	push_back(q->value(1).toString()+"\n"+
		  "["+q->value(2).toString().toUpper()+", "+
		  QString().sprintf("%dx%d]",
				    q->value(3).toInt(),q->value(4).toInt()));
      img.loadFromData(q->value(5).toByteArray());
      c_images.push_back(new QPixmap(QPixmap::fromImage(img.scaled(img_size,
			   Qt::KeepAspectRatio,Qt::SmoothTransformation))));
    }
    endInsertRows();
  }
  delete q;
}
