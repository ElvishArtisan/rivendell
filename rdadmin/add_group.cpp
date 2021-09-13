// add_group.cpp
//
// Add a Rivendell Group
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

#include <edit_group.h>
#include <add_group.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

AddGroup::AddGroup(QString *group,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  group_group=group;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Add Group"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Group Name
  //
  group_name_edit=new QLineEdit(this);
  group_name_edit->setGeometry(145,11,sizeHint().width()-150,19);
  group_name_edit->setMaxLength(10);
  group_name_edit->setValidator(validator);
  QLabel *label=new QLabel(tr("New Group Name:"),this);
  label->setFont(labelFont());
  label->setGeometry(10,11,130,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Enable Users Checkbox
  //
  group_users_box=new QCheckBox(this);
  group_users_box->setGeometry(40,40,15,15);
  group_users_box->setChecked(true);
  label=new QLabel(tr("Enable Group for All Users"),this);
  label->setFont(subLabelFont());
  label->setGeometry(60,38,sizeHint().width()-60,19);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Enable Services Checkbox
  //
  group_svcs_box=new QCheckBox(this);
  group_svcs_box->setGeometry(40,61,15,15);
  group_svcs_box->setChecked(true);
  label=new QLabel(tr("Enable Group for All Services"),this);
  label->setFont(subLabelFont());
  label->setGeometry(60,58,sizeHint().width()-60,19);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

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
  QString err_msg;

  if(group_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("You must give the group a name!"));
    return;
  }
  if(!RDGroup::create(group_name_edit->text(),group_users_box->isChecked(),
		      group_svcs_box->isChecked(),&err_msg)) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Error creating group!")+"\n"+
			 "["+err_msg+"]");
    return;
  }		      

  EditGroup *group=new EditGroup(group_name_edit->text(),this);
  if(!group->exec()) {
    if(!RDGroup::remove(group_name_edit->text(),&err_msg)) {
      QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			   tr("Error removing stale database record!")+"\n"+
			   "["+err_msg+"]");
    }
    delete group;
    done(false);
    return;
  }
  delete group;
  *group_group=group_name_edit->text();
  done(true);
}


void AddGroup::cancelData()
{
  done(false);
}
