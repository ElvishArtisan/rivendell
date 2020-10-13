// webget.h
//
// Rivendell audio download utility
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WEBGET_H
#define WEBGET_H

#include <qobject.h>

#include <rdaudioconvert.h>
#include <rdformpost.h>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_NUMBER QString(STRINGIZE(__LINE__)).toInt()
#define WEBGET_CGI_USAGE "\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void ripcConnectedData(bool state);

 private:
  void ServeForm();
  bool Authenticate();
  void Exit(int code);
  void XmlExit(const QString &msg,int code,
	       const QString &srcfile="",int line=-1,
	       RDAudioConvert::ErrorCode err=RDAudioConvert::ErrorOk);
  RDFormPost *webget_post;
  QString webget_remote_hostname;
  QHostAddress webget_remote_address;
};


#endif  // WEBGET_H
