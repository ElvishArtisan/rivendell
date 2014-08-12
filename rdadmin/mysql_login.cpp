// mysql_login.cpp
//
// mySQL Administrative Login widget for RDAdmin.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: mysql_login.cpp,v 1.9 2010/07/29 19:32:35 cvs Exp $
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
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rdlabel.h>

#include <mysql_login.h>


MySqlLogin::MySqlLogin(QString msg,QString *username,QString *password,
		       QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  setCaption(tr("mySQL Admin"));
  login_name=username;
  login_password=password;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",10,QFont::Normal);
  font.setPixelSize(10);

  //
  // Message Label
  //
  RDLabel *label=new RDLabel(msg,this,"message_label");
  label->setFont(font);
  label->setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-130);
  label->setAlignment(AlignCenter);
  

  //
  // MySql Login Name
  //
  login_name_edit=new QLineEdit(this,"login_name_edit");
  login_name_edit->setFont(font);
  login_name_edit->setGeometry(100,sizeHint().height()-110,100,19);
  login_name_edit->setMaxLength(16);
  login_name_edit->setFocus();
  QLabel *login_name_label=new QLabel(login_name_edit,tr("User &Name:"),this,
				       "login_name_label");
  login_name_label->setFont(font);
  login_name_label->setGeometry(10,sizeHint().height()-109,85,19);
  login_name_label->setAlignment(AlignRight|ShowPrefix);

  //
  // MySql Login Password
  //
  login_password_edit=new QLineEdit(this,"login_password_edit");
  login_password_edit->setFont(font);
  login_password_edit->setGeometry(100,sizeHint().height()-90,100,19);
  login_password_edit->setMaxLength(16);
  login_password_edit->setEchoMode(QLineEdit::Password);
  QLabel *login_password_label=new QLabel(login_password_edit,tr("&Password:"),
					  this,"login_password_label");
  login_password_label->setFont(font);
  login_password_label->setGeometry(10,sizeHint().height()-88,85,19);
  login_password_label->setAlignment(AlignRight|ShowPrefix);

  //
  // OK Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  ok_button->setDefault(true);
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // CANCEL Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
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
  return QSize(360,210);
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
