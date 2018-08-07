// autofill_carts.cpp
//
// Edit a List of Autofill Carts
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <q3listbox.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rduser.h>

#include "autofill_carts.h"
#include "globals.h"

AutofillCarts::AutofillCarts(RDSvc *svc,QWidget *parent)
  : QDialog(parent,"",true)
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

  setCaption(tr("Autofill Carts - Service:")+" "+svc_svc->name());

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
  svc_cart_list=new Q3ListView(this);
  svc_cart_list->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-110);
  svc_cart_list->setAllColumnsShowFocus(true);
  svc_cart_list->setItemMargin(5);
  svc_cart_list->addColumn(tr("Cart"));
  svc_cart_list->setColumnAlignment(0,Qt::AlignCenter);
  svc_cart_list->addColumn(tr("Length"));
  svc_cart_list->setColumnAlignment(1,Qt::AlignRight);
  svc_cart_list->addColumn(tr("Title"));
  svc_cart_list->setColumnAlignment(2,Qt::AlignLeft);
  svc_cart_list->addColumn(tr("Artist"));
  svc_cart_list->setColumnAlignment(3,Qt::AlignLeft);
  svc_cart_list->setSortColumn(1);

  //
  // Add Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(20,sizeHint().height()-90,60,40);
  button->setFont(font);
  button->setText(tr("&Add"));
  connect(button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Delete Button
  //
  button=new QPushButton(this);
  button->setGeometry(90,sizeHint().height()-90,60,40);
  button->setFont(font);
  button->setText(tr("&Delete"));
  connect(button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
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
			     rda->user()->name(),rda->user()->password())<0) {
    return;
  }
  RDCart *rdcart=new RDCart(cart);
  Q3ListViewItem *item=new Q3ListViewItem(svc_cart_list);
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
  Q3ListViewItem *item=svc_cart_list->selectedItem();
  if(item==NULL) {
    return;
  }
  delete item;
}


void AutofillCarts::okData()
{
  QString sql=QString("delete from AUTOFILLS where ")+
    "SERVICE=\""+RDEscapeString(svc_svc->name())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  Q3ListViewItem *item=svc_cart_list->firstChild();
  while(item!=NULL) {
    sql=QString("insert into AUTOFILLS set ")+
      "SERVICE=\""+RDEscapeString(svc_svc->name())+"\","+
      QString().sprintf("CART_NUMBER=%u",item->text(0).toUInt());
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
  Q3ListViewItem *item;

  svc_cart_list->clear();
  QString sql=QString("select ")+
    "AUTOFILLS.CART_NUMBER,"+   // 00
    "CART.FORCED_LENGTH,"+      // 01
    "CART.TITLE,CART.ARTIST "+  // 02
    "from AUTOFILLS left join CART "+
    "on AUTOFILLS.CART_NUMBER=CART.NUMBER where "+
    "SERVICE=\""+RDEscapeString(svc_svc->name())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new Q3ListViewItem(svc_cart_list);
    item->setText(0,QString().sprintf("%06u",q->value(0).toUInt()));
    item->setText(1,RDGetTimeLength(q->value(1).toInt(),false,true));
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
  }
  delete q;
}
