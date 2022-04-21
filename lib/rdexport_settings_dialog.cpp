// rdexport_settings_dialog.cpp
//
// Edit Audio Export Settings
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

#include <QMessageBox>
#include <QPushButton>

#include "rdescape_string.h"
#include "rdexport_settings_dialog.h"

RDExportSettingsDialog::RDExportSettingsDialog(const QString &caption,
					       QWidget *parent)
  : RDDialog(parent)
{
  lib_id=0;
  lib_caption=caption;
  lib_normalization_level_enabled=false;
  lib_autotrim_level_enabled=false;

  //
  // Dialog Name
  //
  setMinimumSize(sizeHint());
  setWindowTitle(caption+" - "+tr("Edit Audio Settings"));

  //
  // Name
  //
  lib_name_edit=new QLineEdit(this);
  lib_name_label=new QLabel(tr("Name")+":",this);
  lib_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_name_label->setFont(labelFont());

  //
  // Format
  //
  lib_format_box=new QComboBox(this);
  connect(lib_format_box,SIGNAL(activated(const QString &)),
	  this,SLOT(formatData(const QString &)));
  lib_format_label=new QLabel(tr("Format")+":",this);
  lib_format_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_format_label->setFont(labelFont());

  //
  // Channels
  //
  lib_channels_box=new QComboBox(this);
  lib_channels_label=new QLabel(tr("Channels")+":",this);
  lib_channels_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_channels_label->setFont(labelFont());

  //
  // Sample Rate
  //
  lib_samprate_box=new QComboBox(this);
  connect(lib_samprate_box,SIGNAL(activated(const QString &)),
	  this,SLOT(samprateData(const QString &)));
  lib_samprate_label=new QLabel(tr("Sample Rate")+":",this);
  lib_samprate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_samprate_label->setFont(labelFont());

  //
  // Bitrate
  //
  lib_bitrate_label=new QLabel(tr("Bitrate")+":",this);
  lib_bitrate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_bitrate_label->setFont(labelFont());
  lib_bitrate_box=new QComboBox(this);
  connect(lib_bitrate_box,SIGNAL(activated(const QString &)),
	  this,SLOT(bitrateData(const QString &)));
  lib_bitrate_unit=new QLabel("kbps",this);
  lib_bitrate_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  lib_bitrate_unit->setFont(labelFont());

  //
  // Quality
  //
  lib_quality_spin=new QSpinBox(this);
  lib_quality_spin->setRange(0,10);
  lib_quality_label=new QLabel(tr("Quality")+":",this);
  lib_quality_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_quality_label->setFont(labelFont());

  //
  // Normalization Level
  //
  lib_normalization_level_spin=new QSpinBox(this);
  lib_normalization_level_spin->setRange(-100,0);
  lib_normalization_level_label=new QLabel(tr("Normalization Level")+":",this);
  lib_normalization_level_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_normalization_level_label->setFont(labelFont());
  lib_normalization_level_label->hide();
  lib_normalization_level_spin->hide();
  lib_normalization_level_unit_label=new QLabel(tr("dBFS"),this);
  lib_normalization_level_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  lib_normalization_level_unit_label->setFont(labelFont());
  lib_normalization_level_label->hide();
  lib_normalization_level_spin->hide();
  lib_normalization_level_unit_label->hide();

  //
  // Autotrim Level
  //
  lib_autotrim_level_spin=new QSpinBox(this);
  lib_autotrim_level_spin->setRange(-100,0);
  lib_autotrim_level_label=new QLabel(tr("Autotrim Level")+":",this);
  lib_autotrim_level_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  lib_autotrim_level_label->setFont(labelFont());
  lib_autotrim_level_label->hide();
  lib_autotrim_level_spin->hide();
  lib_autotrim_level_unit_label=new QLabel(tr("dBFS"),this);
  lib_autotrim_level_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  lib_autotrim_level_unit_label->setFont(labelFont());
  lib_autotrim_level_label->hide();
  lib_autotrim_level_spin->hide();
  lib_autotrim_level_unit_label->hide();

  //
  //  Ok Button
  //
  lib_ok_button=new QPushButton(this);
  lib_ok_button->setDefault(true);
  lib_ok_button->setFont(buttonFont());
  lib_ok_button->setText(tr("OK"));
  connect(lib_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  lib_cancel_button=new QPushButton(this);
  lib_cancel_button->setFont(buttonFont());
  lib_cancel_button->setText(tr("Cancel"));
  connect(lib_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDExportSettingsDialog::~RDExportSettingsDialog()
{
  delete lib_channels_box;
  delete lib_samprate_box;
  delete lib_bitrate_box;
}


QSize RDExportSettingsDialog::sizeHint() const
{
  return QSize(325,180);
} 


QSizePolicy RDExportSettingsDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDExportSettingsDialog::setShowNormalizationLevel(bool state)
{
  lib_normalization_level_enabled=state;
  lib_normalization_level_label->setVisible(state);
  lib_normalization_level_spin->setVisible(state);
  lib_normalization_level_unit_label->setVisible(state);
}


void RDExportSettingsDialog::setShowAutotrimLevel(bool state)
{
  lib_autotrim_level_enabled=state;
  lib_autotrim_level_label->setVisible(state);
  lib_autotrim_level_spin->setVisible(state);
  lib_autotrim_level_unit_label->setVisible(state);
}


int RDExportSettingsDialog::exec(RDSettings *s,unsigned id)
{
  lib_settings=s;
  lib_id=id;

  lib_format_box->clear();
  lib_channels_box->clear();

  if(id==0) {
    lib_name_label->hide();
    lib_name_edit->hide();
  }
  else {
    lib_name_label->show();
    lib_name_edit->show();
    lib_name_edit->setText(lib_settings->name());
  }
  lib_format_box->insertItem(lib_format_box->count(),tr("PCM16"));
  if(lib_settings->format()==RDSettings::Pcm16) {
    lib_format_box->setCurrentIndex(lib_format_box->count()-1);
  }
  lib_format_box->insertItem(lib_format_box->count(),tr("PCM24"));
  if(lib_settings->format()==RDSettings::Pcm24) {
    lib_format_box->setCurrentIndex(lib_format_box->count()-1);
  }
  if(rda->station()->haveCapability(RDStation::HaveFlac)) {
    lib_format_box->insertItem(lib_format_box->count(),tr("FLAC"));
    if(lib_settings->format()==RDSettings::Flac) {
      lib_format_box->setCurrentIndex(lib_format_box->count()-1);
    }
  }
  lib_format_box->insertItem(lib_format_box->count(),tr("MPEG Layer 2"));
  if(lib_settings->format()==RDSettings::MpegL2) {
    lib_format_box->setCurrentIndex(lib_format_box->count()-1);
  }
  if(rda->station()->haveCapability(RDStation::HaveLame)) {
    lib_format_box->insertItem(lib_format_box->count(),tr("MPEG Layer 3"));
    if(lib_settings->format()==RDSettings::MpegL3) {
      lib_format_box->setCurrentIndex(lib_format_box->count()-1);
    }
  }
  if(rda->station()->haveCapability(RDStation::HaveOggenc)) {
    lib_format_box->insertItem(lib_format_box->count(),tr("OggVorbis"));
    if(lib_settings->format()==RDSettings::OggVorbis) {
      lib_format_box->setCurrentIndex(lib_format_box->count()-1);
    }
  }
  lib_channels_box->insertItem(lib_channels_box->count(),"1");
  lib_channels_box->insertItem(lib_channels_box->count(),"2");
  lib_channels_box->setCurrentIndex(lib_settings->channels()-1);
  lib_samprate_box->insertItem(lib_samprate_box->count(),"16000",16000);
  lib_samprate_box->insertItem(lib_samprate_box->count(),"22050",22050);
  lib_samprate_box->insertItem(lib_samprate_box->count(),"24000",24000);
  lib_samprate_box->insertItem(lib_samprate_box->count(),"32000",32000);
  lib_samprate_box->insertItem(lib_samprate_box->count(),"44100",44100);
  lib_samprate_box->insertItem(lib_samprate_box->count(),"48000",48000);
  for(int i=0;i<lib_samprate_box->count();i++) {
    if(lib_samprate_box->itemData(i).toString().toUInt()==
       lib_settings->sampleRate()) {
      lib_samprate_box->setCurrentIndex(i);
    }
  }
  ShowBitRates(lib_settings->format(),lib_settings->sampleRate(),
	       lib_settings->bitRate(),lib_settings->quality());

  lib_normalization_level_spin->setValue(lib_settings->normalizationLevel());
  lib_autotrim_level_spin->setValue(lib_settings->autotrimLevel());

  if(id==0) {
    setMinimumHeight(sizeHint().height());
    setMaximumHeight(sizeHint().height());
  }
  else {
    int height=sizeHint().height()+22;
    if(lib_normalization_level_enabled) {
      height+=22;
    }
    if(lib_autotrim_level_enabled) {
      height+=22;
    }
    setMinimumHeight(height);
    setMaximumHeight(height);
  }

  return RDDialog::exec();
}


void RDExportSettingsDialog::formatData(const QString &str)
{
  RDSettings::Format fmt=RDSettings::Pcm16;

  fmt=GetFormat(str);
  bool ok;
  unsigned bitrate=lib_bitrate_box->currentText().toUInt(&ok);
  if(!ok) {
    bitrate=0;
  }
  ShowBitRates(fmt,lib_samprate_box->currentText().toInt(),
	       bitrate,lib_quality_spin->value());
}


void RDExportSettingsDialog::samprateData(const QString &str)
{
  ShowBitRates(GetFormat(lib_format_box->currentText()),str.toInt(),
	       lib_bitrate_box->currentText().replace("kbps","").toInt(),
	       lib_quality_spin->value());
}

void RDExportSettingsDialog::bitrateData(const QString &str)
{
  lib_quality_label->setEnabled(str==tr("VBR"));
  lib_quality_spin->setEnabled(str==tr("VBR"));
}


void RDExportSettingsDialog::okData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  unsigned rate=0;

  if(lib_id>0) {
    sql=QString("select ")+
      "`ID` "+  // 00
      "from `ENCODER_PRESETS` where "+
      "`NAME`='"+RDEscapeString(lib_name_edit->text())+"' && "+
      QString::asprintf("`ID`!=%u",lib_id);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      QMessageBox::information(this,lib_caption+" - "+tr("Duplicate Name"),
			       tr("The name")+" \""+lib_name_edit->text()+"\" "+
			       tr("is already in use."));
      delete q;
      return;
    }
    delete q;
    lib_settings->setName(lib_name_edit->text());
  }
  lib_settings->setFormat(GetFormat(lib_format_box->currentText()));
  lib_settings->setChannels(lib_channels_box->currentIndex()+1);
  lib_settings->setSampleRate(lib_samprate_box->currentText().toInt());
  switch(lib_settings->format()) {
  case RDSettings::Pcm16:
  case RDSettings::Pcm24:
    lib_settings->setBitRate(0);
    lib_settings->setQuality(0);
    break;

  case RDSettings::MpegL1:
  case RDSettings::MpegL2:
  case RDSettings::MpegL2Wav:
  case RDSettings::MpegL3:
    if(!lib_bitrate_box->currentText().isEmpty()){
      rate=lib_bitrate_box->currentText().toInt();
    }
    if(rate!=0) {
      lib_settings->setBitRate(1000*rate);
      lib_settings->setQuality(0);
    }
    else {
      lib_settings->setBitRate(0);
      lib_quality_spin->setRange(0,9);
      lib_settings->setQuality(lib_quality_spin->value());
    }
    break;
      
  case RDSettings::Flac:
    lib_settings->setBitRate(0);
    lib_settings->setQuality(0);
    break;
      
  case RDSettings::OggVorbis:
    lib_settings->setBitRate(0);
    lib_quality_spin->setRange(-1,10);
    lib_settings->setQuality(lib_quality_spin->value());
    break;
  }
  lib_settings->setNormalizationLevel(lib_normalization_level_spin->value());
  lib_settings->setAutotrimLevel(lib_autotrim_level_spin->value());

  done(true);
}


void RDExportSettingsDialog::cancelData()
{
  done(false);
}


void RDExportSettingsDialog::resizeEvent(QResizeEvent *e)
{
  int ypos=2;

  if(lib_id>0) {
    lib_name_label->setGeometry(10,ypos,135,19);
    lib_name_edit->setGeometry(150,ypos,size().width()-160,19);
    ypos+=22;
  }

  lib_format_label->setGeometry(10,ypos,135,19);
  lib_format_box->setGeometry(150,ypos,150,19);
  ypos+=22;

  lib_channels_label->setGeometry(10,ypos,135,19);
  lib_channels_box->setGeometry(150,ypos,60,19);
  ypos+=22;

  lib_samprate_label->setGeometry(10,ypos,135,19);
  lib_samprate_box->setGeometry(150,ypos,100,19);
  ypos+=22;

  lib_bitrate_label->setGeometry(10,ypos,135,19);
  lib_bitrate_box->setGeometry(150,ypos,50,19);
  lib_bitrate_unit->setGeometry(205,ypos,size().width()-215,19);
  ypos+=22;

  lib_quality_label->setGeometry(10,ypos,135,19);
  lib_quality_spin->setGeometry(150,ypos,50,19);
  ypos+=22;

  lib_normalization_level_label->setGeometry(10,ypos,135,19);
  lib_normalization_level_spin->setGeometry(150,ypos,50,19);
  lib_normalization_level_unit_label->setGeometry(205,ypos,100,19);
  ypos+=22;

  lib_autotrim_level_label->setGeometry(10,ypos,135,19);
  lib_autotrim_level_spin->setGeometry(150,ypos,50,19);
  lib_autotrim_level_unit_label->setGeometry(205,ypos,100,19);
  ypos+=22;

  lib_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  lib_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void RDExportSettingsDialog::ShowBitRates(RDSettings::Format fmt,
					  int new_samprate,
					  int bitrate,int qual)
{
  printf("ShowBitRates: fmt: %u  new_samprate: %u  bitrate: %u  qual: %u\n",
	 fmt,new_samprate,bitrate,qual);
  int samprate=lib_samprate_box->currentText().toInt();
  int channels=lib_channels_box->currentText().toInt();
  lib_channels_box->clear();
  lib_samprate_box->clear();
  lib_bitrate_box->clear();
  switch(fmt) {
  case RDSettings::Pcm16:  // PCM16
  case RDSettings::Pcm24:  // PCM24
    lib_channels_box->insertItem(lib_channels_box->count(),"1");
    lib_channels_box->insertItem(lib_channels_box->count(),"2");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"32000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"44100");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"48000");
    lib_bitrate_box->setDisabled(true);
    lib_bitrate_label->setDisabled(true);
    lib_quality_spin->setDisabled(true);
    lib_quality_label->setDisabled(true);
    break;

  case RDSettings::MpegL1:  // MPEG-1 Layer 1
    lib_channels_box->insertItem(lib_channels_box->count(),"1");
    lib_channels_box->insertItem(lib_channels_box->count(),"2");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"16000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"22050");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"32000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"44100");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"48000");
    lib_bitrate_box->setEnabled(true);
    lib_bitrate_label->setEnabled(true);
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"32");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"64");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"96");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"128");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"160");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"192");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"224");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"256");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"288");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"320");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"352");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"384");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"416");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"448");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"VBR");
    switch(bitrate) {
    case 0:
      lib_bitrate_box->setCurrentIndex(14);
      lib_quality_spin->setEnabled(true);
      lib_quality_label->setEnabled(true);
      lib_quality_spin->setValue(qual);
      lib_quality_spin->setRange(0,9);
      break;

    case 32000:
      lib_bitrate_box->setCurrentIndex(0);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 64000:
      lib_bitrate_box->setCurrentIndex(1);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 96000:
      lib_bitrate_box->setCurrentIndex(2);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 128000:
      lib_bitrate_box->setCurrentIndex(3);
      lib_quality_label->setDisabled(true);
      lib_quality_spin->setDisabled(true);
      break;

    case 160000:
      lib_bitrate_box->setCurrentIndex(4);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 192000:
      lib_bitrate_box->setCurrentIndex(5);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 224000:
      lib_bitrate_box->setCurrentIndex(6);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 256000:
      lib_bitrate_box->setCurrentIndex(7);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 288000:
      lib_bitrate_box->setCurrentIndex(8);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 320000:
      lib_bitrate_box->setCurrentIndex(9);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 352000:
      lib_bitrate_box->setCurrentIndex(10);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 384000:
      lib_bitrate_box->setCurrentIndex(11);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 416000:
      lib_bitrate_box->setCurrentIndex(12);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 448000:
      lib_bitrate_box->setCurrentIndex(13);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;
    }
    break;

  case RDSettings::MpegL2:  // MPEG-1 Layer 2
  case RDSettings::MpegL2Wav:
    lib_channels_box->insertItem(lib_channels_box->count(),"1");
    lib_channels_box->insertItem(lib_channels_box->count(),"2");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"16000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"22050");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"32000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"44100");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"48000");
    lib_bitrate_box->setEnabled(true);
    lib_bitrate_label->setEnabled(true);
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"32");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"48");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"56");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"64");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"80");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"96");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"112");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"128");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"160");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"192");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"224");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"256");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"320");
    lib_bitrate_box->insertItem(lib_bitrate_box->count(),"384");
    switch(bitrate) {
    case 0:
      lib_bitrate_box->setCurrentIndex(11);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 32000:
      lib_bitrate_box->setCurrentIndex(0);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 48000:
      lib_bitrate_box->setCurrentIndex(1);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 56000:
      lib_bitrate_box->setCurrentIndex(2);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 64000:
      lib_bitrate_box->setCurrentIndex(3);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 80000:
      lib_bitrate_box->setCurrentIndex(4);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 96000:
      lib_bitrate_box->setCurrentIndex(5);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 112000:
      lib_bitrate_box->setCurrentIndex(6);
      lib_quality_label->setDisabled(true);
      lib_quality_spin->setDisabled(true);
      break;

    case 128000:
      lib_bitrate_box->setCurrentIndex(7);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 160000:
      lib_bitrate_box->setCurrentIndex(8);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 192000:
      lib_bitrate_box->setCurrentIndex(9);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 224000:
      lib_bitrate_box->setCurrentIndex(10);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 256000:
      lib_bitrate_box->setCurrentIndex(11);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 320000:
      lib_bitrate_box->setCurrentIndex(12);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;

    case 384000:
      lib_bitrate_box->setCurrentIndex(13);
      lib_quality_spin->setDisabled(true);
      lib_quality_label->setDisabled(true);
      break;
    }
    break;

  case RDSettings::MpegL3:  // MPEG-1 Layer 3
    lib_channels_box->insertItem(lib_channels_box->count(),"1");
    lib_channels_box->insertItem(lib_channels_box->count(),"2");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"32000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"44100");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"48000");
    lib_bitrate_box->setEnabled(true);
    lib_bitrate_label->setEnabled(true);
    switch(samprate) {
    case 32000:
    case 44100:
    case 48000:
    default:
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"32");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"40");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"48");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"56");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"64");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"80");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"96");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"112");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"128");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"160");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"192");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"224");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"256");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"320");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"VBR");
      switch(bitrate) {
      case 0:
	lib_bitrate_box->setCurrentIndex(14);
	lib_quality_spin->setEnabled(true);
	lib_quality_label->setEnabled(true);
	lib_quality_spin->setRange(0,9);
	lib_quality_spin->setValue(qual);
	break;
		    
      case 32000:
	lib_bitrate_box->setCurrentIndex(0);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 40000:
	lib_bitrate_box->setCurrentIndex(1);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 48000:
	lib_bitrate_box->setCurrentIndex(2);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 56000:
	lib_bitrate_box->setCurrentIndex(3);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 64000:
	lib_bitrate_box->setCurrentIndex(4);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 80000:
	lib_bitrate_box->setCurrentIndex(5);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 96000:
	lib_bitrate_box->setCurrentIndex(6);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 112000:
	lib_bitrate_box->setCurrentIndex(7);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 128000:
	lib_bitrate_box->setCurrentIndex(8);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 160000:
	lib_bitrate_box->setCurrentIndex(9);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 192000:
	lib_bitrate_box->setCurrentIndex(10);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 224000:
	lib_bitrate_box->setCurrentIndex(11);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 256000:
	lib_bitrate_box->setCurrentIndex(12);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 320000:
	lib_bitrate_box->setCurrentIndex(13);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
      }
      break;

    case 16000:
    case 22050:
    case 24000:
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"8");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"16");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"24");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"32");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"40");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"48");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"56");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"64");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"80");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"96");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"112");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"128");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"144");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"160");
      lib_bitrate_box->insertItem(lib_bitrate_box->count(),"VBR");
      switch(bitrate) {
      case 0:
	lib_bitrate_box->setCurrentIndex(14);
	lib_quality_spin->setEnabled(true);
	lib_quality_label->setEnabled(true);
	lib_quality_spin->setRange(0,9);
	lib_quality_spin->setValue(qual);
	break;
		    
      case 8000:
	lib_bitrate_box->setCurrentIndex(0);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 16000:
	lib_bitrate_box->setCurrentIndex(1);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 24000:
	lib_bitrate_box->setCurrentIndex(2);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 32000:
	lib_bitrate_box->setCurrentIndex(3);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 40000:
	lib_bitrate_box->setCurrentIndex(4);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 48000:
	lib_bitrate_box->setCurrentIndex(5);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 56000:
	lib_bitrate_box->setCurrentIndex(6);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 64000:
	lib_bitrate_box->setCurrentIndex(7);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 80000:
	lib_bitrate_box->setCurrentIndex(8);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 96000:
	lib_bitrate_box->setCurrentIndex(9);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 112000:
	lib_bitrate_box->setCurrentIndex(10);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 128000:
	lib_bitrate_box->setCurrentIndex(11);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 144000:
	lib_bitrate_box->setCurrentIndex(12);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
		    
      case 160000:
	lib_bitrate_box->setCurrentIndex(13);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;
      }
      break;
    }
    break;

  case RDSettings::Flac:
    lib_channels_box->insertItem(lib_channels_box->count(),"1");
    lib_channels_box->insertItem(lib_channels_box->count(),"2");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"32000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"44100");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"48000");
    lib_bitrate_box->setDisabled(true);
    lib_bitrate_label->setDisabled(true);
    lib_quality_spin->setDisabled(true);
    lib_quality_label->setDisabled(true);
    break;

  case RDSettings::OggVorbis:
    lib_channels_box->insertItem(lib_channels_box->count(),"1");
    lib_channels_box->insertItem(lib_channels_box->count(),"2");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"32000");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"44100");
    lib_samprate_box->insertItem(lib_samprate_box->count(),"48000");
    lib_bitrate_box->setDisabled(true);
    lib_bitrate_label->setDisabled(true);
    lib_quality_spin->setEnabled(true);
    lib_quality_label->setEnabled(true);
    lib_quality_spin->setRange(-1,10);
    lib_quality_spin->setValue(qual);
    break;
  }
  lib_channels_box->setCurrentText(QString::asprintf("%u",channels));
  lib_samprate_box->setCurrentText(QString::asprintf("%u",samprate));
}


RDSettings::Format RDExportSettingsDialog::GetFormat(QString str)
{
  if(str==tr("PCM16")) {
    return RDSettings::Pcm16;
  }
  if(str==tr("PCM24")) {
    return RDSettings::Pcm24;
  }
  if(str==tr("FLAC")) {
    return RDSettings::Flac;
  }
  if(str==tr("MPEG Layer 2")) {
    return RDSettings::MpegL2;
  }
  if(str==tr("MPEG Layer 3")) {
    return RDSettings::MpegL3;
  }
  if(str==tr("OggVorbis")) {
    return RDSettings::OggVorbis;
  }
  return RDSettings::Pcm16;
}
