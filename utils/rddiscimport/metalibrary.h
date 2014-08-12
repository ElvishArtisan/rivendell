// metalibrary.h
//
// Abstract a library of metadata.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: metalibrary.h,v 1.1.2.1 2013/12/03 23:34:35 cvs Exp $
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

#ifndef METALIBRARY_H
#define METALIBRARY_H

#include <vector>

#include <qstring.h>
#include <qstringlist.h>

#include <metarecord.h>

class MetaLibrary
{
 public:
  MetaLibrary();
  ~MetaLibrary();
  unsigned totalTracks();
  unsigned tracks(const QString &disc_id);
  MetaRecord *track(const QString &disc_id,int track_num);
  int load(const QString &filename);
  void clear();

 private:
  void LoadLine(const QStringList fields);
  QStringList Split(const QString &sep,const QString &str);
  std::vector<MetaRecord *> meta_tracks;
  QStringList meta_headers;
};


#endif  // METALIBRARY_H
