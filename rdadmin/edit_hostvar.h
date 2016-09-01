// edit_hostvar.h
//
// Edit a Rivendell Host Variable
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_HOSTVAR_H
#define EDIT_HOSTVAR_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdhostvariable.h>

class EditHostvar : public QDialog
{
  Q_OBJECT
 public:
  EditHostvar(int id,QWidget *parent=0);
  ~EditHostvar();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_name_label;
  QLineEdit *edit_name_edit;
  QLabel *edit_varvalue_label;
  QLineEdit *edit_varvalue_edit;
  QLabel *edit_remark_label;
  QLineEdit *edit_remark_edit;
  RDHostVariable *edit_hostvar;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_HOSTVAR_H

