//   rdcddbrecord.cpp
//
//   A Container Class for CDDB Data.
//
//   (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcddbrecord.cpp,v 1.1 2007/09/14 14:06:24 fredg Exp $
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

#include <rdcddbrecord.h>

RDCddbRecord::RDCddbRecord()
{
  clear();
}


void RDCddbRecord::clear()
{
  cddb_tracks=0;
  cddb_disc_id=0;
  cddb_disc_length=0;
  cddb_disc_title="";
  cddb_disc_artist="";
  cddb_disc_album="";
  cddb_disc_author="";
  cddb_disc_year=0;
  cddb_disc_genre="";
  cddb_disc_extended="";
  cddb_disc_playorder="";
  for(int i=0;i<CDROM_LEADOUT;i++) {
    cddb_track_title[i]="";
    cddb_track_extended[i]="";
    cddb_track_artist[i]="";
    cddb_track_isrc[i]="";
    cddb_track_offset[i]=0;
  }
}


int RDCddbRecord::tracks() const
{
  return cddb_tracks;
}


void RDCddbRecord::setTracks(int num)
{
  cddb_tracks=num;
}


unsigned RDCddbRecord::discLength() const
{
  return cddb_disc_length;
}


void RDCddbRecord::setDiscLength(unsigned len)
{
  cddb_disc_length=len;
}


unsigned RDCddbRecord::discId() const
{
  return cddb_disc_id;
}


void RDCddbRecord::setDiscId(unsigned id)
{
  cddb_disc_id=id;
}


QString RDCddbRecord::discTitle() const
{
  return cddb_disc_title;
}


void RDCddbRecord::setDiscTitle(QString title)
{
  int n;

  cddb_disc_title=title;
  if((n=title.find(" / "))!=-1) {
    cddb_disc_artist=title.left(n);
    cddb_disc_album=title.right(title.length()-n-3);
    cddb_disc_author="";
  }
  else {
    cddb_disc_album=title;
    cddb_disc_artist=title;
    cddb_disc_author="";
  }
}


QString RDCddbRecord::discArtist() const
{
  return cddb_disc_artist;
}


void RDCddbRecord::setDiscArtist(QString artist)
{
  cddb_disc_artist=artist;
}


QString RDCddbRecord::discAlbum() const
{
  return cddb_disc_album;
}


void RDCddbRecord::setDiscAlbum(QString album)
{
  cddb_disc_album=album;
}


QString RDCddbRecord::discAuthor() const
{
  return cddb_disc_author;
}


void RDCddbRecord::setDiscAuthor(QString author)
{
  cddb_disc_author=author;
}


unsigned RDCddbRecord::discYear() const
{
  return cddb_disc_year;
}


void RDCddbRecord::setDiscYear(unsigned year)
{
  cddb_disc_year=year;
}


QString RDCddbRecord::discGenre() const
{
  return cddb_disc_genre;
}


void RDCddbRecord::setDiscGenre(QString genre)
{
  cddb_disc_genre=genre;
}


QString RDCddbRecord::discExtended() const
{
  return cddb_disc_extended;
}


void RDCddbRecord::setDiscExtended(QString text)
{
  cddb_disc_extended=text;
}


QString RDCddbRecord::discPlayOrder() const
{
  return cddb_disc_playorder;
}


void RDCddbRecord::setDiscPlayOrder(QString order)
{
  cddb_disc_playorder=order;
}


unsigned RDCddbRecord::trackOffset(int track) const
{
  if(track<CDROM_LEADOUT) {
    return cddb_track_offset[track];
  }
  return 0;
}


void RDCddbRecord::setTrackOffset(int track,unsigned frames)
{
  if(track<CDROM_LEADOUT) {
    cddb_track_offset[track]=frames;
  }
}


QString RDCddbRecord::trackTitle(int track) const
{
  if(track<CDROM_LEADOUT) {
    return cddb_track_title[track];
  }
  return QString();
}


void RDCddbRecord::setTrackTitle(int track,QString title)
{
  if(track<CDROM_LEADOUT) {
    cddb_track_title[track]=title;
  }
}


QString RDCddbRecord::trackExtended(int track) const
{
  if(track<CDROM_LEADOUT) {
    return cddb_track_extended[track];
  }
  return QString();
}


void RDCddbRecord::setTrackExtended(int track,QString text)
{
  if(track<CDROM_LEADOUT) {
    cddb_track_extended[track]=text;
  }
}


QString RDCddbRecord::trackArtist(int track) const
{
  if(track<CDROM_LEADOUT) {
    return cddb_track_artist[track];
  }
  return QString();
}


void RDCddbRecord::setTrackArtist(int track,QString artist)
{
  if(track<CDROM_LEADOUT) {
    cddb_track_artist[track]=artist;
  }
}


QString RDCddbRecord::isrc(int track) const
{
  if(track<CDROM_LEADOUT) {
    return cddb_track_isrc[track];
  }
  return QString();
}


void RDCddbRecord::setIsrc(int track,QString isrc)
{
  if(track<CDROM_LEADOUT) {
    cddb_track_isrc[track]=isrc;
  }
}
