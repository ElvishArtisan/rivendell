// rdlist_groups.cpp
//
// A widget to select a Rivendell Group.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlist_groups.cpp,v 1.7 2010/07/29 19:32:33 cvs Exp $
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
#include <qlabel.h>
#include <rddb.h>
#include <qdatetime.h>

#include <rdlist_groups.h>


RDListGroups::RDListGroups(QString *groupname,const QString &username,
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

  group_name=groupname;

  setCaption(tr("Select Group"));

  // 
  // Create Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Cart List
  //
  group_group_list=new QListView(this,"group_group_list");
  group_group_list->setSelectionMode(QListView::Single);
  group_group_list->setGeometry(10,10,
			      sizeHint().width()-20,sizeHint().height()-80);
  group_group_list->setAllColumnsShowFocus(true);
  group_group_list->setItemMargin(5);
  connect(group_group_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));
  group_group_list->addColumn(tr("NAME"));
  group_group_list->setColumnAlignment(0,Qt::AlignHCenter);
  group_group_list->addColumn(tr("DESCRIPTION"));
  group_group_list->setColumnAlignment(1,Qt::AlignLeft);

  //
  // OK Button
  //
  QPushButton *button=new QPushButton(tr("&OK"),this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  button=new QPushButton(tr("&Cancel"),this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  BuildGroupList(username);
}


RDListGroups::~RDListGroups()
{
}


QSize RDListGroups::sizeHint() const
{
  return QSize(400,370);
}


QSizePolicy RDListGroups::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDListGroups::doubleClickedData(QListViewItem *item,const QPoint &pt,
				     int col)
{
  okData();
}


void RDListGroups::okData()
{
  QListViewItem *item=group_group_list->selectedItem();
  if(item==NULL) {
    return;
  }
  *group_name=item->text(0);
  done(0);
}


void RDListGroups::cancelData()
{
  done(-1);
}


void RDListGroups::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void RDListGroups::BuildGroupList(const QString &username)
{
  QString sql;
  RDSqlQuery *q;
  QListViewItem *item=NULL;
  QListViewItem *cur_item=NULL;

  group_group_list->clear();
  sql=QString().sprintf("select USER_PERMS.GROUP_NAME,GROUPS.DESCRIPTION\
                         from USER_PERMS left join GROUPS\
                         on USER_PERMS.GROUP_NAME=GROUPS.NAME\
                         where USER_NAME=\"%s\"",
			(const char *)username);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new QListViewItem(group_group_list);
    item->setText(0,q->value(0).toString());
    item->setText(1,q->value(1).toString());
    if(q->value(0).toString()==*group_name) {
      cur_item=item;
    }
  }
  delete q;
  if(cur_item!=NULL) {
    group_group_list->setSelected(cur_item,true);
    group_group_list->ensureItemVisible(item);
  }
}
