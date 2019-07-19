// rdnotification.cpp
//
// A container class for a Rivendell Notification message.
//
//   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <QStringList>

#include <stdio.h>

#include "rdnotification.h"

RDNotification::RDNotification(Type type,Action action,const QVariant &id)
{
  notify_type=type;
  notify_action=action;
  notify_datetime=QDateTime(QDateTime::currentDateTime());
  notify_id=id;
  notify_arg=QVariant();
}

RDNotification::RDNotification(Type type,Action action,const QVariant &id,const QVariant &arg)
{
  notify_type=type;
  notify_action=action;
  notify_datetime=QDateTime(QDateTime::currentDateTime());
  notify_id=id;
  notify_arg=arg;
}


RDNotification::RDNotification()
{
  notify_type=RDNotification::NullType;
  notify_action=RDNotification::NoAction;
}


RDNotification::Type RDNotification::type() const
{
  return notify_type;
}


void RDNotification::setType(RDNotification::Type type)
{
  notify_type=type;
}


RDNotification::Action RDNotification::action() const
{
  return notify_action;
}


void RDNotification::setAction(RDNotification::Action action)
{
  notify_action=action;
}


QVariant RDNotification::id() const
{
  return notify_id;
}


void RDNotification::setId(const QVariant id)
{
  notify_id=id;
}


QVariant RDNotification::arg() const
{
  return notify_arg;
}


void RDNotification::setArg(const QVariant arg)
{
  notify_arg=arg;
}


QDateTime RDNotification::dateTime() const
{
  return notify_datetime;
}


void RDNotification::setDateTime(const QDateTime datetime)
{
  notify_datetime=datetime;
}


bool RDNotification::isValid() const
{
  return true;
}


bool RDNotification::read(const QString &str)
{
  notify_type=RDNotification::NullType;
  notify_action=RDNotification::NoAction;
  notify_id=QVariant();

  QStringList args=str.split(" ");
  if(args.size()>=5) {
    if(args[PosNotify]!="NOTIFY") {
      return false;
    }
    notify_datetime=QDateTime::fromString(args[PosTimeStamp],Qt::ISODate);
    for(int i=0;i<RDNotification::LastType;i++) {
      RDNotification::Type type=(RDNotification::Type)i;
      if(args[PosType]==RDNotification::typeString(type)) {
	notify_type=type;
	switch(type) {
	  case RDNotification::LogType:
          case RDNotification::DropboxType:
            notify_id=QVariant(args[PosId]);
            break;

          case RDNotification::CartType:
          case RDNotification::CatchEventType:
          case RDNotification::PypadType:
          case RDNotification::PlayoutCartNumType:
          case RDNotification::PlayoutExtIdType:
            notify_id=QVariant(args[PosId].toUInt());
            break;

          case RDNotification::LogExtIdType: 
            notify_id=QVariant(args[PosId].toUInt());
            notify_arg=QVariant(args[PosArg]);
            break;

          case RDNotification::NullType:
          case RDNotification::LastType:
            return false;
        }
      }
    }

    for(int i=0;i<RDNotification::LastAction;i++) {
      RDNotification::Action action=(RDNotification::Action)i;
      if(args[PosAction]==RDNotification::actionString(action)) {
	notify_action=action;
      }
    }
    if(notify_action==RDNotification::NoAction) {
      return false;
    }
  }
  return true;
}


QString RDNotification::write() const
{
  QStringList list;

  list.insert(PosNotify,"NOTIFY");
  list.insert(PosType,RDNotification::typeString(notify_type));
  list.insert(PosAction,RDNotification::actionString(notify_action));
  if(notify_datetime.isValid()) {
    list.insert(PosTimeStamp,QDateTime(notify_datetime).toString(Qt::ISODate));
  }
  else {
    list.insert(PosTimeStamp,QDateTime(QDateTime::currentDateTime()).toString(Qt::ISODate));
  }
  switch(notify_type) {
    case RDNotification::LogType: 
    case RDNotification::DropboxType: 
      list.insert(PosId,notify_id.toString());
      break;

    case RDNotification::CartType: 
    case RDNotification::PypadType: 
    case RDNotification::CatchEventType: 
    case RDNotification::PlayoutCartNumType: 
    case RDNotification::PlayoutExtIdType: 
      list.insert(PosId,QString().sprintf("%u",notify_id.toUInt()));
      break;

    case RDNotification::LogExtIdType: 
      list.insert(PosId,QString().sprintf("%u",notify_id.toUInt()));
      list.insert(PosArg,notify_arg.toString());
      break;

    default:
      break;
  }

  return list.join(" ");
}


QString RDNotification::typeString(RDNotification::Type type)
{
  QString ret="UNKNOWN";

  switch(type) {
  case RDNotification::CartType:
    ret="CART";
    break;

  case RDNotification::LogType:
    ret="LOG";
    break;

  case RDNotification::PypadType:
    ret="PYPAD";
    break;

  case RDNotification::DropboxType:
    ret="DROPBOX";
    break;

  case RDNotification::CatchEventType:
    ret="CATCH_EVENT";
    break;

  case RDNotification::PlayoutCartNumType:
    ret="PLAYOUT_CART";
    break;

  case RDNotification::PlayoutExtIdType:
    ret="PLAYOUT_EXTID";
    break;

  case RDNotification::LogExtIdType:
    ret="LOG_EXTID";
    break;

  case RDNotification::NullType:
  case RDNotification::LastType:
    break;
  }

  return ret;
}


QString RDNotification::actionString(Action action)
{
  QString ret="UNKNOWN";

  switch(action) {
  case RDNotification::AddAction:
    ret="ADD";
    break;

  case RDNotification::DeleteAction:
    ret="DELETE";
    break;

  case RDNotification::ModifyAction:
    ret="MODIFY";
    break;

  case RDNotification::StartAction:
    ret="START";
    break;

  case RDNotification::StopAction:
    ret="STOP";
    break;

  case RDNotification::MoveAction:
    ret="MOVE";
    break;

  case RDNotification::NoAction:
  case RDNotification::LastAction:
    break;
  }

  return ret;
}

QString RDNotification::dateTimeString(QDateTime datetime)
{
  return datetime.toString(Qt::ISODate);
}

