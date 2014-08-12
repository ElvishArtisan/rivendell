// rename_group.cpp
//
// Rename a Rivendell Group
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rename_group.cpp,v 1.14 2010/08/03 16:26:03 cvs Exp $
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

#include <rename_group.h>
#include <rduser.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>
#include <rdescape_string.h>


RenameGroup::RenameGroup(QString group,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
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
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Current Group Name
  //
  group_name_edit=new QLineEdit(this,"group_name_edit");
  group_name_edit->setGeometry(165,11,sizeHint().width()-175,19);
  group_name_edit->setMaxLength(10);
  group_name_edit->setReadOnly(true);
  QLabel *group_name_label=
    new QLabel(group_name_edit,tr("Current Group Name:"),
	       this,"group_name_label");
  group_name_label->setGeometry(10,11,150,19);
  group_name_label->setFont(font);
  group_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // New Group Name
  //
  group_newname_edit=new QLineEdit(this,"group_newname_edit");
  group_newname_edit->setGeometry(165,33,sizeHint().width()-175,19);
  group_newname_edit->setMaxLength(10);
  group_newname_edit->setValidator(validator);
  QLabel *group_newname_label=
    new QLabel(group_newname_edit,tr("New &Group Name:"),
	       this,"group_newname_label");
  group_newname_label->setGeometry(10,33,150,19);
  group_newname_label->setFont(font);
  group_newname_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

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

  sql=QString().
    sprintf("select NAME from GROUPS where NAME=\"%s\"",
	    (const char *)RDEscapeString(group_newname_edit->text()));
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
  sql=QString().
    sprintf("update CART set GROUP_NAME=\"%s\" where GROUP_NAME=\"%s\"",
	    (const char *)RDEscapeString(group_newname_edit->text()),
	    (const char *)RDEscapeString(group_name_edit->text()));
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update LogManager Events
  //
  sql=QString().
    sprintf("update EVENTS set SCHED_GROUP=\"%s\" where SCHED_GROUP=\"%s\"",
	    (const char *)RDEscapeString(group_newname_edit->text()),
	    (const char *)RDEscapeString(group_name_edit->text()));
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update Replicators
  //
  sql=QString().
    sprintf("update REPLICATOR_MAP set GROUP_NAME=\"%s\" \
             where GROUP_NAME=\"%s\"",
	    (const char *)RDEscapeString(group_newname_edit->text()),
	    (const char *)RDEscapeString(group_name_edit->text()));
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update Dropboxes
  //
  sql=QString().
    sprintf("update DROPBOXES set GROUP_NAME=\"%s\" \
             where GROUP_NAME=\"%s\"",
	    (const char *)RDEscapeString(group_newname_edit->text()),
	    (const char *)RDEscapeString(group_name_edit->text()));
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update Group List
  //
  if(!merging) {
    sql=QString().
      sprintf("update GROUPS set NAME=\"%s\" where NAME=\"%s\"",
	      (const char *)RDEscapeString(group_newname_edit->text()),
	      (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Update AUDIO_PERMS
    //
    sql=QString().
      sprintf("update AUDIO_PERMS set GROUP_NAME=\"%s\" \
               where GROUP_NAME=\"%s\"",
	      (const char *)RDEscapeString(group_newname_edit->text()),
	      (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Update USER_PERMS
    //
    sql=QString().
      sprintf("update USER_PERMS set GROUP_NAME=\"%s\" where GROUP_NAME=\"%s\"",
	      (const char *)RDEscapeString(group_newname_edit->text()),
	      (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    sql=QString().
      sprintf("delete from GROUPS where NAME=\"%s\"",
	      (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;

    //
    // Update AUDIO_PERMS
    //
    sql=QString().
      sprintf("delete from AUDIO_PERMS where GROUP_NAME=\"%s\"",
	      (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;
    
    //
    // Update USER_PERMS
    //
    sql=QString().
      sprintf("delete from USER_PERMS where GROUP_NAME=\"%s\"",
	      (const char *)RDEscapeString(group_name_edit->text()));
    q=new RDSqlQuery(sql);
    delete q;
  }

  done(0);
}


void RenameGroup::cancelData()
{
  done(-1);
}
