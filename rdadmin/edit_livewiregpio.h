// edit_livewiregpio.h
//
// Edit a Rivendell Livewire GPIO Slot Association
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QHostAddress>

#include <rddialog.h>
#include <rdmatrix.h>

class EditLiveWireGpio : public RDDialog
{
 Q_OBJECT
 public:
  EditLiveWireGpio(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
   int exec(int slot_id);

 private slots:
  void okData();
  void cancelData();

 private:
  int edit_id;
  QLabel *edit_title_label;
  QSpinBox *edit_source_number_spin;
  QLineEdit *edit_ip_address_edit;
};


#endif  // EDIT_LIVEWIREGPIO

