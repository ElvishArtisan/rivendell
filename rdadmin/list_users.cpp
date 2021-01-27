// list_users.cpp
//
// List Rivendell Users
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

ListUsers::ListUsers(const QString &admin_name,QWidget *parent)
  : RDDialog(parent)
{
  list_admin_name=admin_name;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Rivendell User List"));

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
  list_users_view=new RDTableView(this);
  list_users_model=new RDUserListModel(this);
  list_users_model->setFont(defaultFont());
  list_users_model->setPalette(palette());
  list_users_view->setModel(list_users_model);
  connect(list_users_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_users_model,SIGNAL(modelReset()),
	  list_users_view,SLOT(resizeColumnsToContents()));

  list_users_model->setTypeFilter(RDUser::TypeAll);
}


ListUsers::~ListUsers()
{
  delete list_users_view;
  delete list_users_model;
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

  AddUser *d=new AddUser(&user,this);
  if(!d->exec()) {
    delete d;
    return;
  }
  delete d;

  QModelIndex index=list_users_model->addUser(user);
  if(index.isValid()) {
    list_users_view->selectRow(index.row());
    list_users_view->scrollTo(index,QAbstractItemView::PositionAtCenter);
  }
}


void ListUsers::editData()
{
  QModelIndexList rows=list_users_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditUser *d=new EditUser(list_users_model->userName(rows.first()),this);
  if(d->exec()) {
    list_users_model->refresh(rows.first());
  }
  delete d;
}


void ListUsers::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;
  QModelIndexList rows=list_users_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString username=list_users_model->userName(rows.first());
  if(list_admin_name==username) {
    QMessageBox::warning(this,tr("Delete User"),
			 tr("You cannot delete yourself!"));
    return;
  }

  //
  // Check for default user assignments
  //
  sql=QString("select ")+
    "NAME "+  // 00
    "from STATIONS where "+
    "DEFAULT_NAME=\""+RDEscapeString(username)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    str=tr("This user is set as the default user for the following hosts:\n\n");
    do {
      str+=("     "+q->value(0).toString()+"\n");
    } while(q->next());
    str+="\n";
    str+=tr("You must change this before deleting the user.");
    delete q;
    QMessageBox::warning(this,tr("Delete User"),str);
    return;
  }
  delete q;

  str=QString(tr("Are you sure you want to delete user"));
  warning+=str+" \""+username+"\"?";
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete User"),warning,
			  QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }

  //
  // Delete RSS Feed Perms
  //
  sql=QString("delete from FEED_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(username)+"\"";
  RDSqlQuery::apply(sql);
  
  //
  // Delete Member User Perms
  //
  sql=QString("delete from USER_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(username)+"\"";
  RDSqlQuery::apply(sql);
  
  //
  // Delete from User List
  //
  sql=QString("delete from USERS where ")+
    "LOGIN_NAME=\""+RDEscapeString(username)+"\"";
  RDSqlQuery::apply(sql);

  //
  // Delete from Cached Web Connections
  //
  sql=QString("delete from WEB_CONNECTIONS where ")+
    "LOGIN_NAME=\""+RDEscapeString(username)+"\"";
  RDSqlQuery::apply(sql);

  list_users_model->removeUser(username);
}


void ListUsers::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListUsers::closeData()
{
  done(true);
}


void ListUsers::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_users_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
