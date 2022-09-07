// rdcart_dialog.cpp
//
// A widget to select a Rivendell Cart.
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

#include <stdlib.h>
#include <stdio.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

#include "rdapplication.h"
#include "rdaudioimport.h"
#include "rdcart_dialog.h"
#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdgroup.h"
#include "rdprofile.h"
#include "rdsettings.h"
#include "rdwavefile.h"

RDCartDialog::RDCartDialog(QString *filter,QString *group,QString *schedcode,
			   const QString &caption,bool user_is_admin,
			   QWidget *parent)
  : RDDialog(parent)
{
  cart_caption=caption;

  cart_cartnum=NULL;
  cart_type=RDCart::All;
  cart_temp_allowed=NULL;
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

  setWindowTitle(caption+" - "+tr("Select Cart"));

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
  connect(rda,SIGNAL(userChanged()),cart_cart_filter,SLOT(changeUser()));

  //
  // Cart List
  //
  cart_cart_view=new RDTableView(this);
  cart_cart_view->setSortingEnabled(true);
  cart_cart_view->sortByColumn(0,Qt::AscendingOrder);
  cart_cart_model=new RDLibraryModel(this);
  cart_cart_model->setFont(font());
  cart_cart_model->setPalette(palette());
  cart_cart_view->setModel(cart_cart_model);
  cart_cart_filter->setModel(cart_cart_model);
  connect(cart_cart_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));
  connect(cart_cart_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(cartDoubleClickedData(const QModelIndex &)));
  connect(cart_cart_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));

  //
  // Audition Player
  //
  if((rda->station()->cueCard()<0)||(rda->station()->cuePort()<0)) {
    cart_player=NULL;
  }
  else {
    cart_player=
      new RDSimplePlayer(rda->cae(),rda->ripc(),rda->station()->cueCard(),rda->station()->cuePort(),
			 rda->station()->cueStartCart(),rda->station()->cueStopCart(),this);
    cart_player->playButton()->setDisabled(true);
    cart_player->stopButton()->setDisabled(true);
    cart_player->stopButton()->setOnColor(Qt::red);
  }

  //
  // Load From File Button
  //
  cart_file_button=new QPushButton(tr("Load From\nFile"),this);
  cart_file_button->setFont(buttonFont());
  connect(cart_file_button,SIGNAL(clicked()),this,SLOT(loadFileData()));
  //
  // FIXME: How about this?
  //
  cart_file_button->hide();

  //
  // OK Button
  //
  cart_ok_button=new QPushButton(tr("OK"),this);
  cart_ok_button->setFont(buttonFont());
  connect(cart_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

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


RDCartDialog::~RDCartDialog()
{
  delete cart_cart_filter;
  delete cart_cart_model;
  delete cart_cart_view;
  if(cart_player!=NULL) {
    delete cart_player;
  }
}


QSize RDCartDialog::sizeHint() const
{
  return QSize(cart_cart_filter->sizeHint().width(),400);
}


int RDCartDialog::exec(int *cartnum,RDCart::Type type,const QString &svc,
		       bool *temp_allowed)
{
  LoadState();
  cart_cart_filter->setShowCartType(type);
  cart_cart_filter->setService(svc);
  cart_cartnum=cartnum;
  cart_type=type;
  cart_temp_allowed=temp_allowed;
  switch(cart_type) {
    case RDCart::All:
    case RDCart::Audio:
      if(temp_allowed==NULL) {
	cart_file_button->hide();
      }
      else {
	cart_file_button->show();
      }
      if(cart_player!=NULL) {
	cart_player->playButton()->show();
	cart_player->stopButton()->show();
      }
      break;

    case RDCart::Macro:
      if(cart_player!=NULL) {
	cart_player->playButton()->hide();
	cart_player->stopButton()->hide();
      }
      break;
  }
  cart_ok_button->setEnabled(false);

  if(cart_cartnum!=NULL) {
    QModelIndex index=cart_cart_model->cartRow(*cart_cartnum);
    cart_cart_view->selectRow(index.row());
  }

  return QDialog::exec();
}


void RDCartDialog::modelResetData()
{
  cart_cart_view->resizeColumnsToContents();
  cart_cart_view->resizeRowsToContents();
  if(cart_cartnum!=NULL) {
    QModelIndex index=cart_cart_model->cartRow(*cart_cartnum);
    cart_cart_view->selectRow(index.row());
  }
}


QSizePolicy RDCartDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDCartDialog::cartDoubleClickedData(const QModelIndex &index)
{
  okData();
}


void RDCartDialog::selectionChangedData(const QItemSelection &before,
					const QItemSelection &after)
{
  QModelIndexList rows=cart_cart_view->selectionModel()->selectedRows();

  cart_ok_button->setEnabled(rows.size()==1);
}


void RDCartDialog::loadFileData()
{
  QString filename;
  RDCart *cart=NULL;
  RDCut *cut=NULL;
  RDAudioImport *conv;
  RDAudioImport::ErrorCode err; 
  RDAudioConvert::ErrorCode conv_err;
  RDSettings settings;
  unsigned cartnum=0;
  QString file_title="";
  RDWaveFile *wavefile=NULL;
  RDWaveData wavedata;
  QString err_msg;

  filename=QFileDialog::getOpenFileName(this,cart_caption+" - "+
					tr("Open Audio File"),
					cart_import_path,
					cart_import_file_filter);
  if(!filename.isEmpty()) {
    cart_import_path=RDGetPathPart(filename);

    //
    // Create Cart
    //
    if((cartnum=RDCart::create(rda->system()->tempCartGroup(),RDCart::Audio,
			       &err_msg))==0) {
      delete cart;
      QMessageBox::warning(this,tr("Cart Error"),
			   tr("Unable to create temporary cart for import!")+
			   "["+err_msg+"]");
      return;
    }
    cart=new RDCart(cartnum);
    cart->setOwner(rda->station()->name());
    cut=new RDCut(cartnum,1,true);

    //
    // Import Audio
    //
    cart_busy_dialog->show(tr("Importing"),tr("Importing..."));
    conv=new RDAudioImport(this);
    conv->setCartNumber(cartnum);
    conv->setCutNumber(1);
    conv->setSourceFile(filename);
    settings.setChannels(2);
    settings.setNormalizationLevel(-11);
    conv->setDestinationSettings(&settings);
    conv->setUseMetadata(true);
    err=conv->runImport(rda->user()->name(),rda->user()->password(),&conv_err);
    cart_busy_dialog->hide();
    switch(conv_err) {
    case RDAudioImport::ErrorOk:
      break;

    default:
      QMessageBox::warning(this,tr("Import Error"),
			   RDAudioImport::errorText(err,conv_err));
      delete conv;
      delete cart;
      delete cut;
      return;
    }

    //
    // Check Metadata
    //
    wavefile=new RDWaveFile(filename);
    if(wavefile->openWave(&wavedata)) {
      if((!wavedata.metadataFound())||(wavedata.title().isEmpty())) {
	cart->setTitle(tr("Imported from")+" "+RDGetBasePart(filename));
      }
    }

    *cart_cartnum=cartnum;
    *cart_temp_allowed=true;
    delete conv;
    delete cart;
    delete cut;
    done(true);
  }
}


void RDCartDialog::okData()
{
  QModelIndexList rows=cart_cart_view->selectionModel()->selectedRows();

  if(rows.size()==1) {
    SaveState();
    if(cart_player!=NULL) {
      cart_player->stop();
    }

    *cart_filter=cart_cart_filter->filterText();
    *cart_cartnum=cart_cart_model->cartNumber(rows.first());
    if(cart_temp_allowed!=NULL) {
      *cart_temp_allowed=false;
    }
  }

  done(true);
}


void RDCartDialog::cancelData()
{
  SaveState();
  if(cart_player!=NULL) {
    cart_player->stop();
  }
  done(false);
}


void RDCartDialog::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  cart_cart_filter->setGeometry(0,0,w,cart_cart_filter->sizeHint().height());

  cart_cart_view->setGeometry(10,cart_cart_filter->sizeHint().height(),
			      w-20,h-cart_cart_filter->sizeHint().height()-70);
  cart_file_button->setGeometry(325,h-60,80,50);
  cart_ok_button->setGeometry(w-180,h-60,80,50);
  cart_cancel_button->setGeometry(w-90,h-60,80,50);
  if(cart_player!=NULL) {
    cart_player->playButton()->setGeometry(10,h-60,80,50);
    cart_player->stopButton()->setGeometry(100,h-60,80,50);
  }
}


void RDCartDialog::closeEvent(QCloseEvent *e)
{
  if(cart_player!=NULL) {
    cart_player->stop();
  }
  cancelData();
}


QString RDCartDialog::StateFile() {
  bool home_found = false;
  QString home = RDGetHomeDir(&home_found);
  if (home_found) {
    return home+"/.rdcartdialog";
  } 
  else {
    return NULL;
  }
}

void RDCartDialog::LoadState()
{
  QString state_file=StateFile();
  if (state_file.isEmpty()) {
    return;
  }

  RDProfile *p=new RDProfile();
  p->setSource(state_file);

  cart_cart_filter->
    setLimitSearch(p->boolValue("RDCartDialog","LimitSearch",true));
  delete p;
}


void RDCartDialog::SaveState()
{
  FILE *f=NULL;

  QString state_file=StateFile();
  if (state_file.isEmpty()) {
    return;
  }

  if((f=fopen(state_file.toUtf8(),"w"))==NULL) {
    return;
  }
  fprintf(f,"[RDCartDialog]\n");
  if(cart_cart_filter->limitSearch()) {
    fprintf(f,"LimitSearch=Yes\n");
  }
  else {
    fprintf(f,"LimitSearch=No\n");
  }
  fclose(f);
}
