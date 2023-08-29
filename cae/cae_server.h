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
  void sendCommand(int id,const QString &cmd);

 signals:
  void connectionDropped(int id);
  void loadPlaybackReq(int id,unsigned card,const QString &name);
  void unloadPlaybackReq(int id,unsigned handle);
  void playPositionReq(int id,unsigned handle,unsigned pos);
  void playReq(int id,unsigned handle,unsigned length,unsigned speed,
	       unsigned pitch_flag);
  void stopPlaybackReq(int id,unsigned handle);
  void timescalingSupportReq(int id,unsigned card);
  void loadRecordingReq(int id,unsigned card,unsigned port,unsigned coding,
			unsigned channels,unsigned samprate,unsigned bitrate,
			const QString &name);
  void unloadRecordingReq(int id,unsigned card,unsigned stream);
  void recordReq(int id,unsigned card,unsigned stream,unsigned len,
		 int threshold_level);
  void stopRecordingReq(int id,unsigned card,unsigned stream);
  void setOutputPortReq(int id,unsigned card,unsigned stream,unsigned port,
  			int level);
  void setOutputVolumeReq(int id,unsigned card,unsigned stream,int port,
			  int level);
  void fadeOutputVolumeReq(int id,unsigned card,unsigned stream,unsigned port,
			   int level,unsigned length);
  void getInputStatusReq(int id,unsigned card,unsigned port);
  void setAudioPassthroughLevelReq(int id,unsigned card,unsigned input,
				   unsigned output,int level);
  void updateAudioPortsReq(int id);
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
