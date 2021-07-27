// edit_recording.cpp
//
// Edit a Rivendell RDCatch Recording
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

#include <QGroupBox>
#include <QMessageBox>

#include <rdcut_path.h>
#include <rdescape_string.h>
#include <rdmatrix.h>
#include <rdtextvalidator.h>

#include "edit_recording.h"
#include "globals.h"

EditRecording::EditRecording(int id,std::vector<int> *adds,QString *filter,
			     QWidget *parent)
  : RDDialog(parent)
{
  QString temp;

  edit_deck=NULL;
  edit_added_events=adds;
  edit_filter=filter;

  setWindowTitle("RDCatch - "+tr("Edit Recording"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(id);

  //
  // Dialogs
  //
  edit_cut_dialog=new RDCutDialog(edit_filter,&edit_group,&edit_schedcode,
				  false,true,true,"RDCatch",false,this);

  //
  // Event Widget
  //
  edit_event_widget=new EventWidget(EventWidget::RecordEvent,this);
  connect(edit_event_widget,SIGNAL(locationChanged(const QString &,int)),
	  this,SLOT(locationChangedData(const QString &,int)));

  //
  // Start Parameters
  //
  edit_starttype_group=new QButtonGroup(this);
  connect(edit_starttype_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(startTypeClickedData(int)));

  edit_start_groupbox=new QGroupBox(tr("Start Parameters"),this);
  edit_start_groupbox->setFont(labelFont());

  edit_start_hardtime_radio=new QRadioButton(tr("Use Hard Time"),this);
  edit_starttype_group->
    addButton(edit_start_hardtime_radio,RDRecording::HardStart);  
  edit_start_hardtime_radio->setFont(subLabelFont());
  
  edit_starttime_edit=new QTimeEdit(this);
  edit_starttime_edit->setDisplayFormat("hh:mm:ss");
  edit_starttime_label=new QLabel(tr("Record Start Time:"),this);
  edit_starttime_label->setFont(subLabelFont());
  edit_starttime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_start_gpi_radio=new QRadioButton(tr("Use GPI"),this);
  edit_start_gpi_radio->setFont(subLabelFont());
  
  edit_start_startwindow_edit=new QTimeEdit(this);
  edit_start_startwindow_edit->setDisplayFormat("hh:mm:ss");
  edit_start_startwindow_label=new QLabel(tr("Window Start Time:"),this);
  edit_start_startwindow_label->setFont(subLabelFont());
  edit_start_startwindow_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_start_endwindow_edit=new QTimeEdit(this);
  edit_start_endwindow_edit->setDisplayFormat("hh:mm:ss");
  edit_start_endwindow_label=new QLabel(tr("Window End Time:"),this);
  edit_start_endwindow_label->setFont(subLabelFont());
  edit_start_endwindow_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_startmatrix_spin=new QSpinBox(this);
  edit_startmatrix_spin->setRange(0,MAX_MATRICES-1);
  edit_startmatrix_label=new QLabel(tr("GPI Matrix:"),this);
  edit_startmatrix_label->setFont(subLabelFont());
  edit_startmatrix_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_startline_spin=new QSpinBox(this);
  edit_startline_spin->setRange(1,MAX_GPIO_PINS);
  edit_startline_label=new QLabel(tr("GPI Line:"),this);
  edit_startline_label->setFont(subLabelFont());
  edit_startline_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_startoffset_edit=new QTimeEdit(this);
  edit_startoffset_edit->setDisplayFormat("hh:mm:ss");
  edit_startoffset_time_label=new QLabel(tr("Start Delay:"),this);
  edit_startoffset_time_label->setFont(subLabelFont());
  edit_startoffset_time_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_multirec_box=
    new QCheckBox(tr("Allow Multiple Recordings within this Window"),this);
  edit_multirec_box->setFont(subLabelFont());

  edit_starttype_group->addButton(edit_start_gpi_radio,RDRecording::GpiStart);

  //
  // End Parameters
  //
  edit_endtype_group=new QButtonGroup(this);
  connect(edit_endtype_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(endTypeClickedData(int)));

  edit_end_groupbox=new QGroupBox(tr("End Parameters"),this);
  edit_end_groupbox->setFont(labelFont());
  edit_end_length_radio=new QRadioButton(tr("Use Length"),this);
  edit_endtype_group->addButton(edit_end_length_radio,RDRecording::LengthEnd);  
  edit_end_length_radio->setFont(subLabelFont());
  edit_endlength_edit=new QTimeEdit(this);
  edit_endlength_edit->setDisplayFormat("hh:mm:ss");
  edit_endlength_label=new QLabel(tr("Record Length:"),this);
  edit_endlength_label->setFont(subLabelFont());
  edit_endlength_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_end_hardtime_radio=new QRadioButton(tr("Use Hard Time"),this);
  edit_endtype_group->addButton(edit_end_hardtime_radio,RDRecording::HardEnd);  
  edit_end_hardtime_radio->setFont(subLabelFont());
  edit_endtime_edit=new QTimeEdit(this);
  edit_endtime_edit->setDisplayFormat("hh:mm:ss");
  edit_endtime_label=new QLabel(tr("Record End Time:"),this);
  edit_endtime_label->setFont(subLabelFont());
  edit_endtime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_end_gpi_radio=new QRadioButton(tr("Use GPI"),this);
  edit_end_gpi_radio->setFont(subLabelFont());
  edit_end_startwindow_edit=new QTimeEdit(this);
  edit_end_startwindow_edit->setDisplayFormat("hh:mm:ss");
  edit_end_startwindow_label=new QLabel(tr("Window Start Time:"),this);
  edit_end_startwindow_label->setFont(subLabelFont());
  edit_end_startwindow_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_end_endwindow_edit=new QTimeEdit(this);
  edit_end_endwindow_edit->setDisplayFormat("hh:mm:ss");
  edit_end_endwindow_label=new QLabel(tr("Window End Time:"),this);
  edit_end_endwindow_label->setFont(subLabelFont());
  edit_end_endwindow_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_endmatrix_spin=new QSpinBox(this);
  edit_endmatrix_spin->setRange(0,MAX_MATRICES-1);
  edit_endmatrix_label=new QLabel(tr("GPI Matrix:"),this);
  edit_endmatrix_label->setFont(subLabelFont());
  edit_endmatrix_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_endline_spin=new QSpinBox(this);
  edit_endline_spin->setRange(1,MAX_GPIO_PINS);
  edit_endline_label=new QLabel(tr("GPI Line:"),this);
  edit_endline_label->setFont(subLabelFont());
  edit_endline_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_maxlength_edit=new QTimeEdit(this);
  edit_maxlength_edit->setDisplayFormat("hh:mm:ss");
  edit_maxlength_label=new QLabel(tr("Max Record Length:"),this);
  edit_maxlength_label->setFont(subLabelFont());
  edit_maxlength_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_endtype_group->addButton(edit_end_gpi_radio,RDRecording::GpiEnd);

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setValidator(validator);
  edit_description_label=new QLabel(tr("Description:"),this);
  edit_description_label->setFont(labelFont());
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Source Name
  //
  edit_source_box=new QComboBox(this);
  edit_source_label=new QLabel(tr("Source:"),this);
  edit_source_label->setFont(labelFont());
  edit_source_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Destination
  //
  edit_destination_edit=new QLineEdit(this);
  edit_destination_edit->setReadOnly(true);
  edit_destination_label=new QLabel(tr("Destination:"),this);
  edit_destination_label->setFont(labelFont());
  edit_destination_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_destination_button=new QPushButton(this);
  edit_destination_button->setFont(subButtonFont());
  edit_destination_button->setText(tr("Select"));
  connect(edit_destination_button,SIGNAL(clicked()),this,SLOT(selectCutData()));

  //
  // Channels
  //
  edit_channels_box=new QComboBox(this);
  edit_channels_box->insertItem(0,"1");
  edit_channels_box->insertItem(1,"2");
  edit_channels_label=new QLabel(tr("Channels:"),this);
  edit_channels_label->setFont(labelFont());
  edit_channels_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Autotrim Controls
  //
  edit_autotrim_box=new QCheckBox(tr("Autotrim"),this);
  edit_autotrim_box->setFont(labelFont());
  connect(edit_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimToggledData(bool)));
  edit_autotrim_spin=new QSpinBox(this);
  edit_autotrim_spin->setRange(-99,-1);
  edit_autotrim_label=new QLabel(tr("Level:"),this);
  edit_autotrim_label->setFont(labelFont());
  edit_autotrim_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_autotrim_unit=new QLabel(tr("dBFS"),this);
  edit_autotrim_unit->setFont(labelFont());
  edit_autotrim_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Normalize Controls
  //
  edit_normalize_box=new QCheckBox(tr("Normalize"),this);
  connect(edit_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeToggledData(bool)));
  edit_normalize_box->setFont(labelFont());
  edit_normalize_spin=new QSpinBox(this);
  edit_normalize_spin->setRange(-99,-1);
  edit_normalize_label=new QLabel(tr("Level:"),this);
  edit_normalize_label->setFont(labelFont());
  edit_normalize_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_normalize_unit=new QLabel(tr("dBFS"),this);
  edit_normalize_unit->setFont(labelFont());
  edit_normalize_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // DOW Selector
  //
  edit_dow_selector=new DowSelector(this);
  
  //
  // Start Date Offset
  //
  edit_startoffset_box=new QSpinBox(this);
  edit_startoffset_box->setRange(0,355);
  edit_startoffset_box->setSpecialValueText(tr("None"));
  edit_startoffset_label=new QLabel(tr("Start Date Offset:"),this);
  edit_startoffset_label->setFont(labelFont());
  edit_startoffset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // End Date Offset
  //
  edit_endoffset_box=new QSpinBox(this);
  edit_endoffset_box->setRange(0,355);
  edit_endoffset_box->setSpecialValueText(tr("None"));
  edit_endoffset_label=new QLabel(tr("End Date Offset:"),this);
  edit_endoffset_label->setFont(labelFont());
  edit_endoffset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(tr("Make OneShot"),this);
  edit_oneshot_box->setFont(labelFont());

  //
  //  Save As Button
  //
  edit_saveas_button=new QPushButton(this);
  edit_saveas_button->setFont(buttonFont());
  edit_saveas_button->setText(tr("Save As\nNew"));
  connect(edit_saveas_button,SIGNAL(clicked()),this,SLOT(saveasData()));
  if(adds==NULL) {
    edit_saveas_button->hide();
  }

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_event_widget->fromRecording(edit_recording->id());
  edit_starttime_edit->setTime(edit_recording->startTime());
  edit_description_edit->setText(edit_recording->description());
  edit_starttype_group->button((int)edit_recording->startType())->
    setChecked(true);
  switch((RDRecording::StartType)edit_starttype_group->checkedId()) {
  case RDRecording::HardStart:
    edit_starttime_edit->setTime(edit_recording->startTime());
    break;

  case RDRecording::GpiStart:
    edit_start_startwindow_edit->setTime(edit_recording->startTime());
    edit_start_endwindow_edit->
      setTime(edit_start_startwindow_edit->time().
	      addMSecs(edit_recording->startLength()));
    edit_startmatrix_spin->setValue(edit_recording->startMatrix());
    edit_startline_spin->setValue(edit_recording->startLine());
    edit_startoffset_edit->
      setTime(QTime(0,0,0).addMSecs(edit_recording->startOffset()));
    edit_multirec_box->
      setChecked(edit_recording->allowMultipleRecordings());
    break;
  }
  startTypeClickedData(edit_starttype_group->checkedId());
  edit_endtype_group->button((int)edit_recording->endType())->setChecked(true);
  switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
  case RDRecording::LengthEnd:
    edit_endlength_edit->
      setTime(QTime(0,0,0).addMSecs(edit_recording->length()));
    break;

  case RDRecording::HardEnd:
    edit_endtime_edit->setTime(edit_recording->endTime());
    break;

  case RDRecording::GpiEnd:
    edit_end_startwindow_edit->setTime(edit_recording->endTime());
    edit_end_endwindow_edit->
      setTime(edit_end_startwindow_edit->time().
	      addMSecs(edit_recording->endLength()));
    edit_endmatrix_spin->setValue(edit_recording->endMatrix());
    edit_endline_spin->setValue(edit_recording->endLine());
    break;
  }
  edit_maxlength_edit->
    setTime(QTime(0,0,0).addMSecs(edit_recording->maxGpiRecordingLength()));
  endTypeClickedData(edit_endtype_group->checkedId());

  edit_cutname=edit_recording->cutName();
  edit_destination_edit->setText(RDCutPath(edit_cutname));
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_startoffset_box->setValue(edit_recording->startdateOffset());
  edit_endoffset_box->setValue(edit_recording->enddateOffset());
  locationChangedData(edit_event_widget->stationName(),
		      edit_event_widget->deckNumber());

  QString source=GetSourceName(edit_recording->switchSource());
  for(int i=0;i<edit_source_box->count();i++) {
    if(edit_source_box->itemData(i).toString()==source) {
      edit_source_box->setCurrentIndex(i);
    }
  }
  if(edit_recording->trimThreshold()>0) {
    edit_autotrim_box->setChecked(true);
    edit_autotrim_spin->setValue(-(edit_recording->trimThreshold()/100));
  }
  else {
    edit_autotrim_box->setChecked(false);
    edit_autotrim_spin->setValue(rda->libraryConf()->trimThreshold()/100);
  }
  autotrimToggledData(edit_autotrim_box->isChecked());
  if(edit_recording->normalizationLevel()<0) {
    edit_normalize_box->setChecked(true);
    edit_normalize_spin->setValue(edit_recording->normalizationLevel()/100);
  }
  else {
    edit_normalize_box->setChecked(false);
    edit_normalize_spin->setValue(rda->libraryConf()->ripperLevel()/100);
  }
  normalizeToggledData(edit_normalize_box->isChecked());
  // Populate number of channels; if creating a new recording entry and a valid
  // deck exists, use the deck default for num. channels.  Otherwise use the
  // previously entered (or DB default) recording num. channels.
  if( (edit_recording->station().length() == 0) && (edit_deck!=NULL) ) {
    edit_channels_box->setCurrentIndex(edit_deck->defaultChannels()-1);
  } else {
    edit_channels_box->setCurrentIndex(edit_recording->channels()-1);
  }
  edit_oneshot_box->setChecked(edit_recording->oneShot());
}


EditRecording::~EditRecording()
{
  delete edit_cut_dialog;
  delete edit_event_widget;
  delete edit_dow_selector;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditRecording::sizeHint() const
{
  return QSize(560,619);
} 


QSizePolicy EditRecording::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRecording::locationChangedData(const QString &station,int decknum)
{
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
  edit_deck=new RDDeck(station,decknum);
  if(edit_channels_box->count()>0) {
    edit_channels_box->setCurrentIndex(edit_deck->defaultChannels()-1);
  }
  edit_source_box->clear();
  QString sql=QString("select `NAME` from `INPUTS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_deck->switchStation())+"')&&"+
    QString().sprintf("(`MATRIX`=%d)",edit_deck->switchMatrix());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_source_box->
      insertItem(edit_source_box->count(),q->value(0).toString());
  }
  delete q;
}


void EditRecording::startTypeClickedData(int id)
{
  bool state=false;

  if(((RDRecording::StartType)id)==RDRecording::HardStart) {
    state=true;
    edit_multirec_box->setDisabled(true);
  }
  else {
    switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
    case RDRecording::HardEnd:
      edit_multirec_box->setDisabled(true);
      break;

    case RDRecording::GpiEnd:
    case RDRecording::LengthEnd:
      edit_multirec_box->setEnabled(true);
      break;
    }
  }
  edit_starttime_edit->setEnabled(state);
  edit_starttime_label->setEnabled(state);
  edit_start_startwindow_edit->setDisabled(state);
  edit_start_startwindow_label->setDisabled(state);
  edit_start_endwindow_edit->setDisabled(state);
  edit_start_endwindow_label->setDisabled(state);
  edit_startoffset_edit->setDisabled(state);
  edit_startoffset_time_label->setDisabled(state);
  edit_startmatrix_spin->setDisabled(state);
  edit_startmatrix_label->setDisabled(state);
  edit_startline_spin->setDisabled(state);
  edit_startline_label->setDisabled(state);
}


void EditRecording::endTypeClickedData(int id)
{
  bool hard_state=false;
  bool gpi_state=false;
  bool length_state=false;

  if(((RDRecording::EndType)id)==RDRecording::HardEnd) {
    hard_state=true;
    edit_multirec_box->setDisabled(true);
  }
  if(((RDRecording::EndType)id)==RDRecording::GpiEnd) {
    gpi_state=true;
    edit_multirec_box->
      setEnabled(edit_starttype_group->checkedId()==RDRecording::GpiStart);
  }
  if(((RDRecording::EndType)id)==RDRecording::LengthEnd) {
    length_state=true;
    edit_multirec_box->
      setEnabled(edit_starttype_group->checkedId()==RDRecording::GpiStart);
  }
  edit_endtime_edit->setEnabled(hard_state);
  edit_endtime_label->setEnabled(hard_state);
  edit_end_startwindow_edit->setEnabled(gpi_state);
  edit_end_startwindow_label->setEnabled(gpi_state);
  edit_end_endwindow_edit->setEnabled(gpi_state);
  edit_end_endwindow_label->setEnabled(gpi_state);
  edit_endmatrix_spin->setEnabled(gpi_state);
  edit_endmatrix_label->setEnabled(gpi_state);
  edit_endline_spin->setEnabled(gpi_state);
  edit_endline_label->setEnabled(gpi_state);
  edit_endlength_edit->setEnabled(length_state);
  edit_endlength_label->setEnabled(length_state);
  edit_maxlength_label->setEnabled(gpi_state);
  edit_maxlength_edit->setEnabled(gpi_state);
}


void EditRecording::selectCutData()
{
  QString str;

  if(edit_cut_dialog->exec(&edit_cutname)) {
    edit_description_edit->setText(RDCutPath(edit_cutname));
    str=QString(tr("Cut"));
    edit_destination_edit->setText(tr("Cut")+" "+edit_cutname);
  }
}


void EditRecording::autotrimToggledData(bool state)
{
  edit_autotrim_label->setEnabled(state);
  edit_autotrim_spin->setEnabled(state);
  edit_autotrim_unit->setEnabled(state);
}


void EditRecording::normalizeToggledData(bool state)
{
  edit_normalize_label->setEnabled(state);
  edit_normalize_spin->setEnabled(state);
  edit_normalize_unit->setEnabled(state);
}


void EditRecording::saveasData()
{
  if(!CheckEvent(true)) {
    return;
  }
  delete edit_recording;
  edit_recording=new RDRecording(-1,true);
  edit_added_events->push_back(edit_recording->id());
  Save();
}


void EditRecording::okData()
{
  if(!CheckEvent(false)) {
    return;
  }
  Save();
  done(true);
}


void EditRecording::cancelData()
{
  done(false);
}


void EditRecording::resizeEvent(QResizeEvent *e)
{
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());

  //
  // Start Parameters
  //
  edit_start_groupbox->setGeometry(10,37,size().width()-20,105);
  edit_start_hardtime_radio->setGeometry(20,57,110,15);
  edit_starttime_edit->setGeometry(255,53,80,20);
  edit_starttime_label->setGeometry(135,57,115,15);
  edit_start_gpi_radio->setGeometry(20,81,100,15);
  edit_start_startwindow_edit->setGeometry(255,77,80,20);
  edit_start_startwindow_label->setGeometry(135,81,115,15);
  edit_start_endwindow_edit->setGeometry(455,77,80,20);
  edit_start_endwindow_label->setGeometry(345,81,105,15);
  edit_startmatrix_spin->setGeometry(185,99,35,20);
  edit_startmatrix_label->setGeometry(100,100,80,20);
  edit_startline_spin->setGeometry(305,99,35,20);
  edit_startline_label->setGeometry(240,100,60,20);
  edit_startoffset_edit->setGeometry(455,99,80,20);
  edit_startoffset_time_label->setGeometry(345,100,105,20);
  edit_multirec_box->setGeometry(140,124,size().width()-170,15);

  //
  // End Parameters
  //
  edit_end_groupbox->setGeometry(10,160,size().width()-20,112);
  edit_end_length_radio->setGeometry(20,205,100,15);
  edit_endlength_edit->setGeometry(245,201,80,20);
  edit_endlength_label->setGeometry(125,205,115,15);
  edit_end_hardtime_radio->setGeometry(20,181,1100,15);
  edit_endtime_edit->setGeometry(245,177,80,20);
  edit_endtime_label->setGeometry(125,181,115,15);
  edit_end_gpi_radio->setGeometry(20,229,100,15);
  edit_end_startwindow_edit->setGeometry(245,225,80,20);
  edit_end_startwindow_label->setGeometry(125,229,115,15);
  edit_end_endwindow_edit->setGeometry(455,225,80,20);
  edit_end_endwindow_label->setGeometry(345,229,105,15);
  edit_endmatrix_spin->setGeometry(185,247,35,20);
  edit_endmatrix_label->setGeometry(100,248,80,20);
  edit_endline_spin->setGeometry(295,247,35,20);
  edit_endline_label->setGeometry(230,248,60,20);
  edit_maxlength_edit->setGeometry(455,247,80,20);
  edit_maxlength_label->setGeometry(325,248,125,20);

  edit_description_edit->setGeometry(105,291,size().width()-115,20);
  edit_description_label->setGeometry(10,291,90,20);

  edit_source_box->setGeometry(105,317,size().width()-115,24);
  edit_source_label->setGeometry(10,317,90,24);

  edit_destination_edit->setGeometry(105,345,size().width()-185,20);
  edit_destination_label->setGeometry(10,345,90,20);
  edit_destination_button->setGeometry(size().width()-70,344,60,24);

  edit_channels_label->setGeometry(120,370,70,20);
  edit_channels_box->setGeometry(190,370,40,20);

  edit_autotrim_box->setGeometry(120,395,100,15);
  edit_autotrim_label->setGeometry(220,393,40,20);
  edit_autotrim_spin->setGeometry(265,393,50,20);
  edit_autotrim_unit->setGeometry(320,393,40,20);

  edit_normalize_box->setGeometry(120,420,100,15);
  edit_normalize_label->setGeometry(220,418,40,20);
  edit_normalize_spin->setGeometry(265,418,50,20);
  edit_normalize_unit->setGeometry(320,418,40,20);

  edit_dow_selector->setGeometry(30,442,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());

  edit_startoffset_box->setGeometry(140,516,55,24);
  edit_startoffset_label->setGeometry(10,516,125,24);
  edit_endoffset_box->setGeometry(440,516,55,24);
  edit_endoffset_label->setGeometry(310,516,125,24);

  edit_oneshot_box->setGeometry(20,553,125,15);

  edit_saveas_button->
    setGeometry(size().width()-300,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditRecording::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
  case Qt::Key_Escape:
    e->accept();
    cancelData();
    break;

  default:
    QDialog::keyPressEvent(e);
    break;
  }
}


void EditRecording::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditRecording::Save()
{
  edit_event_widget->toRecording(edit_recording->id());
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_cutname);
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setSwitchSource(GetSource());
  edit_recording->setStartdateOffset(edit_startoffset_box->value());
  edit_recording->setEnddateOffset(edit_endoffset_box->value());
  edit_recording->setFormat(edit_deck->defaultFormat());
  if(edit_deck->defaultFormat()>0) {
    edit_recording->setBitrate(edit_deck->defaultBitrate()*
			       (edit_channels_box->currentIndex()+1));
  }
  else {
    edit_recording->setBitrate(0);
  }
  edit_recording->setChannels(edit_channels_box->currentIndex()+1);
  if(edit_autotrim_box->isChecked()) {
    edit_recording->setTrimThreshold(-100*edit_autotrim_spin->value());
  }
  else {
    edit_recording->setTrimThreshold(0);
  }
  if(edit_normalize_box->isChecked()) {
    edit_recording->setNormalizationLevel(100*edit_normalize_spin->value());
  }
  else {
    edit_recording->setNormalizationLevel(0);
  }
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
  edit_recording->
    setStartType((RDRecording::StartType)edit_starttype_group->checkedId());
  switch((RDRecording::StartType)edit_starttype_group->checkedId()) {
  case RDRecording::HardStart:
    if(edit_starttime_edit->time().isNull()) {
      edit_recording->
	setStartTime(edit_starttime_edit->time().addMSecs(1));
    }
    else {
      edit_recording->setStartTime(edit_starttime_edit->time());
    }
    edit_recording->setAllowMultipleRecordings(false);
    break;

  case RDRecording::GpiStart:
    if(edit_start_startwindow_edit->time().isNull()) {
      edit_recording->
	setStartTime(edit_start_startwindow_edit->time().addMSecs(1));
    }
    else {
      edit_recording->setStartTime(edit_start_startwindow_edit->time());
    }
    edit_recording->
      setStartLength(edit_start_startwindow_edit->time().
		     msecsTo(edit_start_endwindow_edit->time()));
    edit_recording->setStartMatrix(edit_startmatrix_spin->value());
    edit_recording->setStartLine(edit_startline_spin->value());
    edit_recording->
      setStartOffset(QTime(0,0,0).msecsTo(edit_startoffset_edit->time()));
    edit_recording->
      setAllowMultipleRecordings(edit_multirec_box->isChecked());
    break;
  }
  edit_recording->
    setEndType((RDRecording::EndType)edit_endtype_group->checkedId());
  edit_recording->
    setMaxGpiRecordingLength(QTime(0,0,0).msecsTo(edit_maxlength_edit->time()));
  switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
  case RDRecording::LengthEnd:
    edit_recording->
      setLength(QTime(0,0,0).msecsTo(edit_endlength_edit->time()));
    break;

  case RDRecording::HardEnd:
    if(edit_endtime_edit->time().isNull()) {
      edit_recording->setEndTime(edit_endtime_edit->time().addMSecs(1));
    }
    else {
      edit_recording->setEndTime(edit_endtime_edit->time());
    }
    break;

  case RDRecording::GpiEnd:
    if(edit_end_startwindow_edit->time().isNull()) {
      edit_recording->
	setEndTime(edit_end_startwindow_edit->time().addMSecs(1));
    }
    else {
      edit_recording->setEndTime(edit_end_startwindow_edit->time());
    }
    edit_recording->
      setEndLength(edit_end_startwindow_edit->time().
		   msecsTo(edit_end_endwindow_edit->time()));
    edit_recording->setEndMatrix(edit_endmatrix_spin->value());
    edit_recording->setEndLine(edit_endline_spin->value());
    break;
  }
}


bool EditRecording::CheckEvent(bool include_myself)
{
  QTime start;
  QTime finish;
  QTime begin;
  QTime end;
  RDMatrix *matrix;

  //
  // Record Cut
  //
  if(edit_cutname.isEmpty()) {
    QMessageBox::warning(this,tr("Missing Cut"),
			 tr("You must assign a record cut!"));
    return false;
  }

  //
  // Ensure that the time values are sane
  //
  switch((RDRecording::StartType)edit_starttype_group->checkedId()) {
  case RDRecording::GpiStart:
    if(edit_start_startwindow_edit->time()>=
       edit_start_endwindow_edit->time()) {
      QMessageBox::warning(this,tr("Record Parameter Error"),
			   tr("The start GPI window cannot end before it begins!"));
      return false;
    }
    switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
    case RDRecording::HardEnd:
      if(edit_start_startwindow_edit->time()>=
	 edit_endtime_edit->time()) {
	QMessageBox::warning(this,tr("Record Parameter Error"),
			     tr("The recording cannot end before it begins!"));
	return false;
      }
      break;

    case RDRecording::GpiEnd:
      if(edit_start_startwindow_edit->time()>
	 edit_end_startwindow_edit->time()) {
	QMessageBox::warning(this,tr("Record Parameter Error"),
			     tr("The end GPI window cannot end before it begins!"));
	return false;
      }
      break;

    case RDRecording::LengthEnd:
      if(QTime(0,0,0).msecsTo(edit_endlength_edit->time())==0) {
	QMessageBox::warning(this,"RDCatch - "+tr("Record Parameter Error"),
			     tr("The Record Length parameter cannot be \"00:00:00\"!"));
	return false;
      }
      break;
    }
    break;

  case RDRecording::HardStart:
    switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
    case RDRecording::HardEnd:
      if(edit_starttime_edit->time()>=
	 edit_endtime_edit->time()) {
	QMessageBox::warning(this,tr("Record Parameter Error"),
			     tr("The recording cannot end before it begins!"));
	return false;
      }
      break;

    case RDRecording::GpiEnd:
      if(edit_starttime_edit->time()>=
	 edit_end_startwindow_edit->time()) {
	QMessageBox::warning(this,tr("Record Parameter Error"),
			     tr("The end GPI window cannot end before it begins!"));
	return false;
      }
      break;

    case RDRecording::LengthEnd:
      if(QTime(0,0,0).msecsTo(edit_endlength_edit->time())==0) {
	QMessageBox::warning(this,"RDCatch - "+tr("Record Parameter Error"),
			     tr("The Record Length parameter cannot be \"00:00:00\"!"));
	return false;
      }
      break;
    }
    break;
  }
  switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
  case RDRecording::GpiEnd:
    if(edit_end_startwindow_edit->time()>=
       edit_end_endwindow_edit->time()) {
      QMessageBox::warning(this,tr("Record Parameter Error"),
			   tr("The end GPI window cannot end before it begins!"));
      return false;
    }
    break;

  default:
    break;
  }

  //
  // Verify that the GPI values are valid
  //
  switch((RDRecording::StartType)edit_starttype_group->checkedId()) {
  case RDRecording::GpiStart:
    matrix=new RDMatrix(edit_event_widget->stationName(),
			edit_startmatrix_spin->value());
    if(!matrix->exists()) {
      QMessageBox::warning(this,tr("Record Parameter Error"),
			   tr("The start GPI matrix doesn't exist!"));
      delete matrix;
      return false;
    }
    if(matrix->gpis()<edit_startline_spin->value()) {
      QMessageBox::warning(this,tr("Record Parameter Error"),
			   tr("The start GPI line doesn't exist!"));
      delete matrix;
      return false;
    }
    delete matrix;
    edit_starttime_edit->setTime(edit_start_startwindow_edit->time());
    break;

  default:
    break;
  }
  switch((RDRecording::EndType)edit_endtype_group->checkedId()) {
  case RDRecording::GpiEnd:
    matrix=new RDMatrix(edit_event_widget->stationName(),
			edit_endmatrix_spin->value());
    if(!matrix->exists()) {
      QMessageBox::warning(this,tr("Record Parameter Error"),
			   tr("The end GPI matrix doesn't exist!"));
      delete matrix;
      return false;
    }
    if(matrix->gpis()<edit_endline_spin->value()) {
      QMessageBox::warning(this,tr("Record Parameter Error"),
			   tr("The end GPI line doesn't exist!"));
      delete matrix;
      return false;
    }
    delete matrix;
    break;

  default:
    break;
  }

  QString sql=QString("select `ID` from `RECORDINGS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString().sprintf("(`TYPE`=%d)&&",RDRecording::Recording)+
    "(`START_TIME`='"+RDEscapeString(edit_starttime_edit->time().toString("hh:mm:ss"))+"')&&"+
    QString().sprintf("(`CHANNEL`=%d)",edit_event_widget->deckNumber());
  switch((RDRecording::StartType)edit_starttype_group->checkedId()) {
  case RDRecording::HardStart:
    break;

  case RDRecording::GpiStart:
    sql+=QString().sprintf("&&(`START_MATRIX`=%d)&&(`START_LINE`=%d)",
			   edit_startmatrix_spin->value(),
			   edit_startline_spin->value());
    break;
  }
  if(edit_dow_selector->dayOfWeekEnabled(7)) {
    sql+="&&(`SUN`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(1)) {
    sql+="&&(`MON`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(2)) {
    sql+="&&(`TUE`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(3)) {
    sql+="&&(`WED`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(4)) {
    sql+="&&(`THU`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(5)) {
    sql+="&&(`FRI`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(6)) {
    sql+="&&(`SAT`='Y')";
  }
  if(!include_myself) {
    sql+=QString().sprintf("&&(`ID`!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool res=!q->first();
  delete q;
  if(!res) {
    QMessageBox::warning(this,tr("Duplicate Event"),
			 tr("An event with these parameters already exists!"));
  }

  return res;
}


QString EditRecording::GetSourceName(int input)
{
  if(edit_deck==NULL) {
    return QString("[unknown]");
  }
  QString input_name;
  QString sql=QString("select `NAME` from `INPUTS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_deck->switchStation())+"')&&"+
    QString().sprintf("(`MATRIX`=%d)&&",edit_deck->switchMatrix())+
    QString().sprintf("(`NUMBER`=%d)",input);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    input_name=q->value(0).toString();
  }
  delete q;
  return input_name;
}


int EditRecording::GetSource()
{
  int source=-1;

  QString sql=QString("select `NUMBER` from `INPUTS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_deck->switchStation())+"')&&"+
    QString().sprintf("(`MATRIX`=%d)&&",edit_deck->switchMatrix())+
    "(`NAME`='"+RDEscapeString(edit_source_box->currentText())+"')";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    source=q->value(0).toInt();
  }
  delete q;
  return source;
}
