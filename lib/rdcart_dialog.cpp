// rdcart_dialog.cpp
//
// A widget to select a Rivendell Cart.
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

#include <stdlib.h>
#include <stdio.h>

#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include "rdapplication.h"
#include "rdaudioimport.h"
#include "rdcart_dialog.h"
#include "rdcart_search_text.h"
#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdgroup.h"
#include "rdprofile.h"
#include "rdsettings.h"
#include "rdtextvalidator.h"
#include "rdwavefile.h"

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"

RDCartDialog::RDCartDialog(QString *filter,QString *group,QString *schedcode,
			   const QString &caption,QWidget *parent)
  : RDDialog(parent)
{
  cart_caption=caption;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  cart_cartnum=NULL;
  cart_type=RDCart::All;
  cart_group=group;
  cart_schedcode=schedcode;
  if(cart_schedcode->isNull()) {
    *cart_schedcode=tr("ALL");
  }
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
  // Create Icons
  //
  cart_playout_map=new QPixmap(play_xpm);
  cart_macro_map=new QPixmap(rml5_xpm);

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
  // Filter Selector
  //
  cart_filter_edit=new QLineEdit(this);
  cart_filter_label=new QLabel(cart_filter_edit,tr("Cart Filter:"),this);
  cart_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cart_filter_label->setFont(labelFont());
  connect(cart_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Filter Search Button
  //
  cart_search_button=new QPushButton(this);
  cart_search_button->setText(tr("&Search"));
  cart_search_button->setFont(buttonFont());
  connect(cart_search_button,SIGNAL(clicked()),this,SLOT(filterSearchedData()));

  //
  // Filter Clear Button
  //
  cart_clear_button=new QPushButton(this);
  cart_clear_button->setText(tr("C&lear"));
  cart_clear_button->setFont(buttonFont());
  connect(cart_clear_button,SIGNAL(clicked()),this,SLOT(filterClearedData()));

  //
  // Group Code Selector
  //
  cart_group_box=new RDComboBox(this);
  cart_group_label=new QLabel(cart_group_box,tr("Group:"),this);
  cart_group_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cart_group_label->setFont(labelFont());
  connect(cart_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Scheduler Code Selector
  //
  cart_schedcode_box=new RDComboBox(this);
  cart_schedcode_label=
    new QLabel(cart_schedcode_box,tr("Scheduler Code:"),this);
  cart_schedcode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cart_schedcode_label->setFont(labelFont());
  connect(cart_schedcode_box,SIGNAL(activated(const QString &)),
	  this,SLOT(schedcodeActivatedData(const QString &)));

  //
  // Search Limit Checkbox
  //
  cart_limit_box=new QCheckBox(this);
  cart_limit_box->setChecked(true);
  cart_limit_label=
    new QLabel(cart_limit_box,tr("Show Only First")+
	       QString().sprintf(" %d ",
		      RD_LIMITED_CART_SEARCH_QUANTITY)+tr("Matches"),this);
  cart_limit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  cart_limit_label->setFont(labelFont());
  connect(cart_limit_box,SIGNAL(stateChanged(int)),
	  this,SLOT(limitChangedData(int)));

  //
  // Cart List
  //
  cart_cart_list=new RDListView(this);
  cart_cart_list->setSelectionMode(Q3ListView::Single);
  cart_cart_list->setAllColumnsShowFocus(true);
  cart_cart_list->setItemMargin(5);
  connect(cart_cart_list,SIGNAL(clicked(Q3ListViewItem *)),
	  this,SLOT(clickedData(Q3ListViewItem *)));
  connect(cart_cart_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));
  cart_cart_label=new QLabel(cart_cart_list,"Carts",this);
  cart_cart_label->setFont(labelFont());
  cart_cart_list->addColumn("");
  cart_cart_list->setColumnAlignment(0,Qt::AlignHCenter);

  cart_cart_list->addColumn(tr("Number"));
  cart_cart_list->setColumnAlignment(1,Qt::AlignHCenter);

  cart_cart_list->addColumn(tr("Length"));
  cart_cart_list->setColumnAlignment(2,Qt::AlignRight);
  cart_cart_list->setColumnSortType(2,RDListView::TimeSort);

  cart_cart_list->addColumn(tr("Title"),200);
  cart_cart_list->setColumnAlignment(3,Qt::AlignLeft);
  cart_cart_list->setColumnWidthMode(3,Q3ListView::Manual);

  cart_cart_list->addColumn(tr("Artist"));
  cart_cart_list->setColumnAlignment(4,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("Group"));
  cart_cart_list->setColumnAlignment(5,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("Composer"));
  cart_cart_list->setColumnAlignment(6,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("Conductor"));
  cart_cart_list->setColumnAlignment(7,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("Client"));
  cart_cart_list->setColumnAlignment(8,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("Agency"));
  cart_cart_list->setColumnAlignment(9,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("User Def"));
  cart_cart_list->setColumnAlignment(10,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("Start"));
  cart_cart_list->setColumnAlignment(11,Qt::AlignLeft);

  cart_cart_list->addColumn(tr("End"));
  cart_cart_list->setColumnAlignment(12,Qt::AlignLeft);

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
}


RDCartDialog::~RDCartDialog()
{
  if(local_filter) {
    delete cart_filter;
  }
  if(cart_player!=NULL) {
    delete cart_player;
  }
  delete cart_playout_map;
  delete cart_macro_map;
}


QSize RDCartDialog::sizeHint() const
{
  return QSize(640,400);
}

int RDCartDialog::exec(int *cartnum,RDCart::Type type,QString *svcname,
		       int svc_quan,const QString &username,
		       const QString &passwd,bool *temp_allowed)
{
  LoadState();
  cart_cartnum=cartnum;
  cart_type=type;
  cart_service=svcname;
  cart_service_quan=svc_quan;
  cart_user_name=username;
  cart_user_password=passwd;
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
  if(*cart_cartnum==0) {
    cart_ok_button->setDisabled(true);
  }
  switch(cart_filter_mode) {
    case RDStation::FilterAsynchronous:
      cart_search_button->setDefault(true);
      break;

    case RDStation::FilterSynchronous:
      cart_ok_button->setDefault(true);
      cart_search_button->hide();
  }
  BuildGroupList();
  cart_filter_edit->setText(*cart_filter);
  RefreshCarts();
  RDListViewItem *item=(RDListViewItem *)cart_cart_list->firstChild();
  while(item!=NULL) {
    if(item->text(1).toInt()==*cartnum) {
      cart_cart_list->setSelected(item,true);
      cart_cart_list->ensureItemVisible(item);
      clickedData(item);
      return QDialog::exec();
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  cart_filter_edit->setFocus();
  return QDialog::exec();
}


QSizePolicy RDCartDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDCartDialog::filterChangedData(const QString &str)
{
  cart_search_button->setEnabled(true);
  switch(cart_filter_mode) {
    case RDStation::FilterSynchronous:
      filterSearchedData();
      break;

    case RDStation::FilterAsynchronous:
      break;
  }
}


void RDCartDialog::filterSearchedData()
{
  if(cart_filter_edit->text().isEmpty()) {
    cart_clear_button->setDisabled(true);
  }
  else {
    cart_clear_button->setEnabled(true);
  }
  RefreshCarts();
}


void RDCartDialog::filterClearedData()
{
  cart_filter_edit->clear();
  filterChangedData("");
}


void RDCartDialog::groupActivatedData(const QString &group)
{
  filterChangedData("");
  if(cart_group!=NULL) {
    *cart_group=group;
  }
}


void RDCartDialog::schedcodeActivatedData(const QString &schedcode)
{
  filterChangedData("");
  if(cart_schedcode!=NULL) {
    *cart_schedcode=schedcode;
  }
  if(cart_schedcode!=NULL) {
    *cart_schedcode=schedcode;
  }
}


void RDCartDialog::limitChangedData(int state)
{
  filterChangedData("");
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


void RDCartDialog::doubleClickedData(Q3ListViewItem *,const QPoint &,int)
{
  okData();
}


void RDCartDialog::editorData()
{
  RDListViewItem *item=(RDListViewItem *)cart_cart_list->currentItem();
  if(item==NULL) {
    return;
  }

  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select CUTS.CUT_NAME,CUTS.LENGTH,CART.GROUP_NAME,\
                         CART.TITLE,CART.ARTIST,CART.ALBUM,CART.YEAR,\
                         CART.LABEL,CART.CLIENT,CART.AGENCY,CART.COMPOSER,\
                         CART.PUBLISHER,CART.USER_DEFINED \
                         from CUTS left join CART \
                         on CUTS.CART_NUMBER=CART.NUMBER \
                         where (CUTS.CART_NUMBER=%u)&&(CUTS.LENGTH>0)",
			item->text(1).toUInt());
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }
  QString cmd=rda->station()->editorPath();
  cmd.replace("%f",RDCut::pathName(q->value(0).toString()));
  cmd.replace("%n",QString().sprintf("%06u",item->text(1).toUInt()));
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
    err=conv->runImport(cart_user_name,cart_user_password,&conv_err);
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
  RDListViewItem *item=(RDListViewItem *)cart_cart_list->currentItem();
  if(item==NULL) {
    return;
  }

  SaveState();
  if(cart_player!=NULL) {
    cart_player->stop();
  }
  if(!local_filter) {
    *cart_filter=cart_filter_edit->text();
  }
  *cart_cartnum=item->text(1).toInt();
  if(cart_temp_allowed!=NULL) {
    *cart_temp_allowed=false;
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
  cart_filter_label->setGeometry(10,10,85,20);

  cart_search_button->setGeometry(size().width()-160,5,70,30);
  cart_clear_button->setGeometry(size().width()-80,5,70,30);
  cart_group_box->setGeometry(100,40,150,20);
  cart_group_label->setGeometry(10,40,85,20);
  cart_schedcode_box->setGeometry(390,40,150,20);
  cart_schedcode_label->setGeometry(280,40,105,20);
  cart_limit_box->setGeometry(100,72,15,15);
  cart_limit_label->setGeometry(120,70,300,20);
  cart_cart_label->setGeometry(15,90,100,20);
  cart_cart_list->setGeometry(10,110,size().width()-20,size().height()-180);
  cart_editor_button->setGeometry(235,size().height()-60,80,50);
  cart_file_button->setGeometry(325,size().height()-60,80,50);
  cart_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  cart_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
  switch(cart_filter_mode) {
    case RDStation::FilterAsynchronous:
      cart_filter_edit->setGeometry(100,10,size().width()-280,20);
      break;

    case RDStation::FilterSynchronous:
      cart_filter_edit->setGeometry(100,10,size().width()-200,20);
      break;
  }
  if(cart_player!=NULL) {
    cart_player->playButton()->setGeometry(10,size().height()-60,80,50);
    cart_player->stopButton()->setGeometry(100,size().height()-60,80,50);
  }
}


void RDCartDialog::closeEvent(QCloseEvent *e)
{
  if(cart_player!=NULL) {
    cart_player->stop();
  }
  cancelData();
}


void RDCartDialog::RefreshCarts()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *l;

  cart_cart_list->clear();
  QString group=cart_group_box->currentText();
  if(group==QString(tr("ALL"))) {
    group="";
  }
  QString schedcode=cart_schedcode_box->currentText();
  if(schedcode==QString(tr("ALL"))) {
    schedcode="";
  }
  if(cart_type==RDCart::All) {
    sql=QString("select ")+
      "CART.NUMBER,"+          // 00
      "CART.TITLE,"+           // 01
      "CART.ARTIST,"+          // 02
      "CART.CLIENT,"+          // 03
      "CART.AGENCY,"+          // 04
      "CART.USER_DEFINED,"+    // 05
      "CART.COMPOSER,"+        // 06
      "CART.CONDUCTOR,"+       // 07
      "CART.START_DATETIME,"+  // 08
      "CART.END_DATETIME,"+    // 09
      "CART.TYPE,"+            // 10
      "CART.FORCED_LENGTH,"+   // 11
      "CART.GROUP_NAME,"+      // 12
      "GROUPS.COLOR "+         // 13
      "from CART left join GROUPS "+
      "on CART.GROUP_NAME=GROUPS.NAME "+
      GetSearchFilter(cart_filter_edit->text(),group,schedcode);
  }
  else {
    sql=QString("select ")+
      "CART.NUMBER,"+          // 00
      "CART.TITLE,"+           // 01
      "CART.ARTIST,"+          // 02
      "CART.CLIENT,"+          // 03
      "CART.AGENCY,"+          // 04
      "CART.USER_DEFINED,"+    // 05
      "CART.COMPOSER,"+        // 06
      "CART.CONDUCTOR,"+       // 07
      "CART.START_DATETIME,"+  // 08
      "CART.END_DATETIME,"+    // 09
      "CART.TYPE,"+            // 10
      "CART.FORCED_LENGTH,"+   // 11
      "CART.GROUP_NAME,"+      // 12
      "GROUPS.COLOR "+         // 13
      "from CART left join GROUPS "+
      "on CART.GROUP_NAME=GROUPS.NAME "+
      GetSearchFilter(cart_filter_edit->text(),group,schedcode)+"&&"+
      QString().sprintf("(TYPE=%d)",cart_type);
  }
  if(cart_limit_box->isChecked()) {
    sql+=QString().sprintf(" limit %d",RD_LIMITED_CART_SEARCH_QUANTITY);
  }
  q=new RDSqlQuery(sql);
  int step=0;
  int count=0;
  cart_progress_dialog->setMaximum(q->size()/RDCART_DIALOG_STEP_SIZE);
  cart_progress_dialog->setValue(0);
  while(q->next()) {
    l=new RDListViewItem(cart_cart_list);
    l->setId(q->value(10).toUInt());
    switch((RDCart::Type)q->value(10).toUInt()) {
	case RDCart::Audio:
	  l->setPixmap(0,*cart_playout_map);
	  break;

	case RDCart::Macro:
	  l->setPixmap(0,*cart_macro_map);
	  break;

	default:
	  break;
    }
    l->setText(1,QString().sprintf("%06d",q->value(0).toUInt())); // Number
    l->setText(2,RDGetTimeLength(q->value(11).toInt(),false,true)); // Length
    l->setText(3,q->value(1).toString());                       // Title
    l->setText(4,q->value(2).toString());                       // Artist
    l->setText(5,q->value(12).toString());                      // Group
    l->setText(6,q->value(6).toString());                      // Composer
    l->setText(7,q->value(7).toString());                      // Conductor
    l->setTextColor(5,q->value(13).toString(),QFont::Bold);
    l->setText(8,q->value(3).toString());                       // Client
    l->setText(9,q->value(4).toString());                       // Agency
    l->setText(10,q->value(5).toString());                       // User Defined
    if(!q->value(8).toDate().isNull()) {
      l->setText(11,q->value(8).toDate().toString("MM/dd/yyyy"));  // Start Date
    }
    if(!q->value(10).toDate().isNull()) {
      l->setText(12,q->value(9).toDate().toString("MM/dd/yyyy"));  // End Date
    }
    else {
      l->setText(12,"TFN");
    }
    if(count++>RDCART_DIALOG_STEP_SIZE) {
      cart_progress_dialog->setValue(++step);
      count=0;
      QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
  }
  cart_progress_dialog->reset();
  delete q;
  cart_search_button->setDisabled(true);
}


void RDCartDialog::BuildGroupList()
{
  QString sql;
  RDSqlQuery *q;
  
  //
  // Groups
  //
  cart_group_box->clear();
  cart_group_box->insertItem(tr("ALL"));
  sql="select GROUP_NAME from AUDIO_PERMS";
  if(cart_service_quan>0) {
    sql+=" where ";
    for(int i=0;i<cart_service_quan;i++) {
      if(!cart_service[i].isEmpty()) {
	sql+=QString("(SERVICE_NAME=\"")+
	  RDEscapeString(cart_service[i])+"\")||";
      }
    }
    sql=sql.left(sql.length()-2);
  }
  sql+=" order by GROUP_NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cart_group_box->insertItem(q->value(0).toString(),true);
  }
  delete q;

  //
  // Preselect Group
  //
  if(cart_group!=NULL) {
    for(int i=0;i<cart_group_box->count();i++) {
      if(*cart_group==cart_group_box->text(i)) {
	cart_group_box->setCurrentItem(i);
	break;
      }
    }
  }

  //
  // Scheduler Codes
  //
  cart_schedcode_box->clear();
  cart_schedcode_box->insertItem(tr("ALL"));
  sql="select CODE from SCHED_CODES";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cart_schedcode_box->insertItem(q->value(0).toString());
  }
  delete q;

  //
  // Preselect Scheduler Code
  //
  if(cart_schedcode!=NULL) {
    for(int i=0;i<cart_schedcode_box->count();i++) {
      if(*cart_schedcode==cart_schedcode_box->text(i)) {
	cart_schedcode_box->setCurrentItem(i);
	break;
      }
    }
  }
}


QString RDCartDialog::GetSearchFilter(const QString &filter,
				      const QString &group,
				      const QString &schedcode)
{
  QString sql;
  RDSqlQuery *q;
  QString sched="";
  
  if(schedcode!=tr("ALL")) {
    sched=schedcode;
  }
  QString search=RDCartSearchText(filter,group,sched,false);

  //
  // Excluded Groups
  //
  sql=QString().sprintf("select NAME from GROUPS where ");
  for(int i=1;i<cart_group_box->count();i++) {
    sql+=QString("(NAME!=\"")+RDEscapeString(cart_group_box->text(i))+"\")&&";
  }
  sql=sql.left(sql.length()-2);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    search+=QString("&&(GROUP_NAME!=\"")+
      RDEscapeString(q->value(0).toString())+"\")";
  }
  delete q;
  return search;
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

  bool value_read = false;
  cart_limit_box->setChecked(p->boolValue("RDCartDialog", "LimitSearch", true, &value_read));

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
  if(cart_limit_box->isChecked()) {
    fprintf(f,"LimitSearch=Yes\n");
  }
  else {
    fprintf(f,"LimitSearch=No\n");
  }
  fclose(f);
}
