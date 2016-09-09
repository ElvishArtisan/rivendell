// edit_aux_field.cpp
//
// Edit an Auxiliary Field for an RSS Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_aux_field.h"

EditAuxField::EditAuxField(unsigned feed_id,QWidget *parent)
  : QDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

  edit_field_id=feed_id;

  setWindowTitle("RDAdmin - "+tr("Edit Auxiliary Metadata Fields"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Variable Name
  //
  edit_varname_edit=new QLineEdit(this);
  edit_varname_edit->setReadOnly(true);
  edit_varname_label=new QLabel(edit_varname_edit,tr("Variable Name: "),this);
  edit_varname_label->setFont(bold_font);
  edit_varname_label->setAlignment(Qt::AlignRight);

  //
  // Variable Name
  //
  edit_caption_edit=new QLineEdit(this);
  edit_caption_edit->setMaxLength(64);
  edit_caption_label=new QLabel(edit_caption_edit,tr("Caption: "),this);
  edit_caption_label->setFont(bold_font);
  edit_caption_label->setAlignment(Qt::AlignRight);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(bold_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(bold_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  sql=QString("select VAR_NAME,CAPTION from AUX_METADATA where ")+
    QString().sprintf("ID=%u",edit_field_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_varname_edit->setText(q->value(0).toString());
    edit_caption_edit->setText(q->value(1).toString());
  }
  delete q;
}


QSize EditAuxField::sizeHint() const
{
  return QSize(400,127);
}


QSizePolicy EditAuxField::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditAuxField::okData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update AUX_METADATA set ")+
    "CAPTION=\""+RDEscapeString(edit_caption_edit->text())+"\" where "+
    QString().sprintf("ID=%u",edit_field_id);
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditAuxField::cancelData()
{
  done(-1);
}


void EditAuxField::resizeEvent(QResizeEvent *e)
{
  edit_varname_edit->setGeometry(120,10,130,20);
  edit_varname_label->setGeometry(10,13,105,20);
  edit_caption_edit->setGeometry(120,37,size().width()-130,20);
  edit_caption_label->setGeometry(10,37,105,20);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
