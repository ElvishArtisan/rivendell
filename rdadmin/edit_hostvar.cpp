// edit_hostvar.cpp
//
// Edit a Rivendell Host Variable
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

#include <QPushButton>

#include <rdescape_string.h>

#include "edit_hostvar.h"

EditHostvar::EditHostvar(QWidget *parent)
  : RDDialog(parent)
{
  edit_id=-1;

  setWindowTitle("RDAdmin - "+tr("Edit Host Variable"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Variable Name
  //
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setGeometry(125,11,120,19);
  edit_name_edit->setReadOnly(true);
  QLabel *label=new QLabel(tr("Variable Name:"),this);
  label->setGeometry(10,11,110,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Variable Value
  //
  edit_varvalue_edit=new QLineEdit(this);
  edit_varvalue_edit->setGeometry(125,33,sizeHint().width()-135,19);
  edit_varvalue_edit->setMaxLength(255);
  label=new QLabel(tr("Variable Value:"),this);
  label->setGeometry(10,33,110,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Remark
  //
  edit_remark_edit=new QLineEdit(this);
  edit_remark_edit->setGeometry(125,55,sizeHint().width()-135,19);
  edit_remark_edit->setMaxLength(255);
  label=new QLabel(tr("Remark:"),this);
  label->setGeometry(10,55,110,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


EditHostvar::~EditHostvar()
{
}


QSize EditHostvar::sizeHint() const
{
  return QSize(385,150);
} 


QSizePolicy EditHostvar::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditHostvar::exec(int id)
{
  edit_id=id;

  QString sql=QString("select ")+
    "NAME,"+      // 00
    "VARVALUE,"+  // 01
    "REMARK "+    // 02
    "from HOSTVARS where "+
    QString().sprintf("ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_name_edit->setText(q->value(0).toString());
    edit_varvalue_edit->setText(q->value(1).toString());
    edit_remark_edit->setText(q->value(2).toString());
  }
  delete q;

  return QDialog::exec();
}


void EditHostvar::okData()
{
  QString sql=QString("update HOSTVARS set ")+
    "NAME=\""+RDEscapeString(edit_name_edit->text())+"\","+
    "VARVALUE=\""+RDEscapeString(edit_varvalue_edit->text())+"\","+
    "REMARK=\""+RDEscapeString(edit_remark_edit->text())+"\" "+
    QString().sprintf("where ID=%d",edit_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditHostvar::cancelData()
{
  done(false);
}
