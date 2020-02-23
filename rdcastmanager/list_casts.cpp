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
#include <rdfeedlog.h>

#include "edit_cast.h"
#include "globals.h"
#include "list_casts.h"
#include "pick_report_dates.h"

//
// Icons
//
#include "../icons/redball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/whiteball.xpm"

ListCasts::ListCasts(unsigned feed_id,bool is_super,QWidget *parent)
  : RDDialog(parent)
{
  list_feed_id=feed_id;
  list_is_superfeed=is_super;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDCastManager - "+tr("Podcast List"));

  //
  // Create Icons
  //
  list_greenball_map=new QPixmap(greenball_xpm);
  list_redball_map=new QPixmap(redball_xpm);
  list_whiteball_map=new QPixmap(whiteball_xpm);

  //
  // The Feed
  //
  list_feed=new RDFeed(feed_id,rda->config(),this);

  //
  // Progress Dialog
  //
  list_progress_dialog=
    new QProgressDialog(tr("Uploading Audio..."),tr("Cancel"),0,list_feed->totalPostSteps(),this);
  list_progress_dialog->setWindowTitle("RDCastManager - "+tr("Progress"));
  list_progress_dialog->setMinimumDuration(0);
  connect(list_feed,SIGNAL(postProgressChanged(int)),
	  this,SLOT(postProgressChangedData(int)));

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
  // Unexpired Check Box
  //
  list_unexpired_check=new QCheckBox(this);
  list_unexpired_label=
    new QLabel(list_unexpired_check,tr("Only Show Unexpired Casts"),this);
  list_unexpired_label->setFont(labelFont());
  list_unexpired_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(list_unexpired_check,SIGNAL(toggled(bool)),
	  this,SLOT(notexpiredToggledData(bool)));

  //
  // Active Check Box
  //
  list_active_check=new QCheckBox(this);
  list_active_label=
    new QLabel(list_active_check,tr("Only Show Active Casts"),this);
  list_active_label->setFont(labelFont());
  list_active_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(list_active_check,SIGNAL(toggled(bool)),
	  this,SLOT(activeToggledData(bool)));

  //
  // Group List
  //
  list_casts_view=new RDListView(this);
  list_casts_view->setAllColumnsShowFocus(true);
  list_casts_view->setItemMargin(5);
  list_casts_view->addColumn(tr(" "));
  list_casts_view->setColumnAlignment(0,Qt::AlignCenter);
  list_casts_view->addColumn(tr("Title"));
  list_casts_view->setColumnAlignment(1,Qt::AlignLeft);
  list_casts_view->addColumn(tr("Origin"));
  list_casts_view->setColumnAlignment(2,Qt::AlignLeft);
  list_casts_view->addColumn(tr("Expires"));
  list_casts_view->setColumnAlignment(3,Qt::AlignCenter);
  list_casts_view->addColumn(tr("Length"));
  list_casts_view->setColumnAlignment(4,Qt::AlignRight);
  list_casts_view->addColumn(tr("Description"));
  list_casts_view->setColumnAlignment(5,Qt::AlignLeft);

  list_casts_view->addColumn(tr("Feed"));
  list_casts_view->setColumnAlignment(6,Qt::AlignLeft);

  list_casts_view->addColumn(tr("Category"));
  list_casts_view->setColumnAlignment(7,Qt::AlignCenter);
  list_casts_view->addColumn(tr("Link"));
  list_casts_view->setColumnAlignment(8,Qt::AlignCenter);
  connect(list_casts_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  //
  //  Post Cart Button
  //
  list_cart_button=new QPushButton(this);
  list_cart_button->setFont(buttonFont());
  list_cart_button->setText(tr("Post From\nCar&t/Cut"));
  connect(list_cart_button,SIGNAL(clicked()),this,SLOT(addCartData()));

  //
  //  Post File Button
  //
  list_file_button=new QPushButton(this);
  list_file_button->setFont(buttonFont());
  list_file_button->setText(tr("Post From\n&File"));
  connect(list_file_button,SIGNAL(clicked()),this,SLOT(addFileData()));

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
  //  Report Button
  //
  list_report_button=new QPushButton(this);
  list_report_button->setFont(buttonFont());
  list_report_button->setText(tr("Subscription\n&Report"));
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
  GetEncoderId();
  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userChangedData()));
  userChangedData();
}


ListCasts::~ListCasts()
{
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
  unsigned cast_id=list_feed->postCut(rda->user(),rda->station(),cutname,&err,
				      rda->config()->logXloadDebugData(),rda->config());
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
}


void ListCasts::addFileData()
{
  QString srcfile=
    QFileDialog::getOpenFileName(this,"RDCastManager","",RD_AUDIO_FILE_FILTER);
  if(srcfile.isNull()) {
    return;
  }
  RDFeed::Error err;
  unsigned cast_id=list_feed->postFile(rda->station(),srcfile,&err,
				       rda->config()->logXloadDebugData(),rda->config());
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
  if(QMessageBox::question(this,"RDCastManager - "+tr("Delete Podcast"),
			   tr("Are you sure you want to delete this podcast?"),
			   QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::No) {
    return;
  }

  QProgressDialog *pd=
    new QProgressDialog(tr("Deleting Podcast..."),"Cancel",0,2,this);
  pd->setCaption(tr("Progress"));
  pd->setMinimumDuration(0);
  pd->setValue(0);
  qApp->processEvents();
  sleep(1);
  qApp->processEvents();
  RDPodcast *cast=new RDPodcast(rda->config(),item->id());
  if(!cast->removeAudio(list_feed,&err_text,rda->config()->logXloadDebugData())) {
    if(QMessageBox::warning(this,"RDCastManager - "+tr("Remote Error"),
			    tr("Unable to delete remote audio!\n")+
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

  if(!list_feed->audienceMetrics()) {
    if(!list_feed->postXml(&err_text)) {
      QMessageBox::warning(this,"RDCastManager - "+tr("Remote Error"),
			    tr("Unable to update remote XML data!\n")+
			   tr("The server said: \"")+err_text+"\".");
    }
  }
  RDDeleteCastCount(list_feed_id,item->id());

  pd->reset();

  delete pd;
  delete cast;
  delete item;
}


void ListCasts::reportData()
{
  PickReportDates *rd=new PickReportDates(list_feed_id,0);
  rd->exec();
  delete rd;
}


void ListCasts::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListCasts::userChangedData()
{
  list_cart_button->setEnabled(rda->user()->addPodcast()&&(list_encoder_id>=0));
  list_file_button->setEnabled(rda->user()->addPodcast()&&(list_encoder_id>=0));
  list_edit_button->setEnabled(rda->user()->editPodcast());
  list_delete_button->setEnabled(rda->user()->deletePodcast());
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
  list_unexpired_check->setGeometry(55,35,15,15);
  list_unexpired_label->setGeometry(75,33,200,20);
  list_active_check->setGeometry(300,35,15,15);
  list_active_label->setGeometry(320,33,200,20);
  list_casts_view->setGeometry(10,54,size().width()-20,size().height()-124);
  list_cart_button->setGeometry(10,size().height()-60,80,50);
  list_file_button->setGeometry(100,size().height()-60,80,50);
  list_edit_button->setGeometry(190,size().height()-60,80,50);
  list_delete_button->setGeometry(280,size().height()-60,80,50);
  list_report_button->setGeometry(400,size().height()-60,110,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
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
		 list_unexpired_check->isChecked(),
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
    "PODCASTS.STATUS,"+            // 00
    "PODCASTS.ITEM_TITLE,"+        // 01
    "PODCASTS.ORIGIN_DATETIME,"+   // 02
    "PODCASTS.SHELF_LIFE,"+        // 03
    "PODCASTS.AUDIO_TIME,"+        // 04
    "PODCASTS.ITEM_DESCRIPTION,"+  // 05
    "FEEDS.KEY_NAME,"+             // 06
    "PODCASTS.ITEM_CATEGORY,"+     // 07
    "PODCASTS.ITEM_LINK "+         // 08
    "from PODCASTS left join FEEDS "+
    "on PODCASTS.FEED_ID=FEEDS.ID where "+
    QString().sprintf("PODCASTS.ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    switch((RDPodcast::Status)q->value(0).toUInt()) {
      case RDPodcast::StatusActive:
	item->setPixmap(0,*list_greenball_map);
	break;

      case RDPodcast::StatusPending:
	item->setPixmap(0,*list_redball_map);
	break;

      case RDPodcast::StatusExpired:
	item->setPixmap(0,*list_whiteball_map);
	break;
    }
    item->setText(1,q->value(1).toString());
    item->setText(2,RDUtcToLocal(q->value(2).toDateTime()).
		  toString("MM/dd/yyyy hh:mm:ss"));
    if(q->value(3).toInt()==0) {
      item->setText(3,tr("Never"));
    }
    else {
      item->setText(3,RDUtcToLocal(q->value(2).toDateTime()).
		    addDays(q->value(3).toInt()).toString("MM/dd/yyyy"));
    }
    item->setText(4,RDGetTimeLength(q->value(4).toInt(),false,false));
    item->setText(5,q->value(5).toString());
    item->setText(6,q->value(6).toString());
    item->setText(7,q->value(7).toString());
    item->setText(8,q->value(8).toString());
  }
  delete q;
}


void ListCasts::GetEncoderId()
{
  QString sql;
  RDSqlQuery *q;

  list_encoder_id=-1;
  RDFeed *feed=new RDFeed(list_feed_id,rda->config());
  int format=feed->uploadFormat();
  delete feed;
  if((format>0)&&(format<100)) {  // Built-in format
    list_encoder_id=format;
    return;
  }
  sql=QString().sprintf("select NAME from ENCODERS where ID=%d",format);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("select ID from ENCODERS where ")+
      "(NAME=\""+RDEscapeString(q->value(0).toString())+"\")&&"+
      "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")";
    delete q;
    q=new RDSqlQuery(sql);
    if(q->first()) {
      list_encoder_id=q->value(0).toInt();
    }
  }
  delete q;
}
