// edit_cartslots.h
//
// Edit Rivendell CartSlot Configuration
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_cartslots.h,v 1.1.2.5 2013/12/23 18:47:23 cvs Exp $
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

#ifndef EDIT_CARTSLOTS_H
#define EDIT_CARTSLOTS_H

#include <qdialog.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rdstation.h>

class EditCartSlots : public QDialog
{
  Q_OBJECT
 public:
  EditCartSlots(RDStation *station,RDStation *cae_station,
		QWidget *parent=0,const char *name=0);
  ~EditCartSlots();
  QSize sizeHint() const;
 
 private slots:
  void quantityChangedData(int index);
  void slotChangedData(int slotnum);
  void cardChangedData(int card);
  void modeData(int mode);
  void cartActionData(int action);
  void cartSelectData();
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  void ReadSlot(unsigned slotnum);
  void WriteSlot(unsigned slotnum);
  RDStation *edit_station;
  RDStation *edit_cae_station;
  QLabel *edit_slot_columns_label;
  QSpinBox *edit_slot_columns_spin;
  QLabel *edit_slot_rows_label;
  QSpinBox *edit_slot_rows_spin;
  QComboBox *edit_slot_box;
  QComboBox *edit_mode_box;
  QLabel *edit_play_mode_label;
  QComboBox *edit_play_mode_box;
  QLabel *edit_stop_action_label;
  QComboBox *edit_stop_action_box;
  QLabel *edit_cartaction_label;
  QComboBox *edit_cartaction_box;
  QLabel *edit_cart_label;
  QLineEdit *edit_cart_edit;
  QPushButton *edit_cart_button;
  QLabel *edit_service_label;
  QComboBox *edit_service_box;
  QSpinBox *edit_card_spin;
  QSpinBox *edit_input_spin;
  QSpinBox *edit_output_spin;
  int edit_previous_slot;
};


#endif  // EDIT_CARTSLOTS_H
