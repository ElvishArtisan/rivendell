// list_livewiregpios.h
//
// List Rivendell Livewire GPIO Slot Associations
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpushbutton.h>

#include <rd.h>
#include <rddialog.h>
#include <rdgpioslotsmodel.h>
#include <rdmatrix.h>
#include <rdtableview.h>

#include "edit_livewiregpio.h"

class ListLiveWireGpios : public RDDialog
{
 Q_OBJECT
 public:
  ListLiveWireGpios(RDMatrix *matrix,int slot_quan,QWidget *parent=0);
  ~ListLiveWireGpios();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
  private slots:
   void editData();
   void doubleClickedData(const QModelIndex &index);
   void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

  private:
   QPushButton *list_edit_button;
   QPushButton *list_close_button;
   RDTableView *list_view;
   RDGpioSlotsModel *list_model;
   RDMatrix *list_matrix;
   int list_slot_quan;
   EditLiveWireGpio *list_gpio_dialog;
};


#endif  // LIST_LIVEWIREGPIOS_H
