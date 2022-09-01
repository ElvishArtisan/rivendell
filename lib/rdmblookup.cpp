//   rdmblookup.cpp
//
//   RDDiscLookup instance class for MusicBrainz
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

#include <errno.h>

#include <fstream>

#include <coverart/CoverArt.h>
#include <coverart/HTTPFetch.h>

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

#include <qapplication.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpixmapcache.h>

#include "rdconf.h"
#include "rdmblookup.h"

#include "../icons/cover_art_default-60x60.xpm"

QString err_str="OK";
RDDiscLookup::Result result_code=RDDiscLookup::ExactMatch;

RDMbLookup::RDMbLookup(const QString &caption,FILE *profile_msgs,
			     QWidget *parent)
  : RDDiscLookup(caption,profile_msgs,parent)
{
  setWindowTitle(caption+" - MusicBrainz "+tr("Lookup"));

  cover_art_default_icon=new QIcon(QPixmap(cover_art_default_60x60_xpm));

  titlesBox()->setIconSize(QSize(60,60));

  char tempdir[PATH_MAX];

  //
  // Create temporary directory
  //
  temp_directory=NULL;
  strncpy(tempdir,"/tmp",PATH_MAX);
  if(getenv("TEMP")!=NULL) {
    strncpy(tempdir,getenv("TEMP"),PATH_MAX-1);
  }
  strncat(tempdir,"/rivendell-XXXXXX",PATH_MAX-strlen(tempdir));
  if(mkdtemp(tempdir)==NULL) {
    rda->syslog(LOG_WARNING,"unable to create temporary directory [%s]",
		strerror(errno));
  }
  temp_directory=new QDir(tempdir);
  rda->syslog(LOG_DEBUG,
	      "using working directory \"%s\" for cover art processing",
	      (const char *)temp_directory->path().toUtf8());
}


RDMbLookup::~RDMbLookup()
{
  QStringList files=temp_directory->entryList(QDir::Files);
  for(int i=0;i<files.size();i++) {
    unlink((temp_directory->path()+"/"+files[i]).toUtf8());
  }
  rmdir(temp_directory->path().toUtf8());
  rda->syslog(LOG_DEBUG,"deleted working directory \"%s\"",
	      (const char *)temp_directory->path().toUtf8());
  delete temp_directory;
}


QSize RDMbLookup::sizeHint() const
{
  return QSize(500,160);
}


QString RDMbLookup::sourceName() const
{
  return QString("MusicBrainz");
}


QPixmap RDMbLookup::sourceLogo() const
{
  return RDLibraryConf::cdServerLogo(RDLibraryConf::MusicBrainzType);
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
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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
	  titlesBox()->insertItem(titlesBox()->count(),GetReleaseCover(QString::fromUtf8(release->ID().c_str())),title);
	}
	QApplication::restoreOverrideCursor();
	if((index=exec())>=0) {
	  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	  result_code=ProcessRelease(releases->Item(index));
	}
	else {
	  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	  result_code=RDDiscLookup::NoMatch;	  
	}
      }
    }
  }

  catch (MusicBrainz5::CConnectionError &err) {
    err_str="  "+tr("Connection Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString::asprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString::asprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CTimeoutError &err) {
    err_str="  "+tr("Timeout Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString::asprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString::asprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CAuthenticationError &err) {
    err_str="  "+tr("Authentication Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString::asprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString::asprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CFetchError &err) {
    err_str="  "+tr("Fetch Exception")+"\n"+
      "  "+tr("Last Result")+": "+
      QString::asprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString::asprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CRequestError &err) {
    err_str="  "+tr("Request Exception")+"\n"+
      "  "+tr("Last Result")+": "+\
      QString::asprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString::asprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::LookupError;
  }
  catch (MusicBrainz5::CResourceNotFoundError &err) {
    err_str="  "+tr("Resource Not Found")+"\n"+
      "  "+tr("Last Result")+": "+\
      QString::asprintf("%d",mbq.LastResult())+"\n"+
      "  "+tr("LastHTTPCode")+": "+
      QString::asprintf("%d",mbq.LastHTTPCode())+"\n"+
      "  "+tr("LastErrorMessage")+": "+
      QString::fromUtf8(mbq.LastErrorMessage().c_str());
    result_code=RDDiscLookup::NoMatch;
  }
  QApplication::restoreOverrideCursor();
  processLookup(result_code,err_str);
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
  discRecord()->setDiscAlbum(RDDiscRecord::RemoteSource,
                            QString::fromUtf8(release->Title().c_str()));
  //discRecord()->setDiscGenre();
  QStringList f0=QString::fromUtf8(release->Date().c_str()).split("-");
  discRecord()->setDiscYear(f0.at(0).toInt());

  //
  // Extract Extended Release Data
  //
  MusicBrainz5::CQuery::tParamMap params;
  params["inc"]="artists labels recordings isrcs discids";
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
      discRecord()->setDiscArtist(RDDiscRecord::RemoteSource,str);
    }

    //
    // Get Labels
    //
    MusicBrainz5::CLabelInfoList *labels=metadata.Release()->LabelInfoList();
    if(labels&&labels->Item(0)) {
      discRecord()->setDiscLabel(QString::fromUtf8(labels->Item(0)->
						   Label()->Name().c_str()));
    }

    //
    // Get Per-Track Data
    //
    MusicBrainz5::CMediumList media=metadata.Release()->MediaMatchingDiscID(discRecord()->discMbId().toStdString());
    for(int j=0;j<media.NumItems();j++) {
      MusicBrainz5::CMedium *medium=media.Item(j);
      MusicBrainz5::CTrackList *tracks=medium->TrackList();
      for(int k=0;k<tracks->NumItems();k++) {
	MusicBrainz5::CTrack *track=tracks->Item(k);
	MusicBrainz5::CRecording *recording=track->Recording();
	discRecord()->
	  setTrackTitle(RDDiscRecord::RemoteSource,k,
			QString::fromUtf8(recording->Title().c_str()));
	discRecord()->
	  setTrackRecordingMbId(k,QString::fromUtf8(recording->ID().c_str()));
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
  return RDDiscLookup::ExactMatch;
}


QIcon RDMbLookup::GetReleaseCover(const QString &mbid) const
{
  CoverArtArchive::CCoverArt ca((QString("rivendell-")+VERSION).toStdString());
  err_str="";

  try {
    QPixmap pix;
    QString key="$coverart-front-"+mbid;

    if(QPixmapCache::find(key,&pix)) {
      return QIcon(pix);
    }

    std::vector<unsigned char> image=ca.FetchFront(mbid.toStdString());
    if(image.size()) {
      std::stringstream filename;
      filename << temp_directory->path().toStdString() << "/" <<
	mbid.toStdString();

      std::ofstream front(filename.str().c_str());
      front.write((const char *)&image[0],image.size());
      front.close();

      pix=QPixmap(QString(filename.str().c_str()));
      unlink(QString::fromUtf8(filename.str().c_str()).toUtf8());
      if((pix.width()==0)||(pix.height()==0)) {
	return *cover_art_default_icon;
      }
      QPixmapCache::insert(key,pix);
      return QIcon(pix);
    }
  }

  catch (CoverArtArchive::CConnectionError &err) {
    err_str="ConnectionError when downloading coverart ["+
      QString::fromUtf8(ca.LastErrorMessage().c_str())+"]";
  }
  catch (CoverArtArchive::CTimeoutError &err) {
    err_str="TimeoutError when downloading coverart ["+
      QString::fromUtf8(ca.LastErrorMessage().c_str())+"]";
  }
  catch (CoverArtArchive::CAuthenticationError &err) {
    err_str="AuthenticationError when downloading coverart ["+
      QString::fromUtf8(ca.LastErrorMessage().c_str())+"]";
  }
  catch (CoverArtArchive::CFetchError &err) {
    err_str="FetchError when downloading coverart ["+
      QString::fromUtf8(ca.LastErrorMessage().c_str())+"]";
  }
  catch (CoverArtArchive::CRequestError &err) {
    err_str="RequestError when downloading coverart ["+
      QString::fromUtf8(ca.LastErrorMessage().c_str())+"]";
  }
  catch (CoverArtArchive::CResourceNotFoundError &err) {
  }

  if(!err_str.isEmpty()) {
    rda->syslog(LOG_WARNING,"%s",(const char *)err_str.toUtf8());
  }
  return *cover_art_default_icon;
}
