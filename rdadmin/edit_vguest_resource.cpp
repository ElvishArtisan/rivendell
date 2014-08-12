// edit_vguest_resource.cpp
//
// Edit a vGuest Resource Record.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_vguest_resource.cpp,v 1.5 2010/07/29 19:32:34 cvs Exp $
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

#include <qmessagebox.h>

#include <rdtextvalidator.h>

#include <edit_vguest_resource.h>


EditVguestResource::EditVguestResource(RDMatrix::VguestType type,
				       int *enginenum,int *devicenum,
				       int *surfacenum,int *relaynum,
				       QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_type=type;
  edit_enginenum=enginenum;
  edit_devicenum=devicenum;
  edit_surfacenum=surfacenum;
  edit_relaynum=relaynum;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

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
  edit_enginenum_edit=new QLineEdit(this,"edit_enginenum_edit");
  edit_enginenum_edit->setGeometry(135,10,50,20);
  QLabel *label=new QLabel(edit_enginenum_edit,tr("Engine (Hex): "),
			   this,"edit_enginenum_label");
  label->setGeometry(10,10,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Logitek Device Number
  //
  edit_devicenum_edit=new QLineEdit(this,"edit_devicenum_edit");
  edit_devicenum_edit->setGeometry(135,36,50,20);
  label=new QLabel(edit_devicenum_edit,tr("Device (Hex): "),
		   this,"edit_devicenum_label");
  label->setGeometry(10,36,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Logitek Surface Number
  //
  edit_surfacenum_edit=new QLineEdit(this,"edit_surfacenum_edit");
  edit_surfacenum_edit->setGeometry(135,62,50,20);
  label=new QLabel(edit_surfacenum_edit,tr("Surface (Hex): "),
		   this,"edit_surfacenum_label");
  label->setGeometry(10,62,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Logitek Relay Number
  //
  edit_relaynum_edit=new QLineEdit(this,"edit_relaynum_edit");
  edit_relaynum_edit->setGeometry(135,88,50,20);
  edit_relaynum_label=new QLabel(edit_relaynum_edit,tr("Bus/Relay (Hex): "),
		   this,"edit_relaynum_label");
  edit_relaynum_label->setGeometry(10,88,120,20);
  edit_relaynum_label->setFont(bold_font);
  edit_relaynum_label->setAlignment(AlignRight|AlignVCenter);

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
  if(*enginenum>=0) {
    edit_enginenum_edit->setText(QString().sprintf("%04X",*enginenum));
  }
  if(*devicenum>=0) {
    edit_devicenum_edit->setText(QString().sprintf("%04X",*devicenum));
  }
  if(*surfacenum>=0) {
    edit_surfacenum_edit->setText(QString().sprintf("%04X",*surfacenum));
  }
  switch(edit_type) {
      case RDMatrix::VguestTypeRelay:
	setCaption(tr("Edit vGuest Switch"));
	if(*relaynum>=0) {
	  edit_relaynum_edit->setText(QString().sprintf("%04X",*relaynum));
	}
	break;

      case RDMatrix::VguestTypeDisplay:
	setCaption(tr("Edit vGuest Display"));
	edit_relaynum_edit->setDisabled(true);
	break;
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
  *edit_enginenum=enginenum;
  *edit_devicenum=devicenum;
  *edit_surfacenum=surfacenum;
  *edit_relaynum=relaynum;
  done(0);
}


void EditVguestResource::cancelData()
{
  done(1);
}
