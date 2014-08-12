//   rdcddbrecord.h
//
//   A Container Class for CDDB Data.
//
//   (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcddbrecord.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDCDDBRECORD_H
#define RDCDDBRECORD_H

#include <qstring.h>
#include <linux/cdrom.h>


/**
 * @short A container class for CDDB data.
 * @author Fred Gleason <fredg@paravelsystems.com>
 **/

class RDCddbRecord
{
  public:
  /**
   * Create an RDCddbRecord object
   **/
  RDCddbRecord();
  void clear();
  int tracks() const;
  void setTracks(int num);
  unsigned discLength() const;
  void setDiscLength(unsigned len);
  unsigned discId() const;
  void setDiscId(unsigned id);
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
  QString isrc(int track) const;
  void setIsrc(int track,QString isrc);
  QString mcn(int track) const;
  void setMcn(int track,QString mcn);

 private:
  int cddb_tracks;
  unsigned cddb_disc_id;
  unsigned cddb_disc_length;
  QString cddb_disc_title;
  QString cddb_disc_artist;
  QString cddb_disc_album;
  QString cddb_disc_author;
  unsigned cddb_disc_year;
  QString cddb_disc_genre;
  QString cddb_disc_extended;
  QString cddb_disc_playorder;
  QString cddb_track_title[CDROM_LEADOUT];
  QString cddb_track_extended[CDROM_LEADOUT];
  QString cddb_track_artist[CDROM_LEADOUT];
  QString cddb_track_isrc[CDROM_LEADOUT];
  unsigned cddb_track_offset[CDROM_LEADOUT];
};


#endif  // RDCDDBRECORD_H
