// rddelete.h
//
// Delete a Remote File
//
//   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddelete.h,v 1.1 2011/10/17 18:48:40 cvs Exp $
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

#ifndef RDDELETE_H
#define RDDELETE_H

#include <qobject.h>
#include <qurl.h>

class RDDelete : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorUnsupportedProtocol=1,
		  ErrorInvalidHostname=4,
		  ErrorInternal=5,ErrorRemoteServer=6,ErrorUrlInvalid=7,
		  ErrorUnspecified=8,ErrorInvalidUser=9,
		  ErrorInvalidLogin=11,ErrorRemoteAccess=12,
		  ErrorRemoteConnection=13,ErrorUnknown=14};
  RDDelete(QObject *parent=0,const char *name=0);
  void setTargetUrl(const QString &url);
  RDDelete::ErrorCode runDelete(const QString &username,
				const QString &password,
				bool log_debug);
  static QString errorText(RDDelete::ErrorCode err);

 private:
  QUrl conv_target_url;
};


#endif  // RDUPLOAD_H
