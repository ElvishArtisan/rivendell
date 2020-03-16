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
#include "rdpodcast.h"
#include "rdtempdirectory.h"
#include "rdupload.h"
#include "rdwavefile.h"

//
// Default XML Templates
//
#define DEFAULT_HEADER_XML "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"

#define DEFAULT_CHANNEL_XML "<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<managingEditor>%EDITOR%</managingEditor>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>\n<atom:link href=\"%FEED_URL%\" rel=\"self\" type=\"application/rss+xml\" />"

#define DEFAULT_ITEM_XML "<title>%ITEM_TITLE%</title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<author>%ITEM_AUTHOR%</author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\"  type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>"

size_t __RDFeed_Readfunction_Callback(char *buffer,size_t size,size_t nitems,
				      void *userdata)
{
  RDFeed *feed=(RDFeed *)userdata;

  int curlsize=size*nitems;
  int segsize=feed->feed_xml.size()-feed->feed_xml_ptr;
  if(segsize<curlsize) {
    curlsize=segsize;
  }
  memcpy(buffer,feed->feed_xml.mid(feed->feed_xml_ptr,curlsize).constData(),
	 curlsize);
  feed->feed_xml_ptr+=curlsize;
  return curlsize;
}

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


bool RDFeed::audienceMetrics() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "AUDIENCE_METRICS").toString());
}


void RDFeed::setAudienceMetrics(bool state)
{
  SetRow("AUDIENCE_METRICS",RDYesNo(state));
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


QString RDFeed::channelLanguage() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_LANGUAGE").
    toString();
}


void RDFeed::setChannelLanguage(const QString &str)
{
  SetRow("CHANNEL_LANGUAGE",str);
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
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"PURGE_PASSWORD").
    toString();
}


void RDFeed::setPurgePassword(const QString &str) const
{
  SetRow("PURGE_PASSWORD",str);
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


bool RDFeed::keepMetadata() const
{
  return RDBool(RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,
			      "KEEP_METADATA").toString());
}


void RDFeed::setKeepMetadata(bool state)
{
  SetRow("KEEP_METADATA",RDYesNo(state));
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


QString RDFeed::redirectPath() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"REDIRECT_PATH").
    toString();
}


void RDFeed::setRedirectPath(const QString &str)
{
  SetRow("REDIRECT_PATH",str);
}


RDFeed::MediaLinkMode RDFeed::mediaLinkMode() const
{
  return (RDFeed::MediaLinkMode)RDGetSqlValue("FEEDS","KEY_NAME",
					      feed_keyname,"MEDIA_LINK_MODE").
    toUInt();
}
  

void RDFeed::setMediaLinkMode(RDFeed::MediaLinkMode mode) const
{
  SetRow("MEDIA_LINK_MODE",(unsigned)mode);
}


QString RDFeed::audioUrl(RDFeed::MediaLinkMode mode,
			 const QString &cgi_hostname,unsigned cast_id)
{
  RDPodcast *cast=new RDPodcast(feed_config,cast_id);
  QUrl url(baseUrl(cast->feedId()));
  QString ret;

  switch(mode) {
  case RDFeed::LinkNone:
    ret="";
    break;

  case RDFeed::LinkDirect:
    cast=new RDPodcast(feed_config,cast_id);
    ret=url.toString()+"/"+cast->audioFilename();
    break;

  case RDFeed::LinkCounted:
    ret=QString("http://")+basePreamble()+cgi_hostname+
      "/rd-bin/rdfeed."+uploadExtension()+"?"+keyName()+
      QString().sprintf("&cast_id=%d",cast_id);
    break;
  }
  delete cast;

  return ret;
}


bool RDFeed::postXml(QString *err_msg)
{
  CURL *curl=NULL;
  CURLcode curl_err;
  bool ret=false;
  char errstr[CURL_ERROR_SIZE];

  if((curl=curl_easy_init())==NULL) {
    *err_msg=tr("Unable to get CURL handle.");
    return false;
  }

  feed_xml=rssXml(err_msg).toUtf8();
  feed_xml_ptr=0;

  curl_easy_setopt(curl,CURLOPT_URL,feedUrl().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_UPLOAD,1);
  curl_easy_setopt(curl,CURLOPT_READFUNCTION, __RDFeed_Readfunction_Callback);
  curl_easy_setopt(curl,CURLOPT_READDATA,this);
  curl_easy_setopt(curl,CURLOPT_USERNAME,purgeUsername().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_PASSWORD,purgePassword().toUtf8().constData());

  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent().utf8());
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errstr);
  /*
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,UploadErrorCallback);
  */
  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
  case CURLE_PARTIAL_FILE:
    ret=true;
    break;

  default:
    *err_msg=errstr;
    ret=false;
    break;
  }
  curl_easy_cleanup(curl);

  //
  // Update Enclosing Superfeeds
  //
  QStringList superfeeds=isSubfeedOf();
  for(int i=0;i<superfeeds.size();i++) {
    QString err_msg2;
    RDFeed *feed=new RDFeed(superfeeds.at(i),feed_config,this);
    if(!feed->postXml(&err_msg2)) {
      *err_msg+="\n"+err_msg2;
    }
    delete feed;
  }

  return ret;
}


bool RDFeed::postXmlConditional(const QString &caption,QWidget *widget)
{
  QString err_msg;

  if(!audienceMetrics()) {
    if(!postXml(&err_msg)) {
      QMessageBox::warning(widget,caption+" - "+tr("Error"),
			   tr("XML data upload failed!")+"\n"+
			   "["+err_msg+"]");
      return false;
    }
  }

  return true;
}


bool RDFeed::deleteXml(QString *err_msg)
{
  RDDelete::ErrorCode conv_err;
  RDDelete *conv=new RDDelete(rda->config());
  if(!conv->urlIsSupported(feedUrl())) {
    *err_msg="unsupported url scheme";
    delete conv;
    return false;
  }
  conv->setTargetUrl(feedUrl());
  conv_err=conv->runDelete(purgeUsername(),purgePassword(),
			   rda->config()->logXloadDebugData());
  *err_msg=RDDelete::errorText(conv_err);
  delete conv;

  return conv_err==RDDelete::ErrorOk;
}


unsigned RDFeed::postCut(RDUser *user,RDStation *station,
			 const QString &cutname,Error *err,bool log_debug,
			 RDConfig *config)
{
  QString err_msg;
  QString tmpfile;
  QString destfile;
  QString sql;
  RDSqlQuery *q;
  RDPodcast *cast=NULL;
  RDUpload *upload=NULL;
  RDUpload::ErrorCode upload_err;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDAudioExport::ErrorCode export_err;

  emit postProgressChanged(0);
  emit postProgressChanged(1);

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
  switch((export_err=conv->runExport(user->name(),user->password(),&audio_conv_err))) {
  case RDAudioExport::ErrorOk:
    break;

  case RDAudioExport::ErrorInvalidSettings:
    delete settings;
    delete conv;
    *err=RDFeed::ErrorUnsupportedType;
    unlink(tmpfile);
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
    return 0;
  }
  delete settings;
  delete conv;

  //
  // Upload
  //
  emit postProgressChanged(2);
  QFile file(tmpfile);
  int length=file.size();
  unsigned cast_id=CreateCast(&destfile,length,cut->length());
  delete cut;
  cast=new RDPodcast(feed_config,cast_id);
  upload=new RDUpload(rda->config(),this);
  upload->setSourceFile(tmpfile);
  upload->setDestinationUrl(purgeUrl()+"/"+cast->audioFilename());
  switch((upload_err=upload->runUpload(purgeUsername(),purgePassword(),
				       log_debug))) {
  case RDUpload::ErrorOk:
    *err=RDFeed::ErrorOk;
    break;

  default:
    emit postProgressChanged(totalPostSteps());
    *err=RDFeed::ErrorUploadFailed;
    sql=QString().sprintf("delete from PODCASTS where ID=%u",cast_id);
    q=new RDSqlQuery(sql);
    delete q;
    delete upload;
    delete cast;
    *err=RDFeed::ErrorUploadFailed;
    unlink(tmpfile);
    return 0;
  }
  emit postProgressChanged(3);
  unlink(tmpfile);
  delete upload;
  delete cast;

  if(!audienceMetrics()) {
    emit postProgressChanged(4);
    postXml(&err_msg);
  }

  emit postProgressChanged(totalPostSteps());

  return cast_id;
}


unsigned RDFeed::postFile(RDStation *station,const QString &srcfile,Error *err,
			  bool log_debug,RDConfig *config)
{
  QString err_msg;
  QString sql;
  RDSqlQuery *q;
  QString cmd;
  QString tmpfile;
  QString tmpfile2;
  QString destfile;
  int time_length=0;
  RDUpload *upload=NULL;
  RDUpload::ErrorCode upload_err;
  RDWaveFile *wave=NULL;
  unsigned audio_time=0;

  emit postProgressChanged(0);
  emit postProgressChanged(1);
  qApp->processEvents();

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
  switch(conv->convert()) {
  case RDAudioConvert::ErrorOk:
    wave=new RDWaveFile(tmpfile);
    if(wave->openWave()) {
      audio_time=wave->getExtTimeLength();
    }
    delete wave;
    break;

  case RDAudioConvert::ErrorInvalidSettings:
  case RDAudioConvert::ErrorFormatNotSupported:
    emit postProgressChanged(totalPostSteps());
    delete settings;
    delete conv;
    *err=RDFeed::ErrorUnsupportedType;
    unlink(tmpfile);
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
    emit postProgressChanged(totalPostSteps());
    delete settings;
    delete conv;
    *err=RDFeed::ErrorGeneral;
    unlink(tmpfile);
    return 0;
  }
  delete settings;
  delete conv;

  //
  // Upload
  //
  emit postProgressChanged(2);
  emit postProgressChanged(3);
  qApp->processEvents();
  QFile file(tmpfile);
  int length=file.size();

  unsigned cast_id=CreateCast(&destfile,length,time_length);
  RDPodcast *cast=new RDPodcast(feed_config,cast_id);
  upload=new RDUpload(rda->config(),this);
  upload->setSourceFile(tmpfile);
  upload->setDestinationUrl(purgeUrl()+"/"+cast->audioFilename());
  switch((upload_err=upload->runUpload(purgeUsername(),purgePassword(),
				       log_debug))) {
  case RDUpload::ErrorOk:
    sql=QString().sprintf("update PODCASTS set AUDIO_TIME=%u where ID=%u",
			  audio_time,cast_id);
    q=new RDSqlQuery(sql);
    delete q;
    break;

  default:
    emit postProgressChanged(totalPostSteps());
    *err=RDFeed::ErrorUploadFailed;
    sql=QString().sprintf("delete from PODCASTS where ID=%u",cast_id);
    q=new RDSqlQuery(sql);
    delete q;
    delete upload;
    delete cast;
    *err=RDFeed::ErrorUploadFailed;
    unlink(tmpfile);
    return 0;
  }
  delete upload;
  delete cast;
  unlink(QString(tmpfile)+".wav");
  unlink(tmpfile);

  if(!audienceMetrics()) {
    emit postProgressChanged(4);
    postXml(&err_msg);
  }

  emit postProgressChanged(totalPostSteps());

  *err=RDFeed::ErrorOk;
  return cast_id;
}


int RDFeed::totalPostSteps() const
{
  if(audienceMetrics()) {
    return RDFEED_TOTAL_POST_STEPS;
  }
  return RDFEED_TOTAL_POST_STEPS+1;
}


QString RDFeed::rssXml(QString *err_msg,bool *ok)
{
  QString ret;

  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  if(ok!=NULL) {
    *ok=false;
  }
  sql=QString("select ")+
    "CHANNEL_TITLE,"+        // 00
    "CHANNEL_DESCRIPTION,"+  // 01
    "CHANNEL_CATEGORY,"+     // 02
    "CHANNEL_LINK,"+         // 03
    "CHANNEL_COPYRIGHT,"+    // 04

    "CHANNEL_EDITOR,"+       // 05

    "CHANNEL_WEBMASTER,"+    // 06
    "CHANNEL_LANGUAGE,"+     // 07
    "LAST_BUILD_DATETIME,"+  // 08
    "ORIGIN_DATETIME,"+      // 09
    "HEADER_XML,"+           // 10
    "CHANNEL_XML,"+          // 11
    "ITEM_XML,"+             // 12
    "BASE_URL,"+             // 13
    "ID,"+                   // 14
    "UPLOAD_EXTENSION,"+     // 15
    "CAST_ORDER,"+           // 16
    "REDIRECT_PATH,"+        // 17
    "BASE_PREAMBLE,"+        // 18
    "AUDIENCE_METRICS,"+     // 19
    "IS_SUPERFEED "+         // 20
    "from FEEDS where "+
    "KEY_NAME=\""+RDEscapeString(keyName())+"\"";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    *err_msg="no feed matches the supplied key name";
    return QString();
  }

  //
  // Render Header XML
  //
  ret+=q->value(10).toString()+"\r\n";

  //
  // Render Channel XML
  //
  ret+="  <channel>\n";
  ret+=ResolveChannelWildcards(q)+"\r\n";

  //
  // Render Item XML
  //
  QString where;
  if(q->value(20).toString()=="Y") {
    sql=QString("select ")+
      "MEMBER_FEED_ID "+  // 00
      "from SUPERFEED_MAPS where "+
      QString().sprintf("FEED_ID=%d",q->value(14).toUInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      where+=QString().sprintf("(FEED_ID=%u) || ",q1->value(0).toUInt());
    }
    delete q1;
    where=("("+where.left(where.length()-4)+") && ");
  }
  else {
    where =QString().sprintf("(FEED_ID=%u)&&",q->value(14).toUInt());
  }
  sql=QString("select ")+
    "PODCASTS.ITEM_TITLE,"+          // 00
    "PODCASTS.ITEM_DESCRIPTION,"+    // 01
    "PODCASTS.ITEM_CATEGORY,"+       // 02
    "PODCASTS.ITEM_LINK,"+           // 03
    "PODCASTS.ITEM_AUTHOR,"+         // 04
    "PODCASTS.ITEM_SOURCE_TEXT,"+    // 05
    "PODCASTS.ITEM_SOURCE_URL,"+     // 06
    "PODCASTS.ITEM_COMMENTS,"+       // 07
    "PODCASTS.AUDIO_FILENAME,"+      // 08
    "PODCASTS.AUDIO_LENGTH,"+        // 09
    "PODCASTS.AUDIO_TIME,"+          // 10
    "PODCASTS.EFFECTIVE_DATETIME,"+  // 11
    "PODCASTS.ID,"+                  // 12
    "FEEDS.BASE_URL "+               // 13
    "from PODCASTS left join FEEDS "+
    "on PODCASTS.FEED_ID=FEEDS.ID where "+
    where+
    QString().sprintf("(PODCASTS.STATUS=%d) ",RDPodcast::StatusActive)+
    "order by PODCASTS.ORIGIN_DATETIME";
  if(q->value(16).toString()=="N") {
    sql+=" desc";
  }
  q1=new RDSqlQuery(sql);
  while(q1->next()) {
    ret+="    <item>\r\n";
    ret+=ResolveItemWildcards(q1,q);
    ret+="\r\n";
    ret+="    </item>\r\n";
  }
  delete q1;

  ret+="  </channel>\r\n";
  ret+="</rss>\r\n";
  delete q;

  if(ok!=NULL) {
    *ok=true;
  }

  return ret;
}


unsigned RDFeed::create(const QString &keyname,bool enable_users,
			QString *err_msg,const QString &exemplar)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  unsigned feed_id=0;
  bool ok=false;

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

  if(exemplar.isEmpty()) {
    //
    // Create Feed
    //
    sql=QString("insert into FEEDS set ")+
      "KEY_NAME=\""+RDEscapeString(keyname)+"\","+
      "ORIGIN_DATETIME=now(),"+
      "HEADER_XML=\""+RDEscapeString(DEFAULT_HEADER_XML)+"\","+
      "CHANNEL_XML=\""+RDEscapeString(DEFAULT_CHANNEL_XML)+"\","+
      "ITEM_XML=\""+RDEscapeString(DEFAULT_ITEM_XML)+"\"";
    q=new RDSqlQuery(sql);
    feed_id=q->lastInsertId().toUInt();
    delete q;
  }
  else {
    sql=QString("select ")+
      "IS_SUPERFEED,"+         // 00
      "AUDIENCE_METRICS,"+     // 01
      "CHANNEL_TITLE,"+        // 02
      "CHANNEL_DESCRIPTION,"+  // 03
      "CHANNEL_CATEGORY,"+     // 04
      "CHANNEL_LINK,"+         // 05
      "CHANNEL_COPYRIGHT,"+    // 06
      "CHANNEL_WEBMASTER,"+    // 07
      "CHANNEL_LANGUAGE,"+     // 08
      "BASE_URL,"+             // 09
      "BASE_PREAMBLE,"+        // 10
      "PURGE_URL,"+            // 11
      "PURGE_USERNAME,"+       // 12
      "PURGE_PASSWORD,"+       // 13
      "HEADER_XML,"+           // 14
      "CHANNEL_XML,"+          // 15
      "ITEM_XML,"+             // 16
      "CAST_ORDER,"+           // 17
      "MAX_SHELF_LIFE,"+       // 18
      "ENABLE_AUTOPOST,"+      // 19
      "KEEP_METADATA,"+        // 20
      "UPLOAD_FORMAT,"+        // 21
      "UPLOAD_CHANNELS,"+      // 22
      "UPLOAD_SAMPRATE,"+      // 23
      "UPLOAD_BITRATE,"+       // 24
      "UPLOAD_QUALITY,"+       // 25
      "UPLOAD_EXTENSION,"+     // 26
      "NORMALIZE_LEVEL,"+      // 27
      "REDIRECT_PATH,"+        // 28
      "MEDIA_LINK_MODE "+      // 29
      "from FEEDS where "+
      "KEY_NAME=\""+RDEscapeString(exemplar)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into FEEDS set ")+
	"KEY_NAME=\""+RDEscapeString(keyname)+"\","+
	"IS_SUPERFEED=\""+q->value(0).toString()+"\","+
	"AUDIENCE_METRICS=\""+q->value(1).toString()+"\","+
	"CHANNEL_TITLE=\""+RDEscapeString(q->value(2).toString())+"\","+
	"CHANNEL_DESCRIPTION=\""+RDEscapeString(q->value(3).toString())+"\","+
	"CHANNEL_CATEGORY=\""+RDEscapeString(q->value(4).toString())+"\","+
	"CHANNEL_LINK=\""+RDEscapeString(q->value(5).toString())+"\","+
	"CHANNEL_COPYRIGHT=\""+RDEscapeString(q->value(6).toString())+"\","+
	"CHANNEL_WEBMASTER=\""+RDEscapeString(q->value(7).toString())+"\","+
	"CHANNEL_LANGUAGE=\""+RDEscapeString(q->value(8).toString())+"\","+
	"BASE_URL=\""+RDEscapeString(q->value(9).toString())+"\","+
	"BASE_PREAMBLE=\""+RDEscapeString(q->value(10).toString())+"\","+
	"PURGE_URL=\""+RDEscapeString(q->value(11).toString())+"\","+
	"PURGE_USERNAME=\""+RDEscapeString(q->value(12).toString())+"\","+
	"PURGE_PASSWORD=\""+RDEscapeString(q->value(13).toString())+"\","+
	"HEADER_XML=\""+RDEscapeString(q->value(14).toString())+"\","+
	"CHANNEL_XML=\""+RDEscapeString(q->value(15).toString())+"\","+
	"ITEM_XML=\""+RDEscapeString(q->value(16).toString())+"\","+
	"CAST_ORDER=\""+RDEscapeString(q->value(17).toString())+"\","+
	QString().sprintf("MAX_SHELF_LIFE=%d,",q->value(18).toInt())+
	"LAST_BUILD_DATETIME=now(),"+
	"ORIGIN_DATETIME=now(),"+
	"ENABLE_AUTOPOST=\""+RDEscapeString(q->value(19).toString())+"\","+
	"KEEP_METADATA=\""+RDEscapeString(q->value(20).toString())+"\","+
	QString().sprintf("UPLOAD_FORMAT=%d,",q->value(21).toInt())+
	QString().sprintf("UPLOAD_CHANNELS=%d,",q->value(22).toInt())+
	QString().sprintf("UPLOAD_SAMPRATE=%d,",q->value(23).toInt())+
	QString().sprintf("UPLOAD_BITRATE=%d,",q->value(24).toInt())+
	QString().sprintf("UPLOAD_QUALITY=%d,",q->value(25).toInt())+
	"UPLOAD_EXTENSION=\""+RDEscapeString(q->value(26).toString())+"\","+
	QString().sprintf("NORMALIZE_LEVEL=%d,",q->value(27).toInt())+
	"REDIRECT_PATH=\""+RDEscapeString(q->value(28).toString())+"\","+
	QString().sprintf("MEDIA_LINK_MODE=%d",q->value(29).toInt());
      feed_id=RDSqlQuery::run(sql,&ok).toUInt();
      if(!ok) {
	*err_msg=tr("Unable to insert new feed record!");
	delete q;
	return 0;
      }

      //
      // Duplicate member feed references
      //
      if(q->value(0).toString()=="Y") {
	sql=QString("select ")+
	  "MEMBER_KEY_NAME,"+  // 00
	  "FEED_ID,"+          // 01
	  "MEMBER_FEED_ID "+   // 02
	  "from SUPERFEED_MAPS where "+
	  "KEY_NAME=\""+RDEscapeString(exemplar)+"\"";
	q1=new RDSqlQuery(sql);
	while(q1->next()) {
	  sql=QString("insert into SUPERFEED_MAPS set ")+
	    "KEY_NAME=\""+RDEscapeString(keyname)+"\","+
	    "MEMBER_KEY_NAME=\""+RDEscapeString(q1->value(0).toString())+"\","+
	    QString().sprintf("FEED_ID=%u,",q1->value(1).toUInt())+
	    QString().sprintf("MEMBER_FEED_ID=%u",q1->value(2).toUInt());
	  RDSqlQuery::apply(sql);
	}
	delete q1;
      }
    }
    else {
      *err_msg=tr("Exemplar feed")+" \""+exemplar+"\" "+tr("does not exist!");
      delete q;
      return 0;
    }
    delete q;
  }

  //
  // Create Default Feed Perms
  //
  if(enable_users) {
    sql=QString("select LOGIN_NAME from USERS where ")+
      "(ADMIN_USERS_PRIV='N')&&(ADMIN_CONFIG_PRIV='N')";
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
  QString ret="Unknown Error";

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
  }
  return ret;
}


unsigned RDFeed::CreateCast(QString *filename,int bytes,int msecs) const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  unsigned cast_id=0;

  sql=QString("select ")+
    "CHANNEL_TITLE,"+        // 00
    "CHANNEL_DESCRIPTION,"+  // 01
    "CHANNEL_CATEGORY,"+     // 02
    "CHANNEL_LINK,"+         // 03
    "MAX_SHELF_LIFE,"+       // 04
    "UPLOAD_FORMAT,"+        // 05
    "UPLOAD_EXTENSION "+     // 06
    "from FEEDS where "+
    QString().sprintf("ID=%u",feed_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return 0;
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
    QString().sprintf("SHELF_LIFE=%d,",q->value(4).toInt())+
    "ITEM_AUTHOR=\""+RDEscapeString(rda->user()->emailContact())+"\","+
    "EFFECTIVE_DATETIME=UTC_TIMESTAMP(),"+
    "ORIGIN_DATETIME=UTC_TIMESTAMP()";
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


QString RDFeed::ResolveChannelWildcards(RDSqlQuery *chan_q)
{
  QString ret="    "+chan_q->value(11).toString();

  ret.replace("\n","\r\n    ");
  ret.replace("%TITLE%",RDXmlEscape(chan_q->value(0).toString()));
  ret.replace("%DESCRIPTION%",RDXmlEscape(chan_q->value(1).toString()));
  ret.replace("%CATEGORY%",RDXmlEscape(chan_q->value(2).toString()));
  ret.replace("%LINK%",RDXmlEscape(chan_q->value(3).toString()));
  ret.replace("%COPYRIGHT%",RDXmlEscape(chan_q->value(4).toString()));
  ret.replace("%EDITOR%",RDXmlEscape(chan_q->value(5).toString()));
  ret.replace("%WEBMASTER%",RDXmlEscape(chan_q->value(6).toString()));
  ret.replace("%LANGUAGE%",RDXmlEscape(chan_q->value(7).toString()));
  ret.replace("%BUILD_DATE%",chan_q->value(8).toDateTime().
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%PUBLISH_DATE%",chan_q->value(9).toDateTime().
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%GENERATOR%",QString("Rivendell ")+VERSION);
  ret.replace("%FEED_URL%",RDXmlEscape(chan_q->value(13).toString())+"/"+
	      RDXmlEscape(keyName()+"."+RD_RSS_XML_FILE_EXTENSION));

  return ret;
}


QString RDFeed::ResolveItemWildcards(RDSqlQuery *item_q,RDSqlQuery *chan_q)
{
  QString ret="      "+chan_q->value(11).toString();

  ret.replace("\n","\r\n      ");
  ret.replace("%ITEM_TITLE%",RDXmlEscape(item_q->value(0).toString()));
  ret.replace("%ITEM_DESCRIPTION%",
	      RDXmlEscape(item_q->value(1).toString()));
  ret.replace("%ITEM_CATEGORY%",
	      RDXmlEscape(item_q->value(2).toString()));
  ret.replace("%ITEM_LINK%",RDXmlEscape(item_q->value(3).toString()));
  ret.replace("%ITEM_AUTHOR%",RDXmlEscape(item_q->value(4).toString()));
  if(chan_q->value(1).toString()=="Y") {  // Audience Metrics
    ret.replace("%ITEM_SOURCE_TEXT%",
		RDXmlEscape(item_q->value(5).toString()));
    ret.replace("%ITEM_SOURCE_URL%",
		RDXmlEscape(item_q->value(6).toString()));
  }
  else {
    ret.replace("%ITEM_SOURCE_TEXT%",
		RDXmlEscape(chan_q->value(0).toString()));
    ret.replace("%ITEM_SOURCE_URL%",
		RDXmlEscape(item_q->value(13).toString()+"/"+keyName()));    
  }
  ret.replace("%ITEM_COMMENTS%",
	      RDXmlEscape(item_q->value(7).toString()));
  if(chan_q->value(19).toString()=="Y") {
    ret.replace("%ITEM_AUDIO_URL%",
		RDXmlEscape(audioUrl(RDFeed::LinkCounted,feed_cgi_hostname,
				     item_q->value(12).toUInt())));
  }
  else {
    ret.replace("%ITEM_AUDIO_URL%",
		RDXmlEscape(audioUrl(RDFeed::LinkDirect,feed_cgi_hostname,
				     item_q->value(12).toUInt())));
  }
  ret.replace("%ITEM_AUDIO_LENGTH%",item_q->value(9).toString());
  ret.replace("%ITEM_AUDIO_TIME%",
	      RDGetTimeLength(item_q->value(10).toInt(),false,false));
  ret.replace("%ITEM_PUBLISH_DATE%",item_q->value(11).toDateTime().
	      toString("ddd, d MMM yyyy hh:mm:ss ")+"GMT");
  ret.replace("%ITEM_GUID%",RDPodcast::guid(chan_q->value(13).toString(),
					    item_q->value(8).toString(),
					    chan_q->value(12).toUInt(),
					    item_q->value(12).toUInt()));
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
