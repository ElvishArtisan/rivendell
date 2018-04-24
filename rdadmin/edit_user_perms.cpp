// edit_user_perms.cpp
//
// Edit Rivendell User/Group Permissions
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdescape_string.h>

#include "edit_user_perms.h"

EditUserPerms::EditUserPerms(RDUser *user,QWidget *parent)
  : QDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

  user_user=user;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("User: ")+user_user->name());

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Groups Selector
  //
  user_host_sel=new RDListSelector(this);
  user_host_sel->sourceSetLabel(tr("Available Groups"));
  user_host_sel->destSetLabel(tr("Enabled Groups"));
  user_host_sel->setGeometry(10,10,380,130);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  sql=QString("select GROUP_NAME from USER_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(user_user->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    user_host_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select NAME from GROUPS");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(user_host_sel->destFindItem(q->value(0).toString())==0) {
      user_host_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
}


EditUserPerms::~EditUserPerms()
{
}


QSize EditUserPerms::sizeHint() const
{
  return QSize(400,212);
} 


QSizePolicy EditUserPerms::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditUserPerms::okData()
{
  RDSqlQuery *q;
  QString sql;

  //
  // Add New Groups
  //
  for(unsigned i=0;i<user_host_sel->destCount();i++) {
    sql=QString("select GROUP_NAME from USER_PERMS where ")+
      "(USER_NAME=\""+RDEscapeString(user_user->name())+"\")&&"+
      "(GROUP_NAME=\""+RDEscapeString(user_host_sel->destText(i))+"\")";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into USER_PERMS set ")+
	"USER_NAME=\""+RDEscapeString(user_user->name())+"\","+
	"GROUP_NAME=\""+RDEscapeString(user_host_sel->destText(i))+"\"";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Groups
  //
  sql=QString("delete from USER_PERMS where ")+
    "(USER_NAME=\""+RDEscapeString(user_user->name())+"\")";
  for(unsigned i=0;i<user_host_sel->destCount();i++) {
    sql+=QString("&&(GROUP_NAME<>")+"\""+
      RDEscapeString(user_host_sel->destText(i))+"\")";
  }
  q=new RDSqlQuery(sql);
  delete q;
  done(0);
}


void EditUserPerms::cancelData()
{
  done(1);
}
