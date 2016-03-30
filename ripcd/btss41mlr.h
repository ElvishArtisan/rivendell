// btss41mlr.h
//
// A Rivendell switcher driver for the BroadcastTools SS 4.1 MLR
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef BTSS41MLR_H
#define BTSS41MLR_H

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>
#include <rdoneshot.h>

#include <switcher.h>

#define BTSS41MLR_UNIT_ID 0
#define BTSS41MLR_POLL_INTERVAL 100
#define BTSS41MLR_GPIO_PINS 16

class BtSs41Mlr : public Switcher
{
 Q_OBJECT
 public:
  BtSs41Mlr(RDMatrix *matrix,QObject *parent=0);
  ~BtSs41Mlr();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void readyReadData();
  void gpiOneshotData(int value);
  void gpoOneshotData(int value);

 private:
  void ProcessStatus(const QString &msg);
  RDTTYDevice *bt_device;
  RDOneShot *bt_gpi_oneshot;
  RDOneShot *bt_gpo_oneshot;
  int bt_matrix;
  int bt_inputs;
  int bt_outputs;
  int bt_gpis;
  int bt_gpos;
  int bt_istate;
  bool bt_gpi_state[BTSS41MLR_GPIO_PINS];
  bool bt_gpi_mask[BTSS41MLR_GPIO_PINS];
  QString bt_accum;
};


#endif  // BTSS41MLR_H
