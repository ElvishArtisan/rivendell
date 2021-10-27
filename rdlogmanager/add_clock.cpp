// add_clock.cpp
//
// Add a Rivendell Clock
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
#include <QStringList>

#include <rdescape_string.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include "add_clock.h"

AddClock::AddClock(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDLogManager - "+tr("Add Clock"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

  //
  // Create Validators
  //
  RDTextValidator *validator=new RDTextValidator();
  validator->addBannedChar('(');
  validator->addBannedChar(')');
  validator->addBannedChar('!');
  validator->addBannedChar('@');
  validator->addBannedChar('#');
  validator->addBannedChar('$');
  validator->addBannedChar('%');
  validator->addBannedChar('^');
  validator->addBannedChar('&');
  validator->addBannedChar('*');
  validator->addBannedChar('{');
  validator->addBannedChar('}');
  validator->addBannedChar('[');
  validator->addBannedChar(']');
  validator->addBannedChar(':');
  validator->addBannedChar(';');
  validator->addBannedChar(34);
  validator->addBannedChar('<');
  validator->addBannedChar('>');
  validator->addBannedChar('.');
  validator->addBannedChar(',');
  validator->addBannedChar('\\');
  validator->addBannedChar('-');
  validator->addBannedChar('_');
  validator->addBannedChar('/');
  validator->addBannedChar('+');
  validator->addBannedChar('=');
  validator->addBannedChar('~');
  validator->addBannedChar('?');
  validator->addBannedChar('|');

  //
  // Clock Name
  //
  clock_name_edit=new QLineEdit(this);
  clock_name_edit->setMaxLength(58);  // MySQL limitation!
  clock_name_edit->setValidator(validator);
  connect(clock_name_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(clockNameChangedData(const QString &)));
  connect(clock_name_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));
  clock_name_label=new QLabel(tr("New Clock Name:"),this);
  clock_name_label->setFont(labelFont());
  clock_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Clock Code
  //
  clock_code_edit=new QLineEdit(this);
  clock_code_edit->setMaxLength(3);  // MySQL limitation!
  clock_code_edit->setValidator(validator);
  connect(clock_code_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(textChangedData(const QString &)));
  clock_code_label=new QLabel(tr("New Clock Code:"),this);
  clock_code_label->setFont(labelFont());
  clock_code_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  clock_ok_button=new QPushButton(this);
  clock_ok_button->setDefault(true);
  clock_ok_button->setFont(buttonFont());
  clock_ok_button->setText(tr("OK"));
  connect(clock_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  clock_cancel_button=new QPushButton(this);
  clock_cancel_button->setFont(buttonFont());
  clock_cancel_button->setText(tr("Cancel"));
  connect(clock_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddClock::~AddClock()
{
  delete clock_name_edit;
}


QSize AddClock::sizeHint() const
{
  return QSize(400,127);
} 


QSizePolicy AddClock::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int AddClock::exec(QString *clk_name,QString *clk_code)
{
  clock_name=clk_name;
  clock_code=clk_code;

  clock_name_edit->setText(*clock_name);
  clock_name_edit->selectAll();
  clock_code_edit->setText(*clock_code);
  textChangedData("");

  return QDialog::exec();
}


void AddClock::clockNameChangedData(const QString &str)
{
  QStringList f0=str.split(" ",QString::SkipEmptyParts);
  QString code;

  while(f0.size()>3) {
    f0.removeLast();
  }
  for(int i=0;i<f0.size();i++) {
    code+=f0.at(i).left(1).toUpper();
  }
  clock_code_edit->setText(code);
}


void AddClock::textChangedData(const QString &str)
{
  clock_ok_button->setDisabled(clock_name_edit->text().isEmpty()&&
			       clock_code_edit->text().isEmpty());
}


void AddClock::okData()
{
  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Sanity Checks
  //
  sql=QString("select ")+
    "`NAME` "+  // 00
    "from `CLOCKS` where "+
    "`NAME`='"+RDEscapeString(clock_name_edit->text())+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::information(this,"RDLogManager "+tr("Name in use"),
			     tr("A clock with that name already exists!"));
    delete q;
    return;
  }
  delete q;

  sql=QString("select ")+
    "`SHORT_NAME` "+  // 00
    "from `CLOCKS` where "+
    "`SHORT_NAME`='"+RDEscapeString(clock_code_edit->text())+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::information(this,"RDLogManager "+tr("Code in use"),
			     tr("That code is already in use!"));
    delete q;
    return;
  }
  delete q;

  *clock_name=clock_name_edit->text();
  *clock_code=clock_code_edit->text();

  done(true);
}


void AddClock::cancelData()
{
  done(false);
}


void AddClock::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void AddClock::resizeEvent(QResizeEvent *e)
{
  clock_name_edit->setGeometry(145,11,size().width()-155,19);
  clock_name_label->setGeometry(10,11,130,19);

  clock_code_edit->setGeometry(145,33,50,19);
  clock_code_label->setGeometry(10,33,130,19);

  clock_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  clock_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
