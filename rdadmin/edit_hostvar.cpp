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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>

#include <rdcatch_connect.h>

#include <edit_hostvar.h>
#include <edit_rdlibrary.h>
#include <edit_rdairplay.h>
#include <edit_decks.h>
#include <edit_audios.h>
#include <edit_ttys.h>
#include <list_matrices.h>
#include <list_hostvars.h>

EditHostvar::EditHostvar(QString station,QString var,QString *varvalue,
			 QString *remark,QWidget *parent)
  : QDialog(parent,"",true)
{
  edit_varvalue=varvalue;
  edit_remark=remark;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  setCaption(tr("Edit Host Variable"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Variable Name
  //
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setGeometry(125,11,120,19);
  edit_name_edit->setReadOnly(true);
  QLabel *label=new QLabel(edit_name_edit,tr("Variable Name:"),this);
  label->setGeometry(10,11,110,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Variable Value
  //
  edit_varvalue_edit=new QLineEdit(this);
  edit_varvalue_edit->setGeometry(125,33,sizeHint().width()-135,19);
  edit_varvalue_edit->setMaxLength(255);
  label=new QLabel(edit_varvalue_edit,tr("Variable Value:"),this);
  label->setGeometry(10,33,110,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Remark
  //
  edit_remark_edit=new QLineEdit(this);
  edit_remark_edit->setGeometry(125,55,sizeHint().width()-135,19);
  edit_remark_edit->setMaxLength(255);
  label=new QLabel(edit_remark_edit,tr("Remark:"),this);
  label->setGeometry(10,55,110,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

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
  // Load Values
  //
  edit_name_edit->setText(var);
  edit_varvalue_edit->setText(*varvalue);
  edit_remark_edit->setText(*remark);
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


void EditHostvar::okData()
{
  *edit_varvalue=edit_varvalue_edit->text();
  *edit_remark=edit_remark_edit->text();
  done(0);
}


void EditHostvar::cancelData()
{
  done(-1);
}


