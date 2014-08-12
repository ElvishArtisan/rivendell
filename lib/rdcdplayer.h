// rdcdplayer.h
//
// Abstract a Linux CDROM Device.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcdplayer.h,v 1.4.8.2 2014/01/10 18:52:24 cvs Exp $
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

#ifndef RDCDPLAYER_H
#define RDCDPLAYER_H

#include <stdio.h>
#include <linux/cdrom.h>

#include <queue>

#include <qdialog.h>
#include <qtimer.h>
#include <rdcddbrecord.h>

//
// Driver Settings
//
#define RDCDPLAYER_CLOCK_INTERVAL 100
#define RDCDPLAYER_BUTTON_DELAY 100

class RDCdPlayer : public QObject
{
  Q_OBJECT
  public:
   enum Status {NoStatusInfo=CDS_NO_INFO,NoDriveDisc=CDS_NO_DISC,
		    TrayOpen=CDS_TRAY_OPEN,NotReady=CDS_DRIVE_NOT_READY,
		    Ok=CDS_DISC_OK};
   enum Medium {NoMediumInfo=CDS_NO_INFO,NoMediumLoaded=CDS_NO_DISC,
		AudioDisc=CDS_AUDIO,Data1=CDS_DATA_1,Data2=CDS_DATA_2,
		Xa21=CDS_XA_2_1,Xa22=CDS_XA_2_2,Mixed=CDS_MIXED};
   enum State {NoStateInfo=0,Stopped=1,Playing=2,Paused=3};
   enum PlayMode {Single=0,Continuous=1};
   RDCdPlayer(FILE *profile_msgs,QWidget *parent=0,const char *name=0);
   ~RDCdPlayer();
   QString device() const;
   void setDevice(QString device);
   bool open();
   void close();
   RDCdPlayer::Status status();
   RDCdPlayer::Medium medium();
   int tracks() const;
   bool isAudio(int track) const;
   int trackLength(int track) const;
   unsigned trackOffset(int track) const;
   RDCdPlayer::State state() const;
   int leftVolume();
   int rightVolume();
   RDCdPlayer::PlayMode playMode() const;
   void setPlayMode(RDCdPlayer::PlayMode mode);
   void setCddbRecord(RDCddbRecord *);

  public slots:
   void lock();
   void unlock();
   void eject();
   void play(int track);
   void pause();
   void stop();
   void setLeftVolume(int vol);
   void setRightVolume(int vol);

  signals:
   void ejected();
   void mediaChanged();
   void played(int track);
   void paused();
   void stopped();
   void leftVolumeChanged(int vol);
   void rightVolumeChanged(int vol);

  private slots:
   void buttonTimerData();
   void clockData();

  private:
   enum ButtonOp {Play=0,Pause=1,Resume=2,Stop=3,Eject=4,Lock=5,Unlock=6};
   void PushButton(RDCdPlayer::ButtonOp op,int track=-1);
   void Profile(const QString &msg);
   void ReadToc();
   unsigned GetCddbSum(int);
   unsigned GetCddbDiscId();
   QString cdrom_device;
   int cdrom_fd;
   QTimer *cdrom_clock;
   QTimer *cdrom_button_timer;
   RDCdPlayer::State cdrom_state;
   int cdrom_track;
   int cdrom_track_count;
   union cdrom_addr *cdrom_track_start;
   bool *cdrom_audio_track;
   RDCdPlayer::PlayMode cdrom_play_mode;
   union cdrom_addr cdrom_rip_ptr;
   union cdrom_addr cdrom_rip_end;
   bool cdrom_old_state;
   int cdrom_audiostatus;
   unsigned cdrom_disc_id;
   FILE *cdrom_profile_msgs;
   std::queue<int> cdrom_button_queue;
   std::queue<int> cdrom_track_queue;
};


#endif  // RDCDPLAYER_H
