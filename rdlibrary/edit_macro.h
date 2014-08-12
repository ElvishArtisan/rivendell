// edit_macro.h
//
// Edit a Rivendell Macro
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_macro.h,v 1.8.8.2 2013/12/23 22:04:03 cvs Exp $
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

#ifndef EDIT_MACRO_H
#define EDIT_MACRO_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <rdcart.h>
#include <rdmacro.h>


class EditMacro : public QDialog
{
  Q_OBJECT
 public:
  EditMacro(RDMacro *cmd,bool highlight,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  QLineEdit *edit_macro_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  RDMacro *edit_macro;
};


#endif

