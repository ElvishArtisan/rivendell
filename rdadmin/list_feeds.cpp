// list_feeds.cpp
//
// List Rivendell Feeds
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

//
// Icons
//
#include "../icons/rdcastmanager-32x32.xpm"

ListFeeds::ListFeeds(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Feed List"));

  //
  // Create Icons
  //
  list_rdcastmanager_32x32_map=new QPixmap(rdcastmanager_32x32_xpm);

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
  // Group List
  //
  list_feeds_view=new RDListView(this);
  list_feeds_view->setAllColumnsShowFocus(true);
  list_feeds_view->setItemMargin(5);

  list_feeds_view->addColumn(" ");
  list_feeds_view->setColumnAlignment(0,Qt::AlignCenter|Qt::AlignVCenter);

  list_feeds_view->addColumn(tr("Key"));
  list_feeds_view->setColumnAlignment(1,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("Title"));
  list_feeds_view->setColumnAlignment(2,Qt::AlignLeft);
  list_feeds_view->addColumn(tr("Public URL"));
  list_feeds_view->setColumnAlignment(3,Qt::AlignLeft);
  list_feeds_view->addColumn(tr("Superfeed"));
  list_feeds_view->setColumnAlignment(4,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("AutoPost"));
  list_feeds_view->setColumnAlignment(5,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("Creation Date"));
  list_feeds_view->setColumnAlignment(6,Qt::AlignCenter|Qt::AlignVCenter);
  list_box_label=new QLabel(list_feeds_view,tr("Podcast Feeds"),this);
  list_box_label->setFont(bigLabelFont());
  connect(list_feeds_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();
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

  AddFeed *add_feed=new AddFeed(&id,&feed,this);
  if(add_feed->exec()<0) {
    delete add_feed;
    return;
  }
  delete add_feed;
  add_feed=NULL;

  EditFeed *edit_feed=new EditFeed(feed,this);
  if(edit_feed->exec()<0) {
    sql=QString("delete from FEED_PERMS where ")+
      "KEY_NAME=\""+RDEscapeString(feed)+"\"";
    RDSqlQuery::apply(sql);

    sql=QString("delete from FEED_IMAGES where ")+
      "FEED_KEY_NAME=\""+RDEscapeString(feed)+"\"";
    RDSqlQuery::apply(sql);

    sql=QString("delete from FEEDS where ")+
      "KEY_NAME=\""+RDEscapeString(feed)+"\"";
    RDSqlQuery::apply(sql);

    delete edit_feed;
    return;
  }
  delete edit_feed;
  RDListViewItem *item=new RDListViewItem(list_feeds_view);
  item->setId(id);
  item->setText(1,feed);
  RefreshItem(item);
  item->setSelected(true);
  list_feeds_view->setCurrentItem(item);
  list_feeds_view->ensureItemVisible(item);
}


void ListFeeds::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_feeds_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditFeed *edit_feed=new EditFeed(item->text(1),this);
  edit_feed->exec();
  delete edit_feed;
  RefreshItem(item);
}


void ListFeeds::deleteData()
{
  RDListViewItem *item=(RDListViewItem *)list_feeds_view->selectedItem();
  if(item==NULL) {
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;
  RDFeed *feed;
  QString errs;

  QString feedname=item->text(1);
  if(feedname.isEmpty()) {
    return;
  }
  warning+=tr("Are you sure you want to delete feed")+" \""+feedname+"\"?";
  switch(QMessageBox::warning(this,tr("Delete Feed"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
      case QMessageBox::No:
      case QMessageBox::NoButton:
	return;

      default:
	break;
  }
  feed=new RDFeed(feedname,rda->config());

  //
  // Delete Casts
  //
  // First, Delete Remote Audio
  //
  RDPodcast *cast;
  sql=QString().sprintf("select ID from PODCASTS where FEED_ID=%d",item->id());
  q=new RDSqlQuery(sql);
  QProgressDialog *pd=new QProgressDialog(tr("Deleting remote audio..."),"",
					  0,q->size()+1,this);
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
  sql=QString().sprintf("delete from PODCASTS where FEED_ID=%d",item->id());
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
  item->setSelected(false);

  pd->reset();

  delete pd;
  delete feed;
  delete item;
}


void ListFeeds::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListFeeds::repostData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  RDFeed *feed=NULL;
  int count;

  RDListViewItem *item=(RDListViewItem *)list_feeds_view->selectedItem();
  if(item==NULL) {
    return;
  }
  QString keyname=item->text(1);
  if(keyname.isEmpty()) {
    return;
  }

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

  RDListViewItem *item=(RDListViewItem *)list_feeds_view->selectedItem();
  if(item==NULL) {
    return;
  }
  QString keyname=item->text(1);
  if(keyname.isEmpty()) {
    return;
  }

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


void ListFeeds::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_feeds_view->clear();
  sql=QString("select ")+
    "FEEDS.ID,"+               // 00
    "FEEDS.KEY_NAME,"+         // 01
    "FEEDS.CHANNEL_TITLE,"+    // 02
    "FEEDS.IS_SUPERFEED,"+     // 03
    "FEEDS.ENABLE_AUTOPOST,"+  // 04
    "FEEDS.ORIGIN_DATETIME,"+  // 05
    "FEEDS.BASE_URL,"+         // 06
    "FEED_IMAGES.DATA "+       // 07
    "from FEEDS left join FEED_IMAGES "+
    "on FEEDS.CHANNEL_IMAGE_ID=FEED_IMAGES.ID";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_feeds_view);
    item->setId(q->value(0).toInt());
    if(q->value(7).isNull()) {
      item->setPixmap(0,*list_rdcastmanager_32x32_map);
    }
    else {
      QImage img=QImage::fromData(q->value(7).toByteArray());
      item->setPixmap(0,QPixmap::fromImage(img.scaled(32,32)));
    }
    item->setText(1,q->value(1).toString());
    item->setText(2,q->value(2).toString());
    item->setText(3,RDFeed::publicUrl(q->value(6).toString(),
				      q->value(1).toString()));
    item->setText(4,q->value(3).toString());
    item->setText(5,q->value(4).toString());
    item->setText(6,q->value(5).toDateTime().toString("MM/dd/yyyy"));
  }
  delete q;
}


void ListFeeds::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "FEEDS.KEY_NAME,"+         // 00
    "FEEDS.CHANNEL_TITLE,"+    // 01
    "FEEDS.IS_SUPERFEED,"+     // 02
    "FEEDS.ENABLE_AUTOPOST,"+  // 03
    "FEEDS.ORIGIN_DATETIME,"+  // 04
    "FEEDS.BASE_URL,"+         // 05
    "FEED_IMAGES.DATA "+       // 06
    "from FEEDS left join FEED_IMAGES "+
    "on FEEDS.CHANNEL_IMAGE_ID=FEED_IMAGES.ID where "+
    QString().sprintf("FEEDS.ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->next()) {
    if(q->value(6).isNull()) {
      item->setPixmap(0,*list_rdcastmanager_32x32_map);
    }
    else {
      QImage img=QImage::fromData(q->value(6).toByteArray());
      item->setPixmap(0,QPixmap::fromImage(img.scaled(32,32)));
    }
    item->setText(1,q->value(0).toString());
    item->setText(2,q->value(1).toString());
    item->setText(3,RDFeed::publicUrl(q->value(5).toString(),
				      q->value(0).toString()));
    item->setText(4,q->value(2).toString());
    item->setText(5,q->value(3).toString());
    item->setText(6,q->value(4).toDateTime().toString("MM/dd/yyyy"));
  }
  delete q;
}
