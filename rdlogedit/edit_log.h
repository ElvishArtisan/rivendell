// edit_log.h
//
// Create a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_log.h,v 1.36.8.2 2014/02/20 16:33:55 cvs Exp $
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

#ifndef EDIT_LOG_H
#define EDIT_LOG_H

#include <vector>

#include <qdialog.h>
#include <rdlistview.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include <rdtransportbutton.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rduser.h>
#include <rdgroup_list.h>
#include <rdsimpleplayer.h>

#include <drop_listview.h>
#include <list_reports.h>

//
// Widget Settings
//
#define END_MARKER_ID -2

class EditLog : public QDialog
{
 Q_OBJECT
 public:
  EditLog(QString logname,vector<RDLogLine> *clipboard,
	  vector<QString> *new_logs,QWidget *parent=0,const char *name=0);
  ~EditLog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void descriptionChangedData(const QString &);
  void selectPurgeDateData();
  void serviceActivatedData(const QString &svcname);
  void dateValueChangedData(const QDate &);
  void startDateEnabledData(bool);
  void endDateEnabledData(bool);
  void insertCartButtonData();
  void insertMarkerButtonData();
  void clickedData(QListViewItem *item);
  void selectionChangedData();
  void doubleClickData(QListViewItem *item);
  void editButtonData();
  void deleteButtonData();
  void upButtonData();
  void downButtonData();
  void cutButtonData();
  void copyButtonData();
  void pasteButtonData();
  void cartDroppedData(int line,RDLogLine *ll);
  void saveData();
  void saveasData();
  void reportsData();
  void okData();
  void cancelData();
  
 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *);
  
 private:
  void DeleteLines(int line,int count);
  void SaveLog();
  void RefreshLine(RDListViewItem *item);
  void RefreshList();
  void UpdateSelection();
  void RenumberList(int line);
  bool UpdateColor(RDListViewItem *item,RDLogLine *logline);
  void SelectRecord(int id);
  void UpdateTracks();
  bool DeleteTracks();
  bool ValidateSvc();
  RDListViewItem *SingleSelection();
  RDLog *edit_log;
  RDLogEvent *edit_log_event;
  std::vector<RDLogLine> *edit_clipboard;
  std::vector<unsigned> edit_deleted_tracks;
  std::vector<QString> *edit_newlogs;
  QString edit_logname;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QLabel *edit_service_label;
  QComboBox *edit_service_box;
  QLineEdit *edit_service_edit;
  QLabel *edit_autorefresh_label;
  QComboBox *edit_autorefresh_box;
  QLineEdit *edit_autorefresh_edit;
  QDateEdit *edit_startdate_edit;
  QDateEdit *edit_enddate_edit;
  QLabel *edit_startdate_label;
  QLabel *edit_startdate_box_label;
  QLabel *edit_enddate_box_label;
  QCheckBox *edit_startdate_box;
  QLabel *edit_enddate_label;
  QCheckBox *edit_enddate_box;
  DropListView *edit_log_list;
  QString edit_filter;
  QString edit_group;
  QPixmap *edit_playout_map;
  QPixmap *edit_macro_map;
  QPixmap *edit_marker_map;
  QPixmap *edit_chain_map;
  QPixmap *edit_track_cart_map;
  QPixmap *edit_notemarker_map;
  QPixmap *edit_music_map;
  QPixmap *edit_mic16_map;
  QPixmap *edit_traffic_map;
  QLabel *edit_logname_label_label;
  QLabel *edit_logname_label;
  QLabel *edit_origin_label_label;
  QLabel *edit_origin_label;
  QLabel *edit_track_label_label;
  QLabel *edit_track_label;
  bool edit_changed;
  QPushButton *edit_cart_button;
  QPushButton *edit_marker_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  RDTransportButton *edit_up_button;
  RDTransportButton *edit_down_button;
  QPushButton *edit_cut_button;
  QPushButton *edit_copy_button;
  QPushButton *edit_paste_button;
  QPushButton *edit_save_button;
  QPushButton *edit_saveas_button;
  QPushButton *edit_reports_button;
  RDTransportButton *edit_play_button;
  RDTransportButton *edit_stop_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  RDGroupList edit_group_list;
  RDLogLine::TransType edit_default_trans;
  int edit_output_card;
  int edit_output_port;
  unsigned edit_start_macro;
  unsigned edit_end_macro;
  RDSimplePlayer *edit_player;
  QLabel *edit_time_label;
  QLabel *edit_stoptime_label;
  QLineEdit *edit_stoptime_edit;
  QLabel *edit_endtime_label;
  QLineEdit *edit_endtime_edit;
  QCheckBox *edit_purgedate_box;
  QLabel *edit_purgedate_label;
  QDateEdit *edit_purgedate_edit;
  QPushButton *edit_purgedate_button;
};


#endif

