// rdlistsvcs.cpp
//
// Service Picker dialog
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdlistsvcs.h"

RDListSvcs::RDListSvcs(const QString &caption,QWidget *parent)
  : RDDialog(parent)
{
  edit_caption=caption;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle(caption+" - "+tr("Rivendell Services"));

  //
  // Services
  //
  edit_svc_view=new RDTableView(this);
  edit_svc_model=new RDServiceListModel(false,false,this);
  edit_svc_model->setFont(font());
  edit_svc_model->setPalette(palette());
  edit_svc_view->setModel(edit_svc_model);
  for(int i=2;i<edit_svc_model->columnCount();i++) {
    edit_svc_view->hideColumn(i);
  }
  connect(edit_svc_model,SIGNAL(modelReset()),
	  edit_svc_view,SLOT(resizeColumnsToContents()));
  connect(edit_svc_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDListSvcs::~RDListSvcs()
{
}


QSize RDListSvcs::sizeHint() const
{
  return QSize(300,240);
} 


QSizePolicy RDListSvcs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDListSvcs::exec(QString *svcname)
{
  edit_svcname=svcname;

  return QDialog::exec();
}


void RDListSvcs::doubleClickedData(const QModelIndex &index)
{
  okData();
}


void RDListSvcs::okData()
{
  QModelIndexList rows=edit_svc_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  *edit_svcname=edit_svc_model->serviceName(rows.first());

  done(true);
}


void RDListSvcs::cancelData()
{
  done(false);
}


void RDListSvcs::resizeEvent(QResizeEvent *e)
{
  edit_svc_view->setGeometry(10,10,size().width()-20,size().height()-80);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
