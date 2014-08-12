// rdstation.cpp
//
// Abstract a Rivendell Workstation.
//
//   (C) Copyright 2002-2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdstation.cpp,v 1.30.4.8 2014/02/11 23:46:26 cvs Exp $
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

#include <rdconf.h>
#include <rdstation.h>
#include <rd.h>
#include <rddb.h> 
#include <rdescape_string.h>

//
// Global Classes
//
RDStation::RDStation(const QString &name,bool create)
{
  QString sql;
  time_offset_valid = false;
  station_name=name;
}


RDStation::~RDStation()
{
//  printf("Destroying RDStation\n");
}


bool RDStation::exists() const
{
  return RDDoesRowExist("STATIONS","NAME",RDEscapeString(station_name));
}


QString RDStation::name() const
{
  return station_name;
}


QString RDStation::description() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"DESCRIPTION").toString();
}


void RDStation::setDescription(QString desc) const
{
  SetRow("DESCRIPTION",desc);
}


QString RDStation::userName() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"USER_NAME").toString();
}


void RDStation::setUserName(QString name) const
{
  SetRow("USER_NAME",name);
}


QString RDStation::defaultName() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"DEFAULT_NAME").
    toString();
}


void RDStation::setDefaultName(QString name) const
{
  SetRow("DEFAULT_NAME",name);
}


QHostAddress RDStation::address() const
{
  QHostAddress addr;
  addr.setAddress(RDGetSqlValue("STATIONS","NAME",station_name,"IPV4_ADDRESS").
		  toString());
  return addr;
}


void RDStation::setAddress(QHostAddress addr) const
{
  SetRow("IPV4_ADDRESS",addr.toString());
}


QHostAddress RDStation::httpAddress(RDConfig *config) const
{
  QHostAddress addr;
  
  addr.setAddress("127.0.0.1");
  if(httpStation()!="localhost") {
    if(httpStation()==RD_RDSELECT_LABEL) {
      addr.setAddress(config->audioStoreXportHostname());
    }
    else {
      addr.setAddress(RDGetSqlValue("STATIONS","NAME",httpStation(),
				    "IPV4_ADDRESS").toString());
    }
  }

  return addr;
}


QString RDStation::httpStation() const
{
  return
    RDGetSqlValue("STATIONS","NAME",station_name,"HTTP_STATION").toString();
}


void RDStation::setHttpStation(const QString &str)
{
  SetRow("HTTP_STATION",str);
}


QString RDStation::caeStation() const
{
  return
    RDGetSqlValue("STATIONS","NAME",station_name,"CAE_STATION").toString();
}


QHostAddress RDStation::caeAddress(RDConfig *config) const
{
  QHostAddress addr;

  addr.setAddress("127.0.0.1");
  if(caeStation()!="localhost") {
    if(caeStation()==RD_RDSELECT_LABEL) {
      addr.setAddress(config->audioStoreCaeHostname());
    }
    else {
      addr.setAddress(RDGetSqlValue("STATIONS","NAME",caeStation(),
				    "IPV4_ADDRESS").toString());
    }
  }

  return addr;
}


void RDStation::setCaeStation(const QString &str)
{
  SetRow("CAE_STATION",str);
}


QString RDStation::webServiceUrl(RDConfig *config) const
{
  return QString("http://")+httpAddress(config).toString()+
    "/rd-bin/rdxport.cgi";
}


int RDStation::timeOffset()
{
  if (!time_offset_valid){
    time_offset = RDGetSqlValue("STATIONS","NAME",station_name,"TIME_OFFSET").toInt();
    time_offset_valid = true;
  }
  return time_offset;
}


void RDStation::setTimeOffset(int msecs)
{
  SetRow("TIME_OFFSET",msecs);
  time_offset = msecs;
}


QString RDStation::backupPath() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"BACKUP_DIR").toString();
}


void RDStation::setBackupPath(QString path) const
{
  SetRow("BACKUP_DIR",path);
}


int RDStation::backupLife() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"BACKUP_LIFE").toInt();
}


void RDStation::setBackupLife(int days) const
{
  SetRow("BACKUP_LIFE",days);
}


void RDStation::setBroadcastSecurity(BroadcastSecurityMode mode)
{
  SetRow("BROADCAST_SECURITY",mode);
}


RDStation::BroadcastSecurityMode RDStation::broadcastSecurity() const
{
  return (RDStation::BroadcastSecurityMode)RDGetSqlValue("STATIONS","NAME",
                                                         station_name,
                                                         "BROADCAST_SECURITY").
                                                         toUInt();
}


unsigned RDStation::heartbeatCart() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"HEARTBEAT_CART").
    toUInt();
}


void RDStation::setHeartbeatCart(unsigned cartnum) const
{
  SetRow("HEARTBEAT_CART",cartnum);
}


unsigned RDStation::heartbeatInterval() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"HEARTBEAT_INTERVAL").
    toUInt();
}


void RDStation::setHeartbeatInterval(unsigned interval) const
{
  SetRow("HEARTBEAT_INTERVAL",interval);
}


unsigned RDStation::startupCart() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"STARTUP_CART").
    toUInt();
}


void RDStation::setStartupCart(unsigned cartnum) const
{
  SetRow("STARTUP_CART",cartnum);
}


QString RDStation::editorPath() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"EDITOR_PATH").
    toString();
}


void RDStation::setEditorPath(const QString &cmd)
{
  SetRow("EDITOR_PATH",cmd);
}


RDStation::FilterMode RDStation::filterMode() const
{
  return (RDStation::FilterMode)RDGetSqlValue("STATIONS","NAME",station_name,
					      "FILTER_MODE").toInt();
}


void RDStation::setFilterMode(RDStation::FilterMode mode) const
{
  SetRow("FILTER_MODE",(int)mode);
}


bool RDStation::startJack() const
{
  return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,"START_JACK").
		toString());
}


void RDStation::setStartJack(bool state) const
{
  SetRow("START_JACK",RDYesNo(state));
}


QString RDStation::jackServerName() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"JACK_SERVER_NAME").
    toString();
}


void RDStation::setJackServerName(const QString &str) const
{
  SetRow("JACK_SERVER_NAME",str);
}


QString RDStation::jackCommandLine() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"JACK_COMMAND_LINE").
    toString();
}


void RDStation::setJackCommandLine(const QString &str) const
{
  SetRow("JACK_COMMAND_LINE",str);
}


int RDStation::cueCard() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"CUE_CARD").toInt();
}


void RDStation::setCueCard(int card)
{
  SetRow("CUE_CARD",card);
}


int RDStation::cuePort() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"CUE_PORT").toInt();
}


void RDStation::setCuePort(int port)
{
  SetRow("CUE_PORT",port);
}


unsigned RDStation::cueStartCart() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"CUE_START_CART").
    toUInt();
}


void RDStation::setCueStartCart(unsigned cartnum) const
{
  SetRow("CUE_START_CART",cartnum);
}


unsigned RDStation::cueStopCart() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"CUE_STOP_CART").toUInt();
}


void RDStation::setCueStopCart(unsigned cartnum) const
{
  SetRow("CUE_STOP_CART",cartnum);
}


int RDStation::cartSlotColumns() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"CARTSLOT_COLUMNS").
    toInt();
}


void RDStation::setCartSlotColumns(int cols)
{
  SetRow("CARTSLOT_COLUMNS",cols);
}


int RDStation::cartSlotRows() const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,"CARTSLOT_ROWS").toInt();
}


void RDStation::setCartSlotRows(int rows)
{
  SetRow("CARTSLOT_ROWS",rows);
}


bool RDStation::enableDragdrop() const
{
  return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
			      "ENABLE_DRAGDROP").toString());
}


void RDStation::setEnableDragdrop(bool state)
{
  SetRow("ENABLE_DRAGDROP",state);
}


bool RDStation::enforcePanelSetup() const
{
  return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
			      "ENFORCE_PANEL_SETUP").toString());
}


void RDStation::setEnforcePanelSetup(bool state)
{
  SetRow("ENFORCE_PANEL_SETUP",state);
}


bool RDStation::systemMaint() const
{
  return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,"SYSTEM_MAINT").
	       toString());
}


void RDStation::setSystemMaint(bool state) const
{
  SetRow("SYSTEM_MAINT",state);
}


bool RDStation::scanned() const
{
  return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,"STATION_SCANNED").
	       toString());
}


void RDStation::setScanned(bool state) const
{
  SetRow("STATION_SCANNED",state);
}


bool RDStation::haveCapability(Capability cap) const
{
  switch(cap) {
      case RDStation::HaveOggenc:
	return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
				  "HAVE_OGGENC").toString());
	break;

      case RDStation::HaveOgg123:
	return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
				  "HAVE_OGG123").toString());
	break;

      case RDStation::HaveFlac:
	return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
				  "HAVE_FLAC").toString());
	break;

      case RDStation::HaveLame:
	return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
				  "HAVE_LAME").toString());
	break;

      case RDStation::HaveMpg321:
	return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
				  "HAVE_MPG321").toString());

      case RDStation::HaveTwoLame:
	return RDBool(RDGetSqlValue("STATIONS","NAME",station_name,
				  "HAVE_TWOLAME").toString());
	break;
  }
  return false;
}


void RDStation::setHaveCapability(Capability cap,bool state) const
{
  switch(cap) {
      case RDStation::HaveOggenc:
	SetRow("HAVE_OGGENC",state);
	break;

      case RDStation::HaveOgg123:
	SetRow("HAVE_OGG123",state);
	break;

      case RDStation::HaveFlac:
	SetRow("HAVE_FLAC",state);
	break;

      case RDStation::HaveLame:
	SetRow("HAVE_LAME",state);
	break;

      case RDStation::HaveMpg321:
	SetRow("HAVE_MPG321",state);
	break;

      case RDStation::HaveTwoLame:
	SetRow("HAVE_TWOLAME",state);
	break;
  }
}


int RDStation::cards() const
{
  int n=0;

  QString sql=QString().sprintf("select CARD0_DRIVER,CARD1_DRIVER,\
                                 CARD2_DRIVER,CARD3_DRIVER,CARD4_DRIVER,\
                                 CARD5_DRIVER,CARD6_DRIVER,CARD7_DRIVER\
                                 from STATIONS where NAME=\"%s\"",
				(const char *)station_name);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    for(int i=0;i<RD_MAX_CARDS;i++) {
      if((RDStation::AudioDriver)q->value(i).toInt()!=RDStation::None) {
	n++;
      }
      else {
	delete q;
	return n;
      }
    }
  }
  delete q;

  return n;
}


RDStation::AudioDriver RDStation::cardDriver(int cardnum) const
{
  return (RDStation::AudioDriver)
    RDGetSqlValue("STATIONS","NAME",station_name,
		 QString().sprintf("CARD%d_DRIVER",cardnum)).
    toInt();
}


void RDStation::setCardDriver(int cardnum,AudioDriver driver) const
{
  SetRow(QString().sprintf("CARD%d_DRIVER",cardnum),(int)driver);
}


QString RDStation::driverVersion(AudioDriver driver) const
{
  switch(driver) {
      case RDStation::None:
	return QString();

      case RDStation::Hpi:
	return RDGetSqlValue("STATIONS","NAME",station_name,"HPI_VERSION").
	  toString();

      case RDStation::Jack:
	return RDGetSqlValue("STATIONS","NAME",station_name,"JACK_VERSION").
	  toString();

      case RDStation::Alsa:
	return RDGetSqlValue("STATIONS","NAME",station_name,"ALSA_VERSION").
	  toString();
  }
  return QString();
}


void RDStation::setDriverVersion(AudioDriver driver,QString ver) const
{
  switch(driver) {
      case RDStation::None:
	break;

      case RDStation::Hpi:
	SetRow("HPI_VERSION",ver);
	break;

      case RDStation::Jack:
	SetRow("JACK_VERSION",ver);
	break;

      case RDStation::Alsa:
	SetRow("ALSA_VERSION",ver);
	break;
  }
}


QString RDStation::cardName(int cardnum) const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,
		      QString().sprintf("CARD%d_NAME",cardnum)).toString();
}


void RDStation::setCardName(int cardnum,QString name) const
{
  SetRow(QString().sprintf("CARD%d_NAME",cardnum),name);
}


int RDStation::cardInputs(int cardnum) const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,
		      QString().sprintf("CARD%d_INPUTS",cardnum)).toInt();
}


void RDStation::setCardInputs(int cardnum,int inputs) const
{
  SetRow(QString().sprintf("CARD%d_INPUTS",cardnum),inputs);
}


int RDStation::cardOutputs(int cardnum) const
{
  return RDGetSqlValue("STATIONS","NAME",station_name,
		      QString().sprintf("CARD%d_OUTPUTS",cardnum)).toInt();
}


void RDStation::setCardOutputs(int cardnum,int outputs) const
{
  SetRow(QString().sprintf("CARD%d_OUTPUTS",cardnum),outputs);
}


void RDStation::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE STATIONS SET %s=\"%s\" WHERE NAME=\"%s\"",
			(const char *)param,
			(const char *)RDEscapeString(value),
			(const char *)RDEscapeString(station_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDStation::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE STATIONS SET %s=%d WHERE NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(station_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDStation::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE STATIONS SET %s=%u WHERE NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)RDEscapeString(station_name));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDStation::SetRow(const QString &param,bool value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE STATIONS SET %s=\"%s\" WHERE NAME=\"%s\"",
			(const char *)param,
			(const char *)RDYesNo(value),
			(const char *)RDEscapeString(station_name));
  q=new RDSqlQuery(sql);
  delete q;
}
