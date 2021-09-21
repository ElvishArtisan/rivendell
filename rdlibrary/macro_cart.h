// macro_cart.h
//
// The macro cart editor for RDLibrary.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef MACRO_CART_H
#define MACRO_CART_H

#include <QComboBox>
#include <QPushButton>

#include <rdmacro_event.h>
#include <rdmacrocartmodel.h>
#include <rdtableview.h>
#include <rdwidget.h>

#include "edit_macro.h"
#include "globals.h"

class MacroCart : public RDWidget
{
  Q_OBJECT
 public:
  MacroCart(RDCart *cart,QWidget *parent=0);
  ~MacroCart();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  unsigned length();
  void save();

 signals:
  void lengthChanged(unsigned len);

 private slots:
  void addMacroData();
  void deleteMacroData();
  void copyMacroData();
  void pasteMacroData();
  void editMacroData();
  void runLineMacroData();
  void runCartMacroData();
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void doubleClickedData(const QModelIndex &index);  

 private:
  void UpdateLength();
  RDCart *rdcart_cart;
  EditMacro *rdcart_edit_macro_dialog;
  RDTableView *rdcart_macro_view;
  RDMacroCartModel *rdcart_macro_model;
  QLabel *rdcart_macro_list_label;
  RDMacroEvent *rdcart_events;
  QString rdcart_clipboard;
  QPushButton *paste_macro_button;
  QPushButton *rdcart_add_button;
  QPushButton *rdcart_delete_button;
  QPushButton *rdcart_copy_button;
  QPushButton *rdcart_edit_button;
  QPushButton *rdcart_runline_button;
  QPushButton *rdcart_runcart_button;
  unsigned rdcart_length;
  bool rdcart_allow_modification;
};


#endif  // MACRO_CART_H
