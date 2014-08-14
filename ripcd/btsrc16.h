// btsrc16.h
//
// A Rivendell switcher driver for the BroadcastTools SRC-16
//
//   (C) Copyright 2002-2005,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: btsrc16.h,v 1.3 2010/08/03 23:39:25 cvs Exp $
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

#ifndef BTSRC16_H
#define BTSRC16_H

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>
#include <rdoneshot.h>

#include <switcher.h>

#define BTSRC16_UNIT_ID 0
#define BTSRC16_POLL_INTERVAL 100
#define BTSRC16_GPIO_PINS 16


class BtSrc16 : public Switcher
{
 Q_OBJECT
 public:
  BtSrc16(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~BtSrc16();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void processStatus();
  void gpiOneshotData(int value);
  void gpoOneshotData(int value);

 private:
  RDTTYDevice *bt_device;
  RDOneShot *bt_gpi_oneshot;
  RDOneShot *bt_gpo_oneshot;
  int bt_matrix;
  int bt_gpis;
  int bt_gpos;
  int bt_istate;
  bool bt_gpi_state[BTSRC16_GPIO_PINS];
  bool bt_gpi_mask[BTSRC16_GPIO_PINS];
};


#endif  // BTSRC16_H
