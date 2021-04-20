// edit_grid.cpp
//
// Edit Rivendell Log Grid
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
#include <QPainter>
#include <QSignalMapper>

#include <rdescape_string.h>

#include "edit_clock.h"
#include "edit_grid.h"
#include "list_clocks.h"

EditGrid::EditGrid(QString servicename,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  edit_servicename=servicename;

  setWindowTitle("RDLogManager - "+tr("Edit Grid")+": "+servicename);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Hour Buttons
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(hourButtonData(int)));
  for(int i=0;i<5;i++) {
    edit_day_boxes[i]=new QGroupBox(QDate::longDayName(i+1),this);
    edit_day_boxes[i]->setFont(labelFont());
    edit_day_boxes[i]->setGeometry(5,11+75*i,sizeHint().width()-5,65);
    for(int j=0;j<24;j++) {
      edit_hour_button[i][j]=new RDPushButton(this);
      edit_hour_button[i][j]->setGeometry(10+42*j,30+75*i,42,40);
      edit_hour_button[i][j]->setFont(subButtonFont());
      edit_hour_button[i][j]->setId(24*i+j);
      LabelButton(i+1,j,"---");
      mapper->setMapping(edit_hour_button[i][j],24*i+j);
      connect(edit_hour_button[i][j],SIGNAL(clicked()),mapper,SLOT(map()));
      connect(edit_hour_button[i][j],SIGNAL(rightClicked(int,const QPoint &)),
	      this,SLOT(rightHourButtonData(int,const QPoint &)));
    }
  }
  for(int i=5;i<7;i++) {
    edit_day_boxes[i]=new QGroupBox(QDate::longDayName(i+1),this);
    edit_day_boxes[i]->setFont(labelFont());
    edit_day_boxes[i]->setGeometry(5,41+75*i,sizeHint().width()-5,65);
    for(int j=0;j<24;j++) {
      edit_hour_button[i][j]=new RDPushButton(this);
      edit_hour_button[i][j]->setGeometry(10+42*j,60+75*i,42,40);
      edit_hour_button[i][j]->setFont(subButtonFont());
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
  edit_mouse_menu=new QMenu(this);

  edit_edit_clock_action=edit_mouse_menu->
    addAction(tr("Edit Clock"),this,SLOT(editClockData()));
  edit_edit_clock_action->setCheckable(false);
  connect(edit_mouse_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));

  //
  // Change All Button
  //
  QPushButton *all_button=new QPushButton(this);
  all_button->setGeometry(10,sizeHint().height()-60,80,50);
  all_button->setDefault(false);
  all_button->setFont(buttonFont());
  all_button->setText(tr("Change\nAll"));
  connect(all_button,SIGNAL(clicked()),this,SLOT(allHourButtonData()));
                      
  //
  //  Close Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("Close"));
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
  ListClocks *listclocks=new ListClocks(&clockname,this);
  if(listclocks->exec()<0) {
    delete listclocks;
    return;
  }
  delete listclocks;
  current_clockname=clockname;
  QString sql=QString("update `SERVICE_CLOCKS` set ");
  if(clockname.isEmpty()) {
    sql+="`CLOCK_NAME`=null ";
  }
  else {
    sql+="`CLOCK_NAME`='"+RDEscapeString(clockname)+"' ";
  }
  sql+=QString("where ")+
    "(`SERVICE_NAME`='"+RDEscapeString(edit_servicename)+"')&&"+
    QString().sprintf("(`HOUR`=%d)",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  LabelButton(dayofweek,hour,clockname);
}


void EditGrid::allHourButtonData()
{
  QString clockname="";
  ListClocks *listclocks=new ListClocks(&clockname,this);
  if(listclocks->exec()<0) {
    delete listclocks;
    return;
  }
  delete listclocks;
  if(QMessageBox::question(this,"RDLogManager - "+tr("Clear Clocks"),
			   tr("Are you sure you want to update ALL clocks in the grid?")+"\n"+tr("This operation cannot be undone!"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
    for(int id=0;id<168;id++) {
      QString sql=QString("update `SERVICE_CLOCKS` set ")+
	"`CLOCK_NAME`='"+RDEscapeString(clockname)+"' where "+
	"`SERVICE_NAME`='"+RDEscapeString(edit_servicename)+"'";
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
  
  edit_mouse_menu->
    setGeometry(edit_hour_button[dayofweek-1][hour]->geometry().x()+
		geometry().x()+pt.x()+2,
		edit_hour_button[dayofweek-1][hour]->geometry().y()+
		geometry().y()+pt.y(),
		edit_mouse_menu->sizeHint().width(),
		edit_mouse_menu->sizeHint().height());
  edit_mouse_menu->exec();
}


void EditGrid::aboutToShowData()
{
  int dayofweek=edit_rightclick_id/24+1;
  int hour=edit_rightclick_id-24*(dayofweek-1);

  edit_edit_clock_action->setEnabled(!GetClock(dayofweek,hour).isEmpty());
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
  EditClock *dialog=new EditClock(clockname,false,&new_clocks,this);
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

  QString sql=QString("update `SERVICE_CLOCKS` set `CLOCK_NAME`=null where ")+
    "(`SERVICE_NAME`='"+RDEscapeString(edit_servicename)+"')&&"+
    QString().sprintf("(HOUR=%d)",(dayofweek-1)*24+hour);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  LabelButton(dayofweek,hour,"");
}


void EditGrid::closeData()
{
  done(-1);
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
  QColor color=palette().color(QPalette::Background);

  QString sql=QString("select ")+
    "`SHORT_NAME`,"+  // 00
    "`COLOR` "+       // 01
    "from `CLOCKS` where "+
    "`NAME`='"+RDEscapeString(clockname)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    code=q->value(0).toString();
    if(!q->value(1).isNull()) {
      color=QColor(q->value(1).toString());
    }
  }
  edit_hour_button[dayofweek-1][hour]->
    setText(QString().sprintf("%02d-%02d\n",hour,hour+1)+code);
  edit_hour_button[dayofweek-1][hour]->
    setPalette(QPalette(color,palette().color(QPalette::Background)));
}


QString EditGrid::GetClock(int dayofweek,int hour)
{
  QString sql=QString("select `CLOCK_NAME` from `SERVICE_CLOCKS` where ")+
    "(`SERVICE_NAME`='"+RDEscapeString(edit_servicename)+"')&&"+
    QString().sprintf("(`HOUR`=%d)",24*(dayofweek-1)+hour);

  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    return q->value(0).toString();
  }
  return QString();
}

