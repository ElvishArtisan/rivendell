// login.cpp
//
// Login widget for RDAdmin.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: login.cpp,v 1.14 2010/07/29 19:32:35 cvs Exp $
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rdtextvalidator.h>

#include <login.h>


Login::Login(QString *username,QString *password,QWidget *parent,
	     const char *name)  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Login"));
  login_name=username;
  login_password=password;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // OK Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(10,60,100,55);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  ok_button->setDefault(true);
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // CANCEL Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(120,60,100,55);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Login Name
  //
  login_name_edit=new QLineEdit(this,"login_name_edit");
  login_name_edit->setGeometry(100,10,100,19);
  login_name_edit->setMaxLength(16);
  login_name_edit->setFocus();
  login_name_edit->setValidator(validator);
  QLabel *login_name_label=new QLabel(login_name_edit,tr("User &Name:"),this,
				       "login_name_label");
  login_name_label->setGeometry(10,10,85,19);
  login_name_label->setFont(font);
  login_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Login Password
  //
  login_password_edit=new QLineEdit(this,"login_password_edit");
  login_password_edit->setGeometry(100,31,100,19);
  login_password_edit->setMaxLength(16);
  login_password_edit->setEchoMode(QLineEdit::Password);
  login_password_edit->setValidator(validator);
  QLabel *login_password_label=new QLabel(login_password_edit,tr("&Password:"),
					  this,"login_password_label");
  login_password_label->setGeometry(10,31,85,19);
  login_password_label->setFont(font);
  login_password_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

}


Login::~Login()
{
  delete login_name_edit;
  delete login_password_edit;
}


QSize Login::sizeHint() const
{
  return QSize(230,125);
} 


QSizePolicy Login::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void Login::okData()
{
  *login_name=login_name_edit->text();
  *login_password=login_password_edit->text();
  done(0);
}


void Login::cancelData()
{
  done(1);
}


void Login::paintEvent(QPaintEvent *paintevent)
{
  QPainter *p=new QPainter(this);
    
  p->end();
}
