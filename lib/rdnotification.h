// rdnotification.h
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

#ifndef RDNOTIFICATION_H
#define RDNOTIFICATION_H

#include <QString>
#include <QVariant>
#include <QDateTime>

class RDNotification
{
 public:
  enum Type {NullType=0,CartType=1,LogType=2,PypadType=3,DropboxType=4,
	     CatchEventType=5,PlayoutCartNumType=6,PlayoutExtIdType=7,
             LogExtIdType=8,LastType=9};
  enum Action {NoAction=0,AddAction=1,DeleteAction=2,ModifyAction=3,
	       MoveAction=4,StartAction=5,StopAction=6,LastAction=7};
  RDNotification(Type type,Action action,const QVariant &id);
  RDNotification(Type type,Action action,const QVariant &id,const QVariant &arg);
  RDNotification();
  Type type() const;
  void setType(Type type);
  Action action() const;
  void setAction(Action action);
  QVariant id() const;
  void setArg(const QVariant arg);
  QVariant arg() const;
  void setId(const QVariant id);
  QDateTime dateTime() const;
  void setDateTime(const QDateTime datetime);
  bool isValid() const;
  bool read(const QString &str);
  QString write() const;
  static QString typeString(Type type);
  static QString actionString(Action action);
  static QString dateTimeString(QDateTime datetime);

 private:
  enum Position {PosNotify=0,PosType=1,PosAction=2,PosTimeStamp=3,PosId=4,PosArg=5};
  Type notify_type;
  Action notify_action;
  QDateTime notify_datetime;
  QVariant notify_id;
  QVariant notify_arg;
};


#endif  // RDNOTIFICATION_H
