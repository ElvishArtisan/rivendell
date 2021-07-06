// rdslotoptions.cpp
//
// Container class for RDCartSlot options
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

  set_stationname=stationname;
  set_slotno=slotno;

  //
  // Ensure that the DB record exists
  //
  sql=QString("select `ID` from `CARTSLOTS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(stationname)+"')&&"+
    QString().sprintf("(`SLOT_NUMBER`=%u)",slotno);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    sql=QString("insert into `CARTSLOTS` set ")+
      "`STATION_NAME`='"+RDEscapeString(stationname)+"',"+
      QString().sprintf("`SLOT_NUMBER`=%u",slotno);
    RDSqlQuery::apply(sql);
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


QString RDSlotOptions::outputPortLabel() const
{
  return set_output_port_label;
}


bool RDSlotOptions::load()
{
  bool ret=false;
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "`CARTSLOTS`.`CARD`,"+                 // 00
    "`CARTSLOTS`.`INPUT_PORT`,"+           // 01
    "`CARTSLOTS`.`OUTPUT_PORT`,"+          // 02
    "`CARTSLOTS`.`MODE`,"+                 // 03
    "`CARTSLOTS`.`DEFAULT_MODE`,"+         // 04
    "`CARTSLOTS`.`HOOK_MODE`,"+            // 05
    "`CARTSLOTS`.`DEFAULT_HOOK_MODE`,"+    // 06
    "`CARTSLOTS`.`STOP_ACTION`,"+          // 07
    "`CARTSLOTS`.`DEFAULT_STOP_ACTION`,"+  // 08
    "`CARTSLOTS`.`CART_NUMBER`,"+          // 09
    "`CARTSLOTS`.`DEFAULT_CART_NUMBER`,"+  // 10
    "`CARTSLOTS`.`SERVICE_NAME`,"+         // 11
    "`AUDIO_OUTPUTS`.`LABEL` "+            // 12
    "from `CARTSLOTS` left join `AUDIO_OUTPUTS` "+
    "on `CARTSLOTS`.`OUTPUT_PORT`=`AUDIO_OUTPUTS`.`PORT_NUMBER` where "+
    "`AUDIO_OUTPUTS`.`STATION_NAME`='"+RDEscapeString(set_stationname)+"' && "+
    "`AUDIO_OUTPUTS`.`CARD_NUMBER`=`CARTSLOTS`.`CARD` && "+
    "`AUDIO_OUTPUTS`.`PORT_NUMBER`=`CARTSLOTS`.`OUTPUT_PORT` && "+
    QString().sprintf("`CARTSLOTS`.`SLOT_NUMBER`=%u",set_slotno);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=true;

    //
    // Channel Assignments
    //
    set_card=q->value(0).toInt();
    set_input_port=q->value(1).toInt();
    set_output_port=q->value(2).toInt();
    set_output_port_label=q->value(12).toString();

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

  sql=QString("update `CARTSLOTS` set ")+
    QString().sprintf("`MODE`=%d,",set_mode)+
    QString().sprintf("`HOOK_MODE`=%d,",set_hook_mode)+
    QString().sprintf("`STOP_ACTION`=%d,",set_stop_action)+
    QString().sprintf("`CART_NUMBER`=%d,",set_cart_number)+
    "`SERVICE_NAME`='"+RDEscapeString(set_service)+"' "+
    "where (`STATION_NAME`='"+RDEscapeString(set_stationname)+"')&&"+
    QString().sprintf("(`SLOT_NUMBER`=%u)",set_slotno);
  RDSqlQuery::apply(sql);
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
