//
// Add a Rivendell Matrix
//
//   (C) Copyright 2002-2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdmatrix.h>

#include "add_matrix.h"
#include "edit_user.h"

AddMatrix::AddMatrix(QString station,QWidget *parent)
  : QDialog(parent)
{
  add_station=station;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Add Switcher"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Matrix Number
  //
  add_matrix_box=new QSpinBox(this);
  add_matrix_box->setGeometry(165,11,50,19);
  add_matrix_box->setRange(0,MAX_MATRICES-1);
  QLabel *label=new QLabel(add_matrix_box,tr("&New Matrix Number:"),this);
  label->setGeometry(10,11,150,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Matrix Type
  //
  add_type_box=new QComboBox(this);
  add_type_box->setGeometry(165,36,200,19);
  for(int i=0;i<RDMatrix::LastType;i++) {
    add_type_box->insertItem(RDMatrix::typeString((RDMatrix::Type)i));
  }
  label=new QLabel(add_type_box,tr("&Switcher Type:"),this);
  label->setGeometry(10,36,150,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Assign Next Free Matrix
  //
  int n=GetNextMatrix();
  if(n>=0) {
    add_matrix_box->setValue(n);
  }
}


QSize AddMatrix::sizeHint() const
{
  return QSize(400,130);
} 


QSizePolicy AddMatrix::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddMatrix::okData()
{
  if(RDMatrix::exists(add_station,add_matrix_box->value())) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Invalid Matrix"),
			 tr("Matrix already exists!"));
    return;
  }
  RDMatrix::create(add_station,add_matrix_box->value(),
		   (RDMatrix::Type)add_type_box->currentItem());
  done(add_matrix_box->value());
}


void AddMatrix::cancelData()
{
  done(-1);
}


int AddMatrix::GetNextMatrix()
{
  int n=0;

  QString sql=QString("select MATRIX from MATRICES where STATION_NAME=\"")+
    RDEscapeString(add_station)+"\" order by MATRIX";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    if(n!=q->value(0).toInt()) {
      delete q;
      return n;
    }
    n++;
  }
  delete q;
  if(n<MAX_MATRICES) {
    return n;
  }
  return -1;
}
