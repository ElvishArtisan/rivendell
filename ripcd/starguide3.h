// starguide3.h
//
// A Rivendell switcher driver for the StarGuide III Satellite Receiver
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: starguide3.h,v 1.9 2010/08/03 23:39:26 cvs Exp $
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

#ifndef STARGUIDE3_H
#define STARGUIDE3_H

#include <vector>

#include <qobject.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>
#include <starguide_feed.h>

class StarGuide3 : public Switcher
{
 Q_OBJECT
 public:
  StarGuide3(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~StarGuide3();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private:
  RDTTYDevice *sg_device;
  std::vector<StarGuideFeed> sg_feed;
  int sg_inputs;
  int sg_outputs;
};


#endif  // STARGUIDE3_H
