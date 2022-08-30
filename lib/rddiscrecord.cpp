//   rddiscrecord.cpp
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

#include <QObject>

#include "rddiscrecord.h"

RDDiscRecord::RDDiscRecord()
{
  clear();
}


void RDDiscRecord::clear()
{
  for(int i=0;i<RDDiscRecord::LastSource;i++) {
    disc_has_data[i]=false;
    disc_disc_title[i]="";
    disc_disc_artist[i]="";
    disc_disc_album[i]="";
    for(int j=0;j<CDROM_LEADOUT;j++) {
      disc_track_title[i][j]="";
      disc_track_artist[i][j]="";
    }
  }
  disc_tracks=0;
  disc_disc_id=0;
  disc_disc_length=0;
  disc_disc_author="";
  disc_disc_year=0;
  disc_disc_genre="";
  disc_disc_extended="";
  disc_disc_playorder="";
  disc_disc_release_mb_id="";
  for(int i=0;i<CDROM_LEADOUT;i++) {
    disc_track_extended[i]="";
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


bool RDDiscRecord::hasData(RDDiscRecord::DataSource src) const
{
  return disc_has_data[src];
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
  disc_disc_mb_id=mbid.trimmed();
}


QString RDDiscRecord::mcn() const
{
  return disc_mcn;
}


void RDDiscRecord::setMcn(const QString &mcn)
{
  disc_mcn=mcn.trimmed();
}


QString RDDiscRecord::trackRecordingMbId(int track) const
{
  return disc_track_recording_mb_id[track];
}


void RDDiscRecord::setTrackRecordingMbId(int track,const QString &str)
{
  disc_track_recording_mb_id[track]=str.trimmed();
}


QString RDDiscRecord::discReleaseMbId() const
{
  return disc_disc_release_mb_id;
}


void RDDiscRecord::setDiscReleaseMbId(const QString &mbid)
{
  disc_disc_release_mb_id=mbid.trimmed();
}


QString RDDiscRecord::mbSubmissionUrl() const
{
  return disc_mb_submission_url;
}


void RDDiscRecord::setMbSubmissionUrl(const QString &url)
{
  disc_mb_submission_url=url.trimmed();
}


QString RDDiscRecord::discTitle(RDDiscRecord::DataSource src) const
{
  return disc_disc_title[src];
}


void RDDiscRecord::setDiscTitle(RDDiscRecord::DataSource src,
				const QString &title)
{
  int n;

  disc_has_data[src]=true;
  disc_disc_title[src]=title.trimmed();
  if((n=title.find(" / "))!=-1) {
    disc_disc_artist[src]=title.left(n).trimmed();
    disc_disc_album[src]=title.right(title.length()-n-3).trimmed();
    disc_disc_author="";
  }
  else {
    disc_disc_album[src]=title.trimmed();
    disc_disc_artist[src]=title.trimmed();
    disc_disc_author="";
  }
}


QString RDDiscRecord::discArtist(RDDiscRecord::DataSource src) const
{
  return disc_disc_artist[src];
}


void RDDiscRecord::setDiscArtist(RDDiscRecord::DataSource src,
				 const QString &artist)
{
  disc_has_data[src]=true;
  disc_disc_artist[src]=artist.trimmed();
}


QString RDDiscRecord::discAlbum(RDDiscRecord::DataSource src) const
{
  return disc_disc_album[src];
}


void RDDiscRecord::setDiscAlbum(RDDiscRecord::DataSource src,
				const QString &album)
{
  disc_has_data[src]=true;
  disc_disc_album[src]=album.trimmed();
}


QString RDDiscRecord::discAuthor() const
{
  return disc_disc_author;
}


void RDDiscRecord::setDiscAuthor(QString author)
{
  disc_disc_author=author.trimmed();
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
  disc_disc_genre=genre.trimmed();
}


QString RDDiscRecord::discLabel() const
{
  return disc_disc_label;
}


void RDDiscRecord::setDiscLabel(const QString &str)
{
  disc_disc_label=str.trimmed();
}


QString RDDiscRecord::discExtended() const
{
  return disc_disc_extended;
}


void RDDiscRecord::setDiscExtended(QString text)
{
  disc_disc_extended=text.trimmed();
}


QString RDDiscRecord::discPlayOrder() const
{
  return disc_disc_playorder;
}


void RDDiscRecord::setDiscPlayOrder(QString order)
{
  disc_disc_playorder=order.trimmed();
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


QString RDDiscRecord::trackTitle(RDDiscRecord::DataSource src,int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_title[src][track];
  }
  return QString();
}


void RDDiscRecord::setTrackTitle(RDDiscRecord::DataSource src,int track,
				 const QString &title)
{
  if(track<CDROM_LEADOUT) {
    disc_has_data[src]=true;
    disc_track_title[src][track]=title.trimmed();
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
    disc_track_extended[track]=text.trimmed();
  }
}


QString RDDiscRecord::trackArtist(RDDiscRecord::DataSource src,int track) const
{
  if(track<CDROM_LEADOUT) {
    return disc_track_artist[src][track];
  }
  return QString();
}


void RDDiscRecord::setTrackArtist(RDDiscRecord::DataSource src,int track,
				  const QString &artist)
{
  if(track<CDROM_LEADOUT) {
    disc_has_data[src]=true;
    disc_track_artist[src][track]=artist.trimmed();
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


void RDDiscRecord::setIsrc(int track,const QString &isrc)
{
  if(track<CDROM_LEADOUT) {
    disc_track_isrc[track]=isrc.trimmed();
  }
}


QString RDDiscRecord::summary(RDDiscRecord::DataSource src) const
{
  QString ret;

  if(!discTitle(src).isEmpty()) {
    ret+="<strong>"+QObject::tr("Disc Title")+"</strong>: "+
      discTitle(src)+"<br>\n";
  }
  if(!discArtist(src).isEmpty()) {
    ret+="<strong>"+QObject::tr("Disc Artist")+"</strong>: "+
      discArtist(src)+"<br>\n";
  }
  for(int i=0;i<tracks();i++) {
    ret+="<strong>"+QObject::tr("Track")+QString().sprintf(" %2d: ",i+1)+
      "</strong>"+trackTitle(src,i)+"<br>\n";
  }

  return ret;
}


QString RDDiscRecord::dump(RDDiscRecord::DataSource src) const
{
  QString ret=QString().sprintf("RDDiscRecord::dump(%u)\n",src);

  ret+=QString().sprintf("tracks: %d\n",tracks());
  ret+=QString().sprintf("discLength: %d\n",discLength());
  ret+=QString().sprintf("discId: %08x\n",discId());
  ret+="mcn: "+mcn()+"\n";
  ret+="discMbId: "+discMbId()+"\n";
  ret+="mbSubmissionUrl: "+mbSubmissionUrl()+"\n";
  ret+="discTitle: "+discTitle(src)+"\n";
  ret+="discArtist: "+discArtist(src)+"\n";
  ret+="discAlbum: "+discAlbum(src)+"\n";
  ret+="discAuthor: "+discAuthor()+"\n";
  ret+=QString().sprintf("discYear: %u\n",discYear());
  ret+="discGenre: "+discGenre()+"\n";
  ret+="discLabel: "+discLabel()+"\n";
  ret+="discExtended: "+discExtended()+"\n";
  ret+="discPlayOrder: "+discPlayOrder()+"\n";
  for(int i=0;i<tracks();i++) {
    QString num=QString().sprintf("(%d): ",i+1);
    ret+="trackOffset"+num+QString().sprintf("%u",trackOffset(i))+"\n";
    ret+="trackTitle"+num+trackTitle(src,i)+"\n";
    ret+="trackExtended"+num+trackExtended(i)+"\n";
    ret+="trackRecordingMbId"+num+trackRecordingMbId(i)+"\n";
    ret+="isrc"+num+isrc(i)+"\n";
  }

  return ret;
}
