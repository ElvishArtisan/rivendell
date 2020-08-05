// rdlibrary.cpp
//
// The Library Utility for Rivendell.
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

#include <sys/wait.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qshortcut.h>
#include <qtranslator.h>

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

//
// Prototypes
//
void SigHandler(int signo);

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/rdlibrary-22x22.xpm"

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
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
  // Create Icons
  //
  lib_playout_map=new QPixmap(play_xpm);
  lib_macro_map=new QPixmap(rml5_xpm);
  lib_track_cart_map=new QPixmap(track_cart_xpm);
  lib_rivendell_map=new QPixmap(rdlibrary_22x22_xpm);
  setWindowIcon(*lib_rivendell_map);

  //
  // Progress Dialog
  //
  lib_progress_dialog=
    new QProgressDialog(tr("Please Wait..."),tr("Cancel"),0,10,this,
			Qt::WStyle_Customize|Qt::WStyle_NormalBorder);
  lib_progress_dialog->setCaption(" ");
  QLabel *label=new QLabel(tr("Please Wait..."),lib_progress_dialog);
  label->setAlignment(Qt::AlignCenter);
  label->setFont(progressFont());
  lib_progress_dialog->setLabel(label);
  lib_progress_dialog->setCancelButton(NULL);
  lib_progress_dialog->setMinimumDuration(2000);

  //
  // Open the Database
  //
  rda=new RDApplication("RDLibrary","rdlibrary",RDLIBRARY_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDLibrary - "+tr("Error"),err_msg);
    exit(1);
  }

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
  lib_filter_mode=rda->station()->filterMode();
  rdaudioport_conf=new RDAudioPort(rda->config()->stationName(),
				   rda->libraryConf()->inputCard());
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
  cut_clipboard=NULL;
  lib_user_timer=new QTimer(this);
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
  lib_filter_edit=new QLineEdit(this);
  lib_filter_label=new QLabel(lib_filter_edit,tr("Filter:"),this);
  lib_filter_label->setFont(labelFont());
  lib_filter_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(lib_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  connect(lib_filter_edit,SIGNAL(returnPressed()),
	  this,SLOT(searchClickedData()));

  //
  // Filter Search Button
  //
  lib_search_button=new QPushButton(tr("&Search"),this);
  lib_search_button->setFont(buttonFont());
  connect(lib_search_button,SIGNAL(clicked()),this,SLOT(searchClickedData()));
  switch(lib_filter_mode) {
  case RDStation::FilterSynchronous:
    lib_search_button->hide();
    break;

  case RDStation::FilterAsynchronous:
    break;
  }

  //
  // Filter Clear Button
  //
  lib_clear_button=new QPushButton(tr("&Clear"),this);
  lib_clear_button->setFont(buttonFont());
  lib_clear_button->setDisabled(true);
  connect(lib_clear_button,SIGNAL(clicked()),this,SLOT(clearClickedData()));

  //
  // Group Filter
  //
  lib_group_box=new QComboBox(this);
  lib_group_label=new QLabel(lib_group_box,tr("Group:"),this);
  lib_group_label->setFont(labelFont());
  lib_group_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(lib_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Scheduler Codes Filter
  //
  lib_codes_box=new QComboBox(this);
  lib_codes_label=new QLabel(lib_codes_box,tr("Scheduler Code:"),this);
  lib_codes_label->setFont(labelFont());
  lib_codes_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(lib_codes_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Scheduler Codes2 Filter
  //
  lib_codes2_box=new QComboBox(this);
  lib_codes2_label=new QLabel(lib_codes2_box,tr("And Scheduler Code:"),this);
  lib_codes2_label->setFont(labelFont());
  lib_codes2_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(lib_codes2_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Results Counter
  //
  lib_matches_edit=new QLineEdit(this);
  lib_matches_edit->setReadOnly(true);
  lib_matches_label=new QLabel(lib_matches_edit,tr("Matching Carts:"),this);
  lib_matches_label->setFont(labelFont());

  //
  // Show Allow Cart Drags Checkbox
  //
  lib_allowdrag_box=new QCheckBox(this);
  lib_allowdrag_box->setChecked(false);
  lib_allowdrag_label=
    new QLabel(lib_allowdrag_box,tr("Allow Cart Dragging"),this);
  lib_allowdrag_label->setFont(labelFont());
  lib_allowdrag_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(lib_allowdrag_box,SIGNAL(stateChanged(int)),
	  this,SLOT(dragsChangedData(int)));
  if(!rda->station()->enableDragdrop()) {
    lib_allowdrag_box->hide();
    lib_allowdrag_label->hide();
  }

  //
  // Show Audio Carts Checkbox
  //
  lib_showaudio_box=new QCheckBox(this);
  lib_showaudio_box->setChecked(true);
  lib_showaudio_label=new QLabel(lib_showaudio_box,tr("Show Audio Carts"),this);
  lib_showaudio_label->setFont(labelFont());
  lib_showaudio_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(lib_showaudio_box,SIGNAL(stateChanged(int)),
	  this,SLOT(audioChangedData(int)));

  //
  // Show Macro Carts Checkbox
  //
  lib_showmacro_box=new QCheckBox(this);
  lib_showmacro_box->setChecked(true);
  lib_showmacro_label=new QLabel(lib_showmacro_box,tr("Show Macro Carts"),this);
  lib_showmacro_label->setFont(labelFont());
  lib_showmacro_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(lib_showmacro_box,SIGNAL(stateChanged(int)),
	  this,SLOT(macroChangedData(int)));

  //
  // Show Cart Notes Checkbox
  //
  lib_shownotes_box=new QCheckBox(this);
  lib_shownotes_box->setChecked(true);
  lib_shownotes_label=
    new QLabel(lib_shownotes_box,tr("Show Note Bubbles"),this);
  lib_shownotes_label->setFont(labelFont());
  lib_shownotes_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Show Matches Checkbox
  //
  lib_showmatches_box=new QCheckBox(this);
  lib_showmatches_label=
    new QLabel(lib_showmatches_box,tr("Show Only First ")+
	       QString().sprintf("%d",RD_LIMITED_CART_SEARCH_QUANTITY)+
	       tr(" Matches"),this);
  lib_showmatches_label->setFont(labelFont());
  lib_showmatches_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(lib_showmatches_box,SIGNAL(stateChanged(int)),
	  this,SLOT(searchLimitChangedData(int)));

  //
  // Cart List
  //
  lib_cart_list=new LibListView(this);
  lib_cart_list->setAllColumnsShowFocus(true);
  lib_cart_list->setItemMargin(5);
  lib_cart_list->setSelectionMode(Q3ListView::Extended);
  lib_cart_list->setRootIsDecorated(true);
  lib_cart_list->enableNoteBubbles(true);
  lib_cart_list->setContiguous(false);
  connect(lib_cart_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(cartDoubleclickedData(Q3ListViewItem *,const QPoint &,int)));
  connect(lib_cart_list,SIGNAL(selectionChanged()),
	  this,SLOT(cartClickedData()));
  connect(lib_cart_list,SIGNAL(onItem(Q3ListViewItem *)),
	  this,SLOT(cartOnItemData(Q3ListViewItem *)));
  connect(lib_shownotes_box,SIGNAL(toggled(bool)),
  	  lib_cart_list,SLOT(enableNoteBubbles(bool)));
  lib_cart_list->addColumn("");
  lib_cart_list->setColumnAlignment(Icon,Qt::AlignHCenter);
  lib_cart_list->addColumn(tr("Cart"));
  lib_cart_list->setColumnAlignment(Cart,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Group"));
  lib_cart_list->setColumnAlignment(Group,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("Length"));
  lib_cart_list->setColumnAlignment(Length,Qt::AlignRight);
  lib_cart_list->setColumnSortType(Length,RDListView::TimeSort);

  lib_cart_list->addColumn(tr("Talk"));
  lib_cart_list->setColumnAlignment(Talk,Qt::AlignRight);
  lib_cart_list->setColumnSortType(Length,RDListView::TimeSort);

  lib_cart_list->addColumn(tr("Title"));
  lib_cart_list->setColumnAlignment(Title,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Artist"));
  lib_cart_list->setColumnAlignment(Artist,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Start"));
  lib_cart_list->setColumnAlignment(Start,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("End"));
  lib_cart_list->setColumnAlignment(End,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("Album"));
  lib_cart_list->setColumnAlignment(Album,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Label"));
  lib_cart_list->setColumnAlignment(Label,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Composer"));
  lib_cart_list->setColumnAlignment(Composer,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Conductor"));
  lib_cart_list->setColumnAlignment(Conductor,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Publisher"));
  lib_cart_list->setColumnAlignment(Publisher,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Client"));
  lib_cart_list->setColumnAlignment(Client,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Agency"));
  lib_cart_list->setColumnAlignment(Agency,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("User Defined"));
  lib_cart_list->setColumnAlignment(UserDefined,Qt::AlignLeft);

  lib_cart_list->addColumn(tr("Cuts"));
  lib_cart_list->setColumnAlignment(Cuts,Qt::AlignRight);

  lib_cart_list->addColumn(tr("Last Cut Played"));
  lib_cart_list->setColumnAlignment(LastCutPlayed,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("Enforce Length"));
  lib_cart_list->setColumnAlignment(EnforceLength,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("Preserve Pitch"));
  lib_cart_list->setColumnAlignment(PreservePitch,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("Length Deviation"));
  lib_cart_list->setColumnAlignment(LengthDeviation,Qt::AlignHCenter);

  lib_cart_list->addColumn(tr("Owned By"));
  lib_cart_list->setColumnAlignment(OwnedBy,Qt::AlignHCenter);

  //
  // Add Button
  //
  lib_add_button=new QPushButton(this);
  lib_add_button->setFont(buttonFont());
  lib_add_button->setText(tr("&Add"));
  connect(lib_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  lib_edit_button=new QPushButton(this);
  lib_edit_button->setFont(buttonFont());
  lib_edit_button->setText(tr("&Edit"));
  lib_edit_button->setEnabled(false);
  connect(lib_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  lib_delete_button=new QPushButton(this);
  lib_delete_button->setFont(buttonFont());
  lib_delete_button->setText(tr("&Delete"));
  lib_delete_button->setEnabled(false);
  connect(lib_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Run Macro Button
  //
  lib_macro_button=new QPushButton(this);
  lib_macro_button->setFont(buttonFont());
  lib_macro_button->setText(tr("Run\n&Macro"));
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
  lib_rip_button->setText(tr("&Rip\nCD"));
  connect(lib_rip_button,SIGNAL(clicked()),this,SLOT(ripData()));

  //
  // Reports Button
  //
  lib_reports_button=new QPushButton(this);
  lib_reports_button->setFont(buttonFont());
  lib_reports_button->setText(tr("Re&ports"));
  connect(lib_reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  // Close Button
  //
  lib_close_button=new QPushButton(this);
  lib_close_button->setFont(buttonFont());
  lib_close_button->setText(tr("&Close"));
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
  connect(lib_player_shortcut,SIGNAL(activated()),this,SLOT(playerShortcutData()));

  // 
  // Setup Signal Handling 
  //
  ::signal(SIGCHLD,SigHandler);

  //
  // Load Data
  //
  switch(rda->libraryConf()->limitSearch()) {
  case RDLibraryConf::LimitNo:
    lib_showmatches_box->setChecked(false);
    break;

  case RDLibraryConf::LimitYes:
    lib_showmatches_box->setChecked(true);
    break;

  case RDLibraryConf::LimitPrevious:
    lib_showmatches_box->setChecked(rda->libraryConf()->searchLimited());
    break;
  }

  lib_resize=true;

  //
  // Create RDMacroEvent for running macros
  //
  lib_macro_events=new RDMacroEvent(rda->station()->address(),rda->ripc(),this);

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
  QString sql;
  RDSqlQuery *q;

  if(lib_edit_pending) {
    lib_user_changed=true;
    return;
  }

  SetCaption(rda->ripc()->user());

  lib_group_box->clear();
  lib_group_box->insertItem(tr("ALL"));
  sql=QString("select GROUP_NAME from USER_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(rda->user()->name())+"\" order by GROUP_NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    lib_group_box->insertItem(q->value(0).toString());
  }
  delete q;

  if(lib_group_box->count()==1) {
    lib_add_button->setDisabled(true);
    lib_edit_button->setDisabled(true);
    lib_delete_button->setDisabled(true);
    lib_rip_button->setDisabled(true);
  }
  else {
    lib_add_button->setEnabled(rda->user()->createCarts());
    lib_rip_button->setEnabled(rda->user()->editAudio());
  }

  lib_codes_box->clear();
  lib_codes_box->insertItem(tr("ALL"));
  lib_codes2_box->clear();
  lib_codes2_box->insertItem(tr("ALL"));
  sql=QString().sprintf("select CODE from SCHED_CODES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    lib_codes_box->insertItem(q->value(0).toString());
    lib_codes2_box->insertItem(q->value(0).toString());
  }
  delete q;
  lib_search_button->setDisabled(true);
  groupActivatedData(lib_group_box->currentText());
}


void MainWidget::filterChangedData(const QString &str)
{
  lib_search_button->setEnabled(true);
  if(lib_filter_mode!=RDStation::FilterSynchronous) {
    return;
  }
  searchClickedData();
}


void MainWidget::searchClickedData()
{
  lib_search_button->setDisabled(true);
  if(lib_filter_edit->text().isEmpty()) {
    lib_clear_button->setDisabled(true);
  }
  else {
    lib_clear_button->setEnabled(true);
  }
  RefreshList();
}


void MainWidget::clearClickedData()
{
  lib_filter_edit->clear();
  filterChangedData("");
}


void MainWidget::groupActivatedData(const QString &str)
{
  if(str!=tr("ALL")) {
    lib_default_group=str;
  }
  filterChangedData("");
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
  sql=QString("insert into CART set ")+
    QString().sprintf("NUMBER=%u,TYPE=%d,",cart_num,cart_type)+
    "GROUP_NAME=\""+RDEscapeString(lib_default_group)+"\","+
    "TITLE=\""+RDEscapeString(cart_title)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  EditCart *cart=
    new EditCart(cart_num,&lib_import_path,true,profile_ripping,this);
  if(cart->exec() <0) {
    RDCart *rdcart=new RDCart(cart_num);
    rdcart->remove(rda->station(),rda->user(),rda->config());
    delete rdcart;
  } 
  else {
    RDListViewItem *item=new RDListViewItem(lib_cart_list);
    item->setText(Cart,QString().sprintf("%06u",cart_num));
    RefreshLine(item);
    SendNotification(RDNotification::AddAction,cart_num);
    Q3ListViewItemIterator it(lib_cart_list);
    while(it.current()) {
      lib_cart_list->setSelected(it.current(),false);
      ++it;
    }
    lib_cart_list->setSelected(item,true);
    lib_cart_list->ensureItemVisible(item);
  }
  delete cart;

  UnlockUser();
}



void MainWidget::editData()
{
  int sel_count=0;
  Q3ListViewItemIterator *it;

  lib_player->stop();
  LockUser();

  it=new Q3ListViewItemIterator(lib_cart_list);
  while(it->current()) {
    if (it->current()->isSelected() && !it->current()->parent()) {
      sel_count++;
    }
    ++(*it);
  }
  delete it;

  if(sel_count==0) {
    UnlockUser();
    return;
  }
  if(sel_count==1) { //single edit
    it=new Q3ListViewItemIterator(lib_cart_list);
    while(!it->current()->isSelected() || it->current()->parent()) {
      ++(*it);
    }
    RDListViewItem *item=(RDListViewItem *)it->current();

    EditCart *edit_cart=new EditCart(item->text(Cart).toUInt(),&lib_import_path,
				     false,profile_ripping,this);
    edit_cart->exec();
    RefreshLine(item);
    cartOnItemData(item);
    SendNotification(RDNotification::ModifyAction,item->text(Cart).toUInt());
    delete edit_cart;
    delete it;
  }
  else { //multi edit
    if(rda->user()->modifyCarts()) {
      EditCart *edit_cart=
	new EditCart(0,&lib_import_path,false,profile_ripping,this,"",
				       lib_cart_list);
    
      edit_cart->exec();
      delete edit_cart;
    
      it=new Q3ListViewItemIterator(lib_cart_list);
      while(it->current()) {
        if (it->current()->isSelected() && !it->current()->parent()) {
          RefreshLine((RDListViewItem *)it->current());
	  SendNotification(RDNotification::ModifyAction,
			   it->current()->text(1).toUInt());
        }
        ++(*it);
      }
      delete it;
    }
  }
  UnlockUser();
}


void MainWidget::macroData()
{
  Q3ListViewItemIterator *it;
  Q3ListViewItem *item;

  it=new Q3ListViewItemIterator(lib_cart_list);
  while(it->current()) {
    if (it->current()->isSelected()) {
      item=it->current();
      RDCart *rdcart=new RDCart(item->text(Cart).toUInt());
      lib_macro_events->clear();
      lib_macro_events->load(rdcart->macros());
      lib_macro_events->exec();
      delete rdcart;
      break;
    }
    ++(*it);
  }
  delete it;
}

void MainWidget::deleteData()
{
  QString filename;
  QString sql;
  RDSqlQuery *q;
  QString str;
  int sel_count=0;
  Q3ListViewItemIterator *it;
  bool del_flag;

  lib_player->stop();
  LockUser();

  it=new Q3ListViewItemIterator(lib_cart_list);
  while(it->current()) {
    if (it->current()->isSelected() && !it->current()->parent()) {
      sel_count++;
    }
    ++(*it);
  }
  delete it;

  if(sel_count==0) {
    UnlockUser();
    return;
  }

  str=QString(tr("Are you sure you want to delete cart(s)"));
  if(QMessageBox::question(this,tr("Delete Cart(s)"),str,QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    UnlockUser();
    return;
  }
  it=new Q3ListViewItemIterator(lib_cart_list);
  while(it->current()) {
    if (it->current()->isSelected() && !it->current()->parent()) {
    del_flag=true;
    RDListViewItem *item=(RDListViewItem *)it->current();
  sql=QString().sprintf("select CUT_NAME from RECORDINGS where \
                         (CUT_NAME like \"%06u_%%\")||(MACRO_CART=%u)",
			item->text(Cart).toUInt(),item->text(Cart).toUInt());
  q=new RDSqlQuery(sql);
  if(q->first()) {
      QString str=QString().sprintf(tr("Cart %06u is used in one or more RDCatch events!\n\
Do you still want to delete it?"),item->text(Cart).toUInt());
      if(QMessageBox::warning(this,tr("RDCatch Event Exists"),str,
			        QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
        del_flag=false;
    }
  }
  delete q;
  if(cut_clipboard!=NULL) {
    if(item->text(Cart).toUInt()==cut_clipboard->cartNumber()) {
      	QString str=QString().sprintf(tr("Deleting cart %06u will also empty the clipboard.\n\
      	Do you still want to proceed?"),item->text(Cart).toUInt());
        switch(QMessageBox::question(this,tr("Empty Clipboard"),str,
				  QMessageBox::Yes,
				  QMessageBox::No)) {
	case QMessageBox::No:
	case QMessageBox::NoButton:
	  del_flag=false;

	default:
	  break;
      }
      delete cut_clipboard;
      cut_clipboard=NULL;
    }
  }
  if(del_flag && item->text(OwnedBy).isEmpty()) {
    RDCart *rdcart=new RDCart(item->text(Cart).toUInt());
    if(!rdcart->remove(rda->station(),rda->user(),rda->config())) {
      QMessageBox::warning(this,tr("RDLibrary"),tr("Unable to delete audio!"));
      return;
    }
    SendNotification(RDNotification::DeleteAction,rdcart->number());
    delete rdcart;
    delete item;
  } 
  else {
    ++(*it);
  } 
    }
    else {
      ++(*it);
    } 
  }
  delete it;

  UnlockUser();
}


void MainWidget::ripData()
{
  lib_player->stop();
  LockUser();
  QString group=lib_group_box->currentText();
  QString schedcode=lib_codes_box->currentText();
  DiskRipper *dialog=new DiskRipper(&lib_filter_text,&group,&schedcode,
				    profile_ripping,this);
  if(dialog->exec()==0) {
    for(int i=0;i<lib_group_box->count();i++) {
      if(lib_group_box->text(i)==group) {
	lib_filter_edit->setText(lib_filter_text);
	lib_group_box->setCurrentItem(i);
	groupActivatedData(lib_group_box->currentText());
      }
    }
  }
  delete dialog;
  if(!UnlockUser()) {
    RefreshList();
  }
}


void MainWidget::reportsData()
{
  lib_player->stop();
  LockUser();
  ListReports *lr=
    new ListReports(lib_filter_edit->text(),GetTypeFilter(),
		    lib_group_box->currentText(),lib_codes_box->currentText(),
		    this);
  lr->exec();
  delete lr;
  UnlockUser();
}


void MainWidget::cartOnItemData(Q3ListViewItem *item)
{
  if((!lib_shownotes_box->isChecked())||(item==NULL)) {
    return;
  }
  //  lib_cart_tip->
  //    setCartNumber(lib_cart_list->itemRect(item),item->text(Cart).toUInt());
}


void MainWidget::cartClickedData()
{
  int del_count=0;
  int sel_count=0;
  int play_count=0;
  Q3ListViewItemIterator *it;
  Q3ListViewItem *item;

  it=new Q3ListViewItemIterator(lib_cart_list);
  while(it->current()) {
    if (it->current()->isSelected()) {
      play_count++;
      item=it->current();
      if(!it->current()->parent()) {
        sel_count++;
      }
      if(it->current()->text(OwnedBy).isEmpty() && !it->current()->parent()) {
        del_count++;
      }
    }
    ++(*it);
  }
  delete it;
  
  if(del_count>0) {
    lib_delete_button->setEnabled(rda->user()->deleteCarts());
  } 
  else {
    lib_delete_button->setEnabled(false);
  }

  if(play_count==1) {
    RDCart *rdcart=new RDCart(item->text(Cart).toUInt());
    if(rdcart->type()==RDCart::Macro) {
      lib_macro_button->setEnabled(true);
      lib_macro_button->setVisible(true);
      lib_player->playButton()->setEnabled(false);
      lib_player->stopButton()->setEnabled(false);
      lib_player->playButton()->setVisible(false);
      lib_player->stopButton()->setVisible(false);
      lib_player->stop();
    }
    else {
      lib_player->setCart(item->text(Cart));
      lib_player->playButton()->setEnabled(true);
      lib_player->stopButton()->setEnabled(true);
      lib_player->playButton()->setVisible(true);
      lib_player->stopButton()->setVisible(true);
      lib_macro_button->setEnabled(false);
      lib_macro_button->setVisible(false);
      if(lib_player->isPlaying()) {
        lib_player->play();
      }
    }
    delete rdcart;
  }
  else {
    lib_player->stop();
    lib_player->playButton()->setEnabled(false);
    lib_player->stopButton()->setEnabled(false);
    lib_player->playButton()->setVisible(true);
    lib_player->stopButton()->setVisible(true);
    lib_macro_button->setEnabled(false);
    lib_macro_button->setVisible(false);
  }

  if(sel_count) {
    if(del_count==0) {
      lib_edit_button->setEnabled(false);
    }
    else {
      lib_edit_button->setEnabled(rda->user()->modifyCarts());
    }
  } 
  else {
    lib_edit_button->setEnabled(false);
  }
}


void MainWidget::cartDoubleclickedData(Q3ListViewItem * item,const QPoint &,int)
{
  editData();

  //
  // KLUDGE ALERT:
  // The default double-click behavior of a Q3ListViewItem is to expand/collapse the item.
  // We don't want it to do that, so if we set the open status to the opposite of what we
  // want (the current status), it will put it back to what we do want. There is probably
  // a Qt way of doing this which should probably be addressed at a later date.
  //
  if(item!=NULL) {
    item->setOpen(!item->isOpen());
  }
}


void MainWidget::audioChangedData(int state)
{
  filterChangedData("");
}


void MainWidget::macroChangedData(int state)
{
  filterChangedData("");
}


void MainWidget::searchLimitChangedData(int state)
{
  rda->libraryConf()->setSearchLimited(state);
  filterChangedData("");
}


void MainWidget::dragsChangedData(int state)
{
  if(state) {
    lib_cart_list->setSelectionMode(Q3ListView::Single);
  }
  else {
    lib_cart_list->setSelectionMode(Q3ListView::Extended);
  }
}


void MainWidget::notificationReceivedData(RDNotification *notify)
{
  RDListViewItem *item=NULL;
  QString sql;
  RDSqlQuery *q;

  if(notify->type()==RDNotification::CartType) {
    unsigned cartnum=notify->id().toUInt();
    switch(notify->action()) {
    case RDNotification::AddAction:
      sql=QString("select CART.NUMBER from CART ")+
	"left join CUTS on CART.NUMBER=CUTS.CART_NUMBER "+
	WhereClause()+
	QString().sprintf(" && CART.NUMBER=%u ",cartnum);
      q=new RDSqlQuery(sql);
      if(q->first()) {
	item=new RDListViewItem(lib_cart_list);
	item->setText(Cart,QString().sprintf("%06u",cartnum));
	RefreshLine(item);
      }
      delete q;
      break;

    case RDNotification::ModifyAction:
      if((item=(RDListViewItem *)lib_cart_list->
	  findItem(QString().sprintf("%06u",cartnum),Cart))!=NULL) {
	RefreshLine(item);
      }
      break;

    case RDNotification::DeleteAction:
      if(lib_edit_pending) {
	lib_deleted_carts.push_back(cartnum);
      }
      else {
	if((item=(RDListViewItem *)lib_cart_list->findItem(QString().sprintf("%06u",cartnum),Cart))!=NULL) {
	  delete item;
	}
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
    switch(lib_filter_mode) {
    case RDStation::FilterSynchronous:
      lib_filter_edit->setGeometry(70,10,e->size().width()-170,20);
      break;

    case RDStation::FilterAsynchronous:
      lib_search_button->setGeometry(e->size().width()-180,10,80,50);
      lib_filter_edit->setGeometry(70,10,e->size().width()-260,20);
      break;
    }
    lib_clear_button->setGeometry(e->size().width()-90,10,80,50);
    lib_filter_label->setGeometry(10,10,55,20);
    lib_group_label->setGeometry(10,40,55,20);
    lib_group_box->setGeometry(70,40,100,20);
    lib_codes_label->setGeometry(175,40,115,20);
    lib_codes_box->setGeometry(295,40,100,20);
    lib_codes2_label->setGeometry(410,40,130,20);
    lib_codes2_box->setGeometry(545,40,100,20);
    lib_matches_label->setGeometry(660,40,100,20);
    lib_matches_edit->setGeometry(765,40,55,20);
    lib_showmatches_label->setGeometry(760,66,200,20);
    lib_showmatches_box->setGeometry(740,68,15,15);
    lib_allowdrag_label->setGeometry(580,66,130,20);
    lib_allowdrag_box->setGeometry(560,68,15,15);
    lib_showaudio_label->setGeometry(90,66,130,20);
    lib_showaudio_box->setGeometry(70,68,15,15);
    lib_showmacro_label->setGeometry(250,66,130,20);
    lib_showmacro_box->setGeometry(230,68,15,15);
    lib_shownotes_label->setGeometry(410,66,130,20);
    lib_shownotes_box->setGeometry(390,68,15,15);
    lib_cart_list->
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

void MainWidget::RefreshCuts(RDListViewItem *p,unsigned cartnum)
{
  RDListViewItem *l=NULL;
  Q3ListViewItem *i=NULL;
  RDSqlQuery *q;
  QString sql;
  QDateTime current_datetime(QDate::currentDate(),QTime::currentTime());
  QDateTime end_datetime;
  RDCart::Validity cart_validity=RDCart::NeverValid;
  RDCart::Validity cut_validity=RDCart::NeverValid;

  while ((i=p->firstChild())) {
    delete i;
  }

  sql=QString("select ")+
    "CUTS.CART_NUMBER,"+        // 00
    "CUTS.CUT_NAME,"+           // 01
    "CUTS.DESCRIPTION,"+        // 02
    "CUTS.TALK_START_POINT,"+   // 03
    "CUTS.TALK_END_POINT,"+     // 04
    "CUTS.LENGTH,"+             // 05  offsets begin here
    "CUTS.EVERGREEN,"+          // 06
    "CUTS.START_DATETIME,"+     // 07
    "CUTS.END_DATETIME,"+       // 08
    "CUTS.START_DAYPART,"+      // 09
    "CUTS.END_DAYPART,"+        // 10
    "CUTS.MON,"+                // 11
    "CUTS.TUE,"+                // 12
    "CUTS.WED,"+                // 13
    "CUTS.THU,"+                // 14
    "CUTS.FRI,"+                // 15
    "CUTS.SAT,"+                // 16
    "CUTS.SUN "+                // 17
    "from CUTS ";
  sql+=QString().sprintf("where CUTS.CART_NUMBER=%u ",cartnum);
  sql+="order by CUTS.CUT_NAME";
  q=new RDSqlQuery(sql);
  if (q->size()>1) {
    while(q->next()) {
      l=new RDListViewItem(p);
      l->setDragEnabled(false);
      l->setText(Cart,q->value(1).toString());
      l->setText(Length,RDGetTimeLength(q->value(5).toUInt()));
      l->setText(Talk,RDGetTimeLength(q->value(4).toUInt()-q->value(3).toUInt()));
      l->setText(Title,q->value(2).toString());
      if(!q->value(7).toDateTime().isNull()) {
        l->setText(Start,q->value(7).toDateTime().
		   toString("MM/dd/yyyy hh:mm:ss"));
      }
      if(!q->value(8).toDateTime().isNull()) {
        l->setText(End,q->value(8).toDateTime().
		   toString("MM/dd/yyyy - hh:mm:ss"));
      }
      else {
        l->setText(End,"TFN");
      }
      end_datetime=q->value(8).toDateTime();
      cut_validity=ValidateCut(q,5,RDCart::NeverValid,current_datetime);
      UpdateItemColor(l,cut_validity,end_datetime,current_datetime);
      cart_validity=ValidateCut(q,5,cart_validity,current_datetime);
      UpdateItemColor(p,cart_validity,end_datetime,current_datetime);
    }
  }
  else if(q->size()==1){
    if(q->next()) {
      cart_validity=ValidateCut(q,5,cart_validity,current_datetime);
      end_datetime=q->value(8).toDateTime();
      UpdateItemColor(p,cart_validity,end_datetime,current_datetime);
    }
  }
  else {
    p->setBackgroundColor(RD_CART_ERROR_COLOR);
  }

  delete q;
}

void MainWidget::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  RDListViewItem *l=NULL;
  QString type_filter;
  QDateTime current_datetime(QDate::currentDate(),QTime::currentTime());
  QDateTime end_datetime;

  lib_cart_list->clear();

  lib_edit_button->setEnabled(false);
  lib_delete_button->setEnabled(false);

  type_filter=GetTypeFilter();
  if(type_filter.isEmpty()) {
    return;
  }
  sql=QString("select ")+
    "CART.NUMBER,"+             // 00
    "CART.FORCED_LENGTH,"+      // 01
    "CART.TITLE,"+              // 02
    "CART.ARTIST,"+             // 03
    "CART.ALBUM,"+              // 04
    "CART.LABEL,"+              // 05
    "CART.CLIENT,"+             // 06
    "CART.AGENCY,"+             // 07
    "CART.USER_DEFINED,"+       // 08
    "CART.COMPOSER,"+           // 09
    "CART.PUBLISHER,"+          // 10
    "CART.CONDUCTOR,"+          // 11
    "CART.GROUP_NAME,"+         // 12
    "CART.START_DATETIME,"+     // 13
    "CART.END_DATETIME,"+       // 14
    "CART.TYPE,"+               // 15
    "CART.CUT_QUANTITY,"+       // 16
    "CART.LAST_CUT_PLAYED,"+    // 17
    "CART.ENFORCE_LENGTH,"+     // 18
    "CART.PRESERVE_PITCH,"+     // 19
    "CART.LENGTH_DEVIATION,"+   // 20
    "CART.OWNER,"+              // 21
    "CART.VALIDITY,"+           // 22
    "GROUPS.COLOR,"+            // 23
    "CUTS.TALK_START_POINT,"+   // 24
    "CUTS.TALK_END_POINT "+     // 25
    "from CART left join GROUPS on CART.GROUP_NAME=GROUPS.NAME "+
    "left join CUTS on CART.NUMBER=CUTS.CART_NUMBER";
  sql+=WhereClause();
  sql+=" group by CART.NUMBER order by CART.NUMBER";
  if(lib_showmatches_box->isChecked()) {
    sql+=QString().sprintf(" limit %d",RD_LIMITED_CART_SEARCH_QUANTITY);
  }
  q=new RDSqlQuery(sql);
  int step=0;
  int count=0;
  int matches=0;
  lib_progress_dialog->setMaximum(q->size()/RDLIBRARY_STEP_SIZE);
  lib_progress_dialog->setValue(0);
  while(q->next()) {
    end_datetime=q->value(14).toDateTime();

    //
    // Start a new entry
    //
    l=new RDListViewItem(lib_cart_list);
    l->setExpandable(false);
    switch((RDCart::Type)q->value(15).toUInt()) {
      case RDCart::Audio:
	if(q->value(21).isNull()) {
	  l->setPixmap(Icon,*lib_playout_map);
	}
	else {
	  l->setPixmap(Icon,*lib_track_cart_map);
	}
	break;
	
      case RDCart::Macro:
	l->setPixmap(Icon,*lib_macro_map);
	l->setBackgroundColor(backgroundColor());
	break;
	
      case RDCart::All:
	break;
    }
    l->setText(Cart,QString().sprintf("%06d",q->value(0).toUInt()));
    l->setText(Group,q->value(12).toString());
    l->setTextColor(Group,q->value(23).toString(),QFont::Bold);
    if(q->value(16).toUInt()==1) {
      l->setText(Length,RDGetTimeLength(q->value(1).toUInt()));
      l->setText(Talk,RDGetTimeLength(q->value(25).toUInt()-q->value(24).toUInt()));
    }
    l->setText(Title,q->value(2).toString());
    l->setText(Artist,q->value(3).toString());
    if(!q->value(13).toDateTime().isNull()) {
      l->setText(Start,q->value(13).toDateTime().
		   toString("MM/dd/yyyy - hh:mm:ss"));
    }
    if(!q->value(14).toDateTime().isNull()) {
      l->setText(End,q->value(14).toDateTime().
		   toString("MM/dd/yyyy - hh:mm:ss"));
    }
    else {
      l->setText(End,"TFN");
    }
    l->setText(Album,q->value(4).toString());
    l->setText(Label,q->value(5).toString());
    l->setText(Composer,q->value(9).toString());
    l->setText(Conductor,q->value(11).toString());
    l->setText(Publisher,q->value(10).toString());
    l->setText(Client,q->value(6).toString());
    l->setText(Agency,q->value(7).toString());
    l->setText(UserDefined,q->value(8).toString());
    l->setText(Cuts,q->value(16).toString());
    l->setText(LastCutPlayed,q->value(17).toString());
    l->setText(EnforceLength,q->value(18).toString());
    l->setText(PreservePitch,q->value(19).toString());
    l->setText(LengthDeviation,q->value(20).toString());
    l->setText(OwnedBy,q->value(21).toString());
    if(q->value(18).toString()=="Y") {
      l->setTextColor(Length,QColor(RDLIBRARY_ENFORCE_LENGTH_COLOR),QFont::Bold);
    }
    else {
      if((q->value(20).toUInt()>RDLIBRARY_MID_LENGTH_LIMIT)&&
	   (q->value(18).toString()=="N")) {
        if(q->value(20).toUInt()>RDLIBRARY_MAX_LENGTH_LIMIT) {
          l->setTextColor(Length,QColor(RDLIBRARY_MAX_LENGTH_COLOR),QFont::Bold);
        }
        else {
          l->setTextColor(Length,QColor(RDLIBRARY_MID_LENGTH_COLOR),QFont::Bold);
        }
      }
      else {
        l->setTextColor(Length,QColor(Qt::black),QFont::Normal);
      }
    }
    if((RDCart::Type)q->value(15).toUInt()==RDCart::Audio) {
      RefreshCuts(l,q->value(0).toUInt());
    }
    else {
      l->setBackgroundColor(palette().color(QPalette::Active,QColorGroup::Base));
    }
    matches++;
    count++;

    if(count>RDLIBRARY_STEP_SIZE) {
      lib_progress_dialog->setValue(++step);
      count=0;
      qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
  }
  lib_progress_dialog->reset();
  lib_matches_edit->setText(QString().sprintf("%d",matches));
  delete q;
}


QString MainWidget::WhereClause() const
{
  QString sql="";
  QString type_filter=GetTypeFilter();

  QStringList schedcodes;
  if(lib_codes_box->currentText()!=tr("ALL")) {
    schedcodes << lib_codes_box->currentText();
  }
  if(lib_codes2_box->currentText()!=tr("ALL")) {
    schedcodes << lib_codes2_box->currentText();
  }
  if(lib_group_box->currentText()==QString(tr("ALL"))) {
    sql=RDAllCartSearchText(lib_filter_edit->text(),schedcodes,
			  rda->user()->name(),true)+" && "+type_filter;

  }
  else {
    sql=RDCartSearchText(lib_filter_edit->text(),lib_group_box->currentText(),
		       schedcodes,true)+" && "+type_filter;      
  }

  return sql;
}


void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
  case SIGCHLD:
    pLocalPid=waitpid(-1,NULL,WNOHANG);
    while(pLocalPid>0) {
      pLocalPid=waitpid(-1,NULL,WNOHANG);
    }
    ripper_running=false;
    import_active=false;
    signal(SIGCHLD,SigHandler);
    break;
  }
}


void MainWidget::RefreshLine(RDListViewItem *item)
{
  RDCart::Validity validity=RDCart::NeverValid;
  QDateTime current_datetime(QDate::currentDate(),QTime::currentTime());
  QString sql=QString("select ")+
    "CART.FORCED_LENGTH,"+     // 00
    "CART.TITLE,"+             // 01
    "CART.ARTIST,"+            // 02
    "CART.ALBUM,"+             // 03
    "CART.LABEL,"+             // 04
    "CART.CLIENT,"+            // 05
    "CART.AGENCY,"+            // 06
    "CART.USER_DEFINED,"+      // 07
    "CART.COMPOSER,"+          // 08
    "CART.CONDUCTOR,"+         // 09
    "CART.PUBLISHER,"+         // 10
    "CART.GROUP_NAME,"+        // 11
    "CART.START_DATETIME,"+    // 12
    "CART.END_DATETIME,"+      // 13
    "CART.TYPE,"+              // 14
    "CART.CUT_QUANTITY,"+      // 15
    "CART.LAST_CUT_PLAYED,"+   // 16
    "CART.ENFORCE_LENGTH,"+    // 17
    "CART.PRESERVE_PITCH,"+    // 18
    "CART.LENGTH_DEVIATION,"+  // 19
    "CART.OWNER,"+             // 20
    "CART.VALIDITY,"+          // 21
    "GROUPS.COLOR,"+           // 22
    "CUTS.TALK_START_POINT,"+  // 23
    "CUTS.TALK_END_POINT "+    // 24
    "from CART left join GROUPS "+
    "on CART.GROUP_NAME=GROUPS.NAME left join CUTS on "+
    "CART.NUMBER=CUTS.CART_NUMBER where "+
    QString().sprintf("CART.NUMBER=%u ",item->text(Cart).toUInt())+
    "group by CART.NUMBER";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    switch((RDCart::Type)q->value(14).toUInt()) {
    case RDCart::Audio:
      if(q->value(20).isNull()) {
	item->setPixmap(Icon,*lib_playout_map);
      }
      else {
	item->setPixmap(Icon,*lib_track_cart_map);
      }
      if(q->value(0).toUInt()==0) {
	item->setBackgroundColor(RD_CART_ERROR_COLOR);
      }
      else {
	UpdateItemColor(item,validity,
			q->value(13).toDateTime(),current_datetime);
      }
      break;

    case RDCart::Macro:
      item->setPixmap(Icon,*lib_macro_map);
      break;

    case RDCart::All:
      break;
    }
    item->setText(Group,q->value(11).toString());
    item->setTextColor(Group,q->value(22).toString(),QFont::Bold);
    if(q->value(15).toUInt()==1) {
	    item->setText(Length,RDGetTimeLength(q->value(0).toUInt()));
            item->setText(Talk,RDGetTimeLength(q->value(24).toUInt()-q->value(23).toUInt()));
    }
    item->setText(Title,q->value(1).toString());
    item->setText(Artist,q->value(2).toString());
    item->setText(Album,q->value(3).toString());
    item->setText(Label,q->value(4).toString());
    item->setText(Composer,q->value(8).toString());
    item->setText(Conductor,q->value(9).toString());

    item->setText(Publisher,q->value(10).toString());
    item->setText(Client,q->value(5).toString());
    item->setText(Agency,q->value(6).toString());
    if(!q->value(12).toDateTime().isNull()) {
      item->setText(Start,q->value(12).toDateTime().
		    toString("MM/dd/yyyy - hh:mm:ss"));
    }
    else {
      item->setText(Start,"");
    }
    if(!q->value(13).toDateTime().isNull()) {
      item->setText(End,q->value(13).toDateTime().
		    toString("MM/dd/yyyy - hh:mm:ss"));
    }
    else {
      item->setText(End,tr("TFN"));
    }
    item->setText(Cuts,q->value(15).toString());
    item->setText(LastCutPlayed,q->value(16).toString());
    item->setText(EnforceLength,q->value(17).toString());
    item->setText(PreservePitch,q->value(18).toString());
    item->setText(LengthDeviation,q->value(19).toString());
    item->setText(OwnedBy,q->value(20).toString());
    if(q->value(17).toString()=="Y") {
      item->setTextColor(Length,QColor(RDLIBRARY_ENFORCE_LENGTH_COLOR),QFont::Bold);
    }
    else {
      if((q->value(19).toUInt()>RDLIBRARY_MID_LENGTH_LIMIT)&&
	 (q->value(17).toString()=="N")) {
	if(q->value(19).toUInt()>RDLIBRARY_MAX_LENGTH_LIMIT) {
	  item->setTextColor(Length,QColor(RDLIBRARY_MAX_LENGTH_COLOR),QFont::Bold);
	}
	else {
	  item->setTextColor(Length,QColor(RDLIBRARY_MID_LENGTH_COLOR),QFont::Bold);
	}
      }
      else {
	item->setTextColor(Length,QColor(Qt::black),QFont::Normal);
      }
    }

    if((RDCart::Type)q->value(14).toUInt()==RDCart::Audio) {
      RefreshCuts(item,item->text(Cart).toUInt());
    }
  }
  delete q;
}


void MainWidget::UpdateItemColor(RDListViewItem *item,
				 RDCart::Validity validity,
				 const QDateTime &end_datetime,
				 const QDateTime &current_datetime)
{
  if(item!=NULL) {
    switch(validity) {
    case RDCart::NeverValid:
      item->setBackgroundColor(RD_CART_ERROR_COLOR);
      break;
	
    case RDCart::ConditionallyValid:
      if(end_datetime.isValid()&&
	 (end_datetime<current_datetime)) {
	item->setBackgroundColor(RD_CART_ERROR_COLOR);
      }
      else {
	item->setBackgroundColor(RD_CART_CONDITIONAL_COLOR);
      }
      break;
	
    case RDCart::FutureValid:
      item->setBackgroundColor(RD_CART_FUTURE_COLOR);
      break;
	
    case RDCart::AlwaysValid:
      item->setBackgroundColor(palette().color(QPalette::Active,
					       QColorGroup::Base));
      break;

    case RDCart::EvergreenValid:
      item->setBackgroundColor(RD_CART_EVERGREEN_COLOR);
      break;
    }
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


QString MainWidget::GetTypeFilter() const
{
  QString type_filter;

  if(lib_showaudio_box->isChecked()) {
    if(lib_showmacro_box->isChecked()) {
      type_filter="((TYPE=1)||(TYPE=2)||(TYPE=3))";
    }
    else {
      type_filter="((TYPE=1)||(TYPE=3))";
    }
  }
  else {
    if(lib_showmacro_box->isChecked()) {
      type_filter="(TYPE=2)";
    }
  }
  return type_filter;
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
  QString geometry_file=GeometryFile();
  if(geometry_file.isEmpty()) {
    return;
  }
  RDProfile *profile=new RDProfile();
  profile->setSource(geometry_file);
  resize(profile->intValue("RDLibrary","Width",sizeHint().width()),
	 profile->intValue("RDLibrary","Height",sizeHint().height()));
  lib_shownotes_box->
    setChecked(profile->boolValue("RDLibrary","ShowNoteBubbles",true));
  lib_allowdrag_box->
    setChecked(profile->boolValue("RDLibrary","AllowCartDragging",false));

  delete profile;
}


void MainWidget::SaveGeometry()
{
  QString geometry_file=GeometryFile();
  FILE *file=fopen(geometry_file,"w");
  if(file==NULL) {
    return;
  }
  fprintf(file,"[RDLibrary]\n");
  fprintf(file,"Width=%d\n",geometry().width());
  fprintf(file,"Height=%d\n",geometry().height());
  fprintf(file,"ShowNoteBubbles=");
  if(lib_shownotes_box->isChecked()) {
    fputs("Yes\n",file);
  }
  else {
    fputs("No\n",file);
  }
  fprintf(file,"AllowCartDragging=");
  if(lib_allowdrag_box->isChecked()) {
    fprintf(file,"Yes\n");
  }
  else {
    fprintf(file,"No\n");
  }

  fclose(file);
}


void MainWidget::LockUser()
{
  lib_edit_pending=true;
}


bool MainWidget::UnlockUser()
{

  RDListViewItem *item=NULL;

  //
  // Process Deleted Carts
  //
  for(unsigned i=0;i<lib_deleted_carts.size();i++) {
    if((item=(RDListViewItem *)lib_cart_list->findItem(QString().sprintf("%06u",lib_deleted_carts.at(i)),Cart))!=NULL) {
      delete item;
    }
  }

  //
  // Process User Change
  //
  bool ret=lib_user_changed;
  lib_edit_pending=false;
  if(lib_user_changed) {
    lib_user_timer->start(0,true);
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


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);

  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString("/usr/share/qt4/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdlibrary_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  a.setMainWidget(w);
  w->show();
  return a.exec();
}
