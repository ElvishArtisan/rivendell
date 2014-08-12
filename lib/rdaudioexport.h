// rdaudioexport.h
//
// Export an Audio File using the RdXport Web Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioexport.h,v 1.4.4.1 2013/11/13 23:36:30 cvs Exp $
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

#ifndef RDAUDIOEXPORT_H
#define RDAUDIOEXPORT_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>
#include <rdsettings.h>
#include <rdaudioconvert.h>

class RDAudioExport : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInvalidSettings=1,ErrorNoSource=2,
		  ErrorNoDestination=3,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorAborted=10,
		  ErrorConverter=11};
  RDAudioExport(RDStation *station,RDConfig *config,QObject *parent=0,
		const char *name=0);
  void setCartNumber(unsigned cartnum);
  void setCutNumber(unsigned cutnum);
  void setDestinationFile(const QString &filename);
  void setDestinationSettings(RDSettings *settings);
  void setRange(int start_pt,int end_pt);
  void setEnableMetadata(bool state);
  RDAudioExport::ErrorCode runExport(const QString &username,
				     const QString &password,
				     RDAudioConvert::ErrorCode *conv_err);
  bool aborting() const;
  static QString errorText(RDAudioExport::ErrorCode err,
			   RDAudioConvert::ErrorCode conv_err);

 public slots:
  void abort();

 signals:
  void strobe();

 private:
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_cart_number;
  unsigned conv_cut_number;
  QString conv_dst_filename;
  int conv_start_point;
  int conv_end_point;
  bool conv_enable_metadata;
  RDSettings *conv_settings;
  bool conv_aborting;
};


#endif  // RDAUDIOEXPORT_H
