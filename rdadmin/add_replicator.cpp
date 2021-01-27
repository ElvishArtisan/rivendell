// add_replicator.cpp
//
// Add a Rivendell Replicator Configuration
//
//   (C) Copyright 2010-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <qmessagebox.h>

#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_replicator.h"
#include "add_replicator.h"

AddReplicator::AddReplicator(QString *rname,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  repl_name=rname;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Add Replicator"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Replicator Name
  //
  repl_name_edit=new QLineEdit(this);
  repl_name_edit->setGeometry(145,11,sizeHint().width()-150,19);
  repl_name_edit->setMaxLength(10);
  repl_name_edit->setValidator(validator);
  QLabel *label=new QLabel(repl_name_edit,tr("Replicator Name:"),this);
  label->setGeometry(10,11,130,19);
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
}


AddReplicator::~AddReplicator()
{
  delete repl_name_edit;
}


QSize AddReplicator::sizeHint() const
{
  return QSize(250,108);
} 


QSizePolicy AddReplicator::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddReplicator::okData()
{
  RDSqlQuery *q;
  QString sql;

  if(repl_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("You must give the replicator a name!"));
    return;
  }

  sql=QString("insert into REPLICATORS set ")+
    "NAME=\""+RDEscapeString(repl_name_edit->text())+"\"";
  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    QMessageBox::warning(this,tr("Replicator Exists"),tr("A replicator with that name already exists!"));
    delete q;
    return;
  }
  delete q;

  EditReplicator *replicator=new EditReplicator(repl_name_edit->text(),this);
  if(replicator->exec()<0) {
    sql=QString("delete from REPLICATORS where ")+
      "NAME=\""+RDEscapeString(repl_name_edit->text())+"\"";
    q=new RDSqlQuery(sql);
    delete q;
    delete replicator;
    done(false);
    return;
  }
  delete replicator;
  *repl_name=repl_name_edit->text();
  done(true);
}


void AddReplicator::cancelData()
{
  done(false);
}
