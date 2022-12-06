// rdpodcast.h
//
// Abstract a Rivendell Podcast Entry
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

#include <QStringList>

#include <rdconfig.h>
#include <rdfeed.h>

#ifndef RDPODCAST_H
#define RDPODCAST_H

class RDPodcast
{
 public:
  enum Status {StatusPending=1,StatusActive=2,StatusExpired=3};
  RDPodcast(RDConfig *config,unsigned id);
  unsigned id() const;
  QString keyName() const;
  bool exists() const;
  unsigned feedId() const;
  void setFeedId(unsigned id) const;
  QString itemTitle() const;
  void setItemTitle(const QString &str) const;
  QString itemDescription() const;
  void setItemDescription(const QString &str) const;
  bool itemExplicit() const;
  void setItemExplicit(bool state) const;
  int itemImageId() const;
  void setItemImageId(int img_id) const;
  QString itemCategory() const;
  void setItemCategory(const QString &str) const;
  QString itemLink() const;
  void setItemLink(const QString &str) const;
  QString itemAuthor() const;
  void setItemAuthor(const QString &str) const;
  QString itemComments() const;
  void setItemComments(const QString &str) const;
  QString itemSourceText() const;
  void setItemSourceText(const QString &str) const;
  QString itemSourceUrl() const;
  void setItemSourceUrl(const QString &str) const;
  QString originLoginName() const;
  void setOriginLoginName(const QString &str) const;
  QString originStation() const;
  void setOriginStation(const QString &str) const;
  QDateTime originDateTime() const;
  void setOriginDateTime(const QDateTime &datetime) const;
  QDateTime effectiveDateTime() const;
  void setEffectiveDateTime(const QDateTime &datetime) const;
  QString audioFilename() const;
  void setAudioFilename(const QString &str) const;
  int audioLength() const;
  void setAudioLength(int len) const;
  int audioTime() const;
  void setAudioTime(int msecs) const;
  QString sha1Hash() const;
  void setSha1Hash(const QString &str=QString()) const;
  QDateTime expirationDateTime() const;
  void setExpirationDateTime(const QDateTime &dt) const;
  RDPodcast::Status status() const;
  void setStatus(RDPodcast::Status status);
  bool dropAudio(RDFeed *feed,QString *err_text,bool log_debug) const;
  bool removePodcast() const;
  static QString guid(const QString &url,const QString &filename,
		      unsigned feed_id,unsigned cast_id);
  static QString guid(const QString &full_url,
		      unsigned feed_id,unsigned cast_id);

 private:
  bool DeletePodcast(unsigned cast_id) const;
  QStringList *SetupCurlLogging(CURL *curl) const;
  void ProcessCurlLogging(const QString &label,QStringList *err_msgs) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,const QDateTime &datetime,
	      const QString &value) const;
  QString podcast_keyname;
  unsigned podcast_feed_id;
  unsigned podcast_id;
  RDConfig *podcast_config;
};


#endif  // RDPODCAST_H
