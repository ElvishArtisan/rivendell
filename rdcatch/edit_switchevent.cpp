// edit_switchevent.cpp
//
// Edit a Rivendell Netcatch Cart Event
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qgroupbox.h>
#include <qmessagebox.h>

#include <rdcart_dialog.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_switchevent.h"

EditSwitchEvent::EditSwitchEvent(int id,std::vector<int> *adds,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  QString temp;
  edit_matrix=NULL;
  edit_added_events=adds;

  edit_deck=NULL;

  setWindowTitle("RDCatch - "+tr("Edit Switcher Event"));

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
  // Active Button
  //
  edit_active_button=new QCheckBox(this);
  edit_active_button->setGeometry(10,11,20,20);
  QLabel *label=new QLabel(edit_active_button,tr("Event Active"),this);
  label->setGeometry(30,11,125,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Station
  //
  edit_station_box=new QComboBox(this);
  edit_station_box->setGeometry(200,10,140,23);
  label=new QLabel(edit_station_box,tr("Location:"),this);
  label->setGeometry(125,10,70,23);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  connect(edit_station_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateStationData(const QString &)));

  //
  // Start Time
  //
  edit_starttime_edit=new QTimeEdit(this);
  edit_starttime_edit->setGeometry(sizeHint().width()-90,12,80,20);
  edit_starttime_edit->setDisplayFormat("hh:mm:ss");
  label=new QLabel(edit_starttime_edit,tr("Start Time:"),this);
  label->setGeometry(sizeHint().width()-175,12,80,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setGeometry(120,43,sizeHint().width()-130,20);
  edit_description_edit->setValidator(validator);
  label=new QLabel(edit_description_edit,tr("Description:"),this);
  label->setGeometry(10,43,105,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Switch Matrix
  //
  edit_matrix_box=new QComboBox(this,"matrix");
  edit_matrix_box->setGeometry(120,70,sizeHint().width()-130,20);
  label=new QLabel(edit_matrix_box,tr("Switch Matrix:"),this);
  label->setGeometry(10,70,105,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  connect(edit_matrix_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateMatrixData(const QString &)));

  //
  // Switch Input
  //
  edit_input_box=new QComboBox(this,"input");
  edit_input_box->setGeometry(120,100,sizeHint().width()-130,20);
  label=new QLabel(edit_input_box,tr("Switch Input:"),this);
  label->setGeometry(10,100,105,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  edit_input_spin=new QSpinBox(this);
  edit_input_spin->setGeometry(140,125,50,20);
  connect(edit_input_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateInputData(const QString &)));

  //
  // Switch Output
  //
  edit_output_box=new QComboBox(this,"output");
  edit_output_box->setGeometry(120,155,sizeHint().width()-130,20);
  label=new QLabel(edit_output_box,tr("Switch Output:"),this);
  label->setGeometry(10,155,105,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  edit_output_spin=new QSpinBox(this);
  edit_output_spin->setGeometry(140,180,50,20);
  connect(edit_output_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateOutputData(const QString &)));

  //
  // Button Label
  //
  QGroupBox *groupbox=new QGroupBox(tr("Active Days"),this);
  groupbox->setFont(labelFont());
  groupbox->setGeometry(10,213,sizeHint().width()-20,62);

  //
  // Monday Button
  //
  edit_mon_button=new QCheckBox(this);
  edit_mon_button->setGeometry(20,228,20,20);
  label=new QLabel(edit_mon_button,tr("Monday"),this);
  label->setGeometry(40,228,115,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Tuesday Button
  //
  edit_tue_button=new QCheckBox(this);
  edit_tue_button->setGeometry(115,228,20,20);
  label=new QLabel(edit_tue_button,tr("Tuesday"),this);
  label->setGeometry(135,228,115,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Wednesday Button
  //
  edit_wed_button=new QCheckBox(this);
  edit_wed_button->setGeometry(215,228,20,20);
  label=new QLabel(edit_wed_button,tr("Wednesday"),this);
  label->setGeometry(235,228,115,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Thursday Button
  //
  edit_thu_button=new QCheckBox(this);
  edit_thu_button->setGeometry(335,228,20,20);
  label=new QLabel(edit_thu_button,tr("Thursday"),this);
  label->setGeometry(355,228,115,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Friday Button
  //
  edit_fri_button=new QCheckBox(this);
  edit_fri_button->setGeometry(440,228,20,20);
  label=new QLabel(edit_fri_button,tr("Friday"),this);
  label->setGeometry(460,228,40,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Saturday Button
  //
  edit_sat_button=new QCheckBox(this);
  edit_sat_button->setGeometry(130,253,20,20);
  label=new QLabel(edit_sat_button,tr("Saturday"),this);
  label->setGeometry(150,253,60,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Sunday Button
  //
  edit_sun_button=new QCheckBox(this);
  edit_sun_button->setGeometry(300,253,20,20);
  label=new QLabel(edit_sun_button,tr("Sunday"),this);
  label->setGeometry(320,253,60,20);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this);
  edit_oneshot_box->setGeometry(20,290,15,15);
  label=new QLabel(edit_oneshot_box,tr("Make OneShot"),this);
  label->setGeometry(40,288,115,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  Save As Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-300,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
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
  button->setFont(buttonFont());
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_active_button->setChecked(edit_recording->isActive());
  q=new RDSqlQuery("select NAME from STATIONS where NAME!=\"DEFAULT\"");
  while(q->next()) {
    edit_station_box->insertItem(q->value(0).toString());
    if(edit_recording->station()==q->value(0).toString()) {
      edit_station_box->setCurrentItem(edit_station_box->count()-1);
    }
  }
  delete q;
  edit_starttime_edit->setTime(edit_recording->startTime());
  edit_description_edit->setText(edit_recording->description());
  edit_mon_button->setChecked(edit_recording->mon());
  edit_tue_button->setChecked(edit_recording->tue());
  edit_wed_button->setChecked(edit_recording->wed());
  edit_thu_button->setChecked(edit_recording->thu());
  edit_fri_button->setChecked(edit_recording->fri());
  edit_sat_button->setChecked(edit_recording->sat());
  edit_sun_button->setChecked(edit_recording->sun());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  activateStationData(edit_station_box->currentText());

  //
  // Input/Output Spin Box Connections
  // (Placed here to avoid a QComboBox::changeItem error)
  //
  connect(edit_input_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(inputChangedData(int)));
  connect(edit_output_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(outputChangedData(int)));
}


EditSwitchEvent::~EditSwitchEvent()
{
  delete edit_station_box;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditSwitchEvent::sizeHint() const
{
  return QSize(540,360);
} 


QSizePolicy EditSwitchEvent::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSwitchEvent::activateStationData(const QString &str)
{
  QString sql=QString("select ")+
    "NAME,"+
    "MATRIX "+
    "from MATRICES where "+
    "(STATION_NAME=\""+RDEscapeString(str)+"\")&&"+
    "(INPUTS>0)&&"
    "(OUTPUTS>0) "+
    "order by NAME";
  edit_matrix_box->clear();
  RDSqlQuery *q=new RDSqlQuery(sql);
  int matrix=edit_recording->channel();
  while(q->next()) {
    edit_matrix_box->insertItem(q->value(0).toString());
    if(q->value(1).toInt()==matrix) {
      edit_matrix_box->setCurrentItem(edit_matrix_box->count()-1);
    }
  }
  delete q;
  activateMatrixData(edit_matrix_box->currentText());
}


void EditSwitchEvent::activateMatrixData(const QString &str)
{
  int inputs;
  int outputs;

  if(edit_matrix!=NULL) {
    delete edit_matrix;
  }
  edit_matrix=new RDMatrix(edit_station_box->currentText(),GetMatrix());
  QString sql=QString("select ")+
    "NAME,"
    "NUMBER "+
    "from INPUTS where "+
    "(STATION_NAME=\""+RDEscapeString(edit_station_box->currentText())+"\")&&"+
    QString().sprintf("(MATRIX=%d) ",GetMatrix())+
    "order by NAME";
  edit_input_box->clear();
  edit_input_box->insertItem(tr("--OFF--"));
  int input=edit_recording->switchSource();
  if((inputs=edit_matrix->inputs())>0) {
    edit_input_spin->setRange(0,inputs);
    edit_input_spin->setEnabled(true);
  }
  else {
    edit_input_spin->setRange(0,0);
    edit_input_spin->setDisabled(true);
  }
  edit_input_spin->setValue(input);
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_input_box->insertItem(q->value(0).toString());
    if(q->value(1).toInt()==input) {
      edit_input_box->setCurrentItem(edit_input_box->count()-1);
    }
  }
  delete q;

  sql=QString("select ")+
    "NAME,"+
    "NUMBER "+
    "from OUTPUTS where "+
    "(STATION_NAME=\""+RDEscapeString(edit_station_box->currentText())+"\")&&"+
    QString().sprintf("(MATRIX=%d) ",GetMatrix())+
    "order by NAME";
  edit_output_box->clear();
  int output=edit_recording->switchDestination();
  if((outputs=edit_matrix->outputs())>0) {
    edit_output_spin->setRange(1,outputs);
    edit_output_spin->setEnabled(true);
  }
  else {
    edit_output_spin->setRange(0,0);
    edit_output_spin->setDisabled(true);
  }
  edit_output_spin->setValue(output);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_output_box->insertItem(q->value(0).toString());
    if(q->value(1).toInt()==output) {
      edit_output_box->setCurrentItem(edit_output_box->count()-1);
    }
  }
  delete q;
}


void EditSwitchEvent::activateInputData(const QString &str)
{
  edit_input_spin->setValue(GetSource());
}


void EditSwitchEvent::activateOutputData(const QString &str)
{
  edit_output_spin->setValue(GetDestination());
}


void EditSwitchEvent::inputChangedData(int value)
{
  if(value>0) {
    edit_input_box->setCurrentText(edit_matrix->inputName(value));
  }
  else {
    edit_input_box->setCurrentText(tr("--OFF--"));
  }
}


void EditSwitchEvent::outputChangedData(int value)
{
  if((value>0)&&(edit_output_box->count()>0)) {
    edit_output_box->setCurrentText(edit_matrix->outputName(value));
  }
}


void EditSwitchEvent::saveasData()
{
  if(!CheckEvent(true)) {
    QMessageBox::warning(this,tr("Duplicate Event"),
		     tr("An event with these parameters already exists!"));
    return;
  }
  delete edit_recording;
  edit_recording=new RDRecording(-1,true);
  edit_added_events->push_back(edit_recording->id());
  Save();
}


void EditSwitchEvent::okData()
{
  if(!CheckEvent(false)) {
    QMessageBox::warning(this,tr("Duplicate Event"),
		     tr("An event with these parameters already exists!"));
    return;
  }
  Save();
  done(true);
}


void EditSwitchEvent::cancelData()
{
  done(false);
}


void EditSwitchEvent::keyPressEvent(QKeyEvent *e)
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


void EditSwitchEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditSwitchEvent::Save()
{
  int matrix=GetMatrix();
  int source=GetSource();
  int dest=GetDestination();

  edit_recording->setIsActive(edit_active_button->isChecked());
  edit_recording->setStation(edit_station_box->currentText());
  edit_recording->setType(RDRecording::SwitchEvent);
  edit_recording->setStartTime(edit_starttime_edit->time());
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setChannel(matrix);
  edit_recording->setSwitchSource(source);
  edit_recording->setSwitchDestination(dest);
  edit_recording->setMon(edit_mon_button->isChecked());
  edit_recording->setTue(edit_tue_button->isChecked());
  edit_recording->setWed(edit_wed_button->isChecked());
  edit_recording->setThu(edit_thu_button->isChecked());
  edit_recording->setFri(edit_fri_button->isChecked());
  edit_recording->setSat(edit_sat_button->isChecked());
  edit_recording->setSun(edit_sun_button->isChecked());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


int EditSwitchEvent::GetMatrix()
{
  int matrix=-1;

  QString sql=QString("select ")+
    "MATRIX "+
    "from MATRICES where "+
    "(STATION_NAME=\""+RDEscapeString(edit_station_box->currentText())+"\")&&"
    "(NAME=\""+RDEscapeString(edit_matrix_box->currentText())+"\")&&"+
    "(INPUTS>0)&&"+
    "(OUTPUTS>0)";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    matrix=q->value(0).toInt();
  }
  delete q;
  return matrix;
}


int EditSwitchEvent::GetSource()
{
  int input=0;

  QString sql=QString("select ")+
    "NUMBER "+
    "from INPUTS where "+
    "(STATION_NAME=\""+RDEscapeString(edit_station_box->currentText())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",GetMatrix())+
    "(NAME=\""+RDEscapeString(edit_input_box->currentText())+"\")";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    input=q->value(0).toInt();
  }
  delete q;
  return input;
}


int EditSwitchEvent::GetDestination()
{
  int output=-1;

  QString sql=QString("select ")+
    "NUMBER "+
    "from OUTPUTS where "+
    "(STATION_NAME=\""+RDEscapeString(edit_station_box->currentText())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",GetMatrix())+
    "(NAME=\""+RDEscapeString(edit_output_box->currentText())+"\")";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output=q->value(0).toInt();
  }
  delete q;
  return output;
}


bool EditSwitchEvent::CheckEvent(bool include_myself)
{
  QString sql=QString("select ")+
    "ID "+
    "from RECORDINGS where "+
    "(STATION_NAME=\""+RDEscapeString(edit_station_box->currentText())+"\")&&"+
    QString().sprintf("(TYPE=%d)&&",RDRecording::SwitchEvent)+
    "(START_TIME=\""+
    RDEscapeString(edit_starttime_edit->time().toString("hh:mm:ss"))+"\")&&"+
    QString().sprintf("(CHANNEL=%d)&&",GetMatrix())+
    QString().sprintf("(SWITCH_INPUT=%d)&&",GetSource())+
    QString().sprintf("(SWITCH_OUTPUT=%d)",GetDestination());
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

  return res;
}
