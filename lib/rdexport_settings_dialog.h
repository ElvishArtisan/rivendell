// rdexport_settings_dialog.h
//
// Edit Rivendell Audio Settings
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

#include <rdsettings.h>
#include <rdstation.h>

class RDExportSettingsDialog : public QDialog
{
  Q_OBJECT
  public:
  RDExportSettingsDialog(RDSettings *settings,RDStation *station,
			 QWidget *parent=0);
   ~RDExportSettingsDialog();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void formatData(const QString &);
   void samprateData(const QString &);
   void bitrateData(const QString &);
   void okData();
   void cancelData();

  private:
   void ShowBitRates(RDSettings::Format fmt,int samprate,int bitrate,int qual);
   void SetCurrentItem(QComboBox *box,int value);
   RDSettings::Format GetFormat(QString str);
   RDSettings *lib_settings;
   RDStation *lib_station;
   QComboBox *lib_format_box;
   QComboBox *lib_channels_box;
   QLabel *lib_bitrate_label;
   QComboBox *lib_bitrate_box;
   QComboBox *lib_samprate_box;
   QLabel *lib_quality_label;
   QSpinBox *lib_quality_spin;
};


#endif

