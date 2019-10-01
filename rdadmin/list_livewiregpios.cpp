// list_livewiregpios.cpp
//
// List Rivendell Livewire GPIO Slot Associations
//
//   (C) Copyright 2013-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "globals.h"

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_livewiregpio.h"
#include "list_livewiregpios.h"

ListLiveWireGpios::ListLiveWireGpios(RDMatrix *matrix,int slot_quan,
				     QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_matrix=matrix;
  list_slot_quan=slot_quan;
  setWindowTitle("RDAdmin - "+tr("Livewire GPIO Source Assignments"));

  //
  // Matrix List Box
  //
  list_view=new RDListView(this);
  list_title_label=new QLabel(list_view,tr("Switchers:"),this);
  list_title_label->setFont(labelFont());
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(tr("Lines"));
  list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_view->addColumn(tr("Source #"));
  list_view->setColumnAlignment(1,Qt::AlignCenter);
  list_view->addColumn(tr("Surface Address"));
  list_view->setColumnAlignment(2,Qt::AlignCenter);
  list_view->setColumnSortType(0,RDListView::GpioSort);
  connect(list_view,SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Ok Button
  //
  list_ok_button=new QPushButton(this);
  list_ok_button->setDefault(true);
  list_ok_button->setFont(buttonFont());
  list_ok_button->setText(tr("&OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  list_cancel_button=new QPushButton(this);
  list_cancel_button->setDefault(true);
  list_cancel_button->setFont(buttonFont());
  list_cancel_button->setText(tr("&Cancel"));
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


ListLiveWireGpios::~ListLiveWireGpios()
{
  delete list_view;
}


QSize ListLiveWireGpios::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListLiveWireGpios::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListLiveWireGpios::editData()
{
  if(list_view->selectedItem()==NULL) {
    return;
  }
  int source=list_view->currentItem()->text(1).toInt();
  QHostAddress addr;
  addr.setAddress(list_view->currentItem()->text(2));
  EditLiveWireGpio *d=
    new EditLiveWireGpio(list_view->currentItem()->text(0).toInt(),
			 &source,&addr);
  if(d->exec()==0) {
    if(source==0) {
      list_view->currentItem()->setText(1,tr("[none]"));
    }
    else {
      list_view->currentItem()->setText(1,QString().sprintf("%d",source));
    }
    if(addr.isNull()) {
      list_view->currentItem()->setText(2,tr("[all]"));
    }
    else {
      list_view->currentItem()->setText(2,addr.toString());
    }
  }
  delete d;
}


void ListLiveWireGpios::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				     int col)
{
  editData();
}


void ListLiveWireGpios::okData()
{
  QString sql;
  RDSqlQuery *q;
  int slot=0;
  QString addr_str="NULL";

  RDListViewItem *item=(RDListViewItem *)list_view->firstChild();
  while(item!=NULL) {
    QHostAddress addr;
    addr_str="NULL";
    addr.setAddress(item->text(2));
    if(!addr.isNull()) {
      addr_str="\""+addr.toString()+"\"";
    }
    sql=QString("update LIVEWIRE_GPIO_SLOTS set ")+
      QString().sprintf("SOURCE_NUMBER=%d,",item->text(1).toInt())+
      "IP_ADDRESS="+addr_str+" "+
      "where (STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX=%d)&&",list_matrix->matrix())+
      QString().sprintf("(SLOT=%d)",slot);
    q=new RDSqlQuery(sql);
    delete q;
    slot++;
    item=(RDListViewItem *)item->nextSibling();
  }

  done(0);
}


void ListLiveWireGpios::cancelData()
{
  done(-1);
}


void ListLiveWireGpios::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_title_label->setGeometry(14,5,85,19);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  list_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListLiveWireGpios::RefreshList()
{
  Q3ListViewItem *l;
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  list_view->clear();
  sql=QString("select SLOT,SOURCE_NUMBER,IP_ADDRESS from LIVEWIRE_GPIO_SLOTS ")+
    "where (STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d) ",list_matrix->matrix())+
    "order by SLOT";
  q=new RDSqlQuery(sql);
  q->first();
  for(int i=0;i<list_slot_quan;i++) {
    l=new RDListViewItem(list_view);
    l->setText(0,QString().sprintf("%d - %d",5*i+1,5*i+5));
    if(q->isValid()&&(q->value(0).toInt()==i)) {
      if(q->value(1).toInt()==0) {
	l->setText(1,tr("[none]"));
      }
      else {
	l->setText(1,QString().sprintf("%d",q->value(1).toInt()));
      }
      if(q->value(2).toString().isEmpty()) {
	l->setText(2,tr("[all]"));
      }
      else {
	l->setText(2,q->value(2).toString());
      }
      q->next();
    }
    else {
      sql=QString("insert into LIVEWIRE_GPIO_SLOTS set ")+
	"STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\","+
	QString().sprintf("MATRIX=%d,",list_matrix->matrix())+
	QString().sprintf("SLOT=%d",i);
      q1=new RDSqlQuery(sql);
      delete q1;
      l->setText(1,tr("[none]"));
      l->setText(2,tr("[all]"));
    }
  }
  delete q;
}
