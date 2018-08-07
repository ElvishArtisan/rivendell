//   list_listviewitem.h
//
//   A QListViewItem class for RDLogEdit.
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_LISTVIEWITEM_H
#define LIST_LISTVIEWITEM_H

#include <q3listview.h>
#include <qpixmap.h>

#include <q3listview.h>

class ListListViewItem : public Q3ListViewItem
{
 public:
  ListListViewItem(Q3ListView *parent);
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
  Q3ListView *list_parent;
};


#endif  // LIST_LISTVIEWITEM_H
