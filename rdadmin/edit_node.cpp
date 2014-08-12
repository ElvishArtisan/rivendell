// edit_node.cpp
//
// Edit a Rivendell LiveWire Node
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_node.cpp,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#include <qmessagebox.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include <edit_node.h>
#include <view_node_info.h>


EditNode::EditNode(int *id,RDMatrix *matrix,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_id=id;
  edit_matrix=matrix;
  edit_password_changed=false;
  setCaption(tr("Edit LiveWire Node"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

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
  edit_hostname_edit=new QLineEdit(this,"edit_hostname_edit");
  edit_hostname_edit->setGeometry(90,10,190,20);
  QLabel *label=
    new QLabel(edit_hostname_edit,tr("Hostname: "),this,"edit_hostname_label");
  label->setGeometry(10,10,80,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Node TCP Port
  //
  edit_tcpport_spin=new QSpinBox(this,"edit_tcpport_spin");
  edit_tcpport_spin->setGeometry(335,10,sizeHint().width()-345,20);
  edit_tcpport_spin->setRange(0,0xFFFF);
  label=new QLabel(edit_tcpport_spin,tr("Port: "),this,"edit_tcpport_label");
  label->setGeometry(290,10,45,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Node Description
  //
  edit_description_edit=new QLineEdit(this,"edit_description_edit");
  edit_description_edit->setGeometry(90,32,sizeHint().width()-100,20);
  label=new QLabel(edit_description_edit,tr("Description: "),
		   this,"edit_description_label");
  label->setGeometry(10,32,80,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Base Output
  //
  edit_output_spin=new QSpinBox(this,"edit_output_spin");
  edit_output_spin->setGeometry(90,54,60,20);
  edit_output_spin->setRange(0,0x7FFF);
  edit_output_spin->setSpecialValueText(tr("None"));
  label=
    new QLabel(edit_output_spin,tr("First Output: "),this,"edit_output_label");
  label->setGeometry(10,54,80,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Node Password
  //
  edit_password_edit=new QLineEdit(this,"edit_password_edit");
  edit_password_edit->setGeometry(245,54,90,20);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setText("********");
  connect(edit_password_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(passwordChangedData(const QString &)));
  label=
    new QLabel(edit_password_edit,tr("Password: "),this,"edit_password_label");
  label->setGeometry(160,54,80,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  //  View Node Info Button
  //
  QPushButton *button=new QPushButton(this,"cancel_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&View Node\nInfo"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
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
    sql=QString().sprintf("select HOSTNAME,TCP_PORT,DESCRIPTION,PASSWORD,\
                           BASE_OUTPUT \
                           from SWITCHER_NODES \
                           where ID=%d",*edit_id);
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
    sql=QString().sprintf("select ID from SWITCHER_NODES \
                           where (STATION_NAME=\"%s\")&&\
                           (MATRIX=%d)&&\
                           (HOSTNAME=\"%s\")&&\
                           (TCP_PORT=%d)",
			  (const char *)edit_matrix->station(),
			  edit_matrix->matrix(),
			  (const char *)edit_hostname_edit->text(),
			  edit_tcpport_spin->value());
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
    sql=QString().sprintf("insert into SWITCHER_NODES set \
                           ID=%d,\
                           STATION_NAME=\"%s\",\
                           MATRIX=%d,\
                           HOSTNAME=\"%s\",\
                           TCP_PORT=%d,\
                           DESCRIPTION=\"%s\",\
                           BASE_OUTPUT=%d,\
                           PASSWORD=\"%s\"",
			  *edit_id,
			  (const char *)edit_matrix->station(),
			  edit_matrix->matrix(),
			  (const char *)edit_hostname_edit->text(),
			  edit_tcpport_spin->value(),
			  (const char *)edit_description_edit->text(),
			  edit_output_spin->value(),
			  (const char *)RDEscapeString(edit_password));
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    sql=QString().sprintf("update SWITCHER_NODES set HOSTNAME=\"%s\",\
                           TCP_PORT=%d,\
                           DESCRIPTION=\"%s\",\
                           BASE_OUTPUT=%d,\
                           PASSWORD=\"%s\" \
                           where ID=%d",
			  (const char *)edit_hostname_edit->text(),
			  edit_tcpport_spin->value(),
			  (const char *)edit_description_edit->text(),
			  edit_output_spin->value(),
			  (const char *)RDEscapeString(edit_password),
			  *edit_id);
    q=new RDSqlQuery(sql);
    delete q;
  }

  done(0);
}


void EditNode::cancelData()
{
  done(1);
}
