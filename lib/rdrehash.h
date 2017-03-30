// rdrehash.h
//
// Generate a SHA-1 hash of an audio file and write it to the database.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDREHASH_H
#define RDREHASH_H

#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>
#include <rduser.h>

class RDRehash : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInternal=5,ErrorUrlInvalid=7,
		  ErrorService=8,ErrorInvalidUser=9,ErrorNoAudio=10};
  RDRehash(RDStation *station,RDConfig *config,QObject *parent=0);
  void setCartNumber(unsigned cartnum);
  void setCutNumber(unsigned cutnum);
  RDRehash::ErrorCode runRehash(const QString &username,
				const QString &password);
  static QString errorText(RDRehash::ErrorCode err);
  static RDRehash::ErrorCode rehash(RDStation *station,RDUser *user,
				    RDConfig *config,
				    unsigned cartnum,int cutnum);

 private:
  RDStation *conv_station;
  RDConfig *conv_config;
  unsigned conv_cart_number;
  unsigned conv_cut_number;
};


#endif  // RDREHASH_H
