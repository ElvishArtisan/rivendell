// edit_recording.cpp
//
// Edit a Rivendell RDCatch Recording
//
//   (C) Copyright 2002-2005,2014 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>

#include <rddb.h>
#include <rd.h>
#include <rdcut_dialog.h>
#include <rdcut_path.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>
#include <rdmatrix.h>

#include <edit_recording.h>
#include <globals.h>


EditRecording::EditRecording(int id,std::vector<int> *adds,QString *filter,
			     QWidget *parent)
  : QDialog(parent,"",true)
{
  QString temp;

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
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont day_font=QFont("Helvetica",10,QFont::Normal);
  day_font.setPixelSize(10);

  edit_deck=NULL;
  edit_added_events=adds;
  edit_filter=filter;

  setCaption(tr("Edit Recording"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(id);

  //
  // Active Button
  //
  edit_active_button=new QCheckBox(tr("Event Active"),this);
  edit_active_button->setGeometry(10,11,145,20);
  edit_active_button->setFont(label_font);

  //
  // Station
  //
  edit_station_box=new QComboBox(this,"edit_station_box");
  edit_station_box->setGeometry(200,10,140,23);
  QLabel * label=new QLabel(edit_station_box,tr("Location:"),this);
  label->setGeometry(125,10,70,23);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(edit_station_box,SIGNAL(activated(int)),
	  this,SLOT(activateStationData(int)));

  //
  // Start Parameters
  //
  edit_starttype_group=new QButtonGroup(this,"edit_starttype_group");
  edit_starttype_group->setGeometry(10,47,sizeHint().width()-20,104);
  connect(edit_starttype_group,SIGNAL(clicked(int)),
	  this,SLOT(startTypeClickedData(int)));

  label=new QLabel(tr("Start Parameters"),this);
  label->setGeometry(47,38,120,19);
  label->setFont(label_font);
  label->setAlignment(AlignHCenter|ShowPrefix);

  QRadioButton *rbutton=new QRadioButton(tr("Use Hard Time"),this);
  rbutton->setGeometry(20,57,100,15);
  edit_starttype_group->insert(rbutton,RDRecording::HardStart);  
  rbutton->setFont(day_font);
  
  edit_starttime_edit=new QTimeEdit(this);
  edit_starttime_edit->setGeometry(235,53,80,20);
  edit_starttime_label=new QLabel(edit_starttime_edit,tr("Record Start Time:"),
				  this);
  edit_starttime_label->setGeometry(125,57,105,15);
  edit_starttime_label->setFont(day_font);
  edit_starttime_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  rbutton=new QRadioButton(tr("Use GPI"),this);
  rbutton->setGeometry(20,81,100,15);
  rbutton->setFont(day_font);
  
  edit_start_startwindow_edit=new QTimeEdit(this);
  edit_start_startwindow_edit->setGeometry(235,77,80,20);
  edit_start_startwindow_label=
    new QLabel(edit_start_startwindow_edit,tr("Window Start Time:"),this);
  edit_start_startwindow_label->setGeometry(125,81,105,15);
  edit_start_startwindow_label->setFont(day_font);
  edit_start_startwindow_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  edit_start_endwindow_edit=new QTimeEdit(this);
  edit_start_endwindow_edit->setGeometry(435,77,80,20);
  edit_start_endwindow_label=
    new QLabel(edit_start_endwindow_edit,"Window End Time:",this);
  edit_start_endwindow_label->setGeometry(325,81,105,15);
  edit_start_endwindow_label->setFont(day_font);
  edit_start_endwindow_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  edit_startmatrix_spin=new QSpinBox(this);
  edit_startmatrix_spin->setGeometry(185,99,30,20);
  edit_startmatrix_spin->setRange(0,MAX_MATRICES-1);
  edit_startmatrix_label=
    new QLabel(edit_startmatrix_spin,tr("GPI Matrix:"),this);
  edit_startmatrix_label->setGeometry(100,100,80,20);
  edit_startmatrix_label->setFont(day_font);
  edit_startmatrix_label->setAlignment(AlignRight|AlignVCenter);

  edit_startline_spin=new QSpinBox(this);
  edit_startline_spin->setGeometry(295,99,30,20);
  edit_startline_spin->setRange(1,MAX_GPIO_PINS);
  edit_startline_label=new QLabel(edit_startline_spin,tr("GPI Line:"),this);
  edit_startline_label->setGeometry(230,100,60,20);
  edit_startline_label->setFont(day_font);
  edit_startline_label->setAlignment(AlignRight|AlignVCenter);

  edit_startoffset_edit=new QTimeEdit(this);
  edit_startoffset_edit->setGeometry(435,99,80,20);
  edit_startoffset_label=
    new QLabel(edit_startoffset_edit,tr("Start Delay:"),this);
  edit_startoffset_label->setGeometry(325,100,105,20);
  edit_startoffset_label->setFont(day_font);
  edit_startoffset_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  edit_multirec_box=
    new QCheckBox(tr("Allow Multiple Recordings within this Window"),this);
  edit_multirec_box->setGeometry(140,124,sizeHint().width()-170,15);
  edit_multirec_box->setFont(day_font);

  edit_starttype_group->insert(rbutton,RDRecording::GpiStart);

  //
  // End Parameters
  //
  edit_endtype_group=new QButtonGroup(this);
  edit_endtype_group->setGeometry(10,171,sizeHint().width()-20,104);
  connect(edit_endtype_group,SIGNAL(clicked(int)),
	  this,SLOT(endTypeClickedData(int)));

  label=new QLabel(tr("End Parameters"),this);
  label->setGeometry(47,162,120,19);
  label->setFont(label_font);
  label->setAlignment(AlignHCenter|ShowPrefix);

  rbutton=new QRadioButton(tr("Use Length"),this);
  rbutton->setGeometry(20,205,100,15);
  edit_endtype_group->insert(rbutton,RDRecording::LengthEnd);  
  rbutton->setFont(day_font);
  edit_endlength_edit=new QTimeEdit(this);
  edit_endlength_edit->setGeometry(235,201,80,20);
  edit_endlength_label=new QLabel(tr("Record Length:"),this);
  edit_endlength_label->setGeometry(125,205,105,15);
  edit_endlength_label->setFont(day_font);
  edit_endlength_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  rbutton=new QRadioButton(tr("Use Hard Time"),this);
  rbutton->setGeometry(20,181,1100,15);
  edit_endtype_group->insert(rbutton,RDRecording::HardEnd);  
  rbutton->setFont(day_font);
  edit_endtime_edit=new QTimeEdit(this);
  edit_endtime_edit->setGeometry(235,177,80,20);
  edit_endtime_label=new QLabel(edit_endtime_edit,tr("Record End Time:"),this);
  edit_endtime_label->setGeometry(125,177,105,15);
  edit_endtime_label->setFont(day_font);
  edit_endtime_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  rbutton=new QRadioButton(tr("Use GPI"),this,"gpi_button");
  rbutton->setGeometry(20,229,100,15);
  rbutton->setFont(day_font);
  edit_end_startwindow_edit=new QTimeEdit(this);
  edit_end_startwindow_edit->setGeometry(235,225,80,20);
  edit_end_startwindow_label=
    new QLabel(edit_end_startwindow_edit,tr("Window Start Time:"),this);
  edit_end_startwindow_label->setGeometry(125,229,105,15);
  edit_end_startwindow_label->setFont(day_font);
  edit_end_startwindow_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  edit_end_endwindow_edit=new QTimeEdit(this);
  edit_end_endwindow_edit->setGeometry(435,225,80,20);
  edit_end_endwindow_label=
    new QLabel(edit_end_endwindow_edit,tr("Window End Time:"),this);
  edit_end_endwindow_label->setGeometry(325,229,105,15);
  edit_end_endwindow_label->setFont(day_font);
  edit_end_endwindow_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  edit_endmatrix_spin=new QSpinBox(this);
  edit_endmatrix_spin->setGeometry(185,247,30,20);
  edit_endmatrix_spin->setRange(0,MAX_MATRICES-1);
  edit_endmatrix_label=new QLabel(edit_endmatrix_spin,tr("GPI Matrix:"),this);
  edit_endmatrix_label->setGeometry(100,248,80,20);
  edit_endmatrix_label->setFont(day_font);
  edit_endmatrix_label->setAlignment(AlignRight|AlignVCenter);

  edit_endline_spin=new QSpinBox(this);
  edit_endline_spin->setGeometry(295,247,30,20);
  edit_endline_spin->setRange(1,MAX_GPIO_PINS);
  edit_endline_label=new QLabel(edit_endline_spin,tr("GPI Line:"),this);
  edit_endline_label->setGeometry(230,248,60,20);
  edit_endline_label->setFont(day_font);
  edit_endline_label->setAlignment(AlignRight|AlignVCenter);

  edit_maxlength_edit=new QTimeEdit(this);
  edit_maxlength_edit->setGeometry(435,247,80,20);
  edit_maxlength_label=
    new QLabel(edit_maxlength_edit,tr("Max Record Length:"),this);
  edit_maxlength_label->setGeometry(325,248,105,20);
  edit_maxlength_label->setFont(day_font);
  edit_maxlength_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  edit_endtype_group->insert(rbutton,RDRecording::GpiEnd);

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setGeometry(105,291,sizeHint().width()-115,20);
  edit_description_edit->setValidator(validator);
  label=new QLabel(edit_description_edit,tr("Description:"),this);
  label->setGeometry(10,291,90,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Source Name
  //
  edit_source_box=new QComboBox(this);
  edit_source_box->setGeometry(105,317,sizeHint().width()-115,24);
  label=new QLabel(edit_source_box,tr("Source:"),this);
  label->setGeometry(10,317,90,24);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Destination
  //
  edit_destination_edit=new QLineEdit(this);
  edit_destination_edit->setGeometry(105,345,sizeHint().width()-185,20);
  edit_destination_edit->setReadOnly(true);
  label=new QLabel(edit_destination_edit,tr("Destination:"),this);
  label->setGeometry(10,345,90,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-70,344,60,24);
  button->setFont(day_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCutData()));

  //
  // Channels
  //
  edit_channels_box=new QComboBox(this);
  edit_channels_box->setGeometry(190,370,40,20);
  edit_channels_box->insertItem("1");
  edit_channels_box->insertItem("2");
  label=new QLabel(edit_channels_box,tr("Channels:"),this);
  label->setGeometry(120,370,70,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignLeft);

  //
  // Autotrim Controls
  //
  edit_autotrim_box=new QCheckBox(tr("Autotrim"),this);
  edit_autotrim_box->setGeometry(120,395,100,15);
  edit_autotrim_box->setFont(label_font);
  connect(edit_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimToggledData(bool)));
  edit_autotrim_spin=new QSpinBox(this);
  edit_autotrim_spin->setGeometry(265,393,40,20);
  edit_autotrim_spin->setRange(-99,-1);
  edit_autotrim_label=
    new QLabel(edit_autotrim_spin,tr("Level:"),this);
  edit_autotrim_label->setGeometry(220,393,40,20);
  edit_autotrim_label->setFont(label_font);
  edit_autotrim_label->setAlignment(AlignVCenter|AlignRight);
  edit_autotrim_unit=
    new QLabel(edit_autotrim_spin,tr("dBFS"),this);
  edit_autotrim_unit->setGeometry(310,393,40,20);
  edit_autotrim_unit->setFont(label_font);
  edit_autotrim_unit->setAlignment(AlignVCenter|AlignLeft);

  //
  // Normalize Controls
  //
  edit_normalize_box=new QCheckBox(tr("Normalize"),this);
  edit_normalize_box->setGeometry(120,420,100,15);
  connect(edit_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeToggledData(bool)));
  edit_normalize_box->setFont(label_font);
  edit_normalize_spin=new QSpinBox(this);
  edit_normalize_spin->setGeometry(265,418,40,20);
  edit_normalize_spin->setRange(-99,-1);
  edit_normalize_label=
    new QLabel(edit_normalize_spin,tr("Level:"),this);
  edit_normalize_label->setGeometry(220,418,40,20);
  edit_normalize_label->setFont(label_font);
  edit_normalize_label->setAlignment(AlignVCenter|AlignRight);
  edit_normalize_unit=
    new QLabel(edit_normalize_spin,tr("dBFS"),this);
  edit_normalize_unit->setGeometry(310,418,40,20);
  edit_normalize_unit->setFont(label_font);
  edit_normalize_unit->setAlignment(AlignVCenter|AlignLeft);

  //
  // Button Label
  //
  label=new QLabel(tr("Active Days"),this);
  label->setGeometry(47,440,90,19);
  label->setFont(label_font);
  label->setAlignment(AlignHCenter|ShowPrefix);

  //
  // Monday Button
  //
  edit_mon_button=new QCheckBox(tr("Monday"),this);
  edit_mon_button->setGeometry(20,459,135,20);
  edit_mon_button->setFont(day_font);

  //
  // Tuesday Button
  //
  edit_tue_button=new QCheckBox(tr("Tuesday"),this);
  edit_tue_button->setGeometry(115,459,135,20);
  edit_tue_button->setFont(day_font);

  //
  // Wednesday Button
  //
  edit_wed_button=new QCheckBox(tr("Wednesday"),this);
  edit_wed_button->setGeometry(215,459,135,20);
  edit_wed_button->setFont(day_font);

  //
  // Thursday Button
  //
  edit_thu_button=new QCheckBox(tr("Thursday"),this);
  edit_thu_button->setGeometry(335,459,135,20);
  edit_thu_button->setFont(day_font);

  //
  // Friday Button
  //
  edit_fri_button=new QCheckBox(tr("Friday"),this);
  edit_fri_button->setGeometry(440,459,135,20);
  edit_fri_button->setFont(day_font);

  //
  // Saturday Button
  //
  edit_sat_button=new QCheckBox(tr("Saturday"),this);
  edit_sat_button->setGeometry(130,484,80,20);
  edit_sat_button->setFont(day_font);

  //
  // Sunday Button
  //
  edit_sun_button=new QCheckBox(tr("Sunday"),this);
  edit_sun_button->setGeometry(300,484,80,20);
  edit_sun_button->setFont(day_font);

  //
  // Start Date Offset
  //
  edit_startoffset_box=new QSpinBox(this);
  edit_startoffset_box->setGeometry(140,516,55,24);
  edit_startoffset_box->setRange(0,355);
  edit_startoffset_box->setSpecialValueText(tr("None"));
  label=new QLabel(edit_startoffset_box,tr("Start Date Offset:"),this);
  label->setGeometry(10,516,125,24);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // End Date Offset
  //
  edit_endoffset_box=new QSpinBox(this);
  edit_endoffset_box->setGeometry(440,516,55,24);
  edit_endoffset_box->setRange(0,355);
  edit_endoffset_box->setSpecialValueText(tr("None"));
  label=new QLabel(edit_endoffset_box,tr("End Date Offset:"),this);
  label->setGeometry(310,516,125,24);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(tr("Make OneShot"),this);
  edit_oneshot_box->setGeometry(20,553,125,15);
  edit_oneshot_box->setFont(label_font);

  //
  //  Save As Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-300,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Save As\nNew"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveasData()));
  if(adds==NULL) {
    button->hide();
  }

  //
  //  Ok Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(button_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  PopulateDecks(edit_station_box);
  edit_active_button->setChecked(edit_recording->isActive());
  edit_starttime_edit->setTime(edit_recording->startTime());
  edit_description_edit->setText(edit_recording->description());
  edit_starttype_group->setButton((int)edit_recording->startType());
  switch((RDRecording::StartType)edit_starttype_group->selectedId()) {
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
	  setTime(QTime().addMSecs(edit_recording->startOffset()));
	edit_multirec_box->
	  setChecked(edit_recording->allowMultipleRecordings());
	break;
  }
  startTypeClickedData(edit_starttype_group->selectedId());
  edit_endtype_group->setButton((int)edit_recording->endType());
  switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
      case RDRecording::LengthEnd:
	edit_endlength_edit->
	  setTime(QTime().addMSecs(edit_recording->length()));
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
    setTime(QTime().addMSecs(edit_recording->maxGpiRecordingLength()));
  endTypeClickedData(edit_endtype_group->selectedId());

  edit_cutname=edit_recording->cutName();
  edit_destination_edit->setText(RDCutPath(edit_cutname));
  edit_mon_button->setChecked(edit_recording->mon());
  edit_tue_button->setChecked(edit_recording->tue());
  edit_wed_button->setChecked(edit_recording->wed());
  edit_thu_button->setChecked(edit_recording->thu());
  edit_fri_button->setChecked(edit_recording->fri());
  edit_sat_button->setChecked(edit_recording->sat());
  edit_sun_button->setChecked(edit_recording->sun());
  edit_startoffset_box->setValue(edit_recording->startdateOffset());
  edit_endoffset_box->setValue(edit_recording->enddateOffset());
  activateStationData(edit_station_box->currentItem(),false);

  QString source=GetSourceName(edit_recording->switchSource());
  for(int i=0;i<edit_source_box->count();i++) {
    if(edit_source_box->text(i)==source) {
      edit_source_box->setCurrentItem(i);
    }
  }
  if(edit_recording->trimThreshold()>0) {
    edit_autotrim_box->setChecked(true);
    edit_autotrim_spin->setValue(-(edit_recording->trimThreshold()/100));
  }
  else {
    edit_autotrim_box->setChecked(false);
    edit_autotrim_spin->setValue(rdlibrary_conf->trimThreshold()/100);
  }
  autotrimToggledData(edit_autotrim_box->isChecked());
  if(edit_recording->normalizationLevel()<0) {
    edit_normalize_box->setChecked(true);
    edit_normalize_spin->setValue(edit_recording->normalizationLevel()/100);
  }
  else {
    edit_normalize_box->setChecked(false);
    edit_normalize_spin->setValue(rdlibrary_conf->ripperLevel()/100);
  }
  normalizeToggledData(edit_normalize_box->isChecked());
  // Populate number of channels; if creating a new recording entry and a valid
  // deck exists, use the deck default for num. channels.  Otherwise use the
  // previously entered (or DB default) recording num. channels.
  if( (edit_recording->station().length() == 0) && (edit_deck!=NULL) ) {
    edit_channels_box->setCurrentItem(edit_deck->defaultChannels()-1);
  } else {
    edit_channels_box->setCurrentItem(edit_recording->channels()-1);
  }
  edit_oneshot_box->setChecked(edit_recording->oneShot());
}


EditRecording::~EditRecording()
{
  delete edit_station_box;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditRecording::sizeHint() const
{
  return QSize(540,619);
} 


QSizePolicy EditRecording::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRecording::activateStationData(int id,bool use_temp)
{
  char station[65];
  char gunk[3];
  int chan;
  QString sql;
  RDSqlQuery *q;
  
  if(edit_station_box->currentText().isEmpty()) {
    return;
  }
  QString temp=edit_source_box->currentText();
  sscanf((const char *)edit_station_box->currentText(),"%s%s%d",
	 station,gunk,&chan);
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
  edit_deck=new RDDeck(station,chan);
  if(edit_channels_box->count()>0) {
    edit_channels_box->setCurrentItem(edit_deck->defaultChannels()-1);
  }
  edit_source_box->clear();
  sql=QString("select NAME from INPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(edit_deck->switchStation())+"\")&&"+
    QString().sprintf("(MATRIX=%d)",edit_deck->switchMatrix());
  q=new RDSqlQuery(sql);
  
  while(q->next()) {
    edit_source_box->insertItem(q->value(0).toString());
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
    switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
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
  edit_startoffset_label->setDisabled(state);
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
      setEnabled(edit_starttype_group->selectedId()==RDRecording::GpiStart);
  }
  if(((RDRecording::EndType)id)==RDRecording::LengthEnd) {
    length_state=true;
    edit_multirec_box->
      setEnabled(edit_starttype_group->selectedId()==RDRecording::GpiStart);
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

  RDCutDialog *cut=new RDCutDialog(&edit_cutname,rdstation_conf,catch_system,
				   edit_filter,NULL,NULL,catch_user->name(),
				   false,true);
  switch(cut->exec()) {
      case 0:
	edit_description_edit->setText(RDCutPath(edit_cutname));
	str=QString(tr("Cut"));
	edit_destination_edit->
	  setText(QString().sprintf("%s %s",(const char *)str,
				    (const char *)edit_cutname));
	break;
  }
  delete cut;
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
  done(0);
}


void EditRecording::cancelData()
{
  done(-1);
}


void EditRecording::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->drawRect(10,447,sizeHint().width()-20,62);
  p->end();
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


void EditRecording::PopulateDecks(QComboBox *box)
{
  int count=0;

  box->clear();
  QString sql=QString("select STATION_NAME,CHANNEL from DECKS \
where (CARD_NUMBER!=-1)&&(PORT_NUMBER!=-1)&&(CHANNEL!=0) \
&&(CHANNEL<9) order by STATION_NAME,CHANNEL");
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    box->insertItem(QString().sprintf("%s : %dR",
		   (const char *)q->value(0).toString(),q->value(1).toInt()));
    if((q->value(0).toString()==edit_recording->station())&&
       (q->value(1).toUInt()==edit_recording->channel())) {
      box->setCurrentItem(count);
    }
    count++;
  }
  if(q->size()>0) {
    if(edit_deck!=NULL) {
      delete edit_deck;
    }
    q->first();
    edit_deck=new RDDeck(q->value(0).toString(),q->value(1).toUInt()); 
    if(edit_channels_box->count()>0) {
      edit_channels_box->setCurrentItem(edit_deck->defaultChannels()-1);
    }
 }
  delete q;
  if(box->count()==0) {  // In case the deck has been disabled
    box->insertItem(QString().sprintf("%s : %dR",
				      (const char *)edit_recording->station(),
				      edit_recording->channel()));
  }
}


void EditRecording::Save()
{
  char station[65];
  char gunk[3];
  int chan;

  sscanf((const char *)edit_station_box->currentText(),"%s%s%d",
	 station,gunk,&chan);
  edit_recording->setIsActive(edit_active_button->isChecked());
  edit_recording->setStation(station);
  edit_recording->setType(RDRecording::Recording);
  edit_recording->setChannel(chan);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_cutname);
  edit_recording->setMon(edit_mon_button->isChecked());
  edit_recording->setTue(edit_tue_button->isChecked());
  edit_recording->setWed(edit_wed_button->isChecked());
  edit_recording->setThu(edit_thu_button->isChecked());
  edit_recording->setFri(edit_fri_button->isChecked());
  edit_recording->setSat(edit_sat_button->isChecked());
  edit_recording->setSun(edit_sun_button->isChecked());
  edit_recording->setSwitchSource(GetSource());
  edit_recording->setStartdateOffset(edit_startoffset_box->value());
  edit_recording->setEnddateOffset(edit_endoffset_box->value());
  edit_recording->setFormat(edit_deck->defaultFormat());
  if(edit_deck->defaultFormat()>0) {
    edit_recording->setBitrate(edit_deck->defaultBitrate()*
			       (edit_channels_box->currentItem()+1));
  }
  else {
    edit_recording->setBitrate(0);
  }
  edit_recording->setChannels(edit_channels_box->currentItem()+1);
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
    setStartType((RDRecording::StartType)edit_starttype_group->selectedId());
  switch((RDRecording::StartType)edit_starttype_group->selectedId()) {
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
	  setStartOffset(QTime().msecsTo(edit_startoffset_edit->time()));
	edit_recording->
	  setAllowMultipleRecordings(edit_multirec_box->isChecked());
	break;
  }
  edit_recording->
    setEndType((RDRecording::EndType)edit_endtype_group->selectedId());
  edit_recording->
    setMaxGpiRecordingLength(QTime().msecsTo(edit_maxlength_edit->time()));
  switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
      case RDRecording::LengthEnd:
	edit_recording->
	  setLength(QTime().msecsTo(edit_endlength_edit->time()));
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
  char station[65];
  char gunk[3];
  int chan;
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
  switch((RDRecording::StartType)edit_starttype_group->selectedId()) {
      case RDRecording::GpiStart:
	if(edit_start_startwindow_edit->time()>=
	   edit_start_endwindow_edit->time()) {
	  QMessageBox::warning(this,tr("Record Parameter Error"),
	            tr("The start GPI window cannot end before it begins!"));
	  return false;
	}
	switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
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
	      break;
	}
	break;

      case RDRecording::HardStart:
	switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
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
	      break;
	}
	break;
  }
  switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
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
  sscanf((const char *)edit_station_box->currentText(),"%s%s%d",
	 station,gunk,&chan);
  switch((RDRecording::StartType)edit_starttype_group->selectedId()) {
      case RDRecording::GpiStart:
	matrix=new RDMatrix(station,edit_startmatrix_spin->value());
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
  switch((RDRecording::EndType)edit_endtype_group->selectedId()) {
      case RDRecording::GpiEnd:
	matrix=new RDMatrix(station,edit_endmatrix_spin->value());
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

  QString sql=
    QString().sprintf("select ID from RECORDINGS \
                       where (STATION_NAME=\"%s\")&&\
                       (TYPE=%d)&&(START_TIME=\"%s\")&&\
                       (CHANNEL=%d)",
		      station,RDRecording::Recording,
		      (const char *)edit_starttime_edit->time().
		      toString("hh:mm:ss"),chan);
  switch((RDRecording::StartType)edit_starttype_group->selectedId()) {
      case RDRecording::HardStart:
	break;

      case RDRecording::GpiStart:
	sql+=QString().sprintf("&&(START_MATRIX=%d)&&(START_LINE=%d)",
			       edit_startmatrix_spin->value(),
			       edit_startline_spin->value());
	break;
  }
  if(edit_sun_button->isChecked()) {
    sql+="&&(SUN=\"Y\")";
  }
  if(edit_mon_button->isChecked()) {
    sql+="&&(MON=\"Y\")";
  }
  if(edit_tue_button->isChecked()) {
    sql+="&&(TUE=\"Y\")";
  }
  if(edit_wed_button->isChecked()) {
    sql+="&&(WED=\"Y\")";
  }
  if(edit_thu_button->isChecked()) {
    sql+="&&(THU=\"Y\")";
  }
  if(edit_fri_button->isChecked()) {
    sql+="&&(FRI=\"Y\")";
  }
  if(edit_sat_button->isChecked()) {
    sql+="&&(SAT=\"Y\")";
  }
  if(!include_myself) {
    sql+=QString().sprintf("&&(ID!=%d)",edit_recording->id());
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
  QString sql=QString().sprintf("select NAME from INPUTS where \
                                 (STATION_NAME=\"%s\")&&\
                                 (MATRIX=%d)&&(NUMBER=%d)",
				(const char *)edit_deck->switchStation(),
				edit_deck->switchMatrix(),input);
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

  QString sql=QString().sprintf("select NUMBER from INPUTS where \
                               (STATION_NAME=\"%s\")&&(MATRIX=%d)&&\
                               (NAME=\"%s\")",
			       (const char *)edit_deck->switchStation(),
				edit_deck->switchMatrix(),
			       (const char *)edit_source_box->currentText());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    source=q->value(0).toInt();
  }
  delete q;
  return source;
}
