// edit_report.h
//
// Edit a Rivendell Report
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_report.h,v 1.14.8.1.2.1 2014/05/22 01:21:36 cvs Exp $
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

#ifndef EDIT_REPORT_H
#define EDIT_REPORT_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>

#include <rdlistselector.h>

#include <rdreport.h>


class EditReport : public QDialog
{
 Q_OBJECT
 public:
  EditReport(QString rptname,QWidget *parent=0,const char *name=0);
  ~EditReport();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void leadingZerosToggled(bool state);
  void genericEventsToggledData(bool state);
  void okData();
  void cancelData();

 private:
  RDReport *edit_report;
  QLineEdit *edit_description_edit;
  QComboBox *edit_filter_box;
  QLineEdit *edit_stationid_edit;
  QLabel *edit_cartdigits_label;
  QSpinBox *edit_cartdigits_spin;
  QCheckBox *edit_cartzeros_box;
  QComboBox *edit_stationtype_box;
  QLineEdit *edit_servicename_edit;
  QLineEdit *edit_stationformat_edit;
  QSpinBox *edit_linesperpage_spin;
  QLineEdit *edit_path_edit;
  QLineEdit *edit_winpath_edit;
  QLabel *edit_traffic_label;
  QCheckBox *edit_traffic_box;
  QLabel *edit_music_label;
  QCheckBox *edit_music_box;
  QCheckBox *edit_generic_box;
  QCheckBox *edit_forcetraffic_box;
  QCheckBox *edit_forcemusic_box;
  QComboBox *edit_onairflag_box;
  RDListSelector *edit_service_sel;
  RDListSelector *edit_station_sel;
  QCheckBox *edit_group_box;
  RDListSelector *edit_group_sel;
  QCheckBox *edit_daypart_check;
  QLabel *edit_daypart_label;
  QLabel *edit_starttime_label;
  QTimeEdit *edit_starttime_edit;
  QLabel *edit_endtime_label;
  QTimeEdit *edit_endtime_edit;
};


#endif

