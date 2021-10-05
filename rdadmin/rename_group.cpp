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
  validator->addBannedChar(',');

  //
  // Current Group Name
  //
  group_name_edit=new QLineEdit(this);
  group_name_edit->setMaxLength(10);
  group_name_edit->setReadOnly(true);
  group_name_label=new QLabel(tr("Current Group Name:"),this);
  group_name_label->setFont(labelFont());
  group_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // New Group Name
  //
  group_newname_edit=new QLineEdit(this);
  group_newname_edit->setMaxLength(10);
  group_newname_edit->setValidator(validator);
  connect(group_newname_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(newNameChangedData(const QString &)));
  group_newname_label=new QLabel(tr("New Group Name:"),this);
  group_newname_label->setFont(labelFont());
  group_newname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  group_ok_button=new QPushButton(this);
  group_ok_button->setDefault(true);
  group_ok_button->setFont(buttonFont());
  group_ok_button->setText(tr("OK"));
  connect(group_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  group_cancel_button=new QPushButton(this);
  group_cancel_button->setFont(buttonFont());
  group_cancel_button->setText(tr("Cancel"));
  connect(group_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

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
  group_newname_edit->setText(*newname);
  newNameChangedData(*newname);

  return RDDialog::exec();
}


void RenameGroup::newNameChangedData(const QString &str)
{
  group_ok_button->
    setDisabled(str.isEmpty()||(str==group_name_edit->text()));
}


void RenameGroup::okData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  RDSqlQuery *q1=NULL;
  bool merging=false;
  QString newname=group_newname_edit->text();
  
  sql=QString("select ")+
    "`NAME`,"+                // 00
    "`DEFAULT_LOW_CART`,"+    // 01
    "`DEFAULT_HIGH_CART`,"+   // 02
    "`ENFORCE_CART_RANGE` "+  // 03
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(group_newname_edit->text())+"'";
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
    if(q->value(3).toString()=="Y") {
      sql=QString("select ")+
	"`NUMBER` "+  // 00
	"from `CART` where "+
	"`GROUP_NAME`='"+RDEscapeString(group_name_edit->text())+"' && "+
	QString::asprintf(" (`NUMBER`<%u || ",q->value(1).toUInt())+
	QString::asprintf("`NUMBER`>%u)",q->value(2).toUInt());
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	QMessageBox::information(this,"RDAdmin - "+tr("Conflicting Cart Numbers"),
	     tr("One or more carts in this group has an out-of-range number.")+
	        "\n"+tr("Unable to merge!"));
	delete q1;
	delete q;
	return;
      }
      delete q1;
    }
    newname=q->value(0).toString();  // To ensure consistent case
    merging=true;
  }
  delete q;

  Rename(group_name_edit->text(),newname,merging);
  *group_new_name=group_newname_edit->text();

  if(merging) {
    done(RenameGroup::Merged);
  }
  else {
    done(RenameGroup::Renamed);
  }
}


void RenameGroup::cancelData()
{
  done(RenameGroup::Cancelled);
}


void RenameGroup::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void RenameGroup::resizeEvent(QResizeEvent *e)
{
  group_name_edit->setGeometry(165,11,size().width()-175,19);
  group_name_label->setGeometry(10,11,150,19);

  group_newname_edit->setGeometry(165,33,size().width()-175,19);
  group_newname_label->setGeometry(10,33,150,19);

  group_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  group_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void RenameGroup::Rename(const QString &old_name,const QString &new_name,
			 bool merge) const
{
  QString sql;

  RenameField("CART","GROUP_NAME",old_name,new_name);
  RenameField("EVENTS","SCHED_GROUP",old_name,new_name);
  RenameField("REPLICATOR_MAP","GROUP_NAME",old_name,new_name);
  RenameField("DROPBOXES","GROUP_NAME",old_name,new_name);
  RenameField("SERVICES","TRACK_GROUP",old_name,new_name);
  RenameField("SERVICES","AUTOSPOT_GROUP",old_name,new_name);
  RenameField("NEXUS_STATIONS","RD_GROUP_NAME",old_name,new_name);
  RenameField("REPORT_GROUPS","GROUP_NAME",old_name,new_name);

  //
  // Update Group List
  //
  if(merge) {
    sql=QString("delete from `GROUPS` where ")+
      "`NAME`='"+RDEscapeString(old_name)+"'";
    RDSqlQuery::apply(sql);

    //
    // Update AUDIO_PERMS
    //
    sql=QString("delete from `AUDIO_PERMS` where ")+
      "`GROUP_NAME`='"+RDEscapeString(old_name)+"'";
    RDSqlQuery::apply(sql);
    
    //
    // Update USER_PERMS
    //
    sql=QString("delete from `USER_PERMS` where ")+
      "`GROUP_NAME`='"+RDEscapeString(old_name)+"'";
    RDSqlQuery::apply(sql);
  }
  else {
    RenameField("GROUPS","NAME",old_name,new_name);
    RenameField("AUDIO_PERMS","GROUP_NAME",old_name,new_name);
    RenameField("USER_PERMS","GROUP_NAME",old_name,new_name);
  }
}


void RenameGroup::RenameField(const QString &table,const QString &field,
			      const QString &old_name,
			      const QString &new_name) const
{
  QString sql;

  sql=QString("update ")+
    "`"+table+"` set "+
    "`"+field+"`='"+RDEscapeString(new_name)+"' "+
    "where `"+field+"`='"+RDEscapeString(old_name)+"'";
  RDSqlQuery::apply(sql);
}
