// edit_schedrules.cpp
//
// Edit scheduler rules of a clock
//
//   (C) Copyright Stefan Gabriel <stg@st-gabriel.de>
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

#include <QMessageBox>

#include <rdescape_string.h>

#include "edit_schedrules.h"
#include "edit_schedcoderules.h"
#include "globals.h"
#include "list_clocks.h"

EditSchedRules::EditSchedRules(QString clock,unsigned *artistsep,
			       RDSchedRulesList *schedruleslist,
			       bool *rules_modified,QWidget *parent)
  : RDDialog(parent)
{
  edit_artistsep=artistsep;
  edit_rules_modified=rules_modified;
  edit_sched_rules_list=schedruleslist;
  edit_clockname=clock;

  setWindowTitle("RDLogManager - "+tr("Scheduler Rules"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Dialogs
  //
  list_editrules_dialog=new EditSchedCodeRules(this);

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Import Button
  //
  list_import_button=new QPushButton(this);
  list_import_button->setFont(buttonFont());
  list_import_button->setText(tr("&Import"));
  connect(list_import_button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Rules List
  //
  edit_schedcodes_view=new RDTableView(this);
  edit_schedcodes_view->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-80);
  edit_schedcodes_model=new SchedRulesModel(clock,this);
  edit_schedcodes_model->setFont(font());
  edit_schedcodes_model->setPalette(palette());
  edit_schedcodes_view->setModel(edit_schedcodes_model);
  connect(edit_schedcodes_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(edit_schedcodes_model,SIGNAL(modelReset()),
	  edit_schedcodes_view,SLOT(resizeColumnsToContents()));
  edit_schedcodes_view->resizeColumnsToContents();

  list_box_label=
    new QLabel(edit_schedcodes_view,tr("Scheduler Codes:"),this);
  list_box_label->setFont(labelFont());
  edit_modified=false;
}


EditSchedRules::~EditSchedRules()
{
  delete list_editrules_dialog;
}


QSize EditSchedRules::sizeHint() const
{
  return QSize(650,450);
} 


QSizePolicy EditSchedRules::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSchedRules::editData()
{
  QModelIndexList rows=edit_schedcodes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  if(list_editrules_dialog->exec(edit_schedcodes_model->ruleId(rows.first()))) {
    edit_schedcodes_model->refresh(rows.first());
  }
}


void EditSchedRules::importData()
{
  QString src_clockname;
  QString sql;
  RDSqlQuery *q;

  ListClocks *listclocks=new ListClocks(&src_clockname,this);
  listclocks->setWindowTitle("RDLogManager - "+tr("Import Rules from Clock"));
  if(!listclocks->exec()<0) {
    delete listclocks;
    return;
  }
  delete listclocks;

  sql=QString("select ")+
    "ID,"+           // 00
    "CODE,"+         // 01
    "MAX_ROW,"+      // 02
    "MIN_WAIT,"+     // 03
    "NOT_AFTER,"+    // 04
    "OR_AFTER,"+     // 05
    "OR_AFTER_II "+  // 06
    "from RULE_LINES where "+
    "CLOCK_NAME=\""+RDEscapeString(src_clockname)+"\" "+
    "order by CODE";

  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("update RULE_LINES set ");
    if(q->value(2).isNull()) {
      sql+="MAX_ROW=null,";
    }
    else {
      sql+=QString().sprintf("MAX_ROW=%u,",q->value(2).toUInt());
    }
    if(q->value(3).isNull()) {
      sql+="MIN_WAIT=null,";
    }
    else {
      sql+=QString().sprintf("MIN_WAIT=%u,",q->value(3).toUInt());
    }
    if(q->value(4).isNull()) {
      sql+="NOT_AFTER=null,";
    }
    else {
      sql+="NOT_AFTER=\""+RDEscapeString(q->value(4).toString())+"\",";
    }
    
    if(q->value(5).isNull()) {
      sql+="OR_AFTER=null,";
    }
    else {
      sql+="OR_AFTER=\""+RDEscapeString(q->value(5).toString())+"\",";
    }
    
    if(q->value(6).isNull()) {
      sql+="OR_AFTER_II=null ";
    }
    else {
      sql+="OR_AFTER_II=\""+RDEscapeString(q->value(6).toString())+"\" ";
    }
    sql+=QString("where ")+
      "CLOCK_NAME=\""+RDEscapeString(edit_clockname)+"\" && "+
      "CODE=\""+RDEscapeString(q->value(1).toString())+"\"";

    RDSqlQuery::apply(sql);
  }
  delete q;

  edit_schedcodes_model->refresh();
}


void EditSchedRules::doubleClickedData(const QModelIndex &index)
{
  editData();
}



void EditSchedRules::closeData()
{
  done(true);
}


void EditSchedRules::closeEvent(QCloseEvent *e)
{
  closeData();
}


void EditSchedRules::resizeEvent(QResizeEvent *e)
{
  list_box_label->setGeometry(10,2,200,20);
  edit_schedcodes_view->
    setGeometry(10,22,size().width()-20,size().height()-102);

  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_import_button->setGeometry(100,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
