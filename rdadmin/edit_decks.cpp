// edit_decks.cpp
//
// Edit a Rivendell RDCatch Deck Configuration
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_decks.cpp,v 1.35.2.1 2012/11/28 18:49:36 cvs Exp $
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

#include <qstring.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rd.h>
#include <rddb.h>
#include <edit_decks.h>


EditDecks::EditDecks(RDStation *station,RDStation *cae_station,
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

  edit_station=station;

  setCaption(tr("Configure RDCatch"));

  //
  // Create Fonts
  //
  QFont small_font=QFont("Helvetica",12,QFont::Bold);
  small_font.setPixelSize(12);
  QFont big_font=QFont("Helvetica",14,QFont::Bold);
  big_font.setPixelSize(14);

  //
  // Create RDCatchConf
  //
  edit_catch_conf=new RDCatchConf(station->name());

  //
  // Record Deck Selector
  //
  edit_record_deck_box=new QComboBox(this,"edit_record_deck_box");
  edit_record_deck_box->setGeometry(140,10,60,24);
  edit_record_deck_box->setInsertionPolicy(QComboBox::NoInsertion);
  connect(edit_record_deck_box,SIGNAL(activated(int)),
	  this,SLOT(recordDeckActivatedData(int)));
  QLabel *label=new QLabel(edit_record_deck_box,tr("Record Deck"),this,
			   "edit_deck_label");
  label->setFont(small_font);
  label->setGeometry(35,14,100,22);
  label->setAlignment(AlignRight);

  //
  // Settings Label
  //
  label=new QLabel(tr("Settings"),this,"settings_label");
  label->setGeometry(10,40,100,24);
  label->setFont(big_font);

  //
  // Card Selector
  //
  edit_record_selector=new RDCardSelector(this,"edit_record_selector");
  edit_record_selector->setGeometry(67,68,120,65);
  connect(edit_record_selector,SIGNAL(cardChanged(int)),
	  this,SLOT(recordCardChangedData(int)));

  //
  // Monitor Port Selector
  //
  edit_monitor_box=new QSpinBox(this,"edit_monitor_box");
  edit_monitor_box->setGeometry(127,112,50,19);
  edit_monitor_box->setRange(-1,RD_MAX_PORTS-1);
  edit_monitor_box->setSpecialValueText(tr("None"));
  connect(edit_monitor_box,SIGNAL(valueChanged(int)),
	  this,SLOT(monitorPortChangedData(int)));
  edit_monitor_label=new QLabel(edit_monitor_box,tr("Monitor Port:"),this,
			     "edit_monitor_label");
  edit_monitor_label->setGeometry(10,112,112,19);
  edit_monitor_label->setAlignment(AlignRight|AlignVCenter);

  edit_default_on_box=new QComboBox(this,"edit_default_on_box");
  edit_default_on_box->setGeometry(305,112,60,19);
  edit_default_on_box->insertItem(tr("Off"));
  edit_default_on_box->insertItem(tr("On"));
  edit_default_on_label=new QLabel(edit_default_on_box,
				   tr("Monitor defaults to"),
				   this,"edit_default_on_label");
  edit_default_on_label->setGeometry(195,112,105,19);
  edit_default_on_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Format
  //
  edit_format_box=new QComboBox(this,"edit_format_box");
  edit_format_box->setGeometry(125,136,150,24);
  edit_format_box->setInsertionPolicy(QComboBox::NoInsertion);
  connect(edit_format_box,SIGNAL(activated(int)),
	  this,SLOT(formatActivatedData(int)));
  label=new QLabel(edit_format_box,tr("Format:"),this,
			     "edit_format_label");
  label->setGeometry(10,136,110,24);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Bit Rate
  //
  edit_bitrate_box=new QComboBox(this,"edit_bitrate_box");
  edit_bitrate_box->setGeometry(125,160,140,24);
  edit_bitrate_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_bitrate_label=new QLabel(edit_bitrate_box,tr("Bit Rate:"),this,
			     "edit_bitrate_label");
  edit_bitrate_label->setGeometry(10,160,110,24);
  edit_bitrate_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Switcher Station
  //
  edit_swstation_box=new QComboBox(this,"edit_swstation_box");
  edit_swstation_box->setGeometry(125,190,250,24);
  edit_swstation_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_swstation_label=new QLabel(edit_swstation_box,tr("Switcher Host:"),this,
			     "edit_swstation_label");
  edit_swstation_label->setGeometry(10,190,110,24);
  edit_swstation_label->setAlignment(AlignRight|AlignVCenter);
  connect(edit_swstation_box,SIGNAL(activated(const QString &)),
	  this,SLOT(stationActivatedData(const QString &)));

  //
  // Switcher Matrix
  //
  edit_swmatrix_box=new QComboBox(this,"edit_swmatrix_box");
  edit_swmatrix_box->setGeometry(125,214,250,24);
  edit_swmatrix_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_swmatrix_box->setDisabled(true);
  edit_swmatrix_label=new QLabel(edit_swmatrix_box,tr("Switcher Matrix:"),this,
			     "edit_swmatrix_label");
  edit_swmatrix_label->setGeometry(10,214,110,24);
  edit_swmatrix_label->setAlignment(AlignRight|AlignVCenter);
  edit_swmatrix_label->setDisabled(true);
  connect(edit_swmatrix_box,SIGNAL(activated(const QString &)),
	  this,SLOT(matrixActivatedData(const QString &)));

  //
  // Switcher Output
  //
  edit_swoutput_box=new QComboBox(this,"edit_swoutput_box");
  edit_swoutput_box->setGeometry(125,238,250,24);
  edit_swoutput_box->setInsertionPolicy(QComboBox::NoInsertion);
  edit_swoutput_box->setDisabled(true);
  edit_swoutput_label=new QLabel(edit_swoutput_box,tr("Switcher Output:"),this,
			     "edit_swoutput_label");
  edit_swoutput_label->setGeometry(10,238,110,24);
  edit_swoutput_label->setAlignment(AlignRight|AlignVCenter);
  edit_swoutput_label->setDisabled(true);

  //
  // Switcher Delay
  //
  edit_swdelay_box=new QSpinBox(this,"edit_swdelay_box");
  edit_swdelay_box->setGeometry(125,262,40,24);
  edit_swdelay_box->setRange(0,20);
  edit_swdelay_box->setDisabled(true);
  edit_swdelay_label=new QLabel(edit_swdelay_box,tr("Switcher Delay:"),this,
			     "edit_swdelay_label");
  edit_swdelay_label->setGeometry(10,262,110,24);
  edit_swdelay_label->setAlignment(AlignRight|AlignVCenter);
  edit_swdelay_label->setDisabled(true);
  edit_swdelay_unit=new QLabel(edit_swdelay_box,tr("1/10 sec"),this,
			     "edit_swdelay_unit");
  edit_swdelay_unit->setGeometry(170,262,60,24);
  edit_swdelay_unit->setAlignment(AlignLeft|AlignVCenter);
  edit_swdelay_unit->setDisabled(true);
  edit_swdelay_box->hide();
  edit_swdelay_label->hide();
  edit_swdelay_unit->hide();

  //
  // Defaults Label
  //
  label=new QLabel(tr("Defaults"),this,"settings_label");
  label->setGeometry(10,276,100,24);
  label->setFont(big_font);

  //
  // Default Channels
  //
  edit_channels_box=new QComboBox(this,"edit_channels_box");
  edit_channels_box->setGeometry(125,300,60,24);
  edit_channels_box->setInsertionPolicy(QComboBox::NoInsertion);
  label=new QLabel(edit_channels_box,tr("Channels:"),this,
			     "edit_channels_label");
  label->setGeometry(10,300,110,24);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Default Autotrim Threshold
  //
  edit_threshold_box=new QSpinBox(this,"edit_threshold_box");
  edit_threshold_box->setGeometry(125,324,70,24);
  edit_threshold_box->setSuffix(" dB");
  edit_threshold_box->setRange(-100,0);
  label=new QLabel(edit_threshold_box,tr("Trim Threshold:"),this,
			     "edit_threshold_label");
  label->setGeometry(10,324,110,24);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Host-Wide Settings Label
  //
  label=new QLabel(tr("Host-Wide Settings"),this,"settings_label");
  label->setGeometry(10,369,200,24);
  label->setFont(big_font);

  edit_errorrml_edit=new QLineEdit(this,"edit_errorrml_edit");
  edit_errorrml_edit->setGeometry(125,393,248,24);
  label=new QLabel(edit_errorrml_edit,tr("Error RML:"),this,
			     "edit_errorrml_label");
  label->setGeometry(10,393,110,24);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Play Deck Selector
  //
  edit_play_deck_box=new QComboBox(this,"edit_record_deck_box");
  edit_play_deck_box->setGeometry(475,10,60,24);
  edit_play_deck_box->setInsertionPolicy(QComboBox::NoInsertion);
  connect(edit_play_deck_box,SIGNAL(activated(int)),
	  this,SLOT(playDeckActivatedData(int)));
  label=new QLabel(edit_play_deck_box,tr("Play Deck"),this,
		   "edit_play_deck_label");
  label->setFont(small_font);
  label->setGeometry(390,14,80,22);
  label->setAlignment(AlignRight);

  //
  // Play Deck Card Selector
  //
  edit_play_selector=new RDCardSelector(this,"edit_play_selector");
  edit_play_selector->setGeometry(387,42,120,10);

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  close_button->setFont(small_font);
  close_button->setText(tr("&Close"));
  close_button->setDefault(true);
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  if(cae_station->scanned()) {
    edit_record_selector->setMaxCards(cae_station->cards());
    edit_play_selector->setMaxCards(cae_station->cards());
    for(int i=0;i<edit_record_selector->maxCards();i++) {
      edit_record_selector->setMaxPorts(i,cae_station->cardInputs(i));
      edit_play_selector->setMaxPorts(i,cae_station->cardOutputs(i));
    }
  }
  else {
    QMessageBox::information(this,tr("No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    edit_record_selector->setDisabled(true);
    edit_monitor_box->setDisabled(true);
    edit_monitor_label->setDisabled(true);
    edit_play_selector->setDisabled(true);
  }
  edit_errorrml_edit->setText(edit_catch_conf->errorRml());
  edit_record_deck=NULL;
  edit_play_deck=NULL;
  edit_audition_deck=NULL;
  for(int i=0;i<MAX_DECKS;i++) {
    edit_record_deck_box->insertItem(QString().sprintf("%d",i+1));
    edit_play_deck_box->insertItem(QString().sprintf("%d",i+1));
  }
  edit_record_channel=edit_record_deck_box->currentItem()+1;
  edit_play_channel=edit_play_deck_box->currentItem()+129;
  edit_format_box->insertItem(tr("PCM16"));
  edit_format_box->insertItem(tr("MPEG Layer 2"));
  edit_channels_box->insertItem("1");
  edit_channels_box->insertItem("2");
  edit_bitrate_box->insertItem(tr("32 kbps/chan"));
  edit_bitrate_box->insertItem(tr("48 kbps/chan"));
  edit_bitrate_box->insertItem(tr("56 kbps/chan"));
  edit_bitrate_box->insertItem(tr("64 kbps/chan"));
  edit_bitrate_box->insertItem(tr("80 kbps/chan"));
  edit_bitrate_box->insertItem(tr("96 kbps/chan"));
  edit_bitrate_box->insertItem(tr("112 kbps/chan"));
  edit_bitrate_box->insertItem(tr("128 kbps/chan"));
  edit_bitrate_box->insertItem(tr("160 kbps/chan"));
  edit_bitrate_box->insertItem(tr("192 kbps/chan"));
  edit_swstation_box->insertItem(tr("[none]"));
  RDSqlQuery *q=new RDSqlQuery("select NAME from STATIONS where \
                              NAME!=\"DEFAULT\"");
  while(q->next()) {
    edit_swstation_box->insertItem(q->value(0).toString());
  }
  delete q;
  ReadRecord(edit_record_channel);
  ReadRecord(edit_play_channel);
  ReadRecord(0);
  recordCardChangedData(edit_record_selector->card());
}


EditDecks::~EditDecks()
{
  delete edit_record_deck_box;
  delete edit_format_box;
  delete edit_channels_box;
  delete edit_bitrate_box;
  delete edit_threshold_box;
  delete edit_record_selector;
  delete edit_record_deck;
  delete edit_audition_deck;
  delete edit_errorrml_edit;
  delete edit_catch_conf;
}


QSize EditDecks::sizeHint() const
{
  return QSize(560,454);
} 


QSizePolicy EditDecks::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditDecks::recordDeckActivatedData(int chan)
{
  WriteRecord(edit_record_channel);
  edit_record_channel=chan+1;
  ReadRecord(edit_record_channel);
}


void EditDecks::playDeckActivatedData(int chan)
{
  WriteRecord(edit_play_channel);
  edit_play_channel=chan+129;
  ReadRecord(edit_play_channel);
}


void EditDecks::recordCardChangedData(int card)
{
  if((card>=0)&&edit_monitor_label->isEnabled()) {
    edit_monitor_box->setEnabled(true);
    edit_monitor_box->setMaxValue(edit_station->cardOutputs(card)-1);
    monitorPortChangedData(edit_monitor_box->value());
  }
  else {
    edit_monitor_box->setDisabled(true);
    edit_default_on_label->setDisabled(true);
    edit_default_on_box->setDisabled(true);
  }    
}


void EditDecks::monitorPortChangedData(int port)
{
  if(port<0) {
    edit_default_on_label->setDisabled(true);
    edit_default_on_box->setDisabled(true);
  }
  else {
    edit_default_on_label->setEnabled(true);
    edit_default_on_box->setEnabled(true);
  }
}


void EditDecks::formatActivatedData(int index)
{
  if(index==0) {
    edit_bitrate_label->setDisabled(true);
    edit_bitrate_box->setDisabled(true);
  }
  else {
    edit_bitrate_label->setEnabled(true);
    edit_bitrate_box->setEnabled(true);
  }
}


void EditDecks::stationActivatedData(const QString &str)
{
  QString sql;
  RDSqlQuery *q;

  if(str=="[none]") {
    edit_swmatrix_label->setDisabled(true);
    edit_swmatrix_box->setDisabled(true);
    edit_swoutput_label->setDisabled(true);
    edit_swoutput_box->setDisabled(true);
    edit_swdelay_label->setDisabled(true);
    edit_swdelay_unit->setDisabled(true);
    edit_swdelay_box->setDisabled(true);
    return;
  }
  edit_swmatrix_label->setEnabled(true);
  edit_swmatrix_box->setEnabled(true);
  edit_swoutput_label->setEnabled(true);
  edit_swoutput_box->setEnabled(true);
  edit_swdelay_label->setEnabled(true);
  edit_swdelay_unit->setEnabled(true);
  edit_swdelay_box->setEnabled(true);

  edit_swmatrix_box->clear();
  sql=QString().sprintf("select NAME from MATRICES where \
                         (STATION_NAME=\"%s\")&&(OUTPUTS>0)",
			(const char *)str);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_swmatrix_box->insertItem(q->value(0).toString());
  }
  matrixActivatedData(edit_swmatrix_box->currentText());
  delete q;
}


void EditDecks::matrixActivatedData(const QString &str)
{
  QString sql;
  RDSqlQuery *q;

  if(str.isEmpty()) {
    edit_swoutput_label->setDisabled(true);
    edit_swoutput_box->setDisabled(true);
    edit_swdelay_label->setDisabled(true);
    edit_swdelay_unit->setDisabled(true);
    edit_swdelay_box->setDisabled(true);
    return;
  }
  edit_swoutput_label->setEnabled(true);
  edit_swoutput_box->setEnabled(true);
  edit_swdelay_label->setEnabled(true);
  edit_swdelay_unit->setEnabled(true);
  edit_swdelay_box->setEnabled(true);

  edit_swoutput_box->clear();
  int matrix=GetMatrix();
  if(matrix<0) {
    return;
  }
  sql=QString().sprintf("select NAME from OUTPUTS where \
                         STATION_NAME=\"%s\"&&MATRIX=%d",
			(const char *)edit_swstation_box->currentText(),
			matrix);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_swoutput_box->insertItem(q->value(0).toString());
  }
  delete q;
}


void EditDecks::closeData()
{
  edit_catch_conf->setErrorRml(edit_errorrml_edit->text());
  WriteRecord(0);
  WriteRecord(edit_record_channel);
  WriteRecord(edit_play_channel);
  done(0);
}


void EditDecks::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->moveTo(385,10);
  p->lineTo(385,sizeHint().height()-10);
  p->end();
}


void EditDecks::ReadRecord(int chan)
{
  if(chan==0) {  // Audition Deck
    if(edit_audition_deck==NULL) {
      edit_audition_deck=new RDDeck(edit_station->name(),0,true);
    }
  }
  if((chan>128)&&(chan<(MAX_DECKS+129))) {  // Play Deck
    if(edit_play_deck!=NULL) {
      delete edit_play_deck;
    }
    edit_play_deck=new RDDeck(edit_station->name(),edit_play_channel,true);
    edit_play_selector->setCard(edit_play_deck->cardNumber());
    edit_play_selector->setPort(edit_play_deck->portNumber());
  }
  if((chan>0)&&(chan<(MAX_DECKS+1))) {       // Record Deck
    if(edit_record_deck!=NULL) {
      delete edit_record_deck;
    }
    edit_record_deck=new RDDeck(edit_station->name(),edit_record_channel,true);
    edit_record_selector->setCard(edit_record_deck->cardNumber());
    edit_record_selector->setPort(edit_record_deck->portNumber());
    edit_monitor_box->setValue(edit_record_deck->monitorPortNumber());
    if(edit_record_deck->defaultMonitorOn()) {
      edit_default_on_box->setCurrentItem(1);
    }
    else {
      edit_default_on_box->setCurrentItem(0);
    }
    switch(edit_record_deck->defaultFormat()) {
	case RDSettings::Pcm16:
	  edit_format_box->setCurrentItem(0);
	  edit_bitrate_box->setDisabled(true);
	  break;

	case RDSettings::MpegL2:
	case RDSettings::MpegL2Wav:
	  edit_format_box->setCurrentItem(1);
	  edit_bitrate_box->setEnabled(true);
	  break;

	case RDSettings::MpegL1:
	case RDSettings::MpegL3:
	case RDSettings::Flac:
	case RDSettings::OggVorbis:
	  break;
    }
    edit_channels_box->setCurrentItem(edit_record_deck->defaultChannels()-1);
    switch(edit_record_deck->defaultBitrate()) {
	case 32000:
	  edit_bitrate_box->setCurrentItem(0);
	  break;
	case 48000:
	  edit_bitrate_box->setCurrentItem(1);
	  break;
	case 56000:
	  edit_bitrate_box->setCurrentItem(2);
	  break;
	case 64000:
	  edit_bitrate_box->setCurrentItem(3);
	  break;
	case 80000:
	  edit_bitrate_box->setCurrentItem(4);
	  break;
	case 96000:
	  edit_bitrate_box->setCurrentItem(5);
	  break;
	case 112000:
	  edit_bitrate_box->setCurrentItem(6);
	  break;
	case 128000:
	  edit_bitrate_box->setCurrentItem(7);
	  break;
	case 160000:
	  edit_bitrate_box->setCurrentItem(8);
	  break;
	case 192000:
	  edit_bitrate_box->setCurrentItem(9);
	  break;
	case 224000:
	  edit_bitrate_box->setCurrentItem(10);
	  break;
	case 256000:
	  edit_bitrate_box->setCurrentItem(11);
	  break;
	case 320000:
	  edit_bitrate_box->setCurrentItem(12);
	  break;
	case 384000:
	  edit_bitrate_box->setCurrentItem(13);
	  break;
	default:
	  edit_bitrate_box->setCurrentItem(7);    // 128 kbps/chan
	  break;
    }
    for(int i=0;i<edit_swstation_box->count();i++) {
      if(edit_record_deck->switchStation()==edit_swstation_box->text(i)) {
	edit_swstation_box->setCurrentItem(i);
	stationActivatedData(edit_swstation_box->currentText());
      }
    }
    QString matrix_name=edit_record_deck->switchMatrixName();
    for(int i=0;i<edit_swmatrix_box->count();i++) {
      if(edit_swmatrix_box->text(i)==matrix_name) {
	edit_swmatrix_box->setCurrentItem(i);
	matrixActivatedData(edit_swmatrix_box->currentText());
      }
    }
    QString output_name=edit_record_deck->switchOutputName();
    for(int i=0;i<edit_swoutput_box->count();i++) {
      if(edit_swoutput_box->text(i)==output_name) {
	edit_swoutput_box->setCurrentItem(i);
      }
    }
    edit_swdelay_box->setValue(edit_record_deck->switchDelay()/100);
    edit_threshold_box->setValue(-edit_record_deck->defaultThreshold()/100);
  }
}


void EditDecks::WriteRecord(int chan)
{
  int temp;

  if((chan>128)&&(chan<(MAX_DECKS+129))) { // Play Deck
    edit_play_deck->setCardNumber(edit_play_selector->card());
    edit_play_deck->setPortNumber(edit_play_selector->port());
  }
  if((chan>0)&&(chan<(MAX_DECKS+1))) {     // Record Deck
    edit_record_deck->setCardNumber(edit_record_selector->card());
    edit_record_deck->setPortNumber(edit_record_selector->port());
    edit_record_deck->setMonitorPortNumber(edit_monitor_box->value());
    if(edit_monitor_box->value()<0) {
      edit_record_deck->setDefaultMonitorOn(false);
    }
    else {
      if(edit_default_on_box->currentItem()==0) {
	edit_record_deck->setDefaultMonitorOn(false);
      }
      else {
	edit_record_deck->setDefaultMonitorOn(true);
      }
    }
    switch(edit_format_box->currentItem()) {
	case 0:
	  edit_record_deck->setDefaultFormat(RDSettings::Pcm16);
	  break;
	case 1:
	  edit_record_deck->setDefaultFormat(RDSettings::MpegL2);
	  break;
    }
    edit_record_deck->setDefaultChannels(edit_channels_box->currentItem()+1);
    sscanf((const char *)edit_bitrate_box->currentText(),"%d",&temp);
    edit_record_deck->setDefaultBitrate(temp*1000);
    edit_record_deck->setSwitchStation(edit_swstation_box->currentText());
    edit_record_deck->setSwitchMatrix(GetMatrix());
    edit_record_deck->setSwitchOutput(GetOutput());
    edit_record_deck->setSwitchDelay(100*edit_swdelay_box->value());
    edit_record_deck->setDefaultThreshold(-edit_threshold_box->value()*100);
  }
}


int EditDecks::GetMatrix()
{
  int matrix=-1;

  QString sql=QString().sprintf("select MATRIX from MATRICES where \
                               (STATION_NAME=\"%s\")&&(NAME=\"%s\")",
			       (const char *)edit_swstation_box->currentText(),
			       (const char *)edit_swmatrix_box->currentText());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    matrix=q->value(0).toInt();
  }
  delete q;
  return matrix;
}


int EditDecks::GetOutput()
{
  int output=-1;

  QString sql=QString().sprintf("select NUMBER from OUTPUTS where \
                               (STATION_NAME=\"%s\")&&(MATRIX=%d)&&\
                               (NAME=\"%s\")",
			       (const char *)edit_swstation_box->currentText(),
				GetMatrix(),
			       (const char *)edit_swoutput_box->currentText());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output=q->value(0).toInt();
  }
  delete q;
  return output;
}
