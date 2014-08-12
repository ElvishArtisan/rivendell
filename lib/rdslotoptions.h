// rdslotoptions.h
//
// Container class for RDCartSlot options
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdslotoptions.h,v 1.2.2.4 2012/11/28 01:57:38 cvs Exp $
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

#ifndef RDSLOTOPTIONS_H
#define RDSLOTOPTIONS_H

#include <qstring.h>

class RDSlotOptions
{
 public:
  enum Mode {CartDeckMode=0,BreakawayMode=1,LastMode=2};
  enum StopAction {UnloadOnStop=0,RecueOnStop=1,LoopOnStop=2,LastStop=3};
  RDSlotOptions(const QString &stationname,unsigned slotno);
  RDSlotOptions::Mode mode() const;
  void setMode(RDSlotOptions::Mode mode);
  bool hookMode() const;
  void setHookMode(bool state);
  RDSlotOptions::StopAction stopAction() const;
  void setStopAction(RDSlotOptions::StopAction action);
  int cartNumber() const;
  void setCartNumber(int cart);
  QString service() const;
  void setService(const QString &str);
  int card() const;
  int inputPort() const;
  int outputPort() const;
  bool load();
  void save() const;
  void clear();
  static QString modeText(RDSlotOptions::Mode mode);
  static QString stopActionText(RDSlotOptions::StopAction action);

 private:
  Mode set_mode;
  bool set_hook_mode;
  StopAction set_stop_action;
  int set_cart_number;
  QString set_service;
  int set_card;
  int set_input_port;
  int set_output_port;
  QString set_stationname;
  unsigned set_slotno;
};


#endif  // RDSLOTOPTIONS_H
