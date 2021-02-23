// List_schedcodes.cpp
//
// The scheduler codes dialog for rdadmin
//
//   Based on original code by Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsyustems.com>
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

#include <rdcart.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdtextfile.h>

#include "add_schedcodes.h"
#include "edit_schedcodes.h"
#include "list_schedcodes.h"

ListSchedCodes::ListSchedCodes(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin  - "+tr("Rivendell Scheduler Codes List"));

  //
  // Dialogs
  //
  list_add_schedcode_dialog=new AddSchedCode(this);
  list_edit_schedcode_dialog=new EditSchedCode(this);

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Schedule Codes List
  //
  list_schedcodes_view=new RDTableView(this);
  list_schedcodes_model=new RDSchedCodeListModel(false,this);
  list_schedcodes_model->setFont(defaultFont());
  list_schedcodes_model->setPalette(palette());
  list_schedcodes_view->setModel(list_schedcodes_model);
  list_schedcodes_label=new QLabel(tr("Replicators:"),this);
  list_schedcodes_label->setFont(labelFont());
  connect(list_schedcodes_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_schedcodes_model,SIGNAL(modelReset()),
	  list_schedcodes_view,SLOT(resizeColumnsToContents()));
  list_schedcodes_view->resizeColumnsToContents();
}


ListSchedCodes::~ListSchedCodes()
{
  delete list_edit_schedcode_dialog;
  delete list_add_schedcode_dialog;
}


QSize ListSchedCodes::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListSchedCodes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSchedCodes::addData()
{
  QString scode;

  if(list_add_schedcode_dialog->exec(&scode)) {
    if(list_edit_schedcode_dialog->exec(scode)) {
      QModelIndex index=list_schedcodes_model->addSchedCode(scode);
      if(index.isValid()) {
	list_schedcodes_view->selectRow(index.row());
      }
    }
  }
  else {
    QString sql=QString("delete from SCHED_CODES where ")+
      "CODE=\""+RDEscapeString(scode)+"\"";
    RDSqlQuery::apply(sql);
    sql=QString("delete from RULE_LINES where ")+
      "CODE=\""+RDEscapeString(scode)+"\"";
    RDSqlQuery::apply(sql);
  }
}


void ListSchedCodes::editData()
{
  QModelIndexList rows=list_schedcodes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  if(list_edit_schedcode_dialog->
     exec(list_schedcodes_model->schedCode(rows.first()))) {
    list_schedcodes_model->refresh(rows.first());
  }
}


void ListSchedCodes::deleteData()
{
  QModelIndexList rows=list_schedcodes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString sql;
  QString warning;
  QString str;

  QString codename=list_schedcodes_model->schedCode(rows.first());
  if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Scheduler Code"),
			   tr("This operation will delete the selected scheduler code and")+
			   "\n"+tr("all of its associated data.")+" "+
			   tr("This operation cannot be undone.")+"\n\n"+
			   tr("Delete scheduler code")+" \""+codename+"\"?",
			   QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }

  sql=QString("delete from DROPBOX_SCHED_CODES where ")+
    "SCHED_CODE=\""+RDEscapeString(codename)+"\"";
  RDSqlQuery::apply(sql);

  sql=QString("delete from RULE_LINES where ")+
    "CODE=\""+RDEscapeString(codename)+"\"";
  RDSqlQuery::apply(sql);

  sql=QString("delete from SCHED_CODES where ")+
    "CODE=\""+RDEscapeString(codename)+"\"";
  RDSqlQuery::apply(sql);
  list_schedcodes_model->removeSchedCode(rows.first());
}


void ListSchedCodes::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListSchedCodes::closeData()
{
  done(true);
}


void ListSchedCodes::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_schedcodes_view->
    setGeometry(10,30,size().width()-120,size().height()-40);
}
