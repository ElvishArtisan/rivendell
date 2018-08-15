// edit_schedcodes.cpp
//
// Edit scheduler codes dialog
//
//   Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2005-2018
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <qpushbutton.h>
#include <qstring.h>
#include <q3listbox.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <q3buttongroup.h>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>
#include <rduser.h>

#include "edit_group.h"
#include "edit_schedcodes.h"

EditSchedCode::EditSchedCode(QString schedCode,QString description,
			     QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  schedCode_code=new QString(schedCode);
  schedCode_description=new QString(description);
  
  setWindowTitle("RDAdmin - "+tr("Scheduler Code: ")+schedCode);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Text Validators
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Code Name
  //
  schedCode_name_edit=new QLineEdit(this);
  schedCode_name_edit->setGeometry(125,11,100,19);
  schedCode_name_edit->setMaxLength(10);
  schedCode_name_edit->setReadOnly(true);
  QLabel *schedCode_name_label=
    new QLabel(schedCode_name_edit,tr("Scheduler Code:"),this);
  schedCode_name_label->setGeometry(10,11,110,19);
  schedCode_name_label->setFont(font);
  schedCode_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Code Description
  //
  schedCode_description_edit=new QLineEdit(this);
  schedCode_description_edit->setGeometry(125,32,sizeHint().width()-135,19);
  schedCode_description_edit->setMaxLength(255);
  schedCode_description_edit->setValidator(validator);
  QLabel *schedCode_description_label=
    new QLabel(schedCode_description_edit,tr("Code Description:"),this);
  schedCode_description_label->setGeometry(10,32,110,19);
  schedCode_description_label->setFont(font);
  schedCode_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  schedCode_name_edit->setText(*schedCode_code);
  schedCode_description_edit->setText(*schedCode_description);
}


EditSchedCode::~EditSchedCode()
{
  delete schedCode_name_edit;
  delete schedCode_description_edit;
}


QSize EditSchedCode::sizeHint() const
{
  return QSize(400,140);
} 


QSizePolicy EditSchedCode::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSchedCode::okData()
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update SCHED_CODES set ")+
    "DESCRIPTION=\""+RDEscapeString(schedCode_description_edit->text())+"\" "+
    "where CODE=\""+RDEscapeString(schedCode_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  *schedCode_description=schedCode_description_edit->text();
  done(0);
}


void EditSchedCode::cancelData()
{
  done(-1);
}

