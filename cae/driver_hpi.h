// driver_hpi.h
//
// caed(8) driver for AudioScience HPI audio devices.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef DRIVER_HPI_H
#define DRIVER_HPI_H

#include <rdconfig.h>

#include "driver.h"

#ifdef HPI
#include <rdhpisoundcard.h>
#include <rdhpiplaystream.h>
#include <rdhpirecordstream.h>
#endif  // HPI

class DriverHpi : public Driver
{
  Q_OBJECT
 public:
  DriverHpi(QObject *parent=0);
  ~DriverHpi();
  QString version() const;
  bool initialize(unsigned *next_cardnum);
  void updateMeters();
  bool loadPlayback(int card,QString wavename,int *stream);
  bool unloadPlayback(int card,int stream);
  bool playbackPosition(int card,int stream,unsigned pos);
  bool play(int card,int stream,int length,int speed,bool pitch,
	       bool rates);
  bool stopPlayback(int card,int stream);
  bool timescaleSupported(int card);
  bool loadRecord(int card,int port,int coding,int chans,int samprate,
		     int bitrate,QString wavename);
  bool unloadRecord(int card,int port,unsigned *len);
  bool record(int card,int port,int length,int thres);
  bool stopRecord(int card,int port);
  bool setClockSource(int card,int src);
  bool setInputVolume(int card,int stream,int level);
  bool setOutputVolume(int card,int stream,int port,int level);
  bool fadeOutputVolume(int card,int stream,int port,int level,
				int length);
  bool setInputLevel(int card,int port,int level);
  bool setOutputLevel(int card,int port,int level);
  bool setInputMode(int card,int stream,int mode);
  bool setOutputMode(int card,int stream,int mode);
  bool setInputVoxLevel(int card,int stream,int level);
  bool setInputType(int card,int port,int type);
  bool getInputStatus(int card,int port);
  bool getInputMeters(int card,int port,short levels[2]);
  bool getOutputMeters(int card,int port,short levels[2]);
  bool getStreamOutputMeters(int card,int stream,short levels[2]);
  bool setPassthroughLevel(int card,int in_port,int out_port,
				   int level);
  void getOutputPosition(int card,unsigned *pos);

 private:
#ifdef HPI
  RDHPISoundCard *d_sound_card;
  RDHPIRecordStream *d_record_streams[RD_MAX_CARDS][RD_MAX_STREAMS];
  RDHPIPlayStream *d_play_streams[RD_MAX_CARDS][RD_MAX_STREAMS];
#endif  // HPI
};


#endif  // DRIVER_HPI_H
