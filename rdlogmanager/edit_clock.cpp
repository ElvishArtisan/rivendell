// edit_clock.cpp
//
// Edit Rivendell Log Clock
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_clock.cpp,v 1.27.10.1 2014/06/24 18:27:05 cvs Exp $
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qcolordialog.h>
#include <qspinbox.h>
#include <qcombobox.h>

#include <rddb.h>
#include <rd.h>
#include <rdconf.h>
#include <rdevent.h>
#include <rdcreate_log.h>
#include <rdescape_string.h>

#include <edit_clock.h>
#include <add_clock.h>
#include <edit_eventline.h>
#include <edit_perms.h>
#include <edit_event.h>
#include <list_clocks.h>
#include <edit_schedrules.h>


EditClock::EditClock(QString clockname,bool new_clock,
		     std::vector<QString> *new_clocks,
		     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString str;

  str=QString(tr("Edit Clock:"));
  setCaption(QString().sprintf("%s %s",(const char *)str,
			       (const char *)clockname));
  edit_name=clockname;
  edit_new_clock=new_clock;
  edit_new_clocks=new_clocks;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  edit_title_font=new QFont("Helvetica",24,QFont::Bold);
  edit_title_font->setPixelSize(24);
  edit_title_metrics=new QFontMetrics(*edit_title_font);

  //
  // Clock Names
  //
  edit_clockname_label=new QLabel(clockname,this,"edit_clockname_label");
  edit_clockname_label->setGeometry(10,10,280,20);
  edit_clockname_label->setFont(bold_font);
  edit_shortname_edit=new QLineEdit(this,"edit_shortname_edit");
  edit_shortname_edit->setGeometry(350,10,40,20);
  edit_shortname_edit->setMaxLength(3);
  QLabel *label=new QLabel(edit_shortname_edit,tr("Code:"),
			   this,"edit_shortname_label");
  label->setGeometry(295,10,50,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Clock List
  //
  edit_clocks_list=new ClockListView(this,"edit_clocks_list");
  edit_clocks_list->setGeometry(10,35,CENTER_LINE-20,sizeHint().height()-250);
  edit_clocks_list->setAllColumnsShowFocus(true);
  edit_clocks_list->setItemMargin(5);
  edit_clocks_list->addColumn(tr("Start"));
  edit_clocks_list->addColumn(tr("End"));
  edit_clocks_list->addColumn(tr("Event"));
  edit_clocks_list->addColumn(tr("Length"));
  edit_clocks_list->setColumnAlignment(3,AlignRight);
  edit_clocks_list->addColumn(tr("Count"));
  edit_clocks_list->setColumnAlignment(4,AlignCenter);
  connect(edit_clocks_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));
  connect(edit_clocks_list,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(selectionChangedData(QListViewItem *)));
  connect(edit_clocks_list,SIGNAL(editLine(int)),
	  this,SLOT(editEventData(int)));

  //
  //  Add Button
  //
  QPushButton *button=new QPushButton(this,"add_button");
  button->setGeometry(70,sizeHint().height()-210,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Add"));
  connect(button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  button=new QPushButton(this,"edit_button");
  button->setGeometry(160,sizeHint().height()-210,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Edit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  button=new QPushButton(this,"delete_button");
  button->setGeometry(250,sizeHint().height()-210,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Delete"));
  connect(button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Remarks
  //
  edit_remarks_edit=new QTextEdit(this,"edit_remarks_edit");
  edit_remarks_edit->setGeometry(10,sizeHint().height()-140,CENTER_LINE-20,130);
  edit_remarks_edit->setTextFormat(QTextEdit::PlainText);
  label=new QLabel(edit_remarks_edit,tr("Remarks"),this,"edit_remarks_label");
  label->setGeometry(15,sizeHint().height()-155,CENTER_LINE-20,15);
  label->setFont(bold_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  //  Scheduler-Rules button
  //
  button=new QPushButton(this,"schedrules_button");
  button->setGeometry(CENTER_LINE+20,sizeHint().height()-60,70,50);
  button->setFont(bold_font);
  button->setText(tr("Scheduler\nRules"));
  connect(button,SIGNAL(clicked()),this,SLOT(schedRules()));

  //
  //  Save Button
  //
  button=new QPushButton(this,"save_button");
  button->setGeometry(CENTER_LINE+110,sizeHint().height()-60,70,50);
  button->setFont(bold_font);
  button->setText(tr("&Save"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  button=new QPushButton(this,"saveas_button");
  button->setGeometry(CENTER_LINE+190,sizeHint().height()-60,70,50);
  button->setFont(bold_font);
  button->setText(tr("Save &As"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveAsData()));

  //
  //  Service Associations Button
  //
  button=new QPushButton(this,"svc_button");
  button->setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2-25,
		      sizeHint().height()-60,70,50);
  button->setFont(bold_font);
  button->setText(tr("&Services\nList"));
  connect(button,SIGNAL(clicked()),this,SLOT(svcData()));

  //
  //  Color Button
  //
  edit_color_button=new QPushButton(this,"edit_color_button");
  edit_color_button->
    setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2+55,
		sizeHint().height()-60,70,50);
  edit_color_button->setFont(bold_font);
  edit_color_button->setText(tr("Colo&r"));
  connect(edit_color_button,SIGNAL(clicked()),this,SLOT(colorData()));

  //
  // Clock Display
  //
  edit_clock_label=new QLabel(this,"edit_clock_label");
  edit_clock_label->
    setGeometry(CENTER_LINE+10,10,
		sizeHint().width()-CENTER_LINE-20,sizeHint().height()-80);

  //
  //  OK Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-160,sizeHint().height()-60,70,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-80,sizeHint().height()-60,70,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  sched_rules_list = new SchedRulesList(clockname);
  edit_clock=new RDClock();
  edit_clock->setName(clockname);
  edit_clock->load();
  edit_shortname_edit->setText(edit_clock->shortName());
  if(edit_clock->color().isValid()) {
    edit_color_button->
      setPalette(QPalette(edit_clock->color(),backgroundColor()));
  }
  edit_remarks_edit->setText(edit_clock->remarks());
  edit_modified=false;
  RefreshList();
}


QSize EditClock::sizeHint() const
{
  return QSize(1024,698);
} 


QSizePolicy EditClock::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditClock::selectionChangedData(QListViewItem *l)
{
  if(l==NULL) {
    UpdateClock();
    return;
  }
  RDListViewItem *item=(RDListViewItem *)l;
  if(item->text(3).isEmpty()) {
    UpdateClock();
    return;
  }
  UpdateClock(item->text(4).toInt());
}


void EditClock::addData()
{
  int line=0;
  RDEventLine eventline;

  RDListViewItem *item=(RDListViewItem *)edit_clocks_list->selectedItem();
  if(item!=NULL) {
    if(item->text(4).isEmpty()) {
      line=edit_clock->size();
    }
    else {
      line=item->text(4).toInt();
    }
  }
  EditEventLine *edit_eventline=new EditEventLine(&eventline,edit_clock,-1,
						  this,"edit_eventline");
  if(edit_eventline->exec()<0) {
    return;
  }
  delete edit_eventline;
  edit_clock->insert(eventline.name(),line);
  edit_clock->eventLine(line)->setStartTime(eventline.startTime());
  edit_clock->eventLine(line)->setLength(eventline.length());
  edit_clock->eventLine(line)->load();
  edit_modified=true;
  RefreshList(line);
}


void EditClock::editData()
{
  RDListViewItem *item=(RDListViewItem *)edit_clocks_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if(item->text(4).isEmpty()) {
    return;
  }
  int line=item->text(4).toInt();
  line=item->text(4).toInt();
  EditEventLine *edit_eventline=new EditEventLine(edit_clock->eventLine(line),
						  edit_clock,line,
						  this,"edit_eventline");
  if(edit_eventline->exec()<0) {
    delete edit_eventline;
    return;
  }
  delete edit_eventline;
  edit_clock->eventLine(line)->load();
  edit_modified=true;
  RefreshList();
}


void EditClock::deleteData()
{
  QString str;

  RDListViewItem *item=(RDListViewItem *)edit_clocks_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if(item->text(4).isEmpty()) {
    return;
  }
  str=QString(tr("Are you sure you want to\ndelete"));
  if(QMessageBox::question(this,tr("Delete Event"),
			   QString().sprintf("%s \'%s\'?",(const char *)str,
			 (const char *)item->text(3)),
			  QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  edit_clock->remove(item->text(4).toInt());
  edit_modified=true;
  RefreshList();
}


void EditClock::schedRules()
{
  unsigned edit_artistsep = edit_clock->getArtistSep();
  QString clock_name = edit_clock->name();
  bool rules_modified = edit_clock->getRulesModified();

  EditSchedRules *dialog=new EditSchedRules(clock_name,&edit_artistsep,sched_rules_list,&rules_modified,this,"dialog");
  dialog->exec();
  
  if (edit_clock->getArtistSep()!=edit_artistsep)
    {
    edit_modified=true;
    }
  edit_clock->setArtistSep(edit_artistsep);  

  edit_clock->setRulesModified(rules_modified);
  if(rules_modified)
    {
    edit_modified=true;
    }

  delete dialog;
}


void EditClock::svcData()
{
  EditPerms *dialog=new EditPerms(edit_name,EditPerms::ObjectClock,
				  this,"dialog");
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
  QString sql;
  RDSqlQuery *q;
  QString str;

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
  if(edit_shortname_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Missing Clock Code"),
			 tr("You must specify a clock code!"));
    return;
  }
  sql=
    QString().sprintf("select SHORT_NAME from CLOCKS where SHORT_NAME=\"%s\"",
		      (const char *)
		      RDEscapeString(edit_shortname_edit->text()));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QMessageBox::warning(this,tr("Code Exists"),
			 tr("The clock code is already in use!"));
    delete q;
    return;
  }
  delete q;

  AddClock *addclock=new AddClock(&clockname,this,"addclock");
  if(addclock->exec()<0) {
    delete addclock;
    return;
  }
  delete addclock;
  edit_name=clockname;
  sql=QString().sprintf("select NAME from CLOCKS where NAME=\"%s\"",
			(const char *)clockname);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(QMessageBox::question(this,tr("Clock Exists"),
			     tr("Clock already exists!  Overwrite?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete q;
      return;
    }
  }
  delete q;
  edit_clock->setName(clockname);
  sql=RDCreateClockTableSql(RDClock::tableName(clockname));
  q=new RDSqlQuery(sql);
  delete q;


  Save();
  edit_new_clocks->push_back(clockname);
  CopyClockPerms(old_name,clockname);
  if(edit_new_clock) {
    AbandonClock(old_name);
  }
  edit_clockname_label->setText(clockname);
  UpdateClock();
  str=QString(tr("Edit Clock:"));
  setCaption(QString().sprintf("%s %s",(const char *)str,
			       (const char *)edit_name));
}


void EditClock::doubleClickedData(QListViewItem *item,const QPoint &,int)
{
  editData();
}


void EditClock::colorData()
{
  QColor color=QColorDialog::getColor(edit_color_button->backgroundColor(),
				      this,"color_dialog");
  if(color.isValid()) {
    edit_color_button->setPalette(QPalette(color,backgroundColor()));
  }
}


void EditClock::editEventData(int line)
{
  std::vector<QString> new_events;

  RDEventLine *event=edit_clock->eventLine(line);
  if(event==NULL) {
    return;
  }
  EditEvent *dialog=new EditEvent(event->name(),false,&new_events,
				  this,"dialog");
  if(dialog->exec()<-1) {
    delete dialog;
    return;
  }
  delete dialog;
  RefreshNames();
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
  p->setPen(QColor(black));
  p->moveTo(CENTER_LINE,10);
  p->lineTo(CENTER_LINE,sizeHint().height()-10);

  p->end();
}


void EditClock::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditClock::Save()
{
  edit_clock->setColor(edit_color_button->backgroundColor());
  edit_clock->setShortName(edit_shortname_edit->text());
  edit_clock->setRemarks(edit_remarks_edit->text());
  edit_clock->save();
  if(edit_clock->getRulesModified())
     {
     sched_rules_list->Save(edit_clock->name());
     edit_clock->setRulesModified(false);
     }
  edit_modified=false;
}


void EditClock::RefreshList(int select_line)
{
  UpdateClock();
  RDListViewItem *prev_item=(RDListViewItem *)edit_clocks_list->selectedItem();

  if((prev_item!=NULL)&&(select_line>=0)) {
    select_line=prev_item->text(4).toInt();
  }
  RDListViewItem *item;
  RDEventLine *eventline;

  edit_clocks_list->clear();
  item=new RDListViewItem(edit_clocks_list);
  item->setText(2,tr("--- End of clock ---"));
  for(int i=edit_clock->size()-1;i>=0;i--) {
    if((eventline=edit_clock->eventLine(i))!=NULL) {
      item=new RDListViewItem(edit_clocks_list);
      item->setText(0,eventline->startTime().toString("mm:ss.zzz").left(7));
      item->setText(1,eventline->startTime().
		    addMSecs(eventline->length()).toString("mm:ss.zzz").
		    left(7));
      item->setText(3,RDGetTimeLength(eventline->length(),false,true));
      item->setText(2,QString().sprintf("%s [%s]",
		    (const char *)eventline->name(),
		    (const char *)eventline->properties()));
      item->setText(4,QString().sprintf("%d",i));
      if(eventline->color().isValid()) {
	item->setBackgroundColor(eventline->color());
      }
      if(i==select_line) {
	edit_clocks_list->setSelected(item,true);
      }
    }
  }
}


void EditClock::RefreshNames()
{
  QString sql;
  RDSqlQuery *q;
  RDEventLine *eventline=NULL;
  RDListViewItem *item=(RDListViewItem *)edit_clocks_list->firstChild();
  while(item!=NULL) {
    if(!item->text(4).isEmpty()) {
      if((eventline=edit_clock->eventLine(item->text(4).toInt()))!=NULL) {
	sql=QString().sprintf("select PROPERTIES from EVENTS\
                               where NAME=\"%s\"",
			      (const char *)RDEscapeString(eventline->name()));
	q=new RDSqlQuery(sql);
	if(q->next()) {
	  item->
	    setText(2,QString().sprintf("%s [%s]",
					(const char *)eventline->name(),
					(const char *)q->value(0).toString()));
	}
	delete q;
      }
    }
    item=(RDListViewItem *)item->nextSibling();
  }
}


void EditClock::UpdateClock(int line)
{
  QPixmap *map=new QPixmap(edit_clock_label->size());
  map->fill();
  QPainter *p=new QPainter();
  p->begin(map);
  p->setPen(QColor(black));
  p->setBrush(QColor(black));
  p->setFont(*edit_title_font);

  //
  // Title
  //
  p->drawText((edit_clock_label->size().width()-
	       edit_title_metrics->width(edit_clock->name()))/2,
	      50,edit_clock->name());

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
  for(int i=0;i<edit_clock->size();i++) {
    if(i==line) {
      p->setBrush(edit_clocks_list->palette().
		  color(QPalette::Active,QColorGroup::Highlight));
      p->drawPie(-size_x/2,-size_y/2,size_x,size_y,
	  -QTime().secsTo(edit_clock->eventLine(line)->startTime())*5760/3600,
	  -(edit_clock->eventLine(line)->length()/1000)*5760/3600);
    }
    else {
      if(edit_clock->eventLine(i)->color().isValid()) {
	p->setBrush(edit_clock->eventLine(i)->color());
      }
      else {
	p->setBrush(palette().color(QPalette::Active,QColorGroup::Base));
      }
      p->drawPie(-size_x/2,-size_y/2,size_x,size_y,
	     -QTime().secsTo(edit_clock->eventLine(i)->startTime())*5760/3600,
	     -(edit_clock->eventLine(i)->length()/1000)*5760/3600);
    }
  }
  p->end();
  delete p;
  edit_clock_label->setPixmap(*map);
  delete map;
}


void EditClock::CopyClockPerms(QString old_name,QString new_name)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select SERVICE_NAME from CLOCK_PERMS where\
                         CLOCK_NAME=\"%s\"",
			(const char *)RDEscapeString(old_name));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("insert into CLOCK_PERMS set\
                          CLOCK_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			  (const char *)RDEscapeString(new_name),
			  (const char *)RDEscapeString(q->value(0).toString()));
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}


void EditClock::AbandonClock(QString name)
{
  if(name==edit_name) {
    return;
  }
  QString sql=QString().sprintf("delete from CLOCKS where NAME=\"%s\"",
				(const char *)name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;

  sql=QString("drop table `")+RDClock::tableName(name)+"`";
  q=new RDSqlQuery(sql);
  delete q;
}


bool EditClock::ValidateCode()
{
  if(edit_shortname_edit->text().isEmpty()) {
    QMessageBox::information(this,tr("Invalid Code"),
			     tr("You must provide a clock code!"));
    return false;
  }
  RDSqlQuery *q=
    new RDSqlQuery(QString().
		  sprintf("select SHORT_NAME from CLOCKS\
                                     where (SHORT_NAME=\"%s\")&&\
                                     (NAME!=\"%s\")",
			  (const char *)
			  RDEscapeString(edit_shortname_edit->text()),
			  (const char *)RDEscapeString(edit_name)));
  if(q->next()) {
    QMessageBox::information(this,tr("Duplicate Code"),
			     tr("That code is already in use!"));
    delete q;
    return false;
  }
  delete q;

  return true;
}
