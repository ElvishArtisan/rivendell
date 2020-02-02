//   rdmblookup.h
//
//   RDDiscLookup instance class for MusicBrainz
//
//   (C) Copyright 2003-2020 Fred Gleason <fredg@paravelsystems.com>
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
//

#ifndef RDMBLOOKUP_H
#define RDMBLOOKUP_H

#include <qdir.h>
#include <qicon.h>

#include <musicbrainz5/Release.h>

#include <rddisclookup.h>

class RDMbLookup : public RDDiscLookup
{
  Q_OBJECT
 public:
  RDMbLookup(const QString &caption,FILE *profile_msgs,QWidget *parent=0);
  ~RDMbLookup();
  QSize sizeHint() const;
  QString sourceName() const;
  QPixmap sourceLogo() const;
  QString sourceUrl() const;

 protected:
  void lookupRecord();

 private:
  RDDiscLookup::Result ProcessRelease(MusicBrainz5::CRelease *release);
  QIcon GetReleaseCover(const QString &mbid) const;
  QIcon *cover_art_default_icon;
  QDir *temp_directory;
};

#endif  // RDMBLOOKUP_H
