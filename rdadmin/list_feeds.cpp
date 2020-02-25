// list_feeds.cpp
//
// List Rivendell Feeds
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <q3progressdialog.h>

#include <qlabel.h>
#include <qmessagebox.h>
#include <qapplication.h>

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
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

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
  list_feeds_view->addColumn(tr("Key"));
  list_feeds_view->setColumnAlignment(0,Qt::AlignCenter);
  list_feeds_view->addColumn(tr("Title"));
  list_feeds_view->setColumnAlignment(1,Qt::AlignLeft);
  list_feeds_view->addColumn(tr("AutoPost"));
  list_feeds_view->setColumnAlignment(2,Qt::AlignCenter);
  list_feeds_view->addColumn(tr("Keep Metadata"));
  list_feeds_view->setColumnAlignment(3,Qt::AlignCenter);
  list_feeds_view->addColumn(tr("Creation Date"));
  list_feeds_view->setColumnAlignment(4,Qt::AlignCenter);
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
  return QSize(400,280);
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
  RDSqlQuery *q;

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
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString("delete from FEEDS where ")+
      "KEY_NAME=\""+RDEscapeString(feed)+"\"";
    q=new RDSqlQuery(sql);
    delete q;
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
  Q3ProgressDialog *pd=
    new Q3ProgressDialog(tr("Deleting Audio..."),tr("Cancel"),q->size()+1,this,
			 NULL);
  pd->setCaption(tr("Deleting"));
  pd->setProgress(0);
  qApp->processEvents();
  sleep(1);
  while(q->next()) {
    pd->setProgress(pd->progress()+1);
    qApp->processEvents();
    cast=new RDPodcast(rda->config(),q->value(0).toUInt());
    cast->removeAudio(feed,&errs,rda->config()->logXloadDebugData());
    delete cast;
  }
  delete q;

  //
  // Delete Cast Entries
  //
  sql=QString().sprintf("delete from PODCASTS where FEED_ID=%d",item->id());
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Feed
  //
  sql=QString("delete from FEED_PERMS where ")+
    "KEY_NAME=\""+RDEscapeString(feedname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from FEEDS where ")+
    "KEY_NAME=\""+RDEscapeString(feedname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
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
  q=new 
    RDSqlQuery("select ID,KEY_NAME,CHANNEL_TITLE,ENABLE_AUTOPOST,\
                KEEP_METADATA,ORIGIN_DATETIME from FEEDS");
  while (q->next()) {
    item=new RDListViewItem(list_feeds_view);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
    item->setText(1,q->value(2).toString());
    item->setText(2,q->value(3).toString());
    item->setText(3,q->value(4).toString());
    item->setText(4,q->value(5).toDateTime().toString("MM/dd/yyyy"));
  }
  delete q;
}


void ListFeeds::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select KEY_NAME,CHANNEL_TITLE,ENABLE_AUTOPOST,\
                         KEEP_METADATA,ORIGIN_DATETIME from FEEDS \
                         where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(0,q->value(0).toString());
    item->setText(1,q->value(1).toString());
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
    item->setText(4,q->value(4).toDateTime().toString("MM/dd/yyyy"));
  }
  delete q;
}
