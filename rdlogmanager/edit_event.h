// edit_event.h
//
// Edit a Rivendell Log Event
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_event.h,v 1.24.8.1 2013/12/30 20:37:01 cvs Exp $
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

#include <vector>

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qtextedit.h>

#include <rdtransportbutton.h>

#include <rduser.h>
#include <rdemptycart.h>
#include <rdevent.h>
#include <rdevent_line.h>
#include <rdsimpleplayer.h>

#include <lib_listview.h>
#include <import_listview.h>

//
// Layout
//
#define CENTER_LINE 400

class EditEvent : public QDialog
{
 Q_OBJECT
 public:
  EditEvent(QString eventname,bool new_event,std::vector<QString> *new_events,
	    QWidget *parent=0,const char *name=0);
  ~EditEvent();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void filterChangedData(const QString &str);
  void filterActivatedData(const QString &str);
  void filterClickedData(int id);
  void cartClickedData(QListViewItem *item);
  void prepositionToggledData(bool state);
  void timeToggledData(bool);
  void graceClickedData(int);
  void timeTransitionData(int);
  void autofillWarnToggledData(bool);
  void importClickedData(int);
  void preimportChangedData(int size);
  void preimportLengthChangedData(int msecs);
  void preimportUpData();
  void preimportDownData();
  void postimportUpData();
  void postimportDownData();
  void postimportLengthChangedData(int msecs);
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
  void SetPostTransition();
  void Save();
  void CopyEventPerms(QString old_name,QString new_name);
  void AbandonEvent(QString name);
  QString GetProperties();
  QString event_name;
  RDEvent *event_event;
  QLineEdit *event_lib_filter_edit;
  QButtonGroup *event_lib_type_group;
  QComboBox *event_group_box;
  QComboBox *event_sched_group_box;
  QSpinBox* event_title_sep_spinbox;
  QLabel *event_title_sep_label;
  QComboBox* event_have_code_box;
  QLabel *event_have_code_label;
  LibListView *event_lib_list;
  QPixmap *event_playout_map;
  QPixmap *event_macro_map;
  QLabel *event_position_header;
  QLabel *event_position_label;
  QLabel *event_position_unit;
  QCheckBox *event_position_box;
  QTimeEdit *event_position_edit;
  QLabel *event_timetype_header;
  QCheckBox *event_timetype_box;
  QLabel *event_timetype_label;
  QCheckBox *event_post_box;
  QLabel *event_post_label;
  QLabel *event_time_label;
  QLabel *event_grace_label;
  QButtonGroup *event_grace_group;
  QRadioButton *event_immediate_button;
  QRadioButton *event_next_button;
  QRadioButton *event_wait_button;
  QTimeEdit *event_grace_edit;
  QComboBox *event_transtype_box;
  QCheckBox *event_autofill_box;
  QCheckBox *event_autofill_slop_box;
  QLabel *event_autofill_slop_label1;
  QLabel *event_autofill_slop_label;
  QTimeEdit *event_autofill_slop_edit;
  QCheckBox *event_timescale_box;
  ImportListView *event_preimport_list;
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
  QComboBox *event_firsttrans_box;
  QLabel *event_firsttrans_label;
  QLabel *event_firsttrans_unit;
  QComboBox *event_defaulttrans_box;
  QLabel *event_defaulttrans_label;
  QLabel *event_defaulttrans_unit;
  QLabel *event_nestevent_label;
  QComboBox *event_nestevent_box;
  QLabel *event_nestevent_unit;
  ImportListView *event_postimport_list;
  QLineEdit *event_postimport_length_edit;
  RDTransportButton *event_postimport_up_button;
  RDTransportButton *event_postimport_down_button;
  QPushButton *event_color_button;
  bool event_saved;
  bool event_new_event;
  std::vector<QString> *event_new_events;
  RDSimplePlayer *event_player;
  QTextEdit *event_remarks_edit;
  RDEmptyCart *event_empty_cart;
};


#endif

