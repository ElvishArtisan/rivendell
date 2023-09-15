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

#include <QList>
#include <QMap>
#include <QObject>
#include <QSignalMapper>
#include <QUdpSocket>

#include <rdconfig.h>

#include "connection.h"
#include "session.h"

class CaeServer : public QObject
{
  Q_OBJECT;
 public:
  CaeServer(QObject *parent=0);
  bool metersEnabled(int id,unsigned card) const;
  void setMetersEnabled(int id,unsigned card,bool state);
  bool bind(const QHostAddress &addr,uint16_t port);
  void sendCommand(const QString &cmd);
  void sendCommand(const SessionId &dest,const QString &cmd);

 signals:
  //  void connectionDropped(int id);

  //
  // New Signals
  //
  void connectionClosed(const SessionId &sid);
  void startPlaybackReq(const SessionId &sid,const QString &cutname,
			unsigned cardnum,unsigned portnum,
			int start_pos,int end_pos,int speed);
  void playPositionReq(const SessionId &sid,int position);
  void playPauseReq(const SessionId &sid);
  void playResumeReq(const SessionId &sid);
  void playStopReq(const SessionId &sid);
  void playSetOutputVolumeReq(const SessionId &sid,int level);
  void playFadeOutputVolumeReq(const SessionId &sid,int level,int length);
  void recordCueReq(const SessionId &sid,const QString &cutname,
		    unsigned cardnum,unsigned portnum,
		    int coding,int channels,int bitrate);
  void recordStartReq(const SessionId &sid,int length,int threshold);
  void recordCueAndStartReq(const SessionId &sid,const QString &cutname,
			    unsigned cardnum,unsigned portnum,
			    int coding,int channels,int bitrate,
			    int length,int threshold);
  void recordStopReq(const SessionId &sid);
  void getInputStatusReq(const SessionId &origin,
			 unsigned card,unsigned port);
  void setAudioPassthroughLevelReq(const QHostAddress &src_addr,
				   unsigned card,unsigned input,
				   unsigned output,int level);
  void updateAudioPortsReq();

  //
  // Old Signals
  //
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
  void setOutputStatusFlagReq(int id,unsigned card,unsigned port,
			      unsigned stream,bool state);
  void openRtpCaptureChannelReq(int id,unsigned card,unsigned port,uint16_t udp_port,
				unsigned samprate,unsigned chans);
  void meterEnableReq(const QHostAddress &addr,uint16_t udp_port,
		      const QList<unsigned> &cards);

 private slots:
  void readyReadData();
  void connectionExpiredData(const SessionId &sid);
  //  void connectionClosedData(int id);

 private:
  bool ProcessCommand(const QHostAddress &src_addr,uint16_t src_port,
		      const QString &cmd);
  QMap<SessionId,Connection *> cae_connections;
  QSignalMapper *cae_ready_read_mapper;
  QUdpSocket *d_server_socket;
};


#endif  // CAE_SERVER_H
