// list_sas_resources.cpp
//
// List SAS Resources.
//
//   (C) Copyright 2002-2005,2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_sas_resources.cpp,v 1.2 2011/05/23 21:53:22 cvs Exp $
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
#include <rdescape_string.h>
#include <rddb.h>
#include <edit_user.h>
#include <list_sas_resources.h>
#include <edit_sas_resource.h>


ListSasResources::ListSasResources(RDMatrix *matrix,int size,
					 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  QString str;

  list_matrix=matrix;
  list_size=size;
  setCaption(tr("SAS Switches"));

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

  //
  // Resources List Box
  //
  list_list_view=new QListView(this,"list_box");
  list_list_view->
    setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-94);
  QLabel *label=
    new QLabel(list_list_view,list_table,this,"list_list_view_label");
  label->setFont(bold_font);
  label->setGeometry(14,5,85,19);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  list_list_view->addColumn(tr("GPIO LINE"));
  list_list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_list_view->addColumn(tr("CONSOLE"));
  list_list_view->setColumnAlignment(1,Qt::AlignHCenter);
  list_list_view->addColumn(tr("SOURCE"));
  list_list_view->setColumnAlignment(2,Qt::AlignHCenter);
  list_list_view->addColumn(tr("OPTO/RELAY"));
  list_list_view->setColumnAlignment(3,Qt::AlignHCenter);
  connect(list_list_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Edit Button
  //
  QPushButton *button=new QPushButton(this,"edit_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Edit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  RefreshList();
}


QSize ListSasResources::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListSasResources::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSasResources::editData()
{
  int engine_num=-1;
  int device_num=-1;
  int relay_num=-1;

  QListViewItem *item=list_list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  if(!item->text(1).isEmpty()) {
    engine_num=item->text(1).toInt();
  }
  if(!item->text(2).isEmpty()) {
    device_num=item->text(2).toInt();
  }
  if(!item->text(3).isEmpty()) {
    relay_num=item->text(3).toInt();
  }
  EditSasResource *dialog=
    new EditSasResource(&engine_num,&device_num,&relay_num,this,"dialog");
  if(dialog->exec()==0) {
    if(engine_num>=0) {
      item->setText(1,QString().sprintf("%d",engine_num));
    }
    else {
      item->setText(1,"");
    }
    if(device_num>=0) {
      item->setText(2,QString().sprintf("%d",device_num));
    }
    else {
      item->setText(2,"");
    }
    if(relay_num>=0) {
      item->setText(3,QString().sprintf("%d",relay_num));
    }
    else {
      item->setText(3,"");
    }
  }
  delete dialog;
}


void ListSasResources::doubleClickedData(QListViewItem *item,
					    const QPoint &pt,int col)
{
  editData();
}


void ListSasResources::okData()
{
  QString sql;
  RDSqlQuery *q;
  int engine_num=-1;
  int device_num=-1;
  int surface_num=-1;
  int relay_num=-1;

  QListViewItem *item=list_list_view->firstChild();
  while(item!=NULL) {
    engine_num=-1;
    device_num=-1;
    surface_num=-1;
    relay_num=-1;
    if(!item->text(1).isEmpty()) {
      engine_num=item->text(1).toInt();
    }
    if(!item->text(2).isEmpty()) {
      device_num=item->text(2).toInt();
    }
    if(!item->text(3).isEmpty()) {
      relay_num=item->text(3).toInt();
    }
    sql=QString().sprintf("select ID from VGUEST_RESOURCES where\
                           (STATION_NAME=\"%s\")&&\
                           (MATRIX_NUM=%d)&&\
                           (NUMBER=%d)",
			  (const char *)list_matrix->station(),
			  list_matrix->matrix(),item->text(0).toInt());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("update VGUEST_RESOURCES set\
                             ENGINE_NUM=%d,DEVICE_NUM=%d,\
                             SURFACE_NUM=%d,RELAY_NUM=%d\
                             where\
                             (STATION_NAME=\"%s\")&&\
                             (MATRIX_NUM=%d)&&\
                             (NUMBER=%d)",
			    engine_num,device_num,surface_num,
			    relay_num,
			    (const char *)list_matrix->station(),
			    list_matrix->matrix(),
			    item->text(0).toInt());
    }
    else {
      sql=QString().sprintf("insert into VGUEST_RESOURCES set\
                             STATION_NAME=\"%s\",MATRIX_NUM=%d,\
                             NUMBER=%d,\
                             ENGINE_NUM=%d,DEVICE_NUM=%d,\
                             SURFACE_NUM=%d,RELAY_NUM=%d",
			    (const char *)list_matrix->station(),
			    list_matrix->matrix(),
			    item->text(0).toInt(),
			    engine_num,device_num,surface_num,
			    relay_num);
    }
    delete q;
    q=new RDSqlQuery(sql);
    delete q;
    item=item->nextSibling();
  }
  done(0);
}


void ListSasResources::cancelData()
{
  done(-1);
}


void ListSasResources::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QListViewItem *item;
  int n=1;
  int gpis;

  //
  // Populate Resource Records
  //
  sql=QString().sprintf("select GPIS from MATRICES \
                         where (STATION_NAME=\"%s\")&&(MATRIX=%d)",
			(const char *)RDEscapeString(list_matrix->station()),
			list_matrix->matrix());
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }
  gpis=q->value(0).toInt();
  delete q;
  for(int i=0;i<gpis;i++) {
    sql=QString().sprintf("select NUMBER from VGUEST_RESOURCES \
                           where (STATION_NAME=\"%s\")&&(MATRIX_NUM=%d)&&\
                           (NUMBER=%d)",
			  (const char *)RDEscapeString(list_matrix->station()),
			  list_matrix->matrix(),
			  i+1);
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      sql=QString().sprintf("insert into VGUEST_RESOURCES set \
                             NUMBER=%d,\
                             STATION_NAME=\"%s\",\
                             MATRIX_NUM=%d",
			    i+1,
			   (const char *)RDEscapeString(list_matrix->station()),
			    list_matrix->matrix());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  sql=QString().sprintf("select NUMBER,ENGINE_NUM,DEVICE_NUM,RELAY_NUM \
                         from VGUEST_RESOURCES where\
                         (STATION_NAME=\"%s\")&&(MATRIX_NUM=%d) \
                         order by NUMBER",
			(const char *)RDEscapeString(list_matrix->station()),
			list_matrix->matrix());
  q=new RDSqlQuery(sql);
  list_list_view->clear();
  while(q->next()) {
    while(q->value(0).toInt()>n) {
      item=new QListViewItem(list_list_view);
      item->setText(0,QString().sprintf("%03d",n++));
    }
    item=new QListViewItem(list_list_view);
    item->setText(0,QString().sprintf("%03d",q->value(0).toInt()));
    if(q->value(1).toInt()>=0) {
      item->setText(1,QString().sprintf("%d",q->value(1).toInt()));
    }
    if(q->value(2).toInt()>=0) {
      item->setText(2,QString().sprintf("%d",q->value(2).toInt()));
    }
    if(q->value(3).toInt()>=0) {
      item->setText(3,QString().sprintf("%d",q->value(3).toInt()));
    }
    n++;
  }
  for(int i=n;i<(list_size+1);i++) {
    item=new QListViewItem(list_list_view);
    item->setText(0,QString().sprintf("%03d",i));
  } 
  delete q;
}
