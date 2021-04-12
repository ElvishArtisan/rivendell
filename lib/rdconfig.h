// rdconfig.h
//
// A container class for a Rivendell Base Configuration
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <unistd.h>

#include <vector>

#include <qhostaddress.h>
#include <qstring.h>

#include <rd.h>

class RDConfig
{
 public:
  enum RDSelectExitCode {RDSelectOk=0,RDSelectInvalidArguments=1,
			 RDSelectNoSuchConfiguration=2,
			 RDSelectModulesActive=3,
			 RDSelectNotRoot=4,
			 RDSelectSystemctlCrashed=5,
			 RDSelectRivendellShutdownFailed=6,
			 RDSelectAudioUnmountFailed=7,
			 RDSelectAudioMountFailed=8,
			 RDSelectRivendellStartupFailed=9,
			 RDSelectNoCurrentConfig=10,
			 RDSelectSymlinkFailed=11,
			 RDSelectInvalidName=12,
			 RDSelectMountCrashed=13,
			 RDSelectCantAccessAutomount=14,
			 RDSelectLast=15};
  RDConfig();
  RDConfig(const QString &filename);
  QString filename() const;
  void setFilename(QString filename);
  QString moduleName() const;
  void setModuleName(const QString &modname);
  QString userAgent(const QString &modname="") const;
  QString mysqlHostname() const;
  QString mysqlUsername() const;
  QString mysqlDbname() const;
  QString mysqlPassword() const;
  QString mysqlDriver() const;
  int mysqlHeartbeatInterval() const;
  QString mysqlEngine() const;
  QString createTablePostfix() const;
  bool logXloadDebugData() const;
  bool provisioningCreateHost() const;
  QString provisioningHostTemplate() const;
  QHostAddress provisioningHostIpAddress() const;
  QString provisioningHostShortName(const QString &hostname) const;
  bool provisioningCreateService() const;
  QString provisioningServiceTemplate() const;
  QString provisioningServiceName(const QString &hostname) const;
  int alsaPeriodQuantity() const;
  int alsaPeriodSize() const;
  int alsaChannelsPerPcm() const;
  QString stationName() const;
  QString password() const;
  QString audioOwner() const;
  QString audioGroup() const;
  QString pypadOwner() const;
  QString pypadGroup() const;
  QString rnRmlOwner() const;
  QString rnRmlGroup() const;
  int syslogFacility() const;
  QString audioRoot() const;
  QString audioExtension() const;
  QString audioFileName (QString cutname);
  QString label() const;
  QString audioStoreMountSource() const;
  QString audioStoreMountType() const;
  QString audioStoreMountOptions() const;
  QString audioStoreCaeHostname() const;
  QString audioStoreXportHostname() const;
  QString fontFamily() const;
  int fontButtonSize() const;
  int fontLabelSize() const;
  int fontDefaultSize() const;
  int jackConnections() const;
  QString jackPort(int num,int endpt) const;
  bool disableMaintChecks() const;
  bool lockRdairplayMemory() const;
  int meterBasePort() const;
  int meterPortRange() const;
  bool enableMixerLogging() const;
  uid_t uid() const;
  gid_t gid() const;
  uid_t pypadUid() const;
  gid_t pypadGid() const;
  uid_t rnRmlUid() const;
  gid_t rnRmlGid() const;
  bool useRealtime();
  int realtimePriority();
  int transcodingDelay() const;
  int serviceTimeout() const;
  QString tempDirectory();
  QString sasStation() const;
  int sasMatrix() const;
  unsigned sasBaseCart() const;
  QString sasTtyDevice() const;
  QString destination(unsigned n);
  bool load();
  void clear();
  static QString createTablePostfix(const QString &engine);
  static QString rdselectExitCodeText(RDSelectExitCode code);

 private:
  QString conf_filename;
  QString conf_module_name;
  QString conf_mysql_hostname;
  QString conf_mysql_username;
  QString conf_mysql_dbname;
  QString conf_mysql_password;
  QString conf_mysql_driver;
  QString conf_mysql_engine;
  QString conf_create_table_postfix;
  int conf_mysql_heartbeat_interval;
  bool conf_provisioning_create_host;
  QString conf_provisioning_host_template;
  QHostAddress conf_provisioning_host_ip_address;
  QString conf_provisioning_host_short_name_regex;
  unsigned conf_provisioning_host_short_name_group;
  bool conf_provisioning_create_service;
  QString conf_provisioning_service_template;
  QString conf_provisioning_service_name_regex;
  unsigned conf_provisioning_service_name_group;
  bool conf_log_xload_debug_data;
  int conf_alsa_period_quantity;
  int conf_alsa_period_size;
  int conf_alsa_channels_per_pcm;
  QString conf_station_name;
  QString conf_password;
  QString conf_audio_owner;
  QString conf_audio_group;
  QString conf_pypad_owner;
  QString conf_pypad_group;
  QString conf_rn_rml_owner;
  QString conf_rn_rml_group;
  int conf_syslog_facility;
  QString conf_audio_root;
  QString conf_audio_extension;
  QString conf_label;
  QString conf_audio_store_mount_source;
  QString conf_audio_store_mount_type;
  QString conf_audio_store_mount_options;
  QString conf_audio_store_xport_hostname;
  QString conf_audio_store_cae_hostname;
  QString conf_font_family;
  int conf_font_button_size;
  int conf_font_label_size;
  int conf_font_default_size;
  QString conf_http_user_agent;
  bool conf_disable_maint_checks;
  bool conf_lock_rdairplay_memory;
  int conf_meter_base_port;
  int conf_meter_port_range;
  std::vector<QString> conf_jack_ports[2];
  uid_t conf_uid;
  gid_t conf_gid;
  uid_t conf_pypad_uid;
  gid_t conf_pypad_gid;
  uid_t conf_rn_rml_uid;
  gid_t conf_rn_rml_gid;
  bool conf_enable_mixer_logging;
  bool conf_use_realtime;
  int conf_transcoding_delay;
  int conf_realtime_priority;
  int conf_service_timeout;
  QString conf_temp_directory;
  QString conf_sas_station;
  int conf_sas_matrix;
  unsigned conf_sas_base_cart;
  QString conf_sas_tty_device;
  std::vector<QString> conf_destinations;
};

RDConfig *RDConfiguration(void); 

#endif  // RDCONFIG_H
