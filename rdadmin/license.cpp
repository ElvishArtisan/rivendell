// license.cpp
//
// Display License Text.
//
// (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "license.h"

//
// Externally Generated Strings (via cwrap)
//
extern const unsigned char global_credits[];
extern const unsigned char global_gpl2[];

License::License(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  license_edit=new QTextEdit(this);
  license_edit->setReadOnly(true);

  //
  // Close Button
  //
  close_button=new QPushButton(this);
  close_button->setFont(buttonFont());
  close_button->setText(tr("Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));
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
    license_edit->setAcceptRichText(false);
    license_edit->setText(QString::fromUtf8((const char *)global_credits));
    setWindowTitle(tr("Rivendell Credits"));
    break;

  case License::GplV2:
    license_edit->setAcceptRichText(true);
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


void License::resizeEvent(QResizeEvent *e)
{
  license_edit->
    setGeometry(10,10,size().width()-20,size().height()-80);
  close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
