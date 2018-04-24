// edit_sas_resource.cpp
//
// Edit an SAS Resource Record.
//
//   (C) Copyright 2002-2005,2011,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_sas_resource.h"

EditSasResource::EditSasResource(RDMatrix *matrix,int num,QWidget *parent)

  : QDialog(parent)
{
  edit_matrix=matrix;
  edit_number=num;

  edit_guest=new RDVguestResource(matrix,RDMatrix::VguestTypeRelay,num);

  setWindowTitle("RDAdmin - "+tr("Edit SAS Switch"));

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
  // Console Number
  //
  edit_enginenum_edit=new QLineEdit(this);
  edit_enginenum_label=new QLabel(edit_enginenum_edit,tr("Console Number")+":",this);
  edit_enginenum_label->setFont(bold_font);
  edit_enginenum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Source Number
  //
  edit_devicenum_edit=new QLineEdit(this);
  edit_devicenum_label=new QLabel(edit_devicenum_edit,tr("Source Number")+":",this);
  edit_devicenum_label->setFont(bold_font);
  edit_devicenum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Opto/Relay Number
  //
  edit_relaynum_edit=new QLineEdit(this);
  edit_relaynum_label=
    new QLabel(edit_relaynum_edit,tr("Opto/Relay Number")+":",this);
  edit_relaynum_label->setFont(bold_font);
  edit_relaynum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  if(edit_guest->engineNumber()>=0) {
    edit_enginenum_edit->
      setText(QString().sprintf("%d",edit_guest->engineNumber()));
  }
  if(edit_guest->deviceNumber()>=0) {
    edit_devicenum_edit->
      setText(QString().sprintf("%d",edit_guest->deviceNumber()));
  }
  if(edit_guest->relayNumber()>=0) {
    edit_relaynum_edit->
      setText(QString().sprintf("%d",edit_guest->relayNumber()));
  }
}


QSize EditSasResource::sizeHint() const
{
  return QSize(250,152);
}


QSizePolicy EditSasResource::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSasResource::okData()
{
  bool ok;
  int enginenum=edit_enginenum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_enginenum_edit->text().isEmpty()) {
      enginenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Console Number is Invalid!"));
      return;
    }
  }
  int devicenum=edit_devicenum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_devicenum_edit->text().isEmpty()) {
      devicenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Source Number is Invalid!"));
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
			   tr("The Opto/Relay Number is Invalid!"));
      return;
    }
  }
  edit_guest->setEngineNumber(enginenum);
  edit_guest->setDeviceNumber(devicenum);
  edit_guest->setRelayNumber(relaynum);

  done(0);
}


void EditSasResource::cancelData()
{
  done(1);
}


void EditSasResource::resizeEvent(QResizeEvent *e)
{
  edit_enginenum_edit->setGeometry(135,10,50,20);
  edit_enginenum_label->setGeometry(10,10,120,20);
  edit_devicenum_edit->setGeometry(135,36,50,20);
  edit_devicenum_label->setGeometry(10,36,120,20);
  edit_relaynum_edit->setGeometry(135,62,50,20);
  edit_relaynum_label->setGeometry(10,62,120,20);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
