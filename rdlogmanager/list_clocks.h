// list_clocks.h
//
// List Rivendell Log Clocks
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_clocks.h,v 1.15.8.2 2014/01/10 19:32:54 cvs Exp $
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

#ifndef LIST_CLOCKS_H
#define LIST_CLOCKS_H

#include <vector>

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qcombobox.h>

#include <rduser.h>
#include <rdmatrix.h>
#include <rddb.h>


class ListClocks : public QDialog
{
 Q_OBJECT
 public:
  ListClocks(QString *clockname=NULL,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void editData();
  void deleteData();
  void renameData();
  void doubleClickedData(QListViewItem *,const QPoint &,int);
  void filterActivatedData(int id);
  void closeData();
  void clearData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void RefreshList();
  void RefreshItem(QListViewItem *item,std::vector<QString> *new_clocks=NULL);
  void UpdateItem(QListViewItem *item,QString name);
  void WriteItem(QListViewItem *item,RDSqlQuery *q);
  int ActiveClocks(QString clockname,QString *svc_list);
  void DeleteClock(QString clockname);
  QString GetClockFilter(QString svc_name);
  QString GetNoneFilter();
  QListView *edit_clocks_list;
  QString *edit_clockname;
  QLabel *edit_filter_label;
  QComboBox *edit_filter_box;
  QPushButton *edit_add_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  QPushButton *edit_rename_button;
  QPushButton *edit_close_button;
  QPushButton *edit_clear_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif

