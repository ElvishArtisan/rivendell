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

#include <qdialog.h>
#include <qstring.h>
#include <q3textedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qspinbox.h>
#include <qcombobox.h>

#include <rd.h>
#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlistviewitem.h>

#include "edit_schedrules.h"
#include "edit_schedcoderules.h"
#include "globals.h"
#include "list_clocks.h"

EditSchedRules::EditSchedRules(QString clock,unsigned *artistsep,RDSchedRulesList *schedruleslist,bool *rules_modified,QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

  edit_artistsep=artistsep;
  edit_rules_modified=rules_modified;
  sched_rules_list = schedruleslist;
  clockname = clock;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDLogManager - "+tr("Scheduler Rules"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  artistSepLabel = new QLabel(this);
  artistSepLabel->setGeometry( QRect( 10, 10, 130, 20 ) );
  artistSepLabel->setFont(font);
  artistSepLabel->setText(tr("Artist Separation:"));
  artistSepLabel->setEnabled(false);

  artistSepSpinBox = new QSpinBox(this);
  artistSepSpinBox->setGeometry( QRect( 160, 10, 70, 20 ) );
  artistSepSpinBox->setMaxValue( 10000 );
  artistSepSpinBox->setValue( *edit_artistsep );
  artistSepSpinBox->setEnabled(false);

  QLabel *artistSepDep=
    new QLabel(this,tr("(This setting is deprecated and has been moved to events)"),this);
  artistSepDep->setFont(font);
  artistSepDep->setGeometry(240,10,540,20);

  //
  //  Edit Button
  //
  QPushButton *list_edit_button=new QPushButton(this);
  list_edit_button->setGeometry(10,sizeHint().height()-60,80,50);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Import Button
  //
  QPushButton *list_import_button=new QPushButton(this);
  list_import_button->setGeometry(100,sizeHint().height()-60,80,50);
  list_import_button->setFont(font);
  list_import_button->setText(tr("&Import"));
  connect(list_import_button,SIGNAL(clicked()),this,SLOT(importData()));

  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));


  // List
  list_schedCodes_view=new RDListView(this);
  list_schedCodes_view->setGeometry(10,60,size().width()-20,size().height()-140);
  list_schedCodes_view->setAllColumnsShowFocus(true);
  list_schedCodes_view->addColumn(tr("CODE"));
  list_schedCodes_view->addColumn(tr("MAX. IN A ROW"));
  list_schedCodes_view->addColumn(tr("MIN. WAIT"));
  list_schedCodes_view->addColumn(tr("DO NOT SCHEDULE AFTER"));
  list_schedCodes_view->addColumn(tr("OR AFTER"));
  list_schedCodes_view->addColumn(tr("OR AFTER"));
  list_schedCodes_view->addColumn(tr("DESCRIPTION"));
  
  QLabel *list_box_label=
    new QLabel(list_schedCodes_view,tr("Scheduler Codes:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(10,40,200,20);
  connect(list_schedCodes_view,
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

  list_schedCodes_view->clear();
  for (int i=0; i<sched_rules_list->getNumberOfItems(); i++) {
    item=new RDListViewItem(list_schedCodes_view);
    item->setId(i);
    item->setText(0,sched_rules_list->getItemSchedCode(i));
    str=QString().sprintf("%d",sched_rules_list->getItemMaxRow(i));
    item->setText(1,str);
    str=QString().sprintf("%d",sched_rules_list->getItemMinWait(i));
    item->setText(2,str);
    item->setText(3,sched_rules_list->getItemNotAfter(i));
    item->setText(4,sched_rules_list->getItemOrAfter(i));
    item->setText(5,sched_rules_list->getItemOrAfterII(i));
    item->setText(6,sched_rules_list->getItemDescription(i));
  }
}


void EditSchedRules::Close()
{
  RDListViewItem *item=(RDListViewItem *)list_schedCodes_view->firstChild();

  *edit_rules_modified=true;

  while(item!=NULL) {
    sched_rules_list->
      insertItem(item->id(),item->text(1).toInt(),item->text(2).toInt(),
		 item->text(3),item->text(4),item->text(5));
    item=(RDListViewItem *)item->nextSibling();
  }
}


void EditSchedRules::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_schedCodes_view->selectedItem();
  if(item==NULL) {
    return;
  }
  editSchedCodeRules *edit_CodeRules=
    new editSchedCodeRules(item,sched_rules_list,this);
  if(edit_CodeRules->exec()>=0)
    {
    edit_modified=true; 
    }
  delete edit_CodeRules;
  edit_CodeRules=NULL;
}


void EditSchedRules::importData()
{
  QString clockname = "";
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

  list_schedCodes_view->clear();
  for (int i=0; i<import_list->getNumberOfItems(); i++)  
    {
    item=new RDListViewItem(list_schedCodes_view);
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
    *edit_artistsep = q->value(0).toUInt();
    artistSepSpinBox->setValue( *edit_artistsep );
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
  *edit_artistsep = artistSepSpinBox->value();
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
