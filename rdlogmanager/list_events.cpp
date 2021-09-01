// list_events.cpp
//
// List a Rivendell Log Event
//
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

#include <rdconf.h>
#include <rdescape_string.h>

#include "add_event.h"
#include "edit_event.h"
#include "globals.h"
#include "list_events.h"
#include "rename_item.h"

ListEvents::ListEvents(QString *eventname,QWidget *parent)
  : RDDialog(parent)
{
  QStringList services_list;
  edit_eventname=eventname;

  setWindowTitle("RDLogManager - "+tr("Log Events"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Event Filter
  //
  edit_filter_box=new QComboBox(this);
  edit_filter_label=new QLabel(tr("Filter:"),this);
  edit_filter_label->setGeometry(10,10,50,20);
  edit_filter_label->setFont(labelFont());
  edit_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_filter_box,SIGNAL(activated(int)),
	  this,SLOT(filterActivatedData(int)));

  //
  // Events List
  //
  edit_events_view=new RDTableView(this);
  edit_events_model=new EventListModel(this);
  edit_events_model->setFont(font());
  edit_events_model->setPalette(palette());
  edit_events_view->setModel(edit_events_model);
  connect(edit_events_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(edit_events_model,SIGNAL(modelReset()),
	  edit_events_view,SLOT(resizeColumnsToContents()));
  edit_events_view->resizeColumnsToContents();

  //
  //  Add Button
  //
  edit_add_button=new QPushButton(this);
  edit_add_button->setFont(buttonFont());
  edit_add_button->setText(tr("Add"));
  connect(edit_add_button,SIGNAL(clicked()),this,SLOT(addData()));
    
  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setFont(buttonFont());
  edit_edit_button->setText(tr("Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editData()));
    
  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->setFont(buttonFont());
  edit_delete_button->setText(tr("Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));
    
  //
  //  Rename Button
  //
  edit_rename_button=new QPushButton(this);
  edit_rename_button->setFont(buttonFont());
  edit_rename_button->setText(tr("Rename"));
  connect(edit_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));
    
  //
  //  Close Button
  //
  edit_close_button=new QPushButton(this);
  edit_close_button->setFont(buttonFont());
  edit_close_button->setText(tr("OK"));
  connect(edit_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("Ok"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  if(edit_eventname==NULL) {
    edit_close_button->setDefault(true);
    edit_ok_button->hide();
    edit_cancel_button->hide();
  }
  else {
    edit_ok_button->setDefault(true);
    edit_add_button->hide();
    edit_edit_button->hide();
    edit_delete_button->hide();
    edit_rename_button->hide();
    edit_close_button->hide();
  }

  //
  // Populate Data
  //
  edit_filter_box->insertItem(0,tr("ALL"));
  edit_filter_box->insertItem(1,tr("NONE"));

  QString sql="select `NAME` from `SERVICES`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append( q->value(0).toString() );
  }
  delete q;
  services_list.sort();
  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end();
        ++it ) {
    edit_filter_box->insertItem(edit_filter_box->count(),*it);
    if(*event_filter==*it) {
      edit_filter_box->setCurrentIndex(edit_filter_box->count()-1);
    }
  }
}


QSize ListEvents::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy ListEvents::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListEvents::addData()
{
  QString logname;
  RDEvent *event;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  std::vector<QString> new_events;

  AddEvent *add_dialog=new AddEvent(&logname,this);
  if(add_dialog->exec()<0) {
    delete add_dialog;
    return;
  }
  delete add_dialog;
  QString sql=QString("select ")+
    "`NAME` "+
    "from `EVENTS` where "+
    "`NAME`='"+RDEscapeString(logname)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::
      information(this,tr("Event Exists"),
		  tr("An event with that name already exists!"));
    delete q;
    return;
  }
  delete q;
  event=new RDEvent(logname,true);
  delete event;
  EditEvent *event_dialog=new EditEvent(logname,true,&new_events,this);
  if(event_dialog->exec()<-1) {
    sql=QString("delete from `EVENTS` where ")+
      "`NAME`='"+RDEscapeString(logname)+"'";
    RDSqlQuery::apply(sql);
    return;
  }
  else {
    if(edit_filter_box->currentIndex()==0) {
      sql=QString(" select ")+
	"`ID` "+  // 00
	"from `EVENT_PERMS` where "+
	"`EVENT_NAME`='"+RDEscapeString(logname)+"'";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	sql="select `NAME` from `SERVICES`";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into `EVENT_PERMS` set ")+
	    "`EVENT_NAME`='"+RDEscapeString(logname)+"',"+
	    "`SERVICE_NAME`='"+RDEscapeString(q1->value(0).toString())+"'";
	  RDSqlQuery::apply(sql);
	}
	delete q1;
      }
      delete q;
    }
    else {
      sql=QString("insert into `EVENT_PERMS` set ")+
	"`EVENT_NAME`='"+RDEscapeString(logname)+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(edit_filter_box->currentText())+"'";
      RDSqlQuery::apply(sql);
    }
    QModelIndex row=edit_events_model->addEvent(logname);
    if(row.isValid()) {
      edit_events_view->selectRow(row.row());
    }      
  }
  delete event_dialog;
  for(unsigned i=0;i<new_events.size();i++) {
    QModelIndex row=edit_events_model->addEvent(new_events.at(i));
    if(row.isValid()) {
      edit_events_view->selectRow(row.row());
    }
  }
}


void ListEvents::editData()
{
  std::vector<QString> new_events;
  QModelIndexList rows=edit_events_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditEvent *event_dialog=
    new EditEvent(edit_events_model->eventName(rows.first()),false,&new_events,
		  this);
  if(event_dialog->exec()>=-1) {
    edit_events_model->refresh(rows.first());
  }
  for(unsigned i=0;i<new_events.size();i++) {
    edit_events_model->addEvent(new_events.at(i));
  }
  delete event_dialog;
}


void ListEvents::deleteData()
{
  int n;
  QString clock_list;
  QModelIndexList rows=edit_events_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(QMessageBox::question(this,"RDLogManager - "+tr("Delete Event"),
			   tr("Are you sure you want to delete")+" \""+
			   edit_events_model->eventName(rows.first())+"\"?",
			  QMessageBox::Yes,QMessageBox::No)
     !=QMessageBox::Yes) {
    return;
  }
  if((n=ActiveEvents(edit_events_model->eventName(rows.first()),&clock_list))>0) {
    if(QMessageBox::warning(this,"RDLogManager - "+tr("Event In Use"),
			    "\""+edit_events_model->eventName(rows.first())+"\" "+
			    tr("is in use in the following clocks")+":\n\n"+
			    clock_list+"\n"+
			    tr("Do you still want to delete it?"),
			    QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
  }
  DeleteEvent(edit_events_model->eventName(rows.first()));
  edit_events_model->removeEvent(rows.first());
}


void ListEvents::renameData()
{
  QString sql;
  QModelIndexList rows=edit_events_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString old_name=edit_events_model->eventName(rows.first());
  QString new_name=old_name;
  RenameItem *rename_dialog=new RenameItem(&new_name,"EVENTS",this);
  if(rename_dialog->exec()<-1) {
    delete rename_dialog;
    return;
  }
  delete rename_dialog;
  if(old_name==new_name) {
    return;
  }
  
  //
  // Rename Clock References
  //
  sql=QString("update `CLOCK_LINES` set ")+
    "`EVENT_NAME`='"+RDEscapeString(new_name)+"' where "+
    "`EVENT_NAME`='"+RDEscapeString(old_name)+"'";
  RDSqlQuery::apply(sql);

  //
  // Rename Event Line References
  //
  sql=QString("update `EVENT_LINES` set ")+
    "`EVENT_NAME`='"+RDEscapeString(new_name)+"' where "+
    "`EVENT_NAME`='"+RDEscapeString(old_name)+"'";
  RDSqlQuery::apply(sql);

  //
  // Rename Service Permissions
  //
  sql=QString("update `EVENT_PERMS` set ")+
    "`EVENT_NAME`='"+RDEscapeString(new_name)+"' "+
    "where `EVENT_NAME`='"+RDEscapeString(old_name)+"'";
  RDSqlQuery::apply(sql);

  //
  // Rename Primary Key
  //
  sql=QString("update `EVENTS` set ")+
    "`NAME`='"+RDEscapeString(new_name)+"' where "+
    "`NAME`='"+RDEscapeString(old_name)+"'";
  RDSqlQuery::apply(sql);

  edit_events_model->removeEvent(old_name);
  QModelIndex row=edit_events_model->addEvent(new_name);
  if(row.isValid()) {
    edit_events_view->selectRow(row.row());
  }
}


void ListEvents::filterActivatedData(int id)
{
  QString filter;

  if(id==1) {  // NONE Filter
    filter=GetNoneFilter();
  }
  else {
    if(id>1) {
      filter=GetEventFilter(edit_filter_box->currentText());
    }
  }
  edit_events_model->setFilterSql(filter);
}


void ListEvents::doubleClickedData(const QModelIndex &index)
{
  if(edit_eventname==NULL) {
    editData();
  }
  else {
    okData();
  }
}


void ListEvents::closeData()
{
  done(0);
}


void ListEvents::okData()
{
  *event_filter=edit_filter_box->currentText();
  QModelIndexList rows=edit_events_view->selectionModel()->selectedRows();
  if(rows.size()!=1) {
    done(-1);
  }
  *edit_eventname=edit_events_model->eventName(rows.first());

  done(0);
}


void ListEvents::cancelData()
{
  done(-1);
}


void ListEvents::resizeEvent(QResizeEvent *e)
{
  edit_filter_box->setGeometry(65,10,size().width()-75,20);
  edit_events_view->setGeometry(10,45,size().width()-20,size().height()-125);
  edit_add_button->setGeometry(10,size().height()-60,80,50);
  edit_edit_button->setGeometry(100,size().height()-60,80,50);
  edit_delete_button->setGeometry(190,size().height()-60,80,50);
  edit_rename_button->setGeometry(310,size().height()-60,80,50);
  edit_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListEvents::closeEvent(QCloseEvent *e)
{
  cancelData();
}


int ListEvents::ActiveEvents(QString event_name,QString *clock_list)
{
  int n=0;
  QString sql;
  RDSqlQuery *q,*q1;

  sql="select `NAME` from `CLOCKS`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select ")+
      "`EVENT_NAME` "+
      "from `CLOCK_LINES` where "+
      "`CLOCK_NAME`='"+RDEscapeString(q->value(0).toString())+"' && "+
      "`EVENT_NAME`='"+RDEscapeString(event_name)+"'";
    q1=new RDSqlQuery(sql);
    if(q1->first()) {
      *clock_list+=
	QString::asprintf("    %s\n",
			  q->value(0).toString().toUtf8().constData());
      n++;
    }
    delete q1;
  }
  delete q;
  return n;
}


void ListEvents::DeleteEvent(QString event_name)
{
  QString sql;
  RDSqlQuery *q;
  QString base_name=event_name;
  base_name.replace(" ","_");

  //
  // Delete Active Clock Entries
  //
  sql="select `NAME` from `CLOCKS`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("delete from `CLOCK_LINES` where ")+
      "`CLOCK_NAME`='"+RDEscapeString(q->value(0).toString())+"' && "+
      "`EVENT_NAME`='"+RDEscapeString(event_name)+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;

  //
  // Delete Service Associations
  //
  sql=QString("delete from `EVENT_PERMS` where ")+
    "`EVENT_NAME`='"+RDEscapeString(event_name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Event Definition
  //
  sql=QString("delete from `EVENTS` where ")+
    "`NAME`='"+RDEscapeString(event_name)+"'";
  RDSqlQuery::apply(sql);

  sql=QString("delete from `EVENT_LINES` where ")+
    "`EVENT_NAME`='"+RDEscapeString(event_name)+"'";
  RDSqlQuery::apply(sql);
}


QString ListEvents::GetEventFilter(QString svc_name)
{
  QString filter="where ";
  QString sql=QString("select `EVENT_NAME` from `EVENT_PERMS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(svc_name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->size()>0) {
    while(q->next()) {
      filter+=QString::asprintf("(`NAME`='%s')||",
				q->value(0).toString().toUtf8().constData());
    }
    filter=filter.left(filter.length()-2);
  }
  else {
    filter="(`SERVICE_NAME`='')";
  }
  delete q;

  return filter;
}


QString ListEvents::GetNoneFilter()
{
  QString sql;
  RDSqlQuery *q;
  QString filter;

  sql="select `EVENT_NAME` from `EVENT_PERMS`";
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    filter="where ";
  }
  while(q->next()) {
    filter+=QString::asprintf("(`NAME`!='%s')&&",
			      RDEscapeString(q->value(0).toString()).toUtf8().constData());
  }
  if(q->size()>0) {
    filter=filter.left(filter.length()-2);
  }
  delete q;

  return filter;
}
