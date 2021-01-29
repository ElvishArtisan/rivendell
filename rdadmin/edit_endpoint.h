// edit_endpoint.h
//
// Edit a Rivendell Endpoint
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

#ifndef EDIT_ENDPOINT_H
#define EDIT_ENDPOINT_H

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <rddialog.h>
#include <rdmatrix.h>
#include <rduser.h>

class EditEndpoint : public RDDialog
{
 Q_OBJECT
 public:
  EditEndpoint(RDMatrix::Type type,RDMatrix::Endpoint endpoint,
	       int pointnum,QString *pointname,int *enginenum,int *devicenum,
	       QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDMatrix *mtx,RDMatrix::Endpoint endpt_type,int endpt_id);

 private slots:
  void okData();
  void cancelData();

 private:
  RDMatrix *edit_mtx;
  RDMatrix::Endpoint edit_endpoint_type;
  int edit_pointnum;
  /*
  QString *edit_pointname;
  int *edit_enginenum;
  int *edit_devicenum;
  */
  QString edit_table;
  QLineEdit *edit_endpoint_edit;
  QLabel *edit_enginenum_label;
  QLineEdit *edit_enginenum_edit;
  QLabel *edit_devicenum_label;
  QLineEdit *edit_devicenum_edit;
  int edit_endpoint_id;
};


#endif  // EDIT_ENDPOINT

