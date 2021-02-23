// mysql_login.cpp
//
// mySQL Administrative Login widget for RDDbConfig
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

#include <QPushButton>

#include "mysql_login.h"

MySqlLogin::MySqlLogin(QString *username,QString *password, QWidget *parent)
  : RDDialog(parent)
{
  login_name=username;
  login_password=password;

  setWindowTitle(tr("mySQL Admin"));

  //
  // Message Label
  //
  QLabel *label=new QLabel(tr("Enter your MySQL administrator username and password\nThe Hostname and Database are found in /etc/rd.conf"),this);
  label->setFont(labelFont());
  label->setGeometry(10,10,sizeHint().width()-20,30);
  label->setAlignment(Qt::AlignCenter);
  
  //
  // MySql Login Name
  //
  login_name_edit=new QLineEdit(this);
  login_name_edit->setGeometry(sizeHint().width()/2-125+90,50,140,19);
  login_name_edit->setMaxLength(16);
  login_name_edit->setFocus();
  QLabel *login_name_label=new QLabel(tr("Username:"),this);
  login_name_label->setFont(labelFont());
  login_name_label->setGeometry(sizeHint().width()/2-125,50,85,19);
  login_name_label->setAlignment(Qt::AlignRight);

  //
  // MySql Login Password
  //
  login_password_edit=new QLineEdit(this);
  login_password_edit->setGeometry(sizeHint().width()/2-125+90,70,140,19);
  login_password_edit->setMaxLength(16);
  login_password_edit->setEchoMode(QLineEdit::Password);
  QLabel *login_password_label=new QLabel(tr("Password:"),this);
  login_password_label->setFont(labelFont());
  login_password_label->setGeometry(sizeHint().width()/2-125,70,85,19);
  login_password_label->setAlignment(Qt::AlignRight);

  //
  // OK Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()/2-90,sizeHint().height()-60,80,50);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  ok_button->setDefault(true);
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()/2+10,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


MySqlLogin::~MySqlLogin()
{
  delete login_name_edit;
  delete login_password_edit;
}


QSize MySqlLogin::sizeHint() const
{
  return QSize(340,160);
} 


QSizePolicy MySqlLogin::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MySqlLogin::okData()
{
  *login_name=login_name_edit->text();
  *login_password=login_password_edit->text();

  done(0);
}


void MySqlLogin::cancelData()
{
  done(1);
}

