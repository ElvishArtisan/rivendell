// edit_livewiregpio.cpp
//
// Edit a Rivendell Livewire GPIO Slot Association
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <qlabel.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdescape_string.h>

#include "edit_livewiregpio.h"

EditLiveWireGpio::EditLiveWireGpio(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
  setWindowTitle("RDAdmin - "+tr("Edit GPIO Source"));

  //
  // GPIO Lines
  //
  edit_title_label=new QLabel(this);
  edit_title_label->setGeometry(10,10,sizeHint().width()-20,20);
  edit_title_label->setFont(labelFont());
  edit_title_label->setAlignment(Qt::AlignCenter);

  //
  // Livewire Source Number
  //
  edit_source_number_spin=new QSpinBox(this);
  edit_source_number_spin->setGeometry(130,32,60,20);
  edit_source_number_spin->setRange(0,RD_LIVEWIRE_MAX_SOURCE);
  edit_source_number_spin->setSpecialValueText(tr("None"));
  QLabel *label=new QLabel(tr("Livewire Source: "),this);
  label->setGeometry(10,32,115,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  // Surface IP Address
  //
  edit_ip_address_edit=new QLineEdit(this);
  edit_ip_address_edit->setGeometry(130,54,120,20);
  label=new QLabel(tr("Surface Address: "),this);
  label->setGeometry(10,54,115,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditLiveWireGpio::sizeHint() const
{
  return QSize(270,142);
}


QSizePolicy EditLiveWireGpio::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditLiveWireGpio::exec(int slot_id)
{
  QHostAddress addr;

  edit_id=slot_id;

  QString sql=QString("select ")+
    "SLOT,"+           // 00
    "SOURCE_NUMBER,"+  // 01
    "IP_ADDRESS "+     // 02
    "from LIVEWIRE_GPIO_SLOTS where "+
    QString().sprintf("ID=%u",slot_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_title_label->
      setText(tr("GPIO Lines")+
	      QString().sprintf(" %d - %d",
			5*q->value(0).toInt()+1,5*q->value(0).toInt()+5));
    edit_source_number_spin->setValue(q->value(1).toInt());
    if(addr.setAddress(q->value(2).toString())) {
      edit_ip_address_edit->setText(addr.toString());
    }
    else {
      edit_ip_address_edit->setText("");
    }
  }

  return QDialog::exec();
}


void EditLiveWireGpio::okData()
{
  QHostAddress addr;

  addr.setAddress(edit_ip_address_edit->text());
  if(addr.isNull()&&(!edit_ip_address_edit->text().isEmpty())&&
     (edit_ip_address_edit->text()!="0.0.0.0")) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Invalid IP Address"),
			 tr("The Surface Address is invalid!"));
    return;
  }
  QString sql=QString("update LIVEWIRE_GPIO_SLOTS set ")+
    QString().sprintf("SOURCE_NUMBER=%d,",edit_source_number_spin->value())+
    "IP_ADDRESS=\""+RDEscapeString(addr.toString())+"\" "+
    QString().sprintf("where ID=%u",edit_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditLiveWireGpio::cancelData()
{
  done(false);
}
