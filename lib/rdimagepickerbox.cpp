// rdimagepickerbox.cpp
//
// ComboBox for selecting images
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

#include "rdimagepickerbox.h"

RDImagePickerBox::RDImagePickerBox(const QString &tbl_name,
				   const QString &cat_id_col,
				   const QString &img_id_col,QWidget *parent)
  : QComboBox(parent)
{
  c_model=new RDImagePickerModel(tbl_name,cat_id_col,img_id_col,this);
  setModel(c_model);

  setCurrentIndex(0);
}


RDImagePickerBox::~RDImagePickerBox()
{
  delete c_model;
}


int RDImagePickerBox::currentImageId() const
{
  if(currentIndex()<0) {
    return -1;
  }
  return c_model->imageId(currentIndex());
}


void RDImagePickerBox::setCurrentImageId(int img_id)
{
  setCurrentIndex(c_model->imageRowOfId(img_id));
}


void RDImagePickerBox::refresh()
{
  int current_id=currentImageId();

  c_model->refresh();
  setCurrentImageId(current_id);
}


void RDImagePickerBox::setCategoryId(int id)
{
  c_model->setCategoryId(id);
  setCurrentIndex(0);
}


void RDImagePickerBox::resizeEvent(QResizeEvent *e)
{
  int index=currentIndex();

  QSize img_size(size().height()-4,size().height()-4);
  c_model->rescaleImages(img_size);
  setIconSize(img_size);

  setCurrentIndex(index);
}
