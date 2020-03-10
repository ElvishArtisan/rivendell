// btgpi16.h
//
// A Rivendell switcher driver for the BroadcastTools GPI-16
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef BTGPI16_H
#define BTGPI16_H

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>
#include <rdoneshot.h>

#include <switcher.h>

#define BTGPI16_UNIT_ID 0
#define BTGPI16_GPIO_PINS 16

class BtGpi16 : public Switcher
{
 Q_OBJECT
 public:
  BtGpi16(RDMatrix *matrix,QObject *parent=0);
  ~BtGpi16();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void processStatus();

 private:
  RDTTYDevice *bt_device;
  int bt_matrix;
  int bt_gpis;
  int bt_istate;
  bool bt_gpi_state[BTGPI16_GPIO_PINS];
  bool bt_gpi_mask[BTGPI16_GPIO_PINS];
};


#endif  // BTGPI16_H
