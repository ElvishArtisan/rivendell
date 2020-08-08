// rdpodcast.cpp
//
// Abstract a Rivendell Podcast
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

#include <curl/curl.h>

#include <qurl.h>

#include "rdapplication.h"
#include "rdconf.h"
#include "rddb.h"
#include "rddelete.h"
#include "rdescape_string.h"
#include "rdpodcast.h"
#include "rdurl.h"

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
  rda->syslog(LOG_DEBUG,"CURL MSG: %s",str);
  return 0;
}


RDPodcast::RDPodcast(RDConfig *config,unsigned id)
{
  podcast_config=config;

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


bool RDPodcast::itemExplicit() const
{
  return RDBool(RDGetSqlValue("PODCASTS","ID",podcast_id,
			      "ITEM_EXPLICIT").toString());
}


void RDPodcast::setItemExplicit(bool state) const
{
  SetRow("ITEM_EXPLICIT",RDYesNo(state));
}


int RDPodcast::itemImageId() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ITEM_IMAGE_ID").toInt();
}


void RDPodcast::setItemImageId(int img_id) const
{
  SetRow("ITEM_IMAGE_ID",img_id);
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


QString RDPodcast::originLoginName() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ORIGIN_LOGIN_NAME").
    toString();
}


void RDPodcast::setOriginLoginName(const QString &str) const
{
  SetRow("ORIGIN_LOGIN_NAME",str);
}


QString RDPodcast::originStation() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"ORIGIN_STATION").toString();
}


void RDPodcast::setOriginStation(const QString &str) const
{
  SetRow("ORIGIN_STATION",str);
}


QDateTime RDPodcast::originDateTime() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "ORIGIN_DATETIME").toDateTime();
}


void RDPodcast::setOriginDateTime(const QDateTime &datetime) const
{
  SetRow("ORIGIN_DATETIME",datetime,"yyyy-MM-dd hh:mm:ss");
}


QDateTime RDPodcast::effectiveDateTime() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "EFFECTIVE_DATETIME").toDateTime();
}


void RDPodcast::setEffectiveDateTime(const QDateTime &datetime) const
{
  SetRow("EFFECTIVE_DATETIME",datetime,"yyyy-MM-dd hh:mm:ss");
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


QDateTime RDPodcast::expirationDateTime() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,
		       "EXPIRATION_DATETIME").toDateTime();
}


void RDPodcast::setExpirationDateTime(const QDateTime &dt) const
{
  SetRow("EXPIRATION_DATETIME",dt,"yyyy-MM-dd hh:mm:ss");
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
  RDDelete::ErrorCode conv_err;
  QUrl url(feed->purgeUrl()+"/"+audioFilename());
  RDDelete *conv=new RDDelete(rda->config());
  if(!conv->urlIsSupported(url)) {
    *err_text="unsupported url scheme";
    delete conv;
    return false;
  }
  conv->setTargetUrl(url);
  conv_err=conv->runDelete(feed->purgeUsername(),feed->purgePassword(),
			   rda->config()->logXloadDebugData());
  *err_text=RDDelete::errorText(conv_err);
  delete conv;

  return conv_err==RDDelete::ErrorOk;
}


QString RDPodcast::guid(const QString &url,const QString &filename,
			unsigned feed_id,unsigned cast_id)
{
  return url+"/"+filename+QString().sprintf("_%06u_%06u",feed_id,cast_id);
}


QString RDPodcast::guid(const QString &full_url,unsigned feed_id,
			unsigned cast_id)
{
  return full_url+QString().sprintf("_%06u_%06u",feed_id,cast_id);
}


void RDPodcast::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update PODCASTS set ")+
    param+QString().sprintf("=%d where ",value)+
    QString().sprintf("ID=%u",podcast_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDPodcast::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update PODCASTS set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    QString().sprintf("ID=%u",podcast_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDPodcast::SetRow(const QString &param,const QDateTime &value,
                       const QString &format) const
{
  QString sql;

  if(value.isNull()) {
    sql=QString("update PODCASTS set ")+
      param+"=NULL"+" where "+
      QString().sprintf("ID=%u",podcast_id);
  }
  else {
    sql=QString("update PODCASTS set ")+
      param+"="+RDCheckDateTime(value, format)+" where "+
      QString().sprintf("ID=%u",podcast_id);
  }
  RDSqlQuery::apply(sql);
}
