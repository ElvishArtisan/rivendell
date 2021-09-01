// record_cut.cpp
//
// Record a Rivendell Cut
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

#include <rdconf.h>
#include <rdmixer.h>
#include <rdrehash.h>

#include "globals.h"
#include "record_cut.h"

RecordCut::RecordCut(RDCart *cart,QString cut,bool use_weight,QWidget *parent)
  : RDDialog(parent)
{
  bool valid;
  bool is_track=cart->owner().isEmpty();
  bool allow_modification=rda->user()->modifyCarts()&&is_track;
  bool allow_editing=rda->user()->editAudio()&&is_track;
  rec_use_weighting=use_weight;
  rec_cut=new RDCut(cut);
  is_playing=false;
  is_ready=false;
  is_recording=false;
  is_closing=false;
  rec_timer_value=0;

  setWindowTitle("RDLibrary - "+tr("Cut Info / Record"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Load Audio Assignments
  //
  rec_card_no[0]=rda->libraryConf()->inputCard();
  rec_port_no[0]=rda->libraryConf()->inputPort();
  rec_card_no[1]=rda->libraryConf()->outputCard();
  rec_port_no[1]=rda->libraryConf()->outputPort();
  rec_play_handle=-1;

  //
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),this,SLOT(initData(bool)));
  connect(rda->cae(),SIGNAL(playing(int)),this,SLOT(playedData(int)));
  connect(rda->cae(),SIGNAL(playStopped(int)),this,SLOT(playStoppedData(int)));
  connect(rda->cae(),SIGNAL(recordLoaded(int,int)),
	  this,SLOT(recordLoadedData(int,int)));
  connect(rda->cae(),SIGNAL(recordUnloaded(int,int,unsigned)),
	  this,SLOT(recordUnloadedData(int,int,unsigned)));
  connect(rda->cae(),SIGNAL(recording(int,int)),this,SLOT(recordedData(int,int)));
  connect(rda->cae(),SIGNAL(recordStopped(int,int)),
	  this,SLOT(recordStoppedData(int,int)));
  connect(rda->cae(),SIGNAL(inputStatusChanged(int,int,bool)),
	  this,SLOT(aesAlarmData(int,int,bool)));

  //
  // Audio Parameters
  //
  rec_card_no[0]=rda->libraryConf()->inputCard();
  rec_card_no[1]=rda->libraryConf()->outputCard();
  rec_name=rec_cut->cutName();
  switch(rec_cut->codingFormat()) {
  case 0:
    rec_format=RDCae::Pcm16;
    break;
	
  case 1:
    rec_format=RDCae::MpegL2;
    break;

  default:
    rec_format=RDCae::Pcm16;
    break;
  }	
  rec_channels=rec_cut->channels();
  rec_samprate=rec_cut->sampleRate();
  rec_bitrate=rec_cut->bitRate();
  rec_length=rec_cut->length();

  //
  // Cut Description
  //
  cut_description_edit=new QLineEdit(this);
  cut_description_edit->setMaxLength(64);
  cut_description_label=new QLabel(tr("Description")+":",this);
  cut_description_label->setFont(labelFont());
  cut_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut Outcue
  //
  cut_outcue_edit=new QLineEdit(this);
  cut_outcue_edit->setMaxLength(64);
  cut_outcue_label=new QLabel(tr("Outcue")+":",this);
  cut_outcue_label->setFont(labelFont());
  cut_outcue_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut ISCI Code
  //
  cut_isci_edit=new QLineEdit(this);
  cut_isci_edit->setMaxLength(32);
  cut_isci_label=new QLabel(tr("ISCI Code")+":",this);
  cut_isci_label->setFont(labelFont());
  cut_isci_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut ISRC
  //
  cut_isrc_edit=new QLineEdit(this);
  cut_isrc_edit->setMaxLength(64);
  cut_isrc_label=new QLabel(tr("ISRC")+":",this);
  cut_isrc_label->setFont(labelFont());
  cut_isrc_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut Source
  //
  cut_source_edit=new QLineEdit(this);
  cut_source_edit->setReadOnly(true);
  cut_source_label=new QLabel(tr("Source")+":",this);
  cut_source_label->setFont(labelFont());
  cut_source_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut Ingest
  //
  cut_ingest_edit=new QLineEdit(this);
  cut_ingest_edit->setReadOnly(true);
  cut_ingest_label=new QLabel(tr("Ingest")+":",this);
  cut_ingest_label->setFont(labelFont());
  cut_ingest_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut Play Date Time
  //
  cut_playdate_edit=new QLineEdit(this);
  cut_playdate_edit->setReadOnly(true);
  cut_playdate_edit->setMaxLength(64);
  cut_playdate_label=new QLabel(tr("Last Played")+":",this);
  cut_playdate_label->setFont(labelFont());
  cut_playdate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut Play Counter
  //
  cut_playcounter_edit=new QLineEdit(this);
  cut_playcounter_edit->setAlignment(Qt::AlignRight);
  cut_playcounter_edit->setReadOnly(true);
  cut_playcounter_edit->setMaxLength(64);
  cut_playcounter_label=new QLabel(tr("# of Plays")+":",this);
  cut_playcounter_label->setFont(labelFont());
  cut_playcounter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Evergreen Checkbox
  //
  rec_evergreen_box=new QCheckBox(this);
  rec_evergreen_label=new QLabel(tr("Cut is EVERGREEN")+":",this);
  rec_evergreen_label->setFont(labelFont());
  rec_evergreen_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(rec_evergreen_box,SIGNAL(toggled(bool)),
	  this,SLOT(evergreenToggledData(bool)));

  //
  // Cut Weight
  //
  cut_weight_box=new QSpinBox(this);
  cut_weight_box->setRange(0,100);
  cut_weight_label=new QLabel(tr("Weight")+":",this);
  cut_weight_label->setFont(labelFont());
  cut_weight_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cut Air Date Times
  //
  cut_killdatetime_groupbox=new QGroupBox(tr("Air Date/Time"),this);
  cut_killdatetime_groupbox->setFont(labelFont());
  cut_killdatetime_group=new QButtonGroup(this);
  connect(cut_killdatetime_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(airDateButtonData(int)));
  cut_startdatetime_enable_button=new QRadioButton(tr("Enabled"),this);
  cut_killdatetime_group->addButton(cut_startdatetime_enable_button,true);
  cut_startdatetime_disable_button=new QRadioButton(tr("Disabled"),this);
  cut_killdatetime_group->addButton(cut_startdatetime_disable_button,false);

  cut_startdatetime_edit=new RDDateTimeEdit(this);
  cut_startdatetime_label=new QLabel(tr("Start"),this);
  cut_startdatetime_label->setFont(subLabelFont());
  cut_startdatetime_label->setAlignment(Qt::AlignRight);
  cut_enddatetime_edit=new RDDateTimeEdit(this);
  cut_enddatetime_label=new QLabel(tr("End"),this);
  cut_enddatetime_label->setFont(subLabelFont());
  cut_enddatetime_label->setAlignment(Qt::AlignRight);

  //
  // Cut Daypart
  //
  cut_daypart_groupbox=new QGroupBox(tr("Daypart"),this);
  cut_daypart_groupbox->setFont(labelFont());
  cut_daypart_group=new QButtonGroup(this);
  connect(cut_daypart_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(daypartButtonData(int)));
  cut_startdaypart_enable_button=new QRadioButton(tr("Enabled"),this);
  cut_daypart_group->addButton(cut_startdaypart_enable_button,true);
  cut_startdaypart_disable_button=new QRadioButton(tr("Disabled"),this);
  cut_daypart_group->addButton(cut_startdaypart_disable_button,false);

  cut_startdaypart_edit=new RDTimeEdit(this);
  cut_startdaypart_label=new QLabel(tr("Start Time"),this);
  cut_startdaypart_label->setFont(subLabelFont());
  cut_startdaypart_label->setAlignment(Qt::AlignRight);

  cut_enddaypart_edit=new RDTimeEdit(this);
  cut_enddaypart_label=new QLabel(tr("End Time"),this);
  cut_enddaypart_label->setFont(subLabelFont());
  cut_enddaypart_label->setAlignment(Qt::AlignRight);

  //
  // Days of the Week
  //
  rec_dayofweek_groupbox=new QGroupBox(tr("Day of the Week"),this);
  rec_dayofweek_groupbox->setFont(labelFont());
  rec_weekpart_button[0]=new QCheckBox(this);
  rec_weekpart_label[0]=new QLabel(tr("Monday"),this);
  rec_weekpart_label[0]->setFont(subLabelFont());
  rec_weekpart_label[0]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_weekpart_button[1]=new QCheckBox(this);
  rec_weekpart_label[1]=new QLabel(tr("Tuesday"),this);
  rec_weekpart_label[1]->setFont(subLabelFont());
  rec_weekpart_label[1]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_weekpart_button[2]=new QCheckBox(this);
  rec_weekpart_label[2]=new QLabel(tr("Wednesday"),this);
  rec_weekpart_label[2]->setFont(subLabelFont());
  rec_weekpart_label[2]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_weekpart_button[3]=new QCheckBox(this);
  rec_weekpart_label[3]=new QLabel(tr("Thursday"),this);
  rec_weekpart_label[3]->setFont(subLabelFont());
  rec_weekpart_label[3]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_weekpart_button[4]=new QCheckBox(this);
  rec_weekpart_label[4]=new QLabel(tr("Friday"),this);
  rec_weekpart_label[4]->setFont(subLabelFont());
  rec_weekpart_label[4]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_weekpart_button[5]=new QCheckBox(this);
  rec_weekpart_label[5]=new QLabel(tr("Saturday"),this);
  rec_weekpart_label[5]->setFont(subLabelFont());
  rec_weekpart_label[5]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_weekpart_button[6]=new QCheckBox(this);
  rec_weekpart_label[6]=new QLabel(tr("Sunday"),this);
  rec_weekpart_label[6]->setFont(subLabelFont());
  rec_weekpart_label[6]->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rec_set_button=new QPushButton(tr("Set All"),this);
  rec_set_button->setFont(subButtonFont());
  connect(rec_set_button,SIGNAL(clicked()),this,SLOT(setAllData()));

  rec_clear_button=new QPushButton(tr("Clear All"),this);
  rec_clear_button->setFont(subButtonFont());
  connect(rec_clear_button,SIGNAL(clicked()),this,SLOT(clearAllData()));

  //
  // Audio Meter
  //
  rec_meter=new RDStereoMeter(this);
  rec_meter->setReference(0);
  rec_meter->setMode(RDSegMeter::Independent);
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(meterData()));
  timer->start(RD_METER_UPDATE_INTERVAL);

  //
  // AES Alarm
  //
  rec_aes_alarm_label=new QLabel(this);
  rec_aes_alarm_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
 rec_aes_alarm_label->setFont(progressFont());
  rec_aes_alarm_label->setPalette(QColor(Qt::red));
  rec_aes_alarm_label->setText(tr("AES ALARM"));
  rec_aes_alarm_label->hide();

  //
  // Record Timer
  //
  rec_timer=new QTimer(this);
  connect(rec_timer,SIGNAL(timeout()),this,SLOT(recTimerData()));
  rec_timer_label=new QLabel(this);
  rec_timer_label->setFont(timerFont());
  rec_timer_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  rec_timer_label->setText(RDGetTimeLength(rec_length,true));

  //
  // Channels
  //
  rec_channels_box=new QComboBox(this);
  rec_channels_edit=new QLineEdit(this);
  rec_channels_box_label=new QLabel(tr("Channels"),this);
  rec_channels_box_label->setAlignment(Qt::AlignHCenter);
  rec_channels_box_label->setFont(labelFont());
  connect(rec_channels_box,SIGNAL(activated(int)),
	  this,SLOT(channelsData(int)));

  //
  //  Record Button
  //
  rec_record_button=new RDTransportButton(RDTransportButton::Record,this);
  rec_record_button->setDefault(true);
  connect(rec_record_button,SIGNAL(clicked()),this,SLOT(recordData()));

  //
  //  Play Button
  //
  rec_play_button=new RDTransportButton(RDTransportButton::Play,this);
  rec_play_button->setDefault(true);
  connect(rec_play_button,SIGNAL(clicked()),this,SLOT(playData()));

  //
  //  Stop Button
  //
  rec_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  rec_stop_button->setDefault(true);
  rec_stop_button->setState(RDTransportButton::On);
  rec_stop_button->setOnColor(Qt::red);
  connect(rec_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  //
  // Record Mode 
  //
  rec_mode_box=new QComboBox(this);
  rec_mode_box_label=new QLabel(tr("Record Mode"),this);
  rec_mode_box_label->setAlignment(Qt::AlignHCenter);
  rec_mode_box_label->setFont(labelFont());

  //
  // AutoTrim Mode 
  //
  rec_trim_box=new QComboBox(this);
  rec_trim_box_label=new QLabel(tr("AutoTrim"),this);
  rec_trim_box_label->setAlignment(Qt::AlignHCenter);
  rec_trim_box_label->setFont(labelFont());

  //
  //  Close Button
  //
  close_button=new QPushButton(this);
  close_button->setDefault(true);
  close_button->setFont(buttonFont());
  close_button->setText(tr("Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Fields
  //
  cut_description_edit->setText(rec_cut->description());
  cut_outcue_edit->setText(rec_cut->outcue());
  cut_origin_name=rec_cut->originName();
  cut_origin_datetime=rec_cut->originDatetime(&valid);
  QString user=rec_cut->originLoginName()+"@";
  if(rec_cut->originLoginName().isEmpty()) {
    user="";
  }
  if(valid) {
    cut_ingest_edit->setText(cut_origin_name+" - "+
			     rda->shortDateString(cut_origin_datetime.date())+
			     " "+rda->timeString(cut_origin_datetime.time()));
  }
  if(rec_cut->sourceHostname().isEmpty()) {
    cut_source_edit->setText("["+tr("unknown")+"]");
  }
  else {
    cut_source_edit->setText(user+rec_cut->sourceHostname());
  }
  cut_isci_edit->setText(rec_cut->isci());
  cut_isrc_edit->setText(rec_cut->isrc(RDCut::FormattedIsrc));
  if(use_weight) {
    cut_weight_label->setText(tr("Weight"));
    cut_weight_box->setRange(0,100);
    cut_weight_box->setValue(rec_cut->weight());
  }
  else {
    cut_weight_label->setText(tr("Order"));
    cut_weight_box->setRange(1,RD_MAX_CUT_NUMBER);
    cut_weight_box->setValue(rec_cut->playOrder());
  }
  if(rec_cut->playCounter()>0) {
    cut_playdate_edit->
      setText(rda->shortDateString(rec_cut->lastPlayDatetime(&valid).date())+
	      " "+rda->timeString(rec_cut->lastPlayDatetime(&valid).time()));
  }
  else {
    cut_playdate_edit->setText("Never");
  }
  cut_playcounter_edit->
    setText(QString::asprintf("%d",rec_cut->playCounter()));
  rec_evergreen_box->setChecked(rec_cut->evergreen());
  evergreenToggledData(rec_evergreen_box->isChecked());
  cut_startdatetime_edit->setDateTime(rec_cut->startDatetime(&valid));
  cut_enddatetime_edit->setDateTime(rec_cut->endDatetime(&valid));
  cut_startdatetime_enable_button->setChecked(valid);
  cut_startdatetime_disable_button->setChecked(!valid);
  airDateButtonData(valid);
  cut_startdaypart_edit->setTime(rec_cut->startDaypart(&valid));
  cut_startdaypart_enable_button->setChecked(valid);
  cut_startdaypart_disable_button->setChecked(!valid);
  cut_enddaypart_edit->setTime(rec_cut->endDaypart(&valid));
  daypartButtonData(valid);
  for(int i=0;i<7;i++) {
    if(rec_cut->weekPart(i+1)) {
      rec_weekpart_button[i]->setChecked(true);
    }
  }
  rec_channels_box->insertItem(0,"1");
  rec_channels_box->insertItem(1,"2");
  rec_channels_box->setCurrentIndex(rec_cut->channels()-1);
  rec_channels_edit->setText(QString::asprintf("%d",rec_cut->channels()));
  rec_mode_box->insertItem(0,tr("Manual"));
  rec_mode_box->insertItem(1,tr("VOX"));
  switch(rda->libraryConf()->defaultRecordMode()) {
      case RDLibraryConf::Manual:
	rec_mode_box->setCurrentIndex(0);
	break;

      case RDLibraryConf::Vox:
	rec_mode_box->setCurrentIndex(1);
	break;
  }
  rec_trim_box->insertItem(0,tr("On"));
  rec_trim_box->insertItem(1,tr("Off"));
  if(rda->libraryConf()->defaultTrimState()) {
    rec_trim_box->setCurrentIndex(0);
  }
  else {
    rec_trim_box->setCurrentIndex(1);
  }
  aesAlarmData(rec_card_no[0],rec_port_no[0],
	       rda->cae()->inputStatus(rec_card_no[0],rec_port_no[0]));

  //
  // Set Control Perms
  //
  cut_description_edit->setReadOnly(!allow_modification);
  cut_outcue_edit->setReadOnly(!allow_modification);
  cut_isci_edit->setReadOnly(!allow_modification);
  cut_startdaypart_edit->setReadOnly(!allow_modification);
  cut_enddaypart_edit->setReadOnly(!allow_modification);
  if(!allow_modification) {
    cut_weight_box->setRange(cut_weight_box->value(),cut_weight_box->value());
    if(cut_startdatetime_enable_button->isChecked()) {
    }
    cut_startdatetime_edit->setReadOnly(true);
    cut_enddatetime_edit->setReadOnly(true);
  }
  rec_evergreen_box->setEnabled(allow_modification);
  if(!allow_modification) {
    cut_startdatetime_enable_button->setDisabled(true);
    cut_startdatetime_disable_button->setDisabled(true);
    cut_startdaypart_enable_button->setDisabled(true);
    cut_startdaypart_disable_button->setDisabled(true);
    rec_set_button->setDisabled(true);
    rec_clear_button->setDisabled(true);
  }
  for(int i=0;i<7;i++) {
    rec_weekpart_button[i]->setEnabled(allow_modification);
  }
  rec_record_button->
    setEnabled(allow_editing&&(rec_card_no[0]>=0)&&(rec_port_no[0]>=0));
  rec_mode_box->setEnabled(allow_editing);
  rec_trim_box->setEnabled(allow_editing);
  if(allow_editing) {
    rec_channels_edit->hide();
  }
  else {
    rec_channels_box->hide();
  }
}


RecordCut::~RecordCut()
{
  delete rec_meter;
  delete cut_description_edit;
  delete cut_outcue_edit;
  delete rec_timer;
  delete rec_record_button;
  delete rec_play_button;
  delete rec_stop_button;
}

QSize RecordCut::sizeHint() const
{
  return QSize(375,673);
} 


QSizePolicy RecordCut::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RecordCut::airDateButtonData(int id)
{
  bool valid;
  QDateTime datetime;

  cut_startdatetime_edit->setEnabled(id);
  cut_startdatetime_label->setEnabled(id);
  cut_enddatetime_edit->setEnabled(id);
  cut_enddatetime_label->setEnabled(id);
  if(id) {
    datetime=rec_cut->startDatetime(&valid);
    if(valid&&(!datetime.isNull())) {
      cut_startdatetime_edit->setDateTime(datetime);
      cut_enddatetime_edit->setDateTime(rec_cut->endDatetime(&valid));
    }
    else {
      cut_startdatetime_edit->setDate(QDate::currentDate());
      cut_startdatetime_edit->setTime(QTime());
      cut_enddatetime_edit->setDate(QDate::currentDate());
      cut_enddatetime_edit->setTime(QTime(23,59,59));
    }
  }
}


void RecordCut::daypartButtonData(int id)
{
  cut_startdaypart_edit->setEnabled(id);
  cut_startdaypart_label->setEnabled(id);
  cut_enddaypart_edit->setEnabled(id);
  cut_enddaypart_label->setEnabled(id);
}


void RecordCut::setAllData()
{
  for(int i=0;i<7;i++) {
    rec_weekpart_button[i]->setChecked(true);
  }
}


void RecordCut::clearAllData()
{
  for(int i=0;i<7;i++) {
    rec_weekpart_button[i]->setChecked(false);
  }
}


void RecordCut::channelsData(int id)
{
  rec_channels=id+1;
}


void RecordCut::recordData()
{
  //  QString filename;

  if((!is_ready)&&(!is_recording)&&(!is_playing)) {
    if(rec_cut->length()>0) {
      if(QMessageBox::warning(this,tr("Audio Exists"),
			      tr("This will overwrite the existing recording.\nDo you want to proceed?"),
			      QMessageBox::Yes,
			      QMessageBox::No)==QMessageBox::No) {
	return;
      }
      if(cut_clipboard!=NULL) {
	if(rec_cut->cutName()==cut_clipboard->cutName()) {
	  if(QMessageBox::warning(this,tr("Empty Clipboard"),
 	   tr("This will empty the clipboard.\nDo you still want to proceed?"),
				  QMessageBox::Yes,
				  QMessageBox::No)==QMessageBox::No) {
	    return;
	  }
	  delete cut_clipboard;
	  cut_clipboard=NULL;
	}
      }
    }
    RDCart *cart=new RDCart(rec_cut->cartNumber());
    cart->removeCutAudio(rda->station(),rda->user(),rec_cut->cutName(),rda->config());
    delete cart;
    switch(rda->libraryConf()->defaultFormat()) {
    case 0:
      rec_cut->setCodingFormat(0);
      rec_format=RDCae::Pcm16;
      break;

    case 1:
      rec_cut->setCodingFormat(1);
      rec_format=RDCae::MpegL2;
      break;

    case 2:
      rec_cut->setCodingFormat(2);
      rec_format=RDCae::Pcm24;
      break;

    default:
      rec_cut->setCodingFormat(0);
      rec_format=RDCae::Pcm16;
      break;
    }
    rec_samprate=rda->system()->sampleRate();
    rec_cut->setSampleRate(rec_samprate);
    rec_bitrate=rda->libraryConf()->defaultBitrate();
    rec_cut->setBitRate(rec_bitrate);
    rec_channels=rec_channels_box->currentIndex()+1;
    rec_cut->setChannels(rec_channels);
    rec_cut->setOriginDatetime(QDateTime::currentDateTime());
    rec_cut->setOriginName(rda->station()->name());
    cut_origin_name=rda->station()->name();
    cut_origin_datetime=QDateTime::currentDateTime();
    cut_ingest_edit->setText(cut_origin_name+" - "+
			    cut_origin_datetime.toString("M/d/yyyy hh:mm:ss"));
    rda->cae()->loadRecord(rec_card_no[0],rec_port_no[0],rec_name,rec_format,
			rec_channels,rec_samprate,rec_bitrate*rec_channels);
  }
}


void RecordCut::playData()
{
  int start=rec_cut->startPoint(true);
  int end=rec_cut->endPoint(true);

  if((!is_recording)&&(!is_playing)&&(!is_ready)) {  // Start Play
    rda->cae()->loadPlay(rec_card_no[1],rec_cut->cutName(),
		    &rec_stream_no[1],&rec_play_handle);
    RDSetMixerOutputPort(rda->cae(),rec_card_no[1],rec_stream_no[1],rec_port_no[1]);
    rda->cae()->positionPlay(rec_play_handle,start);
    rda->cae()->setPlayPortActive(rec_card_no[1],rec_port_no[1],rec_stream_no[1]);
    rda->cae()->setOutputVolume(rec_card_no[1],rec_stream_no[1],rec_port_no[1],
           0+rec_cut->playGain());
    rda->cae()->play(rec_play_handle,end-start,RD_TIMESCALE_DIVISOR,false);
  }
  if(is_ready&&(!is_recording)) {
    if(rec_mode_box->currentIndex()==1) {
      rda->cae()->
	record(rec_card_no[0],rec_port_no[0],rda->libraryConf()->maxLength(),
	       rda->libraryConf()->voxThreshold());
    }
    else {
      rda->cae()->
	record(rec_card_no[0],rec_port_no[0],rda->libraryConf()->maxLength(),0);
    }
  }
}


void RecordCut::stopData()
{
  if(is_playing) {
    rda->cae()->stopPlay(rec_play_handle);
    return;
  }
  if(is_recording) {
    rda->cae()->stopRecord(rec_card_no[0],rec_port_no[0]);
    return;
  }
  if(is_ready) {
    rda->cae()->unloadRecord(rec_card_no[0],rec_port_no[0]);
    rec_record_button->off();
    rec_play_button->off();
    rec_stop_button->on();
    is_ready=false;
  }
}


void RecordCut::recordLoadedData(int card,int stream)
{
  rec_timer_value=-1;
  recTimerData();
  rec_record_button->on();
  rec_play_button->flash();
  rec_stop_button->off();
  is_ready=true;
  is_recording=false;
}


void RecordCut::recordedData(int card,int stream)
{
  rec_timer->start(RECORD_CUT_TIMER_INTERVAL);
  rec_play_button->on();
  is_ready=false;
  is_recording=true;
}


void RecordCut::playedData(int handle)
{
  rec_play_button->on();
  rec_stop_button->off();
  rec_timer_value=-1;
  recTimerData();
  rec_timer->start(RECORD_CUT_TIMER_INTERVAL);
  is_playing=true;
  is_recording=false;
}


void RecordCut::playStoppedData(int handle)
{
  rda->cae()->unloadPlay(rec_play_handle);
  rec_timer->stop();
  rec_play_button->off();
  rec_stop_button->on();
  rec_meter->resetClipLight();
  is_playing=false;
  is_recording=false;
  rec_meter->setLeftSolidBar(-10000);
  rec_meter->setRightSolidBar(-10000);
  if(is_closing) {
    is_closing=false;
    closeData();
  }
}


void RecordCut::recordStoppedData(int card,int stream)
{
  //printf("recordStoppedData()\n");
  rda->cae()->unloadRecord(rec_card_no[0],rec_port_no[0]);
  rec_timer->stop();
  rec_play_button->off();
  rec_stop_button->on();
  rec_record_button->off();
  rec_meter->resetClipLight();
  is_playing=false;
  is_recording=false;
}


void RecordCut::recordUnloadedData(int card,int stream,unsigned len)
{
  //printf("recordUnloadedData(%d,%d,%u)\n",card,stream,len);
  QString filename;

  rec_meter->setLeftSolidBar(-100000);
  rec_meter->setRightSolidBar(-100000);

  RDSettings *s=new RDSettings();
  s->setSampleRate(rec_samprate);
  s->setBitRate(rec_bitrate);
  s->setChannels(rec_channels);
  s->setFormat((RDSettings::Format)rec_format);
  rec_cut->checkInRecording(rda->station()->name(),rda->user()->name(),
			    rda->station()->name(),s,len);
  RDRehash::rehash(rda->station(),rda->user(),rda->config(),rec_cut->cartNumber(),
		   rec_cut->cutNumber());
  if(rec_trim_box->currentIndex()==0) {
    rec_cut->autoTrim(RDCut::AudioBoth,rda->libraryConf()->trimThreshold());
  }
  rec_length=rec_cut->length();
  if(is_closing) {
    is_closing=false;
    closeData();
  }
}


void RecordCut::closeData()
{
  if(cut_description_edit->text().isEmpty()) {
    QMessageBox::information(this,tr("Missing Description"),
			     tr("You must provide a Cut Description!"),
			     QMessageBox::Ok);
    return;
  }
  QString isrc=cut_isrc_edit->text();
  if(!isrc.isEmpty()) {
    isrc.remove("-");
    if(isrc.length()!=12) {
      QMessageBox::information(this,tr("Invalid ISRC"),
			       tr("The ISRC data is malformed or invalid!"),
			       QMessageBox::Ok);
      return;
    }
  }
  rec_cut->setEvergreen(rec_evergreen_box->isChecked());
  if(cut_startdatetime_enable_button->isChecked()) {
    if(!cut_startdatetime_edit->dateTime().isValid()) {
      QMessageBox::warning(this,tr("Invalid Date"),
			   tr("The Start Date is invalid!"));
      return;
    }
    if(!cut_enddatetime_edit->dateTime().isValid()) {
      QMessageBox::warning(this,tr("Invalid Date"),
			   tr("The End Date is invalid!"));
      return;
    }
    if(cut_enddatetime_edit->dateTime()<cut_startdatetime_edit->dateTime()) {
      QMessageBox::warning(this,tr("Invalid Date"),
			   tr("The End Date is prior to the Start Date!"));
      return;
    }
    if((cut_enddatetime_edit->dateTime()<QDateTime::currentDateTime())&&
       (!rec_evergreen_box->isChecked())) {
      switch(QMessageBox::warning(this,tr("Invalid Date"),
				  tr("The End Date has already passed!\nDo you still want to save?"),
				  QMessageBox::Yes,QMessageBox::No)) {
      case QMessageBox::No:
      case QMessageBox::NoButton:
	return;

      default:
	break;
      }
    }
  }
  if(cut_startdaypart_enable_button->isChecked()) {
    if(!cut_startdaypart_edit->time().isValid()) {
      QMessageBox::warning(this,tr("Invalid Time"),
			   tr("The Start Time is invalid!"));
      return;
    }
    if(!cut_enddaypart_edit->time().isValid()) {
      QMessageBox::warning(this,tr("Invalid Time"),
			   tr("The End Time is invalid!"));
      return;
    }
    if(cut_enddaypart_edit->time()==cut_startdaypart_edit->time()) {
      QMessageBox::warning(this,tr("Invalid Time"),
		   tr("The StartTime cannot be the same as the End Time!"));
      return;
    }
  }
  if(is_playing||is_recording||is_ready) {
    stopData();
    is_closing=true;
    return;
  }
  if((cut_startdatetime_enable_button->isChecked())&&
     (cut_startdatetime_edit->time().isNull())) {
    rec_cut->setStartDatetime(QDateTime(cut_startdatetime_edit->date(),
					cut_startdatetime_edit->
					time().addMSecs(1)),true);
  }
  else {
    rec_cut->setStartDatetime(QDateTime(cut_startdatetime_edit->date(),
					cut_startdatetime_edit->time()),
    			      cut_startdatetime_enable_button->isChecked());
  }
  if((cut_startdatetime_enable_button->isChecked())&&
     (cut_enddatetime_edit->time().isNull())) {
    rec_cut->setEndDatetime(QDateTime(cut_enddatetime_edit->date(),
				      cut_enddatetime_edit->time().
				      addMSecs(1)),true);
  }
  else {
    rec_cut->setEndDatetime(cut_enddatetime_edit->dateTime(),
			    cut_startdatetime_enable_button->isChecked());
  }
  rec_cut->setStartDaypart(cut_startdaypart_edit->time(),
			   cut_startdaypart_enable_button->isChecked());
  rec_cut->setEndDaypart(cut_enddaypart_edit->time(),
			 cut_startdaypart_enable_button->isChecked());
  for(int i=0;i<7;i++) {
    rec_cut->setWeekPart(i+1,rec_weekpart_button[i]->isChecked());
  }
  rec_cut->setDescription(cut_description_edit->text());
  rec_cut->setOutcue(cut_outcue_edit->text());
  rec_cut->setIsrc(isrc);
  rec_cut->setIsci(cut_isci_edit->text());
  if(rec_use_weighting) {
    rec_cut->setWeight(cut_weight_box->value());
  }
  else {
    rec_cut->setPlayOrder(cut_weight_box->value());
  }
  rec_cut->setLength(rec_length);
  RDCart *cart=new RDCart(rec_cut->cartNumber());
  cart->resetRotation();
  delete cart;
  done(0);
}


void RecordCut::initData(bool state)
{
  if(!state) {
    QMessageBox::warning(this,tr("Can't Connect"),
			 tr("Unable to connect to Core AudioEngine"));
    exit(1);
  }
}


void RecordCut::recTimerData()
{
  rec_timer_value+=RECORD_CUT_TIMER_INTERVAL;
  rec_timer_label->setText(RDGetTimeLength(rec_timer_value,true));
}


void RecordCut::aesAlarmData(int card,int port,bool state)
{
  if((card==rda->libraryConf()->inputCard())&&
     (port==rda->libraryConf()->inputPort())) {
    if(rdaudioport_conf->inputPortType(rda->libraryConf()->inputPort())!=
       RDAudioPort::Analog) {
      if(state) {
	rec_aes_alarm_label->hide();
      }
      else {
	rec_aes_alarm_label->show();
      }
    }
  }
}


void RecordCut::meterData()
{
  short levels[2];

  if(is_ready||is_recording) {
    rda->cae()->inputMeterUpdate(rec_card_no[0],rec_port_no[0],levels);
    rec_meter->setLeftSolidBar(levels[0]);
    rec_meter->setRightSolidBar(levels[1]);
  }
  if(is_playing) {
    rda->cae()->outputMeterUpdate(rec_card_no[1],rec_port_no[1],levels);
    rec_meter->setLeftSolidBar(levels[0]);
    rec_meter->setRightSolidBar(levels[1]);
  }
}


void RecordCut::evergreenToggledData(bool state)
{
  cut_weight_label->setDisabled(state);
  cut_weight_box->setDisabled(state);
  cut_killdatetime_groupbox->setDisabled(state);
  cut_startdatetime_enable_button->setDisabled(state);
  cut_startdatetime_disable_button->setDisabled(state);
  cut_startdatetime_label->setDisabled(state);
  cut_startdatetime_edit->setDisabled(state);
  cut_enddatetime_edit->setDisabled(state);
  cut_enddatetime_label->setDisabled(state);
  cut_daypart_groupbox->setDisabled(state);
  cut_startdaypart_enable_button->setDisabled(state);
  cut_startdaypart_disable_button->setDisabled(state);
  cut_startdaypart_edit->setDisabled(state);
  cut_startdaypart_label->setDisabled(state);
  cut_enddaypart_edit->setDisabled(state);
  cut_enddaypart_label->setDisabled(state);
  rec_dayofweek_groupbox->setDisabled(state);
  for(int i=0;i<7;i++) {
    rec_weekpart_button[i]->setDisabled(state);
    rec_weekpart_label[i]->setDisabled(state);
  }
  rec_set_button->setDisabled(state);
  rec_clear_button->setDisabled(state);
  if (!state) {
    airDateButtonData(cut_startdatetime_enable_button->isChecked());
    daypartButtonData(cut_startdaypart_enable_button->isChecked());
  }

  update();
}


void RecordCut::resizeEvent(QResizeEvent *e)
{
  int w=size().width();

  cut_description_label->setGeometry(0,10,85,20);
  cut_description_edit->setGeometry(90,10,w-100,20);

  cut_outcue_label->setGeometry(0,32,85,20);
  cut_outcue_edit->setGeometry(90,32,w-100,20);

  cut_isci_label->setGeometry(0,54,85,20);
  cut_isci_edit->setGeometry(90,54,w-100,20);

  cut_isrc_label->setGeometry(0,76,85,20);
  cut_isrc_edit->setGeometry(90,76,w-100,20);

  cut_source_label->setGeometry(0,105,85,20);
  cut_source_edit->setGeometry(90,105,w-100,20);

  cut_ingest_label->setGeometry(0,127,85,20);
  cut_ingest_edit->setGeometry(90,127,w-100,20);

  cut_playdate_label->setGeometry(0,149,85,20);
  cut_playdate_edit->setGeometry(90,149,140,20);

  cut_playcounter_label->setGeometry(220,149,85,20);
  cut_playcounter_edit->setGeometry(310,149,55,20);

  rec_evergreen_label->setGeometry(30,178,sizeHint().width()-40,15);
  rec_evergreen_box->setGeometry(10,178,15,15);

  cut_weight_label->setGeometry(w/2,178,60,19);
  cut_weight_box->setGeometry(w/2+65,178,61,19);

  cut_startdatetime_enable_button->setGeometry(20,223,100,20);
  cut_startdatetime_disable_button->setGeometry(20,243,100,20);
  cut_killdatetime_groupbox->setGeometry(10,203,size().width()-20,60);

  cut_startdatetime_label->setGeometry(100,226,40,12);
  cut_startdatetime_edit->setGeometry(147,222,200,19);
  cut_enddatetime_label->setGeometry(100,245,40,12);
  cut_enddatetime_edit->setGeometry(147,242,200,19);

  cut_startdaypart_enable_button->setGeometry(57,293,100,20);
  cut_startdaypart_disable_button->setGeometry(57,313,100,20);
  cut_daypart_groupbox->setGeometry(37,273,size().width()-64,62);
  cut_startdaypart_label->setGeometry(137,296,80,12);
  cut_startdaypart_edit->setGeometry(222,292,100,19);
  cut_enddaypart_label->setGeometry(137,316,80,12);
  cut_enddaypart_edit->setGeometry(222,312,100,19);

  rec_dayofweek_groupbox->setGeometry(20,349,size().width()-35,95);
  rec_weekpart_label[0]->setGeometry(62,378,80,20);
  rec_weekpart_button[0]->setGeometry(40,380,15,15);
  rec_weekpart_label[1]->setGeometry(142,378,80,20);
  rec_weekpart_button[1]->setGeometry(120,380,15,15);
  rec_weekpart_label[2]->setGeometry(222,378,80,20);
  rec_weekpart_button[2]->setGeometry(200,380,15,15);
  rec_weekpart_label[3]->setGeometry(102,398,80,20);
  rec_weekpart_button[3]->setGeometry(80,400,15,15);
  rec_weekpart_label[4]->setGeometry(202,398,80,20);
  rec_weekpart_button[4]->setGeometry(180,400,15,15);
  rec_weekpart_label[5]->setGeometry(102,418,80,20);
  rec_weekpart_button[5]->setGeometry(80,420,15,15);
  rec_weekpart_label[6]->setGeometry(202,418,80,20);
  rec_weekpart_button[6]->setGeometry(180,420,15,15);
  rec_set_button->setGeometry(size().width()-80,372,55,30);
  rec_clear_button->setGeometry(size().width()-80,410,55,30);

  rec_meter->setGeometry(20,453,rec_meter->geometry().width(),
			 rec_meter->geometry().height());

  rec_aes_alarm_label->setGeometry(15,525,110,22);
  rec_timer_label->setGeometry(130,513,120,50);

  rec_channels_box->setGeometry(20,568,60,35);
  rec_channels_edit->setGeometry(20,568,60,35);
  rec_channels_box_label->setGeometry(10,549,80,16);

  rec_record_button->setGeometry(100,553,80,50);
  rec_play_button->setGeometry(190,553,80,50);
  rec_stop_button->setGeometry(280,553,80,50);

  rec_mode_box_label->setGeometry(10,609,100,16);
  rec_mode_box->setGeometry(10,628,100,35);

  rec_trim_box_label->setGeometry(130,609,100,16);
  rec_trim_box->setGeometry(145,628,70,35);

  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
}


void RecordCut::closeEvent(QCloseEvent *e)
{
  closeData();
}


void RecordCut::AutoTrim(RDWaveFile *name)
{
  if(name->hasEnergy()) {
    rec_cut->setStartPoint(name->startTrim(rda->libraryConf()->trimThreshold()));
    rec_cut->setEndPoint(name->endTrim(rda->libraryConf()->trimThreshold()));
  }
}
