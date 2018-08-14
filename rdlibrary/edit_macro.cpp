// edit_macro.cpp
//
// Edit a Rivendell Macro
//
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
#include <qlabel.h>
#include <qsqldatabase.h>

#include <rdtextvalidator.h>

#include "edit_macro.h"

EditMacro::EditMacro(RDMacro *cmd,bool highlight,QWidget *parent)
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

  //
  // Generate Fonts
  //
  QFont button_font("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Macro
  //
  edit_macro=cmd;

  edit_macro_edit=new QLineEdit(this);
  edit_macro_edit->setMaxLength(RD_RML_MAX_LENGTH-1);
  edit_macro_edit->setValidator(validator);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(button_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(button_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  edit_macro_edit->setText(edit_macro->toString());
  if(highlight) {
    edit_macro_edit->selectAll();
  }
}


QSize EditMacro::sizeHint() const
{
  return QSize(400,110);
} 


QSizePolicy EditMacro::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditMacro::okData()
{
  *edit_macro=RDMacro::fromString(edit_macro_edit->text());
  done(0);
}


void EditMacro::cancelData()
{
  done(-1);
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
