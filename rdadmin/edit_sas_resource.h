// edit_sas_resource.h
//
// Edit an SAS Resource Record.
//
//   (C) Copyright 2002-2005,2011,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_SAS_RESOURCE_H
#define EDIT_SAS_RESOURCE_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdmatrix.h>
#include <rdvguestresource.h>

class EditSasResource : public QDialog
{
 Q_OBJECT
 public:
 EditSasResource(RDMatrix *matrix,int num,QWidget *parent=0);
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDMatrix *edit_matrix;
  int edit_number;
  QLabel *edit_enginenum_label;
  QLineEdit *edit_enginenum_edit;
  QLabel *edit_devicenum_label;
  QLineEdit *edit_devicenum_edit;
  QLabel *edit_relaynum_label;
  QLineEdit *edit_relaynum_edit;
  RDVguestResource *edit_guest;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_SAS_RESOURCE_H

