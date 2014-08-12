// rdpodcast.cpp
//
// Abstract a Rivendell Podcast
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpodcast.cpp,v 1.11 2010/11/24 16:57:38 cvs Exp $
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

#include <curl/curl.h>

#include <rddb.h>
#include <rdpodcast.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdurl.h>

//
// CURL Callbacks
//
int PodcastErrorCallback(CURL *curl,curl_infotype type,char *msg,size_t size,
			void *clientp)
{
  char str[1000];

  if(type!=CURLINFO_TEXT) {
    return 0;
  }
  if(size>999) {
    size=999;
  }
  memset(&str,0,size+1);
  memcpy(str,msg,size);
  syslog(LOG_DEBUG,"CURL MSG: %s",str);
  return 0;
}


RDPodcast::RDPodcast(unsigned id)
{
  RDSqlQuery *q;
  QString sql;

  podcast_id=id;
  sql=QString().sprintf("select FEEDS.KEY_NAME from \
                         PODCASTS left join FEEDS \
                         on (PODCASTS.FEED_ID=FEEDS.ID) \
                         where PODCASTS.ID=%u",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    podcast_keyname=q->value(0).toString();
  }
  delete q;
}


unsigned RDPodcast::id() const
{
  return podcast_id;
}


QString RDPodcast::keyName() const
{
  return podcast_keyname;
}


bool RDPodcast::exists() const
{
  return RDDoesRowExist("PODCASTS","ID",podcast_id);
}


unsigned RDPodcast::feedId() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"FEED_ID").
    toUInt();
}


void RDPodcast::setFeedId(unsigned id) const
{
  SetRow("FEED_ID",id);
}


QString RDPodcast::itemTitle() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_TITLE").
    toString();
}


void RDPodcast::setItemTitle(const QString &str) const
{
  SetRow("ITEM_TITLE",str);
}


QString RDPodcast::itemDescription() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "ITEM_DESCRIPTION").toString();
}


void RDPodcast::setItemDescription(const QString &str) const
{
  SetRow("ITEM_DESCRIPTION",str);
}


QString RDPodcast::itemCategory() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_CATEGORY").
    toString();
}


void RDPodcast::setItemCategory(const QString &str) const
{
  SetRow("ITEM_CATEGORY",str);
}


QString RDPodcast::itemLink() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_LINK").
    toString();
}


void RDPodcast::setItemLink(const QString &str) const
{
  SetRow("ITEM_LINK",str);
}


QString RDPodcast::itemAuthor() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_AUTHOR").
    toString();
}


void RDPodcast::setItemAuthor(const QString &str) const
{
  SetRow("ITEM_AUTHOR",str);
}


QString RDPodcast::itemComments() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_COMMENTS").
    toString();
}


void RDPodcast::setItemComments(const QString &str) const
{
  SetRow("ITEM_COMMENTS",str);
}


QString RDPodcast::itemSourceText() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_SOURCE_TEXT").
    toString();
}


void RDPodcast::setItemSourceText(const QString &str) const
{
  SetRow("ITEM_SOURCE_TEXT",str);
}


QString RDPodcast::itemSourceUrl() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_SOURCE_URL").
    toString();
}


void RDPodcast::setItemSourceUrl(const QString &str) const
{
  SetRow("ITEM_SOURCE_URL",str);
}


QDateTime RDPodcast::originDateTime() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "ORIGIN_DATETIME").toDateTime();
}


void RDPodcast::setOriginDateTime(const QDateTime &datetime) const
{
  SetRow("ORIGIN_DATETIME",datetime.toString("yyyy-MM-dd hh:mm:ss"));
}


QDateTime RDPodcast::effectiveDateTime() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "EFFECTIVE_DATETIME").toDateTime();
}


void RDPodcast::setEffectiveDateTime(const QDateTime &datetime) const
{
  SetRow("EFFECTIVE_DATETIME",datetime.toString("yyyy-MM-dd hh:mm:ss"));
}


QString RDPodcast::audioFilename() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "AUDIO_FILENAME").toString();
}


void RDPodcast::setAudioFilename(const QString &str) const
{
  SetRow("AUDIO_FILENAME",str);
}


int RDPodcast::audioLength() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"AUDIO_LENGTH").
    toUInt();
}


void RDPodcast::setAudioLength(int len) const
{
  SetRow("AUDIO_LENGTH",len);
}


int RDPodcast::audioTime() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"AUDIO_TIME").
    toUInt();
}


void RDPodcast::setAudioTime(int msecs) const
{
  SetRow("AUDIO_TIME",msecs);
}


unsigned RDPodcast::shelfLife() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"SHELF_LIFE").
    toUInt();
}


void RDPodcast::setShelfLife(unsigned days) const
{
  SetRow("SHELF_LIFE",days);
}


RDPodcast::Status RDPodcast::status() const
{
  return (RDPodcast::Status)RDGetSqlValue("PODCASTS","ID",
					  podcast_id,"STATUS").toUInt();
}


void RDPodcast::setStatus(RDPodcast::Status status)
{
  SetRow("STATUS",(unsigned)status);
}


bool RDPodcast::removeAudio(RDFeed *feed,QString *err_text,bool log_debug) const
{
  CURL *curl=NULL;
  struct curl_slist *cmds=NULL;
  CURLcode err;
  QUrl *url;
  bool ret=true;
  QString currentdir;
  char urlstr[1024];
  char userpwd[256];

  if((curl=curl_easy_init())==NULL) {
    syslog(LOG_ERR,"unable to initialize curl library\n");
    return false;
  }
  url=new QUrl(feed->purgeUrl());
  strncpy(urlstr,(const char *)(url->protocol()+"://"+url->host()+"/"),1024);
  curl_easy_setopt(curl,CURLOPT_URL,urlstr);
  strncpy(userpwd,(const char *)QString().
		   sprintf("%s:%s",(const char *)feed->purgeUsername(),
			   (const char *)feed->purgePassword()),256);
  curl_easy_setopt(curl,CURLOPT_USERPWD,userpwd);
  curl_easy_setopt(curl,CURLOPT_HTTPAUTH,CURLAUTH_ANY);
  if(log_debug) {
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,PodcastErrorCallback);
  }
  currentdir="";
  if(!url->dirPath().right(url->dirPath().length()-1).isEmpty()) {
    currentdir=url->dirPath().right(url->dirPath().length()-1)+"/";
  }
  if(!url->fileName().isEmpty()) {
    currentdir+=url->fileName()+"/";
  }
  if(!currentdir.isEmpty()) {
    cmds=curl_slist_append(cmds,QString().sprintf("cwd %s",
						  (const char *)currentdir));
  }
  cmds=curl_slist_append(cmds, QString().sprintf("dele %s",
	    (const char *)audioFilename()));
  curl_easy_setopt(curl,CURLOPT_QUOTE,cmds);
  switch((err=curl_easy_perform(curl))) {
  case CURLE_OK:
#ifdef CURLE_QUOTE_ERROR
  case CURLE_QUOTE_ERROR:  // In case the file is already gone
#endif  // CURLE_QUOTE_ERROR
    ret=true;
    break;

  default:
    ret=false;
    break;
  }
  if(err_text!=NULL) {
    *err_text=curl_easy_strerror(err);
  }
  curl_slist_free_all(cmds);
  curl_easy_cleanup(curl);
  delete url;
    
  return ret;
}


QString RDPodcast::guid(const QString &url,const QString &filename,
			unsigned feed_id,unsigned cast_id)
{
  return QString().sprintf("%s/%s_%06u_%06u",
			   (const char *)url,(const char *)filename,
			   feed_id,cast_id);
}


QString RDPodcast::guid(const QString &full_url,unsigned feed_id,
			unsigned cast_id)
{
  return QString().sprintf("%s_%06u_%06u",
			   (const char *)full_url,feed_id,cast_id);
}


void RDPodcast::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE PODCASTS SET %s=%d WHERE ID=%u",
			(const char *)param,
			value,
			podcast_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDPodcast::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE PODCASTS SET %s=\"%s\" WHERE ID=%u",
			(const char *)param,
			(const char *)RDEscapeString(value),
			podcast_id);
  q=new RDSqlQuery(sql);
  delete q;
}
