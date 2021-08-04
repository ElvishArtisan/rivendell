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

MySqlLogin::MySqlLogin(QString *username,QString *password,RDConfig *c,
		       QWidget *parent)
  : RDDialog(c,parent)
{
  login_name=username;
  login_password=password;

  setWindowTitle(tr("mySQL Admin"));
  setMinimumSize(sizeHint());

  //
  // Message Label
  //
  login_message_label=
    new QLabel(tr("Enter your MySQL administrator username and password\nThe Hostname and Database are found in /etc/rd.conf"),this);
  login_message_label->setAlignment(Qt::AlignCenter);
  
  //
  // MySql Login Name
  //
  login_name_edit=new QLineEdit(this);
  login_name_edit->setMaxLength(16);
  login_name_edit->setFocus();
  login_name_label=new QLabel(tr("Username:"),this);
  login_name_label->setFont(labelFont());
  login_name_label->setAlignment(Qt::AlignRight);

  //
  // MySql Login Password
  //
  login_password_edit=new QLineEdit(this);
  login_password_edit->setMaxLength(16);
  login_password_edit->setEchoMode(QLineEdit::Password);
  login_password_label=new QLabel(tr("Password:"),this);
  login_password_label->setFont(labelFont());
  login_password_label->setAlignment(Qt::AlignRight);

  //
  // OK Button
  //
  login_ok_button=new QPushButton(this);
  login_ok_button->setFont(buttonFont());
  login_ok_button->setText(tr("OK"));
  login_ok_button->setDefault(true);
  connect(login_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  login_cancel_button=new QPushButton(this);
  login_cancel_button->setFont(buttonFont());
  login_cancel_button->setText(tr("Cancel"));
  connect(login_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
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


void MySqlLogin::resizeEvent(QResizeEvent *e)
{
  login_message_label->setGeometry(10,10,size().width()-20,30);

  login_name_label->setGeometry(size().width()/2-125,50,85,19);
  login_name_edit->setGeometry(size().width()/2-125+90,50,140,19);

  login_password_label->setGeometry(size().width()/2-125,70,85,19);
  login_password_edit->setGeometry(size().width()/2-125+90,70,140,19);

  login_ok_button->setGeometry(size().width()/2-90,size().height()-60,80,50);
  login_cancel_button->
    setGeometry(size().width()/2+10,size().height()-60,80,50);
}
