// eventwidget.cpp
//
// Widget for editing a LogManager event
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

#include <QPainter>

#include <rdconf.h>
#include <rdescape_string.h>

#include "eventwidget.h"

EventWidget::EventWidget(QWidget *parent)
  : RDWidget(parent)
{
  event_event=NULL;

  // *******************************
  // Pre-Position Log Section
  // *******************************
  event_position_group=new QGroupBox(tr("PRE-POSITION LOG"),this);
  event_position_group->setFont(labelFont());

  event_position_box=new QCheckBox(this);
  connect(event_position_box,SIGNAL(toggled(bool)),
	  this,SLOT(prepositionToggledData(bool)));
  event_position_label=new QLabel(tr("Cue to this event"),this);
  event_position_label->setFont(labelFont());
  event_position_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_position_edit=new QTimeEdit(this);
  event_position_edit->setDisplayFormat("mm:ss");
  event_position_unit=new QLabel(tr("before scheduled start.  (First cart will have a STOP transition.)"),
				 this);
  event_position_unit->setFont(labelFont());
  event_position_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  // *******************************
  // Timed Start Section
  // *******************************
  event_timetype_group=new QGroupBox(tr("TIMED START"),this);
  event_timetype_group->setFont(labelFont());

  //
  // Time Type
  //
  event_timetype_check=new QCheckBox(this);
  event_timetype_label=new QLabel(tr("Use hard start time"),this);
  event_timetype_label->setFont(labelFont());
  event_timetype_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Grace Time
  //
  event_grace_groupbox=
    new QGroupBox(tr("Action If Previous Event Still Playing"),this);
  event_grace_groupbox->setFont(labelFont());
  event_grace_group=new QButtonGroup(this);
  event_immediate_button=new QRadioButton(tr("Start immediately"),this);
  event_immediate_button->setFont(subLabelFont());
  event_grace_group->addButton(event_immediate_button,0);
  event_next_button=new QRadioButton(tr("Make next"),this);
  event_next_button->setFont(subLabelFont());
  event_grace_group->addButton(event_next_button,1);
  event_wait_button=new QRadioButton(tr("Wait up to"),this);
  event_wait_button->setFont(subLabelFont());
  event_grace_group->addButton(event_wait_button,2);

  event_grace_edit=new QTimeEdit(this);
  event_grace_edit->setDisplayFormat("mm:ss");
  connect(event_timetype_check,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(event_grace_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(graceClickedData(int)));

  // *******************************
  // Transitions Section
  // *******************************
  event_transitions_group=new QGroupBox(tr("TRANSITIONS"),this);
  event_transitions_group->setFont(labelFont());

  //
  // First Cart Transition Type
  //
  event_firsttrans_label=
    new QLabel(tr("First cart has a"),this);
  event_firsttrans_label->setFont(labelFont());
  event_firsttrans_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_firsttrans_box=new QComboBox(this);
  event_firsttrans_box->insertItem(0,tr("Play"));
  event_firsttrans_box->insertItem(1,tr("Segue"));
  event_firsttrans_box->insertItem(2,tr("Stop"));
  event_firsttrans_unit=new QLabel("transition.",this);
  event_firsttrans_unit->setFont(labelFont());
  event_firsttrans_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(event_firsttrans_box,SIGNAL(activated(int)),
	  this,SLOT(timeTransitionData(int)));
  
  //
  // Default Transition Type
  //
  event_defaulttrans_label=new QLabel(tr("Imported carts have a"),this);
  event_defaulttrans_label->setFont(labelFont());
  event_defaulttrans_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_defaulttrans_box=new QComboBox(this);
  event_defaulttrans_box->insertItem(0,tr("Play"));
  event_defaulttrans_box->insertItem(1,tr("Segue"));
  event_defaulttrans_box->insertItem(2,tr("Stop"));
  event_defaulttrans_unit=new QLabel(tr("transition."),this);
  event_defaulttrans_unit->setFont(labelFont());
  event_defaulttrans_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  // *******************************
  // Enforcing Length Section
  // *******************************
  event_autofill_group=new QGroupBox(tr("ENFORCING LENGTH"),this);
  event_autofill_group->setFont(labelFont());

  event_autofill_box=new QCheckBox(this);
  connect(event_autofill_box,SIGNAL(toggled(bool)),
	  this,SLOT(autofillToggledData(bool)));
  event_autofill_label=new QLabel(tr("Use AutoFill"),this);
  event_autofill_label->setFont(labelFont());
  event_autofill_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  event_autofill_slop_box=new QCheckBox(this);
  connect(event_autofill_slop_box,SIGNAL(toggled(bool)),
	  this,SLOT(autofillWarnToggledData(bool)));
  event_autofill_slop_label1=
    new QLabel(tr("Warn if fill is over or under"),this);
  event_autofill_slop_label1->setFont(labelFont());
  event_autofill_slop_label1->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  event_autofill_slop_edit=new QTimeEdit(this);
  event_autofill_slop_edit->setDisplayFormat("mm:ss");
  event_autofill_slop_label=new QLabel(tr("by at least")+" ",this);
  event_autofill_slop_label->setFont(labelFont());
  event_autofill_slop_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  event_timescale_box=new QCheckBox(this);
  event_timescale_label=new QLabel(tr("Use Timescaling"),this);
  event_timescale_label->setFont(labelFont());
  event_timescale_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_timescale_box->hide();
  event_timescale_label->hide();

  // *******************************
  // Cart Stack Section
  // *******************************
  event_stack_group=new QGroupBox(tr("CART STACK"),this);
  event_stack_group->setFont(labelFont());

  // *******************************
  // Pre-Import Events Section
  // *******************************
  event_preimport_widget=
    new ImportCartsWidget(ImportCartsModel::PreImport,this);

  //
  // Imports
  //
  event_imports_label=new QLabel(tr("IMPORT"),this);
  event_imports_label->setFont(labelFont());
  
  event_source_group=new QButtonGroup(this);
  connect(event_source_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(importClickedData(int)));
  event_source_none_radio=new QRadioButton(this);
  event_source_group->addButton(event_source_none_radio,RDEventLine::None);
  event_source_group_none_label=new QLabel(tr("None"),this);
  event_source_group_none_label->setFont(labelFont());
  event_source_group_none_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  event_source_traffic_radio=new QRadioButton(this);
  event_source_group->addButton(event_source_traffic_radio,RDEventLine::Traffic);
  event_source_group_traffic_label=new QLabel(tr("From Traffic"),this);
  event_source_group_traffic_label->setFont(labelFont());
  event_source_group_traffic_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  event_source_music_radio=new QRadioButton(this);
  event_source_group->addButton(event_source_music_radio,RDEventLine::Music);
  event_source_group_music_label=new QLabel(tr("From Music"),this);
  event_source_group_music_label->setFont(labelFont());
  event_source_group_music_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  event_source_scheduler_radio=new QRadioButton(this);
  event_source_group->addButton(event_source_scheduler_radio,RDEventLine::Scheduler);
  event_source_group_scheduler_label=new QLabel(tr("Select from:"),this);
  event_source_group_scheduler_label->setFont(labelFont());
  event_source_group_scheduler_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  //
  // Scheduler Group
  //
  event_sched_group_box=new RDComboBox(this);

  // 
  // Artist Separation SpinBox
  //
  event_artist_sep_label=new QLabel(tr("Artist Separation"),this);
  event_artist_sep_label->setFont(defaultFont());
  event_artist_sep_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  
  event_artist_sep_spinbox=new QSpinBox(this);
  event_artist_sep_spinbox->setMinimum( -1 );
  event_artist_sep_spinbox->setMaximum( 50000 );
  event_artist_sep_spinbox->setSpecialValueText("None");

  event_artist_none_button=new QPushButton(this);
  event_artist_none_button->setFont(subButtonFont());
  event_artist_none_button->setText(tr("None"));
  connect(event_artist_none_button,SIGNAL(clicked()),this,SLOT(artistData()));

  //
  // Title Separation SpinBox
  //
  event_title_sep_label=new QLabel(tr("Title Separation"),this);
  event_title_sep_label->setFont(defaultFont());
  event_title_sep_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  
  event_title_sep_spinbox=new QSpinBox(this);
  event_title_sep_spinbox->setMinimum( -1 );
  event_title_sep_spinbox->setMaximum( 50000 );
  event_title_sep_spinbox->setSpecialValueText("None");

  event_title_none_button=new QPushButton(this);
  event_title_none_button->setFont(subButtonFont());
  event_title_none_button->setText(tr("None"));
  connect(event_title_none_button,SIGNAL(clicked()),this,SLOT(titleData()));

  //
  // Must have code..
  //
  event_have_code_label=new QLabel(tr("Must have code"),this);
  event_have_code_label->setFont(defaultFont());
  event_have_code_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  
  event_have_code_box=new RDComboBox(this);

  //
  // And code
  //
  event_have_code2_label=new QLabel(tr("and code"),this);
  event_have_code2_label->setFont(defaultFont());
  event_have_code2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  event_have_code2_box=new RDComboBox(this);

  //
  // Start Slop Time
  //
  event_startslop_label=new QLabel(tr("Import carts scheduled"),this);
  event_startslop_label->setFont(defaultFont());
  event_startslop_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_startslop_edit=new QTimeEdit(this);
  event_startslop_edit->setDisplayFormat("mm:ss");
  event_startslop_unit=new QLabel(tr("prior to the start of this event."),this);
  event_startslop_unit->setFont(defaultFont());
  event_startslop_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  //
  // End Slop Time
  //
  event_endslop_label=new QLabel(tr("Import carts scheduled"),this);
  event_endslop_label->setFont(defaultFont());
  event_endslop_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_endslop_edit=new QTimeEdit(this);
  event_endslop_edit->setDisplayFormat("mm:ss");
  event_endslop_unit=new QLabel(tr("after the end of this event."),this);
  event_endslop_unit->setFont(defaultFont());
  event_endslop_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  //
  // Nested Event
  //
  event_nestevent_label=new QLabel(tr("Import inline traffic event"),this);
  event_nestevent_label->setFont(defaultFont());
  event_nestevent_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_nestevent_box=new QComboBox(this);
  event_nestevent_box->insertItem(0,tr("[none]"));

  // *******************************
  // Post-Import Carts Section
  // *******************************
  event_postimport_widget=new ImportCartsWidget(ImportCartsModel::PostImport,
						this);
}


EventWidget::~EventWidget()
{
}


QSize EventWidget::sizeHint() const
{
  return QSize(624,645);
}


QSizePolicy EventWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding);
}


QString EventWidget::properties() const
{
  int prepos_msec=-1;
  RDLogLine::TimeType time_type=RDLogLine::Relative;
  int grace_msec=-1;

  if(event_position_box->isChecked()) {
    prepos_msec=QTime(0,0,0).msecsTo(event_position_edit->time());
  }
  if(event_timetype_check->isChecked()) {
    time_type=RDLogLine::Hard;
  }
  if(event_timetype_check->isChecked()) {
    switch(event_grace_group->checkedId()) {
    case 0:
      grace_msec=0;
      break;

    case 1:
      grace_msec=-1;
      break;

    default:
      grace_msec=QTime(0,0,0).msecsTo(event_grace_edit->time());
      break;	  
    }
  }

  return RDEventLine::
    propertiesText(prepos_msec,
		   (RDLogLine::TransType)event_firsttrans_box->currentIndex(),
		   time_type,
		   grace_msec,
		   event_autofill_box->isChecked(),
		   (RDEventLine::ImportSource)event_source_group->checkedId(),
		   event_nestevent_box->currentIndex()>0);
}


void EventWidget::rename(const QString &str)
{
  event_preimport_widget->setEventName(str);
  event_postimport_widget->setEventName(str);
}


void EventWidget::load(RDEvent *evt)
{
  QString sql;
  RDSqlQuery *q=NULL;
  int pos;
  int grace;

  event_event=evt;

  //
  // Fill scheduler codes
  //
  event_have_code_box->clear();
  event_have_code2_box->clear();
  event_have_code_box->insertItem(tr("[None]"));
  event_have_code2_box->insertItem(tr("[None]"));

  sql="select `CODE` from `SCHED_CODES` order by `CODE`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_have_code_box->insertItem(q->value(0).toString());
    event_have_code2_box->insertItem(q->value(0).toString());
  }
  delete q;

  //
  // Group Names
  //
  event_sched_group_box->clear();
  sql=QString("select ")+
    "`NAME` "+  // 00
    "from `GROUPS` order by `NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_sched_group_box->insertItem(q->value(0).toString());
  }
  delete q;

  //
  // Nested Event Names
  //
  event_nestevent_box->clear();
  event_nestevent_box->insertItem(0,tr("[none]"));
  QString str=event_event->nestedEvent();
  sql=QString("select `NAME` from `EVENTS` where ")+
    "`NAME`!='"+RDEscapeString(evt->name())+"'"+
    "order by `NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_nestevent_box->insertItem(event_nestevent_box->count(),
				    q->value(0).toString());
    if(q->value(0).toString()==str) {
      event_nestevent_box->setCurrentIndex(event_nestevent_box->count()-1);
    }
  }
  delete q;

  //
  // Import Events
  //
  event_preimport_widget->load(event_event);
  event_postimport_widget->load(event_event);

  pos=event_event->preposition();
  if(pos<0) {
    event_position_box->setChecked(false);
    prepositionToggledData(false);
  }
  else {
    event_position_box->setChecked(true);
    event_position_edit->setTime(QTime(0,0,0).addMSecs(pos));
    prepositionToggledData(true);
  }
  
  grace=0;
  switch(event_event->timeType()) {
  case RDLogLine::Relative:
    event_timetype_check->setChecked(false);
    event_grace_group->button(0)->setChecked(true);
    timeToggledData(false);
    break;
	
  case RDLogLine::Hard:
    event_timetype_check->setChecked(true);
    switch((grace=event_event->graceTime())) {
    case 0:
      event_grace_group->button(0)->setChecked(true);
      event_grace_edit->setTime(QTime());
      break;
	      
    case -1:
      event_grace_group->button(1)->setChecked(true);
      event_grace_edit->setTime(QTime());
      break;
	      
    default:
      event_grace_group->button(2)->setChecked(true);
      event_grace_edit->setTime(QTime(0,0,0).addMSecs(grace));
      break;
    }

  case RDLogLine::NoTime:
    break;
  }

  //
  // General Controls
  //
  event_autofill_box->setChecked(event_event->useAutofill());
  int autofill_slop=event_event->autofillSlop();
  if(autofill_slop>=0) {
    event_autofill_slop_box->setChecked(true);
    event_autofill_slop_edit->setTime(QTime(0,0,0).addMSecs(autofill_slop));
  }
  autofillToggledData(event_autofill_box->isChecked());
  event_timescale_box->setChecked(event_event->useTimescale());
  event_source_group->button(event_event->importSource())->setChecked(true);
  event_startslop_edit->setTime(QTime(0,0,0).addMSecs(event_event->startSlop()));
  event_endslop_edit->setTime(QTime(0,0,0).addMSecs(event_event->endSlop()));
  event_firsttrans_box->setCurrentIndex(event_event->firstTransType());
  event_defaulttrans_box->setCurrentIndex(event_event->defaultTransType());
  if(!event_event->schedGroup().isEmpty()) {
    event_sched_group_box->setCurrentText(event_event->schedGroup());
  }
  event_artist_sep_spinbox->setValue(event_event->artistSep());
  event_title_sep_spinbox->setValue(event_event->titleSep());
  if(event_have_code_box->findText(event_event->HaveCode())!=-1) {
    event_have_code_box->setCurrentText(event_event->HaveCode());
  }
  if(event_have_code2_box->findText(event_event->HaveCode2())!=-1) {
    event_have_code2_box->setCurrentText(event_event->HaveCode2());
  }


  pos=event_event->preposition();
  if(pos<0) {
    event_position_box->setChecked(false);
    prepositionToggledData(false);
  }
  else {
    event_position_box->setChecked(true);
    event_position_edit->setTime(QTime(0,0,0).addMSecs(pos));
    prepositionToggledData(true);
  }
  
  grace=0;
  switch(event_event->timeType()) {
  case RDLogLine::Relative:
    event_timetype_check->setChecked(false);
    event_grace_group->button(0)->setChecked(true);
    timeToggledData(false);
    break;
	
  case RDLogLine::Hard:
    event_timetype_check->setChecked(true);
    switch((grace=event_event->graceTime())) {
    case 0:
      event_grace_group->button(0)->setChecked(true);
      event_grace_edit->setTime(QTime());
      break;
	      
    case -1:
      event_grace_group->button(1)->setChecked(true);
      event_grace_edit->setTime(QTime());
      break;
	      
    default:
      event_grace_group->button(2)->setChecked(true);
      event_grace_edit->setTime(QTime(0,0,0).addMSecs(grace));
      break;
    }

  case RDLogLine::NoTime:
    break;
  }
  

  prepositionToggledData(event_position_box->isChecked());
  timeToggledData(event_timetype_check->isChecked());
  importClickedData(event_source_group->checkedId());
}


void EventWidget::save(RDEvent *evt) const
{
  if(event_position_box->isChecked()) {
    event_event->setPreposition(QTime(0,0,0).msecsTo(event_position_edit->time()));
  }
  else {
    event_event->setPreposition(-1);
  }
  if(event_timetype_check->isChecked()) {
    event_event->setTimeType(RDLogLine::Hard);
    event_event->setFirstTransType((RDLogLine::TransType)
				   event_firsttrans_box->currentIndex());
    switch(event_grace_group->checkedId()) {
    case 0:
      event_event->setGraceTime(0);
      break;

    case 1:
      event_event->setGraceTime(-1);
      break;

    default:
      event_event->setGraceTime(QTime(0,0,0).msecsTo(event_grace_edit->time()));
      break;	  
    }
  }
  else {
    event_event->setTimeType(RDLogLine::Relative);
    event_event->setGraceTime(0);
    event_event->setFirstTransType(RDLogLine::Play);
  }

  event_event->setUseAutofill(event_autofill_box->isChecked());
  if(event_autofill_slop_box->isChecked()) {
    event_event->
      setAutofillSlop(QTime(0,0,0).msecsTo(event_autofill_slop_edit->time()));
  }
  else {
    event_event->setAutofillSlop(-1);
  }
  event_event->setUseTimescale(event_timescale_box->isChecked());
  event_event->
    setImportSource((RDEventLine::ImportSource)event_source_group->checkedId());
  event_event->setStartSlop(QTime(0,0,0).msecsTo(event_startslop_edit->time()));
  event_event->setEndSlop(QTime(0,0,0).msecsTo(event_endslop_edit->time()));
  if(!event_timetype_check->isChecked()) {
    event_event->
      setFirstTransType((RDLogLine::TransType)event_firsttrans_box->
			currentIndex());
  }
  event_event->
    setDefaultTransType((RDLogLine::TransType)event_defaulttrans_box->
  			currentIndex());
  if(event_nestevent_box->currentIndex()==0) {
    event_event->setNestedEvent("");
  }
  else {
    event_event->setNestedEvent(event_nestevent_box->currentText());
  }
  event_event->setSchedGroup(event_sched_group_box->currentText());  
  event_event->setArtistSep(event_artist_sep_spinbox->value());
  event_event->setTitleSep(event_title_sep_spinbox->value());
  event_event->setHaveCode("");
  event_event->setHaveCode2("");
  if(event_have_code_box->currentIndex()>0) {
    event_event->setHaveCode(event_have_code_box->currentText());
  }
  if(event_have_code2_box->currentIndex()>0) {
    event_event->setHaveCode2(event_have_code2_box->currentText());
  }

  //
  // If both codes are the same, remove second code
  //
  if (event_event->HaveCode()==event_event->HaveCode2()) {
    event_event->setHaveCode2("");
  }

  //
  // Save second code as first code when first code isn't defined
  //
  if (event_event->HaveCode().isEmpty()) {
    event_event->setHaveCode(event_event->HaveCode2());
    event_event->setHaveCode2("");
  }

  event_preimport_widget->
    save(event_event,
	 (RDLogLine::TransType)event_firsttrans_box->currentIndex());
  event_postimport_widget->save(event_event);
}


void EventWidget::prepositionToggledData(bool state)
{
  event_position_edit->setEnabled(state);

  //
  // TIMED START Section
  //
  event_timetype_check->setDisabled(state);
  event_timetype_group->setDisabled(state);
  event_timetype_label->setDisabled(state);
  if(event_timetype_check->isChecked()) {
    event_grace_groupbox->setDisabled(state);
    event_immediate_button->setDisabled(state);
    event_next_button->setDisabled(state);
    event_wait_button->setDisabled(state);
  }

  //
  // TRANSITIONS Section
  //
  if(state) {
    event_firsttrans_box->setCurrentIndex(2);
  }
  event_firsttrans_box->setDisabled(state);
  event_firsttrans_label->setDisabled(state);
  event_firsttrans_unit->setDisabled(state);
}


void EventWidget::timeToggledData(bool state)
{
  event_grace_groupbox->setEnabled(state);
  event_immediate_button->setEnabled(state);
  event_next_button->setEnabled(state);
  event_wait_button->setEnabled(state);
  event_grace_edit->setEnabled(state);
  if(state) {
    graceClickedData(event_grace_group->checkedId());
    timeTransitionData(2);
    event_position_box->setDisabled(true);
    event_position_edit->setDisabled(true);
    event_position_group->setDisabled(true);
    event_position_label->setDisabled(true);
    event_position_unit->setDisabled(true);
  }
  else {
    event_grace_edit->setDisabled(true);
    if(event_position_box->isChecked()) {
      event_position_edit->setEnabled(true);
    }
    event_position_box->setEnabled(true);
    event_position_group->setEnabled(true);
    event_position_label->setEnabled(true);
    event_position_unit->setEnabled(true);
  }
}


void EventWidget::graceClickedData(int id)
{
  switch(id) {
  case 0:
    timeTransitionData(RDLogLine::Stop);
    event_grace_edit->setDisabled(true);
    break;

  case 1:
    timeTransitionData(RDLogLine::Segue);
    event_grace_edit->setDisabled(true);
    break;

  case 2:
    timeTransitionData(RDLogLine::Segue);
    event_grace_edit->setEnabled(true);
    break;
  }
}


void EventWidget::timeTransitionData(int id)
{
  // Nothing to do??
}


void EventWidget::autofillToggledData(bool state)
{
  event_autofill_slop_box->setEnabled(state);
  event_autofill_slop_label1->setEnabled(state);
  if(state) {
    autofillWarnToggledData(event_autofill_slop_box->isChecked());
  }
  else {
    autofillWarnToggledData(false);
  }
}


void EventWidget::autofillWarnToggledData(bool state)
{
  event_autofill_slop_edit->setEnabled(state);
  event_autofill_slop_label->setEnabled(state);
}


void EventWidget::importClickedData(int id)
{
  bool state=true;
  bool statesched=true;
  bool stateschedinv=false;
  if(id==0) {
    state=false;
    statesched=false;
  }
  if(id==3) {
    state=false;
    statesched=false;
    stateschedinv=true;
  }
  event_startslop_edit->setEnabled(statesched);
  event_startslop_label->setEnabled(statesched);
  event_startslop_unit->setEnabled(statesched);
  event_endslop_edit->setEnabled(statesched);
  event_endslop_label->setEnabled(statesched);
  event_endslop_unit->setEnabled(statesched);
  event_defaulttrans_box->setEnabled(state);
  event_defaulttrans_label->setEnabled(state);
  event_defaulttrans_unit->setEnabled(state);
  state=(id==2)&&state;
  event_nestevent_label->setEnabled(state);
  event_nestevent_box->setEnabled(state);
  event_sched_group_box->setEnabled(stateschedinv);
  event_artist_sep_label->setEnabled(stateschedinv);
  event_artist_sep_spinbox->setEnabled(stateschedinv);
  event_artist_none_button->setEnabled(stateschedinv);
  event_title_sep_label->setEnabled(stateschedinv);
  event_title_sep_spinbox->setEnabled(stateschedinv);
  event_title_none_button->setEnabled(stateschedinv);
  event_have_code_box->setEnabled(stateschedinv);
  event_have_code_label->setEnabled(stateschedinv);
  event_have_code2_box->setEnabled(stateschedinv);
  event_have_code2_label->setEnabled(stateschedinv);
}


void EventWidget::artistData()
{
  event_artist_sep_spinbox->setValue(-1);
}


void EventWidget::titleData()
{
  event_title_sep_spinbox->setValue(-1);
}


void EventWidget::resizeEvent(QResizeEvent *e)
{
  //
  // Pre-Position Section
  //
  event_position_group->setGeometry(0,1,sizeHint().width()-15,43);
  event_position_box->setGeometry(5,21,15,22);
  event_position_label->setGeometry(25,20,150,22);
  event_position_edit->setGeometry(134,20,60,22);
  event_position_unit->setGeometry(202,21,sizeHint().width()-227,22);

  //
  // Timed Start Section
  //
  event_timetype_group->setGeometry(0,49,sizeHint().width()-15,66);

  //
  // Time Start Section
  //
  // Time Type
  event_timetype_check->setGeometry(5,70,15,15);
  event_timetype_label->setGeometry(25,69,120,16);

  // Grace Time
  event_grace_groupbox->setGeometry(160,69,sizeHint().width()-200,42);
  event_immediate_button->setGeometry(170,92,160,15);
  event_next_button->setGeometry(310,92,160,15);
  event_wait_button->setGeometry(420,92,160,15);
  event_grace_edit->setGeometry(500,89,60,20);

  //
  // Transitions Section
  //
  event_transitions_group->
    setGeometry(0,120,sizeHint().width()-15,63);

  // First Cart Transition Type
  event_firsttrans_label->
    setGeometry(5,140,
		labelFontMetrics()->width(event_firsttrans_label->text()+" "),
		20);
  event_firsttrans_box->setGeometry(event_firsttrans_label->geometry().x()+
				    event_firsttrans_label->geometry().width(),
				    140,90,20);
  event_firsttrans_unit->
    setGeometry(event_firsttrans_box->geometry().x()+
		event_firsttrans_box->geometry().width()+5,140,
		labelFontMetrics()->width(tr("transition.")),20);
  // Default Transition Type
  event_defaulttrans_label->
    setGeometry(5,161,
		labelFontMetrics()->width(event_defaulttrans_label->text()+" "),
		20);
  event_defaulttrans_box->
    setGeometry(event_defaulttrans_label->geometry().x()+
		event_defaulttrans_label->geometry().width(),161,90,20);
  event_defaulttrans_unit->
    setGeometry(event_defaulttrans_box->geometry().x()+
		event_defaulttrans_box->geometry().width()+5,161,
		labelFontMetrics()->width(tr("transition.")),20);

  //
  // Enforcing Length Section
  //
  event_autofill_group->
    setGeometry(0,188,sizeHint().width()-15,43);
  event_autofill_box->setGeometry(5,210,15,15);
  event_autofill_label->setGeometry(25,210,150,15);
  event_autofill_slop_box->setGeometry(200,210,15,15);
  event_autofill_slop_label1->
    setGeometry(220,210,
		labelFontMetrics()->width(event_autofill_slop_label1->text()),
		15);
  event_autofill_slop_label->
    setGeometry(event_autofill_slop_label1->geometry().x()+
	   event_autofill_slop_label1->geometry().width(),210,
	   labelFontMetrics()->width(event_autofill_slop_label->text()+" "),15);
  event_autofill_slop_edit->
    setGeometry(event_autofill_slop_label->geometry().x()+
		event_autofill_slop_label->geometry().width(),207,60,22);
  event_timescale_box->setGeometry(240,210,15,15);
  event_timescale_label->setGeometry(260,214,150,15);

  //
  // Cart Stack Section
  //
  event_stack_group->
    setGeometry(0,235,sizeHint().width()-15,size().height()-235);

  //
  // Pre-Import Carts Section
  //
  int events_h=(size().height()-235-106)/2;

  event_preimport_widget->
    setGeometry(0,250,event_preimport_widget->sizeHint().width(),events_h);

  //
  // Imports Section
  //
  int import_y=250+events_h-15;

  event_imports_label->setGeometry(5,import_y+3,200,16);
  event_source_none_radio->setGeometry(70,import_y+3,15,15);
  event_source_traffic_radio->setGeometry(150,import_y+3,15,15);
  event_source_music_radio->setGeometry(270,import_y+3,15,15);
  event_source_scheduler_radio->setGeometry(390,import_y+3,15,15);
  event_source_group_none_label->setGeometry(90,import_y+3,150,15);
  event_source_group_traffic_label->setGeometry(170,import_y+3,150,15);
  event_source_group_music_label->setGeometry(290,import_y+3,150,15);
  event_source_group_scheduler_label->setGeometry(410,import_y+3,150,15);

  //
  // Scheduler Group
  //
  event_sched_group_box->setGeometry(500,import_y,100,20);

  //
  // Artist Separation
  //
  event_artist_sep_label->setGeometry(400,import_y+23,100,20);
  event_artist_sep_spinbox->setGeometry(505,import_y+24,53,20);
  event_artist_none_button->setGeometry(565,import_y+24,40,20);

  //
  // Title Separation
  //
  event_title_sep_label->setGeometry(400,import_y+45,100,20);
  event_title_sep_spinbox->setGeometry(505,import_y+45,53,20);
  event_title_none_button->setGeometry(565,import_y+45,40,20);

  //
  // Must Have Code
  //
  event_have_code_label->setGeometry(400,import_y+68,100,20);
  event_have_code_box->setGeometry(505,import_y+68,100,20);

  //
  // And Code
  //
  event_have_code2_label->setGeometry(400,import_y+89,100,20);
  event_have_code2_box->setGeometry(505,import_y+89,100,20);

  // Start Slop
  //
  event_startslop_label->setGeometry(20,import_y+24,140,22);
  event_startslop_edit->setGeometry(156,import_y+24,60,22);
  event_startslop_unit->setGeometry(220,import_y+24,sizeHint().width()-450,22);

  //
  // End Slop
  //
  event_endslop_label->setGeometry(20,import_y+45,140,22);
  event_endslop_edit->setGeometry(156,import_y+45,60,22);
  event_endslop_unit->setGeometry(220,import_y+45,sizeHint().width()-460,22);

  //
  // Nested Event
  //
  event_nestevent_label->setGeometry(25,import_y+70,190,20);
  event_nestevent_box->setGeometry(20,import_y+89,365,20);

  //
  // Post-Import Carts Section
  //
  event_postimport_widget->
    setGeometry(0,import_y+106+20,event_postimport_widget->sizeHint().width(),
		events_h);
}


void EventWidget::paintEvent(QPaintEvent *e)
{
  int y=85+size().height()/2;

  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->drawLine(396,y,396,y+84);
  p->end();
}
