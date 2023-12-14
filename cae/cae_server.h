// cae_server.h
//
// Network server for caed(8).
//
//   (C) Copyright 2019-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CAE_SERVER_H
#define CAE_SERVER_H

#include <stdint.h>

#include <qlist.h>
#include <qmap.h>
#include <qobject.h>
#include <qsignalmapper.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>

#include <rdconfig.h>

class CaeServerConnection
{
 public:
  CaeServerConnection(QTcpSocket *sock);
  ~CaeServerConnection();
  QTcpSocket *socket;
  bool authenticated;
  QString accum;
  uint16_t meter_port;
  bool meters_enabled[RD_MAX_CARDS];
  unsigned play_serial;
  unsigned play_stream;
  bool belongsTo(QTcpSocket *sock,unsigned serial) const;
};




class CaeServer : public QObject
{
  Q_OBJECT;
 public:
  CaeServer(RDConfig *config,QObject *parent=0);
  QList<int> connectionIds() const;
  QHostAddress peerAddress(int id) const;
  uint16_t peerPort(int id) const;
  uint16_t meterPort(int id) const;
  void setMeterPort(int id,uint16_t port);
  bool metersEnabled(int id,unsigned card) const;
  void setMetersEnabled(int id,unsigned card,bool state);
  bool listen(const QHostAddress &addr,uint16_t port);
  void sendCommand(const QString &cmd);
  void sendCommand(uint64_t phandle,const QString &cmd);
  void sendCommand(int id,const QString &cmd);

 signals:
  void connectionDropped(int id);
  void loadPlaybackReq(uint64_t phandle,unsigned card,unsigned port,
		       const QString &name);
  void unloadPlaybackReq(uint64_t phandle);
  void playPositionReq(uint64_t phandle,unsigned pos);
  void playReq(uint64_t phandle,unsigned length,unsigned speed,
	       unsigned pitch_flag);
  void stopPlaybackReq(uint64_t phandle);
  void timescalingSupportReq(int id,unsigned card);
  void loadRecordingReq(int id,unsigned card,unsigned port,unsigned coding,
			unsigned channels,unsigned samprate,unsigned bitrate,
			const QString &name);
  void unloadRecordingReq(int id,unsigned card,unsigned stream);
  void recordReq(int id,unsigned card,unsigned stream,unsigned len,
		 int threshold_level);
  void stopRecordingReq(int id,unsigned card,unsigned stream);
  void setInputVolumeReq(int id,unsigned card,unsigned stream,int level);
  void setOutputVolumeReq(uint64_t phandle,int level);
  void fadeOutputVolumeReq(uint64_t phandle,int level,unsigned length);
  void setInputLevelReq(int id,unsigned card,unsigned port,int level);
  void setOutputLevelReq(int id,unsigned card,unsigned port,int level);
  void setInputModeReq(int id,unsigned card,unsigned stream,unsigned mode);
  void setOutputModeReq(int id,unsigned card,unsigned stream,unsigned mode);
  void setInputVoxLevelReq(int id,unsigned card,unsigned stream,int level);
  void setInputTypeReq(int id,unsigned card,unsigned port,unsigned type);
  void getInputStatusReq(int id,unsigned card,unsigned port);
  void setAudioPassthroughLevelReq(int id,unsigned card,unsigned input,
				   unsigned output,int level);
  void setClockSourceReq(int id,unsigned card,int input);
  void setOutputStatusFlagReq(int id,unsigned card,unsigned port,
			      unsigned stream,bool state);
  void openRtpCaptureChannelReq(int id,unsigned card,unsigned port,uint16_t udp_port,
				unsigned samprate,unsigned chans);
  void meterEnableReq(int id,uint16_t udp_port,const QList<unsigned> &cards);

 private slots:
  void newConnectionData();
  void readyReadData(int id);
  void connectionClosedData(int id);

 private:
  bool ProcessCommand(int id,const QString &cmd);
  QMap<int,CaeServerConnection *> cae_connections;
  QTcpServer *cae_server;
  QSignalMapper *cae_ready_read_mapper;
  QSignalMapper *cae_connection_closed_mapper;
  RDConfig *cae_config;
};


#endif  // CAE_SERVER_H
