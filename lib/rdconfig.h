// rdconfig.h
//
// A container class for a Rivendell Base Configuration
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdconfig.h,v 1.18.8.7 2013/11/13 23:36:32 cvs Exp $
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

#ifndef RDCONFIG_H
#define RDCONFIG_H

#ifndef WIN32
#include <syslog.h>
#include <unistd.h>
#endif  // WIN32

#include <vector>

#include <qstring.h>

#include <rd.h>


class RDConfig
{
 public:
  enum LogFacility {LogNone=0,LogSyslog=1,LogFile=2};
#ifdef WIN32
  enum LogPriority {LogEmerg=0,LogAlert=1,LogCrit=2,
		    LogErr=3,LogWarning=4,LogNotice=5,
		    LogInfo=6,LogDebug=7};
#else
  enum LogPriority {LogEmerg=LOG_EMERG,LogAlert=LOG_ALERT,LogCrit=LOG_CRIT,
		    LogErr=LOG_ERR,LogWarning=LOG_WARNING,LogNotice=LOG_NOTICE,
		    LogInfo=LOG_INFO,LogDebug=LOG_DEBUG};

#endif  // WIN32
  RDConfig();
  RDConfig(QString filename);
  QString filename() const;
  void setFilename(QString filename);
  QString mysqlHostname() const;
  QString mysqlUsername() const;
  QString mysqlDbname() const;
  QString mysqlPassword() const;
  QString mysqlDriver() const;
  int mysqlHeartbeatInterval() const;
  RDConfig::LogFacility logFacility() const;
  QString logDirectory() const;
  QString logCoreDumpDirectory() const;
  QString logPattern() const;
  bool logXloadDebugData() const;
  void log(const QString &module,LogPriority prio,const QString &msg);
  int alsaPeriodQuantity() const;
  int alsaPeriodSize() const;
  int alsaChannelsPerPcm() const;
  QString stationName() const;
  QString password() const;
  QString audioOwner() const;
  QString audioGroup() const;
  QString audioRoot() const;
  QString audioExtension() const;
  QString audioFileName (QString cutname);
  QString label() const;
  QString audioStoreMountSource() const;
  QString audioStoreMountType() const;
  QString audioStoreMountOptions() const;
  QString audioStoreCaeHostname() const;
  QString audioStoreXportHostname() const;
  QString ripcdLogname() const;
  QString airplayLogname() const;
  QString catchdLogname() const;
  int jackConnections() const;
  QString jackPort(int num,int endpt) const;
  bool useStreamMeters() const;
  bool disableMaintChecks() const;
  QString caeLogfile() const;
  bool enableMixerLogging() const;
  unsigned channels() const;
#ifndef WIN32
  uid_t uid() const;
  gid_t gid() const;
#endif
  bool useRealtime();
  int realtimePriority();
  QString sasStation() const;
  int sasMatrix() const;
  unsigned sasBaseCart() const;
  QString sasTtyDevice() const;
  QString destination(unsigned n);
  void load();
  void clear();

 private:
  QString conf_filename;
  QString conf_mysql_hostname;
  QString conf_mysql_username;
  QString conf_mysql_dbname;
  QString conf_mysql_password;
  QString conf_mysql_driver;
  int conf_mysql_heartbeat_interval;
  RDConfig::LogFacility conf_log_facility;
  QString conf_log_directory;
  QString conf_log_core_dump_directory;
  QString conf_log_pattern;
  bool conf_log_xload_debug_data;
  int conf_alsa_period_quantity;
  int conf_alsa_period_size;
  int conf_alsa_channels_per_pcm;
  QString conf_station_name;
  QString conf_password;
  QString conf_audio_owner;
  QString conf_audio_group;
  QString conf_audio_root;
  QString conf_audio_extension;
  QString conf_label;
  QString conf_audio_store_mount_source;
  QString conf_audio_store_mount_type;
  QString conf_audio_store_mount_options;
  QString conf_audio_store_xport_hostname;
  QString conf_audio_store_cae_hostname;
  QString conf_ripcd_logname;
  QString conf_airplay_logname;
  QString conf_catchd_logname;
  bool conf_use_stream_meters;
  bool conf_disable_maint_checks;
  std::vector<QString> conf_jack_ports[2];
  unsigned conf_channels;
#ifndef WIN32
  uid_t conf_uid;
  gid_t conf_gid;
#endif
  QString conf_cae_logfile;
  bool conf_enable_mixer_logging;
  bool conf_use_realtime;
  int conf_realtime_priority;
  QString conf_sas_station;
  int conf_sas_matrix;
  unsigned conf_sas_base_cart;
  QString conf_sas_tty_device;
  std::vector<QString> conf_destinations;
};

RDConfig *RDConfiguration(void); 

#endif  // RDCONFIG_H
