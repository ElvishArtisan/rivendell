// list_users.cpp
//
// List Rivendell Users
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_users.cpp,v 1.24 2010/07/29 19:32:35 cvs Exp $
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
#include <rddb.h>

#include <rdcart.h>
#include <rdtextfile.h>
#include <rdescape_string.h>

#include <list_users.h>
#include <edit_user.h>
#include <add_user.h>

//
// Icons
//
#include "../icons/admin.xpm"
#include "../icons/user.xpm"

ListUsers::ListUsers(const QString &admin_name,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_admin_name=admin_name;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("Rivendell User List"));

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
  list_add_button=new QPushButton(this,"list_add_button");
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this,"list_edit_button");
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this,"list_delete_button");
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this,"list_close_button");
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // User List
  //
  list_users_view=new RDListView(this,"list_users_view");
  list_users_view->setFont(list_font);
  list_users_view->setAllColumnsShowFocus(true);
  list_users_view->setItemMargin(5);
  list_users_view->addColumn("");
  list_users_view->addColumn(tr("USER NAME"));
  list_users_view->addColumn(tr("FULL NAME"));
  list_users_view->addColumn(tr("DESCRIPTION"));
  QLabel *list_box_label=new QLabel(list_users_view,tr("&Users:"),
				    this,"list_box_label");
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
  connect(list_users_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

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

  AddUser *add_user=new AddUser(&user,this,"add_user");
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
  EditUser *edit_user=new EditUser(item->text(1),this,"edit_user");
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
  sql=QString().sprintf("select NAME from STATIONS where DEFAULT_NAME=\"%s\"",
			(const char *)username);
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
			     (const char *)item->text(1));
  switch(QMessageBox::warning(this,tr("Delete User"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
      case QMessageBox::No:
      case QMessageBox::NoButton:
	return;

      default:
	break;
  }

  //
  // Delete RSS Feed Perms
  //
  sql=QString().sprintf("delete from FEED_PERMS where USER_NAME=\"%s\"",
			(const char *)username);
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete Member User Perms
  //
  sql=QString().sprintf("delete from USER_PERMS where USER_NAME=\"%s\"",
			(const char *)username);
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete from User List
  //
  sql=QString().sprintf("delete from USERS where LOGIN_NAME=\"%s\"",
			(const char *)username);
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete from Cached Web Connections
  //
  sql=QString().sprintf("delete from WEB_CONNECTIONS where LOGIN_NAME=\"%s\"",
			(const char *)username);
  q=new RDSqlQuery(sql);
  delete q;

  item->setSelected(false);
  delete item;
}


void ListUsers::doubleClickedData(QListViewItem *item,const QPoint &pt,
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
  sql="select ADMIN_CONFIG_PRIV,LOGIN_NAME,FULL_NAME,DESCRIPTION from USERS";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    item=new RDListViewItem(list_users_view);
    if(q->value(0).toString()=="Y") {
      item->setPixmap(0,*list_admin_map);
    }
    else {
      item->setPixmap(0,*list_user_map);
    }
    item->setText(1,q->value(1).toString());
    item->setText(2,q->value(2).toString());
    item->setText(3,q->value(3).toString());
  }
  delete q;
}


void ListUsers::RefreshItem(RDListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select ADMIN_CONFIG_PRIV,FULL_NAME,DESCRIPTION \
                         from USERS where LOGIN_NAME=\"%s\"",
			(const char *)RDEscapeString(item->text(1)));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toString()=="Y") {
      item->setPixmap(0,*list_admin_map);
    }
    else {
      item->setPixmap(0,*list_user_map);
    }
    item->setText(2,q->value(1).toString());
    item->setText(3,q->value(2).toString());
  }
  delete q;
}
