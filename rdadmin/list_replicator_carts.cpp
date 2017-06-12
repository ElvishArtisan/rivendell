// list_replicator_carts.cpp
//
// List Rivendell Replicator Carts
//
//   (C) Copyright 2002-2008,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <iostream> //TODO: REMOVE

#include <QAbstractItemView>
#include <QHeaderView>
#include <QIcon>
#include <QResizeEvent>

#include <rdcart.h>
#include <rdescape_string.h>
#include <rdreplicator.h>
#include <rdtextfile.h>

#include "edit_replicator.h"
#include "add_replicator.h"
#include "list_replicator_carts.h"

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"

ListReplicatorCarts::ListReplicatorCarts(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  // Create Icons
  //
  list_playout_map=new QPixmap(play_xpm);
  list_macro_map=new QPixmap(rml5_xpm);

  //
  // Refresh Timer
  //
  list_refresh_timer=new QTimer(this);
  connect(list_refresh_timer,SIGNAL(timeout()),this,SLOT(refreshTimeoutData()));

  //
  //  Repost Button
  //
  list_repost_button=new QPushButton(this);
  list_repost_button->setFont(font);
  list_repost_button->setText(tr("&Repost"));
  connect(list_repost_button,SIGNAL(clicked()),this,SLOT(repostData()));

  //
  //  Repost All Button
  //
  list_repost_all_button=new QPushButton(this);
  list_repost_all_button->setFont(font);
  list_repost_all_button->setText(tr("Repost\n&All"));
  connect(list_repost_all_button,SIGNAL(clicked()),this,SLOT(repostAllData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Replicator List
  //
  list_view=new RDTableWidget(this);
  list_view->setFont(list_font);
  list_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_view->setColumnCount(5);
  QStringList headings={" ",tr("CART"),tr("TITLE"),tr("LAST POSTED"),tr("POSTED FILENAME")};
  list_view->setHorizontalHeaderLabels(headings);
  list_view->verticalHeader()->setVisible(false);
  list_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  QLabel *list_box_label=new QLabel(list_view,tr("&Active Carts:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
}


ListReplicatorCarts::~ListReplicatorCarts()
{
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
  list_replicator_name=replname;
  setWindowTitle("RDAdmin - "+replname+tr(" Replicator Carts"));
  RefreshList();
  list_refresh_timer->start(5000,true);
  return QDialog::exec();
}


void ListReplicatorCarts::repostData()
{
  if(list_view->selectedItems().isEmpty()) {
      return;
  }
  QTableWidgetItem *item=list_view->selectedItems().at(0);
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("update REPL_CART_STATE set REPOST=\"Y\" \
                         where ID=%d",item->data(Qt::UserRole).toInt());
  q=new RDSqlQuery(sql);
  delete q;
}


void ListReplicatorCarts::repostAllData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update REPL_CART_STATE set ")+
    "REPOST=\"Y\" where "+
    "REPLICATOR_NAME=\""+RDEscapeString(list_replicator_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void ListReplicatorCarts::closeData()
{
  list_refresh_timer->stop();
  done(0);
}


void ListReplicatorCarts::refreshTimeoutData()
{
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *item;

  sql=QString("select ")+
    "ID,"+
    "ITEM_DATETIME "+
    "from REPL_CART_STATE where "+
    "REPLICATOR_NAME=\""+RDEscapeString(list_replicator_name)+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=list_view->item(0,0);
    while(item!=NULL) {
      if(item->data(Qt::UserRole)==q->value(0).toInt()) {
        QTableWidgetItem *time=list_view->item(item->row(),3);
        time->setText(q->value(1).
                toDateTime().toString("hh:mm:ss dd/MM/yyyy"));
        break;
      }
      item=list_view->item(item->row()+1,0);
    }
  }
  delete q;
  list_refresh_timer->start(5000,true);
}


void ListReplicatorCarts::resizeEvent(QResizeEvent *e)
{
  list_repost_button->setGeometry(size().width()-90,30,80,50);
  list_repost_all_button->setGeometry(size().width()-90,90,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListReplicatorCarts::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *icon;
  QTableWidgetItem *cart;
  QTableWidgetItem *title;
  QTableWidgetItem *posted;
  QTableWidgetItem *filename;

  list_view->clear();
  QStringList headings={" ",tr("CART"),tr("TITLE"),tr("LAST POSTED"),tr("POSTED FILENAME")};
  list_view->setHorizontalHeaderLabels(headings);
  sql=QString("select ")+
    "REPL_CART_STATE.ID,"+
    "CART.TYPE,"+
    "REPL_CART_STATE.CART_NUMBER,"+
    "CART.TITLE,"+
    "REPL_CART_STATE.ITEM_DATETIME,"+
    "REPL_CART_STATE.POSTED_FILENAME "+
    "from REPL_CART_STATE left join CART "+
    "on REPL_CART_STATE.CART_NUMBER=CART.NUMBER where "+
    "REPLICATOR_NAME=\""+RDEscapeString(list_replicator_name)+"\"";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    icon=new QTableWidgetItem();
    icon->setData(q->value(0).toInt(), Qt::UserRole);
    cart=new QTableWidgetItem(QString().sprintf("%06u",q->value(2).toUInt()));
    switch((RDCart::Type)q->value(1).toInt()) {
    case RDCart::Audio:
      icon->setIcon(QIcon(*list_playout_map));
      break;

    case RDCart::Macro:
      icon->setIcon(QIcon(*list_macro_map));
      break;

    case RDCart::All:
      break;
    }
    title=new QTableWidgetItem(q->value(3).toString());
    posted=new QTableWidgetItem(q->value(4).toDateTime().toString("hh:mm:ss dd/MM/yyyy"));
    filename=new QTableWidgetItem(q->value(5).toString());
    
    int newRow = list_view->rowCount();
    list_view->setItem(newRow,0,icon);
    list_view->setItem(newRow,1,cart);
    list_view->setItem(newRow,2,title);
    list_view->setItem(newRow,3,posted);
    list_view->setItem(newRow,4,filename);
  }
  delete q;
}
