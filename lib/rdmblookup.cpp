//   rdmblookup.cpp
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

#include <musicbrainz5/Artist.h>
#include <musicbrainz5/ArtistCredit.h>
#include <musicbrainz5/Disc.h>
#include <musicbrainz5/HTTPFetch.h>
#include <musicbrainz5/ISRC.h>
#include <musicbrainz5/Label.h>
#include <musicbrainz5/LabelInfo.h>
#include <musicbrainz5/Medium.h>
#include <musicbrainz5/NameCredit.h>
#include <musicbrainz5/NameCreditList.h>
#include <musicbrainz5/Query.h>
#include <musicbrainz5/Recording.h>
#include <musicbrainz5/ReleaseGroup.h>
#include <musicbrainz5/ReleaseGroupList.h>
#include <musicbrainz5/RelationList.h>
#include <musicbrainz5/RelationListList.h>
#include <musicbrainz5/Track.h>

#include <QMessageBox>

#include "rdmblookup.h"

#include "../icons/musicbrainz-159x25.xpm"

QString err_str="OK";
RDDiscLookup::Result result_code=RDDiscLookup::ExactMatch;

RDMbLookup::RDMbLookup(const QString &caption,FILE *profile_msgs,
			     QWidget *parent)
  : RDDiscLookup(caption,profile_msgs,parent)
{
  setWindowTitle(caption+" - MusicBrainz "+tr("Lookup"));
}


QString RDMbLookup::sourceName() const
{
  return QString("MusicBrainz");
}


QPixmap RDMbLookup::sourceLogo() const
{
  return QPixmap(musicbrainz_159x25_xpm);
}


QString RDMbLookup::sourceUrl() const
{
  if(discRecord()->discReleaseMbId().isEmpty()) {
    return QString();
  }
  return "https://"+rda->libraryConf()->mbServer()+"/release/"+
    discRecord()->discReleaseMbId();
}


void RDMbLookup::lookupRecord()
{
  MusicBrainz5::CQuery 
    mbq((const char *)(QString("rivendell-")+VERSION).toUtf8(),
	  (const char *)rda->libraryConf()->mbServer().toUtf8());

  try {
    MusicBrainz5::CMetadata metadata=
      mbq.Query("discid",(const char *)discRecord()->discMbId().toUtf8());
    if(metadata.Disc()&&metadata.Disc()->ReleaseList()) {
      MusicBrainz5::CReleaseList *releases=metadata.Disc()->ReleaseList();
      if(releases->NumItems()==1) { // Exact Match
	result_code=ProcessRelease(releases->Item(0));
      }
      if(releases->NumItems()>1) {  // Multiple Matches
	int index;
	titlesKey()->clear();
	titlesBox()->clear();
	for(int i=0;i<releases->NumItems();i++) {
	  MusicBrainz5::CRelease *release=releases->Item(i);
	  QString barcode=RDDiscLookup::
	    formattedUpcA(QString::fromUtf8(release->Barcode().c_str()));
	  MusicBrainz5::CMediumList *media=release->MediumList();
	  QString format="";
	  for(int j=0;j<media->NumItems();j++) {
	    format=QString::fromUtf8(media->Item(j)->Format().c_str())+" | ";
	  }
	  format=format.left(format.length()-3);
	  QString title=QString::fromUtf8(release->Title().c_str())+"\n";
	  if(!format.isEmpty()) {
	    title+=" ["+format+"]";
	  }
	  if(!barcode.isEmpty()) {
	    title+=" [UPC "+barcode+"]";
	  }
	  titlesKey()->push_back(QString::fromUtf8(release->Title().c_str()));
	  titlesBox()->insertItem(titlesBox()->count(),title);
	}
	if((index=exec())>=0) {
	  result_code=ProcessRelease(releases->Item(index));
	}
	else {
	  result_code=RDDiscLookup::NoMatch;	  
	}
      }
    }
  }

  catch (MusicBrainz5::CConnectionError &err) {
    err_str="  "+tr("Connection Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString().sprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString().sprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CTimeoutError &err) {
    err_str="  "+tr("Timeout Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString().sprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString().sprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CAuthenticationError &err) {
    err_str="  "+tr("Authentication Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString().sprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString().sprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CFetchError &err) {
    err_str="  "+tr("Fetch Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString().sprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString().sprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CRequestError &err) {
    err_str="  "+tr("Request Exception")+"\n"+
      "  "+tr("Last Result")+": "+\
      QString().sprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString().sprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  emit lookupDone(result_code,err_str);
}


RDDiscLookup::Result RDMbLookup::ProcessRelease(MusicBrainz5::CRelease *release)
{
  MusicBrainz5::CQuery 
    mbq((const char *)(QString("rivendell-")+VERSION).toUtf8(),
	(const char *)rda->libraryConf()->mbServer().toUtf8());

  //
  // Extract Basic Release Data
  //
  discRecord()->setDiscReleaseMbId(QString::fromUtf8(release->ID().c_str()));
  discRecord()->setDiscAlbum(QString::fromUtf8(release->Title().c_str()));
  //discRecord()->setDiscGenre();
  QStringList f0=QString::fromUtf8(release->Date().c_str()).split("-");
  discRecord()->setDiscYear(f0.at(0).toInt());

  //
  // Extract Extended Release Data
  //
  MusicBrainz5::CQuery::tParamMap params;
  params["inc"]="artists labels recordings isrcs";
  MusicBrainz5::CMetadata metadata=mbq.Query("release",release->ID(),"",params);
  if(metadata.Release()) {
    //
    // Get Artist(s)
    //
    MusicBrainz5::CNameCreditList *credits=
      metadata.Release()->ArtistCredit()->NameCreditList();
    if(credits) {
      QString str="";
      for(int j=0;j<credits->NumItems();j++) {  // Amalgamate 'em!
	str+=
	  QString::fromUtf8(credits->Item(j)->Artist()->Name().c_str());
	str+=QString::fromUtf8(credits->Item(j)->JoinPhrase().c_str());
      }
      discRecord()->setDiscArtist(str);
    }

    //
    // Get Labels
    //
    MusicBrainz5::CLabelInfoList *labels=metadata.Release()->LabelInfoList();
    if(labels) {
      discRecord()->setDiscLabel(QString::fromUtf8(labels->Item(0)->
						   Label()->Name().c_str()));
    }

    //
    // Get Per-Track Data
    //
    MusicBrainz5::CMediumList *media=metadata.Release()->MediumList();
    if(media) {
      for(int j=0;j<media->NumItems();j++) {
	MusicBrainz5::CMedium *medium=media->Item(j);
	MusicBrainz5::CTrackList *tracks=medium->TrackList();
	for(int k=0;k<tracks->NumItems();k++) {
	  MusicBrainz5::CTrack *track=tracks->Item(k);
	  MusicBrainz5::CRecording *recording=track->Recording();
	  discRecord()->
	    setTrackTitle(k,QString::fromUtf8(recording->Title().c_str()));
	  discRecord()->
	    setTrackMbId(k,QString::fromUtf8(recording->ID().c_str()));
	  MusicBrainz5::CISRCList *isrcs=recording->ISRCList();
	  if(isrcs) {
	    if(isrcs->NumItems()>0) {
	      discRecord()->
		setIsrc(k,RDDiscLookup::normalizedIsrc(QString::fromUtf8(isrcs->
						       Item(0)->ID().c_str())));
	    }
	  }
	}
      }
    }
  }
  return RDDiscLookup::ExactMatch;
}
