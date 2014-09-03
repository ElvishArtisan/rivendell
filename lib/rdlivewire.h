// rdlivewire.h
//
// A LiveWire Node Driver for Rivendell
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlivewire.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDLIVEWIRE_H
#define RDLIVEWIRE_H

#include <vector>

#include <qobject.h>
#include <qsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdlivewiresource.h>
#include <rdlivewiredestination.h>

#define RDLIVEWIRE_WATCHDOG_INTERVAL 10000
#define RDLIVEWIRE_WATCHDOG_TIMEOUT 30000
#define RDLIVEWIRE_RECONNECT_MIN_INTERVAL 5000
#define RDLIVEWIRE_RECONNECT_MAX_INTERVAL 30000

class RDLiveWire : public QObject
{
  Q_OBJECT
 public:
  RDLiveWire(unsigned id,QObject *parent=0,const char *name=0);
  unsigned id() const;
  QString hostname() const;
  Q_UINT16 tcpPort() const;
  unsigned baseOutput();
  void connectToHost(const QString &hostname,Q_UINT16 port,
		     const QString &passwd,unsigned base_output);
  bool loadSettings(const QString &hostname,Q_UINT16 port,
		    const QString &passwd,unsigned base_output);
  QString deviceName() const;
  QString protocolVersion() const;
  QString systemVersion() const;
  int sources() const;
  int destinations() const;
  int channels() const;
  int gpis() const;
  int gpos() const;
  unsigned gpiChannel(int slot,int line) const;
  unsigned gpoChannel(int slot,int line) const;
  bool gpiState(int slot,int line) const;
  bool gpoState(int slot,int line) const;
  void gpiSet(int slot,int line,unsigned interval=0);
  void gpiReset(int slot,int line,unsigned interval=0);
  void gpoSet(int slot,int line,unsigned interval=0);
  void gpoReset(int slot,int line,unsigned interval=0);
  void setRoute(int src_num,int dest_slot) const;

 signals:
  void connected(unsigned id);
  void sourceChanged(unsigned id,RDLiveWireSource *src);
  void destinationChanged(unsigned id,RDLiveWireDestination *dst);
  void gpoConfigChanged(unsigned id,unsigned slot,unsigned chan);
  void gpiChanged(unsigned id,unsigned slot,unsigned line,bool state);
  void gpoChanged(unsigned id,unsigned slot,unsigned line,bool state);
  void watchdogStateChanged(unsigned id,const QString &msg);

 private slots:
  void connectedData();
  void connectionClosedData();
  void readyReadData();
  void errorData(int err);
  void gpiTimeoutData(int id);
  void gpoTimeoutData(int id);
  void watchdogData();
  void watchdogTimeoutData();
  void holdoffData();
  void resetConnectionData();

 private:
  void DespatchCommand(const QString &cmd);
  void ReadVersion(const QString &cmd);
  void ReadSources(const QString &cmd);
  void ReadDestinations(const QString &cmd);
  void ReadGpis(const QString &cmd);
  void ReadGpos(const QString &cmd);
  void ReadGpioConfig(const QString &cmd);
  int ParseString(const QString &str,int ptr,QString *tag,
		  QString *value) const;
  QString PruneUrl(const QString &str);
  void ResetConnection();
  int GetHoldoff();
  unsigned live_id;
  unsigned live_base_output;
  QString live_hostname;
  Q_UINT16 live_tcp_port;
  QString live_password;
  QString live_device_name;
  QString live_protocol_version;
  QString live_system_version;
  int live_sources;
  int live_destinations;
  int live_channels;
  int live_gpis;
  std::vector<unsigned *> live_gpi_channels;
  std::vector<unsigned *> live_gpo_channels;
  std::vector<bool *> live_gpi_states;
  std::vector<bool *> live_gpo_states;
  std::vector<QTimer *>live_gpi_timers;
  std::vector<QTimer *>live_gpo_timers;
  int live_gpos;
  QSocket *live_socket;
  char live_buf[RD_LIVEWIRE_MAX_CMD_LENGTH];
  int live_ptr;
  bool live_connected;
  bool live_watchdog_state;
  QTimer *live_watchdog_timer;
  QTimer *live_watchdog_timeout_timer;
  QTimer *live_holdoff_timer;
  int live_load_ver_count;
};


#endif  // RDLIVEWIRE_H
