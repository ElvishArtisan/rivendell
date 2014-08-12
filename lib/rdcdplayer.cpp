// rdcdplayer.cpp
//
// Abstract a Linux CDROM Device.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcdplayer.cpp,v 1.5.2.2 2014/01/10 18:52:24 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/cdrom.h>

#include <qdatetime.h>

#include <rdcdplayer.h>


RDCdPlayer::RDCdPlayer(FILE *profile_msgs,QWidget *parent,const char *name)
  : QObject(parent,name)
{
  cdrom_profile_msgs=profile_msgs;
  cdrom_fd=-1;
  cdrom_track_count=0;
  cdrom_track_start=NULL;
  cdrom_audio_track=NULL;
  cdrom_play_mode=RDCdPlayer::Single;
  cdrom_old_state=false;
  cdrom_audiostatus=0;

  //
  // The Button Timer
  //
  cdrom_button_timer=new QTimer(this,"cdrom_button_timer");
  connect(cdrom_button_timer,SIGNAL(timeout()),this,SLOT(buttonTimerData()));

  //
  // The Clock
  //
  cdrom_clock=new QTimer(this,"cdrom_clock");
  connect(cdrom_clock,SIGNAL(timeout()),this,SLOT(clockData()));
  cdrom_clock->start(RDCDPLAYER_CLOCK_INTERVAL,true);
}


RDCdPlayer::~RDCdPlayer()
{
  if(cdrom_fd>0) {
    close();
  }
  if(cdrom_track_start!=NULL) {
    delete cdrom_track_start;
  }
  if(cdrom_audio_track!=NULL) {
    delete cdrom_audio_track;
  }
  delete cdrom_clock;
  delete cdrom_button_timer;
}


QString RDCdPlayer::device() const
{
  return cdrom_device;
}


void RDCdPlayer::setDevice(QString device)
{
  if(cdrom_fd<0) {
    cdrom_device=device;
  }
}


bool RDCdPlayer::open()
{
  if((cdrom_fd=::open((const char *)cdrom_device,O_RDONLY|O_NONBLOCK))<0) {
    return false;
  }
  return true;
}


void RDCdPlayer::close()
{
  ::close(cdrom_fd);
  cdrom_fd=-1;
}


RDCdPlayer::Status RDCdPlayer::status()
{
  return (RDCdPlayer::Status)ioctl(cdrom_fd,CDROM_DRIVE_STATUS,NULL);
}


RDCdPlayer::Medium RDCdPlayer::medium()
{
  return (RDCdPlayer::Medium)ioctl(cdrom_fd,CDROM_DISC_STATUS,NULL);
}


int RDCdPlayer::tracks() const
{
  return cdrom_track_count;
}


bool RDCdPlayer::isAudio(int track) const
{
  if(cdrom_audio_track==NULL) {
    return false;
  }
  if(track>cdrom_track_count) {
    return false;
  }
  return cdrom_audio_track[track-1];
}


/*
 * TODO:
 * Right now, we return length based just on the MSF minute and second data.
 * Frames should be taken into account too.
 */
int RDCdPlayer::trackLength(int track) const
{
  if(cdrom_track_start==NULL) {
    return 0;
  }
  if(track>cdrom_track_count) {
    return 0;
  }
  return 1000*(60*cdrom_track_start[track].msf.minute+
	       cdrom_track_start[track].msf.second-
	       (60*cdrom_track_start[track-1].msf.minute+
	       cdrom_track_start[track-1].msf.second));
}


unsigned RDCdPlayer::trackOffset(int track) const
{
  if(cdrom_track_start==NULL) {
    return 0;
  }
  if(track>cdrom_track_count) {
    return 0;
  }
  return ((75*(60*cdrom_track_start[track].msf.minute+
	       cdrom_track_start[track].msf.second))+
	  cdrom_track_start[track].msf.frame);
}


RDCdPlayer::State RDCdPlayer::state() const
{
  return cdrom_state;
}


int RDCdPlayer::leftVolume()
{
  struct cdrom_volctrl volctrl;

  if(ioctl(cdrom_fd,CDROMVOLREAD,&volctrl)<0) {
    return -1;
  }
  return (int)volctrl.channel0;
}


int RDCdPlayer::rightVolume()
{
  struct cdrom_volctrl volctrl;

  if(ioctl(cdrom_fd,CDROMVOLREAD,&volctrl)<0) {
    return -1;
  }
  return (int)volctrl.channel1;
}


void RDCdPlayer::setCddbRecord(RDCddbRecord *rec)
{  
  rec->setTracks(cdrom_track_count);
  rec->setDiscId(cdrom_disc_id);
  rec->setDiscLength(75*(60*cdrom_track_start[cdrom_track_count].msf.minute+
    cdrom_track_start[cdrom_track_count].msf.second)+
		     cdrom_track_start[cdrom_track_count].msf.frame);
  for(int i=0;i<cdrom_track_count;i++) {
    rec->setTrackOffset(i,trackOffset(i));
  }
}


void RDCdPlayer::lock()
{
  PushButton(RDCdPlayer::Lock);
}


void RDCdPlayer::unlock()
{
  system("eject -i off "+cdrom_device);
}


void RDCdPlayer::eject()
{
  system("eject "+cdrom_device);
}


void RDCdPlayer::play(int track)
{
  if((cdrom_state!=RDCdPlayer::Paused)||(cdrom_track!=track)) {
    PushButton(RDCdPlayer::Play,track);
  }
  else {
    PushButton(RDCdPlayer::Resume);
  }
}


void RDCdPlayer::pause()
{
  PushButton(RDCdPlayer::Pause);
}


void RDCdPlayer::stop()
{
  PushButton(RDCdPlayer::Stop);
}


void RDCdPlayer::setLeftVolume(int vol)
{
  struct cdrom_volctrl volctrl;

  if(ioctl(cdrom_fd,CDROMVOLREAD,&volctrl)<0) {
    return;
  }
  if(volctrl.channel0!=vol) {
    volctrl.channel0=vol;
    ioctl(cdrom_fd,CDROMVOLCTRL,&volctrl);
    emit leftVolumeChanged(vol);
  }
}


void RDCdPlayer::setRightVolume(int vol)
{
  struct cdrom_volctrl volctrl;

  if(ioctl(cdrom_fd,CDROMVOLREAD,&volctrl)<0) {
    return;
  }
  if(volctrl.channel1!=vol) {
    volctrl.channel1=vol;
    ioctl(cdrom_fd,CDROMVOLCTRL,&volctrl);
    emit rightVolumeChanged(vol);
  }
}


RDCdPlayer::PlayMode RDCdPlayer::playMode() const
{
  return cdrom_play_mode;
}


void RDCdPlayer::setPlayMode(RDCdPlayer::PlayMode mode)
{
  cdrom_play_mode=mode;
}


void RDCdPlayer::buttonTimerData()
{
  struct cdrom_msf msf;

  if(cdrom_fd>0) {
    switch(cdrom_button_queue.front()) {
      case RDCdPlayer::Play:
	memset(&msf,0,sizeof(struct cdrom_msf));
	msf.cdmsf_min0=
	  cdrom_track_start[cdrom_track_queue.front()-1].msf.minute;
	msf.cdmsf_sec0=
	  cdrom_track_start[cdrom_track_queue.front()-1].msf.second;
	msf.cdmsf_frame0=
	  cdrom_track_start[cdrom_track_queue.front()-1].msf.frame;
	if(cdrom_play_mode==Single) {
	  msf.cdmsf_min1=
	    cdrom_track_start[cdrom_track_queue.front()].msf.minute;
	  msf.cdmsf_sec1=
	    cdrom_track_start[cdrom_track_queue.front()].msf.second;
	  msf.cdmsf_frame1=
	    cdrom_track_start[cdrom_track_queue.front()].msf.frame;
	}
	else {
	  msf.cdmsf_min1=cdrom_track_start[cdrom_track_count].msf.minute;
	  msf.cdmsf_sec1=cdrom_track_start[cdrom_track_count].msf.second;
	  msf.cdmsf_frame1=cdrom_track_start[cdrom_track_count].msf.frame;
	}
	ioctl(cdrom_fd,CDROMPLAYMSF,&msf);
	cdrom_state=RDCdPlayer::Playing;
	break;

      case RDCdPlayer::Pause:
	ioctl(cdrom_fd,CDROMPAUSE,NULL);
	cdrom_state=RDCdPlayer::Paused;
	break;

      case RDCdPlayer::Resume:
	ioctl(cdrom_fd,CDROMRESUME,NULL);
	cdrom_state=RDCdPlayer::Playing;
	break;

      case RDCdPlayer::Stop:
	ioctl(cdrom_fd,CDROMSTOP,NULL);
	cdrom_state=RDCdPlayer::Stopped;
	break;

      case RDCdPlayer::Eject:
	if(ioctl(cdrom_fd,CDROM_LOCKDOOR,0)<0) {
	  fprintf(stderr,"RDCdPlayer::Unlock failed: %s\n",strerror(errno));
	}
	if(ioctl(cdrom_fd,CDROMEJECT,NULL)<0) {
	  fprintf(stderr,"RDCdPlayer::Eject failed: %s\n",strerror(errno));
	}
	break;

      case RDCdPlayer::Lock:
	if(ioctl(cdrom_fd,CDROM_LOCKDOOR,1)<0) {
	  fprintf(stderr,"RDCdPlayer::Lock failed: %s\n",strerror(errno));
	}
	break;

      case RDCdPlayer::Unlock:
	if(ioctl(cdrom_fd,CDROM_LOCKDOOR,0)<0) {
	  fprintf(stderr,"RDCdPlayer::Unlock failed: %s\n",strerror(errno));
	}
	break;
    }
  }
  cdrom_button_queue.pop();
  cdrom_track_queue.pop();
  if(cdrom_button_queue.size()>0) {
    cdrom_button_timer->start(RDCDPLAYER_BUTTON_DELAY,true);
  }
}


void RDCdPlayer::clockData()
{
  bool new_state;
  struct cdrom_subchnl subchnl;

  //
  // Media Status
  //
  if(ioctl(cdrom_fd,CDROM_MEDIA_CHANGED,NULL)==0) {
    new_state=true;
    if(cdrom_old_state==false) {
      Profile("ReadToc() started");
      ReadToc();
      Profile("ReadToc() finished");
      Profile("emitting mediaChanged()");
      emit mediaChanged();
      Profile("mediaChanged() emitted");
    }
  }
  else {
    new_state=false;
    if(cdrom_old_state==true) {
      Profile("emitting ejected()");
      emit ejected();
      Profile("ejected() emitted");
    }
  }
  cdrom_old_state=new_state;

  //
  // Audio State
  //
  memset(&subchnl,0,sizeof(struct cdrom_subchnl));
  subchnl.cdsc_format=CDROM_MSF;
  Profile("calling ioctl(CDROMSUBCHNL)");
  if(ioctl(cdrom_fd,CDROMSUBCHNL,&subchnl)>=0) {
    Profile("ioctl(CDROMSUBCHNL) success");
    if(cdrom_audiostatus!=subchnl.cdsc_audiostatus) {
      cdrom_audiostatus=subchnl.cdsc_audiostatus;
      cdrom_track=subchnl.cdsc_trk;
      switch(cdrom_audiostatus) {
	  case CDROM_AUDIO_INVALID:
	    cdrom_state=NoStateInfo;
	    break;
	  case CDROM_AUDIO_PLAY:
	    cdrom_state=RDCdPlayer::Playing;
	    emit played(cdrom_track);
	    break;
	  case CDROM_AUDIO_PAUSED:
	    cdrom_state=RDCdPlayer::Paused;
	    emit paused();
	    break;
	  case CDROM_AUDIO_COMPLETED:
	    cdrom_state=RDCdPlayer::Stopped;
	    emit stopped();
	    break;
	  case CDROM_AUDIO_ERROR:
	    cdrom_state=RDCdPlayer::Stopped;
	    emit stopped();
	    break;
	  case CDROM_AUDIO_NO_STATUS:
	    cdrom_state=RDCdPlayer::Stopped;
	    emit stopped();
	    break;
      }
    }
  }
  else {
    Profile("ioctl(CDROMSUBCHNL) failure");
    if(cdrom_audiostatus!=CDROM_AUDIO_NO_STATUS) {
      cdrom_audiostatus=CDROM_AUDIO_NO_STATUS;
      cdrom_state=RDCdPlayer::Stopped;
      emit stopped();
    }
  }
  cdrom_clock->start(RDCDPLAYER_CLOCK_INTERVAL,true);
}


void RDCdPlayer::ReadToc()
{
  struct cdrom_tochdr tochdr;
  struct cdrom_tocentry tocentry;

  //
  // TOC Header
  //
  if(ioctl(cdrom_fd,CDROMREADTOCHDR,&tochdr)<0) {
    cdrom_track_count=0;
  }
  cdrom_track_count=tochdr.cdth_trk1-tochdr.cdth_trk0+1;

  //
  // TOC Entries
  //
  if(cdrom_track_start!=NULL) {
    delete cdrom_track_start;
  }
  if(cdrom_audio_track!=NULL) {
    delete cdrom_audio_track;
  }
  cdrom_track_start=new union cdrom_addr[cdrom_track_count+1];
  cdrom_audio_track=new bool[cdrom_track_count];
  for(int i=1;i<=cdrom_track_count;i++) {
    memset(&tocentry,0,sizeof(struct cdrom_tocentry));
    tocentry.cdte_track=i;
    tocentry.cdte_format=CDROM_MSF;
    ioctl(cdrom_fd,CDROMREADTOCENTRY,&tocentry);
    cdrom_track_start[i-1]=tocentry.cdte_addr;
    if((tocentry.cdte_ctrl&CDROM_DATA_TRACK)==0) {
      cdrom_audio_track[i-1]=true;
    }
    else {
      cdrom_audio_track[i-1]=false;
    }
  }
  memset(&tocentry,0,sizeof(struct cdrom_tocentry));
  tocentry.cdte_track=CDROM_LEADOUT;
  tocentry.cdte_format=CDROM_MSF;
  ioctl(cdrom_fd,CDROMREADTOCENTRY,&tocentry);
  cdrom_track_start[cdrom_track_count]=tocentry.cdte_addr;
  cdrom_disc_id=GetCddbDiscId();
}


//
// Methods for calculating the CDDB Disc ID are derived from code in
// the 'discid-1.3' package, from http://www.freedb.org/, by:
//   Jeremy D. Zawodny <Jeremy@Zawodny.com>
//   Byron Ellacott <rodent@route-qn.uqnga.org.au> 
//
unsigned RDCdPlayer::GetCddbSum(int n) 
{
  unsigned ret;
  
  ret=0;
  while(n>0) {
    ret+=(n%10);
    n/=10;
  }
  return ret;
}


unsigned RDCdPlayer::GetCddbDiscId() 
{
  int i; 
  unsigned t=0;
  unsigned n=0;
  
  i=0;
  while(i<cdrom_track_count) {
    n=n+GetCddbSum((cdrom_track_start[i].msf.minute*60)+ 
		   cdrom_track_start[i].msf.second);
    i++;
  }
  t=((cdrom_track_start[cdrom_track_count].msf.minute*60)+
     cdrom_track_start[cdrom_track_count].msf.second)-
    ((cdrom_track_start[0].msf.minute*60)+cdrom_track_start[0].msf.second);
  return ((n%0xff)<<24|t<<8|cdrom_track_count);
}


void RDCdPlayer::PushButton(RDCdPlayer::ButtonOp op,int track)
{
  cdrom_button_queue.push(op);
  cdrom_track_queue.push(track);
  if(!cdrom_button_timer->isActive()) {
    cdrom_button_timer->start(RDCDPLAYER_BUTTON_DELAY,true);
  }
}


void RDCdPlayer::Profile(const QString &msg)
{
  if(cdrom_profile_msgs!=NULL) {
    fprintf(cdrom_profile_msgs,"%s | RDCdPlayer::%s\n",
	    (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	    (const char *)msg);
  }
}

