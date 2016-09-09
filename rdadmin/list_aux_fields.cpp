// list_aux_fields.cpp
//
// List Auxiliary Fields for an RSS Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>

#include "edit_aux_field.h"
#include "add_aux_field.h"
#include "list_aux_fields.h"

ListAuxFields::ListAuxFields(unsigned feed_id,QWidget *parent)
  : QDialog(parent)
{
  list_feed_id=feed_id;
  setWindowTitle("RDAdmin - "+tr("Auxiliary Metadata Fields"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Fields List Box
  //
  list_label=new QLabel(tr("Auxiliary Metadata Fields"),this);
  list_label->setFont(bold_font);
  list_model=new RDSqlTableModel(this);
  QString   sql=QString("select ")+
    "ID,"+
    "VAR_NAME,"+
    "CAPTION "+
    "from AUX_METADATA where "+
    QString().sprintf("FEED_ID=%u ",list_feed_id)+
    "order by CAPTION";
  list_model->setQuery(sql);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Variable Name"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Caption"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(bold_font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(bold_font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(bold_font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setFont(bold_font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ListAuxFields::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListAuxFields::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListAuxFields::addData()
{
  unsigned field_id=0;

  AddAuxField *af=new AddAuxField(list_feed_id,&field_id,this);
  if(af->exec()==0) {
    list_model->update();
  }
  delete af;
}


void ListAuxFields::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditAuxField *ef=new EditAuxField(s->selectedRows()[0].data().toInt(),this);
    if(ef->exec()==0) {
      list_model->update();
    }
  }
}


void ListAuxFields::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Warning"),
			     tr("This will delete all data associated with this field!\nAre you sure you want to continue?"),QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }

    QString sql=QString("select KEY_NAME from FEEDS where ")+
      QString().sprintf("ID=%d",list_feed_id);
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      QString keyname=q->value(0).toString();
      delete q;
      keyname.replace(" ","_");
      sql=QString("select VAR_NAME from AUX_METADATA where ")+
	QString().sprintf("ID=%d",s->selectedRows()[0].data().toInt());
      q=new RDSqlQuery(sql);
      if(q->first()) {
	sql=QString("alter table `")+keyname+"_FIELDS` "+
	  "drop column `"+q->value(0).toString()+"`";
	RDSqlQuery::run(sql);
      }
    }
    delete q;

    sql=QString("delete from AUX_METADATA where ")+
      QString().sprintf("ID=%d",s->selectedRows()[0].data().toInt());
    RDSqlQuery::run(sql);
    list_model->update();
  }
}


void ListAuxFields::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListAuxFields::closeData()
{
  done(0);
}


void ListAuxFields::resizeEvent(QResizeEvent *e)
{
  list_label->setGeometry(14,5,size().width()-28,19);
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
