// edit_event.h
//
// Edit a Rivendell Log Event
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

#ifndef EDIT_EVENT_H
#define EDIT_EVENT_H

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
#include <rddialog.h>
#include <rdemptycart.h>
#include <rdevent.h>
#include <rdgrouplistmodel.h>
#include <rdlibrarymodel.h>
#include <rdsimpleplayer.h>

#include "eventwidget.h"
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
  EditEvent(QString eventname,bool new_event,QStringList *new_events,
	    QStringList *modified_events,QWidget *parent=0);
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
  void saveData();
  void saveAsData();
  void svcData();
  void colorData();
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  void RefreshLibrary();
  void Save();
  void CopyEventPerms(QString old_name,QString new_name);
  void AbandonEvent(QString name);
  QString event_name;
  RDEvent *event_event;
  QLabel *event_lib_filter_label;
  QLineEdit *event_lib_filter_edit;
  QButtonGroup *event_lib_type_group;
  QLabel *event_lib_type_none_label;
  QRadioButton *event_lib_type_none_radio;
  QLabel *event_lib_type_audio_label;
  QRadioButton *event_lib_type_audio_radio;
  QLabel *event_lib_type_macro_label;
  QRadioButton *event_lib_type_macro_radio;
  QLabel *event_group_label;
  QComboBox *event_group_box;
  RDGroupListModel *event_group_model;
  RDComboBox *event_sched_group_box;
  QPushButton *event_search_button;
  LibraryTableView *event_lib_view;
  RDLibraryModel *event_lib_model;
  RDTransportButton *event_postimport_up_button;
  RDTransportButton *event_postimport_down_button;
  QPushButton *event_save_button;
  QPushButton *event_saveas_button;
  QPushButton *event_services_list_button;
  QPushButton *event_color_button;
  QPushButton *event_ok_button;
  QPushButton *event_cancel_button;
  QColor event_color;
  bool event_saved;
  bool event_new_event;
  QStringList *event_new_events;
  QStringList *event_modified_events;
  RDSimplePlayer *event_player;
  QLabel *event_remarks_label;
  QTextEdit *event_remarks_edit;
  RDEmptyCart *event_empty_cart;
  EventWidget *event_widget;
};


#endif  // EDIT_EVENT_H
