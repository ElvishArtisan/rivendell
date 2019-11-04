// list_pypads.cpp
//
// List PyPAD Instances
//
//   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qfile.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstringlist.h>

#include <rd.h>
#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdpasswd.h>
#include <rdpaths.h>

#include "edit_pypad.h"
#include "list_pypads.h"
#include "view_pypad_errors.h"

//
// Icons
//
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"

ListPypads::ListPypads(RDStation *station,QWidget *parent)
  : RDDialog(parent)
{
  list_station=station;

  setModal(true);
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("PyPAD Instances on")+" "+
		 rda->station()->name());

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Icons
  //
  list_greenball_pixmap=new QPixmap(greenball_xpm);
  list_redball_pixmap=new QPixmap(redball_xpm);

  //
  // Instances List Box
  //
  list_list_view=new RDListView(this);
  list_list_view->setSelectionMode(Q3ListView::Single);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  list_list_view->addColumn(" ");
  list_list_view->setColumnAlignment(0,Qt::AlignCenter);
  list_list_view->addColumn(tr("ID"));
  list_list_view->setColumnAlignment(1,Qt::AlignRight);
  list_list_view->addColumn(tr("Description"));
  list_list_view->setColumnAlignment(2,Qt::AlignLeft);
  list_list_view->addColumn(tr("Script Path"));
  list_list_view->setColumnAlignment(3,Qt::AlignLeft);
  list_list_view->addColumn(tr("Exit Code"));
  list_list_view->setColumnAlignment(4,Qt::AlignRight);
  connect(list_list_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

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
  //  Error Button
  //
  list_error_button=new QPushButton(this);
  list_error_button->setFont(buttonFont());
  list_error_button->setText(tr("&Error\nLog"));
  connect(list_error_button,SIGNAL(clicked()),this,SLOT(errorData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Load Values
  //
  RefreshList();

  //
  // Update Timer
  //
  list_update_timer=new QTimer(this);
  list_update_timer->setSingleShot(true);
  connect(list_update_timer,SIGNAL(timeout()),this,SLOT(updateData()));
  list_update_timer->start(3000);
}


QSize ListPypads::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy ListPypads::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListPypads::addData()
{
  //
  // Get Script Name
  //
  QString script=
    QFileDialog::getOpenFileName(this,tr("Select PyPAD Script"),
				 RD_PYPAD_SCRIPT_DIR,
				 "Python Scripts (*.py)");
  if(script.isNull()) {
    return;
  }

  //
  // Get Exemplar
  //
  QString exemplar="";
  QStringList f0=script.split(".");
  f0.last()="exemplar";
  QFile *file=new QFile(f0.join("."));
  if(file->open(QIODevice::ReadOnly)) {
    exemplar=file->readAll();
    file->close();
  }
  delete file;

  QString sql=QString("insert into PYPAD_INSTANCES set ")+
    "STATION_NAME=\""+RDEscapeString(list_station->name())+"\","+
    "SCRIPT_PATH=\""+RDEscapeString(script)+"\","+
    "DESCRIPTION=\""+
    RDEscapeString("new "+script.split("/").last()+" instance")+"\","+
    "CONFIG=\""+RDEscapeString(exemplar)+"\"";
  int id=RDSqlQuery::run(sql).toInt();
  EditPypad *d=new EditPypad(id,this);
  if(d->exec()) {
    RDListViewItem *item=new RDListViewItem(list_list_view);
    item->setId(id);
    RefreshItem(item);
    list_list_view->clearSelection();
    list_list_view->ensureItemVisible(item);
    list_list_view->setCurrentItem(item);
    item->setSelected(true);
    RDNotification notify=RDNotification(RDNotification::PypadType,
					 RDNotification::AddAction,id);
    rda->ripc()->sendNotification(notify);
  }
  else {
    sql=QString("delete from PYPAD_INSTANCES where ")+
      QString().sprintf("ID=%u",id);
    RDSqlQuery::apply(sql);
  }
  delete d;
}


void ListPypads::editData()
{
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  EditPypad *d=new EditPypad(item->id(),this);
  if(d->exec()) {
    RefreshItem(item);
    RDNotification notify=RDNotification(RDNotification::PypadType,
					 RDNotification::ModifyAction,
					 item->id());
    rda->ripc()->sendNotification(notify);
  }
  delete d;
}


void ListPypads::deleteData()
{
  QString sql;
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  if(QMessageBox::question(this,tr("Delete Instance"),
			   tr("Are your sure you want to delete this instance?"),
			   QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::No) {
    return;
  }
  sql=QString("delete from PYPAD_INSTANCES where ")+
    QString().sprintf("ID=%d",item->id());
  RDSqlQuery::apply(sql);
  RDNotification notify=RDNotification(RDNotification::PypadType,
				       RDNotification::DeleteAction,item->id());
  rda->ripc()->sendNotification(notify);
  delete item;
}


void ListPypads::errorData()
{
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  ViewPypadErrors *d=new ViewPypadErrors(item->id(),this);
  d->exec();
  delete d;
}


void ListPypads::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				      int col)
{
  editData();
}


void ListPypads::closeData()
{
  done(0);
}


void ListPypads::updateData()
{
  QString sql;
  RDSqlQuery *q;

  RDListViewItem *item=(RDListViewItem *)list_list_view->firstChild();
  while(item!=NULL) {
    sql=QString("select ")+
      "IS_RUNNING,"+  // 00
      "EXIT_CODE "+   // 01
      "from PYPAD_INSTANCES where "+
      QString().sprintf("ID=%d",item->id());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      if(q->value(0).toString()=="Y") {
	item->setPixmap(0,*list_greenball_pixmap);
      }
      else {
	item->setPixmap(0,*list_redball_pixmap);
      }
      item->setText(4,QString().sprintf("%d",q->value(1).toInt()));
    }
    delete q;
    item=(RDListViewItem *)item->nextSibling();
  }
  list_update_timer->start(3000);
}


void ListPypads::resizeEvent(QResizeEvent *e)
{
  list_list_view->setGeometry(10,10,size().width()-20,size().height()-80);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_error_button->setGeometry(300,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListPypads::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_list_view->clear();
  sql=QString("select ")+
    "ID,"+           // 00
    "IS_RUNNING,"+   // 01
    "DESCRIPTION,"+  // 02
    "SCRIPT_PATH,"+  // 03
    "EXIT_CODE "+    // 04
    "from PYPAD_INSTANCES where "+
    "STATION_NAME=\""+RDEscapeString(list_station->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(list_list_view);
    item->setId(q->value(0).toInt());
    if(q->value(1).toString()=="Y") {
      item->setPixmap(0,*list_greenball_pixmap);
    }
    else {
      item->setPixmap(0,*list_redball_pixmap);
    }
    item->setText(1,QString().sprintf("%u",q->value(0).toUInt()));
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
    item->setText(4,QString().sprintf("%d",q->value(4).toInt()));
  }
  delete q;
}


void ListPypads::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "IS_RUNNING,"+   // 00
    "DESCRIPTION,"+  // 01
    "SCRIPT_PATH,"+  // 02
    "EXIT_CODE "+    //03
    "from PYPAD_INSTANCES where "+
    QString().sprintf("ID=%u",item->id());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toString()=="Y") {
      item->setPixmap(0,*list_greenball_pixmap);
    }
    else {
      item->setPixmap(0,*list_redball_pixmap);
    }
    item->setText(1,QString().sprintf("%u",item->id()));
    item->setText(2,q->value(1).toString());
    item->setText(3,q->value(2).toString());
    item->setText(4,QString().sprintf("%d",q->value(3).toInt()));
  }
  delete q;
}
