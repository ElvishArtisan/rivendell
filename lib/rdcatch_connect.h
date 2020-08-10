// rdcatch_connect.h
//
// Connect to the Rivendell Netcatcher Daemon.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qlabel.h>
#include <qobject.h>
#include <qstring.h>
#include <qtcpsocket.h>

#include <rd.h>
#include <rddeck.h>
#include <rdrecording.h>

#ifndef RDCATCH_CONNECT_H
#define RDCATCH_CONNECT_H

#define CC_MAX_ARGS 10
#define CC_MAX_LENGTH 256
#define CC_HEARTBEAT_INTERVAL 15000

class RDCatchConnect : public QObject
{
 Q_OBJECT;
 public:
  RDCatchConnect(int serial,QObject *parent=0);
  ~RDCatchConnect();
  void connectHost(QString hostname,Q_UINT16 hostport,QString password);
  RDDeck::Status status(unsigned chan) const;
  int currentId(unsigned chan) const;
  void enableMetering(bool state);
  void reloadHeartbeat();
  void reloadDropboxes();
  
 public slots:
  void reset();
  void reload();
  void refresh();
  void reloadOffset();
  void stop(int deck);
  void monitor(int deck,bool state);
  void toggleMonitor(int deck);
  void setExitCode(int id,RDRecording::ExitCode code,const QString &msg);

 signals:
  void connected(int serial,bool state);
  void statusChanged(int serial,unsigned channel,RDDeck::Status status,
		     int id,const QString &cutname);
  void monitorChanged(int serial,unsigned channel,bool state);
  void meterLevel(int serial,int deck,int chan,int level);
  void eventUpdated(int id);
  void eventPurged(int id);
  void deckEventSent(int serial,int chan,int number);
  void heartbeatFailed(int id);
  
 private slots:
  void connectedData();
  void readyData();
  void heartbeatTimeoutData();
  
 private:
  void SendCommand(QString cmd);
  void DispatchCommand();
  QTcpSocket *cc_socket;
  QString cc_password;
  bool debug;
  char args[CC_MAX_ARGS][CC_MAX_LENGTH];
  int argnum;
  int argptr;
  bool cc_connected;
  RDDeck::Status cc_record_deck_status[MAX_DECKS];
  RDDeck::Status cc_play_deck_status[MAX_DECKS];
  int cc_record_id[MAX_DECKS];
  int cc_play_id[MAX_DECKS];
  int cc_serial;
  QTimer *cc_heartbeat_timer;
  bool cc_heartbeat_valid;
  bool cc_monitor_state[MAX_DECKS];
};


#endif  // RDCATCH_CONNECT_H
