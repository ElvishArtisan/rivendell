// rossnkscp.h
//
// A Rivendell switcher driver for the Ross NK switchers via the SCP/A
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

#ifndef ROSSNKSCP_H
#define ROSSNKSCP_H

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define ROSSNKSCP_MIN_GAIN -99
#define ROSSNKSCP_MAX_GAIN 28

class RossNkScp : public Switcher
{
 Q_OBJECT
 public:
 RossNkScp(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~RossNkScp();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private:
  RDTTYDevice *ross_device;
  int ross_inputs;
  int ross_outputs;
  int ross_breakaway;
};


#endif  // ROSSNKSCP_H
