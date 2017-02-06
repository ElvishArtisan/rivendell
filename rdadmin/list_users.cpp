// list_users.cpp
//
// List Rivendell Users
//
//   (C) Copyright 2002-2008,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <QAbstractItemView>
#include <QHeaderView>
#include <QIcon>
#include <QMessageBox>

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
#include "../icons/user.xpm"

ListUsers::ListUsers(const QString &admin_name,QWidget *parent)
  : QDialog(parent)
{
  list_admin_name=admin_name;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin  - "+tr("Rivendell User List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  // Create Icons
  //
  list_admin_map=new QPixmap(admin_xpm);
  list_user_map=new QPixmap(user_xpm);

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
  // User List
  //
  list_users_view=new RDTableWidget(this);
  list_users_view->setFont(list_font);
  list_users_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_users_view->setColumnCount(4);
  QStringList headings = {"",tr("USER NAME"),tr("FULL NAME"),tr("DESCRIPTION")};
  list_users_view->setHorizontalHeaderLabels(headings);
  list_users_view->verticalHeader()->setVisible(false);
  list_users_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  QLabel *list_box_label=new QLabel(list_users_view,tr("&Users:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
  connect(list_users_view,
	  SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	  this,
	  SLOT(doubleClickedData(QTableWidgetItem *)));

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
  QTableWidgetItem *item=new QTableWidgetItem(user);
  int newRow = list_users_view->rowCount();
  list_users_view->insertRow(newRow);
  list_users_view->setItem(newRow,1,item);
  RefreshItem(item);
  item->setSelected(true);
  list_users_view->setCurrentItem(item);
}


void ListUsers::editData()
{
  if(list_users_view->selectedItems().isEmpty()) {
    return;
  }
  QTableWidgetItem *item=list_users_view->selectedItems().at(1);
  EditUser *edit_user=new EditUser(item->text(),this);
  if(edit_user->exec()==0) {
    RefreshItem(item);
  }
  delete edit_user;
}


void ListUsers::deleteData()
{
  if(list_users_view->selectedItems().isEmpty()) {
    return;
  }
  QTableWidgetItem *item=list_users_view->selectedItems().at(1);

  if(list_admin_name==item->text()) {
    QMessageBox::warning(this,tr("Delete User"),
			 tr("You cannot delete yourself!"));
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;

  QString username=RDEscapeString(item->text());

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
  warning+=QString().sprintf("%s \"%s\"?",(const char *)str,
			     (const char *)item->text());
  switch(QMessageBox::warning(this,tr("Delete User"),warning,
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
  list_users_view->removeRow(item->row());
}


void ListUsers::doubleClickedData(QTableWidgetItem *item)
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
  QTableWidgetItem *icon;
  QTableWidgetItem *username;
  QTableWidgetItem *name;
  QTableWidgetItem *description;

  list_users_view->clear();
  QStringList headings = {"",tr("USER NAME"),tr("FULL NAME"),tr("DESCRIPTION")};
  list_users_view->setHorizontalHeaderLabels(headings);
  sql=QString("select ")+
    "ADMIN_CONFIG_PRIV,"+
    "LOGIN_NAME,"+
    "FULL_NAME,"+
    "DESCRIPTION "+
    "from USERS "+
    "order by LOGIN_NAME";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    icon=new QTableWidgetItem();
    if(q->value(0).toString()=="Y") {
      icon->setIcon(QIcon(*list_admin_map));
    }
    else {
      icon->setIcon(QIcon(*list_user_map));
    }
    username = new QTableWidgetItem(q->value(1).toString());
    name = new QTableWidgetItem(q->value(2).toString());
    description = new QTableWidgetItem(q->value(3).toString());

    int newRow = list_users_view->rowCount();
    list_users_view->insertRow(newRow);
    list_users_view->setItem(newRow,0,icon);
    list_users_view->setItem(newRow,1,username);
    list_users_view->setItem(newRow,2,name);
    list_users_view->setItem(newRow,3,description);
  }
  delete q;
}


void ListUsers::RefreshItem(QTableWidgetItem *username)
{
  int row = username->row();
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *icon = list_users_view->item(row,0);
  QTableWidgetItem *name = list_users_view->item(row,2);
  QTableWidgetItem *description = list_users_view->item(row,3);

  sql=QString("select ")+
    "ADMIN_CONFIG_PRIV,"+
    "FULL_NAME,DESCRIPTION "+
    "from USERS where "+
    "LOGIN_NAME=\""+RDEscapeString(username->text())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(!icon) {
      icon = new QTableWidgetItem();
      list_users_view->setItem(row,0,icon);
    }
    if(q->value(0).toString()=="Y") {
      icon->setIcon(QIcon(*list_admin_map));
    }
    else {
      icon->setIcon(QIcon(*list_user_map));
    }
    if(!name) {
      name = new QTableWidgetItem(q->value(1).toString());
      list_users_view->setItem(row,2,name);
    } else {
      name->setText(q->value(1).toString());
    }
    if(!description) {
      description = new QTableWidgetItem(q->value(2).toString());
      list_users_view->setItem(row,3,description);
    } else {
      description->setText(q->value(2).toString());
    }
  }
  delete q;
}
