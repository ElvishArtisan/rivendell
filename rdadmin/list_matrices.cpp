// list_matrices.cpp
//
// List Rivendell Matrices
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

#include <QMessageBox>

#include <rdapplication.h>
#include <rdescape_string.h>

#include "add_matrix.h"
#include "edit_matrix.h"
#include "globals.h"
#include "list_matrices.h"

ListMatrices::ListMatrices(QString station,QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_station=station;
  setWindowTitle("RDAdmin - "+tr("Rivendell Switcher List"));

  //
  // Initialize Data Structures
  //
  for(int i=0;i<MAX_MATRICES;i++) {
    list_matrix_modified[i]=false;
  }

  //
  // Matrix List Box
  //
  list_view=new RDTableView(this);
  list_model=new RDMatrixListModel(station,false,this);
  list_model->setFont(defaultFont());
  list_model->setPalette(palette());
  list_view->setModel(list_model);
  list_title_label=
    new QLabel(list_view,tr("&Replicators:"),this);
  list_title_label->setFont(labelFont());
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_model,SIGNAL(modelReset()),
	  list_view,SLOT(resizeColumnsToContents()));
  list_view->resizeColumnsToContents();

  /*
  list_view=new Q3ListView(this,"list_box");
  list_title_label=new QLabel(list_view,tr("Switchers:"),this);
  list_title_label->setFont(labelFont());
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(tr("MATRIX"));
  list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_view->addColumn(tr("DESCRIPTION"));
  list_view->setColumnAlignment(1,Qt::AlignLeft);
  list_view->addColumn(tr("TYPE"));
  list_view->setColumnAlignment(2,Qt::AlignLeft);
  connect(list_view,SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();
  */
  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListMatrices::~ListMatrices()
{
  delete list_view;
}


QSize ListMatrices::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListMatrices::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListMatrices::addData()
{
  int matrix_num;

  AddMatrix *add=new AddMatrix(list_station,this);
  if((matrix_num=add->exec())<0) {
    delete add;
    return;
  }
  delete add;
  RDMatrix *mtx=new RDMatrix(list_station,matrix_num);
  EditMatrix *edit=new EditMatrix(mtx,this);
  if(edit->exec()) {
    list_matrix_modified[matrix_num]=true;
    QModelIndex row=list_model->addMatrix(mtx);
    if(row.isValid()) {
      list_view->selectRow(row.row());
    }
  }
  else {
    DeleteMatrix(mtx);
  }
  delete edit;
  delete mtx;
}


void ListMatrices::editData()
{
  QModelIndexList rows=list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int matrix_id=list_model->matrixId(rows.first());
  RDMatrix *mtx=new RDMatrix(matrix_id);
  EditMatrix *d=new EditMatrix(mtx,this);
  if(d->exec()) {
    list_matrix_modified[mtx->matrix()]=true;
    list_model->refresh(rows.first());
  }
  delete d;
  delete mtx;
}


void ListMatrices::deleteData()
{
  QModelIndexList rows=list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  RDMatrix *mtx=new RDMatrix(list_model->matrixId(rows.first()));
  QString msg=tr("Are you sure you want to delete switcher")+
    " \""+QString().sprintf("%d",mtx->matrix())+":"+
    mtx->name()+"\" "+
    tr("on")+" \""+list_station+"\"?"+"\n"+
    tr("ALL references to this switcher will be deleted!");
  if(QMessageBox::warning(this,tr("Deleting Switcher"),msg,
			  QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  DeleteMatrix(mtx);
  list_matrix_modified[mtx->matrix()]=true;
}


void ListMatrices::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListMatrices::closeData()
{
  RDStation *rmt_station=new RDStation(list_station);
  QHostAddress addr=rmt_station->address();
  RDMacro macro;
  
  macro.setCommand(RDMacro::SZ);
  macro.setRole(RDMacro::Cmd);
  macro.setEchoRequested(false);
  for(int i=0;i<MAX_MATRICES;i++) {
    if(list_matrix_modified[i]) {
      macro.setAddress(rmt_station->address());
      macro.addArg(i);
      rda->ripc()->sendRml(&macro);
    }
  }
  delete rmt_station;
  done(0);
}


void ListMatrices::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_title_label->setGeometry(14,5,85,19);
  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListMatrices::DeleteMatrix(RDMatrix *mtx)
{
  QString sql=QString("delete from MATRICES where ")+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
    QString().sprintf("MATRIX=%d",mtx->matrix());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from INPUTS where ")+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
    QString().sprintf("MATRIX=%d",mtx->matrix());
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from OUTPUTS where ")+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
    QString().sprintf("MATRIX=%d",mtx->matrix());
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from SWITCHER_NODES where ")+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
    QString().sprintf("MATRIX=%d",mtx->matrix());
  q=new RDSqlQuery(sql);
  delete q;
sql=QString("delete from GPIS where ")+
  "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
  QString().sprintf("MATRIX=%d",mtx->matrix());
  q=new RDSqlQuery(sql);
  delete q;
sql=QString("delete from GPOS where ")+
  "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
  QString().sprintf("MATRIX=%d",mtx->matrix());
  q=new RDSqlQuery(sql);
  delete q;
sql=QString("delete from VGUEST_RESOURCES where ")+
  "STATION_NAME=\""+RDEscapeString(list_station)+"\" && "+
  QString().sprintf("MATRIX_NUM=%d",mtx->matrix());
  q=new RDSqlQuery(sql);
  delete q;

  list_model->removeMatrix(mtx->id());
}

/*
void ListMatrices::RefreshList()
{
  Q3ListViewItem *l;

  list_view->clear();
  QString sql=QString("select ")+
    "MATRIX,"+  // 00
    "NAME,"+    // 01
    "TYPE "+    // 02
    "from MATRICES where "+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" "+
    "order by MATRIX";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new Q3ListViewItem(list_view);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,RDMatrix::typeString((RDMatrix::Type)q->value(2).toInt()));
  }
  delete q;
}
*/

void ListMatrices::AddList(int matrix_num)
{
  /*
  RDMatrix *matrix=new RDMatrix(list_station,matrix_num);
  Q3ListViewItem *item=new Q3ListViewItem(list_view);
  item->setText(0,QString().sprintf("%d",matrix_num));
  item->setText(1,matrix->name());
  item->setText(2,RDMatrix::typeString(matrix->type()));
  delete matrix;
  list_view->setCurrentItem(item);
  list_view->setSelected(item,true);
  */
}

/*
void ListMatrices::RefreshRecord(Q3ListViewItem *item)
{
  RDMatrix *matrix=new RDMatrix(list_station,item->text(0).toInt());
  item->setText(1,matrix->name());
  delete matrix;
}
*/
