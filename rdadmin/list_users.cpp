// list_users.cpp
//
// List Rivendell Users
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

#include <math.h>

#include <qmessagebox.h>

#include <rdcart.h>
#include <rdescape_string.h>
#include <rdtextfile.h>

#include "add_user.h"
#include "edit_user.h"
#include "list_users.h"

//
// Icons
//
#include "../icons/admin.xpm"
#include "../icons/localuser.xpm"
#include "../icons/rss.xpm"
#include "../icons/user.xpm"

ListUsers::ListUsers(const QString &admin_name,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);
  list_admin_name=admin_name;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Rivendell User List"));

  //
  // Create Icons
  //
  list_admin_map=new QPixmap(admin_xpm);
  list_localuser_map=new QPixmap(localuser_xpm);
  list_rss_map=new QPixmap(rss_xpm);
  list_user_map=new QPixmap(user_xpm);

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // User List
  //
  list_users_view=new RDListView(this);
  list_users_view->setAllColumnsShowFocus(true);
  list_users_view->setItemMargin(5);
  list_users_view->addColumn("");
  list_users_view->addColumn(tr("Login Name"));
  list_users_view->addColumn(tr("Full Name"));
  list_users_view->addColumn(tr("Description"));
  list_users_view->addColumn(tr("E-Mail Address"));
  list_users_view->addColumn(tr("Phone Number"));
  list_users_view->addColumn(tr("Local Auth"));
  list_users_view->setColumnAlignment(6,Qt::AlignCenter);
  QLabel *list_box_label=new QLabel(list_users_view,tr("&Users:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,11,85,19);
  connect(list_users_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();
}


ListUsers::~ListUsers()
{
}


QSize ListUsers::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListUsers::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListUsers::addData()
{
  QString user;

  AddUser *add_user=new AddUser(&user,this);
  if(add_user->exec()<0) {
    delete add_user;
    return;
  }
  delete add_user;
  add_user=NULL;
  RDListViewItem *item=new RDListViewItem(list_users_view);
  item->setText(1,user);
  RefreshItem(item);
  item->setSelected(true);
  list_users_view->setCurrentItem(item);
  list_users_view->ensureItemVisible(item);
}


void ListUsers::editData()
{
  RDListViewItem *item=(RDListViewItem *)list_users_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditUser *edit_user=new EditUser(item->text(1),this);
  if(edit_user->exec()==0) {
    RefreshItem(item);
  }
  delete edit_user;
}


void ListUsers::deleteData()
{
  RDListViewItem *item=(RDListViewItem *)list_users_view->selectedItem();
  if(item==NULL) {
    return;
  }

  if(list_admin_name==item->text(1)) {
    QMessageBox::warning(this,tr("Delete User"),
			 tr("You cannot delete yourself!"));
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;

  QString username=RDEscapeString(item->text(1));

  //
  // Check for default user assignments
  //
  sql=QString("select NAME from STATIONS where ")+
    "DEFAULT_NAME=\""+RDEscapeString(username)+"\"";
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    str=tr("This user is set as the default user for the following hosts:\n\n");
    while(q->next()) {
      str+=("     "+q->value(0).toString()+"\n");
    }
    str+="\n";
    str+=tr("You must change this before deleting the user.");
    delete q;
    QMessageBox::warning(this,tr("Delete User"),str);
    return;
  }
  delete q;

  str=QString(tr("Are you sure you want to delete user"));
  warning+=str+" \""+item->text(1)+"\"?";
  switch(QMessageBox::warning(this,"RDAdmin - "+tr("Delete User"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
  case QMessageBox::No:
  case Qt::NoButton:
    return;

  default:
    break;
  }

  //
  // Delete RSS Feed Perms
  //
  sql=QString("delete from FEED_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(username)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete Member User Perms
  //
  sql=QString("delete from USER_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(username)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete from User List
  //
  sql=QString("delete from USERS where ")+
    "LOGIN_NAME=\""+RDEscapeString(username)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete from Cached Web Connections
  //
  sql=QString("delete from WEB_CONNECTIONS where ")+
    "LOGIN_NAME=\""+RDEscapeString(username)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  item->setSelected(false);
  delete item;
}


void ListUsers::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListUsers::closeData()
{
  done(0);
}


void ListUsers::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_users_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListUsers::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  list_users_view->clear();
  sql=QString("select ")+
    "ADMIN_CONFIG_PRIV,"+  // 00
    "ADMIN_RSS_PRIV,"+     // 01
    "LOGIN_NAME,"+         // 02
    "FULL_NAME,"+          // 03
    "DESCRIPTION,"+        // 04
    "EMAIL_ADDRESS,"+      // 05
    "PHONE_NUMBER,"+       // 06
    "LOCAL_AUTH "+         // 07
    "from USERS";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_users_view);
    if(q->value(0).toString()=="Y") {
      item->setPixmap(0,*list_admin_map);
    }
    else {
      if(q->value(1).toString()=="Y") {
	item->setPixmap(0,*list_rss_map);
      }
      else {
	if(q->value(7).toString()=="Y") {
	  item->setPixmap(0,*list_localuser_map);
	}
	else {
	  item->setPixmap(0,*list_user_map);
	}
      }
    }
    item->setText(1,q->value(2).toString());
    item->setText(2,q->value(3).toString());
    item->setText(3,q->value(4).toString());
    item->setText(4,q->value(5).toString());
    item->setText(5,q->value(6).toString());
    item->setText(6,q->value(7).toString());
  }
  delete q;
}


void ListUsers::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "ADMIN_CONFIG_PRIV,"+  // 00
    "ADMIN_RSS_PRIV,"+     // 01
    "FULL_NAME,"+          // 02
    "DESCRIPTION,"+        // 03
    "EMAIL_ADDRESS,"+      // 04
    "PHONE_NUMBER,"+       // 05
    "LOCAL_AUTH "+         // 06
    "from USERS where "+
    "LOGIN_NAME=\""+RDEscapeString(item->text(1))+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toString()=="Y") {
      item->setPixmap(0,*list_admin_map);
    }
    else {
      if(q->value(1).toString()=="Y") {
	item->setPixmap(0,*list_rss_map);
      }
      else {
	if(q->value(6).toString()=="Y") {
	  item->setPixmap(0,*list_localuser_map);
	}
	else {
	  item->setPixmap(0,*list_user_map);
	}
      }
    }
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
    item->setText(4,q->value(4).toString());
    item->setText(5,q->value(5).toString());
    item->setText(6,q->value(6).toString());
  }
  delete q;
}
