//   rdlistviewitem.h
//
//   A color-selectable QListViewItem class for Rivendell
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLISTVIEWITEM_H
#define RDLISTVIEWITEM_H

#include <vector>

#include <q3listview.h>
#include <qpixmap.h>

#include <rdlistview.h>

class RDListViewItem : public Q3ListViewItem
{
 public:
  RDListViewItem(Q3ListView *parent);
  int line() const;
  void setLine(int line);
  int id() const;
  void setId(int id);
  QColor backgroundColor() const;
  void setBackgroundColor(QColor color);
  QColor textColor(int column) const;
  void setTextColor(QColor color);
  void setTextColor(int column,QColor color,int weight);
  void paintCell(QPainter *p,const QColorGroup &cg,int column,
		 int width,int align);
  int compare(Q3ListViewItem *i,int col,bool ascending) const;

 private:
  int item_line;
  int item_id;
  std::vector<QColor> item_text_color;
  std::vector<int> item_text_weight;
  QColor item_background_color;
  RDListView *list_parent;
};


#endif  // RDLISTVIEWITEM_H
