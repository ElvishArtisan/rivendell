// list_gpis.cpp
//
// List a Rivendell GPIOs
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_gpis.cpp,v 1.13.8.1 2013/11/17 04:27:05 cvs Exp $
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
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdpasswd.h>
#include <rdmacro.h>
#include <rdstation.h>
#include <rddb.h>

#include <list_gpis.h>
#include <edit_gpi.h>
#include <globals.h>


ListGpis::ListGpis(RDMatrix *matrix,RDMatrix::GpioType type,
		   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QListViewItem *l;
  QString list_label;

  list_matrix=matrix;
  list_type=type;
  switch(type) {
    case RDMatrix::GpioInput:
      list_tablename="GPIS";
      list_size=list_matrix->gpis();
      list_label=tr("GPI Lines");
      setCaption(tr("List GPIs"));
      break;

    case RDMatrix::GpioOutput:
      list_tablename="GPOS";
      list_size=list_matrix->gpos();
      list_label=tr("GPO Lines");
      setCaption(tr("List GPOs"));
      break;
  }

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
  // Gpis List Box
  //
  list_list_view=new QListView(this,"list_box");
  list_list_label=
    new QLabel(list_list_view,list_label,this,"list_view_label");
  list_list_label->setFont(bold_font);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  connect(list_list_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

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
  list_edit_button=new QPushButton(this,"list_edit_button");
  list_edit_button->setFont(bold_font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Ok Button
  //
  list_ok_button=new QPushButton(this,"list_ok_button");
  list_ok_button->setDefault(true);
  list_ok_button->setFont(bold_font);
  list_ok_button->setText(tr("&OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  list_cancel_button=new QPushButton(this,"list_cancel_button");
  list_cancel_button->setFont(bold_font);
  list_cancel_button->setText(tr("&Cancel"));
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  sql=QString().sprintf("select %s.NUMBER,%s.MACRO_CART,%s.OFF_MACRO_CART,\
                         CART.TITLE \
                         from %s left join CART \
                         on %s.MACRO_CART=CART.NUMBER \
                         where (%s.STATION_NAME=\"%s\")&&(%s.MATRIX=%d)\
                         order by %s.NUMBER",
			(const char *)list_tablename,
			(const char *)list_tablename,
			(const char *)list_tablename,
			(const char *)list_tablename,
			(const char *)list_tablename,
			(const char *)list_tablename,
			(const char *)list_matrix->station(),
			(const char *)list_tablename,
			list_matrix->matrix(),
			(const char *)list_tablename);
  q=new RDSqlQuery(sql);
  if(list_matrix->type()==RDMatrix::LiveWireLwrpAudio) {
    while(q->next()) {
      l=new QListViewItem(list_list_view);
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
      l=new QListViewItem(list_list_view); 
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
  QListViewItem *item=list_list_view->selectedItem();
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
  EditGpi *gpi_box=
    new EditGpi(gpi,&oncart,&ondesc,&offcart,&offdesc,this,"gpi_box");
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


void ListGpis::doubleClickedData(QListViewItem *,const QPoint &,int)
{
  editData();
}


void ListGpis::okData()
{
  QString sql;
  RDSqlQuery *q;
  RDMacro rml;

  sql=QString().sprintf("delete from %s where (STATION_NAME=\"%s\")&&\
                           MATRIX=%d",
			(const char *)list_tablename,
			(const char *)list_matrix->station(),
			list_matrix->matrix());
  q=new RDSqlQuery(sql);
  delete q;
  RDStation *station=new RDStation(list_matrix->station());
  rml.setCommand(RDMacro::GI);
  rml.setRole(RDMacro::Cmd);
  rml.setArgQuantity(5);
  rml.setArg(0,list_matrix->matrix());
  switch(list_type) {
    case RDMatrix::GpioInput:
      rml.setArg(1,"I");
      break;

    case RDMatrix::GpioOutput:
      rml.setArg(1,"O");
      break;
  }
  rml.setAddress(station->address());
  rml.setEchoRequested(false);
  delete station;

  QListViewItem *item=list_list_view->firstChild();
  while(item!=NULL) {
    sql=QString().sprintf("insert into %s set STATION_NAME=\"%s\",\
                           MATRIX=%d,NUMBER=%d,MACRO_CART=%d,OFF_MACRO_CART=%d",
			  (const char *)list_tablename,
			  (const char *)list_matrix->station(),
			  list_matrix->matrix(),
			  item->text(0).toInt(),
			  item->text(1).toInt(),
			  item->text(3).toInt());
    q=new RDSqlQuery(sql);
    delete q;
    rml.setArg(2,item->text(0).toInt());
    rml.setArg(3,true);
    rml.setArg(4,item->text(1).toInt());
    rdripc->sendRml(&rml);
    rml.setArg(3,false);
    rml.setArg(4,item->text(3).toInt());
    rdripc->sendRml(&rml);
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
