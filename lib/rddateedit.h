// rddateedit.h
//
// QDateEdit with date-format awareness
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDATEEDIT_H
#define RDDATEEDIT_H

#include <QDateEdit>

class RDDateEdit : public QDateEdit
{
  Q_OBJECT
 public:
  RDDateEdit(QWidget *parent=0);
  bool isReadOnly();

 public slots:
  void setReadOnly(bool state);

 private:
  bool d_read_only;
};


#endif  // RDDATEEDIT_H
