// metarecord.cpp
//
// Container class for metadata
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: metarecord.cpp,v 1.1.2.3 2013/12/04 22:22:49 cvs Exp $
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

#include <metarecord.h>

MetaRecord::MetaRecord(const QString &disc_id,int track_num)
{
  meta_disc_id=disc_id;
  meta_track_number=track_num;
}


MetaRecord::MetaRecord()
{
  meta_disc_id="";
  meta_track_number=0;
}


QString MetaRecord::discId() const
{
  return meta_disc_id;
}


void MetaRecord::setDiscId(const QString &disc_id)
{
  meta_disc_id=disc_id;
}


int MetaRecord::trackNumber() const
{
  return meta_track_number;
}


void MetaRecord::setTrackNumber(int track_num)
{
  meta_track_number=track_num;
}


QString MetaRecord::songId() const
{
  return meta_song_id;
}


void MetaRecord::setSongId(const QString &str)
{
  meta_song_id=str;
}


QString MetaRecord::title() const
{
  return meta_title;
}


void MetaRecord::setTitle(const QString &str)
{
  meta_title=str;
}


QString MetaRecord::artist() const
{
  return meta_artist;
}


void MetaRecord::setArtist(const QString &str)
{
  meta_artist=str;
}


QString MetaRecord::version() const
{
  return meta_version;
}


void MetaRecord::setVersion(const QString &str)
{
  meta_version=str;
}


QString MetaRecord::ending() const
{
  return meta_ending;
}


void MetaRecord::setEnding(const QString &str)
{
  meta_ending=str;
}


int MetaRecord::introLength() const
{
  return meta_intro_length;
}


void MetaRecord::setIntroLength(int msecs)
{
  meta_intro_length=msecs;
}


int MetaRecord::length() const
{
  return meta_length;
}


void MetaRecord::setLength(int msecs)
{
  meta_length=msecs;
}


int MetaRecord::year() const
{
  return meta_year;
}


void MetaRecord::setYear(int year)
{
  meta_year=year;
}


QString MetaRecord::album() const
{
  return meta_album;
}


void MetaRecord::setAlbum(const QString &str)
{
  meta_album=str;
}


QString MetaRecord::composer() const
{
  return meta_composer;
}


void MetaRecord::setComposer(const QString &str)
{
  meta_composer=str;
}


QString MetaRecord::publisher() const
{
  return meta_publisher;
}


void MetaRecord::setPublisher(const QString &str)
{
  meta_publisher=str;
}


QString MetaRecord::license() const
{
  return meta_license;
}


void MetaRecord::setLicense(const QString &str)
{
  meta_license=str;
}


QString MetaRecord::label() const
{
  return meta_label;
}


void MetaRecord::setLabel(const QString &str)
{
  meta_label=str;
}


QString MetaRecord::client() const
{
  return meta_client;
}


void MetaRecord::setClient(const QString &str)
{
  meta_client=str;
}


QString MetaRecord::agency() const
{
  return meta_agency;
}


void MetaRecord::setAgency(const QString &str)
{
  meta_agency=str;
}


QString MetaRecord::isrc() const
{
  return meta_isrc;
}


void MetaRecord::setIsrc(const QString &str)
{
  meta_isrc=str;
}


int MetaRecord::beatsPerMinute() const
{
  return meta_beats_per_minute;
}


void MetaRecord::setBeatsPerMinute(int bpm)
{
  meta_beats_per_minute=bpm;
}


int MetaRecord::segueStart() const
{
  return meta_segue_start;
}


void MetaRecord::setSegueStart(int msec)
{
  meta_segue_start=msec;
}


int MetaRecord::segueEnd() const
{
  return meta_segue_end;
}


void MetaRecord::setSegueEnd(int msec)
{
  meta_segue_end=msec;
}


void MetaRecord::getMetadata(RDWaveData *data,int track_len)
{
  data->setTitle(meta_title);
  data->setArtist(meta_artist);
  data->setReleaseYear(meta_year);
  data->setAlbum(meta_album);
  data->setLabel(meta_label);
  data->setComposer(meta_composer);
  data->setPublisher(meta_publisher);
  data->setIsrc(meta_isrc);
  data->setLicensingOrganization(meta_license);
  data->setIntroStartPos(0);
  data->setIntroEndPos(meta_intro_length);
  if(meta_segue_start>=0) {
    data->setSegueStartPos(meta_segue_start);
    data->setSegueEndPos(track_len);
  }
  data->setTmciSongId(meta_song_id);
  data->setBeatsPerMinute(meta_beats_per_minute);
  if(!meta_ending.isEmpty()) {
    data->setEndType((RDWaveData::EndType)((const char *)meta_ending)[0]);
  }
}


void MetaRecord::clear()
{
  meta_song_id="";
  meta_title="";
  meta_artist="";
  meta_version="";
  meta_ending="";
  meta_intro_length=0;
  meta_length=-1;
  meta_year=0;
  meta_album="";
  meta_composer="";
  meta_publisher="";
  meta_license="";
  meta_label="";
  meta_client="";
  meta_agency="";
  meta_isrc="";
  meta_beats_per_minute=0;
  meta_segue_start=-1;
  meta_segue_end=-1;
}
