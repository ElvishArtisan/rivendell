// edit_superfeed.cpp
//
// Edit Rivendell Superfeed
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

#include <qpushbutton.h>

#include <rddb.h>
#include <rdescape_string.h>

#include <qmap.h>

#include "edit_superfeed.h"

EditSuperfeed::EditSuperfeed(RDFeed *feed,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

  feed_feed=feed;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("RSS Superfeed")+": "+feed_feed->keyName());

  //
  // Feed Selector
  //
  feed_host_sel=new RDListSelector(this);
  feed_host_sel->sourceSetLabel(tr("Available Feeds"));
  feed_host_sel->destSetLabel(tr("Member Feeds"));
  feed_host_sel->setGeometry(10,10,380,130);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  sql=QString("select ")+
    "MEMBER_KEY_NAME "+  // 00
    "from SUPERFEED_MAPS where "+
    "KEY_NAME=\""+RDEscapeString(feed_feed->keyName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    feed_host_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString("select KEY_NAME from FEEDS where ")+
    "IS_SUPERFEED='N'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(feed_host_sel->destFindItem(q->value(0).toString())==0) {
      feed_host_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
}


EditSuperfeed::~EditSuperfeed()
{
}


QSize EditSuperfeed::sizeHint() const
{
  return QSize(400,212);
} 


QSizePolicy EditSuperfeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSuperfeed::okData()
{
  RDSqlQuery *q;
  QString sql;

  //
  // Feed ID Map
  //
  QMap<QString,unsigned> feed_ids;
  sql=QString("select KEY_NAME,ID from FEEDS");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    feed_ids[q->value(0).toString()]=q->value(1).toUInt();
  }
  delete q;

  //
  // Add New Groups
  //
  for(unsigned i=0;i<feed_host_sel->destCount();i++) {
    sql=QString("select ")+
      "MEMBER_KEY_NAME "  // 00
      "from SUPERFEED_MAPS where "+
      "KEY_NAME=\""+RDEscapeString(feed_feed->keyName())+"\" && "
      "MEMBER_KEY_NAME=\""+RDEscapeString(feed_host_sel->destText(i))+"\"";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into SUPERFEED_MAPS set ")+
	"KEY_NAME=\""+RDEscapeString(feed_feed->keyName())+"\","+
	"MEMBER_KEY_NAME=\""+RDEscapeString(feed_host_sel->destText(i))+"\","+
	QString().sprintf("FEED_ID=%u,",feed_ids.value(feed_feed->keyName()))+
	QString().sprintf("MEMBER_FEED_ID=%u",feed_ids.value(feed_host_sel->destText(i)));
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Groups
  //
  sql=QString("delete from SUPERFEED_MAPS where ")+
    "KEY_NAME=\""+RDEscapeString(feed_feed->keyName())+"\"";
  for(unsigned i=0;i<feed_host_sel->destCount();i++) {
    sql+=QString(" && MEMBER_KEY_NAME<>\"")+
      RDEscapeString(feed_host_sel->destText(i))+"\"";
  }
  q=new RDSqlQuery(sql);
  delete q;
  done(0);
}


void EditSuperfeed::cancelData()
{
  done(1);
}
