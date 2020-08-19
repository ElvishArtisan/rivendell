// edit_station.cpp
//
// Edit a Rivendell Workstation Configuration
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

#include <qmessagebox.h>

#include <rdapplication.h>
#include <rdcatch_connect.h>
#include <rdcart_dialog.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_audios.h"
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
#include "list_pypads.h"
#include "view_adapters.h"

EditStation::EditStation(QString sname,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  RDSqlQuery *q;
  QString sql;
  int item=0;
  char temp[256];

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

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
  station_name_edit->setReadOnly(true);
  station_name_label=new QLabel(station_name_edit,tr("Ho&st Name:"),this);
  station_name_label->setFont(labelFont());
  station_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station Short Name
  //
  station_short_name_edit=new QLineEdit(this);
  station_short_name_edit->setMaxLength(64);
  station_short_name_label=
    new QLabel(station_short_name_edit,tr("Short Name:"),this);
  station_short_name_label->setFont(labelFont());
  station_short_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station Description
  //
  station_description_edit=new QLineEdit(this);
  station_description_edit->setMaxLength(64);
  station_description_edit->setValidator(validator);
  station_description_label=
    new QLabel(station_description_edit,tr("&Description:"),this);
  station_description_label->setFont(labelFont());
  station_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station Default Name
  //
  station_default_name_edit=new QComboBox(this);
  station_default_name_edit->setEditable(false);
  station_default_name_label=
    new QLabel(station_default_name_edit,tr("Default &User:"),this);
  station_default_name_label->setFont(labelFont());
  station_default_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station IP Address
  //
  station_address_edit=new QLineEdit(this);
  station_address_edit->setMaxLength(15);
  station_address_edit->setValidator(validator);
  station_address_label=
    new QLabel(station_address_edit,tr("&IP Address:"),this);
  station_address_label->setFont(labelFont());
  station_address_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Audio Editor Command
  //
  station_audio_editor_edit=new QLineEdit(this);
  station_audio_editor_edit->setMaxLength(191);
  station_audio_editor_label=
    new QLabel(station_audio_editor_edit,tr("Audio Editor")+":",this);
  station_audio_editor_label->setFont(labelFont());
  station_audio_editor_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Report Editor Command
  //
  station_report_editor_edit=new QLineEdit(this);
  station_report_editor_edit->setMaxLength(191);
  station_report_editor_label=
    new QLabel(station_report_editor_edit,tr("Report Editor")+":",this);
  station_report_editor_label->setFont(labelFont());
  station_report_editor_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Web Browser Command
  //
  station_web_browser_edit=new QLineEdit(this);
  station_web_browser_edit->setMaxLength(191);
  station_web_browser_label=
    new QLabel(station_web_browser_edit,tr("Web Browser")+":",this);
  station_web_browser_label->setFont(labelFont());
  station_web_browser_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // ssh(1) Identity File
  //
  station_ssh_identity_file_edit=new QLineEdit(this);
  station_ssh_identity_file_edit->setMaxLength(191);
  station_ssh_identity_file_label=
    new QLabel(station_ssh_identity_file_edit,tr("SSH Ident. File")+":",this);
  station_ssh_identity_file_label->setFont(labelFont());
  station_ssh_identity_file_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station Time Offset
  //
  station_timeoffset_box=new QSpinBox(this);
  station_timeoffset_box->setRange(-RD_MAX_TIME_OFFSET,RD_MAX_TIME_OFFSET);
  station_timeoffset_box->setSuffix(tr(" mS"));
  station_timeoffset_label=
    new QLabel(station_timeoffset_box,tr("&Time Offset:"),this);
  station_timeoffset_label->setFont(labelFont());
  station_timeoffset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Startup Cart
  //
  station_startup_cart_edit=new QLineEdit(this);
  station_startup_cart_edit->setMaxLength(15);
  station_startup_cart_edit->setValidator(macro_validator);
  station_startup_cart_label=
    new QLabel(station_startup_cart_edit,tr("&Startup Cart:"),this);
  station_startup_cart_label->setFont(labelFont());
  station_startup_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  station_startup_select_button=new QPushButton(tr("Select"),this);
  station_startup_select_button->setFont(subButtonFont());
  connect(station_startup_select_button,SIGNAL(clicked()),
	  this,SLOT(selectClicked()));

  //
  // Cue Output
  //
  station_cue_sel=new RDCardSelector(this);
  station_cue_sel_label=
    new QLabel(station_cue_sel,tr("Cue &Output:"),this);
  station_cue_sel_label->setFont(labelFont());
  station_cue_sel_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  station_start_cart_edit=new QLineEdit(this);
  station_start_cart_edit->setValidator(macro_validator);
  station_start_cart_label=
    new QLabel(station_start_cart_edit,tr("Start Cart")+":",this);
  station_start_cart_label->setFont(subLabelFont());
  station_start_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  station_start_cart_button=new QPushButton(tr("Select"),this);
  station_start_cart_button->setFont(subButtonFont());
  connect(station_start_cart_button,SIGNAL(clicked()),
	  this,SLOT(startCartClickedData()));

  station_stop_cart_edit=new QLineEdit(this);
  station_stop_cart_edit->setValidator(macro_validator);
  station_stop_cart_label=
    new QLabel(station_stop_cart_edit,tr("Stop Cart")+":",this);
  station_stop_cart_label->setFont(subLabelFont());
  station_stop_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  station_stop_cart_button=new QPushButton(tr("Select"),this);
  station_stop_cart_button->setFont(subButtonFont());
  connect(station_stop_cart_button,SIGNAL(clicked()),
	  this,SLOT(stopCartClickedData()));

  //
  // Heartbeat Checkbox
  //
  station_heartbeat_box=new QCheckBox(this);
  station_heartbeat_label=
    new QLabel(station_heartbeat_box,tr("Enable Heartbeat"),this);
  station_heartbeat_label->setFont(labelFont());
  station_heartbeat_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(station_heartbeat_box,SIGNAL(toggled(bool)),
	  this,SLOT(heartbeatToggledData(bool)));

  //
  // Filter Checkbox
  //
  station_filter_box=new QCheckBox(this);
  station_filter_label=
    new QLabel(station_filter_box,tr("Use Realtime Filtering"),this);
  station_filter_label->setFont(labelFont());
  station_filter_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Heartbeat Cart
  //
  station_hbcart_edit=new QLineEdit(this);
  station_hbcart_edit->setValidator(macro_validator);
  station_hbcart_label=new QLabel(station_hbcart_edit,tr("Cart:"),this);
  station_hbcart_label->setFont(labelFont());
  station_hbcart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  station_hbcart_button=new QPushButton(this);
  station_hbcart_button->setFont(subButtonFont());
  station_hbcart_button->setText(tr("Select"));
  connect(station_hbcart_button,SIGNAL(clicked()),
	  this,SLOT(heartbeatClickedData()));

  //
  // Heartbeat Interval
  //
  station_hbinterval_spin=new QSpinBox(this);
  station_hbinterval_spin->setRange(1,300);
  station_hbinterval_label=
    new QLabel(station_hbinterval_spin,tr("Interval:"),this);
  station_hbinterval_label->setFont(labelFont());
  station_hbinterval_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  station_hbinterval_unit=new QLabel(tr("secs"),this);
  station_hbinterval_unit->setFont(labelFont());
  station_hbinterval_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // System Maintenance Checkbox
  //
  station_maint_box=new QCheckBox(this);
  station_maint_label=
    new QLabel(station_maint_box,tr("Include in System Maintenance Pool"),this);
  station_maint_label->setFont(labelFont());
  station_maint_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Enable Drag & Drop Checkbox
  //
  station_dragdrop_box=new QCheckBox(this);
  station_dragdrop_label=
    new QLabel(station_dragdrop_box,tr("Enable Drag && Drop"),this);
  station_dragdrop_label->setFont(labelFont());
  station_dragdrop_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Enforce Panel Setup Checkbox
  //
  station_panel_enforce_box=new QCheckBox(this);
  station_panel_enforce_label=
    new QLabel(station_panel_enforce_box,
	       tr("Allow Drops on Panels not in Setup Mode"),this);
  station_panel_enforce_label->setFont(labelFont());
  station_panel_enforce_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(station_dragdrop_box,SIGNAL(toggled(bool)),
	  station_panel_enforce_label,SLOT(setEnabled(bool)));
  connect(station_dragdrop_box,SIGNAL(toggled(bool)),
	  station_panel_enforce_box,SLOT(setEnabled(bool)));

  //
  // System Services Section
  //
  station_systemservices_groupbox=new QGroupBox(tr("System Services"),this);
  station_systemservices_groupbox->setFont(labelFont());

  //
  // HTTP Service Host
  //
  station_http_station_box=new QComboBox(this);
  station_http_station_box->setEditable(false);
  station_http_station_label=
    new QLabel(station_http_station_box,tr("HTTP Xport:"),this);
  station_http_station_label->setFont(labelFont());
  station_http_station_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // CAE Service Host
  //
  station_cae_station_box=new QComboBox(this);
  station_cae_station_box->setEditable(false);
  connect(station_cae_station_box,SIGNAL(activated(const QString &)),
	  this,SLOT(caeStationActivatedData(const QString &)));
  station_cae_station_label=
    new QLabel(station_cae_station_box,tr("Core Audio Engine:"),this);
  station_cae_station_label->setFont(labelFont());
  station_cae_station_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // RDLibrary Configuration Button
  //
  station_rdlibrary_button=new QPushButton(this);
  station_rdlibrary_button->setFont(buttonFont());
  station_rdlibrary_button->setText(tr("RD&Library"));
  connect(station_rdlibrary_button,SIGNAL(clicked()),this,SLOT(editLibraryData()));

  //
  // RDCatch Configuration Button
  //
  station_rdcatch_button=new QPushButton(this);
  station_rdcatch_button->setFont(buttonFont());
  station_rdcatch_button->setText(tr("RDCatch"));
  connect(station_rdcatch_button,SIGNAL(clicked()),this,SLOT(editDeckData()));

  //
  // RDAirPlay Configuration Button
  //
  station_rdairplay_button=new QPushButton(this);
  station_rdairplay_button->setFont(buttonFont());
  station_rdairplay_button->setText(tr("RDAirPlay"));
  connect(station_rdairplay_button,SIGNAL(clicked()),
	  this,SLOT(editAirPlayData()));

  //
  // RDPanel Configuration Button
  //
  station_rdpanel_button=new QPushButton(this);
  station_rdpanel_button->setFont(buttonFont());
  station_rdpanel_button->setText(tr("RDPanel"));
  connect(station_rdpanel_button,SIGNAL(clicked()),this,SLOT(editPanelData()));

  //
  // RDLogEdit Configuration Button
  //
  station_rdlogedit_button=new QPushButton(this);
  station_rdlogedit_button->setFont(buttonFont());
  station_rdlogedit_button->setText(tr("RDLogEdit"));
  connect(station_rdlogedit_button,SIGNAL(clicked()),
	  this,SLOT(editLogEditData()));

  //
  // RDCartSlots Configuration Button
  //
  station_rdcartslots_button=new QPushButton(this);
  station_rdcartslots_button->setFont(buttonFont());
  station_rdcartslots_button->setText(tr("RDCart\nSlots"));
  connect(station_rdcartslots_button,SIGNAL(clicked()),
	  this,SLOT(editCartSlotsData()));

  //
  // Dropboxes Configuration Button
  //
  station_dropboxes_button=new QPushButton(this);
  station_dropboxes_button->setFont(buttonFont());
  station_dropboxes_button->setText(tr("Dropboxes"));
  connect(station_dropboxes_button,SIGNAL(clicked()),
	  this,SLOT(editDropboxesData()));

  //
  // Switcher Configuration Button
  //
  station_switchers_button=new QPushButton(this);
  station_switchers_button->setFont(buttonFont());
  station_switchers_button->setText(tr("Switchers\nGPIO"));
  connect(station_switchers_button,SIGNAL(clicked()),
	  this,SLOT(editSwitcherData()));

  //
  // Host Variables Configuration Button
  //
  station_hostvars_button=new QPushButton(this);
  station_hostvars_button->setFont(buttonFont());
  station_hostvars_button->setText(tr("Host\nVariables"));
  connect(station_hostvars_button,SIGNAL(clicked()),
	  this,SLOT(editHostvarsData()));

  //
  // Audio Ports Configuration Button
  //
  station_audioports_button=new QPushButton(this);
  station_audioports_button->setFont(buttonFont());
  station_audioports_button->setText(tr("ASI Audio\nPorts"));
  connect(station_audioports_button,SIGNAL(clicked()),
	  this,SLOT(editAudioData()));

  //
  // TTY Configuration Button
  //
  station_ttys_button=new QPushButton(this);
  station_ttys_button->setFont(buttonFont());
  station_ttys_button->setText(tr("Serial\nPorts"));
  connect(station_ttys_button,SIGNAL(clicked()),this,SLOT(editTtyData()));

  //
  // View Adapters (Audio Resources) Configuration Button
  //
  station_adapters_button=new QPushButton(this);
  station_adapters_button->setFont(buttonFont());
  station_adapters_button->setText(tr("Audio\nResources"));
  connect(station_adapters_button,SIGNAL(clicked()),
	  this,SLOT(viewAdaptersData()));

  //
  // JACK Settings Button
  //
  station_jack_button=new QPushButton(this);
  station_jack_button->setFont(buttonFont());
  station_jack_button->setText(tr("JACK\nSettings"));
  connect(station_jack_button,SIGNAL(clicked()),this,SLOT(jackSettingsData()));

  //
  // PyPAD Instances Button
  //
  station_pypad_button=new QPushButton(this);
  station_pypad_button->setFont(buttonFont());
  station_pypad_button->setText(tr("PyPAD\nInstances"));
  connect(station_pypad_button,SIGNAL(clicked()),
	  this,SLOT(pypadInstancesData()));

  //
  // Ok Button
  //
  station_ok_button=new QPushButton(this);
  station_ok_button->setDefault(true);
  station_ok_button->setFont(buttonFont());
  station_ok_button->setText(tr("&OK"));
  connect(station_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  station_cancel_button=new QPushButton(this);
  station_cancel_button->setFont(buttonFont());
  station_cancel_button->setText(tr("&Cancel"));
  connect(station_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  caeStationActivatedData(station_station->caeStation());
  station_name_edit->setText(station_station->name());
  station_short_name_edit->setText(station_station->shortName());
  station_description_edit->setText(station_station->description());
  station_address_edit->setText(station_station->address().toString());
  station_audio_editor_edit->setText(station_station->editorPath());
  station_report_editor_edit->setText(station_station->reportEditorPath());
  station_web_browser_edit->setText(station_station->browserPath());
  station_ssh_identity_file_edit->setText(station_station->sshIdentityFile());
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
  return QSize(415,765);
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
  station_station->setShortName(station_short_name_edit->text());
  station_station->setCueCard(station_cue_sel->card());
  station_station->setCuePort(station_cue_sel->port());
  station_station->setCueStartCart(station_start_cart_edit->text().toInt());
  station_station->setCueStopCart(station_stop_cart_edit->text().toInt());
  station_station->setDescription(station_description_edit->text());
  station_station->setDefaultName(station_default_name_edit->currentText());
  station_station->setAddress(addr);
  station_station->
    setEditorPath(station_audio_editor_edit->text());
  station_station->setReportEditorPath(station_report_editor_edit->text());
  station_station->setBrowserPath(station_web_browser_edit->text());
  station_station->setSshIdentityFile(station_ssh_identity_file_edit->text());
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


void EditStation::pypadInstancesData()
{
  ListPypads *d=new ListPypads(station_station,this);
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


void EditStation::resizeEvent(QResizeEvent *e)
{
  station_name_edit->setGeometry(115,2,size().width()-120,19);
  station_name_label->setGeometry(10,2,100,19);

  station_short_name_edit->setGeometry(115,23,size().width()-120,19);
  station_short_name_label->setGeometry(10,23,100,19);

  station_description_edit->setGeometry(115,44,size().width()-120,19);
  station_description_label->setGeometry(10,44,100,19);

  station_default_name_edit->setGeometry(115,65,160,19);
  station_default_name_label->setGeometry(10,65,100,19);

  station_address_edit->setGeometry(115,86,120,19);
  station_address_label->setGeometry(10,86,100,19);

  station_audio_editor_edit->setGeometry(115,107,size().width()-120,19);
  station_audio_editor_label->setGeometry(10,107,100,19);

  station_report_editor_edit->setGeometry(115,128,size().width()-120,19);
  station_report_editor_label->setGeometry(10,128,100,19);

  station_web_browser_edit->setGeometry(115,149,size().width()-120,19);
  station_web_browser_label->setGeometry(10,149,100,19);

  station_ssh_identity_file_edit->setGeometry(115,170,size().width()-120,19);
  station_ssh_identity_file_label->setGeometry(10,170,100,19);

  station_timeoffset_box->setGeometry(115,191,80,19);
  station_timeoffset_label->setGeometry(10,191,100,19);

  station_startup_cart_edit->setGeometry(115,212,60,19);
  station_startup_cart_label->setGeometry(10,212,100,19);
  station_startup_select_button->setGeometry(180,211,50,22);

  station_cue_sel->setGeometry(90,243,110,117);
  station_cue_sel_label->setGeometry(10,243,100,19);

  station_start_cart_edit->setGeometry(290,243,60,20);
  station_start_cart_label->setGeometry(205,243,80,20);
  station_start_cart_button->setGeometry(355,242,50,22);

  station_stop_cart_edit->setGeometry(290,264,60,20);
  station_stop_cart_label->setGeometry(205,264,80,20);
  station_stop_cart_button->setGeometry(355,263,50,22);

  station_heartbeat_box->setGeometry(10,290,15,15);
  station_heartbeat_label->setGeometry(30,285,150,20);

  station_filter_box->setGeometry(210,290,15,15);
  station_filter_label->setGeometry(230,285,150,20);

  station_hbcart_edit->setGeometry(65,310,60,19);
  station_hbcart_label->setGeometry(10,310,50,19);
  station_hbcart_button->setGeometry(140,307,60,26);

  station_hbinterval_spin->setGeometry(275,310,45,19);
  station_hbinterval_label->setGeometry(220,310,50,19);
  station_hbinterval_unit->setGeometry(325,310,100,19);

  station_maint_box->setGeometry(10,335,15,15);
  station_maint_label->setGeometry(30,333,size().width()-40,20);

  station_dragdrop_box->setGeometry(10,356,15,15);
  station_dragdrop_label->setGeometry(30,353,size().width()-40,20);

  station_panel_enforce_box->setGeometry(25,374,15,15);
  station_panel_enforce_label->setGeometry(45,374,size().width()-55,20);

  station_systemservices_groupbox->setGeometry(10,395,size().width()-20,60);

  station_http_station_box->setGeometry(145,410,size().width()-165,19);
  station_http_station_label->setGeometry(11,408,130,19);

  station_cae_station_box->setGeometry(145,431,size().width()-165,19);
  station_cae_station_label->setGeometry(11,431,130,19);

  station_rdlibrary_button->setGeometry(30,461,80,50);

  station_rdcatch_button->setGeometry(120,461,80,50);

  station_rdairplay_button->setGeometry(210,461,80,50);

  station_rdpanel_button->setGeometry(300,461,80,50);

  station_rdlogedit_button->setGeometry(30,519,80,50);

  station_rdcartslots_button->setGeometry(120,519,80,50);

  station_dropboxes_button->setGeometry(210,519,80,50);

  station_switchers_button->setGeometry(300,519,80,50);

  station_hostvars_button->setGeometry(30,577,80,50);

  station_audioports_button->setGeometry(120,577,80,50);

  station_ttys_button->setGeometry(210,577,80,50);

  station_adapters_button->setGeometry(300,577,80,50);

  station_jack_button->setGeometry(120,635,80,50);

  station_pypad_button->setGeometry(210,635,80,50);

  station_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  station_cancel_button->
    setGeometry(size().width()-90,size().height()-60,80,50);
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
