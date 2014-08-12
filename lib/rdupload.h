// rdupload.h
//
// Upload a File
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdupload.h,v 1.2 2010/07/29 19:32:34 cvs Exp $
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

#ifndef RDUPLOAD_H
#define RDUPLOAD_H

#include <qobject.h>
#include <qurl.h>

class RDUpload : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorUnsupportedProtocol=1,ErrorNoSource=2,
		  ErrorNoDestination=3,ErrorInvalidHostname=4,
		  ErrorInternal=5,ErrorRemoteServer=6,ErrorUrlInvalid=7,
		  ErrorUnspecified=8,ErrorInvalidUser=9,ErrorAborted=10,
		  ErrorInvalidLogin=11,ErrorRemoteAccess=12,
		  ErrorRemoteConnection=13};
  RDUpload(const QString &station_name,
		QObject *parent=0,const char *name=0);
  void setSourceFile(const QString &filename);
  void setDestinationUrl(const QString &url);
  int totalSteps() const;
  RDUpload::ErrorCode runUpload(const QString &username,
				const QString &password,
				bool log_debug);
  bool aborting() const;
  static QString errorText(RDUpload::ErrorCode err);

 public slots:
  void abort();

 signals:
  void progressChanged(int step);

 private:
  void UpdateProgress(int step);
  friend int UploadProgressCallback(void *clientp,double dltotal,double dlnow,
				    double ultotal,double ulnow);
  QString conv_src_filename;
  QUrl conv_dst_url;
  bool conv_aborting;
  uint conv_src_size;
};


#endif  // RDUPLOAD_H
