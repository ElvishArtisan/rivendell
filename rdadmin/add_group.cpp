// add_group.cpp
//
// Add a Rivendell Service
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_group.cpp,v 1.20 2010/07/29 19:32:34 cvs Exp $
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
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <rddb.h>

#include <edit_group.h>
#include <add_group.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>
#include <rdescape_string.h>

AddGroup::AddGroup(QString *group,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  group_group=group;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Group"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont user_font=QFont("Helvetica",12,QFont::Normal);
  user_font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Group Name
  //
  group_name_edit=new QLineEdit(this,"group_name_edit");
  group_name_edit->setGeometry(145,11,sizeHint().width()-150,19);
  group_name_edit->setMaxLength(10);
  group_name_edit->setValidator(validator);
  QLabel *label=new QLabel(group_name_edit,tr("&New Group Name:"),
				      this,"group_name_label");
  label->setGeometry(10,11,130,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Enable Users Checkbox
  //
  group_users_box=new QCheckBox(this,"group_users_box");
  group_users_box->setGeometry(40,40,15,15);
  group_users_box->setChecked(true);
  label=new QLabel(group_users_box,tr("Enable Group for All Users"),
				      this,"group_users_label");
  label->setGeometry(60,38,sizeHint().width()-60,19);
  label->setFont(user_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Enable Services Checkbox
  //
  group_svcs_box=new QCheckBox(this,"group_svcs_box");
  group_svcs_box->setGeometry(40,61,15,15);
  group_svcs_box->setChecked(true);
  label=new QLabel(group_svcs_box,tr("Enable Group for All Services"),
				      this,"group_svcs_label");
  label->setGeometry(60,58,sizeHint().width()-60,19);
  label->setFont(user_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddGroup::~AddGroup()
{
  delete group_name_edit;
}


QSize AddGroup::sizeHint() const
{
  return QSize(250,152);
} 


QSizePolicy AddGroup::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddGroup::okData()
{
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString sql;

  if(group_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("You must give the group a name!"));
    return;
  }

  sql=QString().sprintf("insert into GROUPS set NAME=\"%s\"",
			(const char *)RDEscapeString(group_name_edit->text()));

  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    QMessageBox::warning(this,tr("Group Exists"),tr("Group Already Exists!"),
			 1,0,0);
    delete q;
    return;
  }
  delete q;

  //
  // Create Default Users Perms
  //
  if(group_users_box->isChecked()) {
    sql="select LOGIN_NAME from USERS";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into USER_PERMS set USER_NAME=\"%s\",\
                             GROUP_NAME=\"%s\"",
			    (const char *)q->value(0).toString(),
			    (const char *)RDEscapeString(group_name_edit->text()));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  //
  // Create Default Service Perms
  //
  if(group_svcs_box->isChecked()) {
    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into AUDIO_PERMS set SERVICE_NAME=\"%s\",\
                             GROUP_NAME=\"%s\"",
			    (const char *)q->value(0).toString(),
			    (const char *)RDEscapeString(group_name_edit->text()));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  EditGroup *group=new EditGroup(group_name_edit->text(),this,"group");
  if(group->exec()<0) {
    sql=QString().sprintf("delete from USER_PERMS where GROUP_NAME=\"%s\"",
			  (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString().sprintf("delete from AUDIO_PERMS where GROUP_NAME=\"%s\"",
			  (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString().sprintf("delete from GROUPS where NAME=\"%s\"",
			  (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;
    delete group;
    done(-1);
    return;
  }
  delete group;
  *group_group=group_name_edit->text();
  done(0);
}


void AddGroup::cancelData()
{
  done(-1);
}
