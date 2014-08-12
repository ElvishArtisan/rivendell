// rdaudiosettings_dialog.h
//
// Edit Rivendell Audio Settings
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdaudiosettings_dialog.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDAUDIOSETTINGS_DIALOG_H
#define RDAUDIOSETTINGS_DIALOG_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <rdaudiosettings.h>


class RDAudioSettingsDialog : public QDialog
{
  Q_OBJECT
 public:
  RDAudioSettingsDialog(RDAudioSettings *settings,bool mpeg=true,
			QWidget *parent=0,const char *name=0);
  ~RDAudioSettingsDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void formatData(int);
  void okData();
  void cancelData();

 private:
  void ShowBitRates(RDAudioSettings::Format fmt,int rate);
  RDAudioSettings::Format ReadFormat();
  RDAudioSettings *lib_lib;
  QComboBox *lib_format_box;
  QComboBox *lib_channels_box;
  QComboBox *lib_bitrate_box;
  QComboBox *lib_samprate_box;
};


#endif  // RDAUDIOSETTINGS_DIALOG_H
