// list_groups.cpp
//
// List Rivendell Groups
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <math.h>

#include <qmessagebox.h>

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
  setModal(true);

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
  //  Rename Button
  //
  list_rename_button=new QPushButton(this);
  list_rename_button->setFont(buttonFont());
  list_rename_button->setText(tr("&Rename"));
  connect(list_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Report Button
  //
  list_report_button=new QPushButton(this);
  list_report_button->setFont(buttonFont());
  list_report_button->setText(tr("Generate\n&Report"));
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Group List
  //
  list_groups_view=new RDListView(this);
  list_groups_view->setAllColumnsShowFocus(true);
  list_groups_view->addColumn(tr("Name"));
  list_groups_view->addColumn(tr("Description"));
  list_groups_view->addColumn(tr("Start Cart"));
  list_groups_view->setColumnAlignment(2,Qt::AlignCenter);
  list_groups_view->addColumn(tr("End Cart"));
  list_groups_view->setColumnAlignment(3,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("Enforce Range"));
  list_groups_view->setColumnAlignment(4,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("Default Type"));
  list_groups_view->setColumnAlignment(5,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("Traffic Report"));
  list_groups_view->setColumnAlignment(6,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("Music Report"));
  list_groups_view->setColumnAlignment(7,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("Now & Next"));
  list_groups_view->setColumnAlignment(8,Qt::AlignHCenter);
  QLabel *list_box_label=new QLabel(list_groups_view,tr("&Groups:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,11,85,19);
  connect(list_groups_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();
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
  if(add_group->exec()<0) {
    delete add_group;
    return;
  }
  delete add_group;
  add_group=NULL;
  RDListViewItem *item=new RDListViewItem(list_groups_view);
  item->setText(0,group);
  RefreshItem(item);
  item->setSelected(true);
  list_groups_view->setCurrentItem(item);
  list_groups_view->ensureItemVisible(item);
}


void ListGroups::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_groups_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditGroup *edit_group=new EditGroup(item->text(0),this);
  edit_group->exec();
  delete edit_group;
  edit_group=NULL;
  RefreshItem(item);
}


void ListGroups::renameData()
{
  RDListViewItem *item=(RDListViewItem *)list_groups_view->selectedItem();
  if(item==NULL) {
    return;
  }

  QString groupname=item->text(0);
  RenameGroup *rename_group=new RenameGroup(groupname,this);
  rename_group->exec();
  delete rename_group;
  rename_group=NULL;
  RefreshList();
}


void ListGroups::deleteData()
{
  RDListViewItem *item=(RDListViewItem *)list_groups_view->selectedItem();
  if(item==NULL) {
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QString warning;
  int carts=0;

  QString groupname=item->text(0);
  if(groupname.isEmpty()) {
    return;
  }
  sql=QString("select NUMBER from CART where ")+
    "GROUP_NAME=\""+RDEscapeString(groupname)+"\"";
  q=new RDSqlQuery(sql);
  if((carts=q->size())>0) {
    warning=QString().sprintf("%d ",carts)+tr("member carts will be deleted along with group")+" \""+groupname+"\"!";
  }
  warning+=tr("Are you sure you want to delete group")+" \""+groupname+"\"?";
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
  
  //
  // Delete Member Audio Perms
  //
  sql=QString("delete from AUDIO_PERMS where ")+
    "GROUP_NAME=\""+RDEscapeString(groupname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete Member User Perms
  //
  sql=QString("delete from USER_PERMS where ")+
    "GROUP_NAME=\""+RDEscapeString(groupname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete Replicator Map Records
  //
  sql=QString("delete from REPLICATOR_MAP where ")+
    "GROUP_NAME=\""+RDEscapeString(groupname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete from Group List
  //
  sql=QString("delete from GROUPS where ")+
    "NAME=\""+RDEscapeString(groupname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  item->setSelected(false);
  delete item;
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
  report+="-Name----- -Description-------------------------------- -Cart Range---- Enf DefType Mus Tfc N&N\n";

  //
  // Generate Body
  //
  sql=QString("select ")+
    "NAME,"+                // 00
    "DESCRIPTION,"+         // 01
    "DEFAULT_LOW_CART,"+    // 02
    "DEFAULT_HIGH_CART,"+   // 03
    "ENFORCE_CART_RANGE,"+  // 04
    "DEFAULT_CART_TYPE,"+   // 05
    "REPORT_MUS,"+          // 06
    "REPORT_TFC,"+          // 07
    "ENABLE_NOW_NEXT "+     // 08
    "from GROUPS order by NAME";
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
      report+=QString().sprintf("%06u - %06u ",q->value(2).toUInt(),
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
    // Now & Next
    //
    report+=QString(" ")+q->value(8).toString();

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


void ListGroups::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				   int col)
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
  list_groups_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListGroups::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_groups_view->clear();
  sql=QString("select ")+
    "NAME,"+                // 00
    "DESCRIPTION,"+         // 01
    "DEFAULT_LOW_CART,"+    // 02
    "DEFAULT_HIGH_CART,"+   // 03
    "ENFORCE_CART_RANGE,"+  // 04
    "DEFAULT_CART_TYPE,"+   // 05
    "REPORT_TFC,"+          // 06
    "REPORT_MUS,"+          // 07
    "ENABLE_NOW_NEXT,"+     // 08
    "COLOR "+               // 09
    "from GROUPS";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_groups_view);
    WriteItem(item,q);
  }
  delete q;
}


void ListGroups::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "NAME,"+                // 00
    "DESCRIPTION,"+         // 01
    "DEFAULT_LOW_CART,"+    // 02
    "DEFAULT_HIGH_CART,"+   // 03
    "ENFORCE_CART_RANGE,"+  // 04
    "DEFAULT_CART_TYPE,"+   // 05
    "REPORT_TFC,"+          // 06
    "REPORT_MUS,"+          // 07
    "ENABLE_NOW_NEXT,"+     // 08
    "COLOR "+               // 09
    "from GROUPS where "+
    "NAME=\""+RDEscapeString(item->text(0))+"\"";
  q=new RDSqlQuery(sql);
  if(q->next()) {
    WriteItem(item,q);
  }
  delete q;
}


void ListGroups::WriteItem(RDListViewItem *item,RDSqlQuery *q)
{
  item->setText(0,q->value(0).toString());
  item->setTextColor(0,q->value(9).toString(),QFont::Bold);
  item->setText(1,q->value(1).toString());
  if(q->value(2).toUInt()>0) {
    item->setText(2,QString().sprintf("%06u",q->value(2).toUInt()));
    item->setText(3,QString().sprintf("%06u",q->value(3).toUInt()));
  }
  else {
    item->setText(2,"[none]");
    item->setText(3,"[none]");
  }
  item->setText(4,q->value(4).toString());
  switch((RDCart::Type)q->value(5).toUInt()) {
  case RDCart::Audio:
    item->setText(5,"Audio");
    break;
	
  case RDCart::Macro:
    item->setText(5,"Macro");
    break;
	
  default:	
    item->setText(5,"[none]");
    break;
  }
  item->setText(6,q->value(6).toString());
  item->setText(7,q->value(7).toString());
  item->setText(8,q->value(8).toString());
}
