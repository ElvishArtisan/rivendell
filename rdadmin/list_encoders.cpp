// list_encoders.cpp
//
// List a Rivendell Encoders
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_encoders.cpp,v 1.5 2012/02/13 19:26:14 cvs Exp $
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
#include <rddb.h>
#include <rdescape_string.h>
#include <list_encoders.h>
#include <edit_encoder.h>
#include <add_encoder.h>

ListEncoders::ListEncoders(const QString &stationname,
			   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_stationname=stationname;
  setCaption(tr("RDAdmin - List Encoders"));

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
  // Encoders List Box
  //
  list_list_view=new RDListView(this,"list_box");
  list_list_label=new QLabel(list_list_view,tr("Encoders on")+" "+stationname,
			     this,"list_list_label");
  list_list_label->setFont(bold_font);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  list_list_view->addColumn(tr("Format Name"));
  list_list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_list_view->addColumn(tr("Extension"));
  list_list_view->setColumnAlignment(1,Qt::AlignHCenter);
  list_list_view->addColumn(tr("Valid Channels"));
  list_list_view->setColumnAlignment(2,Qt::AlignLeft);
  list_list_view->addColumn(tr("Valid Sample Rates"));
  list_list_view->setColumnAlignment(3,Qt::AlignLeft);
  list_list_view->addColumn(tr("Valid Bit Rates"));
  list_list_view->setColumnAlignment(4,Qt::AlignLeft);
  connect(list_list_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this,"list_add_button");
  list_add_button->setFont(bold_font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this,"edit_button");
  list_edit_button->setFont(bold_font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this,"delete_button");
  list_delete_button->setFont(bold_font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this,"close_button");
  list_close_button->setDefault(true);
  list_close_button->setFont(bold_font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
}


QSize ListEncoders::sizeHint() const
{
  return QSize(600,375);
} 


QSizePolicy ListEncoders::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListEncoders::resizeEvent(QResizeEvent *e)
{
  list_list_label->setGeometry(14,5,size().width()-20,19);
  list_list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListEncoders::addData()
{
  QString name;
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  AddEncoder *ad=new AddEncoder(&name,list_stationname,this);
  if(ad->exec()==0) {
    sql=QString().sprintf("select ID from ENCODERS \
                           where (NAME=\"%s\")&&(STATION_NAME=\"%s\")",
			  (const char *)RDEscapeString(name),
			  (const char *)RDEscapeString(list_stationname));
    q=new RDSqlQuery(sql);
    if(q->first()) {
      EditEncoder *ee=new EditEncoder(q->value(0).toInt());
      if(ee->exec()==0) {
	RDListViewItem *item=new RDListViewItem(list_list_view);
	item->setId(q->value(0).toInt());
	item->setText(0,name);
	RefreshItem(item);
	list_list_view->ensureItemVisible(item);
      }
      else {
	sql=QString().sprintf("delete from ENCODERS \
                              where (NAME=\"%s\")&&(STATION_NAME=\"%s\")",
			      (const char *)RDEscapeString(name),
			      (const char *)RDEscapeString(list_stationname));
	q1=new RDSqlQuery(sql);
	delete q1;
      }
      delete ee;
    }
    delete q;
  }
  else {
    sql=QString().sprintf("delete from ENCODERS \
                          where (NAME=\"%s\")&&(STATION_NAME=\"%s\")",
			  (const char *)RDEscapeString(name),
			  (const char *)RDEscapeString(list_stationname));
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete ad;
}


void ListEncoders::editData()
{
  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  EditEncoder *ee=new EditEncoder(item->id());
  if(ee->exec()==0) {
    RefreshItem(item);
  }
  delete ee;
}


void ListEncoders::doubleClickedData(QListViewItem *item,const QPoint &pt,
				      int col)
{
  editData();
}


void ListEncoders::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  QString feedlist;

  RDListViewItem *item;

  if((item=(RDListViewItem *)list_list_view->selectedItem())==NULL) {
    return;
  }
  if(QMessageBox::question(this,tr("RDAdmin - Delete Encoder"),
			   tr("Are you sure you want to delete this encoder?"),
			   QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
    return;
  }
  sql=QString().sprintf("select KEY_NAME,CHANNEL_TITLE from FEEDS \
                         where UPLOAD_FORMAT=%d",item->id());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    feedlist+=(q->value(0).toString()+" - "+q->value(1).toString());
  }
  delete q;
  if(!feedlist.isEmpty()) {
    if(QMessageBox::warning(this,tr("RDAdmin - List Encoders"),
			    tr("This encoder is in use by the following RSS feeds:\n\n")+feedlist+tr("\n\nDo you still want to delete it?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }

  sql=QString().sprintf("delete from ENCODERS where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  delete q;
  delete item;
}


void ListEncoders::closeData()
{
  done(0);
}


void ListEncoders::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item=NULL;

  sql=QString().sprintf("select ID,NAME,COMMAND_LINE from ENCODERS\
                         where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(list_stationname));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(list_list_view);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
    RefreshItem(item);
  }
  delete q;
}


void ListEncoders::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;
  sql=QString().sprintf("select DEFAULT_EXTENSION from ENCODERS \
                         where ID=%d",item->id());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    item->setText(1,q->value(0).toString());
  }
  delete q;
  item->setText(2,BuildList(item->id(),"CHANNELS"));
  item->setText(3,BuildList(item->id(),"SAMPLERATES"));
  item->setText(4,BuildList(item->id(),"BITRATES"));
}


QString ListEncoders::BuildList(int encoder_id,const QString &paramname)
{
  QString sql;
  RDSqlQuery *q;
  QString ret;

  sql=QString().sprintf("select %s from ENCODER_%s where ENCODER_ID=%d\
                         order by %s",
			(const char *)paramname,
			(const char *)paramname,
			encoder_id,
			(const char *)paramname);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ret=ret+QString().sprintf("%d,",q->value(0).toInt());
  }
  delete q;
  if(ret.isEmpty()) {
    ret=tr("[none]");
  }
  else {
    ret=ret.left(ret.length()-1);
  }

  return ret;
}
