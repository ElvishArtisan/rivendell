// edit_rdlibrary.cpp
//
// Edit an RDLibrary Configuration
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_rdlibrary.cpp,v 1.33.6.1 2014/01/09 01:03:55 cvs Exp $
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

#include <samplerate.h>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsqldatabase.h>

#include <rd.h>
#include <edit_rdlibrary.h>
#include <rdtextvalidator.h>


EditRDLibrary::EditRDLibrary(RDStation *station,RDStation *cae_station,
			     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  lib_lib=new RDLibraryConf(station->name(),0);

  //
  // Create Fonts
  //
  QFont small_font=QFont("Helvetica",12,QFont::Bold);
  small_font.setPixelSize(12);
  QFont big_font=QFont("Helvetica",14,QFont::Bold);
  big_font.setPixelSize(14);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Dialog Name
  //
  setCaption(tr("RDLibrary config for ")+station->name());

  //
  // Input Configuration
  //
  lib_input_card=new RDCardSelector(this,"lib_input_card");
  lib_input_card->setGeometry(10,29,120,117);
  QLabel *label=new QLabel(lib_input_card,tr("INPUT"),this,"lib_input_label");
  label->setGeometry(10,10,110,19);
  label->setFont(big_font);
  label->setAlignment(AlignCenter);

  //
  // Output Configuration
  //
  lib_output_card=new RDCardSelector(this,"lib_output_card");
  lib_output_card->setGeometry(170,29,120,87);
  label=new QLabel(lib_output_card,tr("OUTPUT"),this,"lib_output_label");
  label->setGeometry(170,10,110,19);
  label->setFont(big_font);
  label->setAlignment(AlignCenter);

  //
  // Settings
  //
  QLabel *setting_label=new QLabel(tr("Settings"),this,"setting_label");
  setting_label->setGeometry(25,79,120,19);
  setting_label->setFont(big_font);
  setting_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Maximum Record Length
  //
  lib_maxlength_time=new QTimeEdit(this,"lib_maxlength_time");
  lib_maxlength_time->setGeometry(160,100,85,19);
  QLabel *lib_maxlength_label=new QLabel(lib_maxlength_time,
					 tr("&Max Record Time:"),this,
					 "lib_maxlength_label");
  lib_maxlength_label->setGeometry(25,101,130,19);
  lib_maxlength_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // VOX threshold
  //
  lib_vox_spin=new QSpinBox(this,"lib_vox_spin");
  lib_vox_spin->setGeometry(160,122,40,19);
  lib_vox_spin->setMinValue(-99);
  lib_vox_spin->setMaxValue(0);
  QLabel *lib_vox_spin_label=new QLabel(lib_vox_spin,tr("&VOX Threshold:"),
					this,"lib_vox_spin_label");
  lib_vox_spin_label->setGeometry(25,122,130,19);
  lib_vox_spin_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QLabel *lib_vox_spin_unit=new QLabel(tr("dbFS"),this,"lib_vox_spin_unit");
  lib_vox_spin_unit->setGeometry(205,122,120,19);
  lib_vox_spin_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // AutoTrim threshold
  //
  lib_trim_spin=new QSpinBox(this,"lib_trim_spin");
  lib_trim_spin->setGeometry(160,144,40,19);
  lib_trim_spin->setMinValue(-99);
  lib_trim_spin->setMaxValue(0);
  QLabel *lib_trim_spin_label=
    new QLabel(lib_trim_spin,tr("&AutoTrim Threshold:"),
	       this,"lib_trim_spin_label");
  lib_trim_spin_label->setGeometry(25,144,130,19);
  lib_trim_spin_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QLabel *lib_trim_spin_unit=new QLabel(tr("dbFS"),this,"lib_trim_spin_unit");
  lib_trim_spin_unit->setGeometry(205,144,120,19);
  lib_trim_spin_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Tail Preroll
  //
  lib_preroll_spin=new QSpinBox(this,"lib_preroll_spin");
  lib_preroll_spin->setGeometry(160,166,50,19);
  lib_preroll_spin->setMinValue(0);
  lib_preroll_spin->setMaxValue(10000);
  lib_preroll_spin->setLineStep(100);
  QLabel *lib_preroll_spin_label=new QLabel(lib_preroll_spin,
					    tr("&Tail Preroll:"),this,
					    "lib_preroll_spin_label");
  lib_preroll_spin_label->setGeometry(25,166,130,19);
  lib_preroll_spin_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QLabel *lib_preroll_spin_unit=new QLabel(tr("milliseconds"),this,
					   "lib_preroll_spin_unit");
  lib_preroll_spin_unit->setGeometry(215,166,120,19);
  lib_preroll_spin_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Ripper Device
  //
  lib_ripdev_edit=new QLineEdit(this,"lib_ripper_device");
  lib_ripdev_edit->setGeometry(160,188,100,19);
  lib_ripdev_edit->setValidator(validator);
  QLabel *lib_ripdev_label=new QLabel(lib_ripdev_edit,tr("&Ripper Device:"),
				      this,"lib_format_label");
  lib_ripdev_label->setGeometry(25,188,130,19);
  lib_ripdev_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Paranoia Level
  //
  lib_paranoia_box=new QComboBox(this,"lib_paranoia_box");
  lib_paranoia_box->setGeometry(160,210,100,19);
  QLabel *lib_paranoia_label=
    new QLabel(lib_paranoia_box,tr("&Paranoia Level:"),
	       this,"lib_paranoia_label");
  lib_paranoia_label->setGeometry(25,210,130,19);
  lib_paranoia_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Ripper Level
  //
  lib_riplevel_spin=new QSpinBox(this,"lib_riplevel_spin");
  lib_riplevel_spin->setGeometry(160,232,40,19);
  lib_riplevel_spin->setMinValue(-99);
  lib_riplevel_spin->setMaxValue(0);
  QLabel *lib_riplevel_spin_label=new QLabel(lib_riplevel_spin,
					     tr("Ripper Level:"),this,
					     "lib_riplevel_spin_label");
  lib_riplevel_spin_label->setGeometry(25,232,130,19);
  lib_riplevel_spin_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QLabel *lib_riplevel_spin_unit=new QLabel(tr("dbFS"),
					    this,"lib_riplevel_spin_unit");
  lib_riplevel_spin_unit->setGeometry(205,232,120,19);
  lib_riplevel_spin_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // FreeDB Server
  //
  lib_cddb_edit=new QLineEdit(this,"lib_cddb_edit");
  lib_cddb_edit->setGeometry(160,256,160,19);
  lib_cddb_edit->setValidator(validator);
  QLabel *lib_cddb_label=new QLabel(lib_cddb_edit,tr("&FreeDB Server:"),this,
				    "lib_cddb_label");
  lib_cddb_label->setGeometry(25,256,130,19);
  lib_cddb_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Format
  //
  lib_format_box=new QComboBox(this,"lib_format_box");
  lib_format_box->setGeometry(160,280,150,19);
  connect(lib_format_box,SIGNAL(activated(int)),this,SLOT(formatData(int)));
  QLabel *lib_format_label=new QLabel(lib_format_box,tr("&Format:"),this,
				      "lib_format_label");
  lib_format_label->setGeometry(25,280,130,19);
  lib_format_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Bitrate
  //
  lib_bitrate_box=new QComboBox(this,"lib_bitrate_box");
  lib_bitrate_box->setGeometry(160,304,130,19);
  QLabel *lib_bitrate_label=new QLabel(lib_bitrate_box,tr("&Bitrate:"),this,
				       "lib_bitrate_label");
  lib_bitrate_label->setGeometry(25,304,130,19);
  lib_bitrate_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Enable Editor
  //
  lib_editor_box=new QComboBox(this,"lib_editor_box");
  lib_editor_box->setGeometry(160,328,60,19);
  lib_editor_box->insertItem(tr("No"));
  lib_editor_box->insertItem(tr("Yes"));
  QLabel *lib_editor_label=new QLabel(lib_editor_box,
				      tr("Allow E&xternal Editing:"),this,
				      "lib_editor_label");
  lib_editor_label->setGeometry(25,328,130,19);
  lib_editor_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Sample Rate Converter
  //
  lib_converter_box=new QComboBox(this,"lib_converter_box");
  lib_converter_box->setGeometry(160,352,sizeHint().width()-170,19);
  int conv=0;
  while(src_get_name(conv)!=NULL) {
    lib_converter_box->insertItem(src_get_name(conv++));
  }
  QLabel *lib_converter_label=new QLabel(lib_converter_box,
					 tr("Sample Rate Converter:"),this,
					 "lib_converter_label");
  lib_converter_label->setGeometry(10,352,145,19);
  lib_converter_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Limit Searches at Startup
  //
  lib_limit_search_box=new QComboBox(this);
  lib_limit_search_box->setGeometry(160,376,80,19);
  lib_limit_search_box->insertItem(tr("No"));
  lib_limit_search_box->insertItem(tr("Yes"));
  lib_limit_search_box->insertItem(tr("Previous"));
  QLabel *lib_limit_search_label=
    new QLabel(lib_limit_search_box,tr("Limit Searches at Startup")+":",this);
  lib_limit_search_label->setGeometry(10,376,145,19);
  lib_limit_search_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Defaults
  //
  QLabel *default_label=new QLabel(tr("Defaults"),this,"default_label");
  default_label->setGeometry(25,415,120,19);
  default_label->setFont(big_font);
  default_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Default Channels
  //
  lib_channels_box=new QComboBox(this,"lib_channels_box");
  lib_channels_box->setGeometry(160,434,60,19);
  QLabel *lib_channels_label=new QLabel(lib_channels_box,tr("&Channels:"),this,
				       "lib_channels_label");
  lib_channels_label->setGeometry(25,434,130,19);
  lib_channels_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Record Mode
  //
  lib_recmode_box=new QComboBox(this,"lib_recmode_box");
  lib_recmode_box->setGeometry(160,456,100,19);
  QLabel *lib_recmode_label=new QLabel(lib_recmode_box,tr("Record Mode:"),this,
				       "lib_recmode_label");
  lib_recmode_label->setGeometry(25,456,130,19);
  lib_recmode_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Trim State
  //
  lib_trimstate_box=new QComboBox(this,"lib_trimstate_box");
  lib_trimstate_box->setGeometry(160,480,100,19);
  QLabel *lib_trimstate_label=new QLabel(lib_trimstate_box,tr("AutoTrim:"),
					 this,"lib_trimstate_label");
  lib_trimstate_label->setGeometry(25,480,130,19);
  lib_trimstate_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(small_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(small_font);
  cancel_button->setText(tr("&Cancel"));
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
  lib_maxlength_time->setTime(QTime().addMSecs(lib_lib->maxLength()));
  lib_vox_spin->setValue(lib_lib->voxThreshold()/100);
  lib_trim_spin->setValue(lib_lib->trimThreshold()/100);
  lib_preroll_spin->setValue(lib_lib->tailPreroll());
  lib_ripdev_edit->setText(lib_lib->ripperDevice());
  lib_paranoia_box->insertItem(tr("Normal"));
  lib_paranoia_box->insertItem(tr("Low"));
  lib_paranoia_box->insertItem(tr("None"));
  lib_paranoia_box->setCurrentItem(lib_lib->paranoiaLevel());
  lib_riplevel_spin->setValue(lib_lib->ripperLevel()/100);
  lib_format_box->insertItem(tr("PCM16"));
  lib_format_box->insertItem(tr("MPEG Layer 2"));
  lib_format_box->setCurrentItem(lib_lib->defaultFormat());
  lib_channels_box->insertItem("1");
  lib_channels_box->insertItem("2");
  lib_channels_box->setCurrentItem(lib_lib->defaultChannels()-1);
  ShowBitRates(lib_lib->defaultFormat(),lib_lib->defaultBitrate());
  lib_recmode_box->insertItem(tr("Manual"));
  lib_recmode_box->insertItem(tr("VOX"));
  switch(lib_lib->defaultRecordMode()) {
      case RDLibraryConf::Manual:
	lib_recmode_box->setCurrentItem(0);
	break;

      case RDLibraryConf::Vox:
	lib_recmode_box->setCurrentItem(1);
	break;
  }
  lib_trimstate_box->insertItem("On");
  lib_trimstate_box->insertItem("Off");
  if(lib_lib->defaultTrimState()) {
    lib_trimstate_box->setCurrentItem(0);
  }
  else {
    lib_trimstate_box->setCurrentItem(1);
  }
  lib_cddb_edit->setText(lib_lib->cddbServer());
  lib_editor_box->setCurrentItem(lib_lib->enableEditor());
  lib_converter_box->setCurrentItem(lib_lib->srcConverter());
  lib_limit_search_box->setCurrentItem((int)lib_lib->limitSearch());
}


EditRDLibrary::~EditRDLibrary()
{
  delete lib_input_card;
  delete lib_output_card;
  delete lib_format_box;
  delete lib_channels_box;
  delete lib_bitrate_box;
  delete lib_maxlength_time;
}


QSize EditRDLibrary::sizeHint() const
{
  return QSize(375,584);
} 


QSizePolicy EditRDLibrary::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRDLibrary::formatData(int index)
{
  ShowBitRates(index,lib_lib->defaultBitrate());
}


void EditRDLibrary::okData()
{
  unsigned rate=0;

  lib_lib->setInputCard(lib_input_card->card());
  lib_lib->setInputPort(lib_input_card->port());
  lib_lib->setOutputCard(lib_output_card->card());
  lib_lib->setOutputPort(lib_output_card->port());
  lib_lib->setMaxLength(QTime().msecsTo(lib_maxlength_time->time()));
  lib_lib->setVoxThreshold(100*lib_vox_spin->value());
  lib_lib->setTrimThreshold(100*lib_trim_spin->value());
  lib_lib->setTailPreroll(lib_preroll_spin->value());
  lib_lib->setRipperDevice(lib_ripdev_edit->text());
  lib_lib->setParanoiaLevel(lib_paranoia_box->currentItem());
  lib_lib->setRipperLevel(lib_riplevel_spin->value()*100);
  lib_lib->setDefaultFormat(lib_format_box->currentItem());
  lib_lib->setDefaultChannels(lib_channels_box->currentItem()+1);
  rate=0;
  if(lib_format_box->currentItem()!=0) {
    sscanf(lib_bitrate_box->currentText(),"%d",&rate);
  }
  lib_lib->setDefaultBitrate(rate*1000);
  switch(lib_recmode_box->currentItem()) {
      case 0:
	lib_lib->setDefaultRecordMode(RDLibraryConf::Manual);
	break;

      case 1:
	lib_lib->setDefaultRecordMode(RDLibraryConf::Vox);
	break;
  }
  switch(lib_trimstate_box->currentItem()) {
      case 0:
	lib_lib->setDefaultTrimState(true);
	break;

      case 1:
	lib_lib->setDefaultTrimState(false);
	break;
  }
  lib_lib->setCddbServer(lib_cddb_edit->text());
  lib_lib->setEnableEditor(lib_editor_box->currentItem());
  lib_lib->setSrcConverter(lib_converter_box->currentItem());
  lib_lib->setLimitSearch((RDLibraryConf::SearchLimit)
			  lib_limit_search_box->currentItem());
  done(0);
}


void EditRDLibrary::cancelData()
{
  done(1);
}


void EditRDLibrary::ShowBitRates(int layer,int rate)
{
  lib_bitrate_box->clear();
  switch(layer) {
      case 0:  // PCM16
	lib_bitrate_box->setDisabled(true);
	break;

      case 1:  // MPEG-1 Layer 2
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem(tr("32 kbps/chan"));
	lib_bitrate_box->insertItem(tr("48 kbps/chan"));
	lib_bitrate_box->insertItem(tr("56 kbps/chan"));
	lib_bitrate_box->insertItem(tr("64 kbps/chan"));
	lib_bitrate_box->insertItem(tr("80 kbps/chan"));
	lib_bitrate_box->insertItem(tr("96 kbps/chan"));
	lib_bitrate_box->insertItem(tr("112 kbps/chan"));
	lib_bitrate_box->insertItem(tr("128 kbps/chan"));
	lib_bitrate_box->insertItem(tr("160 kbps/chan"));
	lib_bitrate_box->insertItem(tr("192 kbps/chan"));
	switch(lib_lib->defaultBitrate()) {
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

	    default:
	      lib_bitrate_box->setCurrentItem(7);   // 128 kbps/chan
	      break;
	}
	break;

      case 2:  // MPEG-1 Layer 3
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem(tr("32 kbps/chan"));
	lib_bitrate_box->insertItem(tr("40 kbps/chan"));
	lib_bitrate_box->insertItem(tr("48 kbps/chan"));
	lib_bitrate_box->insertItem(tr("56 kbps/chan"));
	lib_bitrate_box->insertItem(tr("64 kbps/chan"));
	lib_bitrate_box->insertItem(tr("80 kbps/chan"));
	lib_bitrate_box->insertItem(tr("96 kbps/chan"));
	lib_bitrate_box->insertItem(tr("112 kbps/chan"));
	lib_bitrate_box->insertItem(tr("128 kbps/chan"));
	lib_bitrate_box->insertItem(tr("160 kbps/chan"));
	lib_bitrate_box->insertItem(tr("192 kbps/chan"));
	lib_bitrate_box->insertItem(tr("224 kbps/chan"));
	lib_bitrate_box->insertItem(tr("256 kbps/chan"));
	lib_bitrate_box->insertItem(tr("320 kbps/chan"));
	switch(lib_lib->defaultLayer()) {
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
