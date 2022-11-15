// edit_playout.cpp
//
// Edit a Rivendell RDCatch Playout
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
#include <rdtextvalidator.h>

#include "edit_playout.h"
#include "globals.h"

EditPlayout::EditPlayout(QString *filter,QWidget *parent)
  : RDDialog(parent)
{
  QString temp;

  edit_deck=NULL;
  edit_added_events=NULL;
  edit_recording=NULL;
  edit_filter=filter;

  setWindowTitle("RDCatch - "+tr("Edit Playout"));

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
  // Dialogs
  //
  edit_cut_dialog=new RDCutDialog(edit_filter,NULL,NULL,false,false,false,
				  "RDCatch",false,this);

  //
  // Event Widget
  //
  edit_event_widget=new EventWidget(EventWidget::PlayEvent,this);
  connect(edit_event_widget,SIGNAL(locationChanged(const QString &,int)),
	  this,SLOT(locationChangedData(const QString &,int)));

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setValidator(validator);
  edit_description_label=new QLabel(tr("Description:"),this);
  edit_description_label->setFont(labelFont());
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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


  // DOW Selector
  //
  edit_dow_selector=new DowSelector(this);

  //
  // OneShot Checkbox
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
}


EditPlayout::~EditPlayout()
{
  delete edit_cut_dialog;
  delete edit_event_widget;
  delete edit_dow_selector;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditPlayout::sizeHint() const
{
  return QSize(540,245);
} 


QSizePolicy EditPlayout::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditPlayout::exec(int id,std::vector<int> *adds)
{
  edit_added_events=adds;
  if(edit_added_events==NULL) {
    edit_saveas_button->hide();
  }
  else {
    edit_saveas_button->show();
  }
  if(edit_recording!=NULL) {
    delete edit_recording;
  }
  edit_recording=new RDRecording(id);

  edit_event_widget->fromRecording(edit_recording->id());
  edit_description_edit->setText(edit_recording->description());
  edit_cutname=edit_recording->cutName();
  if(edit_cutname.isEmpty()) {
    edit_destination_edit->clear();
  }
  else {
    edit_destination_edit->
      setText(tr("Cut")+" "+RDCut::prettyText(edit_cutname));
  }
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  locationChangedData(edit_event_widget->stationName(),
		      edit_event_widget->deckNumber());

  return QDialog::exec();
}


void EditPlayout::locationChangedData(const QString &station,int decknum)
{
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
  edit_deck=new RDDeck(station,decknum);
}


void EditPlayout::selectCutData()
{
  if(edit_cut_dialog->exec(&edit_cutname)) {
    if(edit_cutname.isEmpty()) {
      edit_destination_edit->clear();
      edit_description_edit->clear();
    }
    else {
      edit_destination_edit->
	setText(tr("Cut")+" "+RDCut::prettyText(edit_cutname));
      edit_description_edit->setText(RDCutPath(edit_cutname));
    }
  }
}


void EditPlayout::saveasData()
{
  if(!CheckEvent(true)) {
    return;
  }
  delete edit_recording;
  edit_recording=new RDRecording(-1,true);
  edit_added_events->push_back(edit_recording->id());
  Save();
}


void EditPlayout::okData()
{
  if(!CheckEvent(false)) {
    return;
  }
  Save();
  done(true);
}


void EditPlayout::cancelData()
{
  done(false);
}


void EditPlayout::resizeEvent(QResizeEvent *e)
{
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());

  edit_description_edit->setGeometry(105,43,size().width()-115,20);
  edit_description_label->setGeometry(10,43,90,20);

  edit_destination_edit->setGeometry(105,70,size().width()-185,20);
  edit_destination_label->setGeometry(10,70,90,20);
  edit_destination_button->setGeometry(size().width()-70,65,60,30);

  edit_dow_selector->setGeometry(10,104,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());

  edit_oneshot_box->setGeometry(20,180,15,15);
  edit_oneshot_label->setGeometry(40,178,115,20);

  edit_saveas_button->setGeometry(size().width()-300,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditPlayout::keyPressEvent(QKeyEvent *e)
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


void EditPlayout::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditPlayout::Save()
{
  edit_event_widget->toRecording(edit_recording->id());
  edit_recording->setType(RDRecording::Playout);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_cutname);
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


bool EditPlayout::CheckEvent(bool include_myself)
{
  //
  // Source Cut
  //
  if(edit_cutname.isEmpty()) {
    QMessageBox::warning(this,tr("Missing Cut"),
			 tr("You must assign a source cut!"));
    return false;
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
    QString::asprintf("(`TYPE`=%d)&&",RDRecording::Playout)+
    "(`START_TIME`='"+RDEscapeString(edit_event_widget->startTime().
				     toString("hh:mm:ss"))+"')&&"+
    QString::asprintf("(`CHANNEL`=%d)",128+edit_event_widget->deckNumber());
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
    QMessageBox::warning(this,"RDCatch - "+tr("Conflicting Event"),
			 tr("The parameters of this event conflict with")+"\n"+
			 descriptions+".");
  }

  return res;
}
