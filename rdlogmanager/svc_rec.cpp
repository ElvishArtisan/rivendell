// svc_rec.cpp
//
// A Qt-based application for testing General Purpose Outputs (GPO).
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: svc_rec.cpp,v 1.8 2010/07/29 19:32:37 cvs Exp $
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


#include <qwidget.h>
#include <qstring.h>
#include <qlabel.h>
#include <qpalette.h>
#include <rddb.h>
#include <svc_rec.h>

//
// Global Classes
//
SvcRec::SvcRec(const QString &svcname,
			   QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  QString sql;
  RDSqlQuery *q;
  pick_service_name=svcname;

  //
  // Generate Fonts
  //
  pick_day_font[0]=QFont("Helvetica",12,QFont::Normal);
  pick_day_font[0].setPixelSize(12);
  pick_day_font[1]=QFont("Helvetica",12,QFont::Bold);
  pick_day_font[1].setPixelSize(12);

  //
  // Generate Date Boundaries
  //
  pick_tablename=svcname;
  pick_tablename.replace(" ","_");
  QDate current_date=QDate::currentDate();
  pick_high_year=current_date.year();
  pick_low_year=pick_high_year;
  sql=QString().sprintf("select EVENT_DATETIME from `%s_SRT`\
                         order by EVENT_DATETIME",
			(const char *)pick_tablename);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    pick_low_year=q->value(0).toDate().year();
  }
  delete q;

  //
  // Month
  //
  pick_month_box=new QComboBox(this,"pick_month_box");
  pick_month_box->setGeometry(0,0,120,26);
  for(int i=1;i<13;i++) {
    pick_month_box->insertItem(QDate::longMonthName(i));
  }
  connect(pick_month_box,SIGNAL(activated(int)),
	  this,SLOT(monthActivatedData(int)));

  //
  // Year
  //
  pick_year_box=new QComboBox(this,"pick_year_box");
  pick_year_box->setGeometry(130,0,90,26);
  for(int i=pick_low_year;i<(pick_high_year+1);i++) {
    pick_year_box->insertItem(QString().sprintf("%04d",i));
  }
  connect(pick_year_box,SIGNAL(activated(int)),
	  this,SLOT(yearActivatedData(int)));
  pick_year_spin=NULL;

  //
  // Date Labels
  //
  QPalette weekend_palette=palette();
  weekend_palette.setColor(QPalette::Active,QColorGroup::Background,
			   palette().color(QPalette::Active,
					   QColorGroup::Mid));
  weekend_palette.setColor(QPalette::Inactive,QColorGroup::Background,
			   palette().color(QPalette::Active,
					   QColorGroup::Mid));

  QLabel *label=new QLabel(tr("Mo"),this,"monday_label");
  label->setGeometry(SVC_REC_X_ORIGIN,30,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("Tu"),this,"tuesday_label");
  label->setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL,
		     SVC_REC_Y_ORIGIN,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("We"),this,"wednesday_label");
  label->setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*2,
		     SVC_REC_Y_ORIGIN,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("Th"),this,"thursday_label");
  label->setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*3,
		     SVC_REC_Y_ORIGIN,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("Fr"),this,"friday_label");
  label->setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*4,
		     SVC_REC_Y_ORIGIN,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("Sa"),this,"saturday_label");
  label->setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*5,
		     SVC_REC_Y_ORIGIN,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);
  label->setPalette(weekend_palette);

  label=new QLabel(tr("Su"),this,"sunday_label");
  label->setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*6,
		     SVC_REC_Y_ORIGIN,30,30);
  label->setFont(pick_day_font[1]);
  label->setAlignment(AlignCenter);
  label->setPalette(weekend_palette);


  for(int i=0;i<6;i++) {
    for(int j=0;j<7;j++) {
      pick_date_label[i][j]=new QLabel(this,"date_label");
      pick_date_label[i][j]->
	setGeometry(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*j,
		    SVC_REC_Y_ORIGIN+20+SVC_REC_Y_INTERVAL*i,30,30);
      pick_date_label[i][j]->setAlignment(AlignCenter);
    }
  }
  PrintDays();
  setDate(current_date);
}


SvcRec::~SvcRec()
{
}


QSize SvcRec::sizeHint() const
{
  return QSize(220,175);
}


QSizePolicy SvcRec::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


QString SvcRec::serviceName() const
{
  return pick_service_name;
}


QDate SvcRec::date() const
{
  return pick_date;
}


bool SvcRec::setDate(QDate date)
{
  if(!date.isValid()) {
    date=QDate::currentDate();
  }
  if((date.year()<pick_low_year)||(date.year()>pick_high_year)) {
    return false;
  }
  pick_date=date;
  pick_month_box->setCurrentItem(date.month()-1);
  if(pick_year_box!=NULL) {
    pick_year_box->setCurrentItem(date.year()-pick_low_year);
  }
  else {
    pick_year_spin->setValue(date.year());
  }
  GetActiveDays(date);
  PrintDays();
  return true;
}


bool SvcRec::dayActive(int day) const
{
  return pick_active_days[day-1];
}


void SvcRec::deleteDay()
{
  QString sql;
  RDSqlQuery *q;

  QString tablename=pick_service_name;
  tablename.replace(" ","_");
  sql=QString().sprintf("delete from `%s_SRT` where \
                         (EVENT_DATETIME>=\"%s\")&&\
                         (EVENT_DATETIME<=\"%s\")",
			(const char *)tablename,
			(const char *)date().toString("yyyy-MM-dd 00:00:00"),
			(const char *)date().toString("yyyy-MM-dd 23:59:59"));
  q=new RDSqlQuery(sql);
  delete q;
  GetActiveDays(pick_date);
  PrintDays();
}


void SvcRec::monthActivatedData(int id)
{
  QDate date=QDate(pick_date.year(),id+1,1);
  if(pick_date.day()<=date.daysInMonth()) {
    pick_date=QDate(pick_date.year(),id+1,pick_date.day());
  }
  else {
    pick_date=QDate(pick_date.year(),id+1,date.daysInMonth());
  }
  GetActiveDays(pick_date);
  PrintDays();
}


void SvcRec::yearActivatedData(int id)
{
  QDate date=QDate(pick_low_year+pick_year_box->currentItem(),
		   pick_date.month(),1);
  if(pick_date.day()<=date.daysInMonth()) {
    pick_date=QDate(pick_low_year+pick_year_box->currentItem(),
		    pick_date.month(),pick_date.day());
  }
  else {
    pick_date=QDate(pick_low_year+pick_year_box->currentItem(),
		    pick_date.month(),date.daysInMonth());
  }
  GetActiveDays(pick_date);
  PrintDays();
}


void SvcRec::yearChangedData(int year)
{
  QDate date=QDate(pick_year_spin->value(),pick_date.month(),1);
  if(pick_date.day()<=date.daysInMonth()) {
    pick_date=QDate(pick_year_spin->value(),
		    pick_date.month(),pick_date.day());
  }
  else {
    pick_date=QDate(pick_year_spin->value(),
		    pick_date.month(),date.daysInMonth());
  }
  GetActiveDays(pick_date);
  PrintDays();
}


void SvcRec::mousePressEvent(QMouseEvent *e)
{
  if((e->pos().x()<SVC_REC_X_ORIGIN)||
     (e->pos().x()>(SVC_REC_X_ORIGIN+SVC_REC_X_INTERVAL*7))||
     (e->pos().y()<SVC_REC_Y_ORIGIN)||
     (e->pos().y()>(SVC_REC_Y_ORIGIN+SVC_REC_Y_INTERVAL*7))) {
    QWidget::mousePressEvent(e);
    return;
  }
  int dow=(e->pos().x()-SVC_REC_X_ORIGIN)/SVC_REC_X_INTERVAL;
  int week=(e->pos().y()-SVC_REC_Y_ORIGIN)/SVC_REC_Y_INTERVAL-1;
  if((dow<0)||(dow>6)||(week<0)||(week>6)) {
    return;
  }
  if(pick_date_label[week][dow]->text().isEmpty()) {
    return;
  }
  pick_date=QDate(pick_date.year(),pick_date.month(),
		  pick_date_label[week][dow]->text().toInt());
  PrintDays();
  emit dateSelected(pick_date,pick_active_days[pick_date.day()-1]);
}


void SvcRec::PrintDays()
{
  QDate top_date;

  //
  // Clear Days
  //
  QPalette weekend_palette=palette();
  weekend_palette.setColor(QPalette::Active,QColorGroup::Background,
			   palette().color(QPalette::Active,
					   QColorGroup::Mid));
  weekend_palette.setColor(QPalette::Inactive,QColorGroup::Background,
			   palette().color(QPalette::Active,
					   QColorGroup::Mid));
  for(int i=0;i<6;i++) {
    for(int j=0;j<5;j++) {
      pick_date_label[i][j]->clear();
      pick_date_label[i][j]->setPalette(palette());
    }
  }
  for(int i=0;i<6;i++) {
    for(int j=5;j<7;j++) {
      pick_date_label[i][j]->clear();
      pick_date_label[i][j]->setPalette(weekend_palette);
    }
  }

  //
  // Get Top of Month
  //
  if(pick_year_box!=NULL) {
    top_date=QDate(pick_low_year+pick_year_box->currentItem(),
		   pick_month_box->currentItem()+1,1);
  }
  else {
    top_date=QDate(pick_year_spin->value(),pick_month_box->currentItem()+1,1);
  }

  //
  // Print Days
  //
  int dow_offset=top_date.dayOfWeek()-1;
  for(int i=1;i<(top_date.daysInMonth()+1);i++) {
    PrintDay(i,dow_offset);
    if(pick_date.day()==i) {
      SelectDay(i,dow_offset,true);
    }
  }
}


void SvcRec::PrintDay(int day,int dow_offset)
{
  int slot=day+dow_offset-1;
  int week=slot/7;
  int dow=slot-7*week;
  pick_date_label[week][dow]->setText(QString().sprintf("%d",day));
  if(pick_active_days[day-1]) {
    pick_date_label[week][dow]->setFont(pick_day_font[1]);
  }
  else {
    pick_date_label[week][dow]->setFont(pick_day_font[0]);
  }
}


void SvcRec::SelectDay(int day,int dow_offset,bool state)
{
  int slot=day+dow_offset-1;
  int week=slot/7;
  int dow=slot-7*week;
  QPalette pal=palette();
  if(state) {
    pal.setColor(QPalette::Active,QColorGroup::Foreground,
		 palette().
		 color(QPalette::Active,QColorGroup::HighlightedText));
    pal.setColor(QPalette::Active,QColorGroup::Background,
		 palette().color(QPalette::Active,QColorGroup::Highlight));
    pal.setColor(QPalette::Inactive,QColorGroup::Foreground,
		 palette().
		 color(QPalette::Active,QColorGroup::HighlightedText));
    pal.setColor(QPalette::Inactive,QColorGroup::Background,
		 palette().color(QPalette::Active,QColorGroup::Highlight));
  }
  else {
    pal.setColor(QPalette::Active,QColorGroup::Foreground,
		 palette().color(QPalette::Active,QColorGroup::Text));
    pal.setColor(QPalette::Active,QColorGroup::Background,
		 palette().color(QPalette::Active,QColorGroup::Background));
    pal.setColor(QPalette::Inactive,QColorGroup::Foreground,
		 palette().color(QPalette::Active,QColorGroup::Text));
    pal.setColor(QPalette::Inactive,QColorGroup::Background,
		 palette().color(QPalette::Active,QColorGroup::Background));
  }
  pick_date_label[week][dow]->setPalette(pal);
}


void SvcRec::GetActiveDays(const QDate &date)
{
  QString sql;
  RDSqlQuery *q;

  for(int i=0;i<=31;i++) {
    sql=QString().sprintf("select ID from `%s_SRT` where \
                           (EVENT_DATETIME>=\"%s-%02d 00:00:00\")&&\
                           (EVENT_DATETIME<=\"%s-%02d 23:59:59\")",
			  (const char *)pick_tablename,
			  (const char *)date.toString("yyyy-MM"),i+1,
			  (const char *)date.toString("yyyy-MM"),i+1);
    q=new RDSqlQuery(sql);
    pick_active_days[i]=q->first();
    delete q;
  }
}

