// list_nodes.cpp
//
// List Rivendell LiveWire Nodes
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_nodes.cpp,v 1.3 2010/07/29 19:32:35 cvs Exp $
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
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdpasswd.h>
#include <rddb.h>

#include <edit_node.h>
#include <list_nodes.h>
#include <edit_endpoint.h>


ListNodes::ListNodes(RDMatrix *matrix,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_matrix=matrix;
  setCaption(tr("LiveWire Node List"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Nodes List Box
  //
  list_list_view=new RDListView(this,"list_box");
  list_list_view->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-80);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  list_list_view->addColumn(tr("HOSTNAME"));
  list_list_view->setColumnAlignment(0,Qt::AlignLeft);
  list_list_view->addColumn(tr("DESCRIPTION"));
  list_list_view->setColumnAlignment(1,Qt::AlignLeft);
  list_list_view->addColumn(tr("FIRST OUTPUT"));
  list_list_view->setColumnAlignment(2,Qt::AlignLeft);
  list_list_view->addColumn(tr("TCP PORT"));
  list_list_view->setColumnAlignment(3,Qt::AlignCenter);
  connect(list_list_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Add Button
  //
  QPushButton *button=new QPushButton(this,"add_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Add"));
  connect(button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  button=new QPushButton(this,"edit_button");
  button->setGeometry(100,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Edit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  button=new QPushButton(this,"delete_button");
  button->setGeometry(190,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Delete"));
  connect(button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Load Values
  //
  RefreshList();
}


QSize ListNodes::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListNodes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListNodes::addData()
{
  RDListViewItem *item;
  int id=-1;
  EditNode *node=new EditNode(&id,list_matrix,this);
  if(node->exec()==0) {
    item=new RDListViewItem(list_list_view);
    item->setId(id);
    RefreshItem(item);
    list_list_view->setSelected(item,true);
    list_list_view->ensureItemVisible(item);
  }
  delete node;
}


void ListNodes::editData()
{
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  int id=item->id();
  EditNode *node=new EditNode(&id,list_matrix,this);
  if(node->exec()==0) {
    RefreshItem(item);
  }
  delete node;
}


void ListNodes::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  if(QMessageBox::question(this,tr("Delete Node"),
			   tr("Are your sure you want to delete this node?"),
			   QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::No) {
    return;
  }
  sql=QString().sprintf("delete from SWITCHER_NODES where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  delete q;
  delete item;
}


void ListNodes::doubleClickedData(QListViewItem *item,const QPoint &pt,
				      int col)
{
  editData();
}


void ListNodes::closeData()
{
  PurgeEndpoints("INPUTS");
  PurgeEndpoints("OUTPUTS");
  done(0);
}


void ListNodes::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_list_view->clear();
  sql=QString().sprintf("select ID,HOSTNAME,DESCRIPTION,BASE_OUTPUT,TCP_PORT \
                         from SWITCHER_NODES \
                         where (STATION_NAME=\"%s\")&&(MATRIX=%d)",
			(const char *)list_matrix->station(),
			list_matrix->matrix());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(list_list_view);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
    item->setText(1,q->value(2).toString());
    if(q->value(3).toInt()==0) {
      item->setText(2,tr("[none]"));
    }
    else {
      item->setText(2,QString().sprintf("%d",q->value(3).toInt()));
    }
    item->setText(3,q->value(4).toString());
  }
  delete q;
}


void ListNodes::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select HOSTNAME,DESCRIPTION,BASE_OUTPUT,TCP_PORT \
                         from SWITCHER_NODES where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    item->setText(0,q->value(0).toString());
    item->setText(1,q->value(1).toString());
    if(q->value(2).toInt()==0) {
      item->setText(2,tr("[none]"));
    }
    else {
      item->setText(2,QString().sprintf("%d",q->value(2).toInt()));
    }
    item->setText(3,q->value(3).toString());
  }
  delete q;
}


void ListNodes::PurgeEndpoints(const QString &tablename)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("delete from %s where \
                         (STATION_NAME=\"%s\")&&\
                         (MATRIX=%d)&&",
			(const char *)tablename,
			(const char *)list_matrix->station(),
			list_matrix->matrix());
  RDListViewItem *item=(RDListViewItem *)list_list_view->firstChild();
  while(item!=NULL) {
    sql+=QString().sprintf("((NODE_HOSTNAME!=\"%s\")&&(NODE_TCP_PORT!=%d))&&",
			   (const char *)item->text(0),item->text(3).toInt());
    item=(RDListViewItem *)item->nextSibling();
  }
  sql=sql.left(sql.length()-2);
  q=new RDSqlQuery(sql);
  delete q;
}
