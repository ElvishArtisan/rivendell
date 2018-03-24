// edit_chain.cpp
//
// Edit a Rivendell Log Chain Entry
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpushbutton.h>
#include <qmessagebox.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdlist_logs.h>

#include <edit_chain.h>

#include "globals.h"

EditChain::EditChain(RDLogLine *line,QWidget *parent)
  : QDialog(parent,"",true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Edit Log Chain"));

  edit_logline=line;

  // 
  // Create Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont radio_font=QFont("Helvetica",10,QFont::Normal);
  radio_font.setPixelSize(10);

  //
  // Time Type
  //
  edit_timetype_box=new QCheckBox(this);
  edit_timetype_box->setGeometry(10,22,15,15);
  edit_timetype_label=new QLabel(edit_timetype_box,tr("Start at:"),this);
  edit_timetype_label->setGeometry(30,21,85,17);
  edit_timetype_label->setFont(label_font);
  edit_timetype_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Start Time
  //
  edit_time_edit=new RDTimeEdit(this);
  edit_time_edit->setGeometry(85,19,85,20);
  edit_time_edit->setDisplay(RDTimeEdit::Hours|RDTimeEdit::Minutes|
			     RDTimeEdit::Seconds|RDTimeEdit::Tenths);
  connect(edit_time_edit,SIGNAL(valueChanged(const QTime &)),
	  this,SLOT(timeChangedData(const QTime &)));

  //
  // Grace Time
  //
  edit_grace_group
    =new QButtonGroup(1,Qt::Vertical,
		      tr("Action If Previous Event Still Playing"),this);
  edit_grace_group->setGeometry(175,11,435,50);
  edit_grace_group->setFont(label_font);
  edit_grace_group->setRadioButtonExclusive(true);
  QRadioButton *radio_button=
    new QRadioButton(tr("Start Immediately"),edit_grace_group);
  edit_grace_group->insert(radio_button);
  radio_button->setFont(radio_font);
  radio_button=new QRadioButton(tr("Make Next"),edit_grace_group);
  edit_grace_group->insert(radio_button);
  radio_button->setFont(radio_font);
  radio_button=new QRadioButton(tr("Wait up to"),edit_grace_group);
  edit_grace_group->insert(radio_button);
  radio_button->setFont(radio_font);
  edit_grace_box=new QTimeEdit(this);
  edit_grace_box->setGeometry(543,31,60,20);
  edit_grace_box->setDisplay(QTimeEdit::Minutes|QTimeEdit::Seconds);
  connect(edit_timetype_box,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(edit_grace_group,SIGNAL(clicked(int)),
	  this,SLOT(graceClickedData(int)));

  //
  // Transition Type
  //
  edit_transtype_box=new QComboBox(this);
  edit_transtype_box->setGeometry(385,68,110,26);
  edit_transtype_box->insertItem(tr("Play"));
  edit_transtype_box->insertItem(tr("Segue"));
  edit_transtype_box->insertItem(tr("Stop"));  
  edit_time_label=new QLabel(edit_transtype_box,tr("Transition Type:"),this);
  edit_time_label->setGeometry(10,68,370,26);
  edit_time_label->setFont(label_font);
  edit_time_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Label
  //
  edit_label_edit=new QLineEdit(this);
  edit_label_edit->setGeometry(10,116,sizeHint().width()-90,18);
  edit_label_edit->setMaxLength(64);
  QLabel *label=new QLabel(tr("Log Name"),this);
  label->setFont(label_font);
  label->setGeometry(12,100,160,14);
  connect(edit_label_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(labelChangedData(const QString &)));

  //
  // Select Log Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-70,111,60,28);
  button->setDefault(true);
  button->setFont(radio_font);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectLogData()));

  //
  // Comment
  //
  edit_comment_edit=new QLineEdit(this);
  edit_comment_edit->setGeometry(10,156,sizeHint().width()-20,18);
  edit_comment_edit->setReadOnly(true);
  label=new QLabel(tr("Log Description"),this);
  label->setFont(label_font);
  label->setGeometry(12,140,160,14);

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
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_time_edit->setTime(edit_logline->startTime(RDLogLine::Logged));
  if(edit_logline->timeType()==RDLogLine::Hard) {
    edit_timetype_box->setChecked(true);
    timeToggledData(true);
  }
  else {
    timeToggledData(false);
  }
  timeChangedData(edit_time_edit->time());
  switch(edit_logline->graceTime()) {
  case -1:
    edit_grace_group->setButton(1);
    graceClickedData(1);
    break;

  case 0:
    edit_grace_group->setButton(0);
    graceClickedData(0);
    break;

  default:
    edit_grace_group->setButton(2);
    edit_grace_box->setTime(QTime().addMSecs(edit_logline->graceTime()));
    graceClickedData(2);
    break;
  }
  edit_transtype_box->setCurrentItem(edit_logline->transType());
  edit_label_edit->setText(edit_logline->markerLabel());
  labelChangedData(edit_label_edit->text());
}


EditChain::~EditChain()
{
}


QSize EditChain::sizeHint() const
{
  return QSize(625,260);
} 


QSizePolicy EditChain::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditChain::timeChangedData(const QTime &time)
{
  QString str;

  if(edit_timetype_box->isChecked()) {
    str=QString(tr("Transition If Previous Cart Ends Before"));
    edit_time_label->
      setText(QString().sprintf("%s %s:",(const char *)str,
		  (const char *)edit_time_edit->time().
				toString("hh:mm:ss.zzz").left(10)));
  }
}


void EditChain::timeToggledData(bool state)
{
  QString str;

  edit_time_edit->setEnabled(state);
  edit_grace_group->setEnabled(state);
  if(state) {
    graceClickedData(edit_grace_group->selectedId());
    str=QString(tr("Transition If Previous Cart Ends Before"));
    edit_time_label->
      setText(QString().sprintf("%s %s:",(const char *)str,
		  (const char *)edit_time_edit->time().
				toString("hh:mm:ss.zzz").left(10)));
  }
  else {
    edit_grace_box->setDisabled(true);
    edit_time_label->setText(tr("Transition Type:"));
  }
}


void EditChain::graceClickedData(int id)
{
  switch(id) {
  case 0:
    edit_grace_box->setDisabled(true);
    break;

  case 1:
    edit_grace_box->setDisabled(true);
    break;

  case 2:
    edit_grace_box->setEnabled(true);
    break;
  }
}


void EditChain::selectTimeData(int id)
{
  if(id==RDLogLine::Relative) {
    edit_time_edit->setDisabled(true);
    edit_time_label->setDisabled(true);
    edit_grace_box->setDisabled(true);
  }
  else {
    edit_time_edit->setEnabled(true);
    edit_time_label->setEnabled(true);
  }
}


void EditChain::selectLogData()
{
  QString logname;

  RDListLogs *d=
    new RDListLogs(&logname,RDLogFilter::UserFilter,this);
  if(d->exec()!=0) {
    delete d;
    return;
  }
  delete d;
  edit_label_edit->setText(logname);
  labelChangedData(logname);
}


void EditChain::labelChangedData(const QString &logname)
{
  QString sql=
    QString().sprintf("select DESCRIPTION from LOGS where NAME=\"%s\"",
		      (const char *)logname);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    edit_comment_edit->clear();
    return;
  }
  edit_comment_edit->setText(q->value(0).toString());
  delete q;
}


void EditChain::okData()
{
  if(edit_timetype_box->isChecked()) {
    edit_logline->setTimeType(RDLogLine::Hard);
    edit_logline->setStartTime(RDLogLine::Logged,edit_time_edit->time());
    switch(edit_grace_group->selectedId()) {
    case 0:
      edit_logline->setGraceTime(0);
      break;

    case 1:
      edit_logline->setGraceTime(-1);
      break;

    case 2:
      edit_logline->setGraceTime(QTime().msecsTo(edit_grace_box->time()));
      break;
    }
  }
  else {
    edit_logline->setTimeType(RDLogLine::Relative);
    edit_logline->setStartTime(RDLogLine::Logged,QTime());
    edit_logline->setGraceTime(0);
  }
  edit_logline->
    setTransType((RDLogLine::TransType)edit_transtype_box->currentItem());
  edit_logline->setMarkerLabel(edit_label_edit->text());
  edit_logline->setMarkerComment(edit_comment_edit->text());
  done(edit_logline->id());
}


void EditChain::cancelData()
{
  done(-1);
}


void EditChain::closeEvent(QCloseEvent *e)
{
  cancelData();
}
