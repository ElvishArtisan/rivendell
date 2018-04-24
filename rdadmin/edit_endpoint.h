// edit_endpoint.h
//
// Edit a Rivendell Endpoint
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdendpoint.h>
#include <rdmatrix.h>
#include <rduser.h>

class EditEndpoint : public QDialog
{
 Q_OBJECT
 public:
 EditEndpoint(RDMatrix *matrix,int endpt,RDMatrix::Endpoint type,
	      QWidget *parent);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDMatrix *edit_matrix;
  int edit_endpt;
  RDEndPoint *edit_endpoint;
  QString edit_table;
  QLabel *edit_endpoint_label;
  QLineEdit *edit_endpoint_edit;
  QLabel *edit_feed_label;
  QLineEdit *edit_feed_edit;
  QLabel *edit_enginenum_label;
  QLineEdit *edit_enginenum_edit;
  QLabel *edit_devicenum_label;
  QLineEdit *edit_devicenum_edit;
  QLabel *edit_mode_label;
  QComboBox *edit_mode_box;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_ENDPOINT

