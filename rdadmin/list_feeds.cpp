// list_feeds.cpp
//
// List Rivendell Feeds
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <math.h>
#include <unistd.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdpodcast.h>
#include <rdtextfile.h>

#include "add_feed.h"
#include "edit_feed.h"
#include "globals.h"
#include "list_feeds.h"

ListFeeds::ListFeeds(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Feed List"));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Repost Button
  //
  list_repost_button=new QPushButton(this);
  list_repost_button->setFont(buttonFont());
  list_repost_button->setText(tr("&Repost"));
  connect(list_repost_button,SIGNAL(clicked()),this,SLOT(repostData()));

  //
  //  Unpost Button
  //
  list_unpost_button=new QPushButton(this);
  list_unpost_button->setFont(buttonFont());
  list_unpost_button->setText(tr("&Unpost"));
  connect(list_unpost_button,SIGNAL(clicked()),this,SLOT(unpostData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Feed List
  //
  list_feeds_view=new QTableView(this);
  list_feeds_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_feeds_view->setSelectionMode(QAbstractItemView::SingleSelection);
  list_feeds_view->setShowGrid(false);
  list_feeds_view->setSortingEnabled(false);
  list_feeds_view->setSortingEnabled(false);
  list_feeds_view->setWordWrap(false);
  list_feeds_model=new RDFeedListModel(true,this);
  list_feeds_model->setFont(font());
  list_feeds_model->setPalette(palette());
  list_feeds_view->setModel(list_feeds_model);
  list_feeds_view->setColumnHidden(2,true);
  list_box_label=new QLabel(list_feeds_view,tr("Podcast Feeds"),this);
  list_box_label->setFont(bigLabelFont());
  connect(list_feeds_model,SIGNAL(modelReset()),this,SLOT(resetModelData()));
  connect(list_feeds_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(doubleClickedData(const QModelIndex &)));
  resetModelData();
}


ListFeeds::~ListFeeds()
{
}


QSize ListFeeds::sizeHint() const
{
  return QSize(800,450);
} 


QSizePolicy ListFeeds::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListFeeds::addData()
{
  QString feed;
  unsigned id;
  QString sql;

  AddFeed *ad=new AddFeed(&id,&feed,this);
  if(ad->exec()) {
    EditFeed *d=new EditFeed(feed,this);
    if(d->exec()) {
      QModelIndex row=list_feeds_model->addFeed(feed);
      if(row.isValid()) {
	list_feeds_model->refreshRow(row);
	list_feeds_view->selectRow(row.row());
      }
    }
    else {
      sql=QString("delete from FEED_PERMS where ")+
	"KEY_NAME=\""+RDEscapeString(feed)+"\"";
      RDSqlQuery::apply(sql);

      sql=QString("delete from FEED_IMAGES where ")+
	"FEED_KEY_NAME=\""+RDEscapeString(feed)+"\"";
      RDSqlQuery::apply(sql);

      sql=QString("delete from FEEDS where ")+
	"KEY_NAME=\""+RDEscapeString(feed)+"\"";
      RDSqlQuery::apply(sql);

      return;
    }
    delete d;
  }
  delete ad;
}


void ListFeeds::editData()
{
  QModelIndexList rows=list_feeds_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditFeed *d=new EditFeed(list_feeds_model->keyName(rows.first()),this);
  if(d->exec()) {
    list_feeds_model->refreshRow(rows.first());
 }
  delete d;
}


void ListFeeds::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;
  RDFeed *feed;
  QString errs;
  QModelIndexList rows=list_feeds_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  QString feedname=list_feeds_model->keyName(rows.first());
  warning+=tr("Are you sure you want to delete feed")+" \""+feedname+"\"?";
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Feed"),warning,
			  QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  feed=new RDFeed(feedname,rda->config());

  //
  // Delete Casts
  //
  // First, Delete Remote Audio
  //
  RDPodcast *cast;
  sql=QString("select ")+
    "ID "+  // 00
    "from PODCASTS where "+
    QString().sprintf("FEED_ID=%u",list_feeds_model->feedId(rows.first()));
  q=new RDSqlQuery(sql);
  QProgressDialog *pd=
    new QProgressDialog(tr("Deleting remote audio..."),"",0,q->size()+1,this);
  pd->setWindowTitle("RDAdmin");
  pd->setValue(0);
  qApp->processEvents();
  sleep(1);
  while(q->next()) {
    pd->setValue(pd->value()+1);
    qApp->processEvents();
    cast=new RDPodcast(rda->config(),q->value(0).toUInt());
    cast->dropAudio(feed,&errs,rda->config()->logXloadDebugData());
    delete cast;
  }
  delete q;

  //
  // Delete Remote XML
  //
  if(!feed->removeRss()) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Warning"),
			 tr("Failed to delete remote feed XML."));
  }

  //
  // Delete Cast Entries
  //
  sql=QString("delete from PODCASTS where ")+
    QString().sprintf("FEED_ID=%u",list_feeds_model->feedId(rows.first()));
  RDSqlQuery::apply(sql);

  //
  // Delete Images
  //
  feed->removeAllImages();
  sql=QString("delete from FEED_IMAGES where ")+
    QString().sprintf("FEED_ID=%d",feed->id());
  RDSqlQuery::apply(sql);

  //
  // Delete Feed
  //
  sql=QString("delete from FEED_PERMS where ")+
    "KEY_NAME=\""+RDEscapeString(feedname)+"\"";
  RDSqlQuery::apply(sql);
  sql=QString("delete from SUPERFEED_MAPS where ")+
    "KEY_NAME=\""+RDEscapeString(feedname)+"\" || "+
    "MEMBER_KEY_NAME=\""+RDEscapeString(feedname)+"\"";
  RDSqlQuery::apply(sql);
  sql=QString("delete from FEEDS where ")+
    "KEY_NAME=\""+RDEscapeString(feedname)+"\"";
  RDSqlQuery::apply(sql);
  list_feeds_model->removeFeed(feedname);

  pd->reset();

  delete pd;
  delete feed;
}


void ListFeeds::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListFeeds::repostData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  RDFeed *feed=NULL;
  int count;
  QModelIndexList rows=list_feeds_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  QString keyname=list_feeds_model->keyName(rows.first());

  if(QMessageBox::question(this,"RDAdmin - "+tr("Feed Repost"),
		      tr("This operation will repost all XML, image and")+"\n"+
		      tr("audio data to the remote archive, and thus may")+"\n"+
		      tr("take signficant time to complete.")+"\n\n"+
			   tr("Continue?"),QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }

  feed=new RDFeed(keyname,rda->config(),this);
  QProgressDialog *pd=new QProgressDialog(this);
  pd->setCancelButton(NULL);

  //
  // Post Images
  //
  sql=QString("select ")+
    "ID "+              // 00
    "from FEED_IMAGES where "+
    QString().sprintf("FEED_ID=%u",feed->id());
  q=new RDSqlQuery(sql);
  pd->setLabelText(tr("Posting images..."));
  pd->setRange(0,q->size());
  count=0;
  pd->setValue(0);
  while(q->next()) {
    feed->postImage(q->value(0).toUInt());
    pd->setValue(++count);
  }
  delete q;

  //
  // Post Item Data
  //
  sql=QString("select ")+
    "ID "+              // 00
    "from PODCASTS where "+
    QString().sprintf("FEED_ID=%u",feed->id());
  q=new RDSqlQuery(sql);
  pd->setLabelText(tr("Posting item data..."));
  pd->setRange(0,q->size());
  count=0;
  pd->setValue(0);
  while(q->next()) {
    feed->postPodcast(q->value(0).toUInt());
    pd->setValue(++count);
  }
  delete q;

  //
  // Post RSS XML
  //
  pd->setLabelText(tr("Posting RSS XML data..."));
  pd->setRange(0,1);
  pd->setValue(0);
  feed->postXml();
  pd->setValue(1);

  delete pd;
}


void ListFeeds::unpostData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  RDFeed *feed=NULL;
  int count;
  QModelIndexList rows=list_feeds_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  QString keyname=list_feeds_model->keyName(rows.first());

  if(QMessageBox::question(this,"RDAdmin - "+tr("Feed Repost"),
	     tr("This operation will unpost (remove) all XML, image and")+"\n"+
	     tr("audio data from the remote archive, and thus may")+"\n"+
	     tr("take signficant time to complete.")+"\n\n"+
			   tr("Continue?"),QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }

  feed=new RDFeed(keyname,rda->config(),this);
  QProgressDialog *pd=new QProgressDialog(this);
  pd->setCancelButton(NULL);

  //
  // Remove RSS XML
  //
  pd->setLabelText(tr("Unposting RSS XML data..."));
  pd->setRange(0,1);
  pd->setValue(0);
  feed->removeRss();
  pd->setValue(1);

  //
  // Remove Item Data
  //
  sql=QString("select ")+
    "ID "+              // 00
    "from PODCASTS where "+
    QString().sprintf("FEED_ID=%u",feed->id());
  q=new RDSqlQuery(sql);
  pd->setLabelText(tr("Unposting item data..."));
  pd->setRange(0,q->size());
  count=0;
  pd->setValue(0);
  while(q->next()) {
    RDPodcast *cast=new RDPodcast(rda->config(),q->value(0).toUInt());
    cast->removePodcast();
    delete cast;
    pd->setValue(++count);

  }
  delete q;

  //
  // Remove Images
  //
  sql=QString("select ")+
    "ID "+              // 00
    "from FEED_IMAGES where "+
    QString().sprintf("FEED_ID=%u",feed->id());
  q=new RDSqlQuery(sql);
  pd->setLabelText(tr("Unposting images..."));
  pd->setRange(0,q->size());
  count=0;
  pd->setValue(0);
  while(q->next()) {
    feed->removeImage(q->value(0).toUInt());
    pd->setValue(++count);
  }
  delete q;

  delete pd;
}


void ListFeeds::closeData()
{
  done(0);
}


void ListFeeds::resetModelData()
{
  list_feeds_view->resizeColumnsToContents();
  list_feeds_view->resizeRowsToContents();
}


void ListFeeds::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_repost_button->setGeometry(size().width()-90,240,80,50);
  list_unpost_button->setGeometry(size().width()-90,300,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_box_label->setGeometry(14,11,size().width()-28,19);
  list_feeds_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
