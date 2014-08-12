// add_clock.cpp
//
// Add a Rivendell Service
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_clock.cpp,v 1.13.8.1 2012/04/23 17:22:47 cvs Exp $
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
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsqldatabase.h>

#include <add_clock.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>


AddClock::AddClock(QString *logname,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  clock_name=logname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Log Clock"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

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
  // Clock Name
  //
  clock_name_edit=new QLineEdit(this,"clock_name_edit");
  clock_name_edit->setGeometry(145,11,sizeHint().width()-155,19);
  clock_name_edit->setMaxLength(58);  // MySQL limitation!
  clock_name_edit->setValidator(validator);
  QLabel *clock_name_label=new QLabel(clock_name_edit,tr("&New Clock Name:"),
				      this,"clock_name_label");
  clock_name_label->setGeometry(10,11,130,19);
  clock_name_label->setFont(font);
  clock_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  clock_name_edit->setText(*clock_name);
  clock_name_edit->selectAll();
}


AddClock::~AddClock()
{
  delete clock_name_edit;
}


QSize AddClock::sizeHint() const
{
  return QSize(400,105);
} 


QSizePolicy AddClock::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddClock::okData()
{
  *clock_name=clock_name_edit->text();
  done(0);
}


void AddClock::cancelData()
{
  done(-1);
}


void AddClock::closeEvent(QCloseEvent *e)
{
  cancelData();
}
