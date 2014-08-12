// list_livewiregpios.cpp
//
// List Rivendell Livewire GPIO Slot Associations
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_livewiregpios.cpp,v 1.1.2.3 2013/03/09 00:21:12 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rdstation.h>
#include <rdescape_string.h>
#include <rddb.h>
#include <globals.h>
#include <list_livewiregpios.h>
#include <edit_livewiregpio.h>

ListLiveWireGpios::ListLiveWireGpios(RDMatrix *matrix,int slot_quan,
				     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  list_matrix=matrix;
  list_slot_quan=slot_quan;
  setCaption(tr("LiveWire GPIO Source Assignments"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Matrix List Box
  //
  list_view=new RDListView(this,"list_box");
  list_view->setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-94);
  QLabel *label=new QLabel(list_view,tr("Switchers:"),this,"list_view_label");
  label->setFont(font);
  label->setGeometry(14,5,85,19);
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(tr("Lines"));
  list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_view->addColumn(tr("Source #"));
  list_view->setColumnAlignment(1,Qt::AlignCenter);
  list_view->addColumn(tr("Surface Address"));
  list_view->setColumnAlignment(2,Qt::AlignCenter);
  list_view->setColumnSortType(0,RDListView::GpioSort);
  connect(list_view,SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  RefreshList();

  //
  //  Edit Button
  //
  QPushButton *edit_button=new QPushButton(this,"edit_button");
  edit_button->setGeometry(10,sizeHint().height()-60,80,50);
  edit_button->setFont(font);
  edit_button->setText(tr("&Edit"));
  connect(edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
			    80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  cancel_button->setDefault(true);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
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


void ListLiveWireGpios::doubleClickedData(QListViewItem *item,const QPoint &pt,
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


void ListLiveWireGpios::RefreshList()
{
  QListViewItem *l;
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
