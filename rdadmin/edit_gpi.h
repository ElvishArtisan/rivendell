// edit_gpi.h
//
// Edit a Rivendell GPI
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <rddialog.h>
#include <rdmatrix.h>
#include <rduser.h>

class EditGpi : public RDDialog
{
 Q_OBJECT
 public:
  EditGpi(int gpi,int *oncart,QString *ondesc,int *offcart,QString *offdesc,
	  QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectOnData();
  void clearOnData();
  void selectOffData();
  void clearOffData();
  void okData();
  void cancelData();

 private:
  int edit_gpi;
  int *edit_oncart;
  QLineEdit *edit_ondescription_edit;
  QLineEdit *edit_onmacro_edit;
  int *edit_offcart;
  QLineEdit *edit_offdescription_edit;
  QLineEdit *edit_offmacro_edit;
  QString edit_filter;
  QString edit_group;
  QString *edit_ondescription;
  QString *edit_offdescription;
  QGroupBox *edit_on_group;
  QGroupBox *edit_off_group;
};


#endif  // EDIT_GPI

