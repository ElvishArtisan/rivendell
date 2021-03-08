// add_station.cpp
//
// Add a Rivendell Workstation
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

#include <rddb.h>
#include <rdairplay_conf.h>
#include <rdescape_string.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include "add_station.h"
#include "edit_station.h"

AddStation::AddStation(QString *stationname,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  add_name=stationname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Add Host"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Station Name
  //
  add_name_edit=new QLineEdit(this);
  add_name_edit->setGeometry(130,10,sizeHint().width()-140,19);
  add_name_edit->setMaxLength(64);
  add_name_edit->setValidator(validator);
  QLabel *label=new QLabel(tr("New Host Name:"),this);
  label->setGeometry(10,10,115,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Exemplar
  //
  add_exemplar_box=new QComboBox(this);
  add_exemplar_box->setGeometry(130,35,sizeHint().width()-140,19);
  label=new QLabel(tr("Base Host On:"),this);
  label->setGeometry(10,35,115,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
			 80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Fill Exemplar List
  //
  add_exemplar_box->insertItem(0,tr("Empty Host Config"));
  QString sql="select NAME from STATIONS";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    add_exemplar_box->
      insertItem(add_exemplar_box->count(),q->value(0).toString());
  }
  delete q;
}


AddStation::~AddStation() 
{
  delete add_name_edit;
}


QSize AddStation::sizeHint() const
{
  return QSize(380,130);
} 


QSizePolicy AddStation::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddStation::okData()
{
  QString err_msg;
  bool ok=false;

  if(add_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),
			 tr("You must give the host a name!"));
    return;
  }

  if(add_exemplar_box->currentIndex()==0) {  // Create Blank Host Config
    ok=RDStation::create(add_name_edit->text(),&err_msg);
  }
  else {
    ok=RDStation::create(add_name_edit->text(),&err_msg,add_exemplar_box->currentText());
  }
  if(!ok) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Unable to create host!")+"\n"+err_msg);
    return;
  }

  *add_name=add_name_edit->text();

  EditStation *station=new EditStation(add_name_edit->text(),this);
  int res=station->exec();
  delete station;

  done(res);
}


void AddStation::cancelData()
{
  done(-1);
}


void AddStation::CloneEncoderValues(const QString &paramname,
				    int src_id,int dest_id)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

sql=QString("select ")+
  paramname+" from `ENCODER_"+paramname+"` where "+
  QString().sprintf("ENCODER_ID=%d",src_id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `ENCODER_")+
      paramname+"` set "+paramname+
      QString().sprintf("=%d,ENCODER_ID=%d",q->value(0).toInt(),dest_id);
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}
