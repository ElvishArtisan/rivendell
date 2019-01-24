// edit_pypad.cpp
//
// Edit a PyPAD Instance Configuration
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdescape_string.h>

#include "edit_pypad.h"

EditPypad::EditPypad(int id,QWidget *parent)
  : QDialog(parent)
{
  edit_id=id;

  setMinimumSize(sizeHint());
  setWindowTitle(tr("Edit PyPAD Instance")+
		 " ["+tr("ID")+QString().sprintf(": %u]",id));

  //
  // Fonts
  //
  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Script Path
  //
  edit_script_path_label=new QLabel(tr("Script Path")+":",this);
  edit_script_path_label->setFont(label_font);
  edit_script_path_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_script_path_edit=new QLineEdit(this);
  edit_script_path_edit->setReadOnly(true);

  //
  // Description
  //
  edit_description_label=new QLabel(tr("Description")+":",this);
  edit_description_label->setFont(label_font);
  edit_description_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_description_edit=new QLineEdit(this);

  //
  // Configuration
  //
  edit_config_label=new QLabel(tr("Configuration"),this);
  edit_config_label->setFont(label_font);
  edit_config_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit_config_text=new QTextEdit(this);
  edit_config_text->setAcceptRichText(false);
  edit_config_text->setWordWrapMode(QTextOption::WrapAnywhere);

  //
  // OK Button
  //
  edit_ok_button=new QPushButton(tr("OK"),this);
  edit_ok_button->setFont(label_font);
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  edit_cancel_button=new QPushButton(tr("Cancel"),this);
  edit_cancel_button->setFont(label_font);
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  QString sql=QString("select ")+
    "SCRIPT_PATH,"+  // 00
    "DESCRIPTION,"+  // 01
    "CONFIG "+       // 02
    "from PYPAD_INSTANCES where "+
    QString().sprintf("ID=%u",edit_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_script_path_edit->setText(q->value(0).toString());
    edit_description_edit->setText(q->value(1).toString());
    edit_config_text->insertPlainText(q->value(2).toString());
    edit_config_text->moveCursor(QTextCursor::Start);
  }
  delete q;
}


QSize EditPypad::sizeHint() const
{
  return QSize(600,738);
}


QSizePolicy EditPypad::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditPypad::okData()
{
  QString sql=QString("update PYPAD_INSTANCES set ")+
    "DESCRIPTION=\""+RDEscapeString(edit_description_edit->text())+"\","+
    "CONFIG=\""+RDEscapeString(edit_config_text->toPlainText())+"\" where "+
    QString().sprintf("ID=%u",edit_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditPypad::cancelData()
{
  done(false);
}
  

void EditPypad::resizeEvent(QResizeEvent *e)
{
  edit_script_path_label->setGeometry(10,10,100,20);
  edit_script_path_edit->setGeometry(115,10,size().width()-135,20);

  edit_description_label->setGeometry(10,32,100,20);
  edit_description_edit->setGeometry(115,32,size().width()-135,20);

  edit_config_label->setGeometry(10,54,150,20);
  edit_config_text->setGeometry(10,76,size().width()-20,size().height()-146);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);

  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
