// rename_group.cpp
//
// Rename a Rivendell Group
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
#include <rdescape_string.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>
#include <rduser.h>

#include "rename_group.h"

RenameGroup::RenameGroup(QString group,QWidget *parent)
  : RDDialog(parent)
{
  group_name=group;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rename Group"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Current Group Name
  //
  group_name_edit=new QLineEdit(this);
  group_name_edit->setGeometry(165,11,sizeHint().width()-175,19);
  group_name_edit->setMaxLength(10);
  group_name_edit->setReadOnly(true);
  QLabel *group_name_label=new QLabel(tr("Current Group Name:"),this);
  group_name_label->setGeometry(10,11,150,19);
  group_name_label->setFont(labelFont());
  group_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // New Group Name
  //
  group_newname_edit=new QLineEdit(this);
  group_newname_edit->setGeometry(165,33,sizeHint().width()-175,19);
  group_newname_edit->setMaxLength(10);
  group_newname_edit->setValidator(validator);
  QLabel *group_newname_label=new QLabel(tr("New Group Name:"),this);
  group_newname_label->setGeometry(10,33,150,19);
  group_newname_label->setFont(labelFont());
  group_newname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  group_name_edit->setText(group_name);
}


RenameGroup::~RenameGroup()
{
}


QSize RenameGroup::sizeHint() const
{
  return QSize(300,130);
} 


QSizePolicy RenameGroup::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RenameGroup::exec(QString *newname)
{
  group_new_name=newname;

  return RDDialog::exec();
}


void RenameGroup::okData()
{
  QString sql;
  RDSqlQuery *q;
  bool merging=false;

  if(group_newname_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Group"),
			 tr("The group name is invalid!"));
    return;
  }

  sql=QString("select NAME from GROUPS where ")+
    "NAME=\""+RDEscapeString(group_newname_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Group Exists"),
			     tr("A")+" \""+group_newname_edit->text()+"\" "+
			     tr("group already exists.")+"\n"+
			     tr("Do you want to combine the two?"),
		      QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete q;
      return;
    }
    merging=true;
  }
  delete q;

  //
  // Update Cart List
  //
  sql=QString("update CART set ")+
    "GROUP_NAME=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
    "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update LogManager Events
  //
  sql=QString("update EVENTS set ")+
    "SCHED_GROUP=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
    "SCHED_GROUP=\""+RDEscapeString(group_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update Replicators
  //
  sql=QString("update REPLICATOR_MAP set ")+
    "GROUP_NAME=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
    "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"",
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update Dropboxes
  //
  sql=QString("update DROPBOXES set ")+
    "GROUP_NAME=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
    "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update Group List
  //
  if(!merging) {
    sql=QString("update GROUPS set ")+
      "NAME=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
      "NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Update AUDIO_PERMS
    //
    sql=QString("update AUDIO_PERMS set ")+
      "GROUP_NAME=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
      "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Update USER_PERMS
    //
    sql=QString("update USER_PERMS set ")+
      "GROUP_NAME=\""+RDEscapeString(group_newname_edit->text())+"\" where "+
      "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    sql=QString("delete from GROUPS where ")+
      "NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Update AUDIO_PERMS
    //
    sql=QString("delete from AUDIO_PERMS where ")+
      "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;
    
    //
    // Update USER_PERMS
    //
    sql=QString("delete from USER_PERMS where ")+
      "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;
  }

  *group_new_name=group_newname_edit->text();

  done(true);
}


void RenameGroup::cancelData()
{
  done(false);
}


void RenameGroup::closeEvent(QCloseEvent *e)
{
  cancelData();
}
