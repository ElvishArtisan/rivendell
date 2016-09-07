// edit_gpi.h
//
// Edit a Rivendell GPI
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

#ifndef EDIT_GPI_H
#define EDIT_GPI_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rduser.h>
#include <rdmatrix.h>

class EditGpi : public QDialog
{
 Q_OBJECT
 public:
 EditGpi(RDMatrix *matrix,RDMatrix::GpioType type,int gpi,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectOnData();
  void clearOnData();
  void selectOffData();
  void clearOffData();
  void okData();
  void cancelData();

 protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  RDMatrix *edit_matrix;
  RDMatrix::GpioType edit_type;
  int edit_gpi;
  QString edit_table;
  QLabel *edit_onsection_label;
  QLabel *edit_onmacro_label;
  QLineEdit *edit_onmacro_edit;
  QLabel *edit_ondescription_label;
  QLineEdit *edit_ondescription_edit;
  QPushButton *edit_onselect_button;
  QPushButton *edit_onclear_button;
  QLabel *edit_offsection_label;
  QLabel *edit_offmacro_label;
  QLineEdit *edit_offmacro_edit;
  QLabel *edit_offdescription_label;
  QLineEdit *edit_offdescription_edit;
  QPushButton *edit_offselect_button;
  QPushButton *edit_offclear_button;
  QString edit_filter;
  QString edit_group;
  QString *edit_ondescription;
  QString *edit_offdescription;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_GPI

