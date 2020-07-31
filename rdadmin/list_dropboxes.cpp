// list_dropboxes.cpp
//
// List Rivendell Dropboxes
//
//   (C) Copyright 2002-2019 Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdnotification.h>

#include "edit_dropbox.h"
#include "list_dropboxes.h"

ListDropboxes::ListDropboxes(const QString &stationname,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  list_stationname=stationname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Rivendell Dropbox Configurations on")+" "+
		 stationname);

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
  //  Duplicate Button
  //
  list_duplicate_button=new QPushButton(this);
  list_duplicate_button->setFont(buttonFont());
  list_duplicate_button->setText(tr("D&uplicate"));
  connect(list_duplicate_button,SIGNAL(clicked()),this,SLOT(duplicateData()));

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
  // Group List
  //
  list_dropboxes_view=new RDListView(this);
  list_dropboxes_view->setAllColumnsShowFocus(true);
  list_dropboxes_view->addColumn(tr("ID"));
  list_dropboxes_view->setColumnAlignment(0,Qt::AlignRight);
  list_dropboxes_view->addColumn(tr("Group"));
  list_dropboxes_view->setColumnAlignment(1,Qt::AlignLeft);
  list_dropboxes_view->addColumn(tr("Path"));
  list_dropboxes_view->setColumnAlignment(2,Qt::AlignLeft);
  list_dropboxes_view->addColumn(tr("Normalization Level"));
  list_dropboxes_view->setColumnAlignment(3,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Autotrim Level"));
  list_dropboxes_view->setColumnAlignment(4,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("To Cart"));
  list_dropboxes_view->setColumnAlignment(5,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Use CartChunk ID"));
  list_dropboxes_view->setColumnAlignment(6,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Delete Cuts"));
  list_dropboxes_view->setColumnAlignment(7,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Metadata Pattern"));
  list_dropboxes_view->setColumnAlignment(8,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Fix Broken Formats"));
  list_dropboxes_view->setColumnAlignment(9,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("User Defined"));
  list_dropboxes_view->setColumnAlignment(10,Qt::AlignLeft);
  connect(list_dropboxes_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();
}


ListDropboxes::~ListDropboxes()
{
}


QSize ListDropboxes::sizeHint() const
{
  return QSize(640,420);
} 


QSizePolicy ListDropboxes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListDropboxes::addData()
{
  RDDropbox *box=new RDDropbox(-1,list_stationname);
  int id=box->id();
  delete box;
  EditDropbox *edit_dropbox=new EditDropbox(id,false,this);
  if(edit_dropbox->exec()) {
    RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					      RDNotification::AddAction,
					      list_stationname);
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
  else {
    QString sql=QString().sprintf("delete from DROPBOXES where ID=%d",id);
    RDSqlQuery *q=new RDSqlQuery(sql);
    delete q;
    delete edit_dropbox;
    return;
  }
  RDListViewItem *item=new RDListViewItem(list_dropboxes_view);
  item->setId(id);
  RefreshItem(item);
  item->setSelected(true);
  list_dropboxes_view->setCurrentItem(item);
  list_dropboxes_view->ensureItemVisible(item);
}


void ListDropboxes::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_dropboxes_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditDropbox *edit_dropbox=new EditDropbox(item->id(),false,this);
  if(edit_dropbox->exec()) {
    RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					      RDNotification::ModifyAction,
					      list_stationname);
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
  delete edit_dropbox;
  RefreshItem(item);
}


void ListDropboxes::duplicateData()
{
  RDListViewItem *item=(RDListViewItem *)list_dropboxes_view->selectedItem();
  if(item==NULL) {
    return;
  }

  RDDropbox *src_box=new RDDropbox(item->id(),list_stationname);
  int new_box_id=src_box->duplicate();
  delete src_box;

  EditDropbox *edit_dropbox=new EditDropbox(new_box_id,true,this);
  if(edit_dropbox->exec()) {
    RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					      RDNotification::AddAction,
					      list_stationname);
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
  else {
    QString sql=QString().sprintf("delete from DROPBOXES where ID=%d",new_box_id);
    RDSqlQuery *q=new RDSqlQuery(sql);
    delete q;
    delete edit_dropbox;
    return;
  }
  item=new RDListViewItem(list_dropboxes_view);
  item->setId(new_box_id);
  RefreshItem(item);
  item->setSelected(true);
  list_dropboxes_view->setCurrentItem(item);
  list_dropboxes_view->ensureItemVisible(item);
}


void ListDropboxes::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item=(RDListViewItem *)list_dropboxes_view->selectedItem();
  if(item==NULL) {
    return;
  }
  sql=QString().sprintf("delete from DROPBOX_PATHS where DROPBOX_ID=%d",
			item->id());
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString().sprintf("delete from DROPBOXES where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  delete q;

  RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					    RDNotification::DeleteAction,
					    list_stationname);
  rda->ripc()->sendNotification(*notify);
  delete notify;

  delete item;
}


void ListDropboxes::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListDropboxes::closeData()
{
  done(0);
}


void ListDropboxes::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,10,80,50);
  list_edit_button->setGeometry(size().width()-90,70,80,50);
  list_duplicate_button->setGeometry(size().width()-90,130,80,50);
  list_delete_button->setGeometry(size().width()-90,190,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_dropboxes_view->
    setGeometry(10,10,size().width()-120,size().height()-40);
}


void ListDropboxes::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_dropboxes_view->clear();
  sql=QString("select ")+
    "DROPBOXES.ID,"+                   // 00
    "DROPBOXES.GROUP_NAME,"+           // 01
    "DROPBOXES.PATH,"+                 // 02
    "DROPBOXES.NORMALIZATION_LEVEL,"+  // 03
    "DROPBOXES.AUTOTRIM_LEVEL,"+       // 04
    "DROPBOXES.TO_CART,"+              // 05
    "DROPBOXES.USE_CARTCHUNK_ID,"+     // 06
    "DROPBOXES.DELETE_CUTS,"+          // 07
    "DROPBOXES.METADATA_PATTERN,"+     // 08
    "DROPBOXES.FIX_BROKEN_FORMATS,"+   // 09
    "DROPBOXES.SET_USER_DEFINED,"+     // 10
    "GROUPS.COLOR "+                   // 11
    "from DROPBOXES left join GROUPS "+
    "on DROPBOXES.GROUP_NAME=GROUPS.NAME where "+
    "DROPBOXES.STATION_NAME=\""+RDEscapeString(list_stationname)+"\"";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_dropboxes_view);
    WriteItem(item,q);
  }
  delete q;
}


void ListDropboxes::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "DROPBOXES.ID,"+                   // 00
    "DROPBOXES.GROUP_NAME,"+           // 01
    "DROPBOXES.PATH,"+                 // 02
    "DROPBOXES.NORMALIZATION_LEVEL,"+  // 03
    "DROPBOXES.AUTOTRIM_LEVEL,"+       // 04
    "DROPBOXES.TO_CART,"+              // 05
    "DROPBOXES.USE_CARTCHUNK_ID,"+     // 06
    "DROPBOXES.DELETE_CUTS,"+          // 07
    "DROPBOXES.METADATA_PATTERN,"+     // 08
    "DROPBOXES.FIX_BROKEN_FORMATS,"+   // 09
    "DROPBOXES.SET_USER_DEFINED,"+     // 10
    "GROUPS.COLOR "+                   // 11
    "from DROPBOXES left join GROUPS on "+
    "DROPBOXES.GROUP_NAME=GROUPS.NAME where "+
    QString().sprintf("DROPBOXES.ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->next()) {
    WriteItem(item,q);
  }
  delete q;
}


void ListDropboxes::WriteItem(RDListViewItem *item,RDSqlQuery *q)
{
  item->setId(q->value(0).toInt());
  item->setText(0,QString().sprintf("%d",q->value(0).toInt()));
  item->setText(1,q->value(1).toString());
  item->setTextColor(1,q->value(11).toString(),QFont::Bold);
  item->setText(2,q->value(2).toString());
  if(q->value(3).toInt()<0) {
    item->setText(3,QString().sprintf("%d",q->value(3).toInt()/100));
  }
  else {
    item->setText(3,tr("[off]"));
  }
  if(q->value(4).toInt()<0) {
    item->setText(4,QString().sprintf("%d",q->value(4).toInt()/100));
  }
  else {
    item->setText(4,tr("[off]"));
  }
  if(q->value(5).toUInt()>0) {
    item->setText(5,QString().sprintf("%06u",q->value(5).toUInt()));
  }
  else {
    item->setText(5,tr("[auto]"));
  }
  item->setText(6,q->value(6).toString());
  item->setText(7,q->value(7).toString());
  if(q->value(8).toString().isEmpty()) {
    item->setText(8,tr("[none]"));
  }
  else {
    item->setText(8,q->value(8).toString());
  }
  item->setText(9,q->value(9).toString());
  item->setText(10,q->value(10).toString());
}
