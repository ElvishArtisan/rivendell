// edit_livewiregpio.h
//
// Edit a Rivendell Livewire GPIO Slot Association
//
//   (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_LIVEWIREGPIO_H
#define EDIT_LIVEWIREGPIO_H

#include <QDialog>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <rdmatrix.h>

class EditLiveWireGpio : public QDialog
{
 Q_OBJECT
 public:
  EditLiveWireGpio(int id,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  int edit_id;
  int edit_slot;
  QLabel *edit_gpiolines_label;
  QLabel *edit_source_number_label;
  QSpinBox *edit_source_number_spin;
  QLabel *edit_ip_address_label;
  QLineEdit *edit_ip_address_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_LIVEWIREGPIO

