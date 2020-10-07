// list_images.cpp
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

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>

#include <rdescape_string.h>
#include <rdupload.h>

#include "list_images.h"

ListImages::ListImages(RDImagePickerModel *model,QWidget *parent)
: RDDialog(parent)
{
  list_model=model;

  setWindowTitle("RDAdmin - "+tr("Image Manager"));

  //
  // Dialogs
  //
  list_edit_image_dialog=new EditImage(this);
  if(getenv("HOME")!=NULL) {
    list_file_dir=getenv("HOME");
  }
  else {
    list_file_dir="/";
  }

  list_view=new QListView(this);
  connect(list_view,SIGNAL(clicked(const QModelIndex &)),
	  this,SLOT(clickedData(const QModelIndex &)));
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  list_view->setModel(list_model);

  list_add_button=new QPushButton(tr("Add"),this);
  list_add_button->setFont(buttonFont());
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  list_view_button=new QPushButton(tr("View"),this);
  list_view_button->setFont(buttonFont());
  connect(list_view_button,SIGNAL(clicked()),this,SLOT(viewData()));

  list_delete_button=new QPushButton(tr("Delete"),this);
  list_delete_button->setFont(buttonFont());
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(buttonFont());
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListImages::~ListImages()
{
  delete list_close_button;
  delete list_delete_button;
  delete list_view_button;
  delete list_add_button;
  delete list_view;
  delete list_edit_image_dialog;
}


QSize ListImages::sizeHint() const
{
  return QSize(400,300);
}


int ListImages::exec(RDFeed *feed)
{
  list_feed=feed;
  list_model->setCategoryId(feed->id());

  return QDialog::exec();
}


void ListImages::addData()
{
  //  RDUpload::ErrorCode err_code;
  QStringList f0;
  int img_id=-1;
  QString err_msg="";
  QString filename=
    QFileDialog::getOpenFileName(this,
				 "RDAdmin - "+tr("Open Podcast Image File"),
				 list_file_dir,RD_PODCAST_IMAGE_FILE_FILTER);
  if(!filename.isNull()) {
    //
    // Import the image
    //
    if((img_id=list_feed->importImageFile(filename,&err_msg))<0) {
      QMessageBox::warning(this,"RDAdmin - "+tr("Import Error"),
			   tr("Image import failed.")+"\n"+
			   "["+err_msg+"].");
      return;
    }

    //
    // Upload the image
    //
    f0=filename.split(".",QString::SkipEmptyParts);
    if(!list_feed->postImage(img_id)) {
      QMessageBox::warning(this,"RDAdmin - "+tr("Upload Error"),
			   tr("Image upload failed!")+"\n"+
			   "["+err_msg+"].");
      list_feed->deleteImage(img_id,&err_msg);
      return;
    }

    //
    // Open dialog for setting the metadata
    //
    if(!list_edit_image_dialog->exec(img_id)) {
      list_feed->deleteImage(img_id,&err_msg);
      return;
    }

    list_model->refresh();

    //
    // Save import path
    //
    f0=filename.split("/",QString::SkipEmptyParts);
    f0.removeLast();
    list_file_dir=f0.join("/");
  }
}


void ListImages::viewData()
{
  int row;

  if((row=SelectedRow())>=0) {
    if(list_edit_image_dialog->exec(list_model->imageId(row))) {
      list_model->update(row);
    }
  }
}


void ListImages::deleteData()
{
  int row;
  QString err_msg="";
  QString sql;
  RDSqlQuery *q=NULL;
  int channel_ids=0;
  int channel_default_ids=0;
  int item_ids=0;

  if((row=SelectedRow())>=0) {
    sql=QString("select ")+
      "ID "+
      "from FEEDS where "+
      QString().sprintf("CHANNEL_IMAGE_ID=%d",list_model->imageId(row));
    q=new RDSqlQuery(sql);
    channel_ids=q->size();
    delete q;

    sql=QString("select ")+
      "ID "+
      "from FEEDS where "+
      QString().sprintf("DEFAULT_ITEM_IMAGE_ID=%d",list_model->imageId(row));
    q=new RDSqlQuery(sql);
    channel_default_ids=q->size();
    delete q;

    sql=QString("select ")+
      "ID "+
      "from PODCASTS where "+
      QString().sprintf("ITEM_IMAGE_ID=%d",list_model->imageId(row));
    q=new RDSqlQuery(sql);
    item_ids=q->size();
    delete q;

    if((channel_ids>0)||(channel_default_ids>0)||(item_ids>0)) {
      QString msg=tr("Image is in use as")+" ";
      if(channel_ids>0) {
	msg+=tr("a channel image")+", ";
      }
      if(channel_default_ids>0) {
	msg+=tr("a default item image")+", ";
      }
      if(item_ids>0) {
	msg+=tr("an item image")+", ";
      }
      msg=msg.left(msg.length()-2)+".";
      QMessageBox::warning(this,"RDAdmin - "+tr("Image in Use"),msg);
      return;
    }

    sql=QString("select ID from FEED_IMAGES where ")+
      QString().sprintf("ID=%d",list_model->imageId(row));
    q=new RDSqlQuery(sql);
    if(q->first()) {
      if((row=SelectedRow())>=0) {
	if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Podcast Image"),
			      tr("Are you sure you want to delete this image?"),
				 QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  return;
	}
	if(list_feed->deleteImage(list_model->imageId(row),&err_msg)) {
	  list_model->refresh();
	}
	else {
	  QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			       tr("Image deletion failed!")+"\n"+
			       "["+err_msg+"].");
	}
      }
    }
  }
}


void ListImages::clickedData(const QModelIndex &index)
{
  list_view_button->
    setDisabled(list_view->model()->data(index,Qt::DecorationRole).isNull());
  list_delete_button->
    setDisabled(list_view->model()->data(index,Qt::DecorationRole).isNull());
}


void ListImages::doubleClickedData(const QModelIndex &index)
{
  viewData();
}


void ListImages::closeData()
{
  done(true);
}


void ListImages::closeEvent(QCloseEvent *e)
{
  closeData();
}


void ListImages::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  list_model->rescaleImages(QSize(40,40));

  list_view->setGeometry(10,2,w-20,h-70);

  list_add_button->setGeometry(15,h-60,60,37);
  list_view_button->setGeometry(95,h-60,60,37);
  list_delete_button->setGeometry(175,h-60,60,37);

  list_close_button->setGeometry(w-90,h-60,80,50);
}


int ListImages::SelectedRow() const
{
  QModelIndexList indexes=list_view->selectionModel()->selectedIndexes();

  if(indexes.size()>0) {
    return indexes.at(0).row();
  }

  return -1;
}
