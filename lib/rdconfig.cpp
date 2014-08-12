// rdconfig.cpp
//
// A container class for a Rivendell Base Configuration
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdconfig.cpp,v 1.24.6.7 2013/11/13 23:36:32 cvs Exp $
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

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#endif

#include <qmessagebox.h>
#include <qsettings.h>
#include <qstringlist.h>

#include <rdprofile.h>
#include <rdconfig.h>


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


RDConfig::RDConfig(QString filename)
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


RDConfig::LogFacility RDConfig::logFacility() const
{
  return conf_log_facility;
}


QString RDConfig::logDirectory() const
{
  return conf_log_directory;
}


QString RDConfig::logCoreDumpDirectory() const
{
  return conf_log_core_dump_directory;
}


QString RDConfig::logPattern() const
{
  return conf_log_pattern;
}


bool RDConfig::logXloadDebugData() const
{
  return conf_log_xload_debug_data;
}


void RDConfig::log(const QString &module,LogPriority prio,const QString &msg)
{
#ifndef WIN32
  QDateTime dt;
  QString filename;
  FILE *f=NULL;

  switch(conf_log_facility) {
    case RDConfig::LogSyslog:
      syslog((int)prio,"%s",(const char *)msg);
      break;

    case RDConfig::LogFile:
      if(conf_log_directory.isEmpty()||conf_log_pattern.isEmpty()) {
	return;
      }
      filename=QString().sprintf("%s/%s",(const char *)conf_log_directory,
				 (const char *)conf_log_pattern);
      dt=QDateTime(QDate::currentDate(),QTime::currentTime());
      filename.replace("%n",module);
      filename.replace("%d",dt.date().toString("dd"));
      filename.replace("%M",dt.date().toString("MM"));
      filename.replace("%Y",dt.date().toString("yyyy"));
      filename.replace("%h",dt.time().toString("hh"));
      filename.replace("%m",dt.time().toString("mm"));
      filename.replace("%s",dt.time().toString("ss"));
      if((f=fopen(filename,"a"))!=NULL) {
	fprintf(f,"%s: %s\n",(const char *)dt.
		toString("dd/MM/yyyy - hh:mm:ss.zzz "),
		(const char *)msg);
	fclose(f);
      }
      chmod(filename,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
      chown(filename,uid(),gid());
      break;

    case RDConfig::LogNone:
      break;
  }
#endif  // WIN32
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


QString RDConfig::ripcdLogname() const
{
  return conf_ripcd_logname;
}


QString RDConfig::airplayLogname() const
{
  return conf_airplay_logname;
}


QString RDConfig::catchdLogname() const
{
  return conf_catchd_logname;
}


int RDConfig::jackConnections() const
{
  return conf_jack_ports[0].size();
}


QString RDConfig::jackPort(int num,int endpt) const
{
  return conf_jack_ports[num][endpt];
}


bool RDConfig::useStreamMeters() const
{
  return conf_use_stream_meters;
}


bool RDConfig::disableMaintChecks() const
{
  return conf_disable_maint_checks;
}


unsigned RDConfig::channels() const
{
  return conf_channels;
}


#ifndef WIN32
uid_t RDConfig::uid() const
{
  return conf_uid;
}


gid_t RDConfig::gid() const
{
  return conf_gid;
}
#endif


QString RDConfig::caeLogfile() const
{
  return conf_cae_logfile;
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


void RDConfig::load()
{
  char sname[256];
  QString client;
  QString facility;
#ifndef WIN32
  struct passwd *user;
  struct group *groups;
#endif 

  RDProfile *profile=new RDProfile();
  profile->setSource(conf_filename);
#ifdef WIN32
  strcpy(sname,"windows");
#else
  gethostname(sname,255);
  QStringList list=list.split(".",sname);  // Strip domain name parts
  strncpy(sname,list[0],256);
#endif
  conf_station_name=
    profile->stringValue("Identity","StationName",sname);
  conf_password=profile->stringValue("Identity","Password","");
  conf_audio_owner=
    profile->stringValue("Identity","AudioOwner",RD_DEFAULT_AUDIO_OWNER);
  conf_audio_group=
    profile->stringValue("Identity","AudioGroup",RD_DEFAULT_AUDIO_GROUP);
  conf_label=profile->stringValue("Identity","Label",RD_DEFAULT_LABEL);

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

  facility=profile->stringValue("Logs","Facility",DEFAULT_LOG_FACILITY).lower();
  if(facility=="syslog") {
    conf_log_facility=RDConfig::LogSyslog;
  }
  if(facility=="file") {
    conf_log_facility=RDConfig::LogFile;
  }
  conf_log_directory=profile->stringValue("Logs","LogDirectory","");
  conf_log_pattern=profile->stringValue("Logs","LogPattern",
					DEFAULT_LOG_PATTERN);
  conf_log_xload_debug_data=profile->
    boolValue("Logs","LogXloadDebugData",false);
  conf_log_core_dump_directory=
    profile->stringValue("Logs","CoreDumpDirectory",
			 DEFAULT_LOG_CORE_DUMP_DIRECTORY);

  conf_alsa_period_quantity=
    profile->intValue("Alsa","PeriodQuantity",RD_ALSA_DEFAULT_PERIOD_QUANTITY);
  conf_alsa_period_size=
    profile->intValue("Alsa","PeriodSize",RD_ALSA_DEFAULT_PERIOD_SIZE);
  conf_alsa_channels_per_pcm=profile->intValue("Alsa","ChannelsPerPcm",-1);
  conf_ripcd_logname=profile->stringValue("Ripcd","Logfile","");
  conf_airplay_logname=profile->stringValue("RDAirPlay","Logfile","");
  conf_catchd_logname=profile->stringValue("RDCatchd","Logfile","");
  conf_use_stream_meters=profile->boolValue("Hacks","UseStreamMeters",false);
  conf_disable_maint_checks=
    profile->boolValue("Hacks","DisableMaintChecks",false);
  conf_channels=profile->intValue("Format","Channels",RD_DEFAULT_CHANNELS);
#ifndef WIN32
  if((user=getpwnam(profile->stringValue("Identity","AudioOwner")))!=NULL) {
    conf_uid=user->pw_uid;
  }
  if((groups=getgrnam(profile->stringValue("Identity","AudioGroup")))!=NULL) {
    conf_gid=groups->gr_gid;
  }
#endif
  conf_cae_logfile=profile->stringValue("Caed","Logfile","");
  conf_enable_mixer_logging=profile->boolValue("Caed","EnableMixerLogging");
  conf_use_realtime=profile->boolValue("Tuning","UseRealtime",false);
  conf_realtime_priority=profile->intValue("Tuning","RealtimePriority",9);
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
}


void RDConfig::clear()
{
#ifdef WIN32
  QSettings settings;
  settings.insertSearchPath(QSettings::Windows,"/SalemRadioLabs");
  conf_filename=QString().sprintf("%s\\%s",
				  (const char *)settings.
				  readEntry("/Rivendell/InstallDir"),
				  (const char *)RD_WIN_CONF_FILE);  
#else
  conf_filename=RD_CONF_FILE;
#endif
  conf_mysql_hostname="";
  conf_mysql_username="";
  conf_mysql_dbname="";
  conf_mysql_password="";
  conf_mysql_driver="";
  conf_mysql_heartbeat_interval=DEFAULT_MYSQL_HEARTBEAT_INTERVAL;
  conf_log_facility=RDConfig::LogSyslog;
  conf_log_directory="";
  conf_log_core_dump_directory=DEFAULT_LOG_CORE_DUMP_DIRECTORY;
  conf_log_pattern=DEFAULT_LOG_PATTERN;
  conf_log_xload_debug_data=false;
  conf_alsa_period_quantity=RD_ALSA_DEFAULT_PERIOD_QUANTITY;
  conf_alsa_period_size=RD_ALSA_DEFAULT_PERIOD_SIZE;
  conf_alsa_channels_per_pcm=-1;
  conf_station_name="";
  conf_password="";
  conf_audio_owner="";
  conf_audio_group="";
  conf_audio_root=RD_AUDIO_ROOT;
  conf_audio_extension=RD_AUDIO_EXTENSION;
  conf_label=RD_DEFAULT_LABEL;
  conf_audio_store_mount_source="";
  conf_audio_store_mount_type="";
  conf_audio_store_mount_options=RD_DEFAULT_AUDIO_STORE_MOUNT_OPTIONS;
  conf_audio_store_xport_hostname="";
  conf_audio_store_cae_hostname="";
  conf_ripcd_logname="";
  conf_airplay_logname="";
  conf_catchd_logname="";
  conf_jack_ports[0].clear();
  conf_jack_ports[1].clear();
  conf_use_stream_meters=false;
  conf_disable_maint_checks=false;
  conf_channels=RD_DEFAULT_CHANNELS;
#ifndef WIN32
  conf_uid=65535;
  conf_gid=65535;
#endif
  conf_cae_logfile="";
  conf_enable_mixer_logging=false;
  conf_use_realtime=false;
  conf_realtime_priority=9;
  conf_sas_station="";
  conf_sas_matrix=-1;
  conf_sas_base_cart=1;
  conf_sas_tty_device="";
  conf_destinations.clear();
}
