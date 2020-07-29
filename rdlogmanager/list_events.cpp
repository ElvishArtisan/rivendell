// list_events.cpp
//
// List a Rivendell Log Event
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
  edit_filter_label=new QLabel(edit_filter_box,tr("Filter:"),this);
  edit_filter_label->setGeometry(10,10,50,20);
  edit_filter_label->setFont(labelFont());
  edit_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_filter_box,SIGNAL(activated(int)),
	  this,SLOT(filterActivatedData(int)));

  //
  // Events List
  //
  edit_events_list=new Q3ListView(this);
  edit_events_list->setAllColumnsShowFocus(true);
  edit_events_list->setItemMargin(5);
  edit_events_list->addColumn(tr("Name"));
  edit_events_list->addColumn(tr("Trans"));
  edit_events_list->setColumnAlignment(1,Qt::AlignCenter);
  edit_events_list->addColumn(tr("Properties"));
  connect(edit_events_list,
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
  edit_close_button->setText(tr("&OK"));
  connect(edit_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("&Ok"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("&Cancel"));
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
    if(*event_filter==*it) {
      edit_filter_box->setCurrentItem(edit_filter_box->count()-1);
    }
  }

  RefreshList();
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
  QString sql=QString().sprintf("select NAME from EVENTS where NAME=\"%s\"",
				(const char *)logname);
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
    sql=QString().sprintf("delete from EVENTS where NAME=\"%s\"",
			  (const char *)logname);
    q=new RDSqlQuery(sql);
    delete q;
    return;
  }
  else {
    if(edit_filter_box->currentItem()==0) {
      sql="select NAME from SERVICES";
      q=new RDSqlQuery(sql);
      while(q->next()) {
	sql=QString().sprintf("insert into EVENT_PERMS set\
                               EVENT_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			      (const char *)logname,
			      (const char *)q->value(0).toString());
	q1=new RDSqlQuery(sql);
	delete q1;
      }
      delete q;
    }
    else {
      sql=QString().sprintf("insert into EVENT_PERMS set\
                             EVENT_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)logname,
			    (const char *)edit_filter_box->currentText());
      q=new RDSqlQuery(sql);
      delete q;
    }
  }
  delete event_dialog;
  Q3ListViewItem *item=new Q3ListViewItem(edit_events_list);
  item->setText(0,logname);
  RefreshItem(item,&new_events);
  edit_events_list->setSelected(item,true);
  edit_events_list->ensureItemVisible(item);
}


void ListEvents::editData()
{
  std::vector<QString> new_events;

  Q3ListViewItem *item=edit_events_list->selectedItem();
  if(item==NULL) {
    return;
  }
  EditEvent *event_dialog=new EditEvent(item->text(0),false,&new_events,this);
  if(event_dialog->exec()<-1) {
    delete event_dialog;
    return;
  }
  delete event_dialog;
  RefreshItem(item,&new_events);
}


void ListEvents::deleteData()
{
  int n;
  QString clock_list;

  Q3ListViewItem *item=edit_events_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if(QMessageBox::question(this,"RDLogManager - "+tr("Delete Event"),
			   tr("Are you sure you want to delete")+" \""+
			   item->text(0)+"\"?",
			  QMessageBox::Yes,QMessageBox::No)
     !=QMessageBox::Yes) {
    return;
  }
  if((n=ActiveEvents(item->text(0),&clock_list))>0) {
    if(QMessageBox::warning(this,"RDLogManager - "+tr("Event In Use"),
			    "\""+item->text(0)+"\" "+
			    tr("is in use in the following clocks")+":\n\n"+
			    clock_list+"\n"+
			    tr("Do you still want to delete it?"),
			    QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
  }
  DeleteEvent(item->text(0));
  delete item;
  RefreshList();
}


void ListEvents::renameData()
{
  QString sql;
  RDSqlQuery *q;
  Q3ListViewItem *item=edit_events_list->selectedItem();
  if(item==NULL) {
    return;
  }
  QString new_name=item->text(0);
  RenameItem *rename_dialog=new RenameItem(&new_name,"EVENTS",this);
  if(rename_dialog->exec()<-1) {
    delete rename_dialog;
    return;
  }
  delete rename_dialog;
  
  //
  // Rename Clock References
  //
  sql=QString("update CLOCK_LINES set ")+
    "EVENT_NAME=\""+RDEscapeString(new_name)+"\" where "+
    "EVENT_NAME=\""+RDEscapeString(item->text(0))+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Rename Event Line References
  //
  sql=QString("update EVENT_LINES set ")+
    "EVENT_NAME=\""+RDEscapeString(new_name)+"\" where "+
    "EVENT_NAME=\""+RDEscapeString(item->text(0))+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Rename Service Permissions
  //
  sql=QString().sprintf("update EVENT_PERMS set EVENT_NAME=\"%s\"\
                         where EVENT_NAME=\"%s\"",
			(const char *)new_name,
			(const char *)item->text(0));
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Rename Primary Key
  //
  sql=QString().sprintf("update EVENTS set NAME=\"%s\" where NAME=\"%s\"",
			(const char *)new_name,
			(const char *)item->text(0));
  q=new RDSqlQuery(sql);
  delete q;

  item->setText(0,new_name);
  RefreshItem(item);
}


void ListEvents::filterActivatedData(int id)
{
  RefreshList();
}


void ListEvents::doubleClickedData(Q3ListViewItem *item,const QPoint &,int)
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
  Q3ListViewItem *item=edit_events_list->selectedItem();
  *event_filter=edit_filter_box->currentText();
  if(item==NULL) {
    done(-1);
    return;
  }
  *edit_eventname=item->text(0);
  done(0);
}


void ListEvents::cancelData()
{
  done(-1);
}


void ListEvents::resizeEvent(QResizeEvent *e)
{
  edit_filter_box->setGeometry(65,10,size().width()-75,20);
  edit_events_list->setGeometry(10,45,size().width()-20,size().height()-125);
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


void ListEvents::RefreshList()
{
  QString filter;

  if(edit_filter_box->currentItem()==1) {  // NONE Filter
    filter=GetNoneFilter();
  }
  else {
    if(edit_filter_box->currentItem()>1) {
      filter=GetEventFilter(edit_filter_box->currentText());
    }
  }

  edit_events_list->clear();
  QString sql=WriteItemSql()+" "+filter;
  RDSqlQuery *q=new RDSqlQuery(sql);
  Q3ListViewItem *item=NULL;
  while(q->next()) {
    item=new Q3ListViewItem(edit_events_list);
    WriteItem(item,q);
  }
  delete q;
}


void ListEvents::RefreshItem(Q3ListViewItem *item,
			     std::vector<QString> *new_events)
{
  Q3ListViewItem *new_item;
  UpdateItem(item,item->text(0));

  if(new_events!=NULL) {
    for(unsigned i=0;i<new_events->size();i++) {
      if((new_item=edit_events_list->findItem(new_events->at(i),0))==NULL) {
	new_item=new Q3ListViewItem(edit_events_list);
      }
      UpdateItem(new_item,new_events->at(i));
    }
  }
}


void ListEvents::UpdateItem(Q3ListViewItem *item,QString name)
{
  QString sql=WriteItemSql()+"where NAME=\""+RDEscapeString(name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(0,name);
    WriteItem(item,q);
  }
  delete q;
}


void ListEvents::WriteItem(Q3ListViewItem *item,RDSqlQuery *q)
{
  QPixmap *pix;
  QPainter *p=new QPainter();

  item->setText(0,q->value(0).toString());
  item->setText(1,
	RDLogLine::transText((RDLogLine::TransType)q->value(3).toUInt()));
  item->setText(2,RDEventLine::
		propertiesText(q->value(2).toInt(),
			       (RDLogLine::TransType)q->value(3).toUInt(),
			       (RDLogLine::TimeType)q->value(4).toUInt(),
			       q->value(5).toInt(),
			       RDBool(q->value(6).toString()),
			       (RDEventLine::ImportSource)q->value(7).toUInt(),
			       !q->value(8).toString().isEmpty()));
  pix=new QPixmap(QSize(15,15));
  p->begin(pix);
  p->fillRect(0,0,15,15,QColor(q->value(1).toString()));
  p->end();
  item->setPixmap(0,*pix);

  delete p;
}


QString ListEvents::WriteItemSql() const
{
  QString sql=QString("select ")+
    "NAME,"+              // 00
    "COLOR,"+             // 01
    "PREPOSITION,"+       // 02
    "FIRST_TRANS_TYPE,"+  // 03
    "TIME_TYPE,"+         // 04
    "GRACE_TIME,"+        // 05
    "USE_AUTOFILL,"+      // 06
    "IMPORT_SOURCE,"+     // 07
    "NESTED_EVENT "+      // 08
    "from EVENTS ";

  return sql;
}


int ListEvents::ActiveEvents(QString event_name,QString *clock_list)
{
  int n=0;
  QString sql;
  RDSqlQuery *q,*q1;

  sql="select NAME from CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("select EVENT_NAME from CLOCK_LINES where ")+
      "CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
      "EVENT_NAME=\""+RDEscapeString(event_name)+"\"";
    q1=new RDSqlQuery(sql);
    if(q1->first()) {
      *clock_list+=
	QString().sprintf("    %s\n",(const char *)q->value(0).toString());
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
  RDSqlQuery *q,*q1;
  QString base_name=event_name;
  base_name.replace(" ","_");

  //
  // Delete Active Clock Entries
  //
  sql="select NAME from CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("delete from CLOCK_LINES where ")+
      "CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
      "EVENT_NAME=\""+RDEscapeString(event_name)+"\"";
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;

  //
  // Delete Service Associations
  //
  sql=QString().sprintf("delete from EVENT_PERMS where EVENT_NAME=\"%s\"",
			(const char *)event_name);
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Event Definition
  //
  sql=QString().sprintf("delete from EVENTS where NAME=\"%s\"",
			(const char *)event_name);
  RDSqlQuery::apply(sql);
  sql=QString("delete from EVENT_LINES where ")+
    "EVENT_NAME=\""+RDEscapeString(event_name)+"\"";
  RDSqlQuery::apply(sql);
}


QString ListEvents::GetEventFilter(QString svc_name)
{
  QString filter="where ";
  QString sql=QString("select EVENT_NAME from EVENT_PERMS where ")+
    "SERVICE_NAME=\""+RDEscapeString(svc_name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->size()>0) {
    while(q->next()) {
      filter+=QString().sprintf("(NAME=\"%s\")||",
				(const char *)q->value(0).toString());
    }
    filter=filter.left(filter.length()-2);
  }
  else {
    filter="(SERVICE_NAME=\"\")";
  }
  delete q;

  return filter;
}


QString ListEvents::GetNoneFilter()
{
  QString sql;
  RDSqlQuery *q;
  QString filter;

  sql="select EVENT_NAME from EVENT_PERMS";
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    filter="where ";
  }
  while(q->next()) {
    filter+=QString().sprintf("(NAME!=\"%s\")&&",
			      (const char *)q->value(0).toString());
  }
  if(q->size()>0) {
    filter=filter.left(filter.length()-2);
  }
  delete q;

  return filter;
}
