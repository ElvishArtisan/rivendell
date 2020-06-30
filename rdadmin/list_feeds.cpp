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
#include <rdfeedlog.h>
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
  list_feeds_view->addColumn(tr("Key"));
  list_feeds_view->setColumnAlignment(0,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("Title"));
  list_feeds_view->setColumnAlignment(1,Qt::AlignLeft);
  list_feeds_view->addColumn(tr("Public URL"));
  list_feeds_view->setColumnAlignment(2,Qt::AlignLeft);
  list_feeds_view->addColumn(tr("Superfeed"));
  list_feeds_view->setColumnAlignment(3,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("AutoPost"));
  list_feeds_view->setColumnAlignment(4,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("Keep Metadata"));
  list_feeds_view->setColumnAlignment(5,Qt::AlignCenter|Qt::AlignVCenter);
  list_feeds_view->addColumn(tr("Creation Date"));
  list_feeds_view->setColumnAlignment(6,Qt::AlignCenter|Qt::AlignVCenter);
  QLabel *list_box_label=new QLabel(list_feeds_view,tr("&Feeds:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,11,85,19);
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
  return QSize(800,300);
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

    RDDeleteFeedLog(feed);
    delete edit_feed;
    return;
  }
  delete edit_feed;
  RDListViewItem *item=new RDListViewItem(list_feeds_view);
  item->setId(id);
  item->setText(0,feed);
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
  EditFeed *edit_feed=new EditFeed(item->text(0),this);
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

  QString feedname=item->text(0);
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
    cast->removeAudio(feed,&errs,rda->config()->logXloadDebugData());
    delete cast;
  }
  delete q;

  //
  // Delete Remote XML
  //
  if(!feed->audienceMetrics()) {
    if(!feed->deleteXml(&errs)) {
      QMessageBox::warning(this,"RDAdmin - "+tr("Warning"),
			   tr("Failed to delete remote feed XML.")+
			   "["+errs+"].");
    }
  }

  //
  // Delete Cast Entries
  //
  sql=QString().sprintf("delete from PODCASTS where FEED_ID=%d",item->id());
  RDSqlQuery::apply(sql);

  //
  // Delete Images
  //
  if(!feed->deleteImages(&errs)) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Warning"),
			 tr("Failed to delete remote images.")+
			 "["+errs+"].");
  }
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
  RDDeleteFeedLog(feedname);
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
  list_feeds_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListFeeds::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_feeds_view->clear();
  sql=QString("select ")+
    "ID,"+               // 00
    "KEY_NAME,"+         // 01
    "CHANNEL_TITLE,"+    // 02
    "IS_SUPERFEED,"+     // 03
    "ENABLE_AUTOPOST,"+  // 04
    "KEEP_METADATA,"+    // 05
    "ORIGIN_DATETIME,"+  // 06
    "BASE_URL "+         // 07
    "from FEEDS";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_feeds_view);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
    item->setText(1,q->value(2).toString());
    item->setText(2,RDFeed::publicUrl(q->value(7).toString(),
				      q->value(1).toString()));
    item->setText(3,q->value(3).toString());
    item->setText(4,q->value(4).toString());
    item->setText(5,q->value(5).toString());
    item->setText(6,q->value(6).toDateTime().toString("MM/dd/yyyy"));
  }
  delete q;
}


void ListFeeds::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "KEY_NAME,"+         // 00
    "CHANNEL_TITLE,"+    // 01
    "IS_SUPERFEED,"+     // 02
    "ENABLE_AUTOPOST,"+  // 03
    "KEEP_METADATA,"+    // 04
    "ORIGIN_DATETIME,"+  // 05
    "BASE_URL "+         // 06
    "from FEEDS where "+
    QString().sprintf("ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(0,q->value(0).toString());
    item->setText(1,q->value(1).toString());
    item->setText(2,RDFeed::publicUrl(q->value(0).toString(),
				      q->value(6).toString()));
    item->setText(3,q->value(2).toString());
    item->setText(4,q->value(3).toString());
    item->setText(5,q->value(4).toString());
    item->setText(6,q->value(5).toDateTime().toString("MM/dd/yyyy"));
  }
  delete q;
}
