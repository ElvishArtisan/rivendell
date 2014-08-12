// list_dropboxes.cpp
//
// List Rivendell Dropboxes
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_dropboxes.cpp,v 1.7.8.1 2013/12/11 20:17:14 cvs Exp $
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
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rddb.h>
#include <list_dropboxes.h>
#include <edit_dropbox.h>


ListDropboxes::ListDropboxes(const QString &stationname,
			     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_stationname=stationname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(QString().sprintf("Rivendell Dropbox Configurations on %s",
			       (const char *)stationname));

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
  //  Delete Button
  //
  list_delete_button=new QPushButton(this,"list_delete_button");
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

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
  list_dropboxes_view=new RDListView(this,"list_dropboxes_view");
  list_dropboxes_view->setFont(list_font);
  list_dropboxes_view->setAllColumnsShowFocus(true);
  list_dropboxes_view->addColumn(tr("Group"));
  list_dropboxes_view->setColumnAlignment(0,Qt::AlignVCenter|Qt::AlignLeft);
  list_dropboxes_view->addColumn(tr("Path"));
  list_dropboxes_view->setColumnAlignment(1,Qt::AlignVCenter|Qt::AlignLeft);
  list_dropboxes_view->addColumn(tr("Normalization Level"));
  list_dropboxes_view->setColumnAlignment(2,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Autotrim Level"));
  list_dropboxes_view->setColumnAlignment(3,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("To Cart"));
  list_dropboxes_view->setColumnAlignment(4,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Use CartChunk ID"));
  list_dropboxes_view->setColumnAlignment(5,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Delete Cuts"));
  list_dropboxes_view->setColumnAlignment(6,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Metadata Pattern"));
  list_dropboxes_view->setColumnAlignment(7,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("Fix Broken Formats"));
  list_dropboxes_view->setColumnAlignment(8,Qt::AlignCenter);
  list_dropboxes_view->addColumn(tr("User Defined"));
  list_dropboxes_view->setColumnAlignment(9,Qt::AlignVCenter|Qt::AlignLeft);
  connect(list_dropboxes_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

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
  EditDropbox *edit_dropbox=new EditDropbox(id,this,"edit_dropbox");
  if(edit_dropbox->exec()<0) {
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
  EditDropbox *edit_dropbox=new EditDropbox(item->id(),this,"edit_dropbox");
  edit_dropbox->exec();
  delete edit_dropbox;
  RefreshItem(item);
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
  delete item;
}


void ListDropboxes::doubleClickedData(QListViewItem *item,const QPoint &pt,
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
  list_delete_button->setGeometry(size().width()-90,130,80,50);
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
  sql=QString().sprintf("select DROPBOXES.ID,DROPBOXES.GROUP_NAME,\
                         DROPBOXES.PATH,DROPBOXES.NORMALIZATION_LEVEL,\
                         DROPBOXES.AUTOTRIM_LEVEL,\
                         DROPBOXES.TO_CART,DROPBOXES.USE_CARTCHUNK_ID,\
                         DROPBOXES.DELETE_CUTS,DROPBOXES.METADATA_PATTERN,\
                         DROPBOXES.FIX_BROKEN_FORMATS,\
                         DROPBOXES.SET_USER_DEFINED,GROUPS.COLOR \
                         from DROPBOXES left join GROUPS on \
                         DROPBOXES.GROUP_NAME=GROUPS.NAME \
                         where DROPBOXES.STATION_NAME=\"%s\"",
			(const char *)list_stationname);
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

  sql=QString().sprintf("select DROPBOXES.ID,DROPBOXES.GROUP_NAME,\
                         DROPBOXES.PATH,DROPBOXES.NORMALIZATION_LEVEL,\
                         DROPBOXES.AUTOTRIM_LEVEL,\
                         DROPBOXES.TO_CART,DROPBOXES.USE_CARTCHUNK_ID,\
                         DROPBOXES.DELETE_CUTS,DROPBOXES.METADATA_PATTERN,\
                         DROPBOXES.FIX_BROKEN_FORMATS,\
                         DROPBOXES.SET_USER_DEFINED,GROUPS.COLOR \
                         from DROPBOXES left join GROUPS on \
                         DROPBOXES.GROUP_NAME=GROUPS.NAME \
                         where DROPBOXES.ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->next()) {
    WriteItem(item,q);
  }
  delete q;
}


void ListDropboxes::WriteItem(RDListViewItem *item,RDSqlQuery *q)
{
  item->setId(q->value(0).toInt());
  item->setText(0,q->value(1).toString());
  item->setTextColor(0,q->value(11).toString(),QFont::Bold);
  item->setText(1,q->value(2).toString());
  if(q->value(3).toInt()<0) {
    item->setText(2,QString().sprintf("%d",q->value(3).toInt()/100));
  }
  else {
    item->setText(2,tr("[off]"));
  }
  if(q->value(4).toInt()<0) {
    item->setText(3,QString().sprintf("%d",q->value(4).toInt()/100));
  }
  else {
    item->setText(3,tr("[off]"));
  }
  if(q->value(5).toUInt()>0) {
    item->setText(4,QString().sprintf("%06u",q->value(5).toUInt()));
  }
  else {
    item->setText(4,tr("[auto]"));
  }
  item->setText(5,q->value(6).toString());
  item->setText(6,q->value(7).toString());
  if(q->value(8).toString().isEmpty()) {
    item->setText(7,tr("[none]"));
  }
  else {
    item->setText(7,q->value(8).toString());
  }
  item->setText(8,q->value(9).toString());
  item->setText(9,q->value(10).toString());
}
