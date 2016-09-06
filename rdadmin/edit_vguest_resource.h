// edit_vguest_resource.h
//
// Edit a vGuest Resource Record.
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdmatrix.h>
#include <rdvguestresource.h>

class EditVguestResource : public QDialog
{
 Q_OBJECT
 public:
  EditVguestResource(RDMatrix *matrix,RDMatrix::VguestType type,int num,
		     QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  RDMatrix *edit_matrix;
  RDMatrix::VguestType edit_type;
  int edit_number;
  QLineEdit *edit_enginenum_edit;
  QLineEdit *edit_devicenum_edit;
  QLineEdit *edit_surfacenum_edit;
  QLabel *edit_relaynum_label;
  QLineEdit *edit_relaynum_edit;
  RDVguestResource *edit_guest;
};


#endif  // EDIT_VGUEST_RESOURCE
