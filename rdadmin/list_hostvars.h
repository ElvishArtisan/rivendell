// list_hostvars.h
//
// List Rivendell Host Variables
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_hostvars.h,v 1.5 2010/07/29 19:32:35 cvs Exp $
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

#ifndef LIST_HOSTVARS_H
#define LIST_HOSTVARS_H

#include <qdialog.h>
#include <qlistview.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>

#include <rd.h>
#include <rdmatrix.h>


class ListHostvars : public QDialog
{
 Q_OBJECT
 public:
  ListHostvars(QString station,QWidget *parent=0,const char *name=0);
  ~ListHostvars();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
  private slots:
   void addData();
   void editData();
   void deleteData();
   void doubleClickedData(QListViewItem *,const QPoint &,int);
   void okData();
   void cancelData();

  private:
   void RefreshList();
   QListView *list_view;
   QString list_station;
};


#endif

