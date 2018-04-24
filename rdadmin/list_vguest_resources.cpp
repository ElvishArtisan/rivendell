// list_vguest_resources.cpp
//
// List vGuest Resources.
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_user.h"
#include "edit_vguest_resource.h"
#include "list_vguest_resources.h"

ListVguestResources::ListVguestResources(RDMatrix *matrix,
					 RDMatrix::VguestType type,int size,
					 QWidget *parent)
  : QDialog(parent)
{
  QString str;

  list_matrix=matrix;
  list_type=type;
  list_size=size;

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
  list_label=new QLabel(list_table,this);
  list_label->setFont(bold_font);
  list_model=new RDSqlTableModel(this);
  list_model->setQuery(ModelSql(list_type));
  SetHeaders(list_model,list_type);
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


QSize ListVguestResources::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListVguestResources::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListVguestResources::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditVguestResource *edit=
      new EditVguestResource(list_matrix,list_type,
			     s->selectedRows()[0].data().toInt(),this);
    if(edit->exec()==0) {
      list_model->update();
    }
    delete edit;
  }
}


void ListVguestResources::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListVguestResources::okData()
{
  done(0);
}


void ListVguestResources::cancelData()
{
  done(-1);
}


void ListVguestResources::resizeEvent(QResizeEvent *e)
{
  list_label->setGeometry(14,5,85,19);
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


QString ListVguestResources::ModelSql(RDMatrix::VguestType type) const
{
  QString sql;

  switch(type) {
  case RDMatrix::VguestTypeRelay:
  sql=QString("select ")+
    "NUMBER,"+
    "ENGINE_NUM,"+
    "DEVICE_NUM,"+
    "SURFACE_NUM,"+
    "RELAY_NUM "+
    "from VGUEST_RESOURCES where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX_NUM=%d)&&",list_matrix->matrix())+
    QString().sprintf("(VGUEST_TYPE=%d) ",list_type)+
    "order by NUMBER";
    break;

  case RDMatrix::VguestTypeDisplay:
  sql=QString("select ")+
    "NUMBER,"+
    "ENGINE_NUM,"+
    "DEVICE_NUM,"+
    "SURFACE_NUM "+
    "from VGUEST_RESOURCES where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX_NUM=%d)&&",list_matrix->matrix())+
    QString().sprintf("(VGUEST_TYPE=%d) ",list_type)+
    "order by NUMBER";
    break;
  }

  return sql;
}


void ListVguestResources::SetHeaders(RDSqlTableModel *model,
				     RDMatrix::VguestType type)
{
  switch(type) {
  case RDMatrix::VguestTypeRelay:
    setWindowTitle("RDAdmin - "+tr("vGuest Switches"));
    model->setHeaderData(0,Qt::Horizontal,tr("GPIO Line"));
    break;

  case RDMatrix::VguestTypeDisplay:
    setWindowTitle("RDAdmin - "+tr("vGuest Displays"));
    model->setHeaderData(0,Qt::Horizontal,tr("Display"));
    break;
  }
  model->setHeaderData(1,Qt::Horizontal,tr("Engine"));
  model->setFieldType(1,RDSqlTableModel::EngineNumberType);
  model->setHeaderData(2,Qt::Horizontal,tr("Device (hex)"));
  model->setFieldType(2,RDSqlTableModel::DeviceNumberType);
  model->setHeaderData(3,Qt::Horizontal,tr("Surface"));
  model->setFieldType(3,RDSqlTableModel::EngineNumberType);
  switch(list_type) {
  case RDMatrix::VguestTypeRelay:
    model->setHeaderData(4,Qt::Horizontal,tr("Buss / Relay"));
    model->setFieldType(4,RDSqlTableModel::EngineNumberType);
    break;

  case RDMatrix::VguestTypeDisplay:
    break;
  }
}
