// add_event.cpp
//
// Add a Rivendell LogManager Event
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdtextvalidator.h>

#include "add_event.h"
#include "edit_event.h"

AddEvent::AddEvent(QString *logname,QWidget *parent)
  : RDDialog(parent)
{
  event_name=logname;

  setWindowTitle("RDLogManager - "+tr("Add Log Event"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

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
  validator->addBannedChar('/');
  validator->addBannedChar('+');
  validator->addBannedChar('=');
  validator->addBannedChar('~');
  validator->addBannedChar('?');
  validator->addBannedChar('|');

  //
  // Event Name
  //
  event_name_edit=new QLineEdit(this);
  event_name_edit->setGeometry(145,11,sizeHint().width()-155,19);
  event_name_edit->setMaxLength(58);  // MySQL limitation!
  event_name_edit->setValidator(validator);
  QLabel *event_name_label=new QLabel(tr("New Event Name:"),this);
  event_name_label->setGeometry(10,11,130,19);
  event_name_label->setFont(labelFont());
  event_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(event_name_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(nameChangedData(const QString &)));

  //
  //  Ok Button
  //
  event_ok_button=new QPushButton(this);
  event_ok_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  event_ok_button->setDefault(true);
  event_ok_button->setFont(buttonFont());
  event_ok_button->setText(tr("OK"));
  connect(event_ok_button,SIGNAL(clicked()),this,SLOT(okData()));
  event_ok_button->setDisabled(true);

  //
  //  Cancel Button
  //
  event_cancel_button=new QPushButton(this);
  event_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  event_cancel_button->setFont(buttonFont());
  event_cancel_button->setText(tr("Cancel"));
  connect(event_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  event_name_edit->setText(*event_name);
  event_name_edit->selectAll();
}


AddEvent::~AddEvent()
{
  delete event_name_edit;
}


QSize AddEvent::sizeHint() const
{
  return QSize(400,105);
} 


QSizePolicy AddEvent::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddEvent::nameChangedData(const QString &str)
{
  event_ok_button->setDisabled(str.isEmpty());
}


void AddEvent::okData()
{
  *event_name=event_name_edit->text();
  done(0);
}


void AddEvent::cancelData()
{
  done(-1);
}


void AddEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}
