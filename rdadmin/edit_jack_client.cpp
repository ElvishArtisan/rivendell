// edit_jack_client.cpp
//
// Edit a Rivendell Jack Client Configuration
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <math.h>

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_jack_client.h"
#include "globals.h"

EditJackClient::EditJackClient(RDStation *station,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;

  edit_station=station;
  edit_id=0;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("JACK Client Configuration for ")+
		 edit_station->name());

  //
  // JACK Client Description
  //
  edit_jack_description_edit=new QLineEdit(this);
  edit_jack_description_label=new QLabel(tr("Description:"),this);
  edit_jack_description_label->setFont(labelFont());
  edit_jack_description_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // JACK Client Description
  //
  edit_jack_command_line_edit=new QLineEdit(this);
  edit_jack_command_line_label=new QLabel(tr("Command Line:"),this);
  edit_jack_command_line_label->setFont(labelFont());
  edit_jack_command_line_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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


QSize EditJackClient::sizeHint() const
{
  return QSize(450,130);
} 


QSizePolicy EditJackClient::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditJackClient::exec(unsigned id)
{
  QString sql;
  RDSqlQuery *q=NULL;

  edit_id=id;

  sql=QString("select ")+
    "`DESCRIPTION`,"    // 00
    "`COMMAND_LINE` "+  // 01
    "from `JACK_CLIENTS` where "+
    QString::asprintf("`ID`=%u",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_jack_description_edit->setText(q->value(0).toString());
    edit_jack_command_line_edit->setText(q->value(1).toString());
  }
  delete q;

  return QDialog::exec();
}


void EditJackClient::okData()
{
  QString sql;

  sql=QString("update `JACK_CLIENTS` set ")+
    "`DESCRIPTION`='"+RDEscapeString(edit_jack_description_edit->text())+"',"+
    "`COMMAND_LINE`='"+
    RDEscapeString(edit_jack_command_line_edit->text().trimmed())+"' "+
    "where "+
    QString::asprintf("`ID`=%u",edit_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditJackClient::cancelData()
{
  done(false);
}


void EditJackClient::resizeEvent(QResizeEvent *e)
{
  edit_jack_description_label->setGeometry(10,10,130,20);
  edit_jack_description_edit->setGeometry(145,10,size().width()-155,20);

  edit_jack_command_line_label->setGeometry(10,32,130,20);
  edit_jack_command_line_edit->setGeometry(145,32,size().width()-155,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
