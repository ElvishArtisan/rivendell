// rdlog.h
//
// Abstract a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlog.h,v 1.16.6.5.2.1 2014/05/20 14:01:49 cvs Exp $
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

#include <rdconfig.h>
#include <rdlog_event.h>
#include <rduser.h>
#include <rdstation.h>

#ifndef RDLOG_H
#define RDLOG_H

class RDLog
{
  public:
   enum Type {Log=0,Event=1,Clock=2,Grid=3};
   enum Source {SourceTraffic=0,SourceMusic=1};
   enum LinkState {LinkMissing=0,LinkDone=1,LinkNotPresent=2};
   RDLog(const QString &name,bool create=false);
   QString name() const;
   bool exists() const;
   bool logExists() const;
   void setLogExists(bool state) const;
   RDLog::Type type() const;
   void setType(RDLog::Type type) const;
   QString description() const;
   void setDescription(const QString &desc) const;
   QString service() const;
   void setService(const QString &svc) const;
   QDate startDate() const;
   void setStartDate(const QDate &date) const;
   QDate endDate() const;
   void setEndDate(const QDate &date) const;
   QDate purgeDate() const;
   void setPurgeDate(const QDate &date) const;
   QString originUser() const;
   void setOriginUser(const QString &user) const;
   QDateTime originDatetime() const;
   void setOriginDatetime(const QDateTime &datetime) const;
   QDateTime linkDatetime() const;
   void setLinkDatetime(const QDateTime &datetime) const;
   QDateTime modifiedDatetime() const;
   void setModifiedDatetime(const QDateTime &datetime) const;
   bool autoRefresh() const;
   void setAutoRefresh(bool state) const;
   unsigned scheduledTracks() const;
   void setScheduledTracks(unsigned tracks) const;
   unsigned completedTracks() const;
   void setCompletedTracks(unsigned tracks) const;
   int linkQuantity(RDLog::Source src) const;
   void setLinkQuantity(RDLog::Source src,int quan) const;
   void updateLinkQuantity(RDLog::Source src) const;
   RDLog::LinkState linkState(RDLog::Source src) const;
   void setLinkState(RDLog::Source src,bool state) const;
   int nextId() const;
   void setNextId(int id) const;
   bool isReady() const;
   bool remove(RDStation *station,RDUser *user,RDConfig *config) const;
   void updateTracks();
   int removeTracks(RDStation *station,RDUser *user,RDConfig *config) const;
   RDLogEvent *createLogEvent() const;
   QString xml() const;
   static bool exists(const QString &name);
   static QString tableName(const QString &log_name);

  private:
   int GetIntValue(const QString &field) const;
   unsigned GetUnsignedValue(const QString &field) const;
   QString GetStringValue(const QString &field) const;
   QDate GetDateValue(const QString &field) const;
   QDateTime GetDatetimeValue(const QString &field) const;
   void SetRow(const QString &param,int value) const;
   void SetRow(const QString &param,unsigned value) const;
   void SetRow(const QString &param,const QString &value) const;
   void SetRow(const QString &param,const QDate &value) const;
   void SetRow(const QString &param,const QDateTime &value) const;
   QString log_name;
};


#endif 
