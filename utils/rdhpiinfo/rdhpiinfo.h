// rdhpiinfo.h
//
// A Qt-based application for display info about ASI cards.
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDHPIINFO_H
#define RDHPIINFO_H

#include <qwidget.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <asihpi/hpi.h>
#if HPI_VER < 0x040600
typedef uint16_t hpi_err_t;
typedef uint32_t hpi_handle_t;
#endif

#define RDHPIINFO_USAGE "\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void nameActivatedData(int id);
  void changeModeData();
  void utilizationData();
  void updateDspUtilization();

 private:
  void LoadAdapters();
  hpi_err_t HpiErr(hpi_err_t err,const char *func_name=0) const;
  QLabel *info_name_label;
  QComboBox *info_name_box;
  QLabel *info_utilization_label;
  QLineEdit *info_utilization_edit;
  QPushButton *info_utilization_button;
  QLabel *info_index_label;
  QLabel *info_serial_label;
  QLabel *info_istreams_label;
  QLabel *info_ostreams_label;
  QLabel *info_dsp_label;
  QLabel *info_adapter_label;
  QLineEdit *info_mode_edit;
  QPushButton *info_changemode_button;
  uint32_t hpi_version;
  QString hpi_name[HPI_MAX_ADAPTERS];
  int name_map[HPI_MAX_ADAPTERS];
  uint32_t hpi_indexes[HPI_MAX_ADAPTERS];
  uint16_t hpi_ostreams[HPI_MAX_ADAPTERS];
  uint16_t hpi_istreams[HPI_MAX_ADAPTERS];
  uint16_t hpi_card_version[HPI_MAX_ADAPTERS];
  uint32_t hpi_serial[HPI_MAX_ADAPTERS];
  uint16_t hpi_type[HPI_MAX_ADAPTERS];
  uint32_t hpi_mode[HPI_MAX_ADAPTERS];
  hpi_handle_t hpi_profile[HPI_MAX_ADAPTERS];
  uint16_t hpi_profile_quan[HPI_MAX_ADAPTERS];
  QTimer *hpi_profile_timer;
};


#endif  // RDHPIINFO_H
