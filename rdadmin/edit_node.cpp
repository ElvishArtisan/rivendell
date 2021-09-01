// edit_node.cpp
//
// Edit a Rivendell LiveWire Node
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_node.h"
#include "view_node_info.h"

EditNode::EditNode(int *id,RDMatrix *matrix,QWidget *parent)
  : RDDialog(parent)
{
  edit_id=id;
  edit_matrix=matrix;
  edit_password_changed=false;
  setWindowTitle("RDAdmin - "+tr("Edit LiveWire Node"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Node Hostname
  //
  edit_hostname_edit=new QLineEdit(this);
  edit_hostname_edit->setGeometry(100,10,180,20);
  QLabel *label=new QLabel(tr("Hostname: "),this);
  label->setGeometry(10,10,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node TCP Port
  //
  edit_tcpport_spin=new QSpinBox(this);
  edit_tcpport_spin->setGeometry(335,10,sizeHint().width()-345,20);
  edit_tcpport_spin->setRange(0,0xFFFF);
  label=new QLabel(tr("Port: "),this);
  label->setGeometry(290,10,45,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setGeometry(100,32,sizeHint().width()-110,20);
  label=new QLabel(tr("Description: "),this);
  label->setGeometry(10,32,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Base Output
  //
  edit_output_spin=new QSpinBox(this);
  edit_output_spin->setGeometry(100,54,60,20);
  edit_output_spin->setRange(0,0x7FFF);
  edit_output_spin->setSpecialValueText(tr("None"));
  label=new QLabel(tr("First Output: "),this);
  label->setGeometry(10,54,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node Password
  //
  edit_password_edit=new QLineEdit(this);
  edit_password_edit->setGeometry(255,54,90,20);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setText("********");
  connect(edit_password_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(passwordChangedData(const QString &)));
  label=new QLabel(tr("Password: "),this);
  label->setGeometry(170,54,80,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  View Node Info Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("View Node\nInfo"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this);
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

  //
  // Load Data
  //
  if((*edit_id)<0) {
    edit_tcpport_spin->setValue(RD_LIVEWIRE_DEFAULT_TCP_PORT);
  }
  else {
    QString sql;
    RDSqlQuery *q;
    sql=QString("select ")+
      "`HOSTNAME`,"+     // 00
      "`TCP_PORT`,"+     // 01
      "`DESCRIPTION`,"+  // 02
      "`PASSWORD`,"+     // 03
      "`BASE_OUTPUT` "+  // 04
      "from `SWITCHER_NODES` where "+
      QString::asprintf("`ID`=%d",*edit_id);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      edit_hostname_edit->setText(q->value(0).toString());
      edit_tcpport_spin->setValue(q->value(1).toInt());
      edit_description_edit->setText(q->value(2).toString());
      edit_password=q->value(3).toString();
      edit_output_spin->setValue(q->value(4).toInt());
    }
    delete q;
  }
}


QSize EditNode::sizeHint() const
{
  return QSize(400,144);
}


QSizePolicy EditNode::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditNode::passwordChangedData(const QString &)
{
  edit_password_changed=true;
}


void EditNode::viewData()
{
  QString password=edit_password;
  if(edit_password_changed) {
    password=edit_password_edit->text();
  }
  ViewNodeInfo *node=new ViewNodeInfo(this);
  node->exec(edit_hostname_edit->text(),edit_tcpport_spin->value(),
	     password,edit_output_spin->value());
  delete node;
}


void EditNode::okData()
{
  QString sql;
  RDSqlQuery *q;

  if(edit_password_changed) {
    edit_password=edit_password_edit->text();
  }

  if((*edit_id)<0) {
    sql=QString("select `ID` from `SWITCHER_NODES` where ")+
      "(`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"') && "+
      QString::asprintf("(`MATRIX`=%d) && ",edit_matrix->matrix())+
      "(`HOSTNAME`='"+RDEscapeString(edit_hostname_edit->text())+"') && "+
      QString::asprintf("(`TCP_PORT`=%d)",edit_tcpport_spin->value());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      delete q;
      QMessageBox::warning(this,tr("Duplicate Node"),
			   tr("That node is already listed for this matrix!"));
      return;
    }
    delete q;

    sql="select `ID` from `SWITCHER_NODES` order by `ID` desc";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      *edit_id=q->value(0).toInt()+1;
    }
    else {
      *edit_id=1;
    }
    delete q;
    sql=QString("insert into `SWITCHER_NODES` set ")+
      QString::asprintf("`ID`=%d,",*edit_id)+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"',"+
      QString::asprintf("`MATRIX`=%d,",edit_matrix->matrix())+
      "`HOSTNAME`='"+RDEscapeString(edit_hostname_edit->text())+"',"+
      QString::asprintf("`TCP_PORT`=%d,",edit_tcpport_spin->value())+
      "`DESCRIPTION`='"+RDEscapeString(edit_description_edit->text())+"',"+
      QString::asprintf("`BASE_OUTPUT`=%d,",edit_output_spin->value())+
      "`PASSWORD`='"+RDEscapeString(edit_password)+"'";
    RDSqlQuery::apply(sql);
  }
  else {
    sql=QString("update `SWITCHER_NODES` set ")+
      "`HOSTNAME`='"+RDEscapeString(edit_hostname_edit->text())+"',"+
      QString::asprintf("`TCP_PORT`=%d,",edit_tcpport_spin->value())+
      "`DESCRIPTION`='"+RDEscapeString(edit_description_edit->text())+"',"+
      QString::asprintf("`BASE_OUTPUT`=%d,",edit_output_spin->value())+
      "`PASSWORD`='"+RDEscapeString(edit_password)+"' where "+
      QString::asprintf("`ID`=%d",*edit_id);
    q=new RDSqlQuery(sql);
    delete q;
  }

  done(true);
}


void EditNode::cancelData()
{
  done(false);
}
