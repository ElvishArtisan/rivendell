// rdlibrary.cpp
//
// The Library Utility for Rivendell.
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

#include <sys/wait.h>

#include <QApplication>
#include <QMessageBox>
#include <QShortcut>
#include <QTranslator>

#include <curl/curl.h>

#include <rdadd_cart.h>
#include <rdcart_search_text.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdprofile.h>

#include "cdripper.h"
#include "disk_ripper.h"
#include "edit_cart.h"
#include "globals.h"
#include "list_reports.h"
#include "rdlibrary.h"
#include "validate_cut.h"

//
// Global Resources
//
RDAudioPort *rdaudioport_conf;
DiskGauge *disk_gauge;
RDCut *cut_clipboard=NULL;
bool audio_changed;

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : MainWindow("rdlibrary",c)
{
  QString err_msg;

  lib_resize=false;
  profile_ripping=false;
  lib_edit_pending=false;
  lib_user_changed=false;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Initialize LibCurl
  //
  curl_global_init(CURL_GLOBAL_ALL);

  //
  // Progress Dialog
  //
  lib_progress_dialog=
    new QProgressDialog(tr("Please Wait..."),tr("Cancel"),0,10,this);
  lib_progress_dialog->setWindowTitle(" ");
  lib_progress_dialog->setValue(10);
  QLabel *label=new QLabel(tr("Please Wait..."),lib_progress_dialog);
  label->setAlignment(Qt::AlignCenter);
  label->setFont(progressFont());
  lib_progress_dialog->setLabel(label);
  lib_progress_dialog->setCancelButton(NULL);
  lib_progress_dialog->setMinimumDuration(2000);

  //
  // Open the Database
  //
  rda=new RDApplication("RDLibrary","rdlibrary",RDLIBRARY_USAGE,true,this);
  if(!rda->open(&err_msg,NULL,true)) {
    QMessageBox::critical(this,"RDLibrary - "+tr("Error"),err_msg);
    exit(1);
  }
  setWindowIcon(rda->iconEngine()->applicationIcon(RDIconEngine::RdLibrary,22));

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--profile-ripping") {
      profile_ripping=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDLibrary - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  SetCaption("");
  lib_import_path=RDGetHomeDir();

  //
  // Allocate Global Resources
  //
  rdaudioport_conf=new RDAudioPort(rda->config()->stationName(),
				   rda->libraryConf()->inputCard());
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
  cut_clipboard=NULL;
  lib_user_timer=new QTimer(this);
  lib_user_timer->setSingleShot(true);
  connect(lib_user_timer,SIGNAL(timeout()),this,SLOT(userData()));

  //
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),
	  this,SLOT(caeConnectedData(bool)));
  rda->cae()->connectHost();

  //
  // Filter
  //
  lib_cart_filter=new RDCartFilter(true,false,this);
  connect(rda,SIGNAL(userChanged()),lib_cart_filter,SLOT(changeUser()));
  connect(lib_cart_filter,SIGNAL(selectedGroupChanged(const QString &)),
	  this,SLOT(selectedGroupChangedData(const QString &)));
  
  //
  // Cart List
  //
  lib_cart_view=new LibraryView(this);
  lib_cart_view->setGeometry(100,0,430,sizeHint().height());
  lib_cart_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  lib_cart_view->setWordWrap(false);
  lib_cart_model=new RDLibraryModel(this);
  lib_cart_model->setFont(font());
  lib_cart_model->setPalette(palette());
  lib_cart_filter->setModel(lib_cart_model);
  lib_cart_view->setModel(lib_cart_model);
  connect(lib_cart_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(cartDoubleClickedData(const QModelIndex &)));
  connect(lib_cart_filter,SIGNAL(dragEnabledChanged(bool)),
	  this,SLOT(dragsChangedData(bool)));
  connect(lib_cart_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));
  connect(lib_cart_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));

  //
  // Add Button
  //
  lib_add_button=new QPushButton(this);
  lib_add_button->setFont(buttonFont());
  lib_add_button->setText(tr("Add"));
  connect(lib_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  lib_edit_button=new QPushButton(this);
  lib_edit_button->setFont(buttonFont());
  lib_edit_button->setText(tr("Edit"));
  lib_edit_button->setEnabled(false);
  connect(lib_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  lib_delete_button=new QPushButton(this);
  lib_delete_button->setFont(buttonFont());
  lib_delete_button->setText(tr("Delete"));
  lib_delete_button->setEnabled(false);
  connect(lib_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Run Macro Button
  //
  lib_macro_button=new QPushButton(this);
  lib_macro_button->setFont(buttonFont());
  lib_macro_button->setText(tr("Run\nMacro"));
  lib_macro_button->setEnabled(false);
  lib_macro_button->setVisible(false);
  connect(lib_macro_button,SIGNAL(clicked()),this,SLOT(macroData()));

  //
  // Disk Gauge
  //
  disk_gauge=new DiskGauge(rda->system()->sampleRate(),
			   rda->libraryConf()->defaultChannels(),this);

  //
  // Rip Button
  //
  lib_rip_button=new QPushButton(this);
  lib_rip_button->setFont(buttonFont());
  lib_rip_button->setText(tr("Rip\nCD"));
  connect(lib_rip_button,SIGNAL(clicked()),this,SLOT(ripData()));

  //
  // Reports Button
  //
  lib_reports_button=new QPushButton(this);
  lib_reports_button->setFont(buttonFont());
  lib_reports_button->setText(tr("Reports"));
  connect(lib_reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  // Close Button
  //
  lib_close_button=new QPushButton(this);
  lib_close_button->setFont(buttonFont());
  lib_close_button->setText(tr("Close"));
  connect(lib_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));


  //
  // Load Output Assignment
  //
  lib_output_card=rda->libraryConf()->outputCard();
  lib_output_port=rda->libraryConf()->outputPort();


  //
  // Cart Player
  //
  lib_player=
    new RDSimplePlayer(rda->cae(),rda->ripc(),lib_output_card,lib_output_port,
                       0,0,this);
  lib_player->playButton()->setEnabled(false);
  lib_player->stopButton()->setEnabled(false);
  lib_player->stopButton()->setOnColor(Qt::red);
  lib_player->playButton()->setFocusPolicy(Qt::NoFocus);
  lib_player->stopButton()->setFocusPolicy(Qt::NoFocus);

  QShortcut *lib_player_shortcut=new QShortcut(Qt::Key_Space,this);
  connect(lib_player_shortcut,SIGNAL(activated()),
	  this,SLOT(playerShortcutData()));

  lib_resize=true;

  //
  // Create RDMacroEvent for running macros
  //
  lib_macro_events=new RDMacroEvent(rda->station()->address(),rda->ripc(),this);

  dragsChangedData(lib_cart_filter->dragEnabled());

  LoadGeometry();
}


QSize MainWidget::sizeHint() const
{
  return QSize(1010,600);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::selectedGroupChangedData(const QString &grpname)
{
  lib_default_group=grpname;
}


void MainWidget::caeConnectedData(bool state)
{
  if(state) {
    QList<int> cards;
    cards.push_back(rda->libraryConf()->inputCard());
    cards.push_back(rda->libraryConf()->outputCard());
    rda->cae()->enableMetering(&cards);
  }
}


void MainWidget::userData()
{
  if(lib_edit_pending) {
    lib_user_changed=true;
    return;
  }

  SetCaption(rda->ripc()->user());
  disk_gauge->update();
}


void MainWidget::playerShortcutData()
{
  if(lib_player->isPlaying()) {
    lib_player->stop();
  }
  else if(lib_player->playButton()->isEnabled()) {
    lib_player->play();
  }
  else if(lib_macro_button->isEnabled()) {
    macroData();
  }
}

void MainWidget::addData()
{
  QString sql;
  RDSqlQuery *q;
  int cart_num;
  RDCart::Type cart_type=RDCart::All;
  QString cart_title;
  QList<unsigned> cartnums;

  lib_player->stop();
  LockUser();

  RDAddCart *add_cart=new RDAddCart(&lib_default_group,&cart_type,&cart_title,
				    rda->user()->name(),"RDLibrary",
				    rda->system(),this);
  if((cart_num=add_cart->exec())<0) {
    delete add_cart;
    UnlockUser();
    return;
  }
  delete add_cart;

  sql=QString("insert into `CART` set ")+
    QString::asprintf("`NUMBER`=%u,`TYPE`=%d,",cart_num,cart_type)+
    "`GROUP_NAME`='"+RDEscapeString(lib_default_group)+"',"+
    "`TITLE`='"+RDEscapeString(cart_title)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  cartnums.push_back(cart_num);
  EditCart *cart=
    new EditCart(cartnums,&lib_import_path,true,profile_ripping,this);
  if(cart->exec()) {
    QModelIndex row=lib_cart_model->addCart(cart_num);
    SelectRow(row);
    SendNotification(RDNotification::AddAction,cart_num);
  } 
  else {
    RDCart *rdcart=new RDCart(cart_num);
    rdcart->remove(rda->station(),rda->user(),rda->config());
    delete rdcart;
  }
  delete cart;

  UnlockUser();
}



void MainWidget::editData()
{
  QModelIndexList carts;

  if(!CurrentSelection(&carts)) {
    return;
  }

  lib_player->stop();
  LockUser();

  QList<unsigned> cartnums;
  for(int i=0;i<carts.size();i++) {
    cartnums.push_back(lib_cart_model->cartNumber(carts.at(i)));
  }
  EditCart *d=
    new EditCart(cartnums,&lib_import_path,false,profile_ripping,this);
  if(d->exec()) {
    for(int i=0;i<carts.size();i++) {
      lib_cart_model->refreshRow(carts.at(i));
      SendNotification(RDNotification::ModifyAction,
		       lib_cart_model->cartNumber(carts.at(i)));
    }
  }
  delete d;

  UnlockUser();
}


void MainWidget::macroData()
{
  QModelIndexList carts;

  if((!CurrentSelection(&carts))||(carts.size()!=1)) {
    return;
  }
  RDCart *rdcart=new RDCart(lib_cart_model->cartNumber(carts.first()));
  lib_macro_events->clear();
  lib_macro_events->load(rdcart->macros());
  lib_macro_events->exec();
  delete rdcart;
}


void MainWidget::deleteData()
{
  QString sql;
  RDSqlQuery *q=NULL;

  lib_player->stop();
  LockUser();

  QModelIndexList carts;

  if(!CurrentSelection(&carts)) {
    UnlockUser();
    return;
  }

  //
  // Get confirmation
  //
  QString str=tr("Are you sure you want to delete cart(s)");
  if(QMessageBox::question(this,tr("Delete Cart(s)"),str,QMessageBox::Yes,
			   QMessageBox::No)!=QMessageBox::Yes) {
    UnlockUser();
    return;
  }

  //
  // Check for RDCatch events
  //
  for(int i=0;i<carts.size();i++) {
    unsigned cartnum=lib_cart_model->cartNumber(carts.at(i));
    sql=QString("select ")+
      "`CUT_NAME` "+  // 00
      "from `RECORDINGS` where "+
      QString::asprintf("(`CUT_NAME` like '%06u_%%')||",cartnum)+
      QString::asprintf("(`MACRO_CART`=%u)",cartnum);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      QString str=tr("Cart")+
	QString::asprintf(" %06u ",cartnum)+
	tr("is used in one or more RDCatch events!")+"\n"+
	tr("Do you still want to delete it?");
      if(QMessageBox::warning(this,"RDLibrary - "+tr("RDCatch Event Exists"),
			      str,QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete q;
        UnlockUser();
	return;
      }
    }
    delete q;
  }

  //
  // Check clipboard
  //
  if(cut_clipboard!=NULL) {
    for(int i=0;i<carts.size();i++) {
      unsigned cartnum=lib_cart_model->cartNumber(carts.at(i));
      if(cartnum==cut_clipboard->cartNumber()) {
      	QString str=tr("Deleting cart")+QString::asprintf(" %06u ",cartnum)+
	  tr("will also empty the clipboard.")+"\n"+
	  tr("Do you still want to proceed?");
        if(QMessageBox::question(this,"RDLibrary - "+tr("Empty Clipboard"),
				     str,QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  UnlockUser();
	  return;
	}
      }
      delete cut_clipboard;
      cut_clipboard=NULL;
      break;
    }
  }

  //
  // Check for voicetracks
  //
  for(int i=0;i<carts.size();i++) {
    unsigned cartnum=lib_cart_model->cartNumber(carts.at(i));
    if(!lib_cart_model->cartOwnedBy(carts.at(i)).isEmpty()) {
      QString str=tr("Cart")+QString::asprintf(" %06u ",cartnum)+
	tr("is a voicetrack belonging to log")+" \""+
	lib_cart_model->cartOwnedBy(carts.at(i))+"\".\n"+tr("It cannot be deleted here!");
      QMessageBox::information(this,"RDLibrary - "+tr("Voicetrack Found"),str);
      UnlockUser();
      return;
    }
  }

  //
  // Find row to be selected after deletion
  //
  unsigned newcartnum=0;
  QModelIndex row=lib_cart_model->index(carts.last().row(),0).
    sibling(carts.last().row()+1,0);
  if((!row.isValid())||(row.row()>=lib_cart_model->rowCount())) {
    row=lib_cart_model->index(carts.last().row(),0).
    sibling(carts.first().row()-1,0);
  }
  if(row.isValid()) {
    newcartnum=lib_cart_model->cartNumber(row);
  }

  //
  // Delete Carts
  //
  for(int i=carts.size()-1;i>=0;i--) {
    RDCart *rdcart=new RDCart(lib_cart_model->cartNumber(carts.at(i)));
    if(!rdcart->remove(rda->station(),rda->user(),rda->config())) {
      QMessageBox::warning(this,tr("RDLibrary"),tr("Unable to delete audio!"));
      return;
    }
    lib_cart_model->removeCart(rdcart->number());
    SendNotification(RDNotification::DeleteAction,rdcart->number());
    delete rdcart;
  }

  if(newcartnum>0) {
    QModelIndex index=lib_cart_model->cartRow(newcartnum);
    if(index.isValid()) {
      SelectRow(index);
    }
  }
  UnlockUser();
}


void MainWidget::ripData()
{
  QList<unsigned> added_cartnums;

  lib_player->stop();
  LockUser();
  QString group=lib_cart_filter->selectedGroup();
  QString schedcode=lib_cart_filter->selectedSchedCode();
  DiskRipper *dialog=new DiskRipper(&lib_filter_text,&group,&schedcode,
				    &added_cartnums,profile_ripping,this);
  if(dialog->exec()) {
    lib_cart_filter->setSelectedGroup(group);
    lib_cart_filter->setFilterText(lib_filter_text);
  }
  delete dialog;
  UnlockUser();

  //
  // So the local rdlibrary(1) instance gets updated
  //
  for(int i=0;i<added_cartnums.size();i++) {
    RDNotification *notify=new RDNotification(RDNotification::CartType,
					      RDNotification::AddAction,
					      added_cartnums.at(i));
    notificationReceivedData(notify);
    delete notify;
  }
}


void MainWidget::reportsData()
{
  lib_player->stop();
  LockUser();
  ListReports *lr=
    new ListReports(lib_cart_filter->filterText(),
		    lib_cart_filter->selectedGroup(),
		    lib_cart_filter->filterSql(),this);

  lr->exec();
  delete lr;
  UnlockUser();
}


void MainWidget::cartDoubleClickedData(const QModelIndex &)
{
  editData();
}


void MainWidget::selectionChangedData(const QItemSelection &,
				      const QItemSelection &)
{
  QModelIndexList carts;
  QModelIndexList cuts;

  if(!CurrentSelection(&carts,&cuts)) {
    lib_edit_button->setEnabled(false);
    lib_delete_button->setEnabled(false);
    lib_player->playButton()->setEnabled(false);
    lib_player->stopButton()->setEnabled(false);
    return;
  }
  /*
  printf("CARTS\n");
  for(int i=0;i<carts.size();i++) {
    printf("  %d: %06u\n",i,lib_cart_model->cartNumber(carts.at(i)));
  }
  printf("CUTS\n");
  for(int i=0;i<cuts.size();i++) {
    printf("  %d: %s\n",i,lib_cart_model->cutName(cuts.at(i)).toUtf8().constData());
  }
  printf("\n");
  return;
  */
  if(carts.size()>0) {
    lib_edit_button->setEnabled(true);
    lib_delete_button->setEnabled(rda->user()->deleteCarts());
    lib_player->playButton()->setEnabled(carts.size()==1);
    lib_player->stopButton()->setEnabled(carts.size()==1);
    lib_player->setCart(lib_cart_model->cartNumber(carts.first()));
    SetPlayer(lib_cart_model->cartType(carts.first()));
  }
  else {
    lib_edit_button->setEnabled(false);
    lib_delete_button->setEnabled(false);
    lib_player->playButton()->setEnabled(cuts.size()==1);
    lib_player->stopButton()->setEnabled(cuts.size()==1);
    if(cuts.size()>=1) {
      lib_player->setCart(lib_cart_model->cartNumber(cuts.first()));
      lib_player->setCut(lib_cart_model->cutName(cuts.first()));
    }
    SetPlayer(lib_cart_model->cartType(cuts.first()));
  }
  lib_macro_button->setEnabled(carts.size()==1);
}


void MainWidget::modelResetData()
{
  for(int i=0;i<lib_cart_model->columnCount();i++) {
    lib_cart_view->resizeColumnToContents(i);
  }
}


void MainWidget::audioChangedData(int state)
{
  //  filterChangedData("");
}


void MainWidget::macroChangedData(int state)
{
  //  filterChangedData("");
}


void MainWidget::dragsChangedData(bool state)
{
  if(state) {
    QModelIndexList rows=lib_cart_view->selectionModel()->selectedRows();
    if(rows.size()>1) {
      SelectRow(rows.first());
    }
    lib_cart_view->setSelectionMode(QAbstractItemView::SingleSelection);
  }
  else {
    lib_cart_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }
  lib_cart_view->setDragEnabled(state);
}


void MainWidget::notificationReceivedData(RDNotification *notify)
{
  QString sql;
  RDSqlQuery *q=NULL;
  QStringList and_fields;

  if(notify->type()==RDNotification::CartType) {
    unsigned cartnum=notify->id().toUInt();
    switch(notify->action()) {
    case RDNotification::AddAction:
      if(lib_cart_model->cartRow(cartnum).isValid()) {
	//
	// Redundant Add, treat it like a Modify
	//
	lib_cart_model->refreshCart(cartnum);
      }
      else {
	and_fields.push_back(QString::asprintf("`CART`.`NUMBER`=%u",cartnum));
	sql=QString("select ")+
	  "`CART`.`NUMBER` "+  // 00
	  "from `CART` "+
	  "left join `GROUPS` on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` "+
	  "left join `CUTS` on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` "+
	  lib_cart_filter->filterSql(and_fields);
	q=new RDSqlQuery(sql);
	if(q->first()) {
	  lib_cart_model->addCart(cartnum);
	}
      }
      delete q;
      break;

    case RDNotification::ModifyAction:
      lib_cart_model->refreshCart(cartnum);
      break;

    case RDNotification::DeleteAction:
      if(lib_edit_pending) {
	lib_deleted_carts.push_back(cartnum);
      }
      else {
	lib_cart_model->removeCart(cartnum);
      }
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void MainWidget::quitMainWidget()
{
  SaveGeometry();
  exit(0);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  quitMainWidget();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if(lib_resize) {
    lib_cart_filter->
      setGeometry(0,0,size().width(),lib_cart_filter->sizeHint().height());
    lib_cart_view->
      setGeometry(10,90,e->size().width()-20,e->size().height()-155);
    lib_add_button->setGeometry(10,e->size().height()-60,80,50);
    lib_edit_button->setGeometry(100,e->size().height()-60,80,50);
    lib_delete_button->setGeometry(190,e->size().height()-60,80,50);
    lib_macro_button->setGeometry(290,e->size().height()-60,80,50);
    lib_player->playButton()->setGeometry(290,e->size().height()-60,80,50);
    lib_player->stopButton()->setGeometry(380,e->size().height()-60,80,50);
    disk_gauge->setGeometry(475,e->size().height()-55,
			    e->size().width()-765,
			    disk_gauge->sizeHint().height());
    lib_rip_button->
      setGeometry(e->size().width()-280,e->size().height()-60,80,50);
    lib_reports_button->
      setGeometry(e->size().width()-190,e->size().height()-60,80,50);
    lib_close_button->setGeometry(e->size().width()-90,e->size().height()-60,
				  80,50);
  }
}


void MainWidget::SetCaption(QString user)
{
  QString str1;
  QString str2;

  str1=QString("RDLibrary")+" v"+VERSION+" - "+tr("Host")+":";
  str2=tr("User")+":";
  setWindowTitle(str1+" "+rda->config()->stationName()+", "+str2+" "+user);
}


int MainWidget::CurrentSelection(QModelIndexList *carts,
				 QModelIndexList *cuts) const
{
  QModelIndexList rows=lib_cart_view->selectionModel()->selectedRows();

  carts->clear();
  if(cuts!=NULL) {
    cuts->clear();
  }
  for(int i=0;i<rows.size();i++) {
    if(lib_cart_model->isCart(rows.at(i))) {
      carts->push_back(rows.at(i));
    }
    else {
      if(cuts!=NULL) {
	cuts->push_back(rows.at(i));
      }
    }
  }
  if(cuts!=NULL) {
    return (carts->size()>0)||(cuts->size()>0);
  }
  return carts->size()>0;
}


void MainWidget::SelectRow(const QModelIndex &index)
{
  if(index.isValid()) {
    lib_cart_view->selectionModel()->
      select(index,QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
    lib_cart_view->scrollTo(index);
  }
}


void MainWidget::SelectRow(unsigned cartnum)
{
  SelectRow(lib_cart_model->cartRow(cartnum));
}


QString MainWidget::GeometryFile() {
  bool home_found = false;
  QString home = RDGetHomeDir(&home_found);
  if (home_found) {
    return home + "/" + RDLIBRARY_GEOMETRY_FILE;
  } else {
    return NULL;
  }
}


void MainWidget::LoadGeometry()
{
  loadSettings(true);
  lib_cart_filter->setDragEnabled(dragEnabled());
  lib_cart_filter->setShowNoteBubbles(showNoteBubbles());
}


void MainWidget::SaveGeometry()
{
  setDragEnabled(lib_cart_filter->dragEnabled());
  setShowNoteBubbles(lib_cart_filter->showNoteBubbles());
  saveSettings();
}


void MainWidget::LockUser()
{
  lib_edit_pending=true;
}


bool MainWidget::UnlockUser()
{
  //
  // Process Deleted Carts
  //
  for(int i=0;i<lib_deleted_carts.size();i++) {
    lib_cart_model->removeCart(lib_deleted_carts.at(i));
  }
  lib_deleted_carts.clear();

  //
  // Process User Change
  //
  bool ret=lib_user_changed;
  lib_edit_pending=false;
  if(lib_user_changed) {
    lib_user_timer->start(0);
    lib_user_changed=false;
  }

  return ret;
}


void MainWidget::SendNotification(RDNotification::Action action,
				  unsigned cartnum)
{
  RDNotification *notify=
    new RDNotification(RDNotification::CartType,action,QVariant(cartnum));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}


void MainWidget::SetPlayer(RDCart::Type type)
{
  switch(type) {
  case RDCart::Audio:
    lib_player->playButton()->show();
    lib_player->stopButton()->show();
    lib_macro_button->hide();
    break;

  case RDCart::Macro:
    lib_player->playButton()->hide();
    lib_player->stopButton()->hide();
    lib_macro_button->show();
    break;

  case RDCart::All:
    lib_player->playButton()->hide();
    lib_player->stopButton()->hide();
    lib_macro_button->hide();
    break;
  }
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();
  return a.exec();
}
