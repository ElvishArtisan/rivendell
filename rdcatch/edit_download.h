// edit_download.h
//
// Edit a Rivendell Download Event
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_download.h,v 1.10 2010/07/29 19:32:36 cvs Exp $
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

#ifndef EDIT_DOWNLOAD_H
#define EDIT_DOWNLOAD_H

#include <vector>

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <rdrecording.h>
#include <rddeck.h>
#include <rdcart.h>

class EditDownload : public QDialog
{
 Q_OBJECT
 public:
  EditDownload(int id,std::vector<int> *adds,QString *filter,
	       QWidget *parent=0,const char *name=0);
  ~EditDownload();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void urlChangedData(const QString &str);
  void selectCartData();
  void autotrimToggledData(bool state);
  void normalizeToggledData(bool state);
  void saveasData();
  void okData();
  void cancelData();
  
 protected:
  void paintEvent(QPaintEvent *e);
  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *e);
  
 private:
  void Save();
  bool CheckEvent(bool include_myself);
  RDDeck *edit_deck;
  RDRecording *edit_recording;
  QCheckBox *edit_active_button;
  QComboBox *edit_station_box;
  QTimeEdit *edit_starttime_edit;
  QLineEdit *edit_description_edit;
  QLineEdit *edit_url_edit;
  QLabel *edit_username_label;
  QLineEdit *edit_username_edit;
  QLabel *edit_password_label;
  QLineEdit *edit_password_edit;
  QString edit_cutname;
  QLineEdit *edit_destination_edit;
  QCheckBox *edit_metadata_box;
  QCheckBox *edit_sun_button;
  QCheckBox *edit_mon_button;
  QCheckBox *edit_tue_button;
  QCheckBox *edit_wed_button;
  QCheckBox *edit_thu_button;
  QCheckBox *edit_fri_button;
  QCheckBox *edit_sat_button;
  QSpinBox *edit_eventoffset_spin;
  QCheckBox *edit_oneshot_box;
  std::vector<int> *edit_added_events;
  QComboBox *edit_channels_box;
  QCheckBox *edit_autotrim_box;
  QLabel *edit_autotrim_label;
  QSpinBox *edit_autotrim_spin;
  QLabel *edit_autotrim_unit;
  QCheckBox *edit_normalize_box;
  QLabel *edit_normalize_label;
  QSpinBox *edit_normalize_spin;
  QLabel *edit_normalize_unit;
  QString *edit_filter;
};


#endif
