// edit_eventline.cpp
//
// Edit Rivendell Log Eventline
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_eventline.cpp,v 1.16 2010/07/29 19:32:37 cvs Exp $
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
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include <rddb.h>
#include <rd.h>
#include <rdevent.h>
#include <rdcreate_log.h>
#include <rdtextvalidator.h>

#include <list_events.h>
#include <edit_eventline.h>


EditEventLine::EditEventLine(RDEventLine *eventline,RDClock *clock,int line,
			     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  setCaption(tr("Edit Event Assignment"));
  edit_eventline=eventline;
  edit_clock=clock;
  edit_line=line;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Text Validator
  //
  //
  // Create Validators
  //
  RDTextValidator *validator=new RDTextValidator();
  validator->addBannedChar('(');
  validator->addBannedChar(')');
  validator->addBannedChar('!');
  validator->addBannedChar('@');
  validator->addBannedChar('#');
  validator->addBannedChar('$');
  validator->addBannedChar('%');
  validator->addBannedChar('^');
  validator->addBannedChar('&');
  validator->addBannedChar('*');
  validator->addBannedChar('{');
  validator->addBannedChar('}');
  validator->addBannedChar('[');
  validator->addBannedChar(']');
  validator->addBannedChar(':');
  validator->addBannedChar(';');
  validator->addBannedChar(34);
  validator->addBannedChar('<');
  validator->addBannedChar('>');
  validator->addBannedChar('.');
  validator->addBannedChar(',');
  validator->addBannedChar('\\');
  validator->addBannedChar('-');
  validator->addBannedChar('_');

  //
  // Event Name
  //
  edit_eventname_edit=new QLineEdit(this,"edit_eventname_edit");
  edit_eventname_edit->setGeometry(65,12,sizeHint().width()-140,18);
  edit_eventname_edit->setMaxLength(64);
  edit_eventname_edit->setValidator(validator);
  QLabel *label=new QLabel(edit_eventname_edit,tr("Event:"),
			   this,"edit_eventname_label");
  label->setGeometry(10,12,50,18);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Event Select Button
  //
  QPushButton *button=new QPushButton(this,"select_button");
  button->setGeometry(sizeHint().width()-60,7,50,30);
  button->setFont(font);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Start Time
  //
  edit_starttime_edit=new RDTimeEdit(this,"edit_starttime_edit");
  edit_starttime_edit->setGeometry(150,40,70,20);
  edit_starttime_edit->
    setDisplay(RDTimeEdit::Minutes|RDTimeEdit::Seconds|RDTimeEdit::Tenths);
  edit_starttime_edit->setFont(font);
  label=new QLabel(edit_starttime_edit,tr("Start Time:"),
			   this,"edit_starttime_label");
  label->setGeometry(65,42,80,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // End Time
  //
  edit_endtime_edit=new RDTimeEdit(this,"edit_length_edit");
  edit_endtime_edit->setGeometry(325,40,70,20);
  edit_endtime_edit->
    setDisplay(RDTimeEdit::Minutes|RDTimeEdit::Seconds|RDTimeEdit::Tenths);
  label=new QLabel(edit_endtime_edit,tr("End Time:"),
		   this,"edit_endtime_label");
  label->setGeometry(250,42,70,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  //  OK Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  if(edit_eventline!=NULL) {
    edit_eventname_edit->setText(edit_eventline->name());
    edit_starttime_edit->setTime(edit_eventline->startTime());
    edit_endtime_edit->
      setTime(edit_eventline->startTime().addMSecs(edit_eventline->length()));
  }
}


QSize EditEventLine::sizeHint() const
{
  return QSize(550,130);
} 


QSizePolicy EditEventLine::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditEventLine::selectData()
{
  QString eventname;
  ListEvents *list_events=new ListEvents(&eventname,this,"add_dialog");
  if(list_events->exec()<0) {
    delete list_events;
    return;
  }
  delete list_events;
  edit_eventname_edit->setText(eventname);
}


void EditEventLine::okData()
{
  if(edit_starttime_edit->time()>edit_endtime_edit->time()) {
    QMessageBox::information(this,tr("Invalid Event"),
	    tr("The event end time cannot be earlier than the start time."));
    return;
  }
  QString sql=QString().sprintf("select NAME from EVENTS where NAME=\"%s\"",
				(const char *)edit_eventname_edit->text());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    QMessageBox::information(this,tr("No Such Event"),
			     tr("There is no such event."));
    delete q;
    return;
  }
  edit_eventname_edit->setText(q->value(0).toString());
  delete q;
  if(!edit_clock->validate(edit_starttime_edit->time(),
			   edit_starttime_edit->time().
			   msecsTo(edit_endtime_edit->time()),
			   edit_line)) {
    QMessageBox::information(this,tr("Invalid Event"),
		 tr("This event overlaps with an\nalready existing event."));
    return;
  }
  edit_eventline->setName(edit_eventname_edit->text());
  edit_eventline->setStartTime(edit_starttime_edit->time());
  edit_eventline->
    setLength(edit_starttime_edit->time().msecsTo(edit_endtime_edit->time()));
  done(0);
}


void EditEventLine::cancelData()
{
  done(-1);
}
