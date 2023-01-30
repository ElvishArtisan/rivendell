// edit_rdlogedit.cpp
//
// Edit an RDLogedit Configuration
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

#include <QMessageBox>

#include <rd.h>
#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdtextvalidator.h>

#include "edit_rdlogedit.h"
#include "globals.h"

EditRDLogedit::EditRDLogedit(RDStation *station,RDStation *cae_station,
			     QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  lib_lib=new RDLogeditConf(station->name());

  //
  // Dialog Name
  //
  setWindowTitle("RDAdmin - "+tr("Configure RDLogedit"));

  //
  // Input Configuration
  //
  lib_input_card=new RDCardSelector(this);
  lib_input_card->
    setGeometry(sizeHint().width()/5,29,lib_input_card->sizeHint().width(),
		lib_input_card->sizeHint().height());
  QLabel *label=new QLabel(tr("Input"),this);
  label->setGeometry(sizeHint().width()/5,10,120,19);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

  //
  // Output Configuration
  //
  lib_output_card=new RDCardSelector(this);
  lib_output_card->
    setGeometry(4*sizeHint().width()/5-120,29,
		lib_output_card->sizeHint().width(),
		lib_output_card->sizeHint().height());
  label=new QLabel(tr("Output"),this);
  label->setGeometry(4*sizeHint().width()/5-120,10,120,19);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  // Settings
  //
  QLabel *setting_label=new QLabel(tr("Voice Tracker Settings"),this);
  setting_label->setGeometry(70,80,sizeHint().width()-80,19);
  setting_label->setFont(sectionLabelFont());
  setting_label->setAlignment(Qt::AlignLeft);

  //
  // Maximum Record Length
  //
  lib_maxlength_time=new QTimeEdit(this);
  lib_maxlength_time->setDisplayFormat("hh:mm:ss");
  lib_maxlength_time->setGeometry(180,100,85,19);
  QLabel *lib_maxlength_label=
    new QLabel(tr("Max Record Time:"),this);
  lib_maxlength_label->setFont(labelFont());
  lib_maxlength_label->setGeometry(25,101,150,19);
  lib_maxlength_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Autotrim Level
  //
  lib_threshold_spin=new QSpinBox(this);
  lib_threshold_spin->setGeometry(180,124,50,19);
  lib_threshold_spin->setMinimum(-99);
  lib_threshold_spin->setMaximum(0);
  label=new QLabel(tr("AutoTrim Threshold:"),this);
  label->setFont(labelFont());
  label->setGeometry(25,124,150,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label=new QLabel(tr("dbFS"),this);
  label->setFont(labelFont());
  label->setGeometry(235,124,120,19);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Normalization Level
  //
  lib_normalization_spin=new QSpinBox(this);
  lib_normalization_spin->setGeometry(180,148,50,19);
  lib_normalization_spin->setMinimum(-99);
  lib_normalization_spin->setMaximum(0);
  label=new QLabel(tr("Normalization Level:"),this);
  label->setFont(labelFont());
  label->setGeometry(25,148,150,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label=new QLabel(tr("dbFS"),this);
  label->setFont(labelFont());
  label->setGeometry(235,148,120,19);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Audio Margin
  //
  lib_preroll_spin=new QSpinBox(this);
  lib_preroll_spin->setGeometry(180,172,60,19);
  lib_preroll_spin->setMinimum(0);
  lib_preroll_spin->setMaximum(10000);
  lib_preroll_spin->setSingleStep(100);
  QLabel *lib_preroll_spin_label=new QLabel(tr("Audio Margin:"),this);
  lib_preroll_spin_label->setFont(labelFont());
  lib_preroll_spin_label->setGeometry(25,172,150,19);
  lib_preroll_spin_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QLabel *lib_preroll_spin_unit=new QLabel(tr("milliseconds"),this);
  lib_preroll_spin_unit->setFont(labelFont());
  lib_preroll_spin_unit->setGeometry(245,172,120,19);
  lib_preroll_spin_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Format
  //
  lib_format_box=new QComboBox(this);
  lib_format_box->setGeometry(180,196,150,19);
  connect(lib_format_box,SIGNAL(activated(int)),this,SLOT(formatData(int)));
  QLabel *lib_format_label=new QLabel(tr("Format:"),this);
  lib_format_label->setFont(labelFont());
  lib_format_label->setGeometry(25,196,150,19);
  lib_format_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Bitrate
  //
  lib_bitrate_box=new QComboBox(this);
  lib_bitrate_box->setGeometry(180,220,130,19);
  QLabel *lib_bitrate_label=new QLabel(tr("Bitrate:"),this);
  lib_bitrate_label->setFont(labelFont());
  lib_bitrate_label->setGeometry(25,220,150,19);
  lib_bitrate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Enable Second Start Button
  //
  lib_enable_second_start_box=new QComboBox(this);
  lib_enable_second_start_box->setGeometry(180,244,60,19);
  lib_enable_second_start_box->insertItem(0,tr("No"));
  lib_enable_second_start_box->insertItem(1,tr("Yes"));
  QLabel *lib_enable_second_start_label=
   new QLabel(tr("Enable 2nd Start Button:"),this);
  lib_enable_second_start_label->setFont(labelFont());
  lib_enable_second_start_label->setGeometry(10,244,165,19);
  lib_enable_second_start_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Validator
  //
  QIntValidator *validator=new QIntValidator(this);
  validator->setRange(1,999999);

  //
  // Waveform Caption
  //
  lib_waveform_caption_edit=new QLineEdit(this);
  lib_waveform_caption_edit->setGeometry(180,268,sizeHint().width()-190,19);
  QLabel *lib_waveform_caption_label=new QLabel(tr("WaveForm Caption:"),this);
  lib_waveform_caption_label->setFont(labelFont());
  lib_waveform_caption_label->setGeometry(25,268,150,19);
  lib_waveform_caption_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Play Start Cart
  //
  lib_startcart_edit=new QLineEdit(this);
  lib_startcart_edit->setGeometry(180,290,70,19);
  lib_startcart_edit->setValidator(validator);
  QLabel *lib_startcart_label=new QLabel(tr("Play Start Cart:"),this);
  lib_startcart_label->setFont(labelFont());
  lib_startcart_label->setGeometry(25,290,150,19);
  lib_startcart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QPushButton *button=new QPushButton(this);
  button->setFont(subButtonFont());
  button->setGeometry(260,288,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectStartData()));

  //
  // Play End Cart
  //
  lib_endcart_edit=new QLineEdit(this);
  lib_endcart_edit->setGeometry(180,314,70,19);
  lib_endcart_edit->setValidator(validator);
  QLabel *lib_endcart_label=new QLabel(tr("Play End Cart:"),this);
  lib_endcart_label->setFont(labelFont());
  lib_endcart_label->setGeometry(25,314,150,19);
  lib_endcart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  button=new QPushButton(this);
  button->setFont(subButtonFont());
  button->setGeometry(260,310,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectEndData()));

  //
  // Record Start Cart
  //
  lib_recstartcart_edit=new QLineEdit(this);
  lib_recstartcart_edit->setGeometry(180,338,70,19);
  lib_recstartcart_edit->setValidator(validator);
  QLabel *lib_recstartcart_label=new QLabel(tr("Record Start Cart:"),this);
  lib_recstartcart_label->setFont(labelFont());
  lib_recstartcart_label->setGeometry(25,338,150,19);
  lib_recstartcart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  button=new QPushButton(this);
  button->setFont(subButtonFont());
  button->setGeometry(260,336,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectRecordStartData()));

  //
  // Record End Cart
  //
  lib_recendcart_edit=new QLineEdit(this);
  lib_recendcart_edit->setGeometry(180,362,70,19);
  lib_recendcart_edit->setValidator(validator);
  QLabel *lib_recendcart_label=new QLabel(tr("Record End Cart:"),this);
  lib_recendcart_label->setFont(labelFont());
  lib_recendcart_label->setGeometry(25,362,150,19);
  lib_recendcart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  button=new QPushButton(this);
  button->setFont(subButtonFont());
  button->setGeometry(260,360,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectRecordEndData()));

  //
  // Default Channels
  //
  lib_channels_box=new QComboBox(this);
  lib_channels_box->setGeometry(180,386,60,19);
  QLabel *lib_channels_label=new QLabel(tr("Channels:"),this);
  lib_channels_label->setFont(labelFont());
  lib_channels_label->setGeometry(25,386,150,19);
  lib_channels_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Default Transition Type
  //
  lib_default_transtype_box=new QComboBox(this);
  lib_default_transtype_box->setGeometry(180,410,100,19);
  QLabel *lib_default_transtype_label=
    new QLabel(tr("Default Transition:"),this);
  lib_default_transtype_label->setFont(labelFont());
  lib_default_transtype_label->setGeometry(25,410,150,19);
  lib_default_transtype_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_default_transtype_box->insertItem(0,tr("Play"));
  lib_default_transtype_box->insertItem(1,tr("Segue"));
  lib_default_transtype_box->insertItem(2,tr("Stop"));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  if(cae_station->scanned()) {
    lib_input_card->setMaxCards(cae_station->cards());
    lib_output_card->setMaxCards(cae_station->cards());
    for(int i=0;i<lib_input_card->maxCards();i++) {
      lib_input_card->setMaxPorts(i,cae_station->cardInputs(i));
      lib_output_card->setMaxPorts(i,cae_station->cardOutputs(i));
    }
  }
  else {
    QMessageBox::information(this,tr("No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    lib_input_card->setDisabled(true);
    lib_output_card->setDisabled(true);
  }
  lib_input_card->setCard(lib_lib->inputCard());
  lib_input_card->setPort(lib_lib->inputPort());
  lib_output_card->setCard(lib_lib->outputCard());
  lib_output_card->setPort(lib_lib->outputPort());
  lib_maxlength_time->setTime(QTime(0,0,0).addMSecs(lib_lib->maxLength()));
  lib_threshold_spin->setValue(lib_lib->trimThreshold()/100);
  lib_normalization_spin->setValue(lib_lib->ripperLevel()/100);
  lib_waveform_caption_edit->setText(lib_lib->waveformCaption());
  unsigned cart=lib_lib->startCart();
  if(cart>0) {
    lib_startcart_edit->setText(QString::asprintf("%06u",cart));
  }
  if((cart=lib_lib->endCart())>0) {
    lib_endcart_edit->setText(QString::asprintf("%06u",cart));
  }  
  cart=lib_lib->recStartCart();
  if(cart>0) {
    lib_recstartcart_edit->setText(QString::asprintf("%06u",cart));
  }
  if((cart=lib_lib->recEndCart())>0) {
    lib_recendcart_edit->setText(QString::asprintf("%06u",cart));
  }  
  lib_preroll_spin->setValue(lib_lib->tailPreroll());
  lib_format_box->insertItem(0,tr("PCM16"));
  lib_format_box->insertItem(1,tr("PCM24"));
  lib_format_box->insertItem(2,tr("MPEG Layer 2"));
  switch(lib_lib->format()) {
  case 0:  // PCM16
    lib_format_box->setCurrentIndex(0);
    break;

  case 1:  // MPEG L2
    lib_format_box->setCurrentIndex(2);
    break;

  case 2:  // PCM24
    lib_format_box->setCurrentIndex(1);
    break;

  }
  lib_channels_box->insertItem(0,"1");
  lib_channels_box->insertItem(1,"2");
  lib_channels_box->setCurrentIndex(lib_lib->defaultChannels()-1);
  ShowBitRates(lib_format_box->currentIndex(),lib_lib->bitrate());
  lib_enable_second_start_box->setCurrentIndex(lib_lib->enableSecondStart());
  lib_default_transtype_box->setCurrentIndex(lib_lib->defaultTransType());
}


EditRDLogedit::~EditRDLogedit()
{
  delete lib_input_card;
  delete lib_output_card;
  delete lib_format_box;
  delete lib_channels_box;
  delete lib_bitrate_box;
  delete lib_maxlength_time;
}


QSize EditRDLogedit::sizeHint() const
{
  return QSize(395,524);
} 


QSizePolicy EditRDLogedit::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRDLogedit::formatData(int index)
{
  ShowBitRates(index,lib_lib->bitrate());
}


void EditRDLogedit::selectStartData()
{
  int cartnum=lib_startcart_edit->text().toInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    lib_startcart_edit->setText(QString::asprintf("%d",cartnum));
  }
}


void EditRDLogedit::selectEndData()
{
  int cartnum=lib_endcart_edit->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    lib_endcart_edit->setText(QString::asprintf("%d",cartnum));
  }
}


void EditRDLogedit::selectRecordStartData()
{
  int cartnum=lib_recstartcart_edit->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    lib_recstartcart_edit->setText(QString::asprintf("%d",cartnum));
  }
}


void EditRDLogedit::selectRecordEndData()
{
  int cartnum=lib_recendcart_edit->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    lib_recendcart_edit->setText(QString::asprintf("%d",cartnum));
  }
}


void EditRDLogedit::okData()
{
  unsigned rate=0;

  lib_lib->setInputCard(lib_input_card->card());
  lib_lib->setInputPort(lib_input_card->port());
  lib_lib->setOutputCard(lib_output_card->card());
  lib_lib->setOutputPort(lib_output_card->port());
  lib_lib->setMaxLength(QTime(0,0,0).msecsTo(lib_maxlength_time->time()));
  lib_lib->setTrimThreshold(lib_threshold_spin->value()*100);
  lib_lib->setRipperLevel(lib_normalization_spin->value()*100);
  lib_lib->setTailPreroll(lib_preroll_spin->value());
  lib_lib->setWaveformCaption(lib_waveform_caption_edit->text());
  if(lib_startcart_edit->text().isEmpty()) {
    lib_lib->setStartCart(0);
  }
  else {
    lib_lib->setStartCart(lib_startcart_edit->text().toUInt());
  }
  if(lib_endcart_edit->text().isEmpty()) {
    lib_lib->setEndCart(0);
  }
  else {
    lib_lib->setEndCart(lib_endcart_edit->text().toUInt());
  }
  if(lib_recstartcart_edit->text().isEmpty()) {
    lib_lib->setRecStartCart(0);
  }
  else {
    lib_lib->setRecStartCart(lib_recstartcart_edit->text().toUInt());
  }
  if(lib_recendcart_edit->text().isEmpty()) {
    lib_lib->setRecEndCart(0);
  }
  else {
    lib_lib->setRecEndCart(lib_recendcart_edit->text().toUInt());
  }
  switch(lib_format_box->currentIndex()) {
  case 0:  // PCM16
    lib_lib->setFormat(0);
    break;

  case 1:  // PCM24
    lib_lib->setFormat(2);
    break;

  case 2:  // MPEG L2
    lib_lib->setFormat(1);
    break;
  }
  lib_lib->setDefaultChannels(lib_channels_box->currentIndex()+1);
  rate=0;
  if(lib_format_box->currentIndex()==2) {
    rate=lib_bitrate_box->currentText().toInt();
  }
  lib_lib->setBitrate(rate*1000);
  lib_lib->setEnableSecondStart(lib_enable_second_start_box->currentIndex());
  lib_lib->setDefaultTransType(
    (RDLogLine::TransType)lib_default_transtype_box->currentIndex());

  done(0);
}


void EditRDLogedit::cancelData()
{
  done(1);
}


void EditRDLogedit::ShowBitRates(int index,int rate)
{
  lib_bitrate_box->clear();
  switch(index) {
  case 0:  // PCM16
  case 1:  // PCM24
    lib_bitrate_box->setDisabled(true);
    break;

  case 2:  // MPEG-1 Layer 2
    lib_bitrate_box->setEnabled(true);
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("32 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("48 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("56 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("64 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("80 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("96 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("112 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("128 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("160 kbps/chan"));
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),tr("192 kbps/chan"));
    switch(lib_lib->bitrate()) {
    case 32000:
      lib_bitrate_box->setCurrentIndex(0);
      break;

    case 48000:
      lib_bitrate_box->setCurrentIndex(1);
      break;

    case 56000:
      lib_bitrate_box->setCurrentIndex(2);
      break;

    case 64000:
      lib_bitrate_box->setCurrentIndex(3);
      break;

    case 80000:
      lib_bitrate_box->setCurrentIndex(4);
      break;

    case 96000:
      lib_bitrate_box->setCurrentIndex(5);
      break;

    case 112000:
      lib_bitrate_box->setCurrentIndex(6);
      break;

    case 128000:
      lib_bitrate_box->setCurrentIndex(7);
      break;

    case 160000:
      lib_bitrate_box->setCurrentIndex(8);
      break;

    case 192000:
      lib_bitrate_box->setCurrentIndex(9);
      break;

    default:
      lib_bitrate_box->setCurrentIndex(7);   // 128 kbps/chan
      break;
    }
    break;
  }
}
