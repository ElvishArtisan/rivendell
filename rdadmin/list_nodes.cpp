// list_nodes.cpp
//
// List Rivendell LiveWire Nodes
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_endpoint.h"
#include "edit_node.h"
#include "list_nodes.h"

ListNodes::ListNodes(RDMatrix *matrix,QWidget *parent)
  : QDialog(parent)
{
  list_matrix=matrix;
  setWindowTitle("RDAdmin - "+tr("LiveWire Node List"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

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
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "HOSTNAME,"+
    "DESCRIPTION,"+
    "BASE_OUTPUT,"+
    "TCP_PORT "+
    "from SWITCHER_NODES where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)",list_matrix->matrix());
  list_model->setQuery(sql);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Hostname"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Description"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("First Output"));
  list_model->setHeaderData(4,Qt::Horizontal,tr("TCP Port"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(bold_font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(bold_font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(bold_font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(bold_font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
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
  int id=-1;
  EditNode *edit=new EditNode(&id,list_matrix,this);
  if(edit->exec()==0) {
    list_model->update();
  }
}


void ListNodes::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int id=s->selectedRows()[0].data().toInt();
    EditNode *edit=new EditNode(&id,list_matrix,this);
    if(edit->exec()==0) {
      list_model->update();
    }
  }
}


void ListNodes::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    int id=s->selectedRows()[0].data().toInt();
    QString sql=QString().sprintf("delete from SWITCHER_NODES where ID=%d",id);
    RDSqlQuery::run(sql);
    list_model->update();
  }
}


void ListNodes::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListNodes::closeData()
{
  PurgeEndpoints("INPUTS");
  PurgeEndpoints("OUTPUTS");
  done(0);
}


void ListNodes::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-80);
  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListNodes::PurgeEndpoints(const QString &tablename)
{
  /*
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from ")+tablename+" where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",list_matrix->matrix());
  RDListViewItem *item=(RDListViewItem *)list_list_view->firstChild();
  while(item!=NULL) {
    sql+=QString("((NODE_HOSTNAME!=")+"\""+
      RDEscapeString(item->text(0))+"\")&&"+
      QString().sprintf("(NODE_TCP_PORT!=%d))&&",item->text(3).toInt());
    item=(RDListViewItem *)item->nextSibling();
  }
  sql=sql.left(sql.length()-2);
  q=new RDSqlQuery(sql);
  delete q;
  */
}
