// rdslotoptions.cpp
//
// Container class for RDCartSlot options
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdslotoptions.cpp,v 1.2.2.5 2012/11/28 18:49:36 cvs Exp $
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

#include <qobject.h>

#include <rddb.h>
#include <rdescape_string.h>

#include "rdslotoptions.h"

RDSlotOptions::RDSlotOptions(const QString &stationname,unsigned slotno)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  set_stationname=stationname;
  set_slotno=slotno;

  //
  // Ensure that the DB record exists
  //
  sql=QString("select ID from CARTSLOTS where (STATION_NAME=\"")+
    RDEscapeString(stationname)+"\")&&"+
    QString().sprintf("(SLOT_NUMBER=%u)",slotno);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    sql=QString("insert into CARTSLOTS set ")+
      "STATION_NAME=\""+RDEscapeString(stationname)+"\","+
      QString().sprintf("SLOT_NUMBER=%u",slotno);
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;

  clear();
}


RDSlotOptions::Mode RDSlotOptions::mode() const
{
  return set_mode;
}


void RDSlotOptions::setMode(RDSlotOptions::Mode mode)
{
  set_mode=mode;
}


bool RDSlotOptions::hookMode() const
{
  return set_hook_mode;
}


void RDSlotOptions::setHookMode(bool state)
{
  set_hook_mode=state;
}


RDSlotOptions::StopAction RDSlotOptions::stopAction() const
{
  return set_stop_action;
}


void RDSlotOptions::setStopAction(RDSlotOptions::StopAction action)
{
  set_stop_action=action;
}


int RDSlotOptions::cartNumber() const
{
  return set_cart_number;
}


void RDSlotOptions::setCartNumber(int cart)
{
  set_cart_number=cart;
}


QString RDSlotOptions::service() const
{
  return set_service;
}


void RDSlotOptions::setService(const QString &str)
{
  set_service=str;
}


int RDSlotOptions::card() const
{
  return set_card;
}


int RDSlotOptions::inputPort() const
{
  return set_input_port;
}


int RDSlotOptions::outputPort() const
{
  return set_output_port;
}


bool RDSlotOptions::load()
{
  bool ret=false;
  QString sql;
  RDSqlQuery *q;

  sql=QString("select CARD,INPUT_PORT,OUTPUT_PORT,")+
    "MODE,DEFAULT_MODE,HOOK_MODE,DEFAULT_HOOK_MODE,"+
    "STOP_ACTION,DEFAULT_STOP_ACTION,"+
    "CART_NUMBER,DEFAULT_CART_NUMBER,SERVICE_NAME from CARTSLOTS "+
    "where (STATION_NAME=\""+RDEscapeString(set_stationname)+"\")&&"+
    QString().sprintf("(SLOT_NUMBER=%u)",set_slotno);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=true;

    //
    // Channel Assignments
    //
    set_card=q->value(0).toInt();
    set_input_port=q->value(1).toInt();
    set_output_port=q->value(2).toInt();

    //
    // Mode
    //
    switch(q->value(4).toInt()) {
    case -1:
      set_mode=(RDSlotOptions::Mode)q->value(3).toInt();
      break;

    case 1:
      set_mode=RDSlotOptions::BreakawayMode;
      break;

    default:
      set_mode=RDSlotOptions::CartDeckMode;
      break;
    }

    //
    // Play Mode
    //
    switch(q->value(6).toInt()) {
    case -1:
      set_hook_mode=q->value(5).toInt()==1;
      break;

    case 1:
      set_hook_mode=true;
      break;

    default:
      set_hook_mode=false;
      break;
    }

    //
    // Stop Action
    //
    if(q->value(8).toInt()<0) {
      set_stop_action=(RDSlotOptions::StopAction)q->value(7).toInt();
    }
    else {
      set_stop_action=(RDSlotOptions::StopAction)q->value(8).toInt();
    }

    //
    // Cart
    //
    switch(q->value(10).toInt()) {
    case -1:
      set_cart_number=q->value(9).toInt();
      break;

    case 0:
      set_cart_number=0;
      break;

    default:
      set_cart_number=q->value(10).toInt();
      break;
    }

    //
    // Breakaway Service
    //
    set_service=q->value(11).toString();
  }

  return ret;
}


void RDSlotOptions::save() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update CARTSLOTS set ")+
    QString().sprintf("MODE=%d,HOOK_MODE=%d,STOP_ACTION=%d,CART_NUMBER=%d,",
		      set_mode,set_hook_mode,set_stop_action,set_cart_number)+
    "SERVICE_NAME=\""+RDEscapeString(set_service)+"\" "+
    "where (STATION_NAME=\""+RDEscapeString(set_stationname)+"\")&&"+
    QString().sprintf("(SLOT_NUMBER=%u)",set_slotno);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSlotOptions::clear()
{
  set_mode=RDSlotOptions::CartDeckMode;
  set_hook_mode=false;
  set_stop_action=RDSlotOptions::UnloadOnStop;
  set_cart_number=0;
  set_service="";
  set_card=0;
  set_input_port=0;
  set_output_port=0;
}


QString RDSlotOptions::modeText(RDSlotOptions::Mode mode)
{
  QString ret=QObject::tr("Unknown Mode");
  switch(mode) {
  case RDSlotOptions::CartDeckMode:
    ret=QObject::tr("Cart Deck");
    break;

  case RDSlotOptions::BreakawayMode:
    ret=QObject::tr("Breakaway");
    break;

  case RDSlotOptions::LastMode:
    break;
  }
  return ret;
}


QString RDSlotOptions::stopActionText(RDSlotOptions::StopAction action)
{
  QString ret=QObject::tr("Unknown Stop Action");
  switch(action) {
  case RDSlotOptions::UnloadOnStop:
    ret=QObject::tr("Unload Slot");
    break;

  case RDSlotOptions::RecueOnStop:
    ret=QObject::tr("Recue to Start");
    break;

  case RDSlotOptions::LoopOnStop:
    ret=QObject::tr("Restart Playout (Loop)");
    break;

  case RDSlotOptions::LastStop:
    break;
  }
  return ret;
}
