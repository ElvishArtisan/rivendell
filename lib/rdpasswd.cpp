// rdpasswd.cpp
//
// Set Password widget for Rivendell.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdpasswd.h"
#include "rdtextvalidator.h"

RDPasswd::RDPasswd(QString *password,QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  passwd_password=password;
  setWindowTitle(tr("Change Password"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // OK Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(10,60,100,55);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  ok_button->setDefault(true);
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(120,60,100,55);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Password
  //
  passwd_password_1_edit=new QLineEdit(this);
  passwd_password_1_edit->setGeometry(90,11,100,19);
  passwd_password_1_edit->setMaxLength(16);
  passwd_password_1_edit->setEchoMode(QLineEdit::Password);
  passwd_password_1_edit->setFocus();
  passwd_password_1_edit->setValidator(validator);
  QLabel *passwd_password_1_label=
    new QLabel(passwd_password_1_edit,tr("&Password:"),this);
  passwd_password_1_label->setFont(labelFont());
  passwd_password_1_label->setGeometry(10,13,75,19);
  passwd_password_1_label->setAlignment(Qt::AlignRight|Qt::TextShowMnemonic);

  //
  // Confirm Password
  //
  passwd_password_2_edit=new QLineEdit(this);
  passwd_password_2_edit->setGeometry(90,32,100,19);
  passwd_password_2_edit->setMaxLength(16);
  passwd_password_2_edit->setEchoMode(QLineEdit::Password);
  passwd_password_2_edit->setValidator(validator);
  QLabel *passwd_password_2_label=
    new QLabel(passwd_password_2_edit,tr("C&onfirm:"),this);
  passwd_password_2_label->setFont(labelFont());
  passwd_password_2_label->setGeometry(10,34,75,19);
  passwd_password_2_label->setAlignment(Qt::AlignRight|Qt::TextShowMnemonic);
}


RDPasswd::~RDPasswd()
{
  delete passwd_password_1_edit;
  delete passwd_password_2_edit;
}


QSize RDPasswd::sizeHint() const
{
  return QSize(230,125);
} 


QSizePolicy RDPasswd::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDPasswd::okData()
{
  if(passwd_password_1_edit->text()==passwd_password_2_edit->text()) {
    *passwd_password=passwd_password_1_edit->text();
    done(0);
  }
  else {
    QMessageBox::warning(this,tr("Password Mismatch"),
			 tr("The passwords don't match,\nplease try again!"),
			 tr("OK"));
  }
}


void RDPasswd::cancelData()
{
  done(1);
}
