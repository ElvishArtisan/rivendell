// list_replicator_carts.cpp
//
// List Rivendell Replicator Carts
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

#include <rdescape_string.h>

#include "add_replicator.h"
#include "edit_replicator.h"
#include "list_replicator_carts.h"

ListReplicatorCarts::ListReplicatorCarts(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  //  Repost Button
  //
  list_repost_button=new QPushButton(this);
  list_repost_button->setFont(buttonFont());
  list_repost_button->setText(tr("&Repost"));
  connect(list_repost_button,SIGNAL(clicked()),this,SLOT(repostData()));

  //
  //  Repost All Button
  //
  list_repost_all_button=new QPushButton(this);
  list_repost_all_button->setFont(buttonFont());
  list_repost_all_button->setText(tr("Repost\n&All"));
  connect(list_repost_all_button,SIGNAL(clicked()),this,SLOT(repostAllData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Replicator List
  //
  list_view=new RDTableView(this);
  list_model=new RDReplCartListModel(this);
  list_model->setFont(defaultFont());
  list_model->setPalette(palette());
  list_view->setModel(list_model);
  connect(list_model,SIGNAL(modelReset()),
	  list_view,SLOT(resizeColumnsToContents()));

  QLabel *list_box_label=new QLabel(list_view,tr("&Active Carts:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,11,85,19);
}


ListReplicatorCarts::~ListReplicatorCarts()
{
  delete list_model;
}


QSize ListReplicatorCarts::sizeHint() const
{
  return QSize(500,400);
} 


QSizePolicy ListReplicatorCarts::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int ListReplicatorCarts::exec(const QString &replname)
{
  setWindowTitle("RDAdmin - "+replname+tr(" Replicator Carts"));
  list_model->setReplicatorName(replname);

  return QDialog::exec();
}


void ListReplicatorCarts::repostData()
{
  QModelIndexList rows=list_view->selectionModel()->selectedRows();

  if(rows.size()==0) {
    return;
  }
  QString sql=QString("update REPL_CART_STATE set ")+
    "REPOST=\"Y\" where ";
  for(int i=0;i<rows.size();i++) {
    sql+=QString().sprintf("(ID=%u)||",list_model->cartId(rows.at(i)));
  }
  sql=sql.left(sql.length()-2);
  RDSqlQuery::apply(sql);
}


void ListReplicatorCarts::repostAllData()
{
  QString sql=QString("update REPL_CART_STATE set ")+
    "REPOST=\"Y\" where "+
    "REPLICATOR_NAME=\""+RDEscapeString(list_model->replicatorName())+"\"";
  RDSqlQuery::apply(sql);
}


void ListReplicatorCarts::closeData()
{
  done(true);
}


void ListReplicatorCarts::resizeEvent(QResizeEvent *e)
{
  list_repost_button->setGeometry(size().width()-90,30,80,50);
  list_repost_all_button->setGeometry(size().width()-90,90,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
