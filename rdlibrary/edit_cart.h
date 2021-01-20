// edit_cart.h
//
// Edit a Rivendell Cart
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_CART_H
#define EDIT_CART_H

#include <QList>

#include <rddialog.h>
#include <rdgrouplistmodel.h>

#include "audio_cart.h"
#include "audio_controls.h"
#include "globals.h"
#include "macro_cart.h"

class EditCart : public RDDialog
{
  Q_OBJECT
 public:
  EditCart(const QList<unsigned> &cartnums,QString *path,bool new_cart,
	   bool profile_rip,QWidget *parent=0);
  ~EditCart();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void notesData();
  void scriptData();
  void lengthChangedData(unsigned len);
  void okData();
  void cancelData();
  void forcedLengthData(bool);
  void asyncronousToggledData(bool state);
  void cartDataChangedData();
  void schedCodesData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  QList<unsigned> rdcart_cart_numbers;
  bool ValidateLengths();
  RDCart *rdcart_cart;
  QLineEdit *rdcart_type_edit;
  QLineEdit *rdcart_number_edit;
  QComboBox *rdcart_group_box;
  RDGroupListModel *rdcart_group_model;
  QLineEdit *rdcart_group_edit;
  AudioControls rdcart_controls;
  QComboBox *rdcart_cut_sched_box;
  QLineEdit *rdcart_cut_sched_edit;
  QCheckBox *rdcart_syncronous_box;
  QCheckBox *rdcart_use_event_length_box;
  QLabel *rdcart_syncronous_label;
  QLineEdit *rdcart_start_date_edit;
  QLineEdit *rdcart_end_date_edit;
  QLineEdit *rdcart_average_length_edit;
  QLabel *rdcart_forced_length_label;
  QLineEdit *rdcart_forced_length_ledit;
  unsigned rdcart_average_length;
  unsigned rdcart_length_deviation;
  AudioCart *rdcart_audio_cart;
  MacroCart *rdcart_macro_cart;
  QString *rdcart_import_path;
  bool rdcart_new_cart;
  QComboBox *rdcart_usage_box;
  QLineEdit *rdcart_usage_edit;
  QPushButton *rdcart_notes_button;
  QString sched_codes;
  QString add_codes;
  QString remove_codes;
  bool rdcart_profile_rip;
};

#endif  // EDIT_CART_H
