// rdcae.h
//
// Connection to the Rivendell Core Audio Engine
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCAE_H
#define RDCAE_H

#include <QList>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#include <rd.h>
#include <rdcmd_cache.h>
#include <rdstation.h>
#include <rdconfig.h>

class __RDCaeMeterPoint
{
 public:
  __RDCaeMeterPoint();
  short levels[2];
};




class RDCae : public QObject
{
 Q_OBJECT
 public:
  enum ClockSource {InternalClock=0,AesEbuClock=1,SpDiffClock=2,WordClock=4};
  enum ChannelMode {Normal=0,Swap=1,LeftOnly=2,RightOnly=3};
  enum SourceType {Analog=0,AesEbu=1};
  enum AudioCoding {Pcm16=0,MpegL1=1,MpegL2=2,MpegL3=3,Pcm24=4};
  RDCae(RDStation *station,RDConfig *config,QObject *parent=0);
  ~RDCae();
  void connectToHost(int timeout_msecs=-1);
  void enableMetering(QList<int> *cards);
  int startPlayback(const QString &cutname,int cardnum,int portnum,
		    int start_pos,int end_pos,int speed);
  void positionPlay(int serial,int pos);
  void pausePlayback(int serial);
  void resumePlayback(int serial);
  void stopPlayback(int serial);
  /*
  bool loadPlay(int card,QString name,int *stream,int *handle);
  void unloadPlay(int handle);
  void positionPlay(int handle,int msec);
  void play(int handle,unsigned length,int speed,bool pitch);
  void stopPlay(int handle);
  */

  void loadRecord(int card,int stream,QString name,AudioCoding coding,
		  int chan,int samp_rate,int bit_rate);
  void unloadRecord(int card,int stream);
  void record(int card,int stream,unsigned length,int threshold);
  void stopRecord(int card,int stream);
  void setOutputVolume(int card,int stream,int port,int level);
  void setOutputPort(int card,int stream,int port);
  void fadeOutputVolume(int card,int stream,int port,int level,int length);
  void setPassthroughVolume(int card,int in_port,int out_port,int level);
  bool inputStatus(int card,int port) const;
  void inputMeterUpdate(int card,int port,short levels[2]);
  void outputMeterUpdate(int card,int port,short levels[2]);
  //  void outputStreamMeterUpdate(int card,int stream,short levels[2]);
  void outputStreamMeterUpdate(int serial,short levels[2]);
  unsigned playPosition(int handle);
  void requestTimescale(int card);
  bool playPortActive(int card,int port,int except_stream=-1);
  void setPlayPortActive(int card,int port,int stream);

 signals:
  //  void isConnected(bool state);
  //  void playLoaded(int handle);
  void playPositioned(int handle,unsigned msec);
  //  void playing(int handle);
  void playStarted(int serial);
  void playbackStopped(int serial);
  //  void playStopped(int handle);
  void playUnloaded(int handle);
  void recordLoaded(int card,int stream);
  void recording(int card,int stream);
  void recordStopped(int card,int stream);
  void recordUnloaded(int card,int stream,unsigned msecs);
  void gpiInputChanged(int line,bool state);
  void connected(bool state);
  void inputStatusChanged(int card,int stream,bool state);
  void playPositionChanged(int handle,unsigned sample);
  void timescalingSupported(int card,bool state);

 private slots:
   void readyReadData();
   //  void readyData();
   //  void readyData(int *stream,int *handle,QString name);
  void clockData();
  void keepaliveData();
  
 private:
  void SendCommand(const QString &cmd);
  //  void SendCommand(QString cmd);
  //  void DispatchCommand(RDCmdCache *cmd);
  void ProcessCommand(const QString &cmd);
  int CardNumber(const char *arg);
  int StreamNumber(const char *arg);
  int GetHandle(const char *arg);
  void UpdateMeters();
  //  int cae_socket;
  bool debug;
  char args[CAE_MAX_ARGS][CAE_MAX_LENGTH];
  int argnum;
  int argptr;
  bool cae_connected;
  bool input_status[RD_MAX_CARDS][RD_MAX_PORTS];
  int cae_handle[RD_MAX_CARDS][RD_MAX_STREAMS];
  unsigned cae_pos[RD_MAX_CARDS][RD_MAX_STREAMS];
  QUdpSocket *cae_meter_socket;
  int cae_meter_base_port;
  int cae_meter_port_range;
  short cae_input_levels[RD_MAX_CARDS][RD_MAX_PORTS][2];
  short cae_output_levels[RD_MAX_CARDS][RD_MAX_PORTS][2];

  //  short cae_stream_output_levels[RD_MAX_CARDS][RD_MAX_PORTS][2];
  QMap<int,__RDCaeMeterPoint *> cae_stream_output_levels;

  unsigned cae_output_positions[RD_MAX_CARDS][RD_MAX_STREAMS];
  bool cae_output_status_flags[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_STREAMS];
  std::vector<RDCmdCache> delayed_cmds;
  RDStation *cae_station;
  RDConfig *cae_config;

  QUdpSocket *cae_socket;
  QTimer *cae_keepalive_timer;
  int cae_next_serial_number;
};


#endif  // RDCAE_H
