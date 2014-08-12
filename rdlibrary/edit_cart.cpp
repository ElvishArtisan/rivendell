// edit_cart.cpp
//
// Edit a Rivendell Cart
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_cart.cpp,v 1.74.2.7.2.2 2014/05/28 21:21:41 cvs Exp $
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

#include <qbitmap.h>
#include <unistd.h>
#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include <rddb.h>
#include <rd.h>
#include <rdconf.h>
#include <rdaudio_exists.h>
#include <rdsystem.h>
#include <rdescape_string.h>
#include <rdaudioexport.h>
#include <rdupload.h>

#include <cdripper.h>
#include <edit_cart.h>
#include <record_cut.h>
#include <edit_schedulercodes.h>
#include <edit_notes.h>

EditCart::EditCart(unsigned number,QString *path,bool new_cart,bool profile_rip,
		   QWidget *parent,const char *name,QListView *lib_cart_list)
  : QDialog(parent,name,true)
{
  bool modification_allowed;
  rdcart_cart=NULL;
  rdcart_profile_rip=profile_rip;

  rdcart_new_cart=new_cart;
  sched_codes="";
  add_codes="";
  remove_codes="";
  lib_cart_list_edit=lib_cart_list;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  if(lib_cart_list_edit==NULL) {
    setMinimumHeight(sizeHint().height());
    setMaximumHeight(sizeHint().height());
  }
  else {
    setMinimumHeight(sizeHint().height()-270);
    setMaximumHeight(sizeHint().height()-270);
  }

  if(lib_cart_list_edit==NULL) {
    rdcart_cart=new RDCart(number);
    rdcart_import_path=path;
    setCaption(QString().sprintf("%06u",rdcart_cart->number())+" - "+
	       rdcart_cart->title());
    modification_allowed=
      lib_user->modifyCarts()&&rdcart_cart->owner().isEmpty();
  }
  else {
    setCaption("Edit Carts");
    modification_allowed=true;
  }

  //
  // Create Default Audio Cut
  //
  if(new_cart&&((rdcart_cart->type()==RDCart::Audio))) {
    if(rdcart_cart->addCut(rdlibrary_conf->defaultFormat(),
			   rdlibrary_conf->defaultBitrate(),
			   rdlibrary_conf->defaultChannels())<0) {
      QMessageBox::warning(this,tr("RDLibrary - Edit Cart"),
			   tr("This cart cannot contain any additional cuts!"));
    }
  }

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont line_edit_font=QFont("Helvetica",12,QFont::Normal);
  line_edit_font.setPixelSize(12);

  if(lib_cart_list_edit==NULL) {
  //
  // Cart Number
  //
  rdcart_number_edit=new QLineEdit(this,"rdcart_number_edit");
  rdcart_number_edit->setGeometry(135,11,70,21);
  rdcart_number_edit->setFont(line_edit_font);
  rdcart_number_edit->setMaxLength(6);
  rdcart_number_edit->setReadOnly(true);
  QLabel *rdcart_number_label=new QLabel(rdcart_number_edit,tr("Number:"),this,
				       "rdcart_number_label");
  rdcart_number_label->setGeometry(10,13,120,19);
  rdcart_number_label->setFont(button_font);
  rdcart_number_label->setAlignment(AlignRight|ShowPrefix);
  }

  //
  // Cart Group
  //
  rdcart_group_box=new QComboBox(this,"rdcart_group_box");
  if(lib_cart_list_edit==NULL) {
    rdcart_group_box->setGeometry(280,11,140,21);
  }
  else {
    rdcart_group_box->setGeometry(135,38,110,19);
  }
  rdcart_group_box->setFont(line_edit_font);
  rdcart_group_edit=new QLineEdit(this,"rdcart_group_edit");
  rdcart_group_edit->setGeometry(280,11,140,21);
  rdcart_group_edit->setFont(line_edit_font);
  rdcart_group_edit->setReadOnly(true);
  QLabel *rdcart_group_label=new QLabel(rdcart_group_box,tr("Group:"),this,
				       "rdcart_group_label");
  if(lib_cart_list_edit==NULL) {
    rdcart_group_label->setGeometry(215,13,60,19);
  }
  else {
    rdcart_group_label->setGeometry(10,38,120,19);
  }
  rdcart_group_label->setFont(button_font);
  rdcart_group_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Cart Type
  //
  rdcart_type_edit=new QLineEdit(this,"rdcart_type_edit");
  rdcart_type_edit->setGeometry(500,11,80,21);
  rdcart_type_edit->setFont(line_edit_font);
  rdcart_type_edit->setMaxLength(6);
  rdcart_type_edit->setReadOnly(true);
  QLabel *rdcart_type_label=new QLabel(rdcart_type_edit,tr("Type:"),this,
				       "rdcart_type_label");
  rdcart_type_label->setGeometry(440,13,55,19);
  rdcart_type_label->setFont(button_font);
  rdcart_type_label->setAlignment(AlignRight|ShowPrefix);
  if(lib_cart_list_edit!=NULL) {
    rdcart_type_label->hide();
    rdcart_type_edit->hide();
  }

  //
  // Cart Average Length
  //
  rdcart_average_length_edit=new QLineEdit(this,"rdcart_average_length_edit");
  rdcart_average_length_edit->setGeometry(135,36,70,21);
  rdcart_average_length_edit->setFont(line_edit_font);
  rdcart_average_length_edit->setMaxLength(10);
  rdcart_average_length_edit->setAlignment(AlignRight);
  rdcart_average_length_edit->setReadOnly(true);
  QLabel *rdcart_average_length_label=new QLabel(rdcart_average_length_edit,
						tr("Average Length:"),this,
						"rdcart_average_length_label");
  rdcart_average_length_label->setGeometry(10,38,120,19);
  rdcart_average_length_label->setFont(button_font);
  rdcart_average_length_label->setAlignment(AlignRight|ShowPrefix);
  if(lib_cart_list_edit!=NULL) {
    rdcart_average_length_label->hide();
    rdcart_average_length_edit->hide();
  }

  //
  // Cart Enforce Length
  //
  rdcart_controls.enforce_length_box=new QCheckBox(this,
					       "rdcart_enforce_length_button");
  rdcart_controls.enforce_length_box->setGeometry(285,38,20,15);
  QLabel *rdcart_enforce_length_label=new QLabel(rdcart_controls.enforce_length_box,
					        tr("Enforce Length"),this,
						"rdcart_enforce_length_label");
  rdcart_enforce_length_label->setGeometry(305,38,110,19);
  rdcart_enforce_length_label->setFont(button_font);
  rdcart_enforce_length_label->setAlignment(AlignLeft|ShowPrefix);
  connect(rdcart_controls.enforce_length_box,SIGNAL(toggled(bool)),
	  this,SLOT(forcedLengthData(bool)));
  if(lib_cart_list_edit!=NULL) {
    rdcart_enforce_length_label->hide();
    rdcart_controls.enforce_length_box->hide();
  }

  //
  // Cart Forced Length
  //
  rdcart_controls.forced_length_edit=
    new RDTimeEdit(this,"rdcart_forced_length_edit");
  rdcart_controls.forced_length_edit->setGeometry(530,36,85,21);
  rdcart_controls.forced_length_edit->
    setDisplay(RDTimeEdit::Hours|RDTimeEdit::Minutes|RDTimeEdit::Seconds|
	       RDTimeEdit::Tenths);
  rdcart_controls.forced_length_edit->setFont(line_edit_font);
  rdcart_forced_length_ledit=new QLineEdit(this,"rdcart_forced_length_edit");
  rdcart_forced_length_ledit->setGeometry(535,36,80,21);
  rdcart_forced_length_ledit->setFont(line_edit_font);
  rdcart_forced_length_ledit->hide();
  rdcart_forced_length_ledit->setReadOnly(true);
  rdcart_forced_length_label=new QLabel(rdcart_controls.forced_length_edit,
					tr("Forced Length:"),this,
					"rdcart_forced_length_label");
  rdcart_forced_length_label->setGeometry(415,38,110,19);
  rdcart_forced_length_label->setFont(button_font);
  rdcart_forced_length_label->setAlignment(AlignRight|ShowPrefix);
  if(lib_cart_list_edit!=NULL) {
    rdcart_forced_length_label->hide();
    rdcart_controls.forced_length_edit->hide();
  }

  //
  // Cart Preserve Pitch
  //
  rdcart_preserve_pitch_button=new QCheckBox(this,
					       "rdcart_preserve_pitch_button");
  rdcart_preserve_pitch_button->setGeometry(430,38,20,15);
  rdcart_preserve_pitch_label=new QLabel(rdcart_preserve_pitch_button,
					 tr("Preserve Pitch"),this,
					 "rdcart_preserve_pitch_label");
  rdcart_preserve_pitch_label->setGeometry(450,38,140,19);
  rdcart_preserve_pitch_label->setFont(button_font);
  rdcart_preserve_pitch_label->setAlignment(AlignLeft|ShowPrefix);
  // ???????????????????????????????
  rdcart_preserve_pitch_button->hide();
  rdcart_preserve_pitch_label->hide();

  //
  // Cart Title
  //
  rdcart_controls.title_edit=new QLineEdit(this,"rdcart_title_edit");
  rdcart_controls.title_edit->setGeometry(135,60,480,21);
  rdcart_controls.title_edit->setFont(line_edit_font);
  rdcart_controls.title_edit->setMaxLength(255);
  QLabel *rdcart_title_label=new QLabel(rdcart_controls.title_edit,tr("&Title:"),this,
				       "rdcart_title_label");
  rdcart_title_label->setGeometry(10,62,120,19);
  rdcart_title_label->setFont(button_font);
  rdcart_title_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Cart Start Date
  //
  rdcart_start_date_edit=new QLineEdit(this,"rdcart_start_date_edit");
  rdcart_start_date_edit->setGeometry(135,84,100,21);
  rdcart_start_date_edit->setFont(line_edit_font);
  rdcart_start_date_edit->setMaxLength(255);
  QLabel *rdcart_start_date_label=new QLabel(rdcart_start_date_edit,
					     tr("&Start Date:"),this,
					     "rdcart_start_date_label");
  rdcart_start_date_label->setGeometry(10,86,120,19);
  rdcart_start_date_label->setFont(button_font);
  rdcart_start_date_label->setAlignment(AlignRight|ShowPrefix);
  rdcart_start_date_edit->hide();
  rdcart_start_date_label->hide();

  //
  // Cart End Date
  //
  rdcart_end_date_edit=new QLineEdit(this,"rdcart_end_date_edit");
  rdcart_end_date_edit->setGeometry(350,84,100,21);
  rdcart_end_date_edit->setFont(line_edit_font);
  rdcart_end_date_edit->setMaxLength(255);
  QLabel *rdcart_end_date_label=new QLabel(rdcart_end_date_edit,
					     tr("&End Date:"),this,
					     "rdcart_end_date_label");
  rdcart_end_date_label->setGeometry(260,86,85,19);
  rdcart_end_date_label->setFont(button_font);
  rdcart_end_date_label->setAlignment(AlignRight|ShowPrefix);
  rdcart_end_date_edit->hide();
  rdcart_end_date_label->hide();

  //
  // Cart Artist
  //
  rdcart_controls.artist_edit=new QLineEdit(this,"rdcart_artist_edit");
  rdcart_controls.artist_edit->setGeometry(135,84,480,21);
  rdcart_controls.artist_edit->setFont(line_edit_font);
  rdcart_controls.artist_edit->setMaxLength(255);
  QLabel *rdcart_artist_label=new QLabel(rdcart_controls.artist_edit,tr("&Artist:"),
					 this,"rdcart_artist_label");
  rdcart_artist_label->setGeometry(10,86,120,19);
  rdcart_artist_label->setFont(button_font);
  rdcart_artist_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Cart Origination Year
  //
  QIntValidator *val=new QIntValidator(this);
  val->setBottom(1);
  rdcart_controls.year_edit=new QLineEdit(this,"rdcart_year_edit");
  rdcart_controls.year_edit->setGeometry(135,110,50,21);
  rdcart_controls.year_edit->setFont(line_edit_font);
  rdcart_controls.year_edit->setValidator(val);
  rdcart_controls.year_edit->setMaxLength(255);
  QLabel *rdcart_year_label=new QLabel(rdcart_controls.year_edit,
					     tr("&Year Released:"),this,
					     "rdcart_year_label");
  rdcart_year_label->setGeometry(10,112,120,19);
  rdcart_year_label->setFont(button_font);
  rdcart_year_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Cart Usage Code
  //
  rdcart_usage_box=new QComboBox(this,"rdcart_usage_box");
  rdcart_usage_box->setGeometry(270,110,150,21);
  if(lib_cart_list_edit!=0) {
    rdcart_usage_box->insertItem("");
    }
  for(int i=0;i<(int)RDCart::UsageLast;i++) {
    rdcart_usage_box->insertItem(RDCart::usageText((RDCart::UsageCode)i));
  }
  QLabel *label=
    new QLabel(rdcart_usage_box,tr("U&sage:"),this,"rdcart_usage_label");
  label->setGeometry(195,112,70,19);
  label->setFont(button_font);
  label->setAlignment(AlignRight|ShowPrefix);
  rdcart_usage_edit=new QLineEdit(this,"rdcart_usage_edit");
  rdcart_usage_edit->setGeometry(270,110,150,21);
  rdcart_usage_edit->setReadOnly(true);

  //
  // Scheduler Codes
  //
  QPushButton *sched_codes_button=new QPushButton(this,"sched_codes_button");
  sched_codes_button->setGeometry(470,106,150,28);
  sched_codes_button->setDefault(true);
  sched_codes_button->setFont(button_font);
  sched_codes_button->setText(tr("Scheduler Codes"));
  connect(sched_codes_button,SIGNAL(clicked()),this,SLOT(schedCodesData()));
 
  //
  // Song ID
  //
  rdcart_controls.song_id_edit=new QLineEdit(this);
  rdcart_controls.song_id_edit->setGeometry(135,135,240,21);
  rdcart_controls.song_id_edit->setFont(line_edit_font);
  rdcart_controls.song_id_edit->setMaxLength(32);
  QLabel *rdcart_song_id_label=
    new QLabel(rdcart_controls.song_id_edit,tr("Song &ID:"),this);
  rdcart_song_id_label->setGeometry(10,135,120,21);
  rdcart_song_id_label->setFont(button_font);
  rdcart_song_id_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Beats per Minute
  //
  rdcart_controls.bpm_spin=new QSpinBox(this);
  rdcart_controls.bpm_spin->setGeometry(515,135,100,21);
  rdcart_controls.bpm_spin->setFont(line_edit_font);
  rdcart_controls.bpm_spin->setRange(0,300);
  rdcart_controls.bpm_spin->setSpecialValueText(tr("Unknown"));
  QLabel *rdcart_bpm_label=
    new QLabel(rdcart_controls.bpm_spin,tr("&Beats per Minute:"),this);
  rdcart_bpm_label->setGeometry(390,135,120,21);
  rdcart_bpm_label->setFont(button_font);
  rdcart_bpm_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Album
  //
  rdcart_controls.album_edit=new QLineEdit(this,"rdcart_album_edit");
  rdcart_controls.album_edit->setGeometry(135,158,480,21);
  rdcart_controls.album_edit->setFont(line_edit_font);
  rdcart_controls.album_edit->setMaxLength(64);
  QLabel *rdcart_album_label=new QLabel(rdcart_controls.album_edit,tr("Al&bum:"),this,
				       "rdcart_album_label");
  rdcart_album_label->setGeometry(10,158,120,21);
  rdcart_album_label->setFont(button_font);
  rdcart_album_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Label
  // 
  rdcart_controls.label_edit=new QLineEdit(this,"rdcart_label_edit");
  rdcart_controls.label_edit->setGeometry(135,182,480,21);
  rdcart_controls.label_edit->setFont(line_edit_font);
  rdcart_controls.label_edit->setMaxLength(64);
  QLabel *rdcart_label_label=new QLabel(rdcart_controls.label_edit,tr("Re&cord Label:"),
					this,"rdcart_label_label");
  rdcart_label_label->setGeometry(10,182,120,21);
  rdcart_label_label->setFont(button_font);
  rdcart_label_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Client
  //
  rdcart_controls.client_edit=new QLineEdit(this,"rdcart_client_edit");
  rdcart_controls.client_edit->setGeometry(135,206,480,21);
  rdcart_controls.client_edit->setFont(line_edit_font);
  rdcart_controls.client_edit->setMaxLength(64);
  QLabel *rdcart_client_label=new QLabel(rdcart_controls.label_edit,tr("C&lient:"),this,
				       "rdcart_client_label");
  rdcart_client_label->setGeometry(10,206,120,21);
  rdcart_client_label->setFont(button_font);
  rdcart_client_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Agency
  //
  rdcart_controls.agency_edit=new QLineEdit(this,"rdcart_agency_edit");
  rdcart_controls.agency_edit->setGeometry(135,230,480,21);
  rdcart_controls.agency_edit->setFont(line_edit_font);
  rdcart_controls.agency_edit->setMaxLength(64);
  QLabel *rdcart_agency_label=new QLabel(rdcart_controls.label_edit,tr("A&gency:"),this,
				       "rdcart_agency_label");
  rdcart_agency_label->setGeometry(10,230,120,21);
  rdcart_agency_label->setFont(button_font);
  rdcart_agency_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Publisher
  //
  rdcart_controls.publisher_edit=new QLineEdit(this,"rdcart_publisher_edit");
  rdcart_controls.publisher_edit->setGeometry(135,254,480,21);
  rdcart_controls.publisher_edit->setFont(line_edit_font);
  rdcart_controls.publisher_edit->setMaxLength(64);
  QLabel *rdcart_publisher_label=new QLabel(rdcart_controls.label_edit,
					    tr("&Publisher:"),this,
					    "rdcart_publisher_label");
  rdcart_publisher_label->setGeometry(10,254,120,21);
  rdcart_publisher_label->setFont(button_font);
  rdcart_publisher_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Composer
  //
  rdcart_controls.composer_edit=new QLineEdit(this,"rdcart_composer_edit");
  rdcart_controls.composer_edit->setGeometry(135,278,480,21);
  rdcart_controls.composer_edit->setFont(line_edit_font);
  rdcart_controls.composer_edit->setMaxLength(64);
  QLabel *rdcart_composer_label=new QLabel(rdcart_controls.label_edit,
					   tr("Compos&er:"),this,
					   "rdcart_composer_label");
  rdcart_composer_label->setGeometry(10,278,120,21);
  rdcart_composer_label->setFont(button_font);
  rdcart_composer_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Conductor
  //
  rdcart_controls.conductor_edit=new QLineEdit(this,"rdcart_conductor_edit");
  rdcart_controls.conductor_edit->setGeometry(135,302,480,21);
  rdcart_controls.conductor_edit->setFont(line_edit_font);
  rdcart_controls.conductor_edit->setMaxLength(255);
  QLabel *rdcart_conductor_label=
    new QLabel(rdcart_controls.label_edit,tr("Cond&uctor:"),this,
	       "rdcart_conductor_label");
  rdcart_conductor_label->setGeometry(10,302,120,21);
  rdcart_conductor_label->setFont(button_font);
  rdcart_conductor_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
 
  //
  // Cart User Defined
  //
  rdcart_controls.user_defined_edit=new QLineEdit(this);
  rdcart_controls.user_defined_edit->setGeometry(135,325,480,21);
  rdcart_controls.user_defined_edit->setFont(line_edit_font);
  rdcart_controls.user_defined_edit->setMaxLength(255);
  QLabel *rdcart_user_defined_label=
    new QLabel(rdcart_controls.label_edit,tr("&User Defined:"),this,
	       "rdcart_user_defined_label");
  rdcart_user_defined_label->setGeometry(10,325,120,21);
  rdcart_user_defined_label->setFont(button_font);
  rdcart_user_defined_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
 
  //
  // Synchronous Scheduling Policy
  //
  rdcart_syncronous_box=new QCheckBox(this,"rdcart_syncronous_box");
  rdcart_syncronous_box->setGeometry(135,351,15,15);
  connect(rdcart_syncronous_box,SIGNAL(toggled(bool)),
	  this,SLOT(asyncronousToggledData(bool)));
  rdcart_syncronous_box->hide();
  QLabel *rdcart_syncronous_label=new QLabel(rdcart_syncronous_box,
					    tr("Execute Asynchronously"),this,
					    "rdcart_syncronous_label");
  rdcart_syncronous_label->setGeometry(155,351,200,19);
  rdcart_syncronous_label->setFont(button_font);
  rdcart_syncronous_label->setAlignment(AlignLeft|ShowPrefix);
  rdcart_syncronous_label->hide();

  //
  // Use Event Length Policy
  //
  rdcart_use_event_length_box=new QCheckBox(this);
  rdcart_use_event_length_box->setGeometry(330,351,15,15);
  connect(rdcart_use_event_length_box,SIGNAL(toggled(bool)),
	  this,SLOT(asyncronousToggledData(bool)));
  rdcart_use_event_length_box->hide();
  QLabel *rdcart_use_event_length_label=
    new QLabel(rdcart_use_event_length_box,
	       tr("Use Event Length for Now && Next Updates"),this);
  rdcart_use_event_length_label->setGeometry(350,351,sizeHint().width()-350,19);
  rdcart_use_event_length_label->setFont(button_font);
  rdcart_use_event_length_label->setAlignment(AlignLeft|ShowPrefix);
  rdcart_use_event_length_label->hide();

  //
  // Notes Button
  //
  rdcart_notes_button=new QPushButton(this,"rdcart_notes_button");
  //  rdcart_notes_button->setGeometry(360,304,80,50);
  rdcart_notes_button->setGeometry(10,sizeHint().height()-60,80,50);
  rdcart_notes_button->setFont(button_font);
  rdcart_notes_button->setText(tr("&Edit\nNotes"));
  connect(rdcart_notes_button,SIGNAL(clicked()),this,SLOT(notesData()));

  //
  //  Script Button
  //
  QPushButton *script_button=new QPushButton(this,"script_button");
  script_button->setGeometry(450,304,80,50);
  script_button->setFont(button_font);
  script_button->setText(tr("&Edit\nScript"));
  connect(script_button,SIGNAL(clicked()),this,SLOT(scriptData()));
  script_button->hide();

  if(lib_cart_list_edit==NULL) {
    //
    // Cut Widget
    //
    switch(rdcart_cart->type()) {
      case RDCart::Audio:
	rdcart_audio_cart=new AudioCart(&rdcart_controls,rdcart_cart,
					rdcart_import_path,new_cart,
					rdcart_profile_rip,this);
	rdcart_audio_cart->
	  setGeometry(0,378,rdcart_audio_cart->sizeHint().width(),
		      rdcart_audio_cart->sizeHint().height());
	connect(rdcart_audio_cart,SIGNAL(cartDataChanged()),
		this,SLOT(cartDataChangedData()));
	rdcart_macro_cart=NULL;
	break;
	
      case RDCart::Macro:
	rdcart_macro_cart=new MacroCart(rdcart_cart,this,"rdcart_macro_cart");
	rdcart_macro_cart->
	  setGeometry(0,378,rdcart_macro_cart->sizeHint().width(),
		      rdcart_macro_cart->sizeHint().height());
	connect(rdcart_macro_cart,SIGNAL(lengthChanged(unsigned)),
		this,SLOT(lengthChangedData(unsigned)));
	rdcart_audio_cart=NULL;
	rdcart_controls.enforce_length_box->setDisabled(true);
	rdcart_enforce_length_label->setDisabled(true);
	rdcart_syncronous_box->show();
	rdcart_use_event_length_box->show();
	rdcart_syncronous_label->show();
	rdcart_use_event_length_label->show();
	break;
	
      default:
	break;
    }
  }

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  if(lib_cart_list_edit==NULL) 
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  else
    ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60-270,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(button_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  if(lib_cart_list_edit==NULL) 
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  else 
    cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60-270,
			     80,50);
  cancel_button->setFont(button_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  if(lib_cart_list_edit==NULL) { //single edit
    rdcart_number_edit->
      setText(QString().sprintf("%06d",rdcart_cart->number()));
    if(rdcart_group_box->count() == 0)
    {
      PopulateGroupList();
      for(int i=0;i<rdcart_group_box->count();i++) {
	if(rdcart_group_box->text(i)==rdcart_cart->groupName()) {
	  rdcart_group_box->setCurrentItem(i);
	}
      }
      rdcart_group_edit->setText(rdcart_cart->groupName());
    }
    switch(rdcart_cart->type()) {
	case RDCart::Audio:
	  rdcart_type_edit->setText(tr("AUDIO"));
	  break;
	  
	case RDCart::Macro:
	  rdcart_type_edit->setText(tr("MACRO"));
	  rdcart_controls.enforce_length_box->setDisabled(true);
	  rdcart_enforce_length_label->setDisabled(true);
	  break;
	  
	default:
	  rdcart_type_edit->setText(tr("UNKNOWN"));
	  break;
    }
    rdcart_controls.enforce_length_box->setChecked(rdcart_cart->enforceLength());
    forcedLengthData(rdcart_controls.enforce_length_box->isChecked());
    if(lib_cart_list_edit==NULL) {
      rdcart_average_length_edit->
	setText(RDGetTimeLength(rdcart_cart->averageLength()));
    }
    rdcart_controls.forced_length_edit->
      setTime(QTime().addMSecs(rdcart_cart->forcedLength()));
    rdcart_forced_length_ledit->
      setText(rdcart_controls.forced_length_edit->time().toString("hh:mm:ss"));
    if(rdcart_cart->preservePitch()) {
      rdcart_preserve_pitch_button->setChecked(true);
    }
    rdcart_preserve_pitch_button->
      setEnabled(rdcart_controls.enforce_length_box->isChecked());
    rdcart_preserve_pitch_label->
      setEnabled(rdcart_controls.enforce_length_box->isChecked());
    rdcart_controls.title_edit->setText(rdcart_cart->title());
    if(!rdcart_cart->startDateTime().isNull()) {
      rdcart_start_date_edit->
	setText(rdcart_cart->startDateTime().toString("M/d/yyyy"));
    }
    if(!rdcart_cart->endDateTime().isNull()) {
      rdcart_end_date_edit->
	setText(rdcart_cart->endDateTime().toString("M/d/yyyy"));
    }
    else {
      rdcart_end_date_edit->setText(tr("TFN"));
    }
    if(rdcart_cart->year()>0) {
      rdcart_controls.year_edit->
	setText(QString().sprintf("%d",rdcart_cart->year()));
    }
    sched_codes=rdcart_cart->schedCodes();
    rdcart_controls.artist_edit->setText(rdcart_cart->artist());
    rdcart_controls.song_id_edit->setText(rdcart_cart->songId());
    rdcart_controls.bpm_spin->setValue(rdcart_cart->beatsPerMinute());
    rdcart_controls.album_edit->setText(rdcart_cart->album());
    rdcart_controls.label_edit->setText(rdcart_cart->label());
    rdcart_controls.client_edit->setText(rdcart_cart->client());
    rdcart_controls.agency_edit->setText(rdcart_cart->agency());
    rdcart_controls.publisher_edit->setText(rdcart_cart->publisher());
    rdcart_controls.conductor_edit->setText(rdcart_cart->conductor());
    rdcart_controls.composer_edit->setText(rdcart_cart->composer());
    rdcart_controls.user_defined_edit->setText(rdcart_cart->userDefined());
    rdcart_usage_box->setCurrentItem((int)rdcart_cart->usageCode());
    rdcart_usage_edit->
      setText(RDCart::usageText((RDCart::UsageCode)rdcart_usage_box->
				currentItem()));
    rdcart_syncronous_box->setChecked(rdcart_cart->asyncronous());
    rdcart_use_event_length_box->
      setChecked(rdcart_cart->useEventLength());
  }
  else {//multi edit
    if(rdcart_group_box->count() == 0) {
      rdcart_group_box->insertItem("");
      PopulateGroupList();
      rdcart_group_box->setCurrentItem(0);
    }
    rdcart_usage_box->setCurrentItem(0);
    rdcart_notes_button->hide();
  }

  //
  // Set Control Perms
  //
  if(modification_allowed) {
    rdcart_group_edit->hide();
    rdcart_usage_edit->hide();
  }
  else {
    rdcart_group_box->hide(); 
    rdcart_usage_box->hide();
  }
  rdcart_syncronous_box->setEnabled(modification_allowed);
  rdcart_use_event_length_box->setEnabled(modification_allowed);
  rdcart_controls.title_edit->setReadOnly(!modification_allowed);
  rdcart_controls.artist_edit->setReadOnly(!modification_allowed);
  rdcart_controls.song_id_edit->setReadOnly(!modification_allowed);
  rdcart_controls.album_edit->setReadOnly(!modification_allowed);
  rdcart_controls.year_edit->setReadOnly(!modification_allowed);
  rdcart_controls.label_edit->setReadOnly(!modification_allowed);
  rdcart_controls.client_edit->setReadOnly(!modification_allowed);
  rdcart_controls.agency_edit->setReadOnly(!modification_allowed);
  rdcart_controls.publisher_edit->setReadOnly(!modification_allowed);
  rdcart_controls.conductor_edit->setReadOnly(!modification_allowed);
  rdcart_controls.composer_edit->setReadOnly(!modification_allowed);
  rdcart_controls.user_defined_edit->setReadOnly(!modification_allowed);
  rdcart_start_date_edit->setReadOnly(!modification_allowed);
  rdcart_end_date_edit->setReadOnly(!modification_allowed);
  rdcart_notes_button->setEnabled(modification_allowed);
  if(lib_cart_list_edit==NULL) {
    rdcart_average_length_edit->
      setReadOnly((!modification_allowed)||
		(!rdcart_controls.enforce_length_box->isChecked()));
    if(rdcart_cart->type()!=RDCart::Audio) {
      rdcart_controls.enforce_length_box->setDisabled(true);
    }
    else {
    rdcart_controls.enforce_length_box->setEnabled(modification_allowed);
    }
  }
  if(modification_allowed) {
    rdcart_controls.bpm_spin->setRange(0,200);
  }
  else {
    rdcart_controls.bpm_spin->
      setRange(rdcart_cart->beatsPerMinute(),rdcart_cart->beatsPerMinute());
    rdcart_controls.forced_length_edit->hide();
    rdcart_forced_length_ledit->show();
  }
}


EditCart::~EditCart()
{
  if(rdcart_cart!=NULL) {
    delete rdcart_cart;
  }
}


QSize EditCart::sizeHint() const
{
  return QSize(640,750);
} 


QSizePolicy EditCart::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditCart::notesData()
{
  EditNotes *d=new EditNotes(rdcart_cart,this);
  d->exec();
  delete d;
}


void EditCart::scriptData()
{
}


void EditCart::lengthChangedData(unsigned len)
{
  if(!rdcart_syncronous_box->isChecked()) {
    rdcart_average_length_edit->setText(RDGetTimeLength(len));
    rdcart_cart->calculateAverageLength(&rdcart_length_deviation);
  }
}


void EditCart::okData()
{
  QListViewItemIterator *it;
  RDCart *rdcart_cart_medit;
  RDSystem *system;
  QString sql;
  RDSqlQuery *q;

  if(lib_cart_list_edit==NULL) { // Single Edit
    if(rdcart_controls.title_edit->text().isEmpty()) {
      QMessageBox::warning(this,tr("Missing Title"),
			   tr("You must provide at least a Cart Title!"));
      return;
    }
    system=new RDSystem();
    if(!system->allowDuplicateCartTitles()) {
      sql=QString("select NUMBER from CART where ")+
	"(TITLE=\""+RDEscapeString(rdcart_controls.title_edit->text())+"\") &&"+
	QString().sprintf("(NUMBER=%u)",rdcart_cart->number());
      q=new RDSqlQuery(sql);
      if(q->first()) {
	QMessageBox::warning(this,tr("Duplicate Title"),
			     tr("The cart title must be unique!"));
	delete q;
	delete system;
	return;
      }
      delete q;
    }
    delete system;
    if(rdcart_controls.enforce_length_box->isChecked()) {
      if(!ValidateLengths()) {
	switch(QMessageBox::warning(this,tr("Length Mismatch"),
				    tr("One or more cut lengths exceed the timescaling\nlimits of the system!  Do you still want to save?"),QMessageBox::Yes,QMessageBox::No)) {
	  case QMessageBox::No:
	  case QMessageBox::NoButton:
	    return;
	    
	  default:
	    break;
	}
      }
    }
    rdcart_cart->setGroupName(rdcart_group_box->currentText());
    rdcart_cart->calculateAverageLength(&rdcart_length_deviation);
    rdcart_cart->setLengthDeviation(rdcart_length_deviation);
    rdcart_cart->updateLength(rdcart_controls.enforce_length_box->isChecked(),
			      QTime().msecsTo(rdcart_controls.
					      forced_length_edit->time()));
    rdcart_cart->
      setAverageLength(RDSetTimeLength(rdcart_average_length_edit->text()));
    if(rdcart_controls.enforce_length_box->isChecked()) {
      rdcart_cart->
	setForcedLength(QTime().msecsTo(rdcart_controls.forced_length_edit->time()));
      rdcart_cart->setEnforceLength(true);
    }
    else {
      rdcart_cart->
	setForcedLength(RDSetTimeLength(rdcart_average_length_edit->text()));
      rdcart_cart->setEnforceLength(false);
    }
    rdcart_cart->setPreservePitch(rdcart_preserve_pitch_button->isChecked());
    rdcart_cart->setTitle(rdcart_controls.title_edit->text());
    if(rdcart_controls.year_edit->text().toInt()==0) {
      rdcart_cart->setYear();
    }
    else {
      rdcart_cart->setYear(rdcart_controls.year_edit->text().toInt());
    }
    rdcart_cart->setSchedCodes(sched_codes);
    rdcart_cart->setArtist(rdcart_controls.artist_edit->text());
    rdcart_cart->setSongId(rdcart_controls.song_id_edit->text());
    rdcart_cart->setBeatsPerMinute(rdcart_controls.bpm_spin->value());
    rdcart_cart->setAlbum(rdcart_controls.album_edit->text());
    rdcart_cart->setLabel(rdcart_controls.label_edit->text());
    rdcart_cart->setClient(rdcart_controls.client_edit->text());
    rdcart_cart->setAgency(rdcart_controls.agency_edit->text());
    rdcart_cart->setPublisher(rdcart_controls.publisher_edit->text());
    rdcart_cart->setConductor(rdcart_controls.conductor_edit->text());
    rdcart_cart->setComposer(rdcart_controls.composer_edit->text());
    rdcart_cart->setUserDefined(rdcart_controls.user_defined_edit->text());
    rdcart_cart->
      setUsageCode((RDCart::UsageCode)rdcart_usage_box->currentItem());
    if(rdcart_cart->type()==RDCart::Macro) {
      rdcart_macro_cart->save();
      rdcart_cart->setAsyncronous(rdcart_syncronous_box->isChecked());
      rdcart_cart->setUseEventLength(rdcart_use_event_length_box->isChecked());
    }
  }
  else {  // Multi Edit
    it=new QListViewItemIterator(lib_cart_list_edit);
    while(it->current()) {
      if(it->current()->isSelected()) {  
        RDListViewItem *item=(RDListViewItem *)it->current();
        if(item->text(21).isEmpty()) {
 
          rdcart_cart_medit=new RDCart(item->text(1).toUInt());
	  
          if(!rdcart_group_box->currentText().stripWhiteSpace().isEmpty()) {
	    rdcart_cart_medit->setGroupName(rdcart_group_box->currentText());
	  }
          if(!rdcart_controls.title_edit->text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setTitle(rdcart_controls.title_edit->text());
	  }
	  rdcart_cart_medit->setYear(rdcart_controls.year_edit->text().toInt());
          if(!rdcart_controls.artist_edit->text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setArtist(rdcart_controls.artist_edit->text());
	  }
          if(!rdcart_controls.album_edit->text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setAlbum(rdcart_controls.album_edit->text());
	  }
          if(!rdcart_controls.label_edit->text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setLabel(rdcart_controls.label_edit->text());
	  }
          if(!rdcart_controls.client_edit->text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setClient(rdcart_controls.client_edit->text());
	  }
          if(!rdcart_controls.agency_edit->text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setAgency(rdcart_controls.agency_edit->text());
	  }
          if(!rdcart_controls.song_id_edit->
	     text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->
	      setSongId(rdcart_controls.song_id_edit->text());
	  }
          if(!rdcart_controls.publisher_edit->text().stripWhiteSpace().
	     isEmpty()) {
            rdcart_cart_medit->
	      setPublisher(rdcart_controls.publisher_edit->text());
	  }
          if(!rdcart_controls.composer_edit->
	     text().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->
	      setComposer(rdcart_controls.composer_edit->text());
	  }
          if(!rdcart_controls.conductor_edit->text().
	     stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->
	      setConductor(rdcart_controls.conductor_edit->text());
	  }
          if(!rdcart_controls.user_defined_edit->text().
	     stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->
	      setUserDefined(rdcart_controls.user_defined_edit->text());
	  }
          if(!rdcart_usage_box->currentText().stripWhiteSpace().isEmpty()) {
            rdcart_cart_medit->setUsageCode((RDCart::UsageCode)
				    (rdcart_usage_box->currentItem()-1));
	  }
          rdcart_cart_medit->updateSchedCodes(add_codes,remove_codes);
	  delete rdcart_cart_medit; 
        }
      }
      ++(*it);
    }
    delete it;
  }
  
  done(0);
}


void EditCart::cancelData()
{
  unsigned len;
  if((lib_cart_list_edit==NULL)&&(rdcart_cart->type()==RDCart::Audio)) {
    len=rdcart_cart->calculateAverageLength(&rdcart_length_deviation);
    rdcart_cart->setLengthDeviation(rdcart_length_deviation);
    if(!rdcart_controls.enforce_length_box->isChecked()) {
      rdcart_cart->setForcedLength(len);
    }
    rdcart_cart->updateLength(rdcart_controls.enforce_length_box->isChecked(),
			      QTime().msecsTo(rdcart_controls.
					      forced_length_edit->time()));
  }
  done(-1);
}


void EditCart::forcedLengthData(bool state)
{
  rdcart_forced_length_label->setEnabled(state);
  rdcart_controls.forced_length_edit->setEnabled(state);
  if(state) {
    rdcart_controls.forced_length_edit->
      setTime(QTime().
	      addMSecs(RDSetTimeLength(rdcart_average_length_edit->text())));
  }
}


void EditCart::asyncronousToggledData(bool state)
{
  if(state) {
    rdcart_average_length_edit->setText("00.0");
  }
  else {
    rdcart_average_length_edit->
      setText(RDGetTimeLength(rdcart_macro_cart->length()));
  }
}


void EditCart::cartDataChangedData()
{
  if(!rdcart_controls.enforce_length_box->isChecked()) {
    rdcart_average_length_edit->
      setText(RDGetTimeLength(rdcart_cart->calculateAverageLength()));
  }
}


void EditCart::closeEvent(QCloseEvent *e)
{
  cancelData();
}


bool EditCart::ValidateLengths()
{
  return rdcart_cart->validateLengths(QTime().
		     msecsTo(rdcart_controls.forced_length_edit->time()));
  /*
  int maxlen=(int)(RD_TIMESCALE_MAX*
		   (double)QTime().msecsTo(rdcart_controls.forced_length_edit->
					   time()));
  int minlen=(int)(RD_TIMESCALE_MIN*
		   (double)QTime().msecsTo(rdcart_controls.forced_length_edit->
					   time()));
  QString sql=QString().sprintf("select LENGTH from CUTS where CART_NUMBER=%u",
				rdcart_cart->number());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    if((q->value(0).toInt()>maxlen)||(q->value(0).toInt()<minlen)) {
      delete q;
      return false;
    }
  }
  delete q;

  return true;
  */
}


void EditCart::schedCodesData()
{
  if(lib_cart_list_edit==NULL) {
    EditSchedulerCodes *dialog=new EditSchedulerCodes(&sched_codes,NULL,this,"dialog");
    dialog->exec();
    delete dialog;
    }
  else  {
    EditSchedulerCodes *dialog=new EditSchedulerCodes(&add_codes,&remove_codes,this,"dialog");
    dialog->exec();
    delete dialog;
    }
}


void EditCart::PopulateGroupList()
{
  QString sql=QString("select GROUP_NAME from USER_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(lib_user->name())+"\" "+
    "order by GROUP_NAME";

  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    rdcart_group_box->insertItem(q->value(0).toString());
  }
  delete q;
}
