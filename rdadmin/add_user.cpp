// add_user.cpp
//
// Add a Rivendell User
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

#include <QMessageBox>

#include <rddb.h>

#include <edit_user.h>
#include <add_user.h>
#include <rdpasswd.h>
#include <rdescape_string.h>

AddUser::AddUser(QString *username,QWidget *parent)
  : RDDialog(parent)
{
  user_name=username;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Add User"));

  //
  // User Name
  //
  user_name_edit=new QLineEdit(this);
  user_name_edit->setGeometry(125,11,sizeHint().width()-135,19);
  user_name_edit->setMaxLength(255);
  QLabel *user_name_label=new QLabel(tr("New User Name:"),this);
  user_name_label->setGeometry(10,13,110,19);
  user_name_label->setFont(labelFont());
  user_name_label->setAlignment(Qt::AlignRight);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
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
  QString sql;
  QString username=RDEscapeString(user_name_edit->text());

  if(user_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("You must give the user a name!"));
    return;
  }

  sql=QString("insert into `USERS` set ")+
    "`LOGIN_NAME`='"+RDEscapeString(username)+"'";
  if(!RDSqlQuery::apply(sql)) {
    QMessageBox::warning(this,tr("User Exists"),tr("User Already Exists!"),
			 1,0,0);
    return;
  }
  sql="select `NAME` from `GROUPS`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `USER_PERMS` set ")+
      "`USER_NAME`='"+RDEscapeString(username)+"',"+
      "`GROUP_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;
  EditUser *user=new EditUser(user_name_edit->text(),this);
  if(user->exec()<0) {
    sql=QString("delete from `USER_PERMS` where ")+
      "`USER_NAME`='"+RDEscapeString(username)+"'";
    RDSqlQuery::apply(sql);

    sql=QString("delete from `USERS` where ")+
      "`LOGIN_NAME`='"+RDEscapeString(username)+"'";
    RDSqlQuery::apply(sql);
    delete user;
    done(false);
    return;
  }
  delete user;
  *user_name=user_name_edit->text();

  done(true);
}


void AddUser::cancelData()
{
  done(false);
}
