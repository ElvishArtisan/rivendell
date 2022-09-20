// edit_download.h
//
// Edit a Rivendell Download Event
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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


#include <QCheckBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QSpinBox>

#include <rdcombobox.h>
#include <rdcut_dialog.h>
#include <rddeck.h>
#include <rddialog.h>
#include <rdrecording.h>
#include <rdstationlistmodel.h>

#include "dowselector.h"
#include "eventwidget.h"

class EditDownload : public RDDialog
{
 Q_OBJECT
 public:
  EditDownload(QString *filter,QWidget *parent=0);
  ~EditDownload();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(int record_id,std::vector<int> *adds);

 private slots:
  void urlChangedData(const QString &str);
  void useIdFileData(bool state);
  void selectCartData();
  void autotrimToggledData(bool state);
  void normalizeToggledData(bool state);
  void saveasData();
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void closeEvent(QCloseEvent *e);
  
 private:
  void Save();
  bool CheckEvent(bool include_myself);
  RDDeck *edit_deck;
  RDRecording *edit_recording;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QLabel *edit_url_label;
  QLineEdit *edit_url_edit;
  QLabel *edit_username_label;
  QLineEdit *edit_username_edit;
  QLabel *edit_password_label;
  QLineEdit *edit_password_edit;
  QCheckBox *edit_use_id_file_check;;
  QLabel *edit_use_id_file_label;
  QString edit_cutname;
  QLabel *edit_destination_label;
  QLineEdit *edit_destination_edit;
  QPushButton *edit_destination_button;
  QLabel *edit_metadata_label;
  QCheckBox *edit_metadata_box;
  QLabel *edit_eventoffset_label;
  QSpinBox *edit_eventoffset_spin;
  QLabel *edit_eventoffset_unit;
  QCheckBox *edit_oneshot_box;
  QLabel *edit_oneshot_label;
  std::vector<int> *edit_added_events;
  QLabel *edit_channels_label;
  QComboBox *edit_channels_box;
  QLabel *edit_autotrim_label_label;
  QCheckBox *edit_autotrim_box;
  QLabel *edit_autotrim_label;
  QSpinBox *edit_autotrim_spin;
  QLabel *edit_autotrim_unit;
  QLabel *edit_normalize_label_label;
  QCheckBox *edit_normalize_box;
  QLabel *edit_normalize_label;
  QSpinBox *edit_normalize_spin;
  QLabel *edit_normalize_unit;
  QPushButton *edit_saveas_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  QString *edit_filter;
  QString edit_group;
  QString edit_schedcode;
  EventWidget *edit_event_widget;
  DowSelector *edit_dow_selector;
};


#endif  // EDIT_DOWNLOAD_H
