// edit_cart.h
//
// Edit a Rivendell Cart
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_cart.h,v 1.30.2.2 2014/01/13 23:02:42 cvs Exp $
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

#include <qdialog.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <globals.h>
#include <qdatetimeedit.h>

#include <rdcart.h>
#include <rdlibrary_conf.h>

#include <audio_cart.h>
#include <audio_controls.h>
#include <macro_cart.h>


class EditCart : public QDialog
{
  Q_OBJECT
 public:
  EditCart(unsigned number,QString *path,bool new_cart,bool profile_rip,
	   QWidget *parent=0,const char *name=0,QListView *lib_cart_list=NULL);
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
  void PopulateGroupList();
  QListView *lib_cart_list_edit;
  bool ValidateLengths();
  RDCart *rdcart_cart;
  QLineEdit *rdcart_type_edit;
  QLineEdit *rdcart_number_edit;
  QComboBox *rdcart_group_box;
  QLineEdit *rdcart_group_edit;
  AudioControls rdcart_controls;
  QCheckBox *rdcart_syncronous_box;
  QCheckBox *rdcart_use_event_length_box;
  QLabel *rdcart_syncronous_label;
  QLineEdit *rdcart_start_date_edit;
  QLineEdit *rdcart_end_date_edit;
  QLineEdit *rdcart_average_length_edit;
  QLabel *rdcart_forced_length_label;
  QLineEdit *rdcart_forced_length_ledit;
  QCheckBox *rdcart_preserve_pitch_button;
  QLabel *rdcart_preserve_pitch_label;
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

#endif

