// rlmhost.cpp
//
// A container class for a Rivendell Loadable Module host.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rlmhost.cpp,v 1.7.6.9.2.1 2014/03/19 19:25:18 cvs Exp $
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

#include <dlfcn.h>
#include <iostream>

#include <rdconf.h>
#include <rdprofile.h>
#include <rdnownext.h>
#include <rdsvc.h>

#include <globals.h>
#include <rlmhost.h>


RLMHost::RLMHost(const QString &path,const QString &arg,
		 QSocketDevice *udp_socket,QObject *parent,const char *name)
  : QObject(parent,name)
{
  plugin_path=path;
  plugin_arg=arg;
  plugin_udp_socket=udp_socket;
  plugin_handle=NULL;
  plugin_start_sym=NULL;
  plugin_free_sym=NULL;
  plugin_pad_data_sent_sym=NULL;
  plugin_timer_expired_sym=NULL;
  plugin_serial_data_received_sym=NULL;

  //
  // Utility Timers
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(timerData(int)));
  for(int i=0;i<RLM_MAX_TIMERS;i++) {
    plugin_callback_timers[i]=new QTimer(this);
    mapper->setMapping(plugin_callback_timers[i],i);
    connect(plugin_callback_timers[i],SIGNAL(timeout()),mapper,SLOT(map()));
  }
}


RLMHost::~RLMHost()
{
}


QString RLMHost::pluginPath() const
{
  return plugin_path;
}


QString RLMHost::pluginArg() const
{
  return plugin_arg;
}


void RLMHost::sendEvent(const QString &svcname,const QString &logname,
			int lognum,RDLogLine **loglines,bool onair,
			RDAirPlayConf::OpMode mode)
{
  if(plugin_pad_data_sent_sym!=NULL) {
    QDateTime now_dt(QDate::currentDate(),QTime::currentTime());
    struct rlm_svc *svc=new struct rlm_svc;
    struct rlm_log *log=new struct rlm_log;
    struct rlm_pad *now=new struct rlm_pad;
    struct rlm_pad *next=new struct rlm_pad;
    memset(svc,0,sizeof(svc));
    RDSvc *service=new RDSvc(svcname);
    if(!svcname.isEmpty()) {
      sprintf(svc->svc_name,"%s",(const char *)svcname.left(255));
      if(!service->programCode().isEmpty()) {
	sprintf(svc->svc_pgmcode,"%s",(const char *)service->programCode());
      }
      else {
	svc->svc_pgmcode[0]=0;
      }
    }
    else {
      svc->svc_name[0]=0;
      svc->svc_pgmcode[0]=0;
    }
    delete service;
    memset(log,0,sizeof(log));
    if(!logname.isEmpty()) {
      sprintf(log->log_name,"%s",(const char *)logname.left(64));
    }
    else {
      log->log_name[0]=0;
    }
    log->log_mach=lognum;
    log->log_onair=onair;
    log->log_mode=mode;
    RLMHost::loadMetadata(loglines[0],now,now_dt);
    RLMHost::loadMetadata(loglines[1],next); 
    plugin_pad_data_sent_sym(this,svc,log,now,next);
    delete next;
    delete now;
    delete log;
    delete svc;
  }
}


bool RLMHost::load()
{
  QString basename=RDGetBasePart(plugin_path);
  basename=basename.left(basename.findRev("."));
  if((plugin_handle=dlopen(plugin_path,RTLD_LAZY))==NULL) {
    return false;
  }
  *(void **)(&plugin_start_sym)=dlsym(plugin_handle,basename+"_RLMStart");
  *(void **)(&plugin_free_sym)=dlsym(plugin_handle,basename+"_RLMFree");
  *(void **)(&plugin_pad_data_sent_sym)=
    dlsym(plugin_handle,basename+"_RLMPadDataSent");
  *(void **)(&plugin_timer_expired_sym)=
    dlsym(plugin_handle,basename+"_RLMTimerExpired");
  *(void **)(&plugin_serial_data_received_sym)=
    dlsym(plugin_handle,basename+"_RLMSerialDataReceived");
  if(plugin_start_sym!=NULL) {
    plugin_start_sym(this,plugin_arg);
  }

  return true;
}


void RLMHost::unload()
{
  if(plugin_free_sym!=NULL) {
    plugin_free_sym(this);
  }
}


void RLMHost::loadMetadata(const RDLogLine *logline,struct rlm_pad *pad,
			   const QDateTime &start_datetime)
{
  QDateTime now(QDate::currentDate(),QTime::currentTime());

  if(pad==NULL) {
    return;
  }
  memset(pad,0,sizeof(struct rlm_pad));
  if(logline==NULL) {
    return;
  }
  if(logline!=NULL) {
    pad->rlm_cartnum=logline->cartNumber();
    switch(logline->cartType()) {
    case RDCart::Audio:
      pad->rlm_len=logline->effectiveLength();
      break;

    case RDCart::Macro:
      if((logline->eventLength()>=0)&&logline->useEventLength()) {
	pad->rlm_len=logline->eventLength();
      }
      else {
	pad->rlm_len=logline->effectiveLength();
      }
      break;

    case RDCart::All:
      break;
    }
    pad->rlm_carttype=logline->cartType();
    if(!logline->year().isNull()) {
      snprintf(pad->rlm_year,5,"%s",
	       (const char *)logline->year().toString("YYYY"));
    }
    if(!logline->groupName().isEmpty()) {
      snprintf(pad->rlm_group,11,"%s",
	       (const char *)logline->groupName().utf8());
    }
    if(!logline->title().isEmpty()) {
      snprintf(pad->rlm_title,256,"%s",(const char *)logline->title().utf8());
    }
    if(!logline->artist().isEmpty()) {
      snprintf(pad->rlm_artist,256,"%s",(const char *)logline->artist().utf8());
    }
    if(!logline->label().isEmpty()) {
      snprintf(pad->rlm_label,65,"%s",(const char *)logline->label().utf8());
    }
    if(!logline->client().isEmpty()) {
      snprintf(pad->rlm_client,65,"%s",(const char *)logline->client().utf8());
    }
    if(!logline->agency().isEmpty()) {
      snprintf(pad->rlm_agency,65,"%s",(const char *)logline->agency().utf8());
    }
    if(!logline->composer().isEmpty()) {
      snprintf(pad->rlm_comp,65,"%s",(const char *)logline->composer().utf8());
    }
    if(!logline->publisher().isEmpty()) {
      snprintf(pad->rlm_pub,65,"%s",(const char *)logline->publisher().utf8());
    }
    if(!logline->userDefined().isEmpty()) {
      snprintf(pad->rlm_userdef,256,"%s",
	       (const char *)logline->userDefined().utf8());
    }
    if(!logline->outcue().isEmpty()) {
      snprintf(pad->rlm_outcue,65,"%s",(const char *)logline->outcue().utf8());
    }
    if(!logline->description().isEmpty()) {
      snprintf(pad->rlm_description,65,"%s",
	       (const char *)logline->description().utf8());
    }
    if(!logline->conductor().isEmpty()) {
      snprintf(pad->rlm_conductor,65,"%s",
	      (const char *)logline->conductor().utf8());
    }
    if(!logline->songId().isEmpty()) {
      snprintf(pad->rlm_song_id,33,"%s",(const char *)logline->songId().utf8());
    }
    if(!logline->album().isEmpty()) {
      snprintf(pad->rlm_album,256,"%s",(const char *)logline->album().utf8());
    }
    if(!logline->isrc().isEmpty()) {
      strncpy(pad->rlm_isrc,(const char *)logline->isrc().utf8().left(12),12);
    }
    if(!logline->isci().isEmpty()) {
      strncpy(pad->rlm_isci,(const char *)logline->isci().utf8().left(32),32);
    }
    if(!logline->extData().isEmpty()) {
      snprintf(pad->rlm_ext_data,32,"%s",(const char *)logline->extData());
    }
    if(!logline->extEventId().isEmpty()) {
      snprintf(pad->rlm_ext_eventid,32,"%s",
	       (const char *)logline->extEventId());
    }
    if(!logline->extAnncType().isEmpty()) {
      snprintf(pad->rlm_ext_annctype,32,"%s",
	       (const char *)logline->extAnncType());
    }
    if(start_datetime.isValid()) {
      pad->rlm_start_msec=start_datetime.time().msec();
      pad->rlm_start_sec=start_datetime.time().second();
      pad->rlm_start_min=start_datetime.time().minute();
      pad->rlm_start_hour=start_datetime.time().hour();
      pad->rlm_start_day=start_datetime.date().day();
      pad->rlm_start_mon=start_datetime.date().month();
      pad->rlm_start_year=start_datetime.date().year();
    }
    else {
      QTime start_time=logline->startTime(RDLogLine::Predicted);
      if(start_time.isNull()) {
	start_time=logline->startTime(RDLogLine::Imported);
      }
      if(!start_time.isNull()) {
	if(start_time<now.time()) {  // Crossing midnight
	  now=now.addDays(1);
	}
      }
      pad->rlm_start_msec=start_time.msec();
      pad->rlm_start_sec=start_time.second();
      pad->rlm_start_min=start_time.minute();
      pad->rlm_start_hour=start_time.hour();
      pad->rlm_start_day=now.date().day();
      pad->rlm_start_mon=now.date().month();
      pad->rlm_start_year=now.date().year();
    }
  }
}


void RLMHost::saveMetadata(const struct rlm_pad *pad,RDLogLine *logline)
{
  if(logline==NULL) {
    return;
  }
  logline->clear();
  if(pad==NULL) {
    return;
  }
  logline->setCartNumber(pad->rlm_cartnum);
  logline->setForcedLength(pad->rlm_len);
  logline->setYear(QDate(QString(pad->rlm_year).toInt(),1,1));
  logline->setGroupName(pad->rlm_group);
  logline->setTitle(pad->rlm_title);
  logline->setArtist(pad->rlm_artist);
  logline->setLabel(pad->rlm_label);
  logline->setClient(pad->rlm_client);
  logline->setAgency(pad->rlm_agency);
  logline->setComposer(pad->rlm_comp);
  logline->setPublisher(pad->rlm_pub);
  logline->setUserDefined(pad->rlm_userdef);
  logline->setOutcue(pad->rlm_outcue);
  logline->setDescription(pad->rlm_description);
  logline->setAlbum(pad->rlm_album);
  logline->setIsrc(QString::fromAscii(pad->rlm_isrc,12));
  logline->setIsci(QString::fromAscii(pad->rlm_isci,32));
  if((pad->rlm_start_year>0)&&(pad->rlm_start_mon>0)&&(pad->rlm_start_day)) {
    logline->setStartDatetime(QDateTime(QDate(pad->rlm_start_year,
					      pad->rlm_start_mon,
					      pad->rlm_start_day),
					QTime(pad->rlm_start_hour,
					      pad->rlm_start_min,
					      pad->rlm_start_sec,
					      pad->rlm_start_msec)));
  }
  else {
    logline->setStartDatetime(QDateTime());
  }
}


void RLMHost::timerData(int timernum)
{
  if(plugin_timer_expired_sym!=NULL) {
    plugin_timer_expired_sym(this,timernum);
  }
}


void RLMHost::ttyReceiveReadyData(int fd)
{
  char data[1024];
  int n;

  for(unsigned i=0;i<plugin_tty_devices.size();i++) {
    if(plugin_tty_devices[i]->socket()==fd) {
      while((n=plugin_tty_devices[i]->readBlock(data,1024))>0) {
	if(plugin_serial_data_received_sym!=NULL) {
	  plugin_serial_data_received_sym(this,i,data,n);
	}
      }
      return;
    }
  }
  fprintf(stderr,"unknown tty descriptor: %d\n",fd);
}


//
// RLM Utility Functions
//
void RLMSendUdp(void *ptr,const char *ipaddr,uint16_t port,
		const char *data,int len)
{
  RLMHost *host=(RLMHost *)ptr;
  QHostAddress addr;
  addr.setAddress(ipaddr);
  if(!addr.isNull()) {
    host->plugin_udp_socket->writeBlock(data,len,addr,port);
  }
}


int RLMOpenSerial(void *ptr,const char *port,int speed,int parity,
		  int word_length)
{
  RLMHost *host=(RLMHost *)ptr;
  host->plugin_tty_devices.push_back(new RDTTYDevice);
  host->plugin_tty_devices.back()->setName(port);
  host->plugin_tty_devices.back()->setSpeed(speed);
  host->plugin_tty_devices.back()->setParity((RDTTYDevice::Parity)parity);
  host->plugin_tty_devices.back()->setWordLength(word_length);
  if(host->plugin_tty_devices.back()->open(IO_ReadWrite)) {

    host->plugin_tty_notifiers.
      push_back(new QSocketNotifier(host->plugin_tty_devices.back()->socket(),
				    QSocketNotifier::Read));
    host->connect(host->plugin_tty_notifiers.back(),SIGNAL(activated(int)),
		  host,SLOT(ttyReceiveReadyData(int)));
    return (int)host->plugin_tty_devices.size()-1;
  }
  return -1;
}


void RLMSendSerial(void *ptr,int handle,const char *data,int len)
{
  RLMHost *host=(RLMHost *)ptr;
  if((handle<0)||(handle>=(int)host->plugin_tty_devices.size())) {
    return;
  }
  host->plugin_tty_devices[handle]->writeBlock(data,len);
}


void RLMCloseSerial(void *ptr,int handle)
{
  RLMHost *host=(RLMHost *)ptr;

  //
  // FIXME: We really ought to take out the trash here!
  //
  host->plugin_tty_devices[handle]->close();
  delete host->plugin_tty_devices[handle];
  host->plugin_tty_devices[handle]=NULL;
}


const char *RLMDateTime(void *ptr,int offset_msecs,const char *format)
{
  RLMHost *host=(RLMHost *)ptr;
  QDateTime datetime=QDateTime(QDate::currentDate(),QTime::currentTime().
			       addMSecs(offset_msecs));
  strncpy(host->plugin_value_string,datetime.toString(format),1024);
  return host->plugin_value_string;
}


const char *RLMResolveNowNextEncoded(void *ptr,const struct rlm_pad *now,
				     const struct rlm_pad *next,
				     const char *format,int encoding)
{
  RLMHost *host=(RLMHost *)ptr;
  RDLogLine *loglines[2];
  QString str=format;

  loglines[0]=new RDLogLine();
  loglines[1]=new RDLogLine();
  RLMHost::saveMetadata(now,loglines[0]);
  RLMHost::saveMetadata(next,loglines[1]);
  RDResolveNowNext(&str,loglines,encoding);
  strncpy(host->plugin_value_string,str,1024);
  delete loglines[1];
  delete loglines[0];

  return host->plugin_value_string;
}


const char *RLMResolveNowNext(void *ptr,const struct rlm_pad *now,
			      const struct rlm_pad *next,const char *format)
{
  return RLMResolveNowNextEncoded(ptr,now,next,format,RLM_ENCODE_NONE);
}


void RLMLog(void *ptr,int prio,const char *msg)
{
  LogLine((RDConfig::LogPriority)prio,msg);
}


void RLMStartTimer(void *ptr,int timernum,int msecs,int mode)
{
  RLMHost *host=(RLMHost *)ptr;
  if((timernum<0)||(timernum>=RLM_MAX_TIMERS)) {
    return;
  }
  if(host->plugin_callback_timers[timernum]->isActive()) {
    host->plugin_callback_timers[timernum]->stop();
  }
  host->plugin_callback_timers[timernum]->start(msecs,mode);
}


void RLMStopTimer(void *ptr,int timernum)
{
  RLMHost *host=(RLMHost *)ptr;
  if((timernum<0)||(timernum>=RLM_MAX_TIMERS)) {
    return;
  }
  if(host->plugin_callback_timers[timernum]->isActive()) {
    host->plugin_callback_timers[timernum]->stop();
  }
}


int RLMGetIntegerValue(void *ptr,const char *filename,const char *section,
		       const char *label,int default_value)
{
  RDProfile *p=new RDProfile();
  p->setSource(filename);
  int r=p->intValue(section,label,default_value);
  delete p;
  return r;
}


int RLMGetHexValue(void *ptr,const char *filename,const char *section,
		   const char *label,int default_value)
{
  RDProfile *p=new RDProfile();
  p->setSource(filename);
  int r=p->hexValue(section,label,default_value);
  delete p;
  return r;
}


int RLMGetBooleanValue(void *ptr,const char *filename,const char *section,
		       const char *label,int default_value)
{
  RDProfile *p=new RDProfile();
  p->setSource(filename);
  bool r=p->boolValue(section,label,default_value);
  delete p;
  return (int)r;
}


const char *RLMGetStringValue(void *ptr,const char *filename,
			      const char *section,const char *label,
			      const char *default_value)
{
  RLMHost *host=(RLMHost *)ptr;
  RDProfile *p=new RDProfile();
  p->setSource(filename);
  strncpy(host->plugin_value_string,
	  p->stringValue(section,label,default_value),1024);
  delete p;
  return host->plugin_value_string;
}
