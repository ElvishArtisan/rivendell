// rdreport.h
//
// Abstract a Rivendell Report Descriptor
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>

#include <rdconfig.h>
#include <rdlog_line.h>
#include <rdstation.h>
#include <rdsvc.h>

class RDReport
{
 public:
  enum ExportFilter {CbsiDeltaFlex=0,TextLog=1,BmiEmr=2,Technical=3,
		     SoundExchange=4,RadioTraffic=5,VisualTraffic=6,
		     CounterPoint=7,Music1=8,MusicSummary=9,WideOrbit=10,
		     NprSoundExchange=11,MusicPlayout=12,NaturalLog=13,
		     MusicClassical=14,MrMaster=15,SpinCount=16,CutLog=17,
		     CounterPoint2=18,ResultsReport=19,RadioTraffic2=20,
		     LastFilter=21};
  enum ExportOs {Linux=0,Windows=1};
  enum ExportType {Generic=0,Traffic=1,Music=2};
  enum StationType {TypeOther=0,TypeAm=1,TypeFm=2,TypeLast=3};
  enum ErrorCode {ErrorOk=0,ErrorCanceled=1,ErrorCantOpen=2};
  RDReport(const QString &rptname,RDStation *station,RDConfig *config,
	   QObject *parent=0);
  QString name() const;
  bool exists() const;
  QString description() const;
  void setDescription(const QString &desc) const;
  ExportFilter filter() const;
  void setFilter(ExportFilter filter) const;
  QString exportPath(ExportOs ostype) const;
  void setExportPath(ExportOs ostype,const QString &path) const;
  QString postExportCommand(ExportOs ostype) const;
  void setPostExportCommand(ExportOs ostype,const QString &cmd) const;
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
  bool aggregateTuningHoursRequired() const;
  RDReport::ErrorCode errorCode() const;
  bool outputExists(const QDate &startdate);
  bool generateReport(const QDate &startdate,const QDate &enddate,
		      RDStation *station,QString *out_path,double ath=-1.0);
  static QString filterText(RDReport::ExportFilter filter);
  static QString stationTypeText(RDReport::StationType type);
  static bool multipleDaysAllowed(RDReport::ExportFilter filter);
  static bool multipleMonthsAllowed(RDReport::ExportFilter filter);
  static bool aggregateTuningHoursRequired(RDReport::ExportFilter filter);
  static QString errorText(RDReport::ErrorCode code);
  static QString leftJustify(const QString &str,int width);
  static QString rightJustify(const QString &str,int width);
  static QString center(const QString &str,int width);

 private:
  bool ExportDeltaflex(const QString &filename,const QDate &startdate,
		       const QDate &enddate,const QString &mixtable);
  bool ExportTextLog(const QString &filename,const QDate &startdate,
		     const QDate &enddate,const QString &mixtable);
  bool ExportBmiEmr(const QString &filename,const QDate &startdate,
		    const QDate &enddate,const QString &mixtable);
  bool ExportTechnical(const QString &filename,const QDate &startdate,
		       const QDate &enddate,bool incl_hdr,bool incl_crs,
		       const QString &mixtable);
  bool ExportSoundEx(const QString &filename,const QDate &startdate,
		     const QDate &enddate,double ath,const QString &mixtable);
  bool ExportNprSoundEx(const QString &filename,const QDate &startdate,
			const QDate &enddate,const QString &mixtable);
  bool ExportRadioTraffic(const QString &filename,const QDate &startdate,
			  const QDate &enddate,const QString &mixtable,
			  int version);
  bool ExportMusicClassical(const QString &filename,const QDate &startdate,
			    const QDate &enddate,const QString &mixtable);
  bool ExportMusicPlayout(const QString &filename,const QDate &startdate,
			  const QDate &enddate,const QString &mixtable);
  bool ExportMusicSummary(const QString &filename,const QDate &startdate,
			  const QDate &enddate,const QString &mixtable);
  bool ExportSpinCount(const QString &filename,const QDate &startdate,
		       const QDate &enddate,const QString &mixtable);
  bool ExportCutLog(const QString &filename,const QDate &startdate,
		    const QDate &enddate,const QString &mixtable);
  bool ExportResultsReport(const QString &filename,const QDate &startdate,
			   const QDate &enddate,const QString &mixtable);
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,bool value) const;
  void SetRow(const QString &param,const QTime &value) const;
  void SetRowNull(const QString &param) const;
  QString OsFieldName(ExportOs os) const;
  QString TypeFieldName(ExportType type,bool forced) const;
  QString report_name;
  RDStation *report_station;
  RDConfig *report_config;
  RDReport::ErrorCode report_error_code;
};


#endif   // RDREPORT_H
