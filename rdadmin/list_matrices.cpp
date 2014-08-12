// list_matrices.cpp
//
// List Rivendell Matrices
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_matrices.cpp,v 1.28.6.2 2012/12/10 15:40:15 cvs Exp $
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
#include <rddb.h>
#include <globals.h>
#include <list_matrices.h>
#include <edit_matrix.h>
#include <add_matrix.h>

ListMatrices::ListMatrices(QString station,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  list_station=station;
  setCaption(tr("Rivendell Switcher List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Initialize Data Structures
  //
  for(int i=0;i<MAX_MATRICES;i++) {
    list_matrix_modified[i]=false;
  }

  //
  // Matrix List Box
  //
  list_view=new QListView(this,"list_box");
  list_view->setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-94);
  QLabel *label=new QLabel(list_view,tr("Switchers:"),this,"list_view_label");
  label->setFont(font);
  label->setGeometry(14,5,85,19);
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(tr("MATRIX"));
  list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_view->addColumn(tr("DESCRIPTION"));
  list_view->setColumnAlignment(1,Qt::AlignLeft);
  list_view->addColumn(tr("TYPE"));
  list_view->setColumnAlignment(2,Qt::AlignLeft);
  connect(list_view,SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  RefreshList();

  //
  //  Add Button
  //
  QPushButton *add_button=new QPushButton(this,"add_button");
  add_button->setGeometry(10,sizeHint().height()-60,80,50);
  add_button->setFont(font);
  add_button->setText(tr("&Add"));
  connect(add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  QPushButton *edit_button=new QPushButton(this,"edit_button");
  edit_button->setGeometry(100,sizeHint().height()-60,80,50);
  edit_button->setFont(font);
  edit_button->setText(tr("&Edit"));
  connect(edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  QPushButton *delete_button=new QPushButton(this,"delete_button");
  delete_button->setGeometry(190,sizeHint().height()-60,80,50);
  delete_button->setFont(font);
  delete_button->setText(tr("&Delete"));
  connect(delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  close_button->setDefault(true);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListMatrices::~ListMatrices()
{
  delete list_view;
}


QSize ListMatrices::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListMatrices::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListMatrices::addData()
{
  int matrix_num;

  AddMatrix *add=new AddMatrix(list_station,this);
  if((matrix_num=add->exec())<0) {
    delete add;
    return;
  }
  delete add;
  RDMatrix *matrix=new RDMatrix(list_station,matrix_num);
  EditMatrix *edit=new EditMatrix(matrix,this);
  if(edit->exec()!=0) {
    DeleteMatrix(matrix_num);
  }
  else {
    list_matrix_modified[matrix_num]=true;
    AddList(matrix_num);
  }
  delete edit;
  delete matrix;
}


void ListMatrices::editData()
{
  if(list_view->selectedItem()==NULL) {
    return;
  }
  int matrix_num=list_view->currentItem()->text(0).toInt();
  RDMatrix *matrix=new RDMatrix(list_station,matrix_num);
  QListViewItem *item=list_view->selectedItem();
  EditMatrix *edit=new EditMatrix(matrix,this);
  if(edit->exec()==0) {
    RefreshRecord(item);
    list_matrix_modified[matrix_num]=true;
  }
  delete edit;
  delete matrix;
}


void ListMatrices::deleteData()
{
  QString str1;
  QString str2;
  QString str3;

  if(list_view->currentItem()==NULL) {
    return;
  }
  int matrix=list_view->currentItem()->text(0).toInt();
  QString desc=list_view->currentItem()->text(1);
  str1=QString(tr("Are you sure you want to delete switcher"));
  str2=QString(tr("on"));
  str3=QString(tr("ALL references to this switcher will be deleted!"));
  QString msg=QString().sprintf("%s \"%d:%s\" %s \"%s\"?\n%s",
				(const char *)str1,
				matrix,
				(const char *)desc,
				(const char *)str2,
				(const char *)list_station,
				(const char *)str3);
  if(QMessageBox::warning(this,tr("Deleting Switcher"),msg,
			  QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  DeleteMatrix(matrix);
  list_matrix_modified[matrix]=true;
  RefreshList();
}


void ListMatrices::doubleClickedData(QListViewItem *item,const QPoint &pt,
				     int col)
{
  editData();
}


void ListMatrices::closeData()
{
  RDStation *rmt_station=new RDStation(list_station);
  QHostAddress addr=rmt_station->address();
  RDMacro macro;
  
  macro.setCommand(RDMacro::SZ);
  macro.setRole(RDMacro::Cmd);
  macro.setEchoRequested(false);
  macro.setArgQuantity(1);
  for(int i=0;i<MAX_MATRICES;i++) {
    if(list_matrix_modified[i]) {
      macro.setAddress(rmt_station->address());
      macro.setArg(0,i);
      rdripc->sendRml(&macro);
    }
  }
  delete rmt_station;
  done(0);
}


void ListMatrices::DeleteMatrix(int matrix)
{
  QString sql=QString().sprintf("delete from MATRICES where \
                               STATION_NAME=\"%s\" && MATRIX=%d",
				(const char *)list_station,
				matrix);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from INPUTS where \
                         STATION_NAME=\"%s\" && MATRIX=%d",
			(const char *)list_station,
			matrix);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from OUTPUTS where \
                         STATION_NAME=\"%s\" && MATRIX=%d",
			(const char *)list_station,
			matrix);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from SWITCHER_NODES where \
                         STATION_NAME=\"%s\" && MATRIX=%d",
			(const char *)list_station,
			matrix);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from GPIS where \
                         STATION_NAME=\"%s\" && MATRIX=%d",
			(const char *)list_station,
			matrix);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from GPOS where \
                         STATION_NAME=\"%s\" && MATRIX=%d",
			(const char *)list_station,
			matrix);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from VGUEST_RESOURCES where \
                         STATION_NAME=\"%s\" && MATRIX_NUM=%d",
			(const char *)list_station,
			matrix);
  q=new RDSqlQuery(sql);
  delete q;
}


void ListMatrices::RefreshList()
{
  QListViewItem *l;

  list_view->clear();
  QString sql=QString().sprintf("select MATRIX,NAME,TYPE from MATRICES \
                                 where STATION_NAME=\"%s\" order by MATRIX",
				(const char *)list_station);
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new QListViewItem(list_view);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,RDMatrix::typeString((RDMatrix::Type)q->value(2).toInt()));
  }
  delete q;
}


void ListMatrices::AddList(int matrix_num)
{
  RDMatrix *matrix=new RDMatrix(list_station,matrix_num);
  QListViewItem *item=new QListViewItem(list_view);
  item->setText(0,QString().sprintf("%d",matrix_num));
  item->setText(1,matrix->name());
  item->setText(2,RDMatrix::typeString(matrix->type()));
  delete matrix;
  list_view->setCurrentItem(item);
  list_view->setSelected(item,true);
}


void ListMatrices::RefreshRecord(QListViewItem *item)
{
  RDMatrix *matrix=new RDMatrix(list_station,item->text(0).toInt());
  item->setText(1,matrix->name());
  delete matrix;
}
