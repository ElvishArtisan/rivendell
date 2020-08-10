// add_svc.cpp
//
// Add a Rivendell Service
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

#include <qstring.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include "add_svc.h"
#include "edit_svc.h"
#include "globals.h"

AddSvc::AddSvc(QString *svcname,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  svc_name=svcname;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Add Service"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);
  validator->addBannedChar(QChar(' '));
  validator->addBannedChar(QChar('\t'));

  //
  // Service Name
  //
  svc_name_edit=new QLineEdit(this);
  svc_name_edit->setGeometry(155,11,sizeHint().width()-165,19);
  svc_name_edit->setMaxLength(10);
  svc_name_edit->setValidator(validator);
  QLabel *svc_name_label=
    new QLabel(svc_name_edit,tr("&New Service Name:"),this);
  svc_name_label->setGeometry(10,11,140,19);
  svc_name_label->setFont(labelFont());
  svc_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Exemplar
  //
  svc_exemplar_box=new QComboBox(this);
  svc_exemplar_box->setGeometry(155,36,sizeHint().width()-165,19);
  QLabel *svc_exemplar_label=
    new QLabel(svc_exemplar_box,tr("Base Service On:"),this);
  svc_exemplar_label->setGeometry(10,36,140,19);
  svc_exemplar_label->setFont(labelFont());
  svc_exemplar_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Exemplar List
  //
  svc_exemplar_box->insertItem(tr("Empty Host Config"));
  QString sql="select NAME from SERVICES";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    svc_exemplar_box->insertItem(q->value(0).toString());
  }
  delete q;
}


AddSvc::~AddSvc()
{
  delete svc_name_edit;
}


QSize AddSvc::sizeHint() const
{
  return QSize(350,135);
} 


QSizePolicy AddSvc::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddSvc::okData()
{
  QString err_msg;
  QString exemplar="";

  if(svc_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),
			 tr("You must give the service a name!"));
    return;
  }

  if(svc_exemplar_box->currentItem()>0) {
    exemplar=svc_exemplar_box->currentText();
  }
  if(!RDSvc::create(svc_name_edit->text(),&err_msg,exemplar,rda->config())) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),err_msg);
    return;
  }
  /*
  RDSvc *svc=new RDSvc(svc_name_edit->text(),admin_station,admin_config,this);
  if(svc->exists()) {
    QMessageBox::warning(this,tr("Service Exists"),
			 tr("Service Already Exists!"));
    delete svc;
    return;
  }
  if(svc_exemplar_box->currentItem()==0) {  // Create Empty Service
    svc->create("",admin_config);
  }
  else {
    svc->create(svc_exemplar_box->currentText(),admin_config);
  }
  delete svc;
  */
  *svc_name=svc_name_edit->text();

  EditSvc *edit_svc=new EditSvc(svc_name_edit->text(),this);
  if(edit_svc->exec()<0) {
    delete edit_svc;
    done(-1);
    return;
  }
  delete edit_svc;
  done(0);
}


void AddSvc::cancelData()
{
  done(-1);
}
