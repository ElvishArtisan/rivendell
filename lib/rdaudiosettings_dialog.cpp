// rdaudiosettings_dialog.cpp
//
// Edit an RDAudioSettings object.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdaudiosettings_dialog.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <rdconf.h>
#include <math.h>

#include <rdaudiosettings_dialog.h>


RDAudioSettingsDialog::RDAudioSettingsDialog(RDAudioSettings *settings,
					     bool mpeg,
					     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  lib_lib=settings;

  //
  // Dialog Name
  //
  setCaption(tr("Edit Settings"));

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
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Default Format
  //
  lib_format_box=new QComboBox(this,"lib_name_edit");
  lib_format_box->setGeometry(150,10,150,20);
  lib_format_box->setFont(font);
  connect(lib_format_box,SIGNAL(activated(int)),this,SLOT(formatData(int)));
  QLabel *lib_format_label=new QLabel(lib_format_box,tr("Default &Format:"),
				      this,"lib_format_label");
  lib_format_label->setGeometry(25,10,120,20);
  lib_format_label->setFont(font);
  lib_format_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Channels
  //
  lib_channels_box=new QComboBox(this,"lib_name_edit");
  lib_channels_box->setGeometry(150,32,60,20);
  lib_channels_box->setFont(font);
  QLabel *lib_channels_label=new QLabel(lib_channels_box,
					tr("Default &Channels:"),
					this,"lib_channels_label");
  lib_channels_label->setGeometry(25,32,120,20);
  lib_channels_label->setFont(font);
  lib_channels_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Sample Rate
  //
  lib_samprate_box=new QComboBox(this,"lib_name_edit");
  lib_samprate_box->setGeometry(150,54,100,20);
  lib_samprate_box->setFont(font);
  QLabel *lib_samprate_label=
    new QLabel(lib_samprate_box,tr("Default &Sample Rate:"),this,
	       "lib_samprate_label");
  lib_samprate_label->setGeometry(20,54,125,20);
  lib_samprate_label->setFont(font);
  lib_samprate_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Bitrate
  //
  lib_bitrate_box=new QComboBox(this,"lib_name_edit");
  lib_bitrate_box->setGeometry(150,76,100,20);
  lib_bitrate_box->setFont(font);
  QLabel *lib_bitrate_label=
    new QLabel(lib_bitrate_box,tr("Default &Bitrate:"),this,
	       "lib_bitrate_label");
  lib_bitrate_label->setGeometry(25,76,120,20);
  lib_bitrate_label->setFont(font);
  lib_bitrate_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(145,108,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(button_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(235,108,80,50);
  cancel_button->setFont(button_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  lib_format_box->insertItem(tr("PCM16"));
#ifdef HAVE_VORBIS
  lib_format_box->insertItem(tr("OggVorbis"));
#endif  // HAVE_VORBIS
  if(mpeg) {
    lib_format_box->insertItem(tr("MPEG Layer 2"));
    lib_format_box->insertItem(tr("MPEG Layer 3"));
  }
  QString str;
  switch(lib_lib->format()) {
      case RDAudioSettings::Pcm16:
	str="PCM16";
	break;

      case RDAudioSettings::OggVorbis:
	str="OggVorbis";
	break;

      case RDAudioSettings::Layer1:
	str="MPEG Layer 1";
	break;

      case RDAudioSettings::Layer2:
	str="MPEG Layer 2";
	break;

      case RDAudioSettings::Layer3:
	str="MPEG Layer 3";
	break;
  }
  for(int i=0;i<lib_format_box->count();i++) {
    if(lib_format_box->text(i)==str) {
      lib_format_box->setCurrentItem(i);
    }
  }
  lib_channels_box->insertItem("1");
  lib_channels_box->insertItem("2");
  lib_channels_box->setCurrentItem(lib_lib->channels()-1);
  lib_samprate_box->insertItem("32000");
  lib_samprate_box->insertItem("44100");
  lib_samprate_box->insertItem("48000");
  switch(lib_lib->sampleRate()) {
      case 32000:
	lib_samprate_box->setCurrentItem(0);
	break;

      case 44100:
	lib_samprate_box->setCurrentItem(1);
	break;

      case 48000:
	lib_samprate_box->setCurrentItem(2);
	break;
  }
  ShowBitRates(lib_lib->format(),lib_lib->bitRate());
}


RDAudioSettingsDialog::~RDAudioSettingsDialog()
{
  delete lib_channels_box;
  delete lib_samprate_box;
  delete lib_bitrate_box;
}


QSize RDAudioSettingsDialog::sizeHint() const
{
  return QSize(325,168);
} 


QSizePolicy RDAudioSettingsDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDAudioSettingsDialog::formatData(int index)
{
  ShowBitRates(ReadFormat(),lib_lib->bitRate());
}


void RDAudioSettingsDialog::okData()
{
  unsigned rate=0;
  lib_lib->setFormat(ReadFormat());
  lib_lib->setChannels(lib_channels_box->currentItem()+1);
  sscanf(lib_samprate_box->currentText(),"%d",&rate);
  lib_lib->setSampleRate(rate);
  rate=0;
  switch(lib_lib->format()) {
      case RDAudioSettings::Layer1:
      case RDAudioSettings::Layer2:
      case RDAudioSettings::Layer3:
	sscanf(lib_bitrate_box->currentText(),"%d",&rate);
	break;

      default:
	break;
  }
  lib_lib->setBitRate(rate*1000);
  done(0);
}


void RDAudioSettingsDialog::cancelData()
{
  done(1);
}


void RDAudioSettingsDialog::ShowBitRates(RDAudioSettings::Format fmt,int rate)
{
  lib_bitrate_box->clear();
  switch(fmt) {
      case RDAudioSettings::Pcm16:
      case RDAudioSettings::OggVorbis:
	lib_bitrate_box->setDisabled(true);
	break;

      case RDAudioSettings::Layer1:
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem("32 kbps");
	lib_bitrate_box->insertItem("64 kbps");
	lib_bitrate_box->insertItem("96 kbps");
	lib_bitrate_box->insertItem("128 kbps");
	lib_bitrate_box->insertItem("160 kbps");
	lib_bitrate_box->insertItem("192 kbps");
	lib_bitrate_box->insertItem("224 kbps");
	lib_bitrate_box->insertItem("256 kbps");
	lib_bitrate_box->insertItem("288 kbps");
	lib_bitrate_box->insertItem("320 kbps");
	lib_bitrate_box->insertItem("352 kbps");
	lib_bitrate_box->insertItem("384 kbps");
	lib_bitrate_box->insertItem("416 kbps");
	lib_bitrate_box->insertItem("448 kbps");
	switch(lib_lib->bitRate()) {
	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(1);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(2);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(3);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(4);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(5);
	      break;

	    case 224000:
	      lib_bitrate_box->setCurrentItem(6);
	      break;

	    case 256000:
	      lib_bitrate_box->setCurrentItem(7);
	      break;

	    case 288000:
	      lib_bitrate_box->setCurrentItem(8);
	      break;

	    case 320000:
	      lib_bitrate_box->setCurrentItem(9);
	      break;

	    case 352000:
	      lib_bitrate_box->setCurrentItem(10);
	      break;

	    case 384000:
	      lib_bitrate_box->setCurrentItem(11);
	      break;

	    case 416000:
	      lib_bitrate_box->setCurrentItem(12);
	      break;

	    case 448000:
	      lib_bitrate_box->setCurrentItem(13);
	      break;
	}
	break;

      case RDAudioSettings::Layer2:
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem("32 kbps");
	lib_bitrate_box->insertItem("48 kbps");
	lib_bitrate_box->insertItem("56 kbps");
	lib_bitrate_box->insertItem("64 kbps");
	lib_bitrate_box->insertItem("80 kbps");
	lib_bitrate_box->insertItem("96 kbps");
	lib_bitrate_box->insertItem("112 kbps");
	lib_bitrate_box->insertItem("128 kbps");
	lib_bitrate_box->insertItem("160 kbps");
	lib_bitrate_box->insertItem("192 kbps");
	lib_bitrate_box->insertItem("224 kbps");
	lib_bitrate_box->insertItem("256 kbps");
	lib_bitrate_box->insertItem("320 kbps");
	lib_bitrate_box->insertItem("384 kbps");
	switch(lib_lib->bitRate()) {
	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      break;

	    case 48000:
	      lib_bitrate_box->setCurrentItem(1);
	      break;

	    case 56000:
	      lib_bitrate_box->setCurrentItem(2);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(3);
	      break;

	    case 80000:
	      lib_bitrate_box->setCurrentItem(4);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(5);
	      break;

	    case 112000:
	      lib_bitrate_box->setCurrentItem(6);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(7);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(8);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(9);
	      break;

	    case 224000:
	      lib_bitrate_box->setCurrentItem(10);
	      break;

	    case 256000:
	      lib_bitrate_box->setCurrentItem(11);
	      break;

	    case 320000:
	      lib_bitrate_box->setCurrentItem(12);
	      break;

	    case 384000:
	      lib_bitrate_box->setCurrentItem(13);
	      break;
	}
	break;

      case RDAudioSettings::Layer3:
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem("32 kbps");
	lib_bitrate_box->insertItem("40 kbps");
	lib_bitrate_box->insertItem("48 kbps");
	lib_bitrate_box->insertItem("56 kbps");
	lib_bitrate_box->insertItem("64 kbps");
	lib_bitrate_box->insertItem("80 kbps");
	lib_bitrate_box->insertItem("96 kbps");
	lib_bitrate_box->insertItem("112 kbps");
	lib_bitrate_box->insertItem("128 kbps");
	lib_bitrate_box->insertItem("160 kbps");
	lib_bitrate_box->insertItem("192 kbps");
	lib_bitrate_box->insertItem("224 kbps");
	lib_bitrate_box->insertItem("256 kbps");
	lib_bitrate_box->insertItem("320 kbps");
	switch(lib_lib->bitRate()) {
	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      break;

	    case 40000:
	      lib_bitrate_box->setCurrentItem(1);
	      break;

	    case 48000:
	      lib_bitrate_box->setCurrentItem(2);
	      break;

	    case 56000:
	      lib_bitrate_box->setCurrentItem(3);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(4);
	      break;

	    case 80000:
	      lib_bitrate_box->setCurrentItem(5);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(6);
	      break;

	    case 112000:
	      lib_bitrate_box->setCurrentItem(7);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(8);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(9);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(10);
	      break;

	    case 224000:
	      lib_bitrate_box->setCurrentItem(11);
	      break;

	    case 256000:
	      lib_bitrate_box->setCurrentItem(12);
	      break;

	    case 320000:
	      lib_bitrate_box->setCurrentItem(13);
	      break;
	}
	break;

  }
}


RDAudioSettings::Format RDAudioSettingsDialog::ReadFormat()
{
  if(lib_format_box->currentText()==tr("PCM16")) {
    return RDAudioSettings::Pcm16;
  }
  if(lib_format_box->currentText()==tr("OggVorbis")) {
    return RDAudioSettings::OggVorbis;
  }
  if(lib_format_box->currentText()==tr("MPEG Layer 2")) {
    return RDAudioSettings::Layer2;
  }
  if(lib_format_box->currentText()==tr("MPEG Layer 3")) {
    return RDAudioSettings::Layer3;
  }
  return RDAudioSettings::Pcm16;
}
