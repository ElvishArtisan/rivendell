// rdsvc.h
//
// Abstract a Rivendell Service
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <vector>

#include <qobject.h>
#include <qsqldatabase.h>

#ifndef RDSVC_H
#define RDSVC_H

class RDSvc : public QObject
{
  Q_OBJECT
 public:
  enum ImportOs {Linux=0,Windows=1};
  enum ImportSource {Traffic=0,Music=1,NoSource=255};
  enum ImportField {CartNumber=0,ExtData=3,ExtEventId=4,ExtAnncType=5,
		    Title=6,StartHours=7,StartMinutes=8,StartSeconds=9,
		    LengthHours=10,LengthMinutes=11,LengthSeconds=12,
		    TimeType=13,TimeWaitMinutes=14,TimeWaitSeconds=15,
		    TransitionType=16};
  enum ImportType {Cart=0,Break=1,Track=2,Label=3};
  RDSvc(QString svcname,QObject *parent=0,const char *name=0);
  QString name() const;
  bool exists() const;
  QString description() const;
  void setDescription(const QString &desc) const;
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
  int elrShelflife() const;
  void setElrShelflife(int days) const;
  bool chainto() const;
  void setChainto(bool state) const;
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
  QString importPath(ImportSource src,ImportOs os) const;
  void setImportPath(ImportSource src,ImportOs os,const QString &path) const;
  QString preimportCommand(ImportSource src,ImportOs os) const;
  void setPreimportCommand(ImportSource src,ImportOs os,
			   const QString &path) const;
  int importOffset(ImportSource src,ImportField field,
		   const QString &templ="") const;
  void setImportOffset(ImportSource src,ImportField field,int offset) const;
  int importLength(ImportSource src,ImportField field,
		   const QString &templ="") const;
  void setImportLength(ImportSource src,ImportField field,int len) const;
  QString importFilename(ImportSource src,const QDate &date) const;
  bool import(ImportSource src,const QDate &date,const QString &break_str,
	      const QString &track_str,const QString &dest_table) const;
  bool generateLog(const QDate &date,const QString &logname,
		   const QString &nextname,QString *report);
  bool linkLog(RDSvc::ImportSource src,const QDate &date,
	       const QString &logname,QString *report);
  void clearLogLinks(RDSvc::ImportSource src,const QString &logname);
  void create(const QString exemplar) const;
  void remove() const;
  QString xml() const;
  static QString timeString(int hour,int secs);
  static QString svcTableName(const QString &svc_name);
  static QString className(ImportSource src);
  static QStringList importerClassList();
  static QStringList importerParameterList();

 signals:
  void generationProgress(int step);

 private:
  void GetImportLine(const QString &line,ImportSource src,const QDate &date,
		     const QString &break_str,const QString &track_str,
		     const QString &label_cart,const QString &dest_table,
		     int *prev_hour,int *prev_min,int *prev_sec) const;
  QString GetImportField(const QString &line,ImportSource src,
			 ImportField fld) const;
  QString SourceString(ImportSource src) const;
  QString OsString(ImportOs os) const;
  QString FieldString(ImportField field) const;
  void SetRow(const QString &param,QString value) const;
  void SetRow(const QString &param,int value) const;
  void GetParserStrings(ImportSource src,QString *break_str,QString *track_str,
			QString *label_cart,QString *track_cart);
  bool CheckId(std::vector<int> *v,int value);
  int GetTimeDiff(int hours1,int secs1,int hours2,int secs2) const;
  QString svc_name;
};


#endif 
