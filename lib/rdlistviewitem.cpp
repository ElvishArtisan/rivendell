//   rdlistviewitem.cpp
//
//   A color-selectable QListViewItem class for Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlistviewitem.cpp,v 1.20.6.1 2013/02/21 02:46:23 cvs Exp $
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

#include <qpainter.h>
#include <qstringlist.h>

#include <rdconf.h>
#include <rdlistviewitem.h>


RDListViewItem::RDListViewItem(QListView *parent) 
  : QListViewItem(parent)
{
  item_line=-1;
  item_id=-1;
  list_parent=(RDListView *)parent;
  item_background_color=
    listView()->palette().color(QPalette::Active,QColorGroup::Base);
  for(int i=0;i<parent->columns();i++) {
    item_text_color.
      push_back(parent->palette().color(QPalette::Active,QColorGroup::Text));
    item_text_weight.push_back(parent->font().weight());
  }
}


int RDListViewItem::line() const
{
  return item_line;
}


void RDListViewItem::setLine(int line)
{
  item_line=line;
}


int RDListViewItem::id() const
{
  return item_id;
}


void RDListViewItem::setId(int id)
{
  item_id=id;
}


QColor RDListViewItem::backgroundColor() const
{
  return item_background_color;
}


void RDListViewItem::setBackgroundColor(QColor color)
{
  item_background_color=color;
  listView()->repaintItem(this);
}


void RDListViewItem::setTextColor(QColor color)
{
  for(unsigned i=0;i<item_text_color.size();i++) {
    item_text_color[i]=color;
  }
  listView()->repaintItem(this);
}


QColor RDListViewItem::textColor(int column) const
{
  return item_text_color[column];
}


void RDListViewItem::setTextColor(int column,QColor color,int weight)
{
  item_text_color[column]=color;
  item_text_weight[column]=weight;
  listView()->repaintItem(this);
}


void RDListViewItem::paintCell(QPainter *p,const QColorGroup &cg,int column,
			       int width,int align)
{
  QColor text_color=item_text_color[column];
  QColor back_color=item_background_color;
  int x=0;
  int y=0;

  if(item_text_weight[column]!=p->font().weight()) {
    int pt_size=0;
    if((pt_size=p->font().pointSize())<0) {
      pt_size=p->font().pixelSize();
    }
    QFont f(p->font().family(),pt_size,item_text_weight[column]);
    f.setPixelSize(pt_size);
    p->setFont(f);
  }
  if(isSelected()&&((column==0)||listView()->allColumnsShowFocus())) {
    text_color=cg.highlightedText();
    back_color=cg.highlight();
  }
  p->fillRect(0,0,width,height(),back_color);
  if(pixmap(column)==NULL) {
    for(int i=0;i<listView()->columns();i++) {
      if(!text(i).isEmpty()) {
	y=(height()+p->fontMetrics().boundingRect(text(i)).height())/2;
	i=listView()->columns();
      }
    }
    x=listView()->itemMargin();
    if(((align&AlignHCenter)!=0)||((align&AlignCenter)!=0)) {
      x=(width-p->fontMetrics().width(text(column)))/2;
    }
    if((align&AlignRight)!=0) {
      x=width-p->fontMetrics().width(text(column))-listView()->itemMargin();
    }
    p->setPen(text_color);
    p->drawText(x,y,text(column));
  }
  else {
    x=listView()->itemMargin();
    y=(height()-pixmap(column)->height())/2;
    if((align&AlignRight)!=0) {
      x=width-pixmap(column)->width()-listView()->itemMargin();
    }
    if(((align&AlignHCenter)!=0)||((align&AlignCenter)!=0)) {
      x=(width-pixmap(column)->width())/2;
    }
    p->drawPixmap(x,y,*pixmap(column));
  }
}


int RDListViewItem::compare(QListViewItem *i,int col,bool ascending) const
{
  int hard_column;
  int prev_length;
  int length;
  QStringList fields;
  QStringList prev_fields;

  if((hard_column=list_parent->hardSortColumn())<0) {
    switch(list_parent->columnSortType(col)) {
	case RDListView::TimeSort:
	  prev_length=RDSetTimeLength(i->text(col));
	  length=RDSetTimeLength(text(col));
	  if(length<prev_length) {
	    return -1;
	  }
	  if(length>prev_length) {
	    return 1;
	  }
	  return 0;

	case RDListView::LineSort:
	  if(line()<((RDListViewItem *)i)->line()) {
	    return -1;
	  }
	  if(line()>((RDListViewItem *)i)->line()) {
	    return 1;
	  }
	  return 0;

	case RDListView::GpioSort:
	  fields=fields.split("-",text(col));
	  prev_fields=fields.split("-",i->text(col));
	  if(fields[0].toInt()>prev_fields[0].toInt()) {
	    return 1;
	  }
	  if(fields[0].toInt()<prev_fields[0].toInt()) {
	    return -1;
	  }
	  return 0;

	case RDListView::NormalSort:
	  return QListViewItem::compare(i,col,ascending);
    }
  }
  if(ascending) {
    int this_count=text(hard_column).toInt();
    if(this_count<0) {
      return 1;
    }
    int that_count=i->text(hard_column).toInt();
    if(that_count<0) {
      return -1;
    }
    if(this_count>that_count) {
      return 1;
    }
    if(this_count<that_count) {
      return -1;
    }
    return 0;
  }
  else {
    int this_count=text(hard_column).toInt();
    if(this_count<0) {
      return -1;
    }
    int that_count=i->text(hard_column).toInt();
    if(that_count<0) {
      return 1;
    }
    if(this_count>that_count) {
      return -1;
    }
    if(this_count<that_count) {
      return 1;
    }
    return 0;
  }
}
