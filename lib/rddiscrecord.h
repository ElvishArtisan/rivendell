//   rddiscrecord.h
//
//   Container Class for Compact Disc Metadata
//
//   (C) Copyright 2003-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDISCRECORD_H
#define RDDISCRECORD_H

#include <qstring.h>
#include <linux/cdrom.h>

class RDDiscRecord
{
 public:
  enum DataSource {LocalSource=0,RemoteSource=1,LastSource=2};
  RDDiscRecord();
  void clear();
  int tracks() const;
  void setTracks(int num);
  bool hasData(RDDiscRecord::DataSource src) const;
  unsigned discLength() const;
  void setDiscLength(unsigned len);
  unsigned discId() const;
  QString discMbId() const;
  void setDiscMbId(const QString &mbid);
  void setDiscId(unsigned id);
  QString mcn() const;
  void setMcn(const QString &mcn);
  QString discReleaseMbId() const;
  void setDiscReleaseMbId(const QString &mbid);
  QString mbSubmissionUrl() const;
  void setMbSubmissionUrl(const QString &url);
  QString discTitle(DataSource src) const;
  void setDiscTitle(DataSource src,const QString &title);
  QString discArtist(DataSource src) const;
  void setDiscArtist(DataSource src,const QString &artist);
  QString discAlbum(DataSource src) const;
  void setDiscAlbum(DataSource src, const QString &album);
  QString discAuthor() const;
  void setDiscAuthor(QString author);
  unsigned discYear() const;
  void setDiscYear(unsigned year);
  QString discGenre() const;
  void setDiscGenre(QString genre);
  QString discLabel() const;
  void setDiscLabel(const QString &str);
  QString discExtended() const;
  void setDiscExtended(QString text);
  QString discPlayOrder() const;
  void setDiscPlayOrder(QString order);
  unsigned trackOffset(int track) const;
  void setTrackOffset(int track,unsigned frames);
  QString trackTitle(DataSource src,int track) const;
  void setTrackTitle(DataSource src,int track,const QString &title);
  QString trackExtended(int track) const;
  void setTrackExtended(int track,QString text);
  QString trackArtist(DataSource src,int track) const;
  void setTrackArtist(DataSource src,int track,const QString &artist);
  QString trackRecordingMbId(int track) const;
  void setTrackRecordingMbId(int track,const QString &str);
  bool hasIsrcs() const;
  QString isrc(int track) const;
  void setIsrc(int track,const QString &isrc);
  QString summary(RDDiscRecord::DataSource src) const;
  QString dump(RDDiscRecord::DataSource src) const;

 private:
  int disc_tracks;
  unsigned disc_disc_id;
  QString disc_disc_mb_id;
  QString disc_mcn;
  QString disc_disc_release_mb_id;
  QString disc_mb_submission_url;
  unsigned disc_disc_length;
  QString disc_disc_title[RDDiscRecord::LastSource];
  QString disc_disc_artist[RDDiscRecord::LastSource];
  QString disc_disc_album[RDDiscRecord::LastSource];
  QString disc_disc_author;
  unsigned disc_disc_year;
  QString disc_disc_genre;
  QString disc_disc_label;
  QString disc_disc_extended;
  QString disc_disc_playorder;
  QString disc_track_title[RDDiscRecord::LastSource][CDROM_LEADOUT];
  QString disc_track_extended[CDROM_LEADOUT];
  QString disc_track_artist[RDDiscRecord::LastSource][CDROM_LEADOUT];
  QString disc_track_isrc[CDROM_LEADOUT];
  unsigned disc_track_offset[CDROM_LEADOUT];
  QString disc_track_recording_mb_id[CDROM_LEADOUT];
  bool disc_has_data[RDDiscRecord::LastSource];
};


#endif  // RDDISCRECORD_H
