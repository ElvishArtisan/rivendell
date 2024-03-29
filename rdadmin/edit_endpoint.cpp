// edit_endpoint.cpp
//
// Edit a Rivendell Endpoint
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

#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include <edit_endpoint.h>

EditEndpoint::EditEndpoint(RDMatrix::Type type,RDMatrix::Endpoint endpoint,
			   int pointnum,QString *pointname,int *enginenum,
			   int *devicenum,QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Endpoint Name
  //
  edit_endpoint_edit=new QLineEdit(this);
  edit_endpoint_edit->setGeometry(75,10,sizeHint().width()-85,20);
  edit_endpoint_edit->setValidator(validator);
  QLabel *label=new QLabel(tr("Name: "),this);
  label->setGeometry(10,13,60,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight);

  //
  // Logitek Engine Number
  //
  edit_enginenum_edit=new QLineEdit(this);
  edit_enginenum_edit->setGeometry(135,36,50,20);
  edit_enginenum_label=new QLabel(tr("Engine")+":",this);
  edit_enginenum_label->setGeometry(10,36,120,20);
  edit_enginenum_label->setFont(labelFont());
  edit_enginenum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Logitek Device Number
  //
  edit_devicenum_edit=new QLineEdit(this);
  edit_devicenum_edit->setGeometry(135,62,50,20);
  edit_devicenum_label=new QLabel(tr("Device (Hex)")+":",this);
  edit_devicenum_label->setGeometry(10,62,120,20);
  edit_devicenum_label->setFont(labelFont());
  edit_devicenum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditEndpoint::sizeHint() const
{
  return QSize(400,100);
}


QSizePolicy EditEndpoint::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditEndpoint::exec(RDMatrix *mtx,RDMatrix::Endpoint endpt_type,int endpt_id)
{
  QString sql;
  RDSqlQuery *q=NULL;

  edit_mtx=mtx;
  edit_endpoint_type=endpt_type;
  edit_endpoint_id=endpt_id;
  if(endpt_type==RDMatrix::Input) {
    edit_table="`INPUTS`";
    setWindowTitle("RDAdmin - "+tr("Edit Input"));
  }
  else {
    edit_table="`OUTPUTS`";
    setWindowTitle("RDAdmin - "+tr("Edit Output"));
  }

  sql=QString("select ")+
    "`NAME`,"+        // 00
    "`ENGINE_NUM`,"+  // 01
    "`DEVICE_NUM` "+  // 02
    "from "+edit_table+" where "+
    QString::asprintf("`ID`=%d",endpt_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_endpoint_edit->setText(q->value(0).toString());
    if(q->value(1).toInt()>=0) {
      edit_enginenum_edit->setText(QString::asprintf("%d",q->value(1).toInt()));
    }
    else {
      edit_enginenum_edit->setText("");
    }
    if(q->value(2).toInt()>=0) {
      edit_devicenum_edit->
	setText(QString::asprintf("%04X",q->value(2).toInt()));
    }
    else {
      edit_enginenum_edit->setText("");
    }
  }
  delete q;

  if(mtx->type()!=RDMatrix::LogitekVguest) {
    edit_enginenum_label->hide();
    edit_enginenum_edit->hide();
    edit_devicenum_label->hide();
    edit_devicenum_edit->hide();
  }

  return QDialog::exec();
}


void EditEndpoint::okData()
{
  QString sql;

  sql=QString("update ")+edit_table+" set "+
    "`NAME`='"+RDEscapeString(edit_endpoint_edit->text())+"',"+
    QString::asprintf("`ENGINE_NUM`=%d,",edit_enginenum_edit->text().toInt())+
    QString().
    sprintf("`DEVICE_NUM`=%d ",edit_devicenum_edit->text().toInt(NULL,16))+
    "where "+
    QString::asprintf("`ID`=%d",edit_endpoint_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditEndpoint::cancelData()
{
  done(false);
}
