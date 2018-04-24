// rdaudiostore.h
//
// Get information about the audio store.
//
//   (C) Copyright 2014,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDAUDIOSTORE_H
#define RDAUDIOSTORE_H

#include <stdint.h>

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>

class RDAudioStore : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorNoAudio=10};
  RDAudioStore(RDStation *station,RDConfig *config,QObject *parent=0);
  uint64_t freeBytes() const;
  uint64_t totalBytes() const;
  RDAudioStore::ErrorCode runStore(const QString &username,
				   const QString &password);
  static QString errorText(RDAudioStore::ErrorCode err);

 private:
  uint64_t ParseInt(const QString &tag,const QString &xml);
  RDStation *conv_station;
  RDConfig *conv_config;
  QString conv_xml;
  uint64_t conv_free_bytes;
  uint64_t conv_total_bytes;
};


#endif  // RDAUDIOSTORE_H
