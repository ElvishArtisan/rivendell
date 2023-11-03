// rdcoreapplication.h
//
// Base Application Class
//
//   (C) Copyright 2018-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCOREAPPLICATION_H
#define RDCOREAPPLICATION_H

#include <stdarg.h>
#include <syslog.h>

#include <QObject>
#include <QStringList>

#include <rdairplay_conf.h>
#include <rdcae.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>
#include <rddb.h>
#include <rddbheartbeat.h>
#include <rdlibrary_conf.h>
#include <rdlogedit_conf.h>
#include <rdportnames.h>
#include <rdripc.h>
#include <rdrssschemas.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rduser.h>

class RDCoreApplication : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorType {ErrorOk=0,ErrorDbVersionSkew=1,ErrorNoHostEntry=2,
  ErrorNoService=3};
  enum ExitCode {ExitOk=0,ExitPriorInstance=1,ExitNoDb=2,ExitSvcFailed=3,
		 ExitInvalidOption=4,ExitOutputProtected=5,ExitNoSvc=6,
		 ExitNoLog=7,ExitNoReport=8,ExitLogGenFailed=9,
		 ExitLogLinkFailed=10,ExitNoPerms=11,ExitReportFailed=12,
		 ExitImportFailed=13,ExitNoDropbox=14,ExitNoGroup=15,
		 ExitInvalidCart=16,ExitNoSchedCode=17,
		 ExitBadTicket=18,ExitNoStation=19,ExitInternalError=20,
		 ExitLast=21};
  RDCoreApplication(const QString &module_name,const QString &cmdname,
		    const QString &usage,bool use_translations,QObject *parent);
  ~RDCoreApplication();
  bool open(QString *err_msg,ErrorType *err_type,
	    bool check_svc,bool check_unique);
  RDAirPlayConf *airplayConf();
  RDCae *cae();
  RDCmdSwitch *cmdSwitch();
  RDConfig *config();
  RDLibraryConf *libraryConf();
  RDLogeditConf *logeditConf();
  RDAirPlayConf *panelConf();
  RDPortNames *portNames();
  RDRipc *ripc();
  RDRssSchemas *rssSchemas();
  RDStation *station();
  RDSystem *system();
  RDUser *user();
  bool showTwelveHourTime() const;
  QString longDateFormat() const;
  QString longDateString(const QDate &date)  const;
  QString shortDateFormat() const;
  QString shortDateString(const QDate &date) const;
  QString shortDateTimeString(const QDateTime &dt,bool show_secs=true) const;
  QString timeString(const QTime &time,bool show_secs=true,
		     const QString &padding="") const;
  QString timeFormat(bool show_secs) const;
  QString tenthsTimeString(const QTime &time,const QString &padding="") const;
  bool dropTable(const QString &tbl_name);
  void addTempFile(const QString &pathname);
  void syslog(int priority,const char *fmt,...) const;
  void logAuthenticationFailure(const QHostAddress &orig_addr,
				const QString &login_name=QString());
  static void syslog(RDConfig *config,int priority,const char *fmt,...);
  static QString exitCodeText(ExitCode code);
  static bool isUniqueProcess(const QString &cmdname);

 private slots:
  void userChangedData();

 signals:
  void userChanged();

 protected:
  QString moduleName() const;
  QString commandName() const;

 private:
  bool CheckService(QString *err_msg);
  RDAirPlayConf *app_airplay_conf;
  RDAirPlayConf *app_panel_conf;
  RDCae *app_cae;
  RDCmdSwitch *app_cmd_switch;
  RDConfig  *app_config;
  RDLibraryConf *app_library_conf;
  RDLogeditConf *app_logedit_conf;
  RDPortNames *app_port_names;
  RDRipc *app_ripc;
  RDRssSchemas *app_schemas;
  RDStation *app_station;
  RDSystem *app_system;
  RDUser *app_user;
  RDDbHeartbeat *app_heartbeat;
  QString app_ticket;
  QString app_module_name;
  char app_syslog_name[PATH_MAX];
  QString app_command_name;
  QString app_usage;
  QString app_long_date_format;
  QString app_short_date_format;
  bool app_show_twelve_hour_time;
};

#endif  // RDCOREAPPLICATION_H
