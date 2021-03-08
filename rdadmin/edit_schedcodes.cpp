// edit_schedcodes.cpp
//
// Edit scheduler codes dialog
//
//   Based on original code by Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2005-2018
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

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_group.h"
#include "edit_schedcodes.h"

EditSchedCode::EditSchedCode(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Code Name
  //
  d_code_edit=new QLineEdit(this);
  d_code_edit->setGeometry(125,11,100,19);
  d_code_edit->setMaxLength(10);
  d_code_edit->setReadOnly(true);
  QLabel *schedcode_name_label=new QLabel(tr("Scheduler Code:"),this);
  schedcode_name_label->setGeometry(10,11,110,19);
  schedcode_name_label->setFont(labelFont());
  schedcode_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Code Description
  //
  d_description_edit=new QLineEdit(this);
  d_description_edit->setGeometry(125,32,sizeHint().width()-135,19);
  d_description_edit->setMaxLength(255);
  QLabel *schedcode_description_label=new QLabel(tr("Code Description:"),this);
  schedcode_description_label->setGeometry(10,32,110,19);
  schedcode_description_label->setFont(labelFont());
  schedcode_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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


EditSchedCode::~EditSchedCode()
{
  delete d_code_edit;
  delete d_description_edit;
}


QSize EditSchedCode::sizeHint() const
{
  return QSize(400,140);
} 


QSizePolicy EditSchedCode::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditSchedCode::exec(const QString &scode)
{
  setWindowTitle("RDAdmin - "+tr("Scheduler Code: ")+scode);

  d_code_edit->setText(scode);

  QString sql=QString("select ")+
    "DESCRIPTION "+  // 00
    "from SCHED_CODES where "+
    "CODE=\""+RDEscapeString(scode)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    d_description_edit->setText(q->value(0).toString());
  }
  delete q;

  return QDialog::exec();
}


void EditSchedCode::okData()
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update SCHED_CODES set ")+
    "DESCRIPTION=\""+RDEscapeString(d_description_edit->text())+"\" "+
    "where CODE=\""+RDEscapeString(d_code_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  done(true);
}


void EditSchedCode::cancelData()
{
  done(false);
}

