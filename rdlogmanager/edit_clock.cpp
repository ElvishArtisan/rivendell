// edit_clock.cpp
//
// Edit Rivendell Log Clock
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

#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>

#include <rdconf.h>
#include <rdescape_string.h>

#include "add_clock.h"
#include "edit_clock.h"
#include "edit_event.h"
#include "edit_eventline.h"
#include "edit_perms.h"
#include "edit_schedrules.h"
#include "globals.h"
#include "list_clocks.h"

EditClock::EditClock(QString clockname,bool new_clock,
		     QStringList *new_clocks,QWidget *parent)
  : RDDialog(parent)
{
  QString str;

  edit_name=clockname;
  edit_new_clock=new_clock;
  edit_new_clocks=new_clocks;

  setWindowTitle("RDLogManager - "+tr("Edit Clock")+": "+clockname);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Fonts
  //
  edit_title_font=new QFont(labelFont().family(),24,QFont::Bold);
  edit_title_metrics=new QFontMetrics(*edit_title_font);

  //
  // Clock Names
  //
  edit_clockname_label=new QLabel(clockname,this);
  edit_clockname_label->setGeometry(10,10,280,20);
  edit_clockname_label->setFont(labelFont());
  edit_shortname_edit=new QLineEdit(this);
  edit_shortname_edit->setGeometry(350,10,40,20);
  edit_shortname_edit->setMaxLength(3);
  QLabel *label=new QLabel(tr("Code:"),this);
  label->setGeometry(295,10,50,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Clock List
  //
  edit_clocks_view=new ClockListView(this);
  edit_clocks_view->setGeometry(10,35,CENTER_LINE-20,sizeHint().height()-250);
  edit_clocks_model=new RDClockModel(rda->station(),this);
  edit_clocks_model->setFont(font());
  edit_clocks_model->setPalette(palette());
  edit_clocks_view->setModel(edit_clocks_model);
  connect(edit_clocks_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(edit_clocks_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,
				  const QItemSelection &)),
	  this,
	  SLOT(selectionChangedData(const QItemSelection &,
				    const QItemSelection &)));
  connect(edit_clocks_view,SIGNAL(editEventAtLine(int)),
	  this,SLOT(editEventData(int)));
  connect(edit_clocks_model,SIGNAL(modelReset()),
	  edit_clocks_view,SLOT(resizeColumnsToContents()));
  edit_clocks_view->resizeColumnsToContents();

  //
  //  Add Button
  //
  edit_add_button=new QPushButton(this);
  edit_add_button->setGeometry(10,sizeHint().height()-210,80,50);
  edit_add_button->setFont(buttonFont());
  edit_add_button->setText(tr("Add"));
  connect(edit_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Clone Button
  //
  edit_clone_button=new QPushButton(this);
  edit_clone_button->setGeometry(110,sizeHint().height()-210,80,50);
  edit_clone_button->setFont(buttonFont());
  edit_clone_button->setText(tr("Clone"));
  connect(edit_clone_button,SIGNAL(clicked()),this,SLOT(cloneData()));
  
  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setGeometry(210,sizeHint().height()-210,80,50);
  edit_edit_button->setFont(buttonFont());
  edit_edit_button->setText(tr("Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->setGeometry(310,sizeHint().height()-210,80,50);
  edit_delete_button->setFont(buttonFont());
  edit_delete_button->setText(tr("Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Remarks
  //
  edit_remarks_edit=new QTextEdit(this);
  edit_remarks_edit->setGeometry(10,sizeHint().height()-140,CENTER_LINE-20,130);
  edit_remarks_edit->setAcceptRichText(false);
  label=new QLabel(tr("USER NOTES"),this);
  label->setGeometry(15,sizeHint().height()-155,CENTER_LINE-20,15);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  //  Scheduler-Rules button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+10,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Scheduler\nRules"));
  connect(button,SIGNAL(clicked()),this,SLOT(schedRules()));

  //
  //  Save Button
  //
  button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+110,sizeHint().height()-60,70,50);
  button->setFont(buttonFont());
  button->setText(tr("Save"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+190,sizeHint().height()-60,70,50);
  button->setFont(buttonFont());
  button->setText(tr("Save As"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveAsData()));
  button->setDisabled(new_clock);

  //
  //  Service Associations Button
  //
  button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2-25,
		      sizeHint().height()-60,70,50);
  button->setFont(buttonFont());
  button->setText(tr("Services\nList"));
  connect(button,SIGNAL(clicked()),this,SLOT(svcData()));

  //
  //  Color Button
  //
  edit_color_button=new QPushButton(this);
  edit_color_button->
    setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2+55,
		sizeHint().height()-60,70,50);
  edit_color_button->setFont(buttonFont());
  edit_color_button->setText(tr("Color"));
  connect(edit_color_button,SIGNAL(clicked()),this,SLOT(colorData()));
  edit_color=palette().color(QPalette::Background);
  
  //
  // Clock Display
  //
  edit_clock_label=new QLabel(this);
  edit_clock_label->
    setGeometry(CENTER_LINE+10,10,
		sizeHint().width()-CENTER_LINE-20,sizeHint().height()-80);

  //
  //  OK Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-160,sizeHint().height()-60,70,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-80,sizeHint().height()-60,70,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  sched_rules_list = new RDSchedRulesList(clockname,rda->config());
  edit_clocks_model->setClockName(clockname);
  edit_clocks_model->load();
  edit_shortname_edit->setText(edit_clocks_model->shortName());
  if(edit_clocks_model->color().isValid()) {
    edit_color=edit_clocks_model->color();
    edit_color_button->
      setPalette(QPalette(edit_clocks_model->color(),palette().color(QPalette::Background)));
  }
  edit_remarks_edit->setText(edit_clocks_model->remarks());
  edit_modified=false;

  UpdateClock();
}


QSize EditClock::sizeHint() const
{
  return QSize(1024,698);
} 


QSizePolicy EditClock::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditClock::selectionChangedData(const QItemSelection &before,
				     const QItemSelection &after)
{
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()==0) {
    UpdateClock();
  }
  else {
    UpdateClock(rows.first().row());
  }
}


void EditClock::addData()
{
  int line=0;
  RDEventLine eventline(rda->station());
  EditEventLine *edit_eventline=
    new EditEventLine(&eventline,edit_clocks_model,-1,this);
  if(edit_eventline->exec()<0) {
    return;
  }
  delete edit_eventline;
  if(line<0) {
    line=edit_clocks_model->size();
  }
  line=edit_clocks_model->
    insert(eventline.name(),eventline.startTime(),eventline.length());
  if(line<0) {
    QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			 tr("That event does not exist."));
    return;
  }
  edit_modified=true;
  UpdateClock(line);
}


void EditClock::editData()
{
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditEventLine *edit_eventline=
    new EditEventLine(edit_clocks_model->eventLine(rows.first()),
		      edit_clocks_model,rows.first().row(),this);
  if(edit_eventline->exec()<0) {
    delete edit_eventline;
    return;
  }
  delete edit_eventline;
  edit_clocks_model->eventLine(rows.first())->load();
  edit_clocks_model->refresh(rows.first());
  edit_modified=true;
  UpdateClock(rows.first().row());
}

void EditClock::cloneData()
{
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  RDEventLine *selectedEventLine = edit_clocks_model->eventLine(rows.first());

  RDEventLine eventline(rda->station());
  eventline.setName(selectedEventLine->name());
  eventline.setStartTime(selectedEventLine->startTime().addMSecs(selectedEventLine->length()));
  eventline.setLength(selectedEventLine->length());
  if(eventline.startTime().addMSecs(eventline.length()).hour()) {
    eventline.setLength(0);
  }
  
  EditEventLine *edit_eventline=
    new EditEventLine(&eventline,edit_clocks_model,-1,this);
  if(edit_eventline->exec()<0) {
    delete edit_eventline;
    return;
  }
  delete edit_eventline;
  int line=edit_clocks_model->
    insert(eventline.name(),eventline.startTime(),eventline.length());
  if(line<0) {
    QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			 tr("That event does not exist."));
    return;
  }
  edit_clocks_model->refresh(edit_clocks_model->index(line,0));
  edit_modified=true;
  UpdateClock(line);
}


void EditClock::deleteData()
{
  QString str;
  QModelIndexList rows=edit_clocks_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  RDEventLine *el=edit_clocks_model->eventLine(rows.first());
  str=QString(tr("Are you sure you want to delete\n"));
  if(QMessageBox::
     question(this,"RDLogManager - "+tr("Delete Event"),
	      tr("Are you sure you want to delete")+
	      "\n \"("+el->startTime().toString("mm:ss.zzz").left(7)+") "+
	      el->name()+"\"?",QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  edit_clocks_model->remove(rows.first().row());
  edit_modified=true;
  rows=edit_clocks_view->selectionModel()->selectedRows();
  if(rows.size()==0) {
    UpdateClock();
  }
  else {
    UpdateClock(rows.first().row());
  }
}


void EditClock::schedRules()
{
  unsigned edit_artistsep=edit_clocks_model->getArtistSep();
  QString clock_name=edit_clocks_model->clockName();
  bool rules_modified=edit_clocks_model->getRulesModified();

  EditSchedRules *dialog=
    new EditSchedRules(clock_name,&edit_artistsep,sched_rules_list,
		       &rules_modified,this);
  dialog->exec();
  
  if(edit_clocks_model->getArtistSep()!=edit_artistsep) {
    edit_modified=true;
  }
  edit_clocks_model->setArtistSep(edit_artistsep);  

  edit_clocks_model->setRulesModified(rules_modified);
  if(rules_modified) {
    edit_modified=true;
  }

  delete dialog;
}


void EditClock::svcData()
{
  EditPerms *dialog=new EditPerms(edit_name,EditPerms::ObjectClock,this);
  dialog->exec();
  delete dialog;
}


void EditClock::saveData()
{
  if(!ValidateCode()) {
    return;
  }
  Save();
  edit_new_clock=false;
}


void EditClock::saveAsData()
{
  QString clockname=edit_name;
  QString code=edit_shortname_edit->text();

  if(!ValidateCode()) {
    return;
  }
  QString old_name=edit_name;
  if(edit_modified) {
    switch(QMessageBox::question(this,tr("Clock Modified"),
				 tr("The clock has been modified.\nDo you want to save?"),QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel)) {
	case QMessageBox::Yes:
	  Save();
	  break;

	case QMessageBox::Cancel:
	case QMessageBox::NoButton:
	  return;
	  break;
    }
  }

  AddClock *addclock=new AddClock(this);
  if(!addclock->exec(&clockname,&code)) {
    delete addclock;
    return;
  }
  delete addclock;
  edit_name=clockname;
  edit_shortname_edit->setText(code);
  edit_clocks_model->setClockName(clockname);

  Save();
  sched_rules_list->Save(clockname);
  edit_clocks_model->setRulesModified(false);
  edit_new_clocks->push_back(clockname);
  CopyClockPerms(old_name,clockname);
  if(edit_new_clock) {
    AbandonClock(old_name);
  }
  edit_clockname_label->setText(clockname);
  UpdateClock();
  setWindowTitle("RDLogManager - "+tr("Edit Clock")+": "+edit_name);
}


void EditClock::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void EditClock::colorData()
{
  QColor color=
    QColorDialog::getColor(edit_color_button->palette().color(QPalette::Background),this);
  if(color.isValid()) {
    edit_color=color;
    edit_color_button->setPalette(QPalette(color,palette().color(QPalette::Background)));
  }
}


void EditClock::editEventData(int line)
{
  QStringList new_events;
  QStringList modified_events;

  RDEventLine *event=edit_clocks_model->eventLine(line);
  if(event==NULL) {
    return;
  }
  EditEvent *dialog=
    new EditEvent(event->name(),false,&new_events,&modified_events,this);
  if(dialog->exec()<-1) {
    delete dialog;
    return;
  }
  delete dialog;
  edit_clocks_model->refresh(edit_clocks_model->index(line,0));
}


void EditClock::okData()
{
  if(!ValidateCode()) {
    return;
  }
  Save();
  done(0);
}


void EditClock::cancelData()
{
  if(edit_modified) {
    switch(QMessageBox::question(this,tr("Clock Modified"),
				 tr("The clock has been modified.\nDo you want to save?"),QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel)) {
    case QMessageBox::Yes:
      Save();
      done(0);
      break;

    case QMessageBox::No:
      done(-1);
      break;

    case QMessageBox::NoButton:
      return;
    }
  }
  else {
    done(-1);
  }
}


void EditClock::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(Qt::black));
  p->drawLine(CENTER_LINE,10,CENTER_LINE,sizeHint().height()-10);

  p->end();
}


void EditClock::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditClock::Save()
{
  edit_clocks_model->setColor(edit_color);
  edit_clocks_model->setShortName(edit_shortname_edit->text());
  edit_clocks_model->setRemarks(edit_remarks_edit->toPlainText());
  edit_clocks_model->save();
  if(edit_clocks_model->getRulesModified()) {
    sched_rules_list->Save(edit_clocks_model->clockName());
    edit_clocks_model->setRulesModified(false);
  }
  edit_modified=false;
}


void EditClock::UpdateClock(int line)
{
  QPixmap *map=new QPixmap(edit_clock_label->size());
  map->fill();
  QPainter *p=new QPainter();
  p->begin(map);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);
  p->setFont(*edit_title_font);

  //
  // Title
  //
  p->drawText((edit_clock_label->size().width()-
	       edit_title_metrics->width(edit_clocks_model->clockName()))/2,
	      50,edit_clocks_model->clockName());

  //
  // Pie Circle
  //
  p->translate(edit_clock_label->size().width()/2,
	       edit_clock_label->size().height()/2);
  p->rotate(-90.0);
  int size_x=edit_clock_label->size().width()-2*PIE_X_MARGIN;
  int size_y=edit_clock_label->size().width()-2*PIE_X_MARGIN;
  p->drawArc(-size_x/2,-size_y/2,size_x,size_y,0,5760);     

  //
  // Segments
  //
  for(int i=0;i<edit_clocks_model->size();i++) {
    if(i==line) {
      p->setBrush(edit_clocks_view->palette().
		  color(QPalette::Active,QPalette::Highlight));
      p->drawPie(-size_x/2,-size_y/2,size_x,size_y,
	  -QTime(0,0,0).secsTo(edit_clocks_model->eventLine(line)->startTime())*5760/3600,
	  -(edit_clocks_model->eventLine(line)->length()/1000)*5760/3600);
    }
    else {
      if(edit_clocks_model->eventLine(i)->color().isValid()) {
	p->setBrush(edit_clocks_model->eventLine(i)->color());
      }
      else {
	p->setBrush(palette().color(QPalette::Active,QPalette::Base));
      }
      p->drawPie(-size_x/2,-size_y/2,size_x,size_y,
	     -QTime(0,0,0).secsTo(edit_clocks_model->eventLine(i)->startTime())*5760/3600,
	     -(edit_clocks_model->eventLine(i)->length()/1000)*5760/3600);
    }
  }
  p->end();
  delete p;
  edit_clock_label->setPixmap(*map);
  delete map;

  edit_clone_button->setDisabled(line<0);
  edit_edit_button->setDisabled(line<0);
  edit_delete_button->setDisabled(line<0);
}


void EditClock::CopyClockPerms(QString old_name,QString new_name)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select `SERVICE_NAME` from `CLOCK_PERMS` where ")+
    "`CLOCK_NAME`='"+RDEscapeString(old_name)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `CLOCK_PERMS` set ")+
      "`CLOCK_NAME`='"+RDEscapeString(new_name)+"',"+
      "`SERVICE_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
    RDSqlQuery::apply(sql);
  }
  delete q;
}


void EditClock::AbandonClock(QString name)
{
  if(name==edit_name) {
    return;
  }
  QString sql=QString("delete from `CLOCKS` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
  sql=QString("delete from `CLOCK_LINES` where ")+
    "`CLOCK_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
}


bool EditClock::ValidateCode()
{
  if(edit_shortname_edit->text().isEmpty()) {
    QMessageBox::information(this,"RDLogManager - "+tr("Invalid Code"),
			     tr("You must provide a clock code!"));
    return false;
  }
  QString sql=QString("select `SHORT_NAME` from `CLOCKS` where ")+
    "(`SHORT_NAME`='"+RDEscapeString(edit_shortname_edit->text())+"')&&"+
    "(`NAME`!='"+RDEscapeString(edit_name)+"')";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->next()) {
    QMessageBox::information(this,"RDLogManager - "+tr("Duplicate Code"),
			     tr("That code is already in use!"));
    delete q;
    return false;
  }
  delete q;
  return true;
}
