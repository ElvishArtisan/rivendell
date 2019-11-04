// rdgpimon.h
//
// A Qt-based application for testing general purpose input (GPI) devices.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDGPIMON_H
#define RDGPIMON_H

#include <q3datetimeedit.h>

#include <qcombobox.h>

#include <rdmatrix.h>
#include <rdtransportbutton.h>
#include <rdwidget.h>

#include "gpi_label.h"

#define GPIMON_START_UP_DELAY 100
#define GPIMON_ROWS 4
#define GPIMON_COLS 8
#define RDGPIMON_USAGE "\n"

class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
   ~MainWidget();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

 private slots:
  void userData();
  void typeActivatedData(int index);
  void matrixActivatedData(int index);
  void eventsDateChangedData(const QDate &date);
  void eventsStateChangedData(int n);
  void eventsScrollData();
  void eventsReportData();
  void gpiStateChangedData(int matrix,int line,bool state);
  void gpoStateChangedData(int matrix,int line,bool state);
  void gpiMaskChangedData(int matrix,int line,bool state);
  void gpoMaskChangedData(int matrix,int line,bool state);
  void gpiCartChangedData(int matrix,int line,int off_cartnum,int on_cartnum);
  void gpoCartChangedData(int matrix,int line,int off_cartnum,int on_cartnum);
  void startUpData();
  void upData();
  void downData();
  void quitMainWidget();

 private:
  void UpdateLabelsUp(int last_line);
  void UpdateLabelsDown(int first_line);
  void RefreshEventsList();
  void AddEventsItem(int line,bool state);
  RDMatrix *gpi_matrix;
  QComboBox *gpi_type_box;
  QComboBox *gpi_matrix_box;
  QPixmap *gpi_rivendell_map;
  GpiLabel *gpi_labels[GPIMON_ROWS*GPIMON_COLS];
  RDTransportButton *gpi_up_button;
  RDTransportButton *gpi_down_button;
  int gpi_first_line;
  int gpi_last_line;
  QLabel *gpi_events_date_label;
  Q3DateEdit *gpi_events_date_edit;
  QLabel *gpi_events_state_label;
  QComboBox *gpi_events_state_box;
  QPushButton *gpi_events_scroll_button;
  RDListView *gpi_events_list;
  QTimer *gpi_events_startup_timer;
  bool gpi_scroll_mode;
  QPalette gpi_scroll_color;
  QPushButton *gpi_events_report_button;
};


#endif  // RDGPIMON_H
