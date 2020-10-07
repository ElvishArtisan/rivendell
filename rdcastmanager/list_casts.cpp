// list_casts.cpp
//
// List Rivendell Casts
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

#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <rdcastsearch.h>
#include <rdconf.h>
#include <rdcut_dialog.h>
#include <rdescape_string.h>
#include <rdlist_logs.h>
#include <rdpodcast.h>

#include "edit_cast.h"
#include "globals.h"
#include "list_casts.h"

//
// Icons
//
#include "../icons/blueball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"
#include "../icons/whiteball.xpm"
#include "../icons/rdcastmanager-32x32.xpm"

ListCasts::ListCasts(unsigned feed_id,bool is_super,QWidget *parent)
  : RDDialog(parent)
{
  list_feed_id=feed_id;
  list_is_superfeed=is_super;

  setWindowTitle("RDCastManager - "+tr("Podcast Item List")+
		 "  [Feed ID: "+QString().sprintf("%u",feed_id)+"]");

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Create Icons
  //
  list_blueball_map=new QPixmap(blueball_xpm);
  list_greenball_map=new QPixmap(greenball_xpm);
  list_redball_map=new QPixmap(redball_xpm);
  list_whiteball_map=new QPixmap(whiteball_xpm);
  list_rdcastmanager_32x32_map=new QPixmap(rdcastmanager_32x32_xpm);

  //
  // Notifications
  //
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));

  //
  // The Feed
  //
  list_feed=new RDFeed(feed_id,rda->config(),this);

  //
  // Dialogs
  //
  list_render_dialog=new RenderDialog(this);

  list_progress_dialog=
    new QProgressDialog(tr("Uploading Audio..."),tr("Cancel"),0,1,this);
  list_progress_dialog->setWindowTitle("RDCastManager - "+tr("Progress"));
  list_progress_dialog->setCancelButton(NULL);
  list_progress_dialog->setMinimumDuration(0);
  connect(list_feed,SIGNAL(postProgressChanged(int)),
	  this,SLOT(postProgressChangedData(int)));
  connect(list_feed,SIGNAL(postProgressRangeChanged(int,int)),
	  list_progress_dialog,SLOT(setRange(int,int)));

  //
  // Filter
  //
  list_filter_edit=new QLineEdit(this);
  list_filter_label=
    new QLabel(list_filter_edit,tr("Filter:"),this);
  list_filter_label->setFont(labelFont());
  list_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(list_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Active Check Box
  //
  list_active_check=new QCheckBox(this);
  list_active_label=
    new QLabel(list_active_check,tr("Only Show Active Items"),this);
  list_active_label->setFont(labelFont());
  list_active_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(list_active_check,SIGNAL(toggled(bool)),
	  this,SLOT(activeToggledData(bool)));

  //
  // Group List
  //
  list_casts_label=
    new QLabel(list_feed->channelTitle(),this);
  list_casts_label->setFont(bigLabelFont());
  list_casts_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  list_casts_view=new RDListView(this);
  list_casts_view->setAllColumnsShowFocus(true);
  list_casts_view->setItemMargin(5);
  list_casts_view->addColumn(tr(" "));
  list_casts_view->setColumnAlignment(0,Qt::AlignCenter);
  list_casts_view->addColumn(tr("Title"));
  list_casts_view->setColumnAlignment(1,Qt::AlignLeft);

  list_casts_view->addColumn(tr("Status"));
  list_casts_view->setColumnAlignment(2,Qt::AlignCenter);

  list_casts_view->addColumn(tr("Start"));
  list_casts_view->setColumnAlignment(3,Qt::AlignLeft);
  list_casts_view->addColumn(tr("Expiration"));
  list_casts_view->setColumnAlignment(4,Qt::AlignCenter);
  list_casts_view->addColumn(tr("Length"));
  list_casts_view->setColumnAlignment(5,Qt::AlignRight);
  list_casts_view->addColumn(tr("Feed"));
  list_casts_view->setColumnAlignment(6,Qt::AlignLeft);
  list_casts_view->addColumn(tr("Category"));
  list_casts_view->setColumnAlignment(7,Qt::AlignCenter);
  list_casts_view->addColumn(tr("Posted By"));
  list_casts_view->setColumnAlignment(8,Qt::AlignLeft);
  list_casts_view->addColumn(tr("SHA1"));
  list_casts_view->setColumnAlignment(9,Qt::AlignLeft);
  if(!list_feed->isSuperfeed()) {
    connect(list_casts_view,
	    SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	    this,
	    SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));
  }

  //
  //  Post Cart Button
  //
  list_cart_button=new QPushButton(this);
  list_cart_button->setFont(buttonFont());
  list_cart_button->setText(tr("Post From\nCar&t/Cut"));
  list_cart_button->setDisabled(list_feed->isSuperfeed());
  connect(list_cart_button,SIGNAL(clicked()),this,SLOT(addCartData()));

  //
  //  Post File Button
  //
  list_file_button=new QPushButton(this);
  list_file_button->setFont(buttonFont());
  list_file_button->setText(tr("Post From\n&File"));
  list_file_button->setDisabled(list_feed->isSuperfeed());
  connect(list_file_button,SIGNAL(clicked()),this,SLOT(addFileData()));

  //
  //  Post Log Button
  //
  list_log_button=new QPushButton(this);
  list_log_button->setFont(buttonFont());
  list_log_button->setText(tr("Post From\nLog"));
  list_log_button->setDisabled(list_feed->isSuperfeed());
  connect(list_log_button,SIGNAL(clicked()),this,SLOT(addLogData()));

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

  RefreshList();

  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userChangedData()));

  userChangedData();
}


ListCasts::~ListCasts()
{
  delete list_render_dialog;
  delete list_progress_dialog;
  delete list_feed;
}


QSize ListCasts::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListCasts::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListCasts::addCartData()
{
  QString cutname;
  RDCutDialog *cd=
    new RDCutDialog(&cutname,"RDCastManager",&cast_filter,&cast_group,
		    &cast_schedcode);
  if(cd->exec()!=0) {
    delete cd;
    return;
  }
  delete cd;
  RDFeed::Error err;
  unsigned cast_id=list_feed->postCut(cutname,&err);
  if(err!=RDFeed::ErrorOk) {
    QMessageBox::warning(this,"RDCastManager - "+tr("Posting Error"),
			 RDFeed::errorString(err));
    return;
  }
  EditCast *edit_cast=new EditCast(cast_id,this);
  edit_cast->exec();
  RDListViewItem *item=new RDListViewItem(list_casts_view);
  item->setId(cast_id);
  RefreshItem(item);
  list_casts_view->setSelected(item,true);
  list_casts_view->ensureItemVisible(item);
  delete edit_cast;

  rda->ripc()->sendNotification(RDNotification::FeedType,
				RDNotification::ModifyAction,
				list_feed->keyName());
  rda->ripc()->sendNotification(RDNotification::FeedItemType,
				RDNotification::AddAction,cast_id);
}


void ListCasts::addFileData()
{
  QString srcfile=
    QFileDialog::getOpenFileName(this,"RDCastManager - "+
				 tr("Select Audio File"),"",
				 RD_AUDIO_FILE_FILTER);
  if(srcfile.isNull()) {
    return;
  }
  RDFeed::Error err;
  unsigned cast_id=list_feed->postFile(srcfile,&err);
  if(err!=RDFeed::ErrorOk) {
    QMessageBox::warning(this,"RDCastManager - "+tr("Posting Error"),
			 RDFeed::errorString(err));
    return;
  }
  EditCast *edit_cast=new EditCast(cast_id,this);
  edit_cast->exec();
  RDListViewItem *item=new RDListViewItem(list_casts_view);
  item->setId(cast_id);
  RefreshItem(item);
  list_casts_view->setSelected(item,true);
  list_casts_view->ensureItemVisible(item);
  delete edit_cast;

  rda->ripc()->sendNotification(RDNotification::FeedType,
				RDNotification::ModifyAction,
				list_feed->keyName());
  rda->ripc()->sendNotification(RDNotification::FeedItemType,
				RDNotification::AddAction,cast_id);
}


void ListCasts::addLogData()
{
  QString logname;
  RDFeed::Error err=RDFeed::ErrorOk; 
  unsigned cast_id=0;

  RDListLogs *d=new RDListLogs(&logname,RDLogFilter::UserFilter,this);
  if(d->exec()) {
    RDLogEvent *log=new RDLogEvent(logname);
    log->load();
    QTime start_time;
    bool ignore_stops=true;
    int start_line=0;
    int end_line=log->size()-1;

    if(list_render_dialog->exec(log,&start_time,&ignore_stops,
				&start_line,&end_line)) {
      if((cast_id=list_feed->postLog(logname,start_time,ignore_stops,
				     start_line,end_line,&err))!=0) {
	EditCast *cast=new EditCast(cast_id,this);
	cast->exec();
	RDListViewItem *item=new RDListViewItem(list_casts_view);
	item->setId(cast_id);
	RefreshItem(item);
	list_casts_view->setSelected(item,true);
	list_casts_view->ensureItemVisible(item);
	delete cast;
	rda->ripc()->sendNotification(RDNotification::FeedType,
				      RDNotification::ModifyAction,
				      list_feed->keyName());
	rda->ripc()->sendNotification(RDNotification::FeedItemType,
				      RDNotification::AddAction,cast_id);
      }
      else {
	QMessageBox::warning(this,"RDCastManager - "+tr("Posting Error"),
			     RDFeed::errorString(err));
	delete d;
	delete log;
	return;
      }

      delete log;
    }
    else {  // Render dialog was canceled!
    }
  }
  delete d;
}


void ListCasts::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_casts_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditCast *edit_cast=new EditCast(item->id(),this);
  if(edit_cast->exec()==0) {
    RefreshItem(item);

    rda->ripc()->sendNotification(RDNotification::FeedType,
				  RDNotification::ModifyAction,
				  list_feed->keyName());
    rda->ripc()->sendNotification(RDNotification::FeedItemType,
				  RDNotification::ModifyAction,item->id());
  }
  delete edit_cast;
}


void ListCasts::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  QString err_text;

  RDListViewItem *item=(RDListViewItem *)list_casts_view->selectedItem();
  if(item==NULL) {
    return;
  }
  unsigned cast_id=item->id();
  if(QMessageBox::question(this,"RDCastManager - "+tr("Delete Item"),
			   tr("Are you sure you want to delete this item?"),
			   QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::No) {
    return;
  }

  QProgressDialog *pd=
    new QProgressDialog(tr("Deleting Item..."),"Cancel",0,2,this);
  pd->setWindowTitle(tr("Progress"));
  pd->setCancelButton(NULL);
  pd->setMinimumDuration(0);
  pd->setValue(0);
  qApp->processEvents();
  sleep(1);
  qApp->processEvents();
  RDPodcast *cast=new RDPodcast(rda->config(),item->id());
  if(!cast->dropAudio(list_feed,&err_text,rda->config()->logXloadDebugData())) {
    if(QMessageBox::warning(this,"RDCastManager - "+tr("Remote Error"),
			    tr("Unable to drop remote audio!\n")+
			    tr("The server said: \"")+err_text+"\".\n\n"+
			    tr("Continue deleting cast?"),
		 QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      delete pd;
      delete cast;
      return;
    }
  }
  pd->setValue(1);
  qApp->processEvents();
  sql=QString().sprintf("delete from PODCASTS where ID=%u",item->id());
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("update FEEDS set LAST_BUILD_DATETIME=now() \
                        where ID=%u",list_feed_id);
  q=new RDSqlQuery(sql);
  delete q;

  if(!list_feed->postXml()) {
    QMessageBox::warning(this,"RDCastManager - "+tr("Remote Error"),
			 tr("Unable to update remote XML data!"));
  }

  pd->reset();

  delete pd;
  delete cast;
  delete item;
  rda->ripc()->sendNotification(RDNotification::FeedType,
				RDNotification::ModifyAction,
				list_feed->keyName());
  rda->ripc()->sendNotification(RDNotification::FeedItemType,
				RDNotification::DeleteAction,cast_id);
}


void ListCasts::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListCasts::userChangedData()
{
  bool is_superfeed=list_feed->isSuperfeed();

  list_cart_button->setEnabled(rda->user()->addPodcast()&&(!is_superfeed));
  list_file_button->setEnabled(rda->user()->addPodcast()&&(!is_superfeed));
  list_log_button->setEnabled(rda->user()->addPodcast()&&(!is_superfeed));
  list_edit_button->setEnabled(rda->user()->editPodcast()&&(!is_superfeed));
  list_delete_button->
    setEnabled(rda->user()->deletePodcast()&&(!is_superfeed));
}


void ListCasts::filterChangedData(const QString &str)
{
  RefreshList();
}


void ListCasts::notexpiredToggledData(bool state)
{
  RefreshList();
}


void ListCasts::activeToggledData(bool state)
{
  RefreshList();
}


void ListCasts::postProgressChangedData(int step)
{
  list_progress_dialog->setValue(step);
  if(step==list_progress_dialog->maximum()) {
    list_progress_dialog->reset();
  }
  qApp->processEvents();
}


void ListCasts::closeData()
{
  done(0);
}


void ListCasts::resizeEvent(QResizeEvent *e)
{
  list_filter_label->setGeometry(10,10,40,20);
  list_filter_edit->setGeometry(55,10,size().width()-65,20);
  list_active_check->setGeometry(60,35,15,15);
  list_active_label->setGeometry(80,33,200,20);
  list_casts_label->setGeometry(15,57,size().width()-25,20);
  list_casts_view->setGeometry(10,76,size().width()-20,size().height()-146);
  list_cart_button->setGeometry(10,size().height()-60,80,50);
  list_file_button->setGeometry(100,size().height()-60,80,50);
  list_log_button->setGeometry(190,size().height()-60,80,50);
  list_edit_button->setGeometry(300,size().height()-60,80,50);
  list_delete_button->setGeometry(390,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListCasts::notificationReceivedData(RDNotification *notify)
{
  unsigned cast_id=0;
  RDListViewItem *item=NULL;
  RDPodcast *cast=NULL;

  if(notify->type()==RDNotification::FeedItemType) {
    cast_id=notify->id().toUInt();
    switch(notify->action()) {
    case RDNotification::AddAction:
      cast=new RDPodcast(rda->config(),cast_id);
      if(cast->keyName()==list_feed->keyName()) {
	item=new RDListViewItem(list_casts_view);
	item->setId(cast_id);
	RefreshItem(item);
	delete cast;
	return;
      }
      delete cast;
      break;

    case RDNotification::DeleteAction:
      item=(RDListViewItem *)list_casts_view->firstChild();
      while(item!=NULL) {
	if(item->id()==(int)cast_id) {
	  delete item;
	  return;
	}
	item=(RDListViewItem *)item->nextSibling();
      }
      break;

    case RDNotification::ModifyAction:
      item=(RDListViewItem *)list_casts_view->firstChild();
      while(item!=NULL) {
	if(item->id()==(int)cast_id) {
	  RefreshItem(item);
	}
	item=(RDListViewItem *)item->nextSibling();
      }
      break;

    case RDNotification::LastAction:
    case RDNotification::NoAction:
      break;

    }
  }
}


void ListCasts::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_casts_view->clear();
  sql=QString("select ID from PODCASTS ")+
    RDCastSearch(list_feed->keyName(),list_is_superfeed,
		 list_filter_edit->text(),
		 list_active_check->isChecked())+
		 " order by ORIGIN_DATETIME";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_casts_view);
    item->setId(q->value(0).toInt());
    RefreshItem(item);
  }
  delete q;
}


void ListCasts::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "PODCASTS.STATUS,"+               // 00
    "PODCASTS.ITEM_TITLE,"+           // 01
    "PODCASTS.EFFECTIVE_DATETIME,"+   // 02
    "PODCASTS.EXPIRATION_DATETIME,"+  // 03
    "PODCASTS.AUDIO_TIME,"+           // 04
    "PODCASTS.ITEM_DESCRIPTION,"+     // 05
    "FEEDS.KEY_NAME,"+                // 06
    "PODCASTS.ITEM_CATEGORY,"+        // 07
    "PODCASTS.ORIGIN_LOGIN_NAME,"+    // 08
    "PODCASTS.ORIGIN_STATION,"+       // 09
    "PODCASTS.ORIGIN_DATETIME,"+      // 10
    "PODCASTS.SHA1_HASH,"+            // 11
    "FEED_IMAGES.DATA "+              // 12
    "from PODCASTS left join FEEDS "+
    "on PODCASTS.FEED_ID=FEEDS.ID left join FEED_IMAGES "+
    "on PODCASTS.ITEM_IMAGE_ID=FEED_IMAGES.ID where "+
    QString().sprintf("PODCASTS.ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(12).isNull()) {
      item->setPixmap(0,*list_rdcastmanager_32x32_map);
    }
    else {
      QImage img=QImage::fromData(q->value(12).toByteArray());
      item->setPixmap(0,QPixmap::fromImage(img.scaled(32,32)));
    }
    item->setText(1,q->value(1).toString());
    switch((RDPodcast::Status)q->value(0).toUInt()) {
      case RDPodcast::StatusActive:
	if(q->value(2).toDateTime()<=QDateTime::currentDateTime()) {
	  item->setPixmap(2,*list_greenball_map);
	}
	else {
	  item->setPixmap(2,*list_blueball_map);
	}
	break;

      case RDPodcast::StatusPending:
	item->setPixmap(2,*list_redball_map);
	break;

      case RDPodcast::StatusExpired:
	item->setPixmap(2,*list_whiteball_map);
	break;
    }
    item->setText(3,q->value(2).toDateTime().toString("MM/dd/yyyy hh:mm:ss"));
    if(q->value(3).isNull()) {
      item->setText(4,tr("Never"));
    }
    else {
      item->setText(4,q->value(3).toDateTime().toString("MM/dd/yyyy hh:mm:ss"));
    }
    item->setText(5,RDGetTimeLength(q->value(4).toInt(),false,false));

    item->setText(6,q->value(6).toString());
    item->setText(7,q->value(7).toString());
    if(q->value(8).isNull()) {
      item->setText(8,tr("unknown")+" "+tr("at")+" "+
		    q->value(10).toDateTime().toString("MM/dd/yyyy hh:mm:ss"));
    }
    else {
      item->setText(8,q->value(8).toString()+" "+tr("on")+" "+
		    q->value(9).toString()+" "+tr("at")+" "+
		    q->value(10).toDateTime().toString("MM/dd/yyyy hh:mm:ss"));
    }
    if(q->value(11).toString().isEmpty()) {
      item->setText(9,tr("[none]"));
    }
    else {
      item->setText(9,q->value(11).toString());
    }
  }
  delete q;
}
