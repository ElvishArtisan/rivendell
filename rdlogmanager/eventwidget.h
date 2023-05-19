// eventwidget.h
//
// Widget for editing a LogManager event
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QStringList>
#include <QTextEdit>

#include <rdcombobox.h>
#include <rdemptycart.h>
#include <rdevent.h>
#include <rdgrouplistmodel.h>
#include <rdlibrarymodel.h>
#include <rdsimpleplayer.h>
#include <rdwidget.h>

#include "importcartsmodel.h"
#include "importcartsview.h"
#include "librarytableview.h"

//
// Layout
//
#define CENTER_LINE 400

class EventWidget : public RDWidget
{
 Q_OBJECT
 public:
  EventWidget(QWidget *parent=0);
  ~EventWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QString properties() const;
  void rename(const QString &str);
  void load(RDEvent *evt);
  void save(RDEvent *evt) const;

 private slots:
  void prepositionToggledData(bool state);
  void timeToggledData(bool);
  void graceClickedData(int);
  void timeTransitionData(int);
  void autofillToggledData(bool);
  void autofillWarnToggledData(bool);
  void importClickedData(int);
  void preimportLengthChangedData(int msecs);
  void preimportUpData();
  void preimportDownData();
  void postimportUpData();
  void postimportDownData();
  void postimportLengthChangedData(int msecs);
  void artistData();
  void titleData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  RDEvent *event_event;
  RDComboBox *event_sched_group_box;
  QSpinBox* event_artist_sep_spinbox;
  QLabel *event_artist_sep_label;
  QPushButton *event_artist_none_button;
  QSpinBox* event_title_sep_spinbox;
  QLabel *event_title_sep_label;
  QPushButton *event_title_none_button;
  RDComboBox* event_have_code_box;
  QLabel *event_have_code_label;
  RDComboBox* event_have_code2_box;
  QLabel *event_have_code2_label;
  LibraryTableView *event_lib_view;
  RDLibraryModel *event_lib_model;
  QPushButton *event_search_button;
  QGroupBox *event_position_group;
  QLabel *event_position_label;
  QLabel *event_position_unit;
  QCheckBox *event_position_box;
  QTimeEdit *event_position_edit;
  QGroupBox *event_timetype_group;
  QCheckBox *event_timetype_check;
  QLabel *event_timetype_label;
  QButtonGroup *event_grace_group;
  QGroupBox *event_grace_groupbox;
  QRadioButton *event_immediate_button;
  QRadioButton *event_next_button;
  QRadioButton *event_wait_button;
  QTimeEdit *event_grace_edit;
  QGroupBox *event_autofill_group;
  QLabel *event_autofill_label;
  QCheckBox *event_autofill_box;
  QCheckBox *event_autofill_slop_box;
  QLabel *event_autofill_slop_label1;
  QLabel *event_autofill_slop_label;
  QTimeEdit *event_autofill_slop_edit;
  QLabel *event_timescale_label;
  QCheckBox *event_timescale_box;
  QGroupBox *event_stack_group;
  QLabel *event_preimport_carts_label;
  ImportCartsView *event_preimport_view;
  ImportCartsModel *event_preimport_model;
  QLabel *event_preimport_length_label;
  QLineEdit *event_preimport_length_edit;
  RDTransportButton *event_preimport_up_button;
  RDTransportButton *event_preimport_down_button;
  QRadioButton *event_source_none_radio;
  QRadioButton *event_source_traffic_radio;
  QRadioButton *event_source_music_radio;
  QRadioButton *event_source_scheduler_radio;
  QLabel *event_imports_label;
  QButtonGroup *event_source_group;
  QLabel *event_source_group_none_label;
  QLabel *event_source_group_traffic_label;
  QLabel *event_source_group_music_label;
  QLabel *event_source_group_scheduler_label;
  QTimeEdit *event_startslop_edit;
  QLabel *event_startslop_label;
  QLabel *event_startslop_unit;
  QTimeEdit *event_endslop_edit;
  QLabel *event_endslop_label;
  QLabel *event_endslop_unit;
  QGroupBox *event_transitions_group;
  QComboBox *event_firsttrans_box;
  QLabel *event_firsttrans_label;
  QLabel *event_firsttrans_unit;
  QComboBox *event_defaulttrans_box;
  QLabel *event_defaulttrans_label;
  QLabel *event_defaulttrans_unit;
  QLabel *event_nestevent_label;
  QComboBox *event_nestevent_box;
  QLabel *event_postimport_carts_label;
  ImportCartsView *event_postimport_view;
  ImportCartsModel *event_postimport_model;
  QLabel *event_postimport_length_label;
  QLineEdit *event_postimport_length_edit;
  RDTransportButton *event_postimport_up_button;
  RDTransportButton *event_postimport_down_button;
};


#endif  // EVENTWIDGET_H
