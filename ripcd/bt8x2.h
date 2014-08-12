// bt8x2.h
//
// A Rivendell switcher driver for the BroadcastTools 8x2
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: bt8x2.h,v 1.9 2010/08/03 23:39:25 cvs Exp $
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

#ifndef BT8X2_H
#define BT8X2_H

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define BT8X2_MIN_GAIN -99
#define BT8X2_MAX_GAIN 28

class Bt8x2 : public Switcher
{
 Q_OBJECT
 public:
 Bt8x2(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~Bt8x2();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private:
  RDTTYDevice *bt_device;
  int bt_inputs;
  int bt_outputs;
};


#endif  // BT8X2_H
