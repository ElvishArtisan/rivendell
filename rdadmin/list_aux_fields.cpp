// list_aux_fields.cpp
//
// List Auxiliary Fields for an RSS Feed
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_aux_fields.cpp,v 1.5 2010/07/29 19:32:34 cvs Exp $
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

#include <rddb.h>
#include <list_aux_fields.h>
#include <edit_aux_field.h>
#include <add_aux_field.h>


ListAuxFields::ListAuxFields(unsigned feed_id,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_feed_id=feed_id;
  setCaption(tr("Auxiliary Metadata Fields"));

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
  // Fields List Box
  //
  list_list_view=new QListView(this,"list_box");
  list_list_view->
    setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-94);
  QLabel *label=
    new QLabel(list_list_view,tr("Auxiliary Metadata Fields"),
	       this,"list_list_view_label");
  label->setFont(bold_font);
  label->setGeometry(14,5,sizeHint().width()-28,19);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  list_list_view->addColumn(tr("Var Name"));
  list_list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_list_view->addColumn(tr("Caption"));
  list_list_view->setColumnAlignment(1,Qt::AlignLeft);
  connect(list_list_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Add Button
  //
  QPushButton *button=new QPushButton(this,"add_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Add"));
  connect(button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  button=new QPushButton(this,"edit_button");
  button->setGeometry(100,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Edit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  button=new QPushButton(this,"delete_button");
  button->setGeometry(190,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Delete"));
  connect(button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
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
    RDListViewItem *item=new RDListViewItem(list_list_view);
    item->setId(field_id);
    RefreshItem(item);
    list_list_view->setSelected(item,true);
    list_list_view->ensureItemVisible(item);
  }
  delete af;
}


void ListAuxFields::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditAuxField *ef=new EditAuxField(item->id(),this);
  if(ef->exec()==0) {
    RefreshItem(item);
  }
  delete ef;
}


void ListAuxFields::deleteData()
{
  QString sql;
  RDSqlQuery *q;

  RDListViewItem *item=(RDListViewItem *)list_list_view->selectedItem();
  if(item==NULL) {
    return;
  }

  if(QMessageBox::question(this,tr("Warning"),
			   tr("This will delete all data associated with this field!\nAre you sure you want to continue?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
    return;
  }

  sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%d",list_feed_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QString keyname=q->value(0).toString();
    delete q;
    keyname.replace(" ","_");
    QString varname=item->text(0).mid(1,item->text(0).length()-2);
    sql=QString().sprintf("alter table %s_FIELDS drop column %s",
			  (const char *)keyname,(const char *)varname);
    q=new RDSqlQuery(sql);
  }
  delete q;

  sql=QString().sprintf("delete from AUX_METADATA where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  delete q;
  delete item;
}


void ListAuxFields::doubleClickedData(QListViewItem *item,const QPoint &pt,
				      int col)
{
  editData();
}


void ListAuxFields::closeData()
{
  done(0);
}


void ListAuxFields::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_list_view->clear();
  sql=QString().sprintf("select ID,VAR_NAME,CAPTION from AUX_METADATA \
                         where FEED_ID=%u order by CAPTION",list_feed_id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(list_list_view);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
    item->setText(1,q->value(2).toString());
  }
  delete q;
}


void ListAuxFields::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select VAR_NAME,CAPTION from AUX_METADATA \
                         where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    item->setText(0,q->value(0).toString());
    item->setText(1,q->value(1).toString());
  }
  delete q;
}
