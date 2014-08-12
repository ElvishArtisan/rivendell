// edit_grid.cpp
//
// Edit Rivendell Log Grid
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_grid.cpp,v 1.12.8.2 2014/01/21 21:28:34 cvs Exp $
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
#include <qsignalmapper.h>

#include <rddb.h>
#include <rd.h>
#include <rdevent.h>
#include <rdcreate_log.h>

#include <edit_grid.h>
#include <list_clocks.h>
#include <edit_clock.h>


EditGrid::EditGrid(QString servicename,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  QString str;

  str=QString(tr("Edit Grid:"));
  setCaption(QString().sprintf("%s %s",(const char *)str,
			       (const char *)servicename));
  edit_servicename=servicename;

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
  QFont button_font("Helvetica",10,QFont::Normal);
  button_font.setPixelSize(10);

  //
  // Hour Buttons
  //
  QLabel *label;
  QSignalMapper *mapper=new QSignalMapper(this,"hour_button_mapper");
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(hourButtonData(int)));
  for(int i=0;i<5;i++) {
    label=new QLabel(QDate::longDayName(i+1),this,"day_label");
    label->setGeometry(20,14+75*i,90,16);
    label->setFont(bold_font);
    label->setAlignment(AlignCenter);
    for(int j=0;j<24;j++) {
      edit_hour_button[i][j]=new RDPushButton(this,"hour_button");
      edit_hour_button[i][j]->setGeometry(10+42*j,30+75*i,42,40);
      edit_hour_button[i][j]->setFont(button_font);
      edit_hour_button[i][j]->setId(24*i+j);
      LabelButton(i+1,j,"---");
      mapper->setMapping(edit_hour_button[i][j],24*i+j);
      connect(edit_hour_button[i][j],SIGNAL(clicked()),mapper,SLOT(map()));
      connect(edit_hour_button[i][j],SIGNAL(rightClicked(int,const QPoint &)),
	      this,SLOT(rightHourButtonData(int,const QPoint &)));
    }
  }
  for(int i=5;i<7;i++) {
    label=new QLabel(QDate::longDayName(i+1),this,"day_label");
    label->setGeometry(20,44+75*i,90,16);
    label->setFont(bold_font);
    label->setAlignment(AlignCenter);
    for(int j=0;j<24;j++) {
      edit_hour_button[i][j]=new RDPushButton(this,"hour_button");
      edit_hour_button[i][j]->setGeometry(10+42*j,60+75*i,42,40);
      edit_hour_button[i][j]->setFont(button_font);
      edit_hour_button[i][j]->setId(24*i+j);
      LabelButton(i+1,j,"---");
      mapper->setMapping(edit_hour_button[i][j],24*i+j);
      connect(edit_hour_button[i][j],SIGNAL(clicked()),mapper,SLOT(map()));
      connect(edit_hour_button[i][j],SIGNAL(rightClicked(int,const QPoint &)),
	      this,SLOT(rightHourButtonData(int,const QPoint &)));
    }
  }

  //
  // Right Button Menu
  //
  edit_right_menu=new QPopupMenu(this,"edit_right_menu");
  connect(edit_right_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
  edit_right_menu->
    insertItem(tr("Edit Clock"),this,SLOT(editClockData()),0,0);
  edit_right_menu->
    insertItem(tr("Clear Hour"),this,SLOT(clearHourData()),0,1);

  //
  // Change All Button
  //
  QPushButton *all_button=new QPushButton(this,"change_all_button");
  all_button->setGeometry(10,sizeHint().height()-60,80,50);
  all_button->setDefault(false);
  all_button->setFont(bold_font);
  all_button->setText(tr("Change\n&All"));
  connect(all_button,SIGNAL(clicked()),this,SLOT(allHourButtonData()));
                      
  //
  //  Close Button
  //
  QPushButton *button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Load Buttons
  //
  LoadButtons();
}


QSize EditGrid::sizeHint() const
{
  return QSize(1024,638);
} 


QSizePolicy EditGrid::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditGrid::hourButtonData(int id)
{
  int dayofweek=id/24+1;
  int hour=id-24*(dayofweek-1);
  QString clockname=GetClock(dayofweek,hour);
  if(clockname.isEmpty()) {
    clockname=current_clockname;
  }
  ListClocks *listclocks=new ListClocks(&clockname,this,"listclocks");
  if(listclocks->exec()<0) {
    delete listclocks;
    return;
  }
  delete listclocks;
  current_clockname=clockname;
  QString sql=QString().sprintf("update SERVICES set CLOCK%d=\"%s\"\
                                 where NAME=\"%s\"",
				id,(const char *)clockname,
				(const char *)edit_servicename);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  LabelButton(dayofweek,hour,clockname);
}


void EditGrid::allHourButtonData()
{
  QString clockname="";
  ListClocks *listclocks=new ListClocks(&clockname,this,"listclocks");
  if(listclocks->exec()<0) {
    delete listclocks;
    return;
  }
  delete listclocks;
  if(QMessageBox::question(this,"RDLogManager - "+tr("Clear Clocks"),
			   tr("Are you sure you want to update ALL clocks in the grid?")+"\n"+tr("This operation cannot be undone!"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
    for(int id=0;id<168;id++) {
      QString sql=QString().sprintf("update SERVICES set CLOCK%d=\"%s\"\
                                   where NAME=\"%s\"",
				    id,(const char *)clockname,
				    (const char *)edit_servicename);
      RDSqlQuery *q=new RDSqlQuery(sql);
      delete q;
      int dayofweek=id/24+1;
      int hour=id-24*(dayofweek-1);
      LabelButton(dayofweek,hour,clockname);
    }
  }
}


void EditGrid::rightHourButtonData(int id,const QPoint &pt)
{
  edit_rightclick_id=id;
  int dayofweek=edit_rightclick_id/24+1;
  int hour=edit_rightclick_id-24*(dayofweek-1);
  edit_right_menu->
    setGeometry(edit_hour_button[dayofweek-1][hour]->geometry().x()+
		geometry().x()+pt.x()+2,
		edit_hour_button[dayofweek-1][hour]->geometry().y()+
		geometry().y()+pt.y(),
		edit_right_menu->sizeHint().width(),
		edit_right_menu->sizeHint().height());
  edit_right_menu->exec();
}


void EditGrid::aboutToShowData()
{
  int dayofweek=edit_rightclick_id/24+1;
  int hour=edit_rightclick_id-24*(dayofweek-1);
  edit_right_menu->setItemEnabled(0,!GetClock(dayofweek,hour).isEmpty());
  edit_right_menu->setItemEnabled(1,!GetClock(dayofweek,hour).isEmpty());
}


void EditGrid::editClockData()
{
  std::vector<QString> new_clocks;

  int dayofweek=edit_rightclick_id/24+1;
  int hour=edit_rightclick_id-24*(dayofweek-1);
  QString clockname=GetClock(dayofweek,hour);
  if(clockname.isEmpty()) {
    return;
  }
  EditClock *dialog=new EditClock(clockname,false,&new_clocks,this,"dialog");
  if(dialog->exec()<0) {
    delete dialog;
    return;
  }
  delete dialog;
  LoadButtons();
}


void EditGrid::clearHourData()
{
  int dayofweek=edit_rightclick_id/24+1;
  int hour=edit_rightclick_id-24*(dayofweek-1);
  QString sql=QString().sprintf("update SERVICES set CLOCK%d=\"\"\
                                 where NAME=\"%s\"",
				edit_rightclick_id,
				(const char *)edit_servicename);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  LabelButton(dayofweek,hour,"");
}


void EditGrid::closeData()
{
  done(-1);
}


void EditGrid::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  for(int i=0;i<5;i++) {
    p->drawRect(5,21+75*i,sizeHint().width()-10,55);
  }
  for(int i=5;i<7;i++) {
    p->drawRect(5,51+75*i,sizeHint().width()-10,55);
  }
  p->end();
}


void EditGrid::closeEvent(QCloseEvent *e)
{
  closeData();
}


void EditGrid::LoadButtons()
{
  for(int i=0;i<7;i++) {
    for(int j=0;j<24;j++) {
      LabelButton(i+1,j,GetClock(i+1,j));
    }
  }
}


void EditGrid::LabelButton(int dayofweek,int hour,QString clockname)
{
  QString code=QString("---");
  QColor color=backgroundColor();

  QString sql=QString().sprintf("select SHORT_NAME,COLOR from CLOCKS\
                                 where NAME=\"%s\"",
				(const char *)clockname);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    code=q->value(0).toString();
    if(!q->value(1).isNull()) {
      color=QColor(q->value(1).toString());
    }
  }
  edit_hour_button[dayofweek-1][hour]->
    setText(QString().sprintf("%02d-%02d\n%s",hour,hour+1,(const char *)code));
  edit_hour_button[dayofweek-1][hour]->
    setPalette(QPalette(color,backgroundColor()));
}


QString EditGrid::GetClock(int dayofweek,int hour)
{
  QString sql=QString().sprintf("select CLOCK%d from SERVICES where\
                                 NAME=\"%s\"",
				24*(dayofweek-1)+hour,
				(const char *)edit_servicename);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    return q->value(0).toString();
  }
  return QString();
}

