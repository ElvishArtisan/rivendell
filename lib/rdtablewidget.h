// rdtablewidget.h
//
// Table Widget for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTABLEWIDGET_H
#define RDTABLEWIDGET_H

#include <map>

#include <QTableWidget>

class RDTableWidget : public QTableWidget
{
  Q_OBJECT
 public:
  RDTableWidget(QWidget *parent=0);
  RDTableWidget(int rows,int cols,QWidget *parent=0);
  bool select(int column,unsigned value);
  bool select(int column,const QString &value,bool case_sensitive=true);
  void setItem(int row,int column,QTableWidgetItem *item);

 private:
  void Initialize();
};


#endif  //  RDTABLEVIEW_H
