// rdexport_settings_dialog.cpp
//
// Edit RDLibrary Settings
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <q3textedit.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <QLabel>
#include <math.h>

#include <rdexport_settings_dialog.h>


RDExportSettingsDialog::RDExportSettingsDialog(RDSettings *settings,
					       RDStation *station,
					       QWidget *parent)
  : QDialog(parent,"",true)
{
  lib_settings=settings;
  lib_station=station;

  //
  // Generate Fonts
  //
  QFont button_font("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Dialog Name
  //
  setCaption(tr("Edit Export Settings"));

  //
  // Default Format
  //
  lib_format_box=new QComboBox(this);
  lib_format_box->setGeometry(100,10,150,19);
  connect(lib_format_box,SIGNAL(activated(const QString &)),
	  this,SLOT(formatData(const QString &)));
  QLabel *lib_format_label=new QLabel(lib_format_box,"Format:",this);
  lib_format_label->setGeometry(25,10,70,19);
  lib_format_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Default Channels
  //
  lib_channels_box=new QComboBox(this);
  lib_channels_box->setGeometry(100,32,60,19);
  QLabel *lib_channels_label=
    new QLabel(lib_channels_box,tr("&Channels:"),this);
  lib_channels_label->setGeometry(25,32,70,19);
  lib_channels_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Default Sample Rate
  //
  lib_samprate_box=new QComboBox(this);
  lib_samprate_box->setGeometry(100,54,100,19);
  connect(lib_samprate_box,SIGNAL(activated(const QString &)),
	  this,SLOT(samprateData(const QString &)));
  QLabel *lib_samprate_label=
    new QLabel(lib_samprate_box,tr("&Sample Rate:"),this);
  lib_samprate_label->setGeometry(25,54,75,19);
  lib_samprate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Default Bitrate
  //
  lib_bitrate_box=new QComboBox(this);
  lib_bitrate_box->setGeometry(100,76,100,19);
  connect(lib_bitrate_box,SIGNAL(activated(const QString &)),
	  this,SLOT(bitrateData(const QString &)));
  lib_bitrate_label=new QLabel(lib_bitrate_box,tr("&Bitrate:"),this);
  lib_bitrate_label->setGeometry(25,76,70,19);
  lib_bitrate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Quality
  //
  lib_quality_spin=new QSpinBox(this);
  lib_quality_spin->setGeometry(100,98,50,19);
  lib_quality_spin->setRange(0,10);
  lib_quality_label=new QLabel(lib_quality_spin,tr("&Quality:"),this);
  lib_quality_label->setGeometry(25,98,70,19);
  lib_quality_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(button_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(button_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  lib_format_box->insertItem(tr("PCM16"));
  if(settings->format()==RDSettings::Pcm16) {
    lib_format_box->setCurrentItem(lib_format_box->count()-1);
  }
  lib_format_box->insertItem(tr("PCM24"));
  if(settings->format()==RDSettings::Pcm24) {
    lib_format_box->setCurrentItem(lib_format_box->count()-1);
  }
  if(station->haveCapability(RDStation::HaveFlac)) {
    lib_format_box->insertItem(tr("FLAC"));
    if(settings->format()==RDSettings::Flac) {
      lib_format_box->setCurrentItem(lib_format_box->count()-1);
    }
  }
  lib_format_box->insertItem(tr("MPEG Layer 2"));
  if(settings->format()==RDSettings::MpegL2) {
    lib_format_box->setCurrentItem(lib_format_box->count()-1);
  }
  if(station->haveCapability(RDStation::HaveLame)) {
    lib_format_box->insertItem(tr("MPEG Layer 3"));
    if(settings->format()==RDSettings::MpegL3) {
      lib_format_box->setCurrentItem(lib_format_box->count()-1);
    }
  }
  if(station->haveCapability(RDStation::HaveOggenc)) {
    lib_format_box->insertItem(tr("OggVorbis"));
    if(settings->format()==RDSettings::OggVorbis) {
      lib_format_box->setCurrentItem(lib_format_box->count()-1);
    }
  }
  lib_channels_box->insertItem("1");
  lib_channels_box->insertItem("2");
  lib_channels_box->setCurrentItem(lib_settings->channels()-1);
  lib_samprate_box->insertItem("16000");
  lib_samprate_box->insertItem("22050");
  lib_samprate_box->insertItem("24000");
  lib_samprate_box->insertItem("32000");
  lib_samprate_box->insertItem("44100");
  lib_samprate_box->insertItem("48000");
  for(int i=0;i<lib_samprate_box->count();i++) {
    if(lib_samprate_box->text(i).toUInt()==lib_settings->sampleRate()) {
      lib_samprate_box->setCurrentItem(i);
    }
  }
  ShowBitRates(lib_settings->format(),lib_settings->sampleRate(),
	       lib_settings->bitRate(),lib_settings->quality());
}


RDExportSettingsDialog::~RDExportSettingsDialog()
{
  delete lib_channels_box;
  delete lib_samprate_box;
  delete lib_bitrate_box;
}


QSize RDExportSettingsDialog::sizeHint() const
{
  return QSize(275,190);
} 


QSizePolicy RDExportSettingsDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
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
  unsigned rate=0;

  lib_settings->setFormat(GetFormat(lib_format_box->currentText()));
  lib_settings->setChannels(lib_channels_box->currentItem()+1);
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
      rate=lib_bitrate_box->currentText().toInt();
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
  done(0);
}


void RDExportSettingsDialog::cancelData()
{
  done(1);
}


void RDExportSettingsDialog::ShowBitRates(RDSettings::Format fmt,
					  int new_samprate,
					  int bitrate,int qual)
{
  int samprate=lib_samprate_box->currentText().toInt();
  int channels=lib_channels_box->currentText().toInt();
  lib_channels_box->clear();
  lib_samprate_box->clear();
  lib_bitrate_box->clear();
  switch(fmt) {
      case RDSettings::Pcm16:  // PCM16
      case RDSettings::Pcm24:  // PCM16
	lib_channels_box->insertItem("1");
	lib_channels_box->insertItem("2");
	lib_samprate_box->insertItem("32000");
	lib_samprate_box->insertItem("44100");
	lib_samprate_box->insertItem("48000");
	lib_bitrate_box->setDisabled(true);
	lib_bitrate_label->setDisabled(true);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;

      case RDSettings::MpegL1:  // MPEG-1 Layer 1
	lib_channels_box->insertItem("1");
	lib_channels_box->insertItem("2");
	lib_samprate_box->insertItem("16000");
	lib_samprate_box->insertItem("22050");
	lib_samprate_box->insertItem("32000");
	lib_samprate_box->insertItem("44100");
	lib_samprate_box->insertItem("48000");
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_label->setEnabled(true);
	lib_bitrate_box->insertItem(tr("32 kbps"));
	lib_bitrate_box->insertItem(tr("64 kbps"));
	lib_bitrate_box->insertItem(tr("96 kbps"));
	lib_bitrate_box->insertItem(tr("128 kbps"));
	lib_bitrate_box->insertItem(tr("160 kbps"));
	lib_bitrate_box->insertItem(tr("192 kbps"));
	lib_bitrate_box->insertItem(tr("224 kbps"));
	lib_bitrate_box->insertItem(tr("256 kbps"));
	lib_bitrate_box->insertItem(tr("288 kbps"));
	lib_bitrate_box->insertItem(tr("320 kbps"));
	lib_bitrate_box->insertItem(tr("352 kbps"));
	lib_bitrate_box->insertItem(tr("384 kbps"));
	lib_bitrate_box->insertItem(tr("416 kbps"));
	lib_bitrate_box->insertItem(tr("448 kbps"));
	lib_bitrate_box->insertItem(tr("VBR"));
	switch(bitrate) {
	    case 0:
	      lib_bitrate_box->setCurrentItem(14);
	      lib_quality_spin->setEnabled(true);
	      lib_quality_label->setEnabled(true);
	      lib_quality_spin->setValue(qual);
	      lib_quality_spin->setRange(0,9);
	      break;

	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(1);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(2);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(3);
	      lib_quality_label->setDisabled(true);
	      lib_quality_spin->setDisabled(true);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(4);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(5);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 224000:
	      lib_bitrate_box->setCurrentItem(6);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 256000:
	      lib_bitrate_box->setCurrentItem(7);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 288000:
	      lib_bitrate_box->setCurrentItem(8);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 320000:
	      lib_bitrate_box->setCurrentItem(9);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 352000:
	      lib_bitrate_box->setCurrentItem(10);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 384000:
	      lib_bitrate_box->setCurrentItem(11);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 416000:
	      lib_bitrate_box->setCurrentItem(12);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 448000:
	      lib_bitrate_box->setCurrentItem(13);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;
	}
	break;

      case RDSettings::MpegL2:  // MPEG-1 Layer 2
  case RDSettings::MpegL2Wav:
	lib_channels_box->insertItem("1");
	lib_channels_box->insertItem("2");
	lib_samprate_box->insertItem("16000");
	lib_samprate_box->insertItem("22050");
	lib_samprate_box->insertItem("32000");
	lib_samprate_box->insertItem("44100");
	lib_samprate_box->insertItem("48000");
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_label->setEnabled(true);
	lib_bitrate_box->insertItem(tr("32 kbps"));
	lib_bitrate_box->insertItem(tr("48 kbps"));
	lib_bitrate_box->insertItem(tr("56 kbps"));
	lib_bitrate_box->insertItem(tr("64 kbps"));
	lib_bitrate_box->insertItem(tr("80 kbps"));
	lib_bitrate_box->insertItem(tr("96 kbps"));
	lib_bitrate_box->insertItem(tr("112 kbps"));
	lib_bitrate_box->insertItem(tr("128 kbps"));
	lib_bitrate_box->insertItem(tr("160 kbps"));
	lib_bitrate_box->insertItem(tr("192 kbps"));
	lib_bitrate_box->insertItem(tr("224 kbps"));
	lib_bitrate_box->insertItem(tr("256 kbps"));
	lib_bitrate_box->insertItem(tr("320 kbps"));
	lib_bitrate_box->insertItem(tr("384 kbps"));
	switch(bitrate) {
	    case 0:
	      lib_bitrate_box->setCurrentItem(11);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 48000:
	      lib_bitrate_box->setCurrentItem(1);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 56000:
	      lib_bitrate_box->setCurrentItem(2);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(3);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 80000:
	      lib_bitrate_box->setCurrentItem(4);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(5);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 112000:
	      lib_bitrate_box->setCurrentItem(6);
	      lib_quality_label->setDisabled(true);
	      lib_quality_spin->setDisabled(true);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(7);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(8);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(9);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 224000:
	      lib_bitrate_box->setCurrentItem(10);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 256000:
	      lib_bitrate_box->setCurrentItem(11);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 320000:
	      lib_bitrate_box->setCurrentItem(12);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;

	    case 384000:
	      lib_bitrate_box->setCurrentItem(13);
	      lib_quality_spin->setDisabled(true);
	      lib_quality_label->setDisabled(true);
	      break;
	}
	break;

      case RDSettings::MpegL3:  // MPEG-1 Layer 3
	lib_channels_box->insertItem("1");
	lib_channels_box->insertItem("2");
	lib_samprate_box->insertItem("32000");
	lib_samprate_box->insertItem("44100");
	lib_samprate_box->insertItem("48000");
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_label->setEnabled(true);
	switch(samprate) {
	    case 32000:
	    case 44100:
	    case 48000:
	    default:
	      lib_bitrate_box->insertItem(tr("32 kbps"));
	      lib_bitrate_box->insertItem(tr("40 kbps"));
	      lib_bitrate_box->insertItem(tr("48 kbps"));
	      lib_bitrate_box->insertItem(tr("56 kbps"));
	      lib_bitrate_box->insertItem(tr("64 kbps"));
	      lib_bitrate_box->insertItem(tr("80 kbps"));
	      lib_bitrate_box->insertItem(tr("96 kbps"));
	      lib_bitrate_box->insertItem(tr("112 kbps"));
	      lib_bitrate_box->insertItem(tr("128 kbps"));
	      lib_bitrate_box->insertItem(tr("160 kbps"));
	      lib_bitrate_box->insertItem(tr("192 kbps"));
	      lib_bitrate_box->insertItem(tr("224 kbps"));
	      lib_bitrate_box->insertItem(tr("256 kbps"));
	      lib_bitrate_box->insertItem(tr("320 kbps"));
	      lib_bitrate_box->insertItem(tr("VBR"));
	      switch(bitrate) {
		  case 0:
		    lib_bitrate_box->setCurrentItem(14);
		    lib_quality_spin->setEnabled(true);
		    lib_quality_label->setEnabled(true);
		    lib_quality_spin->setRange(0,9);
		    lib_quality_spin->setValue(qual);
		    break;
		    
		  case 32000:
		    lib_bitrate_box->setCurrentItem(0);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 40000:
		    lib_bitrate_box->setCurrentItem(1);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 48000:
		    lib_bitrate_box->setCurrentItem(2);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 56000:
		    lib_bitrate_box->setCurrentItem(3);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 64000:
		    lib_bitrate_box->setCurrentItem(4);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 80000:
		    lib_bitrate_box->setCurrentItem(5);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 96000:
		    lib_bitrate_box->setCurrentItem(6);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 112000:
		    lib_bitrate_box->setCurrentItem(7);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 128000:
		    lib_bitrate_box->setCurrentItem(8);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 160000:
		    lib_bitrate_box->setCurrentItem(9);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 192000:
		    lib_bitrate_box->setCurrentItem(10);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 224000:
		    lib_bitrate_box->setCurrentItem(11);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 256000:
		    lib_bitrate_box->setCurrentItem(12);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 320000:
		    lib_bitrate_box->setCurrentItem(13);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
	      }
	      break;

	    case 16000:
	    case 22050:
	    case 24000:
	      lib_bitrate_box->insertItem(tr("8 kbps"));
	      lib_bitrate_box->insertItem(tr("16 kbps"));
	      lib_bitrate_box->insertItem(tr("24 kbps"));
	      lib_bitrate_box->insertItem(tr("32 kbps"));
	      lib_bitrate_box->insertItem(tr("40 kbps"));
	      lib_bitrate_box->insertItem(tr("48 kbps"));
	      lib_bitrate_box->insertItem(tr("56 kbps"));
	      lib_bitrate_box->insertItem(tr("64 kbps"));
	      lib_bitrate_box->insertItem(tr("80 kbps"));
	      lib_bitrate_box->insertItem(tr("96 kbps"));
	      lib_bitrate_box->insertItem(tr("112 kbps"));
	      lib_bitrate_box->insertItem(tr("128 kbps"));
	      lib_bitrate_box->insertItem(tr("144 kbps"));
	      lib_bitrate_box->insertItem(tr("160 kbps"));
	      lib_bitrate_box->insertItem(tr("VBR"));
	      switch(bitrate) {
		  case 0:
		    lib_bitrate_box->setCurrentItem(14);
		    lib_quality_spin->setEnabled(true);
		    lib_quality_label->setEnabled(true);
		    lib_quality_spin->setRange(0,9);
		    lib_quality_spin->setValue(qual);
		    break;
		    
		  case 8000:
		    lib_bitrate_box->setCurrentItem(0);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 16000:
		    lib_bitrate_box->setCurrentItem(1);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 24000:
		    lib_bitrate_box->setCurrentItem(2);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 32000:
		    lib_bitrate_box->setCurrentItem(3);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 40000:
		    lib_bitrate_box->setCurrentItem(4);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 48000:
		    lib_bitrate_box->setCurrentItem(5);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 56000:
		    lib_bitrate_box->setCurrentItem(6);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 64000:
		    lib_bitrate_box->setCurrentItem(7);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 80000:
		    lib_bitrate_box->setCurrentItem(8);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 96000:
		    lib_bitrate_box->setCurrentItem(9);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 112000:
		    lib_bitrate_box->setCurrentItem(10);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 128000:
		    lib_bitrate_box->setCurrentItem(11);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 144000:
		    lib_bitrate_box->setCurrentItem(12);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
		    
		  case 160000:
		    lib_bitrate_box->setCurrentItem(13);
		    lib_quality_spin->setDisabled(true);
		    lib_quality_label->setDisabled(true);
		    break;
	      }
	      break;
	}
	break;

      case RDSettings::Flac:
	lib_channels_box->insertItem("1");
	lib_channels_box->insertItem("2");
	lib_samprate_box->insertItem("32000");
	lib_samprate_box->insertItem("44100");
	lib_samprate_box->insertItem("48000");
	lib_bitrate_box->setDisabled(true);
	lib_bitrate_label->setDisabled(true);
	lib_quality_spin->setDisabled(true);
	lib_quality_label->setDisabled(true);
	break;

      case RDSettings::OggVorbis:
	lib_channels_box->insertItem("1");
	lib_channels_box->insertItem("2");
	lib_samprate_box->insertItem("32000");
	lib_samprate_box->insertItem("44100");
	lib_samprate_box->insertItem("48000");
	lib_bitrate_box->setDisabled(true);
	lib_bitrate_label->setDisabled(true);
	lib_quality_spin->setEnabled(true);
	lib_quality_label->setEnabled(true);
	lib_quality_spin->setRange(-1,10);
	lib_quality_spin->setValue(qual);
	break;
  }
  SetCurrentItem(lib_channels_box,channels);
  SetCurrentItem(lib_samprate_box,samprate);
}


void RDExportSettingsDialog::SetCurrentItem(QComboBox *box,int value)
{
  for(int i=0;i<box->count();i++) {
    if(box->text(i).toInt()==value) {
      box->setCurrentItem(i);
    }
  }
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

