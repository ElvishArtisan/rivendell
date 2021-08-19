// caedriver.h
//
// Abstract base class for caed(8) audio driver implementations.
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

#ifndef CAEDRIVER_H
#define CAEDRIVER_H

#ifdef HAVE_TWOLAME
#include <twolame.h>
#endif  // HAVE_TWOLAME
#ifdef HAVE_MAD
#include <mad.h>
#endif  // HAVE_MAD

#include <QList>
#include <QObject>
#include <QSignalMapper>

#include <rdapplication.h>
#include <rdwavefile.h>

#define RINGBUFFER_SIZE 262144

extern void SigHandler(int signum);

class CaeDriver : public QObject
{
  Q_OBJECT
 public:
  CaeDriver(RDStation::AudioDriver type,QObject *parent=0);
  RDStation::AudioDriver driverType() const;
  bool hasCard(int cardnum) const;
  virtual QString version() const=0;
  virtual bool initialize(unsigned *next_cardnum)=0;;

 public:
  virtual bool loadPlayback(int card,QString wavename,int *stream)=0;
  virtual bool unloadPlayback(int card,int stream)=0;
  virtual bool playbackPosition(int card,int stream,unsigned pos)=0;
  virtual bool play(int card,int stream,int length,int speed,bool pitch,
	       bool rates)=0;
  virtual bool stopPlayback(int card,int stream)=0;
  virtual bool timescaleSupported(int card)=0;
  virtual bool loadRecord(int card,int port,int coding,int chans,int samprate,
		     int bitrate,QString wavename)=0;
  virtual bool unloadRecord(int card,int stream,unsigned *len)=0;
  virtual bool record(int card,int stream,int length,int thres)=0;
  virtual bool stopRecord(int card,int stream)=0;
  virtual bool setClockSource(int card,int src)=0;
  virtual bool setInputVolume(int card,int stream,int level)=0;
  virtual bool setOutputVolume(int card,int stream,int port,int level)=0;
  virtual bool fadeOutputVolume(int card,int stream,int port,int level,
				int length)=0;
  virtual bool setInputLevel(int card,int port,int level)=0;
  virtual bool setOutputLevel(int card,int port,int level)=0;
  virtual bool setInputMode(int card,int stream,int mode)=0;
  virtual bool setOutputMode(int card,int stream,int mode)=0;
  virtual bool setInputVoxLevel(int card,int stream,int level)=0;
  virtual bool setInputType(int card,int port,int type)=0;
  virtual bool getInputStatus(int card,int port)=0;
  virtual bool getInputMeters(int card,int port,short levels[2])=0;
  virtual bool getOutputMeters(int card,int port,short levels[2])=0;
  virtual bool getStreamOutputMeters(int card,int stream,short levels[2])=0;
  virtual bool setPassthroughLevel(int card,int in_port,int out_port,
				   int level)=0;
  virtual void getOutputPosition(int card,unsigned *pos)=0;

 signals:
  void playStateChanged(int card,int stream,int state);
  void recordStateChanged(int card,int stream,int state);

 public slots:
  virtual void processBuffers();

 protected slots:
  void statePlayUpdate(int card,int stream,int state);
  void stateRecordUpdate(int card,int stream,int state);

 protected:
  void addCard(unsigned cardnum);
  unsigned systemSampleRate() const;
  RDConfig *config() const;
  //
  // TwoLAME Encoder
  //
  bool LoadTwoLame();
  bool InitTwoLameEncoder(int card,int stream,int chans,int samprate,
			  int bitrate);
  void FreeTwoLameEncoder(int card,int stream);
  void *twolame_handle;
#ifdef HAVE_TWOLAME
  twolame_options *(*twolame_init)(void);
  void (*twolame_set_mode)(twolame_options *,TWOLAME_MPEG_mode);
  void (*twolame_set_num_channels)(twolame_options *,int);
  void (*twolame_set_in_samplerate)(twolame_options *,int);
  void (*twolame_set_out_samplerate)(twolame_options *,int);
  void (*twolame_set_bitrate)(twolame_options *,int);
  int (*twolame_init_params)(twolame_options *);
  void (*twolame_close)(twolame_options **);
  int (*twolame_encode_buffer_interleaved)(twolame_options *,const short int[],
					   int,unsigned char *,int);
  int (*twolame_encode_buffer_float32_interleaved)
    (twolame_options *,const float[],int,unsigned char *,int);
  int (*twolame_encode_flush)(twolame_options *,unsigned char *,int);
  int (*twolame_set_energy_levels)(twolame_options *,int);
  twolame_options *twolame_lameopts[RD_MAX_CARDS][RD_MAX_STREAMS];
#endif  // HAVE_TWOLAME

  //
  // MAD Decoder
  //
  bool LoadMad();
  bool InitMadDecoder(int card,int stream,RDWaveFile *wave);
  void FreeMadDecoder(int card,int stream);
  void *mad_handle;
#ifdef HAVE_MAD
  void (*mad_stream_init)(struct mad_stream *);
  void (*mad_frame_init)(struct mad_frame *);
  void (*mad_synth_init)(struct mad_synth *);
  void (*mad_stream_buffer)(struct mad_stream *,unsigned char const *,
			    unsigned long);
  int (*mad_frame_decode)(struct mad_frame *, struct mad_stream *);
  void (*mad_synth_frame)(struct mad_synth *, struct mad_frame const *);
  void (*mad_frame_finish)(struct mad_frame *);
  void (*mad_stream_finish)(struct mad_stream *);
  struct mad_stream mad_stream[RD_MAX_CARDS][RD_MAX_STREAMS];
  struct mad_frame mad_frame[RD_MAX_CARDS][RD_MAX_STREAMS];
  struct mad_synth mad_synth[RD_MAX_CARDS][RD_MAX_STREAMS];
  bool mad_active[RD_MAX_CARDS][RD_MAX_STREAMS];
  int mad_frame_size[RD_MAX_CARDS][RD_MAX_STREAMS];
  int mad_left_over[RD_MAX_CARDS][RD_MAX_STREAMS];
  unsigned char *mad_mpeg[RD_MAX_CARDS][RD_MAX_STREAMS];
#endif  // HAVE_MAD

 private:
  RDStation::AudioDriver d_driver_type;
  QList<unsigned> d_cards;
  unsigned d_system_sample_rate;
};


#endif  // CAEDRIVER_H
