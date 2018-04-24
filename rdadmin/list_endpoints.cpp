// list_endpoints.cpp
//
// List Rivendell Endpoints
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
#include "edit_user.h"
#include "list_endpoints.h"

ListEndpoints::ListEndpoints(RDMatrix *matrix,RDMatrix::Endpoint endpoint,
			     QWidget *parent)
  : QDialog(parent)
{
  QString str;

  list_matrix=matrix;
  list_endpoint=endpoint;
  switch(list_endpoint) {
      case RDMatrix::Input:
	list_size=list_matrix->inputs();
	list_table="INPUTS";
	setWindowTitle("RDAdmin - "+tr("List Inputs"));
	break;

      case RDMatrix::Output:
	list_size=list_matrix->outputs();
	list_table="OUTPUTS";
	setWindowTitle("RDAdmin - "+tr("List Outputs"));
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
  // Endpoints List Box
  //
  QString sql;
  list_label=new QLabel(list_table,this);
  list_label->setFont(bold_font);
  list_model=new RDSqlTableModel(this);
  list_model->setQuery(ModelSql(list_matrix->type()));
  list_model->
    setHeaderData(0,Qt::Horizontal,RDMatrix::endpointString(list_endpoint));
  list_model->setHeaderData(1,Qt::Horizontal,tr("Label"));
  list_readonly=SetHeaders(list_model,matrix->type(),list_endpoint);

  list_view=new RDTableView(this);
  list_view->setModel(list_model); 
  list_view->resizeColumnsToContents();
  if(!list_readonly) {
    connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	    this,SLOT(doubleClickedData(const QModelIndex &)));
  }

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(bold_font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));
  list_edit_button->setDisabled(list_readonly);

  //
  //  Ok Button
  //
  list_ok_button=new QPushButton(this);
  list_ok_button->setDefault(true);
  list_ok_button->setFont(bold_font);
  list_ok_button->setText(tr("&OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  list_cancel_button=new QPushButton(this);
  list_cancel_button->setFont(bold_font);
  list_cancel_button->setText(tr("&Cancel"));
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize ListEndpoints::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListEndpoints::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListEndpoints::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditEndpoint *edit=new EditEndpoint(list_matrix,s->selectedRows()[0].data().toInt(),list_endpoint,this);
    if(edit->exec()==0) {
      list_model->update();
    }
    delete edit;
  }
}


void ListEndpoints::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListEndpoints::okData()
{
  done(0);
}


void ListEndpoints::cancelData()
{
  done(1);
}


void ListEndpoints::resizeEvent(QResizeEvent *e)
{
  list_label->setGeometry(14,5,85,19);
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  list_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


QString ListEndpoints::ModelSql(RDMatrix::Type type) const
{
  QString sql;

  switch(list_matrix->type()) {
  case RDMatrix::Unity4000:
    if(list_endpoint==RDMatrix::Input) {
      sql=QString("select ")+
	"NUMBER,"+
	"NAME,"+
	"FEED_NAME,"+
	"CHANNEL_MODE "+
	"from "+list_table+" where "+
	"(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
	QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
	"order by NUMBER";
    }
    else {
      sql=QString("select NUMBER,NAME from ")+list_table+" where "+
	"(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
	QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
	"order by NUMBER";
    }
    break;

  case RDMatrix::LogitekVguest:
    sql=QString("select ")+
      "NUMBER,"+
      "NAME,"+
      "ENGINE_NUM,"+
      "DEVICE_NUM "+
      "from "+list_table+" where "+
      "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
      "order by NUMBER";
    break;

  case RDMatrix::StarGuideIII:
    if(list_endpoint==RDMatrix::Input) {
      sql=QString("select ")+
	"NUMBER,"+
	"NAME,"+
	"ENGINE_NUM,"+
	"DEVICE_NUM,"+
	"CHANNEL_MODE "+
	"from "+list_table+" where "+
	"(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
	QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
	"order by NUMBER";
    }
    else {
      sql=QString("select ")+
	"NUMBER,"+
	"NAME "+
	"from "+list_table+" where "+
	"(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
	QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
	"order by NUMBER";
    }
    break;

  case RDMatrix::LiveWireLwrpAudio:
    sql=QString("select ")+
      "NUMBER,"+
      "NAME,"+
      "NODE_HOSTNAME,"+
      "NODE_SLOT "+
      "from "+list_table+" where "+
      "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
      "order by NUMBER";
    break;

  default:
    sql=QString("select ")+
      "NUMBER,"+
      "NAME "+
      "from "+list_table+" where "+
      "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
      "order by NUMBER";
    break;
  }

  return sql;
}


bool ListEndpoints::SetHeaders(RDSqlTableModel *model,RDMatrix::Type type,
			       RDMatrix::Endpoint end)
{
  bool ret=false;

  switch(type) {
  case RDMatrix::Unity4000:
    ret=false;
      if(end==RDMatrix::Input) {
	model->setHeaderData(2,Qt::Horizontal,tr("Source"));
	model->setHeaderData(3,Qt::Horizontal,tr("Mode"));
      }
      break;
      
    case RDMatrix::LogitekVguest:
      ret=false;
      model->setHeaderData(2,Qt::Horizontal,tr("Engine"));
      model->setFieldType(2,RDSqlTableModel::EngineNumberType);
      model->setHeaderData(3,Qt::Horizontal,tr("Device (hex)"));
      model->setFieldType(3,RDSqlTableModel::DeviceNumberType);
      break;
      
    case RDMatrix::StarGuideIII:
      ret=false;
      if(end==RDMatrix::Input) {
	model->setHeaderData(2,Qt::Horizontal,tr("Provider ID"));
	model->setHeaderData(3,Qt::Horizontal,tr("Service ID"));
	model->setHeaderData(4,Qt::Horizontal,tr("Mode"));
      }
      break;
      
    case RDMatrix::LiveWireLwrpAudio:
      ret=true;
      model->setHeaderData(2,Qt::Horizontal,tr("Node"));
      model->setHeaderData(3,Qt::Horizontal,tr("#"));
      break;

    case RDMatrix::SasUsi:
      ret=true;
      break;

    default:
      ret=false;
      break;
  }
  return ret;
}
