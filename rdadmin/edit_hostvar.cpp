// edit_hostvar.cpp
//
// Edit a Rivendell Host Variable
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

#include "edit_hostvar.h"

EditHostvar::EditHostvar(int id,QWidget *parent)
  : QDialog(parent)
{
  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  setWindowTitle("RDAdmin - "+tr("Edit Host Variable"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  edit_hostvar=new RDHostVariable(id);

  //
  // Variable Name
  //
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setReadOnly(true);
  edit_name_label=new QLabel(edit_name_edit,tr("Variable Name")+":",this);
  edit_name_label->setFont(font);
  edit_name_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Variable Value
  //
  edit_varvalue_edit=new QLineEdit(this);
  edit_varvalue_edit->setMaxLength(255);
  edit_varvalue_label=
    new QLabel(edit_varvalue_edit,tr("Variable Value")+":",this);
  edit_varvalue_label->setFont(font);
  edit_varvalue_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Remark
  //
  edit_remark_edit=new QLineEdit(this);
  edit_remark_edit->setMaxLength(255);
  edit_remark_label=new QLabel(edit_remark_edit,tr("Remark")+":",this);
  edit_remark_label->setFont(font);
  edit_remark_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  edit_name_edit->setText(edit_hostvar->name());
  edit_varvalue_edit->setText(edit_hostvar->value());
  edit_remark_edit->setText(edit_hostvar->remarks());
}


EditHostvar::~EditHostvar()
{
  delete edit_hostvar;
}


QSize EditHostvar::sizeHint() const
{
  return QSize(385,150);
} 


QSizePolicy EditHostvar::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditHostvar::okData()
{
  edit_hostvar->setValue(edit_varvalue_edit->text());
  edit_hostvar->setRemarks(edit_remark_edit->text());
  done(0);
}


void EditHostvar::cancelData()
{
  done(-1);
}


void EditHostvar::resizeEvent(QResizeEvent *e)
{
  edit_name_label->setGeometry(10,11,110,19);
  edit_name_edit->setGeometry(125,11,size().width()-135,19);
  edit_varvalue_label->setGeometry(10,33,110,19);
  edit_varvalue_edit->setGeometry(125,33,size().width()-135,19);
  edit_remark_label->setGeometry(10,55,110,19);
  edit_remark_edit->setGeometry(125,55,size().width()-135,19);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
