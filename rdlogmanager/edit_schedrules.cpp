// edit_schedrules.cpp
//
// Edit scheduler rules of a clock
//
//   (C) Copyright Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>

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
  setMaximumSize(sizeHint());

  edit_artist_sep_label=new QLabel(this);
  edit_artist_sep_label->setGeometry(QRect(10,10,130,20));
  edit_artist_sep_label->setFont(labelFont());
  edit_artist_sep_label->setText(tr("Artist Separation:"));
  edit_artist_sep_label->setEnabled(false);

  edit_artist_sep_spin=new QSpinBox(this);
  edit_artist_sep_spin->setGeometry(QRect(160,10,70,20));
  edit_artist_sep_spin->setMaxValue(10000);
  edit_artist_sep_spin->setValue(*edit_artistsep);
  edit_artist_sep_spin->setEnabled(false);

  QLabel *artist_sep_dep=
    new QLabel(this,tr("(This setting is deprecated and has been moved to events)"),this);
  artist_sep_dep->setFont(labelFont());
  artist_sep_dep->setGeometry(240,10,540,20);

  //
  //  Edit Button
  //
  QPushButton *list_edit_button=new QPushButton(this);
  list_edit_button->setGeometry(10,sizeHint().height()-60,80,50);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Import Button
  //
  QPushButton *list_import_button=new QPushButton(this);
  list_import_button->setGeometry(100,sizeHint().height()-60,80,50);
  list_import_button->setFont(buttonFont());
  list_import_button->setText(tr("&Import"));
  connect(list_import_button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));


  // List
  edit_schedcodes_view=new RDListView(this);
  edit_schedcodes_view->setGeometry(10,60,size().width()-20,size().height()-140);
  edit_schedcodes_view->setAllColumnsShowFocus(true);
  edit_schedcodes_view->addColumn(tr("CODE"));
  edit_schedcodes_view->addColumn(tr("MAX. IN A ROW"));
  edit_schedcodes_view->addColumn(tr("MIN. WAIT"));
  edit_schedcodes_view->addColumn(tr("DO NOT SCHEDULE AFTER"));
  edit_schedcodes_view->addColumn(tr("OR AFTER"));
  edit_schedcodes_view->addColumn(tr("OR AFTER"));
  edit_schedcodes_view->addColumn(tr("DESCRIPTION"));
  
  QLabel *list_box_label=
    new QLabel(edit_schedcodes_view,tr("Scheduler Codes:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(10,40,200,20);
  connect(edit_schedcodes_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

 edit_modified=false;
 Load();
}


EditSchedRules::~EditSchedRules()
{
}


QSize EditSchedRules::sizeHint() const
{
  return QSize(650,450);
} 


QSizePolicy EditSchedRules::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

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


void EditSchedRules::Close()
{
  RDListViewItem *item=(RDListViewItem *)edit_schedcodes_view->firstChild();

  *edit_rules_modified=true;

  while(item!=NULL) {
    edit_sched_rules_list->
      insertItem(item->id(),item->text(1).toInt(),item->text(2).toInt(),
		 item->text(3),item->text(4),item->text(5));
    item=(RDListViewItem *)item->nextSibling();
  }
}


void EditSchedRules::editData()
{
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
}


void EditSchedRules::importData()
{
  QString clockname="";
  QString str;
  RDListViewItem *item;
  QString sql;
  RDSqlQuery *q;

  ListClocks *listclocks=new ListClocks(&clockname,this);
  listclocks->setCaption(tr("Import Rules from Clock"));
  if(listclocks->exec()<0) {
    delete listclocks;
    return;
  }
  delete listclocks;
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
    edit_artist_sep_spin->setValue(*edit_artistsep);
    }
  delete q; 
  edit_modified=true;
}


void EditSchedRules::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col)
{
  editData();
}



void EditSchedRules::okData()
{
  *edit_artistsep=edit_artist_sep_spin->value();
  Close();
  done(0);
}


void EditSchedRules::cancelData()
{
  if(edit_modified) {
    switch(QMessageBox::question(this,tr("Rules Modified"),
				 tr("The rules have been modified.\nDo you want to save?"),QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel)) {
	case QMessageBox::Yes:
	  Close();
	  done(0);
	  break;

	case QMessageBox::No:
	  done(-1);
	  break;

	case QMessageBox::NoButton:
	  return;
    }
  }
  else {
    done(-1);
  }
}


void EditSchedRules::closeEvent(QCloseEvent *e)
{
  cancelData();
}
