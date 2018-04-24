// rdfeed.h
//
// Abstract a Rivendell RSS Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDFEED_H
#define RDFEED_H

#include <qsqldatabase.h>
#include <qobject.h>

#include <rdconfig.h>
#include <rduser.h>
#include <rdstation.h>
#include <rdsettings.h>

//
// Default XML Templates
//
#define DEFAULT_HEADER_XML "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<rss version=\"2.0\">"
#define DEFAULT_CHANNEL_XML "<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>"
#define DEFAULT_ITEM_XML "<title>%ITEM_TITLE%</title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<author>%ITEM_AUTHOR%</author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\"  type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>"

#define RDFEED_TOTAL_POST_STEPS 4

class RDFeed : public QObject
{
  Q_OBJECT;
 public:
  enum Error {ErrorOk=0,ErrorNoFile=1,ErrorCannotOpenFile=2,
	      ErrorUnsupportedType=3,ErrorUploadFailed=4,ErrorGeneral=5};
  enum MediaLinkMode {LinkNone=0,LinkDirect=1,LinkCounted=2};
  RDFeed(const QString &keyname,QObject *parent=0);
  RDFeed(unsigned id,QObject *parent=0);
  QString keyName() const;
  unsigned id() const;
  bool exists() const;
  QString channelTitle() const;
  void setChannelTitle(const QString &str) const;
  QString channelDescription() const;
  void setChannelDescription(const QString &str) const;
  QString channelCategory() const;
  void setChannelCategory(const QString &str) const;
  QString channelLink() const;
  void setChannelLink(const QString &str) const;
  QString channelCopyright() const;
  void setChannelCopyright(const QString &str) const;
  QString channelWebmaster() const;
  void setChannelWebmaster(const QString &str) const;
  QString channelLanguage() const;
  void setChannelLanguage(const QString &str);
  QString baseUrl() const;
  void setBaseUrl(const QString &str) const;
  QString basePreamble() const;
  void setBasePreamble(const QString &str) const;
  QString purgeUrl() const;
  void setPurgeUrl(const QString &str) const;
  QString purgeUsername() const;
  void setPurgeUsername(const QString &str) const;
  QString purgePassword() const;
  void setPurgePassword(const QString &str) const;
  QString headerXml() const;
  void setHeaderXml(const QString &str);
  QString channelXml() const;
  void setChannelXml(const QString &str);
  QString itemXml() const;
  void setItemXml(const QString &str);
  bool castOrder() const;
  void setCastOrder(bool state) const;
  int maxShelfLife() const;
  void setMaxShelfLife(int days);
  QDateTime lastBuildDateTime() const;
  void setLastBuildDateTime(const QDateTime &datetime) const;
  QDateTime originDateTime() const;
  void setOriginDateTime(const QDateTime &datetime) const;
  bool enableAutopost() const;
  void setEnableAutopost(bool state) const;
  bool keepMetadata() const;
  void setKeepMetadata(bool state);
  RDSettings::Format uploadFormat() const;
  void setUploadFormat(RDSettings::Format fmt) const;
  int uploadChannels() const;
  void setUploadChannels(int chans) const;
  int uploadQuality() const;
  void setUploadQuality(int qual) const;
  int uploadBitRate() const;
  void setUploadBitRate(int rate) const;
  int uploadSampleRate() const;
  void setUploadSampleRate(int rate) const;
  QString uploadExtension() const;
  void setUploadExtension(const QString &str);
  QString uploadMimetype() const;
  void setUploadMimetype(const QString &str);
  int normalizeLevel() const;
  void setNormalizeLevel(int lvl) const;
  QString redirectPath() const;
  void setRedirectPath(const QString &str);
  RDFeed::MediaLinkMode mediaLinkMode() const;
  void setMediaLinkMode(RDFeed::MediaLinkMode mode) const;
  QString audioUrl(RDFeed::MediaLinkMode mode,const QString &cgi_hostname,
		   unsigned cast_id);
  unsigned postCut(RDUser *user,RDStation *station,
		   const QString &cutname,Error *err,bool log_debug,
		   RDConfig *config);
  unsigned postFile(RDStation *station,const QString &srcfile,Error *err,
		    bool log_debug,RDConfig *config);
  int totalPostSteps() const;
  static int create(const QString &keyname,bool allow_all_users);
  static void remove(const QString &keyname,QString *errs=NULL);
  static QString errorString(RDFeed::Error err);
  static QString tableName(const QString &keyname);

 signals:
  void postProgressChanged(int step);

 private:
  unsigned CreateCast(QString *filename,int bytes,int msecs) const;
  QString GetTempFilename() const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,const QDateTime &value,
              const QString &format) const;
  QString feed_keyname;
  unsigned feed_id;
};


#endif 
