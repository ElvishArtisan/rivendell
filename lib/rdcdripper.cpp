// rdcdripper.cpp
//
// Rip an audio from from CD
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcdripper.cpp,v 1.4.6.3 2014/01/10 02:25:35 cvs Exp $
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

/*
extern "C" {
#include <cdda_interface.h>
}
*/

#include <qapplication.h>
#include <qdatetime.h>

#include <rdpaths.h>
#include <rdcdripper.h>
#include <rd.h>

RDCdRipper::RDCdRipper(FILE *profile_msgs,QObject *parent)
  : QObject(parent)
{
  conv_profile_msgs=profile_msgs;
  conv_aborting=false;
}


RDCdRipper::~RDCdRipper()
{
}


void RDCdRipper::setDevice(const QString &device)
{
  conv_device=device;
  Profile("using device \""+device+"\"");
}


void RDCdRipper::setDestinationFile(const QString &filename)
{
  conv_dst_filename=filename;  
}


int RDCdRipper::totalSteps() const
{
  return 4;
}


RDCdRipper::ErrorCode RDCdRipper::rip(int track)
{
  return rip(track,track);
}


RDCdRipper::ErrorCode RDCdRipper::rip(int first_track,int last_track)
{
  SNDFILE *sf_dst=NULL;
  SF_INFO sf_dst_info;
  cdrom_drive *drive;
  int err;
  char *msg;
  long sect=0;
  long n;
  short buffer[63504];
  long start;
  long end;
  long size=RIPPER_MAX_SECTORS;
  int step;
  int step_size;

  //
  // Open the CD
  //
  if((drive=cdda_identify(conv_device,1,&msg))==NULL) {
    return RDCdRipper::ErrorNoDevice;
  }
  if((err=cdda_open(drive))!=0) {
    return RDCdRipper::ErrorNoDisc;
  }
  if((first_track>=cdda_tracks(drive))||(last_track>=cdda_tracks(drive))||
     (last_track<first_track)) {
    cdda_close(drive);
    return RDCdRipper::ErrorNoTrack;
  }
  start=cdda_track_firstsector(drive,first_track+1);
  end=cdda_track_lastsector(drive,last_track+1);
  step_size=(end-start)/4;
  step=0;

  //
  // Open Destination
  //
  memset(&sf_dst_info,0,sizeof(sf_dst_info));
  sf_dst_info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_32;
  sf_dst_info.channels=cdda_track_channels(drive,first_track+1);
  sf_dst_info.samplerate=44100;
  if((sf_dst=sf_open(conv_dst_filename,SFM_WRITE,&sf_dst_info))==NULL) {
    cdda_close(drive);
    return RDCdRipper::ErrorNoDestination;
  }

  //
  // Rip Track
  //
  emit progressChanged(step);
  qApp->processEvents();
  for(long i=start;i<end;i+=RIPPER_MAX_SECTORS) {
    if((end-i)<RIPPER_MAX_SECTORS) {
      size=end-i;
    }
    n=cdda_read(drive,buffer,i,size);
    sf_writef_short(sf_dst,buffer,CD_FRAMESIZE_RAW*n/(2*sf_dst_info.channels));
    sect+=n;
    if(sect>((step+1)*step_size)) {
      emit progressChanged(++step);
      qApp->processEvents();
      if(conv_aborting) {
	sf_close(sf_dst);
	unlink(conv_dst_filename);
	cdda_close(drive);
	return RDCdRipper::ErrorAborted;
      }
    }
  }

  //
  // Clean Up
  //
  sf_close(sf_dst);
  cdda_close(drive);

  return RDCdRipper::ErrorOk;
}


QString RDCdRipper::errorText(RDCdRipper::ErrorCode err)
{
  QString ret="Unknown Error";

  switch(err) {
  case RDCdRipper::ErrorOk:
    ret="OK";
    break;

  case RDCdRipper::ErrorNoDevice:
    ret="No such device";
    break;

  case RDCdRipper::ErrorNoDestination:
    ret="Unable to create output file";
    break;

  case RDCdRipper::ErrorInternal:
    ret="Internal error";
    break;

  case RDCdRipper::ErrorNoDisc:
    ret="No disc found";
    break;

  case RDCdRipper::ErrorNoTrack:
    ret="No such track";
    break;

  case RDCdRipper::ErrorAborted:
    ret="Rip Aborted";
    break;
  }
  return ret;
}


void RDCdRipper::abort()
{
  conv_aborting=true;
}


void RDCdRipper::Profile(const QString &msg)
{
  if(conv_profile_msgs!=NULL) {
    fprintf(conv_profile_msgs,"%s | RDCdPlayer::%s\n",
	    (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	    (const char *)msg);
  }
}
