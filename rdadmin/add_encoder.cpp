// add_encoder.cpp
//
// Add a Rivendell Encoder
//
//   (C) Copyright 2002,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <qlabel.h>
#include <qmessagebox.h>
#include <qdatetime.h>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdcreateauxfieldstable.h>

#include <edit_encoder.h>
#include <add_encoder.h>

AddEncoder::AddEncoder(QString *encname,const QString &stationname,
		       QWidget *parent)
  : QDialog(parent,"",true)
{
  encoder_name=encname;
  encoder_stationname=stationname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Encoder"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont user_font=QFont("Helvetica",12,QFont::Normal);
  user_font.setPixelSize(12);

  //
  // Encoder Name
  //
  encoder_name_edit=new QLineEdit(this);
  encoder_name_edit->setGeometry(145,11,sizeHint().width()-150,19);
  encoder_name_edit->setMaxLength(32);
  QLabel *label=new QLabel(encoder_name_edit,tr("&New Encoder Name:"),this);
  label->setGeometry(10,11,130,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

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
}


AddEncoder::~AddEncoder()
{
  delete encoder_name_edit;
}


QSize AddEncoder::sizeHint() const
{
  return QSize(250,124);
} 


QSizePolicy AddEncoder::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddEncoder::okData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select NAME from ENCODERS where ")+
    "(NAME=\""+RDEscapeString(encoder_name_edit->text())+"\")&&"+
    "(STATION_NAME=\""+RDEscapeString(encoder_stationname)+"\")";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::warning(this,tr("Add Encoder Error"),
			 tr("A encoder with that name already exists!"));
    delete q;
    return;
  }
  delete q;

  //
  // Create Encoder
  //
  sql=QString("insert into ENCODERS set ")+
    "NAME=\""+RDEscapeString(encoder_name_edit->text())+"\""+
    "STATION_NAME=\""+RDEscapeString(encoder_stationname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  *encoder_name=encoder_name_edit->text();

  done(0);
}


void AddEncoder::cancelData()
{
  done(-1);
}
