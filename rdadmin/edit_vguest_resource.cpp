// edit_vguest_resource.cpp
//
// Edit a vGuest Resource Record.
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

#include <rdtextvalidator.h>

#include "edit_vguest_resource.h"

EditVguestResource::EditVguestResource(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Logitek Engine Number
  //
  edit_enginenum_edit=new QLineEdit(this);
  edit_enginenum_edit->setGeometry(135,10,50,20);
  QLabel *label=new QLabel(tr("Engine")+":",this);
  label->setGeometry(10,10,120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Device Number
  //
  edit_devicenum_edit=new QLineEdit(this);
  edit_devicenum_edit->setGeometry(135,36,50,20);
  label=new QLabel(tr("Device (Hex): "),this);
  label->setGeometry(10,36,120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Surface Number
  //
  edit_surfacenum_edit=new QLineEdit(this);
  edit_surfacenum_edit->setGeometry(135,62,50,20);
  label=new QLabel(tr("Surface")+":",this);
  label->setGeometry(10,62,120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Relay Number
  //
  edit_relaynum_edit=new QLineEdit(this);
  edit_relaynum_edit->setGeometry(135,88,50,20);
  edit_relaynum_label=new QLabel(tr("Bus/Relay")+":",this);
  edit_relaynum_label->setGeometry(10,88,120,20);
  edit_relaynum_label->setFont(labelFont());
  edit_relaynum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
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
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditVguestResource::sizeHint() const
{
  return QSize(200,190);
}


QSizePolicy EditVguestResource::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditVguestResource::exec(RDMatrix::VguestType type,unsigned id)
{
  edit_type=type;
  edit_id=id;

  QString sql=QString("select ")+
    "`ENGINE_NUM`,"+   // 00
    "`DEVICE_NUM`,"+   // 01
    "`SURFACE_NUM`,"+  // 02
    "`RELAY_NUM`,"+    // 03
    "`BUSS_NUM` "+     // 04
    "from `VGUEST_RESOURCES` where "+
    QString::asprintf("`ID`=%u",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toInt()>=0) {
      edit_enginenum_edit->
	setText(QString::asprintf("%d",q->value(0).toInt()));
    }
    if(q->value(1).toInt()>=0) {
      edit_devicenum_edit->
	setText(QString::asprintf("%04X",q->value(1).toInt()));
    }
    if(q->value(2).toInt()>=0) {
      edit_surfacenum_edit->
	setText(QString::asprintf("%d",q->value(2).toInt()));
    }
    switch(edit_type) {
    case RDMatrix::VguestTypeRelay:
      setWindowTitle("RDAdmin - "+tr("Edit vGuest Switch"));
      if(q->value(3).toInt()>=0) {
	edit_relaynum_edit->
	  setText(QString::asprintf("%d",q->value(3).toInt()));
      }
      break;

    case RDMatrix::VguestTypeDisplay:
      setWindowTitle("RDADmin - "+tr("Edit vGuest Display"));
      edit_relaynum_edit->setDisabled(true);
      break;

    case RDMatrix::VguestTypeNone:
      break;
    }
  }
  delete q;

  return QDialog::exec();
}


void EditVguestResource::okData()
{
  bool ok;
  int enginenum=edit_enginenum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_enginenum_edit->text().isEmpty()) {
      enginenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Engine Number is Invalid!"));
      return;
    }
  }
  int devicenum=edit_devicenum_edit->text().toInt(&ok,16);
  if(!ok) {
    if(edit_devicenum_edit->text().isEmpty()) {
      devicenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Device Number is Invalid!"));
      return;
    }
  }
  int surfacenum=edit_surfacenum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_surfacenum_edit->text().isEmpty()) {
      surfacenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Surface Number is Invalid!"));
      return;
    }
  }
  int relaynum=edit_relaynum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_relaynum_edit->text().isEmpty()) {
      relaynum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Bus/Relay Number is Invalid!"));
      return;
    }
  }
  QString sql=QString("update `VGUEST_RESOURCES` set ")+
    QString::asprintf("`ENGINE_NUM`=%d,",enginenum)+
    QString::asprintf("`DEVICE_NUM`=%d,",devicenum)+
    QString::asprintf("`SURFACE_NUM`=%d,",surfacenum)+
    QString::asprintf("`RELAY_NUM`=%d ",relaynum)+
    QString::asprintf("where `ID`=%u",edit_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditVguestResource::cancelData()
{
  done(false);
}
