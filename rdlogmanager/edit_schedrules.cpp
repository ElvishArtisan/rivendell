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

  /*
  edit_schedcodes_view=new RDListView(this);
  edit_schedcodes_view->setAllColumnsShowFocus(true);
  edit_schedcodes_view->addColumn(tr("CODE"));
  edit_schedcodes_view->addColumn(tr("MAX. IN A ROW"));
  edit_schedcodes_view->addColumn(tr("MIN. WAIT"));
  edit_schedcodes_view->addColumn(tr("DO NOT SCHEDULE AFTER"));
  edit_schedcodes_view->addColumn(tr("OR AFTER"));
  edit_schedcodes_view->addColumn(tr("OR AFTER"));
  edit_schedcodes_view->addColumn(tr("DESCRIPTION"));
  */
  
  list_box_label=
    new QLabel(edit_schedcodes_view,tr("Scheduler Codes:"),this);
  list_box_label->setFont(labelFont());
  edit_modified=false;
 // Load();
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
  /*
  RDListViewItem *item=(RDListViewItem *)edit_schedcodes_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditSchedCodeRules *edit_CodeRules=
    new EditSchedCodeRules(item,edit_sched_rules_list,this);
  if(edit_CodeRules->exec()>=0)
    {
    edit_modified=true; 
    }
  delete edit_CodeRules;
  edit_CodeRules=NULL;
  */
}


void EditSchedRules::importData()
{
  /*
  QModelIndexList rows=edit_schedcodes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  
  //  RDListViewItem *item;
  QString sql;
  RDSqlQuery *q;

  ListClocks *listclocks=new ListClocks(&clockname,this);
  listclocks->setWindowTitle("RDLogManager - "+tr("Import Rules from Clock"));
  if(!listclocks->exec()) {
    delete listclocks;
    return;
  }
  delete listclocks;

  sql=QString("select ")+
    "
  RDSchedRulesList *import_list=new RDSchedRulesList(clockname,rda->config()); 

  edit_schedcodes_view->clear();
  for (int i=0; i<import_list->getNumberOfItems(); i++)  
    {
    item=new RDListViewItem(edit_schedcodes_view);
    item->setId(i);
    item->setText(0,import_list->getItemSchedCode(i));
    str=QString().sprintf("%d",import_list->getItemMaxRow(i));
    item->setText(1,str);
    str=QString().sprintf("%d",import_list->getItemMinWait(i));
    item->setText(2,str);
    item->setText(3,import_list->getItemNotAfter(i));
    item->setText(4,import_list->getItemOrAfter(i));
    item->setText(5,import_list->getItemOrAfterII(i));
    item->setText(6,import_list->getItemDescription(i));
    }
  delete import_list;

  sql=QString("select ARTISTSEP from CLOCKS where ")+
    "NAME=\""+RDEscapeString(clockname)+"\"";
  q=new RDSqlQuery(sql);
  if (q->first())
    {
    *edit_artistsep=q->value(0).toUInt();
    //    edit_artist_sep_spin->setValue(*edit_artistsep);
    }
  delete q; 
  edit_modified=true;
  */
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

/*
void EditSchedRules::Load()
{
  QString str;
  RDListViewItem *item;

  edit_schedcodes_view->clear();
  for (int i=0; i<edit_sched_rules_list->getNumberOfItems(); i++) {
    item=new RDListViewItem(edit_schedcodes_view);
    item->setId(i);
    item->setText(0,edit_sched_rules_list->getItemSchedCode(i));
    str=QString().sprintf("%d",edit_sched_rules_list->getItemMaxRow(i));
    item->setText(1,str);
    str=QString().sprintf("%d",edit_sched_rules_list->getItemMinWait(i));
    item->setText(2,str);
    item->setText(3,edit_sched_rules_list->getItemNotAfter(i));
    item->setText(4,edit_sched_rules_list->getItemOrAfter(i));
    item->setText(5,edit_sched_rules_list->getItemOrAfterII(i));
    item->setText(6,edit_sched_rules_list->getItemDescription(i));
  }
}
*/
