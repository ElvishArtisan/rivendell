// edit_schedcodes.cpp
//
// Edit scheduler codes dialog
//
//   by Stefan Gabriel <stg@st-gabriel.de>
//   Changes for Qt4 (C) 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QPushButton>

#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_group.h"
#include "edit_schedcodes.h"

EditSchedCode::EditSchedCode(QString schedcode,QString description,
			     QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  schedcode_code=new QString(schedcode);
  schedcode_description=new QString(description);
  
  setWindowTitle("RDAdmin - "+tr("Scheduler Code: ")+schedcode);

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
  schedcode_name_edit=new QLineEdit(this);
  schedcode_name_edit->setGeometry(125,11,100,19);
  schedcode_name_edit->setMaxLength(10);
  schedcode_name_edit->setReadOnly(true);
  QLabel *schedcode_name_label=
    new QLabel(schedcode_name_edit,tr("Scheduler Code:"),this);
  schedcode_name_label->setGeometry(10,11,110,19);
  schedcode_name_label->setFont(font);
  schedcode_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Code Description
  //
  schedcode_description_edit=new QLineEdit(this);
  schedcode_description_edit->setGeometry(125,32,sizeHint().width()-135,19);
  schedcode_description_edit->setMaxLength(255);
  schedcode_description_edit->setValidator(validator);
  QLabel *schedcode_description_label=
    new QLabel(schedcode_description_edit,tr("Code Description:"),this);
  schedcode_description_label->setGeometry(10,32,110,19);
  schedcode_description_label->setFont(font);
  schedcode_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

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
  schedcode_name_edit->setText(*schedcode_code);
  schedcode_description_edit->setText(*schedcode_description);
}


EditSchedCode::~EditSchedCode()
{
  delete schedcode_name_edit;
  delete schedcode_description_edit;
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
    "DESCRIPTION=\""+RDEscapeString(schedcode_description_edit->text())+"\" "+
    "where CODE=\""+RDEscapeString(schedcode_name_edit->text())+"\"";

  q=new RDSqlQuery(sql);
  delete q;

  *schedcode_description=schedcode_description_edit->text();
  done(0);
}


void EditSchedCode::cancelData()
{
  done(-1);
}

