// edit_sas_resource.cpp
//
// Edit an SAS Resource Record.
//
//   (C) Copyright 2002-2005,2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_sas_resource.cpp,v 1.1 2011/05/07 00:32:29 cvs Exp $
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

#include <edit_sas_resource.h>

EditSasResource::EditSasResource(int *enginenum,int *devicenum,int *relaynum,
				 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_enginenum=enginenum;
  edit_devicenum=devicenum;
  edit_relaynum=relaynum;
  setCaption(tr("Edit SAS Switch"));

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
  // Console Number
  //
  edit_enginenum_edit=new QLineEdit(this,"edit_enginenum_edit");
  edit_enginenum_edit->setGeometry(135,10,50,20);
  QLabel *label=new QLabel(edit_enginenum_edit,tr("Console Number: "),
			   this,"edit_enginenum_label");
  label->setGeometry(10,10,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Source Number
  //
  edit_devicenum_edit=new QLineEdit(this,"edit_devicenum_edit");
  edit_devicenum_edit->setGeometry(135,36,50,20);
  label=new QLabel(edit_devicenum_edit,tr("Source Number: "),
		   this,"edit_devicenum_label");
  label->setGeometry(10,36,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Opto/Relay Number
  //
  edit_relaynum_edit=new QLineEdit(this,"edit_relaynum_edit");
  edit_relaynum_edit->setGeometry(135,62,50,20);
  edit_relaynum_label=new QLabel(edit_relaynum_edit,tr("Opto/Relay Number: "),
		   this,"edit_relaynum_label");
  edit_relaynum_label->setGeometry(10,62,120,20);
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
    edit_enginenum_edit->setText(QString().sprintf("%d",*enginenum));
  }
  if(*devicenum>=0) {
    edit_devicenum_edit->setText(QString().sprintf("%d",*devicenum));
  }
  if(*relaynum>=0) {
    edit_relaynum_edit->setText(QString().sprintf("%d",*relaynum));
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
  *edit_enginenum=enginenum;
  *edit_devicenum=devicenum;
  *edit_relaynum=relaynum;
  done(0);
}


void EditSasResource::cancelData()
{
  done(1);
}
