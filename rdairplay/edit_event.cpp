// edit_event.cpp
//
// Event Editor for RDAirPlay
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_event.cpp,v 1.48.8.3.2.3 2014/05/20 18:46:17 cvs Exp $
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
#include <qlabel.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rdconf.h>

#include <edit_event.h>
#include <globals.h>


EditEvent::EditEvent(LogPlay *log,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_log=log;
  edit_height=385;
  setCaption(tr("Edit Event"));

  //
  // Create Fonts
  //
  QFont radio_font=QFont("Helvetica",10,QFont::Normal);
  radio_font.setPixelSize(10);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont notes_font=QFont("Helvetica",16,QFont::Normal);
  notes_font.setPixelSize(16);
  QFont counter_font=QFont("Helvetica",20,QFont::Bold);
  counter_font.setPixelSize(20);

  //
  // Time Type
  //
  edit_timetype_box=new QCheckBox(this);
  edit_timetype_label=new QLabel(edit_timetype_box,tr("Start at:"),this);
  edit_timetype_label->setAlignment(AlignLeft|AlignVCenter);

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
  edit_grace_group
    =new QButtonGroup(1,Qt::Vertical,
		      tr("Action If Previous Event Still Playing"),this);
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
  edit_grace_edit=new RDTimeEdit(this);
  edit_grace_edit->setDisplay(RDTimeEdit::Minutes|RDTimeEdit::Seconds|
			      RDTimeEdit::Tenths);
  connect(edit_timetype_box,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(edit_grace_group,SIGNAL(clicked(int)),
	  this,SLOT(graceClickedData(int)));

  //
  // Transition Type
  //
  edit_transtype_box=new QComboBox(this);
  edit_timetype_label->setFont(label_font);
  edit_transtype_box->insertItem(tr("Play"));
  edit_transtype_box->insertItem(tr("Segue"));
  edit_transtype_box->insertItem(tr("Stop"));  
  edit_time_label=
    new QLabel(edit_transtype_box,tr("Start Transition Type:"),this);
  edit_time_label->setFont(label_font);
  edit_time_label->setAlignment(AlignRight|AlignVCenter);

  // Overlap Box
  edit_overlap_box=new QCheckBox(this);
  edit_overlap_label=
    new QLabel(edit_overlap_box,tr("No Fade at Segue Out"),this);
  edit_overlap_label->setFont(button_font);
  edit_overlap_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  

  //
  // Horizontal Rule
  //
  edit_horizrule_label=new QLabel(this);
  QPixmap *pix=new QPixmap(sizeHint().width(),3);
  QPainter *p=new QPainter(pix);
  p->setPen(QColor(black));
  p->setBrush(QColor(black));
  p->fillRect(0,0,sizeHint().width(),3,backgroundColor());
  p->moveTo(10,1);
  p->lineTo(sizeHint().width()-10,1);
  p->end();
  edit_horizrule_label->setPixmap(*pix);
  delete p;
  delete pix;

  //
  // Cue Editor
  //
  edit_cue_edit=
    new RDCueEdit(rdcae,rdairplay_conf->card(RDAirPlayConf::CueChannel),
		  rdairplay_conf->port(RDAirPlayConf::CueChannel),this);

  //
  // Cart Notes
  //
  edit_cart_notes_label=new QLabel(tr("Cart Notes"),this);
  edit_cart_notes_label->setFont(label_font);
  edit_cart_notes_text=new QTextEdit(this);
  edit_cart_notes_text->setFont(notes_font);
  edit_cart_notes_text->setReadOnly(true);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(button_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(button_font);
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
	edit_grace_group->setButton(1);
	graceClickedData(1);
	break;

      case 0:
	edit_grace_group->setButton(0);
	graceClickedData(0);
	break;

      default:
	edit_grace_group->setButton(2);
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
  setCaption(QString().sprintf("%d - %s",
			      edit_logline->cartNumber(),
			      (const char *)edit_logline->title()));
  edit_cart_notes_text->setText(edit_logline->cartNotes());
  switch(edit_logline->type()) {
  case RDLogLine::Cart:
    if((edit_logline->cutNumber()<1)||
       (edit_logline->forcedLength()<=0)) {
      edit_cue_edit->hide();
      if(edit_logline->cartNotes().isEmpty()) {
	edit_height=170;
	edit_cart_notes_label->hide();
	edit_cart_notes_text->hide();
      }
      else {
	edit_height=350;
	edit_cart_notes_label->show();
	edit_cart_notes_text->show();
      }
    }
    else {
      edit_cue_edit->initialize(edit_logline);
      edit_cue_edit->show();
      if(edit_logline->cartNotes().isEmpty()) {
	edit_height=335;
	edit_cart_notes_label->hide();
	edit_cart_notes_text->hide();
      }
      else {
	edit_height=515;
	edit_cart_notes_label->show();
	edit_cart_notes_text->show();
      }
    }
    break;

  case RDLogLine::Macro:
    edit_cue_edit->hide();
    if(edit_logline->cartNotes().isEmpty()) {
      edit_height=170;
      edit_cart_notes_label->hide();
      edit_cart_notes_text->hide();
    }
    else {
      edit_height=300;
      edit_cart_notes_label->show();
      edit_cart_notes_text->show();
    }
    break;

  case RDLogLine::Marker:
    setCaption(tr("Edit Marker"));
    edit_cue_edit->hide();
    edit_cart_notes_label->hide();
    edit_cart_notes_text->hide();
    edit_height=170;
    break;

  case RDLogLine::Track:
    setCaption(tr("Edit Track"));
    edit_cue_edit->hide();
    edit_cart_notes_label->hide();
    edit_cart_notes_text->hide();
    edit_height=170;
    break;

  case RDLogLine::Chain:
    setCaption(tr("Edit Log Track"));
    edit_cue_edit->hide();
    edit_cart_notes_label->hide();
    edit_cart_notes_text->hide();
    edit_height=170;
    break;

  default:
    edit_cue_edit->hide();
    edit_height=170;
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
  edit_ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
			      80,50);
  edit_cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
				  80,50);
  resize(sizeHint());

  return QDialog::exec();
}


void EditEvent::timeChangedData(const QTime &time)
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


void EditEvent::timeToggledData(bool state)
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
    edit_grace_edit->setDisabled(true);
    edit_time_label->setText(tr("Start Transition Type:"));
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
      switch(edit_grace_group->selectedId()) {
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
  edit_time_edit->setGeometry(85,19,85,20);
  edit_grace_group->setGeometry(175,11,435,50);
  edit_grace_edit->setGeometry(538,31,65,20);
  edit_transtype_box->setGeometry(485,68,110,26);
  edit_timetype_box->setGeometry(10,22,15,15);
  edit_time_label->setGeometry(190,68,290,26);
  edit_timetype_label->setGeometry(30,21,85,17);
  edit_overlap_box->setGeometry(30,72,15,15);
  edit_overlap_label->setGeometry(50,68,130,26);
  edit_horizrule_label->setGeometry(0,100,size().width(),3);
  edit_cue_edit->setGeometry(20,110,edit_cue_edit->sizeHint().width(),
			     edit_cue_edit->sizeHint().height());
  if(edit_cue_edit->isShown()) {
    edit_cart_notes_label->
      setGeometry(15,105+edit_cue_edit->sizeHint().height(),
		  size().width()-20,20);
    edit_cart_notes_text->setGeometry(10,125+edit_cue_edit->sizeHint().height(),
				      size().width()-20,100);
  }
  else {
    edit_cart_notes_label->setGeometry(15,105,size().width()-20,20);
    edit_cart_notes_text->setGeometry(10,125,size().width()-20,100);
  }
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}
