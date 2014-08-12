// list_groups.cpp
//
// List Rivendell Groups
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_groups.cpp,v 1.27.6.1 2013/11/13 23:36:34 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>
#include <rddb.h>

#include <rdcart.h>
#include <rdtextfile.h>
#include <rdescape_string.h>

#include <globals.h>
#include <list_groups.h>
#include <edit_group.h>
#include <add_group.h>
#include <rename_group.h>


ListGroups::ListGroups(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("Rivendell Group List"));

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
  list_add_button=new QPushButton(this,"list_add_button");
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this,"list_edit_button");
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Rename Button
  //
  list_rename_button=new QPushButton(this,"list_rename_button");
  list_rename_button->setFont(font);
  list_rename_button->setText(tr("&Rename"));
  connect(list_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this,"list_delete_button");
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Report Button
  //
  list_report_button=new QPushButton(this,"list_report_button");
  list_report_button->setFont(font);
  list_report_button->setText(tr("Generate\n&Report"));
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this,"list_close_button");
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Group List
  //
  list_groups_view=new RDListView(this,"list_groups_view");
  list_groups_view->setFont(list_font);
  list_groups_view->setAllColumnsShowFocus(true);
  list_groups_view->addColumn(tr("NAME"));
  list_groups_view->addColumn(tr("DESCRIPTION"));
  list_groups_view->addColumn(tr("START CART"));
  list_groups_view->setColumnAlignment(2,Qt::AlignCenter);
  list_groups_view->addColumn(tr("END CART"));
  list_groups_view->setColumnAlignment(3,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("ENFORCE RANGE"));
  list_groups_view->setColumnAlignment(4,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("DEFAULT TYPE"));
  list_groups_view->setColumnAlignment(5,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("TRAFFIC REPORT"));
  list_groups_view->setColumnAlignment(6,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("MUSIC REPORT"));
  list_groups_view->setColumnAlignment(7,Qt::AlignHCenter);
  list_groups_view->addColumn(tr("NOW & NEXT"));
  list_groups_view->setColumnAlignment(8,Qt::AlignHCenter);
  QLabel *list_box_label=new QLabel(list_groups_view,tr("&Groups:"),
				    this,"list_box_label");
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
  connect(list_groups_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

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

  AddGroup *add_group=new AddGroup(&group,this,"add_group");
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
  EditGroup *edit_group=new EditGroup(item->text(0),this,"edit_group");
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
  RenameGroup *rename_group=new RenameGroup(groupname,this,"rename_group");
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
  QString str;

  QString groupname=item->text(0);
  if(groupname.isEmpty()) {
    return;
  }
  sql=QString().sprintf("select NUMBER from CART where GROUP_NAME=\"%s\"",
			(const char *)groupname);
  q=new RDSqlQuery(sql);
  if((carts=q->size())>0) {
    str=QString(tr("member carts will be deleted along with group"));
    warning=QString().
      sprintf("%d %s %s!\n",
	      carts,(const char *)str,(const char *)groupname);
  }
  str=QString(tr("Are you sure you want to delete group"));
  warning+=QString().sprintf("%s %s?",(const char *)str,
			     (const char *)groupname);
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
    cart->remove(admin_station,admin_user,admin_config);
    delete cart;
  }
  delete q;
  
  //
  // Delete Member Audio Perms
  //
  sql=QString().sprintf("delete from AUDIO_PERMS where GROUP_NAME=\"%s\"",
			(const char *)RDEscapeString(groupname));
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete Member User Perms
  //
  sql=QString().sprintf("delete from USER_PERMS where GROUP_NAME=\"%s\"",
			(const char *)RDEscapeString(groupname));
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete Replicator Map Records
  //
  sql=QString().sprintf("delete from REPLICATOR_MAP where GROUP_NAME=\"%s\"",
			(const char *)RDEscapeString(groupname));
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete from Group List
  //
  sql=QString().sprintf("delete from GROUPS where NAME=\"%s\"",
			(const char *)RDEscapeString(groupname));
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
  sql="select NAME,DESCRIPTION,DEFAULT_LOW_CART,DEFAULT_HIGH_CART,\
       ENFORCE_CART_RANGE,DEFAULT_CART_TYPE,REPORT_MUS,REPORT_TFC,\
       ENABLE_NOW_NEXT from GROUPS order by NAME";
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


void ListGroups::doubleClickedData(QListViewItem *item,const QPoint &pt,
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
  q=new RDSqlQuery("select NAME,DESCRIPTION,DEFAULT_LOW_CART,DEFAULT_HIGH_CART,\
                   ENFORCE_CART_RANGE,DEFAULT_CART_TYPE,REPORT_TFC,REPORT_MUS,\
                   ENABLE_NOW_NEXT,COLOR from GROUPS",
		  0);
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

  sql=QString().sprintf("select NAME,DESCRIPTION,DEFAULT_LOW_CART,\
                         DEFAULT_HIGH_CART,ENFORCE_CART_RANGE,\
                         DEFAULT_CART_TYPE,REPORT_TFC,REPORT_MUS,\
                         ENABLE_NOW_NEXT,COLOR from GROUPS where NAME=\"%s\"",
			(const char *)item->text(0));
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
