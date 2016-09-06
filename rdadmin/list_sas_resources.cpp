// list_sas_resources.cpp
//
// List SAS Resources.
//
//   (C) Copyright 2002-2005,2011,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_sas_resource.h"
#include "list_sas_resources.h"

ListSasResources::ListSasResources(RDMatrix *matrix,int size,QWidget *parent)
  : QDialog(parent)
{
  QString str;

  list_matrix=matrix;
  list_size=size;
  setWindowTitle("RDAdmin - "+tr("SAS Switches"));

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
  // Resources List Box
  //
  list_label=new QLabel(tr("SAS Switches"),this);
  list_label->setFont(bold_font);
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "NUMBER,"+
    "ENGINE_NUM,"+
    "DEVICE_NUM,"+
    "RELAY_NUM "+
    "from VGUEST_RESOURCES where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX_NUM=%d) ",list_matrix->matrix())+
    "order by NUMBER";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("GPIO Line"));
  list_model->setHeaderData(1,Qt::Horizontal,tr("Console"));
  list_model->setFieldType(1,RDSqlTableModel::EngineNumberType);
  list_model->setHeaderData(2,Qt::Horizontal,tr("Source"));
  list_model->setFieldType(2,RDSqlTableModel::EngineNumberType);
  list_model->setHeaderData(3,Qt::Horizontal,tr("Opto / Relay"));
  list_model->setFieldType(3,RDSqlTableModel::EngineNumberType);
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
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
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize ListSasResources::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListSasResources::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSasResources::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditSasResource *edit=
      new EditSasResource(list_matrix,s->selectedRows()[0].data().toInt(),this);
    if(edit->exec()==0) {
      list_model->update();
    }
    delete edit;
  }
}


void ListSasResources::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListSasResources::okData()
{
  done(0);
}


void ListSasResources::cancelData()
{
  done(-1);
}


void ListSasResources::resizeEvent(QResizeEvent *e)
{
  list_label->setGeometry(14,5,85,19);
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
