// list_replicators.cpp
//
// List Rivendell Replicators
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_replicators.cpp,v 1.3 2011/10/17 18:48:40 cvs Exp $
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

#include <list_replicators.h>
#include <list_replicator_carts.h>
#include <edit_replicator.h>
#include <add_replicator.h>

ListReplicators::ListReplicators(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("Rivendell Replicators"));

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
  //  Add Button
  //
  list_add_button=new QPushButton(this,"list_add_button");
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this,"list_edit_button");
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this,"list_delete_button");
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  List Carts Button
  //
  list_list_button=new QPushButton(this,"list_list_button");
  list_list_button->setFont(font);
  list_list_button->setText(tr("&List\nCarts"));
  connect(list_list_button,SIGNAL(clicked()),this,SLOT(listData()));

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
  list_replicators_view=new RDListView(this,"list_replicators_view");
  list_replicators_view->setFont(list_font);
  list_replicators_view->setAllColumnsShowFocus(true);
  list_replicators_view->setItemMargin(5);
  list_replicators_view->addColumn(tr("NAME"));
  list_replicators_view->addColumn(tr("TYPE"));
  list_replicators_view->addColumn(tr("DESCRIPTION"));
  list_replicators_view->addColumn(tr("HOST"));
  QLabel *list_box_label=new QLabel(list_replicators_view,tr("&Replicators:"),
				    this,"list_box_label");
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
  connect(list_replicators_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  RefreshList();
}


ListReplicators::~ListReplicators()
{
}


QSize ListReplicators::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListReplicators::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReplicators::addData()
{
  QString name;

  AddReplicator *d=new AddReplicator(&name,this);
  if(d->exec()<0) {
    delete d;
    return;
  }
  delete d;
  RDListViewItem *item=new RDListViewItem(list_replicators_view);
  item->setText(0,name);
  RefreshItem(item);
  item->setSelected(true);
  list_replicators_view->setCurrentItem(item);
  list_replicators_view->ensureItemVisible(item);
}


void ListReplicators::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_replicators_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditReplicator *d=new EditReplicator(item->text(0),this);
  if(d->exec()==0) {
    RefreshItem(item);
  }
  delete d;
}


void ListReplicators::deleteData()
{
  RDListViewItem *item=(RDListViewItem *)list_replicators_view->selectedItem();
  if(item==NULL) {
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;

  QString name=RDEscapeString(item->text(0));

  str=QString(tr("Are you sure you want to delete replicator"));
  warning+=QString().sprintf("%s \"%s\"?",(const char *)str,
			     (const char *)item->text(0));
  switch(QMessageBox::warning(this,tr("Delete Replicator"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
      case QMessageBox::No:
      case QMessageBox::NoButton:
	return;

      default:
	break;
  }

  //
  // Delete Group Assignments
  //
  sql=QString().sprintf("delete from REPLICATOR_MAP \
                         where REPLICATOR_NAME=\"%s\"",
			(const char *)name);
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete State Records
  //
  sql=QString().sprintf("delete from REPL_CART_STATE \
                         where REPLICATOR_NAME=\"%s\"",
			(const char *)name);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from REPL_CUT_STATE \
                         where REPLICATOR_NAME=\"%s\"",
			(const char *)name);
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete from Replicator List
  //
  sql=QString().sprintf("delete from REPLICATORS where NAME=\"%s\"",
			(const char *)name);
  q=new RDSqlQuery(sql);
  delete q;
  delete item;
}


void ListReplicators::listData()
{
  RDListViewItem *item=(RDListViewItem *)list_replicators_view->selectedItem();
  if(item==NULL) {
    return;
  }
  ListReplicatorCarts *d=new ListReplicatorCarts(this);
  d->exec(item->text(0));
  delete d;
}


void ListReplicators::doubleClickedData(QListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListReplicators::closeData()
{
  done(0);
}


void ListReplicators::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_list_button->setGeometry(size().width()-90,250,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_replicators_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListReplicators::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_replicators_view->clear();
  sql="select NAME,TYPE_ID,DESCRIPTION,STATION_NAME from REPLICATORS";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_replicators_view);
    item->setText(0,q->value(0).toString());
    item->setText(1,
	   RDReplicator::typeString((RDReplicator::Type)q->value(1).toUInt()));
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
  }
  delete q;
}


void ListReplicators::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select TYPE_ID,DESCRIPTION,STATION_NAME \
                         from REPLICATORS where NAME=\"%s\"",
			(const char *)RDEscapeString(item->text(0)));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    item->setText(1,
	   RDReplicator::typeString((RDReplicator::Type)q->value(0).toUInt()));
    item->setText(2,q->value(1).toString());
    item->setText(3,q->value(2).toString());
  }
  delete q;
}
