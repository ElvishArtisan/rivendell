// edit_node.cpp
//
// Edit a Rivendell LiveWire Node
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_node.h"
#include "view_node_info.h"

EditNode::EditNode(int *id,RDMatrix *matrix,QWidget *parent)
  : QDialog(parent)
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
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Node Hostname
  //
  edit_hostname_edit=new QLineEdit(this);
  edit_hostname_label=new QLabel(tr("Hostname: "),this);
  edit_hostname_label->setFont(bold_font);
  edit_hostname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node TCP Port
  //
  edit_tcpport_spin=new QSpinBox(this);
  edit_tcpport_spin->setRange(0,0xFFFF);
  edit_tcpport_label=new QLabel(tr("Port: "),this);
  edit_tcpport_label->setFont(bold_font);
  edit_tcpport_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_label=new QLabel(tr("Description: "),this);
  edit_description_label->setFont(bold_font);
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Base Output
  //
  edit_output_spin=new QSpinBox(this);
  edit_output_spin->setRange(0,0x7FFF);
  edit_output_spin->setSpecialValueText(tr("None"));
  edit_output_label=new QLabel(tr("First Output: "),this);
  edit_output_label->setFont(bold_font);
  edit_output_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node Password
  //
  edit_password_edit=new QLineEdit(this);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setText("********");
  connect(edit_password_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(passwordChangedData(const QString &)));
  edit_password_label=new QLabel(tr("Password: "),this);
  edit_password_label->setFont(bold_font);
  edit_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  View Node Info Button
  //
  edit_view_button=new QPushButton(this);
  edit_view_button->setFont(bold_font);
  edit_view_button->setText(tr("&View Node\nInfo"));
  connect(edit_view_button,SIGNAL(clicked()),this,SLOT(viewData()));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(bold_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(bold_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

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
      "HOSTNAME,"+
      "TCP_PORT,"+
      "DESCRIPTION,"+
      "PASSWORD,"+
      "BASE_OUTPUT "+
      "from SWITCHER_NODES where "+
      QString().sprintf("ID=%d",*edit_id);
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
    sql=QString("select ID from SWITCHER_NODES where ")+
      "(STATION_NAME=\""+RDEscapeString(edit_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX=%d)&&",edit_matrix->matrix())+
      "(HOSTNAME=\""+RDEscapeString(edit_hostname_edit->text())+"\")&&"+
      QString().sprintf("(TCP_PORT=%d)",edit_tcpport_spin->value());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      delete q;
      QMessageBox::warning(this,tr("Duplicate Node"),
			   tr("That node is already listed for this matrix!"));
      return;
    }
    delete q;

    sql="select ID from SWITCHER_NODES order by ID desc";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      *edit_id=q->value(0).toInt()+1;
    }
    else {
      *edit_id=1;
    }
    delete q;
    sql=QString("insert into SWITCHER_NODES set ")+
      QString().sprintf("ID=%d,",*edit_id)+
      "STATION_NAME=\""+RDEscapeString(edit_matrix->station())+"\","+
      QString().sprintf("MATRIX=%d,",edit_matrix->matrix())+
      "HOSTNAME=\""+RDEscapeString(edit_hostname_edit->text())+"\","+
      QString().sprintf("TCP_PORT=%d,",edit_tcpport_spin->value())+
      "DESCRIPTION=\""+RDEscapeString(edit_description_edit->text())+"\","+
      QString().sprintf("BASE_OUTPUT=%d,",edit_output_spin->value())+
      "PASSWORD=\""+RDEscapeString(edit_password)+"\"";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    sql=QString("update SWITCHER_NODES set ")+
      "HOSTNAME=\""+RDEscapeString(edit_hostname_edit->text())+"\","+
      QString().sprintf("TCP_PORT=%d,",edit_tcpport_spin->value())+
      "DESCRIPTION=\""+RDEscapeString(edit_description_edit->text())+"\","+
      QString().sprintf("BASE_OUTPUT=%d,",edit_output_spin->value())+
      "PASSWORD=\""+RDEscapeString(edit_password)+"\" where "+
      QString().sprintf("ID=%d",*edit_id);
    q=new RDSqlQuery(sql);
    delete q;
  }

  done(0);
}


void EditNode::cancelData()
{
  done(1);
}


void EditNode::resizeEvent(QResizeEvent *e)
{
  edit_hostname_edit->setGeometry(90,10,190,20);
  edit_hostname_label->setGeometry(10,10,80,20);
  edit_tcpport_spin->setGeometry(335,10,size().width()-345,20);
  edit_tcpport_label->setGeometry(290,10,45,20);
  edit_description_edit->setGeometry(90,32,size().width()-100,20);
  edit_description_label->setGeometry(10,32,80,20);
  edit_output_spin->setGeometry(90,54,60,20);
  edit_output_label->setGeometry(10,54,80,20);
  edit_password_edit->setGeometry(245,54,90,20);
  edit_password_label->setGeometry(160,54,80,20);
  edit_view_button->setGeometry(10,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
