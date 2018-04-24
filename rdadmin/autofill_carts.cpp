// autofill_carts.cpp
//
// Edit a List of Autofill Carts
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDialog>
#include <QPushButton>

#include <rd.h>
#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rduser.h>

#include "autofill_carts.h"
#include "globals.h"

AutofillCarts::AutofillCarts(RDSvc *svc,QWidget *parent)
  : QDialog(parent)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  svc_svc=svc;

  setWindowTitle("RDAdmin - "+tr("Autofill Carts")+" - "+tr("Service")+": "+
		 svc_svc->name());

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
  svc_cart_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "AUTOFILLS.CART_NUMBER,"+
    "CART.FORCED_LENGTH,"+
    "CART.TITLE,"+
    "CART.ARTIST "+
    "from AUTOFILLS left join CART "+
    "on AUTOFILLS.CART_NUMBER=CART.NUMBER where "+
    "SERVICE=\""+RDEscapeString(svc_svc->name())+"\"";
  svc_cart_model->setQuery(sql);
  svc_cart_model->setHeaderData(0,Qt::Horizontal,tr("Cart"),Qt::DisplayRole);
  svc_cart_model->setFieldType(0,RDSqlTableModel::CartNumberType);
  svc_cart_model->setHeaderData(1,Qt::Horizontal,tr("Length"),Qt::DisplayRole);
  svc_cart_model->setFieldType(1,RDSqlTableModel::LengthType);
  svc_cart_model->setHeaderData(2,Qt::Horizontal,tr("Title"),Qt::DisplayRole);
  svc_cart_model->setHeaderData(3,Qt::Horizontal,tr("Artist"),Qt::DisplayRole);
  svc_cart_view=new RDTableView(this);
  svc_cart_view->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-110);
  svc_cart_view->setModel(svc_cart_model);
  svc_cart_view->resizeColumnsToContents();

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
  int cartnum=0;
  if(admin_cart_dialog->exec(&cartnum,RDCart::Audio,NULL,0,
			     rda->user()->name(),rda->user()->password())<0) {
    return;
  }
  QString sql=QString("select ")+
    "NUMBER,"+
    "FORCED_LENGTH,"+
    "TITLE,"+
    "ARTIST "+
    "from CART where "+
    QString().sprintf("NUMBER=%u",cartnum);
  svc_cart_model->insertRows(0,sql);
  svc_cart_view->select(0,cartnum);
}


void AutofillCarts::deleteData()
{
  QItemSelectionModel *s=svc_cart_view->selectionModel();
  if(s->hasSelection()) {
    svc_cart_model->removeRow(s->selectedRows()[0].row());
  }
}


void AutofillCarts::okData()
{
  QString sql=QString("delete from AUTOFILLS where ")+
    "SERVICE=\""+RDEscapeString(svc_svc->name())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  for(int i=0;i<svc_cart_model->rowCount();i++) {
    sql=QString("insert into AUTOFILLS set ")+
      "SERVICE=\""+RDEscapeString(svc_svc->name())+"\","+
      QString().sprintf("CART_NUMBER=%u",svc_cart_model->data(i,0).toUInt());
    q=new RDSqlQuery(sql);
    delete q;
  }

  done(0);
}


void AutofillCarts::cancelData()
{
  done(1);
}
