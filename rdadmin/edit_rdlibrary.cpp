// edit_rdlibrary.cpp
//
// Edit an RDLibrary Configuration
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>
#include <QPushButton>

#include <rd.h>
#include <edit_rdlibrary.h>
#include <rdtextvalidator.h>

EditRDLibrary::EditRDLibrary(RDStation *station,RDStation *cae_station,
			     QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  lib_lib=new RDLibraryConf(station->name());

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Dialog Name
  //
  setWindowTitle("RDAdmin - "+tr("Configure RDLibrary"));

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
  label->setGeometry(4*sizeHint().width()/5-120,10,110,19);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  // Settings
  //
  QLabel *setting_label=new QLabel(tr("Settings"),this);
  setting_label->setGeometry(25,80,120,19);
  setting_label->setFont(sectionLabelFont());
  setting_label->setAlignment(Qt::AlignRight);

  //
  // Maximum Record Length
  //
  lib_maxlength_time=new QTimeEdit(this);
  lib_maxlength_time->setDisplayFormat("hh:mm:ss");
  lib_maxlength_time->setGeometry(190,100,85,19);
  QLabel *lib_maxlength_label=new QLabel(tr("Max Record Time:"),this);
  lib_maxlength_label->setFont(labelFont());
  lib_maxlength_label->setGeometry(25,101,160,19);
  lib_maxlength_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // VOX threshold
  //
  lib_vox_spin=new QSpinBox(this);
  lib_vox_spin->setGeometry(190,122,40,19);
  lib_vox_spin->setMinimum(-99);
  lib_vox_spin->setMaximum(0);
  QLabel *lib_vox_spin_label=new QLabel(tr("VOX Threshold:"),this);
  lib_vox_spin_label->setFont(labelFont());
  lib_vox_spin_label->setGeometry(25,122,160,19);
  lib_vox_spin_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QLabel *lib_vox_spin_unit=new QLabel(tr("dbFS"),this);
  lib_vox_spin_unit->setFont(labelFont());
  lib_vox_spin_unit->setGeometry(235,122,120,19);
  lib_vox_spin_unit->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // AutoTrim threshold
  //
  lib_trim_spin=new QSpinBox(this);
  lib_trim_spin->setGeometry(190,144,40,19);
  lib_trim_spin->setMinimum(-99);
  lib_trim_spin->setMaximum(0);
  QLabel *lib_trim_spin_label=new QLabel(tr("AutoTrim Threshold:"),this);
  lib_trim_spin_label->setFont(labelFont());
  lib_trim_spin_label->setGeometry(25,144,160,19);
  lib_trim_spin_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QLabel *lib_trim_spin_unit=new QLabel(tr("dbFS"),this);
  lib_trim_spin_unit->setFont(labelFont());
  lib_trim_spin_unit->setGeometry(235,144,120,19);
  lib_trim_spin_unit->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Tail Preroll
  //
  lib_preroll_spin=new QSpinBox(this);
  lib_preroll_spin->setGeometry(190,166,50,19);
  lib_preroll_spin->setMinimum(0);
  lib_preroll_spin->setMaximum(10000);
  lib_preroll_spin->setSingleStep(100);
  QLabel *lib_preroll_spin_label=new QLabel(tr("Tail Preroll:"),this);
  lib_preroll_spin_label->setFont(labelFont());
  lib_preroll_spin_label->setGeometry(25,166,160,19);
  lib_preroll_spin_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QLabel *lib_preroll_spin_unit=new QLabel(tr("milliseconds"),this);
  lib_preroll_spin_unit->setFont(labelFont());
  lib_preroll_spin_unit->setGeometry(245,166,120,19);
  lib_preroll_spin_unit->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Ripper Device
  //
  lib_ripdev_edit=new QLineEdit(this);
  lib_ripdev_edit->setGeometry(190,188,100,19);
  lib_ripdev_edit->setValidator(validator);
  QLabel *lib_ripdev_label=new QLabel(tr("Ripper Device:"),this);
  lib_ripdev_label->setFont(labelFont());
  lib_ripdev_label->setGeometry(25,188,160,19);
  lib_ripdev_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Paranoia Level
  //
  lib_paranoia_box=new QComboBox(this);
  lib_paranoia_box->setGeometry(190,210,100,19);
  QLabel *lib_paranoia_label=new QLabel(tr("Paranoia Level:"),this);
  lib_paranoia_label->setFont(labelFont());
  lib_paranoia_label->setGeometry(25,210,160,19);
  lib_paranoia_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Read ISRC
  //
  lib_isrc_box=new QComboBox(this);
  lib_isrc_box->setGeometry(190,232,60,19);
  QLabel *lib_isrc_label=new QLabel(tr("Read ISRCs from CD:"),this);
  lib_isrc_label->setFont(labelFont());
  lib_isrc_label->setGeometry(25,232,160,19);
  lib_isrc_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // CD Server Type
  //
  lib_cd_server_type_box=new QComboBox(this);
  lib_cd_server_type_box->setGeometry(190,256,120,19);
  for(int i=0;i<RDLibraryConf::LastType;i++) {
    lib_cd_server_type_box->
      insertItem(lib_cd_server_type_box->count(),
	     RDLibraryConf::cdServerTypeText((RDLibraryConf::CdServerType)i));
  }
  connect(lib_cd_server_type_box,SIGNAL(activated(int)),
	  this,SLOT(cdServerTypeData(int)));
  lib_cd_server_label=
    new QLabel(tr("CD Metadata Source:"),this);
  lib_cd_server_label->setFont(labelFont());
  lib_cd_server_label->setGeometry(25,256,160,19);
  lib_cd_server_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // CD Server
  //
  lib_cddb_server_edit=new QLineEdit(this);
  lib_cddb_server_edit->setGeometry(190,278,160,19);
  lib_cddb_server_edit->setValidator(validator);
  lib_mb_server_edit=new QLineEdit(this);
  lib_mb_server_edit->setGeometry(190,278,160,19);
  lib_mb_server_edit->setValidator(validator);
  lib_cd_server_label=new QLabel(tr("FreeDB Server:"),this);
  lib_cd_server_label->setFont(labelFont());
  lib_cd_server_label->setGeometry(25,278,160,19);
  lib_cd_server_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Format
  //
  lib_format_box=new QComboBox(this);
  lib_format_box->setGeometry(190,302,150,19);
  connect(lib_format_box,SIGNAL(activated(int)),this,SLOT(formatData(int)));
  QLabel *lib_format_label=new QLabel(tr("Format:"),this);
  lib_format_label->setFont(labelFont());
  lib_format_label->setGeometry(25,302,160,19);
  lib_format_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Bitrate
  //
  lib_bitrate_box=new QComboBox(this);
  lib_bitrate_box->setGeometry(190,326,50,19);
  QLabel *lib_bitrate_label=new QLabel(tr("Bitrate:"),this);
  lib_bitrate_label->setFont(labelFont());
  lib_bitrate_label->setGeometry(25,326,160,19);
  lib_bitrate_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QLabel *lib_bitrate_unit=new QLabel("kbps/chan",this);
  lib_bitrate_unit->setFont(labelFont());
  lib_bitrate_unit->setGeometry(245,326,160,19);
  lib_bitrate_unit->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Enable Editor
  //
  lib_editor_box=new QComboBox(this);
  lib_editor_box->setGeometry(190,350,60,19);
  lib_editor_box->insertItem(0,tr("No"));
  lib_editor_box->insertItem(1,tr("Yes"));
  QLabel *lib_editor_label=new QLabel(tr("Allow External Editing:"),this);
  lib_editor_label->setFont(labelFont());
  lib_editor_label->setGeometry(25,350,160,19);
  lib_editor_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Sample Rate Converter
  //
  lib_converter_box=new QComboBox(this);
  lib_converter_box->setGeometry(190,374,sizeHint().width()-200,19);
  int conv=0;
  while(src_get_name(conv)!=NULL) {
    lib_converter_box->
      insertItem(lib_converter_box->count(),src_get_name(conv++));
  }
  QLabel *lib_converter_label=new QLabel(tr("Sample Rate Converter:"),this);
  lib_converter_label->setFont(labelFont());
  lib_converter_label->setGeometry(10,374,175,19);
  lib_converter_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Limit Searches at Startup
  //
  lib_limit_search_box=new QComboBox(this);
  lib_limit_search_box->setGeometry(190,398,80,19);
  lib_limit_search_box->insertItem(0,tr("No"));
  lib_limit_search_box->insertItem(1,tr("Yes"));
  lib_limit_search_box->insertItem(2,tr("Previous"));
  QLabel *lib_limit_search_label=
    new QLabel(tr("Limit Searches at Startup")+":",this);
  lib_limit_search_label->setFont(labelFont());
  lib_limit_search_label->setGeometry(10,398,175,19);
  lib_limit_search_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Defaults
  //
  QLabel *default_label=new QLabel(tr("Defaults"),this);
  default_label->setGeometry(25,436,120,19);
  default_label->setFont(sectionLabelFont());
  default_label->setAlignment(Qt::AlignRight);

  //
  // Default Channels
  //
  lib_channels_box=new QComboBox(this);
  lib_channels_box->setGeometry(190,454,60,19);
  QLabel *lib_channels_label=new QLabel(tr("Channels:"),this);
  lib_channels_label->setFont(labelFont());
  lib_channels_label->setGeometry(25,454,160,19);
  lib_channels_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Default Record Mode
  //
  lib_recmode_box=new QComboBox(this);
  lib_recmode_box->setGeometry(190,478,100,19);
  QLabel *lib_recmode_label=new QLabel(tr("Record Mode:"),this);
  lib_recmode_label->setFont(labelFont());
  lib_recmode_label->setGeometry(25,478,160,19);
  lib_recmode_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Default Trim State
  //
  lib_trimstate_box=new QComboBox(this);
  lib_trimstate_box->setGeometry(190,502,100,19);
  QLabel *lib_trimstate_label=new QLabel(tr("AutoTrim:"),this);
  lib_trimstate_label->setFont(labelFont());
  lib_trimstate_label->setGeometry(25,502,160,19);
  lib_trimstate_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Normalization Level
  //
  lib_riplevel_spin=new QSpinBox(this);
  lib_riplevel_spin->setGeometry(190,526,40,19);
  lib_riplevel_spin->setMinimum(-99);
  lib_riplevel_spin->setMaximum(0);
  QLabel *lib_riplevel_spin_label=new QLabel(tr("Normalization Level:"),this);
  lib_riplevel_spin_label->setFont(labelFont());
  lib_riplevel_spin_label->setGeometry(25,526,160,19);
  lib_riplevel_spin_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QLabel *lib_riplevel_spin_unit=new QLabel(tr("dbFS"),this);
  lib_riplevel_spin_unit->setFont(labelFont());
  lib_riplevel_spin_unit->setGeometry(235,526,120,19);
  lib_riplevel_spin_unit->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

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
  lib_vox_spin->setValue(lib_lib->voxThreshold()/100);
  lib_trim_spin->setValue(lib_lib->trimThreshold()/100);
  lib_preroll_spin->setValue(lib_lib->tailPreroll());
  lib_ripdev_edit->setText(lib_lib->ripperDevice());
  lib_paranoia_box->insertItem(0,tr("Normal"));
  lib_paranoia_box->insertItem(1,tr("Low"));
  lib_paranoia_box->insertItem(2,tr("None"));
  lib_paranoia_box->setCurrentIndex(lib_lib->paranoiaLevel());
  lib_isrc_box->insertItem(0,tr("No"));
  lib_isrc_box->insertItem(1,tr("Yes"));
  lib_isrc_box->setCurrentIndex(lib_lib->readIsrc());
  lib_riplevel_spin->setValue(lib_lib->ripperLevel()/100);
  lib_channels_box->insertItem(0,"1");
  lib_channels_box->insertItem(1,"2");
  lib_channels_box->setCurrentIndex(lib_lib->defaultChannels()-1);
  lib_format_box->insertItem(0,tr("PCM16"));
  lib_format_box->insertItem(1,tr("PCM24"));
  lib_format_box->insertItem(2,tr("MPEG Layer 2"));
  switch(lib_lib->defaultFormat()) {
  case 0:  // PCM16
    lib_format_box->setCurrentIndex(0);
    ShowBitRates(0,lib_lib->defaultBitrate());
    break;

  case 1:  // MPEG L2
    lib_format_box->setCurrentIndex(2);
    ShowBitRates(2,lib_lib->defaultBitrate());
    break;

  case 2:  // PCM24
    lib_format_box->setCurrentIndex(1);
    ShowBitRates(1,lib_lib->defaultBitrate());
    break;
  }
  lib_recmode_box->insertItem(0,tr("Manual"));
  lib_recmode_box->insertItem(1,tr("VOX"));
  switch(lib_lib->defaultRecordMode()) {
      case RDLibraryConf::Manual:
	lib_recmode_box->setCurrentIndex(0);
	break;

      case RDLibraryConf::Vox:
	lib_recmode_box->setCurrentIndex(1);
	break;
  }
  lib_trimstate_box->insertItem(0,"On");
  lib_trimstate_box->insertItem(1,"Off");
  if(lib_lib->defaultTrimState()) {
    lib_trimstate_box->setCurrentIndex(0);
  }
  else {
    lib_trimstate_box->setCurrentIndex(1);
  }
  lib_cd_server_type_box->setCurrentIndex((int)lib_lib->cdServerType());
  lib_cddb_server_edit->setText(lib_lib->cddbServer());
  lib_mb_server_edit->setText(lib_lib->mbServer());
  cdServerTypeData(lib_cd_server_type_box->currentIndex());
  lib_editor_box->setCurrentIndex(lib_lib->enableEditor());
  lib_converter_box->setCurrentIndex(lib_lib->srcConverter());
  lib_limit_search_box->setCurrentIndex((int)lib_lib->limitSearch());
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
  return QSize(405,630);
} 


QSizePolicy EditRDLibrary::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRDLibrary::formatData(int index)
{
  ShowBitRates(index,lib_lib->defaultBitrate());
}


void EditRDLibrary::cdServerTypeData(int n)
{
  switch((RDLibraryConf::CdServerType)n) {
  case RDLibraryConf::DummyType:
    lib_cd_server_label->setDisabled(true);
    lib_cddb_server_edit->setDisabled(true);
    lib_mb_server_edit->setDisabled(true);
    break;

  case RDLibraryConf::CddbType:
    lib_cd_server_label->setEnabled(true);
    lib_cd_server_label->setText(tr("FreeDB Server")+":");
    lib_cddb_server_edit->setEnabled(true);
    lib_cddb_server_edit->show();
    lib_mb_server_edit->hide();
    break;

  case RDLibraryConf::MusicBrainzType:
    lib_cd_server_label->setEnabled(true);
    lib_cd_server_label->setText(tr("MusicBrainz Server")+":");
    lib_mb_server_edit->setEnabled(true);
    lib_cddb_server_edit->hide();
    lib_mb_server_edit->show();
    break;

  case RDLibraryConf::LastType:
    break;
  }
}


void EditRDLibrary::okData()
{
  unsigned rate=0;

  lib_lib->setInputCard(lib_input_card->card());
  lib_lib->setInputPort(lib_input_card->port());
  lib_lib->setOutputCard(lib_output_card->card());
  lib_lib->setOutputPort(lib_output_card->port());
  lib_lib->setMaxLength(QTime(0,0,0).msecsTo(lib_maxlength_time->time()));
  lib_lib->setVoxThreshold(100*lib_vox_spin->value());
  lib_lib->setTrimThreshold(100*lib_trim_spin->value());
  lib_lib->setTailPreroll(lib_preroll_spin->value());
  lib_lib->setRipperDevice(lib_ripdev_edit->text());
  lib_lib->setParanoiaLevel(lib_paranoia_box->currentIndex());
  lib_lib->setReadIsrc(lib_isrc_box->currentIndex());
  lib_lib->setRipperLevel(lib_riplevel_spin->value()*100);
  switch(lib_format_box->currentIndex()) {
  case 0:  // PCM16
    lib_lib->setDefaultFormat(0);
    break;

  case 1:  // PCM24
    lib_lib->setDefaultFormat(2);
    break;

  case 2:  // MPEG L2
    lib_lib->setDefaultFormat(1);
    break;
  }
  lib_lib->setDefaultChannels(lib_channels_box->currentIndex()+1);
  rate=0;
  if(lib_format_box->currentIndex()==2) {
    rate=lib_bitrate_box->currentText().toInt();
  }
  lib_lib->setDefaultBitrate(rate*1000);
  switch(lib_recmode_box->currentIndex()) {
      case 0:
	lib_lib->setDefaultRecordMode(RDLibraryConf::Manual);
	break;

      case 1:
	lib_lib->setDefaultRecordMode(RDLibraryConf::Vox);
	break;
  }
  switch(lib_trimstate_box->currentIndex()) {
      case 0:
	lib_lib->setDefaultTrimState(true);
	break;

      case 1:
	lib_lib->setDefaultTrimState(false);
	break;
  }
  lib_lib->setCdServerType((RDLibraryConf::CdServerType)lib_cd_server_type_box->
			   currentIndex());
  lib_lib->setCddbServer(lib_cddb_server_edit->text());
  lib_lib->setMbServer(lib_mb_server_edit->text());
  lib_lib->setEnableEditor(lib_editor_box->currentIndex());
  lib_lib->setSrcConverter(lib_converter_box->currentIndex());
  lib_lib->setLimitSearch((RDLibraryConf::SearchLimit)
			  lib_limit_search_box->currentIndex());
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
  case 1:  // PCM24
    lib_bitrate_box->setDisabled(true);
    break;

  case 2:  // MPEG-1 Layer 2
    lib_bitrate_box->setEnabled(true);
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
    switch(lib_lib->defaultBitrate()) {
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
