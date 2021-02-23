// rdlist_groups.cpp
//
// A widget to select a Rivendell Group.
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

#include "rddb.h"
#include "rdescape_string.h"
#include "rdlist_groups.h"

RDListGroups::RDListGroups(QString *groupname,const QString &caption,
			   QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  group_name=groupname;

  setWindowTitle(caption+" - "+tr("Select Group"));

  //
  // Cart List
  //
  group_group_view=new RDTableView(this);
  group_group_view->setGeometry(10,10,
			      sizeHint().width()-20,sizeHint().height()-80);
  group_group_model=new RDGroupListModel(false,false,this);
  group_group_model->setFont(font());
  group_group_model->setPalette(palette());
  group_group_view->setModel(group_group_model);
  for(int i=2;i<group_group_model->columnCount();i++) {
    group_group_view->hideColumn(i);
  }
  connect(group_group_model,SIGNAL(modelReset()),
	  group_group_view,SLOT(resizeColumnsToContents()));
  connect(group_group_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(doubleClickedData(const QModelIndex &)));
  group_group_model->changeUser();

  //
  // OK Button
  //
  QPushButton *button=new QPushButton(tr("&OK"),this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  button=new QPushButton(tr("&Cancel"),this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize RDListGroups::sizeHint() const
{
  return QSize(400,370);
}


QSizePolicy RDListGroups::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDListGroups::doubleClickedData(const QModelIndex &index)
{
  okData();
}


void RDListGroups::okData()
{
  QModelIndexList rows=group_group_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  *group_name=group_group_model->groupName(rows.first());

  done(true);
}


void RDListGroups::cancelData()
{
  done(false);
}


void RDListGroups::closeEvent(QCloseEvent *e)
{
  cancelData();
}
