// edit_endpoint.h
//
// Edit a Rivendell Endpoint
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_endpoint.h,v 1.7 2010/07/29 19:32:34 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <rduser.h>
#include <rdmatrix.h>


class EditEndpoint : public QDialog
{
 Q_OBJECT
 public:
  EditEndpoint(RDMatrix::Type type,RDMatrix::Endpoint endpoint,
	       int pointnum,QString *pointname,QString *feedname,
	       RDMatrix::Mode *mode,int *enginenum,int *devicenum,
	       QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  RDMatrix::Type edit_type;
  RDMatrix::Endpoint edit_endpoint;
  int edit_pointnum;
  QString *edit_pointname;
  QString *edit_feedname;
  RDMatrix::Mode *edit_mode;
  int *edit_enginenum;
  int *edit_devicenum;
  QString edit_table;
  QLineEdit *edit_endpoint_edit;
  QLineEdit *edit_feed_edit;
  QLineEdit *edit_enginenum_edit;
  QLineEdit *edit_devicenum_edit;
  QComboBox *edit_mode_box;
};


#endif  // EDIT_ENDPOINT

