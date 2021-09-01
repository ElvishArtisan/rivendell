// edit_sas_resource.cpp
//
// Edit an SAS Resource Record.
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

#include "edit_sas_resource.h"

EditSasResource::EditSasResource(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDAdmin - "+tr("Edit SAS Switch"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Console Number
  //
  edit_enginenum_edit=new QLineEdit(this);
  edit_enginenum_edit->setGeometry(165,10,50,20);
  QLabel *label=new QLabel(tr("Console Number: "),this);
  label->setGeometry(10,10,150,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Source Number
  //
  edit_devicenum_edit=new QLineEdit(this);
  edit_devicenum_edit->setGeometry(165,36,50,20);
  label=new QLabel(tr("Source Number: "),this);
  label->setGeometry(10,36,150,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Opto/Relay Number
  //
  edit_relaynum_edit=new QLineEdit(this);
  edit_relaynum_edit->setGeometry(165,62,50,20);
  edit_relaynum_label=new QLabel(tr("Opto/Relay Number: "),this);
  edit_relaynum_label->setGeometry(10,62,150,20);
  edit_relaynum_label->setFont(labelFont());
  edit_relaynum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditSasResource::sizeHint() const
{
  return QSize(250,152);
}


QSizePolicy EditSasResource::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditSasResource::exec(unsigned id)
{
  edit_id=id;

  QString sql=QString("select ")+
    "`NUMBER`,"+      // 00
    "`ENGINE_NUM`,"+  // 01
    "`DEVICE_NUM`,"+  // 02
    "`RELAY_NUM` "+   // 03
    "from `VGUEST_RESOURCES` where "+
    QString::asprintf("`ID`=%u",edit_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(1).toInt()>=0) {
      edit_enginenum_edit->setText(QString::asprintf("%d",q->value(1).toInt()));
    }
    else {
      edit_enginenum_edit->setText("");
    }
    if(q->value(2).toInt()>=0) {
      edit_devicenum_edit->setText(QString::asprintf("%d",q->value(2).toInt()));
    }
    else {
      edit_devicenum_edit->setText("");
    }
    if(q->value(3).toInt()>=0) {
      edit_relaynum_edit->setText(QString::asprintf("%d",q->value(3).toInt()));
    }
    else {
      edit_relaynum_edit->setText("");
    }
  }
  delete q;
  return QDialog::exec();
}


void EditSasResource::okData()
{
  bool ok;
  int enginenum=edit_enginenum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_enginenum_edit->text().isEmpty()) {
      enginenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Console Number is Invalid!"));
      return;
    }
  }
  int devicenum=edit_devicenum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_devicenum_edit->text().isEmpty()) {
      devicenum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Source Number is Invalid!"));
      return;
    }
  }
  int relaynum=edit_relaynum_edit->text().toInt(&ok);
  if(!ok) {
    if(edit_relaynum_edit->text().isEmpty()) {
      relaynum=-1;
    }
    else {
      QMessageBox::warning(this,tr("Invalid Number"),
			   tr("The Opto/Relay Number is Invalid!"));
      return;
    }
  }

  QString sql=QString("update `VGUEST_RESOURCES` set ")+
    QString::asprintf("`ENGINE_NUM`=%d,",enginenum)+
    QString::asprintf("`DEVICE_NUM`=%d,",devicenum)+
    QString::asprintf("`RELAY_NUM`=%d ",relaynum)+
    QString::asprintf("where `ID`=%u",edit_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditSasResource::cancelData()
{
  done(false);
}
