// edit_recording.h
//
// Edit a Rivendell Netcatch Recording
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_recording.h,v 1.22 2010/07/29 19:32:36 cvs Exp $
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

#include <vector>

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>

#include <rdrecording.h>
#include <rddeck.h>

class EditRecording : public QDialog
{
 Q_OBJECT
 public:
  EditRecording(int id,std::vector<int> *adds,QString *filter,
		QWidget *parent=0,const char *name=0);
  ~EditRecording();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void activateStationData(int,bool use_temp=true);
  void startTypeClickedData(int id);
  void endTypeClickedData(int id);
  void selectCutData();
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
  void PopulateDecks(QComboBox *box);
  void Save();
  bool CheckEvent(bool include_myself);
  QString GetSourceName(int input);
  int GetSource();
  RDDeck *edit_deck;
  RDRecording *edit_recording;
  QCheckBox *edit_active_button;
  QComboBox *edit_station_box;
  QLineEdit *edit_description_edit;
  QString edit_cutname;
  QLineEdit *edit_destination_edit;
  QCheckBox *edit_sun_button;
  QCheckBox *edit_mon_button;
  QCheckBox *edit_tue_button;
  QCheckBox *edit_wed_button;
  QCheckBox *edit_thu_button;
  QCheckBox *edit_fri_button;
  QCheckBox *edit_sat_button;
  QComboBox *edit_source_box;
  QSpinBox *edit_startoffset_box;
  QSpinBox *edit_endoffset_box;
  QCheckBox *edit_oneshot_box;
  QButtonGroup *edit_starttype_group;
  QLabel *edit_starttime_label;
  QButtonGroup *edit_endtype_group;
  QLabel *edit_endtime_label;
  QLabel *edit_endlength_label;
  QTimeEdit *edit_starttime_edit;
  QTimeEdit *edit_endtime_edit;
  QTimeEdit *edit_endlength_edit;
  QLabel *edit_start_startwindow_label;
  QTimeEdit *edit_start_startwindow_edit;
  QLabel *edit_start_endwindow_label;
  QTimeEdit *edit_start_endwindow_edit;
  QLabel *edit_end_startwindow_label;
  QCheckBox *edit_multirec_box;
  QLabel *edit_maxlength_label;
  QTimeEdit *edit_maxlength_edit;
  QTimeEdit *edit_end_startwindow_edit;
  QLabel *edit_end_endwindow_label;
  QTimeEdit *edit_end_endwindow_edit;
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

