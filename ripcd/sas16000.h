// sas16000.h
//
// A Rivendell switcher driver for the SAS 16000(D) Audio Switcher
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

#ifndef SAS16000_H
#define SAS16000_H

#include <vector>

#include <q3socket.h>
#include <qhostaddress.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define SAS16000_MAX_LENGTH 256

class Sas16000 : public Switcher
{
 Q_OBJECT
 public:
  Sas16000(RDMatrix *matrix,QObject *parent=0);
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
  char sas_buffer[SAS16000_MAX_LENGTH];
  unsigned sas_ptr;
  int sas_matrix;
  int sas_ipport;
  int sas_inputs;
  int sas_outputs;
  int sas_gpis;
  int sas_gpos;
};


#endif  // SAS16000_H
