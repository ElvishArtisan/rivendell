//   rdlistview.cpp
//
//   A contiguous-selection only QListView widget for Rivendell
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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
//

#include <stdio.h>
#include <vector>

#include <rdlistview.h>
#include <rdlistviewitem.h>

RDListView::RDListView(QWidget *parent)
  : Q3ListView(parent)
{
  list_contiguous=true;
  list_hard_sort_column=-1;
  connect(this,
	  SIGNAL(mouseButtonClicked(int,Q3ListViewItem *,const QPoint &,int)),
	  this,
	 SLOT(mouseButtonClickedData(int,Q3ListViewItem *,const QPoint &,int)));
}

void RDListView::setContiguous(bool state) {
  list_contiguous=state;
}


bool RDListView::contiguous() const
{
  return list_contiguous;
}


int RDListView::hardSortColumn() const
{
  return list_hard_sort_column;
}


void RDListView::setHardSortColumn(int col)
{
  list_hard_sort_column=col;
}


RDListView::SortType RDListView::columnSortType(int column) const
{
  return sort_type[column];
}


void RDListView::setColumnSortType(int column,SortType type)
{
  sort_type[column]=type;
}


int RDListView::addColumn(const QString &label,int width)
{
  sort_type.push_back(RDListView::NormalSort);
  return Q3ListView::addColumn(label,width);
}


int RDListView::addColumn(const QIcon &iconset,const QString &label,
			  int width)
{
  sort_type.push_back(RDListView::NormalSort);
  return Q3ListView::addColumn(iconset,label,width);
}


void RDListView::selectLine(int line)
{
  RDListViewItem *item=(RDListViewItem *)firstChild();
  while(item!=NULL) {
    if(item->line()==line) {
      setSelected(item,true);
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
}


void RDListView::mouseButtonClickedData(int button,Q3ListViewItem *item,
					const QPoint &pt,int col)
{
  Q3ListViewItem *l;
  bool contiguous;

  if((list_contiguous==false)||(selectionMode()!=Q3ListView::Extended)||(item==NULL)||(button!=1)) {
    return;
  }

  //
  // Get Selected Range
  //
  l=item;
  contiguous=true;
  while((l=l->itemAbove())!=NULL) {
    if(!l->isSelected()) {
      contiguous=false;
    }
    if(!contiguous) {
      setSelected(l,false);
    }
  }
  l=item;
  contiguous=true;
  while((l=l->itemBelow())!=NULL) {
    if(!l->isSelected()) {
      contiguous=false;
    }
    if(!contiguous) {
      setSelected(l,false);
    }
  }
}
