//   drop_listview.h
//
//   The Log ListView widget for RDLogEdit.
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: drop_listview.h,v 1.1.2.1 2013/12/27 22:12:28 cvs Exp $
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

#ifndef DROP_LISTVIEW_H
#define DROP_LISTVIEW_H

#include <rdlistview.h>
#include <rdlog_line.h>

class DropListView : public RDListView
{
  Q_OBJECT
 public:
  DropListView(QWidget *parent,const char *name=0);

 signals:
  void cartDropped(int line,RDLogLine *ll);

 protected:
  void dragEnterEvent(QDragEnterEvent *e);
  void dropEvent(QDropEvent *e);
};


#endif  // DROP_LISTVIEW_H
