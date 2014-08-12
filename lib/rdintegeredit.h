//   rdintegeredit.h
//
//   A widget for editing a list of integer values.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdintegeredit.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDINTEGEREDIT_H
#define RDINTEGEREDIT_H

#include <vector>

#include <qwidget.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>

class RDIntegerEdit : public QWidget
{
  Q_OBJECT
 public:
  RDIntegerEdit(const QString &lbl,int low,int high,
		QWidget *parent,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  unsigned values(std::vector<int> *v) const;
  void setValues(std::vector<int> *v);

 public slots:
  void setGeometry(int x,int y,int w,int h);

 private slots:
  void addData();
  void deleteData();

 private:
  QListBoxItem *GetItem(int value);
  QLabel *edit_label;
  QListBox *edit_values_box;
  QPushButton *edit_add_button;
  QPushButton *edit_delete_button;
  int edit_low;
  int edit_high;
};


#endif  // RDINTEGEREDIT_H
