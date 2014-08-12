// metarecord.h
//
// Container class for metadata
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: metarecord.h,v 1.1.2.3 2013/12/04 22:22:49 cvs Exp $
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

#ifndef METARECORD_H
#define METARECORD_H

#include <qstring.h>

#include <rdwavedata.h>

class MetaRecord
{
 public:
  MetaRecord(const QString &disc_id,int track_num);
  MetaRecord();
  QString discId() const;
  void setDiscId(const QString &disc_id);
  int trackNumber() const;
  void setTrackNumber(int track_num);
  QString songId() const;
  void setSongId(const QString &str);
  QString title() const;
  void setTitle(const QString &str);
  QString artist() const;
  void setArtist(const QString &str);
  QString version() const;
  void setVersion(const QString &str);
  QString ending() const;
  void setEnding(const QString &str);
  int introLength() const;
  void setIntroLength(int msecs);
  int length() const;
  void setLength(int msecs);
  int year() const;
  void setYear(int year);
  QString album() const;
  void setAlbum(const QString &str);
  QString composer() const;
  void setComposer(const QString &str);
  QString publisher() const;
  void setPublisher(const QString &str);
  QString license() const;
  void setLicense(const QString &str);
  QString label() const;
  void setLabel(const QString &str);
  QString client() const;
  void setClient(const QString &str);
  QString agency() const;
  void setAgency(const QString &str);
  QString isrc() const;
  void setIsrc(const QString &str);
  int beatsPerMinute() const;
  void setBeatsPerMinute(int bpm);
  int segueStart() const;
  void setSegueStart(int msec);
  int segueEnd() const;
  void setSegueEnd(int msec);
  void getMetadata(RDWaveData *data,int track_len);
  void clear();

 private:
  QString meta_disc_id;
  int meta_track_number;
  QString meta_song_id;
  QString meta_title;
  QString meta_artist;
  QString meta_version;
  QString meta_ending;
  int meta_intro_length;
  int meta_length;
  int meta_year;
  QString meta_album;
  QString meta_composer;
  QString meta_license;
  QString meta_publisher;
  QString meta_label;
  QString meta_client;
  QString meta_agency;
  QString meta_isrc;
  int meta_beats_per_minute;
  int meta_segue_start;
  int meta_segue_end;
};


#endif  // METARECORD_H
