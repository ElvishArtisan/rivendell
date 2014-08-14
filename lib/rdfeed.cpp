// rdfeed.cpp
//
// Abstract a Rivendell RSS Feed
//
//   (C) Copyright 2002-2007,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdfeed.cpp,v 1.17.2.2 2013/11/13 23:36:33 cvs Exp $
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

#include <qfile.h>
#include <qurl.h>
#include <qapplication.h>

#include <rddb.h>
#include <rdfeed.h>
#include <rdconf.h>
#include <rdlibrary_conf.h>
#include <rdescape_string.h>
#include <rdwavefile.h>
#include <rdpodcast.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdaudioexport.h>
#include <rdaudioconvert.h>
#include <rdupload.h>

RDFeed::RDFeed(const QString &keyname,QObject *parent,const char *name)
  : QObject(parent,name)
{
  RDSqlQuery *q;
  QString sql;

  feed_keyname=keyname;

  sql=QString().sprintf("select ID from FEEDS where KEY_NAME=\"%s\"",
			(const char *)RDEscapeString(keyname));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    feed_id=q->value(0).toUInt();
  }
  delete q;
}


RDFeed::RDFeed(unsigned id,QObject *parent,const char *name)
  : QObject(parent,name)
{
  RDSqlQuery *q;
  QString sql;

  feed_id=id;

  sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%u",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    feed_keyname=q->value(0).toString();
  }
  delete q;
}


bool RDFeed::exists() const
{
  return RDDoesRowExist("FEEDS","NAME",feed_keyname);
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


QString RDFeed::channelLanguage() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"CHANNEL_LANGUAGE").
    toString();
}


void RDFeed::setChannelLanguage(const QString &str)
{
  SetRow("CHANNEL_LANGUAGE",str);
}


QString RDFeed::baseUrl() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"BASE_URL").
    toString();
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
  SetRow("LAST_BUILD_DATETIME",datetime.toString("yyyy-MM-dd hh:mm:ss"));
}


QDateTime RDFeed::originDateTime() const
{
  return RDGetSqlValue("FEEDS","KEY_NAME",feed_keyname,"ORIGIN_DATETIME").
    toDateTime();
}


void RDFeed::setOriginDateTime(const QDateTime &datetime) const
{
  SetRow("ORIGIN_DATETIME",datetime.toString("yyyy-MM-dd hh:mm:ss"));
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
  QUrl url(baseUrl());
  QString ret;
  RDPodcast *cast;

  switch(mode) {
    case RDFeed::LinkNone:
      ret="";
      break;

    case RDFeed::LinkDirect:
      cast=new RDPodcast(cast_id);
      ret=QString().sprintf("%s/%s",
			    (const char *)baseUrl(),
			    (const char *)cast->audioFilename());
      delete cast;
      break;

    case RDFeed::LinkCounted:
      ret=QString().sprintf("http://%s%s/rd-bin/rdfeed.%s?%s&cast_id=%d",
			    (const char *)basePreamble(),
			    (const char *)cgi_hostname,
			    (const char *)uploadExtension(),
			    (const char *)keyName(),
			    cast_id);
      break;
  }
  return ret;
}


unsigned RDFeed::postCut(RDUser *user,RDStation *station,
			 const QString &cutname,Error *err,bool log_debug,
			 RDConfig *config)
{
  QString tmpfile;
  QString destfile;
  QString sql;
  RDSqlQuery *q;
  RDPodcast *cast=NULL;
  RDUpload *upload=NULL;
  RDUpload::ErrorCode upload_err;
  RDAudioConvert::ErrorCode audio_conv_err;

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
  RDAudioExport *conv=new RDAudioExport(station,config,this);
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
  switch(conv->runExport(user->name(),user->password(),&audio_conv_err)) {
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
  cast=new RDPodcast(cast_id);
  upload=new RDUpload(station->name(),this);
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

  emit postProgressChanged(totalPostSteps());

  return cast_id;
}


unsigned RDFeed::postFile(RDStation *station,const QString &srcfile,Error *err,
			  bool log_debug,RDConfig *config)
{
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
  RDAudioConvert *conv=new RDAudioConvert(station->name(),this);
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
  RDPodcast *cast=new RDPodcast(cast_id);
  upload=new RDUpload(station->name(),this);
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
    printf("Upload Error: %s\n",(const char *)RDUpload::errorText(upload_err));
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
  unlink(QString().sprintf("%s.wav",(const char *)tmpfile));
  unlink(tmpfile);
  emit postProgressChanged(totalPostSteps());

  *err=RDFeed::ErrorOk;
  return cast_id;
}


int RDFeed::totalPostSteps() const
{
  return RDFEED_TOTAL_POST_STEPS;
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

  sql=QString().sprintf("select CHANNEL_TITLE,CHANNEL_DESCRIPTION,\
                         CHANNEL_CATEGORY,CHANNEL_LINK,MAX_SHELF_LIFE,\
                         UPLOAD_FORMAT,UPLOAD_EXTENSION from FEEDS \
                         where ID=%u",feed_id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return 0;
  }

  //
  // Create Entry
  //
  sql=QString().sprintf("insert into PODCASTS set \
                         FEED_ID=%u,\
                         ITEM_TITLE=\"%s\",\
                         ITEM_DESCRIPTION=\"%s\",\
                         ITEM_CATEGORY=\"%s\",\
                         ITEM_LINK=\"%s\",\
                         SHELF_LIFE=%d,\
                         EFFECTIVE_DATETIME=UTC_TIMESTAMP(),\
                         ORIGIN_DATETIME=UTC_TIMESTAMP()",
			feed_id,
			(const char *)RDEscapeString(q->value(0).toString()),
			(const char *)RDEscapeString(q->value(1).toString()),
			(const char *)RDEscapeString(q->value(2).toString()),
			(const char *)RDEscapeString(q->value(3).toString()),
			q->value(4).toInt());
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
  *filename=QString().
    sprintf("%s.%s",
	    (const char *)QString().sprintf("%06u_%06u",feed_id,cast_id),
	    (const char *)q->value(6).toString());
  sql=QString().sprintf("update PODCASTS set AUDIO_FILENAME=\"%s\",\
                         AUDIO_LENGTH=%d,\
                         AUDIO_TIME=%d where ID=%u",
			(const char *)(*filename),
			bytes,msecs,cast_id);
  q1=new RDSqlQuery(sql);
  delete q1;
  delete q;
  return cast_id;
}


QString RDFeed::GetTempFilename() const
{
  char tempname[PATH_MAX];

  sprintf(tempname,"%s/podcastXXXXXX",(const char *)RDTempDir());
  if(mkstemp(tempname)<0) {
    return QString();
  }

  return QString(tempname);
}


void RDFeed::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE FEEDS SET %s=%d WHERE KEY_NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)feed_keyname);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDFeed::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE FEEDS SET %s=\"%s\" WHERE KEY_NAME=\"%s\"",
			(const char *)param,
			(const char *)RDEscapeString(value),
			(const char *)feed_keyname);
  q=new RDSqlQuery(sql);
  delete q;
}
