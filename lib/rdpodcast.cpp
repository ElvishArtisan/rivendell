// rdpodcast.cpp
//
// Abstract a Rivendell Podcast
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdapplication.h"
#include "rdconf.h"
#include "rddb.h"
#include "rddelete.h"
#include "rdescape_string.h"
#include "rdpodcast.h"
#include "rdurl.h"
#include "rdxport_interface.h"

//
// CURL Callbacks
//
int __RDPodcast_Debug_Callback(CURL *handle,curl_infotype type,char *data,
			       size_t size,void *userptr)
{
  QStringList *lines=(QStringList *)userptr;

  if(type==CURLINFO_TEXT) {
    lines->push_back(QString::fromUtf8(QByteArray(data,size)));
  }

  return 0;
}


RDPodcast::RDPodcast(RDConfig *config,unsigned id)
{
  podcast_config=config;
  podcast_feed_id=0;

  RDSqlQuery *q;
  QString sql;

  podcast_id=id;
  sql=QString("select ")+
    "`FEEDS`.`ID`,"+      // 00
    "`FEEDS`.KEY_NAME "+  // 01
    "from `PODCASTS` left join `FEEDS` "+
    "on (`PODCASTS`.`FEED_ID`=`FEEDS`.`ID`) "+
    QString::asprintf("where `PODCASTS`.`ID`=%u",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    podcast_feed_id=q->value(0).toUInt();
    podcast_keyname=q->value(1).toString();
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
  return podcast_feed_id;
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


QString RDPodcast::sha1Hash() const
{
  return RDGetSqlValue("PODCASTS","ID",podcast_id,"SHA1_HASH").toString();
}


void RDPodcast::setSha1Hash(const QString &str) const
{
  SetRow("SHA1_HASH",str);
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


bool RDPodcast::dropAudio(RDFeed *feed,QString *err_text,bool log_debug) const
{
  if(!removePodcast()) {
    return false;
  }
  return DeletePodcast(podcast_id);
}


bool RDPodcast::removePodcast() const
{
  long response_code;
  CURL *curl=NULL;
  CURLcode curl_err;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;

  //
  // Generate POST Data
  //
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"COMMAND",
	       CURLFORM_COPYCONTENTS,
	       QString::asprintf("%u",RDXPORT_COMMAND_REMOVE_PODCAST).toUtf8().
	       constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       QString::asprintf("%u",podcast_id).toUtf8().constData(),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  QStringList *err_msgs=SetupCurlLogging(curl);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   rda->config()->userAgent().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_URL,
	    rda->station()->webServiceUrl(rda->config()).toUtf8().constData());
  rda->syslog(LOG_DEBUG,"using web service URL: %s",
	   rda->station()->webServiceUrl(rda->config()).toUtf8().constData());

  //
  // Send it
  //
  if((curl_err=curl_easy_perform(curl))!=CURLE_OK) {
    curl_easy_cleanup(curl);
    curl_formfree(first);
    ProcessCurlLogging("RDFeed::postPodcast()",err_msgs);
    return false;
  }

  //
  // Clean up
  //
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  curl_formfree(first);

  //
  // Process the results
  //
  if((response_code<200)||(response_code>299)) {
    ProcessCurlLogging("RDFeed::postPodcast()",err_msgs);
    return false;
  }
  delete err_msgs;

  return true;
}


QString RDPodcast::guid(const QString &url,const QString &filename,
			unsigned feed_id,unsigned cast_id)
{
  return url+"/"+filename+QString::asprintf("_%06u_%06u",feed_id,cast_id);
}


QString RDPodcast::guid(const QString &full_url,unsigned feed_id,
			unsigned cast_id)
{
  return full_url+QString::asprintf("_%06u_%06u",feed_id,cast_id);
}


bool RDPodcast::DeletePodcast(unsigned cast_id) const
{
  long response_code;
  CURL *curl=NULL;
  CURLcode curl_err;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;

  //
  // Generate POST Data
  //
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"COMMAND",
	       CURLFORM_COPYCONTENTS,
	       QString::asprintf("%u",RDXPORT_COMMAND_DELETE_PODCAST).toUtf8().
	       constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       QString::asprintf("%u",cast_id).toUtf8().constData(),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  QStringList *err_msgs=SetupCurlLogging(curl);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   rda->config()->userAgent().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_URL,
	    rda->station()->webServiceUrl(rda->config()).toUtf8().constData());
  rda->syslog(LOG_DEBUG,"using web service URL: %s",
	   rda->station()->webServiceUrl(rda->config()).toUtf8().constData());

  //
  // Send it
  //
  if((curl_err=curl_easy_perform(curl))!=CURLE_OK) {
    curl_easy_cleanup(curl);
    curl_formfree(first);
    ProcessCurlLogging("RDFeed::postPodcast()",err_msgs);
    return false;
  }

  //
  // Clean up
  //
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  curl_formfree(first);

  //
  // Process the results
  //
  if((response_code<200)||(response_code>299)) {
    ProcessCurlLogging("RDFeed::postPodcast()",err_msgs);
    return false;
  }
  delete err_msgs;

  return true;
}


QStringList *RDPodcast::SetupCurlLogging(CURL *curl) const
{
  QStringList *err_msgs=new QStringList();

  curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,__RDPodcast_Debug_Callback);
  curl_easy_setopt(curl,CURLOPT_DEBUGDATA,err_msgs);
  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);

  return err_msgs;
}


void RDPodcast::ProcessCurlLogging(const QString &label,
                               QStringList *err_msgs) const
{
  if(err_msgs->size()>0) {
    rda->syslog(LOG_ERR,"*** %s: extended CURL information begins ***",
               label.toUtf8().constData());
    for(int i=0;i<err_msgs->size();i++) {
      rda->syslog(LOG_ERR,"[%d]: %s",i,err_msgs->at(i).toUtf8().constData());
    }
    rda->syslog(LOG_ERR,"*** %s: extended CURL information ends ***",
               label.toUtf8().constData());
  }
  delete err_msgs;
}


void RDPodcast::SetRow(const QString &param,int value) const
{
  QString sql;

  sql=QString("update `PODCASTS` set `")+
    param+QString::asprintf("`=%d where ",value)+
    QString::asprintf("`ID`=%u",podcast_id);
  RDSqlQuery::apply(sql);
}


void RDPodcast::SetRow(const QString &param,const QString &value) const
{
  QString sql;

  if(value.isNull()) {
    sql=QString("update `PODCASTS` set `")+
      param+"`=NULL where "+
      QString::asprintf("`ID`=%u",podcast_id);
  }
  else {
    sql=QString("update `PODCASTS` set `")+
      param+"`='"+RDEscapeString(value)+"' where "+
      QString::asprintf("`ID`=%u",podcast_id);
  }
  RDSqlQuery::apply(sql);
}


void RDPodcast::SetRow(const QString &param,const QDateTime &value,
                       const QString &format) const
{
  QString sql;

  if(value.isNull()) {
    sql=QString("update `PODCASTS` set ")+
      param+"=NULL"+" where "+
      QString::asprintf("`ID`=%u",podcast_id);
  }
  else {
    sql=QString("update `PODCASTS` set `")+
      param+"`="+RDCheckDateTime(value, format)+" where "+
      QString::asprintf("`ID`=%u",podcast_id);
  }
  RDSqlQuery::apply(sql);
}
