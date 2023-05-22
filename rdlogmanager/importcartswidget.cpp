// importcartswidget.cpp
//
// Widget for listing import events in rdlogmanager(1)
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdconf.h>

#include "importcartswidget.h"

ImportCartsWidget::ImportCartsWidget(ImportCartsModel::ImportType type,
				     QWidget *parent)
  : RDWidget(parent)
{
  d_import_type=type;
  d_model=NULL;

  if(d_import_type==ImportCartsModel::PreImport) {
    d_title_label=new QLabel(tr("PRE-IMPORT EVENTS"),this);
  }
  if(d_import_type==ImportCartsModel::PostImport) {
    d_title_label=new QLabel(tr("POST-IMPORT EVENTS"),this);
  }
  d_title_label->setFont(labelFont());

  d_length_edit=new QLineEdit(this);
  d_length_edit->setReadOnly(true);
  d_length_label=new QLabel(tr("Len")+":",this);
  d_length_label->setFont(labelFont());
  d_length_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  d_view=new ImportCartsView(this);
  d_view->setDragEnabled(true);

  d_up_button=new RDTransportButton(RDTransportButton::Up,this);
  connect(d_up_button,SIGNAL(clicked()),this,SLOT(upData()));
  d_down_button=
    new RDTransportButton(RDTransportButton::Down,this);
  connect(d_down_button,SIGNAL(clicked()),this,SLOT(downData()));
}


ImportCartsWidget::~ImportCartsWidget()
{
  if(d_model!=NULL) {
    delete d_model;
  }
}


QSize ImportCartsWidget::sizeHint() const
{
  return QSize(624,160);
}


QSizePolicy ImportCartsWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding);
}


ImportCartsModel::ImportType ImportCartsWidget::importType() const
{
  return d_import_type;
}


void ImportCartsWidget::setEventName(const QString &str)
{
  d_model->setEventName(str);
}


void ImportCartsWidget::load(RDEvent *evt)
{
  if(d_model!=NULL) {
    delete d_model;
  }
  d_model=new ImportCartsModel(evt->name(),d_import_type,true,this);
  d_model->setFont(font());
  d_model->setPalette(palette());
  d_view->setModel(d_model);
  connect(d_model,SIGNAL(modelReset()),d_view,SLOT(resizeColumnsToContents()));
  connect(d_view,SIGNAL(cartDropped(int,RDLogLine *)),
	  d_model,SLOT(processCartDrop(int,RDLogLine *)));
  connect(d_model,SIGNAL(totalLengthChanged(int)),
	  this,SLOT(lengthChangedData(int)));
  d_view->resizeColumnsToContents();
  lengthChangedData(d_model->totalLength());
}


void ImportCartsWidget::save(RDEvent *evt,
			     RDLogLine::TransType first_trans) const
{
  d_model->save(first_trans);
}


void ImportCartsWidget::lengthChangedData(int msecs)
{
  d_length_edit->setText(RDGetTimeLength(msecs,true,false));
}


void ImportCartsWidget::upData()
{
  QModelIndexList rows=d_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(d_model->moveUp(rows.first())) {
    d_view->selectRow(rows.first().row()-1);
  }
}


void ImportCartsWidget::downData()
{
  QModelIndexList rows=d_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(d_model->moveDown(rows.first())) {
    d_view->selectRow(rows.first().row()+1);
  }
}


void ImportCartsWidget::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  d_title_label->setGeometry(5,2,200,16);
  d_length_edit->setGeometry(w-150,0,80,20);
  d_length_label->setGeometry(w-340,2,185,16);
  //  d_view->setGeometry(5,20,w-75,115);
  d_view->setGeometry(5,20,w-75,h-45);
  d_up_button->setGeometry(w-60,24,40,40);
  d_down_button->setGeometry(w-60,85,40,40);
}
