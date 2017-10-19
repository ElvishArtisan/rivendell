// list_grids.cpp
//
// List Rivendell Log Grids
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdialog.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rddb.h>
#include <rd.h>
#include <rdevent.h>
#include <rdcreate_log.h>

#include <list_grids.h>
#include <edit_grid.h>
#include <globals.h>

ListGrids::ListGrids(QWidget *parent)
  : QDialog(parent,"",true)
{
  setCaption(tr("Log Grids"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Grids List
  //
  edit_grids_list=new QListView(this);
  edit_grids_list->setGeometry(10,10,
				sizeHint().width()-20,sizeHint().height()-80);
  edit_grids_list->setAllColumnsShowFocus(true);
  edit_grids_list->setItemMargin(5);
  edit_grids_list->addColumn(tr("Name"));
  edit_grids_list->addColumn(tr("Description"));
  connect(edit_grids_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Edit Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Edit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Close Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("C&lose"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
}


QSize ListGrids::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListGrids::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListGrids::editData()
{
  QListViewItem *item=edit_grids_list->selectedItem();
  if(item==NULL) {
    return;
  }
  EditGrid *grid_dialog=new EditGrid(item->text(0),this);
  grid_dialog->exec();
  delete grid_dialog;
}


void ListGrids::doubleClickedData(QListViewItem *item,const QPoint &,int)
{
  editData();
}


void ListGrids::closeData()
{
  done(0);
}


void ListGrids::RefreshList()
{
  QListViewItem *prev_item=edit_grids_list->selectedItem();
  QString sql="select NAME,DESCRIPTION from SERVICES";

  edit_grids_list->clear();
  RDSqlQuery *q=new RDSqlQuery(sql);
  QListViewItem *item=NULL;
  while(q->next()) {
    item=new QListViewItem(edit_grids_list);
    item->setText(0,q->value(0).toString());
    item->setText(1,q->value(1).toString());
  }
  delete q;
  if(prev_item!=NULL) {
    edit_grids_list->setSelected(item,true);
  }
}
