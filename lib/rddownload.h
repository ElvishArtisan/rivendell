// rddownload.h
//
// Download a File
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddownload.h,v 1.2 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDDOWNLOAD_H
#define RDDOWNLOAD_H

#include <qobject.h>
#include <qurl.h>

class RDDownload : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorUnsupportedProtocol=1,ErrorNoSource=2,
		  ErrorNoDestination=3,ErrorInvalidHostname=4,
		  ErrorInternal=5,ErrorRemoteServer=6,ErrorUrlInvalid=7,
		  ErrorUnspecified=8,ErrorInvalidUser=9,ErrorAborted=10,
		  ErrorInvalidLogin=11,ErrorRemoteAccess=12,
		  ErrorRemoteConnection=13};
  RDDownload(const QString &station_name,
		QObject *parent=0,const char *name=0);
  void setSourceUrl(const QString &url);
  void setDestinationFile(const QString &filename);
  int totalSteps() const;
  RDDownload::ErrorCode runDownload(const QString &username,
				    const QString &password,
				    bool log_debug);
  bool aborting() const;
  static QString errorText(RDDownload::ErrorCode err);

 public slots:
  void abort();

 signals:
  void progressChanged(int step);

 private:
  void UpdateProgress(int step);
  friend int DownloadProgressCallback(void *clientp,double dltotal,double dlnow,
				      double ultotal,double ulnow);
  QUrl conv_src_url;
  QString conv_dst_filename;
  bool conv_aborting;
  uint conv_dst_size;
};


#endif  // RDDOWNLOAD_H
