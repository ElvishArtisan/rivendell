// edit_event.cpp
//
// Edit a Rivendell Log Event
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>

#include <rdcartfilter.h>
#include <rdcart_search_text.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include "add_event.h"
#include "edit_event.h"
#include "edit_perms.h"

EditEvent::EditEvent(QString eventname,bool new_event,QStringList *new_events,
		     QStringList *modified_events,QWidget *parent)
  : RDDialog(parent)
{
  event_saved=false;
  event_name=eventname;
  event_new_event=new_event;
  event_new_events=new_events;
  event_modified_events=modified_events;
  event_event=new RDEvent(eventname);

  setWindowTitle("RDLogManager - "+tr("Editing Event")+" - "+
		 event_event->name());

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  // *******************************
  // Library Section
  // *******************************
  //
  // Text Filter
  //
  event_lib_filter_edit=new QLineEdit(this);
  if(rda->station()->filterMode()==RDStation::FilterAsynchronous) {
    connect(event_lib_filter_edit,SIGNAL(returnPressed()),
	    this,SLOT(searchData()));
  }
  connect(event_lib_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  event_lib_filter_label=new QLabel(tr("Filter:"),this);
  event_lib_filter_label->setFont(labelFont());
  event_lib_filter_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  event_search_button=new QPushButton(tr("Search"),this);
  event_search_button->setFont(subButtonFont());
  event_search_button->setDisabled(true);
  connect(event_search_button,SIGNAL(clicked()),this,SLOT(searchData()));
  event_search_button->
    setVisible(rda->station()->filterMode()==RDStation::FilterAsynchronous);

  //
  // Group Filter
  //
  event_group_box=new QComboBox(this);
  event_group_model=new RDGroupListModel(true,false,false,this);
  event_group_model->changeUser();
  event_group_box->setModel(event_group_model);
  connect(event_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(filterActivatedData(const QString &)));
  event_group_label=new QLabel(tr("Group:"),this);
  event_group_label->setFont(labelFont());
  event_group_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  // Type Filter
  //
  event_lib_type_group=new QButtonGroup(this);
  connect(event_lib_type_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(filterClickedData(int)));
  event_lib_type_none_radio=new QRadioButton(this);
  event_lib_type_group->addButton(event_lib_type_none_radio,0);
  event_lib_type_none_label=new QLabel(tr("All"),this);
  event_lib_type_none_label->setFont(labelFont());
  event_lib_type_none_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_lib_type_none_radio->setChecked(true);

  event_lib_type_audio_radio=new QRadioButton(this);
  event_lib_type_group->addButton(event_lib_type_audio_radio,1);
  event_lib_type_audio_label=new QLabel(tr("Audio Only"),this);
  event_lib_type_audio_label->setFont(labelFont());
  event_lib_type_audio_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  event_lib_type_macro_radio=new QRadioButton(this);
  event_lib_type_group->addButton(event_lib_type_macro_radio,2);
  event_lib_type_macro_label=new QLabel(tr("Macros Only"),this);
  event_lib_type_macro_label->setFont(labelFont());
  event_lib_type_macro_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Cart List
  //
  event_lib_view=new LibraryTableView(this);
  event_lib_view->setDragEnabled(true);
  event_lib_model=new RDLibraryModel(this);
  event_lib_model->setFont(font());
  event_lib_model->setPalette(palette());
  event_lib_view->setModel(event_lib_model);
  event_lib_view->hideColumn(3);
  connect(event_lib_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));
  connect(event_lib_model,SIGNAL(modelReset()),
	  event_lib_view,SLOT(resizeColumnsToContents()));

  //
  // Empty Cart Source
  //
  event_empty_cart=new RDEmptyCart(this);

  //
  // Cart Player
  //
  QString sql;
  RDSqlQuery *q;
  event_player=NULL;
  sql=QString("select ")+
    "`OUTPUT_CARD`,"+  // 00
    "`OUTPUT_PORT`,"+  // 01
    "`START_CART`,"+   // 02
    "`END_CART` "+     // 03
    "from `RDLOGEDIT` where "+
    "`STATION`='"+RDEscapeString(rda->station()->name())+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    event_player=
      new RDSimplePlayer(rda->cae(),rda->ripc(),q->value(0).toInt(),
			 q->value(1).toInt(),q->value(2).toUInt(),
			 q->value(3).toUInt(),this);
    event_player->stopButton()->setOnColor(Qt::red);
  }
  delete q;

  //
  // Remarks
  //
  event_remarks_edit=new QTextEdit(this);
  event_remarks_edit->setAcceptRichText(false);
  event_remarks_label=new QLabel(tr("USER NOTES"),this);
  event_remarks_label->setFont(labelFont());
  event_remarks_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Load Group List
  //
  RefreshLibrary();

  //
  // Properties Section
  //
  event_widget=new EventWidget(this);
  
  //
  //  Save Button
  //
  event_save_button=new QPushButton(this);
  event_save_button->setFont(buttonFont());
  event_save_button->setText(tr("Save"));
  connect(event_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  event_saveas_button=new QPushButton(this);
  event_saveas_button->setFont(buttonFont());
  event_saveas_button->setText(tr("Save As"));
  connect(event_saveas_button,SIGNAL(clicked()),this,SLOT(saveAsData()));
  event_saveas_button->setDisabled(new_event);

  //
  //  Service Association Button
  //
  event_services_list_button=new QPushButton(this);
  event_services_list_button->setFont(buttonFont());
  event_services_list_button->setText(tr("Services\nList"));
  connect(event_services_list_button,SIGNAL(clicked()),this,SLOT(svcData()));

  //
  //  Color Button
  //
  event_color_button=new QPushButton(this);
  event_color_button->setFont(buttonFont());
  event_color_button->setText(tr("Color"));
  connect(event_color_button,SIGNAL(clicked()),this,SLOT(colorData()));
  event_color=palette().color(QPalette::Background);
  
  //
  //  OK Button
  //
  event_ok_button=new QPushButton(this);
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    event_ok_button->setDefault(true);
  }
  event_ok_button->setFont(buttonFont());
  event_ok_button->setText(tr("OK"));
  connect(event_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  event_cancel_button=new QPushButton(this);
  event_cancel_button->setFont(buttonFont());
  event_cancel_button->setText(tr("Cancel"));
  connect(event_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Event
  //
  event_remarks_edit->setText(event_event->remarks());
  event_color=event_event->color();
  if(event_color.isValid()) {
    event_color_button->
      setPalette(QPalette(event_color,palette().color(QPalette::Background)));
  }
  event_widget->load(event_event);
}


EditEvent::~EditEvent()
{
  delete event_lib_view;
}


QSize EditEvent::sizeHint() const
{
  return QSize(1300,800);
} 


QSizePolicy EditEvent::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditEvent::filterChangedData(const QString &str)
{
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    RefreshLibrary();
  }
  else {
    event_search_button->setEnabled(true);
  }
}


void EditEvent::filterActivatedData(const QString &str)
{
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    RefreshLibrary();
  }
  else {
    event_search_button->setEnabled(true);
  }
}


void EditEvent::filterClickedData(int id)
{
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    RefreshLibrary();
  }
  else {
    event_search_button->setEnabled(true);
  }
}


void EditEvent::searchData()
{
  RefreshLibrary();
  event_search_button->setDisabled(true);
}


void EditEvent::selectionChangedData(const QItemSelection &before,
				     const QItemSelection &after)
{
  QModelIndexList rows=event_lib_view->selectionModel()->selectedRows();

  if(event_player==NULL) {
    return;
  }
  if(rows.size()!=1) {
    event_player->setCart(0);
    return;
  }
  event_player->setCart(event_lib_model->cartNumber(rows.first()));
}


void EditEvent::saveData()
{
  Save();
  event_new_event=false;
}


void EditEvent::saveAsData()
{
  QString old_name;
  QString str;

  old_name=event_name;
  AddEvent *add_dialog=new AddEvent(&event_name,this);
  if(!add_dialog->exec()) {
    delete add_dialog;
    return;
  }
  delete add_dialog;
  QString sql=QString("select `NAME` from `EVENTS` where ")+
    "`NAME`='"+RDEscapeString(event_name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete event_event;
    event_event=new RDEvent(event_name,true);
    event_widget->rename(event_name);
    Save();
    event_new_events->push_back(event_name);
    CopyEventPerms(old_name,event_name);
    if(event_new_event) {
      AbandonEvent(old_name);
    }
    setWindowTitle("RDLogManager - "+tr("Editing Event")+" - "+
		   event_event->name());
  }
  else {
    if(QMessageBox::question(this,"RDLogManager - "+tr("Event Exists"),
			     tr("An event with that name already exists!")+
			     "\n"+tr("Do you want to overwrite it?"),
		   QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    delete event_event;
    event_event=new RDEvent(event_name,true);
    event_widget->rename(event_name);
    Save();
    event_modified_events->push_back(event_name);
    sql=QString("delete from `EVENT_PERMS` where ")+
      "`EVENT_NAME`='"+RDEscapeString(event_name)+"'";
    q=new RDSqlQuery(sql);
    delete q;
    CopyEventPerms(old_name,event_name);
    if(event_new_event) {
      AbandonEvent(old_name);
    }
    str=QString(tr("Edit Event"));
    setWindowTitle("RDLogManager - "+tr("Edit Event")+" - "+
		   event_event->name());
  }
}


void EditEvent::svcData()
{
  EditPerms *dialog=new EditPerms(event_name,EditPerms::ObjectEvent,this);
  dialog->exec();
  delete dialog;
}


void EditEvent::colorData()
{
  QColor color=
    QColorDialog::getColor(event_color_button->palette().color(QPalette::Background),this);
  if(color.isValid()) {
    event_color=color;
    event_color_button->setPalette(QPalette(color,palette().color(QPalette::Background)));
  }
}


void EditEvent::okData()
{
  Save();
  if (event_player){
    event_player->stop();
  }

  done(0);
}


void EditEvent::cancelData()
{
  if (event_player){
    event_player->stop();
  }
  if(event_saved) {
    done(-1);
  }
  else {
    done(-2);
  }
}


void EditEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditEvent::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();
  int x_divide=w-event_widget->sizeHint().width();

  if(rda->station()->filterMode()==RDStation::FilterAsynchronous) {
    event_lib_filter_edit->setGeometry(55,2,x_divide-130,20);
  }
  else {
    event_lib_filter_edit->setGeometry(55,2,x_divide-65,20);
  }

  event_lib_filter_label->setGeometry(5,2,45,20);
  event_search_button->setGeometry(CENTER_LINE-70,2,60,20);
  event_group_box->setGeometry(55,25,x_divide-65,20);
  event_group_label->setGeometry(5,25,45,20);
  event_lib_type_none_radio->setGeometry(55,55,15,15);
  event_lib_type_none_label->setGeometry(75,55,30,15);
  event_lib_type_audio_radio->setGeometry(125,55,15,15);
  event_lib_type_audio_label->setGeometry(145,55,80,15);
  event_lib_type_macro_radio->setGeometry(235,55,15,15);
  event_lib_type_macro_label->setGeometry(255,55,80,15);

  event_lib_view->setGeometry(10,80,x_divide-20,h/2);
  event_empty_cart->setGeometry(x_divide-230,h/2+100,32,32);
  if(event_player!=NULL) {
    event_player->playButton()->setGeometry(x_divide-180,h/2+90,80,50);
    event_player->stopButton()->setGeometry(x_divide-90,h/2+90,80,50);
  }
  event_remarks_label->setGeometry(15,h/2+135,100,15);
  event_remarks_edit->setGeometry(10,h/2+150,x_divide-20,h-(h/2+180));
  event_widget->setGeometry(30+x_divide,0,
			    event_widget->sizeHint().width(),
			    event_widget->sizeHint().height());

  event_save_button->setGeometry(w-610,h-60,80,50);
  event_saveas_button->setGeometry(w-520,h-60,80,50);

  event_services_list_button->setGeometry(w-395,h-60,80,50);
  event_color_button->
    setGeometry(w-305,h-60,80,50);

  event_ok_button->setGeometry(w-180,h-60,80,50);
  event_cancel_button->setGeometry(w-90,h-60,80,50);
}


void EditEvent::paintEvent(QPaintEvent *e)
{
  int x_divide=size().width()-event_widget->sizeHint().width();

  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->drawLine(x_divide,10,x_divide,size().height()-10);
  p->end();
}


void EditEvent::RefreshLibrary()
{
  QString sql=QString("where ")+
    RDCartFilter::typeFilter(event_lib_type_group->button(0)->isChecked()||
			     event_lib_type_group->button(1)->isChecked(),
			     event_lib_type_group->button(0)->isChecked()||
			     event_lib_type_group->button(2)->isChecked(),
			     RDCart::All)+
    RDCartFilter::phraseFilter(event_lib_filter_edit->text(),false)+
    RDCartFilter::groupFilter(event_group_box->currentText(),
			      event_group_model->allGroupNames());
  sql=sql.left(sql.length()-3);
  event_lib_model->setFilterSql(sql,RD_MAX_CART_NUMBER+1);
}


void EditEvent::Save()
{
  QString properties;

  event_widget->save(event_event);
  event_event->setRemarks(event_remarks_edit->toPlainText());
  event_event->setColor(event_color);

  event_saved=true;
}


void EditEvent::CopyEventPerms(QString old_name,QString new_name)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("select `SERVICE_NAME` from `EVENT_PERMS` where ")+
    "`EVENT_NAME`='"+RDEscapeString(old_name)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `EVENT_PERMS` set ")+
      "`EVENT_NAME`='"+RDEscapeString(new_name)+"',"+
      "`SERVICE_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}


void EditEvent::AbandonEvent(QString name)
{
  if(name==event_name) {
    return;
  }
  QString sql=QString("delete from `EVENTS` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from `EVENT_PERMS` where ")+
    "`EVENT_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `EVENT_LINES` where ")+
    "`EVENT_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
}
