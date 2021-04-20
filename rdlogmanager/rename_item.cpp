// rename_item.cpp
//
// Edit a Rivendell LogManager Note
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

#include <QMessageBox>

#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "rename_item.h"

RenameItem::RenameItem(QString *text,QString table,QWidget *parent)
  : RDDialog(parent)
{
  edit_text=text;
  edit_tablename=table;

  setWindowTitle("RDLogManager - "+tr("Rename"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Validators
  //
  RDTextValidator *validator=new RDTextValidator();
  validator->addBannedChar('(');
  validator->addBannedChar(')');
  validator->addBannedChar('!');
  validator->addBannedChar('@');
  validator->addBannedChar('#');
  validator->addBannedChar('$');
  validator->addBannedChar('%');
  validator->addBannedChar('^');
  validator->addBannedChar('&');
  validator->addBannedChar('*');
  validator->addBannedChar('{');
  validator->addBannedChar('}');
  validator->addBannedChar('[');
  validator->addBannedChar(']');
  validator->addBannedChar(':');
  validator->addBannedChar(';');
  validator->addBannedChar(34);
  validator->addBannedChar('<');
  validator->addBannedChar('>');
  validator->addBannedChar('.');
  validator->addBannedChar(',');
  validator->addBannedChar('\\');
  validator->addBannedChar('-');
  validator->addBannedChar('_');
  validator->addBannedChar('/');

  //
  // The Name Editor
  //
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setGeometry(10,10,sizeHint().width()-20,20);
  edit_name_edit->setValidator(validator);
  edit_name_edit->setText(*text);

  //
  //  OK Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize RenameItem::sizeHint() const
{
  return QSize(400,100);
} 


QSizePolicy RenameItem::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RenameItem::okData()
{
  if(edit_name_edit->text()==*edit_text) {
    done(-1);
    return;
  }
  QString sql=QString("select ")+
    "`NAME` "+
    "from `"+edit_tablename+"` where "+
    "`NAME`='"+RDEscapeString(edit_name_edit->text())+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->next()) {
    delete q;
    QMessageBox::warning(this,tr("Name Conflict"),
			 tr("That name already exists!"));
    return;
  }
  delete q;
  *edit_text=edit_name_edit->text();
  done(0);
}


void RenameItem::cancelData()
{
  done(-1);
}
