// edit_log.h
//
// Edit a Rivendell Log
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

#ifndef EDIT_LOG_H
#define EDIT_LOG_H

#include <qgroupbox.h>

#include <rdcart_dialog.h>
#include <rddialog.h>
#include <rdgroup_list.h>
#include <rdloglock.h>

#include "drop_listview.h"
#include "list_reports.h"
#include "render_dialog.h"

//
// Widget Settings
//
#define END_MARKER_ID -2
#define RDLOGEDIT_EDITLOG_DEFAULT_WIDTH 950
#define RDLOGEDIT_EDITLOG_DEFAULT_HEIGHT 600

class EditLog : public RDDialog
{
 Q_OBJECT
 public:
 EditLog(QString logname,QString *filter,QString *group,QString *schedcode,
	 vector<RDLogLine> *clipboard,vector<QString> *new_logs,
	 QWidget *parent=0);
  ~EditLog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec();
  
 private slots:
  void descriptionChangedData(const QString &);
  void purgeDateChangedData(const QDate &date);
  void purgeDateToggledData(bool state);
  void selectPurgeDateData();
  void serviceActivatedData(const QString &svcname);
  void dateValueChangedData(const QDate &);
  void autorefreshChangedData(int index);
  void startDateEnabledData(bool);
  void endDateEnabledData(bool);
  void timestyleChangedData(int index);
  void insertCartButtonData();
  void insertMarkerButtonData();
  void clickedData(Q3ListViewItem *item);
  void selectionChangedData();
  void doubleClickData(Q3ListViewItem *item);
  void editButtonData();
  void deleteButtonData();
  void upButtonData();
  void downButtonData();
  void cutButtonData();
  void copyButtonData();
  void pasteButtonData();
  void cartDroppedData(int line,RDLogLine *ll);
  void notificationReceivedData(RDNotification *notify);
  void saveData();
  void saveasData();
  void renderasData();
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
  void SetStartTimeField(RDListViewItem *item);
  void RefreshList();
  void UpdateSelection();
  void RenumberList(int line);
  bool UpdateColor(RDListViewItem *item,RDLogLine *logline);
  void SelectRecord(int id);
  void UpdateTracks();
  bool DeleteTracks();
  bool ValidateSvc();
  void LoadClipboard(bool clear_ext);
  RDListViewItem *SingleSelection();
  void SetLogModified(bool state);
  void SendNotification(RDNotification::Action action,const QString &log_name);
  RDLog *edit_log;
  RDLogEvent *edit_log_event;
  std::vector<RDLogLine> *edit_clipboard;
  std::vector<unsigned> edit_deleted_tracks;
  std::vector<QString> *edit_newlogs;
  QString edit_logname;
  QString *edit_filter;
  QString *edit_group;
  QString *edit_schedcode;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QLabel *edit_service_label;
  QComboBox *edit_service_box;
  QLineEdit *edit_service_edit;
  QLabel *edit_autorefresh_label;
  QComboBox *edit_autorefresh_box;
  QLineEdit *edit_autorefresh_edit;
  Q3DateEdit *edit_startdate_edit;
  Q3DateEdit *edit_enddate_edit;
  QLabel *edit_startdate_label;
  QLabel *edit_startdate_box_label;
  QLabel *edit_enddate_box_label;
  QCheckBox *edit_startdate_box;
  QLabel *edit_enddate_label;
  QCheckBox *edit_enddate_box;
  DropListView *edit_log_list;
  QPixmap *edit_playout_map;
  QPixmap *edit_macro_map;
  QPixmap *edit_marker_map;
  QPixmap *edit_chain_map;
  QPixmap *edit_track_cart_map;
  QPixmap *edit_notemarker_map;
  QPixmap *edit_music_map;
  QPixmap *edit_mic16_map;
  QPixmap *edit_traffic_map;
  QLabel *edit_modified_label;
  QLabel *edit_logname_label_label;
  QLabel *edit_logname_label;
  QLabel *edit_origin_label_label;
  QLabel *edit_origin_label;
  QLabel *edit_track_label_label;
  QLabel *edit_track_label;
  QLabel *edit_timestyle_label;
  QComboBox *edit_timestyle_box;
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
  //  QLabel *edit_time_label;
  QGroupBox *edit_time_groupbox;
  QLabel *edit_stoptime_label;
  QLineEdit *edit_stoptime_edit;
  QLabel *edit_endtime_label;
  QLineEdit *edit_endtime_edit;
  QCheckBox *edit_purgedate_box;
  QLabel *edit_purgedate_label;
  Q3DateEdit *edit_purgedate_edit;
  QPushButton *edit_purgedate_button;
  RDLogLock *edit_log_lock;
  QPushButton *edit_renderas_button;
  RenderDialog *edit_render_dialog;
};


#endif  // EDIT_LOG_H
