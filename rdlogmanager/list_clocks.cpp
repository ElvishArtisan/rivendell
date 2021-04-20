// list_clocks.cpp
//
// List Rivendell Log Clocks
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

#include <rdescape_string.h>

#include "add_clock.h"
#include "edit_clock.h"
#include "globals.h"
#include "list_clocks.h"
#include "rename_item.h"

ListClocks::ListClocks(QString *clockname,QWidget *parent)
  : RDDialog(parent)
{
  QStringList services_list;
  edit_clockname=clockname;

  setWindowTitle("RDLogManager - "+tr("Log Clocks"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Event Filter
  //
  edit_filter_box=new QComboBox(this);
  edit_filter_label=new QLabel(tr("Filter:"),this);
  edit_filter_label->setFont(labelFont());
  edit_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_filter_box,SIGNAL(activated(int)),
	  this,SLOT(filterActivatedData(int)));

  //
  // Clocks List
  //
  edit_clocks_view=new RDTableView(this);
  edit_clocks_model=new ClockListModel(this);
  edit_clocks_model->setFont(font());
  edit_clocks_model->setPalette(palette());
  edit_clocks_view->setModel(edit_clocks_model);
  connect(edit_clocks_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(edit_clocks_model,SIGNAL(modelReset()),
	  edit_clocks_view,SLOT(resizeColumnsToContents()));
  edit_clocks_view->resizeColumnsToContents();

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
  edit_close_button->setText(tr("Close"));
  connect(edit_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  //  Clear Button
  //
  edit_clear_button=new QPushButton(this);
  edit_clear_button->setFont(buttonFont());
  edit_clear_button->setText(tr("Clear"));
  connect(edit_clear_button,SIGNAL(clicked()),this,SLOT(clearData()));

  //
  //  OK Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  if(edit_clockname==NULL) {
    edit_close_button->setDefault(true);
    edit_clear_button->hide();
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
    if(*clock_filter==*it) {
      edit_filter_box->setCurrentIndex(edit_filter_box->count()-1);
    }
  }
  if(edit_clockname!=NULL) {
    QModelIndex row=edit_clocks_model->clockIndex(*edit_clockname);
    if(row.isValid()) {
      edit_clocks_view->selectRow(row.row());
    }
  }
}


QSize ListClocks::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy ListClocks::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListClocks::addData()
{
  QString clockname;  
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  std::vector<QString> new_clocks;

  AddClock *add_dialog=new AddClock(&clockname,this);
  if(add_dialog->exec()<0) {
    delete add_dialog;
    return;
  }
  delete add_dialog;
  sql=QString("select `NAME` from `CLOCKS` where ")+
    "NAME='"+RDEscapeString(clockname)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::
      information(this,tr("Clock Exists"),
		  tr("An clock with that name already exists!"));
    delete q;
    return;
  }
  delete q;
  sql=QString("insert into `CLOCKS` set ")+
    "`NAME`='"+RDEscapeString(clockname)+"',"+
    "`ARTISTSEP`=15";
  RDSqlQuery::apply(sql);

  //
  // Create default schedcode rules
  //
  sql=QString("select ")+
    "`CODE` "+  // 00
    "from `SCHED_CODES` "+
    "order by `CODE`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `RULE_LINES` set ")+
      "`CLOCK_NAME`='"+RDEscapeString(clockname)+"',"+
      "`CODE`='"+RDEscapeString(q->value(0).toString())+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;

  EditClock *clock_dialog=new EditClock(clockname,true,&new_clocks,this);
  if(clock_dialog->exec()<0) {
    // Back everything out!
    sql=QString("delete from `RULE_LINES` where ")+
      "`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
    RDSqlQuery::apply(sql);
    sql=QString("delete from `CLOCK_LINES` where ")+
      "`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
    RDSqlQuery::apply(sql);
    sql=QString("delete from `CLOCKS` where ")+
      "`NAME`='"+RDEscapeString(clockname)+"'";
    RDSqlQuery::apply(sql);
  }
  else {
    if(edit_filter_box->currentIndex()==0) {
      //
      // Create default clock permissions
      //
      sql=QString("select ")+
	"`ID` "+  // 00
	"from `CLOCK_PERMS` where "+
	"`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	sql="select `NAME` from `SERVICES`";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into `CLOCK_PERMS` set ")+
	    "`CLOCK_NAME`='"+RDEscapeString(clockname)+"',"+
	    "`SERVICE_NAME`='"+RDEscapeString(q1->value(0).toString())+"'";
	  RDSqlQuery::apply(sql);
	}
	delete q1;
      }
      delete q;
    }
    else {
      sql=QString("insert into `CLOCK_PERMS` set ")+
	"`CLOCK_NAME`='"+RDEscapeString(clockname)+"',"+
	"`SERVICE_NAME`='"+RDEscapeString(edit_filter_box->currentText())+"'";
      RDSqlQuery::apply(sql);
    }
    QModelIndex row=edit_clocks_model->addClock(clockname);
    if(row.isValid()) {
      edit_clocks_view->selectRow(row.row());
    }
  }
  delete clock_dialog;
}


void ListClocks::editData()
{
  std::vector<QString> new_clocks;
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditClock *clock_dialog=
    new EditClock(edit_clocks_model->clockName(rows.first()),false,
		  &new_clocks,this);
  if(clock_dialog->exec()<0) {
    delete clock_dialog;
    return;
  }
  delete clock_dialog;
  edit_clocks_model->refresh(rows.first());
  for(unsigned i=0;i<new_clocks.size();i++) {
    edit_clocks_model->addClock(new_clocks.at(i));
  }
}


void ListClocks::deleteData()
{
  int n;
  QString svc_list;
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(QMessageBox::question(this,"RDLogManager - "+tr("Delete Clock"),
			   tr("Are you sure you want to delete")+" \""+
			   edit_clocks_model->clockName(rows.first())+"\"?",
			  QMessageBox::Yes,QMessageBox::No)
     !=QMessageBox::Yes) {
    return;
  }
  if((n=ActiveClocks(edit_clocks_model->clockName(rows.first()),&svc_list))>0) {
    if(QMessageBox::warning(this,"RDLogManager - "+tr("Clock In Use"),
			    "\""+edit_clocks_model->clockName(rows.first())+"\" "+
			    tr("is in use in the following grid(s)")+":\n\n"+
			    svc_list+"\n"+
			    tr("Do you still want to delete it?"),
			    QMessageBox::Yes,
			    QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }
  }
  DeleteClock(edit_clocks_model->clockName(rows.first()));
  edit_clocks_model->removeClock(rows.first());
}


void ListClocks::renameData()
{
  QString sql;
  RDSqlQuery *q;
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString old_name=edit_clocks_model->clockName(rows.first());
  QString new_name=old_name;
  RenameItem *rename_dialog=new RenameItem(&new_name,"CLOCKS",this);
  if(rename_dialog->exec()<-1) {
    delete rename_dialog;
    return;
  }
  delete rename_dialog;
  
  //
  // Rename Grid References
  //
  sql="select `NAME` from `SERVICES`";
  // FIXME: not sure if the usersec service filter should be applied here, or
  // if all services should be brought over
  q=new RDSqlQuery(sql);
  while(q->next()) {
    for(int i=0;i<168;i++) {
      sql=QString("update `SERVICE_CLOCKS` set ")+
	"`CLOCK_NAME`='"+RDEscapeString(new_name)+"' where "+
	"`CLOCK_NAME`='"+RDEscapeString(edit_clocks_model->clockName(rows.first()))+"'";
      RDSqlQuery::apply(sql);
    }
  }
  delete q;

  //
  // Rename Meta Table
  //
  sql=QString("update `CLOCK_LINES` set ")+
    "`CLOCK_NAME`='"+RDEscapeString(new_name)+"' where "+
    "`CLOCK_NAME`='"+RDEscapeString(edit_clocks_model->clockName(rows.first()))+"'";
  RDSqlQuery::apply(sql);

  sql=QString("update `RULE_LINES` set ")+
    "`CLOCK_NAME`='"+RDEscapeString(new_name)+"' where "+
    "`CLOCK_NAME`='"+RDEscapeString(edit_clocks_model->clockName(rows.first()))+"'";
  RDSqlQuery::apply(sql);

  //
  // Rename Service Permissions
  //
  sql=QString("update `CLOCK_PERMS` set ")+
    "`CLOCK_NAME`='"+RDEscapeString(new_name)+"' where "+
    "`CLOCK_NAME`='"+RDEscapeString(edit_clocks_model->clockName(rows.first()))+"'";
  RDSqlQuery::apply(sql);

  //
  // Rename Primary Key
  //
  sql=QString("update `CLOCKS` set ")+
    "`NAME`='"+RDEscapeString(new_name)+"' where "+
    "`NAME`='"+RDEscapeString(edit_clocks_model->clockName(rows.first()))+"'";
  RDSqlQuery::apply(sql);

  edit_clocks_model->removeClock(old_name);
  QModelIndex row=edit_clocks_model->addClock(new_name);
  if(row.isValid()) {
    edit_clocks_view->selectRow(row.row());
  }
}


void ListClocks::filterActivatedData(int id)
{
  QString filter;

  if(id==1) {
    filter=GetNoneFilter();
  }
  else {
    if(id>1) {
      filter=GetClockFilter(edit_filter_box->currentText());
    }
  }
  edit_clocks_model->setFilterSql(filter);
}


void ListClocks::doubleClickedData(const QModelIndex &index)
{
  if(edit_clockname==NULL) {
    editData();
  }
  else {
    okData();
  }
}


void ListClocks::closeData()
{
  *clock_filter=edit_filter_box->currentText();
  done(0);
}


void ListClocks::clearData()
{
  edit_clocks_view->clearSelection();
}


void ListClocks::okData()
{
  *clock_filter=edit_filter_box->currentText();
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();
  if(rows.size()==1) {
    *edit_clockname=edit_clocks_model->clockName(rows.first());
  }
  else {
    *edit_clockname="";
  }
  done(0);
}


void ListClocks::cancelData()
{
  done(-1);
}


void ListClocks::resizeEvent(QResizeEvent *e)
{
  edit_filter_box->setGeometry(65,10,size().width()-75,20);
  edit_filter_label->setGeometry(10,10,50,20);
  edit_clocks_view->setGeometry(10,45,
				size().width()-20,size().height()-115);
  edit_add_button->setGeometry(10,size().height()-60,80,50);
  edit_edit_button->setGeometry(100,size().height()-60,80,50);
  edit_delete_button->setGeometry(190,size().height()-60,80,50);
  edit_rename_button->setGeometry(310,size().height()-60,80,50);
  edit_close_button->
    setGeometry(size().width()-90,size().height()-60,80,50);
  edit_clear_button->setGeometry(10,size().height()-60,80,50);
  edit_ok_button->
    setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->
    setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListClocks::closeEvent(QCloseEvent *e)
{
  cancelData();
}


int ListClocks::ActiveClocks(QString clockname,QString *svc_list)
{
  int n=0;
  QString sql;
  RDSqlQuery *q;
  QString svcname;
  QStringList svcs;

  sql=QString("select `SERVICE_NAME` from `SERVICE_CLOCKS` where ")+
    "`CLOCK_NAME`='"+RDEscapeString(clockname)+"' order by `CLOCK_NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if((svcs.size()==0)||(svcs.back()!=q->value(0).toString())) {
      svcs.push_back(q->value(0).toString());
    }
  }
  delete q;
  for(int i=0;i<svcs.size();i++) {
    n++;
    *svc_list+="    "+svcs[i]+"\n";
  }

  return n;
}


void ListClocks::DeleteClock(QString clockname)
{
  QString sql;
  RDSqlQuery *q;
  QString base_name=clockname;
  base_name.replace(" ","_");

  //
  // Clear Active Clocks
  //
  sql=QString("update `SERVICE_CLOCKS` set ")+
    "`CLOCK_NAME`=null where "+
    "`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Service Associations
  //
  sql=QString("delete from `CLOCK_PERMS` where ")+
    "`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Clock Definition
  //
  sql=QString("delete from `CLOCKS` where ")+
    "`NAME`='"+RDEscapeString(clockname)+"'";
  RDSqlQuery::apply(sql);
  sql=QString("delete from `CLOCK_LINES` where ")+
    "`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
  RDSqlQuery::apply(sql);
  sql=QString("delete from `RULE_LINES` where ")+
    "`CLOCK_NAME`='"+RDEscapeString(clockname)+"'";
  RDSqlQuery::apply(sql);
}


QString ListClocks::GetClockFilter(QString svc_name)
{
  QString filter="where ";
  QString sql=QString("select `CLOCK_NAME` from `CLOCK_PERMS` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(svc_name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->size()>0) {
    while(q->next()) {
      filter+=QString("(`NAME`='")+
	RDEscapeString(q->value(0).toString())+"')||";
    }
    filter=filter.left(filter.length()-2);
  }
  else {
    filter="(`SERVICE_NAME`='')";
  }
  delete q;

  return filter;
}


QString ListClocks::GetNoneFilter()
{
  QString sql;
  RDSqlQuery *q;
  QString filter;

  sql="select `CLOCK_NAME` from `CLOCK_PERMS`";
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    filter="where ";
  }
  while(q->next()) {
    filter+=QString("(`NAME`!='")+RDEscapeString(q->value(0).toString())+"')&&";
  }
  if(q->size()>0) {
    filter=filter.left(filter.length()-2);
  }
  delete q;

  return filter;
}

