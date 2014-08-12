// rdpodcast.h
//
// Abstract a Rivendell Podcast Entry
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpodcast.h,v 1.7 2010/07/29 19:32:33 cvs Exp $
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

#include <qsqldatabase.h>

#include <rdfeed.h>

#ifndef RDPODCAST_H
#define RDPODCAST_H

class RDPodcast
{
 public:
  enum Status {StatusPending=1,StatusActive=2,StatusExpired=3};
  RDPodcast(unsigned id);
  unsigned id() const;
  QString keyName() const;
  bool exists() const;
  unsigned feedId() const;
  void setFeedId(unsigned id) const;
  QString itemTitle() const;
  void setItemTitle(const QString &str) const;
  QString itemDescription() const;
  void setItemDescription(const QString &str) const;
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
  unsigned shelfLife() const;
  void setShelfLife(unsigned days) const;
  RDPodcast::Status status() const;
  void setStatus(RDPodcast::Status status);
  //QString audioUploadCommand(const QString &srcfile) const;
  //QString audioPurgeCommand() const;
  bool removeAudio(RDFeed *feed,QString *err_text,bool log_debug) const;
  static QString guid(const QString &url,const QString &filename,
		      unsigned feed_id,unsigned cast_id);
  static QString guid(const QString &full_url,
		      unsigned feed_id,unsigned cast_id);

 private:
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QString &value) const;
  QString podcast_keyname;
  unsigned podcast_id;
};


#endif 
