// edit_vguest_resource.cpp
//
// Edit a vGuest Resource Record.
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_vguest_resource.h"

EditVguestResource::EditVguestResource(RDMatrix *matrix,
				       RDMatrix::VguestType type,int num,
				       QWidget *parent)
  : QDialog(parent)
{
  edit_matrix=matrix;
  edit_type=type;
  edit_number=num;

  edit_guest=new RDVguestResource(matrix,type,num);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Logitek Engine Number
  //
  edit_enginenum_edit=new QLineEdit(this);
  edit_enginenum_edit->setGeometry(135,10,50,20);
  QLabel *label=new QLabel(edit_enginenum_edit,tr("Engine")+":",this);
  label->setGeometry(10,10,120,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Device Number
  //
  edit_devicenum_edit=new QLineEdit(this);
  edit_devicenum_edit->setGeometry(135,36,50,20);
  label=new QLabel(edit_devicenum_edit,tr("Device (Hex)")+":",this);
  label->setGeometry(10,36,120,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Surface Number
  //
  edit_surfacenum_edit=new QLineEdit(this);
  edit_surfacenum_edit->setGeometry(135,62,50,20);
  label=new QLabel(edit_surfacenum_edit,tr("Surface")+":",this);
  label->setGeometry(10,62,120,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Relay Number
  //
  edit_relaynum_edit=new QLineEdit(this);
  edit_relaynum_edit->setGeometry(135,88,50,20);
  edit_relaynum_label=
    new QLabel(edit_relaynum_edit,tr("Bus/Relay")+":",this);
  edit_relaynum_label->setGeometry(10,88,120,20);
  edit_relaynum_label->setFont(bold_font);
  edit_relaynum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  if(edit_guest->engineNumber()>=0) {
    edit_enginenum_edit->
      setText(QString().sprintf("%d",edit_guest->engineNumber()));
  }
  if(edit_guest->deviceNumber()>=0) {
    edit_devicenum_edit->
      setText(QString().sprintf("%04X",edit_guest->deviceNumber()));
  }
  if(edit_guest->surfaceNumber()>=0) {
    edit_surfacenum_edit->
      setText(QString().sprintf("%d",edit_guest->surfaceNumber()));
  }
  switch(edit_guest->type()) {
  case RDMatrix::VguestTypeRelay:
    setWindowTitle("RDAdmin - "+tr("Edit vGuest Switch"));
    if(edit_guest->relayNumber()>=0) {
      edit_relaynum_edit->
	setText(QString().sprintf("%d",edit_guest->relayNumber()));
    }
    break;

  case RDMatrix::VguestTypeDisplay:
    setWindowTitle("RDAdmin - "+tr("Edit vGuest Display"));
    edit_relaynum_edit->setDisabled(true);
  }
}


QSize EditVguestResource::sizeHint() const
{
  return QSize(400,174);
}


QSizePolicy EditVguestResource::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditVguestResource::okData()
{
  bool ok;
  int enginenum=edit_enginenum_edit->text().toInt(&ok,16);
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
  int surfacenum=edit_surfacenum_edit->text().toInt(&ok,16);
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
  int relaynum=edit_relaynum_edit->text().toInt(&ok,16);
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
  edit_guest->setEngineNumber(enginenum);
  edit_guest->setDeviceNumber(devicenum);
  edit_guest->setSurfaceNumber(surfacenum);
  edit_guest->setRelayNumber(relaynum);

  done(0);
}


void EditVguestResource::cancelData()
{
  done(1);
}
