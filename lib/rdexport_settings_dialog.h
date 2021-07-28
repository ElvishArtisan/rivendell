// rdexport_settings_dialog.h
//
// Edit Rivendell Audio Settings
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

#ifndef RDEXPORT_SETTINGS_DIALOG_H
#define RDEXPORT_SETTINGS_DIALOG_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

#include <rddialog.h>
#include <rdsettings.h>

class RDExportSettingsDialog : public RDDialog
{
  Q_OBJECT
 public:
  RDExportSettingsDialog(const QString &caption,QWidget *parent=0);
  ~RDExportSettingsDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setShowNormalizationLevel(bool state);
  void setShowAutotrimLevel(bool state);

 public slots:
  int exec(RDSettings *s,unsigned id=0);

 private slots:
  void formatData(const QString &);
  void samprateData(const QString &);
  void bitrateData(const QString &);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e); 

 private:
  void ShowBitRates(RDSettings::Format fmt,int samprate,int bitrate,int qual);
  RDSettings::Format GetFormat(QString str);
  RDSettings *lib_settings;
  QLabel *lib_name_label;
  QLineEdit *lib_name_edit;
  unsigned lib_id;
  QString lib_caption;
  QLabel *lib_format_label;
  QComboBox *lib_format_box;
  QLabel *lib_channels_label;
  QComboBox *lib_channels_box;
  QLabel *lib_bitrate_label;
  QComboBox *lib_bitrate_box;
  QLabel *lib_bitrate_unit;
  bool lib_normalization_level_enabled;
  QLabel *lib_normalization_level_label;
  QSpinBox *lib_normalization_level_spin;
  QLabel *lib_normalization_level_unit_label;
  bool lib_autotrim_level_enabled;
  QLabel *lib_autotrim_level_label;
  QSpinBox *lib_autotrim_level_spin;
  QLabel *lib_autotrim_level_unit_label;
  QLabel *lib_samprate_label;
  QComboBox *lib_samprate_box;
  QLabel *lib_quality_label;
  QSpinBox *lib_quality_spin;
  QPushButton *lib_ok_button;
  QPushButton *lib_cancel_button;
};


#endif  // RDEXPORT_SETTINGS_DIALOG_H
