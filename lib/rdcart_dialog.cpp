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
    new QProgressDialog(tr("Please Wait..."),tr("Cancel"),0,10,this,
			Qt::WStyle_Customize|Qt::WStyle_NormalBorder);
  cart_progress_dialog->setCaption(" ");
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
  cart_cart_filter=new RDCartFilter(false,this);
  cart_cart_filter->setUserIsAdmin(user_is_admin);
  connect(rda,SIGNAL(userChanged()),cart_cart_filter,SLOT(changeUser()));

  //
  // Cart List
  //
  cart_cart_view=new QTableView(this);
  cart_cart_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  cart_cart_view->setSelectionMode(QAbstractItemView::SingleSelection);
  cart_cart_view->setShowGrid(false);
  cart_cart_view->setSortingEnabled(false);
  cart_cart_view->setWordWrap(false);
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
  // Send to Editor Button
  //
  cart_editor_button=new QPushButton(tr("Send to\n&Editor"),this);
  cart_editor_button->setFont(buttonFont());
  connect(cart_editor_button,SIGNAL(clicked()),this,SLOT(editorData()));
  if(rda->station()->editorPath().isEmpty()) {
    cart_editor_button->hide();
  }

  //
  // Load From File Button
  //
  cart_file_button=new QPushButton(tr("Load From\n&File"),this);
  cart_file_button->setFont(buttonFont());
  connect(cart_file_button,SIGNAL(clicked()),this,SLOT(loadFileData()));
  if(rda->station()->editorPath().isEmpty()) {
    cart_file_button->hide();
  }

  //
  // OK Button
  //
  cart_ok_button=new QPushButton(tr("&OK"),this);
  cart_ok_button->setFont(buttonFont());
  connect(cart_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  cart_cancel_button=new QPushButton(tr("&Cancel"),this);
  cart_cancel_button->setFont(buttonFont());
  connect(cart_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  cart_cart_model->setFilterSql(cart_cart_filter->filterSql());

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
  printf("RDCartDialog service: %s\n",svc.toUtf8().constData());
  LoadState();
  cart_cart_filter->setShowCartType(type);
  cart_cart_filter->setService(svc);
  cart_cartnum=cartnum;
  cart_type=type;
  cart_temp_allowed=temp_allowed;
  switch(cart_type) {
    case RDCart::All:
    case RDCart::Audio:
      if(rda->station()->editorPath().isEmpty()) {
	cart_editor_button->hide();
      }
      else {
	cart_editor_button->show();
      }
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
      cart_editor_button->hide();
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
  if(cart_cartnum!=NULL) {
    QModelIndex index=cart_cart_model->cartRow(*cart_cartnum);
    cart_cart_view->selectRow(index.row());
  }
}


QSizePolicy RDCartDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDCartDialog::clickedData(Q3ListViewItem *item)
{
  RDListViewItem *i=(RDListViewItem *)item;
  if (i==NULL) {
    return;
  }
  cart_ok_button->setEnabled(true);
  bool audio=((RDCart::Type)i->id())==RDCart::Audio;
  if(cart_player!=NULL) {
    cart_player->playButton()->setEnabled(audio);
    cart_player->stopButton()->setEnabled(audio);
    cart_player->setCart(i->text(1).toUInt());
  }
  cart_editor_button->setEnabled(audio);
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


void RDCartDialog::editorData()
{
  QModelIndexList rows=cart_cart_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  unsigned cartnum=cart_cart_model->cartNumber(rows.first());
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "CUTS.CUT_NAME,"+      // 00
    "CUTS.LENGTH,"+        // 01
    "CART.GROUP_NAME,"+    // 02
    "CART.TITLE,"+         // 03
    "CART.ARTIST,"+        // 04
    "CART.ALBUM,"+         // 05
    "CART.YEAR,"+          // 06
    "CART.LABEL,"+         // 07
    "CART.CLIENT,"+        // 08
    "CART.AGENCY,"+        // 09
    "CART.COMPOSER,"+      // 10
    "CART.PUBLISHER,"+     // 11
    "CART.USER_DEFINED "+  // 12
    "from CUTS left join CART "+
    "on CUTS.CART_NUMBER=CART.NUMBER where "+
    QString().sprintf("(CUTS.CART_NUMBER=%u)&&",cartnum)+
    "(CUTS.LENGTH>0)";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }
  QString cmd=rda->station()->editorPath();
  cmd.replace("%f",RDCut::pathName(q->value(0).toString()));
  cmd.replace("%n",QString().sprintf("%06u",cartnum));
  cmd.replace("%h",QString().sprintf("%d",q->value(1).toInt()));
  cmd.replace("%g",q->value(2).toString());
  cmd.replace("%t",q->value(3).toString());
  cmd.replace("%a",q->value(4).toString());
  cmd.replace("%l",q->value(5).toString());
  cmd.replace("%y",q->value(6).toString());
  cmd.replace("%b",q->value(7).toString());
  cmd.replace("%c",q->value(8).toString());
  cmd.replace("%e",q->value(9).toString());
  cmd.replace("%m",q->value(10).toString());
  cmd.replace("%p",q->value(11).toString());
  cmd.replace("%u",q->value(12).toString());
  delete q;

  if(fork()==0) {
    system(cmd+" &");
    exit(0);
  }
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
    done(0);
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

  done(0);
}


void RDCartDialog::cancelData()
{
  SaveState();
  if(cart_player!=NULL) {
    cart_player->stop();
  }
  done(-1);
}


void RDCartDialog::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  cart_cart_filter->setGeometry(0,0,w,cart_cart_filter->sizeHint().height());

  cart_cart_view->setGeometry(10,cart_cart_filter->sizeHint().height(),
			      w-20,h-cart_cart_filter->sizeHint().height()-70);
  cart_editor_button->setGeometry(235,h-60,80,50);
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

  if((f=fopen(state_file,"w"))==NULL) {
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
