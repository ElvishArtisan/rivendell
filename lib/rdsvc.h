// rdsvc.h
//
// Abstract a Rivendell Service
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

#ifndef RDSVC_H
#define RDSVC_H

#include <vector>

#include <QObject>

#include "rdconfig.h"
#include "rdlog.h"
#include "rdloglock.h"
#include "rdstation.h"
#include "rduser.h"

class RDSvc : public QObject
{
  Q_OBJECT
 public:
  enum ImportSource {Traffic=0,Music=1};
  enum ImportField {CartNumber=0,ExtData=3,ExtEventId=4,ExtAnncType=5,
		    Title=6,StartHours=7,StartMinutes=8,StartSeconds=9,
		    LengthHours=10,LengthMinutes=11,LengthSeconds=12,
		    TransType=13,TimeType=14};
  enum ShelflifeOrigin {OriginAirDate=0,OriginCreationDate=1};
  enum SubEventInheritance {ParentEvent=0,SchedFile=1};
  RDSvc(QString svcname,RDStation *station,RDConfig *config,QObject *parent=0);
  QString name() const;
  bool exists() const;
  QString description() const;
  void setDescription(const QString &desc) const;
  bool bypassMode() const;
  void setBypassMode(bool state) const;
  QString programCode() const;
  void setProgramCode(const QString &str) const;
  QString nameTemplate() const;
  void setNameTemplate(const QString &str) const;
  QString descriptionTemplate() const;
  void setDescriptionTemplate(const QString &str) const;
  QString trackGroup() const;
  void setTrackGroup(const QString &group) const;
  QString autospotGroup() const;
  void setAutospotGroup(const QString &group) const;
  bool autoRefresh() const;
  void setAutoRefresh(bool state);
  int defaultLogShelflife() const;
  void setDefaultLogShelflife(int days) const;
  ShelflifeOrigin logShelflifeOrigin() const;
  void setLogShelflifeOrigin(ShelflifeOrigin orig);
  int elrShelflife() const;
  void setElrShelflife(int days) const;
  bool includeImportMarkers(ImportSource src) const;
  void setIncludeImportMarkers(ImportSource src,bool state);
  bool chainto() const;
  void setChainto(bool state) const;
  SubEventInheritance subEventInheritance() const;
  void setSubEventInheritance(SubEventInheritance inherit) const;
  QString importTemplate(ImportSource src) const;
  void setImportTemplate(ImportSource src,const QString &str) const;
  QString breakString() const;
  void setBreakString(const QString &str);
  QString trackString(ImportSource src) const;
  void setTrackString(ImportSource src,const QString &str);
  QString labelCart(ImportSource src) const;
  void setLabelCart(ImportSource src,const QString &str);
  QString trackCart(ImportSource src) const;
  void setTrackCart(ImportSource src,const QString &str);
  QString importPath(ImportSource src) const;
  void setImportPath(ImportSource src,const QString &path) const;
  QString preimportCommand(ImportSource src) const;
  void setPreimportCommand(ImportSource src,const QString &path) const;
  int importOffset(ImportSource src,ImportField field) const;
  void setImportOffset(ImportSource src,ImportField field,int offset) const;
  int importLength(ImportSource src,ImportField field) const;
  void setImportLength(ImportSource src,ImportField field,int len) const;
  QString importFilename(ImportSource src,const QDate &date) const;
  bool import(ImportSource src,const QDate &date,const QString &break_str,
	      const QString &track_str,bool resolve_implied_times) const;
  bool generateLog(const QDate &date,const QString &logname,
		   const QString &nextname,QString *report,RDUser *user,
		   QString *err_msg);
  bool linkLog(RDSvc::ImportSource src,const QDate &date,
	       const QString &logname,QString *report,RDUser *user,
	       QString *err_msg);
  bool clearLogLinks(RDSvc::ImportSource src,const QString &logname,
		     RDUser *user,QString *err_msg);
  void create(const QString exemplar) const;
  void remove() const;
  QString xml() const;
  static bool create(const QString &name,QString *err_msg,
		     const QString &exemplar,RDConfig *config);
  static void remove(const QString &name);
  static bool exists(const QString &name);
  static QString timeString(int hour,int secs);

 signals:
  void generationProgress(int step);

 private:
  bool TryLock(RDLogLock *lock,QString *err_msg);
  QString SourceString(ImportSource src) const;
  QString FieldString(ImportField field) const;
  void SetRow(const QString &param,QString value) const;
  void SetRow(const QString &param,int value) const;
  void GetParserStrings(ImportSource src,QString *break_str,QString *track_str,
			QString *label_cart,QString *track_cart);
  bool CheckId(std::vector<int> *v,int value);
  QString MakeErrorLine(int indent,unsigned lineno,const QString &msg) const;
  bool ResolveInlineEvents(const QString &logname,QString *err_msg);
  bool ValidateInlineEvents(QString *err_msg) const;
  void ProcessGridEvents(RDLog *log,RDLogModel *dst_model,RDLogModel *src_model,
			 const QString &track_str,const QString &label_cart,
			 const QString &track_cart,RDLog::Source link_src,
			 RDLogLine::Type src_type,QString *err_msgs);
  void ProcessBypassMusicEvents(RDLog *log,RDLogModel *dst_model,
				RDLogModel *src_model,const QString &track_str,
				const QString &label_cart,
				const QString &track_cart,
				RDLog::Source link_src,RDLogLine::Type src_type,
				QString *err_msgs);
  void ProcessBypassTrafficEvents(RDLog *log,RDLogModel *dst_model,
				  RDLogModel *src_model,
				  const QString &track_str,
				  const QString &label_cart,
				  const QString &track_cart,
				  RDLog::Source link_src,
				  QString *err_msgs);
  int GetCartLength(unsigned cartnum,int def_length) const;
  QString svc_name;
  RDStation *svc_station;
  RDConfig *svc_config;
};


#endif  // RDSVC_H
