// rdlicense.cpp
//
// Display License Text.
//
// (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdlicense.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <qpushbutton.h>

#include <rdlicense.h>

#include <html_gpl2.cpp>

RDLicense::RDLicense(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
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
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);

  license_edit=new QTextEdit(this,"license_edit");
  license_edit->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-70);
  license_edit->setTextFormat(RichText);
  license_edit->setReadOnly(true);

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize RDLicense::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy RDLicense::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDLicense::exec(RDLicense::License lic)
{
  switch(lic) {
      case RDLicense::GplV2:
	license_edit->setText((const char *)html_gpl2);
	setCaption(tr("GNU Public License v2"));
	break;
  }
  QDialog::exec();
}


void RDLicense::closeData()
{
  done(0);
}
