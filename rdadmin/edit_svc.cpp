// edit_svc.cpp
//
// Edit a Rivendell Service
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rdapplication.h>
#include <rddb.h>
#include <rdidvalidator.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include "autofill_carts.h"
#include "edit_svc.h"
#include "globals.h"
#include "edit_svc_perms.h"
#include "test_import.h"

EditSvc::EditSvc(QString svc,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  QString sql;
  RDSqlQuery *q;
  QString group;
  QString autospot;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  svc_svc=new RDSvc(svc,rda->station(),rda->config());

  setWindowTitle("RDAdmin - "+tr("Edit Service"));

  //
  // Text Validators
  //
  RDTextValidator *validator=new RDTextValidator(this);
  RDIdValidator *log_validator=new RDIdValidator(this);
  log_validator->addBannedChar(' ');

  //
  // General Section
  //
  QLabel *label=new QLabel("General",this);
  label->setGeometry(10,10,120,24);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignLeft);

  //
  // Service Name
  //
  svc_name_edit=new QLineEdit(this);
  svc_name_edit->setGeometry(185,31,80,19);
  svc_name_edit->setMaxLength(10);
  svc_name_edit->setReadOnly(true);
  label=new QLabel(svc_name_edit,tr("&Service Name:"),this);
  label->setGeometry(10,33,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Service Description
  //
  svc_description_edit=new QLineEdit(this);
  svc_description_edit->setGeometry(185,52,240,19);
  svc_description_edit->setMaxLength(255);
  svc_description_edit->setValidator(validator);
  label=new QLabel(svc_description_edit,tr("Service &Description:"),this);
  label->setGeometry(10,54,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Service Program Code
  //
  svc_program_code_edit=new QLineEdit(this);
  svc_program_code_edit->setGeometry(185,73,240,19);
  svc_program_code_edit->setMaxLength(255);
  svc_program_code_edit->setValidator(validator);
  label=new QLabel(svc_program_code_edit,tr("&Program Code:"),this);
  label->setGeometry(10,73,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Log Name Template
  //
  svc_name_template_edit=new QLineEdit(this);
  svc_name_template_edit->setGeometry(185,94,240,19);
  svc_name_template_edit->setMaxLength(255);
  svc_name_template_edit->setValidator(log_validator);
  label=new QLabel(svc_name_template_edit,tr("Log Name &Template:"),this);
  label->setGeometry(10,94,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Log Description Template
  //
  svc_description_template_edit=new QLineEdit(this);
  svc_description_template_edit->setGeometry(185,115,240,19);
  svc_description_template_edit->setMaxLength(255);
  label=new QLabel(svc_description_template_edit,
		   tr("Log &Description Template:"),this);
  label->setGeometry(10,115,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Voicetracking Group
  //
  svc_voice_group_box=new QComboBox(this);
  svc_voice_group_box->setGeometry(185,136,240,19);
  svc_voice_group_box->insertItem(tr("[none]"));
  label=new QLabel(svc_voice_group_box,tr("Voicetrack Group:"),this);
  label->setGeometry(10,136,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Autospot Group
  //
  svc_autospot_group_box=new QComboBox(this);
  svc_autospot_group_box->setGeometry(185,157,240,19);
  svc_autospot_group_box->insertItem(tr("[none]"));
  label=new QLabel(svc_autospot_group_box,tr("AutoSpot Group:"),this);
  label->setGeometry(10,157,170,19);
  label->setAlignment(Qt::AlignRight);

  //
  // Chain Log
  //
  svc_chain_box=new QCheckBox(this);
  svc_chain_box->setGeometry(30,180,15,15);
  label=new QLabel(svc_chain_box,tr("Insert CHAIN TO at log end"),this);
  label->setGeometry(50,180,170,19);
  label->setAlignment(Qt::AlignLeft);

  //
  // Default Auto Refresh
  //
  svc_autorefresh_box=new QCheckBox(this);
  svc_autorefresh_box->setGeometry(230,180,15,15);
  label=
    new QLabel(svc_autorefresh_box,tr("Enable AutoRefresh By Default"),this);
  label->setGeometry(250,180,200,19);
  label->setAlignment(Qt::AlignLeft);

  //
  // Autofill Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(455,31,150,50);
  button->setFont(buttonFont());
  button->setText(tr("Configure \n&Autofill Carts"));
  connect(button,SIGNAL(clicked()),this,SLOT(autofillData()));

  //
  // Default Log Deletion Date
  //
  svc_loglife_box=new QCheckBox(this);
  svc_loglife_box->setGeometry(460,95,15,15);
  label=new QLabel(svc_loglife_box,tr("Set Logs to auto-delete"),this);
  label->setGeometry(480,95,240,19);
  label->setAlignment(Qt::AlignLeft);
  svc_loglife_spin=new QSpinBox(this);
  svc_loglife_spin->setGeometry(625,93,50,19);
  svc_loglife_spin->setRange(0,365);
  connect(svc_loglife_box,SIGNAL(toggled(bool)),
	  svc_loglife_spin,SLOT(setEnabled(bool)));
  svc_loglifeorigin_label=new QLabel(svc_loglife_box,tr("days after"),this);
  svc_loglifeorigin_label->setGeometry(685,95,100,19);
  svc_loglifeorigin_label->setAlignment(Qt::AlignLeft);
  connect(svc_loglife_box,SIGNAL(toggled(bool)),
	  svc_loglifeorigin_label,SLOT(setEnabled(bool)));
  svc_loglifeorigin_box=new QComboBox(this);
  svc_loglifeorigin_box->insertItem(tr("air date"));
  svc_loglifeorigin_box->insertItem(tr("creation"));
  svc_loglifeorigin_box->setGeometry(750,93,100,19);
  connect(svc_loglife_box,SIGNAL(toggled(bool)),
	  svc_loglifeorigin_box,SLOT(setEnabled(bool)));

  //
  // Purge Expired ELR Data
  //
  svc_shelflife_box=new QCheckBox(this);
  svc_shelflife_box->setGeometry(460,117,15,15);
  label=new QLabel(svc_shelflife_box,tr("Purge ELR Data"),this);
  label->setGeometry(480,117,170,19);
  label->setAlignment(Qt::AlignLeft);
  svc_shelflife_spin=new QSpinBox(this);
  svc_shelflife_spin->setGeometry(580,115,50,19);
  svc_shelflife_spin->setRange(0,365);
  connect(svc_shelflife_box,SIGNAL(toggled(bool)),
	  svc_shelflife_spin,SLOT(setEnabled(bool)));
  label=new QLabel(svc_shelflife_box,tr("days after airing"),this);
  label->setGeometry(640,117,200,19);
  label->setAlignment(Qt::AlignLeft);

  //
  // Include Import Markers by Default
  //
  svc_import_markers_check=new QCheckBox(this);
  svc_import_markers_check->setGeometry(460,140,15,15);
  label=
    new QLabel(svc_shelflife_box,tr("Include Import Markers in Finished Logs"),
	       this);
  label->setGeometry(480,140,sizeHint().width()-490,19);
  label->setAlignment(Qt::AlignLeft);

  //
  // Enable Hosts Button
  //
  button=new QPushButton(this);
  button->setGeometry(625,31,150,50);
  button->setFont(buttonFont());
  button->setText(tr("Enable &Hosts"));
  connect(button,SIGNAL(clicked()),this,SLOT(enableHostsData()));

  //
  // Traffic Import Section
  //
  label=new QLabel(tr("Traffic Data Import"),this);
  label->setGeometry(10,210,160,24);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignLeft);

  //
  // Linux Traffic Import Path
  //
  svc_tfc_path_edit=new QLineEdit(this);
  svc_tfc_path_edit->setGeometry(185,234,240,19);
  svc_tfc_path_edit->setMaxLength(191);
  svc_tfc_path_edit->setValidator(validator);
  label=new QLabel(svc_tfc_path_edit,tr("Import Path:"),this);
  label->setGeometry(10,234,170,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_tfc_path_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Linux Traffic Preimport Command
  //
  svc_tfc_preimport_cmd_edit=new QLineEdit(this);
  svc_tfc_preimport_cmd_edit->setGeometry(185,255,240,19);
  svc_tfc_preimport_cmd_edit->setValidator(validator);
  label=
    new QLabel(svc_tfc_preimport_cmd_edit,tr("Preimport Command:"),this);
  label->setGeometry(10,255,170,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_tfc_preimport_cmd_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Note Cart
  //
  svc_tfc_label_cart_edit=new QLineEdit(this);
  svc_tfc_label_cart_edit->setGeometry(185,276,240,19);
  svc_tfc_label_cart_edit->setMaxLength(32);
  label=new QLabel(svc_tfc_label_cart_edit,tr("Note Cart String:"),this);
  label->setGeometry(10,276,170,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_tfc_label_cart_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Track String
  //
  svc_tfc_track_edit=new QLineEdit(this);
  svc_tfc_track_edit->setGeometry(185,297,240,19);
  svc_tfc_track_edit->setMaxLength(32);
  label=new QLabel(svc_tfc_track_edit,tr("Insert Voice Track String:"),this);
  label->setGeometry(10,297,170,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_tfc_track_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Traffic Import Template
  //
  svc_tfc_import_template_box=new QComboBox(this);
  svc_tfc_import_template_box->setGeometry(185,318,240,19);
  label=new QLabel(svc_tfc_import_template_box,tr("Import Template:"),this);
  label->setGeometry(10,318,170,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_tfc_import_template_box,SIGNAL(activated(int)),
	  this,SLOT(tfcTemplateActivatedData(int)));

  //
  // Traffic Parser Settings
  //
  svc_tfc_fields=new ImportFields(this);
  svc_tfc_fields->setGeometry(10,339,svc_tfc_fields->sizeHint().width(),
			      svc_tfc_fields->sizeHint().height());

  //
  //  Traffic Test Button
  //
  button=new QPushButton(this);
  button->setGeometry(360,339,60,40);
  button->setFont(buttonFont());
  button->setText(tr("Test \n&Traffic"));
  connect(button,SIGNAL(clicked()),this,SLOT(trafficData()));

  //
  //  Traffic Copy Button
  //
  svc_tfc_copy_button=new QPushButton(this);
  svc_tfc_copy_button->setGeometry(360,389,60,40);
  svc_tfc_copy_button->setFont(buttonFont());
  svc_tfc_copy_button->setText(tr("Copy To\nCustom"));
  connect(svc_tfc_copy_button,SIGNAL(clicked()),this,SLOT(trafficCopyData()));

  //
  // Music Import Section
  //
  label=new QLabel(tr("Music Data Import"),this);
  label->setGeometry(445,213,160,24);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignLeft);

  //
  // Linux Music Import Path
  //
  svc_mus_path_edit=new QLineEdit(this);
  svc_mus_path_edit->setGeometry(620,234,240,19);
  svc_mus_path_edit->setMaxLength(191);
  svc_mus_path_edit->setValidator(validator);
  label=new QLabel(svc_mus_path_edit,tr("Import Path:"),this);
  label->setGeometry(450,234,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_mus_path_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Linux Music Preimport Command
  //
  svc_mus_preimport_cmd_edit=new QLineEdit(this);
  svc_mus_preimport_cmd_edit->setGeometry(620,255,240,19);
  svc_mus_preimport_cmd_edit->setValidator(validator);
  label=
    new QLabel(svc_mus_preimport_cmd_edit,tr("Preimport Command:"),this);
  label->setGeometry(450,255,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_mus_preimport_cmd_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Label Cart
  //
  svc_mus_label_cart_edit=new QLineEdit(this);
  svc_mus_label_cart_edit->setGeometry(620,276,240,19);
  svc_mus_label_cart_edit->setMaxLength(32);
  label=new QLabel(svc_mus_label_cart_edit,tr("Note Cart String:"),this);
  label->setGeometry(450,276,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_mus_label_cart_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Insert Voice Track String
  //
  svc_mus_track_edit=new QLineEdit(this);
  svc_mus_track_edit->setGeometry(620,297,240,19);
  svc_mus_track_edit->setMaxLength(255);
  label=new QLabel(svc_mus_track_edit,tr("Insert Voice Track String:"),this);
  label->setGeometry(450,297,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_mus_track_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Insert Spot Break String
  //
  svc_mus_break_edit=new QLineEdit(this);
  svc_mus_break_edit->setGeometry(620,318,240,19);
  svc_mus_break_edit->setMaxLength(255);
  label=new QLabel(svc_mus_break_edit,tr("Insert Traffic Break String:"),this);
  label->setGeometry(450,318,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_mus_break_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));

  //
  // Inline Event Inheritance Log
  //
  svc_sub_event_inheritance_box=new QComboBox(this);
  svc_sub_event_inheritance_box->setGeometry(620,339,240,19);
  svc_sub_event_inheritance_box->
    insertItem((int)RDSvc::ParentEvent,tr("From Parent RDLogManager Event"));
  svc_sub_event_inheritance_box->
    insertItem((int)RDSvc::SchedFile,tr("From Music Scheduler File"));
  label=new QLabel(svc_sub_event_inheritance_box,
		   tr("Inline Traffic Start/Length")+":",this);
  label->setGeometry(450,339,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Music Import Template
  //
  svc_mus_import_template_box=new QComboBox(this);
  svc_mus_import_template_box->setGeometry(620,360,240,19);
  label=new QLabel(svc_mus_import_template_box,tr("Import Template:"),this);
  label->setGeometry(450,360,165,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(svc_mus_import_template_box,SIGNAL(activated(int)),
	  this,SLOT(musTemplateActivatedData(int)));

  //
  // Music Parser Settings
  //
  svc_mus_fields=new ImportFields(this);
  svc_mus_fields->setGeometry(445,381,svc_mus_fields->sizeHint().width(),
			      svc_mus_fields->sizeHint().height());

  //
  // Music Test Button
  //
  button=new QPushButton(this);
  button->setGeometry(795,381,60,40);
  button->setFont(buttonFont());
  button->setText(tr("Test \n&Music"));
  connect(button,SIGNAL(clicked()),this,SLOT(musicData()));

  //
  // Music Copy Button
  //
  svc_mus_copy_button=new QPushButton(this);
  svc_mus_copy_button->setGeometry(795,431,60,40);
  svc_mus_copy_button->setFont(buttonFont());
  svc_mus_copy_button->setText(tr("Copy To\nCustom"));
  connect(svc_mus_copy_button,SIGNAL(clicked()),this,SLOT(musicCopyData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Group Lists
  //
  group=svc_svc->trackGroup();
  autospot=svc_svc->autospotGroup();
  sql="select NAME from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    svc_voice_group_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==group) {
      svc_voice_group_box->setCurrentItem(svc_voice_group_box->count()-1);
    }
    svc_autospot_group_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==autospot) {
      svc_autospot_group_box->setCurrentItem(svc_autospot_group_box->count()-1);
    }
  }
  delete q;

  //
  // Populate Fields
  //
  svc_name_edit->setText(svc_svc->name());
  svc_description_edit->setText(svc_svc->description());
  svc_program_code_edit->setText(svc_svc->programCode());
  svc_name_template_edit->setText(svc_svc->nameTemplate());
  svc_description_template_edit->setText(svc_svc->descriptionTemplate());
  svc_sub_event_inheritance_box->
    setCurrentIndex((RDSvc::SubEventInheritance)svc_svc->subEventInheritance());
  svc_chain_box->setChecked(svc_svc->chainto());
  svc_autorefresh_box->setChecked(svc_svc->autoRefresh());
  if(svc_svc->defaultLogShelflife()>=0) {
    svc_loglife_box->setChecked(true);
    svc_loglife_spin->setValue(svc_svc->defaultLogShelflife());
    svc_loglifeorigin_box->setCurrentItem(svc_svc->logShelflifeOrigin());
    svc_loglifeorigin_label->setEnabled(true); 
 }
  else {
    svc_loglife_spin->setDisabled(true);
    svc_loglifeorigin_label->setDisabled(true);
    svc_loglifeorigin_box->setDisabled(true);
  }
  if(svc_svc->elrShelflife()>=0) {
    svc_shelflife_box->setChecked(true);
    svc_shelflife_spin->setValue(svc_svc->elrShelflife());
  }
  else {
    svc_shelflife_spin->setDisabled(true);
  }
  svc_import_markers_check->setChecked(svc_svc->includeImportMarkers());
  svc_tfc_path_edit->setText(svc_svc->importPath(RDSvc::Traffic));
  svc_tfc_preimport_cmd_edit->
    setText(svc_svc->preimportCommand(RDSvc::Traffic));
  svc_tfc_import_template_box->insertItem(tr("[custom]"));
  svc_mus_import_template_box->insertItem(tr("[custom]"));
  sql="select NAME from IMPORT_TEMPLATES order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    svc_tfc_import_template_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==svc_svc->importTemplate(RDSvc::Traffic)) {
      svc_tfc_import_template_box->
	setCurrentItem(svc_tfc_import_template_box->count()-1);
    }
    svc_mus_import_template_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==svc_svc->importTemplate(RDSvc::Music)) {
      svc_mus_import_template_box->
	setCurrentItem(svc_mus_import_template_box->count()-1);
    }
  }
  delete q;
  svc_tfc_label_cart_edit->setText(svc_svc->labelCart(RDSvc::Traffic));
  svc_tfc_track_edit->setText(svc_svc->trackString(RDSvc::Traffic));
  svc_mus_path_edit->setText(svc_svc->importPath(RDSvc::Music));
  svc_mus_preimport_cmd_edit->
    setText(svc_svc->preimportCommand(RDSvc::Music));
  svc_mus_label_cart_edit->setText(svc_svc->labelCart(RDSvc::Music));
  svc_mus_break_edit->
    setText(svc_svc->breakString());
  svc_mus_track_edit->
    setText(svc_svc->trackString(RDSvc::Music));
  tfcTemplateActivatedData(svc_tfc_import_template_box->currentItem());
  musTemplateActivatedData(svc_mus_import_template_box->currentItem());
  import_changed=false;
}


EditSvc::~EditSvc()
{
  delete svc_name_edit;
  delete svc_description_edit;
}


QSize EditSvc::sizeHint() const
{
  return QSize(870,691);
} 


QSizePolicy EditSvc::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSvc::autofillData()
{
  AutofillCarts *autofill=new AutofillCarts(svc_svc,this);
  autofill->exec();
  delete autofill;
}


void EditSvc::enableHostsData()
{
  EditSvcPerms *edit_perms=new EditSvcPerms(svc_svc,this);
  edit_perms->exec();
  delete edit_perms;
}


void EditSvc::trafficData()
{
  TestDataImport(RDSvc::Traffic);
}


void EditSvc::trafficCopyData()
{
  if (QMessageBox::question(this,tr("Copy To Custom"),
                              tr("This action will overwrite your existing [custom] Traffic Data Import settings. Continue?"),
                              QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
    return;
  }
  svc_svc->setImportTemplate(RDSvc::Traffic,"");
  svc_tfc_import_template_box->setCurrentItem(0);
  svc_tfc_fields->setEnabled(1);
  svc_tfc_copy_button->setEnabled(0);
}


void EditSvc::musicData()
{
  TestDataImport(RDSvc::Music);
}


void EditSvc::musicCopyData()
{
  if (QMessageBox::question(this,tr("Copy To Custom"),
                              tr("This action will overwrite your existing [custom] Music Data Import settings. Continue?"),
                              QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
    return;
  }
  svc_svc->setImportTemplate(RDSvc::Music,"");
  svc_mus_import_template_box->setCurrentItem(0);
  svc_mus_fields->setEnabled(1);
  svc_mus_copy_button->setEnabled(0);
}


void EditSvc::textChangedData(const QString &)
{
  import_changed=true;
}


void EditSvc::tfcTemplateActivatedData(int index)
{
  //
  // Save stored template
  //
  QString t;

  t=svc_svc->importTemplate(RDSvc::Traffic);

  //
  // Temporarily set selected template
  //
  if(svc_tfc_import_template_box->currentItem()==0) {
    svc_svc->setImportTemplate(RDSvc::Traffic,"");
  }
  else {
    svc_svc->setImportTemplate(RDSvc::Traffic,
                              svc_tfc_import_template_box->currentText());
  }

  svc_tfc_fields->setFields(svc_svc,RDSvc::Traffic);
  svc_tfc_fields->setEnabled(index==0);
  svc_tfc_copy_button->setEnabled(index!=0);
  import_changed=true;

  //
  // Restore saved template
  //
  svc_svc->setImportTemplate(RDSvc::Traffic,t);
}


void EditSvc::musTemplateActivatedData(int index)
{

  //
  // Save stored template
  //
  QString t;

  t=svc_svc->importTemplate(RDSvc::Music);

  //
  // Temporarily set selected template
  //
  if(svc_mus_import_template_box->currentItem()==0) {
    svc_svc->setImportTemplate(RDSvc::Music,"");
  }
  else {
    svc_svc->setImportTemplate(RDSvc::Music,
			       svc_mus_import_template_box->currentText());
  }

  svc_mus_fields->setFields(svc_svc,RDSvc::Music);
  svc_mus_fields->setEnabled(index==0);
  svc_mus_copy_button->setEnabled(index!=0);
  import_changed=true;

  //
  // Restore saved template
  //
  svc_svc->setImportTemplate(RDSvc::Music,t);
}


void EditSvc::okData()
{
  Save();
  done(0);
}


void EditSvc::cancelData()
{
  done(-1);
}


void EditSvc::TestDataImport(RDSvc::ImportSource src)
{
  if(import_changed||
     ((src==RDSvc::Traffic)&&(svc_tfc_fields->changed()))||
     ((src==RDSvc::Music)&&(svc_mus_fields->changed()))) {
    switch(QMessageBox::question(this,tr("Save Import Data"),
				 tr("Before testing, the import configuration\nmust be saved.  Save now?"),QMessageBox::Yes,QMessageBox::No)) {
	case QMessageBox::No:
	case QMessageBox::NoButton:
	  return;

	default:
	  break;
    }
    Save();
  }
  TestImport *testimport=new TestImport(svc_svc,src,this);
  testimport->exec();
  delete testimport;
}


void EditSvc::Save()
{
  svc_svc->setDescription(svc_description_edit->text());
  svc_svc->setProgramCode(svc_program_code_edit->text());
  svc_svc->setNameTemplate(svc_name_template_edit->text().stripWhiteSpace());
  svc_svc->setDescriptionTemplate(svc_description_template_edit->text());
  svc_svc-> setSubEventInheritance((RDSvc::SubEventInheritance)
			       svc_sub_event_inheritance_box->currentIndex());
  svc_svc->setChainto(svc_chain_box->isChecked());
  svc_svc->setAutoRefresh(svc_autorefresh_box->isChecked());
  if(svc_loglife_box->isChecked()) {
    svc_svc->setDefaultLogShelflife(svc_loglife_spin->value());
    svc_svc->setLogShelflifeOrigin((RDSvc::ShelflifeOrigin)
				   svc_loglifeorigin_box->currentItem());
  }
  else {
    svc_svc->setDefaultLogShelflife(-1);
  }
  if(svc_shelflife_box->isChecked()) {
    svc_svc->setElrShelflife(svc_shelflife_spin->value());
  }
  else {
    svc_svc->setElrShelflife(-1);
  }
  svc_svc->setIncludeImportMarkers(svc_import_markers_check->isChecked());
  svc_svc->setImportPath(RDSvc::Traffic,svc_tfc_path_edit->text());
  svc_svc->
    setPreimportCommand(RDSvc::Traffic,svc_tfc_preimport_cmd_edit->text());
  if(svc_tfc_import_template_box->currentItem()==0) {
    svc_svc->setImportTemplate(RDSvc::Traffic,"");
    svc_tfc_fields->readFields(svc_svc,RDSvc::Traffic);
  }
  else {
    svc_svc->setImportTemplate(RDSvc::Traffic,
			       svc_tfc_import_template_box->currentText());
  }
  svc_svc->setLabelCart(RDSvc::Traffic,svc_tfc_label_cart_edit->text());
  svc_svc->setTrackString(RDSvc::Traffic,svc_tfc_track_edit->text());
  svc_svc->setImportPath(RDSvc::Music,svc_mus_path_edit->text());
  svc_svc->setPreimportCommand(RDSvc::Music,svc_mus_preimport_cmd_edit->text());
  if(svc_mus_import_template_box->currentItem()==0) {
    svc_svc->setImportTemplate(RDSvc::Music,"");
    svc_mus_fields->readFields(svc_svc,RDSvc::Music);
  }
  else {
    svc_svc->setImportTemplate(RDSvc::Music,
			       svc_mus_import_template_box->currentText());
  }
  svc_svc->setBreakString(svc_mus_break_edit->text());
  svc_svc->setTrackString(RDSvc::Music,svc_mus_track_edit->text());
  svc_svc->setLabelCart(RDSvc::Music,svc_mus_label_cart_edit->text());
  import_changed=false;
  if(svc_voice_group_box->currentItem()==0) {
    svc_svc->setTrackGroup("");
  }
  else {
    svc_svc->setTrackGroup(svc_voice_group_box->currentText());
  }
  if(svc_autospot_group_box->currentItem()==0) {
    svc_svc->setAutospotGroup("");
  }
  else {
    svc_svc->setAutospotGroup(svc_autospot_group_box->currentText());
  }
}
