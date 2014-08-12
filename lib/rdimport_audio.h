// import_audio.h
//
// Audio File Importation Dialog for Rivendell.
//
//   (C) Copyright 2002-2003,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdimport_audio.h,v 1.10 2010/07/29 19:32:33 cvs Exp $
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

#ifndef IMPORT_AUDIO_H
#define IMPORT_AUDIO_H

#include <sys/types.h>
#include <unistd.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qtimer.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <rdbusybar.h>
#include <rdwavedata.h>
#include <rdconfig.h>
#include <rdsettings.h>
#include <rdexport_settings_dialog.h>
#include <rdcut.h>
#include <rduser.h>
#include <rdaudioexport.h>
#include <rdaudioimport.h>

//
// Widget Settings
//
#define IMPORT_BAR_INTERVAL 500
#define IMPORT_TEMP_BASENAME "rdlib"


class RDImportAudio : public QDialog
{
 Q_OBJECT
 public:
 RDImportAudio(QString cutname,QString *path,RDSettings *settings,
	       bool *import_metadata,RDWaveData *wavedata,
	       RDCut *clipboard,RDStation *station,RDUser *user,bool *running,
	       RDConfig *config,QWidget *parent=0,const char *name=0);
  ~RDImportAudio();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void enableAutotrim(bool state);
  void setAutotrimLevel(int lvl);
  void enableNormalization(bool state);
  void setNormalizationLevel(int lvl);
  void setChannels(int chans);

 public slots:
  int exec(bool enable_import,bool enable_export);

 private slots:
  void modeClickedData(int id);
  void filenameChangedData(const QString &str);
  void normalizeCheckData(bool state);
  void autotrimCheckData(bool state);
  void selectInputFileData();
  void selectOutputFileData();
  void selectOutputFormatData();
  void importData();
  void cancelData();

 protected:
  void paintEvent(QPaintEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void Import();
  void Export();
  void StartBar();
  void StopBar();
  RDConfig *import_config;
  RDSettings *import_default_settings;
  RDStation *import_station;
  RDUser *import_user;
  RDCut *import_clipboard;
  bool *import_running;
  QButtonGroup *import_mode_group;
  QRadioButton *import_importmode_button;
  QRadioButton *import_exportmode_button;
  QLabel *import_in_filename_label;
  QLineEdit *import_in_filename_edit;
  QCheckBox *import_in_metadata_box;
  QPushButton *import_in_selector_button;
  QLabel *import_channels_label;
  QLabel *import_out_filename_label;
  QLineEdit *import_out_filename_edit;
  QCheckBox *import_out_metadata_box;
  QPushButton *import_out_selector_button;
  QLabel *import_format_label;
  QLineEdit *import_format_edit;
  QPushButton *import_out_format_button;
  RDBusyBar *import_bar;
  QCheckBox *import_normalize_box;
  QSpinBox *import_normalize_spin;
  QLabel *import_normalize_label;
  QLabel *import_normalize_unit;
  QCheckBox *import_autotrim_box;
  QSpinBox *import_autotrim_spin;
  QLabel *import_autotrim_label;
  QLabel *import_autotrim_unit;
  QComboBox *import_channels_box;
  QPushButton *import_cancel_button;
  QPushButton *import_import_button;
  QString *import_path;
  QString import_file_filter;
  QString import_cutname;
  QString import_dest_filename;
  bool import_import_aborted;
  bool *import_import_metadata;
  RDSettings *import_settings;
  RDWaveData *import_wavedata;
  RDAudioImport *import_import_conv;
  RDAudioExport *import_export_conv;
};


#endif  // RDIMPORT_AUDIO_H
