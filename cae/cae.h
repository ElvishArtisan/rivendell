// cae.h
//
// The Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cae.h,v 1.79.4.4 2012/11/30 16:14:58 cvs Exp $
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


#ifndef CAE_H
#define CAE_H

#include <sys/types.h>
#include <pthread.h>
#include <stdint.h>

#include <soundtouch/SoundTouch.h>

#include <qobject.h>
#include <qstring.h>
#include <qsocketdevice.h>
#include <qserversocket.h>
#include <qsignalmapper.h>
#include <qtimer.h>
#include <qprocess.h>

#include <rdwavefile.h>
#include <rdsocket.h>

#ifdef HPI
#include <rdhpisoundcard.h>
#include <rdhpiplaystream.h>
#include <rdhpirecordstream.h>
#endif  // HPI

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

#ifdef JACK
#include <jack/jack.h>
#endif  // JACK

#ifdef HAVE_TWOLAME
#include <twolame.h>
#endif  // HAVE_TWOLAME
#ifdef HAVE_MAD
#include <mad.h>
#endif  // HAVE_MAD

#include <rd.h>
#include <rdconfig.h>
#include <rdstation.h>

//
// Debug Options
//
//#define PRINT_COMMANDS

//
// Global CAE Definitions
//
#define RINGBUFFER_SIZE 262144
#define CAED_USAGE "[-d]\n\nSupplying the '-d' flag will set 'debug' mode, causing caed(8) to stay\nin the foreground and print debugging info on standard output.\n" 

//
// Function Prototypes
//
void LogLine(RDConfig::LogPriority prio,const QString &line);
void SigHandler(int signum);
extern RDConfig *rd_config;


class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);
  ~MainObject();

 public slots:
  void newConnection(int fd);

 private slots:
  void socketData(int);
  void socketKill(int);
  void statePlayUpdate(int card,int stream,int state);
  void stateRecordUpdate(int card,int stream,int state);
  void updateMeters();
  
 private:
  void InitMixers();
  void ParseCommand(int);
  void DispatchCommand(int);
  void KillSocket(int);
  void BroadcastCommand(const char *);
  void EchoCommand(int,const char *);
  void EchoArgs(int,const char);
  bool CheckDaemon(QString);
  pid_t GetPid(QString pidfile);
  int GetNextHandle();
  int GetHandle(int ch,int *card,int *stream);
  int GetHandle(int card,int stream);
  void ProbeCaps(RDStation *station);
  void ClearDriverEntries(RDStation *station);
  void SendMeterLevelUpdate(const QString &type,int cardnum,int portnum,
			    short levels[]);
  void SendStreamMeterLevelUpdate(int cardnum,int streamnum,short levels[]);
  void SendMeterPositionUpdate(int cardnum,unsigned pos[]);
  void SendMeterOutputStatusUpdate();
  void SendMeterOutputStatusUpdate(int card,int port,int stream);
  void SendMeterUpdate(const char *msg,unsigned len);
  bool debug;
  unsigned system_sample_rate;
  Q_INT16 tcp_port;
  QServerSocket *server;
  QSocketDevice *meter_socket;
  RDSocket *socket[CAE_MAX_CONNECTIONS];
  Q_UINT16 meter_port[CAE_MAX_CONNECTIONS];
  char args[CAE_MAX_CONNECTIONS][CAE_MAX_ARGS][CAE_MAX_LENGTH];
  int istate[CAE_MAX_CONNECTIONS];
  int argnum[CAE_MAX_CONNECTIONS];
  int argptr[CAE_MAX_CONNECTIONS];
  bool auth[CAE_MAX_CONNECTIONS];
  RDStation::AudioDriver cae_driver[RD_MAX_CARDS];
  int record_owner[RD_MAX_CARDS][RD_MAX_STREAMS];
  int record_length[RD_MAX_CARDS][RD_MAX_STREAMS];
  int record_threshold[RD_MAX_CARDS][RD_MAX_STREAMS];
  int play_owner[RD_MAX_CARDS][RD_MAX_STREAMS];
  int play_length[RD_MAX_CARDS][RD_MAX_STREAMS];
  int play_speed[RD_MAX_CARDS][RD_MAX_STREAMS];
  bool play_pitch[RD_MAX_CARDS][RD_MAX_STREAMS];
  bool port_status[RD_MAX_CARDS][RD_MAX_PORTS];
  bool output_status_flag[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_STREAMS];
  struct {
    int card;
    int stream;
    int owner;
  } play_handle[256];
  int next_play_handle;

  //
  // HPI Driver
  //
 private:
  void hpiInit(RDStation *station);
  void hpiFree();
  QString hpiVersion();
  bool hpiLoadPlayback(int card,QString wavename,int *stream);
  bool hpiUnloadPlayback(int card,int stream);
  bool hpiPlaybackPosition(int card,int stream,unsigned pos);
  bool hpiPlay(int card,int stream,int length,int speed,bool pitch,
	       bool rates);
  bool hpiStopPlayback(int card,int stream);
  bool hpiTimescaleSupported(int card);
  bool hpiLoadRecord(int card,int port,int coding,int chans,int samprate,
		     int bitrate,QString wavename);
  bool hpiUnloadRecord(int card,int stream,unsigned *len);
  bool hpiRecord(int card,int stream,int length,int thres);
  bool hpiStopRecord(int card,int stream);
  bool hpiSetClockSource(int card,int src);
  bool hpiSetInputVolume(int card,int stream,int level);
  bool hpiSetOutputVolume(int card,int stream,int port,int level);
  bool hpiFadeOutputVolume(int card,int stream,int port,int level,int length);
  bool hpiSetInputLevel(int card,int port,int level);
  bool hpiSetOutputLevel(int card,int port,int level);
  bool hpiSetInputMode(int card,int stream,int mode);
  bool hpiSetOutputMode(int card,int stream,int mode);
  bool hpiSetInputVoxLevel(int card,int stream,int level);
  bool hpiSetInputType(int card,int port,int type);
  bool hpiGetInputStatus(int card,int port);
  bool hpiGetInputMeters(int card,int port,short levels[2]);
  bool hpiGetOutputMeters(int card,int port,short levels[2]);
  bool hpiGetStreamOutputMeters(int card,int stream,short levels[2]);
  bool hpiSetPassthroughLevel(int card,int in_port,int out_port,int level);
  void hpiGetOutputPosition(int card,unsigned *pos);
#ifdef HPI
  RDHPISoundCard *sound_card;
  RDHPIRecordStream *record[RD_MAX_CARDS][RD_MAX_STREAMS];
  RDHPIPlayStream *play[RD_MAX_CARDS][RD_MAX_STREAMS];
#endif  // HPI

  //
  // JACK Driver
  //
 private slots:
  void jackStopTimerData(int stream);
  void jackFadeTimerData(int stream);
  void jackRecordTimerData(int stream);

 private:
  void jackInit(RDStation *station);
  void jackFree();
  bool jackLoadPlayback(int card,QString wavename,int *stream);
  bool jackUnloadPlayback(int card,int stream);
  bool jackPlaybackPosition(int card,int stream,unsigned pos);
  bool jackPlay(int card,int stream,int length,int speed,bool pitch,
	       bool rates);
  bool jackStopPlayback(int card,int stream);
  bool jackTimescaleSupported(int card);
  bool jackLoadRecord(int card,int port,int coding,int chans,int samprate,
		     int bitrate,QString wavename);
  bool jackUnloadRecord(int card,int stream,unsigned *len);
  bool jackRecord(int card,int stream,int length,int thres);
  bool jackStopRecord(int card,int stream);
  bool jackSetInputVolume(int card,int stream,int level);
  bool jackSetOutputVolume(int card,int stream,int port,int level);
  bool jackFadeOutputVolume(int card,int stream,int port,int level,int length);
  bool jackSetInputLevel(int card,int port,int level);
  bool jackSetOutputLevel(int card,int port,int level);
  bool jackSetInputMode(int card,int stream,int mode);
  bool jackSetOutputMode(int card,int stream,int mode);
  bool jackSetInputVoxLevel(int card,int stream,int level);
  bool jackSetInputType(int card,int port,int type);
  bool jackGetInputStatus(int card,int port);
  bool jackGetInputMeters(int card,int port,short levels[2]);
  bool jackGetOutputMeters(int card,int port,short levels[2]);
  bool jackGetStreamOutputMeters(int card,int stream,short levels[2]);
  bool jackSetPassthroughLevel(int card,int in_port,int out_port,int level);
  void jackGetOutputPosition(int card,unsigned *pos);
  void jackConnectPorts(const QString &out,const QString &in);
  void jackDisconnectPorts(const QString &out,const QString &in);
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
  std::vector<QProcess *> jack_clients;
  RDWaveFile *jack_record_wave[RD_MAX_STREAMS];
  RDWaveFile *jack_play_wave[RD_MAX_STREAMS];
  short *jack_wave_buffer;
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
  int jack_offset[RD_MAX_STREAMS];
  int jack_clock_phase;
  unsigned jack_samples_recorded[RD_MAX_STREAMS];
#endif  // JACK

  //
  // ALSA Driver
  //
 private slots:
  void alsaStopTimerData(int cardstream);
  void alsaFadeTimerData(int cardstream);
  void alsaRecordTimerData(int cardport);

 private:
  void alsaInit(RDStation *station);
  void alsaFree();
  bool alsaLoadPlayback(int card,QString wavename,int *stream);
  bool alsaUnloadPlayback(int card,int stream);
  bool alsaPlaybackPosition(int card,int stream,unsigned pos);
  bool alsaPlay(int card,int stream,int length,int speed,bool pitch,
	       bool rates);
  bool alsaStopPlayback(int card,int stream);
  bool alsaTimescaleSupported(int card);
  bool alsaLoadRecord(int card,int port,int coding,int chans,int samprate,
		     int bitrate,QString wavename);
  bool alsaUnloadRecord(int card,int stream,unsigned *len);
  bool alsaRecord(int card,int stream,int length,int thres);
  bool alsaStopRecord(int card,int stream);
  bool alsaSetInputVolume(int card,int stream,int level);
  bool alsaSetOutputVolume(int card,int stream,int port,int level);
  bool alsaFadeOutputVolume(int card,int stream,int port,int level,int length);
  bool alsaSetInputLevel(int card,int port,int level);
  bool alsaSetOutputLevel(int card,int port,int level);
  bool alsaSetInputMode(int card,int stream,int mode);
  bool alsaSetOutputMode(int card,int stream,int mode);
  bool alsaSetInputVoxLevel(int card,int stream,int level);
  bool alsaSetInputType(int card,int port,int type);
  bool alsaGetInputStatus(int card,int port);
  bool alsaGetInputMeters(int card,int port,short levels[2]);
  bool alsaGetOutputMeters(int card,int port,short levels[2]);
  bool alsaGetStreamOutputMeters(int card,int stream,short levels[2]);
  bool alsaSetPassthroughLevel(int card,int in_port,int out_port,int level);
  void alsaGetOutputPosition(int card,unsigned *pos);
  void AlsaClock();
#ifdef ALSA
  bool AlsaStartCaptureDevice(QString &dev,int card,snd_pcm_t *pcm);
  bool AlsaStartPlayDevice(QString &dev,int card,snd_pcm_t *pcm);
  void AlsaInitCallback();
  int GetAlsaOutputStream(int card);
  void FreeAlsaOutputStream(int card,int stream);
  void EmptyAlsaInputStream(int card,int stream);
  void WriteAlsaBuffer(int card,int stream,short *buffer,unsigned len);
  void FillAlsaOutputStream(int card,int stream);
  struct alsa_format alsa_play_format[RD_MAX_CARDS];
  struct alsa_format alsa_capture_format[RD_MAX_CARDS];
  short alsa_input_volume_db[RD_MAX_CARDS][RD_MAX_STREAMS];
  short alsa_output_volume_db[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_STREAMS];
  short alsa_passthrough_volume_db[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_PORTS];
  short *alsa_wave_buffer;
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

  bool CheckLame();

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
  void InitMadDecoder(int card,int stream,RDWaveFile *wave);
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
};


#endif  // CAE_H
