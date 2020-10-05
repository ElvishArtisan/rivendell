// list_clocks.cpp
//
// List Rivendell Log Clocks
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

#include <qmessagebox.h>
#include <qpainter.h>

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
  edit_filter_label=new QLabel(edit_filter_box,tr("Filter:"),this);
  edit_filter_label->setFont(labelFont());
  edit_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_filter_box,SIGNAL(activated(int)),
	  this,SLOT(filterActivatedData(int)));

  //
  // Clocks List
  //
  edit_clocks_list=new Q3ListView(this);
  edit_clocks_list->setAllColumnsShowFocus(true);
  edit_clocks_list->setItemMargin(5);
  edit_clocks_list->addColumn(tr("Name"));
  edit_clocks_list->addColumn(tr("Code"));
  edit_clocks_list->addColumn(tr("Color"));
  edit_clocks_list->setColumnAlignment(2,Qt::AlignCenter);
  connect(edit_clocks_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  //
  //  Add Button
  //
  edit_add_button=new QPushButton(this);
  edit_add_button->setFont(buttonFont());
  edit_add_button->setText(tr("&Add"));
  connect(edit_add_button,SIGNAL(clicked()),this,SLOT(addData()));
    
  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setFont(buttonFont());
  edit_edit_button->setText(tr("&Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editData()));
    
  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->setFont(buttonFont());
  edit_delete_button->setText(tr("&Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));
    
  //
  //  Rename Button
  //
  edit_rename_button=new QPushButton(this);
  edit_rename_button->setFont(buttonFont());
  edit_rename_button->setText(tr("&Rename"));
  connect(edit_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));
    
  //
  //  Close Button
  //
  edit_close_button=new QPushButton(this);
  edit_close_button->setFont(buttonFont());
  edit_close_button->setText(tr("C&lose"));
  connect(edit_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  //  Clear Button
  //
  edit_clear_button=new QPushButton(this);
  edit_clear_button->setFont(buttonFont());
  edit_clear_button->setText(tr("C&lear"));
  connect(edit_clear_button,SIGNAL(clicked()),this,SLOT(clearData()));

  //
  //  OK Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("&Cancel"));
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
  edit_filter_box->insertItem(tr("ALL"));
  edit_filter_box->insertItem(tr("NONE"));

  QString sql="select NAME from SERVICES";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append( q->value(0).toString() );
  }
  delete q;
  services_list.sort();
  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end();
        ++it ) {
    edit_filter_box->insertItem(*it);
    if(*clock_filter==*it) {
      edit_filter_box->setCurrentItem(edit_filter_box->count()-1);
    }
  }
  RefreshList();

  if(edit_clockname!=NULL) {
    Q3ListViewItem *item=edit_clocks_list->firstChild();
    while(item!=NULL) {
      if(item->text(0)==*edit_clockname) {
	edit_clocks_list->setSelected(item,true);
      }
      item=item->nextSibling();
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
  sql=QString("select NAME from CLOCKS where ")+
    "NAME=\""+RDEscapeString(clockname)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::
      information(this,tr("Clock Exists"),
		  tr("An clock with that name already exists!"));
    delete q;
    return;
  }
  delete q;
  sql=QString("insert into CLOCKS set ")+
    "NAME=\""+RDEscapeString(clockname)+"\","+
    "ARTISTSEP=15";
  q=new RDSqlQuery(sql);
  delete q;
  EditClock *clock_dialog=new EditClock(clockname,true,&new_clocks,this);
  if(clock_dialog->exec()<0) {
    sql=QString("delete from CLOCK_LINES where ")+
      "CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
    RDSqlQuery::apply(sql);
    sql=QString("delete from CLOCKS where ")+
      "NAME=\""+RDEscapeString(clockname)+"\"";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    if(edit_filter_box->currentItem()==0) {
      sql=QString("select ")+
	"ID "+  // 00
	"from CLOCK_PERMS where "+
	"CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	sql="select NAME from SERVICES";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into CLOCK_PERMS set ")+
	    "CLOCK_NAME=\""+RDEscapeString(clockname)+"\","+
	    "SERVICE_NAME=\""+RDEscapeString(q1->value(0).toString())+"\"";
	  RDSqlQuery::apply(sql);
	}
	delete q1;
      }
      delete q;
    }
    else {
      sql=QString("insert into CLOCK_PERMS set ")+
	"CLOCK_NAME=\""+RDEscapeString(clockname)+"\","+
	"SERVICE_NAME=\""+RDEscapeString(edit_filter_box->currentText())+"\"";
      RDSqlQuery::apply(sql);
    }
    Q3ListViewItem *item=new Q3ListViewItem(edit_clocks_list);
    item->setText(0,clockname);
    RefreshItem(item,&new_clocks);
    edit_clocks_list->setSelected(item,true);
  }
  delete clock_dialog;
}


void ListClocks::editData()
{
  std::vector<QString> new_clocks;
  Q3ListViewItem *item=edit_clocks_list->selectedItem();
  if(item==NULL) {
    return;
  }
  EditClock *clock_dialog=new EditClock(item->text(0),false,&new_clocks,this);
  if(clock_dialog->exec()<0) {
    delete clock_dialog;
    return;
  }
  delete clock_dialog;
  RefreshItem(item,&new_clocks);
}


void ListClocks::deleteData()
{
  int n;
  QString svc_list;
  Q3ListViewItem *item=edit_clocks_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if(QMessageBox::question(this,"RDLogManager - "+tr("Delete Clock"),
			   tr("Are you sure you want to delete")+" \""+
			   item->text(0)+"\"?",
			  QMessageBox::Yes,QMessageBox::No)
     !=QMessageBox::Yes) {
    return;
  }
  if((n=ActiveClocks(item->text(0),&svc_list))>0) {
    if(QMessageBox::warning(this,"RDLogManager - "+tr("Clock In Use"),
			    "\""+item->text(0)+"\" "+
			    tr("is in use in the following grid(s)")+":\n\n"+
			    svc_list+"\n"+
			    tr("Do you still want to delete it?"),
			    QMessageBox::Yes,
			    QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }
  }
  DeleteClock(item->text(0));
  RefreshList();
}


void ListClocks::renameData()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  Q3ListViewItem *item=edit_clocks_list->selectedItem();
  if(item==NULL) {
    return;
  }
  QString new_name=item->text(0);
  RenameItem *rename_dialog=new RenameItem(&new_name,"CLOCKS",this);
  if(rename_dialog->exec()<-1) {
    delete rename_dialog;
    return;
  }
  delete rename_dialog;
  
  //
  // Rename Grid References
  //
  sql="select NAME from SERVICES";
  // FIXME: not sure if the usersec service filter should be applied here, or
  // if all services should be brought over
  q=new RDSqlQuery(sql);
  while(q->next()) {
    for(int i=0;i<168;i++) {
      sql=QString("update SERVICE_CLOCKS set ")+
	"CLOCK_NAME=\""+RDEscapeString(new_name)+"\" where "+
	"CLOCK_NAME=\""+RDEscapeString(item->text(0))+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
  }
  delete q;

  //
  // Rename Meta Table
  //
  sql=QString("update CLOCK_LINES set ")+
    "CLOCK_NAME=\""+RDEscapeString(new_name)+"\" where "+
    "CLOCK_NAME=\""+RDEscapeString(item->text(0))+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("update RULE_LINES set ")+
    "CLOCK_NAME=\""+RDEscapeString(new_name)+"\" where "+
    "CLOCK_NAME=\""+RDEscapeString(item->text(0))+"\"";
  RDSqlQuery::apply(sql);

  //
  // Rename Service Permissions
  //
  sql=QString("update CLOCK_PERMS set ")+
    "CLOCK_NAME=\""+RDEscapeString(new_name)+"\" where "+
    "CLOCK_NAME=\""+RDEscapeString(item->text(0))+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Rename Primary Key
  //
  sql=QString("update CLOCKS set ")+
    "NAME=\""+RDEscapeString(new_name)+"\" where "+
    "NAME=\""+RDEscapeString(item->text(0))+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  item->setText(0,new_name);
  RefreshItem(item);
}


void ListClocks::filterActivatedData(int id)
{
  RefreshList();
}


void ListClocks::doubleClickedData(Q3ListViewItem *item,const QPoint &,int)
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
  Q3ListViewItem *item=edit_clocks_list->selectedItem();
  if(item!=NULL) {
    edit_clocks_list->setSelected(item,false);
  }
}


void ListClocks::okData()
{
  Q3ListViewItem *item=edit_clocks_list->selectedItem();
  *clock_filter=edit_filter_box->currentText();
  if(item==NULL) {
    *edit_clockname="";
  }
  else {
    *edit_clockname=item->text(0);
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
  edit_clocks_list->setGeometry(10,45,
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


void ListClocks::RefreshList()
{
  QString filter;

  if(edit_filter_box->currentItem()==1) {
    filter=GetNoneFilter();
  }
  else {
    if(edit_filter_box->currentItem()>1) {
      filter=GetClockFilter(edit_filter_box->currentText());
    }
  }

  edit_clocks_list->clear();
  QString sql=QString("select ")+
    "NAME,"+        // 00
    "SHORT_NAME,"+  // 01
    "COLOR "+       // 02
    "from CLOCKS "+filter;
  RDSqlQuery *q=new RDSqlQuery(sql);
  Q3ListViewItem *item=NULL;
  while(q->next()) {
    item=new Q3ListViewItem(edit_clocks_list);
    WriteItem(item,q);
  }
  delete q;
}


void ListClocks::RefreshItem(Q3ListViewItem *item,
			     std::vector<QString> *new_clocks)
{
  Q3ListViewItem *new_item;
  UpdateItem(item,item->text(0));

  if(new_clocks!=NULL) {
    for(unsigned i=0;i<new_clocks->size();i++) {
      if((new_item=edit_clocks_list->findItem(new_clocks->at(i),0))==NULL) {
	new_item=new Q3ListViewItem(edit_clocks_list);
      }
      UpdateItem(new_item,new_clocks->at(i));
    }
  }
}


void ListClocks::UpdateItem(Q3ListViewItem *item,QString name)
{
  QString sql=QString("select ")+
    "NAME,"+        // 00
    "SHORT_NAME,"+  // 01
    "COLOR "+       // 02
    "from CLOCKS where "+
    "NAME=\""+RDEscapeString(name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(0,name);
    WriteItem(item,q);
  }
  delete q;
}


void ListClocks::WriteItem(Q3ListViewItem *item,RDSqlQuery *q)
{
  QPixmap *pix;
  QPainter *p=new QPainter();

  item->setText(0,q->value(0).toString());
  item->setText(1,q->value(1).toString());
  pix=new QPixmap(QSize(15,15));
  p->begin(pix);
  p->fillRect(0,0,15,15,QColor(q->value(2).toString()));
  p->end();
  item->setPixmap(2,*pix);

  delete p;
}


int ListClocks::ActiveClocks(QString clockname,QString *svc_list)
{
  int n=0;
  QString sql;
  RDSqlQuery *q;
  QString svcname;
  QStringList svcs;

  sql=QString("select SERVICE_NAME from SERVICE_CLOCKS where ")+
    "CLOCK_NAME=\""+RDEscapeString(clockname)+"\" order by CLOCK_NAME";
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
  sql=QString("update SERVICE_CLOCKS set ")+
    "CLOCK_NAME=null where "+
    "CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Service Associations
  //
  sql=QString("delete from CLOCK_PERMS where ")+
    "CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Clock Definition
  //
  sql=QString("delete from CLOCKS where ")+
    "NAME=\""+RDEscapeString(clockname)+"\"";
  RDSqlQuery::apply(sql);
  sql=QString("delete from CLOCK_LINES where ")+
    "CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
  RDSqlQuery::apply(sql);

  sql=QString("delete from RULE_LINES where ")+
    "CLOCK_NAME=\""+RDEscapeString(clockname)+"\"";
  RDSqlQuery::apply(sql);
}


QString ListClocks::GetClockFilter(QString svc_name)
{
  QString filter="where ";
  QString sql=QString("select CLOCK_NAME from CLOCK_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->size()>0) {
    while(q->next()) {
      filter+=QString("(NAME=\"")+
	RDEscapeString(q->value(0).toString())+"\")||";
    }
    filter=filter.left(filter.length()-2);
  }
  else {
    filter="(SERVICE_NAME=\"\")";
  }
  delete q;

  return filter;
}


QString ListClocks::GetNoneFilter()
{
  QString sql;
  RDSqlQuery *q;
  QString filter;

  sql="select CLOCK_NAME from CLOCK_PERMS";
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    filter="where ";
  }
  while(q->next()) {
    filter+=QString("(NAME!=\"")+RDEscapeString(q->value(0).toString())+"\")&&";
  }
  if(q->size()>0) {
    filter=filter.left(filter.length()-2);
  }
  delete q;

  return filter;
}

