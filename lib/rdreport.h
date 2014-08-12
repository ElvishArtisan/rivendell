// rdreport.h
//
// Abstract a Rivendell Report Descriptor
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdreport.h,v 1.17.8.7.2.4 2014/05/22 01:21:35 cvs Exp $
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

#ifndef RDREPORT_H
#define RDREPORT_H

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdstation.h>
#include <rdsvc.h>
#include <rdlog_line.h>

class RDReport
{
 public:
  enum ExportFilter {CbsiDeltaFlex=0,TextLog=1,BmiEmr=2,Technical=3,
		     SoundExchange=4,RadioTraffic=5,VisualTraffic=6,
		     CounterPoint=7,Music1=8,MusicSummary=9,WideOrbit=10,
		     NprSoundExchange=11,MusicPlayout=12,NaturalLog=13,
		     MusicClassical=14,LastFilter=15};
  enum ExportOs {Linux=0,Windows=1};
  enum ExportType {Generic=0,Traffic=1,Music=2};
  enum StationType {TypeOther=0,TypeAm=1,TypeFm=2,TypeLast=3};
  enum ErrorCode {ErrorOk=0,ErrorCanceled=1,ErrorCantOpen=2};
  RDReport(const QString &rptname,QObject *parent=0,const char *name=0);
  QString name() const;
  bool exists() const;
  QString description() const;
  void setDescription(const QString &desc) const;
  ExportFilter filter() const;
  void setFilter(ExportFilter filter) const;
  QString exportPath(ExportOs ostype) const;
  void setExportPath(ExportOs ostype,const QString &path) const;
  bool exportTypeEnabled(ExportType type) const;
  void setExportTypeEnabled(ExportType type,bool state) const;
  bool exportTypeForced(ExportType type) const;
  void setExportTypeForced(ExportType type,bool state) const;
  QString stationId() const;
  void setStationId(const QString &id) const;
  unsigned cartDigits() const;
  void setCartDigits(unsigned num) const;
  bool useLeadingZeros() const;
  void setUseLeadingZeros(bool state) const;
  int linesPerPage() const;
  void setLinesPerPage(int lines) const;
  QString serviceName() const;
  void setServiceName(const QString &name) const;
  RDReport::StationType stationType() const;
  void setStationType(RDReport::StationType type) const;
  QString stationFormat() const;
  void setStationFormat(const QString &fmt) const;
  RDLogLine::StartSource startSource() const;
  void setStartSource(RDLogLine::StartSource src) const;
  bool filterOnairFlag() const;
  void setFilterOnairFlag(bool state) const;
  bool filterGroups() const;
  void setFilterGroups(bool state) const;
  QTime startTime(bool *is_null=NULL) const;
  void setStartTime(const QTime &time) const;
  void setStartTime() const;
  QTime endTime(bool *is_null=NULL) const;
  void setEndTime(const QTime &time) const;
  void setEndTime() const;
  RDReport::ErrorCode errorCode() const;
  bool outputExists(const QDate &startdate);
  bool generateReport(const QDate &startdate,const QDate &enddate,
		      RDStation *station,QString *out_path);
  static QString filterText(RDReport::ExportFilter filter);
  static QString stationTypeText(RDReport::StationType type);
  static bool multipleDaysAllowed(RDReport::ExportFilter filter);
  static bool multipleMonthsAllowed(RDReport::ExportFilter filter);
  static QString errorText(RDReport::ErrorCode code);

 private:
  bool ExportDeltaflex(const QDate &startdate,const QDate &enddate,
		       const QString &mixtable);
  bool ExportTextLog(const QDate &startdate,const QDate &enddate,
		     const QString &mixtable);
  bool ExportBmiEmr(const QDate &startdate,const QDate &enddate,
		    const QString &mixtable);
  bool ExportTechnical(const QDate &startdate,const QDate &enddate,
		       const QString &mixtable);
  bool ExportSoundEx(const QDate &startdate,const QDate &enddate,
		     const QString &mixtable);
  bool ExportNprSoundEx(const QDate &startdate,const QDate &enddate,
			const QString &mixtable);
  bool ExportRadioTraffic(const QDate &startdate,const QDate &enddate,
			  const QString &mixtable);
  bool ExportMusicClassical(const QDate &startdate,const QDate &enddate,
			    const QString &mixtable);
  bool ExportMusicPlayout(const QDate &startdate,const QDate &enddate,
			  const QString &mixtable);
  bool ExportMusicSummary(const QDate &startdate,const QDate &enddate,
			  const QString &mixtable);
  QString StringField(const QString &str,const QString &null_text="") const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,bool value) const;
  void SetRow(const QString &param,const QTime &value) const;
  void SetRowNull(const QString &param) const;
  QString OsFieldName(ExportOs os) const;
  QString TypeFieldName(ExportType type,bool forced) const;
  QString report_name;
  RDReport::ErrorCode report_error_code;
};


#endif   // RDREPORT_H
