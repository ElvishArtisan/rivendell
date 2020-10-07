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
#include <rdfeed.h>
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
  c_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  c_description_label->setFont(labelFont());
  c_description_edit=new QLineEdit(this);
  c_description_edit->setMaxLength(191);

  c_url_label=new QLabel(tr("URL")+":",this);
  c_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  c_url_label->setFont(labelFont());
  c_url_edit=new QLineEdit(this);
  c_url_edit->setReadOnly(true);

  c_size_label=new QLabel(tr("Native Size")+":",this);
  c_size_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  c_size_label->setFont(labelFont());
  c_size_value_label=new QLabel(this);
  c_size_value_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  c_extension_label=new QLabel(tr("Type")+":",this);
  c_extension_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  c_extension_label->setFont(labelFont());
  c_extension_value_label=new QLabel(this);
  c_extension_value_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  c_ok_button=new QPushButton(tr("OK"),this);
  c_ok_button->setFont(buttonFont());
  connect(c_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  c_cancel_button=new QPushButton(tr("Cancel"),this);
  c_cancel_button->setFont(buttonFont());
  connect(c_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  setMinimumSize(sizeHint());
}


EditImage::~EditImage()
{
  delete c_image_label;
  delete c_description_label;
  delete c_description_edit;
  delete c_url_label;
  delete c_url_edit;
  delete c_size_label;
  delete c_size_value_label;
  delete c_extension_label;
  delete c_extension_value_label;
  delete c_ok_button;
  delete c_cancel_button;
}


QSize EditImage::sizeHint() const
{
  return QSize(600,722);
}


int EditImage::exec(int img_id)
{
  QString sql;
  RDSqlQuery *q=NULL;

  c_image_id=img_id;

  sql=QString("select ")+
    "FEED_IMAGES.DESCRIPTION,"+     // 00
    "FEED_IMAGES.FILE_EXTENSION,"+  // 01
    "FEED_IMAGES.WIDTH,"+           // 02
    "FEED_IMAGES.HEIGHT,"+          // 03
    "FEED_IMAGES.DEPTH,"+           // 04
    "FEED_IMAGES.DATA,"+            // 05
    "FEED_IMAGES.FEED_ID,"+         // 06
    "FEEDS.BASE_URL "+              // 07
    "from FEED_IMAGES left join FEEDS "+
    "on FEED_IMAGES.FEED_ID=FEEDS.ID where "+
    QString().sprintf("FEED_IMAGES.ID=%d",img_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    c_description_edit->setText(q->value(0).toString());
    c_url_edit->setText(q->value(7).toString()+"/"+
			RDFeed::imageFilename(q->value(6).toInt(),img_id,
					      q->value(1).toString()));
    c_extension_value_label->setText(q->value(1).toString().toUpper());
    c_size_value_label->
      setText(QString().sprintf("%dx%d",q->value(2).toInt(),
				q->value(3).toInt()));
    c_image=QImage();
    c_image.loadFromData(q->value(5).toByteArray());

    QSize fsize=FittedSize(c_image.size());
    c_image_label->setPixmap(QPixmap::fromImage(c_image.
						scaled(fsize,
						       Qt::KeepAspectRatio)));
    //    resize(EDIT_IMAGE_WIDTH_OFFSET+fsize.width(),
    //	   EDIT_IMAGE_HEIGHT_OFFSET+fsize.height());
    resize(sizeHint());
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

  c_description_label->setGeometry(10,h-109,120,20);
  c_description_edit->setGeometry(135,h-109,w-145,20);

  c_url_label->setGeometry(10,h-87,120,20);
  c_url_edit->setGeometry(135,h-87,w-145,20);

  c_size_label->
    setGeometry(140,h-65,80,20);
  c_size_value_label->setGeometry(225,h-65,80,20);

  c_extension_label->setGeometry(300,h-65,50,20);
  c_extension_value_label->setGeometry(355,h-65,100,20);

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
