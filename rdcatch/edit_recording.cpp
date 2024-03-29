// edit_recording.cpp
//
// Edit a Rivendell RDCatch Recording
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

EditRecording::EditRecording(QString *filter,QWidget *parent)
  : RDDialog(parent)
{
  QString temp;

  edit_deck=NULL;
  edit_recording=NULL;
  edit_added_events=NULL;
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
  
  edit_starttime_edit=new RDTimeEdit(this);
  edit_starttime_label=new QLabel(tr("Record Start Time:"),this);
  edit_starttime_label->setFont(subLabelFont());
  edit_starttime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_start_gpi_radio=new QRadioButton(tr("Use GPI"),this);
  edit_start_gpi_radio->setFont(subLabelFont());
  
  edit_start_startwindow_edit=new RDTimeEdit(this);
  edit_start_startwindow_label=new QLabel(tr("Window Start Time:"),this);
  edit_start_startwindow_label->setFont(subLabelFont());
  edit_start_startwindow_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_start_endwindow_edit=new RDTimeEdit(this);
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
  edit_endtime_edit=new RDTimeEdit(this);
  edit_endtime_label=new QLabel(tr("Record End Time:"),this);
  edit_endtime_label->setFont(subLabelFont());
  edit_endtime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_end_gpi_radio=new QRadioButton(tr("Use GPI"),this);
  edit_end_gpi_radio->setFont(subLabelFont());
  edit_end_startwindow_edit=new RDTimeEdit(this);
  edit_end_startwindow_label=new QLabel(tr("Window Start Time:"),this);
  edit_end_startwindow_label->setFont(subLabelFont());
  edit_end_startwindow_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_end_endwindow_edit=new RDTimeEdit(this);
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
}


EditRecording::~EditRecording()
{
  delete edit_event_widget;
  delete edit_dow_selector;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditRecording::sizeHint() const
{
  return QSize(640,619);
} 


QSizePolicy EditRecording::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditRecording::exec(int id,std::vector<int> *adds)
{
  if(edit_recording!=NULL) {
    delete edit_recording;
  }
  edit_recording=new RDRecording(id);
  edit_added_events=adds;
  if(edit_added_events==NULL) {
    edit_saveas_button->hide();
  }
  else {
    edit_saveas_button->show();
  }

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
  if(edit_cutname.isEmpty()) {
    edit_destination_edit->clear();
  }
  else {
    edit_destination_edit->
      setText(tr("Cut")+" "+RDCut::prettyText(edit_cutname));
  }
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_startoffset_box->setValue(edit_recording->startdateOffset());
  edit_endoffset_box->setValue(edit_recording->enddateOffset());
  locationChangedData(edit_event_widget->stationName(),
		      edit_event_widget->deckNumber());

  int inputnum=edit_recording->switchSource();
  for(int i=0;i<edit_source_box->count();i++) {
    if(edit_source_box->itemData(i).toInt()==inputnum) {
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

  return QDialog::exec();
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
  QString sql=QString("select ")+
    "`NAME`,"+   // 00
    "`NUMBER` "+  // 01
    "from `INPUTS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_deck->switchStation())+"')&&"+
    QString::asprintf("(`MATRIX`=%d)",edit_deck->switchMatrix());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_source_box->
      insertItem(edit_source_box->count(),
		 rda->iconEngine()->listIcon(RDIconEngine::Input),
		 q->value(0).toString(),q->value(1));
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
  if(catch_cut_dialog->exec(&edit_cutname)) {
    edit_description_edit->setText(RDCutPath(edit_cutname));
    if(edit_cutname.isEmpty()) {
      edit_destination_edit->clear();
    }
    else {
      edit_destination_edit->
	setText(tr("Cut")+" "+RDCut::prettyText(edit_cutname));
    }
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
  edit_start_hardtime_radio->setGeometry(20,59,110,15);
  edit_starttime_edit->setGeometry(255,57,120,20);
  edit_starttime_label->setGeometry(135,57,115,20);
  edit_start_gpi_radio->setGeometry(20,80,100,15);
  edit_start_startwindow_edit->setGeometry(255,79,120,20);
  edit_start_startwindow_label->setGeometry(135,79,115,20);
  edit_start_endwindow_edit->setGeometry(495,79,120,20);
  edit_start_endwindow_label->setGeometry(385,79,105,20);
  edit_startmatrix_spin->setGeometry(185,101,35,20);
  edit_startmatrix_label->setGeometry(100,101,80,20);
  edit_startline_spin->setGeometry(305,101,35,20);
  edit_startline_label->setGeometry(240,101,60,20);
  edit_startoffset_edit->setGeometry(455,101,80,20);
  edit_startoffset_time_label->setGeometry(345,101,105,20);
  edit_multirec_box->setGeometry(140,124,size().width()-170,15);

  //
  // End Parameters
  //
  edit_end_groupbox->setGeometry(10,160,size().width()-20,112);
  edit_end_length_radio->setGeometry(20,206,100,15);
  edit_endlength_edit->setGeometry(245,204,80,20);
  edit_endlength_label->setGeometry(125,204,115,20);
  edit_end_hardtime_radio->setGeometry(20,183,1100,15);
  edit_endtime_edit->setGeometry(245,181,120,20);
  edit_endtime_label->setGeometry(125,181,115,20);
  edit_end_gpi_radio->setGeometry(20,229,100,15);
  edit_end_startwindow_edit->setGeometry(245,227,120,20);
  edit_end_startwindow_label->setGeometry(125,227,115,20);
  edit_end_endwindow_edit->setGeometry(495,227,120,20);
  edit_end_endwindow_label->setGeometry(385,227,105,20);
  edit_endmatrix_spin->setGeometry(185,249,35,20);
  edit_endmatrix_label->setGeometry(100,249,80,20);
  edit_endline_spin->setGeometry(295,249,35,20);
  edit_endline_label->setGeometry(230,249,60,20);
  edit_maxlength_edit->setGeometry(455,249,80,20);
  edit_maxlength_label->setGeometry(325,249,125,20);

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
  edit_recording->setSwitchSource(edit_source_box->
		  itemData(edit_source_box->currentIndex()).toInt());
  edit_recording->setStartdateOffset(edit_startoffset_box->value());
  edit_recording->setEnddateOffset(edit_endoffset_box->value());
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

  //
  // Check for Conflicting Events
  //
  int dows=0;
  for(int i=0;i<7;i++) {
    if(edit_dow_selector->dayOfWeekEnabled(i)) {
      dows++;
    }
  }
  if(dows==0) {  // No days scheduled
    return true;
  }

  QString sql=QString("select ")+
    "`ID`,"+           // 00
    "`DESCRIPTION` "+  // 01
    "from `RECORDINGS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`TYPE`=%d)&&",RDRecording::Recording)+
    "(`START_TIME`='"+RDEscapeString(edit_starttime_edit->time().toString("hh:mm:ss"))+"')&&"+
    QString::asprintf("(`CHANNEL`=%d)",edit_event_widget->deckNumber());
  switch((RDRecording::StartType)edit_starttype_group->checkedId()) {
  case RDRecording::HardStart:
    break;

  case RDRecording::GpiStart:
    sql+=QString::asprintf("&&(`START_MATRIX`=%d)&&(`START_LINE`=%d)",
			   edit_startmatrix_spin->value(),
			   edit_startline_spin->value());
    break;
  }
  sql+="&&(";
  if(edit_dow_selector->dayOfWeekEnabled(0)) {
    sql+="(`MON`='Y')||";
  }
  if(edit_dow_selector->dayOfWeekEnabled(1)) {
    sql+="(`TUE`='Y')||";
  }
  if(edit_dow_selector->dayOfWeekEnabled(2)) {
    sql+="(`WED`='Y')||";
  }
  if(edit_dow_selector->dayOfWeekEnabled(3)) {
    sql+="(`THU`='Y')||";
  }
  if(edit_dow_selector->dayOfWeekEnabled(4)) {
    sql+="(`FRI`='Y')||";
  }
  if(edit_dow_selector->dayOfWeekEnabled(5)) {
    sql+="(`SAT`='Y')||";
  }
  if(edit_dow_selector->dayOfWeekEnabled(6)) {
    sql+="(`SUN`='Y')||";
  }
  sql=sql.left(sql.length()-2)+")";
  if(!include_myself) {
    sql+=QString::asprintf("&&(`ID`!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool res=true;
  QString descriptions;
  if(q->first()) {
    res=false;
    descriptions="\""+q->value(1).toString()+"\""+
      QString::asprintf(" [ID: %u]",q->value(0).toUInt());
  }
  delete q;
  if(!res) {
    QMessageBox::warning(this,tr("Conflicting Event"),
			 tr("The parameters of this event conflict with")+"\n"+
			 descriptions+".");
  }

  return res;
}
