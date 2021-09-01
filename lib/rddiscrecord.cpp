//   rddiscrecord.cpp
//
//   Container Class for Compact Disc Metadata
//
//   (C) Copyright 2003-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rddiscrecord.h"

RDDiscRecord::RDDiscRecord()
{
  clear();
}


void RDDiscRecord::clear()
{
  disc_tracks=0;
  disc_disc_id=0;
  disc_disc_length=0;
  disc_disc_title="";
  disc_disc_artist="";
  disc_disc_album="";
  disc_disc_author="";
  disc_disc_year=0;
  disc_disc_genre="";
  disc_disc_extended="";
  disc_disc_playorder="";
  disc_disc_release_mb_id="";
  for(int i=0;i<CDROM_LEADOUT;i++) {
    disc_track_title[i]="";
    disc_track_extended[i]="";
    disc_track_artist[i]="";
    disc_track_isrc[i]="";
    disc_track_offset[i]=0;
    disc_track_recording_mb_id[i]="";
  }
}


int RDDiscRecord::tracks() const
{
  return disc_tracks;
}


void RDDiscRecord::setTracks(int num)
{
  disc_tracks=num;
}


unsigned RDDiscRecord::discLength() const
{
  return disc_disc_length;
}


void RDDiscRecord::setDiscLength(unsigned len)
{
  disc_disc_length=len;
}


unsigned RDDiscRecord::discId() const
{
  return disc_disc_id;
}


void RDDiscRecord::setDiscId(unsigned id)
{
  disc_disc_id=id;
}


QString RDDiscRecord::discMbId() const
{
  return disc_disc_mb_id;
}


void RDDiscRecord::setDiscMbId(const QString &mbid)
{
  disc_disc_mb_id=mbid;
}


QString RDDiscRecord::mcn() const
{
  return disc_mcn;
}


void RDDiscRecord::setMcn(const QString &mcn)
{
  disc_mcn=mcn;
}


QString RDDiscRecord::trackRecordingMbId(int track) const
{
  return disc_track_recording_mb_id[track];
}


void RDDiscRecord::setTrackRecordingMbId(int track,const QString &str)
{
  disc_track_recording_mb_id[track]=str;
}


QString RDDiscRecord::discReleaseMbId() const
{
  return disc_disc_release_mb_id;
}


void RDDiscRecord::setDiscReleaseMbId(const QString &mbid)
{
  disc_disc_release_mb_id=mbid;
}


QString RDDiscRecord::mbSubmissionUrl() const
{
  return disc_mb_submission_url;
}


void RDDiscRecord::setMbSubmissionUrl(const QString &url)
{
  disc_mb_submission_url=url;
}


QString RDDiscRecord::discTitle() const
{
  return disc_disc_title;
}


void RDDiscRecord::setDiscTitle(QString title)
{
  int n;

  disc_disc_title=title;
  if((n=title.indexOf(" / "))!=-1) {
    disc_disc_artist=title.left(n);
    disc_disc_album=title.right(title.length()-n-3);
    disc_disc_author="";
  }
  else {
    disc_disc_album=title;
    disc_disc_artist=title;
    disc_disc_author="";
  }
}


QString RDDiscRecord::discArtist() const
{
  return disc_disc_artist;
}


void RDDiscRecord::setDiscArtist(QString artist)
{
  disc_disc_artist=artist;
}


QString RDDiscRecord::discAlbum() const
{
  return disc_disc_album;
}


void RDDiscRecord::setDiscAlbum(QString album)
{
  disc_disc_album=album;
}


QString RDDiscRecord::discAuthor() const
{
  return disc_disc_author;
}


void RDDiscRecord::setDiscAuthor(QString author)
{
  disc_disc_author=author;
}


unsigned RDDiscRecord::discYear() const
{
  return disc_disc_year;
}


void RDDiscRecord::setDiscYear(unsigned year)
{
  disc_disc_year=year;
}


QString RDDiscRecord::discGenre() const
{
  return disc_disc_genre;
}


void RDDiscRecord::setDiscGenre(QString genre)
{
  disc_disc_genre=genre;
}


QString RDDiscRecord::discLabel() const
{
  return disc_disc_label;
}


void RDDiscRecord::setDiscLabel(const QString &str)
{
  disc_disc_label=str;
}


QString RDDiscRecord::discExtended() const
{
  return disc_disc_extended;
}


void RDDiscRecord::setDiscExtended(QString text)
{
  disc_disc_extended=text;
}


QString RDDiscRecord::discPlayOrder() const
{
  return disc_disc_playorder;
}


void RDDiscRecord::setDiscPlayOrder(QString order)
{
  disc_disc_playorder=order;
}


unsigned RDDiscRecord::trackOffset(int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_offset[track];
  }
  return 0;
}


void RDDiscRecord::setTrackOffset(int track,unsigned frames)
{
  if(track<CDROM_LEADOUT) {
    disc_track_offset[track]=frames;
  }
}


QString RDDiscRecord::trackTitle(int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_title[track];
  }
  return QString();
}


void RDDiscRecord::setTrackTitle(int track,QString title)
{
  if(track<CDROM_LEADOUT) {
    disc_track_title[track]=title;
  }
}


QString RDDiscRecord::trackExtended(int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_extended[track];
  }
  return QString();
}


void RDDiscRecord::setTrackExtended(int track,QString text)
{
  if(track<CDROM_LEADOUT) {
    disc_track_extended[track]=text;
  }
}


QString RDDiscRecord::trackArtist(int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_artist[track];
  }
  return QString();
}


void RDDiscRecord::setTrackArtist(int track,QString artist)
{
  if(track<CDROM_LEADOUT) {
    disc_track_artist[track]=artist;
  }
}


bool RDDiscRecord::hasIsrcs() const
{
  for(int i=0;i<tracks();i++) {
    if(isrc(i).isEmpty()) {
      return false;
    }
  }
  return true;
}


QString RDDiscRecord::isrc(int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_isrc[track];
  }
  return QString();
}


void RDDiscRecord::setIsrc(int track,QString isrc)
{
  if(track<CDROM_LEADOUT) {
    disc_track_isrc[track]=isrc;
  }
}


QString RDDiscRecord::dump()
{
  QString ret="RDDiscRecord::dump()\n";

  ret+=QString::asprintf("tracks: %d\n",tracks());
  ret+=QString::asprintf("discLength: %d\n",discLength());
  ret+=QString::asprintf("discId: %08x\n",discId());
  ret+="mcn: "+mcn()+"\n";
  ret+="discMbId: "+discMbId()+"\n";
  ret+="mbSubmissionUrl: "+mbSubmissionUrl()+"\n";
  ret+="discTitle: "+discTitle()+"\n";
  ret+="discArtist: "+discArtist()+"\n";
  ret+="discAlbum: "+discAlbum()+"\n";
  ret+="discAuthor: "+discAuthor()+"\n";
  ret+=QString::asprintf("discYear: %u\n",discYear());
  ret+="discGenre: "+discGenre()+"\n";
  ret+="discLabel: "+discLabel()+"\n";
  ret+="discExtended: "+discExtended()+"\n";
  ret+="discPlayOrder: "+discPlayOrder()+"\n";
  for(int i=0;i<tracks();i++) {
    QString num=QString::asprintf("(%d): ",i+1);
    ret+="trackOffset"+num+QString::asprintf("%u",trackOffset(i))+"\n";
    ret+="trackTitle"+num+trackTitle(i)+"\n";
    ret+="trackExtended"+num+trackExtended(i)+"\n";
    ret+="trackRecordingMbId"+num+trackRecordingMbId(i)+"\n";
    ret+="isrc"+num+isrc(i)+"\n";
  }

  return ret;
}
