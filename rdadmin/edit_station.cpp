// edit_station.cpp
//
// Edit a Rivendell Workstation Configuration
//
//   (C) Copyright 2002-2010,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>
#include <QPainter>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdcatch_connect.h>
#include <rdcart_dialog.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_audios.h"
#include "edit_backup.h"
#include "edit_cartslots.h"
#include "edit_decks.h"
#include "edit_jack.h"
#include "edit_rdairplay.h"
#include "edit_rdlibrary.h"
#include "edit_rdlogedit.h"
#include "edit_rdpanel.h"
#include "edit_station.h"
#include "edit_ttys.h"
#include "globals.h"
#include "list_dropboxes.h"
#include "list_hostvars.h"
#include "list_matrices.h"
#include "view_adapters.h"

EditStation::EditStation(QString sname,QWidget *parent)
  : QDialog(parent)
{
  RDSqlQuery *q;
  QString sql;
  int item=0;
  char temp[256];

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  station_station=new RDStation(sname);
  station_cae_station=NULL;

  setWindowTitle("RDAdmin - "+tr("Host: ")+sname);

  GetPrivateProfileString(RD_CONF_FILE,"Identity","Password",temp,"",255);
  station_catch_connect=new RDCatchConnect(0,this);
  station_catch_connect->connectHost(station_station->address().toString(),
				     RDCATCHD_TCP_PORT,temp);

  //
  // Validators
  //
  RDTextValidator *validator=new RDTextValidator(this);
  QIntValidator *macro_validator=new QIntValidator(1,RD_MAX_CART_NUMBER,this);


  //
  // Station Name
  //
  station_name_edit=new QLineEdit(this);
  station_name_edit->setGeometry(115,11,80,19);
  station_name_edit->setReadOnly(true);
  QLabel *station_name_label=
    new QLabel(station_name_edit,tr("Ho&st Name:"),this);
  station_name_label->setGeometry(10,11,100,19);
  station_name_label->setFont(font);
  station_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Station Description
  //
  station_description_edit=new QLineEdit(this);
  station_description_edit->setGeometry(115,32,sizeHint().width()-125,19);
  station_description_edit->setMaxLength(64);
  station_description_edit->setValidator(validator);
  QLabel *station_description_label=new QLabel(station_description_edit,
					       tr("&Description:"),this);
  station_description_label->setGeometry(10,32,100,19);
  station_description_label->setFont(font);
  station_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Station Default Name
  //
  station_default_name_edit=new QComboBox(this);
  station_default_name_edit->setGeometry(115,53,160,19);
  station_default_name_edit->setEditable(false);
  QLabel *station_default_name_label=new QLabel(station_default_name_edit,
						tr("Default &User:"),this);
  station_default_name_label->setGeometry(10,53,100,19);
  station_default_name_label->setFont(font);
  station_default_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Station IP Address
  //
  station_address_edit=new QLineEdit(this);
  station_address_edit->setGeometry(115,74,120,19);
  station_address_edit->setMaxLength(15);
  station_address_edit->setValidator(validator);
  QLabel *station_address_label=new QLabel(station_address_edit,
					   tr("&IP Address:"),this);
  station_address_label->setGeometry(10,74,100,19);
  station_address_label->setFont(font);
  station_address_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Station Editor Command
  //
  station_editor_cmd_edit=new QLineEdit(this);
  station_editor_cmd_edit->setGeometry(115,95,sizeHint().width()-130,19);
  station_editor_cmd_edit->setMaxLength(255);
  QLabel *station_editor_cmd_label=new QLabel(station_editor_cmd_edit,
					      tr("Editor &Command:"),this);
  station_editor_cmd_label->setGeometry(10,95,100,19);
  station_editor_cmd_label->setFont(font);
  station_editor_cmd_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Station Time Offset
  //
  station_timeoffset_box=new QSpinBox(this);
  station_timeoffset_box->setGeometry(115,116,80,19);
  station_timeoffset_box->setRange(-RD_MAX_TIME_OFFSET,RD_MAX_TIME_OFFSET);
  station_timeoffset_box->setSuffix(tr(" mS"));
  QLabel *station_timeoffset_label=new QLabel(station_timeoffset_box,
					      tr("&Time Offset:"),this);
  station_timeoffset_label->setGeometry(10,116,100,19);
  station_timeoffset_label->setFont(font);
  station_timeoffset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Startup Cart
  //
  station_startup_cart_edit=new QLineEdit(this);
  station_startup_cart_edit->setGeometry(115,137,60,19);
  station_startup_cart_edit->setMaxLength(15);
  station_startup_cart_edit->setValidator(macro_validator);
  QLabel *station_startup_cart_label=new QLabel(station_startup_cart_edit,
						tr("&Startup Cart:"),this);
  station_startup_cart_label->setGeometry(10,137,100,19);
  station_startup_cart_label->setFont(font);
  station_startup_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  QPushButton *select_button=new QPushButton(tr("Select"),this);
  select_button->setFont(small_font);
  select_button->setGeometry(180,136,50,22);
  connect(select_button,SIGNAL(clicked()),this,SLOT(selectClicked()));

  //
  // Cue Output
  //
  station_cue_sel=new RDCardSelector(this);
  station_cue_sel->setGeometry(90,158,110,117);
  QLabel *station_cue_sel_label=new QLabel(station_cue_sel,
					   tr("Cue &Output:"),this);
  station_cue_sel_label->setGeometry(10,158,100,19);
  station_cue_sel_label->setFont(font);
  station_cue_sel_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  station_start_cart_edit=new QLineEdit(this);
  station_start_cart_edit->setGeometry(270,158,60,20);
  station_start_cart_edit->setValidator(macro_validator);
  QLabel *station_start_cart_label=new QLabel(station_start_cart_edit,
					      tr("Start Cart")+":",this);
  station_start_cart_label->setGeometry(205,158,60,20);
  station_start_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  station_start_cart_button=new QPushButton(tr("Select"),this);
  station_start_cart_button->setGeometry(335,158,50,22);
  station_start_cart_button->setFont(small_font);
  connect(station_start_cart_button,SIGNAL(clicked()),
	  this,SLOT(startCartClickedData()));

  station_stop_cart_edit=new QLineEdit(this);
  station_stop_cart_edit->setGeometry(270,180,60,20);
  station_stop_cart_edit->setValidator(macro_validator);
  QLabel *station_stop_cart_label=new QLabel(station_stop_cart_edit,
					      tr("Stop Cart")+":",this);
  station_stop_cart_label->setGeometry(205,180,60,20);
  station_stop_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  station_stop_cart_button=new QPushButton(tr("Select"),this);
  station_stop_cart_button->setGeometry(335,179,50,22);
  station_stop_cart_button->setFont(small_font);
  connect(station_stop_cart_button,SIGNAL(clicked()),
	  this,SLOT(stopCartClickedData()));
  caeStationActivatedData(station_station->caeStation());

  //
  // Heartbeat Checkbox
  //
  station_heartbeat_box=new QCheckBox(this);
  station_heartbeat_box->setGeometry(10,203,15,15);
  QLabel *label=new QLabel(station_heartbeat_box,tr("Enable Heartbeat"),this);
  label->setGeometry(30,201,150,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);
  connect(station_heartbeat_box,SIGNAL(toggled(bool)),
	  this,SLOT(heartbeatToggledData(bool)));

  //
  // Filter Checkbox
  //
  station_filter_box=new QCheckBox(this);
  station_filter_box->setGeometry(210,203,15,15);
  label=new QLabel(station_filter_box,tr("Use Realtime Filtering"),this);
  label->setGeometry(230,201,150,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Heartbeat Cart
  //
  station_hbcart_edit=new QLineEdit(this);
  station_hbcart_edit->setGeometry(65,227,60,19);
  station_hbcart_edit->setValidator(macro_validator);
  station_hbcart_label=new QLabel(station_hbcart_edit,tr("Cart:"),this);
  station_hbcart_label->setGeometry(10,227,50,19);
  station_hbcart_label->setFont(font);
  station_hbcart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  station_hbcart_button=new QPushButton(this);
  station_hbcart_button->setGeometry(140,224,60,26);
  station_hbcart_button->setFont(font);
  station_hbcart_button->setText(tr("Select"));
  connect(station_hbcart_button,SIGNAL(clicked()),
	  this,SLOT(heartbeatClickedData()));

  //
  // Heartbeat Interval
  //
  station_hbinterval_spin=new QSpinBox(this);
  station_hbinterval_spin->setGeometry(275,227,45,19);
  station_hbinterval_spin->setRange(1,300);
  station_hbinterval_label=
    new QLabel(station_hbinterval_spin,tr("Interval:"),this);
  station_hbinterval_label->setGeometry(220,227,50,19);
  station_hbinterval_label->setFont(font);
  station_hbinterval_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  station_hbinterval_unit=new QLabel(tr("secs"),this);
  station_hbinterval_unit->setGeometry(325,227,100,19);
  station_hbinterval_unit->setFont(font);
  station_hbinterval_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // System Maintenance Checkbox
  //
  station_maint_box=new QCheckBox(this);
  station_maint_box->setGeometry(10,254,15,15);
  label=
    new QLabel(station_maint_box,tr("Include in System Maintenance Pool"),this);
  label->setGeometry(30,252,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Enable Drag & Drop Checkbox
  //
  station_dragdrop_box=new QCheckBox(this);
  station_dragdrop_box->setGeometry(10,275,15,15);
  label=new QLabel(station_dragdrop_box,tr("Enable Drag && Drop"),this);
  label->setGeometry(30,272,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Enforce Panel Setup Checkbox
  //
  station_panel_enforce_box=new QCheckBox(this);
  station_panel_enforce_box->setGeometry(25,293,15,15);
  station_panel_enforce_label=
    new QLabel(station_panel_enforce_box,
	       tr("Allow Drops on Panels not in Setup Mode"),this);
  station_panel_enforce_label->setGeometry(45,291,sizeHint().width()-55,20);
  station_panel_enforce_label->setFont(font);
  station_panel_enforce_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);
  connect(station_dragdrop_box,SIGNAL(toggled(bool)),
	  station_panel_enforce_label,SLOT(setEnabled(bool)));
  connect(station_dragdrop_box,SIGNAL(toggled(bool)),
	  station_panel_enforce_box,SLOT(setEnabled(bool)));

  //
  // System Services Section
  //
  label=new QLabel(tr("System Services"),this);
  label->setGeometry(30,312,110,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignCenter|Qt::TextShowMnemonic);
  label->setAutoFillBackground(true);

  //
  // HTTP Service Host
  //
  station_http_station_box=new QComboBox(this);
  station_http_station_box->setGeometry(155,335,200,20);
  station_http_station_box->setEditable(false);
  QLabel *station_http_station_label=
    new QLabel(station_http_station_box,tr("HTTP Xport:"),this);
  station_http_station_label->setGeometry(20,335,130,20);
  station_http_station_label->setFont(font);
  station_http_station_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // CAE Service Host
  //
  station_cae_station_box=new QComboBox(this);
  station_cae_station_box->setGeometry(155,357,200,20);
  station_cae_station_box->setEditable(false);
  connect(station_cae_station_box,SIGNAL(activated(const QString &)),
	  this,SLOT(caeStationActivatedData(const QString &)));
  QLabel *station_cae_station_label=
    new QLabel(station_cae_station_box,tr("Core Audio Engine:"),this);
  station_cae_station_label->setGeometry(20,357,130,20);
  station_cae_station_label->setFont(font);
  station_cae_station_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  RDLibrary Configuration Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(20,392,80,50);
  button->setFont(font);
  button->setText(tr("RD&Library"));
  connect(button,SIGNAL(clicked()),this,SLOT(editLibraryData()));

  //
  //  RDCatch Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(110,392,80,50);
  button->setFont(font);
  button->setText(tr("RDCatch"));
  connect(button,SIGNAL(clicked()),this,SLOT(editDeckData()));

  //
  //  RDAirPlay Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(200,392,80,50);
  button->setFont(font);
  button->setText(tr("RDAirPlay"));
  connect(button,SIGNAL(clicked()),this,SLOT(editAirPlayData()));

  //
  //  RDPanel Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(290,392,80,50);
  button->setFont(font);
  button->setText(tr("RDPanel"));
  connect(button,SIGNAL(clicked()),this,SLOT(editPanelData()));

  //
  //  RDLogEdit Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(20,452,80,50);
  button->setFont(font);
  button->setText(tr("RDLogEdit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editLogEditData()));

  //
  //  RDCartSlots Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(110,452,80,50);
  button->setFont(font);
  button->setText(tr("RDCart\nSlots"));
  connect(button,SIGNAL(clicked()),this,SLOT(editCartSlotsData()));

  //
  // Dropboxes Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(200,452,80,50);
  button->setFont(font);
  button->setText(tr("Dropboxes"));
  connect(button,SIGNAL(clicked()),this,SLOT(editDropboxesData()));

  //
  //  Switcher Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(290,452,80,50);
  button->setFont(font);
  button->setText(tr("Switchers\nGPIO"));
  connect(button,SIGNAL(clicked()),this,SLOT(editSwitcherData()));

  //
  //  Host Variables Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(20,512,80,50);
  button->setFont(font);
  button->setText(tr("Host\nVariables"));
  connect(button,SIGNAL(clicked()),this,SLOT(editHostvarsData()));

  //
  //  Audio Ports Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(110,512,80,50);
  button->setFont(font);
  button->setText(tr("Audio\nPorts"));
  connect(button,SIGNAL(clicked()),this,SLOT(editAudioData()));

  //
  //  TTY Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(200,512,80,50);
  button->setFont(font);
  button->setText(tr("Serial\nPorts"));
  connect(button,SIGNAL(clicked()),this,SLOT(editTtyData()));

  //
  //  View Adapters (Audio Resources) Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(290,512,80,50);
  button->setFont(font);
  button->setText(tr("Audio\nResources"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewAdaptersData()));

  //
  // JACK Settings Button
  //
  button=new QPushButton(this);
  button->setGeometry(110,572,80,50);
  button->setFont(font);
  button->setText(tr("JACK\nSettings"));
  connect(button,SIGNAL(clicked()),this,SLOT(jackSettingsData()));

  //
  // Backups Configuration Button
  //
  button=new QPushButton(this);
  button->setGeometry(200,572,80,50);
  button->setFont(font);
  button->setText(tr("Backups"));
  connect(button,SIGNAL(clicked()),this,SLOT(editBackupsData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  station_name_edit->setText(station_station->name());
  station_description_edit->setText(station_station->description());
  station_address_edit->setText(station_station->address().toString());
  station_editor_cmd_edit->setText(station_station->editorPath());
  station_timeoffset_box->setValue(station_station->timeOffset());
  unsigned cartnum=station_station->startupCart();
  if(cartnum>0) {
    station_startup_cart_edit->setText(QString().sprintf("%06u",cartnum));
  }

  RDStation *cue_station=station_station;
  if(station_station->caeStation()!="localhost") {
    cue_station=new RDStation(station_station->caeStation());
  }
  if(cue_station->scanned()) {
    station_cue_sel->setMaxCards(cue_station->cards());
    for(int i=0;i<station_cue_sel->maxCards();i++) {
      station_cue_sel->setMaxPorts(i,cue_station->cardOutputs(i));
    }
  }
  else {
    QMessageBox::information(this,tr("RDAdmin - No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    station_cue_sel->setDisabled(true);
    station_cue_sel->setDisabled(true);
  }
  if(cue_station!=station_station) {
    delete cue_station;
  }
  station_cue_sel->setCard(station_station->cueCard());
  station_cue_sel->setPort(station_station->cuePort());
  if(station_station->cueStartCart()>0) {
    station_start_cart_edit->
      setText(QString().sprintf("%06u",station_station->cueStartCart()));
  }
  if(station_station->cueStopCart()>0) {
    station_stop_cart_edit->
      setText(QString().sprintf("%06u",station_station->cueStopCart()));
  }

  sql="select LOGIN_NAME from USERS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    station_default_name_edit->insertItem(q->value(0).toString());
    if(q->value(0).toString()==station_station->defaultName()) {
      station_default_name_edit->setCurrentItem(item);
    }
    item++;
  }
  delete q;
  if((cartnum=station_station->heartbeatCart())>0) {
    station_heartbeat_box->setChecked(true);
    station_hbcart_edit->setText(QString().sprintf("%u",cartnum));
    station_hbinterval_spin->
      setValue(station_station->heartbeatInterval()/1000);
    heartbeatToggledData(true);
  }
  else {
    station_heartbeat_box->setChecked(false);
    heartbeatToggledData(false);
  }
  switch(station_station->filterMode()) {
    case RDStation::FilterSynchronous:
      station_filter_box->setChecked(true);
      break;

    case RDStation::FilterAsynchronous:
      station_filter_box->setChecked(false);
      break;
  }
  station_maint_box->setChecked(station_station->systemMaint());
  station_dragdrop_box->setChecked(station_station->enableDragdrop());
  station_panel_enforce_box->setChecked(!station_station->enforcePanelSetup());
  if(!station_station->enableDragdrop()) {
    station_panel_enforce_label->setDisabled(true);
    station_panel_enforce_box->setDisabled(true);
  }

  station_http_station_box->insertItem("localhost");
  station_cae_station_box->insertItem("localhost");
  sql=QString("select NAME from STATIONS where ")+
    "NAME!=\""+RDEscapeString(sname)+"\" order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    station_http_station_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==station_station->httpStation()) {
      station_http_station_box->
	setCurrentItem(station_http_station_box->count()-1);
    }
    station_cae_station_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==station_station->caeStation()) {
      station_cae_station_box->
	setCurrentItem(station_cae_station_box->count()-1);
    }
  }
  delete q;
  for(int i=0;i<station_http_station_box->count();i++) {
    if(station_http_station_box->text(i)==station_station->httpStation()) {
      station_http_station_box->setCurrentItem(i);
    }
    if(station_cae_station_box->text(i)==station_station->caeStation()) {
      station_cae_station_box->setCurrentItem(i);
    }
  }
}


EditStation::~EditStation()
{
  delete station_station;
  delete station_cae_station;
}


QSize EditStation::sizeHint() const
{
  return QSize(395,702);
} 


void EditStation::selectClicked()
{
  int cartnum=station_startup_cart_edit->text().toInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     rda->user()->name(),rda->user()->password())==0) {
    station_startup_cart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditStation::heartbeatToggledData(bool state)
{
   station_hbcart_label->setEnabled(state);
   station_hbcart_edit->setEnabled(state);
   station_hbcart_button->setEnabled(state);
   station_hbinterval_label->setEnabled(state);
   station_hbinterval_spin->setEnabled(state);
   station_hbinterval_unit->setEnabled(state);
}


void EditStation::heartbeatClickedData()
{
  int cartnum=station_hbcart_edit->text().toInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     rda->user()->name(),rda->user()->password())==0) {
    station_hbcart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditStation::caeStationActivatedData(const QString &station_name)
{
  if(station_cae_station!=NULL) {
    delete station_cae_station;
  }
  if(station_name=="localhost") {
    station_cae_station=new RDStation(station_station->name());
  }
  else {
    station_cae_station=new RDStation(station_name);
  }
  if(station_cae_station->scanned()) {
    station_cue_sel->setMaxCards(station_cae_station->cards());
    for(int i=0;i<station_cue_sel->maxCards();i++) {
      station_cue_sel->setMaxPorts(i,station_cae_station->cardOutputs(i));
    }
  }
  else {
    QMessageBox::information(this,tr("RDAdmin - No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    station_cue_sel->setDisabled(true);
    station_cue_sel->setDisabled(true);
  }

}


QSizePolicy EditStation::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditStation::okData()
{
  QHostAddress addr;
  unsigned cartnum=0;
  bool ok=false;
  QString sql;
  RDSqlQuery *q;

  if(!station_maint_box->isChecked()) {
    sql=QString("select NAME from STATIONS where ")+
      "(NAME!=\""+RDEscapeString(station_station->name())+"\")&&"+
      "(SYSTEM_MAINT=\"Y\")";
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      QMessageBox::warning(this,tr("System Maintenance"),
	  tr("At least one host must belong to the system maintenance pool!"));
      delete q;
      return;
    }
    delete q;
  }
  if(!addr.setAddress(station_address_edit->text())) {
    QMessageBox::warning(this,tr("Invalid Address"),
			 tr("The specified IP address is invalid!"));
    return;
  }
  if(station_heartbeat_box->isChecked()) {
    cartnum=station_hbcart_edit->text().toUInt();
    if((cartnum==0)||(cartnum>999999)) {
      QMessageBox::warning(this,tr("Invalid Cart"),
			   tr("The Heartbeat Cart number is invalid!"));
      return;
    }
    station_station->setHeartbeatCart(station_hbcart_edit->text().toUInt());
    station_station->
      setHeartbeatInterval(station_hbinterval_spin->value()*1000);
  }
  else {
    station_station->setHeartbeatCart(0);
    station_station->setHeartbeatInterval(0);
  }
  station_station->setCueCard(station_cue_sel->card());
  station_station->setCuePort(station_cue_sel->port());
  station_station->setCueStartCart(station_start_cart_edit->text().toInt());
  station_station->setCueStopCart(station_stop_cart_edit->text().toInt());
  station_station->setDescription(station_description_edit->text());
  station_station->setDefaultName(station_default_name_edit->currentText());
  station_station->setAddress(addr);
  station_station->
    setEditorPath(station_editor_cmd_edit->text());
  station_station->setTimeOffset(station_timeoffset_box->value());
  cartnum=station_startup_cart_edit->text().toUInt(&ok);
  if(ok&&(cartnum<=999999)) {
    station_station->setStartupCart(cartnum);
  }
  else {
    station_station->setStartupCart(0);
  }
  if(station_filter_box->isChecked()) {
    station_station->setFilterMode(RDStation::FilterSynchronous);
  }
  else {
    station_station->setFilterMode(RDStation::FilterAsynchronous);
  }
  station_station->setSystemMaint(station_maint_box->isChecked());
  station_station->setEnableDragdrop(station_dragdrop_box->isChecked());
  station_station->
    setEnforcePanelSetup(!station_panel_enforce_box->isChecked());
  station_station->setHttpStation(station_http_station_box->currentText());
  station_station->setCaeStation(station_cae_station_box->currentText());
  station_catch_connect->reloadHeartbeat();
  station_catch_connect->reloadOffset();

  //
  // Allow the event loop to run so the packets get delivered
  //
  QTimer *timer=new QTimer(this,"ok_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(okTimerData()));
  timer->start(1,true);
}


void EditStation::okTimerData()
{
  done(0);
}


void EditStation::cancelData()
{
  done(-1);
}


void EditStation::editLibraryData()
{
  EditRDLibrary *edit_conf=
    new EditRDLibrary(station_station,station_cae_station);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editDeckData()
{
  EditDecks *edit_conf=new EditDecks(station_station,station_cae_station,this);
  if(edit_conf->exec()==0) {
    station_catch_connect->reload();
  }
  delete edit_conf;
}


void EditStation::editAirPlayData()
{
  EditRDAirPlay *edit_conf=
    new EditRDAirPlay(station_station,station_cae_station);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editPanelData()
{
  EditRDPanel *edit_conf=
    new EditRDPanel(station_station,station_cae_station,this);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editLogEditData()
{
  EditRDLogedit *edit_conf=
    new EditRDLogedit(station_station,station_cae_station);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editCartSlotsData()
{
  EditCartSlots *edit_conf=
    new EditCartSlots(station_station,station_cae_station,this);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::viewAdaptersData()
{
  ViewAdapters *view=new ViewAdapters(station_station,this);
  view->exec();
  delete view;
}


void EditStation::editAudioData()
{
  EditAudioPorts *edit_conf=new EditAudioPorts(station_station->name(),0);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editTtyData()
{
  EditTtys *edit_conf=new EditTtys(station_station->name(),0);
  edit_conf->exec();
  delete edit_conf;
}


void EditStation::editSwitcherData()
{
  ListMatrices *list_conf=new ListMatrices(station_station->name(),this);
  list_conf->exec();
  delete list_conf;
}


void EditStation::editHostvarsData()
{
  ListHostvars *list_conf=new ListHostvars(station_station->name(),this);
  list_conf->exec();
  delete list_conf;
}

void EditStation::editBackupsData()
{
  EditBackup *edit_backup=new EditBackup(station_station,this);
  edit_backup->exec();
  delete edit_backup;
}


void EditStation::editDropboxesData()
{
  ListDropboxes *list_conf=new ListDropboxes(station_station->name(),this);
  list_conf->exec();
  station_catch_connect->reloadDropboxes();
  delete list_conf;
}


void EditStation::jackSettingsData()
{
  EditJack *d=new EditJack(station_station,this);
  d->exec();
  delete d;
}


void EditStation::startCartClickedData()
{
  int cartnum=station_start_cart_edit->text().toUInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     rda->user()->name(),rda->user()->password())==0) {
    station_start_cart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditStation::stopCartClickedData()
{
  int cartnum=station_stop_cart_edit->text().toUInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     rda->user()->name(),rda->user()->password())==0) {
    station_stop_cart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditStation::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);

  //
  // System Services
  //
  p->drawLine(10,323,sizeHint().width()-10,323);
  p->drawLine(sizeHint().width()-10,323,sizeHint().width()-10,381);
  p->drawLine(sizeHint().width()-10,381,10,381);
  p->drawLine(10,381,10,323);

  delete p;
}


QString EditStation::DisplayPart(QString string)
{
  for(int i=0;i<string.length();i++) {
    if(string.at(i)=='|') {
      return string.right(string.length()-i-1);
    }
  }
  return string;
}


QString EditStation::HostPart(QString string)
{
  for(int i=0;i<string.length();i++) {
    if(string.at(i)=='|') {
      return string.left(i);
    }
  }
  return string;
}
