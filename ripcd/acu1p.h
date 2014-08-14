// acu1p.h
//
// Rivendell switcher driver for the Sine Systems ACU-1 (Prophet)
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: acu1p.h,v 1.1.2.2 2012/12/13 03:14:00 cvs Exp $
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

#ifndef ACU1P_H
#define ACU1P_H

#include <stdint.h>

#include <qsocketnotifier.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>
#include <rdoneshot.h>

#include <switcher.h>

#define ACU1P_UNIT_ID 1
#define ACU1P_POLL_INTERVAL 100
#define ACU1P_GPIO_PINS 16

class Acu1p : public Switcher
{
 Q_OBJECT
 public:
 Acu1p(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~Acu1p();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void pollData();
  void readyReadData(int sock);
  void gpiOneshotData(int value);
  void gpoOneshotData(int value);

 private:
  void ProcessGpi(uint16_t gpi_data);
  void SetRelay(int gpo,bool state);
  void PulseRelay(int gpo);
  RDTTYDevice *bt_device;
  QSocketNotifier *bt_notify;
  QTimer *bt_poll_timer;
  bool bt_gpi_state[ACU1P_GPIO_PINS];
  bool bt_gpi_mask[ACU1P_GPIO_PINS];
  uint8_t bt_gpo_mask;
  RDOneShot *bt_gpi_oneshot;
  RDOneShot *bt_gpo_oneshot;
  int bt_matrix;
  int bt_inputs;
  int bt_outputs;
  int bt_gpis;
  int bt_gpos;
};


#endif  // ACU1P_H
