// edit_event.cpp
//
// Event Editor for RDAirPlay
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpainter.h>
#include <qmessagebox.h>

#include "edit_event.h"

EditEvent::EditEvent(RDLogPlay *log,QWidget *parent)
  : RDDialog(parent)
{
  edit_log=log;
  edit_height=385;
  setWindowTitle("RDAirPlay - "+tr("Edit Event"));

  //
  // Time Type
  //
  edit_timetype_box=new QCheckBox(this);
  edit_timetype_label=new QLabel(edit_timetype_box,tr("Start at:"),this);
  edit_timetype_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Start Time
  //
  edit_time_edit=new RDTimeEdit(this);
  edit_time_edit->setDisplay(RDTimeEdit::Hours|RDTimeEdit::Minutes|
			     RDTimeEdit::Seconds|RDTimeEdit::Tenths);
  connect(edit_time_edit,SIGNAL(valueChanged(const QTime &)),
	  this,SLOT(timeChangedData(const QTime &)));

  //
  // Grace Time
  //
  edit_grace_bgroup=new QButtonGroup(this);
  connect(edit_grace_bgroup,SIGNAL(buttonClicked(int)),
	  this,SLOT(graceClickedData(int)));
  edit_grace_bgroup->setExclusive(true);
  edit_grace_group=
    new QGroupBox(tr("Action If Previous Event Still Playing"),this);
  edit_grace_group->setFont(labelFont());
  QRadioButton *radio_button=
    new QRadioButton(tr("Start Immediately"),edit_grace_group);
  edit_grace_bgroup->addButton(radio_button,0);
  radio_button->setFont(subLabelFont());
  radio_button=new QRadioButton(tr("Make Next"),edit_grace_group);
  edit_grace_bgroup->addButton(radio_button,1);
  radio_button->setFont(subLabelFont());
  radio_button=new QRadioButton(tr("Wait up to"),edit_grace_group);
  edit_grace_bgroup->addButton(radio_button,2);
  radio_button->setFont(subLabelFont());
  edit_grace_edit=new RDTimeEdit(this);
  edit_grace_edit->setDisplay(RDTimeEdit::Minutes|RDTimeEdit::Seconds|
			      RDTimeEdit::Tenths);
  connect(edit_timetype_box,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));

  //
  // Transition Type
  //
  edit_transtype_box=new QComboBox(this);
  edit_timetype_label->setFont(labelFont());
  edit_transtype_box->insertItem(tr("Play"));
  edit_transtype_box->insertItem(tr("Segue"));
  edit_transtype_box->insertItem(tr("Stop"));  
  edit_transtype_label=
    new QLabel(edit_transtype_box,tr("Start Transition Type:"),this);
  edit_transtype_label->setFont(labelFont());
  edit_transtype_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  // Overlap Box
  edit_overlap_box=new QCheckBox(this);
  edit_overlap_label=
    new QLabel(edit_overlap_box,tr("No Fade at Segue Out"),this);
  edit_overlap_label->setFont(labelFont());
  edit_overlap_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  

  //
  // Horizontal Rule
  //
  edit_horizrule_label=new QLabel(this);
  QPixmap *pix=new QPixmap(sizeHint().width(),3);
  QPainter *p=new QPainter(pix);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);
  p->fillRect(0,0,sizeHint().width(),3,backgroundColor());
  p->drawLine(10,1,sizeHint().width()-10,1);
  p->end();
  edit_horizrule_label->setPixmap(*pix);
  delete p;
  delete pix;

  //
  // Cue Editor
  //
  edit_cue_edit=new RDCueEdit(this);

  //
  // Cart Notes
  //
  edit_cart_notes_label=new QLabel(tr("Cart Notes"),this);
  edit_cart_notes_label->setFont(labelFont());
  edit_cart_notes_text=new QTextEdit(this);
  edit_cart_notes_text->setReadOnly(true);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


EditEvent::~EditEvent()
{
}


QSize EditEvent::sizeHint() const
{
  return QSize(625,edit_height);
} 


QSizePolicy EditEvent::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditEvent::exec(int line)
{
  edit_line=line;
  edit_time_changed=false;
  if((edit_logline=edit_log->logLine(line))==NULL) {
    return -1;
  }
  switch(edit_logline->timeType()) {
      case ::RDLogLine::Hard:
	edit_timetype_box->setChecked(true);
	timeToggledData(true);
	break;

      default:
	edit_timetype_box->setChecked(false);
	timeToggledData(false);
	break;
  }
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
	graceClickedData(2);
	edit_grace_edit->setTime(QTime().addMSecs(edit_logline->graceTime()));
	break;
  }
  edit_transtype_box->setCurrentItem((int)edit_logline->transType());
  if(edit_logline->segueStartPoint(RDLogLine::LogPointer)<0
     && edit_logline->segueEndPoint(RDLogLine::LogPointer)<0
     && edit_logline->endPoint(RDLogLine::LogPointer)<0
     && edit_logline->fadedownPoint(RDLogLine::LogPointer)<0) {
    edit_overlap_box->setEnabled(true);
    edit_overlap_label->setEnabled(true);
    if(edit_logline->segueGain()==0) {
      edit_overlap_box->setChecked(true);
    }
    else {
      edit_overlap_box->setChecked(false);
    }
  }
  else {
    edit_overlap_box->setEnabled(false);
    edit_overlap_label->setEnabled(false);
    edit_overlap_box->setChecked(false);
  }  
  if(!edit_logline->startTime(RDLogLine::Logged).isNull()) {
    edit_time_edit->setTime(edit_logline->startTime(RDLogLine::Logged));
  }
  else {
    edit_time_edit->setTime(QTime());
  }
  setWindowTitle("RDAirPlay - "+
		 QString().sprintf("%d - ",edit_logline->cartNumber())+
		 edit_logline->title());
  edit_cart_notes_text->setText(edit_logline->cartNotes());
  switch(edit_logline->type()) {
  case RDLogLine::Cart:
    if((edit_logline->cutNumber()<1)||
       (edit_logline->forcedLength()<=0)) {
      edit_cue_edit->hide();
      if(edit_logline->cartNotes().isEmpty()) {
	edit_height=195;
	edit_cart_notes_label->hide();
	edit_cart_notes_text->hide();
      }
      else {
	edit_height=375;
	edit_cart_notes_label->show();
	edit_cart_notes_text->show();
      }
    }
    else {
      edit_cue_edit->initialize(edit_logline);
      edit_cue_edit->show();
      if(edit_logline->cartNotes().isEmpty()) {
	edit_height=360;
	edit_cart_notes_label->hide();
	edit_cart_notes_text->hide();
      }
      else {
	edit_height=540;
	edit_cart_notes_label->show();
	edit_cart_notes_text->show();
      }
    }
    break;

  case RDLogLine::Macro:
    edit_cue_edit->hide();
    if(edit_logline->cartNotes().isEmpty()) {
      edit_height=195;
      edit_cart_notes_label->hide();
      edit_cart_notes_text->hide();
    }
    else {
      edit_height=325;
      edit_cart_notes_label->show();
      edit_cart_notes_text->show();
    }
    break;

  case RDLogLine::Marker:
    setCaption(tr("Edit Marker"));
    edit_cue_edit->hide();
    edit_cart_notes_label->hide();
    edit_cart_notes_text->hide();
    edit_height=195;
    break;

  case RDLogLine::Track:
    setCaption(tr("Edit Track"));
    edit_cue_edit->hide();
    edit_cart_notes_label->hide();
    edit_cart_notes_text->hide();
    edit_height=195;
    break;

  case RDLogLine::Chain:
    setCaption(tr("Edit Log Track"));
    edit_cue_edit->hide();
    edit_cart_notes_label->hide();
    edit_cart_notes_text->hide();
    edit_height=195;
    break;

  default:
    edit_cue_edit->hide();
    edit_height=195;
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  resize(sizeHint());

  return QDialog::exec();
}


void EditEvent::timeChangedData(const QTime &time)
{
  QString str;

  if(edit_timetype_box->isChecked()) {
    edit_transtype_label->setText(tr("Transition If Previous Cart Ends Before")+
			     " "+edit_time_edit->time().
			     toString("hh:mm:ss.zzz").left(10)+":");
  }
}


void EditEvent::timeToggledData(bool state)
{
  QString str;

  edit_time_edit->setEnabled(state);
  edit_grace_group->setEnabled(state);
  if(state) {
    graceClickedData(edit_grace_bgroup->checkedId());
    edit_transtype_label->setText(tr("Transition If Previous Cart Ends Before")+
			     " "+edit_time_edit->time().
			     toString("hh:mm:ss.zzz").left(10)+":");
  }
  else {
    edit_grace_edit->setDisabled(true);
    edit_transtype_label->setText(tr("Start Transition Type:"));
  }
}


void EditEvent::graceClickedData(int id)
{
  switch(id) {
      case 0:
	edit_grace_edit->setDisabled(true);
	break;

      case 1:
	edit_grace_edit->setDisabled(true);
	break;

      case 2:
	edit_grace_edit->setEnabled(true);
	break;
  }
}


void EditEvent::okData()
{
  if(edit_timetype_box->isChecked()&&
     edit_log->exists(edit_time_edit->time(),edit_line)) {
    QMessageBox::warning(this,tr("Duplicate Start Time"),
	       	 tr("An event is already scheduled with this start time!"));
    return;
  }
  edit_cue_edit->stop();
  if((edit_logline->status()==RDLogLine::Scheduled)||
     (edit_logline->status()==RDLogLine::Paused)) {
    if(edit_timetype_box->isChecked()) {
      edit_logline->setTimeType(RDLogLine::Hard);
      switch(edit_grace_bgroup->checkedId()) {
	  case 0:
	    edit_logline->setGraceTime(0);
	    break;

	  case 1:
	    edit_logline->setGraceTime(-1);
	    break;

	  case 2:
	    edit_logline->
	      setGraceTime(QTime().msecsTo(edit_grace_edit->time()));
	    break;
      }
    }
    else {
      edit_logline->setTimeType(RDLogLine::Relative);
      edit_logline->setStartTime(RDLogLine::Logged,edit_logline->
				 startTime(RDLogLine::Imported));
    }
    edit_logline->
      setTransType((RDLogLine::TransType)edit_transtype_box->currentItem());
    if(edit_logline->segueStartPoint(RDLogLine::LogPointer)<0
       && edit_logline->segueEndPoint(RDLogLine::LogPointer)<0
       && edit_logline->endPoint(RDLogLine::LogPointer)<0
       && edit_logline->fadedownPoint(RDLogLine::LogPointer)<0) {
      if(edit_overlap_box->isChecked()) {
        edit_logline->setSegueGain(0);
      }
      else {
        edit_logline->setSegueGain(RD_FADE_DEPTH);
      }
    }
    if(edit_time_changed||
       (edit_logline->timeType()!=RDLogLine::Relative)) {
      edit_logline->
	setStartTime(RDLogLine::Logged,edit_time_edit->time());
    }
    if(edit_cue_edit->playPosition(RDMarkerBar::Start)!=
       edit_logline->playPosition()) {
      edit_logline->
	setPlayPosition(edit_cue_edit->playPosition(RDMarkerBar::Start));
      edit_logline->setPlayPositionChanged(true);
    }
    if(edit_cue_edit->playPosition(RDMarkerBar::End)!=
       (unsigned)edit_logline->endPoint()) {
      edit_logline->setEndPoint(edit_cue_edit->playPosition(RDMarkerBar::End),
				RDLogLine::LogPointer);
      edit_logline->setPlayPositionChanged(true);
    }
    edit_log->lineModified(edit_line);
  }

  done(0);
}


void EditEvent::cancelData()
{
  edit_cue_edit->stop();

  done(1);
}


void EditEvent::resizeEvent(QResizeEvent *e)
{
  edit_timetype_box->setGeometry(10,22,15,15);
  edit_timetype_label->setGeometry(30,21,85,17);
  edit_time_edit->setGeometry(85,19,85,20);

  edit_grace_group->setGeometry(175,11,435,50);
  edit_grace_bgroup->button(0)->setGeometry(10,21,145,20);
  edit_grace_bgroup->button(1)->setGeometry(155,21,105,20);
  edit_grace_bgroup->button(2)->setGeometry(265,21,95,20);
  edit_grace_edit->setGeometry(538,31,65,20);

  edit_overlap_box->setGeometry(30,72,15,15);
  edit_overlap_label->setGeometry(50,68,400,26);

  edit_transtype_label->setGeometry(10,90,470,26);
  edit_transtype_box->setGeometry(485,90,110,26);

  edit_horizrule_label->setGeometry(0,122,size().width(),3);

  edit_cue_edit->setGeometry(20,132,edit_cue_edit->sizeHint().width(),
			     edit_cue_edit->sizeHint().height());
  if(edit_cue_edit->isShown()) {
    edit_cart_notes_label->
      setGeometry(15,127+edit_cue_edit->sizeHint().height(),
		  size().width()-20,20);
    edit_cart_notes_text->setGeometry(10,147+edit_cue_edit->sizeHint().height(),
				      size().width()-20,100);
  }
  else {
    edit_cart_notes_label->setGeometry(15,127,size().width()-20,20);
    edit_cart_notes_text->setGeometry(10,147,size().width()-20,100);
  }
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}
