// list_replicator_carts.cpp
//
// List Rivendell Replicators
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_replicator_carts.cpp,v 1.1 2011/10/17 18:48:40 cvs Exp $
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
#include <qbuttongroup.h>
#include <rddb.h>

#include <rdcart.h>
#include <rdtextfile.h>
#include <rdescape_string.h>
#include <rdreplicator.h>

#include <list_replicator_carts.h>
#include <edit_replicator.h>
#include <add_replicator.h>

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"

ListReplicatorCarts::ListReplicatorCarts(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

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
  list_repost_button=new QPushButton(this,"list_repost_button");
  list_repost_button->setFont(font);
  list_repost_button->setText(tr("&Repost"));
  connect(list_repost_button,SIGNAL(clicked()),this,SLOT(repostData()));

  //
  //  Repost All Button
  //
  list_repost_all_button=new QPushButton(this,"list_repost_button");
  list_repost_all_button->setFont(font);
  list_repost_all_button->setText(tr("Repost\n&All"));
  connect(list_repost_all_button,SIGNAL(clicked()),this,SLOT(repostAllData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this,"list_close_button");
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Replicator List
  //
  list_view=new RDListView(this,"list_view");
  list_view->setFont(list_font);
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(" ");
  list_view->addColumn(tr("CART"));
  list_view->addColumn(tr("TITLE"));
  list_view->addColumn(tr("LAST POSTED"));
  list_view->addColumn(tr("POSTED FILENAME"));
  QLabel *list_box_label=new QLabel(list_view,tr("&Active Carts:"),
				    this,"list_box_label");
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
  setCaption(replname+tr(" Replicator Carts"));
  RefreshList();
  list_refresh_timer->start(5000,true);
  return QDialog::exec();
}


void ListReplicatorCarts::repostData()
{
  QString sql;
  RDSqlQuery *q;

  RDListViewItem *item=(RDListViewItem *)list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  sql=QString().sprintf("update REPL_CART_STATE set REPOST=\"Y\" \
                         where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  delete q;
}


void ListReplicatorCarts::repostAllData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("update REPL_CART_STATE set REPOST=\"Y\" \
                         where REPLICATOR_NAME=\"%s\"",
			(const char *)list_replicator_name);
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
  RDListViewItem *item;

  sql=QString().sprintf("select ID,ITEM_DATETIME from REPL_CART_STATE \
                         where REPLICATOR_NAME=\"%s\"",
			(const char *)RDEscapeString(list_replicator_name));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=(RDListViewItem *)list_view->firstChild();
    while(item!=NULL) {
      if(item->id()==q->value(0).toInt()) {
	item->setText(3,q->value(1).
		      toDateTime().toString("hh:mm:ss dd/MM/yyyy"));
	break;
      }
      item=(RDListViewItem *)item->nextSibling();
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
  RDListViewItem *item;

  list_view->clear();
  sql=QString().sprintf("select REPL_CART_STATE.ID,\
                         CART.TYPE,REPL_CART_STATE.CART_NUMBER,\
                         CART.TITLE,\
                         REPL_CART_STATE.ITEM_DATETIME,\
                         REPL_CART_STATE.POSTED_FILENAME \
                         from REPL_CART_STATE left join CART \
                         on REPL_CART_STATE.CART_NUMBER=CART.NUMBER \
                         where REPLICATOR_NAME=\"%s\"",
			(const char *)RDEscapeString(list_replicator_name));
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_view);
    item->setId(q->value(0).toInt());
    item->setText(1,QString().sprintf("%06u",q->value(2).toUInt()));
    switch((RDCart::Type)q->value(1).toInt()) {
    case RDCart::Audio:
      item->setPixmap(0,*list_playout_map);
      break;

    case RDCart::Macro:
      item->setPixmap(0,*list_macro_map);
      break;

    case RDCart::All:
      break;
    }
    item->setText(2,q->value(3).toString());
    item->setText(3,q->value(4).toDateTime().toString("hh:mm:ss dd/MM/yyyy"));
    item->setText(4,q->value(5).toString());
  }
  delete q;
}
