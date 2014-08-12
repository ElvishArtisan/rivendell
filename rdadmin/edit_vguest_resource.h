// edit_vguest_resource.h
//
// Edit a vGuest Resource Record.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_vguest_resource.h,v 1.5 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_VGUEST_RESOURCE_H
#define EDIT_VGUEST_RESOURCE_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <rduser.h>
#include <rdmatrix.h>


class EditVguestResource : public QDialog
{
 Q_OBJECT
 public:
 EditVguestResource(RDMatrix::VguestType type,int *enginenum,int *devicenum,
		    int *surfacenum,int *relaynum,
		    QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  RDMatrix::VguestType edit_type;
  int *edit_enginenum;
  int *edit_devicenum;
  int *edit_surfacenum;
  int *edit_relaynum;
  QLineEdit *edit_enginenum_edit;
  QLineEdit *edit_devicenum_edit;
  QLineEdit *edit_surfacenum_edit;
  QLabel *edit_relaynum_label;
  QLineEdit *edit_relaynum_edit;
};


#endif  // EDIT_ENDPOINT

