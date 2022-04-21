// edit_decks.cpp
//
// Edit a Rivendell RDCatch Deck Configuration
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

#include <QMessageBox>
#include <QPainter>

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlivewire.h>
#include <rdmatrix.h>

#include "edit_decks.h"
#include "globals.h"

EditDecks::EditDecks(RDStation *station,RDStation *cae_station,QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

  edit_station=station;

  setWindowTitle("RDAdmin - "+tr("Configure RDCatch"));

  //
  // Create RDCatchConf
  //
  edit_catch_conf=new RDCatchConf(station->name());

  //
  // Record Deck Selector
  //
  edit_record_deck_box=new QComboBox(this);
  edit_record_deck_box->setGeometry(140,10,60,24);
  connect(edit_record_deck_box,SIGNAL(activated(int)),
	  this,SLOT(recordDeckActivatedData(int)));
  QLabel *label=new QLabel(tr("Record Deck"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(35,14,100,22);
  label->setAlignment(Qt::AlignRight);

  //
  // Models
  //
  edit_station_model=new RDStationListModel(true,"",this);

  //
  // Settings Label
  //
  label=new QLabel(tr("Settings"),this);
  label->setGeometry(10,40,100,24);
  label->setFont(sectionLabelFont());

  //
  // Card Selector
  //
  edit_record_selector=new RDCardSelector(this);
  edit_record_selector->
    setGeometry(67,68,edit_record_selector->sizeHint().width(),
		edit_record_selector->sizeHint().height());
  connect(edit_record_selector,SIGNAL(cardChanged(int)),
	  this,SLOT(recordCardChangedData(int)));

  //
  // Monitor Port Selector
  //
  edit_monitor_box=new QSpinBox(this);
  edit_monitor_box->setGeometry(125,112,55,19);
  edit_monitor_box->setRange(-1,RD_MAX_PORTS-1);
  edit_monitor_box->setSpecialValueText(tr("None"));
  connect(edit_monitor_box,SIGNAL(valueChanged(int)),
	  this,SLOT(monitorPortChangedData(int)));
  edit_monitor_label=new QLabel(tr("Monitor Port:"),this);
  edit_monitor_label->setFont(labelFont());
  edit_monitor_label->setGeometry(10,112,112,19);
  edit_monitor_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_default_on_box=new QComboBox(this);
  edit_default_on_box->setGeometry(315,112,50,19);
  edit_default_on_box->insertItem(0,tr("Off"));
  edit_default_on_box->insertItem(1,tr("On"));
  edit_default_on_label=new QLabel(tr("Monitor defaults to"),this);
  edit_default_on_label->setFont(labelFont());
  edit_default_on_label->setGeometry(185,112,125,19);
  edit_default_on_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Format
  //
  edit_format_box=new QComboBox(this);
  edit_format_box->setGeometry(125,136,150,24);
  connect(edit_format_box,SIGNAL(activated(int)),
	  this,SLOT(formatActivatedData(int)));
  label=new QLabel(tr("Format:"),this);
  label->setFont(labelFont());
  label->setGeometry(10,136,110,24);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Bit Rate
  //
  edit_bitrate_box=new QComboBox(this);
  edit_bitrate_box->setGeometry(125,160,140,24);
  edit_bitrate_label=new QLabel(tr("Bit Rate:"),this);
  edit_bitrate_label->setFont(labelFont());
  edit_bitrate_label->setGeometry(10,160,110,24);
  edit_bitrate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Switcher Station
  //
  edit_swstation_box=new QComboBox(this);
  edit_swstation_box->setGeometry(125,190,250,24);
  edit_swstation_box->setModel(edit_station_model);
  edit_swstation_label=new QLabel(tr("Switcher Host:"),this);
  edit_swstation_label->setFont(labelFont());
  edit_swstation_label->setGeometry(10,190,110,24);
  edit_swstation_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_swstation_box,SIGNAL(activated(const QString &)),
	  this,SLOT(stationActivatedData(const QString &)));

  //
  // Switcher Matrix
  //
  edit_swmatrix_box=new QComboBox(this);
  edit_swmatrix_box->setGeometry(125,214,250,24);
  edit_swmatrix_box->setDisabled(true);
  edit_swmatrix_label=new QLabel(tr("Switcher Matrix:"),this);
  edit_swmatrix_label->setFont(labelFont());
  edit_swmatrix_label->setGeometry(10,214,110,24);
  edit_swmatrix_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_swmatrix_label->setDisabled(true);
  connect(edit_swmatrix_box,SIGNAL(activated(const QString &)),
	  this,SLOT(matrixActivatedData(const QString &)));

  //
  // Switcher Output
  //
  edit_swoutput_box=new QComboBox(this);
  edit_swoutput_box->setGeometry(125,238,250,24);
  edit_swoutput_box->setDisabled(true);
  edit_swoutput_label=new QLabel(tr("Switcher Output:"),this);
  edit_swoutput_label->setFont(labelFont());
  edit_swoutput_label->setGeometry(10,238,110,24);
  edit_swoutput_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_swoutput_label->setDisabled(true);

  //
  // Switcher Delay
  //
  edit_swdelay_box=new QSpinBox(this);
  edit_swdelay_box->setGeometry(125,262,40,24);
  edit_swdelay_box->setRange(0,20);
  edit_swdelay_box->setDisabled(true);
  edit_swdelay_label=new QLabel(tr("Switcher Delay:"),this);
  edit_swdelay_label->setFont(labelFont());
  edit_swdelay_label->setGeometry(10,262,110,24);
  edit_swdelay_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_swdelay_label->setDisabled(true);
  edit_swdelay_unit=new QLabel(tr("1/10 sec"),this);
  edit_swdelay_unit->setFont(labelFont());
  edit_swdelay_unit->setGeometry(170,262,60,24);
  edit_swdelay_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  edit_swdelay_unit->setDisabled(true);
  edit_swdelay_box->hide();
  edit_swdelay_label->hide();
  edit_swdelay_unit->hide();

  //
  // Defaults Label
  //
  label=new QLabel(tr("Defaults"),this);
  label->setGeometry(10,276,100,24);
  label->setFont(sectionLabelFont());

  //
  // Default Channels
  //
  edit_channels_box=new QComboBox(this);
  edit_channels_box->setGeometry(125,300,60,24);
  label=new QLabel(tr("Channels:"),this);
  label->setFont(labelFont());
  label->setGeometry(10,300,110,24);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Default Autotrim Threshold
  //
  edit_threshold_box=new QSpinBox(this);
  edit_threshold_box->setGeometry(125,324,70,24);
  edit_threshold_box->setSuffix(" dB");
  edit_threshold_box->setRange(-100,0);
  label=new QLabel(tr("Trim Threshold:"),this);
  label->setFont(labelFont());
  label->setGeometry(10,324,110,24);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Host-Wide Settings Label
  //
  label=new QLabel(tr("Host-Wide Settings"),this);
  label->setGeometry(10,369,200,24);
  label->setFont(sectionLabelFont());

  edit_errorrml_edit=new QLineEdit(this);
  edit_errorrml_edit->setGeometry(125,393,248,24);
  label=new QLabel(tr("Error RML:"),this);
  label->setFont(labelFont());
  label->setGeometry(10,393,110,24);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Play Deck Selector
  //
  edit_play_deck_box=new QComboBox(this);
  edit_play_deck_box->setGeometry(475,10,60,24);
  connect(edit_play_deck_box,SIGNAL(activated(int)),
	  this,SLOT(playDeckActivatedData(int)));
  label=new QLabel(tr("Play Deck"),this);
  label->setFont(sectionLabelFont());
  label->setGeometry(390,14,80,22);
  label->setAlignment(Qt::AlignRight);

  //
  // Play Deck Card Selector
  //
  edit_play_selector=new RDCardSelector(this);
  edit_play_selector->
    setGeometry(392,37,edit_play_selector->sizeHint().width(),
		edit_play_selector->sizeHint().height());
  connect(edit_play_selector,SIGNAL(settingsChanged(int,int,int)),
	  this,SLOT(playSettingsChangedData(int,int,int)));

  //
  // Deck Event Carts
  //
  edit_event_section_label=new QLabel(tr("Event Carts"),this);
  edit_event_section_label->setGeometry(395,99,100,24);
  edit_event_section_label->setFont(sectionLabelFont());

  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(eventCartSelectedData(int)));
  for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i+=2) {
    for(unsigned j=0;j<2;j++) {
      edit_event_labels[i+j]=new QLabel(QString::asprintf("%u:",i+j+1),this);
      edit_event_labels[i+j]->setFont(labelFont());
      edit_event_labels[i+j]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
      edit_event_labels[i+j]->setGeometry(387+165*j,126+14*i,20,20);

      edit_event_edits[i+j]=new QLineEdit(this);
      edit_event_edits[i+j]->setGeometry(412+165*j,126+14*i,60,20);
      edit_event_edits[i+j]->
	setValidator(new QIntValidator(1,RD_MAX_CART_NUMBER,this));

      edit_event_buttons[i+j]=new QPushButton(tr("Select"),this);
      edit_event_buttons[i+j]->setFont(subButtonFont());
      edit_event_buttons[i+j]->setGeometry(477+165*j,124+14*i,60,24);
      mapper->setMapping(edit_event_buttons[i+j],i+j);
      connect(edit_event_buttons[i+j],SIGNAL(clicked()),
	      mapper,SLOT(map()));
    }
  }

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this);
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  close_button->setFont(buttonFont());
  close_button->setText(tr("Close"));
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
    edit_record_deck_box->
      insertItem(edit_record_deck_box->count(),QString::asprintf("%d",i+1));
    edit_play_deck_box->
      insertItem(edit_play_deck_box->count(),QString::asprintf("%d",i+1));
  }
  edit_record_channel=edit_record_deck_box->currentIndex()+1;
  edit_play_channel=edit_play_deck_box->currentIndex()+129;
  edit_format_box->
    insertItem(edit_format_box->count(),tr("PCM16"),RDCae::Pcm16);
  edit_format_box->
    insertItem(edit_format_box->count(),tr("PCM24"),RDCae::Pcm24);
  edit_format_box->
    insertItem(edit_format_box->count(),tr("MPEG Layer 2"),RDCae::MpegL2);
  edit_channels_box->insertItem(0,"1");
  edit_channels_box->insertItem(1,"2");
  edit_bitrate_box->insertItem(edit_bitrate_box->count(),tr("32 kbps/chan"),32);
  edit_bitrate_box->insertItem(edit_bitrate_box->count(),tr("48 kbps/chan"),48);
  edit_bitrate_box->insertItem(edit_bitrate_box->count(),tr("56 kbps/chan"),56);
  edit_bitrate_box->insertItem(edit_bitrate_box->count(),tr("64 kbps/chan"),64);
  edit_bitrate_box->insertItem(edit_bitrate_box->count(),tr("80 kbps/chan"),80);
  edit_bitrate_box->insertItem(edit_bitrate_box->count(),tr("96 kbps/chan"),96);
  edit_bitrate_box->
    insertItem(edit_bitrate_box->count(),tr("112 kbps/chan"),112);
  edit_bitrate_box->
    insertItem(edit_bitrate_box->count(),tr("128 kbps/chan"),128);
  edit_bitrate_box->
    insertItem(edit_bitrate_box->count(),tr("160 kbps/chan"),160);
  edit_bitrate_box->
    insertItem(edit_bitrate_box->count(),tr("192 kbps/chan"),192);
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
  delete edit_station_model;
}


QSize EditDecks::sizeHint() const
{
  return QSize(710,454);
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
    edit_monitor_box->setMaximum(edit_station->cardOutputs(card)-1);
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
  if(index==2) {
    edit_bitrate_label->setEnabled(true);
    edit_bitrate_box->setEnabled(true);
  }
  else {
    edit_bitrate_label->setDisabled(true);
    edit_bitrate_box->setDisabled(true);
  }
}


void EditDecks::stationActivatedData(const QString &str)
{
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
  edit_swmatrix_box->insertItems(0,GetActiveOutputMatrices());
  for(unsigned i=0;i<edit_matrix_ids.size();i++) {
    if(edit_matrix_ids[i]==edit_record_deck->switchMatrix()) {
      edit_swmatrix_box->setCurrentIndex(i);
    }
  }
  matrixActivatedData(edit_swmatrix_box->currentText());
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
  if(edit_swmatrix_box->currentIndex()>=(int)edit_matrix_ids.size()) {
    return;
  }
  int matrix=edit_matrix_ids[edit_swmatrix_box->currentIndex()];
  sql=QString("select `NAME` from `OUTPUTS` where ")+
    "(`STATION_NAME`='"+
    RDEscapeString(edit_swstation_box->currentText())+"')&&"+
    QString::asprintf("(`MATRIX`=%d)&&",matrix)+
    "(`NAME`!='')";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_swoutput_box->
      insertItem(edit_swoutput_box->count(),q->value(0).toString());
  }
  delete q;
}


void EditDecks::playSettingsChangedData(int id,int card,int port)
{
  edit_event_section_label->setEnabled((card>=0)&&(port>=0));
  for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i++) {
    edit_event_labels[i]->setEnabled((card>=0)&&(port>=0));
    edit_event_edits[i]->setEnabled((card>=0)&&(port>=0));
    edit_event_buttons[i]->setEnabled((card>=0)&&(port>=0));
  }
}


void EditDecks::eventCartSelectedData(int n)
{
  int cartnum=edit_event_edits[n]->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    if(cartnum==0) {
      edit_event_edits[n]->setText("");
    }
    else {
      edit_event_edits[n]->setText(QString::asprintf("%06d",cartnum));
    }
  }
}


void EditDecks::closeData()
{
  edit_catch_conf->setErrorRml(edit_errorrml_edit->text());
  WriteRecord(0);
  WriteRecord(edit_record_channel);
  WriteRecord(edit_play_channel);
  done(true);
}


void EditDecks::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(Qt::black));
  p->drawLine(385,10,385,sizeHint().height()-10);
  p->end();
  delete p;
}


void EditDecks::ReadRecord(int chan)
{
  QString sql;
  RDSqlQuery *q;

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
      edit_default_on_box->setCurrentIndex(1);
    }
    else {
      edit_default_on_box->setCurrentIndex(0);
    }
    switch(edit_record_deck->defaultFormat()) {
	case RDCae::Pcm16:
	  edit_format_box->setCurrentIndex(0);
	  edit_bitrate_box->setDisabled(true);
	  break;

	case RDCae::Pcm24:
	  edit_format_box->setCurrentIndex(1);
	  edit_bitrate_box->setDisabled(true);
	  break;

	case RDCae::MpegL2:
	  //	case RDSettings::MpegL2Wav:
	  edit_format_box->setCurrentIndex(2);
	  edit_bitrate_box->setEnabled(true);
	  break;

	  //	case RDSettings::MpegL1:
	case RDCae::MpegL1:
	case RDSettings::MpegL3:
	  //	case RDSettings::Flac:
	  //	case RDSettings::OggVorbis:
	  break;
    }
    edit_channels_box->setCurrentIndex(edit_record_deck->defaultChannels()-1);
    switch(edit_record_deck->defaultBitrate()) {
	case 32000:
	  edit_bitrate_box->setCurrentIndex(0);
	  break;
	case 48000:
	  edit_bitrate_box->setCurrentIndex(1);
	  break;
	case 56000:
	  edit_bitrate_box->setCurrentIndex(2);
	  break;
	case 64000:
	  edit_bitrate_box->setCurrentIndex(3);
	  break;
	case 80000:
	  edit_bitrate_box->setCurrentIndex(4);
	  break;
	case 96000:
	  edit_bitrate_box->setCurrentIndex(5);
	  break;
	case 112000:
	  edit_bitrate_box->setCurrentIndex(6);
	  break;
	case 128000:
	  edit_bitrate_box->setCurrentIndex(7);
	  break;
	case 160000:
	  edit_bitrate_box->setCurrentIndex(8);
	  break;
	case 192000:
	  edit_bitrate_box->setCurrentIndex(9);
	  break;
	case 224000:
	  edit_bitrate_box->setCurrentIndex(10);
	  break;
	case 256000:
	  edit_bitrate_box->setCurrentIndex(11);
	  break;
	case 320000:
	  edit_bitrate_box->setCurrentIndex(12);
	  break;
	case 384000:
	  edit_bitrate_box->setCurrentIndex(13);
	  break;
	default:
	  edit_bitrate_box->setCurrentIndex(7);    // 128 kbps/chan
	  break;
    }
    for(int i=0;i<edit_swstation_box->count();i++) {
      if(edit_record_deck->switchStation()==edit_swstation_box->itemText(i)) {
	edit_swstation_box->setCurrentIndex(i);
	stationActivatedData(edit_swstation_box->currentText());
      }
    }
    QString matrix_name=edit_record_deck->switchMatrixName();
    for(int i=0;i<edit_swmatrix_box->count();i++) {
      if(edit_swmatrix_box->itemText(i)==matrix_name) {
	edit_swmatrix_box->setCurrentIndex(i);
	matrixActivatedData(edit_swmatrix_box->currentText());
      }
    }
    QString output_name=edit_record_deck->switchOutputName();
    for(int i=0;i<edit_swoutput_box->count();i++) {
      if(edit_swoutput_box->itemText(i)==output_name) {
	edit_swoutput_box->setCurrentIndex(i);
      }
    }
    edit_swdelay_box->setValue(edit_record_deck->switchDelay()/100);
    edit_threshold_box->setValue(-edit_record_deck->defaultThreshold()/100);
  }

  if(chan>128) {
    sql=QString("select ")+
      "`NUMBER`,"+       // 00
      "`CART_NUMBER` "+  // 01
      "from `DECK_EVENTS` where "+
      "(`STATION_NAME`='"+RDEscapeString(edit_station->name())+"')&&"+
      QString::asprintf("(`CHANNEL`=%d)",chan);
    q=new RDSqlQuery(sql);
    while(q->next()) {
      if((q->value(0).toInt()-1)<RD_CUT_EVENT_ID_QUAN) {
	if(q->value(1).toUInt()==0) {
	  edit_event_edits[q->value(0).toInt()-1]->setText("");
	}
	else {
	  edit_event_edits[q->value(0).toInt()-1]->
	    setText(QString::asprintf("%06u",q->value(1).toUInt()));
	}
      }
    }
    playSettingsChangedData(0,edit_play_selector->card(),
			    edit_play_selector->port());
  }
}


void EditDecks::WriteRecord(int chan)
{
  QString sql;
  RDSqlQuery *q;
  unsigned cartnum=0;

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
      if(edit_default_on_box->currentIndex()==0) {
	edit_record_deck->setDefaultMonitorOn(false);
      }
      else {
	edit_record_deck->setDefaultMonitorOn(true);
      }
    }
    /*
    switch(edit_format_box->currentIndex()) {
    case 0:
      edit_record_deck->setDefaultFormat(RDSettings::Pcm16);
      break;

    case 1:
      edit_record_deck->setDefaultFormat(RDSettings::Pcm24);
      break;

    case 2:
      edit_record_deck->setDefaultFormat(RDSettings::MpegL2);
      break;
    }
    */
    edit_record_deck->
      setDefaultFormat((RDCae::AudioCoding)edit_format_box->
		       itemData(edit_format_box->currentIndex()).toInt());
    edit_record_deck->setDefaultChannels(edit_channels_box->currentIndex()+1);
    edit_record_deck->setDefaultBitrate(1000*edit_bitrate_box->
		 itemData(edit_bitrate_box->currentIndex()).toInt());
    edit_record_deck->setSwitchStation(edit_swstation_box->currentText());
    edit_record_deck->setSwitchMatrix(GetMatrix());
    edit_record_deck->setSwitchOutput(GetOutput());
    edit_record_deck->setSwitchDelay(100*edit_swdelay_box->value());
    edit_record_deck->setDefaultThreshold(-edit_threshold_box->value()*100);
  }

  if(chan>128) {
    for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i++) {
      if(edit_event_edits[i]->text().isEmpty()) {
	cartnum=0;
      }
      else {
	cartnum=edit_event_edits[i]->text().toUInt();
      }
      sql=QString("update `DECK_EVENTS` set ")+
	QString::asprintf("`CART_NUMBER`=%u ",cartnum)+
	"where (`STATION_NAME`='"+RDEscapeString(edit_station->name())+"')&&"+
	QString::asprintf("(`CHANNEL`=%d)&&",chan)+
	QString::asprintf("(`NUMBER`=%u)",i+1);
      q=new RDSqlQuery(sql);
      delete q;
    }
  }
}


int EditDecks::GetMatrix()
{
  if(edit_swmatrix_box->currentIndex()>=0) {
    if(edit_swmatrix_box->currentIndex()<(int)edit_matrix_ids.size()) {
      return edit_matrix_ids[edit_swmatrix_box->currentIndex()];
    }
  }
  return -1;
}


int EditDecks::GetOutput()
{
  int output=-1;

  QString sql=QString("select `NUMBER` from `OUTPUTS` where ")+
    "(`STATION_NAME`='"+
    RDEscapeString(edit_swstation_box->currentText())+"')&&"+
    QString::asprintf("(`MATRIX`=%d)&&",GetMatrix())+
    "(`NAME`='"+RDEscapeString(edit_swoutput_box->currentText())+"')";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output=q->value(0).toInt();
  }
  delete q;
  return output;
}


QStringList EditDecks::GetActiveOutputMatrices()
{
  QStringList ret;
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  
  edit_matrix_ids.clear();
  sql=QString("select ")+
    "`TYPE`,"+     // 00
    "`NAME`,"+     // 01
    "`OUTPUTS`,"+  // 02
    "`MATRIX` "+   // 03
    "from `MATRICES` where "+
    "`STATION_NAME`='"+RDEscapeString(edit_swstation_box->currentText())+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    switch((RDMatrix::Type)q->value(0).toInt()) {
    case RDMatrix::LiveWireLwrpAudio:
      sql=QString("select ")+
	"`HOSTNAME`,"+     // 00
	"`PASSWORD`,"+     // 01
	"`TCP_PORT`,"+     // 02
	"`BASE_OUTPUT` "+  // 03
	"from `SWITCHER_NODES` where "+
	"(`STATION_NAME`='"+RDEscapeString(edit_station->name())+"')&& "+
	QString::asprintf("(`MATRIX`=%d) ",q->value(3).toInt())+
	"order by `BASE_OUTPUT` desc";
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	RDLiveWire *lw=new RDLiveWire(0,this);
	if(!lw->loadSettings(q1->value(0).toString(),q1->value(2).toUInt(),
			     q1->value(1).toString(),q1->value(3).toInt())) {
	  QMessageBox::warning(this,"RDAdmin - "+tr("Connection Error"),
			       tr("Unable to connect to node at")+" \" "+
			       q1->value(0).toString()+"\"."+
			    tr("Check that the unit is online and try again."));
	  delete lw;
	  return ret;
	}
	ret.push_back(q->value(1).toString()+
	   " ["+RDMatrix::typeString((RDMatrix::Type)q->value(0).toInt())+"]");
	delete lw;
	edit_matrix_ids.push_back(q->value(3).toInt());
      }
      delete q1;
      break;

    default:
      if(q->value(2).toInt()>0) {
	ret.push_back(q->value(1).toString()+
	   " ["+RDMatrix::typeString((RDMatrix::Type)q->value(0).toInt())+"]");
	edit_matrix_ids.push_back(q->value(3).toInt());
      }
      break;
    }
  }
  delete q;

  return ret;
}
