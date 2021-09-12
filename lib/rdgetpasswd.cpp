// rdgetpasswd.cpp
//
// Prompt for a password.
//
//   (C) Copyright 2006-2021 Fred Gleason <fredg@salemradiolabs.com>
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

#include <QPushButton>

#include "rdgetpasswd.h"

RDGetPasswd::RDGetPasswd(QString *passwd,QWidget *parent)
  : RDDialog(parent)
{
  pw_password=passwd;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle(tr("Enter Password"));

  //
  // Label
  //
  QLabel *label=new QLabel(tr("Enter password"),this);
  label->setGeometry(0,5,sizeHint().width(),20);
  label->setAlignment(Qt::AlignCenter);

  //
  // Password
  //
  pw_password_edit=new QLineEdit(this);
  pw_password_edit->setGeometry(10,30,sizeHint().width()-20,20);
  pw_password_edit->setEchoMode(QLineEdit::Password);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDGetPasswd::~RDGetPasswd()
{
}


QSize RDGetPasswd::sizeHint() const
{
  return QSize(190,120);
} 


QSizePolicy RDGetPasswd::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDGetPasswd::okData()
{
  *pw_password=pw_password_edit->text();
  done(0);
}


void RDGetPasswd::cancelData()
{
  done(-1);
}
