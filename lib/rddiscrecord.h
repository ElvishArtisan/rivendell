//   rddiscrecord.h
//
//   Container Class for Compact Disc Metadata
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

#ifndef RDDISCRECORD_H
#define RDDISCRECORD_H

#include <qstring.h>
#include <linux/cdrom.h>


/**
 * @short A container class for CDDB data.
 * @author Fred Gleason <fredg@paravelsystems.com>
 **/

class RDDiscRecord
{
  public:
  /**
   * Create an RDDiscRecord object
   **/
  RDDiscRecord();
  void clear();
  int tracks() const;
  void setTracks(int num);
  unsigned discLength() const;
  void setDiscLength(unsigned len);
  unsigned discId() const;
  void setDiscId(unsigned id);
  QString mcn() const;
  void setMcn(const QString &mcn);
  QString discMbId() const;
  void setDiscMbId(const QString &str);
  QString mbSubmissionUrl() const;
  void setMbSubmissionUrl(const QString &url);
  QString discTitle() const;
  void setDiscTitle(QString title);
  QString discArtist() const;
  void setDiscArtist(QString artist);
  QString discAlbum() const;
  void setDiscAlbum(QString album);
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
  QString trackTitle(int track) const;
  void setTrackTitle(int track,QString title);
  QString trackExtended(int track) const;
  void setTrackExtended(int track,QString text);
  QString trackArtist(int track) const;
  void setTrackArtist(int track,QString artist);
  QString trackMbId(int track) const;
  void setTrackMbId(int track,const QString &str);
  QString isrc(int track) const;
  void setIsrc(int track,QString isrc);
  QString dump();

 private:
  int disc_tracks;
  unsigned disc_disc_id;
  QString disc_mcn;
  QString disc_disc_mb_id;
  QString disc_mb_submission_url;
  unsigned disc_disc_length;
  QString disc_disc_title;
  QString disc_disc_artist;
  QString disc_disc_album;
  QString disc_disc_author;
  unsigned disc_disc_year;
  QString disc_disc_genre;
  QString disc_disc_label;
  QString disc_disc_extended;
  QString disc_disc_playorder;
  QString disc_track_title[CDROM_LEADOUT];
  QString disc_track_extended[CDROM_LEADOUT];
  QString disc_track_artist[CDROM_LEADOUT];
  QString disc_track_mbid[CDROM_LEADOUT];
  QString disc_track_isrc[CDROM_LEADOUT];
  unsigned disc_track_offset[CDROM_LEADOUT];
};


#endif  // RDDISCRECORD_H
