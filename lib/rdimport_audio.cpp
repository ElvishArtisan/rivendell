// rdimport_audio.cpp
//
// Audio File Importation Dialog for Rivendell.
//
//   (C) Copyright 2002-2004,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdimport_audio.cpp,v 1.27.4.3 2013/11/13 23:36:33 cvs Exp $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qpainter.h>

#include <rd.h>
#include <rdconf.h>
#include <rdwavefile.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdescape_string.h>
#include <rdstation.h>
#include <rdimport_audio.h>
#include <rdlibrary_conf.h>

RDImportAudio::RDImportAudio(QString cutname,QString *path,
			     RDSettings *settings,bool *import_metadata,
			     RDWaveData *wavedata,RDCut *clipboard,
			     RDStation *station,RDUser *user,
			     bool *running,RDConfig *config,
			     QWidget *parent,const char *name) 
  : QDialog(parent,name)
{
  import_config=config;
  import_default_settings=settings;
  import_path=path;
  import_settings=settings;
  import_cutname=cutname;
  import_import_metadata=import_metadata;
  import_wavedata=wavedata;
  import_clipboard=clipboard;
  import_running=running;
  import_station=station;
  import_user=user;
  import_file_filter=RD_AUDIO_FILE_FILTER;
  import_import_conv=NULL;
  import_export_conv=NULL;

  setCaption(tr("Import/Export Audio File"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont mode_font=QFont("Helvetica",14,QFont::Bold);
  mode_font.setPixelSize(14);

  //
  // Mode Group
  //
  import_mode_group=new QButtonGroup(this,"import_mode_group");
  import_mode_group->hide();
  connect(import_mode_group,SIGNAL(clicked(int)),
	  this,SLOT(modeClickedData(int)));

  //
  // Input Mode Button
  //
  import_importmode_button=new QRadioButton(tr("Import File"), this,"import_importmode_button");
  import_mode_group->insert(import_importmode_button);
  import_importmode_button->setGeometry(10,10,sizeHint().width()-40,15);
  import_importmode_button->setFont(mode_font);
  import_importmode_button->setChecked(true);

  //
  // Input Filename
  //
  import_in_filename_edit=new QLineEdit(this,"import_in_filename_edit");
  import_in_filename_edit->setGeometry(85,30,sizeHint().width()-180,20);
  connect(import_in_filename_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filenameChangedData(const QString &)));
  import_in_filename_label=
    new QLabel(import_in_filename_edit,tr("Filename:"),
	       this,"import_in_filename_label");
  import_in_filename_label->setGeometry(10,30,70,20);
  import_in_filename_label->setFont(label_font);
  import_in_filename_label->setAlignment(AlignVCenter|AlignRight);

  //
  // Input File Selector Button
  //
  import_in_selector_button=
    new QPushButton(tr("&Select"),this,"import_in_selector_button");
  import_in_selector_button->setGeometry(sizeHint().width()-85,27,70,26);
  connect(import_in_selector_button,SIGNAL(clicked()),
	  this,SLOT(selectInputFileData()));

  //
  // Input Metadata
  //
  import_in_metadata_box=new QCheckBox(tr("Import file metadata"),this,"import_in_metadata_box");
  import_in_metadata_box->setGeometry(95,56,160,15);
  import_in_metadata_box->setChecked(*import_import_metadata);
  import_in_metadata_box->setFont(label_font);

  //
  // Input Channels
  //
  import_channels_box=new QComboBox(this,"import_channels_box");
  import_channels_box->setGeometry(310,54,50,20);
  import_channels_label=
    new QLabel(import_channels_box,tr("Channels:"),
	       this,"import_channels_label");
  import_channels_label->setGeometry(230,54,75,20);
  import_channels_label->setFont(label_font);
  import_channels_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Autotrim Check Box
  //
  import_autotrim_box=new QCheckBox(tr("Autotrim"),this,"import_autotrim_box");
  import_autotrim_box->setGeometry(95,82,80,15);
  import_autotrim_box->setChecked(true);
  import_autotrim_box->setFont(label_font);
  connect(import_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimCheckData(bool)));

  //
  // Autotrim Level
  //
  import_autotrim_spin=new QSpinBox(this,"import_autotrim_spin");
  import_autotrim_spin->setGeometry(235,80,40,20);
  import_autotrim_spin->setRange(-99,0);
  import_autotrim_label=new QLabel(import_autotrim_spin,tr("Level:"),
				 this,"autotrim_spin_label");
  import_autotrim_label->setGeometry(185,80,45,20);
  import_autotrim_label->setFont(label_font);
  import_autotrim_label->setAlignment(AlignRight|AlignVCenter);
  import_autotrim_unit=new QLabel(tr("dBFS"),this,"autotrim_unit_label");
  import_autotrim_unit->setGeometry(280,80,40,20);
  import_autotrim_unit->setFont(label_font);
  import_autotrim_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Output Mode Button
  //
  import_exportmode_button=new QRadioButton(tr("Export File"),this,"import_exportmode_button");
  import_mode_group->insert(import_exportmode_button);
  import_exportmode_button->setGeometry(10,120,sizeHint().width()-40,15);
  import_exportmode_button->setFont(mode_font);

  //
  // Output Filename
  //
  import_out_filename_edit=new QLineEdit(this,"import_out_filename_edit");
  import_out_filename_edit->setGeometry(85,140,sizeHint().width()-180,20);
  connect(import_out_filename_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filenameChangedData(const QString &)));
  import_out_filename_edit->setReadOnly(true);
  import_out_filename_label=
    new QLabel(import_out_filename_edit,tr("Filename:"),
	       this,"import_out_filename_label");
  import_out_filename_label->setGeometry(10,140,70,20);
  import_out_filename_label->setFont(label_font);
  import_out_filename_label->setAlignment(AlignVCenter|AlignRight);

  //
  // Output File Selector Button
  //
  import_out_selector_button=
    new QPushButton(tr("&Select"),this,"import_out_selector_button");
  import_out_selector_button->setGeometry(sizeHint().width()-85,137,70,26);
  connect(import_out_selector_button,SIGNAL(clicked()),
	  this,SLOT(selectOutputFileData()));

  //
  // Output Metadata
  //
  import_out_metadata_box=new QCheckBox(tr("Export file metadata"),this,"import_out_metadata_box");
  import_out_metadata_box->setGeometry(95,161,sizeHint().width()-210,15);
  import_out_metadata_box->setChecked(*import_import_metadata);
  import_out_metadata_box->setFont(label_font);

  //
  // Output Format
  //
  import_format_edit=new QLineEdit(this,"import_format_edit");
  import_format_edit->setGeometry(85,181,sizeHint().width()-180,20);
  import_format_edit->setReadOnly(true);
  import_format_edit->setText(import_settings->description());
  import_format_label=new QLabel(import_out_filename_edit,tr("Format:"),
		   this,"import_out_filename_label");
  import_format_label->setGeometry(10,181,70,20);
  import_format_label->setFont(label_font);
  import_format_label->setAlignment(AlignVCenter|AlignRight);

  //
  // Output Format Selector Button
  //
  import_out_format_button=
    new QPushButton(tr("S&et"),this,"import_out_format_button");
  import_out_format_button->setGeometry(sizeHint().width()-85,178,70,26);
  connect(import_out_format_button,SIGNAL(clicked()),
	  this,SLOT(selectOutputFormatData()));

  //
  // Progress Bar
  //
  import_bar=new RDBusyBar(this,"import_bar");
  import_bar->setGeometry(10,230,sizeHint().width()-20,20);

  //
  // Normalize Check Box
  //
  import_normalize_box=new QCheckBox(tr("Normalize"),this,"import_normalize_box");
  import_normalize_box->setGeometry(10,262,113,15);
  import_normalize_box->setChecked(true);
  import_normalize_box->setFont(label_font);
  connect(import_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  import_normalize_spin=new QSpinBox(this,"import_normalize_spin");
  import_normalize_spin->setGeometry(160,260,40,20);
  import_normalize_spin->setRange(-30,0);
  import_normalize_label=new QLabel(import_normalize_spin,tr("Level:"),
				 this,"normalize_spin_label");
  import_normalize_label->setGeometry(110,260,45,20);
  import_normalize_label->setFont(label_font);
  import_normalize_label->setAlignment(AlignRight|AlignVCenter);
  import_normalize_unit=new QLabel(tr("dBFS"),this,"normalize_unit_label");
  import_normalize_unit->setGeometry(205,260,40,20);
  import_normalize_unit->setFont(label_font);
  import_normalize_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Import Button
  //
  import_import_button=new QPushButton(tr("&Import"),this,"import_button");
  import_import_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  import_import_button->setFont(button_font);
  connect(import_import_button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  // Cancel Button
  //
  import_cancel_button=new QPushButton(tr("&Cancel"),this,"cancel_button");
  import_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  import_cancel_button->setFont(button_font);
  import_cancel_button->setDefault(true);
  connect(import_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  import_normalize_spin->setValue(settings->normalizationLevel()/100);
  import_autotrim_spin->setValue(settings->autotrimLevel()/100);
  import_channels_box->insertItem("1");
  import_channels_box->insertItem("2");
  import_channels_box->setCurrentItem(settings->channels()-1);

  filenameChangedData("");
  modeClickedData(import_mode_group->selectedId());
}


RDImportAudio::~RDImportAudio()
{
}


QSize RDImportAudio::sizeHint() const
{
  return QSize(470,332);
}


QSizePolicy RDImportAudio::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDImportAudio::enableAutotrim(bool state)
{
  import_autotrim_box->setChecked(state);
  autotrimCheckData(state);
}


void RDImportAudio::setAutotrimLevel(int lvl)
{
  import_autotrim_spin->setValue(lvl/100);
}


void RDImportAudio::enableNormalization(bool state)
{
  import_normalize_box->setChecked(state);
  normalizeCheckData(state);
}


void RDImportAudio::setNormalizationLevel(int lvl)
{
  import_normalize_spin->setValue(lvl/100);
}


void RDImportAudio::setChannels(int chans)
{
  import_channels_box->setCurrentItem(chans-1);
}


int RDImportAudio::exec(bool enable_import,bool enable_export)
{
  import_importmode_button->setEnabled(enable_import);
  import_in_filename_label->setEnabled(enable_import);
  import_in_filename_edit->setEnabled(enable_import);
  import_in_metadata_box->setEnabled(enable_import&&(import_wavedata!=NULL));
  import_in_selector_button->setEnabled(enable_import);
  import_channels_label->setEnabled(enable_import);
  import_autotrim_box->setEnabled(enable_import);
  import_autotrim_spin->
    setEnabled(enable_import&&import_autotrim_box->isChecked());
  import_autotrim_label->
    setEnabled(enable_import&&import_autotrim_box->isChecked());
  import_autotrim_unit->
    setEnabled(enable_import&&import_autotrim_box->isChecked());
  import_channels_box->setEnabled(enable_import);

  import_exportmode_button->setEnabled(enable_export);

  if(enable_export&&(!enable_import)) {
    import_exportmode_button->setChecked(true);
    modeClickedData(import_mode_group->id(import_exportmode_button));
  }
  return QDialog::exec();
}


void RDImportAudio::modeClickedData(int id)
{
  import_in_filename_label->setDisabled(id);
  import_in_filename_edit->setDisabled(id);
  import_in_metadata_box->setDisabled(id||(import_wavedata==NULL));
  import_in_selector_button->setDisabled(id);
  import_autotrim_box->setDisabled(id);
  import_autotrim_spin->setDisabled(id);
  import_autotrim_label->setDisabled(id);
  import_autotrim_unit->setDisabled(id);
  import_channels_box->setDisabled(id);
  import_channels_label->setDisabled(id);
  import_out_filename_label->setEnabled(id);
  import_out_filename_edit->setEnabled(id);
  import_out_metadata_box->setEnabled(id);
  import_out_selector_button->setEnabled(id);
  import_format_edit->setEnabled(id);
  import_format_label->setEnabled(id);
  import_out_format_button->setEnabled(id);
  if(id) {
    import_import_button->setText(tr("Export"));
  }
  else {
    import_import_button->setText(tr("Import"));
  }
}


void RDImportAudio::filenameChangedData(const QString &str)
{
  import_import_button->setDisabled(str.isEmpty());
}


void RDImportAudio::normalizeCheckData(bool state)
{
  import_normalize_spin->setEnabled(state);
  import_normalize_label->setEnabled(state);
  import_normalize_unit->setEnabled(state);
}


void RDImportAudio::autotrimCheckData(bool state)
{
  import_autotrim_spin->setEnabled(state);
  import_autotrim_label->setEnabled(state);
  import_autotrim_unit->setEnabled(state);
}


void RDImportAudio::selectInputFileData()
{
  QString filename;

  if(import_in_filename_edit->text().isEmpty()) {
    filename=
      QFileDialog::getOpenFileName(*import_path,
				   import_file_filter,this);
  }
  else {
    filename=
      QFileDialog::getOpenFileName(import_in_filename_edit->text(),
				   import_file_filter,this);
  }
  if(!filename.isEmpty()) {
    import_in_filename_edit->setText(filename);
    *import_path=RDGetPathPart(import_in_filename_edit->text());
  }
}


void RDImportAudio::selectOutputFileData()
{
  QString filename;
  QString filter=import_settings->formatName()+" (*."+
    RDSettings::defaultExtension(import_station->name(),
				 import_settings->format())+")";

  if(import_out_filename_edit->text().isEmpty()) {
    filename=
      QFileDialog::getSaveFileName(*import_path,filter,this);
  }
  else {
    filename=QFileDialog::getSaveFileName(import_out_filename_edit->text(),
					  filter,this);
  }
  if(!filename.isEmpty()) {
    import_out_filename_edit->
      setText(RDSettings::pathName(import_station->name(),filename,
				   import_settings->format()));
    *import_path=RDGetPathPart(import_out_filename_edit->text());
  }
}


void RDImportAudio::selectOutputFormatData()
{
  RDExportSettingsDialog *dialog=
    new RDExportSettingsDialog(import_settings,import_station,this,"dialog");
  dialog->exec();
  delete dialog;
  import_format_edit->setText(import_settings->description());
  import_out_filename_edit->
    setText(RDSettings::pathName(import_station->name(),
				 import_out_filename_edit->text(),
				 import_settings->format()));
}


void RDImportAudio::importData()
{
  if(import_import_conv!=NULL) {
    import_import_conv->abort();
    return;
  }
  if(import_export_conv!=NULL) {
    import_export_conv->abort();
    return;
  }
  if(import_mode_group->selectedId()==0) {
    Import();
  }
  else {
    Export();
  }
}


void RDImportAudio::cancelData()
{
  done(-1);
}


void RDImportAudio::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->moveTo(10,110);
  p->lineTo(sizeHint().width()-10,110);
  p->moveTo(0,215);
  p->lineTo(sizeHint().width(),215);
  p->moveTo(0,216);
  p->lineTo(sizeHint().width(),216);  
  p->end();
  delete p;
}


void RDImportAudio::closeEvent(QCloseEvent *e)
{
  if(!(*import_running)) {
    cancelData();
  }
}


void RDImportAudio::Import()
{
  RDSettings settings;
  RDAudioImport::ErrorCode conv_err;
  RDAudioConvert::ErrorCode audio_conv_err;

  if(*import_running) {
    return;
  }

  import_import_button->setText(tr("Abort"));
  if(!QFile::exists(import_in_filename_edit->text())) {
    QMessageBox::warning(this,tr("Import Audio File"),
			 tr("File does not exist!"));
    return;
  }

  //
  // Import
  //
  StartBar();
  RDCut *cut=new RDCut(import_cutname);
  import_import_conv=new RDAudioImport(import_station,import_config,this);
  import_import_conv->setCartNumber(cut->cartNumber());
  import_import_conv->setCutNumber(cut->cutNumber());
  import_import_conv->setSourceFile(import_in_filename_edit->text());
  settings.setChannels(import_channels_box->currentItem()+1);
  if(import_normalize_box->isChecked()) {
    settings.setNormalizationLevel(import_normalize_spin->value());
  }
  if(import_autotrim_box->isChecked()) {
    settings.setAutotrimLevel(import_autotrim_spin->value());
  }
  import_import_conv->setDestinationSettings(&settings);
  import_import_conv->setUseMetadata(false);
  *import_running=true;
  import_import_aborted=false;
  conv_err=import_import_conv->runImport(import_user->name(),import_user->password(),&audio_conv_err);
  *import_running=false;
  StopBar();
  switch(conv_err) {
  case RDAudioImport::ErrorOk:
    if(import_in_metadata_box->isChecked()) {
      RDWaveFile *wave=new RDWaveFile(import_in_filename_edit->text());
      wave->openWave(import_wavedata);
      wave->closeWave();
      delete wave;
    }
    cut->setOriginName(import_station->name());
    cut->
      setOriginDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    QMessageBox::information(this,tr("Import Complete"),
			     tr("Import complete!"));
    break;

  default:
    QMessageBox::warning(this,tr("Import Error"),
			 RDAudioImport::errorText(conv_err,audio_conv_err));
  }
  delete import_import_conv;
  import_import_conv=NULL;
  delete cut;
  import_import_button->setText(tr("Import"));
  done(0);
}


void RDImportAudio::Export()
{
  QString custom_cmd;
  RDAudioExport::ErrorCode conv_err;
  RDAudioConvert::ErrorCode audio_conv_err;

  if(*import_running) {
    return;
  }
  import_import_button->setText(tr("Abort"));

  //
  // Check Before Overwriting
  //
  import_dest_filename=import_out_filename_edit->text();
  if(QFile::exists(import_dest_filename)) {
    if(QMessageBox::warning(this,tr("File Exists"),
      tr("The selected file already exists!\nDo you want to overwrite it?"),
	       	    QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }

  //
  // Export
  //
  StartBar();
  RDCut *cut=new RDCut(import_cutname);
  import_settings->setNormalizationLevel(import_normalize_spin->value());
  import_export_conv=new RDAudioExport(import_station,import_config,this);
  import_export_conv->setCartNumber(cut->cartNumber());
  import_export_conv->setCutNumber(cut->cutNumber());
  import_export_conv->setDestinationFile(import_dest_filename);
  import_export_conv->setDestinationSettings(import_settings);
  import_export_conv->setRange(cut->startPoint(),cut->endPoint());
  import_export_conv->setEnableMetadata(import_out_metadata_box->isChecked());
  *import_running=true;
  import_import_aborted=false;
  conv_err=import_export_conv->runExport(import_user->name(),
					 import_user->password(),
					 &audio_conv_err);
  *import_running=false;
  StopBar();
  switch(conv_err) {
  case RDAudioExport::ErrorOk:
    QMessageBox::information(this,tr("Export Complete"),
			     tr("Export complete!"));
    break;

  default:
    QMessageBox::warning(this,tr("Export Error"),
			 RDAudioExport::errorText(conv_err,audio_conv_err));
  }
  delete import_export_conv;
  import_export_conv=NULL;
  delete cut;
  import_import_button->setText(tr("Export"));
  done(0);
}


void RDImportAudio::StartBar()
{
  import_bar->setEnabled(true);
  import_bar->activate(true);
  import_importmode_button->setDisabled(true);
  import_exportmode_button->setDisabled(true);
  import_in_filename_label->setDisabled(true);
  import_in_filename_edit->setDisabled(true);
  import_in_metadata_box->setDisabled(true);
  import_in_selector_button->setDisabled(true);
  import_channels_label->setDisabled(true);
  import_out_filename_label->setDisabled(true);
  import_out_filename_edit->setDisabled(true);
  import_out_metadata_box->setDisabled(true);
  import_out_selector_button->setDisabled(true);
  import_format_label->setDisabled(true);
  import_format_edit->setDisabled(true);
  import_out_format_button->setDisabled(true);
  import_normalize_box->setDisabled(true);
  import_normalize_spin->setDisabled(true);
  import_normalize_label->setDisabled(true);
  import_normalize_unit->setDisabled(true);
  import_autotrim_box->setDisabled(true);
  import_autotrim_spin->setDisabled(true);
  import_autotrim_label->setDisabled(true);
  import_autotrim_unit->setDisabled(true);
  import_channels_box->setDisabled(true);
  import_cancel_button->setDisabled(true);
  //  import_import_button->setDisabled(true);
}


void RDImportAudio::StopBar()
{
  import_bar->activate(false);
  import_bar->setDisabled(true);
}
