// list_gpis.cpp
//
// List Rivendell GPIOs
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

#include <rd.h>
#include <rdapplication.h>
#include <rdescape_string.h>

#include "edit_gpi.h"
#include "globals.h"
#include "list_gpis.h"

ListGpis::ListGpis(RDMatrix *matrix,RDMatrix::GpioType type,QWidget *parent)
  : QDialog(parent)
{
  QString label_text;
  QString column_text;
  int rows=0;

  list_matrix=matrix;
  list_type=type;
  switch(type) {
    case RDMatrix::GpioInput:
      list_tablename="GPIS";
      list_size=list_matrix->gpis();
      label_text=tr("GPI Lines");
      column_text=tr("GPI");
      rows=matrix->gpis();
      setWindowTitle("RDAdmin - "+tr("List GPIs"));
      break;

    case RDMatrix::GpioOutput:
      list_tablename="GPOS";
      list_size=list_matrix->gpos();
      label_text=tr("GPO Lines");
      column_text=tr("GPO");
      rows=matrix->gpos();
      setWindowTitle("RDAdmin - "+tr("List GPOs"));
      break;
  }

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
  // Gpis List Box
  //
  list_label=new QLabel(label_text,this);
  list_label->setFont(bold_font);
  list_widget=new RDTableWidget(rows,5,this);
  list_widget->setHorizontalHeaderItem(0,new QTableWidgetItem(column_text));
  list_widget->
    setHorizontalHeaderItem(1,new QTableWidgetItem(tr("ON Macro Cart")));
  list_widget->
    setHorizontalHeaderItem(2,new QTableWidgetItem(tr("ON Description")));
  list_widget->
    setHorizontalHeaderItem(3,new QTableWidgetItem(tr("OFF Macro Cart")));
  list_widget->
    setHorizontalHeaderItem(4,new QTableWidgetItem(tr("OFF Description")));
  connect(list_widget,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(bold_font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setFont(bold_font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
}


QSize ListGpis::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy ListGpis::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListGpis::editData()
{
  QItemSelectionModel *s=list_widget->selectionModel();
  if(s->hasSelection()) {
    EditGpi *gpi_box=new EditGpi(list_matrix,list_type,
				 s->selectedRows()[0].data().toInt(),this);
    if(gpi_box->exec()==0) {
      UpdateRow(s->selectedRows()[0].row());
      //      list_model->update();
    }
    delete gpi_box;
  }
}


void ListGpis::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListGpis::closeData()
{
  done(0);
}


void ListGpis::resizeEvent(QResizeEvent *e)
{
  list_widget->setGeometry(10,24,size().width()-20,size().height()-94);
  list_label->setGeometry(14,5,85,19);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListGpis::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *item=NULL;

  //
  // ON Carts
  //
  sql=QString("select ")+
    list_tablename+".NUMBER,"+
    list_tablename+".MACRO_CART,"+
    "CART.TITLE "+
    "from "+list_tablename+" left join CART "+
    "on "+list_tablename+".MACRO_CART=CART.NUMBER where "+
    "("+list_tablename+".STATION_NAME=\""+
    RDEscapeString(list_matrix->station())+"\")&&"+
    "("+list_tablename+QString().sprintf(".MATRIX=%d) ",list_matrix->matrix())+
    "order by "+list_tablename+".NUMBER";
  q=new RDSqlQuery(sql);
  void RefreshRow(int row);
  while(q->next()) {
    item=new QTableWidgetItem(QString().sprintf("%d",q->value(0).toInt()));
    list_widget->setItem(q->value(0).toInt()-1,0,item);

    if(q->value(1).toUInt()==0) {
      item=new QTableWidgetItem(tr("[none]"));
    }
    else {
      item=new QTableWidgetItem(QString().sprintf("%06u",q->value(1).toUInt()));
    }
    list_widget->setItem(q->value(0).toInt()-1,1,item);

    item=new QTableWidgetItem(q->value(2).toString());
    list_widget->setItem(q->value(0).toInt()-1,2,item);
  }
  delete q;

  //
  // OFF Carts
  //
  sql=QString("select ")+
    list_tablename+".NUMBER,"+
    list_tablename+".OFF_MACRO_CART,"+
    "CART.TITLE "+
    "from "+list_tablename+" left join CART "+
    "on "+list_tablename+".OFF_MACRO_CART=CART.NUMBER where "+
    "("+list_tablename+".STATION_NAME=\""+
    RDEscapeString(list_matrix->station())+"\")&&"+
    "("+list_tablename+QString().sprintf(".MATRIX=%d) ",list_matrix->matrix())+
    "order by "+list_tablename+".NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(1).toUInt()==0) {
      item=new QTableWidgetItem(tr("[none]"));
    }
    else {
      item=new QTableWidgetItem(QString().sprintf("%06u",q->value(1).toUInt()));
    }
    list_widget->setItem(q->value(0).toInt()-1,3,item);

    item=new QTableWidgetItem(q->value(2).toString());
    list_widget->setItem(q->value(0).toInt()-1,4,item);
  }
  delete q;
  list_widget->resizeColumnsToContents();
}


void ListGpis::UpdateRow(int row)
{
  QString sql;
  RDSqlQuery *q;

  //
  // ON Cart
  //
  sql=QString("select ")+
    list_tablename+".NUMBER,"+
    list_tablename+".MACRO_CART,"+
    "CART.TITLE "+
    "from "+list_tablename+" left join CART "+
    "on "+list_tablename+".MACRO_CART=CART.NUMBER where "+
    "("+list_tablename+".STATION_NAME=\""+
    RDEscapeString(list_matrix->station())+"\")&&"+
    "("+list_tablename+QString().sprintf(".MATRIX=%d)&&",list_matrix->matrix())+
    "("+list_tablename+QString().sprintf(".NUMBER=%d) ",row+1);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(1).toUInt()==0) {
      list_widget->item(row,1)->setData(Qt::DisplayRole,tr("[none]"));
    }
    else {
      list_widget->item(row,1)->
	setData(Qt::DisplayRole,QString().sprintf("%06u",q->value(1).toUInt()));
    }
    list_widget->item(row,2)->setData(Qt::DisplayRole,q->value(2).toString());
  }
  delete q;

  //
  // OFF Cart
  //
  sql=QString("select ")+
    list_tablename+".NUMBER,"+
    list_tablename+".OFF_MACRO_CART,"+
    "CART.TITLE "+
    "from "+list_tablename+" left join CART "+
    "on "+list_tablename+".OFF_MACRO_CART=CART.NUMBER where "+
    "("+list_tablename+".STATION_NAME=\""+
    RDEscapeString(list_matrix->station())+"\")&&"+
    "("+list_tablename+QString().sprintf(".MATRIX=%d)&&",list_matrix->matrix())+
    "("+list_tablename+QString().sprintf(".NUMBER=%d) ",row+1);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(1).toUInt()==0) {
      list_widget->item(row,3)->setData(Qt::DisplayRole,tr("[none]"));
    }
    else {
      list_widget->item(row,3)->
	setData(Qt::DisplayRole,QString().sprintf("%06u",q->value(1).toUInt()));
    }
    list_widget->item(row,4)->setData(Qt::DisplayRole,q->value(2).toString());
  }
  delete q;
}
