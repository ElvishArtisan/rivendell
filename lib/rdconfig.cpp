// rdconfig.cpp
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

#include <grp.h>
#include <pwd.h>
#include <syslog.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <qdatetime.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qregexp.h>
#include <qsettings.h>
#include <qstringlist.h>

#include "rdconfig.h"
#include "rdprofile.h"

RDConfig *RDConfiguration(void) 
{
  static RDConfig *config = NULL;
  if (!config){
    config = new RDConfig();
    config->load();
  }
  return config;
}


RDConfig::RDConfig()
{
  clear();
}


RDConfig::RDConfig(const QString &filename)
{
  clear();
  conf_filename=filename;
}


QString RDConfig::filename() const
{
  return conf_filename;
}


void RDConfig::setFilename(QString filename)
{
  conf_filename=filename;
}


QString RDConfig::moduleName() const
{
  return conf_module_name;
}


void RDConfig::setModuleName(const QString &modname)
{
  conf_module_name=modname;
}


QString RDConfig::userAgent(const QString &modname) const
{
  if(!conf_http_user_agent.isEmpty()) {
    return conf_http_user_agent;
  }
  if(modname.isEmpty()) {
    return QString("Mozilla/5.0")+" rivendell/"+VERSION;
  }
  return QString("Mozilla/5.0 rivendell/")+VERSION+" ("+modname+")";
}


QString RDConfig::audioRoot() const
{
  return conf_audio_root;
}


QString RDConfig::audioExtension() const
{
  return conf_audio_extension;
}


QString RDConfig::audioFileName (QString cutname)
{
  return audioRoot() + QString("/") + cutname + QString(".") +
    audioExtension();
};


QString RDConfig::label() const
{
  return conf_label;
}


QString RDConfig::audioStoreMountSource() const
{
  return conf_audio_store_mount_source;
}


QString RDConfig::audioStoreMountType() const
{
  return conf_audio_store_mount_type;
}


QString RDConfig::audioStoreMountOptions() const
{
  return conf_audio_store_mount_options;
}


QString RDConfig::audioStoreCaeHostname() const
{
  return conf_audio_store_cae_hostname;
}


QString RDConfig::audioStoreXportHostname() const
{
  return conf_audio_store_xport_hostname;
}


QString RDConfig::fontFamily() const
{
  return conf_font_family;
}


int RDConfig::fontButtonSize() const
{
  return conf_font_button_size;
}


int RDConfig::fontLabelSize() const
{
  return conf_font_label_size;
}


int RDConfig::fontDefaultSize() const
{
  return conf_font_default_size;
}


QString RDConfig::mysqlHostname() const
{
  return conf_mysql_hostname;
}

QString RDConfig::mysqlUsername() const
{
  return conf_mysql_username;
}


QString RDConfig::mysqlDbname() const
{
  return conf_mysql_dbname;
}


QString RDConfig::mysqlPassword() const
{
  return conf_mysql_password;
}


QString RDConfig::mysqlDriver() const
{
  return conf_mysql_driver;
}


int RDConfig::mysqlHeartbeatInterval() const
{
  return conf_mysql_heartbeat_interval;
}


QString RDConfig::mysqlEngine() const
{
  return conf_mysql_engine;
}


QString RDConfig::createTablePostfix() const
{
  return conf_create_table_postfix;
}


bool RDConfig::logXloadDebugData() const
{
  return conf_log_xload_debug_data;
}


bool RDConfig::provisioningCreateHost() const
{
  return conf_provisioning_create_host;
}


QString RDConfig::provisioningHostTemplate() const
{
  return conf_provisioning_host_template;
}


QHostAddress RDConfig::provisioningHostIpAddress() const
{
  return conf_provisioning_host_ip_address;
}


QString RDConfig::provisioningHostShortName(const QString &hostname) const
{
  QRegExp exp(conf_provisioning_host_short_name_regex);

  exp.search(hostname);
  QStringList texts=exp.capturedTexts();
  if((unsigned)texts.size()<conf_provisioning_host_short_name_group) {
    return QString();
  }
  return texts[conf_provisioning_host_short_name_group];
}


bool RDConfig::provisioningCreateService() const
{
  return conf_provisioning_create_service;
}


QString RDConfig::provisioningServiceTemplate() const
{
  return conf_provisioning_service_template;
}


QString RDConfig::provisioningServiceName(const QString &hostname) const
{
  QRegExp exp(conf_provisioning_service_name_regex);

  exp.search(hostname);
  QStringList texts=exp.capturedTexts();
  if((unsigned)texts.size()<conf_provisioning_service_name_group) {
    return QString();
  }
  return texts[conf_provisioning_service_name_group];
}


int RDConfig::alsaPeriodQuantity() const
{
  return conf_alsa_period_quantity;
}


int RDConfig::alsaPeriodSize() const
{
  return conf_alsa_period_size;
}


int RDConfig::alsaChannelsPerPcm() const
{
  return conf_alsa_channels_per_pcm;
}


QString RDConfig::stationName() const
{
  return conf_station_name;
}


QString RDConfig::password() const
{
  return conf_password;
}


QString RDConfig::audioOwner() const
{
  return conf_audio_owner;
}


QString RDConfig::audioGroup() const
{
  return conf_audio_group;
}


QString RDConfig::pypadOwner() const
{
  return conf_pypad_owner;
}


QString RDConfig::pypadGroup() const
{
  return conf_pypad_group;
}


QString RDConfig::rnRmlOwner() const
{
  return conf_rn_rml_owner;
}


QString RDConfig::rnRmlGroup() const
{
  return conf_rn_rml_group;
}


int RDConfig::syslogFacility() const
{
  return conf_syslog_facility;
}


int RDConfig::jackConnections() const
{
  return conf_jack_ports[0].size();
}


QString RDConfig::jackPort(int num,int endpt) const
{
  return conf_jack_ports[num][endpt];
}


bool RDConfig::disableMaintChecks() const
{
  return conf_disable_maint_checks;
}


bool RDConfig::lockRdairplayMemory() const
{
  return conf_lock_rdairplay_memory;
}


int RDConfig::meterBasePort() const
{
  return conf_meter_base_port;
}


int RDConfig::meterPortRange() const
{
  return conf_meter_port_range;
}


uid_t RDConfig::uid() const
{
  return conf_uid;
}


gid_t RDConfig::gid() const
{
  return conf_gid;
}


uid_t RDConfig::pypadUid() const
{
  return conf_pypad_uid;
}


gid_t RDConfig::pypadGid() const
{
  return conf_pypad_gid;
}


uid_t RDConfig::rnRmlUid() const
{
  return conf_rn_rml_uid;
}


gid_t RDConfig::rnRmlGid() const
{
  return conf_rn_rml_gid;
}


bool RDConfig::enableMixerLogging() const
{
  return conf_enable_mixer_logging;
}


bool RDConfig::useRealtime()
{
  return conf_use_realtime;
}


int RDConfig::realtimePriority()
{
  return conf_realtime_priority;
}


int RDConfig::transcodingDelay() const
{
  return conf_transcoding_delay;
}


int RDConfig::serviceTimeout() const
{
  return conf_service_timeout;
}


// Don't use this method in application code, use RDTempDirectory()
QString RDConfig::tempDirectory()
{
  return conf_temp_directory;
}

QString RDConfig::sasStation() const
{
  return conf_sas_station;
}


int RDConfig::sasMatrix() const
{
  return conf_sas_matrix;
}


unsigned RDConfig::sasBaseCart() const
{
  return conf_sas_base_cart;
}


QString RDConfig::sasTtyDevice() const
{
  return conf_sas_tty_device;
}


QString RDConfig::destination(unsigned n)
{
  if(n>=conf_destinations.size()) {
    return QString();
  }
  return conf_destinations[n];
}


bool RDConfig::load()
{
  char sname[256];
  QString client;
  QString facility;
  QString iface;
  struct passwd *user;
  struct group *groups;

  RDProfile *profile=new RDProfile();
  if(!profile->setSource(conf_filename)) {
    return false;
  }
  gethostname(sname,255);
  QStringList list=QString(sname).split(".");  // Strip domain name parts
  strncpy(sname,list[0],256);
  conf_station_name=
    profile->stringValue("Identity","StationName",sname);
  conf_password=profile->stringValue("Identity","Password","");
  conf_audio_owner=
    profile->stringValue("Identity","AudioOwner",RD_DEFAULT_AUDIO_OWNER);
  conf_audio_group=
    profile->stringValue("Identity","AudioGroup",RD_DEFAULT_AUDIO_GROUP);
  conf_pypad_owner=
    profile->stringValue("Identity","PypadOwner",RD_DEFAULT_PYPAD_OWNER);
  conf_pypad_group=
    profile->stringValue("Identity","PypadGroup",RD_DEFAULT_PYPAD_GROUP);
  conf_rn_rml_owner=
    profile->stringValue("Identity","RnRmlOwner",RD_DEFAULT_RN_RML_OWNER);
  conf_rn_rml_group=
    profile->stringValue("Identity","RnRmlGroup",RD_DEFAULT_RN_RML_GROUP);
  conf_label=profile->stringValue("Identity","Label",RD_DEFAULT_LABEL);
  conf_http_user_agent=profile->stringValue("Identity","HttpUserAgent");

  conf_audio_store_mount_source=
    profile->stringValue("AudioStore","MountSource");
  conf_audio_store_mount_type=
    profile->stringValue("AudioStore","MountType");
  conf_audio_store_mount_options=
    profile->stringValue("AudioStore","MountOptions",
			 RD_DEFAULT_AUDIO_STORE_MOUNT_OPTIONS);
  conf_audio_store_cae_hostname=
    profile->stringValue("AudioStore","CaeHostname","localhost");
  conf_audio_store_xport_hostname=
    profile->stringValue("AudioStore","XportHostname","localhost");

  conf_font_family=profile->stringValue("Fonts","Family");
  conf_font_button_size=profile->intValue("Fonts","ButtonSize",-1);
  conf_font_label_size=profile->intValue("Fonts","LabelSize",-1);
  conf_font_default_size=profile->intValue("Fonts","DefaultSize",-1);

  conf_provisioning_create_host=
    profile->boolValue("Provisioning","CreateHost");
  conf_provisioning_host_template=
    profile->stringValue("Provisioning","NewHostTemplate");
  iface=profile->stringValue("Provisioning","NewHostIpAddress","lo");
  conf_provisioning_host_short_name_regex=
    profile->stringValue("Provisioning","NewHostShortNameRegex","[^*]*");
  conf_provisioning_host_short_name_group=
    profile->intValue("Provisioning","NewHostShortNameGroup");
  conf_provisioning_create_service=
    profile->boolValue("Provisioning","CreateService");
  conf_provisioning_service_template=
    profile->stringValue("Provisioning","NewServiceTemplate");
  conf_provisioning_service_name_regex=
    profile->stringValue("Provisioning","NewServiceNameRegex","[^*]*");
  conf_provisioning_service_name_group=
    profile->intValue("Provisioning","NewServiceNameGroup");

  conf_audio_root=
    profile->stringValue("Cae","AudioRoot",RD_AUDIO_ROOT);
  conf_audio_extension=
    profile->stringValue("Cae","AudioExtension",RD_AUDIO_EXTENSION);
  conf_mysql_hostname=
    profile->stringValue("mySQL","Hostname",DEFAULT_MYSQL_HOSTNAME);
  conf_mysql_username=
    profile->stringValue("mySQL","Loginname",DEFAULT_MYSQL_USERNAME);
  conf_mysql_dbname=
    profile->stringValue("mySQL","Database",DEFAULT_MYSQL_DATABASE);
  conf_mysql_password=profile->stringValue("mySQL","Password",conf_password);
  conf_mysql_driver=
    profile->stringValue("mySQL","Driver",DEFAULT_MYSQL_DRIVER);
  conf_mysql_heartbeat_interval=
    profile->intValue("mySQL","HeartbeatInterval",
		      DEFAULT_MYSQL_HEARTBEAT_INTERVAL);
  conf_mysql_engine=
    profile->stringValue("mySQL","Engine",DEFAULT_MYSQL_ENGINE);
  conf_create_table_postfix=
    RDConfig::createTablePostfix(conf_mysql_engine);

  conf_log_xload_debug_data=profile->
    boolValue("Logs","LogXloadDebugData",false);

  conf_alsa_period_quantity=
    profile->intValue("Alsa","PeriodQuantity",RD_ALSA_DEFAULT_PERIOD_QUANTITY);
  conf_alsa_period_size=
    profile->intValue("Alsa","PeriodSize",RD_ALSA_DEFAULT_PERIOD_SIZE);
  conf_alsa_channels_per_pcm=profile->intValue("Alsa","ChannelsPerPcm",-1);

  conf_disable_maint_checks=
    profile->boolValue("Hacks","DisableMaintChecks",false);
  conf_lock_rdairplay_memory=
    profile->boolValue("Hacks","LockRdairplayMemory",false);
  conf_meter_base_port=
    profile->intValue("Hacks","MeterPortBaseNumber",RD_DEFAULT_METER_SOCKET_BASE_UDP_PORT);
  conf_meter_port_range=
    profile->intValue("Hacks","MeterPortRange",RD_METER_SOCKET_PORT_RANGE);
  if((user=getpwnam(profile->stringValue("Identity","AudioOwner")))!=NULL) {
    conf_uid=user->pw_uid;
  }
  if((groups=getgrnam(profile->stringValue("Identity","AudioGroup")))!=NULL) {
    conf_gid=groups->gr_gid;
  }
  if((user=getpwnam(profile->stringValue("Identity","PypadOwner",
					 RD_DEFAULT_PYPAD_OWNER)))!=NULL) {
    conf_pypad_uid=user->pw_uid;
  }
  if((groups=getgrnam(profile->stringValue("Identity","PypadGroup",
					   RD_DEFAULT_PYPAD_GROUP)))!=NULL) {
    conf_pypad_gid=groups->gr_gid;
  }

  if((user=getpwnam(profile->stringValue("Identity","RnRmlOwner",
					 RD_DEFAULT_RN_RML_OWNER)))!=NULL) {
    conf_rn_rml_uid=user->pw_uid;
  }
  if((groups=getgrnam(profile->stringValue("Identity","RnRmlGroup",
					   RD_DEFAULT_RN_RML_GROUP)))!=NULL) {
    conf_rn_rml_gid=groups->gr_gid;
  }
  conf_syslog_facility=profile->intValue("Identity","SyslogFacility",LOG_USER);

  conf_enable_mixer_logging=profile->boolValue("Caed","EnableMixerLogging");
  conf_use_realtime=profile->boolValue("Tuning","UseRealtime",false);
  conf_realtime_priority=profile->intValue("Tuning","RealtimePriority",9);
  conf_transcoding_delay=profile->intValue("Tuning","TranscodingDelay");
  conf_service_timeout=
    profile->intValue("Tuning","ServiceTimeout",RD_DEFAULT_SERVICE_TIMEOUT);
  conf_temp_directory=profile->stringValue("Tuning","TempDirectory","");
  conf_sas_station=profile->stringValue("SASFilter","Station","");
  conf_sas_matrix=profile->intValue("SASFilter","Matrix",0);
  conf_sas_base_cart=profile->intValue("SASFilter","BaseCart",0);
  conf_sas_tty_device=profile->stringValue("SASFilter","TtyDevice","");
  int c=1;
  QString dest;
  while(!(dest=profile->
	  stringValue("RDBackup",QString().
		      sprintf("Destination%d",c++),"")).isEmpty()) {
    conf_destinations.push_back(dest);
  }
  delete profile;

  //
  // Resolve Interface Address
  //
  int sock=-1;

  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    return true;
  }
  struct ifreq ifr;
  int index=0;

  memset(&ifr,0,sizeof(ifr));
  index=1;
  ifr.ifr_ifindex=index;
  while(ioctl(sock,SIOCGIFNAME,&ifr)==0) {
    if(ifr.ifr_name==iface) {
      if(ioctl(sock,SIOCGIFADDR,&ifr)==0) {
	struct sockaddr_in sa=*(sockaddr_in *)(&ifr.ifr_addr);
	conf_provisioning_host_ip_address.setAddress(ntohl(sa.sin_addr.s_addr));
      }
    }
    ifr.ifr_ifindex=++index;
  }
  close(sock);

  return true;
}


void RDConfig::clear()
{
  conf_filename=RD_CONF_FILE;
  conf_module_name="";
  conf_mysql_hostname="";
  conf_mysql_username="";
  conf_mysql_dbname="";
  conf_mysql_password="";
  conf_mysql_driver="";
  conf_mysql_heartbeat_interval=DEFAULT_MYSQL_HEARTBEAT_INTERVAL;
  conf_mysql_engine=DEFAULT_MYSQL_ENGINE;
  conf_create_table_postfix="";
  conf_log_xload_debug_data=false;
  conf_provisioning_create_host=false;
  conf_provisioning_host_template="";
  conf_provisioning_host_ip_address.setAddress("127.0.0.2");
  conf_provisioning_host_short_name_regex="[^%]*";
  conf_provisioning_host_short_name_group=0;
  conf_provisioning_create_service=false;
  conf_provisioning_service_template="";
  conf_alsa_period_quantity=RD_ALSA_DEFAULT_PERIOD_QUANTITY;
  conf_alsa_period_size=RD_ALSA_DEFAULT_PERIOD_SIZE;
  conf_alsa_channels_per_pcm=-1;
  conf_station_name="";
  conf_password="";
  conf_http_user_agent="";
  conf_audio_owner="";
  conf_audio_group="";
  conf_pypad_owner="";
  conf_pypad_group="";
  conf_rn_rml_owner="";
  conf_rn_rml_group="";
  conf_syslog_facility=LOG_USER;
  conf_audio_root=RD_AUDIO_ROOT;
  conf_audio_extension=RD_AUDIO_EXTENSION;
  conf_label=RD_DEFAULT_LABEL;
  conf_audio_store_mount_source="";
  conf_audio_store_mount_type="";
  conf_audio_store_mount_options=RD_DEFAULT_AUDIO_STORE_MOUNT_OPTIONS;
  conf_audio_store_xport_hostname="";
  conf_font_family="";
  conf_font_button_size=-1;
  conf_font_label_size=-1;
  conf_font_default_size=-1;
  conf_audio_store_cae_hostname="";
  conf_jack_ports[0].clear();
  conf_jack_ports[1].clear();
  conf_disable_maint_checks=false;
  conf_lock_rdairplay_memory=false;
  conf_meter_base_port=RD_DEFAULT_METER_SOCKET_BASE_UDP_PORT;
  conf_meter_port_range=RD_METER_SOCKET_PORT_RANGE;
  conf_uid=65535;
  conf_gid=65535;
  conf_pypad_uid=65535;
  conf_pypad_gid=65535;
  conf_rn_rml_uid=65535;
  conf_rn_rml_gid=65535;
  conf_enable_mixer_logging=false;
  conf_use_realtime=false;
  conf_realtime_priority=9;
  conf_transcoding_delay=0;
  conf_service_timeout=RD_DEFAULT_SERVICE_TIMEOUT;
  conf_temp_directory="";
  conf_sas_station="";
  conf_sas_matrix=-1;
  conf_sas_base_cart=1;
  conf_sas_tty_device="";
  conf_destinations.clear();
}


QString RDConfig::createTablePostfix(const QString &engine)
{
  return QString(" engine ")+engine+" ";
}


QString RDConfig::rdselectExitCodeText(RDSelectExitCode code)
{
  QString ret=QObject::tr("Unknown error")+QString().sprintf(" [%d]",code);

  switch(code) {
  case RDConfig::RDSelectOk:
    ret=QObject::tr("OK");
    break;

  case RDConfig::RDSelectInvalidArguments:
    ret=QObject::tr("Invalid arguments specified");
    break;

  case RDConfig::RDSelectNoSuchConfiguration:
    ret=QObject::tr("Specified configuration was not found");
    break;

  case RDConfig::RDSelectModulesActive:
    ret=QObject::tr("One or more Rivendell modules are active");
    break;

  case RDConfig::RDSelectNotRoot:
    ret=QObject::tr("No running as root");
    break;

  case RDConfig::RDSelectSystemctlCrashed:
    ret=QObject::tr("systemctl(8) crashed");
    break;

  case RDConfig::RDSelectRivendellShutdownFailed:
    ret=QObject::tr("Rivendell service shutdown failed");
    break;

  case RDConfig::RDSelectAudioUnmountFailed:
    ret=QObject::tr("Audio store unmount failed");
    break;

  case RDConfig::RDSelectAudioMountFailed:
    ret=QObject::tr("Audio store mount failed");
    break;

  case RDConfig::RDSelectRivendellStartupFailed:
    ret=QObject::tr("Rivendell service startup failed");
    break;

  case RDConfig::RDSelectNoCurrentConfig:
    ret=QObject::tr("Current configuration was not found");
    break;

  case RDConfig::RDSelectSymlinkFailed:
    ret=QObject::tr("Synlink creation failed");
    break;

  case RDConfig::RDSelectInvalidName:
    ret=QObject::tr("Invalid configuration name");
    break;

  case RDConfig::RDSelectMountCrashed:
    ret=QObject::tr("mount(8) crashed");
    break;

  case RDConfig::RDSelectCantAccessAutomount:
    ret=QObject::tr("Unable to update automounter configuration");
    break;

  case RDConfig::RDSelectLast:
    break;
  }

  return ret;
}
