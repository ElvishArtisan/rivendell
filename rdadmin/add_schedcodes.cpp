// add_schedcodes.cpp
//
// Add scheduler codes dialog
//
//   (C) Copyright 2005-2018 Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2018-2023 Fred Gleason <fredg@paravelsystems.com>
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
#include <QRegExpValidator>

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_schedcodes.h"
#include "add_schedcodes.h"

AddSchedCode::AddSchedCode(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Add Scheduler Code"));

  //
  // Code Name
  //
  d_code_edit=new QLineEdit(this);
  d_code_edit->setMaxLength(10);
  QRegExpValidator *code_validator=
    new QRegExpValidator(QRegExp("[a-z0-9 ]{1,10}",Qt::CaseInsensitive),this);
  d_code_edit->setValidator(code_validator);
  connect(d_code_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(codeChangedData(const QString &)));
  d_code_label=new QLabel(tr("New Code:"),this);
  d_code_label->setFont(labelFont());
  d_code_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  d_ok_button=new QPushButton(this);
  d_ok_button->setDefault(true);
  d_ok_button->setFont(buttonFont());
  d_ok_button->setText(tr("OK"));
  d_ok_button->setDisabled(true);
  connect(d_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  d_cancel_button=new QPushButton(this);
  d_cancel_button->setFont(buttonFont());
  d_cancel_button->setText(tr("Cancel"));
  connect(d_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddSchedCode::~AddSchedCode()
{
  delete d_code_edit;
}


QSize AddSchedCode::sizeHint() const
{
  return QSize(250,120);
} 


QSizePolicy AddSchedCode::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int AddSchedCode::exec(QString *scode)
{
  d_sched_code=scode;

  d_code_edit->setText("");

  return QDialog::exec();
}


void AddSchedCode::resizeEvent(QResizeEvent *e)
{
  d_code_label->setGeometry(10,11,90,19);
  d_code_edit->setGeometry(105,11,size().width()-150,19);

  d_ok_button->setGeometry(size().width()-180,sizeHint().height()-60,80,50);
  d_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void AddSchedCode::codeChangedData(const QString &str)
{
  d_ok_button->setDisabled(str.isEmpty());
}


void AddSchedCode::okData()
{
  if(d_code_edit->text().isEmpty()) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Invalid Schedule Code!"));
    return;
  }

  QString sql=QString("insert into `SCHED_CODES` set ")+
    "`CODE`='"+RDEscapeString(d_code_edit->text())+"'";
  if(!RDSqlQuery::apply(sql)) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Schedule Code Already Exists!"));
    return;
  }

  //
  // Add schedcode rules
  //
  sql=QString("select ")+
    "`NAME` "  // 00
    "from `CLOCKS` "+
    "order by `NAME`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `RULE_LINES` set ")+
      "`CLOCK_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
      "`CODE`='"+RDEscapeString(d_code_edit->text())+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;

  *d_sched_code=d_code_edit->text();

  done(true);
}


void AddSchedCode::cancelData()
{
  done(false);
}
