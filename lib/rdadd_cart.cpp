// rdadd_cart.cpp
//
// Add a Rivendell Cart
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdadd_cart.cpp,v 1.8.10.1 2014/05/19 19:31:15 cvs Exp $
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rdsystem.h>
#include <rddb.h>
#include <rdpasswd.h>
#include <rdgroup.h>
#include <rdtextvalidator.h>
#include <rdadd_cart.h>
#include <rdescape_string.h>

RDAddCart::RDAddCart(QString *group,RDCart::Type *type,QString *title,
		     const QString &username,RDSystem *system,
		     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
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
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption("Add Cart");

  //
  // Generate Fonts
  //
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Group
  //
  cart_group_box=new QComboBox(this,"cart_group_box");
  cart_group_box->setGeometry(145,11,160,19);
  QLabel *cart_group_label=
    new QLabel(cart_group_box,tr("&Group:"),this,
	       "cart_group_label");
  cart_group_label->setGeometry(10,11,130,19);
  cart_group_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  cart_group_label->setFont(label_font);
  sql=QString().sprintf("select GROUP_NAME from USER_PERMS \
                         where USER_NAME=\"%s\" order by GROUP_NAME",
			(const char *)username);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cart_group_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==*cart_group) {
      cart_group_box->setCurrentItem(cart_group_box->count()-1);
    }
  }
  delete q;
  connect(cart_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Cart Number
  //
  cart_number_edit=new QLineEdit(this,"cart_number_edit");
  cart_number_edit->setGeometry(145,32,60,19);
  cart_number_edit->setMaxLength(6);
  QIntValidator *validator=new QIntValidator(this,"validator");
  validator->setRange(1,999999);
  cart_number_edit->setValidator(validator);
  QLabel *cart_number_label=
    new QLabel(cart_number_edit,tr("&New Cart Number:"),this,
				       "cart_number_label");
  cart_number_label->setGeometry(10,32,130,19);
  cart_number_label->setFont(label_font);
  cart_number_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Cart Type
  //
  cart_type_box=new QComboBox(this,"cart_type_box");
  cart_type_box->setGeometry(145,53,100,19);
  QLabel *cart_type_label=
    new QLabel(cart_type_box,tr("&New Cart Type:"),this,
				       "cart_type_label");
  cart_type_label->setGeometry(10,53,130,19);
  cart_type_label->setFont(label_font);
  cart_type_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  if((*cart_type==RDCart::All)||(*cart_type==RDCart::Audio)) {
    cart_type_box->insertItem(tr("Audio"));
  }
  if((*cart_type==RDCart::All)||(*cart_type==RDCart::Macro)) {
    cart_type_box->insertItem(tr("Macro"));
  }
  if(*cart_type==RDCart::All) {
    sql=
      QString().sprintf("select DEFAULT_CART_TYPE from GROUPS\
                         where NAME=\"%s\"",
			(const char *)*cart_group);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      cart_type_box->setCurrentItem(q->value(0).toUInt()-1);
    }
    delete q;
  }

  //
  // Cart Title
  //
  cart_title_edit=new QLineEdit(this,"cart_title_edit");
  cart_title_edit->setGeometry(145,73,sizeHint().width()-155,19);
  cart_title_edit->setMaxLength(255);
  //  cart_title_edit->setValidator(text_validator);
  cart_title_edit->setText(tr("[new cart]"));
  QLabel *cart_title_label=
    new QLabel(cart_title_edit,tr("&New Cart Title:"),this,
				       "cart_title_label");
  cart_title_label->setGeometry(10,73,130,19);
  cart_title_label->setFont(label_font);
  cart_title_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(label_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(label_font);
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

  if((sscanf((const char *)cart_number_edit->text(),"%d",&num)!=1)||
     (num<=0)) {
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
    sql=QString().sprintf("select NUMBER from CART where TITLE=\"%s\"",
			 (const char *)RDEscapeString(cart_title_edit->text()));
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
  sql=QString().sprintf("select NUMBER from CART where NUMBER=%u",num);
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
    *cart_type=(RDCart::Type)(cart_type_box->currentItem()+1);
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
