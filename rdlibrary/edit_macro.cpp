// edit_macro.cpp
//
// Edit a Rivendell Macro
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

#include "edit_macro.h"

EditMacro::EditMacro(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());
  setWindowTitle("RDLibrary - Edit Macro");

  //
  // Macro
  //
  edit_macro_edit=new QLineEdit(this);
  edit_macro_edit->setMaxLength(RD_RML_MAX_LENGTH-1);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditMacro::sizeHint() const
{
  return QSize(400,110);
} 


QSizePolicy EditMacro::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditMacro::exec(QString *code,bool highlight)
{
  edit_code=code;

  edit_macro_edit->setText(*code);
  if(highlight) {
    edit_macro_edit->selectAll();
  }

  return QDialog::exec();
}


void EditMacro::okData()
{
  *edit_code=edit_macro_edit->text();
  done(true);
}


void EditMacro::cancelData()
{
  done(false);
}


void EditMacro::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditMacro::resizeEvent(QResizeEvent *e)
{
  edit_macro_edit->setGeometry(10,11,size().width()-20,19);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
