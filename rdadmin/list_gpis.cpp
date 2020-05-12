// list_gpis.cpp
//
// List Rivendell GPIOs
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

#include <rd.h>
#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_gpi.h"
#include "globals.h"
#include "list_gpis.h"

ListGpis::ListGpis(RDMatrix *matrix,RDMatrix::GpioType type,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  Q3ListViewItem *l;
  QString list_label;

  list_matrix=matrix;
  list_type=type;
  switch(type) {
  case RDMatrix::GpioInput:
    list_tablename="GPIS";
    list_size=list_matrix->gpis();
    list_label=tr("GPI Lines");
    setWindowTitle("RDAdmin - "+tr("List GPIs"));
    break;

  case RDMatrix::GpioOutput:
    list_tablename="GPOS";
    list_size=list_matrix->gpos();
    list_label=tr("GPO Lines");
    setWindowTitle("RDAdmin - "+tr("List GPOs"));
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Gpis List Box
  //
  list_list_view=new Q3ListView(this);
  list_list_label=
    new QLabel(list_list_view,list_label,this);
  list_list_label->setFont(labelFont());
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  connect(list_list_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  switch(list_type) {
    case RDMatrix::GpioInput:
      list_list_view->addColumn(tr("GPI"));
      break;

    case RDMatrix::GpioOutput:
      list_list_view->addColumn(tr("GPO"));
      break;
  }
  list_list_view->setColumnAlignment(0,Qt::AlignHCenter);

  list_list_view->addColumn(tr("ON MACRO CART"));
  list_list_view->setColumnAlignment(1,Qt::AlignHCenter);

  list_list_view->addColumn(tr("ON DESCRIPTION"));
  list_list_view->setColumnAlignment(2,Qt::AlignLeft);

  list_list_view->addColumn(tr("OFF MACRO CART"));
  list_list_view->setColumnAlignment(3,Qt::AlignHCenter);

  list_list_view->addColumn(tr("OFF DESCRIPTION"));
  list_list_view->setColumnAlignment(4,Qt::AlignLeft);

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
  list_cancel_button->setFont(buttonFont());
  list_cancel_button->setText(tr("&Cancel"));
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  sql=QString("select ")+
    list_tablename+".NUMBER,"+          // 00
    list_tablename+".MACRO_CART,"+      // 01
    list_tablename+".OFF_MACRO_CART,"+  // 02
    "CART.TITLE "+                      // 03
    "from "+list_tablename+" left join CART "+
    "on "+list_tablename+".MACRO_CART=CART.NUMBER where "+
    "("+list_tablename+".STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    "("+list_tablename+QString().sprintf(".MATRIX=%d)",list_matrix->matrix())+
    "order by "+list_tablename+".NUMBER";
  q=new RDSqlQuery(sql);
  if(list_matrix->type()==RDMatrix::LiveWireLwrpAudio) {
    while(q->next()) {
      l=new Q3ListViewItem(list_list_view);
      l->setText(0,QString().sprintf("%05d",q->value(0).toInt()));
      if(q->value(1).toInt()>0) {
	l->setText(1,QString().sprintf("%06d",q->value(1).toInt()));
      }
      if(q->value(2).toInt()>0) {
	l->setText(3,QString().sprintf("%06d",q->value(2).toInt()));
      }
      if(q->value(1).toInt()>0) {
	l->setText(2,q->value(3).toString());
      }
      else {
	l->setText(2,tr("[unassigned]"));
      }
    }
  }
  else {
    q->first();
    for(int i=0;i<list_size;i++) {
      l=new Q3ListViewItem(list_list_view); 
      l->setText(0,QString().sprintf("%03d",i+1));
      if(q->isValid()&&(q->value(0).toInt()==(i+1))){
	if(q->value(1).toInt()>0) {
	  l->setText(1,QString().sprintf("%06d",q->value(1).toInt()));
	}
	if(q->value(2).toInt()>0) {
	  l->setText(3,QString().sprintf("%06d",q->value(2).toInt()));
	}
	if(q->value(1).toInt()>0) {
	  l->setText(2,q->value(3).toString());
	}
	else {
	  l->setText(2,tr("[unassigned]"));
	}
	q->next();
      }
      else {
	l->setText(2,tr("[unassigned]"));
      }
    }
  }
  delete q;

  //
  // Now go back and get the descriptions for the 'Off' carts
  //
  l=list_list_view->firstChild();
  while(l!=NULL) {
    if(l->text(0).toInt()>0) {
      if(l->text(3).toInt()>0) {
	RDCart *cart=new RDCart(l->text(3).toUInt());
	l->setText(4,cart->title());
	delete cart;
      }
      else {
	l->setText(4,tr("[unassigned]"));
      }
    }
    l=l->nextSibling();
  }

}


QSize ListGpis::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy ListGpis::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListGpis::editData()
{
  int oncart=-1;
  int offcart=-1;

  QString ondesc;
  QString offdesc;
  Q3ListViewItem *item=list_list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  int gpi=item->text(0).toInt();
  if(!item->text(1).isEmpty()) {
    oncart=item->text(1).toInt();
  }
  if(!item->text(3).isEmpty()) {
    offcart=item->text(3).toInt();
  }
  EditGpi *gpi_box=new EditGpi(gpi,&oncart,&ondesc,&offcart,&offdesc,this);
  if(gpi_box->exec()==0) {
    if(oncart>0) {
      item->setText(1,QString().sprintf("%06d",oncart));
      item->setText(2,ondesc);
    }
    else {
      item->setText(1,"");
      item->setText(2,tr("[unassigned]"));
    }
    if(offcart>0) {
      item->setText(3,QString().sprintf("%06d",offcart));
      item->setText(4,offdesc);
    }
    else {
      item->setText(3,"");
      item->setText(4,tr("[unassigned]"));
    }
  }
  delete gpi_box;
}


void ListGpis::doubleClickedData(Q3ListViewItem *,const QPoint &,int)
{
  editData();
}


void ListGpis::okData()
{
  QString sql;
  RDSqlQuery *q;
  RDMacro rml;

  sql=QString("delete from ")+list_tablename+" where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("MATRIX=%d",list_matrix->matrix());
  q=new RDSqlQuery(sql);
  delete q;
  RDStation *station=new RDStation(list_matrix->station());
  rml.setCommand(RDMacro::GI);
  rml.setRole(RDMacro::Cmd);
  rml.addArg(list_matrix->matrix());
  switch(list_type) {
  case RDMatrix::GpioInput:
    rml.addArg("I");
    break;

  case RDMatrix::GpioOutput:
    rml.addArg("O");
    break;
  }
  rml.setAddress(station->address());
  rml.setEchoRequested(false);
  delete station;

  //
  // Placeholders for the actual values generated below
  //
  rml.addArg(0);
  rml.addArg(0);
  rml.addArg(0);

  Q3ListViewItem *item=list_list_view->firstChild();
  while(item!=NULL) {
    sql=QString("insert into ")+list_tablename+" set "+
      "STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\","+
      QString().sprintf("MATRIX=%d,",list_matrix->matrix())+
      QString().sprintf("NUMBER=%d,",item->text(0).toInt())+
      QString().sprintf("MACRO_CART=%d,",item->text(1).toInt())+
      QString().sprintf("OFF_MACRO_CART=%d",item->text(3).toInt());
    q=new RDSqlQuery(sql);
    delete q;
    rml.setArg(2,item->text(0).toInt());
    rml.setArg(3,true);
    rml.setArg(4,item->text(1).toInt());
    rda->ripc()->sendRml(&rml);
    rml.setArg(3,false);
    rml.setArg(4,item->text(3).toInt());
    rda->ripc()->sendRml(&rml);
    item=item->nextSibling();
  }
  done(0);
}


void ListGpis::cancelData()
{
  done(1);
}


void ListGpis::resizeEvent(QResizeEvent *e)
{
  list_list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_list_label->setGeometry(14,5,85,19);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  list_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
