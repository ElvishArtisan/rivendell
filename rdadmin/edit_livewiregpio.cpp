// edit_livewiregpio.cpp
//
// Edit a Rivendell Livewire GPIO Slot Association
//
//   (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlivewire.h>

#include "edit_livewiregpio.h"

EditLiveWireGpio::EditLiveWireGpio(int id,QWidget *parent)
  : QDialog(parent)
{
  edit_id=id;
  edit_slot=-1;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Edit GPIO Source"));

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // GPIO Lines
  //
  edit_gpiolines_label=new QLabel(this);
  edit_gpiolines_label->setFont(bold_font);
  edit_gpiolines_label->setAlignment(Qt::AlignCenter);

  //
  // Livewire Source Number
  //
  edit_source_number_spin=new QSpinBox(this);
  edit_source_number_spin->setRange(0,RD_LIVEWIRE_MAX_SOURCE);
  edit_source_number_spin->setSpecialValueText(tr("None"));
  edit_source_number_label=new QLabel(tr("Livewire Source: "),this);
  edit_source_number_label->setGeometry(10,32,120,20);
  edit_source_number_label->setFont(bold_font);
  edit_source_number_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  // Surface IP Address
  //
  edit_ip_address_edit=new QLineEdit(this);
  edit_ip_address_label=new QLabel(tr("Surface Address: "),this);
  edit_ip_address_label->setFont(bold_font);
  edit_ip_address_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

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
  QString sql=QString("select ")+
    "SLOT,"+
    "SOURCE_NUMBER,"+
    "IP_ADDRESS "+
    "from LIVEWIRE_GPIO_SLOTS where "+
    QString().sprintf("ID=%d",edit_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_gpiolines_label->
      setText(tr("GPIO Lines")+" "+RDLiveWire::gpioLineText(edit_slot));
    edit_slot=q->value(0).toInt();
    edit_source_number_spin->setValue(q->value(1).toInt());
    edit_ip_address_edit->setText(q->value(2).toString());
  }
  delete q;
}


QSize EditLiveWireGpio::sizeHint() const
{
  return QSize(270,142);
}


QSizePolicy EditLiveWireGpio::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditLiveWireGpio::okData()
{
  QHostAddress addr;

  addr.setAddress(edit_ip_address_edit->text());
  if(addr.isNull()&&(!edit_ip_address_edit->text().isEmpty())&&
     (edit_ip_address_edit->text()!="0.0.0.0")) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Invalid IP Address"),
			 tr("The IP address is invalid!"));
    return;
  }
  QString sql=QString("update LIVEWIRE_GPIO_SLOTS set ")+
    "IP_ADDRESS=\""+RDEscapeString(edit_ip_address_edit->text())+"\","+
    QString().sprintf("SOURCE_NUMBER=%d",edit_source_number_spin->value());
  RDSqlQuery::run(sql);

  done(0);
}


void EditLiveWireGpio::cancelData()
{
  done(-1);
}


void EditLiveWireGpio::resizeEvent(QResizeEvent *e)
{
  edit_gpiolines_label->setGeometry(10,10,size().width()-20,20);
  edit_source_number_spin->setGeometry(130,32,60,20);
  edit_ip_address_edit->setGeometry(130,54,120,20);
  edit_ip_address_label->setGeometry(10,54,115,20);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
