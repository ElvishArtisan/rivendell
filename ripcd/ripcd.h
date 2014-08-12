// ripcd.h
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ripcd.h,v 1.55 2010/08/03 23:39:26 cvs Exp $
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

#ifndef RIPCD_H
#define RIPCD_H

#include <sys/types.h>

#include <vector>

#include <qobject.h>
#include <qstring.h>
#include <qserversocket.h>
#include <qsqldatabase.h>
#include <qsocketdevice.h>
#include <qtimer.h>

#include <rdsocket.h>
#include <rdttydevice.h>
#include <rdcodetrap.h>
#include <rdstation.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <ripcd_connection.h>
#include <globals.h>
#include <switcher.h>

//
// Global RIPCD Definitions
//
#define RIPCD_MAX_LENGTH 256
#define RIPCD_RML_READ_INTERVAL 100
#define RIPCD_TTY_READ_INTERVAL 100
#define RIPCD_USAGE "[-d]\n\nSupplying the '-d' flag will set 'debug' mode, causing ripcd(8) to stay\nin the foreground and print debugging info on standard output.\n" 

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);
  ~MainObject();

 public slots:
  void newConnection(int fd);

 private slots:
  void log(RDConfig::LogPriority prio,const QString &msg);
  void sendRml(RDMacro *rml);
  void readRml();
  void gpiChangedData(int matrix,int line,bool state);
  void gpoChangedData(int matrix,int line,bool state);
  void gpiStateData(int matrix,unsigned line,bool state);
  void gpoStateData(int matrix,unsigned line,bool state);
  void ttyTrapData(int cartnum);
  void ttyScanData();
  void databaseBackup();
  void macroTimerData(int num);
  void socketData(int);
  void socketKill(int);
  void checkMaintData();
  
 private:
  void SetUser(QString username);
  void ExecCart(int cartnum);
  void ParseCommand(int);
  void DispatchCommand(int);
  void KillSocket(int);
  void EchoCommand(int,const char *);
  void BroadcastCommand(const char *);
  void EchoArgs(int,const char);
  void ReadRmlSocket(QSocketDevice *dev,RDMacro::Role role,bool echo);
  QString StripPoint(QString);
  void LoadLocalMacros();
  void RunLocalMacros(RDMacro *rml);
  void LoadGpiTable();
  void SendGpi(int ch,int matrix);
  void SendGpo(int ch,int matrix);
  void SendGpiMask(int ch,int matrix);
  void SendGpoMask(int ch,int matrix);
  void SendGpiCart(int ch,int matrix);
  void SendGpoCart(int ch,int matrix);
  void RunSystemMaintRoutine();
  void RunLocalMaintRoutine();
  int GetMaintInterval() const;
  void ForwardConvert(RDMacro *rml) const;
  bool LoadSwitchDriver(int matrix_num);
  QSqlDatabase *ripcd_db;
  QString ripcd_host;
  bool debug;
  QServerSocket *server;
  std::vector<RipcdConnection *> ripcd_conns;
  QSocketDevice *ripcd_rml_send;
  QSocketDevice *ripcd_rml_echo;
  QSocketDevice *ripcd_rml_noecho;
  QSocketDevice *ripcd_rml_reply;
  QHostAddress ripcd_host_addr;
  Switcher *ripcd_switcher[MAX_MATRICES];
  bool ripcd_gpi_state[MAX_MATRICES][MAX_GPIO_PINS];
  bool ripcd_gpo_state[MAX_MATRICES][MAX_GPIO_PINS];
  int ripcd_gpi_macro[MAX_MATRICES][MAX_GPIO_PINS][2];
  int ripcd_gpo_macro[MAX_MATRICES][MAX_GPIO_PINS][2];
  bool ripcd_gpi_mask[MAX_MATRICES][MAX_GPIO_PINS];
  bool ripcd_gpo_mask[MAX_MATRICES][MAX_GPIO_PINS];
  bool ripcd_tty_inuse[MAX_TTYS];
  int ripcd_switcher_tty[MAX_MATRICES][2];
  RDTTYDevice *ripcd_tty_dev[MAX_TTYS];
  RDTty::Termination ripcd_tty_term[MAX_TTYS];
  RDCodeTrap *ripcd_tty_trap[MAX_TTYS];
  QTimer *ripcd_backup_timer;
  bool ripc_onair_flag;
  QTimer *ripc_macro_timer[RD_MAX_MACRO_TIMERS];
  unsigned ripc_macro_cart[RD_MAX_MACRO_TIMERS];
  QTimer *ripcd_maint_timer;
};


#endif  // RIPCD_H
