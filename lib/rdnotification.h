// rdnotification.h
//
// A container class for a Rivendell Notification message.
//
//   (C) Copyright 2018-2023 Fred Gleason <fredg@paravelsystems.com>
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

class RDNotification
{
 public:
  enum Type {NullType=0,CartType=1,LogType=2,PypadType=3,DropboxType=4,
	     CatchEventType=5,FeedItemType=6,FeedType=7,
	     PanelButtonType=8,ExtendedPanelButtonType=9,LastType=10};
  enum Action {NoAction=0,AddAction=1,DeleteAction=2,ModifyAction=3,
	       LastAction=4};
  RDNotification(Type type,Action action,const QVariant &id);
  RDNotification();
  Type type() const;
  void setType(Type type);
  Action action() const;
  void setAction(Action action);
  QVariant id() const;
  void setId(const QVariant id);
  bool isValid() const;
  bool read(const QString &str);
  QString write() const;
  QString dump() const;
  static QString typeString(Type type);
  static QString actionString(Action action);

 private:
  Type notify_type;
  Action notify_action;
  QVariant notify_id;
};


#endif  // RDNOTIFICATION_H
