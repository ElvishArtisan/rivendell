// rdcatch.h
//
// The Event Schedule Manager for Rivendell.
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

#ifndef RDCATCH_H
#define RDCATCH_H

#include <QCheckBox>
#include <QComboBox>

#include <rdcatch_connect.h>
#include <rdtableview.h>
#include <rdtransportbutton.h>
#include <rdwidget.h>

#include "catch_monitor.h"
#include "catchtableview.h"
#include "deckmon.h"
#include "recordlistmodel.h"
#include "vbox.h"

/*
 * Widget Settings
 */
#define RDCATCH_AUDITION_LENGTH 5000
#define RDCATCH_GEOMETRY_FILE ".rdcatch"
#define RDCATCH_MAX_VISIBLE_MONITORS 8
#define RDCATCH_USAGE "[--offline-host-warnings=yes|no]\n"

class CatchConnector
{
 public:
  CatchConnector(RDCatchConnect *conn,const QString &station_name);
  RDCatchConnect *connector() const;
  QString stationName();
  std::vector<unsigned> chan;
  std::vector<unsigned> mon_id;

 private:
  RDCatchConnect *catch_connect;
  QString catch_station_name;
};


class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void resizeData();
  void connectedData(int serial,bool state);
  void nextEventData();
  void addData();
  void editData();
  void deleteData();
  void ripcConnectedData(bool);
  void ripcUserData();
  void statusChangedData(int,unsigned,RDDeck::Status,int,
			 const QString &cutname);
  void monitorChangedData(int serial,unsigned chan,bool state);
  void deckEventSentData(int serial,int chan,int number);
  void scrollButtonData();
  void reportsButtonData();
  void headButtonData();
  void tailButtonData();
  void stopButtonData();
  void initData(bool);
  void playedData(int);
  void playStoppedData(int);
  void meterLevelData(int,int,int,int);
  void abortData(int);
  void monitorData(int);
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void doubleClickedData(const QModelIndex &index);
  void filterChangedData(bool state);
  void filterActivatedData(int id);
  void clockData();
  void midnightData();
  void eventUpdatedData(int id);
  void eventPurgedData(int id);
  void heartbeatFailedData(int id);
  void quitMainWidget();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  int ShowNextEvents(int day,QTime time,QTime *next);
  unsigned AddRecord();
  void ProcessNewRecords(std::vector<int> *adds);
  void EnableScroll(bool state);
  void UpdateScroll();
  int GetMonitor(int serial,int chan);
  int GetConnection(QString station,unsigned chan=0);
  QString GeometryFile();
  void LoadGeometry();
  void SaveGeometry();
  std::vector<CatchMonitor *> catch_monitor;
  Q3ScrollView *catch_monitor_view;
  VBox *catch_monitor_vbox;
  std::vector<CatchConnector *> catch_connect;
  QSqlDatabase *catch_db;
  int catch_audition_stream;
  int catch_play_handle;
  CatchTableView *catch_recordings_view;
  RecordListModel *catch_recordings_model;
  RDTransportButton *catch_head_button;
  RDTransportButton *catch_tail_button;
  RDTransportButton *catch_stop_button;
  QPushButton *catch_add_button;
  QPushButton *catch_edit_button;
  QPushButton *catch_delete_button;
  QPushButton *catch_close_button;
  QPushButton *catch_scroll_button;
  QPushButton *catch_reports_button;
  bool head_playing;
  bool tail_playing;
  QString catch_filter;
  QString catch_group;
  QString catch_schedcode;
  QTimer *catch_next_timer;
  QPalette catch_scroll_color[2];
  bool catch_scroll;
  QLabel *catch_clock_label;
  QTimer *catch_clock_timer;
  QLabel *catch_show_active_label;
  QCheckBox *catch_show_active_box;
  QLabel *catch_show_today_label;
  QCheckBox *catch_show_today_box;
  QLabel *catch_dow_label;
  QComboBox *catch_dow_box;
  QLabel *catch_type_label;
  QComboBox *catch_type_box;
  QString catch_filter_string;
  QTimer *catch_midnight_timer;
  int catch_time_offset;
  bool catch_host_warnings;
  bool catch_resize;
};


#endif  // RDCATCH_H
