// kernelgpio.h
//
// A Rivendell switcher driver for the Kernel GPIO interface. 
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef KERNELGPIO_H
#define KERNELGPIO_H

#include <vector>

#include <qsignalmapper.h>

#include <rdkernelgpio.h>
#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdoneshot.h>

#include <switcher.h>

class KernelGpio : public Switcher
{
 Q_OBJECT
 public:
  KernelGpio(RDMatrix *matrix,QObject *parent=0);
  ~KernelGpio();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void gpiChangedData(int line,bool state);
  //  void gpoChangedData(int line,bool state);
  void gpiOneshotData(int value);
  void gpoResetData(int line);

 private:
  RDKernelGpio *gpio_gpio;
  RDOneShot *gpio_gpi_oneshot;
  int gpio_matrix;
  int gpio_gpis;
  int gpio_gpos;
  bool gpio_open;
  std::vector<bool> gpio_gpi_mask;
  QSignalMapper *gpio_reset_mapper;
  std::vector<QTimer *> gpio_reset_timers;
  std::vector<int> gpio_reset_states;
};


#endif  // KERNELGPIO_H
