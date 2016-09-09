// list_feeds.cpp
//
// List Rivendell Feeds
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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


#include <QMessageBox>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdescape_string.h>
#include <rdfeedlog.h>
#include <rdpodcast.h>
#include <rdtextfile.h>

#include "add_feed.h"
#include "edit_feed.h"
#include "globals.h"
#include "list_feeds.h"

ListFeeds::ListFeeds(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Feed List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Feed List
  //
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "KEY_NAME,"+
    "CHANNEL_TITLE,"+
    "ENABLE_AUTOPOST,"+
    "KEEP_METADATA,"+
    "ORIGIN_DATETIME from FEEDS "+
    "order by KEY_NAME";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Id"));
  list_model->setHeaderData(1,Qt::Horizontal,tr("Key"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Title"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("AutoPost"));
  list_model->setHeaderData(4,Qt::Horizontal,tr("Keep Metadata"));
  list_model->setHeaderData(5,Qt::Horizontal,tr("Creation Date"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  QLabel *list_box_label=new QLabel(list_view,tr("&Feeds:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
}


ListFeeds::~ListFeeds()
{
}


QSize ListFeeds::sizeHint() const
{
  return QSize(600,280);
} 


QSizePolicy ListFeeds::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListFeeds::addData()
{
  QString feed;
  unsigned id;

  AddFeed *add_feed=new AddFeed(&id,&feed,this);
  if(add_feed->exec()<0) {
    delete add_feed;
    return;
  }
  delete add_feed;
  add_feed=NULL;

  EditFeed *edit_feed=new EditFeed(feed,this);
  if(edit_feed->exec()<0) {
    RDFeed::remove(feed);
    return;
  }
  delete edit_feed;
  list_model->update();
}


void ListFeeds::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    doubleClickedData(list_model->index(s->selectedRows()[0].row(),1));
  }
}


void ListFeeds::deleteData()
{
  QString err_str;
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    QString keyname=list_model->data(s->selectedRows()[0].row(),1).toString();
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete RSS Feed"),
			     tr("Are you sure you want to delete feed")+
			     " \""+keyname+"\"?",
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    RDFeed::remove(keyname,&err_str);
    if(!err_str.isEmpty()) {
      QMessageBox::warning(this,"RDAdmin - "+tr("Feed Removal"),err_str);
    }
    list_model->update();
  }
}

void ListFeeds::doubleClickedData(const QModelIndex &index)
{
  EditFeed *edit_feed=
    new EditFeed(list_model->data(index.row(),1).toString(),this);
  edit_feed->exec();
  delete edit_feed;
  list_model->update();
}


void ListFeeds::closeData()
{
  done(0);
}


void ListFeeds::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
