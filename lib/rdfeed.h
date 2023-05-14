// rdfeed.h
//
// Abstract a Rivendell RSS Feed
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <curl/curl.h>

#include <QObject>

#include <rdapplication.h>
#include <rdconfig.h>
#include <rdrssschemas.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rdtempdirectory.h>
#include <rduser.h>
#include <rdweb.h>

#define RDFEED_TOTAL_POST_STEPS 4

class RDFeed : public QObject
{
  Q_OBJECT;
 public:
  RDFeed(const QString &keyname,RDConfig *config,QObject *parent=0);
  RDFeed(unsigned id,RDConfig *config,QObject *parent=0);
  QString keyName() const;
  unsigned id() const;
  bool exists() const;
  bool isSuperfeed() const;
  void setIsSuperfeed(bool state) const;
  QStringList subfeedNames() const;
  QStringList isSubfeedOf() const;
  QString channelTitle() const;
  void setChannelTitle(const QString &str) const;
  QString channelDescription() const;
  void setChannelDescription(const QString &str) const;
  QString channelCategory() const;
  void setChannelCategory(const QString &str) const;
  QString channelSubCategory() const;
  void setChannelSubCategory(const QString &str) const;
  QString channelLink() const;
  void setChannelLink(const QString &str) const;
  QString channelCopyright() const;
  void setChannelCopyright(const QString &str) const;
  QString channelEditor() const;
  void setChannelEditor(const QString &str) const;
  QString channelAuthor() const;
  void setChannelAuthor(const QString &str) const;
  bool channelAuthorIsDefault() const;
  void setChannelAuthorIsDefault(bool state) const;
  QString channelOwnerName() const;
  void setChannelOwnerName(const QString &str) const;
  QString channelOwnerEmail() const;
  void setChannelOwnerEmail(const QString &str) const;
  QString channelWebmaster() const;
  void setChannelWebmaster(const QString &str) const;
  QString channelLanguage() const;
  void setChannelLanguage(const QString &str);
  bool channelExplicit() const;
  void setChannelExplicit(bool state) const;
  int channelImageId() const;
  void setChannelImageId(int img_id) const;
  int defaultItemImageId() const;
  void setDefaultItemImageId(int img_id) const;
  QString baseUrl(const QString &subfeed_key_name) const;
  QString baseUrl(int subfeed_feed_id) const;
  void setBaseUrl(const QString &str) const;
  QString basePreamble() const;
  void setBasePreamble(const QString &str) const;
  QString purgeUrl() const;
  void setPurgeUrl(const QString &str) const;
  QString purgeUsername() const;
  void setPurgeUsername(const QString &str) const;
  QString purgePassword() const;
  void setPurgePassword(const QString &str) const;
  bool purgeUseIdFile() const;
  void setPurgeUseIdFile(bool state) const;
  RDRssSchemas::RssSchema rssSchema() const;
  void setRssSchema(RDRssSchemas::RssSchema schema) const;
  QString headerXml() const;
  void setHeaderXml(const QString &str);
  QString channelXml() const;
  void setChannelXml(const QString &str);
  QString itemXml() const;
  void setItemXml(const QString &str);
  QString feedUrl() const;
  bool castOrderIsAscending() const;
  void setCastOrderIsAscending(bool state) const;
  int maxShelfLife() const;
  void setMaxShelfLife(int days);
  QDateTime lastBuildDateTime() const;
  void setLastBuildDateTime(const QDateTime &datetime) const;
  QDateTime originDateTime() const;
  void setOriginDateTime(const QDateTime &datetime) const;
  bool enableAutopost() const;
  void setEnableAutopost(bool state) const;
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
  QString sha1Hash() const;
  void setSha1Hash(const QString &str) const;
  QString cdnPurgePluginPath() const;
  void setCdnPurgePluginPath(const QString &str) const;
  QByteArray imageData(int img_id) const;
  int importImageFile(const QString &pathname,QString *err_msg,
		      QString desc="") const;
  bool deleteImage(int img_id,QString *err_msg);
  bool postPodcast(unsigned cast_id,QString *err_msg);
  QString audioUrl(unsigned cast_id);
  QString imageUrl(int img_id) const;
  bool postXml(QString *err_msg);  // WebAPI Call
  bool postXmlConditional(const QString &caption,QWidget *widget);
  bool removeRss();  // WebAPI Call
  bool postImage(int img_id) const;  // WebAPI Call
  bool removeImage(int img_id) const;  // WebAPI Call
  void removeAllImages();
  unsigned postCut(const QString &cutname,QString *err_msg);
  unsigned postFile(const QString &srcfile,QString *err_msg);
  unsigned postLog(const QString &logname,const QTime &start_time,
		   bool stop_at_stop,int start_line,int end_line,
		   QString *err_msg);
  QString rssXml(QString *err_msg,const QDateTime &now,bool *ok,
		 QList<unsigned> *active_cast_ids=NULL);
  bool rssFrontXml(QByteArray *xml,QString *err_msg) const;  // WebAPI Call
  bool rssBackXml(QByteArray *xml,QString *err_msg) const;// Public HTTP(S) Call
  void activeCasts(QList<unsigned> *cast_ids);
  bool frontActiveCasts(QList<unsigned> *cast_ids,QString *err_msg);
  bool backActiveCasts(QList<unsigned> *cast_ids,QString *err_msg);
  static unsigned create(const QString &keyname,bool enable_users,
			 QString *err_msg);
  static QString imageFilename(int feed_id,int img_id,const QString &ext);
  static QString publicUrl(const QString &base_url,const QString &keyname);
  static QString itunesCategoryXml(const QString &category,
				   const QString &sub_category,int padding=0);

 signals:
  void postProgressChanged(int step);
  void postProgressRangeChanged(int min,int max);

 private slots:
  void renderMessage(const QString &msg);
  void renderLineStartedData(int lineno,int total_lines);

 private:
  bool SavePodcast(unsigned cast_id,const QString &src_filename,
		   QString *err_msg);
  unsigned CreateCast(QString *filename,int bytes,int msecs) const;
  void AbandonCast(unsigned cast_id) const;
  QString ResolveChannelWildcards(const QString &tmplt,RDSqlQuery *chan_q,
				  const QDateTime &build_datetime);
  QString ResolveItemWildcards(const QString &tmplt,RDSqlQuery *item_q,
			       RDSqlQuery *chan_q);
  QString GetTempFilename() const;
  void LoadSchemas();
  QStringList *SetupCurlLogging(CURL *curl) const;
  void ProcessCurlLogging(const QString &label,QStringList *err_msgs) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,const QDateTime &value,
              const QString &format) const;
  QString feed_keyname;
  unsigned feed_id;
  QString feed_cgi_hostname;
  RDConfig *feed_config;
  QByteArray feed_xml;
  int feed_xml_ptr;
  int feed_render_start_line;
  int feed_render_end_line;
};


#endif  // RDFEED_H
