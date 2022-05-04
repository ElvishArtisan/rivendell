// list_groups.cpp
//
// List Rivendell Groups
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdcart.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdreport.h>
#include <rdtextfile.h>

#include "add_group.h"
#include "globals.h"
#include "edit_group.h"
#include "list_groups.h"
#include "rename_group.h"

ListGroups::ListGroups(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Group List"));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Rename Button
  //
  list_rename_button=new QPushButton(this);
  list_rename_button->setFont(buttonFont());
  list_rename_button->setText(tr("Rename"));
  connect(list_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Report Button
  //
  list_report_button=new QPushButton(this);
  list_report_button->setFont(buttonFont());
  list_report_button->setText(tr("Generate\nReport"));
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Group List
  //
  list_groups_view=new RDTableView(this);
  list_groups_view->setSortingEnabled(true);
  list_groups_view->sortByColumn(0,Qt::AscendingOrder);
  list_groups_model=new RDGroupListModel(false,false,true,this);
  list_groups_model->setFont(defaultFont());
  list_groups_model->setPalette(palette());
  list_groups_view->setModel(list_groups_model);
  list_groups_view->resizeColumnsToContents();
  connect(list_groups_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_groups_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));
}


ListGroups::~ListGroups()
{
}


QSize ListGroups::sizeHint() const
{
  return QSize(1024,750);
} 


QSizePolicy ListGroups::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListGroups::addData()
{
  QString grpname;

  AddGroup *add_group=new AddGroup(&grpname,this);
  if(!add_group->exec()) {
    delete add_group;
    return;
  }
  delete add_group;
  QModelIndex index=list_groups_model->addGroup(grpname);
  if(index.isValid()) {
    list_groups_view->selectRow(index.row());
  }
}


void ListGroups::editData()
{
  QModelIndexList rows=list_groups_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditGroup *edit_group=
    new EditGroup(list_groups_model->groupName(rows.first()),this);
  if(edit_group->exec()) {
    list_groups_model->refresh(rows.first());
  }
  delete edit_group;
  edit_group=NULL;
}


void ListGroups::renameData()
{
  QModelIndexList rows=list_groups_view->selectionModel()->selectedRows();
  QModelIndex index;

  if(rows.size()!=1) {
    return;
  }

  QString grpname=list_groups_model->groupName(rows.first());
  QString newgrpname;
  RenameGroup *rename_group=new RenameGroup(grpname,this);
  switch((RenameGroup::Result)rename_group->exec(&newgrpname)) {
  case RenameGroup::Renamed:
    index=list_groups_model->renameGroup(grpname,newgrpname);
    if(index.isValid()) {
      list_groups_view->selectRow(index.row());
    }
    break;

  case RenameGroup::Merged:
    list_groups_model->removeGroup(grpname);
    index=list_groups_model->indexOf(newgrpname);
    if(index.isValid()) {
      list_groups_view->selectRow(index.row());
    }
    break;

  case RenameGroup::Cancelled:
    break;
  }
  delete rename_group;
  rename_group=NULL;
}


void ListGroups::deleteData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  QString warning;
  int carts=0;
  QString err_msg;
  QModelIndexList rows=list_groups_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  QString grpname=list_groups_model->groupName(rows.first());
  if(grpname.isEmpty()) {
    return;
  }
  sql=QString("select ")+
    "`NUMBER` "+  // 00
    "from `CART` where "+
    "`GROUP_NAME`='"+RDEscapeString(grpname)+"'";
  q=new RDSqlQuery(sql);
  if((carts=q->size())>0) {
    warning=QString::asprintf("%d ",carts)+tr("member carts will be deleted along with group")+" \""+grpname+"\"!\n";
  }
  warning+=tr("Are you sure you want to delete group")+" \""+grpname+"\"?";
  switch(QMessageBox::warning(this,tr("Delete Group"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
  case QMessageBox::No:
  case QMessageBox::NoButton:
    delete q;
    return;

  default:
    break;
  }

  //
  // Delete Member Carts
  //
  RDCart *cart;
  while(q->next()) {
    cart=new RDCart(q->value(0).toUInt());
    cart->remove(rda->station(),rda->user(),rda->config());
    delete cart;
  }
  delete q;
  
  if(!RDGroup::remove(grpname,&err_msg)) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Unable to remove group!")+"\n"+
			 "["+err_msg+"]");
    return;
  }

  list_groups_model->removeGroup(grpname);
}


void ListGroups::reportData()
{
  QString sql;
  RDSqlQuery *q;
  QString report;

  //
  // Generate Header
  //
  report=RDReport::center("Rivendell Group Report",94)+"\n";
  report+=QString("Generated: ")+
    QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"\n";
  report+="\n";
  report+="-Name----- -Description-------------------------------- -Cart Range---- Enf DefType Mus Tfc\n";

  //
  // Generate Body
  //
  sql=QString("select ")+
    "`NAME`,"+                // 00
    "`DESCRIPTION`,"+         // 01
    "`DEFAULT_LOW_CART`,"+    // 02
    "`DEFAULT_HIGH_CART`,"+   // 03
    "`ENFORCE_CART_RANGE`,"+  // 04
    "`DEFAULT_CART_TYPE`,"+   // 05
    "`REPORT_MUS`,"+          // 06
    "`REPORT_TFC` "+          // 07
    "from `GROUPS` order by `NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Group Name
    //
    report+=RDReport::leftJustify(q->value(0).toString(),10)+" ";

    //
    // Group Description
    //
    report+=RDReport::leftJustify(q->value(1).toString(),44)+" ";

    //
    // Cart Range
    //
    if((q->value(2).isNull())||(q->value(2).toUInt()==0)) {
      report+="    [none]      ";
    }
    else {
      report+=QString::asprintf("%06u - %06u ",q->value(2).toUInt(),
				q->value(3).toUInt());
    }

    //
    // Enforce Range
    //
    report+=QString(" ")+q->value(4).toString()+"  ";

    //
    // Default Cart Type
    //
    switch((RDCart::Type)q->value(5).toInt()) {
    case RDCart::Audio:
      report+="Audio   ";
      break;

    case RDCart::Macro:
      report+="Macro   ";
      break;

    default:
      report+="Unknown ";
      break;
    }

    //
    // Music Reports
    //
    report+=QString(" ")+q->value(6).toString()+"  ";
    
    //
    // Traffic Reports
    //
    report+=QString(" ")+q->value(7).toString()+"  ";
    
    //
    // End of Line
    //
    report+="\n";
  }
  delete q;

  //
  // Display Report
  //
  RDTextFile(report);
}


void ListGroups::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListGroups::modelResetData()
{
  list_groups_view->resizeColumnsToContents();
}


void ListGroups::closeData()
{
  done(true);
}


void ListGroups::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_rename_button->setGeometry(size().width()-90,150,80,50);
  list_delete_button->setGeometry(size().width()-90,210,80,50);
  list_report_button->setGeometry(size().width()-90,300,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_groups_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
