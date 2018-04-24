// List_schedcodes.h
//
// The scheduler codes dialog for rdadmin
//
//   Stefan Gabriel <stg@st-gabriel.de>
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

#ifndef LIST_SCHEDCODES_H
#define LIST_SCHEDCODES_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>

#include <rdsqltablemodel.h>
#include <rdtableview.h>

class ListSchedCodes : public QDialog
{
  Q_OBJECT
 public:
  ListSchedCodes(QWidget *parent=0);
  ~ListSchedCodes();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *list_box_label;
  RDTableView *list_view;
  RDSqlTableModel *list_model;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
};


#endif  // LIST_SCHEDCODES_H
