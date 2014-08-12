// edit_gpi.h
//
// Edit a Rivendell GPI
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_gpi.h,v 1.8 2010/07/29 19:32:34 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <rduser.h>
#include <rdmatrix.h>


class EditGpi : public QDialog
{
 Q_OBJECT
 public:
  EditGpi(int gpi,int *oncart,QString *ondesc,int *offcart,QString *offdesc,
	  QWidget *parent=0,const char *name=0);
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
  void paintEvent(QPaintEvent *e);
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
};


#endif  // EDIT_GPI

