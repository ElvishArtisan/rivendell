// am16.h
//
// A Rivendell switcher driver for the 360 Systems AM16
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: am16.h,v 1.1.2.1 2013/06/28 00:33:34 cvs Exp $
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
//
//  -- Driver Design Rationale  FFG 6/27/2013 --
//
// The AM16 was originally designed for use by musicians rather than
// broadcasters (it has a port for a foot pedal, for crying out loud!).
// Thus, it assumes that crosspoint changes are applied in batches
// ('patches' in MIDI parlance), necessitating a surprisingly complex
// procedure to change a single crosspoint without disturbing any of
// the others.  The approach this driver takes is as follows:
//
//  1) Request a copy of the crosspoint map for the patch defined by
//     AM16_PATCH_NUMBER from the AM16.
//  2) If we don't get the crosspoint map after waiting AM16_TIMEOUT_INTERVAL
//     milliseconds, abort the operation (this to guard against unplugged
//     MIDI cables and such).
//  3) Change the requested crosspoints on the map, then use it to update
//     patches AM16_PATCH_NUMBER and AM16_PATCH_NUMBER+1 on the AM16.
//  4) Change the current patch on the AM16 to AM16_PATCH_NUMBER+1 and then
//     immediately back to AM16_PATCH_NUMBER.  (This bit of goofiness is
//     required because the AM16 will not apply changes to a patch to
//     the actual crosspoints while that patch is active.)
//

#ifndef AM16_H
#define AM16_H

#include <vector>

#include <qtimer.h>
#include <qsocketnotifier.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define AM16_SYSEX_START 0xF0
#define AM16_SYSEX_END 0xF7
#define AM16_SYSTEMS_ID 0x1C          // 360 Systems
#define AM16_DEVICE_NUMBER 0x04       // AM16/B (rev 2.00 or greater)
#define AM16_DEVICE_ADDRESS 0x01      // Configured in the hardware
#define AM16_PATCH_NUMBER 0x00        // Program to use for crosspoint updates
#define AM16_TIMEOUT_INTERVAL 1000    // How long to wait for crosspoint map

class Am16 : public Switcher
{
 Q_OBJECT
 public:
  Am16(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~Am16();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void readyReadData(int sock);
  void timeoutData();

 private:
  void ProcessMessage(char *msg,int len);
  int bt_inputs;
  int bt_outputs;
  int bt_midi_socket;
  bool bt_sysex_active;
  char bt_data_buffer[1024];
  int bt_data_ptr;
  std::vector<int> bt_pending_inputs;
  std::vector<int> bt_pending_outputs;
  QSocketNotifier *bt_notifier;
  QTimer *bt_timeout_timer;
};


#endif  // AM16_H
