// schemamap.cpp
//
// DB schema version <==> Rivendell version map
//
//   (C) Copyright 2018-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include "rddbmgr.h"

class VersionString : public QString
{
 public:
  VersionString();
  VersionString(const char *str);
  int major() const;
  int minor() const;
  int point() const;
  bool operator<(const VersionString &rhs) const;
  bool operator==(const VersionString &rhs) const;

private:
  int ver_major;
  int ver_minor;
  int ver_point;
};


VersionString::VersionString()
  : QString()
{
  ver_major=0;
  ver_minor=0;
  ver_point=0;
}


VersionString::VersionString(const char *str)
  : QString(str)
{
  QStringList f0=QString(str).split(".",Qt::KeepEmptyParts);
  ver_major=f0[0].toInt();
  if(f0.size()>=2) {
    ver_minor=f0[1].toInt();
    if(f0.size()>=3) {
      ver_point=f0[2].toInt();
    }
  }
}


int VersionString::major() const
{
  return ver_major;
}


int VersionString::minor() const
{
  return ver_minor;
}


int VersionString::point() const
{
  return ver_point;
}


bool VersionString::operator<(const VersionString &rhs) const
{
  if(major()<rhs.major()) {
    return true;
  }
  if(major()>rhs.major()) {
    return false;
  }
  if(minor()<rhs.minor()) {
    return true;
  }
  if(minor()>rhs.minor()) {
    return false;
  }
  return false;
}


bool VersionString::operator==(const VersionString &rhs) const
{
  return (major()==rhs.major())&&(minor()==rhs.minor());
}


//
// Version -> Schema Map
//
QMap<VersionString,int> global_version_map;

void MainObject::InitializeSchemaMap() {
  //
  // Maintainer's Note
  //
  // With the (possible) exception of the very last entry, the items in
  // this mapping should be considered immutable. The value of the last
  // entry should be either that of the current production minor release
  // (if no subsequent schema updates have been made), or that of the *next*
  // (upcoming) release. If this is the first schema change made after a
  // public release, a line referencing the next (upcoming) release should
  // be added and its value set to that of the new schema version. Otherwise,
  // it should be incremented every time a schema update is committed.
  //
  // When the first production release in the minor version referenced
  // on the last line occurs, that line too is declared immutable, and
  // the cycle begins again.
  //
  // When updating this map, it is also crucial that the canonical set
  // of database tables be updated in 'GetCanonicalTables()' method.
  //
  global_version_map["1.0"]=159;
  global_version_map["1.1"]=162;
  global_version_map["1.2"]=169;
  global_version_map["1.3"]=173;
  global_version_map["1.4"]=177;
  global_version_map["1.5"]=179;
  global_version_map["1.6"]=182;
  global_version_map["1.7"]=186;
  global_version_map["2.0"]=202;
  global_version_map["2.1"]=205;
  global_version_map["2.2"]=207;
  global_version_map["2.3"]=213;
  global_version_map["2.4"]=216;
  global_version_map["2.5"]=220;
  global_version_map["2.6"]=224;
  global_version_map["2.7"]=231;
  global_version_map["2.8"]=234;
  global_version_map["2.9"]=239;
  global_version_map["2.10"]=242;
  global_version_map["2.11"]=245;
  global_version_map["2.12"]=254;
  global_version_map["2.13"]=255;
  global_version_map["2.14"]=258;
  global_version_map["2.15"]=259;
  global_version_map["2.16"]=263;
  global_version_map["2.17"]=268;
  global_version_map["2.18"]=272;
  global_version_map["2.19"]=275;
  global_version_map["3.0"]=308;
  global_version_map["3.1"]=310;
  global_version_map["3.2"]=311;
  global_version_map["3.3"]=314;
  global_version_map["3.4"]=317;
  global_version_map["3.5"]=346;
  global_version_map["3.6"]=347;
  global_version_map["4.0"]=370;
  global_version_map["4.1"]=371;
  global_version_map["4.2"]=374;
  global_version_map["4.3"]=375;
  global_version_map["4.4"]=376;
}


QStringList MainObject::GetCanonicalTables(int schema) const
{
  QStringList tables;

  switch(schema) {
  case 275:   // v2.19.x
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUDIO_PORTS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ENCODER_CHANNELS");
    tables.push_back("ENCODER_BITRATES");
    tables.push_back("ENCODER_SAMPLERATES");
    tables.push_back("ENCODERS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("OUTPUTS");
    tables.push_back("NOWNEXT_PLUGINS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STATIONS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    tables.push_back("*_CLK");
    tables.push_back("*_FLG");
    tables.push_back("*_LOG");
    tables.push_back("*_PRE");
    tables.push_back("*_POST");
    tables.push_back("*_RULES");
    tables.push_back("*_SRT");
    tables.push_back("*_STACK");
    break;

  case 308:   // v3.0.x
    tables.push_back("AUDIO_CARDS");
    tables.push_back("AUDIO_INPUTS");
    tables.push_back("AUDIO_OUTPUTS");
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CART_SCHED_CODES");
    tables.push_back("CAST_DOWNLOADS");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_LINES");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ELR_LINES");
    tables.push_back("ENCODER_CHANNELS");
    tables.push_back("ENCODER_BITRATES");
    tables.push_back("ENCODER_SAMPLERATES");
    tables.push_back("ENCODERS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_LINES");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORTER_LINES");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_LINES");
    tables.push_back("LOG_MACHINES");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("OUTPUTS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("PYPAD_INSTANCES");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("RULE_LINES");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STACK_LINES");
    tables.push_back("STACK_SCHED_CODES");
    tables.push_back("STATIONS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    break;

  case 310:   // v3.1.x
    tables.push_back("AUDIO_CARDS");
    tables.push_back("AUDIO_INPUTS");
    tables.push_back("AUDIO_OUTPUTS");
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CART_SCHED_CODES");
    tables.push_back("CAST_DOWNLOADS");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_LINES");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ELR_LINES");
    tables.push_back("ENCODER_CHANNELS");
    tables.push_back("ENCODER_BITRATES");
    tables.push_back("ENCODER_SAMPLERATES");
    tables.push_back("ENCODERS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_LINES");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORTER_LINES");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_LINES");
    tables.push_back("LOG_MACHINES");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("NEXUS_FIELDS");
    tables.push_back("NEXUS_QUEUE");
    tables.push_back("NEXUS_SERVER");
    tables.push_back("NEXUS_STATIONS");
    tables.push_back("OUTPUTS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("PYPAD_INSTANCES");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("RULE_LINES");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STACK_LINES");
    tables.push_back("STACK_SCHED_CODES");
    tables.push_back("STATIONS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    break;

  case 375:   // v3.2.x
  case 376:   // v3.3.x
    tables.push_back("AUDIO_CARDS");
    tables.push_back("AUDIO_INPUTS");
    tables.push_back("AUDIO_OUTPUTS");
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CART_SCHED_CODES");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_LINES");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ELR_LINES");
    tables.push_back("ENCODER_PRESETS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_LINES");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_IMAGES");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORTER_LINES");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_LINES");
    tables.push_back("LOG_MACHINES");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("NEXUS_FIELDS");
    tables.push_back("NEXUS_QUEUE");
    tables.push_back("NEXUS_SERVER");
    tables.push_back("NEXUS_STATIONS");
    tables.push_back("OUTPUTS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("PYPAD_INSTANCES");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("RULE_LINES");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STACK_LINES");
    tables.push_back("STACK_SCHED_CODES");
    tables.push_back("STATIONS");
    tables.push_back("SUPERFEED_MAPS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    break;
  }

  return tables;
}


int MainObject::GetVersionSchema(const QString &ver) const
{
  QString version=ver;
  bool ok=false;

  //
  // Normalize String
  //
  if(version.left(1).toLower()=="v") {
    version=version.right(version.length()-1);
  }
  QStringList f0=version.split(".",Qt::KeepEmptyParts);
  if(f0.size()!=3) {
    return 0;
  }
  for(int i=0;i<3;i++) {
    f0[i].toInt(&ok);
    if(!ok) {
      return 0;
    }
  }

  //
  // Lookup Schema
  //
  if(global_version_map.count(VersionString((f0[0]+"."+f0[1]).toUtf8()))==0) {
    return 0;
  }

  return global_version_map.value(VersionString((f0[0]+"."+f0[1]).toUtf8()));
}


QString MainObject::GetSchemaVersion(int schema) const
{
  QString prev_version="_preproduction";

  for(QMap<VersionString,int>::const_iterator it=global_version_map.begin();
      it!=global_version_map.end();it++) {
    if(it.value()==schema) {
      return "v"+it.key()+".x";
    }
    if(it.value()>schema) {
      return tr("between")+" v"+prev_version+".x "+tr("and")+
	" v"+it.key()+".x";
    } 
    prev_version=it.key();
  }
  return QString("after")+" v"+prev_version+".x";
}
