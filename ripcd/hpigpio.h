// hpigpio.h
//
// A Rivendell switcher driver for AudioScience HPI GPIO devices
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef HPIGPIO_H
#define HPIGPIO_H

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>
#include <rdoneshot.h>

#ifdef HPI
#include <asihpi/hpi.h>
#endif  // HPI

#include "switcher.h"

#define HPIGPIO_POLL_INTERVAL 100
#define HPIGPIO_GPIO_PINS 16

class HpiGpio : public Switcher
{
 Q_OBJECT
 public:
  HpiGpio(RDMatrix *matrix,QObject *parent=0);
  ~HpiGpio();
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
#ifdef HPI
  void UpdateDb(RDMatrix *matrix) const;
  hpi_err_t LogHpi(hpi_err_t err,int lineno);
  hpi_handle_t hpi_mixer;
  hpi_handle_t hpi_gpis_param;
  hpi_handle_t hpi_gpos_param;
#endif  // HPI
  RDOneShot *hpi_gpi_oneshot;
  RDOneShot *hpi_gpo_oneshot;
  int hpi_matrix;
  int hpi_card;
  int hpi_gpis;
  int hpi_gpos;
  int hpi_istate;
  bool hpi_gpi_state[HPIGPIO_GPIO_PINS];
  bool hpi_gpi_mask[HPIGPIO_GPIO_PINS];
};


#endif  // HPIGPIO_H
