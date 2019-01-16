// license.cpp
//
// Display License Text.
//
// (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "license.h"

//
// Externally Generated Strings (via cwrap)
//
extern const unsigned char global_credits[];
extern const unsigned char global_gpl2[];

License::License(QWidget *parent)
  : QDialog(parent)
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

  license_edit=new QTextEdit(this);
  license_edit->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-80);
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


QSize License::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy License::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void License::exec(License::Text lic)
{
  switch(lic) {
  case License::Credits:
    license_edit->setTextFormat(Qt::PlainText);
    license_edit->setText(QString::fromUtf8((const char *)global_credits));
    setWindowTitle(tr("Rivendell Credits"));
    break;

  case License::GplV2:
    license_edit->setTextFormat(Qt::RichText);
    license_edit->setText((const char *)global_gpl2);
    setWindowTitle(tr("GNU Public License v2"));
    break;
  }
  QDialog::exec();
}


void License::closeData()
{
  done(0);
}
