// add_hostvar.cpp
//
// Add a Rivendell Host Variable
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdhostvariable.h>

#include "add_hostvar.h"

AddHostvar::AddHostvar(QString station,int *id,QWidget *parent)
  : QDialog(parent)
{
  add_station_name=station;
  add_id=id;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  setWindowTitle("RDAdmin  - "+tr("Add Host Variable"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Variable Name
  //
  add_name_edit=new QLineEdit(this);
  add_name_edit->setMaxLength(32);
  add_name_label=new QLabel(add_name_edit,tr("Variable Name:"),this);
  add_name_label->setFont(font);
  add_name_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  Ok Button
  //
  add_ok_button=new QPushButton(this);
  add_ok_button->setDefault(true);
  add_ok_button->setFont(font);
  add_ok_button->setText(tr("&OK"));
  connect(add_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  add_cancel_button=new QPushButton(this);
  add_cancel_button->setFont(font);
  add_cancel_button->setText(tr("&Cancel"));
  connect(add_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddHostvar::~AddHostvar()
{
}


QSize AddHostvar::sizeHint() const
{
  return QSize(385,108);
} 


QSizePolicy AddHostvar::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddHostvar::okData()
{
  if((add_name_edit->text().left(1)!=QString("%"))||
     (add_name_edit->text().right(1)!=QString("%"))||
     (add_name_edit->text().length()<3)) {
    QMessageBox::warning(this,tr("Invalid Name"),
			 tr("The variable name is invalid."));
    return;
  }
  if(RDHostVariable::exists(add_station_name,add_name_edit->text())) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Variable Exists"),
			 tr("The variable")+" \""+add_name_edit->text()+"\" "+
			 tr("already exists!"));
    return;
  }
  *add_id=RDHostVariable::create(add_station_name,add_name_edit->text());
  if(*add_id<0) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Internal Error"),
			 tr("Unable to create host variable!"));
    return;
  }

  done(0);
}


void AddHostvar::cancelData()
{
  done(-1);
}


void AddHostvar::resizeEvent(QResizeEvent *e)
{
  add_name_label->setGeometry(10,11,110,19);
  add_name_edit->setGeometry(125,11,size().width()-140,20);
  add_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  add_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
