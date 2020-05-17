// edit_image.cpp
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

#include <rddb.h>
#include <rdescape_string.h>

#include <qapplication.h>
#include <qdesktopwidget.h>

#include "edit_image.h"

EditImage::EditImage(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDAdmin - "+tr("Image Viewer"));
  setMinimumWidth(200);
  setMinimumHeight(150);

  c_image_label=new QLabel(this);

  c_description_label=new QLabel(tr("Description")+":",this);
  c_description_label->setAlignment(Qt::AlignRight);
  c_description_label->setFont(labelFont());
  c_description_edit=new QLineEdit(this);

  c_ok_button=new QPushButton(tr("OK"),this);
  c_ok_button->setFont(buttonFont());
  connect(c_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  c_cancel_button=new QPushButton(tr("Cancel"),this);
  c_cancel_button->setFont(buttonFont());
  connect(c_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


EditImage::~EditImage()
{
  delete c_image_label;
  delete c_description_label;
  delete c_description_edit;
  delete c_ok_button;
  delete c_cancel_button;
}


QSize EditImage::sizeHint() const
{
  return QSize(400,300);
}


int EditImage::exec(int img_id)
{
  QString sql;
  RDSqlQuery *q=NULL;

  c_image_id=img_id;

  sql=QString("select ")+
    "DESCRIPTION,"+  // 00
    "WIDTH,"+        // 01
    "HEIGHT,"+       // 02
    "DEPTH,"+        // 03
    "DATA "+         // 04
    "from FEED_IMAGES where "+
    QString().sprintf("ID=%d",img_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    c_description_edit->setText(q->value(0).toString());
    c_image=QImage();
    c_image.loadFromData(q->value(4).toByteArray());

    QSize fsize=FittedSize(c_image.size());
    c_image_label->setPixmap(QPixmap::fromImage(c_image.
						scaled(fsize,
						       Qt::KeepAspectRatio)));
    resize(EDIT_IMAGE_WIDTH_OFFSET+fsize.width(),
	   EDIT_IMAGE_HEIGHT_OFFSET+fsize.height());
  }
  delete q;
  
  return QDialog::exec();
}


void EditImage::okData()
{
  QString sql=QString("update FEED_IMAGES set ")+
    "DESCRIPTION=\""+RDEscapeString(c_description_edit->text())+"\" "+
    QString().sprintf("where ID=%d",c_image_id);
  RDSqlQuery::apply(sql);
   
  done(true);
}


void EditImage::cancelData()
{
  done(false);
}


void EditImage::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditImage::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  c_image_label->
    setGeometry(10,2,w-EDIT_IMAGE_WIDTH_OFFSET,h-EDIT_IMAGE_HEIGHT_OFFSET);
  c_image_label->setPixmap(QPixmap::fromImage(c_image.
			 scaled(c_image_label->size(),Qt::KeepAspectRatio)));

  c_description_label->setGeometry(10,h-87,120,20);
  c_description_edit->setGeometry(135,h-87,w-145,20);

  c_ok_button->setGeometry(w-180,h-60,80,50);
  c_cancel_button->setGeometry(w-90,h-60,80,50);
}


QSize EditImage::FittedSize(const QSize &img_size) const
{
  QSize max_size=MaxFriendlyImageSize();

  if((img_size.width()<=max_size.width())&&
     (img_size.height()<=max_size.height())) {
    return img_size;
  }
  QSize ret(img_size.boundedTo(max_size));
  if(ret.height()==max_size.height()) {
    ret.setWidth(img_size.width()*ret.height()/img_size.height());
  }
  else {
    ret.setHeight(img_size.height()*ret.width()/img_size.width());
  }

  return ret;
}


QSize EditImage::MaxFriendlyImageSize() const
{
  QDesktopWidget *dt=QApplication::desktop();
  QSize dsize(dt->screenGeometry(dt->screenNumber(this)).size());

  return QSize(dsize.width()-EDIT_IMAGE_WIDTH_OFFSET,
	       dsize.height()-EDIT_IMAGE_HEIGHT_OFFSET-100);
}
