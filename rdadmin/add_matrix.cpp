// add_matrix.cpp
//
// Add a Rivendell Matrix
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qcombobox.h>
#include <qspinbox.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_user.h"
#include "add_matrix.h"
#include "rdpasswd.h"

AddMatrix::AddMatrix(QString station,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  add_station=station;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Add Switcher"));

  //
  // Matrix Number
  //
  add_matrix_box=new QSpinBox(this);
  add_matrix_box->setGeometry(165,11,30,19);
  add_matrix_box->setRange(0,MAX_MATRICES-1);
  QLabel *label=new QLabel(tr("&New Matrix Number:"),this);
  label->setGeometry(10,11,150,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Matrix Type
  //
  add_type_box=new QComboBox(this);
  add_type_box->setGeometry(165,36,200,19);
  for(int i=0;i<RDMatrix::LastType;i++) {
    add_type_box->
      insertItem(add_type_box->count(),RDMatrix::typeString((RDMatrix::Type)i));
  }
  label=new QLabel(tr("&Switcher Type:"),this);
  label->setGeometry(10,36,150,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(buttonFont());
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
  QString sql=QString("select MATRIX from MATRICES where STATION_NAME=\"")+
    RDEscapeString(add_station)+"\" && MATRIX="+
    QString().sprintf("%d",add_matrix_box->value());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    delete q;
    QMessageBox::warning(this,tr("Invalid Matrix"),
			 tr("Matrix already exists!"));
    return;
  }
  delete q;

  sql=QString("insert into MATRICES set STATION_NAME=\"")+
    RDEscapeString(add_station)+"\","+
    "NAME=\""+tr("New Switcher")+"\","+
    "GPIO_DEVICE=\""+RD_DEFAULT_GPIO_DEVICE+"\","+
    QString().
    sprintf("MATRIX=%d,\
             PORT=0,\
             TYPE=%d,\
             INPUTS=%d,\
             OUTPUTS=%d,\
             GPIS=%d,\
             GPOS=%d,\
             PORT_TYPE=%d,\
             PORT_TYPE_2=%d",
	    add_matrix_box->value(),
	    add_type_box->currentIndex(),
      RDMatrix::defaultControlValue((RDMatrix::Type)add_type_box->
				    currentIndex(),
				    RDMatrix::InputsControl),
      RDMatrix::defaultControlValue((RDMatrix::Type)add_type_box->
				    currentIndex(),
				    RDMatrix::OutputsControl),
      RDMatrix::defaultControlValue((RDMatrix::Type)add_type_box->
				    currentIndex(),
				    RDMatrix::GpisControl),
      RDMatrix::defaultControlValue((RDMatrix::Type)add_type_box->
				    currentIndex(),
				    RDMatrix::GposControl),
      RDMatrix::defaultControlValue((RDMatrix::Type)add_type_box->
				    currentIndex(),
				    RDMatrix::PortTypeControl),
	    RDMatrix::NoPort);
  q=new RDSqlQuery(sql);
  delete q;
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
