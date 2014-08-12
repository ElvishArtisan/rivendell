// list_encoders.h
//
// List Rivendell Encoders
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_encoders.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#ifndef LIST_ENCODERS_H
#define LIST_ENCODERS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <rdlistviewitem.h>

#include <rduser.h>
#include <rdmatrix.h>


class ListEncoders : public QDialog
{
 Q_OBJECT
 public:
  ListEncoders(const QString &stationname,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void resizeEvent(QResizeEvent *e);

 private slots:
  void addData();
  void editData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void deleteData();
  void closeData();

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  QString BuildList(int encoder_id,const QString &paramname);
  QString list_stationname;
  QLabel *list_list_label;
  RDListView *list_list_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
};


#endif  // LIST_ENCODERS_H
