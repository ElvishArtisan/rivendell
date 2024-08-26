// edit_rdairplay.h
//
// Edit an RDAirPlay Configuration
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

#ifndef EDIT_RDAIRPLAY_H
#define EDIT_RDAIRPLAY_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

#include <rd.h>
#include <rdairplay_conf.h>
#include <rdcardselector.h>
#include <rddialog.h>
#include <rdlist_logs.h>
#include <rdservicelistmodel.h>
#include <rdstation.h>

/*
 * Application Settings
 */
#define LOG_PLAY_PORTS 2
#define MAX_MANUAL_SEGUE 10

class EditRDAirPlay : public RDDialog
{
 Q_OBJECT
 public:
  EditRDAirPlay(RDStation *station,RDStation *cae_station,QWidget *parent=0);
  ~EditRDAirPlay();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void audioSettingsChangedData(int id,int card,int port);
  void editGpiosData(int num);
  void exitPasswordChangedData(const QString &str);
  void logActivatedData(int lognum);
  void virtualLogActivatedData(int vlognum);
  void virtualModeActivatedData(int vlognum);
  void startModeChangedData(int mode);
  void selectData();
  //  void editHotKeys();
  void selectSkinData();
  void selectLogoData();
  void modeControlActivatedData(int n);
  void logStartupModeActivatedData(int n);
  void okData();
  void cancelData();

 protected:
  void paintEvent(QPaintEvent *e);
  
 private:
  RDAirPlayConf *air_conf;
  RDCardSelector *air_card_sel[RDAirPlayConf::LastChannel];
  QLabel *air_start_rml_label[RDAirPlayConf::LastChannel];
  QLineEdit *air_start_rml_edit[RDAirPlayConf::LastChannel];
  QLabel *air_stop_rml_label[RDAirPlayConf::LastChannel];
  QLineEdit *air_stop_rml_edit[RDAirPlayConf::LastChannel];
  QPushButton *air_channel_button[RDAirPlayConf::LastChannel];
  QComboBox *air_virtual_machine_box;
  RDCardSelector *air_virtual_card_sel;
  QLabel *air_virtual_start_rml_label;
  QLineEdit *air_virtual_start_rml_edit;
  QLabel *air_virtual_stop_rml_label;
  QLineEdit *air_virtual_stop_rml_edit;
  int air_virtual_cards[RD_RDVAIRPLAY_LOG_QUAN];
  int air_virtual_ports[RD_RDVAIRPLAY_LOG_QUAN];
  QString air_virtual_start_rmls[RD_RDVAIRPLAY_LOG_QUAN];
  QString air_virtual_stop_rmls[RD_RDVAIRPLAY_LOG_QUAN];
  QLabel *air_segue_label;
  QLineEdit *air_segue_edit;
  QLabel *air_trans_label;
  QLineEdit *air_trans_edit;
  QLabel *air_trans_unit;
  QLabel *air_piecount_label;
  QSpinBox *air_piecount_box;
  QLabel *air_piecount_unit;
  QLabel *air_countto_label;
  QComboBox *air_countto_box;
  QLabel *air_segue_unit;
  QLabel *air_station_label;
  QSpinBox *air_station_box;
  QLabel *air_user_label;
  QSpinBox *air_user_box;
  QLabel *air_timesync_label;
  QCheckBox *air_timesync_box;
  QLabel *air_aux1_label;
  QSpinBox *air_aux1_box;
  QLabel *air_aux2_label;
  QSpinBox *air_aux2_box;
  QCheckBox *air_auxlog_box[2];
  QCheckBox *air_clearfilter_box;
  QLabel *air_baraction_label;
  QComboBox *air_baraction_box;
  QCheckBox *air_flash_box;
  QCheckBox *air_panel_pause_box;
  QCheckBox *air_show_counters_box;
  QLabel *air_audition_preroll_label;
  QLabel *air_audition_preroll_unit;
  QSpinBox *air_audition_preroll_spin;
  QLineEdit *air_label_template_edit;
  QCheckBox *air_pause_box;
  QCheckBox *air_hour_selector_box;
  QComboBox *air_default_transtype_box;
  QComboBox *air_defaultsvc_box;
  QLineEdit *air_exitpasswd_edit;
  bool air_exitpasswd_changed;
  int air_logmachine;
  int air_virtual_logmachine;
  QComboBox *air_logmachine_box;
  QComboBox *air_startmode_box;
  QMap<int,RDAirPlayConf::StartMode> air_startmodes;
  QLineEdit *air_startlog_edit;
  QLabel *air_startlog_label;
  QPushButton *air_startlog_button;
  QMap<int,QString> air_startlogs;
  QCheckBox *air_autorestart_box;
  QMap<int,bool> air_autorestarts;
  QLabel *air_autorestart_label;
  QLineEdit *air_skin_edit;
  QLineEdit *air_logo_edit;
  QLineEdit *air_title_template_edit;
  QLineEdit *air_artist_template_edit;
  QLineEdit *air_outcue_template_edit;
  QLineEdit *air_description_template_edit;
  QComboBox *air_modecontrol_box;
  QLabel *air_logstartmode_label[RDAIRPLAY_LOG_QUANTITY];
  QComboBox *air_logstartmode_box[RDAIRPLAY_LOG_QUANTITY];
  QComboBox *air_virtual_logstartsel_box;
  QComboBox *air_virtual_logstartmode_box;
  int air_virtual_logstartmachine;
  RDAirPlayConf::OpMode air_virtual_opmodes[RD_RDVAIRPLAY_LOG_QUAN];
  RDListLogs *air_listlogs_dialog;
  RDServiceListModel *air_service_model;
};


#endif  // EDIT_RDAIRPLAY_H

