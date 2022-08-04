// edit_switchevent.cpp
//
// Edit a Rivendell Netcatch Cart Event
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

#include <rdcart_dialog.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_switchevent.h"

EditSwitchEvent::EditSwitchEvent(QWidget *parent)
  : RDDialog(parent)
{
  edit_matrix=NULL;
  edit_added_events=NULL;
  edit_recording=NULL;

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
  //  edit_recording=new RDRecording(id);

  //
  // Event Widget
  //
  edit_event_widget=new EventWidget(EventWidget::OtherEvent,this);
  connect(edit_event_widget,SIGNAL(locationChanged(const QString &)),
	  this,SLOT(activateStationData(const QString &)));
	  
  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setValidator(validator);
  edit_description_label=new QLabel(tr("Description:"),this);
  edit_description_label->setFont(labelFont());
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Switch Matrix
  //
  edit_matrix_box=new RDComboBox(this);
  edit_matrix_label=new QLabel(tr("Switch Matrix:"),this);
  edit_matrix_label->setFont(labelFont());
  edit_matrix_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_matrix_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateMatrixData(const QString &)));

  //
  // Switch Input
  //
  edit_input_box=new RDComboBox(this);
  edit_input_label=new QLabel(tr("Switch Input:"),this);
  edit_input_label->setFont(labelFont());
  edit_input_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_input_spin=new QSpinBox(this);
  connect(edit_input_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateInputData(const QString &)));

  //
  // Switch Output
  //
  edit_output_box=new RDComboBox(this);
  edit_output_label=new QLabel(tr("Switch Output:"),this);
  edit_output_label->setFont(labelFont());
  edit_output_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_output_spin=new QSpinBox(this);
  connect(edit_output_box,SIGNAL(activated(const QString &)),
	  this,SLOT(activateOutputData(const QString &)));

  //
  // DOW Selector
  //
  edit_dow_selector=new DowSelector(this);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this);
  edit_oneshot_label=new QLabel(tr("Make OneShot"),this);
  edit_oneshot_label->setFont(labelFont());
  edit_oneshot_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

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

  //
  // Populate Data
  //
  /*
  edit_event_widget->fromRecording(edit_recording->id());
  edit_description_edit->setText(edit_recording->description());
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  activateStationData(edit_event_widget->stationName());
  */
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
  delete edit_event_widget;
  delete edit_dow_selector;
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


int EditSwitchEvent::exec(int id,std::vector<int> *adds)
{
  if(edit_recording!=NULL) {
    delete edit_recording;
  }
  edit_added_events=adds;
  if(edit_added_events==NULL) {
    edit_saveas_button->hide();
  }
  else {
    edit_saveas_button->show();
  }

  edit_recording=new RDRecording(id);

  edit_event_widget->fromRecording(edit_recording->id());
  edit_description_edit->setText(edit_recording->description());
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  activateStationData(edit_event_widget->stationName());

  return QDialog::exec();
}


void EditSwitchEvent::activateStationData(const QString &str)
{
  QString sql=QString("select ")+
    "`NAME`,"+
    "`MATRIX` "+
    "from `MATRICES` where "+
    "(`STATION_NAME`='"+RDEscapeString(str)+"')&&"+
    "(`INPUTS`>0)&&"
    "(`OUTPUTS`>0) "+
    "order by `NAME`";
  edit_matrix_box->clear();
  RDSqlQuery *q=new RDSqlQuery(sql);
  int matrix=edit_recording->channel();
  while(q->next()) {
    edit_matrix_box->insertItem(q->value(0).toString());
    if(q->value(1).toInt()==matrix) {
      edit_matrix_box->setCurrentIndex(edit_matrix_box->count()-1);
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
  edit_matrix=new RDMatrix(edit_event_widget->stationName(),GetMatrix());
  QString sql=QString("select ")+
    "`NAME`,"
    "`NUMBER` "+
    "from `INPUTS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`MATRIX`=%d) ",GetMatrix())+
    "order by `NUMBER`";
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
      edit_input_box->setCurrentIndex(edit_input_box->count()-1);
    }
  }
  delete q;

  sql=QString("select ")+
    "`NAME`,"+
    "`NUMBER` "+
    "from `OUTPUTS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`MATRIX`=%d) ",GetMatrix())+
    "order by `NUMBER`";
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
      edit_output_box->setCurrentIndex(edit_output_box->count()-1);
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


void EditSwitchEvent::resizeEvent(QResizeEvent *e)
{
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());

  edit_description_edit->setGeometry(120,43,size().width()-130,20);
  edit_description_label->setGeometry(10,43,105,20);

  edit_matrix_box->setGeometry(120,70,size().width()-130,20);
  edit_matrix_label->setGeometry(10,70,105,20);

  edit_input_box->setGeometry(120,100,size().width()-130,20);
  edit_input_label->setGeometry(10,100,105,20);
  edit_input_spin->setGeometry(140,125,50,20);

  edit_output_box->setGeometry(120,155,size().width()-130,20);
  edit_output_label->setGeometry(10,155,105,20);
  edit_output_spin->setGeometry(140,180,50,20);

  edit_dow_selector->setGeometry(10,213,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());

  edit_oneshot_box->setGeometry(20,290,15,15);
  edit_oneshot_label->setGeometry(40,288,115,20);

  edit_saveas_button->setGeometry(size().width()-300,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
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

  edit_event_widget->toRecording(edit_recording->id());
  edit_recording->setType(RDRecording::SwitchEvent);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setChannel(matrix);
  edit_recording->setSwitchSource(source);
  edit_recording->setSwitchDestination(dest);
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


int EditSwitchEvent::GetMatrix()
{
  int matrix=-1;

  QString sql=QString("select ")+
    "`MATRIX` "+
    "from `MATRICES` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"
    "(`NAME`='"+RDEscapeString(edit_matrix_box->currentText())+"')&&"+
    "(`INPUTS`>0)&&"+
    "(`OUTPUTS`>0)";
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
    "`NUMBER` "+
    "from `INPUTS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`MATRIX`=%d)&&",GetMatrix())+
    "(`NAME`='"+RDEscapeString(edit_input_box->currentText())+"')";
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
    "`NUMBER` "+
    "from `OUTPUTS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`MATRIX`=%d)&&",GetMatrix())+
    "(`NAME`='"+RDEscapeString(edit_output_box->currentText())+"')";
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
    "`ID` "+
    "from `RECORDINGS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`TYPE`=%d)&&",RDRecording::SwitchEvent)+
    "(`START_TIME`='"+
    RDEscapeString(edit_event_widget->startTime().toString("hh:mm:ss"))+"')&&"+
    QString::asprintf("(`CHANNEL`=%d)&&",GetMatrix())+
    QString::asprintf("(`SWITCH_INPUT`=%d)&&",GetSource())+
    QString::asprintf("(`SWITCH_OUTPUT`=%d)",GetDestination());
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
    sql+=QString::asprintf("&&(`ID`!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool res=!q->first();
  delete q;

  return res;
}
