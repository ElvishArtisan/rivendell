// rdintegerdialog.cpp
//
// A widget to set an integer value.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdintegerdialog.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#include <qpushbutton.h>
#include <qlabel.h>

#include <rdintegerdialog.h>

RDIntegerDialog::RDIntegerDialog(int *value,const QString &lbl,int low,int high,
				 QWidget *parent,const char *name)
  : QDialog(parent,name,false)
{
  int_value=value;

  //
  // Fix the Window Size
  //
  setCaption(tr("Set Value"));
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont font=QFont("helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Value Control
  //
  int_value_box=new QSpinBox(this);
  int_value_box->setGeometry(125,10,80,20);
  int_value_box->setRange(low,high);
  int_value_box->setValue(*value);
  QLabel *label=new QLabel(int_value_box,lbl,this);
  label->setGeometry(10,10,110,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // OK Button
  //
  QPushButton *button=new QPushButton(tr("&OK"),this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setFont(font);
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  button=new QPushButton(tr("&Cancel"),this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(font);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDIntegerDialog::~RDIntegerDialog()
{
}


QSize RDIntegerDialog::sizeHint() const
{
  return QSize(240,100);
}


QSizePolicy RDIntegerDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDIntegerDialog::okData()
{
  *int_value=int_value_box->value();
  done(0);
}


void RDIntegerDialog::cancelData()
{
  done(-1);
}


void RDIntegerDialog::closeEvent(QCloseEvent *e)
{
  done(-1);
}
