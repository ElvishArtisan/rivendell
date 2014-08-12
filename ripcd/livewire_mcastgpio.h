// livewire_mcastgpio.h
//
// A Rivendell multicast GPIO driver for LiveWire networks.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: livewire_mcastgpio.h,v 1.1.2.1 2013/11/17 03:40:27 cvs Exp $
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

#ifndef LIVEWIRE_MCASTGPIO_H
#define LIVEWIRE_MCASTGPIO_H

#include <stdint.h>
#include <sys/socket.h>

#include <map>
#include <vector>

#include <qsocket.h>
#include <qhostaddress.h>
#include <qsignalmapper.h>
#include <qtimer.h>
#include <qsocketnotifier.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdlivewire.h>
#include <rdoneshot.h>

#include <switcher.h>

class LiveWireMcastGpio : public Switcher
{
 Q_OBJECT
 public:
  LiveWireMcastGpio(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~LiveWireMcastGpio();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void gpioActivatedData(int sock);
  void gpiTimeoutData(int gpo);
  void gpoTimeoutData(int gpo);

 private:
  void ProcessGpi(const QHostAddress &src_addr,int chan,unsigned line,
		  bool state,bool pulse);
  void ProcessGpo(int chan,unsigned line,bool state);
  QString AddressString(uint32_t addr) const;
  void subscribe(const QHostAddress &addr) const;
  void subscribe(const uint32_t addr) const;
  void unsubscribe(const QHostAddress &addr) const;
  void unsubscribe(const uint32_t addr) const;
  QString livewire_stationname;
  int livewire_matrix;
  unsigned livewire_gpios;
  QHostAddress livewire_interface_addr;
  int livewire_gpio_read_socket;
  int livewire_gpio_write_socket;
  QSocketNotifier *livewire_gpio_notify;
  std::map<int,int> livewire_source_numbers;
  std::map<int,QHostAddress> livewire_surface_addresses;
  uint32_t livewire_gpio_send_serial;
  std::map<uint32_t,uint32_t> livewire_gpio_recv_serials;
  QSignalMapper *livewire_gpi_timer_mapper;
  std::vector<QTimer *> livewire_gpi_timers;
  QSignalMapper *livewire_gpo_timer_mapper;
  std::vector<QTimer *> livewire_gpo_timers;
  std::vector<bool> livewire_gpo_states;
};


#endif  // LIVEWIRE_MCASTGPIO_H
