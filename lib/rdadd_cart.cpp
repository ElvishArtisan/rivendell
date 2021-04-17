// rdadd_cart.cpp
//
// Add a Rivendell Cart
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

#include <qpushbutton.h>
#include <qmessagebox.h>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdgroup.h>

#include "rdadd_cart.h"

RDAddCart::RDAddCart(QString *group,RDCart::Type *type,QString *title,
		     const QString &username,const QString &caption,
		     RDSystem *system,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  cart_system=system;
  cart_group=group;
  cart_type=type;
  cart_title=title;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle(caption+" - "+tr("Add Cart"));

  //
  // Group
  //
  cart_group_box=new QComboBox(this);
  cart_group_box->setGeometry(145,11,160,19);
  QLabel *cart_group_label=new QLabel(tr("&Group:"),this);
  cart_group_label->setGeometry(10,11,130,19);
  cart_group_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cart_group_label->setFont(labelFont());
  sql=QString("select `GROUP_NAME` from `USER_PERMS` where ")+
    "`USER_NAME`='"+RDEscapeString(username)+"' order by `GROUP_NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cart_group_box->insertItem(cart_group_box->count(),q->value(0).toString());
    if(q->value(0).toString()==*cart_group) {
      cart_group_box->setCurrentIndex(cart_group_box->count()-1);
    }
  }
  delete q;
  connect(cart_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Cart Number
  //
  cart_number_edit=new QLineEdit(this);
  cart_number_edit->setGeometry(145,32,60,19);
  cart_number_edit->setMaxLength(6);
  QIntValidator *validator=new QIntValidator(this);
  validator->setRange(1,999999);
  cart_number_edit->setValidator(validator);
  QLabel *cart_number_label=new QLabel(tr("&New Cart Number:"),this);
  cart_number_label->setGeometry(10,32,130,19);
  cart_number_label->setFont(labelFont());
  cart_number_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart Type
  //
  cart_type_box=new QComboBox(this);
  cart_type_box->setGeometry(145,53,100,19);
  QLabel *cart_type_label=new QLabel(tr("&New Cart Type:"),this);
  cart_type_label->setGeometry(10,53,130,19);
  cart_type_label->setFont(labelFont());
  cart_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  if((*cart_type==RDCart::All)||(*cart_type==RDCart::Audio)) {
    cart_type_box->insertItem(cart_type_box->count(),tr("Audio"));
  }
  if((*cart_type==RDCart::All)||(*cart_type==RDCart::Macro)) {
    cart_type_box->insertItem(cart_type_box->count(),tr("Macro"));
  }
  if(*cart_type==RDCart::All) {
    sql=QString("select `DEFAULT_CART_TYPE` from `GROUPS` where ")+
      "`NAME`='"+RDEscapeString(*cart_group)+"'";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      cart_type_box->setCurrentIndex(q->value(0).toUInt()-1);
    }
    delete q;
  }

  //
  // Cart Title
  //
  cart_title_edit=new QLineEdit(this);
  cart_title_edit->setGeometry(145,73,sizeHint().width()-155,19);
  cart_title_edit->setMaxLength(255);
  //  cart_title_edit->setValidator(text_validator);
  cart_title_edit->setText(tr("[new cart]"));
  QLabel *cart_title_label=new QLabel(tr("&New Cart Title:"),this);
  cart_title_label->setGeometry(10,73,130,19);
  cart_title_label->setFont(labelFont());
  cart_title_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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

  groupActivatedData(cart_group_box->currentText());
}


QSize RDAddCart::sizeHint() const
{
  return QSize(400,160);
} 


QSizePolicy RDAddCart::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDAddCart::groupActivatedData(const QString &groupname)
{
  unsigned cartnum=0;

  RDGroup *group=new RDGroup(groupname);
  if((cartnum=group->nextFreeCart())==0) {
    cart_number_edit->clear();
    if(group->enforceCartRange()) {
      QMessageBox::warning(this,tr("No Available Cart Numbers"),
	     tr("There are no more available cart numbers for the group!"));
    }
  }
  else {
    cart_number_edit->setText(QString().sprintf("%06u",cartnum));
  }
  delete group;
}


void RDAddCart::okData()
{
  RDSqlQuery *q;
  QString sql;
  unsigned num;
  RDGroup *group=new RDGroup(cart_group_box->currentText());
  bool ok=false;

  num=cart_number_edit->text().toUInt(&ok);
  if((!ok)||(num==0)) {
    QMessageBox::warning(this,tr("Invalid Number"),tr("Invalid Cart Number!"));
    return;
  }
  if(cart_title_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Title Required"),
			 tr("You must enter a cart title!"));
    return;
  }
  RDSystem *system=new RDSystem();
  if(!system->allowDuplicateCartTitles()) {
    sql=QString("select `NUMBER` from `CART` where ")+
      "`TITLE`='"+RDEscapeString(cart_title_edit->text())+"'";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      QMessageBox::warning(this,tr("Duplicate Title"),
			   tr("The cart title must be unique!"));
      delete q;
      return;
    }
    delete q;
  }
  delete system;
  if(group->enforceCartRange()) {
    if((num<group->defaultLowCart())||(num>group->defaultHighCart())) {
      QMessageBox::warning(this,tr("Invalid Number"),
      tr("The cart number is outside of the permitted range for this group!"));
      delete group;
      return;
    }
  }
  sql=QString().sprintf("select `NUMBER` from `CART` where `NUMBER`=%u",num);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::information(this,tr("Cart Exists"),
			     tr("This cart already exists."),
			     QMessageBox::Ok);
    delete q;
    delete group;
    return;
  }
  delete q;
  delete group;
  *cart_group=cart_group_box->currentText();
  if(*cart_type==RDCart::All) {
    *cart_type=(RDCart::Type)(cart_type_box->currentIndex()+1);
  }
  *cart_title=cart_title_edit->text();
  done(num);
}


void RDAddCart::cancelData()
{
  done(-1);
}


void RDAddCart::closeEvent(QCloseEvent *e)
{
  cancelData();
}
