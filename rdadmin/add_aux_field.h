// add_aux_field.h
//
// Add an Auxiliary Field for an RSS Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ADD_AUX_FIELD_H
#define ADD_AUX_FIELD_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AddAuxField : public QDialog
{
 Q_OBJECT
 public:
  AddAuxField(unsigned feed_id,unsigned *field_id,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *add_varname_label;
  QLabel *add_varname_label1;
  QLabel *add_varname_label2;
  QLineEdit *add_varname_edit;
  QLabel *add_caption_label;
  QLineEdit *add_caption_edit;
  QPushButton *add_ok_button;
  QPushButton *add_cancel_button;
  unsigned add_feed_id;
  unsigned *add_field_id;
};


#endif  // ADD_AUX_FIELD

