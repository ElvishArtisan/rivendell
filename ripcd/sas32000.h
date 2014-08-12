// sas32000.h
//
// A Rivendell switcher driver for the SAS32000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas32000.h,v 1.11.8.1 2013/03/03 22:58:22 cvs Exp $
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

#ifndef SAS32000_H
#define SAS32000_H

#include <queue>

#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define SAS32000_MIN_GAIN -99
#define SAS32000_MAX_GAIN 28
#define SAS32000_COMMAND_DELAY 10

class Sas32000 : public Switcher
{
 Q_OBJECT
 public:
 Sas32000(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~Sas32000();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void runQueue();

 private:
  void SendCommand(RDMacro *cmd,const char *format);
  RDTTYDevice *sas_device;
  int sas_inputs;
  int sas_outputs;
  std::queue<QString> sas_commands;
  QTimer *sas_timer;
};


#endif  // SAS32000_H
