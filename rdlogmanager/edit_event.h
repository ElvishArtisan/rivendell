// edit_event.h
//
// Edit a Rivendell Log Event
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

#ifndef EDIT_EVENT_H
#define EDIT_EVENT_H


#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextEdit>

#include <rddialog.h>
#include <rd3emptycart.h>
#include <rdevent.h>
#include <rdgrouplistmodel.h>
#include <rdlibrarymodel.h>
#include <rdsimpleplayer.h>

#include "importcartsmodel.h"
#include "importcartsview.h"
#include "librarytableview.h"

//
// Layout
//
#define CENTER_LINE 400

class EditEvent : public RDDialog
{
 Q_OBJECT
 public:
  EditEvent(QString eventname,bool new_event,std::vector<QString> *new_events,
	    QWidget *parent=0);
  ~EditEvent();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void filterChangedData(const QString &str);
  void filterActivatedData(const QString &str);
  void filterClickedData(int id);
  void searchData();
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
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
  void saveData();
  void saveAsData();
  void svcData();
  void colorData();
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  void RefreshLibrary();
  void Save();
  void CopyEventPerms(QString old_name,QString new_name);
  void AbandonEvent(QString name);
  QString GetProperties();
  QString event_name;
  RDEvent *event_event;
  QLineEdit *event_lib_filter_edit;
  QButtonGroup *event_lib_type_group;
  QComboBox *event_group_box;
  RDGroupListModel *event_group_model;
  QComboBox *event_sched_group_box;
  QSpinBox* event_artist_sep_spinbox;
  QLabel *event_artist_sep_label;
  QPushButton *event_artist_none_button;
  QSpinBox* event_title_sep_spinbox;
  QLabel *event_title_sep_label;
  QPushButton *event_title_none_button;
  QComboBox* event_have_code_box;
  QLabel *event_have_code_label;
  QComboBox* event_have_code2_box;
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
  QCheckBox *event_autofill_box;
  QCheckBox *event_autofill_slop_box;
  QLabel *event_autofill_slop_label1;
  QLabel *event_autofill_slop_label;
  QTimeEdit *event_autofill_slop_edit;
  QCheckBox *event_timescale_box;
  QGroupBox *event_stack_group;
  ImportCartsView *event_preimport_view;
  ImportCartsModel *event_preimport_model;
  QLineEdit *event_preimport_length_edit;
  RDTransportButton *event_preimport_up_button;
  RDTransportButton *event_preimport_down_button;
  QButtonGroup *event_source_group;
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
  ImportCartsView *event_postimport_view;
  ImportCartsModel *event_postimport_model;
  QLineEdit *event_postimport_length_edit;
  RDTransportButton *event_postimport_up_button;
  RDTransportButton *event_postimport_down_button;
  QPushButton *event_color_button;
  bool event_saved;
  bool event_new_event;
  std::vector<QString> *event_new_events;
  RDSimplePlayer *event_player;
  QTextEdit *event_remarks_edit;
  RD3EmptyCart *event_empty_cart;
};


#endif  // EDIT_EVENT_H
