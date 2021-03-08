// autofill_carts.cpp
//
// Edit a List of Autofill Carts
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

#include <rd.h>
#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "autofill_carts.h"
#include "globals.h"

AutofillCarts::AutofillCarts(RDSvc *svc,QWidget *parent)
  : RDDialog(parent)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  svc_svc=svc;

  setWindowTitle("RDAdmin - "+tr("Autofill Carts - Service:")+" "+svc_svc->name());

  //
  // Cart List
  //
  svc_cart_view=new RDTableView(this);
  svc_cart_model=new RDLibraryModel(this);
  svc_cart_model->setFont(font());
  svc_cart_model->setPalette(palette());
  svc_cart_view->setModel(svc_cart_model);
  connect(svc_cart_model,SIGNAL(modelReset()),
	  svc_cart_view,SLOT(resizeColumnsToContents()));

  //
  // Add Button
  //
  svc_add_button=new QPushButton(this);
  svc_add_button->setFont(buttonFont());
  svc_add_button->setText(tr("Add"));
  connect(svc_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Remove Button
  //
  svc_remove_button=new QPushButton(this);
  svc_remove_button->setFont(buttonFont());
  svc_remove_button->setText(tr("Remove"));
  connect(svc_remove_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  svc_close_button=new QPushButton(this);
  svc_close_button->setFont(buttonFont());
  svc_close_button->setText(tr("Close"));
  connect(svc_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  QString sql=QString("left join AUTOFILLS ")+
    "on CART.NUMBER=AUTOFILLS.CART_NUMBER where "+
    "AUTOFILLS.SERVICE=\""+RDEscapeString(svc_svc->name())+"\"";
  svc_cart_model->setFilterSql(sql);
}


AutofillCarts::~AutofillCarts()
{
}


QSize AutofillCarts::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy AutofillCarts::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AutofillCarts::addData()
{
  QString sql;
  int cartnum=0;

  if(admin_cart_dialog->exec(&cartnum,RDCart::Audio,svc_svc->name(),NULL)) {
    sql=QString("insert into AUTOFILLS set ")+
      "SERVICE=\""+RDEscapeString(svc_svc->name())+"\","+
      QString().sprintf("CART_NUMBER=%d",cartnum);
    RDSqlQuery::apply(sql);
    QModelIndex index=svc_cart_model->addCart(cartnum);
    if(index.isValid()) {
      svc_cart_view->selectRow(index.row());
    }
  }
}


void AutofillCarts::deleteData()
{
  QModelIndexList rows=svc_cart_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  QString sql=QString("delete from AUTOFILLS where ")+
    "SERVICE=\""+RDEscapeString(svc_svc->name())+"\" && "+
    QString().sprintf("CART_NUMBER=%u",
		      svc_cart_model->cartNumber(rows.first()));
  RDSqlQuery::apply(sql);
  svc_cart_model->removeCart(rows.first());
}


void AutofillCarts::closeData()
{
  done(true);
}


void AutofillCarts::resizeEvent(QResizeEvent *e)
{
  svc_cart_view->setGeometry(10,10,size().width()-20,size().height()-110);
  svc_add_button->setGeometry(20,size().height()-90,80,50);
  svc_remove_button->setGeometry(110,size().height()-90,80,50);

  svc_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
