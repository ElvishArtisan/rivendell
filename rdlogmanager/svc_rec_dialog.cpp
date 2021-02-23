// svc_rec_dialog.cpp
//
// A Services/Reports Management Dialog.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <QPushButton>

#include <rdapplication.h>
#include <rddatedecode.h>
#include <rdreport.h>

#include "globals.h"
#include "svc_rec_dialog.h"

//
// Global Classes
//
SvcRecDialog::SvcRecDialog(const QString &svcname,QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDLogManager - "+svcname+" "+tr("Report Data"));

  //
  // Datepicker
  //
  date_picker=new SvcRec(svcname,this);
  date_picker->setGeometry(10,10,
			   date_picker->sizeHint().width(),
			   date_picker->sizeHint().height());
  connect(date_picker,SIGNAL(dateSelected(const QDate &,bool)),
	  this,SLOT(dateSelectedData(const QDate &,bool)));

  //
  // Delete Button
  //
  date_delete_button=new QPushButton(this);
  date_delete_button->
    setGeometry(10,sizeHint().height()-60,80,50);
  date_delete_button->setFont(buttonFont());
  date_delete_button->setText(tr("&Purge\nData"));
  connect(date_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));
  date_delete_button->setEnabled(rda->user()->deleteRec()&&
    date_picker->dayActive(date_picker->date().day()));

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Close"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


SvcRecDialog::~SvcRecDialog()
{
}


QSize SvcRecDialog::sizeHint() const
{
  return QSize(date_picker->sizeHint().width()+20,
	       date_picker->sizeHint().height()+85);
}


QSizePolicy SvcRecDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void SvcRecDialog::dateSelectedData(const QDate &,bool active)
{
  date_delete_button->setEnabled(rda->user()->deleteRec()&&
    date_picker->dayActive(date_picker->date().day()));
}


void SvcRecDialog::deleteData()
{
  if(QMessageBox::question(this,tr("Delete Report Data"),
			 tr("Are you sure you want to delete report data for")+
			 " "+
			   date_picker->date().toString("MM/dd/yyyy"),
			   QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  date_picker->deleteDay();
  date_delete_button->setDisabled(true);
}


void SvcRecDialog::closeData()
{
  done(-1);
}
