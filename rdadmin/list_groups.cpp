// list_groups.cpp
//
// List Rivendell Groups
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdescape_string.h>
#include <rdtextfile.h>

#include "add_group.h"
#include "edit_group.h"
#include "globals.h"
#include "list_groups.h"
#include "rename_group.h"

ListGroups::ListGroups(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Group List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Rename Button
  //
  list_rename_button=new QPushButton(this);
  list_rename_button->setFont(font);
  list_rename_button->setText(tr("&Rename"));
  connect(list_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Report Button
  //
  list_report_button=new QPushButton(this);
  list_report_button->setFont(font);
  list_report_button->setText(tr("Generate\n&Report"));
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Group List
  //
  QLabel *list_box_label=new QLabel(tr("&Groups:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "NAME,"+                   // 00
    "DESCRIPTION,"+            // 01
    "DEFAULT_LOW_CART,"+       // 02
    "DEFAULT_HIGH_CART,"+      // 03
    "ENFORCE_CART_RANGE,"+     // 04
    "DEFAULT_CART_TYPE,"+      // 05
    "REPORT_TFC,"+             // 06
    "REPORT_MUS,"+             // 07
    "ENABLE_NOW_NEXT,"+        // 08
    "COLOR "+                  // 09
    "from GROUPS";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Name"));
  list_model->setFieldType(0,RDSqlTableModel::ColorTextType,9);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Description"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Start Cart"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("End Cart"));
  list_model->setHeaderData(4,Qt::Horizontal,tr("Enforce Range"));
  list_model->setHeaderData(5,Qt::Horizontal,tr("Default Type"));
  list_model->setHeaderData(6,Qt::Horizontal,tr("Traffic Report"));
  list_model->setHeaderData(7,Qt::Horizontal,tr("Music Report"));
  list_model->setHeaderData(8,Qt::Horizontal,tr("Now & Next"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(9);
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
}


ListGroups::~ListGroups()
{
}


QSize ListGroups::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListGroups::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListGroups::addData()
{
  QString group;

  AddGroup *add_group=new AddGroup(&group,this);
  if(add_group->exec()==0) {
    list_model->update();
  }
  delete add_group;
}


void ListGroups::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditGroup *edit_group=
      new EditGroup(s->selectedRows()[0].data().toString(),this);
    edit_group->exec();
    delete edit_group;
    list_model->update();
  }
}


void ListGroups::renameData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    RenameGroup *rename_group=
      new RenameGroup(s->selectedRows()[0].data().toString(),this);
    rename_group->exec();
    delete rename_group;
    list_model->update();
  }
}


void ListGroups::deleteData()
{
  unsigned carts=0;

  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    QString groupname=s->selectedRows()[0].data().toString();
    if((carts=RDGroup::cartQuantity(groupname))>0) {
      if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Group"),
			      QString().sprintf("%u ",carts)+
			      tr("member carts will deleted along with group")+
			      " \""+groupname+"\" !\n"+
			      tr("Are you sure you want to delete group")+
			      " \""+groupname+"\" ?",
			      QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	return;
      }
    }
    else {
      if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Group"),
			      tr("Are you sure you want to delete group")+
			      " \""+groupname+"\" ?",
			      QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	return;
      }
    }
    RDGroup::remove(groupname);
    list_model->update();
  }
}


void ListGroups::reportData()
{
  QString sql;
  RDSqlQuery *q;
  QString report;

  //
  // Generate Header
  //
  report="                                  Rivendell Group Report\n";
  report+=QString().sprintf("Generated: %s\n",
			    (const char *)QDateTime(QDate::currentDate(),
						    QTime::currentTime()).
			    toString("MM/dd/yyyy - hh:mm:ss"));
  report+="\n";
  report+="-Name----- -Description-------------------------------- -Cart Range---- Enf DefType Mus Tfc N&N\n";

  //
  // Generate Body
  //
  sql=QString("select ")+
    "NAME,"+                   // 00
    "DESCRIPTION,"+            // 01
    "DEFAULT_LOW_CART,"+       // 02
    "DEFAULT_HIGH_CART,"+      // 03
    "ENFORCE_CART_RANGE,"+     // 04
    "DEFAULT_CART_TYPE,"+      // 05
    "REPORT_MUS,REPORT_TFC,"+  // 06
    "ENABLE_NOW_NEXT "+        // 07
    "from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Group Name
    //
    report+=QString().sprintf("%-10s ",(const char *)q->value(0).toString());

    //
    // Group Description
    //
    report+=QString().sprintf("%-44s ",(const char *)q->value(1).toString());

    //
    // Cart Range
    //
    if((q->value(2).isNull())||(q->value(2).toUInt()==0)) {
      report+="    [none]      ";
    }
    else {
      report+=QString().sprintf("%06u - %06u ",q->value(2).toUInt(),
				q->value(3).toUInt());
    }

    //
    // Enforce Range
    //
    report+=QString().sprintf(" %s  ",(const char *)q->value(4).toString());

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
    report+=QString().sprintf(" %s  ",(const char *)q->value(6).toString());
    
    //
    // Traffic Reports
    //
    report+=QString().sprintf(" %s  ",(const char *)q->value(7).toString());
    
    //
    // Now & Next
    //
    report+=QString().sprintf(" %s",(const char *)q->value(8).toString());

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


void ListGroups::closeData()
{
  done(0);
}


void ListGroups::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_rename_button->setGeometry(size().width()-90,150,80,50);
  list_delete_button->setGeometry(size().width()-90,210,80,50);
  list_report_button->setGeometry(size().width()-90,300,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,30,size().width()-120,size().height()-40); 
}
