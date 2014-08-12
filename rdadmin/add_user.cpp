// add_user.cpp
//
// Add a Rivendell User
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_user.cpp,v 1.20 2010/07/29 19:32:34 cvs Exp $
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
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <rddb.h>

#include <edit_user.h>
#include <add_user.h>
#include <rdpasswd.h>
#include <rdescape_string.h>


AddUser::AddUser(QString *username,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  user_name=username;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add User"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // User Name
  //
  user_name_edit=new QLineEdit(this,"user_name_edit");
  user_name_edit->setGeometry(125,11,sizeHint().width()-135,19);
  user_name_edit->setMaxLength(255);
  QLabel *user_name_label=new QLabel(user_name_edit,tr("&New User Name:"),this,
				       "user_name_label");
  user_name_label->setGeometry(10,13,110,19);
  user_name_label->setFont(font);
  user_name_label->setAlignment(AlignRight|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddUser::~AddUser()
{
  delete user_name_edit;
}


QSize AddUser::sizeHint() const
{
  return QSize(400,110);
} 


QSizePolicy AddUser::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddUser::okData()
{
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString sql;
  QString username=RDEscapeString(user_name_edit->text());

  if(user_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("You must give the user a name!"));
    return;
  }

  sql=QString().sprintf("insert into USERS set LOGIN_NAME=\"%s\",\
                         PASSWORD=PASSWORD(\"\")",
			(const char *)username);
  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    QMessageBox::warning(this,tr("User Exists"),tr("User Already Exists!"),
			 1,0,0);
    delete q;
    return;
  }
  delete q;
  sql="select NAME from GROUPS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("insert into USER_PERMS set USER_NAME=\"%s\",\
                           GROUP_NAME=\"%s\"",
			  (const char *)username,
			  (const char *)q->value(0).toString());
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
  EditUser *user=new EditUser(user_name_edit->text(),this,"user");
  if(user->exec()<0) {
    sql=QString().sprintf("delete from USER_PERMS where USER_NAME=\"%s\"",
			  (const char *)username);
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString().sprintf("delete from USERS where LOGIN_NAME=\"%s\"",
			  (const char *)username);
    q=new RDSqlQuery(sql);
    delete q;
    delete user;
    done(-1);
    return;
  }
  delete user;
  *user_name=user_name_edit->text();
  done(0);
}


void AddUser::cancelData()
{
  done(-1);
}
