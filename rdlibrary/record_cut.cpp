// record_cut.cpp
//
// Record a Rivendell Cut
//
//   (C) Copyright 2002-2004,2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: record_cut.cpp,v 1.90.6.5 2014/01/09 01:11:14 cvs Exp $
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
#include <math.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qbuttongroup.h>

#include <rd.h>
#include <rdconf.h>
#include <rdmixer.h>

#include <record_cut.h>
#include <globals.h>
#include <rdconfig.h>

RecordCut::RecordCut(RDCart *cart,QString cut,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  bool valid;
  bool is_track=cart->owner().isEmpty();
  bool allow_modification=lib_user->modifyCarts()&&is_track;
  bool allow_editing=lib_user->editAudio()&&is_track;

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
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);
  QFont large_font=QFont("Helvetica",18,QFont::Bold);
  large_font.setPixelSize(18);
  QFont timer_font=QFont("Helvetica",20,QFont::Bold);
  timer_font.setPixelSize(20);
  QFont day_font=QFont("helvetica",10,QFont::Normal);
  day_font.setPixelSize(10);

  setCaption(tr("RDLibrary - Record"));
  rec_cut=new RDCut(cut);
  is_playing=false;
  is_ready=false;
  is_recording=false;
  is_closing=false;
  rec_timer_value=0;

  //
  // Load Audio Assignments
  //
  rec_card_no[0]=rdlibrary_conf->inputCard();
  rec_port_no[0]=rdlibrary_conf->inputPort();
  rec_card_no[1]=rdlibrary_conf->outputCard();
  rec_port_no[1]=rdlibrary_conf->outputPort();
  rec_play_handle=-1;

  //
  // CAE Connection
  //
  connect(rdcae,SIGNAL(isConnected(bool)),this,SLOT(initData(bool)));
  connect(rdcae,SIGNAL(playing(int)),this,SLOT(playedData(int)));
  connect(rdcae,SIGNAL(playStopped(int)),this,SLOT(playStoppedData(int)));
  connect(rdcae,SIGNAL(recordLoaded(int,int)),
	  this,SLOT(recordLoadedData(int,int)));
  connect(rdcae,SIGNAL(recordUnloaded(int,int,unsigned)),
	  this,SLOT(recordUnloadedData(int,int,unsigned)));
  connect(rdcae,SIGNAL(recording(int,int)),this,SLOT(recordedData(int,int)));
  connect(rdcae,SIGNAL(recordStopped(int,int)),
	  this,SLOT(recordStoppedData(int,int)));
  connect(rdcae,SIGNAL(inputStatusChanged(int,int,bool)),
	  this,SLOT(aesAlarmData(int,int,bool)));

  //
  // Audio Parameters
  //
  rec_card_no[0]=rdlibrary_conf->inputCard();
  rec_card_no[1]=rdlibrary_conf->outputCard();
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
  cut_description_edit=new QLineEdit(this,"cut_description_edit");
  cut_description_edit->setGeometry(10,30,355,19);
  cut_description_edit->setMaxLength(64);
  QLabel *cut_description_label=new QLabel(cut_description_edit,
					   tr("&Description"),this,
					   "cut_description_label");
  cut_description_label->setGeometry(15,11,120,19);
  cut_description_label->setFont(font);
  cut_description_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Cut Outcue
  //
  cut_outcue_edit=new QLineEdit(this,"cut_outcue_edit");
  cut_outcue_edit->setGeometry(10,75,355,19);
  cut_outcue_edit->setMaxLength(64);
  QLabel *cut_outcue_label=new QLabel(cut_outcue_edit,tr("&Outcue"),this,
				       "cut_outcue_label");
  cut_outcue_label->setGeometry(15,56,120,19);
  cut_outcue_label->setFont(font);
  cut_outcue_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Cut ISCI Code
  //
  cut_isci_edit=new QLineEdit(this,"cut_isci_edit");
  cut_isci_edit->setGeometry(10,120,355,19);
  cut_isci_edit->setMaxLength(32);
  QLabel *cut_isci_label=new QLabel(cut_isci_edit,tr("&ISCI Code"),this,
				       "cut_isci_label");
  cut_isci_label->setGeometry(15,101,120,19);
  cut_isci_label->setFont(font);
  cut_isci_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Cut Origin
  //
  cut_origin_edit=new QLineEdit(this,"cut_origin_edit");
  cut_origin_edit->setGeometry(10,165,190,19);
  cut_origin_edit->setReadOnly(true);
  cut_origin_edit->setMaxLength(64);
  QLabel *cut_origin_label=new QLabel(cut_origin_edit,tr("Origin"),this,
				       "cut_origin_label");
  cut_origin_label->setGeometry(15,146,120,19);
  cut_origin_label->setFont(font);
  cut_origin_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Cut ISRC
  //
  cut_isrc_edit=new QLineEdit(this,"cut_isrc_edit");
  cut_isrc_edit->setGeometry(220,165,145,19);
  cut_isrc_edit->setMaxLength(64);
  QLabel *cut_isrc_label=new QLabel(cut_isrc_edit,tr("ISRC"),this,
				       "cut_isrc_label");
  cut_isrc_label->setGeometry(225,146,120,19);
  cut_isrc_label->setFont(font);
  cut_isrc_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Cut Weight
  //
  cut_weight_box=new QSpinBox(this,"cut_weight_box");
  cut_weight_box->setGeometry(10,210,61,19);
  cut_weight_box->setRange(0,100);
  QLabel *cut_weight_label=new QLabel(cut_weight_box,tr("Weight"),this,
				       "cut_weight_label");
  cut_weight_label->setGeometry(10,191,61,19);
  cut_weight_label->setFont(font);
  cut_weight_label->setAlignment(AlignHCenter|ShowPrefix);

  //
  // Cut Play Date Time
  //
  cut_playdate_edit=new QLineEdit(this,"cut_playdate_edit");
  cut_playdate_edit->setGeometry(100,210,150,19);
  cut_playdate_edit->setReadOnly(true);
  cut_playdate_edit->setMaxLength(64);
  QLabel *cut_playdate_label=new QLabel(cut_playdate_edit,tr("Last Played"),
					this,"cut_playdate_label");
  cut_playdate_label->setGeometry(105,191,120,19);
  cut_playdate_label->setFont(font);
  cut_playdate_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Cut Play Counter
  //
  cut_playcounter_edit=new QLineEdit(this,"cut_playcounter_edit");
  cut_playcounter_edit->setGeometry(285,210,80,19);
  cut_playcounter_edit->setAlignment(AlignRight);
  cut_playcounter_edit->setReadOnly(true);
  cut_playcounter_edit->setMaxLength(64);
  QLabel *cut_playcounter_label=
    new QLabel(cut_playcounter_edit,tr("# of Plays"),
	       this,"cut_playcounter_label");
  cut_playcounter_label->setGeometry(290,191,120,19);
  cut_playcounter_label->setFont(font);
  cut_playcounter_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Evergreen Checkbox
  //
  rec_evergreen_box=new QCheckBox(this,"rec_evergreen_box");
  rec_evergreen_box->setGeometry(10,245,15,15);
  rec_evergreen_label=new QLabel(rec_evergreen_box,tr("Cut is EVERGREEN"),
				 this,"rec_evergreen_label");
  rec_evergreen_label->setGeometry(30,245,sizeHint().width()-40,15);
  rec_evergreen_label->setFont(font);
  rec_evergreen_label->setAlignment(AlignVCenter|AlignLeft);
  connect(rec_evergreen_box,SIGNAL(toggled(bool)),
	  this,SLOT(evergreenToggledData(bool)));

  //
  // Cut Air Date Times
  //
  cut_killdatetime_label=new QLabel(tr("Air Date/Time"),
				    this,"cut_killdatetime_label");
  cut_killdatetime_label->setGeometry(50,268,100,19);
  cut_killdatetime_label->setAlignment(Qt::AlignHCenter);
  cut_killdatetime_label->setFont(font);
  QButtonGroup *button_group=new QButtonGroup(this,"air_dates_group");
  button_group->hide();
  connect(button_group,SIGNAL(clicked(int)),this,SLOT(airDateButtonData(int)));
  cut_startdatetime_enable_button=new QRadioButton(tr("Enabled"),this,
					       "air_date_enabled_button");
  cut_startdatetime_enable_button->setGeometry(40,290,100,20);
  button_group->insert(cut_startdatetime_enable_button,true);
  cut_startdatetime_disable_button=new QRadioButton(tr("Disabled"),this,
						"air_date_disabled_button");
  cut_startdatetime_disable_button->setGeometry(40,310,100,20);
  button_group->insert(cut_startdatetime_disable_button,false);

  cut_startdatetime_edit=new QDateTimeEdit(this,"cut_startdatetime_edit");
  cut_startdatetime_edit->setGeometry(165,289,170,19);
  cut_startdatetime_label=new QLabel(cut_startdatetime_edit,tr("&Start"),this,
				       "cut_startdatetime_label");
  cut_startdatetime_label->setGeometry(120,293,40,12);
  cut_startdatetime_label->setFont(small_font);
  cut_startdatetime_label->setAlignment(AlignRight|ShowPrefix);

  cut_enddatetime_edit=new QDateTimeEdit(this,"cut_enddatetime_edit");
  cut_enddatetime_edit->setGeometry(165,309,170,19);
  cut_enddatetime_label=new QLabel(cut_enddatetime_edit,tr("End"),this,
				       "cut_enddatetime_label");
  cut_enddatetime_label->setGeometry(120,313,40,12);
  cut_enddatetime_label->setFont(small_font);
  cut_enddatetime_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Cut Daypart
  //
  cut_daypart_label=new QLabel(tr("Daypart"),this);
  cut_daypart_label->setGeometry(50,348,65,19);
  cut_daypart_label->setAlignment(Qt::AlignHCenter);
  cut_daypart_label->setFont(font);
  button_group=new QButtonGroup(this,"daypart_group");
  button_group->hide();
  connect(button_group,SIGNAL(clicked(int)),this,SLOT(daypartButtonData(int)));
  cut_starttime_enable_button=new QRadioButton(tr("Enabled"),this,
					       "daypart_enabled_button");
  cut_starttime_enable_button->setGeometry(57,370,100,20);
  button_group->insert(cut_starttime_enable_button,true);
  cut_starttime_disable_button=new QRadioButton(tr("Disabled"),this,
				"daypart_disabled_button");
  cut_starttime_disable_button->setGeometry(57,390,100,20);
  button_group->insert(cut_starttime_disable_button,false);

  cut_starttime_edit=new RDTimeEdit(this,"cut_starttime_edit");
  cut_starttime_edit->setGeometry(222,369,90,19);
  cut_starttime_label=new QLabel(cut_starttime_edit,tr("&Start Time"),this,
				       "cut_starttime_label");
  cut_starttime_label->setGeometry(137,373,80,12);
  cut_starttime_label->setFont(small_font);
  cut_starttime_label->setAlignment(AlignRight|ShowPrefix);

  cut_endtime_edit=new RDTimeEdit(this,"cut_endtime_edit");
  cut_endtime_edit->setGeometry(222,389,90,19);
  cut_endtime_label=new QLabel(cut_endtime_edit,tr("End Time"),this,
				       "cut_endtime_label");
  cut_endtime_label->setGeometry(137,393,80,12);
  cut_endtime_label->setFont(small_font);
  cut_endtime_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Days of the Week
  //
  rec_dayofweek_label=new QLabel(tr("Day of the Week"),this);
  rec_dayofweek_label->setGeometry(50,428,125,19);
  rec_dayofweek_label->setAlignment(Qt::AlignHCenter);
  rec_dayofweek_label->setFont(font);
  rec_weekpart_button[0]=new QCheckBox(this,"cut_weekpart_button[0]");
  rec_weekpart_button[0]->setGeometry(40,447,15,15);
  rec_weekpart_label[0]=new QLabel(rec_weekpart_button[0],tr("Monday"),
		   this,"rec_weekpart_label[0]");
  rec_weekpart_label[0]->setGeometry(62,445,80,20);
  rec_weekpart_label[0]->setFont(day_font);
  rec_weekpart_label[0]->setAlignment(AlignVCenter|AlignLeft);

  rec_weekpart_button[1]=new QCheckBox(this,"cut_weekpart_button[1]");
  rec_weekpart_button[1]->setGeometry(120,447,15,15);
  rec_weekpart_label[1]=new QLabel(rec_weekpart_button[1],tr("Tuesday"),
		   this,"rec_weekpart_label[0]");
  rec_weekpart_label[1]->setGeometry(142,445,80,20);
  rec_weekpart_label[1]->setFont(day_font);
  rec_weekpart_label[1]->setAlignment(AlignVCenter|AlignLeft);

  rec_weekpart_button[2]=new QCheckBox(this,"cut_weekpart_button[2]");
  rec_weekpart_button[2]->setGeometry(200,447,15,15);
  rec_weekpart_label[2]=new QLabel(rec_weekpart_button[2],tr("Wednesday"),
		   this,"rec_weekpart_label[0]");
  rec_weekpart_label[2]->setGeometry(222,445,80,20);
  rec_weekpart_label[2]->setFont(day_font);
  rec_weekpart_label[2]->setAlignment(AlignVCenter|AlignLeft);

  rec_weekpart_button[3]=new QCheckBox(this,"cut_weekpart_button[3]");
  rec_weekpart_button[3]->setGeometry(80,467,15,15);
  rec_weekpart_label[3]=new QLabel(rec_weekpart_button[3],tr("Thursday"),
		   this,"rec_weekpart_label[3]");
  rec_weekpart_label[3]->setGeometry(102,465,80,20);
  rec_weekpart_label[3]->setFont(day_font);
  rec_weekpart_label[3]->setAlignment(AlignVCenter|AlignLeft);

  rec_weekpart_button[4]=new QCheckBox(this,"cut_weekpart_button[4]");
  rec_weekpart_button[4]->setGeometry(180,467,15,15);
  rec_weekpart_label[4]=new QLabel(rec_weekpart_button[4],tr("Friday"),
		   this,"rec_weekpart_label[4]");
  rec_weekpart_label[4]->setGeometry(202,465,80,20);
  rec_weekpart_label[4]->setFont(day_font);
  rec_weekpart_label[4]->setAlignment(AlignVCenter|AlignLeft);

  rec_weekpart_button[5]=new QCheckBox(this,"cut_weekpart_button[5]");
  rec_weekpart_button[5]->setGeometry(80,487,15,15);
  rec_weekpart_label[5]=new QLabel(rec_weekpart_button[5],tr("Saturday"),
		   this,"rec_weekpart_label[5]");
  rec_weekpart_label[5]->setGeometry(102,485,80,20);
  rec_weekpart_label[5]->setFont(day_font);
  rec_weekpart_label[5]->setAlignment(AlignVCenter|AlignLeft);

  rec_weekpart_button[6]=new QCheckBox(this,"cut_weekpart_button[6]");
  rec_weekpart_button[6]->setGeometry(180,485,15,15);
  rec_weekpart_label[6]=new QLabel(rec_weekpart_button[6],tr("Sunday"),
		   this,"rec_weekpart_label[6]");
  rec_weekpart_label[6]->setGeometry(202,485,80,20);
  rec_weekpart_label[6]->setFont(day_font);
  rec_weekpart_label[6]->setAlignment(AlignVCenter|AlignLeft);

  rec_set_button=new QPushButton(tr("Set All"),this,"rec_set_button");
  rec_set_button->setGeometry(sizeHint().width()-80,441,55,30);
  rec_set_button->setFont(small_font);
  connect(rec_set_button,SIGNAL(clicked()),this,SLOT(setAllData()));

  rec_clear_button=new QPushButton(tr("Clear All"),this,"rec_clear_button");
  rec_clear_button->setGeometry(sizeHint().width()-80,476,55,30);
  rec_clear_button->setFont(small_font);
  connect(rec_clear_button,SIGNAL(clicked()),this,SLOT(clearAllData()));

  //
  // Audio Meter
  //
  rec_meter=new RDStereoMeter(this,"rec_meter");
  rec_meter->setGeometry(20,520,rec_meter->geometry().width(),
			 rec_meter->geometry().height());
  rec_meter->setReference(0);
  rec_meter->setMode(RDSegMeter::Independent);
  QTimer *timer=new QTimer(this,"meter_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(meterData()));
  timer->start(RD_METER_UPDATE_INTERVAL);

  //
  // AES Alarm
  //
  rec_aes_alarm_label=new QLabel(this,"rec_aes_alarm_label");
  rec_aes_alarm_label->setGeometry(15,592,110,22);
  rec_aes_alarm_label->setAlignment(AlignHCenter|AlignVCenter);
  rec_aes_alarm_label->setFont(large_font);
  rec_aes_alarm_label->setPalette(QColor(red));
  rec_aes_alarm_label->setText(tr("AES ALARM"));
  rec_aes_alarm_label->hide();

  //
  // Record Timer
  //
  rec_timer=new QTimer(this,"rec_timer");
  connect(rec_timer,SIGNAL(timeout()),this,SLOT(recTimerData()));
  rec_timer_label=new QLabel(this,"rec_timer_label");
  rec_timer_label->setGeometry(130,580,120,50);
  rec_timer_label->setFont(timer_font);
  rec_timer_label->setAlignment(AlignLeft|AlignVCenter);
  rec_timer_label->setText(RDGetTimeLength(rec_length,true));

  //
  // Channels
  //
  rec_channels_box=new QComboBox(this,"rec_channels_box");
  rec_channels_box->setGeometry(20,635,60,35);
  rec_channels_edit=new QLineEdit(this,"rec_channels_box");
  rec_channels_edit->setGeometry(20,635,60,35);
  QLabel *rec_channels_box_label=new QLabel(rec_channels_box,tr("Channels"),
					    this,"rec_channels_box_label");
  rec_channels_box_label->setGeometry(10,616,80,16);
  rec_channels_box_label->setAlignment(AlignHCenter);
  rec_channels_box_label->setFont(font);
  connect(rec_channels_box,SIGNAL(activated(int)),
	  this,SLOT(channelsData(int)));

  //
  //  Record Button
  //
  rec_record_button=new RDTransportButton(RDTransportButton::Record,
					this,"rec_record_button");
  rec_record_button->setGeometry(100,620,80,50);
  rec_record_button->setDefault(true);
  connect(rec_record_button,SIGNAL(clicked()),this,SLOT(recordData()));

  //
  //  Play Button
  //
  rec_play_button=new RDTransportButton(RDTransportButton::Play,
					this,"rec_play_button");
  rec_play_button->setGeometry(190,620,80,50);
  rec_play_button->setDefault(true);
  connect(rec_play_button,SIGNAL(clicked()),this,SLOT(playData()));

  //
  //  Stop Button
  //
  rec_stop_button=new RDTransportButton(RDTransportButton::Stop,
					this,"rec_stop_button");
  rec_stop_button->setGeometry(280,620,80,50);
  rec_stop_button->setDefault(true);
  rec_stop_button->setState(RDTransportButton::On);
  rec_stop_button->setOnColor(QColor(red));
  connect(rec_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  //
  // Record Mode 
  //
  rec_mode_box=new QComboBox(this,"rec_mode_box");
  rec_mode_box->setGeometry(10,695,100,35);
  QLabel *rec_mode_box_label=new QLabel(rec_mode_box,tr("Record Mode"),this,
					"rec_mode_box_label");
  rec_mode_box_label->setGeometry(10,676,100,16);
  rec_mode_box_label->setAlignment(AlignHCenter);
  rec_mode_box_label->setFont(font);

  //
  // AutoTrim Mode 
  //
  rec_trim_box=new QComboBox(this,"rec_trim_box");
  rec_trim_box->setGeometry(145,695,70,35);
  QLabel *rec_trim_box_label=new QLabel(rec_trim_box,tr("AutoTrim"),this,
					"rec_trim_box_label");
  rec_trim_box_label->setGeometry(130,676,100,16);
  rec_trim_box_label->setAlignment(AlignHCenter);
  rec_trim_box_label->setFont(font);

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  close_button->setDefault(true);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Fields
  //
  cut_description_edit->setText(rec_cut->description());
  cut_outcue_edit->setText(rec_cut->outcue());
  cut_origin_name=rec_cut->originName();
  cut_origin_datetime=rec_cut->originDatetime(&valid);
  if(valid) {
    cut_origin_edit->setText(cut_origin_name+" - "+
	       	     cut_origin_datetime.toString("M/d/yyyy hh:mm:ss"));
  }
  cut_isci_edit->setText(rec_cut->isci());
  cut_isrc_edit->setText(rec_cut->isrc(RDCut::FormattedIsrc));
  cut_weight_box->setValue(rec_cut->weight());
  if(rec_cut->playCounter()>0) {
    cut_playdate_edit->
      setText(rec_cut->lastPlayDatetime(&valid).toString("M/d/yyyy hh:mm:ss"));
  }
  else {
    cut_playdate_edit->setText("Never");
  }
  cut_playcounter_edit->
    setText(QString().sprintf("%d",rec_cut->playCounter()));
  rec_evergreen_box->setChecked(rec_cut->evergreen());
  evergreenToggledData(rec_evergreen_box->isChecked());
  cut_startdatetime_edit->setDateTime(rec_cut->startDatetime(&valid));
  cut_enddatetime_edit->setDateTime(rec_cut->endDatetime(&valid));
  cut_startdatetime_enable_button->setChecked(valid);
  cut_startdatetime_disable_button->setChecked(!valid);
  airDateButtonData(valid);
  cut_starttime_edit->setTime(rec_cut->startDaypart(&valid));
  cut_endtime_edit->setTime(rec_cut->endDaypart(&valid));
  cut_starttime_enable_button->setChecked(valid);
  cut_starttime_disable_button->setChecked(!valid);
  daypartButtonData(valid);
  for(int i=0;i<7;i++) {
    if(rec_cut->weekPart(i+1)) {
      rec_weekpart_button[i]->setChecked(true);
    }
  }
  rec_channels_box->insertItem("1");
  rec_channels_box->insertItem("2");
  rec_channels_box->setCurrentItem(rec_cut->channels()-1);
  rec_channels_edit->setText(QString().sprintf("%d",rec_cut->channels()));
  rec_mode_box->insertItem(tr("Manual"));
  rec_mode_box->insertItem(tr("VOX"));
  switch(rdlibrary_conf->defaultRecordMode()) {
      case RDLibraryConf::Manual:
	rec_mode_box->setCurrentItem(0);
	break;

      case RDLibraryConf::Vox:
	rec_mode_box->setCurrentItem(1);
	break;
  }
  rec_trim_box->insertItem(tr("On"));
  rec_trim_box->insertItem(tr("Off"));
  if(rdlibrary_conf->defaultTrimState()) {
    rec_trim_box->setCurrentItem(0);
  }
  else {
    rec_trim_box->setCurrentItem(1);
  }
  aesAlarmData(rec_card_no[0],rec_port_no[0],
	       rdcae->inputStatus(rec_card_no[0],rec_port_no[0]));

  //
  // Set Control Perms
  //
  cut_description_edit->setReadOnly(!allow_modification);
  cut_outcue_edit->setReadOnly(!allow_modification);
  cut_isci_edit->setReadOnly(!allow_modification);
  cut_starttime_edit->setReadOnly(!allow_modification);
  cut_endtime_edit->setReadOnly(!allow_modification);
  if(!allow_modification) {
    cut_weight_box->setRange(cut_weight_box->value(),cut_weight_box->value());
    if(cut_startdatetime_enable_button->isChecked()) {
      cut_startdatetime_edit->dateEdit()->
	setRange(cut_startdatetime_edit->dateEdit()->date(),
		 cut_startdatetime_edit->dateEdit()->date());
      cut_startdatetime_edit->timeEdit()->
	setRange(cut_startdatetime_edit->timeEdit()->time(),
		 cut_startdatetime_edit->timeEdit()->time());
      cut_enddatetime_edit->dateEdit()->
	setRange(cut_enddatetime_edit->dateEdit()->date(),
		 cut_enddatetime_edit->dateEdit()->date());
    }
    if(cut_starttime_enable_button->isChecked()) {
      cut_enddatetime_edit->timeEdit()->
	setRange(cut_enddatetime_edit->timeEdit()->time(),
		 cut_enddatetime_edit->timeEdit()->time());
    }
  }
  rec_evergreen_box->setEnabled(allow_modification);
  if(!allow_modification) {
    cut_startdatetime_enable_button->setDisabled(true);
    cut_startdatetime_disable_button->setDisabled(true);
    cut_starttime_enable_button->setDisabled(true);
    cut_starttime_disable_button->setDisabled(true);
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
  return QSize(375,740);
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
      cut_startdatetime_edit->
	setDateTime(QDateTime(QDate::currentDate(),QTime()));
      cut_enddatetime_edit->
	setDateTime(QDateTime(QDate::currentDate(),QTime(23,59,59)));
    }
  }
}


void RecordCut::daypartButtonData(int id)
{
  cut_starttime_edit->setEnabled(id);
  cut_starttime_label->setEnabled(id);
  cut_endtime_edit->setEnabled(id);
  cut_endtime_label->setEnabled(id);
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
    cart->removeCutAudio(rdstation_conf,lib_user,rec_cut->cutName(),lib_config);
    delete cart;
    switch(rdlibrary_conf->defaultFormat()) {
	case 0:
	  rec_cut->setCodingFormat(0);
	  rec_format=RDCae::Pcm16;
	  break;

	case 1:
	  rec_cut->setCodingFormat(1);
	  rec_format=RDCae::MpegL2;
	  break;

	default:
	  rec_cut->setCodingFormat(0);
	  rec_format=RDCae::Pcm16;
	  break;
    }
    rec_samprate=lib_system->sampleRate();
    rec_cut->setSampleRate(rec_samprate);
    rec_bitrate=rdlibrary_conf->defaultBitrate();
    rec_cut->setBitRate(rec_bitrate);
    rec_channels=rec_channels_box->currentItem()+1;
    rec_cut->setChannels(rec_channels);
    rec_cut->setOriginDatetime(QDateTime::currentDateTime());
    rec_cut->setOriginName(rdstation_conf->name());
    cut_origin_name=rdstation_conf->name();
    cut_origin_datetime=QDateTime::currentDateTime();
    cut_origin_edit->setText(cut_origin_name+" - "+
			    cut_origin_datetime.toString("M/d/yyyy hh:mm:ss"));
    rdcae->loadRecord(rec_card_no[0],rec_port_no[0],rec_name,rec_format,
			rec_channels,rec_samprate,rec_bitrate*rec_channels);
  }
}


void RecordCut::playData()
{
  int start=rec_cut->startPoint(true);
  int end=rec_cut->endPoint(true);

  if((!is_recording)&&(!is_playing)&&(!is_ready)) {  // Start Play
    rdcae->loadPlay(rec_card_no[1],rec_cut->cutName(),
		    &rec_stream_no[1],&rec_play_handle);
    RDSetMixerOutputPort(rdcae,rec_card_no[1],rec_stream_no[1],rec_port_no[1]);
    rdcae->positionPlay(rec_play_handle,start);
    rdcae->setPlayPortActive(rec_card_no[1],rec_port_no[1],rec_stream_no[1]);
    rdcae->setOutputVolume(rec_card_no[1],rec_stream_no[1],rec_port_no[1],
           0+rec_cut->playGain());
    rdcae->play(rec_play_handle,end-start,RD_TIMESCALE_DIVISOR,false);
  }
  if(is_ready&&(!is_recording)) {
    if(rec_mode_box->currentItem()==1) {
      rdcae->
	record(rec_card_no[0],rec_port_no[0],rdlibrary_conf->maxLength(),
	       rdlibrary_conf->voxThreshold());
    }
    else {
      rdcae->
	record(rec_card_no[0],rec_port_no[0],rdlibrary_conf->maxLength(),0);
    }
  }
}


void RecordCut::stopData()
{
  if(is_playing) {
    rdcae->stopPlay(rec_play_handle);
    return;
  }
  if(is_recording) {
    rdcae->stopRecord(rec_card_no[0],rec_port_no[0]);
    return;
  }
  if(is_ready) {
    rdcae->unloadRecord(rec_card_no[0],rec_port_no[0]);
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
  rdcae->unloadPlay(rec_play_handle);
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
  rdcae->unloadRecord(rec_card_no[0],rec_port_no[0]);
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
  rec_cut->checkInRecording(rdstation_conf->name(),s,len);
  if(rec_trim_box->currentItem()==0) {
    rec_cut->autoTrim(RDCut::AudioBoth,rdlibrary_conf->trimThreshold());
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
    if((cut_enddatetime_edit->dateTime()<QDate::currentDate())&&
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
  if(cut_starttime_enable_button->isChecked()) {
    if(!cut_starttime_edit->time().isValid()) {
      QMessageBox::warning(this,tr("Invalid Time"),
			   tr("The Start Time is invalid!"));
      return;
    }
    if(!cut_endtime_edit->time().isValid()) {
      QMessageBox::warning(this,tr("Invalid Time"),
			   tr("The End Time is invalid!"));
      return;
    }
    if(cut_endtime_edit->time()==cut_starttime_edit->time()) {
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
     (cut_startdatetime_edit->dateTime().time().isNull())) {
    rec_cut->
      setStartDatetime(QDateTime(cut_startdatetime_edit->dateTime().date(),
				 cut_startdatetime_edit->dateTime().time().
				 addMSecs(1)),true);
  }
  else {
    rec_cut->setStartDatetime(cut_startdatetime_edit->dateTime(),
			      cut_startdatetime_enable_button->isChecked());
  }
  if((cut_startdatetime_enable_button->isChecked())&&
     (cut_enddatetime_edit->dateTime().time().isNull())) {
    rec_cut->
      setEndDatetime(QDateTime(cut_enddatetime_edit->dateTime().date(),
				 cut_enddatetime_edit->dateTime().time().
				 addMSecs(1)),true);
  }
  else {
    rec_cut->setEndDatetime(cut_enddatetime_edit->dateTime(),
			    cut_startdatetime_enable_button->isChecked());
  }
  rec_cut->setStartDaypart(cut_starttime_edit->time(),
			   cut_starttime_enable_button->isChecked());
  rec_cut->setEndDaypart(cut_endtime_edit->time(),
			 cut_starttime_enable_button->isChecked());
  for(int i=0;i<7;i++) {
    rec_cut->setWeekPart(i+1,rec_weekpart_button[i]->isChecked());
  }
  rec_cut->setDescription(cut_description_edit->text());
  rec_cut->setOutcue(cut_outcue_edit->text());
  rec_cut->setIsrc(isrc);
  rec_cut->setIsci(cut_isci_edit->text());
  rec_cut->setWeight(cut_weight_box->value());
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
  if((card==rdlibrary_conf->inputCard())&&
     (port==rdlibrary_conf->inputPort())) {
    if(rdaudioport_conf->inputPortType(rdlibrary_conf->inputPort())!=
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
    rdcae->inputMeterUpdate(rec_card_no[0],rec_port_no[0],levels);
    rec_meter->setLeftSolidBar(levels[0]);
    rec_meter->setRightSolidBar(levels[1]);
  }
  if(is_playing) {
    rdcae->outputMeterUpdate(rec_card_no[1],rec_port_no[1],levels);
    rec_meter->setLeftSolidBar(levels[0]);
    rec_meter->setRightSolidBar(levels[1]);
  }
}


void RecordCut::evergreenToggledData(bool state)
{
  cut_killdatetime_label->setDisabled(state);
  cut_startdatetime_enable_button->setDisabled(state);
  cut_startdatetime_disable_button->setDisabled(state);
  cut_startdatetime_label->setDisabled(state);
  cut_startdatetime_edit->setDisabled(state);
  cut_enddatetime_edit->setDisabled(state);
  cut_enddatetime_label->setDisabled(state);
  cut_daypart_label->setDisabled(state);
  cut_starttime_enable_button->setDisabled(state);
  cut_starttime_disable_button->setDisabled(state);
  cut_starttime_edit->setDisabled(state);
  cut_starttime_label->setDisabled(state);
  cut_endtime_edit->setDisabled(state);
  cut_endtime_label->setDisabled(state);
  rec_dayofweek_label->setDisabled(state);
  for(int i=0;i<7;i++) {
    rec_weekpart_button[i]->setDisabled(state);
    rec_weekpart_label[i]->setDisabled(state);
  }
  rec_set_button->setDisabled(state);
  rec_clear_button->setDisabled(state);
  if (!state) {
    airDateButtonData(cut_startdatetime_enable_button->isChecked());
    daypartButtonData(cut_starttime_enable_button->isChecked());
  }

  update();
}


void RecordCut::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  if(rec_evergreen_box->isChecked()) {
    p->setPen(palette().color(QPalette::Disabled,QColorGroup::Foreground));
  }
  else {
    p->setPen(palette().color(QPalette::Active,QColorGroup::Foreground));
  }
  p->drawRect(30,275,sizeHint().width()-60,60);
  p->drawRect(37,355,sizeHint().width()-74,60);
  p->drawRect(20,436,sizeHint().width()-40,75);
  p->end();
}


void RecordCut::closeEvent(QCloseEvent *e)
{
  closeData();
}


void RecordCut::AutoTrim(RDWaveFile *name)
{
  if(name->hasEnergy()) {
    rec_cut->setStartPoint(name->startTrim(rdlibrary_conf->trimThreshold()));
    rec_cut->setEndPoint(name->endTrim(rdlibrary_conf->trimThreshold()));
  }
}
