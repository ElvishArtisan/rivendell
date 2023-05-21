// rdhash.h
//
// Functions for generating hashes.
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

#ifndef RDHASH_H
#define RDHASH_H

#include <qstring.h>

QString RDSha1HashData(const QByteArray &data);
QString RDSha1HashFile(const QString &filename,bool throttle=false);
QString RDSha1HashPassword(const QString &secret);
bool RDSha1HashCheckPassword(const QString &secret,const QString &hash);


#endif  // RDHASH_H
