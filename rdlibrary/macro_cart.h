// macro_cart.h
//
// The macro cart editor for RDLibrary.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: macro_cart.h,v 1.10 2010/07/29 19:32:36 cvs Exp $
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

#include <qwidget.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

#include <globals.h>

#include <rdcart.h>
#include <rdlibrary_conf.h>
#include <rdmacro_event.h>


class MacroCart : public QWidget
{
  Q_OBJECT
 public:
  MacroCart(RDCart *cart,QWidget *parent=0,const char *name=0);
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
  void doubleClickedData(QListViewItem *,const QPoint &,int);  

 private:
  void RefreshList();
  void RefreshLine(QListViewItem *item);
  void AddLine(unsigned line,RDMacro *cmd);
  void DeleteLine(QListViewItem *item);
  void UpdateLength();
  RDCart *rdcart_cart;
  QListView *rdcart_macro_list;
  QLabel *rdcart_macro_list_label;
  RDMacroEvent *rdcart_events;
  RDMacro rdcart_clipboard;
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


#endif

