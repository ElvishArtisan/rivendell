// edit_node.h
//
// Edit a Rivendell LiveWire Node
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_node.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_NODE_H
#define EDIT_NODE_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qdialog.h>

#include <rdmatrix.h>


class EditNode : public QDialog
{
 Q_OBJECT
 public:
  EditNode(int *id,RDMatrix *matrix,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void passwordChangedData(const QString &);
  void viewData();
  void okData();
  void cancelData();

 private:
  int *edit_id;
  RDMatrix *edit_matrix;
  QLineEdit *edit_hostname_edit;
  QSpinBox *edit_tcpport_spin;
  QLineEdit *edit_description_edit;
  QSpinBox *edit_output_spin;
  QLineEdit *edit_password_edit;
  QString edit_password;
  bool edit_password_changed;
};


#endif  // EDIT_NODE

