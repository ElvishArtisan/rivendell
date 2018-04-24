// list_livewiregpios.cpp
//
// List Rivendell Livewire GPIO Slot Associations
//
//   (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdescape_string.h>
#include <rddb.h>

#include "edit_livewiregpio.h"
#include "list_livewiregpios.h"

ListLiveWireGpios::ListLiveWireGpios(RDMatrix *matrix,int slot_quan,
				     QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_matrix=matrix;
  list_slot_quan=slot_quan;
  setWindowTitle("RDAdmin - "+tr("LiveWire GPIO Source Assignments"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Matrix List Box
  //
  list_label=new QLabel(tr("GPIO Slots:"),this);
  list_label->setFont(font);
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "SLOT,"+
    "SOURCE_NUMBER,"+
    "IP_ADDRESS "+
    "from LIVEWIRE_GPIO_SLOTS where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
    "order by SLOT";
  list_model->setQuery(sql);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Lines"));
  list_model->setFieldType(1,RDSqlTableModel::LiveWireGpioLinesType);
  list_model->setHeaderData(2,Qt::Horizontal,tr("Source #"));
  list_model->setFieldType(2,RDSqlTableModel::LiveWireSourceType);
  list_model->setHeaderData(3,Qt::Horizontal,tr("Surface Address"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


ListLiveWireGpios::~ListLiveWireGpios()
{
  delete list_view;
}


QSize ListLiveWireGpios::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListLiveWireGpios::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListLiveWireGpios::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditLiveWireGpio *edit=
      new EditLiveWireGpio(s->selectedRows()[0].data().toInt(),this);
    if(edit->exec()==0) {
      list_model->update();
    }
  }
}


void ListLiveWireGpios::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListLiveWireGpios::okData()
{
  done(0);
}


void ListLiveWireGpios::cancelData()
{
  done(-1);
}


void ListLiveWireGpios::resizeEvent(QResizeEvent *e)
{
  list_label->setGeometry(14,5,85,19);
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
