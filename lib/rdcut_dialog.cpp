// rdcut_dialog.cpp
//
// A widget to select a Rivendell Cut.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

#include "rdadd_cart.h"
#include "rdapplication.h"
#include "rdaudioimport.h"
#include "rdcut_dialog.h"
#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdgroup.h"
#include "rdprofile.h"
#include "rdsettings.h"
#include "rdwavefile.h"

RDCutDialog::RDCutDialog(QString *filter,QString *group,QString *schedcode,
			 bool show_clear,bool allow_add,bool exclude_tracks,
			 const QString &caption,bool user_is_admin,
			 QWidget *parent)
  : RDDialog(parent)
{
  cart_caption=caption;
  cart_allow_add=allow_add;

  cart_temp_allowed=NULL;
  cart_cutname=NULL;
  cart_filter_mode=rda->station()->filterMode();
  if(filter==NULL) {
    cart_filter=new QString();
    local_filter=true;
  }
  else {
    cart_filter=filter;
    local_filter=false;
  }
  cart_import_path=RDGetHomeDir();
  cart_import_file_filter=RD_AUDIO_FILE_FILTER;

  setWindowTitle(caption+" - "+tr("Select Cut"));

  //
  // Progress Dialog
  //
  cart_progress_dialog=
    new QProgressDialog(tr("Please Wait..."),tr("Cancel"),0,10,this);
  cart_progress_dialog->setWindowTitle(" ");
  cart_progress_dialog->setValue(10);
  QLabel *label=new QLabel(tr("Please Wait..."),cart_progress_dialog);
  label->setAlignment(Qt::AlignCenter);
  label->setFont(progressFont());
  cart_progress_dialog->setLabel(label);
  cart_progress_dialog->setCancelButton(NULL);
  cart_progress_dialog->setMinimumDuration(2000);
  
  cart_busy_dialog=new RDBusyDialog(this);

  //
  // Cart Filter
  //
  cart_cart_filter=new RDCartFilter(false,user_is_admin,this);
  cart_cart_filter->setShowCartType(RDCart::Audio);
  cart_cart_filter->setShowTrackCarts(!exclude_tracks);
  connect(rda,SIGNAL(userChanged()),cart_cart_filter,SLOT(changeUser()));

  //
  // Cart List
  //
  cart_cart_view=new RDTreeView(this);
  cart_cart_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  cart_cart_view->setSelectionMode(QAbstractItemView::SingleSelection);
  cart_cart_view->setSortingEnabled(false);
  cart_cart_view->setWordWrap(false);
  cart_cart_model=new RDLibraryModel(this);
  cart_cart_model->setFont(font());
  cart_cart_model->setPalette(palette());
  cart_cart_view->setModel(cart_cart_model);
  cart_cart_filter->setModel(cart_cart_model);
  cart_cart_view->setSortingEnabled(true);
  cart_cart_view->sortByColumn(0,Qt::AscendingOrder);
  connect(cart_cart_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));
  connect(cart_cart_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(cartDoubleClickedData(const QModelIndex &)));
  connect(cart_cart_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));

  //
  // Add Button
  //
  cart_add_button=new QPushButton(tr("Add New\nCart"),this);
  cart_add_button->setGeometry(10,sizeHint().height()-60,80,50);
  cart_add_button->setFont(buttonFont());
  connect(cart_add_button,SIGNAL(clicked()),this,SLOT(addButtonData()));
  if(!allow_add) {
    cart_add_button->hide();
  }

  //
  // Clear Button
  //
  cart_clear_button=new QPushButton(tr("Clear"),this);
  cart_clear_button->setFont(buttonFont());
  connect(cart_clear_button,SIGNAL(clicked()),
	  cart_cart_view,SLOT(clearSelection()));
  if(!show_clear) {
    cart_clear_button->hide();
  }

  //
  // OK Button
  //
  cart_ok_button=new QPushButton(tr("OK"),this);
  cart_ok_button->setFont(buttonFont());
  connect(cart_ok_button,SIGNAL(clicked()),this,SLOT(okData()));
  cart_ok_button->setDisabled(true);

  //
  // Cancel Button
  //
  cart_cancel_button=new QPushButton(tr("Cancel"),this);
  cart_cancel_button->setFont(buttonFont());
  connect(cart_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  cart_cart_filter->changeUser();
}


RDCutDialog::~RDCutDialog()
{
  delete cart_cart_filter;
  delete cart_cart_model;
  delete cart_cart_view;
}


QSize RDCutDialog::sizeHint() const
{
  return QSize(cart_cart_filter->sizeHint().width(),400);
}


int RDCutDialog::exec(QString *cutname)
{
  LoadState();
  cart_cart_filter->setShowCartType(RDCart::Audio);
  cart_cutname=cutname;
  cart_ok_button->setEnabled(false);

  if((cart_cutname==NULL)||(cart_cutname->isEmpty())) {
    cart_cart_view->selectionModel()->clearSelection();
  }
  else {
    QModelIndex index=cart_cart_model->cartRow(RDCut::cartNumber(*cutname));
    if(index.isValid()) {
      cart_cart_view->setExpanded(index,true);
      index=cart_cart_model->cutRow(*cart_cutname);
      if(index.isValid()) {
	cart_cart_view->selectionModel()->select(index,
	    QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
	cart_cart_view->scrollTo(index,QAbstractItemView::PositionAtCenter);
	cart_ok_button->setEnabled(true);
      }
    }
  }

  return QDialog::exec();
}


void RDCutDialog::modelResetData()
{
  for(int i=0;i<cart_cart_model->columnCount();i++) {
    cart_cart_view->resizeColumnToContents(i);
  }
  for(int i=0;i<cart_cart_model->rowCount();i++) {
    cart_cart_view->setExpanded(cart_cart_model->index(i,0),true);
  }
}


QSizePolicy RDCutDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDCutDialog::cartDoubleClickedData(const QModelIndex &index)
{
  okData();
}


void RDCutDialog::selectionChangedData(const QItemSelection &before,
					const QItemSelection &after)
{
  QModelIndexList rows=cart_cart_view->selectionModel()->selectedRows();

  cart_ok_button->setEnabled((rows.size()==1)&&(cart_cart_model->isCut(rows.first())));
}


void RDCutDialog::addButtonData()
{
  QString cart_group=cart_cart_filter->selectedGroup();
  RDCart::Type cart_type=RDCart::Audio;
  QString cart_title;
  QString sql;
  RDSqlQuery *q;
  int cart_num=-1;

  RDAddCart *add_cart=new RDAddCart(&cart_group,&cart_type,&cart_title,
				    rda->user()->name(),cart_caption,
				    rda->system(),this);
  if((cart_num=add_cart->exec())<0) {
    delete add_cart;
    return;
  }
  sql=QString("insert into `CART` set ")+
    QString::asprintf("`NUMBER`=%d,",cart_num)+
    QString::asprintf("`TYPE`=%d,",cart_type)+
    "`GROUP_NAME`='"+RDEscapeString(cart_group)+"',"+
    "`TITLE`='"+RDEscapeString(cart_title)+"'";
  q=new RDSqlQuery(sql);
  delete q;
  RDCut::create(cart_num,1);
  cart_cart_model->addCart(cart_num);
  QModelIndex row=cart_cart_model->cutRow(RDCut::cutName(cart_num,1));
  cart_cart_view->selectionModel()->
    select(row,QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
  cart_cart_view->scrollTo(row,QAbstractItemView::PositionAtCenter);

  RDNotification *notify=
    new RDNotification(RDNotification::CartType,RDNotification::AddAction,
		       QVariant(cart_num));
  rda->ripc()->sendNotification(*notify);
  delete notify;

  cart_ok_button->setEnabled(true);

  delete add_cart;
}


void RDCutDialog::okData()
{
  QModelIndexList rows=cart_cart_view->selectionModel()->selectedRows();

  if((rows.size()==1)&&(cart_cart_model->isCut(rows.first()))) {
    SaveState();
    if(cart_filter!=NULL) {
      *cart_filter=cart_cart_filter->filterText();
    }
    *cart_cutname=cart_cart_model->cutName(rows.first());
    if(cart_temp_allowed!=NULL) {
      *cart_temp_allowed=false;
    }
    done(true);
  }
}


void RDCutDialog::cancelData()
{
  SaveState();
  done(false);
}


void RDCutDialog::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  cart_cart_filter->setGeometry(0,0,w,cart_cart_filter->sizeHint().height());

  cart_cart_view->setGeometry(10,cart_cart_filter->sizeHint().height(),
			      w-20,h-cart_cart_filter->sizeHint().height()-70);

  cart_add_button->setGeometry(10,size().height()-60,80,50);
  if(cart_allow_add) {
    cart_clear_button->setGeometry(100,size().height()-60,80,50);
  }
  else {
    cart_clear_button->setGeometry(10,size().height()-60,80,50);
  }

  cart_ok_button->setGeometry(w-180,h-60,80,50);
  cart_cancel_button->setGeometry(w-90,h-60,80,50);
}


void RDCutDialog::closeEvent(QCloseEvent *e)
{
  cancelData();
}


QString RDCutDialog::StateFile() {
  bool home_found = false;
  QString home = RDGetHomeDir(&home_found);
  if (home_found) {
    return home+"/.rdcartdialog";
  } 
  else {
    return NULL;
  }
}

void RDCutDialog::LoadState()
{
  QString state_file=StateFile();
  if (state_file.isEmpty()) {
    return;
  }

  RDProfile *p=new RDProfile();
  p->setSource(state_file);

  cart_cart_filter->
    setLimitSearch(p->boolValue("RDCutDialog","LimitSearch",true));
  delete p;
}


void RDCutDialog::SaveState()
{
  FILE *f=NULL;

  QString state_file=StateFile();
  if (state_file.isEmpty()) {
    return;
  }

  if((f=fopen(state_file.toUtf8(),"w"))==NULL) {
    return;
  }
  fprintf(f,"[RDCutDialog]\n");
  if(cart_cart_filter->limitSearch()) {
    fprintf(f,"LimitSearch=Yes\n");
  }
  else {
    fprintf(f,"LimitSearch=No\n");
  }
  fclose(f);
}
