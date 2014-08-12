// list_casts.cpp
//
// List Rivendell Casts
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_casts.cpp,v 1.18.4.1 2013/11/13 23:36:35 cvs Exp $
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
#include <stdlib.h>
#include <unistd.h>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qapplication.h>
#include <qfiledialog.h>

#include <rddb.h>
#include <rdpodcast.h>
#include <rdtextfile.h>
#include <rdcut_dialog.h>
#include <rdconf.h>
#include <rdcut.h>
#include <rdwavefile.h>
#include <rdescape_string.h>
#include <rdurl.h>
#include <rdfeedlog.h>
#include <rdsettings.h>
#include <rdcastsearch.h>

#include <list_casts.h>
#include <edit_cast.h>
#include <globals.h>
#include <pick_report_dates.h>

//
// Icons
//
#include "../icons/redball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/whiteball.xpm"


ListCasts::ListCasts(unsigned feed_id,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_feed_id=feed_id;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("Podcast List"));

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
  // Create Icons
  //
  list_greenball_map=new QPixmap(greenball_xpm);
  list_redball_map=new QPixmap(redball_xpm);
  list_whiteball_map=new QPixmap(whiteball_xpm);

  //
  // The Feed
  //
  list_feed=new RDFeed(feed_id,this);

  //
  // Progress Dialog
  //
  list_progress_dialog=
    new QProgressDialog(tr("Uploading Audio..."),"Cancel",4,this);
  list_progress_dialog->setCaption(tr("Progress"));
  list_progress_dialog->setMinimumDuration(0);
  list_progress_dialog->setTotalSteps(list_feed->totalPostSteps());
  connect(list_feed,SIGNAL(postProgressChanged(int)),
	  this,SLOT(postProgressChangedData(int)));

  //
  // Filter
  //
  list_filter_edit=new QLineEdit(this,"list_filter_edit");
  list_filter_label=
    new QLabel(list_filter_edit,tr("Filter:"),this,"list_filter_label");
  list_filter_label->setFont(font);
  list_filter_label->setAlignment(AlignRight|AlignVCenter);
  connect(list_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Unexpired Check Box
  //
  list_unexpired_check=new QCheckBox(this,"list_unexpired_check");
  list_unexpired_label=
    new QLabel(list_unexpired_check,tr("Only Show Unexpired Casts"),this,
	       "list_unexpired_label");
  list_unexpired_label->setFont(font);
  list_unexpired_label->setAlignment(AlignLeft|AlignVCenter);
  connect(list_unexpired_check,SIGNAL(toggled(bool)),
	  this,SLOT(notexpiredToggledData(bool)));

  //
  // Active Check Box
  //
  list_active_check=new QCheckBox(this,"list_active_check");
  list_active_label=
    new QLabel(list_active_check,tr("Only Show Active Casts"),this,
	       "list_active_label");
  list_active_label->setFont(font);
  list_active_label->setAlignment(AlignLeft|AlignVCenter);
  connect(list_active_check,SIGNAL(toggled(bool)),
	  this,SLOT(activeToggledData(bool)));

  //
  // Group List
  //
  list_casts_view=new RDListView(this,"list_casts_view");
  list_casts_view->setAllColumnsShowFocus(true);
  list_casts_view->setItemMargin(5);
  list_casts_view->addColumn(tr(" "));
  list_casts_view->setColumnAlignment(0,AlignCenter);
  list_casts_view->addColumn(tr("Title"));
  list_casts_view->setColumnAlignment(1,AlignLeft);
  list_casts_view->addColumn(tr("Origin"));
  list_casts_view->setColumnAlignment(2,AlignLeft);
  list_casts_view->addColumn(tr("Expires"));
  list_casts_view->setColumnAlignment(3,AlignCenter);
  list_casts_view->addColumn(tr("Length"));
  list_casts_view->setColumnAlignment(4,AlignRight);
  list_casts_view->addColumn(tr("Description"));
  list_casts_view->setColumnAlignment(5,AlignLeft);
  list_casts_view->addColumn(tr("Category"));
  list_casts_view->setColumnAlignment(6,AlignCenter);
  list_casts_view->addColumn(tr("Link"));
  list_casts_view->setColumnAlignment(7,AlignCenter);
  connect(list_casts_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Post Cart Button
  //
  list_cart_button=new QPushButton(this,"list_cart_button");
  list_cart_button->setFont(font);
  list_cart_button->setText(tr("Post From\nCar&t/Cut"));
  connect(list_cart_button,SIGNAL(clicked()),this,SLOT(addCartData()));

  //
  //  Post File Button
  //
  list_file_button=new QPushButton(this,"list_file_button");
  list_file_button->setFont(font);
  list_file_button->setText(tr("Post From\n&File"));
  connect(list_file_button,SIGNAL(clicked()),this,SLOT(addFileData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this,"list_edit_button");
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this,"list_delete_button");
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Report Button
  //
  list_report_button=new QPushButton(this,"list_report_button");
  list_report_button->setFont(font);
  list_report_button->setText(tr("Subscription\n&Report"));
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this,"list_close_button");
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
  GetEncoderId();
  connect(cast_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));
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
  RDCutDialog *cd=new RDCutDialog(&cutname,rdstation_conf,cast_system,
				  &cast_filter,&cast_group,&cast_schedcode,
				  cast_ripc->user());
  if(cd->exec()!=0) {
    delete cd;
    return;
  }
  delete cd;
  RDFeed::Error err;
  unsigned cast_id=list_feed->postCut(cast_user,rdstation_conf,cutname,&err,
				      config->logXloadDebugData(),config);
  if(err!=RDFeed::ErrorOk) {
    QMessageBox::warning(this,tr("Posting Error"),RDFeed::errorString(err));
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
  QString srcfile=QFileDialog::getOpenFileName("",RD_AUDIO_FILE_FILTER,this);
  if(srcfile.isNull()) {
    return;
  }
  RDFeed::Error err;
  unsigned cast_id=list_feed->postFile(rdstation_conf,srcfile,&err,
				       config->logXloadDebugData(),config);
  if(err!=RDFeed::ErrorOk) {
    QMessageBox::warning(this,tr("Posting Error"),RDFeed::errorString(err));
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
  if(QMessageBox::question(this,tr("Delete Podcast"),
			   tr("Are you sure you want to delete this podcast?"),
			   QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::No) {
    return;
  }

  QProgressDialog *pd=
    new QProgressDialog(tr("Deleting Podcast..."),"Cancel",2,this);
  pd->setCaption(tr("Progress"));
  pd->setMinimumDuration(0);
  pd->setProgress(0);
  qApp->processEvents();
  sleep(1);
  qApp->processEvents();
  RDPodcast *cast=new RDPodcast(item->id());
  if(!cast->removeAudio(list_feed,&err_text,config->logXloadDebugData())) {
    if(QMessageBox::warning(this,tr("Remote Error"),
			    tr("Unable to delete remote audio!\n")+
			    tr("The server said: \"")+err_text+"\".\n\n"+
			    tr("Continue deleting cast?"),
		 QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      delete pd;
      delete cast;
      return;
    }
  }
  pd->setProgress(1);
  qApp->processEvents();
  sql=QString().sprintf("delete from PODCASTS where ID=%u",item->id());
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("update FEEDS set LAST_BUILD_DATETIME=now() \
                        where ID=%u",list_feed_id);
  q=new RDSqlQuery(sql);
  delete q;

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


void ListCasts::doubleClickedData(QListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListCasts::userChangedData()
{
  list_cart_button->setEnabled(cast_user->addPodcast()&&(list_encoder_id>=0));
  list_file_button->setEnabled(cast_user->addPodcast()&&(list_encoder_id>=0));
  list_edit_button->setEnabled(cast_user->editPodcast());
  list_delete_button->setEnabled(cast_user->deletePodcast());
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
  list_progress_dialog->setProgress(step);
  if(step==list_progress_dialog->totalSteps()) {
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
  list_report_button->setGeometry(415,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListCasts::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_casts_view->clear();
  sql=QString().sprintf("select ID from PODCASTS %s \
                         order by ORIGIN_DATETIME",
       (const char *)RDCastSearch(list_feed_id,list_filter_edit->text(),
				  list_unexpired_check->isChecked(),
				  list_active_check->isChecked()));
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

  sql=QString().sprintf("select STATUS,ITEM_TITLE,ORIGIN_DATETIME,SHELF_LIFE,\
                         AUDIO_TIME,ITEM_DESCRIPTION,ITEM_CATEGORY,ITEM_LINK \
                         from PODCASTS where ID=%d",item->id());
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
  }
  delete q;
}


void ListCasts::GetEncoderId()
{
  QString sql;
  RDSqlQuery *q;

  list_encoder_id=-1;
  RDFeed *feed=new RDFeed(list_feed_id);
  int format=feed->uploadFormat();
  delete feed;
  if((format>0)&&(format<100)) {  // Built-in format
    list_encoder_id=format;
    return;
  }
  sql=QString().sprintf("select NAME from ENCODERS where ID=%d",format);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString().sprintf("select ID from ENCODERS \
                           where (NAME=\"%s\")&&(STATION_NAME=\"%s\")",
			  (const char *)RDEscapeString(q->value(0).toString()),
			  (const char *)RDEscapeString(rdstation_conf->name()));
    delete q;
    q=new RDSqlQuery(sql);
    if(q->first()) {
      list_encoder_id=q->value(0).toInt();
    }
  }
  delete q;
}
