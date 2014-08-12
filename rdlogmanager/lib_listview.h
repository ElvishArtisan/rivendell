//   lib_listview.h
//
//   The Library ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: lib_listview.h,v 1.6 2010/07/29 19:32:37 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
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

#ifndef LIB_LISTVIEW_H
#define LIB_LISTVIEW_H

#include <qlistview.h>
#include <qpixmap.h>


class LibListView : public QListView
{
  Q_OBJECT

 public:
  LibListView(QWidget *parent,const char *name=0);

 protected:
  void focusOutEvent(QFocusEvent *e);
  void contentsMousePressEvent(QMouseEvent *e);
};


#endif  // LIB_LISTVIEW_H
