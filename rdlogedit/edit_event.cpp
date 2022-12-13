// edit_event.cpp
//
// Base class for rdlogedit(1) event editor dialogs
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

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlist_logs.h>

#include "edit_event.h"
#include "globals.h"

EditEvent::EditEvent(QWidget *parent)
  : RDDialog(parent)
{
  //  edit_logline=line;

  //
  // Time Type
  //
  edit_timetype_box=new QCheckBox(this);
  edit_timetype_label=new QLabel(tr("Start at:"),this);
  edit_timetype_label->setFont(labelFont());
  edit_timetype_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Start Time
  //
  edit_time_edit=new RDTimeEdit(this);
  edit_time_edit->setShowTenths(true);
  connect(edit_time_edit,SIGNAL(timeChanged(const QTime &)),
	  this,SLOT(timeChangedData(const QTime &)));

  //
  // Grace Time
  //
  edit_grace_group=
    new QGroupBox(tr("Action If Previous Event Still Playing"),this);
  edit_grace_group->setFont(labelFont());
  edit_grace_bgroup=new QButtonGroup(this);
  edit_grace_bgroup->setExclusive(true);
  QRadioButton *radio_button=
    new QRadioButton(tr("Start Immediately"),edit_grace_group);
  radio_button->setFont(subLabelFont());

  edit_grace_bgroup->addButton(radio_button,0);
  radio_button=new QRadioButton(tr("Make Next"),edit_grace_group);
  radio_button->setFont(subLabelFont());
  edit_grace_bgroup->addButton(radio_button,1);
  radio_button=new QRadioButton(tr("Wait up to"),edit_grace_group);
  radio_button->setFont(subLabelFont());
  edit_grace_bgroup->addButton(radio_button,2);
  edit_grace_timeedit=new QTimeEdit(this);
  edit_grace_timeedit->setDisplayFormat("mm:ss");
  connect(edit_timetype_box,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(edit_grace_bgroup,SIGNAL(buttonClicked(int)),
	  this,SLOT(graceClickedData(int)));

  //
  // Transition Type
  //
  edit_transtype_box=new QComboBox(this);
  edit_transtype_box->insertItem(0,tr("Play"));
  edit_transtype_box->insertItem(1,tr("Segue"));
  edit_transtype_box->insertItem(2,tr("Stop"));  
  edit_transtype_label=new QLabel(tr("Transition Type:"),this);
  edit_transtype_label->setFont(labelFont());
  edit_transtype_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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


int EditEvent::exec()
{
  edit_time_edit->setTime(edit_logline->startTime(RDLogLine::Logged));
  edit_timetype_box->setChecked(edit_logline->timeType()==RDLogLine::Hard);
  timeToggledData(edit_logline->timeType()==RDLogLine::Hard);
  timeChangedData(edit_time_edit->time());
  switch(edit_logline->graceTime()) {
  case -1:
    edit_grace_bgroup->button(1)->setChecked(true);
    graceClickedData(1);
    break;

  case 0:
    edit_grace_bgroup->button(0)->setChecked(true);
    graceClickedData(0);
    break;

  default:
    edit_grace_bgroup->button(2)->setChecked(true);
    edit_grace_timeedit->setTime(QTime(0,0,0).addMSecs(edit_logline->graceTime()));
    graceClickedData(2);
    break;
  }
  edit_transtype_box->setCurrentIndex(edit_logline->transType());

  return QDialog::exec();
}


void EditEvent::setOkEnabled(bool state)
{
  edit_ok_button->setEnabled(state);
}


void EditEvent::timeChangedData(const QTime &time)
{
  if(edit_timetype_box->isChecked()) {
    edit_transtype_label->
      setText(tr("Transition If Previous Cart Ends Before")+" "+
	      edit_time_edit->time().toString("hh:mm:ss.zzz").left(10)+":");
  }
}


void EditEvent::timeToggledData(bool state)
{
  edit_time_edit->setEnabled(state);
  edit_grace_group->setEnabled(state);
  edit_grace_bgroup->button(0)->setEnabled(state);
  edit_grace_bgroup->button(1)->setEnabled(state);
  edit_grace_bgroup->button(2)->setEnabled(state);
  if(state) {
    graceClickedData(edit_grace_bgroup->checkedId());
    edit_transtype_label->
      setText(tr("Transition If Previous Cart Ends Before")+" "+
	      edit_time_edit->time().toString("hh:mm:ss.zzz").left(10)+":");
  }
  else {
    edit_grace_timeedit->setDisabled(true);
    edit_transtype_label->setText(tr("Transition Type")+":");
  }
}


void EditEvent::graceClickedData(int id)
{
  switch(id) {
  case 0:
    edit_grace_timeedit->setDisabled(true);
    break;

  case 1:
    edit_grace_timeedit->setDisabled(true);
    break;

  case 2:
    edit_grace_timeedit->setEnabled(true);
    break;
  }
}


void EditEvent::selectTimeData(int id)
{
  if(id==RDLogLine::Relative) {
    edit_time_edit->setDisabled(true);
    edit_transtype_label->setDisabled(true);
    edit_grace_timeedit->setDisabled(true);
  }
  else {
    edit_time_edit->setEnabled(true);
    edit_transtype_label->setEnabled(true);
  }
}


void EditEvent::okData()
{
  if(saveData()) {
    if(edit_timetype_box->isChecked()) {
      edit_logline->setTimeType(RDLogLine::Hard);
      edit_logline->setStartTime(RDLogLine::Logged,edit_time_edit->time());
      switch(edit_grace_bgroup->checkedId()) {
      case 0:
	edit_logline->setGraceTime(0);
	break;

      case 1:
	edit_logline->setGraceTime(-1);
	break;

      case 2:
	edit_logline->setGraceTime(QTime(0,0,0).msecsTo(edit_grace_timeedit->time()));
	break;
      }
    }
    else {
      edit_logline->setTimeType(RDLogLine::Relative);
      edit_logline->setStartTime(RDLogLine::Logged,QTime());
      edit_logline->setGraceTime(0);
    }
    edit_logline->
      setTransType((RDLogLine::TransType)edit_transtype_box->currentIndex());
    done(edit_logline->id());
  }
}


void EditEvent::cancelData()
{
  done(-1);
}


RDLogLine *EditEvent::logLine()
{
  return edit_logline;
}


void EditEvent::setLogLine(RDLogLine *ll)
{
  edit_logline=ll;
}


void EditEvent::resizeEvent(QResizeEvent *e)
{
  edit_timetype_box->setGeometry(10,22,15,15);
  edit_timetype_label->setGeometry(30,19,85,20);
  edit_time_edit->setGeometry(85,19,110,20);
  edit_grace_group->setGeometry(205,11,410,42);
  edit_grace_bgroup->button(0)->setGeometry(10,16,145,20);
  edit_grace_bgroup->button(1)->setGeometry(155,16,105,20);
  edit_grace_bgroup->button(2)->setGeometry(265,16,95,20);
  edit_grace_timeedit->setGeometry(548,26,60,20);

  edit_transtype_label->setGeometry(10,60,370,26);
  edit_transtype_box->setGeometry(385,60,110,26);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}
