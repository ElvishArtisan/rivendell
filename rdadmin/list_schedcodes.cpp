// List_schedcodes.cpp
//
// The scheduler codes dialog for rdadmin
//
//   by Stefan Gabriel <stg@st-gabriel.de>
//   Modifications for Qt4 (C) 2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <QResizeEvent>

#include <rdschedcode.h>

#include "add_schedcodes.h"
#include "edit_schedcodes.h"
#include "list_schedcodes.h"

ListSchedCodes::ListSchedCodes(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle(tr("Rivendell Scheduler Codes List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Group List
  //
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "CODE,"+
    "DESCRIPTION "+
    "from SCHED_CODES "+
    "order by CODE";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Code"),Qt::DisplayRole);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Description"),Qt::DisplayRole);
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  list_box_label=new QLabel(tr("Scheduler Codes:"),this);
  list_box_label->setFont(font);
}


ListSchedCodes::~ListSchedCodes()
{
}


QSize ListSchedCodes::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListSchedCodes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSchedCodes::addData()
{
  QString schedCode;

  AddSchedCode *add_schedCode=new AddSchedCode(&schedCode,this);
  if(add_schedCode->exec()<0) {
    delete add_schedCode;
    return;
  }
  delete add_schedCode;
  add_schedCode=NULL;
  list_model->update();
}


void ListSchedCodes::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    doubleClickedData(list_model->index(s->selectedRows()[0].row(),0));
  }
}


void ListSchedCodes::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    QString code=list_model->data(s->selectedRows()[0].row(),0).toString();
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Scheduler Code"),
	      tr("This operation will delete the selected scheduler code and all of its associated data.")+" "+
			     tr("This operation cannot be undone!")+"\n\n"+
			     tr("Delete scheduler code")+" \""+code+"\"?",
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    RDSchedCode::remove(code);
    list_model->update();
  }
}


void ListSchedCodes::doubleClickedData(const QModelIndex &index)
{
  EditSchedCode *edit_schedCode=
    new EditSchedCode(list_model->data(index.row(),0).toString(),
		      list_model->data(index.row(),1).toString(),this);
  edit_schedCode->exec();
  delete edit_schedCode;
  list_model->update();
}


void ListSchedCodes::closeData()
{
  done(0);
}


void ListSchedCodes::resizeEvent(QResizeEvent *e)
{
  list_box_label->setGeometry(14,11,200,19);
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,210,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
