// add_hostvar.h
//
// Add a Rivendell Host Variable
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

#ifndef ADD_HOSTVAR_H
#define ADD_HOSTVAR_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AddHostvar : public QDialog
{
  Q_OBJECT
 public:
  AddHostvar(QString station,int *id,QWidget *parent=0);
  ~AddHostvar();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *add_name_label;
  QLineEdit *add_name_edit;
  QPushButton *add_ok_button;
  QPushButton *add_cancel_button;
  QString add_station_name;
  int *add_id;
};


#endif  // ADD_HOSTVAR_H
