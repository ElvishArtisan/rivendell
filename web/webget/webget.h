// webget.h
//
// Rivendell audio upload/download utility
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>

#include <rdaudioconvert.h>
#include <rdformpost.h>

#define WEBGET_CGI_USAGE "\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void ripcConnectedData(bool state);

 private:
  void GetAudio();
  void PutAudio();
  void ServeForm();
  void ServeLogin(int resp_code);
  bool Authenticate();
  void SaveSourceFile(const QString &filepath) const;
  void Exit(int code);
  void TextExit(const QString &msg,int code,int line) const;
  RDFormPost *webget_post;
  QString webget_remote_hostname;
  QString webget_remote_username;
  QString webget_remote_password;
  QString webget_ticket;
  QHostAddress webget_remote_address;
};


#endif  // WEBGET_H
