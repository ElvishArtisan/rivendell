// edit_livewiregpio.cpp
//
// Edit a Rivendell Livewire GPIO Slot Association
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_livewiregpio.cpp,v 1.1.2.3 2013/03/06 13:28:59 cvs Exp $
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

#include <edit_livewiregpio.h>

EditLiveWireGpio::EditLiveWireGpio(int slot,int *source,QHostAddress *addr,
				   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_slot=slot;
  edit_source=source;
  edit_address=addr;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
  setCaption(tr("Edit GPIO Source"));

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
  QLabel *label=new QLabel(tr("GPIO Lines")+
			QString().sprintf(" %d - %d",5*slot+1,5*slot+5),this);
  label->setGeometry(10,10,sizeHint().width()-20,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignCenter);

  //
  // Livewire Source Number
  //
  edit_source_number_spin=new QSpinBox(this);
  edit_source_number_spin->setGeometry(130,32,60,20);
  edit_source_number_spin->setRange(0,RD_LIVEWIRE_MAX_SOURCE);
  edit_source_number_spin->setSpecialValueText(tr("None"));
  label=new QLabel(tr("Livewire Source: "),this);
  label->setGeometry(10,32,115,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  // Surface IP Address
  //
  edit_ip_address_edit=new QLineEdit(this);
  edit_ip_address_edit->setGeometry(130,54,120,20);
  label=new QLabel(tr("Surface Address: "),this);
  label->setGeometry(10,54,115,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

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
  edit_source_number_spin->setValue(*edit_source);
  if(!edit_address->isNull()) {
    edit_ip_address_edit->setText(edit_address->toString());
  }
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
  *edit_source=edit_source_number_spin->value();
  edit_address->setAddress(addr.toString());

  done(0);
}


void EditLiveWireGpio::cancelData()
{
  done(-1);
}
