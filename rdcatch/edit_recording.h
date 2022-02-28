// edit_recording.h
//
// Edit a Rivendell Netcatch Recording
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

#ifndef EDIT_RECORDING_H
#define EDIT_RECORDING_H

#include <QButtonGroup>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>

#include <rdcut_dialog.h>
#include <rddeck.h>
#include <rddialog.h>
#include <rdrecording.h>
#include <rdtimeedit.h>

#include "eventwidget.h"
#include "dowselector.h"

class EditRecording : public RDDialog
{
 Q_OBJECT
 public:
  EditRecording(QString *filter,QWidget *parent=0);
  ~EditRecording();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:  
  int exec(int id,std::vector<int> *adds);

 private slots:
  void locationChangedData(const QString &station,int decknum);
  void startTypeClickedData(int id);
  void endTypeClickedData(int id);
  void selectCutData();
  void autotrimToggledData(bool state);
  void normalizeToggledData(bool state);
  void saveasData();
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *e);
  
 private:
  void Save();
  bool CheckEvent(bool include_myself);
  QString GetSourceName(int input);
  int GetSource();
  RDDeck *edit_deck;
  //  RDCutDialog *edit_cut_dialog;
  RDRecording *edit_recording;
  EventWidget *edit_event_widget;
  QGroupBox *edit_start_groupbox;
  QRadioButton *edit_start_hardtime_radio;
  QRadioButton *edit_start_gpi_radio;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QString edit_cutname;
  QLabel *edit_destination_label;
  QLineEdit *edit_destination_edit;
  QPushButton *edit_destination_button;
  DowSelector *edit_dow_selector;
  QLabel *edit_source_label;
  QComboBox *edit_source_box;
  QLabel *edit_startoffset_time_label;
  QSpinBox *edit_startoffset_box;
  QLabel *edit_endoffset_label;
  QSpinBox *edit_endoffset_box;
  QCheckBox *edit_oneshot_box;
  QLabel *edit_oneshot_label;
  QButtonGroup *edit_starttype_group;
  QButtonGroup *edit_endtype_group;
  QGroupBox *edit_end_groupbox;
  QRadioButton *edit_end_hardtime_radio;
  QRadioButton *edit_end_length_radio;
  QRadioButton *edit_end_gpi_radio;
  QLabel *edit_endtime_label;
  QLabel *edit_starttime_label;
  RDTimeEdit *edit_starttime_edit;
  RDTimeEdit *edit_endtime_edit;
  QLabel *edit_endlength_label;
  QTimeEdit *edit_endlength_edit;
  QLabel *edit_start_startwindow_label;
  RDTimeEdit *edit_start_startwindow_edit;
  QLabel *edit_start_endwindow_label;
  RDTimeEdit *edit_start_endwindow_edit;
  QCheckBox *edit_multirec_box;
  QLabel *edit_multirec_label;
  QLabel *edit_maxlength_label;
  QTimeEdit *edit_maxlength_edit;
  QLabel *edit_end_startwindow_label;
  RDTimeEdit *edit_end_startwindow_edit;
  QLabel *edit_end_endwindow_label;
  RDTimeEdit *edit_end_endwindow_edit;
  QLabel *edit_startmatrix_label;
  QSpinBox *edit_startmatrix_spin;
  QLabel *edit_startline_label;
  QSpinBox *edit_startline_spin;
  QLabel *edit_endmatrix_label;
  QSpinBox *edit_endmatrix_spin;
  QLabel *edit_endline_label;
  QSpinBox *edit_endline_spin;
  QLabel *edit_startlength_label;
  QTimeEdit *edit_startlength_edit;
  QLabel *edit_startoffset_label;
  QTimeEdit *edit_startoffset_edit;
  std::vector<int> *edit_added_events;
  QLabel *edit_channels_label;
  QComboBox *edit_channels_box;
  QCheckBox *edit_autotrim_box;
  QLabel *edit_autotrim_label;
  QLabel *edit_autotrim_level_label;
  QSpinBox *edit_autotrim_spin;
  QLabel *edit_autotrim_unit;
  QCheckBox *edit_normalize_box;
  QLabel *edit_normalize_label;
  QLabel *edit_normalize_level_label;
  QSpinBox *edit_normalize_spin;
  QLabel *edit_normalize_unit;
  QPushButton *edit_saveas_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  QString *edit_filter;
  QString edit_group;
  QString edit_schedcode;
};


#endif  // EDIT_RECORDING_H
