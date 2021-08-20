// driver_alsa.h
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

#ifndef DRIVER_ALSA_H
#define DRIVER_ALSA_H

#include <rdconfig.h>
#include <rdwavefile.h>

#include "driver.h"

#ifdef ALSA
#include <alsa/asoundlib.h>
struct alsa_format {
  int card;
  pthread_t thread;
  snd_pcm_t *pcm;
  unsigned channels;
  unsigned capture_channels;
  snd_pcm_uframes_t buffer_size;
  snd_pcm_format_t format;
  unsigned sample_rate;
  char *card_buffer;
  char *passthrough_buffer;
  unsigned card_buffer_size;
  unsigned periods;
  bool exiting;
};
#endif  // ALSA

class DriverAlsa : public Driver
{
  Q_OBJECT
 public:
  DriverAlsa(QObject *parent=0);
  ~DriverAlsa();
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
  void stopTimerData(int cardstream);
  void fadeTimerData(int cardstream);
  void recordTimerData(int cardport);

 private:
#ifdef ALSA
  bool AlsaStartCaptureDevice(QString &dev,int card,snd_pcm_t *pcm);
  bool AlsaStartPlayDevice(QString &dev,int card,snd_pcm_t *pcm);
  void AlsaInitCallback();
  int GetAlsaOutputStream(int card);
  void FreeAlsaOutputStream(int card,int stream);
  void EmptyAlsaInputStream(int card,int stream);
  void WriteAlsaBuffer(int card,int stream,short *buffer,unsigned len);
  void FillAlsaOutputStream(int card,int stream);
  void AlsaClock();
  struct alsa_format alsa_play_format[RD_MAX_CARDS];
  struct alsa_format alsa_capture_format[RD_MAX_CARDS];
  short alsa_input_volume_db[RD_MAX_CARDS][RD_MAX_STREAMS];
  short alsa_output_volume_db[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_STREAMS];
  short alsa_passthrough_volume_db[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_PORTS];
  short *alsa_wave_buffer;
  uint8_t *alsa_wave24_buffer;
  RDWaveFile *alsa_record_wave[RD_MAX_CARDS][RD_MAX_STREAMS];
  RDWaveFile *alsa_play_wave[RD_MAX_CARDS][RD_MAX_STREAMS];
  int alsa_offset[RD_MAX_CARDS][RD_MAX_STREAMS];
  QTimer *alsa_fade_timer[RD_MAX_CARDS][RD_MAX_STREAMS];
  QTimer *alsa_stop_timer[RD_MAX_CARDS][RD_MAX_STREAMS];
  QTimer *alsa_record_timer[RD_MAX_CARDS][RD_MAX_PORTS];
  bool alsa_fade_up[RD_MAX_CARDS][RD_MAX_STREAMS];
  short alsa_fade_volume_db[RD_MAX_CARDS][RD_MAX_STREAMS];
  short alsa_fade_increment[RD_MAX_CARDS][RD_MAX_STREAMS];
  int alsa_fade_port[RD_MAX_CARDS][RD_MAX_STREAMS];
  unsigned alsa_samples_recorded[RD_MAX_CARDS][RD_MAX_STREAMS];
#endif  // ALSA
};


#endif  // DRIVER_ALSA_H
