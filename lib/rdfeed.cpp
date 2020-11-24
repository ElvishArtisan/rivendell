// rdfeed.cpp
//
// Abstract a Rivendell RSS Feed
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

#include <errno.h>
#include <math.h>

#include <curl/curl.h>

#include <qapplication.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qurl.h>

#include "rdapplication.h"
#include "rdaudioconvert.h"
#include "rdaudioexport.h"
#include "rdcart.h"
#include "rdcut.h"
#include "rdconf.h"
#include "rddb.h"
#include "rddelete.h"
#include "rdescape_string.h"
#include "rdfeed.h"
#include "rdlibrary_conf.h"
#include "rdlog.h"
#include "rdlog_event.h"
#include "rdpodcast.h"
#include "rdrenderer.h"
#include "rdtempdirectory.h"
#include "rdupload.h"
#include "rdwavefile.h"
#include "rdxport_interface.h"

RDFeed::RDFeed(const QString &keyname,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  RDSqlQuery *q;
  QString sql;

  feed_keyname=keyname;
  feed_config=config;

  sql=QString("select ID from FEEDS where ")+
    "KEY_NAME=\""+RDEscapeString(keyname)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    feed_id=q->value(0).toUInt();
  }
  delete q;

  //
  // Get the CGI Hostname
  //
  if(getenv("SERVER_NAME")!=NULL) {
    feed_cgi_hostname=getenv("SERVER_NAME");
  }
}


RDFeed::RDFeed(unsigned id,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  RDSqlQuery *q;
  QString sql;

  feed_id=id;
  feed_config=config;

  sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%u",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    feed_keyname=q->value(0).toString();
  }
  delete q;
}


bool RDFeed::exists() const
{
  return RDDoesRowExist("FEEDS","KEY_NAME",feed_keyname);
}


bool RDFeed::isSuperfeed() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"IS_SUPERFEED").
		toString());
  
}


void RDFeed::setIsSuperfeed(bool state) const
{
  SetRow("IS_SUPERFEED",RDYesNo(state));
}


QStringList RDFeed::subfeedNames() const
{
  QString sql;
  RDSqlQuery *q=NULL;
  QStringList ret;

  sql=QString("select ")+
    "MEMBER_KEY_NAME "+  // 00
    "from SUPERFEED_MAPS where "+
    "KEY_NAME=\""+RDEscapeString(keyName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ret.push_back(q->value(0).toString());
  }
  delete q;

  return ret;
}


QStringList RDFeed::isSubfeedOf() const
{
  QStringList ret;

  QString sql=QString("select ")+
    "KEY_NAME "+  // 00
    "from SUPERFEED_MAPS where "+
    "MEMBER_KEY_NAME=\""+RDEscapeString(keyName())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    ret.push_back(q->value(0).toString());
  }
  delete q;

  return ret;
}


QString RDFeed::keyName() const
{
  return feed_keyname;
}


unsigned RDFeed::id() const
{
  return feed_id;
}


QString RDFeed::channelTitle() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_TITLE").
    toString();
}


void RDFeed::setChannelTitle(const QString &str) const
{
  SetRow("CHANNEL_TITLE",str);
}


QString RDFeed::channelDescription() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_DESCRIPTION").
    toString();
}


void RDFeed::setChannelDescription(const QString &str) const
{
  SetRow("CHANNEL_DESCRIPTION",str);
}


QString RDFeed::channelCategory() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_CATEGORY").
    toString();
}


void RDFeed::setChannelCategory(const QString &str) const
{
  SetRow("CHANNEL_CATEGORY",str);
}


QString RDFeed::channelSubCategory() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_SUB_CATEGORY").
    toString();
}


void RDFeed::setChannelSubCategory(const QString &str) const
{
  SetRow("CHANNEL_SUB_CATEGORY",str);
}


QString RDFeed::channelLink() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_LINK").
    toString();
}


void RDFeed::setChannelLink(const QString &str) const
{
  SetRow("CHANNEL_LINK",str);
}


QString RDFeed::channelCopyright() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_COPYRIGHT").
    toString();
}


void RDFeed::setChannelCopyright(const QString &str) const
{
  SetRow("CHANNEL_COPYRIGHT",str);
}


QString RDFeed::channelWebmaster() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_WEBMASTER").
    toString();
}



void RDFeed::setChannelWebmaster(const QString &str) const
{
  SetRow("CHANNEL_WEBMASTER",str);
}


QString RDFeed::channelEditor() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_EDITOR").
    toString();
}


void RDFeed::setChannelEditor(const QString &str) const
{
  SetRow("CHANNEL_EDITOR",str);
}


QString RDFeed::channelAuthor() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_AUTHOR").
    toString();
}


void RDFeed::setChannelAuthor(const QString &str) const
{
  SetRow("CHANNEL_AUTHOR",str);
}


bool RDFeed::channelAuthorIsDefault() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "CHANNEL_AUTHOR_IS_DEFAULT").toString());
}


void RDFeed::setChannelAuthorIsDefault(bool state) const
{
  SetRow("CHANNEL_AUTHOR_IS_DEFAULT",RDYesNo(state));
}


QString RDFeed::channelOwnerName() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_OWNER_NAME").
    toString();
}


void RDFeed::setChannelOwnerName(const QString &str) const
{
  SetRow("CHANNEL_OWNER_NAME",str);
}


QString RDFeed::channelOwnerEmail() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_OWNER_EMAIL").
    toString();
}


void RDFeed::setChannelOwnerEmail(const QString &str) const
{
  SetRow("CHANNEL_OWNER_EMAIL",str);
}


QString RDFeed::channelLanguage() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_LANGUAGE").
    toString();
}


void RDFeed::setChannelLanguage(const QString &str)
{
  SetRow("CHANNEL_LANGUAGE",str);
}


bool RDFeed::channelExplicit() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "CHANNEL_EXPLICIT").toString());
}


void RDFeed::setChannelExplicit(bool state) const
{
  SetRow("CHANNEL_EXPLICIT",RDYesNo(state));
}


int RDFeed::channelImageId() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_IMAGE_ID").
    toInt();
}


void RDFeed::setChannelImageId(int img_id) const
{
  SetRow("CHANNEL_IMAGE_ID",img_id);
}


int RDFeed::defaultItemImageId() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"DEFAULT_ITEM_IMAGE_ID").
    toInt();
}


void RDFeed::setDefaultItemImageId(int img_id) const
{
  SetRow("DEFAULT_ITEM_IMAGE_ID",img_id);
}


QString RDFeed::baseUrl(const QString &subfeed_key_name) const
{
  QString key_name=subfeed_key_name;
  if(subfeed_key_name.isEmpty()) {
    key_name=feed_keyname;
  }
  return RDGetSqlValue("FEEDS","KEY_NAME",key_name,"BASE_URL").toString();  
}


QString RDFeed::baseUrl(int subfeed_feed_id) const
{
  int id=subfeed_feed_id;
  if(subfeed_feed_id<0) {
    id=feed_id;
  }
  return RDGetSqlValue("FEEDS","ID",id,"BASE_URL").toString();  
}



void RDFeed::setBaseUrl(const QString &str) const
{
  SetRow("BASE_URL",str);
}


QString RDFeed::basePreamble() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"BASE_PREAMBLE").
    toString();
}


void RDFeed::setBasePreamble(const QString &str) const
{
  SetRow("BASE_PREAMBLE",str);
}


QString RDFeed::purgeUrl() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"PURGE_URL").
    toString();
}


void RDFeed::setPurgeUrl(const QString &str) const
{
  SetRow("PURGE_URL",str);
}


QString RDFeed::purgeUsername() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"PURGE_USERNAME").
    toString();
}


void RDFeed::setPurgeUsername(const QString &str) const
{
  SetRow("PURGE_USERNAME",str);
}


QString RDFeed::purgePassword() const
{
  return QString(QByteArray::fromBase64(RDGetSqlValue("FEEDS","KEY_NAME",
			 feed_keyname,"PURGE_PASSWORD").toString().toUtf8()));
}


void RDFeed::setPurgePassword(const QString &str) const
{
  SetRow("PURGE_PASSWORD",QString(str.toUtf8().toBase64()));
}


bool RDFeed::purgeUseIdFile() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "PURGE_USE_ID_FILE").toString());
}


void RDFeed::setPurgeUseIdFile(bool state) const
{
  SetRow("PURGE_USE_ID_FILE",RDYesNo(state));
}


RDRssSchemas::RssSchema RDFeed::rssSchema() const
{
  return (RDRssSchemas::RssSchema)RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
					       "RSS_SCHEMA").toUInt();
}


void RDFeed::setRssSchema(RDRssSchemas::RssSchema schema) const
{
  SetRow("RSS_SCHEMA",(unsigned)schema);
}


QString RDFeed::headerXml() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"HEADER_XML").
    toString();
}


void RDFeed::setHeaderXml(const QString &str)
{
  SetRow("HEADER_XML",str);
}


QString RDFeed::channelXml() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_XML").
    toString();
}


void RDFeed::setChannelXml(const QString &str)
{
  SetRow("CHANNEL_XML",str);
}


QString RDFeed::itemXml() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"ITEM_XML").
    toString();
}


void RDFeed::setItemXml(const QString &str)
{
  SetRow("ITEM_XML",str);
}


QString RDFeed::feedUrl() const
{
  return purgeUrl()+"/"+keyName()+"."+RD_RSS_XML_FILE_EXTENSION;
}


bool RDFeed::castOrder() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "CAST_ORDER").toString());
}


void RDFeed::setCastOrder(bool state) const
{
  SetRow("CAST_ORDER",RDYesNo(state));
}


int RDFeed::maxShelfLife() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"MAX_SHELF_LIFE").toInt();
}


void RDFeed::setMaxShelfLife(int days)
{
  SetRow("MAX_SHELF_LIFE",days);
}


QDateTime RDFeed::lastBuildDateTime() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"LAST_BUILD_DATETIME").
    toDateTime();
}


void RDFeed::setLastBuildDateTime(const QDateTime &datetime) const
{
  SetRow("LAST_BUILD_DATETIME",datetime,"yyyy-MM-dd hh:mm:ss");
}


QDateTime RDFeed::originDateTime() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"ORIGIN_DATETIME").
    toDateTime();
}


void RDFeed::setOriginDateTime(const QDateTime &datetime) const
{
  SetRow("ORIGIN_DATETIME",datetime,"yyyy-MM-dd hh:mm:ss");
}


bool RDFeed::enableAutopost() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "ENABLE_AUTOPOST").toString());
}


void RDFeed::setEnableAutopost(bool state) const
{
  SetRow("ENABLE_AUTOPOST",RDYesNo(state));
}


RDSettings::Format RDFeed::uploadFormat() const
{
  return (RDSettings::Format)RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
					   "UPLOAD_FORMAT").toInt();
}


void RDFeed::setUploadFormat(RDSettings::Format fmt) const
{
  SetRow("UPLOAD_FORMAT",(int)fmt);
}


int RDFeed::uploadChannels() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"UPLOAD_CHANNELS").
    toInt();
}


void RDFeed::setUploadChannels(int chans) const
{
  SetRow("UPLOAD_CHANNELS",chans);
}


int RDFeed::uploadQuality() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"UPLOAD_QUALITY").
    toInt();
}


void RDFeed::setUploadQuality(int qual) const
{
  SetRow("UPLOAD_QUALITY",qual);
}


int RDFeed::uploadBitRate() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"UPLOAD_BITRATE").
    toInt();
}


void RDFeed::setUploadBitRate(int rate) const
{
  SetRow("UPLOAD_BITRATE",rate);
}


int RDFeed::uploadSampleRate() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"UPLOAD_SAMPRATE").
    toInt();
}


void RDFeed::setUploadSampleRate(int rate) const
{
  SetRow("UPLOAD_SAMPRATE",rate);
}


QString RDFeed::uploadExtension() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"UPLOAD_EXTENSION").
    toString();
}


void RDFeed::setUploadExtension(const QString &str)
{
  SetRow("UPLOAD_EXTENSION",str);
}


QString RDFeed::uploadMimetype() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"UPLOAD_MIMETYPE").
    toString();
}


void RDFeed::setUploadMimetype(const QString &str)
{
  SetRow("UPLOAD_MIMETYPE",str);
}


int RDFeed::normalizeLevel() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"NORMALIZE_LEVEL").
    toInt();
}


void RDFeed::setNormalizeLevel(int lvl) const
{
  SetRow("NORMALIZE_LEVEL",lvl);
}


QByteArray RDFeed::imageData(int img_id) const
{
  return RDGetSqlValue("FEED_IMAGES","ID",img_id,"DATA").toByteArray();
}


int RDFeed::importImageFile(const QString &pathname,QString *err_msg,
			    QString desc) const
{
  bool ok=false;
  QString sql;
  int ret;
  QSize min=rda->rssSchemas()->minimumImageSize(rssSchema());
  QSize max=rda->rssSchemas()->maximumImageSize(rssSchema());
  *err_msg="OK";

  //
  // Load the image
  //
  QFile file(pathname);
  if(!file.open(QIODevice::ReadOnly)) {
    *err_msg=QString("Unable to open image file [")+
      QString(strerror(errno))+"]";
    return -1;
  }
  QByteArray data=file.readAll();
  file.close();

  //
  // Validate the image
  //
  QImage *img=new QImage();
  if(!img->loadFromData(data)) {
    *err_msg="Invalid image file!";
    return -1;
  }
  if((!min.isNull())&&
     ((img->width()<min.width())||(img->height()<min.height()))) {
    *err_msg=
      QString().sprintf("Image is too small - %dx%d or larger required",
			min.width(),min.height());
    return -1;
  }
  if((!max.isNull())&&
     ((img->width()>max.width())||(img->height()>max.height()))) {
    *err_msg=
      QString().sprintf("Image is too large - %dx%d or smaller required",
			max.width(),max.height());
    return -1;
  }

  //
  // Fix up the Description
  //
  if(desc.isEmpty()) {
    desc=tr("Imported from")+" "+pathname;
  }

  //
  // FIXME: Upload to remote file store here...
  //

  //
  // Write it to the DB
  //
  QStringList f0=pathname.split(".",QString::SkipEmptyParts);
  sql=QString("insert into FEED_IMAGES set ")+
    QString().sprintf("FEED_ID=%u,",id())+
    "FEED_KEY_NAME=\""+RDEscapeString(keyName())+"\","+
    QString().sprintf("WIDTH=%d,",img->width())+
    QString().sprintf("HEIGHT=%d,",img->height())+
    QString().sprintf("DEPTH=%d,",img->depth())+
    "DESCRIPTION=\""+RDEscapeString(desc)+"\","+
    "FILE_EXTENSION=\""+RDEscapeString(f0.last().toLower())+"\","+
    "DATA="+RDEscapeBlob(data);
  ret=RDSqlQuery::run(sql,&ok).toInt();
  if(!ok) {
    *err_msg="Unable to write to database";
    return -1;
  }

  return ret;
}


bool RDFeed::deleteImage(int img_id,QString *err_msg)
{
  QString sql;
  RDSqlQuery *q=NULL;

  *err_msg="OK";

  removeImage(img_id);

  sql=QString("delete from FEED_IMAGES where ")+
    QString().sprintf("ID=%d",img_id);
  if(!RDSqlQuery::apply(sql,err_msg)) {
    *err_msg=QString("database error: ")+*err_msg;
    delete q;
    return false;
  }
  delete q;

  return true;
}


bool RDFeed::postPodcast(unsigned cast_id) const
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
	     (const char *)QString().sprintf("%u",RDXPORT_COMMAND_POST_PODCAST),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",cast_id),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent());
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
    return false;
  }

  return true;
}


QString RDFeed::audioUrl(unsigned cast_id)
{
  RDPodcast *cast=new RDPodcast(feed_config,cast_id);
  QUrl url(baseUrl(cast->feedId()));
  QString ret;

  ret=url.toString()+"/"+cast->audioFilename();
  delete cast;

  return ret;
}


QString RDFeed::imageUrl(int img_id) const
{
  QString ret;

  QString sql=QString("select ")+
    "FEED_ID,"+         // 00
    "FILE_EXTENSION "+  // 01
    "from FEED_IMAGES where "+
    QString().sprintf("ID=%d",img_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=baseUrl(q->value(0).toUInt())+"/"+
      RDFeed::imageFilename(id(),img_id,q->value(1).toString());
  }
  delete q;

  return ret;
}


bool RDFeed::postXml()
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
	     (const char *)QString().sprintf("%u",RDXPORT_COMMAND_POST_RSS),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",feed_id),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent());
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
    return false;
  }

  return true;
}


bool RDFeed::postXmlConditional(const QString &caption,QWidget *widget)
{
  if(!postXml()) {
    QMessageBox::warning(widget,caption+" - "+tr("Error"),
			 tr("XML data upload failed!"));
    return false;
  }
  return true;
}


bool RDFeed::removeRss()
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
	     (const char *)QString().sprintf("%u",RDXPORT_COMMAND_REMOVE_RSS),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",feed_id),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent());
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
    return false;
  }

  return true;
}


bool RDFeed::postImage(int img_id) const
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
	     (const char *)QString().sprintf("%u",RDXPORT_COMMAND_POST_IMAGE),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",img_id),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent());
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
    return false;
  }

  return true;
}


bool RDFeed::removeImage(int img_id) const
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
	     (const char *)QString().sprintf("%u",RDXPORT_COMMAND_REMOVE_IMAGE),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",img_id),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent());
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
    return false;
  }

  return true;
}


void RDFeed::removeAllImages()
{
  QString sql;
  RDSqlQuery *q=NULL;

  sql=QString("select ")+
    "ID "+  // 00
    "from FEED_IMAGES where "+
    QString().sprintf("FEED_ID=%u",feed_id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    removeImage(q->value(0).toUInt());
  }
  delete q;
}


unsigned RDFeed::postCut(const QString &cutname,Error *err)
{
  QString err_msg;
  QString tmpfile;
  QString destfile;
  RDPodcast *cast=NULL;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDAudioExport::ErrorCode export_err;

  emit postProgressRangeChanged(0,5);
  emit postProgressChanged(0);

  //
  // Export Cut
  //
  tmpfile=GetTempFilename();
  RDCut *cut=new RDCut(cutname);
  if(!cut->exists()) {
    delete cut;
    *err=RDFeed::ErrorCannotOpenFile;
    return 0;
  }
  RDAudioExport *conv=new RDAudioExport(this);
  conv->setCartNumber(cut->cartNumber());
  conv->setCutNumber(cut->cutNumber());
  conv->setDestinationFile(tmpfile);
  conv->setRange(cut->startPoint(),cut->endPoint());
  RDSettings *settings=new RDSettings();
  settings->setFormat(uploadFormat());
  settings->setChannels(uploadChannels());
  settings->setSampleRate(uploadSampleRate());
  settings->setBitRate(uploadBitRate());
  settings->setNormalizationLevel(normalizeLevel()/100);
  conv->setDestinationSettings(settings);
  emit postProgressChanged(1);
  switch((export_err=conv->
	  runExport(rda->user()->name(),rda->user()->password(),
		    &audio_conv_err))) {
  case RDAudioExport::ErrorOk:
    break;

  case RDAudioExport::ErrorInvalidSettings:
    delete settings;
    delete conv;
    *err=RDFeed::ErrorUnsupportedType;
    unlink(tmpfile);
    emit postProgressChanged(5);
    return 0;

  case RDAudioExport::ErrorNoSource:
  case RDAudioExport::ErrorNoDestination:
  case RDAudioExport::ErrorInternal:
  case RDAudioExport::ErrorUrlInvalid:
  case RDAudioExport::ErrorService:
  case RDAudioExport::ErrorInvalidUser:
  case RDAudioExport::ErrorAborted:
  case RDAudioExport::ErrorConverter:
    delete settings;
    delete conv;
    *err=RDFeed::ErrorGeneral;
    unlink(tmpfile);
    emit postProgressChanged(5);
    return 0;
  }
  delete settings;
  delete conv;
  postProgressChanged(2);

  //
  // Save to Audio Store
  //
  QFile file(tmpfile);
  int length=file.size();
  unsigned cast_id=CreateCast(&destfile,length,cut->length());
  cast=new RDPodcast(feed_config,cast_id);
  SavePodcast(cast_id,tmpfile);
  unlink(tmpfile);

  //
  // Upload to remote archive
  //
  postPodcast(cast_id);
  postProgressChanged(3);

  //
  // Set default cast parameters
  //
  RDCart *cart=new RDCart(RDCut::cartNumber(cutname));
  cast->setItemTitle(cart->title());
  cast->setItemImageId(defaultItemImageId());
  delete cart;
  delete cut;
  delete cast;
  emit postProgressChanged(4);

  //
  // Update posted XML
  //
  postXml();
  emit postProgressChanged(5);
  *err=RDFeed::ErrorOk;

  return cast_id;
}


unsigned RDFeed::postFile(const QString &srcfile,Error *err)
{
  QString err_msg;
  QString cmd;
  QString tmpfile;
  QString tmpfile2;
  QString destfile;
  int time_length=0;
  RDWaveFile *wave=NULL;
  RDWaveData wavedata;
  unsigned audio_time=0;

  emit postProgressRangeChanged(0,6);
  emit postProgressChanged(0);

  //
  // Convert Cut
  //
  tmpfile=GetTempFilename();
  RDAudioConvert *conv=new RDAudioConvert(this);
  conv->setSourceFile(srcfile);
  conv->setDestinationFile(tmpfile);
  RDSettings *settings=new RDSettings();
  settings->setFormat(uploadFormat());
  settings->setChannels(uploadChannels());
  settings->setSampleRate(uploadSampleRate());
  settings->setBitRate(uploadBitRate());
  settings->setNormalizationLevel(normalizeLevel()/100);
  conv->setDestinationSettings(settings);
  emit postProgressChanged(1);

  switch(conv->convert()) {
  case RDAudioConvert::ErrorOk:
    wave=new RDWaveFile(srcfile);
    if(wave->openWave(&wavedata)) {
      audio_time=wave->getExtTimeLength();
    }
    delete wave;
    break;

  case RDAudioConvert::ErrorInvalidSettings:
  case RDAudioConvert::ErrorFormatNotSupported:
    delete settings;
    delete conv;
    *err=RDFeed::ErrorUnsupportedType;
    unlink(tmpfile);
    emit postProgressChanged(6);
    return 0;

  case RDAudioConvert::ErrorNoSource:
  case RDAudioConvert::ErrorNoDestination:
  case RDAudioConvert::ErrorInternal:
  case RDAudioConvert::ErrorInvalidSource:
  case RDAudioConvert::ErrorNoDisc:
  case RDAudioConvert::ErrorNoTrack:
  case RDAudioConvert::ErrorInvalidSpeed:
  case RDAudioConvert::ErrorFormatError:
  case RDAudioConvert::ErrorNoSpace:
    delete settings;
    delete conv;
    *err=RDFeed::ErrorGeneral;
    unlink(tmpfile);
    emit postProgressChanged(6);
    return 0;
  }
  delete settings;
  delete conv;
  emit postProgressChanged(2);

  //
  // Save to Audio Store
  //
  QFile file(tmpfile);
  int length=file.size();
  unsigned cast_id=CreateCast(&destfile,length,time_length);
  RDPodcast *cast=new RDPodcast(feed_config,cast_id);
  SavePodcast(cast_id,tmpfile);
  unlink(QString(tmpfile)+".wav");
  unlink(tmpfile);
  emit postProgressChanged(3);

  //
  // Upload to remote archive
  //
  postPodcast(cast_id);
  postProgressChanged(4);

  //
  // Set default cast parameters
  //
  if(wavedata.metadataFound()&&(!wavedata.title().isEmpty())) {
    cast->setItemTitle(wavedata.title());
  }
  else {
    cast->setItemTitle(srcfile.split("/").last());
  }
  cast->setAudioTime(audio_time);
  cast->setItemImageId(defaultItemImageId());
  delete cast;
  emit postProgressChanged(5);

  //
  //
  // Update posted XML
  //
  postXml();
  emit postProgressChanged(6);
  *err=RDFeed::ErrorOk;

  return cast_id;
}


unsigned RDFeed::postLog(const QString &logname,const QTime &start_time,
			 bool stop_at_stop,int start_line,int end_line,
			 RDFeed::Error *err)
{
  QString tmpfile;
  QString destfile;
  QString err_msg;
  RDRenderer *renderer=NULL;
  RDSettings *settings=NULL;
  RDLogEvent *log_event=NULL;

  feed_render_start_line=start_line;
  feed_render_end_line=end_line;

  emit postProgressRangeChanged(0,4+(end_line-start_line));
  emit postProgressChanged(0);

  //
  // Open Log
  //
  log_event=new RDLogEvent(logname);
  log_event->load();
  if(!log_event->exists()) {
    *err=RDFeed::ErrorNoLog;
    delete log_event;
    return 0;
  }

  //
  // Render Log
  //
  tmpfile=GetTempFilename();

  settings=new RDSettings();
  settings->setFormat(uploadFormat());
  settings->setChannels(uploadChannels());
  settings->setSampleRate(uploadSampleRate());
  settings->setBitRate(uploadBitRate());
  settings->setNormalizationLevel(normalizeLevel()/100);
  renderer=new RDRenderer(this);
  connect(renderer,SIGNAL(progressMessageSent(const QString &)),
	  this,SLOT(renderMessage(const QString &)));
  connect(renderer,SIGNAL(lineStarted(int,int)),
	  this,SLOT(renderLineStartedData(int,int)));

  if(!renderer->renderToFile(tmpfile,log_event,settings,start_time,stop_at_stop,
			     &err_msg,start_line,end_line)) {
    *err=RDFeed::ErrorRenderError;
    delete renderer;
    delete settings;
    delete log_event;
    unlink(tmpfile);
    return 0;
  }
  delete renderer;
  emit postProgressChanged(1+(end_line-start_line));

  //
  // Save to Audio Store
  //
  QFile f(tmpfile);
  unsigned cast_id=
    CreateCast(&destfile,f.size(),log_event->length(0,log_event->size()));
  RDPodcast *cast=new RDPodcast(feed_config,cast_id);
  SavePodcast(cast_id,tmpfile);
  unlink(tmpfile);
  emit postProgressChanged(2+(end_line-start_line));

  //
  // Save to remote archive
  //
  postPodcast(cast_id);
  emit postProgressChanged(3+(end_line-start_line));

  //
  // Set default cast parameters
  //
  RDLog *log=new RDLog(logname);
  if(log->description().isEmpty()) {
    cast->setItemTitle(logname+" "+tr("log"));
  }
  else {
    cast->setItemTitle(log->description());
  }
  cast->setItemImageId(defaultItemImageId());
  cast->setAudioTime(log_event->length(start_line,1+end_line));
  delete log;

  postXml();
  emit postProgressChanged(4+(end_line-start_line));
  *err=RDFeed::ErrorOk;

  delete cast;
  delete settings;
  delete log_event;
  unlink(tmpfile);

  return cast_id;
}


QString RDFeed::rssXml(QString *err_msg,const QDateTime &now,bool *ok)
{
  QString ret;

  QString sql;
  RDSqlQuery *chan_q;
  RDSqlQuery *item_q;
  RDSqlQuery *q;

  if(ok!=NULL) {
    *ok=false;
  }
  sql=QString("select ")+
    "FEEDS.CHANNEL_TITLE,"+        // 00
    "FEEDS.CHANNEL_DESCRIPTION,"+  // 01
    "FEEDS.CHANNEL_CATEGORY,"+     // 02
    "FEEDS.CHANNEL_SUB_CATEGORY,"+ // 03
    "FEEDS.CHANNEL_LINK,"+         // 04
    "FEEDS.CHANNEL_COPYRIGHT,"+    // 05
    "FEEDS.CHANNEL_EDITOR,"+       // 06
    "FEEDS.CHANNEL_AUTHOR,"+       // 07
    "FEEDS.CHANNEL_OWNER_NAME,"+   // 08
    "FEEDS.CHANNEL_OWNER_EMAIL,"+  // 09
    "FEEDS.CHANNEL_WEBMASTER,"+    // 10
    "FEEDS.CHANNEL_LANGUAGE,"+     // 11
    "FEEDS.CHANNEL_EXPLICIT,"+     // 12
    "FEEDS.ORIGIN_DATETIME,"+      // 13
    "FEEDS.HEADER_XML,"+           // 14
    "FEEDS.CHANNEL_XML,"+          // 15
    "FEEDS.ITEM_XML,"+             // 16
    "FEEDS.BASE_URL,"+             // 17
    "FEEDS.ID,"+                   // 18
    "FEEDS.UPLOAD_EXTENSION,"+     // 19
    "FEEDS.CAST_ORDER,"+           // 20
    "FEEDS.BASE_PREAMBLE,"+        // 21
    "FEEDS.IS_SUPERFEED,"+         // 22
    "FEED_IMAGES.ID,"+             // 23
    "FEED_IMAGES.WIDTH,"+          // 24
    "FEED_IMAGES.HEIGHT,"+         // 25
    "FEED_IMAGES.DESCRIPTION,"+    // 26
    "FEED_IMAGES.FILE_EXTENSION "+ // 27
    "from FEEDS ";
  sql+="left join FEED_IMAGES ";
  sql+="on FEEDS.CHANNEL_IMAGE_ID=FEED_IMAGES.ID ";
  sql+="where ";
  sql+="FEEDS.KEY_NAME=\""+RDEscapeString(keyName())+"\"";
  chan_q=new RDSqlQuery(sql);
  if(!chan_q->first()) {
    *err_msg="no feed matches the supplied key name";
    return QString();
  }

  //
  // Load the XML Templates
  //
  QString header_template=rda->rssSchemas()->headerTemplate(rssSchema());
  QString channel_template=rda->rssSchemas()->channelTemplate(rssSchema());
  QString item_template=rda->rssSchemas()->itemTemplate(rssSchema());
  if(rssSchema()==RDRssSchemas::CustomSchema) {
    header_template=chan_q->value(14).toString();
    channel_template=chan_q->value(15).toString();
    item_template=chan_q->value(16).toString();
  }

  //
  // Render Header XML
  //
  ret+=header_template+"\r\n";

  //
  // Render Channel XML
  //
  ret+="  <channel>\n";
  ret+=ResolveChannelWildcards(channel_template,chan_q,now)+"\r\n";

  //
  // Render Item XML
  //
  QString where;
  if(chan_q->value(22).toString()=="Y") {  // Is a Superfeed
    sql=QString("select ")+
      "MEMBER_FEED_ID "+  // 00
      "from SUPERFEED_MAPS where "+
      QString().sprintf("FEED_ID=%d",chan_q->value(18).toUInt());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      where+=QString().sprintf("(PODCASTS.FEED_ID=%u) || ",q->value(0).toUInt());
    }
    delete q;
    where=("("+where.left(where.length()-4)+") && ");
  }
  else {
    where=QString().sprintf("(PODCASTS.FEED_ID=%u)&&",chan_q->value(18).toUInt());
  }
  sql=QString("select ")+
    "PODCASTS.FEED_ID,"+             // 00
    "PODCASTS.ITEM_TITLE,"+          // 01
    "PODCASTS.ITEM_DESCRIPTION,"+    // 02
    "PODCASTS.ITEM_CATEGORY,"+       // 03
    "PODCASTS.ITEM_LINK,"+           // 04
    "PODCASTS.ITEM_AUTHOR,"+         // 05
    "PODCASTS.ITEM_SOURCE_TEXT,"+    // 06
    "PODCASTS.ITEM_SOURCE_URL,"+     // 07
    "PODCASTS.ITEM_COMMENTS,"+       // 08
    "PODCASTS.ITEM_EXPLICIT,"+       // 09
    "PODCASTS.AUDIO_FILENAME,"+      // 10
    "PODCASTS.AUDIO_LENGTH,"+        // 11
    "PODCASTS.AUDIO_TIME,"+          // 12
    "PODCASTS.EFFECTIVE_DATETIME,"+  // 13
    "PODCASTS.ID,"+                  // 14
    "FEEDS.BASE_URL,"+               // 15
    "FEEDS.CHANNEL_TITLE,"+          // 16
    "FEEDS.CHANNEL_DESCRIPTION,"+    // 17
    "FEED_IMAGES.ID,"+               // 18
    "FEED_IMAGES.WIDTH,"+            // 19
    "FEED_IMAGES.HEIGHT,"+           // 20
    "FEED_IMAGES.DESCRIPTION,"+      // 21
    "FEED_IMAGES.FILE_EXTENSION "+   // 22
    "from PODCASTS left join FEEDS "+
    "on PODCASTS.FEED_ID=FEEDS.ID "+
    "left join FEED_IMAGES "+
    "on PODCASTS.ITEM_IMAGE_ID=FEED_IMAGES.ID where "+
    where+
    QString().sprintf("(PODCASTS.STATUS=%d) && ",RDPodcast::StatusActive)+
    "(PODCASTS.EFFECTIVE_DATETIME<=now()) && "+
    "((PODCASTS.EXPIRATION_DATETIME is null)||"+
    "(PODCASTS.EXPIRATION_DATETIME>now())) "+
    "order by PODCASTS.ORIGIN_DATETIME";
  if(chan_q->value(20).toString()=="N") {
    sql+=" desc";
  }
  //  printf("item_sql: %s\n",sql.toUtf8().constData());
  item_q=new RDSqlQuery(sql);
  while(item_q->next()) {
    ret+="    <item>\r\n";
    ret+=ResolveItemWildcards(item_template,item_q,chan_q);
    ret+="\r\n";
    ret+="    </item>\r\n";
  }
  delete item_q;

  ret+="  </channel>\r\n";
  ret+="</rss>\r\n";
  delete chan_q;

  if(ok!=NULL) {
    *ok=true;
  }

  return ret;
}


unsigned RDFeed::create(const QString &keyname,bool enable_users,
			QString *err_msg)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  unsigned feed_id=0;

  //
  // Sanity Checks
  //
  sql=QString("select KEY_NAME from FEEDS where ")+
    "KEY_NAME=\""+RDEscapeString(keyname)+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *err_msg=tr("A feed with that key name already exists!");
    delete q;
    return 0;
  }
  delete q;

  //
  // Create Feed
  //
  sql=QString("insert into FEEDS set ")+
    "KEY_NAME=\""+RDEscapeString(keyname)+"\","+
    "ORIGIN_DATETIME=now(),"+
    "HEADER_XML=\"\","+
    "CHANNEL_XML=\"\","+
    "ITEM_XML=\"\"";
  q=new RDSqlQuery(sql);
  feed_id=q->lastInsertId().toUInt();
  delete q;

  //
  // Create Default Feed Perms
  //
  if(enable_users) {
    sql=QString("select LOGIN_NAME from USERS where ")+
      "(ADMIN_RSS_PRIV='N')&&(ADMIN_CONFIG_PRIV='N')";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into FEED_PERMS set ")+
	"USER_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"KEY_NAME=\""+RDEscapeString(keyname)+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  return feed_id;
}


QString RDFeed::errorString(RDFeed::Error err)
{
  QString ret=QString().sprintf("Unknown RDFeed Error [%d]",err);

  switch(err) {
  case RDFeed::ErrorOk:
    ret="Ok";
    break;

  case RDFeed::ErrorNoFile:
    ret="No such file or directory";
    break;

  case RDFeed::ErrorCannotOpenFile:
    ret="Cannot open file";
    break;

  case RDFeed::ErrorUnsupportedType:
    ret="Unsupported file format";
    break;

  case RDFeed::ErrorUploadFailed:
    ret="Upload failed";
    break;

  case RDFeed::ErrorGeneral:
    ret="General Error";
    break;

  case RDFeed::ErrorNoLog:
    ret="No such log";
    break;

  case RDFeed::ErrorRenderError:
    ret="Log rendering error";
    break;
  }
  return ret;
}


QString RDFeed::imageFilename(int feed_id,int img_id,const QString &ext)
{
  return QString().sprintf("img%06d_%06d.",feed_id,img_id)+ext;
}


QString RDFeed::publicUrl(const QString &base_url,const QString &keyname)
{
  return base_url+"/"+keyname+"."+RD_RSS_XML_FILE_EXTENSION;
}


QString RDFeed::itunesCategoryXml(const QString &category,
				  const QString &sub_category,int padding)
{
  QString pad_str="";

  for(int i=0;i<padding;i++) {
    pad_str+=" ";
  }
  if(category.isEmpty()) {
    return QString("");
  }
  if(sub_category.isEmpty()) {
    return QString("<itunes:category text=\"")+RDXmlEscape(category)+"\" />";
  }
  return QString("<itunes:category text=\"")+RDXmlEscape(category)+"\">\n"+
    pad_str+"  <itunes:category text=\""+RDXmlEscape(sub_category)+"\" />\n"+
    pad_str+"</itunes:category>";
}


void RDFeed::renderMessage(const QString &msg)
{
  fprintf(stderr,"RENDERER: %s\n",msg.toUtf8().constData());
}


void RDFeed::renderLineStartedData(int lineno,int total_lines)
{
  if((lineno>=feed_render_start_line)&&(lineno<=feed_render_end_line)) {
    emit postProgressChanged(1+(lineno-feed_render_start_line));
  }
}


bool RDFeed::SavePodcast(unsigned cast_id,const QString &src_filename) const
{
  long response_code;
  CURL *curl=NULL;
  CURLcode curl_err;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;

  //
  // Generate POST Data
  //
  // We have to use multipart here because we have a file to send.
  //
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"COMMAND",
	       CURLFORM_COPYCONTENTS,
	     (const char *)QString().sprintf("%u",RDXPORT_COMMAND_SAVE_PODCAST),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,rda->user()->name().toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,
	       rda->user()->password().toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ID",
	       CURLFORM_COPYCONTENTS,
	       (const char *)QString().sprintf("%u",cast_id),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"FILENAME",
	       CURLFORM_FILE,src_filename.toUtf8().constData(),
	       CURLFORM_END);

  //
  // Set up the transfer
  //
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return false;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,stdout);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent());
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
    return false;
  }

  return true;
}


unsigned RDFeed::CreateCast(QString *filename,int bytes,int msecs) const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  unsigned cast_id=0;

  sql=QString("select ")+
    "CHANNEL_TITLE,"+              // 00
    "CHANNEL_DESCRIPTION,"+        // 01
    "CHANNEL_CATEGORY,"+           // 02
    "CHANNEL_LINK,"+               // 03
    "MAX_SHELF_LIFE,"+             // 04
    "UPLOAD_FORMAT,"+              // 05
    "UPLOAD_EXTENSION,"+           // 06
    "ENABLE_AUTOPOST,"+            // 07
    "CHANNEL_AUTHOR,"+             // 08
    "CHANNEL_AUTHOR_IS_DEFAULT "+  // 09
    "from FEEDS where "+
    QString().sprintf("ID=%u",feed_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return 0;
  }
  QString item_author=rda->user()->emailContact();
  if(q->value(9).toString()=="Y") {
    item_author=q->value(8).toString();
  }

  //
  // Create Entry
  //
  sql=QString("insert into PODCASTS set ")+
    QString().sprintf("FEED_ID=%u,",feed_id)+
    "ITEM_TITLE=\""+RDEscapeString(q->value(0).toString())+"\","+
    "ITEM_DESCRIPTION=\""+RDEscapeString(q->value(1).toString())+"\","+
    "ITEM_CATEGORY=\""+RDEscapeString(q->value(2).toString())+"\","+
    "ITEM_LINK=\""+RDEscapeString(q->value(3).toString())+"\","+
    "ITEM_AUTHOR=\""+RDEscapeString(item_author)+"\","+
    "EFFECTIVE_DATETIME=now(),"+
    "ORIGIN_LOGIN_NAME=\""+RDEscapeString(rda->user()->name())+"\","+
    "ORIGIN_STATION=\""+RDEscapeString(rda->station()->name())+"\","+
    "ORIGIN_DATETIME=now(),";
  if(RDBool(q->value(7).toString())) {
    sql+=QString().sprintf("STATUS=%d,",RDPodcast::StatusActive);
  }
  else {
    sql+=QString().sprintf("STATUS=%d,",RDPodcast::StatusPending);
  }
  if(q->value(4).toInt()==0) {
    sql+="EXPIRATION_DATETIME=NULL";
  }
  else {
    sql+="EXPIRATION_DATETIME=\""+
      QDateTime::currentDateTime().addDays(q->value(4).toInt()).
      toString("yyyy-MM-dd hh:mm:ss")+"\"";
  }
  q1=new RDSqlQuery(sql);
  delete q1;

  //
  // Get The Cast ID
  //
  sql="select LAST_INSERT_ID() from PODCASTS";
  q1=new RDSqlQuery(sql);
  if(q1->first()) {
    cast_id=q1->value(0).toUInt();
  }
  delete q1;

  //
  // Generate the Filename
  //
  *filename=
    QString().sprintf("%06u_%06u",feed_id,cast_id)+"."+q->value(6).toString();
  sql=QString("update PODCASTS set ")+
    "AUDIO_FILENAME=\""+RDEscapeString(*filename)+"\","+
    QString().sprintf("AUDIO_LENGTH=%d,",bytes)+
    QString().sprintf("AUDIO_TIME=%d where ",msecs)+
    QString().sprintf("ID=%u",cast_id);
  q1=new RDSqlQuery(sql);
  delete q1;
  delete q;
  return cast_id;
}


QString RDFeed::ResolveChannelWildcards(const QString &tmplt,RDSqlQuery *chan_q,
					const QDateTime &build_datetime)
{
  QString ret="    "+tmplt;

  ret.replace("\n","\r\n    ");
  ret.replace("%TITLE%",RDXmlEscape(chan_q->value(0).toString()));
  ret.replace("%DESCRIPTION%",RDXmlEscape(chan_q->value(1).toString()));
  ret.replace("%CATEGORY%",RDXmlEscape(chan_q->value(2).toString()));
  ret.replace("%SUB_CATEGORY%",RDXmlEscape(chan_q->value(3).toString()));
  ret.replace("%ITUNES_CATEGORY%",
	      RDFeed::itunesCategoryXml(chan_q->value(2).toString(),
					chan_q->value(3).toString(),4));
  ret.replace("%LINK%",RDXmlEscape(chan_q->value(4).toString()));
  ret.replace("%COPYRIGHT%",RDXmlEscape(chan_q->value(5).toString()));
  ret.replace("%EDITOR%",RDXmlEscape(chan_q->value(6).toString()));
  ret.replace("%AUTHOR%",RDXmlEscape(chan_q->value(7).toString()));
  ret.replace("%OWNER_NAME%",RDXmlEscape(chan_q->value(8).toString()));
  ret.replace("%OWNER_EMAIL%",RDXmlEscape(chan_q->value(9).toString()));
  ret.replace("%WEBMASTER%",RDXmlEscape(chan_q->value(10).toString()));
  ret.replace("%LANGUAGE%",RDXmlEscape(chan_q->value(11).toString()));
  QString explicit_str="false";
  if(chan_q->value(12).toString()=="Y") {
    explicit_str="true";
  }
  ret.replace("%EXPLICIT%",RDXmlEscape(explicit_str));
  ret.replace("%BUILD_DATE%",RDLocalToUtc(build_datetime).
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%PUBLISH_DATE%",RDLocalToUtc(chan_q->value(13).toDateTime()).
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%GENERATOR%",QString("Rivendell ")+VERSION);
  ret.replace("%FEED_URL%",RDXmlEscape(chan_q->value(17).toString())+"/"+
	      RDXmlEscape(keyName()+"."+RD_RSS_XML_FILE_EXTENSION));
  ret.replace("%IMAGE_URL%",chan_q->value(17).toString()+"/"+
	      RDFeed::imageFilename(id(),chan_q->value(23).toInt(),
				    chan_q->value(27).toString()));
  ret.replace("%IMAGE_WIDTH%",
	      QString().sprintf("%d",chan_q->value(24).toInt()));
  ret.replace("%IMAGE_HEIGHT%",
	      QString().sprintf("%d",chan_q->value(24).toInt()));
  ret.replace("%IMAGE_DESCRIPTION%",chan_q->value(26).toString());

  return ret;
}


QString RDFeed::ResolveItemWildcards(const QString &tmplt,RDSqlQuery *item_q,
				     RDSqlQuery *chan_q)
{
  QString ret="      "+tmplt;

  ret.replace("\n","\r\n      ");

  ret.replace("%ITEM_CHANNEL_TITLE%",RDXmlEscape(item_q->value(16).toString()));
  ret.replace("%ITEM_CHANNEL_DESCRIPTION%",
	      RDXmlEscape(item_q->value(17).toString()));
  ret.replace("%ITEM_TITLE%",RDXmlEscape(item_q->value(1).toString()));
  ret.replace("%ITEM_DESCRIPTION%",
	      RDXmlEscape(item_q->value(2).toString()));
  ret.replace("%ITEM_CATEGORY%",
	      RDXmlEscape(item_q->value(3).toString()));
  ret.replace("%ITEM_LINK%",RDXmlEscape(item_q->value(4).toString()));
  ret.replace("%ITEM_AUTHOR%",RDXmlEscape(item_q->value(5).toString()));
  ret.replace("%ITEM_SOURCE_TEXT%",
	      RDXmlEscape(chan_q->value(0).toString()));
  ret.replace("%ITEM_SOURCE_URL%",
	      RDXmlEscape(item_q->value(15).toString()+"/"+keyName()));    
  ret.replace("%ITEM_COMMENTS%",
	      RDXmlEscape(item_q->value(8).toString()));
  QString explicit_str="false";
  if(item_q->value(9).toString()=="Y") {
    explicit_str="true";
  }
  ret.replace("%ITEM_EXPLICIT%",explicit_str);
  ret.replace("%ITEM_AUDIO_URL%",
	      RDXmlEscape(audioUrl(item_q->value(14).toUInt())));
  ret.replace("%ITEM_AUDIO_LENGTH%",item_q->value(11).toString());
  ret.replace("%ITEM_AUDIO_TIME%",
	      RDGetTimeLength(item_q->value(12).toInt(),false,false));
  ret.replace("%ITEM_AUDIO_SECONDS%",
	      QString().sprintf("%d",item_q->value(12).toInt()/1000));
  ret.replace("%ITEM_PUBLISH_DATE%",
	      RDLocalToUtc(item_q->value(13).toDateTime()).
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%ITEM_GUID%",RDPodcast::guid(item_q->value(15).toString(),
					    item_q->value(10).toString(),
					    item_q->value(0).toUInt(),
					    item_q->value(14).toUInt()));
  ret.replace("%ITEM_IMAGE_URL%",item_q->value(15).toString()+"/"+
	      RDFeed::imageFilename(item_q->value(0).toInt(),
				    item_q->value(18).toInt(),
				    item_q->value(22).toString()));
  return ret;
}


QString RDFeed::GetTempFilename() const
{
  char tempname[PATH_MAX];

  sprintf(tempname,"%s/podcastXXXXXX",(const char *)RDTempDirectory::basePath());
  if(mkstemp(tempname)<0) {
    return QString();
  }

  return QString(tempname);
}


void RDFeed::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update FEEDS set ")+
    param+QString().sprintf("=%d where ",value)+
    "KEY_NAME=\""+RDEscapeString(feed_keyname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDFeed::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update FEEDS set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    "KEY_NAME=\""+RDEscapeString(feed_keyname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}

void RDFeed::SetRow(const QString &param,const QDateTime &value,
                    const QString &format) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update FEEDS set ")+
    param+"="+RDCheckDateTime(value,format)+" where "+
    "KEY_NAME=\""+RDEscapeString(feed_keyname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}
