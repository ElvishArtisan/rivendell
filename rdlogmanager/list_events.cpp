// list_events.cpp
//
// List a Rivendell Log Event
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_events.cpp,v 1.31.8.2 2014/01/10 19:32:54 cvs Exp $
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

#include <vector>

#include <qdialog.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rddb.h>
#include <rd.h>
#include <rdevent.h>
#include <rdcreate_log.h>

#include <list_events.h>
#include <add_event.h>
#include <edit_event.h>
#include <globals.h>
#include <rename_item.h>


ListEvents::ListEvents(QString *eventname,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QStringList services_list;
  QString str1=tr("Log Events - User: ");
  setCaption(QString().sprintf("%s%s",(const char *)str1,
			       (const char *)rdripc->user()));
  edit_eventname=eventname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Event Filter
  //
  edit_filter_box=new QComboBox(this);
  edit_filter_label=new QLabel(edit_filter_box,tr("Filter:"),this);
  edit_filter_label->setGeometry(10,10,50,20);
  edit_filter_label->setFont(bold_font);
  edit_filter_label->setAlignment(AlignRight|AlignVCenter);
  connect(edit_filter_box,SIGNAL(activated(int)),
	  this,SLOT(filterActivatedData(int)));

  //
  // Events List
  //
  edit_events_list=new QListView(this,"edit_events_list");
  edit_events_list->setAllColumnsShowFocus(true);
  edit_events_list->setItemMargin(5);
  edit_events_list->addColumn(tr("Name"));
  edit_events_list->addColumn(tr("Properties"));
  edit_events_list->addColumn(tr("Color"));
  edit_events_list->setColumnAlignment(2,AlignCenter);
  connect(edit_events_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Add Button
  //
  edit_add_button=new QPushButton(this);
  edit_add_button->setFont(bold_font);
  edit_add_button->setText(tr("&Add"));
  connect(edit_add_button,SIGNAL(clicked()),this,SLOT(addData()));
    
  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setFont(bold_font);
  edit_edit_button->setText(tr("&Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editData()));
    
  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->setFont(bold_font);
  edit_delete_button->setText(tr("&Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));
    
  //
  //  Rename Button
  //
  edit_rename_button=new QPushButton(this);
  edit_rename_button->setFont(bold_font);
  edit_rename_button->setText(tr("&Rename"));
  connect(edit_rename_button,SIGNAL(clicked()),this,SLOT(renameData()));
    
  //
  //  Close Button
  //
  edit_close_button=new QPushButton(this);
  edit_close_button->setFont(bold_font);
  edit_close_button->setText(tr("&OK"));
  connect(edit_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(bold_font);
  edit_ok_button->setText(tr("&Ok"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(bold_font);
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

  if (rdstation_conf->broadcastSecurity() == RDStation::UserSec) {
    services_list = rduser->services();
  } else { // RDStation::HostSec
    QString sql="select NAME from SERVICES";
    RDSqlQuery *q=new RDSqlQuery(sql);
    while(q->next()) {
      services_list.append( q->value(0).toString() );
    }
    delete q;
  } 
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

  AddEvent *add_dialog=new AddEvent(&logname,this,"add_dialog");
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
  EditEvent *event_dialog=new EditEvent(logname,true,&new_events,
					this,"event_dialog");
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
  QListViewItem *item=new QListViewItem(edit_events_list);
  item->setText(0,logname);
  RefreshItem(item,&new_events);
  edit_events_list->setSelected(item,true);
  edit_events_list->ensureItemVisible(item);
}


void ListEvents::editData()
{
  std::vector<QString> new_events;

  QListViewItem *item=edit_events_list->selectedItem();
  if(item==NULL) {
    return;
  }
  EditEvent *event_dialog=
    new EditEvent(item->text(0),false,&new_events,this,"event_dialog");
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
  QString str1;
  QString str2;

  QListViewItem *item=edit_events_list->selectedItem();
  if(item==NULL) {
    return;
  }
  str1=QString(tr("Are you sure you want to\ndelete"));
  if(QMessageBox::question(this,tr("Delete Event"),
			   QString().sprintf("%s \'%s\'?",(const char *)str1,
					     (const char *)item->text(0)),
			  QMessageBox::Yes,QMessageBox::No)
     !=QMessageBox::Yes) {
    return;
  }
  if((n=ActiveEvents(item->text(0),&clock_list))>0) {
    str1=QString(tr("is in use in the following clocks:"));
    str2=QString(tr("Do you still want to delete it?"));
    if(QMessageBox::warning(this,tr("Event In Use"),
			 QString().sprintf("\'%s\' %s\n\n%s\n%s",
					   (const char *)item->text(0),
					   (const char *)str1,
					   (const char *)clock_list,
					   (const char *)str2),
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
  RDSqlQuery *q1;
  QString clock_name_esc;
  QListViewItem *item=edit_events_list->selectedItem();
  if(item==NULL) {
    return;
  }
  QString new_name=item->text(0);
  RenameItem *rename_dialog=
    new RenameItem(&new_name,"EVENTS",this,"event_dialog");
  if(rename_dialog->exec()<-1) {
    delete rename_dialog;
    return;
  }
  delete rename_dialog;
  
  //
  // Rename Clock References
  //
  sql="select NAME from CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    clock_name_esc=q->value(0).toString();
    clock_name_esc.replace(" ","_");
    sql=QString().sprintf("update %s_CLK set EVENT_NAME=\"%s\"\
                           where EVENT_NAME=\"%s\"",
			  (const char *)clock_name_esc,
			  (const char *)new_name,
			  (const char *)item->text(0));
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;

  //
  // Rename Meta Tables
  //
  QString old_name_esc=item->text(0);
  old_name_esc.replace(" ","_");
  QString new_name_esc=new_name;
  new_name_esc.replace(" ","_");
  sql=QString().sprintf("alter table %s_PRE rename to %s_PRE",
			(const char *)old_name_esc,
			(const char *)new_name_esc);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("alter table %s_POST rename to %s_POST",
			(const char *)old_name_esc,
			(const char *)new_name_esc);
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


void ListEvents::doubleClickedData(QListViewItem *item,const QPoint &,int)
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
  QListViewItem *item=edit_events_list->selectedItem();
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
  QString sql=QString().sprintf("select NAME,PROPERTIES,COLOR from EVENTS %s",
				(const char *)filter);
  RDSqlQuery *q=new RDSqlQuery(sql);
  QListViewItem *item=NULL;
  while(q->next()) {
    item=new QListViewItem(edit_events_list);
    WriteItem(item,q);
  }
  delete q;
}


void ListEvents::RefreshItem(QListViewItem *item,
			     std::vector<QString> *new_events)
{
  QListViewItem *new_item;
  UpdateItem(item,item->text(0));

  if(new_events!=NULL) {
    for(unsigned i=0;i<new_events->size();i++) {
      if((new_item=edit_events_list->findItem(new_events->at(i),0))==NULL) {
	new_item=new QListViewItem(edit_events_list);
      }
      UpdateItem(new_item,new_events->at(i));
    }
  }
}


void ListEvents::UpdateItem(QListViewItem *item,QString name)
{
  QString sql=QString().sprintf("select NAME,PROPERTIES,COLOR from EVENTS\
                                 where NAME=\"%s\"",(const char *)name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(0,name);
    WriteItem(item,q);
  }
  delete q;
}


void ListEvents::WriteItem(QListViewItem *item,RDSqlQuery *q)
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


int ListEvents::ActiveEvents(QString event_name,QString *clock_list)
{
  int n=0;
  QString sql;
  RDSqlQuery *q,*q1;
  QString clockname;

  sql="select NAME from CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    clockname=q->value(0).toString();
    clockname.replace(" ","_");
    sql=QString().sprintf("select EVENT_NAME from %s_CLK\
                           where EVENT_NAME=\"%s\"",(const char *)clockname,
			  (const char *)event_name);
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
  QString clockname;
  QString base_name=event_name;
  base_name.replace(" ","_");

  //
  // Delete Active Clock Entries
  //
  sql="select NAME from CLOCKS";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    clockname=q->value(0).toString();
    clockname.replace(" ","_");
    sql=QString().sprintf("delete from %s_CLK\
                           where EVENT_NAME=\"%s\"",(const char *)clockname,
			  (const char *)event_name);
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
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("drop table %s_PRE",(const char *)base_name);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("drop table %s_POST",(const char *)base_name);
  q=new RDSqlQuery(sql);
}


QString ListEvents::GetEventFilter(QString svc_name)
{
  QString filter="where ";
  QString sql=QString().sprintf("select EVENT_NAME from EVENT_PERMS\
                                 where SERVICE_NAME=\"%s\"",
				(const char *)svc_name);
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
