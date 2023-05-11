// list_casts.cpp
//
// List Rivendell Casts
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

#include <rdconf.h>
#include <rdescape_string.h>
#include <rdlist_logs.h>
#include <rdpodcast.h>

#include "edit_cast.h"
#include "globals.h"
#include "list_casts.h"

ListCasts::ListCasts(unsigned feed_id,QWidget *parent)
  : RDDialog(parent)
{
  list_feed_id=feed_id;

  setWindowTitle("RDCastManager - "+tr("Podcast Item List")+
		 "  [Feed ID: "+QString::asprintf("%u",feed_id)+"]");

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // The Feed
  //
  list_feed=new RDFeed(feed_id,rda->config(),this);

  //
  // Dialogs
  //
  list_render_dialog=new RenderDialog(this);

  list_cut_dialog=new RDCutDialog(&cast_filter,&cast_group,&cast_schedcode,
				  false,false,false,"RDCastManager",false,this);
  list_progress_dialog=
    new QProgressDialog(tr("Uploading Audio..."),tr("Cancel"),0,1,this);
  list_progress_dialog->setValue(1);
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
  list_casts_filter=new RDPodcastFilter(this);

  //
  // Podcast List
  //
  list_casts_label=
    new QLabel(list_feed->channelTitle(),this);
  list_casts_label->setFont(bigLabelFont());
  list_casts_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  list_casts_view=new RDTableView(this);
  list_casts_view->setSortingEnabled(true);
  list_casts_view->sortByColumn(2,Qt::DescendingOrder);
  list_casts_model=new RDPodcastListModel(feed_id,this);
  list_casts_model->setFont(font());
  list_casts_model->setPalette(palette());
  list_casts_model->sort(2,Qt::DescendingOrder);
  list_casts_view->setModel(list_casts_model);
  connect(list_casts_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));
  connect(list_casts_model,SIGNAL(rowsInserted(const QModelIndex &,int,int)),
	  this,SLOT(rowsInsertedData(const QModelIndex &,int,int)));
  connect(list_casts_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_casts_filter,SIGNAL(filterChanged(const QString &)),
	  list_casts_model,SLOT(setFilterSql(const QString &)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  list_casts_model,SLOT(processNotification(RDNotification *)));

  //
  //  Post Cart Button
  //
  list_cart_button=new QPushButton(this);
  list_cart_button->setFont(buttonFont());
  list_cart_button->setText(tr("Post From\nCart/Cut"));
  list_cart_button->setDisabled(list_feed->isSuperfeed());
  connect(list_cart_button,SIGNAL(clicked()),this,SLOT(addCartData()));

  //
  //  Post File Button
  //
  list_file_button=new QPushButton(this);
  list_file_button->setFont(buttonFont());
  list_file_button->setText(tr("Post From\nFile"));
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
  list_edit_button->setText(tr("Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userChangedData()));
  modelResetData();
  userChangedData();
}


ListCasts::~ListCasts()
{
  delete list_cut_dialog;
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
  QString err_msg;
  QString cutname;
  if(!list_cut_dialog->exec(&cutname)) {
    return;
  }
  unsigned cast_id=list_feed->postCut(cutname,&err_msg);
  if(cast_id==0) {
    QMessageBox::warning(this,"RDCastManager - "+tr("Posting Error"),err_msg);
    return;
  }
  EditCast *d=new EditCast(cast_id,true,this);
  d->exec();
  QModelIndex row=list_casts_model->addCast(cast_id);
  if(row.isValid()) {
    list_casts_view->selectRow(row.row());
  }
  delete d;

  rda->ripc()->sendNotification(RDNotification::FeedType,
				RDNotification::ModifyAction,
				list_feed->keyName());
  rda->ripc()->sendNotification(RDNotification::FeedItemType,
				RDNotification::AddAction,cast_id);
}


void ListCasts::addFileData()
{
  QString err_msg;

  QString srcfile=
    QFileDialog::getOpenFileName(this,"RDCastManager - "+
				 tr("Select Audio File"),"",
				 RD_AUDIO_FILE_FILTER);
  if(srcfile.isNull()) {
    return;
  }
  unsigned cast_id=list_feed->postFile(srcfile,&err_msg);
  if(cast_id==0) {
    QMessageBox::warning(this,"RDCastManager - "+tr("Posting Error"),err_msg);
    return;
  }
  EditCast *d=new EditCast(cast_id,true,this);
  d->exec();
  QModelIndex row=list_casts_model->addCast(cast_id);
  if(row.isValid()) {
    list_casts_view->selectRow(row.row());
  }
  delete d;

  rda->ripc()->sendNotification(RDNotification::FeedType,
				RDNotification::ModifyAction,
				list_feed->keyName());
  rda->ripc()->sendNotification(RDNotification::FeedItemType,
				RDNotification::AddAction,cast_id);
}


void ListCasts::addLogData()
{
  QString logname;
  QString err_msg;
  unsigned cast_id=0;

  RDListLogs *lld=
    new RDListLogs(&logname,RDLogFilter::UserFilter,"RDCastManager",this);
  if(lld->exec()) {
    RDLogModel *model=new RDLogModel(logname,true,this);
    model->load();
    QTime start_time;
    bool ignore_stops=true;
    int start_line=0;
    int end_line=model->lineCount()-1;

    if(list_render_dialog->exec(model,&start_time,&ignore_stops,
				&start_line,&end_line)) {
      if((cast_id=list_feed->postLog(logname,start_time,ignore_stops,
				     start_line,end_line,&err_msg))!=0) {
	EditCast *cast=new EditCast(cast_id,true,this);
	cast->exec();
	QModelIndex row=list_casts_model->addCast(cast_id);
	if(row.isValid()) {
	  list_casts_view->selectRow(row.row());
	}
	delete cast;
	rda->ripc()->sendNotification(RDNotification::FeedType,
				      RDNotification::ModifyAction,
				      list_feed->keyName());
	rda->ripc()->sendNotification(RDNotification::FeedItemType,
				      RDNotification::AddAction,cast_id);
      }
      else {
	QMessageBox::warning(this,"RDCastManager - "+tr("Posting Error"),
			     err_msg);
	delete lld;
	delete model;
	return;
      }

      delete model;
    }
    else {  // Render dialog was canceled!
    }
  }
  delete lld;
}


void ListCasts::editData()
{
  QModelIndexList rows=list_casts_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditCast *d=new EditCast(list_casts_model->castId(rows.first()),false,this);
  if(d->exec()) {
    list_casts_model->refresh(rows.first());
    rda->ripc()->sendNotification(RDNotification::FeedType,
				  RDNotification::ModifyAction,
				  list_feed->keyName());
    rda->ripc()->sendNotification(RDNotification::FeedItemType,
				  RDNotification::ModifyAction,
				  list_casts_model->castId(rows.first()));
  }
  delete d;
}


void ListCasts::deleteData()
{
  QString sql;
  QString err_text;
  QString err_msg;
  QModelIndexList rows=list_casts_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  unsigned cast_id=list_casts_model->castId(rows.first());
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
  RDPodcast *cast=new RDPodcast(rda->config(),cast_id);
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
  sql=QString("delete from `PODCASTS` where ")+
    QString::asprintf("`ID`=%u",cast_id);
  RDSqlQuery::apply(sql);

  sql=QString("update `FEEDS` set ")+
    "LAST_BUILD_DATETIME=now() "+
    "where "+
    QString::asprintf("`ID`=%u",list_feed_id);
  RDSqlQuery::apply(sql);

  if(!list_feed->postXml(&err_msg)) {
    QMessageBox::warning(this,"RDCastManager - "+tr("Remote Error"),
			 tr("Unable to update remote XML data!")+"\n"+
			 "["+err_msg+"]");
  }

  pd->reset();

  delete pd;
  delete cast;

  list_casts_model->removeCast(cast_id);
  rda->ripc()->sendNotification(RDNotification::FeedType,
				RDNotification::ModifyAction,
				list_feed->keyName());
  rda->ripc()->sendNotification(RDNotification::FeedItemType,
				RDNotification::DeleteAction,cast_id);
}


void ListCasts::doubleClickedData(const QModelIndex &index)
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
  //  RefreshList();
}


void ListCasts::modelResetData()
{
  list_casts_view->resizeColumnsToContents();
  list_casts_view->resizeRowsToContents();
}


void ListCasts::rowsInsertedData(const QModelIndex &parent,int start,int end)
{
  for(int i=start;i<=end;i++) {
    list_casts_view->resizeRowToContents(i);
  }
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
  done(true);
}


void ListCasts::resizeEvent(QResizeEvent *e)
{
  list_casts_filter->
    setGeometry(0,0,size().width(),list_casts_filter->sizeHint().height());
  list_casts_label->setGeometry(15,57,size().width()-25,20);
  list_casts_view->setGeometry(10,76,size().width()-20,size().height()-146);
  list_cart_button->setGeometry(10,size().height()-60,80,50);
  list_file_button->setGeometry(100,size().height()-60,80,50);
  list_log_button->setGeometry(190,size().height()-60,80,50);
  list_edit_button->setGeometry(300,size().height()-60,80,50);
  list_delete_button->setGeometry(390,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
