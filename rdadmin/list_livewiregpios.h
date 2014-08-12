// list_livewiregpios.h
//
// List Rivendell Livewire GPIO Slot Associations
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_livewiregpios.h,v 1.1.2.2 2013/03/05 23:59:07 cvs Exp $
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

#ifndef LIST_LIVEWIREGPIOS_H
#define LIST_LIVEWIREGPIOS_H

#include <qdialog.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdlistview.h>

class ListLiveWireGpios : public QDialog
{
 Q_OBJECT
 public:
  ListLiveWireGpios(RDMatrix *matrix,int slot_quan,
		    QWidget *parent=0,const char *name=0);
  ~ListLiveWireGpios();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
  private slots:
   void editData();
   void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
   void okData();
   void cancelData();

  private:
   void RefreshList();
   RDListView *list_view;
   RDMatrix *list_matrix;
   int list_slot_quan;
};


#endif

