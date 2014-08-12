// edit_jack_client.cpp
//
// Edit a Rivendell Jack Client Configuration
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_jack_client.cpp,v 1.1.2.1 2012/11/14 02:24:23 cvs Exp $
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

#include <globals.h>
#include <rddb.h>
#include <edit_jack_client.h>

EditJackClient::EditJackClient(RDStation *station,
			       QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;

  edit_station=station;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("JACK Client Configuration for ")+edit_station->name());

  //
  // Create Fonts
  //
  QFont normal_font=QFont("Helvetica",12,QFont::Normal);
  normal_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // JACK Client Description
  //
  edit_jack_description_edit=new QLineEdit(this);
  edit_jack_description_label=
    new QLabel(edit_jack_description_edit,tr("Description:"),this);
  edit_jack_description_label->setFont(font);
  edit_jack_description_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // JACK Client Description
  //
  edit_jack_command_line_edit=new QLineEdit(this);
  edit_jack_command_line_label=
    new QLabel(edit_jack_command_line_edit,tr("Command Line:"),this);
  edit_jack_command_line_label->setFont(font);
  edit_jack_command_line_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this,"edit_ok_button");
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this,"edit_cancel_button");
  edit_cancel_button->setFont(font);
  edit_cancel_button->setText(tr("&Cancel"));
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


int EditJackClient::exec(QString *desc,QString *cmd)
{
  edit_description=desc;
  edit_jack_description_edit->setText(*desc);
  edit_command_line=cmd;
  edit_jack_command_line_edit->setText(*cmd);
  return QDialog::exec();
}


void EditJackClient::okData()
{
  *edit_description=edit_jack_description_edit->text();
  *edit_command_line=edit_jack_command_line_edit->text();
  done(0);
}


void EditJackClient::cancelData()
{
  done(-1);
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
