// edit_hostvar.h
//
// Edit a Rivendell Host Variable
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

#ifndef EDIT_HOSTVAR_H
#define EDIT_HOSTVAR_H

#include <QLineEdit>

#include <rddialog.h>

class EditHostvar : public RDDialog
{
  Q_OBJECT
 public:
  EditHostvar(QWidget *parent=0);
  ~EditHostvar();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:  
  int exec(int id);

 private slots:
  void okData();
  void cancelData();

 private:
  int edit_id;
  QLineEdit *edit_name_edit;
  QLineEdit *edit_varvalue_edit;
  QLineEdit *edit_remark_edit;
};


#endif  // EDIT_HOSTVAR_H
