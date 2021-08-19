// jackdriver.h
//
// caed(8) driver for Advanced Linux Audio Architecture devices
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

#ifndef JACKDRIVER_H
#define JACKDRIVER_H

#include <QProcess>

#include <soundtouch/SoundTouch.h>

#include <rdconfig.h>
#include <rdmeteraverage.h>
#include <rdwavefile.h>

#include "caedriver.h"

#ifdef JACK
#include <jack/jack.h>
#endif  // JACK

class JackDriver : public CaeDriver
{
  Q_OBJECT
 public:
  JackDriver(QObject *parent=0);
  ~JackDriver();
  QString version() const;
  bool initialize(unsigned *next_cardnum);
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

 public slots:
  void processBuffers();

 private slots:
  void stopTimerData(int stream);
  void fadeTimerData(int stream);
  void recordTimerData(int stream);
  void clientStartData();

 private:
  int GetJackOutputStream();
  void FreeJackOutputStream(int stream);
  void EmptyJackInputStream(int stream,bool done);
#ifdef JACK
  void WriteJackBuffer(int stream,jack_default_audio_sample_t *buffer,
		       unsigned len,bool done);
#endif  // JACK
  void FillJackOutputStream(int stream);
  void JackClock();
  void JackSessionSetup();
  bool jack_connected;
  bool jack_activated;
#ifdef JACK
  int jack_card;
  QList<QProcess *> jack_clients;
  RDWaveFile *jack_record_wave[RD_MAX_STREAMS];
  RDWaveFile *jack_play_wave[RD_MAX_STREAMS];
  short *jack_wave_buffer;
  int *jack_wave32_buffer;
  uint8_t *jack_wave24_buffer;
  jack_default_audio_sample_t *jack_sample_buffer;
  soundtouch::SoundTouch *jack_st_conv[RD_MAX_STREAMS];
  short jack_input_volume_db[RD_MAX_STREAMS];
  short jack_output_volume_db[RD_MAX_PORTS][RD_MAX_STREAMS];
  short jack_passthrough_volume_db[RD_MAX_PORTS][RD_MAX_PORTS];
  short jack_fade_volume_db[RD_MAX_STREAMS];
  short jack_fade_increment[RD_MAX_STREAMS];
  int jack_fade_port[RD_MAX_STREAMS];
  bool jack_fade_up[RD_MAX_STREAMS];
  QTimer *jack_fade_timer[RD_MAX_STREAMS];
  QTimer *jack_stop_timer[RD_MAX_STREAMS];
  QTimer *jack_record_timer[RD_MAX_PORTS];
  QTimer *jack_client_start_timer;
  int jack_offset[RD_MAX_STREAMS];
  int jack_clock_phase;
  unsigned jack_samples_recorded[RD_MAX_STREAMS];
#endif  // JACK
};


#endif  // JACKDRIVER_H
