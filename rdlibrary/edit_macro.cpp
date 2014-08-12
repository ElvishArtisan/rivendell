// edit_macro.cpp
//
// Edit a Rivendell Macro
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_macro.cpp,v 1.11.8.2 2013/12/23 22:04:02 cvs Exp $
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

#include <edit_macro.h>


EditMacro::EditMacro(RDMacro *cmd,bool highlight,
		     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
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

  setCaption(tr("Edit Macro"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Macro
  //
  edit_macro=cmd;

  edit_macro_edit=new QLineEdit(this,"edit_macro_edit");
  edit_macro_edit->setMaxLength(RD_RML_MAX_LENGTH-1);
  edit_macro_edit->setValidator(validator);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this,"ok_button");
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(button_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this,"cancel_button");
  edit_cancel_button->setFont(button_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  char cmdstr[RD_RML_MAX_LENGTH];
  edit_macro->generateString(cmdstr,RD_RML_MAX_LENGTH);
  edit_macro_edit->setText(cmdstr);
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
  edit_macro->parseString((const char *)edit_macro_edit->text(),
			  edit_macro_edit->text().length());
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
