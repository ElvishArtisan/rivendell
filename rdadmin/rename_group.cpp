// rename_group.cpp
//
// Rename a Rivendell Group
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <q3listbox.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
#include <rddb.h>

#include <rename_group.h>
#include <rduser.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>
#include <rdescape_string.h>


RenameGroup::RenameGroup(QString group,QWidget *parent)
  : QDialog(parent,"",true)
{
  group_name=group;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Rename Group"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

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
  QLabel *group_name_label=
    new QLabel(group_name_edit,tr("Current Group Name:"),this);
  group_name_label->setGeometry(10,11,150,19);
  group_name_label->setFont(font);
  group_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // New Group Name
  //
  group_newname_edit=new QLineEdit(this);
  group_newname_edit->setGeometry(165,33,sizeHint().width()-175,19);
  group_newname_edit->setMaxLength(10);
  group_newname_edit->setValidator(validator);
  QLabel *group_newname_label=
    new QLabel(group_newname_edit,tr("New &Group Name:"),this);
  group_newname_label->setGeometry(10,33,150,19);
  group_newname_label->setFont(font);
  group_newname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

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


void RenameGroup::okData()
{
  QString sql;
  RDSqlQuery *q;
  bool merging=false;
  QString str1;
  QString str2;

  if(group_newname_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Group"),
			 tr("The group name is invalid!"));
    return;
  }

  sql=QString("select NAME from GROUPS where ")+
    "NAME=\""+RDEscapeString(group_newname_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    str1=QString(tr("A"));
    str2=QString(tr("group already exists.\nDo you want to combine the two?"));
    if(QMessageBox::question(this,tr("Group Exists"),
			     QString().sprintf("%s %s %s",(const char *)str1,
		  (const char *)RDEscapeString(group_newname_edit->text()),
					       (const char *)str2),
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
    "GROUP_NAME=\""+RDEscapeString(group_name_edit->text())+"\"";
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

  done(0);
}


void RenameGroup::cancelData()
{
  done(-1);
}
