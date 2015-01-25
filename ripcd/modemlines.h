// modemlines.h
//
// A Rivendell switcher driver for using TTY modem lines for GPIO
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef MODEMLINES_H
#define MODEMLINES_H

#include <qtimer.h>
#include <qsignalmapper.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdoneshot.h>
#include <rdtty.h>

#include <switcher.h>

#define MODEMLINES_POLL_INTERVAL 100

class ModemLines : public Switcher
{
 Q_OBJECT
 public:
  ModemLines(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~ModemLines();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void pollTtyData();
  void gpiOneshotData(int value);
  void gpoResetData(int gpo);

 private:
  int gpio_matrix;
  int gpio_gpis;
  int gpio_gpos;
  int gpio_fd;
  int gpio_poll_state;
  QTimer *gpio_poll_timer;
  unsigned gpio_gpi_table[4];
  int gpio_gpo_table[2];
  QSignalMapper *gpio_gpo_mapper;
  QTimer *gpio_gpo_timers[2];
  bool gpio_gpo_pending_states[2];
  RDTty *gpio_tty;
  RDOneShot *gpio_gpi_oneshot;
  bool gpio_gpi_mask[4];
};


#endif  // LOCAL_GPIO_H
