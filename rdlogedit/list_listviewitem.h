//   list_listviewitem.h
//
//   A QListViewItem class for RDLogEdit.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_listviewitem.h,v 1.5 2010/07/29 19:32:37 cvs Exp $
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

#ifndef LIST_LISTVIEWITEM_H
#define LIST_LISTVIEWITEM_H

#include <qlistview.h>
#include <qpixmap.h>

#include <qlistview.h>


class ListListViewItem : public QListViewItem
{
 public:
  ListListViewItem(QListView *parent);
  int trackColumn() const;
  void setTrackColumn(int col);
  int tracks() const;
  void setTracks(int quan);
  int totalTracks() const;
  void setTotalTracks(int quan);
  void paintCell(QPainter *p,const QColorGroup &cg,int column,
		 int width,int align);

 private:
  int list_track_column;
  int list_tracks;
  int list_total_tracks;
  QPixmap *list_whiteball_map;
  QPixmap *list_redball_map;
  QPixmap *list_greenball_map;
  QListView *list_parent;
};


#endif  // LIST_LISTVIEWITEM_H
