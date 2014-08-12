// rdairplay_conf.cpp
//
// Abstract an RDAirPlay Configuration.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdairplay_conf.cpp,v 1.35.8.6 2014/02/10 20:45:09 cvs Exp $
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

#include <qobject.h>

#include <rddb.h>
#include <rdconf.h>
#include <rdairplay_conf.h>
#include <rdescape_string.h>

RDAirPlayConf::RDAirPlayConf(const QString &station,const QString &tablename)
{
  RDSqlQuery *q;
  QString sql;

  air_station=station;
  air_tablename=tablename;

  sql=QString().
    sprintf("select ID from %s where STATION=\"%s\"",
	    (const char *)air_tablename,
	    (const char *)RDEscapeString(air_station));
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    sql=QString().
      sprintf("insert into %s set STATION=\"%s\"",
	      (const char *)air_tablename,
	      (const char *)RDEscapeString(air_station));
    q=new RDSqlQuery(sql);
    delete q;
    sql=QString().
      sprintf("select ID from %s where STATION=\"%s\"",
	      (const char *)air_tablename,
	      (const char *)RDEscapeString(air_station));
    q=new RDSqlQuery(sql);
    q->first();
  }
  air_id=q->value(0).toUInt();
  delete q;
}


QString RDAirPlayConf::station() const
{
  return air_station;
}


int RDAirPlayConf::card(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("CARD",chan).toInt();
}


void RDAirPlayConf::setCard(RDAirPlayConf::Channel chan,int card) const
{
  SetChannelValue("CARD",chan,card);
}


int RDAirPlayConf::port(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("PORT",chan).toInt();
}


void RDAirPlayConf::setPort(RDAirPlayConf::Channel chan,int port) const
{
  SetChannelValue("PORT",chan,port);
}


QString RDAirPlayConf::startRml(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("START_RML",chan).toString();
}


void RDAirPlayConf::setStartRml(RDAirPlayConf::Channel chan,QString str) const
{
  SetChannelValue("START_RML",chan,str);
}


QString RDAirPlayConf::stopRml(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("STOP_RML",chan).toString();
}


void RDAirPlayConf::setStopRml(RDAirPlayConf::Channel chan,QString str) const
{
  SetChannelValue("STOP_RML",chan,str);
}


RDAirPlayConf::GpioType RDAirPlayConf::gpioType(RDAirPlayConf::Channel chan)
  const
{
  return (RDAirPlayConf::GpioType)GetChannelValue("GPIO_TYPE",chan).toUInt();
}


void RDAirPlayConf::setGpioType(RDAirPlayConf::Channel chan,GpioType type) 
  const
{
  SetChannelValue("GPIO_TYPE",chan,(int)type);
}


int RDAirPlayConf::startGpiMatrix(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("START_GPI_MATRIX",chan).toInt();
}


void RDAirPlayConf::setStartGpiMatrix(RDAirPlayConf::Channel chan,int matrix) const
{
  SetChannelValue("START_GPI_MATRIX",chan,matrix);
}


int RDAirPlayConf::startGpiLine(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("START_GPI_LINE",chan).toInt();
}


void RDAirPlayConf::setStartGpiLine(RDAirPlayConf::Channel chan,int line) const
{
  SetChannelValue("START_GPI_LINE",chan,line);
}


int RDAirPlayConf::startGpoMatrix(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("START_GPO_MATRIX",chan).toInt();
}


void RDAirPlayConf::setStartGpoMatrix(RDAirPlayConf::Channel chan,int matrix) const
{
  SetChannelValue("START_GPO_MATRIX",chan,matrix);
}


int RDAirPlayConf::startGpoLine(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("START_GPO_LINE",chan).toInt();
}


void RDAirPlayConf::setStartGpoLine(RDAirPlayConf::Channel chan,int line) const
{
  SetChannelValue("START_GPO_LINE",chan,line);
}


int RDAirPlayConf::stopGpiMatrix(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("STOP_GPI_MATRIX",chan).toInt();
}


void RDAirPlayConf::setStopGpiMatrix(RDAirPlayConf::Channel chan,int matrix) const
{
  SetChannelValue("STOP_GPI_MATRIX",chan,matrix);
}


int RDAirPlayConf::stopGpiLine(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("STOP_GPI_LINE",chan).toInt();
}


void RDAirPlayConf::setStopGpiLine(RDAirPlayConf::Channel chan,int line) const
{
  SetChannelValue("STOP_GPI_LINE",chan,line);
}


int RDAirPlayConf::stopGpoMatrix(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("STOP_GPO_MATRIX",chan).toInt();
}


void RDAirPlayConf::setStopGpoMatrix(RDAirPlayConf::Channel chan,int matrix) const
{
  SetChannelValue("STOP_GPO_MATRIX",chan,matrix);
}


int RDAirPlayConf::stopGpoLine(RDAirPlayConf::Channel chan) const
{
  return GetChannelValue("STOP_GPO_LINE",chan).toInt();
}


void RDAirPlayConf::setStopGpoLine(RDAirPlayConf::Channel chan,int line) const
{
  SetChannelValue("STOP_GPO_LINE",chan,line);
}


int RDAirPlayConf::segueLength() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"SEGUE_LENGTH").toInt();
}


void RDAirPlayConf::setSegueLength(int len) const
{
  SetRow("SEGUE_LENGTH",len);
}


int RDAirPlayConf::transLength() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"TRANS_LENGTH").toInt();
}


void RDAirPlayConf::setTransLength(int len) const
{
  SetRow("TRANS_LENGTH",len);
}


RDAirPlayConf::OpModeStyle RDAirPlayConf::opModeStyle() const
{
  return (RDAirPlayConf::OpModeStyle)
    RDGetSqlValue(air_tablename,"ID",air_id,"LOG_MODE_STYLE").toInt();
}


void RDAirPlayConf::setOpModeStyle(RDAirPlayConf::OpModeStyle style) const
{
  SetRow("LOG_MODE_STYLE",(int)style);
}


RDAirPlayConf::OpMode RDAirPlayConf::opMode(int mach) const
{
  return GetLogMode("OP_MODE",mach);
}


void RDAirPlayConf::setOpMode(int mach,RDAirPlayConf::OpMode mode) const
{
  SetLogMode("OP_MODE",mach,mode);
}


RDAirPlayConf::OpMode RDAirPlayConf::logStartMode(int mach) const
{
  return GetLogMode("START_MODE",mach);
}


void RDAirPlayConf::setLogStartMode(int mach,RDAirPlayConf::OpMode mode) const
{
  SetLogMode("START_MODE",mach,mode);
}


int RDAirPlayConf::pieCountLength() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"PIE_COUNT_LENGTH").toInt();
}


void RDAirPlayConf::setPieCountLength(int len) const
{
  SetRow("PIE_COUNT_LENGTH",len);
}


RDAirPlayConf::PieEndPoint RDAirPlayConf::pieEndPoint() const
{
  return (RDAirPlayConf::PieEndPoint)
    RDGetSqlValue(air_tablename,"ID",air_id,"PIE_COUNT_ENDPOINT").toInt();
}


void RDAirPlayConf::setPieEndPoint(RDAirPlayConf::PieEndPoint point) const
{
  SetRow("PIE_COUNT_ENDPOINT",(int)point);
}


bool RDAirPlayConf::checkTimesync() const
{
  return RDBool(RDGetSqlValue(air_tablename,"ID",air_id,
			    "CHECK_TIMESYNC").toString());
}


void RDAirPlayConf::setCheckTimesync(bool state) const
{
  SetRow("CHECK_TIMESYNC",RDYesNo(state));
}


int RDAirPlayConf::panels(RDAirPlayConf::PanelType type) const
{
  switch(type) {
      case RDAirPlayConf::StationPanel:
	return RDGetSqlValue(air_tablename,"ID",air_id,"STATION_PANELS").toInt();

      case RDAirPlayConf::UserPanel:
	return RDGetSqlValue(air_tablename,"ID",air_id,"USER_PANELS").toInt();
  }
  return 0;
}


void RDAirPlayConf::setPanels(RDAirPlayConf::PanelType type,int quan) const
{
  switch(type) {
      case RDAirPlayConf::StationPanel:
	SetRow("STATION_PANELS",quan);
	break;

      case RDAirPlayConf::UserPanel:
	SetRow("USER_PANELS",quan);
	break;
  }
}


bool RDAirPlayConf::showAuxButton(int auxbutton) const
{
  return RDBool(RDGetSqlValue(air_tablename,"ID",
	    air_id,QString().sprintf("SHOW_AUX_%d",auxbutton+1)).toString());
}


void RDAirPlayConf::setShowAuxButton(int auxbutton,bool state) const
{
  SetRow(QString().sprintf("SHOW_AUX_%d",auxbutton+1),RDYesNo(state));
}


bool RDAirPlayConf::clearFilter() const
{
  return 
    RDBool(RDGetSqlValue(air_tablename,"ID",air_id,"CLEAR_FILTER").toString());
}


void RDAirPlayConf::setClearFilter(bool state) const
{
  SetRow("CLEAR_FILTER",RDYesNo(state));
}


RDLogLine::TransType RDAirPlayConf::defaultTransType() const
{
  return (RDLogLine::TransType)
    RDGetSqlValue(air_tablename,"ID",air_id,"DEFAULT_TRANS_TYPE").toInt();
}


void RDAirPlayConf::setDefaultTransType(RDLogLine::TransType type) const
{
  SetRow("DEFAULT_TRANS_TYPE",(int)type);
}


RDAirPlayConf::BarAction RDAirPlayConf::barAction() const
{
  return (RDAirPlayConf::BarAction)
    RDGetSqlValue(air_tablename,"ID",air_id,"BAR_ACTION").toUInt();
}


void RDAirPlayConf::setBarAction(RDAirPlayConf::BarAction action) const
{
  SetRow("BAR_ACTION",(int)action);
}


bool RDAirPlayConf::flashPanel() const
{
  return 
    RDBool(RDGetSqlValue(air_tablename,"ID",air_id,"FLASH_PANEL").toString());
}


void RDAirPlayConf::setFlashPanel(bool state) const
{
  SetRow("FLASH_PANEL",RDYesNo(state));
}


bool RDAirPlayConf::panelPauseEnabled() const
{
  return RDBool(RDGetSqlValue(air_tablename,"ID",air_id,"PANEL_PAUSE_ENABLED").
	       toString());
}


void RDAirPlayConf::setPanelPauseEnabled(bool state) const
{
  SetRow("PANEL_PAUSE_ENABLED",RDYesNo(state));
}


QString RDAirPlayConf::buttonLabelTemplate() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"BUTTON_LABEL_TEMPLATE").
    toString();
}


void RDAirPlayConf::setButtonLabelTemplate(const QString &str) const
{
  SetRow("BUTTON_LABEL_TEMPLATE",str);
}


bool RDAirPlayConf::pauseEnabled() const
{
  return 
    RDBool(RDGetSqlValue(air_tablename,"ID",air_id,"PAUSE_ENABLED").toString());
}


void RDAirPlayConf::setPauseEnabled(bool state) const
{
  SetRow("PAUSE_ENABLED",RDYesNo(state));
}


QString RDAirPlayConf::defaultSvc() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"DEFAULT_SERVICE").toString();
}


void RDAirPlayConf::setDefaultSvc(const QString &svcname) const
{
  SetRow("DEFAULT_SERVICE",svcname);
}


bool RDAirPlayConf::hourSelectorEnabled() const
{
  return 
    RDBool(RDGetSqlValue(air_tablename,"ID",air_id,"HOUR_SELECTOR_ENABLED").
	   toString());
}


void RDAirPlayConf::setHourSelectorEnabled(bool state) const
{
  SetRow("HOUR_SELECTOR_ENABLED",RDYesNo(state));
}


QString RDAirPlayConf::titleTemplate() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"TITLE_TEMPLATE").
    toString();
}


void RDAirPlayConf::setTitleTemplate(const QString &str)
{
  SetRow("TITLE_TEMPLATE",str);
}


QString RDAirPlayConf::artistTemplate() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"ARTIST_TEMPLATE").
    toString();
}


void RDAirPlayConf::setArtistTemplate(const QString &str)
{
  SetRow("ARTIST_TEMPLATE",str);
}


QString RDAirPlayConf::outcueTemplate() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"OUTCUE_TEMPLATE").
    toString();
}


void RDAirPlayConf::setOutcueTemplate(const QString &str)
{
  SetRow("OUTCUE_TEMPLATE",str);
}


QString RDAirPlayConf::descriptionTemplate() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"DESCRIPTION_TEMPLATE").
    toString();
}


void RDAirPlayConf::setDescriptionTemplate(const QString &str)
{
  SetRow("DESCRIPTION_TEMPLATE",str);
}


QHostAddress RDAirPlayConf::udpAddress(int logno) const
{
  QHostAddress addr;
  QString str(RDGetSqlValue(air_tablename,"ID",air_id,
			   QString().sprintf("UDP_ADDR%d",logno)).toString());
  addr.setAddress(str);
  return addr;
}


void RDAirPlayConf::setUdpAddress(int logno,QHostAddress addr) const
{
  SetRow(QString().sprintf("UDP_ADDR%d",logno),addr.toString());
}


Q_UINT16 RDAirPlayConf::udpPort(int logno) const
{
  return (Q_UINT16)RDGetSqlValue(air_tablename,"ID",air_id,QString().
				sprintf("UDP_PORT%d",logno)).toInt();
}


void RDAirPlayConf::setUdpPort(int logno,Q_UINT16 port) const
{
  SetRow(QString().sprintf("UDP_PORT%d",logno),(int)port);
}


QString RDAirPlayConf::udpString(int logno) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,
		      QString().sprintf("UDP_STRING%d",logno)).toString();
}


void RDAirPlayConf::setUdpString(int logno,const QString &str) const
{
  SetRow(QString().sprintf("UDP_STRING%d",logno),str);
}


QString RDAirPlayConf::logRml(int logno) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,
		      QString().sprintf("LOG_RML%d",logno)).toString();
}


void RDAirPlayConf::setLogRml(int logno,const QString &str) const
{
  SetRow(QString().sprintf("LOG_RML%d",logno),str);
}


RDAirPlayConf::ExitCode RDAirPlayConf::exitCode() const
{
  return (RDAirPlayConf::ExitCode)
    RDGetSqlValue(air_tablename,"ID",air_id,"EXIT_CODE").toInt();
}


void RDAirPlayConf::setExitCode(RDAirPlayConf::ExitCode code) const
{
  SetRow("EXIT_CODE",(int)code);
}


bool RDAirPlayConf::exitPasswordValid(const QString &passwd) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select EXIT_PASSWORD from %s where \
                         STATION=\"%s\" && EXIT_PASSWORD=PASSWORD(\"%s\")",
			(const char *)air_tablename,
			(const char *)air_station,
			(const char *)passwd);
  q=new RDSqlQuery(sql);
  if(q->size()>0) {
    delete q;
    return true;
  }
  delete q;
  return false;
}


void RDAirPlayConf::setExitPassword(const QString &passwd) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf ("update %s set EXIT_PASSWORD=PASSWORD(\"%s\") \
                          where STATION=\"%s\"",(const char *)air_tablename,
			 (const char *)passwd,
			 (const char *)air_station);
  q=new RDSqlQuery(sql);
  delete q;
}


QString RDAirPlayConf::skinPath() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"SKIN_PATH").toString();
}


void RDAirPlayConf::setSkinPath(const QString &path) const
{
  SetRow("SKIN_PATH",path);
}


bool RDAirPlayConf::showCounters() const
{
  return RDBool(RDGetSqlValue(air_tablename,"ID",air_id,"SHOW_COUNTERS").
		toString());
}


void RDAirPlayConf::setShowCounters(bool state) const
{
  SetRow("SHOW_COUNTERS",RDYesNo(state));
}


int RDAirPlayConf::auditionPreroll() const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,"AUDITION_PREROLL").toInt();
}


void RDAirPlayConf::setAuditionPreroll(int msecs) const
{
  SetRow("AUDITION_PREROLL",msecs);
}


RDAirPlayConf::StartMode RDAirPlayConf::startMode(int lognum) const
{
  return (RDAirPlayConf::StartMode)
    RDGetSqlValue(air_tablename,"ID",
		 air_id,QString().sprintf("LOG%d_START_MODE",lognum)).toInt();
}


void RDAirPlayConf::setStartMode(int lognum,RDAirPlayConf::StartMode mode) const
{
  SetRow(QString().sprintf("LOG%d_START_MODE",lognum),(int)mode);
}


bool RDAirPlayConf::autoRestart(int lognum) const
{
  return RDBool(RDGetSqlValue(air_tablename,"ID",air_id,
			    QString().sprintf("LOG%d_AUTO_RESTART",lognum)).
			    toString());
}


void RDAirPlayConf::setAutoRestart(int lognum,bool state) const
{
  SetRow(QString().sprintf("LOG%d_AUTO_RESTART",lognum),RDYesNo(state));
}


QString RDAirPlayConf::logName(int lognum) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,QString().
		      sprintf("LOG%d_LOG_NAME",lognum)).toString();
}


void RDAirPlayConf::setLogName(int lognum,const QString &name) const
{
  SetRow(QString().sprintf("LOG%d_LOG_NAME",lognum),name);
}


QString RDAirPlayConf::currentLog(int lognum) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,QString().
		      sprintf("LOG%d_CURRENT_LOG",lognum)).toString();
}


void RDAirPlayConf::setCurrentLog(int lognum,const QString &name) const
{
  SetRow(QString().sprintf("LOG%d_CURRENT_LOG",lognum),name);
}


bool RDAirPlayConf::logRunning(int lognum) const
{
  return RDBool(RDGetSqlValue(air_tablename,"ID",air_id,
			    QString().sprintf("LOG%d_RUNNING",lognum)).
			    toString());
}


void RDAirPlayConf::setLogRunning(int lognum,bool state) const
{
  SetRow(QString().sprintf("LOG%d_RUNNING",lognum),RDYesNo(state));
}


int RDAirPlayConf::logId(int lognum) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,QString().
		      sprintf("LOG%d_LOG_ID",lognum)).toInt();
}


void RDAirPlayConf::setLogId(int lognum,int id) const
{
  SetRow(QString().sprintf("LOG%d_LOG_ID",lognum),id);
}


int RDAirPlayConf::logCurrentLine(int lognum) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,QString().
		      sprintf("LOG%d_LOG_LINE",lognum)).toInt();
}


void RDAirPlayConf::setLogCurrentLine(int lognum,int line) const
{
  SetRow(QString().sprintf("LOG%d_LOG_LINE",lognum),line);
}


unsigned RDAirPlayConf::logNowCart(int lognum) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,QString().
		      sprintf("LOG%d_NOW_CART",lognum)).toUInt();
}


void RDAirPlayConf::setLogNowCart(int lognum,unsigned cartnum) const
{
  SetRow(QString().sprintf("LOG%d_NOW_CART",lognum),cartnum);
}


unsigned RDAirPlayConf::logNextCart(int lognum) const
{
  return RDGetSqlValue(air_tablename,"ID",air_id,QString().
		      sprintf("LOG%d_NEXT_CART",lognum)).toUInt();
}


void RDAirPlayConf::setLogNextCart(int lognum,unsigned cartnum) const
{
  SetRow(QString().sprintf("LOG%d_NEXT_CART",lognum),cartnum);
}


QString RDAirPlayConf::channelText(RDAirPlayConf::Channel chan)
{
  QString ret=QObject::tr("Unknown");

  switch(chan) {
  case RDAirPlayConf::MainLog1Channel:
    ret=QObject::tr("Main Log Output 1");
    break;

  case RDAirPlayConf::MainLog2Channel:
    ret=QObject::tr("Main Log Output 2");
    break;

  case RDAirPlayConf::SoundPanel1Channel:
    ret=QObject::tr("Sound Panel First Play Output");
    break;

  case RDAirPlayConf::CueChannel:
    ret=QObject::tr("Audition/Cue Output");
    break;

  case RDAirPlayConf::AuxLog1Channel:
    ret=QObject::tr("Aux Log 1 Output");
    break;

  case RDAirPlayConf::AuxLog2Channel:
    ret=QObject::tr("Aux Log 2 Output");
    break;

  case RDAirPlayConf::SoundPanel2Channel:
    ret=QObject::tr("Sound Panel Second Play Output");
    break;

  case RDAirPlayConf::SoundPanel3Channel:
    ret=QObject::tr("Sound Panel Third Play Output");
    break;

  case RDAirPlayConf::SoundPanel4Channel:
    ret=QObject::tr("Sound Panel Fourth Play Output");
    break;

  case RDAirPlayConf::SoundPanel5Channel:
    ret=QObject::tr("Sound Panel Fifth and Later Play Output");
    break;

  case RDAirPlayConf::LastChannel:
    break;
  }

  return ret;
}


QString RDAirPlayConf::logModeText(RDAirPlayConf::OpMode mode)
{
  QString ret=QObject::tr("Unknown");

  switch(mode) {
  case RDAirPlayConf::LiveAssist:
    ret=QObject::tr("LiveAssist");
    break;

  case RDAirPlayConf::Auto:
    ret=QObject::tr("Automatic");
    break;

  case RDAirPlayConf::Manual:
    ret=QObject::tr("Manual");
    break;

  case RDAirPlayConf::Previous:
    ret=QObject::tr("Previous");
    break;
  }

  return ret;
}


QVariant RDAirPlayConf::GetChannelValue(const QString &param,RDAirPlayConf::Channel chan) const
{
  RDSqlQuery *q;
  QString sql;
  QVariant ret;

  sql=QString("select ")+param+" from "+air_tablename+"_CHANNELS where "+
    "(STATION_NAME=\""+RDEscapeString(air_station)+"\")&&"+
    QString().sprintf("(INSTANCE=%u)",chan);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0);
  }
  delete q;

  return ret;
}


void RDAirPlayConf::SetChannelValue(const QString &param,RDAirPlayConf::Channel chan,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update ")+air_tablename+"_CHANNELS set "+
    param+QString().sprintf("=%d ",value)+
    "where (STATION_NAME=\""+RDEscapeString(air_station)+"\")&&"+
    QString().sprintf("(INSTANCE=%d)",chan);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDAirPlayConf::SetChannelValue(const QString &param,RDAirPlayConf::Channel chan,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update ")+air_tablename+"_CHANNELS set "+
    param+"=\""+RDEscapeString(value)+"\" "+
    "where (STATION_NAME=\""+RDEscapeString(air_station)+"\")&&"+
    QString().sprintf("(INSTANCE=%d)",chan);
  q=new RDSqlQuery(sql);
  delete q;
}


RDAirPlayConf::OpMode RDAirPlayConf::GetLogMode(const QString &param,int mach) const
{
  RDAirPlayConf::OpMode mode=RDAirPlayConf::Auto;
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+param+" from LOG_MODES where "+
    "(STATION_NAME=\""+RDEscapeString(air_station)+"\")&&"+
    QString().sprintf("MACHINE=%d",mach);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    mode=(RDAirPlayConf::OpMode)q->value(0).toInt();
  }
  delete q;
  return mode;
}


void RDAirPlayConf::SetLogMode(const QString &param,int mach,
			       RDAirPlayConf::OpMode mode) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update LOG_MODES set ")+param+QString().sprintf("=%d ",mode)+
    "where (STATION_NAME=\""+RDEscapeString(air_station)+"\")&&"+
    QString().sprintf("(MACHINE=%d)",mach);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDAirPlayConf::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().
    sprintf("UPDATE %s SET %s=%d WHERE STATION=\"%s\"",
	    (const char *)air_tablename,
	    (const char *)param,
	    value,
	    (const char *)RDEscapeString(air_station));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDAirPlayConf::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().
    sprintf("UPDATE %s SET %s=%u WHERE STATION=\"%s\"",
	    (const char *)air_tablename,
	    (const char *)param,
	    value,
	    (const char *)RDEscapeString(air_station));
  q=new RDSqlQuery(sql);
  delete q;
}


void RDAirPlayConf::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().
    sprintf("UPDATE %s SET %s=\"%s\" WHERE STATION=\"%s\"",
	    (const char *)air_tablename,
	    (const char *)RDEscapeString(param),
	    (const char *)RDEscapeString(value),
	    (const char *)RDEscapeString(air_station));
  q=new RDSqlQuery(sql);
  delete q;
}
