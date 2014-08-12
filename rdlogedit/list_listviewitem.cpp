//   list_listviewitem.cpp
//
//   A QListViewItem class for RDLogEdit.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_listviewitem.cpp,v 1.6 2010/07/29 19:32:37 cvs Exp $
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

#include <qfontmetrics.h>
#include <qpainter.h>

#include <list_listviewitem.h>

#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"
#include "../icons/whiteball.xpm"


ListListViewItem::ListListViewItem(QListView *parent)
  : QListViewItem(parent)
{
  list_track_column=-1;
  list_tracks=-1;
  list_total_tracks=0;
  list_parent=parent;

  //
  // Create Icons
  //
  list_whiteball_map=new QPixmap(whiteball_xpm);
  list_greenball_map=new QPixmap(greenball_xpm);
  list_redball_map=new QPixmap(redball_xpm);
}


int ListListViewItem::trackColumn() const
{
  return list_track_column;
}


void ListListViewItem::setTrackColumn(int col)
{
  list_track_column=col;
}


int ListListViewItem::tracks() const
{
  return list_tracks;
}


void ListListViewItem::setTracks(int quan)
{
  list_tracks=quan;
}


int ListListViewItem::totalTracks() const
{
  return list_total_tracks;
}


void ListListViewItem::setTotalTracks(int quan)
{
  list_total_tracks=quan;
}


void ListListViewItem::paintCell(QPainter *p,const QColorGroup &cg,int column,
				 int width,int align)
{
  if(column!=list_track_column) {
    QListViewItem::paintCell(p,cg,column,width,align);
    return;
  }
  QColor fg=cg.text();
  QColor bg=cg.base();
  if(isSelected()) {
    fg=cg.highlightedText();
    bg=cg.highlight();
  }
  QString str=QString().sprintf("%u / %u",list_tracks,list_total_tracks);
  QPixmap *icon=list_whiteball_map;
  if(list_total_tracks>0) {
    if(list_tracks==list_total_tracks) {
      icon=list_greenball_map;
    }
    else {
      icon=list_redball_map;
    }
  }
  QFontMetrics *m=new QFontMetrics(p->font());
  p->setBackgroundColor(bg);
  p->eraseRect(0,0,width,height());
  p->setPen(fg);
  p->drawPixmap(list_parent->itemMargin(),(height()-icon->size().height())/2,
		*icon);
  p->drawText(icon->size().width()+10,3*(height()-m->height())/2,str);
  delete m;
}
