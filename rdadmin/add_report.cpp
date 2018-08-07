// add_report.cpp
//
// Add a Rivendell Report
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

#include <qdialog.h>
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "add_report.h"
#include "test_import.h"
#include "autofill_carts.h"
#include "edit_svc_perms.h"

AddReport::AddReport(QString *rptname,QWidget *parent)
  : QDialog(parent,"",true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Report"));
  add_name=rptname;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont section_font=QFont("Helvetica",14,QFont::Bold);
  section_font.setPixelSize(14);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Report Description
  //
  add_name_edit=new QLineEdit(this);
  add_name_edit->setGeometry(170,10,sizeHint().width()-180,19);
  add_name_edit->setMaxLength(64);
  add_name_edit->setValidator(validator);
  QLabel *label=new QLabel(add_name_edit,tr("&Report Name:"),this);
  label->setGeometry(10,10,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize AddReport::sizeHint() const
{
  return QSize(500,104);
} 


QSizePolicy AddReport::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddReport::okData()
{
  QString sql;
  RDSqlQuery *q;

  if(add_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),
			 tr("You must provide a report name!"));
    return;
  }
  sql=QString("select NAME from REPORTS where ")+
    "NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::warning(this,tr("Report Exists"),
			 tr("A report with that name already exists!"));
    delete q;
    return;
  }
  delete q;
  sql=QString("insert into REPORTS set ")+
    "NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  *add_name=add_name_edit->text();
  done(0);
}


void AddReport::cancelData()
{
  done(-1);
}
