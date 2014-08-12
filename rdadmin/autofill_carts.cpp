// autofill_carts.cpp
//
// Edit a List of Autofill Carts
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: autofill_carts.cpp,v 1.16.8.1 2012/11/26 20:19:37 cvs Exp $
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

#include <rddb.h>
#include <rdconf.h>
#include <rd.h>
#include <rduser.h>
#include <rdcart_dialog.h>

#include <globals.h>
#include <autofill_carts.h>


AutofillCarts::AutofillCarts(RDSvc *svc,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  svc_svc=svc;

  str=QString(tr("Autofill Carts - Service:"));
  setCaption(QString().sprintf("%s %s",(const char *)str,
			       (const char *)svc_svc->name()));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont section_font=QFont("Helvetica",14,QFont::Bold);
  section_font.setPixelSize(14);

  //
  // Cart List
  //
  svc_cart_list=new QListView(this,"svc_cart_list");
  svc_cart_list->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-110);
  svc_cart_list->setAllColumnsShowFocus(true);
  svc_cart_list->setItemMargin(5);
  svc_cart_list->addColumn(tr("Cart"));
  svc_cart_list->setColumnAlignment(0,AlignCenter);
  svc_cart_list->addColumn(tr("Length"));
  svc_cart_list->setColumnAlignment(1,AlignRight);
  svc_cart_list->addColumn(tr("Title"));
  svc_cart_list->setColumnAlignment(2,AlignLeft);
  svc_cart_list->addColumn(tr("Artist"));
  svc_cart_list->setColumnAlignment(3,AlignLeft);
  svc_cart_list->setSortColumn(1);

  //
  // Add Button
  //
  QPushButton *button=new QPushButton(this,"add_button");
  button->setGeometry(20,sizeHint().height()-90,60,40);
  button->setFont(font);
  button->setText(tr("&Add"));
  connect(button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Delete Button
  //
  button=new QPushButton(this,"delete_button");
  button->setGeometry(90,sizeHint().height()-90,60,40);
  button->setFont(font);
  button->setText(tr("&Delete"));
  connect(button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  RefreshList();
}


AutofillCarts::~AutofillCarts()
{
}


QSize AutofillCarts::sizeHint() const
{
  return QSize(375,310);
} 


QSizePolicy AutofillCarts::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AutofillCarts::addData()
{
  int cart=0;
  if(admin_cart_dialog->exec(&cart,RDCart::Audio,NULL,0,
			     admin_user->name(),admin_user->password())<0) {
    return;
  }
  RDCart *rdcart=new RDCart(cart);
  QListViewItem *item=new QListViewItem(svc_cart_list);
  item->setText(0,QString().sprintf("%06d",cart));
  item->setText(1,RDGetTimeLength(rdcart->forcedLength(),false,true));
  item->setText(2,rdcart->title());
  item->setText(3,rdcart->artist());
  svc_cart_list->setSelected(item,true);
  svc_cart_list->ensureItemVisible(item);
  delete rdcart;
}


void AutofillCarts::deleteData()
{
  QListViewItem *item=svc_cart_list->selectedItem();
  if(item==NULL) {
    return;
  }
  delete item;
}


void AutofillCarts::okData()
{
  QString sql=QString().sprintf("delete from AUTOFILLS where SERVICE=\"%s\"",
				(const char *)svc_svc->name());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  QListViewItem *item=svc_cart_list->firstChild();
  while(item!=NULL) {
    sql=QString().sprintf("insert into AUTOFILLS set SERVICE=\"%s\",\
                           CART_NUMBER=%u",(const char *)svc_svc->name(),
			  item->text(0).toUInt());
    q=new RDSqlQuery(sql);
    delete q;
    item=item->nextSibling();
  }
  done(0);
}


void AutofillCarts::cancelData()
{
  done(1);
}


void AutofillCarts::RefreshList()
{
  QListViewItem *item;

  svc_cart_list->clear();
  QString sql=QString().sprintf("select AUTOFILLS.CART_NUMBER,\
                                 CART.FORCED_LENGTH,CART.TITLE,CART.ARTIST\
                                 from AUTOFILLS left join CART\
                                 on AUTOFILLS.CART_NUMBER=CART.NUMBER\
                                 where SERVICE=\"%s\"",
				(const char *)svc_svc->name());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new QListViewItem(svc_cart_list);
    item->setText(0,QString().sprintf("%06u",q->value(0).toUInt()));
    item->setText(1,RDGetTimeLength(q->value(1).toInt(),false,true));
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
  }
  delete q;
}
