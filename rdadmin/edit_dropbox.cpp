// edit_dropbox.cpp
//
// Edit a Rivendell Dropbox Configuration
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

#include <QFileDialog>
#include <QMessageBox>

#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "globals.h"
#include "edit_dropbox.h"

EditDropbox::EditDropbox(int id,bool duplicate,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  box_dropbox=new RDDropbox(id);

  setWindowTitle("RDAdmin - "+tr("Dropbox Configuration")+" ["+
		 tr("ID")+QString::asprintf(": %d]",id));

  //
  // Dialogs
  //
  box_schedcodes_dialog=new RDSchedCodesDialog(this);

  //
  // Group Name
  //
  box_group_name_box=new QComboBox(this);
  box_group_name_box->setGeometry(140,10,140,20);
  box_group_name_model=new RDGroupListModel(false,false,true,this);
  box_group_name_model->setFont(defaultFont());
  box_group_name_model->setPalette(palette());
  box_group_name_box->setModel(box_group_name_model);
  QLabel *label=new QLabel(tr("Default Group:"),this);
  label->setGeometry(10,10,125,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Path
  //
  box_path_edit=new QLineEdit(this);
  box_path_edit->setGeometry(140,32,sizeHint().width()-210,19);
  box_path_edit->setMaxLength(255);
  label=new QLabel(tr("Path Spec:"),this);
  label->setGeometry(10,32,125,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QPushButton *button=
    new QPushButton(tr("Select"),this);
  button->setGeometry(sizeHint().width()-60,30,50,23);
  button->setFont(subButtonFont());
  connect(button,SIGNAL(clicked()),this,SLOT(selectPathData()));

  //
  // To Cart
  //
  box_to_cart_edit=new QLineEdit(this);
  box_to_cart_edit->setGeometry(140,54,60,19);
  box_to_cart_edit->setValidator(new QIntValidator(1,999999,this));
  box_to_cart_edit->setMaxLength(6);
  label=new QLabel(tr("To Cart:"),this);
  label->setGeometry(10,54,125,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  box_select_cart_button=
    new QPushButton(tr("Select"),this);
  box_select_cart_button->setGeometry(210,52,50,23);
  box_select_cart_button->setFont(subButtonFont());
  connect(box_select_cart_button,SIGNAL(clicked()),
	  this,SLOT(selectCartData()));

  //
  // Delete Cuts
  //
  box_delete_cuts_box=new QCheckBox(this);
  box_delete_cuts_box->setGeometry(280,56,15,15);
  box_delete_cuts_label=new QLabel(tr("Delete cuts before importing"),this);
  box_delete_cuts_label->setGeometry(300,54,sizeHint().width()-150,20);
  box_delete_cuts_label->setFont(labelFont());
  box_delete_cuts_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Metadata Format
  //
  box_metadata_pattern_edit=new QLineEdit(this);
  box_metadata_pattern_edit->setGeometry(140,76,sizeHint().width()-170,19);
  box_metadata_pattern_edit->setMaxLength(64);
  label=new QLabel(tr("Metadata Pattern:"),this);
  label->setGeometry(10,76,125,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // User Defined Field
  //
  box_user_defined_edit=new QLineEdit(this);
  box_user_defined_edit->setGeometry(140,98,sizeHint().width()-170,19);
  box_user_defined_edit->setMaxLength(255);
  label=new QLabel(tr("User Defined:"),this);
  label->setGeometry(10,98,125,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logging
  //
  box_log_to_syslog_check=new QCheckBox(this);
  box_log_to_syslog_check->setGeometry(50,125,15,15);
  label=new QLabel(tr("Log events in Syslog"),this);
  label->setGeometry(70,122,250,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  box_log_path_edit=new QLineEdit(this);
  box_log_path_edit->setGeometry(140,141,sizeHint().width()-210,19);
  box_log_path_edit->setMaxLength(191);
  connect(box_log_to_syslog_check,SIGNAL(toggled(bool)),
	  box_log_path_edit,SLOT(setDisabled(bool)));
  box_log_path_label=new QLabel(tr("Log File:"),this);
  box_log_path_label->setGeometry(10,142,125,19);
  box_log_path_label->setFont(labelFont());
  box_log_path_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(box_log_to_syslog_check,SIGNAL(toggled(bool)),
	  box_log_path_label,SLOT(setDisabled(bool)));
  box_log_path_button=new QPushButton(tr("Select"),this);
  box_log_path_button->setGeometry(sizeHint().width()-60,138,50,23);
  box_log_path_button->setFont(subButtonFont());
  connect(box_log_path_button,SIGNAL(clicked()),
	  this,SLOT(selectLogPathData()));
  connect(box_log_to_syslog_check,SIGNAL(toggled(bool)),
	  box_log_path_button,SLOT(setDisabled(bool)));

  //
  // Scheduler Codes
  //
  box_schedcodes_button=new QPushButton(tr("Scheduler Codes"),this);
  box_schedcodes_button->setGeometry(110,165,200,25);
  box_schedcodes_button->setFont(buttonFont());
  connect(box_schedcodes_button,SIGNAL(clicked()),this,SLOT(schedcodesData()));

  //
  // Delete Source
  //
  box_delete_source_box=new QCheckBox(this);
  box_delete_source_box->setGeometry(90,199,15,15);
  label=new QLabel(tr("Delete source files after import"),this);
  label->setGeometry(110,197,sizeHint().width()-120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Update Metadata
  //
  box_update_metadata_box = new QCheckBox(this);
  box_update_metadata_box->setGeometry(90, 221, 15, 15);
  label = new QLabel(tr("Update library metadata"), this);
  label->setGeometry(110, 219, sizeHint().width() - 120, 20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

  //
  // Send E-mail Reports
  //
  box_send_email_box=new QCheckBox(this);
  box_send_email_box->setGeometry(90,241,15,15);
  label=new QLabel(tr("Send e-mail reports"),this);
  label->setGeometry(110,239,sizeHint().width()-120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Force To Mono
  //
  box_force_to_mono_box=new QCheckBox(this);
  box_force_to_mono_box->setGeometry(90,263,15,15);
  label=new QLabel(tr("Force to Monaural"),this);
  label->setGeometry(110,261,sizeHint().width()-120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Normalization
  //
  box_normalization_box=new QCheckBox(this);
  box_normalization_box->setGeometry(90,284,15,15);
  label=new QLabel(tr("Normalize Levels"),this);
  label->setGeometry(110,282,100,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  box_normalization_level_spin=new QSpinBox(this);
  box_normalization_level_spin->setGeometry(275,283,50,20);
  box_normalization_level_spin->setRange(-100,-1);
  box_normalization_level_label=new QLabel(tr("Level:"),this);
  box_normalization_level_label->setGeometry(210,283,60,20);
  box_normalization_level_label->setFont(labelFont());
  box_normalization_level_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  box_normalization_level_unit=new QLabel(tr("dBFS"),this);
  box_normalization_level_unit->setFont(labelFont());
  box_normalization_level_unit->setGeometry(330,283,60,20);
  box_normalization_level_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(box_normalization_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizationToggledData(bool)));

  //
  // Autotrim
  //
  box_autotrim_box=new QCheckBox(this);
  box_autotrim_box->setGeometry(90,309,15,15);
  label=new QLabel(tr("Autotrim Cuts"),this);
  label->setGeometry(110,307,100,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  box_autotrim_level_spin=new QSpinBox(this);
  box_autotrim_level_spin->setGeometry(275,307,50,20);
  box_autotrim_level_spin->setRange(-100,-1);
  box_autotrim_level_label=new QLabel(tr("Level:"),this);
  box_autotrim_level_label->setGeometry(210,307,60,20);
  box_autotrim_level_label->setFont(labelFont());
  box_autotrim_level_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  box_autotrim_level_unit=new QLabel(tr("dBFS"),this);
  box_autotrim_level_unit->setFont(labelFont());
  box_autotrim_level_unit->setGeometry(330,307,60,20);
  box_autotrim_level_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(box_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimToggledData(bool)));

  //
  // Segue
  //
  box_segue_box=new QCheckBox(this);
  box_segue_box->setGeometry(90,335,15,15);
  label=new QLabel(tr("Insert Segue Markers"),this);
  label->setGeometry(110,333,sizeHint().width()-40,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  box_segue_level_spin=new QSpinBox(this);
  box_segue_level_spin->setGeometry(285,359,50,20);
  box_segue_level_spin->setRange(-100,0);
  box_segue_level_label=new QLabel(tr("Segue Level:"),this);
  box_segue_level_label->setGeometry(120,360,160,20);
  box_segue_level_label->setFont(labelFont());
  box_segue_level_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  box_segue_level_unit=new QLabel("dBFS",this);
  box_segue_level_unit->setFont(labelFont());
  box_segue_level_unit->setGeometry(360,360,60,20);
  box_segue_level_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  box_segue_length_spin=new QSpinBox(this);
  box_segue_length_spin->setGeometry(285,384,70,20);
  box_segue_length_spin->setRange(0,180000);
  box_segue_length_label=new QLabel(tr("Segue Length:"),this);
  box_segue_length_label->setGeometry(120,384,160,20);
  box_segue_length_label->setFont(labelFont());
  box_segue_length_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  box_segue_length_unit=new QLabel("msec",this);
  box_segue_length_unit->setFont(labelFont());
  box_segue_length_unit->setGeometry(375,385,60,20);
  box_segue_length_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(box_segue_box,SIGNAL(toggled(bool)),
	  this,SLOT(segueToggledData(bool)));

  //
  // Use CartChunk ID
  //
  box_use_cartchunk_id_box=new QCheckBox(this);
  box_use_cartchunk_id_box->setGeometry(90,418,15,15);
  label=new QLabel(tr("Get cart number from CartChunk CutID"),this);
  label->setGeometry(110,416,sizeHint().width()-40,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Title from CartChunk ID
  //
  box_title_from_cartchunk_id_box=new QCheckBox(this);
  box_title_from_cartchunk_id_box->setGeometry(90,438,15,15);
  label=new QLabel(tr("Get cart title from CartChunk CutID"),this);
  label->setGeometry(110,436,sizeHint().width()-40,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Fix Broken Formats
  //
  box_fix_broken_formats_box=new QCheckBox(this);
  box_fix_broken_formats_box->setGeometry(90,462,15,15);
  label=new QLabel(tr("Attempt to work around malformatted input files"),this);
  label->setGeometry(110,460,sizeHint().width()-40,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Start Date Offset
  //
  box_startoffset_spin=new QSpinBox(this);
  box_startoffset_spin->setGeometry(215,486,50,20);
  box_startoffset_spin->setRange(-7,7);
  label=new QLabel(tr("Offset start date by"),this);
  label->setGeometry(10,486,200,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label=new QLabel(tr("days"),this);
  label->setGeometry(275,488,100,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // End Date Offset
  //
  box_endoffset_spin=new QSpinBox(this);
  box_endoffset_spin->setGeometry(215,510,50,20);
  box_endoffset_spin->setRange(-7,7);
  label=new QLabel(tr("Offset end date by"),this);
  label->setGeometry(10,510,200,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label=new QLabel(tr("days"),this);
  label->setGeometry(275,510,100,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Create Dates
  //
  box_create_dates_box=new QCheckBox(this);
  box_create_dates_box->setGeometry(90,534,15,15);
  label=new QLabel(tr("Create Dates when no Dates Exist"),this);
  label->setGeometry(110,532,sizeHint().width()-40,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  box_create_startdate_offset_spin=new QSpinBox(this);
  box_create_startdate_offset_spin->setGeometry(285,558,50,20);
  box_create_startdate_offset_spin->setRange(-180,180);
  box_create_startdate_label= 
    new QLabel(tr("Create start date offset:"),this);
  box_create_startdate_label->setGeometry(120,558,160,20);
  box_create_startdate_label->setFont(labelFont());
  box_create_startdate_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  box_create_startdate_unit=new QLabel(tr("days"),this);
  box_create_startdate_unit->setFont(labelFont());
  box_create_startdate_unit->setGeometry(345,558,60,20);
  box_create_startdate_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  box_create_enddate_offset_spin=new QSpinBox(this);
  box_create_enddate_offset_spin->setGeometry(285,582,50,20);
  box_create_enddate_offset_spin->setRange(-180,180);
  box_create_enddate_label=new QLabel(tr("Create end date offset:"),this);
  box_create_enddate_label->setGeometry(120,580,160,20);
  box_create_enddate_label->setFont(labelFont());
  box_create_enddate_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  box_create_enddate_unit=
    new QLabel(("days"),this);
  box_create_enddate_unit->setFont(labelFont());
  box_create_enddate_unit->setGeometry(345,578,60,20);
  box_create_enddate_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(box_create_dates_box,SIGNAL(toggled(bool)),
          this,SLOT(createDatesToggledData(bool)));


  //
  //  Reset Button
  //
  QPushButton *reset_button=new QPushButton(this);
  reset_button->setGeometry(10,sizeHint().height()-60,80,50);
  reset_button->setDefault(true);
  reset_button->setFont(buttonFont());
  reset_button->setText(tr("Reset"));
  connect(reset_button,SIGNAL(clicked()),this,SLOT(resetData()));

  //
  //  Ok Button
  //
  ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  box_group_name_box->setCurrentText(box_dropbox->groupName());
  box_path_edit->setText(box_dropbox->path());
  if(box_dropbox->toCart()>0) {
    box_to_cart_edit->setText(QString::asprintf("%06u",box_dropbox->toCart()));
  }
  box_delete_cuts_box->setChecked(box_dropbox->deleteCuts());
  box_metadata_pattern_edit->setText(box_dropbox->metadataPattern());
  box_user_defined_edit->setText(box_dropbox->userDefined());
  box_delete_source_box->setChecked(box_dropbox->deleteSource());
  box_send_email_box->setChecked(box_dropbox->sendEmail());
  box_update_metadata_box->setChecked(box_dropbox->updateMetadata());
  box_force_to_mono_box->setChecked(box_dropbox->forceToMono());
  box_normalization_box->setChecked(box_dropbox->normalizationLevel()<0);
  box_normalization_level_spin->
    setValue(box_dropbox->normalizationLevel()/100);
  box_autotrim_box->setChecked(box_dropbox->autotrimLevel()<0);
  box_autotrim_level_spin->setValue(box_dropbox->autotrimLevel()/100);
  box_use_cartchunk_id_box->setChecked(box_dropbox->useCartchunkId());
  box_title_from_cartchunk_id_box->setChecked(box_dropbox->titleFromCartchunkId());
  box_log_path_edit->setText(box_dropbox->logPath());
  box_fix_broken_formats_box->setChecked(box_dropbox->fixBrokenFormats());

  box_log_to_syslog_check->setChecked(box_dropbox->logToSyslog());
  box_log_path_label->setDisabled(box_dropbox->logToSyslog());
  box_log_path_edit->setDisabled(box_dropbox->logToSyslog());
  box_log_path_button->setDisabled(box_dropbox->logToSyslog());

  box_startoffset_spin->setValue(box_dropbox->startdateOffset());
  box_endoffset_spin->setValue(box_dropbox->enddateOffset());
  box_create_dates_box->setChecked(box_dropbox->createDates());
  box_create_startdate_offset_spin->setValue(box_dropbox->createStartdateOffset());
  box_create_enddate_offset_spin->setValue(box_dropbox->createEnddateOffset());
  normalizationToggledData(box_normalization_box->isChecked());
  autotrimToggledData(box_autotrim_box->isChecked());
  segueToggledData(box_segue_box->isChecked());
  createDatesToggledData(box_create_dates_box->isChecked());
  segueToggledData(box_segue_box->isChecked());
  box_segue_box->setChecked(box_dropbox->segueLevel()<1);
  box_segue_level_spin->setValue(box_dropbox->segueLevel()/100);
  box_segue_length_spin->setValue(box_dropbox->segueLength());

  sql=QString("select `SCHED_CODE` from `DROPBOX_SCHED_CODES` ")+
    QString::asprintf("where `DROPBOX_ID`=%d",box_dropbox->id());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    box_schedcodes.push_back(q->value(0).toString());
  }
  delete q;

  if(duplicate) {
    box_path=box_dropbox->path();
    connect(box_path_edit,SIGNAL(textChanged(QString)),this,SLOT(pathChangedData(QString)));
    ok_button->setEnabled(false);
    box_path_edit->setFocus();
  }
}


QSize EditDropbox::sizeHint() const
{
  return QSize(490,666);
} 


QSizePolicy EditDropbox::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditDropbox::selectPathData()
{
  QString path=box_path_edit->text();
  path=QFileDialog::getExistingDirectory(this,"RDAdmin - "+
					 tr("Select Directory"),path,
					 QFileDialog::ShowDirsOnly);
  if(!path.isEmpty()) {
    box_path_edit->setText(path);
  }
}


void EditDropbox::pathChangedData(QString text)
{
  if(box_path!=text) {
    ok_button->setEnabled(true);
  }
  else {
    ok_button->setEnabled(false);
  }
}


void EditDropbox::selectCartData()
{
  int cartnum=box_to_cart_edit->text().toInt();
  admin_cart_dialog->exec(&cartnum,RDCart::Audio,QString(),NULL);
  if(cartnum>0) {
    box_to_cart_edit->setText(QString::asprintf("%06d",cartnum));
  }
}


void EditDropbox::selectLogPathData()
{
  QString path=box_log_path_edit->text();
  path=QFileDialog::getSaveFileName(this,"RDAdmin - "+tr("Select Log File"),
				    path,"Text files (*.txt);;All files (*)");
  if(!path.isEmpty()) {
    box_log_path_edit->setText(path);
  }
}


void EditDropbox::schedcodesData()
{
  box_schedcodes_dialog->exec(&box_schedcodes,NULL);
}


void EditDropbox::normalizationToggledData(bool state)
{
  box_normalization_level_spin->setEnabled(state);
  box_normalization_level_label->setEnabled(state);
  box_normalization_level_unit->setEnabled(state);
}


void EditDropbox::autotrimToggledData(bool state)
{
  box_autotrim_level_spin->setEnabled(state);
  box_autotrim_level_label->setEnabled(state);
  box_autotrim_level_unit->setEnabled(state);
}

void EditDropbox::segueToggledData(bool state)
{
  box_segue_level_spin->setEnabled(state);
  box_segue_level_label->setEnabled(state);
  box_segue_level_unit->setEnabled(state);
  box_segue_length_spin->setEnabled(state);
  box_segue_length_label->setEnabled(state);
  box_segue_length_unit->setEnabled(state);
  if (!state) {
    box_segue_level_spin->setValue(1);
    box_segue_length_spin->setValue(0);
  }
}

void EditDropbox::createDatesToggledData(bool state)
{
  box_create_startdate_offset_spin->setEnabled(state);
  box_create_startdate_label->setEnabled(state);
  box_create_startdate_unit->setEnabled(state);
  box_create_enddate_offset_spin->setEnabled(state);
  box_create_enddate_label->setEnabled(state);
  box_create_enddate_unit->setEnabled(state);
  if (!state) {
    box_create_startdate_offset_spin->setValue(0);
    box_create_enddate_offset_spin->setValue(0);
  }
}


void EditDropbox::resetData()
{
  if(QMessageBox::question(this,"RDAdmin - "+tr("Reset Dropbox"),
			   tr("Resetting the dropbox will clear the list of already imported\nfiles, causing any whose files remain to be imported again.")+"\n\n"+tr("Reset the dropbox?"),
			   QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  QString sql=QString("delete from `DROPBOX_PATHS` where ")+
    QString::asprintf("`DROPBOX_ID`=%d",box_dropbox->id());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  QMessageBox::information(this,"RDAdmin - "+tr("Reset Dropbox"),
			   tr("The dropbox has been reset."));
}


void EditDropbox::okData()
{
  QString sql;

  //
  //  Validate End Date Offsets
  //
  if(box_create_startdate_offset_spin->value()>
     box_create_enddate_offset_spin->value()) {
    QMessageBox::warning(this,tr("Invalid Offsets"),
    tr("The Create EndDate Offset is less than the Create Start Date Offset!"));
    return;
  }

  box_dropbox->setGroupName(box_group_name_box->currentText());
  box_dropbox->setPath(box_path_edit->text());
  if(box_to_cart_edit->text().isEmpty()) {
    box_dropbox->setToCart(0);
  }
  else {
    box_dropbox->setToCart(box_to_cart_edit->text().toUInt());
  }
  box_dropbox->setDeleteCuts(box_delete_cuts_box->isChecked());
  box_dropbox->setSendEmail(box_send_email_box->isChecked());
  box_dropbox->setMetadataPattern(box_metadata_pattern_edit->text());
  box_dropbox->setUserDefined(box_user_defined_edit->text());
  box_dropbox->setDeleteSource(box_delete_source_box->isChecked());
  box_dropbox->setUpdateMetadata(box_update_metadata_box->isChecked());
  box_dropbox->setForceToMono(box_force_to_mono_box->isChecked());
  if(box_normalization_box->isChecked()) {
    box_dropbox->
      setNormalizationLevel(box_normalization_level_spin->value()*100);
  }
  else {
    box_dropbox->setNormalizationLevel(0);
  }
  if(box_autotrim_box->isChecked()) {
    box_dropbox->
      setAutotrimLevel(box_autotrim_level_spin->value()*100);
  }

  else {
    box_dropbox->setAutotrimLevel(0);
  }
  box_dropbox->setUseCartchunkId(box_use_cartchunk_id_box->isChecked());
  box_dropbox->setTitleFromCartchunkId(box_title_from_cartchunk_id_box->isChecked());
  box_dropbox->setLogToSyslog(box_log_to_syslog_check->isChecked());
  box_dropbox->setLogPath(box_log_path_edit->text());
  box_dropbox->setFixBrokenFormats(box_fix_broken_formats_box->isChecked());
  box_dropbox->setStartdateOffset(box_startoffset_spin->value());
  box_dropbox->setEnddateOffset(box_endoffset_spin->value());
  box_dropbox->setCreateDates(box_create_dates_box->isChecked());
  box_dropbox->setCreateStartdateOffset(box_create_startdate_offset_spin->value());
  box_dropbox->setCreateEnddateOffset(box_create_enddate_offset_spin->value());

  if(box_segue_box->isChecked()){
    box_dropbox->setSegueLevel(box_segue_level_spin->value()*100);
    box_dropbox->setSegueLength(box_segue_length_spin->value());
  }
  else{
    box_dropbox->setSegueLevel(1);
    box_dropbox->setSegueLength(0);
  }

  sql=QString("delete from `DROPBOX_SCHED_CODES` where ")+
    QString::asprintf("`DROPBOX_ID`=%d",box_dropbox->id());
  RDSqlQuery::apply(sql);

  for(int i=0;i<box_schedcodes.size();i++) {
    sql=QString("insert into `DROPBOX_SCHED_CODES` set ")+
      QString::asprintf("`DROPBOX_ID`=%d,",box_dropbox->id())+
      "`SCHED_CODE`='"+RDEscapeString(box_schedcodes[i])+"'";
    RDSqlQuery::apply(sql);
  }
  done(true);
}


void EditDropbox::cancelData()
{
  done(false);
}
