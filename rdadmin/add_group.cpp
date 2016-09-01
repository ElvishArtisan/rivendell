// add_group.cpp
//
// Add a Rivendell Group
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include <rdescape_string.h>
#include <rdpasswd.h>

#include "edit_group.h"
#include "add_group.h"

AddGroup::AddGroup(QString *group,QWidget *parent)
  : QDialog(parent,"",true)
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
  // Group Name
  //
  group_name_edit=new QLineEdit(this);
  group_name_edit->setGeometry(145,11,sizeHint().width()-150,19);
  group_name_edit->setMaxLength(10);
  QLabel *label=new QLabel(group_name_edit,tr("&New Group Name:"),this);
  label->setGeometry(10,11,130,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Enable Users Checkbox
  //
  group_users_box=new QCheckBox(this);
  group_users_box->setGeometry(40,40,15,15);
  group_users_box->setChecked(true);
  label=new QLabel(group_users_box,tr("Enable Group for All Users"),this);
  label->setGeometry(60,38,sizeHint().width()-60,19);
  label->setFont(user_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Enable Services Checkbox
  //
  group_svcs_box=new QCheckBox(this);
  group_svcs_box->setGeometry(40,61,15,15);
  group_svcs_box->setChecked(true);
  label=new QLabel(group_svcs_box,tr("Enable Group for All Services"),this);
  label->setGeometry(60,58,sizeHint().width()-60,19);
  label->setFont(user_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

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
  if(group_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Invalid Name"),
			 tr("You must give the group a name!"));
    return;
  }

  if(RDGroup::exists(group_name_edit->text())) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Group Exists"),
			 tr("A")+" \""+group_name_edit->text()+"\" "+
			 tr("group already exists!"));
    return;
  }
  RDGroup::create(group_name_edit->text(),group_users_box->isChecked(),
		  group_svcs_box->isChecked());
  EditGroup *group=new EditGroup(group_name_edit->text(),this);
  if(group->exec()<0) {
    RDGroup::remove(group_name_edit->text());
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
