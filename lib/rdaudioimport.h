// rdaudioimport.h
//
// Import an Audio File using the RdXport Web Service
//
//   (C) Copyright 2010-2014,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDAUDIOIMPORT_H
#define RDAUDIOIMPORT_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>
#include <rdsettings.h>
#include <rdaudioconvert.h>

class RDAudioImport : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInvalidSettings=1,ErrorNoSource=2,
		  ErrorNoDestination=3,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorAborted=10,
		  ErrorConverter=11};
  RDAudioImport(RDStation *station,RDConfig *config,QObject *parent=0);
  void setCartNumber(unsigned cartnum);
  void setCutNumber(unsigned cutnum);
  void setSourceFile(const QString &filename);
  void setUseMetadata(bool state);
  void setDestinationSettings(RDSettings *settings);
  RDAudioImport::ErrorCode runImport(const QString &username,
				     const QString &password,
				     RDAudioConvert::ErrorCode *conv_err);
  bool aborting() const;
  static QString errorText(RDAudioImport::ErrorCode err,
			   RDAudioConvert::ErrorCode conv_err);

 public slots:
  void abort();

 private:
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_cart_number;
  unsigned conv_cut_number;
  QString conv_src_filename;
  RDSettings *conv_settings;
  bool conv_use_metadata;
  bool conv_aborting;
};


#endif  // RDAUDIOIMPORT_H
