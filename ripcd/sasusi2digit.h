// sasusi2digit.h
//
// A Rivendell switcher driver for the SAS USI Protocol (2 digit)
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SASUSI2DIGIT_H
#define SASUSI2DIGIT_H

#include <vector>

#include <qsocket.h>
#include <qhostaddress.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define SASUSI2DIGIT_MAX_LENGTH 256

class SasUsi2Digit : public Switcher
{
 Q_OBJECT
 public:
  SasUsi2Digit(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private:
  void SendCommand(char *str);
  QString PrettifyCommand(const char *cmd) const;
  RDTTYDevice *sas_device;
  char sas_buffer[SASUSI2DIGIT_MAX_LENGTH];
  unsigned sas_ptr;
  int sas_matrix;
  int sas_ipport;
  int sas_inputs;
  int sas_outputs;
  int sas_gpis;
  int sas_gpos;
};


#endif  // SASUSI2DIGIT_H
