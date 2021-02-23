// edit_switchevent.h
//
// Edit a Rivendell Netcatch Cart Event
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

#ifndef EDIT_SWITCHEVENT_H
#define EDIT_SWITCHEVENT_H

#include <QCheckBox>
#include <QDateTimeEdit>
#include <QSpinBox>

#include <rdcombobox.h>
#include <rddeck.h>
#include <rddialog.h>
#include <rdmatrix.h>
#include <rdrecording.h>
#include <rdstationlistmodel.h>

class EditSwitchEvent : public RDDialog
{
 Q_OBJECT
 public:
 EditSwitchEvent(int id,std::vector<int> *adds,QWidget *parent=0);
 ~EditSwitchEvent();
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;
 
 private slots:
  void activateStationData(const QString &str);
  void activateMatrixData(const QString &str);
  void activateInputData(const QString &str);
  void activateOutputData(const QString &str);
  void inputChangedData(int value);
  void outputChangedData(int value);
  void saveasData();
  void okData();
  void cancelData();
  
 protected:
  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *e);
  
 private:
  void Save();
  int GetMatrix();
  int GetSource();
  int GetDestination();
  bool CheckEvent(bool include_myself);
  RDMatrix *edit_matrix;
  RDDeck *edit_deck;
  RDRecording *edit_recording;
  QCheckBox *edit_active_button;
  RDComboBox *edit_station_box;
  RDStationListModel *edit_station_model;
  QTimeEdit *edit_starttime_edit;
  QLineEdit *edit_description_edit;
  RDComboBox *edit_matrix_box;
  RDComboBox *edit_input_box;
  QSpinBox *edit_input_spin;
  RDComboBox *edit_output_box;
  QSpinBox *edit_output_spin;
  QCheckBox *edit_sun_button;
  QCheckBox *edit_mon_button;
  QCheckBox *edit_tue_button;
  QCheckBox *edit_wed_button;
  QCheckBox *edit_thu_button;
  QCheckBox *edit_fri_button;
  QCheckBox *edit_sat_button;
  QString *edit_filter;
  QCheckBox *edit_oneshot_box;
  std::vector<int>*edit_added_events;
};


#endif

