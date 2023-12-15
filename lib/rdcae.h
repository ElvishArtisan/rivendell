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

#include <QLabel>
#include <QList>
#include <QObject>

#include <rd.h>
#include <rdcmd_cache.h>
#include <rdstation.h>
#include <rdconfig.h>

class __RDCae_PlayChannel
{
 public:
  __RDCae_PlayChannel(unsigned card,unsigned port);
  unsigned card() const;
  unsigned port() const;
  unsigned position() const;
  void setPosition(unsigned pos);
  void getStreamLevels(short lvls[2]);
  void setStreamLevels(short left_lvl,short right_lvl);
  bool operator==(const __RDCae_PlayChannel &other) const;

 private:
  unsigned d_card;
  unsigned d_port;
  unsigned d_position;
  short d_stream_levels[2];
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
  bool connectHost(QString *err_msg);
  void enableMetering(QList<int> *cards);
  unsigned loadPlay(unsigned card,unsigned port,const QString &name);
  void unloadPlay(unsigned serial);
  void positionPlay(unsigned serial,int pos);
  void play(unsigned serial,unsigned length,int speed,bool pitch);
  void stopPlay(unsigned serial);
  void loadRecord(int card,int stream,QString name,AudioCoding coding,
		  int chan,int samp_rate,int bit_rate);
  void unloadRecord(int card,int stream);
  void record(int card,int stream,unsigned length,int threshold);
  void stopRecord(int card,int stream);
  void setClockSource(int card,RDCae::ClockSource src);
  void setInputVolume(int card,int stream,int level);
  void setOutputVolume(unsigned serial,int level);
  void fadeOutputVolume(unsigned serial,int level,int length);
  void setInputLevel(int card,int port,int level);
  void setOutputLevel(int card,int port,int level);
  void setInputMode(int card,int stream,RDCae::ChannelMode mode);
  void setOutputMode(int card,int stream,RDCae::ChannelMode mode);
  void setInputVOXLevel(int card,int stream,int level);
  void setInputType(int card,int port,RDCae::SourceType type);
  void setPassthroughVolume(int card,int in_port,int out_port,int level);
  bool inputStatus(int card,int port) const;
  void inputMeterUpdate(int card,int port,short levels[2]);
  void outputMeterUpdate(int card,int port,short levels[2]);
  void outputStreamMeterUpdate(unsigned serial,short levels[2]);
  unsigned playPosition(unsigned serial);
  void requestTimescale(int card);
  bool playPortStatus(int card,int port,unsigned except_serial=0) const;

 signals:
  void isConnected(bool state);
  void playLoaded(unsigned serial);
  void playPositioned(unsigned serial,unsigned pos);
  void playing(unsigned serial);
  void playStopped(unsigned serial);
  void playUnloaded(unsigned serial);
  void recordLoaded(int card,int stream);
  void recording(int card,int stream);
  void recordStopped(int card,int stream);
  void recordUnloaded(int card,int stream,unsigned msecs);
  void gpiInputChanged(int line,bool state);
  void connected(bool state);
  void inputStatusChanged(int card,int stream,bool state);
  void playPositionChanged(unsigned serial,unsigned sample);
  void timescalingSupported(int card,bool state);
  void playPortStatusChanged(int card,int port,bool status);

 private slots:
  void readyData();
  
 private:
  void SendCommand(QString cmd);
  void DispatchCommand(const QString &cmd);
  bool SerialCheck(unsigned serial,int linenum) const;
  void UpdateMeters();
  unsigned next_serial_number;
  int cae_socket;
  bool debug;
  QByteArray cae_accum;
  bool cae_connected;
  bool input_status[RD_MAX_CARDS][RD_MAX_PORTS];
  int cae_meter_socket;
  uint16_t cae_meter_port;
  int cae_meter_base_port;
  int cae_meter_port_range;
  short cae_input_levels[RD_MAX_CARDS][RD_MAX_PORTS][2];
  short cae_output_levels[RD_MAX_CARDS][RD_MAX_PORTS][2];
  QMap<unsigned,__RDCae_PlayChannel *> cae_play_channels;
  RDStation *cae_station;
  RDConfig *cae_config;
};


#endif  // RDCAE_H
