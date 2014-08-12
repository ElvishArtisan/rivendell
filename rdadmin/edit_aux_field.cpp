// edit_aux_field.cpp
//
// Edit an Auxiliary Field for an RSS Feed
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_aux_field.cpp,v 1.5 2010/07/29 19:32:34 cvs Exp $
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

#include <qmessagebox.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <edit_aux_field.h>


EditAuxField::EditAuxField(unsigned feed_id,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;

  edit_field_id=feed_id;
  setCaption(tr("Edit Auxiliary Metadata Fields"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

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
  edit_varname_edit=new QLineEdit(this,"edit_varname_edit");
  edit_varname_edit->setGeometry(120,10,130,20);
  edit_varname_edit->setReadOnly(true);
  QLabel *label=
    new QLabel(edit_varname_edit,tr("Variable Name: "),
	       this,"edit_varname_label");
  label->setGeometry(10,13,105,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight);

  //
  // Variable Name
  //
  edit_caption_edit=new QLineEdit(this,"edit_caption_edit");
  edit_caption_edit->setGeometry(120,37,sizeHint().width()-130,20);
  edit_caption_edit->setMaxLength(64);
  label=new QLabel(edit_caption_edit,tr("Caption: "),
		   this,"edit_caption_label");
  label->setGeometry(10,37,105,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  sql=QString().sprintf("select VAR_NAME,CAPTION from AUX_METADATA \
                         where ID=%u",edit_field_id);
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

  sql=QString().sprintf("update AUX_METADATA set CAPTION=\"%s\" \
                         where ID=%u",
		       (const char *)RDEscapeString(edit_caption_edit->text()),
		       edit_field_id);
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditAuxField::cancelData()
{
  done(-1);
}
