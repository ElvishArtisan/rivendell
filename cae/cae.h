// cae.h
//
// The Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QTimer>
#include <QObject>
#include <QProcess>
#include <QUdpSocket>

#include <rdwavefile.h>

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

#include "caedriver.h"
#include "cae_server.h"

#ifndef HAVE_SRC_CONV
void src_int_to_float_array (const int *in, float *out, int len);
void src_float_to_int_array (const float *in, int *out, int len);
#endif  // HAVE_SRC_CONV

//
// Debug Options
//
//#define PRINT_COMMANDS

//
// Global CAE Definitions
//
#define CAED_USAGE "[-d]\n\nSupplying the '-d' flag will set 'debug' mode, causing caed(8) to stay\nin the foreground and print debugging info on standard output.\n" 

//
// Function Prototypes
//
void SigHandler(int signum);

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0);

 private slots:
  void loadPlaybackData(int id,unsigned card,const QString &name);
  void unloadPlaybackData(int id,unsigned handle);
  void playPositionData(int id,unsigned handle,unsigned pos);
  void playData(int id,unsigned handle,unsigned length,unsigned speed,
		unsigned pitch_flag);
  void stopPlaybackData(int id,unsigned handle);
  void timescalingSupportData(int id,unsigned card);
  void loadRecordingData(int id,unsigned card,unsigned port,unsigned coding,
			unsigned channels,unsigned samprate,unsigned bitrate,
			const QString &name);
  void unloadRecordingData(int id,unsigned card,unsigned stream);
  void recordData(int id,unsigned card,unsigned stream,unsigned len,
		 int threshold_level);
  void stopRecordingData(int id,unsigned card,unsigned stream);
  void setInputVolumeData(int id,unsigned card,unsigned stream,int level);
  void setOutputVolumeData(int id,unsigned card,unsigned stream,unsigned port,
			  int level);
  void fadeOutputVolumeData(int id,unsigned card,unsigned stream,unsigned port,
			   int level,unsigned length);
  void setInputLevelData(int id,unsigned card,unsigned stream,int level);
  void setOutputLevelData(int id,unsigned card,unsigned port,int level);
  void setInputModeData(int id,unsigned card,unsigned stream,unsigned mode);
  void setOutputModeData(int id,unsigned card,unsigned stream,unsigned mode);
  void setInputVoxLevelData(int id,unsigned card,unsigned stream,int level);
  void setInputTypeData(int id,unsigned card,unsigned port,unsigned type);
  void getInputStatusData(int id,unsigned card,unsigned port);
  void setAudioPassthroughLevelData(int id,unsigned card,unsigned input,
				    unsigned output,int level);
  void setClockSourceData(int id,unsigned card,int input);
  void setOutputStatusFlagData(int id,unsigned card,unsigned port,
			       unsigned stream,bool state);
  void openRtpCaptureChannelData(int id,unsigned card,unsigned port,
				 uint16_t udp_port,unsigned samprate,
				 unsigned chans);
  void meterEnableData(int id,uint16_t udp_port,const QList<unsigned> &cards);
  void statePlayUpdate(int card,int stream,int state);
  void stateRecordUpdate(int card,int stream,int state);
  void updateMeters();
  void connectionDroppedData(int id);
  
 private:
  void InitProvisioning() const;
  void InitMixers();
  void KillSocket(int);
  bool CheckDaemon(QString);
  pid_t GetPid(QString pidfile);
  int GetNextHandle();
  int GetHandle(int card,int stream);
  void ProbeCaps(RDStation *station);
  void ClearDriverEntries() const;
  void SendMeterLevelUpdate(const QString &type,int cardnum,int portnum,
			    short levels[]);
  void SendStreamMeterLevelUpdate(int cardnum,int streamnum,short levels[]);
  void SendMeterPositionUpdate(int cardnum,unsigned pos[]);
  void SendMeterOutputStatusUpdate();
  void SendMeterOutputStatusUpdate(int card,int port,int stream);
  void SendMeterUpdate(const QString &msg,int conn_id);
  CaeDriver *GetDriver(unsigned card) const;
  void MakeDriver(unsigned *next_card,RDStation::AudioDriver type);
  QList<CaeDriver *> d_drivers;
  bool debug;
  unsigned system_sample_rate;
  CaeServer *cae_server;
  int16_t tcp_port;
  QUdpSocket *meter_socket;
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

 private:
  bool CheckLame();
  bool CheckMp4Decode();

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
