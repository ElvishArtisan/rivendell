// local_macros.cpp
//
// A Dedicated Cart Slot Utility for Rivendell.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: local_macros.cpp,v 1.2.2.3 2012/11/28 21:44:08 cvs Exp $
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

#include "rdcartslots.h"

void MainWidget::RunLocalMacros(RDMacro *rml)
{
  bool ok=false;
  unsigned slotnum;
  unsigned cartnum;
  unsigned len;

  if(rml->role()!=RDMacro::Cmd) {
    return;
  }

  switch(rml->command()) {
  case RDMacro::DL:
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    slotnum=rml->arg(0).toUInt(&ok)-1;
    if((!ok)||(slotnum>=panel_slots.size())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(panel_slots[slotnum]->slotOptions()->mode()!=
       RDSlotOptions::CartDeckMode) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    cartnum=rml->arg(1).toUInt(&ok);
    if((!ok)||(cartnum>999999)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(cartnum==0) {
      panel_slots[slotnum]->unload();
    }
    else {
      if(!panel_slots[slotnum]->load(cartnum)) {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  panel_ripc->sendRml(rml);
	}
	return;
      }
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      panel_ripc->sendRml(rml);
    }
    break;

  case RDMacro::DP:
    if(rml->argQuantity()!=1) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    slotnum=rml->arg(0).toUInt(&ok)-1;
    if((!ok)||(slotnum>=panel_slots.size())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(panel_slots[slotnum]->slotOptions()->mode()!=
       RDSlotOptions::CartDeckMode) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(!panel_slots[slotnum]->play()) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      panel_ripc->sendRml(rml);
    }
    break;

  case RDMacro::DS:
    if(rml->argQuantity()!=1) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    slotnum=rml->arg(0).toUInt(&ok)-1;
    if((!ok)||(slotnum>=panel_slots.size())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(panel_slots[slotnum]->slotOptions()->mode()!=
       RDSlotOptions::CartDeckMode) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(!panel_slots[slotnum]->stop()) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      panel_ripc->sendRml(rml);
    }
    break;

  case RDMacro::DX:
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    slotnum=rml->arg(0).toUInt(&ok)-1;
    if((!ok)||(slotnum>=panel_slots.size())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    len=rml->arg(1).toUInt(&ok);
    if(!ok) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(!panel_slots[slotnum]->breakAway(len)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	panel_ripc->sendRml(rml);
      }
      return;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      panel_ripc->sendRml(rml);
    }
    break;

  default:
    break;
  }
}
