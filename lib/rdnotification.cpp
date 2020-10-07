// rdnotification.cpp
//
// A container class for a Rivendell Notification message.
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdnotification.h"

RDNotification::RDNotification(Type type,Action action,const QVariant &id)
{
  notify_type=type;
  notify_action=action;
  notify_id=id;
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
  if(args.size()==4) {
    if(args[0]!="NOTIFY") {
      return false;
    }
    for(int i=0;i<RDNotification::LastType;i++) {
      RDNotification::Type type=(RDNotification::Type)i;
      if(args[1]==RDNotification::typeString(type)) {
	notify_type=type;
	switch(type) {
	case RDNotification::CartType:
	  notify_id=QVariant(args[3].toUInt());
	  break;

	case RDNotification::LogType:
	  notify_id=QVariant(args[3]);
	  break;

	case RDNotification::PypadType:
	  notify_id=QVariant(args[3].toUInt());
	  break;

	case RDNotification::DropboxType:
	  notify_id=QVariant(args[3]);
	  break;

	case RDNotification::CatchEventType:
	  notify_id=QVariant(args[3].toUInt());
	  break;

	case RDNotification::FeedItemType:
	  notify_id=QVariant(args[3].toUInt());
	  break;

	case RDNotification::FeedType:
	  notify_id=QVariant(args[3]);
	  break;

	case RDNotification::NullType:
	case RDNotification::LastType:
	  break;
	}
      }
    }
    if(notify_type==RDNotification::NullType) {
      return false;
    }
    for(int i=0;i<RDNotification::LastAction;i++) {
      RDNotification::Action action=(RDNotification::Action)i;
      if(args[2]==RDNotification::actionString(action)) {
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
  QString ret="";

  ret+="NOTIFY ";
  ret+=RDNotification::typeString(notify_type)+" ";
  ret+=RDNotification::actionString(notify_action)+" ";
  switch(notify_type) {
  case RDNotification::CartType: 
    ret+=QString().sprintf("%u",notify_id.toUInt());
    break;

  case RDNotification::LogType: 
    ret+=notify_id.toString();
    break;

  case RDNotification::PypadType: 
    ret+=QString().sprintf("%u",notify_id.toUInt());
    break;

  case RDNotification::DropboxType: 
    ret+=notify_id.toString();
    break;

  case RDNotification::CatchEventType: 
    ret+=QString().sprintf("%u",notify_id.toUInt());
    break;

  case RDNotification::FeedItemType: 
    ret+=QString().sprintf("%u",notify_id.toUInt());
    break;

  case RDNotification::FeedType: 
    ret+=notify_id.toString();
    break;

  case RDNotification::NullType:
  case RDNotification::LastType:
    break;
  }
  return ret;
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

  case RDNotification::FeedItemType:
    ret="FEED_ITEM";
    break;

  case RDNotification::FeedType:
    ret="FEED";
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

  case RDNotification::NoAction:
  case RDNotification::LastAction:
    break;
  }
  return ret;
}
